/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/common/utils/sbeutil.C $                           */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2016,2022                        */
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
#include "p11_scom_perv_cfam.H"
#include "mbxscratch.H"

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
        getscom_abs(scomt::perv::FSXCOMP_FSXLOG_SCRATCH_REGISTER_13_RW, &secureBootFailStatus.iv_mbx13);
        secureBootFailStatus.iv_secureHeaderFailStatusCode = i_errorCode;
        putscom_abs(scomt::perv::FSXCOMP_FSXLOG_SCRATCH_REGISTER_13_RW, secureBootFailStatus.iv_mbx13);
        SBE_ERROR(SBE_FUNC "Halting PPE...");
        pk_halt();
    }

    void memcpy_byte(void* vdest, const void* vsrc, size_t len)
    {

        // Loop, copying 1 byte
        uint8_t* cdest = (uint8_t *)vdest;
        const uint8_t* csrc = (const uint8_t *)vsrc;
        size_t i = 0;

        for (; i < len; ++i)
        {
            cdest[i] = csrc[i];
        }
    }

    bool isHreset(void)
    {
        #define SBE_FUNC "IS_HRESET"
        bool isHreset = false;
        sbe_local_LFR lfrReg;
        PPE_LVD(0xc0002040, lfrReg);
        isHreset = lfrReg.runtime_reset;
        return (isHreset);
        #undef SBE_FUNC
    }

    void updateProgressCode(uint64_t data, uint8_t value)
    {
        data = ((data & (~(SBE_MESSAGE_REG_PROGRESS_CODE_BIT_MASK))) | (uint64_t)(value)<<32);
        putscom_abs(scomt::perv::FSXCOMP_FSXLOG_SB_MSG, data);
    }
}

