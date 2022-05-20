/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/common/core/sbepipe.H $                    */
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
/*
 * @file: src/runtime/common/core/sbepipe.H
 *
 * @brief This file contains basic SBE PIPE hardware specific definitions
 *        and utilities
 *
 */
#ifndef __SBEPIPE_H
#define __SBEPIPE_H

#include "ppe42_scom.h"
#include "sbetrace.H"

#define SBE_MAX_PIPES 8

//Base address for SBE PIPE
const uint64_t SBE_PIPE1_BASE                    =  0x000B0100;
const uint64_t SBE_PIPE2_BASE                    =  0x000B0200;
const uint64_t SBE_PIPE3_BASE                    =  0x000B0300;
const uint64_t SBE_PIPE4_BASE                    =  0x000B0400;
const uint64_t SBE_PIPE5_BASE                    =  0x000B0500;
const uint64_t SBE_PIPE6_BASE                    =  0x000B0600;
const uint64_t SBE_PIPE7_BASE                    =  0x000B0700;
const uint64_t SBE_PIPE8_BASE                    =  0x000B0800;

// Utility addresses common across all PIPEs

// Access Control & Interrupt Routing Reg
const uint64_t SBE_PIPE_REG_ACCESS_CTRL          = 0x000B0120;
const uint64_t SBE_PIPE_REG_ACCESS_CTRL_SET      = 0x000B0121;
const uint64_t SBE_PIPE_REG_ACCESS_CTRL_CLR      = 0x000B0122;

// PIB Controller Identifier Reg
const uint64_t SBE_PIPE_REG_CTLR_ID              = 0x000B0123;
const uint64_t SBE_PIPE_REG_CTLR_ID_SET          = 0x000B0124;
const uint64_t SBE_PIPE_REG_CTLR_ID_CLR          = 0x000B0125;

// Above registers aggregrate pipe configuration and access data across
// all 8 pipes, 1 byte per pipe starting with the MSB e.g. byte 0 => pipe 1,
// .. byte7 => pipe 8
const uint64_t SBE_PIPE_MASK = 0xFF00000000000000;

// Utility to get pipe base address from pipe number ranging from 0 to 7
#define SBE_PIPE_BASE_ADDR(pipe_nr) (SBE_PIPE1_BASE+((pipe_nr)*100))

// Utility to extract pipe config data from a double word based on pipe number,
// where pipe_nr can range from SBE_PIPE1 to SBE_PIPE8, see sbeFifoType
#define SBE_PIPE_GET_CFG_BYTE(pipe_nr, data64) \
                 ((uint8_t)(((data64) >> ((SBE_PIPE8 - (pipe_nr)) * 8)) & 0xFF))

// Utility to set pipe config data in a double word based on pipe number,
// where pipe_nr can range from SBE_PIPE1 to SBE_PIPE8, see sbeFifoType
#define SBE_PIPE_SET_CFG_BYTE(pipe_nr, data8) \
                 (((uint64_t)data8) << ((SBE_PIPE8 - (pipe_nr)) * 8))

/**
 * @brief Enum to list 4-bit PIB Controller IDs
 *
 *        Chip-Op command-response can be between any of these PIB controllers
 *        & SPPE. A PIPE's read and write ends can be restricted to be accessed
 *        by specific PIB contollers, by configuring their IDs in the PIPE
 *        Access Controller ID register (SBE_PIPE_REG_CTRL_ID)
 */
enum pibCtlrId
{
    PIB_CTLR_GSD2PIB  = 0x1,
    PIB_CTLR_FSI2PIB  = 0x2,
    PIB_CTLR_I2CR     = 0x3,
    PIB_CTLR_MMIO     = 0x9,
    PIB_CTLR_SPPE     = 0xD,
};

/**
  * @brief Access Control Register Bits per Pipe from SBE_PIPE_REG_ACCESS_CTRL
  */
union sbePipeAccessFlags_t
{
    uint8_t flags;
    struct
    {
        uint8_t wr_open:1;
        uint8_t wr_use_ctlr_id:1;
        uint8_t wr_free2:1;
        uint8_t wr_free3:1;
        uint8_t rd_open:1;
        uint8_t rd_use_ctlr_id:1;
        uint8_t rd_intr_enable:1;
        uint8_t rd_intr_pending:1;
    };
};

/**
  * @brief 64-bit access control reg across Pipes
  *        as read from Pipe Access Control Register (0x120)
  */
union sbePipeAccessCtrl_t
{
    uint64_t data64;
    struct
    {
        sbePipeAccessFlags_t pipe1;
        sbePipeAccessFlags_t pipe2;
        sbePipeAccessFlags_t pipe3;
        sbePipeAccessFlags_t pipe4;
        sbePipeAccessFlags_t pipe5;
        sbePipeAccessFlags_t pipe6;
        sbePipeAccessFlags_t pipe7;
        sbePipeAccessFlags_t pipe8;
    };
};

/**
  * @brief Read and write end PIB controller Ids configured for a pipe
  *        from Pipe Access CtrlID Register (SBE_PIPE_REG_CTLR_ID)
  */
union sbePipeCtlrId_t
{
    uint8_t byte;
    struct
    {
        uint8_t writeend:4;
        uint8_t readend:4;
    };
};

/**
  * @brief 64-bit PIB controller ID reg across Pipes as read from the
  *        Pipe Access CtrlID Register (0x123)
  */
union sbePipeCtlrIDs_t
{
    uint64_t data64;
    struct
    {
        sbePipeCtlrId_t pipe1;
        sbePipeCtlrId_t pipe2;
        sbePipeCtlrId_t pipe3;
        sbePipeCtlrId_t pipe4;
        sbePipeCtlrId_t pipe5;
        sbePipeCtlrId_t pipe6;
        sbePipeCtlrId_t pipe7;
        sbePipeCtlrId_t pipe8;
    };
};

#endif // __SBEPIPE_H
