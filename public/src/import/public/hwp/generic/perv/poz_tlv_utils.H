/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/hwp/generic/perv/poz_tlv_utils.H $   */
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
#include <fapi2.H>
#include <hwp_data_stream.H>

#ifndef POZ_TLV_UTILS_H_
#define POZ_TLV_UTILS_H_

using namespace fapi2;

/// @brief Struct to package ring address and unicast target together for use in TLV ring unloads
///
struct compare_fail
{
    uint32_t base_ring_address;
    Target < TARGET_TYPE_PERV | TARGET_TYPE_CORE > unicast_target;
};

/// @brief Enum for generic flags belonging either a ring-unload or diagnostics tag
///
enum generic_tag_flags : uint8_t
{
    DIAGS = 0x80,
    SPARSE = 0x40,
    GEN_FLAG1 = 0x20,
    GEN_FLAG2 = 0x10,
    // other 4 bits are for chip pos
};

/// @brief Enum for diagnostics-specific flags belonging in the diagnostics tag
///
enum diags_tag_flags : uint16_t
{
    DIAGS_FLAG0 = 0x8000,
    DIAGS_FLAG1 = 0x4000,
    DIAGS_FLAG2 = 0x2000,
    DIAGS_FLAG3 = 0x1000 // Total 16 bits worth of diags-unique flags for special requirements
};

/// @brief TLV Ring Unload Tag
///
struct unload_tag
{
    uint8_t flags = 0x00; // Diags or not + additional generic flags + chip position
    uint32_t ring_address = 0x00000000; // Chiplet-specific ring address
};

/// @brief TLV Diagnostics Tag
///
struct diags_tag
{
    uint8_t flags = 0x00; // Diags or not + additional generic flags + chip position
    uint16_t diags_flags = 0x0000; // Diags-specific flags
    uint16_t dict_def =
        0x0000; // Dictionary definition of return struct so that we know how to unpack on the other side of the FIFO
};


/// @brief Generate a 5-byte TLV Tag for ring unload contents
///
/// Generate a ring unload Tag describing outgoing ring contents --
/// bits[0-3] = Generic Tag flags, bit[0] == 0 indicating these are ring unload contents, not diagnostics
/// bits[4-7] = Chip position this data is coming from
/// bits[8-39] = Ring address for the scanned out data OR'd with chiplet address so that chiplet target is also known
///
/// @param[in]   i_target               Chiplet target
/// @param[in]   i_chip_num             Chip position
/// @param[in]   i_ring_address         Ring address for ring to be scanned out
/// @param[out]  o_tag                  Ring unload Tag with information about the outgoing ring data
///
void generate_ring_unload_tag(Target < TARGET_TYPE_PERV | TARGET_TYPE_CORE > i_target, const uint8_t& i_chip_num,
                              const uint32_t i_ring_address, unload_tag& o_tag);

/// @brief Generate a 5-byte TLV Tag for ring unload contents
///
/// Generate a diagnostics Tag describing the outgoing diagnostics data structure --
/// bits[0-3] = Generic Tag flags, bit[0] == 1 indicating these are diagnostics contents, not ring unload contents
/// bits[4-7] = Chip position this data is coming from
/// bits[8-23] = Diags-specific flags, giving more information about the type of diagnostics contents that are being returned
/// bits[24-39] = Dictionary definition of the output data structure (chipop cmd and cmdClass)
///
/// @param[in]   i_chip_num             Chip position
/// @param[in]   i_dict_def             Dictionary definition for input structure (chipop cmd+cmdClass)
/// @param[out]  o_tag                  Diagnostics Tag with information about the data structure
///
void generate_diags_tag(const uint8_t& i_chip_num, const uint16_t& i_dict_def, diags_tag& o_tag);

/// @brief Scan out and write TLV ring unload contents to an output stream
///
/// Create and write out a ring unload Tag to an output stream containing information about data to
/// be scanned out. Then calculate the Length of the outgoing unload contents based on total number of
/// care bits in the passed in care file. Then actually scan out and write the care data (Value) to the
/// output stream.
///
/// @param[in]   i_compare_fail         Compare fail data structure, defining target and ring address to scan out
/// @param[in]   i_care_file            Path to ring care mask file
/// @param[out]  o_stream               Output data stream
///
ReturnCode poz_write_tlv_ring_unload(const compare_fail& i_compare_fail,
                                     const char* i_care_file, fapi2::hwp_data_ostream& o_stream);

/// @brief Scan out and write TLV diagnostics contents to an output stream
///
/// Create and write out a diagnostics Tag to an output stream containing information about the outgoing
/// data structure. Then output the length of the outgoing data structure in terms of total number of data-unit transactions.
/// Then iterate through and write out the diagnostics structure in data-unit sized intervals to an output stream.
///
/// @param[in]   i_target               Chip target
/// @param[in]   i_dict_def             Dictionary definition for input structure (chipop cmd+cmdClass)
/// @param[in]   i_struct_size          Size of input structure (in bytes)
/// @param[in]   i_struct_ptr           Pointer to memory location of input structure
/// @param[out]  o_stream               Output data stream
///
ReturnCode poz_write_tlv_diags(const Target < TARGET_TYPE_ANY_POZ_CHIP | TARGET_TYPE_MULTICAST_CHIP > &i_target,
                               const uint16_t& i_dict_def, const size_t& i_struct_size, void* i_struct_ptr, fapi2::hwp_data_ostream& o_stream);

#endif
