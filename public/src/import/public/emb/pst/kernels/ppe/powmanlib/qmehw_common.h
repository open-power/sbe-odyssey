/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/emb/pst/kernels/ppe/powmanlib/qmehw_common.h $ */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2021,2024                        */
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
#ifndef __QMEHW_COMMON_H__
#define __QMEHW_COMMON_H__


enum QME_CORE_MASKS
{
    QME_MASK_C0                     = 0x8, // Just Core0 = 0b1000
    QME_MASK_C1                     = 0x4, // Just Core1 = 0b0100
    QME_MASK_C2                     = 0x2, // Just Core2 = 0b0010
    QME_MASK_C3                     = 0x1, // Just Core3 = 0b0001
    QME_MASK_ALL_CORES              = 0xF  // ALL  Cores = 0b1111
};

enum QME_PIG_TYPES
{
    PIG_TYPE_0                      = 0x0,
    PIG_TYPE_1                      = 0x1,
    PIG_TYPE_2                      = 0x2,
    PIG_TYPE_3                      = 0x3,
    PIG_TYPE_4                      = 0x4,
    PIG_TYPE_5                      = 0x5,
    PIG_TYPE_6                      = 0x6,
    PIG_TYPE_7                      = 0x7,
    PIG_TYPE_8                      = 0x8,
    PIG_TYPE_9                      = 0x9,
    PIG_TYPE_A                      = 0xA,
    PIG_TYPE_B                      = 0xB,
    PIG_TYPE_C                      = 0xC,
    PIG_TYPE_D                      = 0xD,
    PIG_TYPE_E                      = 0xE,
    PIG_TYPE_F                      = 0xF
};

enum QME_BCEBAR_INDEXES
{
    QME_BCEBAR_0                    = 0,
    QME_BCEBAR_1                    = 1
};

enum QME_MULTICAST_TYPES
{
    QME_MC_RD_OR                    = 0,
    QME_MC_RD_AND                   = 1,
    QME_MC_RD_EQU                   = 4,
    QME_MC_WR                       = 5
};


/// Local Macro (See SCOM macros in ppehw_common.h)

// 0-3 | 4    | 5-7  | 8-11 | 12-15   | 16  | 17  | 18-19  | 20-27 | 28  | 29   | 30-31
// 0xC | MC=0 | 0    | 0    | Regions | Addr                       | Rsv | WSel | 0
// 0xC | MC=1 | MC_T | 0    | Regions | Per | Sat | SatSel | Addr  | Rsv | WSel | 0

#define QME_LCL_CORE_ADDR_MC(type, core, addr)   (0x08000000 | (type << 24) | (core << 16) | (addr))
#define QME_LCL_CORE_ADDR_WR(addr, core)         QME_LCL_CORE_ADDR_MC(QME_MC_WR,     core, addr)
#define QME_LCL_CORE_ADDR_OR(addr, core)         QME_LCL_CORE_ADDR_MC(QME_MC_RD_OR,  core, addr)
#define QME_LCL_CORE_ADDR_AND(addr, core)        QME_LCL_CORE_ADDR_MC(QME_MC_RD_AND, core, addr)
#define QME_LCL_CORE_ADDR_EQ(addr, core)         QME_LCL_CORE_ADDR_MC(QME_MC_RD_EQU, core, addr)

#endif  /* __QMEHW_COMMON_H__ */
