/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/onetime/common/srom/core/securebootutil.C $        */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2022                             */
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

#include "securebootutil.H"
#include "sha3.H"
#include "sbetrace.H"
#include "sbeutil.H"

uint8_t getSignatureAlgoCheck(bool signatureEnableScratchVal,
                              bool signatureEnableFuseVal,
                              bool signatureDisableFuseVal)
{
    bool enable = signatureEnableScratchVal | signatureEnableFuseVal;
    bool disable = signatureDisableFuseVal;

    if(enable == false && disable == false)
        return DISABLE_SIGNATURE_ALGO;
    else if(enable == true && disable == false)
        return ENABLE_SIGNATURE_ALGO;
    else if(enable == false && disable == true)
        return DISABLE_SIGNATURE_ALGO;
    else
        return (signatureEnableFuseVal ? INVALID_SIGNATURE_CNFG : DISABLE_SIGNATURE_ALGO);
}

uint8_t getMSV (uint64_t msvFuseBank)
{
    /* Calculated FW minimum secure version */
    uint8_t minSecVer = 0;

    /* MSV check fuse bank, msv max is 21 */
    for (uint8_t i = 0; i < 21; i++)
    {
        /* 63rd bit is resevered, +1 for reserved bit  */
        if (applyTripleModularRedundancyVoting((msvFuseBank >> (1 + (i * 3))) & 0x07))
        {
            minSecVer = 21 - i;
            break;
        }
    }

    return minSecVer;
}

uint8_t applyTripleModularRedundancyVoting(uint8_t data)
{
    uint8_t resultantValue = 0x0;

    /********************************************************
     * The expected length of the data is 3-bits.
     * If more than one bit is set, then return 0x1.
     *                              otherwise, return 0x0.
     * Ex. 000 - 0  001 - 0
     *     010 - 0  011 - 1
     *     100 - 0  101 - 1
     *     110 - 1  111 - 1
     ********************************************************/
    if ((data == 0x3)  || (data == 0x5) || (data == 0x6) || (data == 0x7))
    {
      resultantValue = 0x1;
    }

    return resultantValue;
}

//NOTE: make32Bit() is just used for convertinf uint8_t array to uint32_t.
//Alternatively it can be achieved by typecasting
void SHA3TruncatedDump(sha3truncated_t *result){
/*  uint8_t *myptr=(uint8_t*)result;
  for(uint32_t x=0;x<SHA3_TRUNCATED_SIZE;x+=16)
  {
    SBE_INFO("%08x%08x%08x%08x", make32Bit(&myptr[x]),make32Bit(&myptr[x+4]),
                                 make32Bit(&myptr[x+8]),make32Bit(&myptr[x+12]));
  }*/
}
