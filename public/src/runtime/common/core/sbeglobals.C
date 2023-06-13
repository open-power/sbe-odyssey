/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/common/core/sbeglobals.C $                 */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2017,2023                        */
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
#include "sbeglobals.H"
#include "sbefifo.H"

////////////////////////////////////////////////////////////////
//// @brief Stacks for Non-critical Interrupts and Threads
//////////////////////////////////////////////////////////////////
sbeRole g_sbeRole = SBE_ROLE_MASTER;

SBEGlobalsSingleton* sbeGlobal = &SBEGlobalsSingleton::getInstance();
SBEGlobalsSingleton& SBEGlobalsSingleton::getInstance()
{
    static SBEGlobalsSingleton iv_instance;
    return iv_instance;
}

uint16_t SBEGlobalsSingleton::failedPrimStatus = SBE_PRI_OPERATION_SUCCESSFUL;
uint16_t SBEGlobalsSingleton::failedSecStatus  = SBE_SEC_OPERATION_SUCCESSFUL;
uint16_t SBEGlobalsSingleton::failedSeqId      = 0;
uint8_t  SBEGlobalsSingleton::failedCmdClass   = 0;
uint8_t  SBEGlobalsSingleton::failedCmd        = 0;
uint32_t SBEGlobalsSingleton::activeUsFifo = SBE_FIFO_UNKNOWN;
uint32_t SBEGlobalsSingleton::activeInterface = SBE_INTERFACE_UNKNOWN;
uint8_t  SBEGlobalsSingleton::pibCtrlId        = 0xD;
