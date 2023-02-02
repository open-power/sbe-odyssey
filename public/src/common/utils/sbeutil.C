/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/common/utils/sbeutil.C $                           */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2016,2023                        */
/* [+] International Business Machines Corp.                              */
/*                                                                        */
/*                                                                        */
/* Licensed under the Apache License, Version 2.0 (the "License");        */
/* you may not use this file except in compliance with the License.       */
/* You may obtain a copy of the License at                                */
/*                                                                        */
/*     http://www.apache.org/licenses/LICENSE-2.0                         */
/*                                                                        */
/* Unless required by applicable law or agreed to in writing, software    */
/* distributed under the License is distributed on an "AS IS" BASIS,      */
/* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or        */
/* implied. See the License for the specific language governing           */
/* permissions and limitations under the License.                         */
/*                                                                        */
/* IBM_PROLOG_END_TAG                                                     */
#include "sbeutil.H"
#include "cmnglobals.H"
#include "ppe42_string.h"
#include "sbetrace.H"
#include "poz_scom_perv_cfam.H"
#include "poz_ppe.H"
#include "mbxscratch.H"
#include "errorcodes.H"

namespace SBE
{
    bool isSimics() __attribute__((alias("__isSimicsRunning")));
    extern "C" void __isSimicsRunning() __attribute__ ((noinline));

    void __isSimicsRunning()
    {
        asm volatile("li 3, 0");
        SBE_MAGIC_INSTRUCTION(MAGIC_SIMICS_CHECK);
    }

    bool isSimicsRunning()
    {
        static bool simics = isSimics();
        return simics;
    }

    HASH_LIST_RET_t check_file_hash(const char *i_fname, const sha3_t &i_hash, const uint8_t *i_hash_list)
    {
        const int in_fnlen = strlen(i_fname);

        // First Byte in hash list is version and 2nd byte is hash algorithm used.
        //We need to skip these two fields and start searching for file
        i_hash_list += 2;

        while (true)
        {
            //1Byte is reserved
            i_hash_list += 1;

            //1Byte is file name length
            const uint8_t fnlen = *i_hash_list++;

            if (!fnlen)
            {
                SBE_ERROR_BIN("File not found in hash list", i_fname, in_fnlen);
                return FILE_NOT_FOUND;
            }

            const sha3_t *filehash = (const sha3_t *)(i_hash_list + fnlen);
            if (in_fnlen == fnlen && !memcmp(i_fname, i_hash_list, fnlen))
            {
                if (!memcmp(filehash, i_hash, sizeof(i_hash)))
                {
                    SBE_INFO("File hash matches");
                    return HASH_COMPARE_PASS;
                }
                else
                {
                    SBE_ERROR_BIN("File hash does not match", i_fname, in_fnlen);
                    return HASH_COMPARE_FAIL;
                }
            }

            i_hash_list = (const uint8_t *)(filehash + 1);
        }
    }

    void updateErrorCodeAndHalt(uint16_t i_errorCode)
    {
        secureBootFailStatus_t secureBootFailStatus;
        getscom_abs(scomt::poz::FSXCOMP_FSXLOG_SCRATCH_REGISTER_13_RW, &secureBootFailStatus.iv_mbx13);
        secureBootFailStatus.iv_secureHeaderFailStatusCode = i_errorCode;
        putscom_abs(scomt::poz::FSXCOMP_FSXLOG_SCRATCH_REGISTER_13_RW, secureBootFailStatus.iv_mbx13);
        pk_halt();
    }

    void updateErrorCode(uint16_t i_errorCode)
    {
        secureBootFailStatus_t secureBootFailStatus;
        getscom_abs(scomt::poz::FSXCOMP_FSXLOG_SCRATCH_REGISTER_13_RW, &secureBootFailStatus.iv_mbx13);
        secureBootFailStatus.iv_secureHeaderFailStatusCode = i_errorCode;
        putscom_abs(scomt::poz::FSXCOMP_FSXLOG_SCRATCH_REGISTER_13_RW, secureBootFailStatus.iv_mbx13);
    }

