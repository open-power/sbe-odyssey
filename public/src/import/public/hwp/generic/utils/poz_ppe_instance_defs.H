/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/hwp/generic/utils/poz_ppe_instance_defs.H $ */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2023                             */
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

///
/// @file  poz_ppe_instance_defs.H
/// @brief PPE instance definitions.
///
/// This file is supposed to be included by poz_ppe_utils.C *only*
///
/// *HWP HW Owner        : Greg Still <stillgs.@us.ibm.com>
/// *HWP HW Backup Owner : Yue Du <daviddu@us.ibm.com>
/// *HWP FW Owner        : Prasad BG Ranganath <prasadbgr@in.ibm.com>
/// *HWP Team            : PM
/// *HWP Level           : 3
/// *HWP Consumed by     : Cronus, HB, FSP


#pragma once

enum PPE_BASE_ADDRESSES
{
    PPE_SBE_BASE_ADDRESS    = 0x000E0000,
    PPE_SPPE_BASE_ADDRESS   = 0x000D0000,
    PPE_PME_BASE_ADDRESS    = 0x000C0000,
    PPE_GPE_BASE_ADDRESS    = 0x00060000,
    PPE_QME_BASE_ADDRESS    = 0x200E0000,
    PPE_IOPPE_BASE_ADDRESS  = 0x10012C00,
};

