/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/common/core/odysseymbxscratch.H $                  */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2022                             */
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

/**
 * @brief This file provided info of various Mail Box Scratch regs used by SBE
 *
 */

typedef union
{
    struct
    {
        uint64_t iv_istepMode : 1;
        uint64_t iv_sbeDestRuntime : 1;
        uint64_t iv_mpiplMode : 1;
        uint64_t iv_fspAttached : 1;
        uint64_t iv_collectFFDC : 1;
        uint64_t iv_sendFFDC : 1;
        uint64_t iv_mbx5DontCare1 : 4;
        uint64_t iv_dontCheckStopForDMT : 1;
        uint64_t iv_disableScomFiltering : 1;
        uint64_t iv_disableInvalidScomAddrCheck : 1;
        uint64_t iv_mbx5DontCare2 : 2;
        uint64_t iv_skipMpiplDumpCollection : 1;
        uint64_t iv_enableECDSA : 1;
        uint64_t iv_enableDilithium : 1;
        uint64_t iv_enableEcidVerification : 1;
        uint64_t iv_enableHwKeyHashVerification : 1;
        uint64_t iv_mbx5DontCare3 : 12;
        uint64_t iv_mbx5Unused : 32;
    };
    uint64_t iv_mbx5;
}mbx5_t;

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
        uint64_t iv_reserved1 : 2;
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

#endif /* _ODYSSEYMBXSCRATCH_H_ */