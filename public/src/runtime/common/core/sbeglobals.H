/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/common/core/sbeglobals.H $                 */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2017,2024                        */
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

#pragma once

#include "sbeexeintf.H"
#include "sbeSpMsg.H"
#include "sbeutil.H"
#include "cmnglobals.H"
#include "rt_pak.H"

#define SBE_GLOBAL sbeGlobal

// To keep track of global constructor pointer functions and bss sections.
extern uint32_t _attrs_start_ SECTION(".attr");
extern uint32_t _attrs_size_  SECTION(".attr");
extern uint64_t _heap_space_start_ SECTION(".bss");
extern uint64_t _heap_space_end_ SECTION(".bss");
extern uint64_t _heap_space_size_ SECTION(".bss");
extern uint8_t g_hash_list SECTION(".bss");

typedef struct
{
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

}SbeGlobalHandlerThreads_t;

////////////////////////////////////////////////////////////////
//// @brief Global semaphores
/////////////////////////////////////////////////////////////////
typedef struct
{
    /**
     * @brief Global semaphore : SBE_GLOBAL->semphores.sbeSemCmdRecv
     *
     *   This is used to synchronize between the ISR and
     *   the command receiver thread.
     *
     */
    PkSemaphore sbeSemCmdRecv;
    /**
     * @brief Global semaphore : SBE_GLOBAL->semphores.sbeSemCmdProcess
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

}SbeGlobalHandlerSemas_t;


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

        // Last failed rc's - for SBE internal ffdc
        static uint16_t failedPrimStatus;
        static uint16_t failedSecStatus;
        static uint16_t failedSeqId;
        static uint8_t  failedCmdClass;
        static uint8_t  failedCmd;
        static uint32_t activeUsFifo;
        static uint32_t activeInterface;

        ////////////////////////////////////////////////////////////////
        ////// @brief PK thread structure for runtime
        ////////////////////////////////////////////////////////////////
        SbeGlobalHandlerThreads_t threads;

        ////////////////////////////////////////////////////////////////
        ////// @brief PK Sema structure for runtime
        ////////////////////////////////////////////////////////////////
        SbeGlobalHandlerSemas_t semphores;

        // size of trusted embedded PAKs
        static uint64_t trustedPakEnd __attribute__ ((section (".sdata"),aligned (8)));

        // @brief FileArchive object to keep track of the pak stack in pibmem
        // Allows access for a global FileArchive object independent of the Heap object
        // initialized below
        RuntimePakWrapper trustedEmbeddedArchive __attribute__ ((aligned (8)))
        { (void*)&_heap_space_start_, (void*)std::min(trustedPakEnd,(uint64_t)&_heap_space_end_)};

        RuntimePakWrapper embeddedArchive __attribute__ ((aligned (8)))
        { (void*)&_heap_space_start_, (void*)&_heap_space_end_};

        // PPE PIB CTRL ID
        static PIBCtrlId pibCtrlId;

    private:
        /* Constructor
         */
        SBEGlobalsSingleton():
            sbeFifoCmdHdr(),
            sbeCmdRespHdr(),
            sbeIntrSource(),
            threads(),
            semphores()
            {}
};

extern SBEGlobalsSingleton* sbeGlobal;