PPE_Type_t G_PPE_Types[PPE_TYPE_MAX] =
{
    {
        "SBE",
        PPE_SBE_BASE_ADDRESS,
        1, //Only one SBE per chip
        0, //Only valid if num_instances > 1
        {   {PPE_IDX_XIXCR,    0x0},
            {PPE_IDX_XIRAMRA,  0x1},
            {PPE_IDX_XIRAMGA,  0x2},
            {PPE_IDX_XIRAMDBG, 0x3},
            {PPE_IDX_XIRAMEDR, 0x4},
            {PPE_IDX_XIDBGPRO, 0x5},
            {PPE_IDX_XIDBGINF, 0xF}
        },
        {   {PPE_IDX_XISIB,     0x6},
            {PPE_IDX_XIMEM,     0x7},
            {PPE_IDX_XISGB,     0x8},
            {PPE_IDX_XIICAC,    0x9},
        },
        0,                  //No VDR available through XIR
        {   {0,     0x0}},
        0,                  //No specific registers
        {   {0,     0x0}},
        {{}}
    },//PPE_TYPE_SBE
    {
        "SPPE",
        PPE_SPPE_BASE_ADDRESS,
        1, //Only one SBE per chip
        0, //Only valid if num_instances > 1
        {   {PPE_IDX_XIXCR,    0x0},
            {PPE_IDX_XIRAMRA,  0x1},
            {PPE_IDX_XIRAMGA,  0x2},
            {PPE_IDX_XIRAMDBG, 0x3},
            {PPE_IDX_XIRAMEDR, 0x4},
            {PPE_IDX_XIDBGPRO, 0x5},
            {PPE_IDX_XIDBGINF, 0xF}
        },
        {   {PPE_IDX_XISIB,     0x6},
            {PPE_IDX_XIMEM,     0x7},
            {PPE_IDX_XISGB,     0x8},
            {PPE_IDX_XIICAC,    0x9},
        },
        0,                  //No VDR available through XIR
        {   {0,     0x0}},
        0,                  //No specific registers
        {   {0,     0x0}},
        {{}}
    },//PPE_TYPE_SPPE
    {
        "PME",
        PPE_PME_BASE_ADDRESS,
        1, //Only one SBE per chip
        0, //Only valid if num_instances > 1
        {   {PPE_IDX_XIXCR,    0x0},
            {PPE_IDX_XIRAMRA,  0x1},
            {PPE_IDX_XIRAMGA,  0x2},
            {PPE_IDX_XIRAMDBG, 0x3},
            {PPE_IDX_XIRAMEDR, 0x4},
            {PPE_IDX_XIDBGPRO, 0x5},
            {PPE_IDX_XIDBGINF, 0xF}
        },
        {   {PPE_IDX_XISIB,     0x6},
            {PPE_IDX_XIMEM,     0x7},
            {PPE_IDX_XISGB,     0x8},
            {PPE_IDX_XIICAC,    0x9},
        },
        0,                  //No VDR available through XIR
        {   {0,     0x0}},
        0,                  //No specific registers
        {   {0,     0x0}},
        {{}}
    },//PPE_TYPE_PME
    {
        "GPE",
        PPE_GPE_BASE_ADDRESS,
        4, //4 GPEs per chip
        0x2000, //Only valid if num_instances > 1
        {   {PPE_IDX_XIXCR,    0x10},
            {PPE_IDX_XIRAMRA,  0x11},
            {PPE_IDX_XIRAMGA,  0x12},
            {PPE_IDX_XIRAMDBG, 0x13},
            {PPE_IDX_XIRAMEDR, 0x14},
            {PPE_IDX_XIDBGPRO, 0x15},
            {PPE_IDX_XIDBGINF, 0x1F}
        },
        {
            {PPE_IDX_XISIB,     0x16},
            {PPE_IDX_XIMEM,     0x17},
            {PPE_IDX_XISGB,     0x18},
            {PPE_IDX_XIICAC,    0x19},
            {PPE_IDX_XIDCAC,    0x1A},
        },
        1,
        {   {PPE_IDX_VDR0,     0x80},
            {PPE_IDX_VDR2,     0x81},
            {PPE_IDX_VDR4,     0x82},
            {PPE_IDX_VDR6,     0x83},
            {PPE_IDX_VDR8,     0x84},
            {PPE_IDX_VDRX,     0x85},
            {PPE_IDX_VDR28,    0x86},
            {PPE_IDX_VDR30,    0x87},
        },
        1, //GPE specific registers
        {   {0,     0x0},   //GPETSEL
            {1,     0x2},   //GPEDBG
            {2,     0x4},   //GPEMACR
            {3,     0x5},   //GPESWPR0
            {4,     0x6},   //GPESWPR1
        },
        {   {0,     "GPETSEL"   },
            {1,     "GPEDBG"    },
            {2,     "GPEMACR"   },
            {3,     "GPESWPR0"  },
            {4,     "GPESWPR1"  },
        },
    },//PPE_TYPE_GPE
    {
        "QME",
        PPE_QME_BASE_ADDRESS,
        8, //8 QMEs per chip, 1 per quad
        0x1000000, //Only valid if num_instances > 1
        {   {PPE_IDX_XIXCR,    0x200},
            {PPE_IDX_XIRAMRA,  0x204},
            {PPE_IDX_XIRAMGA,  0x208},
            {PPE_IDX_XIRAMDBG, 0x20C},
            {PPE_IDX_XIRAMEDR, 0x210},
            {PPE_IDX_XIDBGPRO, 0x214},
            {PPE_IDX_XIDBGINF, 0x23C}
        },
        {
            {PPE_IDX_XISIB,     0x218},
            {PPE_IDX_XIMEM,     0x21C},
            {PPE_IDX_XISGB,     0x220},
            {PPE_IDX_XIICAC,    0x224}
        },
        1,
        {   {PPE_IDX_VDR0,     0x280},
            {PPE_IDX_VDR2,     0x284},
            {PPE_IDX_VDR4,     0x288},
            {PPE_IDX_VDR6,     0x28c},
            {PPE_IDX_VDR8,     0x290},
            {PPE_IDX_VDRX,     0x294},
            {PPE_IDX_VDR28,    0x298},
            {PPE_IDX_VDR30,    0x29C},
        },
        1, //QME specific registers
        {   {0,     0x260},   //EISR
            {1,     0x264},   //EIMR
            {2,     0x268},   //EIPR
            {3,     0x26C},   //EITR
            {4,     0x270},   //EISTR
            {5,     0x274},   //EINR
        },
        {   {0,     "EISR"  },
            {1,     "EIMR"  },
            {2,     "EIPR"  },
            {3,     "EITR"  },
            {4,     "EISTR" },
            {5,     "EINR"  },
        },
    },//PPE_TYPE_QME
    {
        "IOPPE",
        PPE_IOPPE_BASE_ADDRESS,
        4, //4 IOPPEs per chip, 1 per PAU
        0x1000000, //Only valid if num_instances > 1
        {   {PPE_IDX_XIXCR,    0x10},
            {PPE_IDX_XIRAMRA,  0x11},
            {PPE_IDX_XIRAMGA,  0x12},
            {PPE_IDX_XIRAMDBG, 0x13},
            {PPE_IDX_XIDBGPRO, 0x14},
            {PPE_IDX_XIDBGINF, 0x1F}
        },
        {
            {PPE_IDX_XIMEM,     0x17},
            {PPE_IDX_XISGB,     0x18},
            {PPE_IDX_XIICAC,    0x19},
        },
        0,
        {   {0,  0 }},
        0,
        {   {0,  0 }},
        {{}}
    }//PPE_TYPE_IO
};
