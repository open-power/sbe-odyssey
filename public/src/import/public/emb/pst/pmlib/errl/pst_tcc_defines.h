/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/emb/pst/pmlib/errl/pst_tcc_defines.h $ */
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
#ifndef __TCC_DEFINES_H_
#define __TCC_DEFINES_H_

/**
 * @file pst_tcc_defines.h
 * @brief TCC SRAM Defines
 */

/// @addtogroup pm_hcode_lib
/// @{

/**
 * @brief   Header describing all powman code and data resident in TCC SRAM.
 */
typedef struct
{
    uint32_t iv_magic_word;                     ///< Magic Word to id the structure
    uint32_t iv_tcc_sram_header_ver;            ///< Header version number
    uint32_t iv_hcode_struct_addr;              ///< Hcode structure address
    uint32_t iv_hcode_struct_length;            ///< Hcode structure length
    uint32_t iv_tcc_shared_sram_addr;           ///< Shared SRAM region address
    uint32_t iv_tcc_shared_sram_length;         ///< Shared SRAM region length
    uint32_t iv_tcc_pspb_block_addr;            ///< Pstate Parameter Block address
    uint32_t iv_tcc_pspb_block_length;          ///< Pstate Parameter Block length
    uint32_t iv_tcc_elog_table_addr;            ///< Error Log Table address
    uint32_t iv_tcc_elog_table_length;          ///< Error Log Table length
    uint32_t iv_tcc_wof_table_addr;             ///< WOF Table address
    uint32_t iv_tcc_wof_length;                 ///< WOF Table length
    uint8_t  iv_reserve[16];                    ///< Reserved
} TccImageHeader_t;

/**
 * @brief   points to various powman hcode images in TCC SRAM
 */
typedef struct
{
    uint32_t iv_ce0RegionStart;                 ///< CE0 region starting address
    uint32_t iv_ce0ImageHeaderAddress;          ///< CE0 image header address
    uint32_t iv_ce0DebugHeaderAddress;          ///< CE0 debug header address
    uint8_t  iv_ce0Reserve[20];                 ///< Reserved
    uint32_t iv_ce1RegionStart;                 ///< CE1 region starting address
    uint32_t iv_ce1ImageHeaderAddress;          ///< CE1 image header address
    uint32_t iv_ce1DebugHeaderAddress;          ///< CE1 debug header address
    uint8_t  iv_ce1Reserve[20];                 ///< Reserved
    uint32_t iv_ce2RegionStart;                 ///< CE2 region starting address
    uint32_t iv_ce2ImageHeaderAddress;          ///< CE2 image header address
    uint32_t iv_ce2DebugHeaderAddress;          ///< CE2 debug header address
    uint8_t  iv_ce2Reserve[20];                 ///< Reserved
    uint32_t iv_ce3RegionStart;                 ///< CE3 region starting address
    uint32_t iv_ce3ImageHeaderAddress;          ///< CE3 image header address
    uint32_t iv_ce3DebugHeaderAddress;          ///< CE3 debug header address
    uint8_t  iv_ce3Reserve[20];                 ///< Reserved
    uint32_t iv_dceCriticalElogAddress;         ///< DCE critical error log address
    uint32_t iv_oceCriticalElogAddress;         ///< OCE critical error log address
    uint32_t iv_pceCriticalElogAddress;         ///< PCE critical error log address
    uint32_t iv_xceCriticalElogAddress;         ///< XCE critical error log address
    uint32_t iv_dceInfoElogAddress;             ///< DCE info error log address
    uint32_t iv_oceInfoElogAddress;             ///< OCE info error log address
    uint32_t iv_pceInfoElogAddress;             ///< PCE info error log address
    uint32_t iv_xceInfoElogAddress;             ///< XCE info error log address
    uint8_t  iv_tccReserve1[80];                ///< Reserved
    uint32_t iv_auxTaskRegionStart;             ///< Auxiliary task region address
    uint32_t iv_auxTaskRegionLength;            ///< Auxiliary task region length
    uint8_t  iv_tccReserve2[8];                 ///< Reserved
} TccHcodePtr_t;

/// @}  end addtogroup

#endif
