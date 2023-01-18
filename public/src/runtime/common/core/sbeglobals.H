/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/common/core/sbeglobals.H $                 */
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
#ifndef __SBE_GLOBALS_H
#define __SBE_GLOBALS_H

#include "sbeexeintf.H"
#include "sbeSpMsg.H"
#include "sbeutil.H"
#include "cmnglobals.H"
#include "archive.H"

#define SBE_GLOBAL sbeGlobal
extern sbeRole g_sbeRole;

// To keep track of global constructor pointer functions and bss sections.
extern uint64_t _heap_space_start_ SECTION(".bss");
extern uint64_t _heap_space_end_ SECTION(".bss");
extern uint64_t _heap_space_size_ SECTION(".bss");

class SBEGlobalsSingleton
{
    public:
        // Disable copy contructor and assingment operator
        SBEGlobalsSingleton(const SBEGlobalsSingleton&) = delete;
        SBEGlobalsSingleton& operator=(const SBEGlobalsSingleton&) = delete;
        static SBEGlobalsSingleton& getInstance();

        sbeFifoCmdReqBuf_t sbeFifoCmdHdr;
        sbeCmdRespHdr_t sbeCmdRespHdr;

        sbeIntrHandle_t sbeIntrSource;

        ////////////////////////////////////////////////////////////////
        //// @brief Global semaphores
        /////////////////////////////////////////////////////////////////
        /**
         * @brief Global semaphore : SBE_GLOBAL->sbeSemCmdRecv
         *
         *   This is used to synchronize between the ISR and
         *   the command receiver thread.
         *
         */
        PkSemaphore sbeSemCmdRecv;
        /**
         * @brief Global semaphore : SBE_GLOBAL->sbeSemCmdProcess
         *
         *   This is used to synchronize between command receiver thread
         *   and synchronous command processor thread.
         *
         */
        PkSemaphore sbeSemCmdProcess;

        /**
         * @brief Global semaphore for async task
         *
         *   This is used to send invoke commands on Async process
         *
         */
        PkSemaphore sbeSemAsyncProcess;

        #ifdef SBE_CONSOLE_SUPPORT
        // Binary semaphore to protect uart access
        PkSemaphore sbeUartBinSem;
        static bool sbeUartActive;
        #endif

        // Last failed rc's - for SBE internal ffdc
        static uint16_t failedPrimStatus;
        static uint16_t failedSecStatus;
        static uint16_t failedSeqId;
        static uint8_t  failedCmdClass;
        static uint8_t  failedCmd;
        static uint32_t activeUsFifo;
        static uint32_t activeInterface;

        ////////////////////////////////////////////////////////////////
        //// @brief PkThread structure for SBE Command Receiver thread
        //////////////////////////////////////////////////////////////////
        PkThread sbeCommandReceiver_thread;
        ////////////////////////////////////////////////////////////////
        //// @brief PkThread structure for SBE Synchronous ChipOps
        ////        processing thread
        //////////////////////////////////////////////////////////////////
        PkThread sbeSyncCommandProcessor_thread;
        ////////////////////////////////////////////////////////////////
        ////// @brief PkThread structure for SBE Asynchronous ChipOps
        //////        processing thread
        //////////////////////////////////////////////////////////////////
        PkThread sbeAsyncCommandProcessor_thread;

        // @brief FileArchive object to keep track of the pak stack in pibmem
        // Allows access for a global FileArchive object independent of the Heap object
        // initialized below
        FileArchive embeddedArchive __attribute__ ((aligned (8)))
                            { (void*)&_heap_space_start_};

        // i2c mode register
        static uint64_t i2cModeRegister;

    private:
        /* Constructor
         */
        SBEGlobalsSingleton():
            sbeFifoCmdHdr(),
            sbeCmdRespHdr(),
            sbeIntrSource(),
            sbeSemCmdRecv(),
            sbeSemCmdProcess(),
            sbeSemAsyncProcess()
            {
            }
};
extern SBEGlobalsSingleton* sbeGlobal;

#endif //__SBE_GLOBALS_H
