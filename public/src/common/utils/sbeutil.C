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
#include "archive.H"

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

    HASH_LIST_RET_t check_file_hash(const char *i_fname, const sha3_t &i_hash,
                                    const uint8_t *i_hash_list, uint8_t **o_ptrMismatchHash)
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
                    if (o_ptrMismatchHash)
                    {
                        *o_ptrMismatchHash = (uint8_t *)(i_hash_list + fnlen);
                    }
                    SBE_ERROR_BIN("File hash does not match", i_fname, in_fnlen);
                    return HASH_COMPARE_FAIL;
                }
            }
            i_hash_list = (const uint8_t *)(filehash + 1);
        }
    }

    void updatePakErrorCodeAndHalt(uint16_t i_baseErrorCode, uint16_t i_pakRc)
    {
        updateErrorCode(i_baseErrorCode + compressed_arc_return_code(i_pakRc));
        pk_halt();
    }

    void updateErrorCodeAndHalt(uint16_t i_errorCode)
    {
        updateErrorCode(i_errorCode);
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
        bool isHreset = false;
        sbe_local_LFR lfrReg;
        PPE_LVD(scomt::poz_ppe::TP_TPCHIP_PIB_SBE_SBEPRV_LCL_LFR_SCRATCH_RW, lfrReg);
        isHreset = lfrReg.hreset;
        return (isHreset);
    }

    void clearHreset(void)
    {
        sbe_local_LFR lfrReg;
        // load lfrReg
        // Set the hreset bit and write to WO_CLR Reg to clear
        lfrReg.hreset = 1;
        // store lfr with updated hreset bit
        PPE_STVD(scomt::poz_ppe::TP_TPCHIP_PIB_SBE_SBEPRV_LCL_LFR_SCRATCH_PPE2, lfrReg);
    }

    void updateProgressCode(uint8_t value)
    {
        messagingReg_t messagingReg;
        getscom_abs(scomt::poz::FSXCOMP_FSXLOG_SB_MSG, &messagingReg.iv_messagingReg);
        messagingReg.iv_progressCode = value;
        putscom_abs(scomt::poz::FSXCOMP_FSXLOG_SB_MSG, messagingReg.iv_messagingReg);
    }

    bool reserveSemaphore(uint8_t semaphore)
    {
        uint32_t timeoutCount = 1024;     // 1 s
        uint64_t data64; 
        for (uint32_t i = 0; i < timeoutCount; i++) {
            data64 = 0x8000000000000000ULL >> (semaphore * 2 + 1);
            putscom_abs(0x50191, data64);
            getscom_abs(0x50191, &data64);
            if (data64 & (0x8000000000000000ULL >> (semaphore * 2 + 1)))
            {
                return false;
            }
            pk_sleep(PK_MILLISECONDS(1));
        }
        SBE_ERROR("Semaphore %04X reservation timeout", semaphore);
        return true;
    }

    void releaseSemaphore(uint8_t semaphore)
    {
        uint64_t data64;
        getscom_abs(0x50191, &data64);
        if (data64 & (0x8000000000000000ULL >> (semaphore * 2 + 1)))
        {
            data64 = 0xC000000000000000ULL >> (semaphore * 2);
            putscom_abs(0x50191, data64);
        } else {
            SBE_ERROR("Trying to free unreserved semaphore %04X", semaphore);
        }
    }

    uint32_t alphaNumericToHex(const uint8_t *i_str,
                               const uint8_t i_size,
                               const uint8_t i_hexSize,
                               uint8_t *o_hex)
    {
        uint32_t l_rc = SBE_SEC_OPERATION_SUCCESSFUL;
        uint8_t charIdx = 0, hexValIdx = 0;
        uint8_t val = 0;

        do
        {
            // Check for size of hex buffer value
            // Note: added 1 to i_size to take care of odd size number
            if (i_hexSize < ((i_size + 1)/ 2))
            {
                l_rc = SBE_SEC_BUFFER_SIZE_SMALL;
                SBE_ERROR("Buffer size [0x%02x] passed is small than expected [0x%02x]",
                        i_hexSize, (i_size + 1)/2);
                break;
            }

            while (charIdx < i_size)
            {
                if (i_str[charIdx] >= '0' && i_str[charIdx] <= '9')
                    val = i_str[charIdx] - '0';
                else if (i_str[charIdx] >= 'a' && i_str[charIdx] <= 'z')
                    val = i_str[charIdx] - 'a' + 10;
                else if (i_str[charIdx] >= 'A' && i_str[charIdx] <= 'Z')
                    val = i_str[charIdx] - 'A' + 10;
                else
                {
                    l_rc = SBE_SEC_NON_ALPHA_NUMERIC_CHAR_FOUND;
                    SBE_ERROR("Non-alphanumeric character [%c] [0x%02x] found at index [%d]",
                            i_str[charIdx], i_str[charIdx], charIdx);
                    break;
                }

                // each char converted hex would take a nibble position in byte
                if (i_size == 1)
                {
                    o_hex[hexValIdx] |= val;
                }
                else
                {
                    o_hex[hexValIdx] |= (val << (4 - ((charIdx % 2) * 4)));
                }
                if (charIdx % 2)
                {
                    hexValIdx++;
                }
                charIdx++;
            }
        }while(false);

        return l_rc;
    }
}
