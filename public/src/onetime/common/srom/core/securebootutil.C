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

uint8_t getMSV(uint64_t msvFuseBank0, uint64_t msvFuseBank1,
               uint64_t msvFuseBank2)
{
    // Calculated FW minimum secure version
    uint8_t minSecVer = 0;

    for (uint8_t bit = 0; bit < 64; bit ++)
    {
        uint8_t numSet = 0;
        numSet += (msvFuseBank0 >> bit) & 1U;
        numSet += (msvFuseBank1 >> bit) & 1U;
        numSet += (msvFuseBank2 >> bit) & 1U;
        if (numSet >= 2)
        {
            minSecVer = bit + 1;
        }
        else
        {
            break; // stop counting on first non-set fuse bit
        }
    }

    return minSecVer;
}

//NOTE: make32Bit() is just used for convertinf uint8_t array to uint32_t.
//Alternatively it can be achieved by typecasting
void SHA3TruncatedDump(sha3truncated_t *result){
  uint8_t *myptr=(uint8_t*)result;
  for(uint32_t x=0;x<SHA3_TRUNCATED_SIZE;x+=16)
  {
    SBE_INFO("%08x%08x%08x%08x", make32Bit(&myptr[x]),make32Bit(&myptr[x+4]),
                                 make32Bit(&myptr[x+8]),make32Bit(&myptr[x+12]));
  }
}
