/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/emb/pst/pmlib/errl/pst_occ_sram_defines.h $ */
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
#ifndef __OCC_SRAM_DEFINES_H_
#define __OCC_SRAM_DEFINES_H_
#include <stdint.h>

/**
 * @file pst_occ_sram_defines.h $
 * @brief   enumerates all sources of activity count.
 * @note    Below are possible sources which can populate
 */


/// @addtogroup pm_hcode_lib
/// @{

/**
 * @brief core activity count area in TCC SRAM region.
 */
enum SRC_CORE_ACTIVITY_COUNT
{
    SRCACTCT_DISABLE        =   0x00,
    SRCACTCT_QME_TPIT       =   0x01,
    SRCACTCT_EXTERNAL       =   0x02,
    SRCACTCT_MANUAL         =   0x03,
};

/**
 * @brief header pointing powman hcode and data in OCC SRAM
 */
typedef struct
{
    uint8_t     iv_magic_word[3];                   ///< Magic Word to id the structure
    uint8_t     iv_occ_sram_header_ver;             ///< Header version number
    uint32_t    iv_reserve1;                        ///< Reserved
    uint32_t    iv_occ_hcode_struct_addr;           ///< Hcode structure address
    uint32_t    iv_occ_hcode_struct_len;            ///< Hcode structure length
    uint32_t    iv_occ_shared_sram_addr;            ///< Shared SRAM region address
    uint32_t    iv_occ_shared_sram_len;             ///< Shared SRAM region length
    uint32_t    iv_occ_pspb_addr;                   ///< Pstate Parameter Block address
    uint32_t    iv_occ_pspb_len;                    ///< Pstate Parameter Block length
    uint32_t    iv_occ_elog_table_addr;             ///< Error Log Table address
    uint32_t    iv_occ_elog_table_len;              ///< Error Log Table length
    uint32_t    iv_occ_wof_table_addr;              ///< WOF Table address
    uint32_t    iv_occ_wof_table_len;               ///< WOF Table length
    uint8_t     iv_reserve2[16];                    ///< Reserved
} Occ_sram_header_t;

/**
 * @brief   header pointing to hcode images in OCC SRAM.
 */
typedef struct
{
    //GPE0 region
    uint32_t    iv_gpe0_region_start;               ///< GPE0 region starting address
    uint32_t    iv_gpe0_image_header_address;       ///< GPE0 image header address
    uint32_t    iv_gpe0_debug_header_address;       ///< GPE0 debug header address
    uint8_t     iv_gpe0_reserve[20];                ///< Reserved

    //GPE1 region
    uint32_t    iv_gpe1_region_start;               ///< GPE1 region starting address
    uint32_t    iv_gpe1_image_header_address;       ///< GPE1 image header address
    uint32_t    iv_gpe1_debug_header_address;       ///< GPE1 debug header address
    uint8_t     iv_gpe1_reserve[20];                ///< Reserved

    //GPE2 region
    uint32_t    iv_gpe2_region_start;               ///< GPE2 region starting address
    uint32_t    iv_gpe2_image_header_address;       ///< GPE2 image header address
    uint32_t    iv_gpe2_debug_header_address;       ///< GPE2 debug header address
    uint8_t     iv_gpe2_reserve[20];                ///< Reserved

    //GPE3 region
    uint32_t    iv_gpe3_region_start;               ///< GPE3 region starting address
    uint32_t    iv_gpe3_image_header_address;       ///< GPE3 image header address
    uint32_t    iv_gpe3_debug_header_address;       ///< GPE3 debug header address
    uint8_t     iv_gpe3_reserve[20];                ///< Reserved

    uint8_t     iv_occ_reserve1[20];                ///< Reserved
    uint32_t    iv_gpe0_critical_elog;              ///< GPE0 critical error log address
    uint32_t    iv_ngpe_critical_elog_address;      ///< NGPE critical error log address
    uint32_t    iv_pgpe_critical_elog_address;      ///< PGPE critical error log address
    uint32_t    iv_xgpe_critical_elogAddress;       ///< XGPE critical error log address
    uint32_t    iv_occ_reserve2;                    ///< Reserved
    uint32_t    iv_ngpe_info_elog_address;          ///< NGPE info error log address
    uint32_t    iv_pgpe_info_elog_address;          ///< PGPE info error log address
    uint32_t    iv_xgpe_info_elog_address;          ///< XGPE info error log address
    uint8_t     iv_occ_reserve3[80];                ///< Reserved
    uint32_t    iv_aux_task_region_start_address;   ///< Auxiliary task region address
    uint32_t    iv_aux_task_region_length;          ///< Auxiliary task region length
    uint8_t     iv_occ_reserve4[8];                 ///< Reserved
} Occ_sram_hcode_pointer_t;

/// @}  end addtogroup

#endif
