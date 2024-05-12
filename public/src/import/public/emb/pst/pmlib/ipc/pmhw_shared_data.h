/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/emb/pst/pmlib/ipc/pmhw_shared_data.h $ */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2022,2024                        */
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
#ifndef __PMHW_SHARED_DATA_H__
#define __PNHW_SHARED_DATA_H__

/// \file pmhw_shared_data.h
/// \brief Common header for shared data within the OCC and TCC complexes
///

//CMO: The term "OSD" has been changed as follows:
//     - To "PLATSD" for PLATform Shared Data in cases where the define is
//       platform dependent.
//     - To "PMSD" for Power Management Shared Data in cases where the define
//       is identically defined on both platforms.

#include <kernel.h>
#include "ipc_structs.h"
#include <occhw_scom_cmd.h>

/// Hardcoded address for the location of the OCC and TCC shared data segments.
/// In case of the OCC, this is placed in the non-cacheable aliased region of SRAM space.
#ifndef PLATSD_ADDR
    #if defined(__OCC_PLAT)
        #include <pst_hcd_memmap_occ_sram.H>
        #define PLATSD_ADDR  (OCC_SRAM_IPC_REGION_BASE_ADDR) //=0xffe00000
    #elif defined(__TCC_PLAT)
        #include <pst_hcd_memmap_tcc_sram.H>
        #define PLATSD_ADDR  (TCC_SRAM_IPC_REGION_BASE_ADDR) //=0xfffc0000
    #else
        #error "Either __OCC_PLAT or __TCC_PLAT must be defined by the application"
    #endif
#endif

/// Total space of the PM shared data segment
#define PMSD_TOTAL_SHARED_DATA_BYTES 4096

/// Reserve space for IPC data in case it needs to grow
#define PMSD_IPC_RESERVED_BYTES 2048

/// Reserve space for Debug
#define PMSD_DEBUG_RESERVED_BYTES 512

#ifdef __OCC_PLAT
    #define OSD_GPE_SCOM_ADDR (PLATSD_ADDR + PMSD_IPC_RESERVED_BYTES + PMSD_DEBUG_RESERVED_BYTES)

    #define OSD_GPE_SCOM_RESERVED_BYTES 32
#endif

#ifndef __ASSEMBLER__
typedef union
{
    struct
    {
        union
        {
            ipc_shared_data_t   ipc_data;   //880 bytes
            uint8_t             ipc_reserved[PMSD_IPC_RESERVED_BYTES + PMSD_DEBUG_RESERVED_BYTES];
        };
#ifdef __OCC_PLAT
        union
        {
            occhw_scom_cmd_t    scom_cmd;
            uint8_t             gpe_scom_reserved[OSD_GPE_SCOM_RESERVED_BYTES];
        };
#endif
    };
    uint8_t total_reserved[PMSD_TOTAL_SHARED_DATA_BYTES];
} pmhw_psd_t;

// Fail to compile if ipc_shared_data exceeds the reserved space
KERN_STATIC_ASSERT((sizeof(ipc_shared_data_t) <= PMSD_IPC_RESERVED_BYTES));

/// Hardcoded pointer for the location of the Platform (OCC or TCC) shared data segment
#define PLATSD_PTR ((pmhw_psd_t*) PLATSD_ADDR)

#endif /*__ASSEMBLER__*/

#endif /* __PMHW_SHARED_DATA_H__ */
