/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/common/core/mbxscratch.H $                         */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2022,2023                        */
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

#ifndef _ODYSSEYMBXSCRATCH_H_
#define _ODYSSEYMBXSCRATCH_H_

// TODO: Struct Bit Field definitions in this file needs to be changed as per IPL Flow doc.
//       Currently considering p11 and odyssey will have same bit field definitions

/**
 * @brief This file provides info of various Mail Box Scratch regs definitions used by SBE
 *        This is a common file for p11 and odyssey
 *
 */

typedef union
{
    struct
    {
        // @ref iv_bootFlags - sberegaccess::BootMode enum
        uint64_t iv_bootFlags : 2;                        //bit 0..1: p11, odyssey
        uint64_t iv_mpiplMode : 1;                        //bit 2: p11
        uint64_t iv_attrIsSimulation : 1;                 //bit 3: p11, odyssey
        uint64_t iv_emulateSABSet : 1;                    //bit 4: p11, odyssey
        uint64_t iv_disableFFDC : 1;                      //bit 5: p11, odyssey
        uint64_t iv_disableSecurity : 1;                  //bit 6: p11
        uint64_t iv_allowAttrOverride : 1;                //bit 7: p11, odyssey
        uint64_t iv_fwSmtOverride : 2;                    //bit 8..9: p11
        uint64_t iv_dontCheckStopForDMT : 1;              //bit 10: p11
        uint64_t iv_disableScomFiltering : 1;             //bit 11: p11, odyssey
        uint64_t iv_disableInvalidScomAddrCheck : 1;      //bit 12: p11, odyssey
        uint64_t iv_mfgMode : 1;                          //bit 13: p11, odyssey
        uint64_t iv_enableSuart : 1;                      //bit 14: p11
        uint64_t iv_skipMpiplDumpCollection : 1;          //bit 15: p11
        uint64_t iv_enableECDSA : 1;                      //bit 16: p11, odyssey
        uint64_t iv_enableDilithium : 1;                  //bit 17: p11, odyssey
        uint64_t iv_enableEcidVerification : 1;           //bit 18: p11, odyssey
        uint64_t iv_enableHwKeyHashVerification : 1;      //bit 19: p11, odyssey
        uint64_t iv_fileHashCalculationEnable : 1;        //bit 20: p11, odyssey
        uint64_t iv_enforceMatchingSecurity  : 1;         //bit 21: p11, odyssey
        uint64_t iv_secureBootValidityVerification : 1;   //bit 22: p11, odyssey
        uint64_t iv_disableIOBist : 1;                    //bit 23: p11, odyssey
        uint64_t iv_disableAbist : 1;                     //bit 24: p11, odyssey
        uint64_t iv_disableLbist : 1;                     //bit 25: p11, odyssey
        uint64_t iv_enableBurnInMode : 1;                 //bit 26: p11, odyssey
        uint64_t iv_enableIstepTermination : 1;           //bit 27: p11, odyssey
        uint64_t iv_forceProductionMode : 1;              //bit 28: p11, odyssey
        uint64_t iv_mbx11DontCare2 : 3;                   //bit 29..31: p11, odyssey
        uint64_t iv_mbx11Unused : 32;
    };
    uint64_t iv_mbx11;
}mbx11_t;

typedef union
{
    struct
    {
        uint64_t iv_mbx6PLLSelect : 4;   // bit 0..3: Odyssey PLL ring select
        uint64_t iv_mbx6reserved  : 12;  // reserved
        uint64_t iv_mbx6freqInmhz : 16;  // Bytes 2,3 Odyssey Nest freq (in MHz)
        uint64_t iv_mbx6Unused    : 32;
    };
    uint64_t iv_mbx6;
} mbx6_t;

typedef union
{
    struct
    {
        uint64_t iv_sbeBooted : 1;
        uint64_t iv_asyncFFDC : 1;
        uint64_t iv_reserved1 : 1;
        uint64_t iv_currImage : 1; // If 0->SROM , 1->Boot Loader/Runtime
        uint64_t iv_prevState : 4;
        uint64_t iv_currState : 4;
        uint64_t iv_majorStep : 4;
        uint64_t iv_minorStep : 6;
        uint64_t iv_reserved2 : 4;
        uint64_t iv_progressCode : 6;
        uint64_t iv_unused : 32;
    };
    uint64_t iv_messagingReg;
}messagingReg_t;

typedef union
{
    struct
    {
        uint64_t iv_mbx1Valid : 1;
        uint64_t iv_mbx2Valid : 1;
        uint64_t iv_mbx3Valid : 1;
        uint64_t iv_mbx4Valid : 1;
        uint64_t iv_mbx5Valid : 1;
        uint64_t iv_mbx6Valid : 1;
        uint64_t iv_mbx7Valid : 1;
        uint64_t iv_mbx8Valid : 1;
        uint64_t iv_mbx9Valid : 1;
        uint64_t iv_mbx10Valid : 1;
        uint64_t iv_mbx11Valid : 1;
        uint64_t iv_mbx12Valid : 1;
        uint64_t iv_mbx13Valid : 1;
        uint64_t iv_mbx14Valid : 1;
        uint64_t iv_mbx15Valid : 1;
        uint64_t iv_mbx16Valid : 1;
        uint64_t iv_mbx16reserved : 16;
        uint64_t iv_mbx16Unused : 32;
    };
    uint64_t iv_mbx16;
}mbx16_t;

typedef union secureBootFailStatus
{
    struct
    {
        uint64_t iv_reserved : 20;
        uint64_t iv_secureHeaderFailStatusCode : 12;
        uint64_t iv_unused : 32;
    };
    uint64_t iv_mbx13;
}secureBootFailStatus_t;

#endif /* _ODYSSEYMBXSCRATCH_H_ */