   /**************************************************************************
    *
    * Note: This function will be called before pk_init(). Hence, don't add
    *       SBE_INFO or SBE_ERROR. In case of error, update the error code
    *       in the error register.
    *
    * *************************************************************************/
    void updateSbeGlobalFreqFromLFR(void)
    {
        // Read local register 0xC0002040 with frequency value
        sbe_local_LFR lfrReg;
        PPE_LVD(scomt::poz_ppe::TP_TPCHIP_PIB_SBE_SBEPRV_LCL_LFR_SCRATCH_RW, lfrReg);
        // convert a Mhz frequency value to an equivalent SBE frequency value by
        // calculating the uint32_t number range.
        // ((pau_freq_in_mhz * 1000 * 1000)/4) for SBE Freq value
        CMN_GLOBAL->sbefreq = (lfrReg.pau_freq_in_mhz * 1000 * 250 );
        if(!CMN_GLOBAL->sbefreq)
        {
            updateErrorCodeAndHalt(BOOT_RC_SPPE_MBX6_FREQUENCY_INVALID);
        }
    }

   /**************************************************************************
    *
    * Note: This function will be called before pk_init(). Hence, don't add
    *       SBE_INFO or SBE_ERROR. In case of error, update the error code
    *       in the error register.
    *
    * *************************************************************************/
    void setSbeGlobalFreqFromScratchOrLFR(caller_id_t i_caller)
    {
        // Mailbox scratch 6(CFAM 283D, SCOM 0x5003D)
        // Command table writes value based on bucket selection (bits 0:3 above)
        // mbox scratch 6 and Bytes 2,3 from scratch 6 update Odyssey Nest freq
        // (in MHz) from platform

        // In HReset state, should not read Frequency from scratch 6 and Odyssey
        // Frequency read from only from LFR reg data
        if(!isHreset())
        {
            // Read odyssey mbx6 scratch for frequency
            mbx6_t odysseymbx6;
            getscom_abs(scomt::poz::FSXCOMP_FSXLOG_SCRATCH_REGISTER_6_RW,
                       &odysseymbx6.iv_mbx6);
            uint16_t odyFreqInMHZ = odysseymbx6.iv_mbx6freqInmhz;
            if(!odyFreqInMHZ)
            {
                updateErrorCodeAndHalt(
                    (i_caller == CALLER_BOOTLOADER) ? BOOT_RC_BLDR_MBX6_FREQUENCY_INVALID
                                                    : BOOT_RC_SPPE_MBX6_FREQUENCY_INVALID);
            }
            // Update local register 0xC0002040 with frequency value
            sbe_local_LFR lfrReg;
            PPE_LVD(scomt::poz_ppe::TP_TPCHIP_PIB_SBE_SBEPRV_LCL_LFR_SCRATCH_RW,
                    lfrReg);
            lfrReg.pau_freq_in_mhz = odyFreqInMHZ;
            PPE_STVD(scomt::poz_ppe::TP_TPCHIP_PIB_SBE_SBEPRV_LCL_LFR_SCRATCH_RW,
                     lfrReg);
        }
        // Read Freq value from LFR and update CMN_GLOBAL->sbe_freq
        updateSbeGlobalFreqFromLFR();
    }

    bool isHreset(void)
    {
        #define SBE_FUNC "IS_HRESET"
        bool isHreset = false;
        sbe_local_LFR lfrReg;
        PPE_LVD(0xc0002040, lfrReg);
        isHreset = lfrReg.hreset;
        return (isHreset);
        #undef SBE_FUNC
    }

    void updateProgressCode(uint8_t value)
    {
        messagingReg_t messagingReg;
        getscom_abs(scomt::poz::FSXCOMP_FSXLOG_SB_MSG, &messagingReg.iv_messagingReg);
        messagingReg.iv_progressCode = value;
        putscom_abs(scomt::poz::FSXCOMP_FSXLOG_SB_MSG, messagingReg.iv_messagingReg);
    }
}

