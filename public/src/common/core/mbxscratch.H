/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/common/core/mbxscratch.H $                         */
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
        uint64_t iv_istepMode : 1;                        //bit 0: p11, odyssey
        uint64_t iv_sbeDestRuntime : 1;                   //bit 1: p11, odyssey
        uint64_t iv_mpiplMode : 1;                        //bit 2: p11
        uint64_t iv_mbx11DontCare1 : 2;                   //bit 3..4: p11
        uint64_t iv_sendFFDC : 1;                         //bit 5: p11, odyssey
        uint64_t iv_disableSecurity : 1;                  //bit 6: p11, odyssey
        uint64_t iv_allowAttrOverride : 1;                //bit 7: p11, odyssey
        uint64_t iv_fwSmtOverride : 2;                    //bit 8..9: p11, odyssey
        uint64_t iv_dontCheckStopForDMT : 1;              //bit 10: p11
        uint64_t iv_disableScomFiltering : 1;             //bit 11: p11
        uint64_t iv_disableInvalidScomAddrCheck : 1;      //bit 12: p11, odyssey
        uint64_t iv_mfgMode : 1;                          //bit 13: p11, odyssey
        uint64_t iv_enableSuart : 1;                      //bit 14: p11
        uint64_t iv_skipMpiplDumpCollection : 1;          //bit 15: p11
        uint64_t iv_enableECDSA : 1;                      //bit 16: p11, odyssey
        uint64_t iv_enableDilithium : 1;                  //bit 17: p11, odyssey
        uint64_t iv_enableEcidVerification : 1;           //bit 18: p11, odyssey
        uint64_t iv_enableHwKeyHashVerification : 1;      //bit 19: p11, odyssey
        uint64_t iv_enforceMatchingSecurity  : 1;         //bit 20: p11, odyssey
        uint64_t iv_mbx11DontCare2 : 11;                  //bit 21..31: p11
        uint64_t iv_mbx11Unused : 32;
    };
    uint64_t iv_mbx11;
}mbx11_t;

typedef union
{
    struct
    {
        uint64_t iv_mbx6DontCare : 24;
        uint64_t iv_isMaster : 1;
        uint64_t iv_mbx6DontCare2 : 7;
        uint64_t iv_mbx6Unused : 32;
    };
    uint64_t iv_mbx6;
}mbx6_t;

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
        uint64_t iv_majorStep : 8; // Max major is 97
        uint64_t iv_minorStep : 6; // Max minor is 34
        uint64_t iv_progressCode : 6;
        uint64_t iv_unused : 32;
    };
    uint64_t iv_messagingReg;
}messagingReg_t;

typedef union
{
    struct
    {
        uint64_t iv_mbox8Reserve1 : 2;
        uint64_t iv_mbx5_valid : 1;
        uint64_t iv_mbox8Reserve2 : 2;
        uint64_t iv_mbx6_valid : 1;
        uint64_t iv_mbox8Reserve3 : 58;
    };
    uint64_t iv_mbx8;
}mbx8_t;

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
