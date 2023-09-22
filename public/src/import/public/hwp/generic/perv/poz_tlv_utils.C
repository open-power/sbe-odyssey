/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/hwp/generic/perv/poz_tlv_utils.C $   */
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
#include "poz_tlv_utils.H"
#include "file_access.H"
#include "poz_fastarray.H"

uint16_t get_compare_fail_count(std::vector<unload_config>& i_unload_configs)
{
    uint16_t l_compare_fail_count = 0;

    for (auto& l_unload_config : i_unload_configs)
    {
        if (l_unload_config.compare_status == compare_status_code::FAIL)
        {
            l_compare_fail_count++;
        }
    }

    return l_compare_fail_count;
};

void generate_ring_unload_tag(Target < TARGET_TYPE_PERV | TARGET_TYPE_CORE > i_target, const uint8_t& i_chip_num,
                              const uint32_t i_ring_address, unload_tag& o_tag)
{
    // 5 unload TAG bytes
    // 0 = unload / diagnostics
    // 1-3 = flags
    // 4-7 = chip number
    // 8-40 = ring address (or'ed with chiplet number)

    // Turn off diags flag
    o_tag.flags &= ~generic_tag_flags::DIAGS;

    // TODO: Set other generic tag flags here

    // Set chip position value in lower 4 bits of flags
    o_tag.flags |= (i_chip_num & 0x0F);
    // Set ring address in last 32 bits
    uint32_t cplt_address = i_target.getChipletNumber() << 24;
    o_tag.ring_address = cplt_address | (i_ring_address & 0x00FFFFFF);
}

void generate_diags_tag(const uint8_t& i_chip_num, const uint16_t& i_dict_def, diags_tag& o_tag)
{
    // 5 diags TAG bytes
    // 0 = unload / diagnostics
    // 1-3 = flags
    // 4-7 = chip number
    // 8-23 = more flags
    // 24-39 = dict def for return struct

    // Turn on diags flag
    o_tag.flags |= generic_tag_flags::DIAGS;

    // TODO: Set other generic tag flags here

    // Set chip position value in lower 4 bits of flags
    o_tag.flags |= (i_chip_num & 0x0F);
    // Set other diags flags here
}

ReturnCode poz_write_tlv_ring_unload(const unload_config& i_unload_config, const char* i_care_file,
                                     hwp_data_ostream& o_stream)
{
    auto chip = i_unload_config.unicast_target.getParent<TARGET_TYPE_ANY_POZ_CHIP>();
    const void* care_data;
    const void* cd_ptr;
    size_t size;
    unload_tag tag;
    uint8_t chip_num = 0; // TODO update me to work for future projects
    uint32_t care_bits_length;
    uint32_t deq_size = sizeof(hwp_data_unit) * 8; // Total number of bits in one deq
    hwp_data_unit o_stream_unit = 0x0000;

    // Open new scope so we can FAPI_TRY() across initializers at leisure
    {
        // Load the care mask file
        FAPI_TRY(loadEmbeddedFile(chip, i_care_file, care_data, size),
                 "Failed to load ring care mask for generic ring address 0x%08x on cplt %d, skipping...",
                 i_unload_config.base_ring_address,
                 i_unload_config.unicast_target.getChipletNumber());

        // Prep for unloading Tag
        generate_ring_unload_tag(i_unload_config.unicast_target, chip_num, i_unload_config.base_ring_address, tag);

        //// 4 BYTES OF TAG
        // Write first part of tag, leaving only 1 byte
        FAPI_TRY(o_stream.put(o_stream_unit | (tag.flags << 24 & 0xFF000000) | ((tag.ring_address >> 8) & 0x00FFFFFF)));

        // Prep for unloading Length
        // Cast as uint32_t and dereference to capture first 32 bits describing total # care bits
        care_bits_length = *(static_cast<const uint32_t*>(care_data));
        FAPI_DBG("TLV ring unload -- total care bits %d, ring address 0x%08x, chiplet num %d",
                 care_bits_length,
                 tag.ring_address,
                 i_unload_config.unicast_target.getChipletNumber());
        // Calculate total # of 32-bit deqs needed to deq that many care bits
        uint32_t deq_length = ((care_bits_length + deq_size - 1) / deq_size);
        // NOTE: length should not be more than 3 bytes worth of data
        FAPI_DBG("Writing last byte of TAG and 3 bytes of length %x",
                 o_stream_unit | (tag.ring_address << 24 & 0xFF000000) | (deq_length & 0x00FFFFFF));

        //// 1 BYTE OF TAG, 3 BYTES OF LENGTH
        // Write it back out through the FIFO w/ the rest of the ring address
        FAPI_TRY(o_stream.put(o_stream_unit | (tag.ring_address << 24 & 0xFF000000) | (deq_length & 0x00FFFFFF)));

        // Prep for unloading Value
        // Cast as uint32_t so we can move the pointer past the first 32 bits and then cast it back to void for use with sparse_getring
        cd_ptr = (void*)(static_cast<const uint32_t*>(care_data) + 1);
        // Create care data input stream
        hwp_be_array_istream i_stream((hwp_data_unit*)cd_ptr, size - 4);

        //// VALUE
        FAPI_TRY(poz_sparse_getring(i_unload_config.unicast_target, i_unload_config.base_ring_address, i_stream, o_stream));
        FAPI_DBG("Done scanning ring 0x%08x", tag.ring_address);

        cd_ptr = NULL; // End the dangling pointer
        freeEmbeddedFile(care_data);
    }
fapi_try_exit:
    return current_err;
}

ReturnCode poz_write_tlv_diags(const Target <TARGET_TYPE_ANY_POZ_CHIP>& i_target, const uint16_t& i_dict_def,
                               const size_t& i_struct_size, void* i_struct_ptr, hwp_data_ostream& o_stream)
{
    {
        // Write diags data to output stream
        uint32_t* l_struct_ptr = reinterpret_cast<uint32_t*>(i_struct_ptr);
        diags_tag l_diags_tag;
        uint8_t l_chip_num = 0; // TODO update me to work for future projects
        l_diags_tag.dict_def = i_dict_def;
        generate_diags_tag(l_chip_num, i_dict_def, l_diags_tag);
        uint32_t deq_size = sizeof(hwp_data_unit) * 8; // Total number of bits in one deq
        hwp_data_unit l_deq_length = (hwp_data_unit) ( ( (i_struct_size * 8) + (deq_size - 1) ) /
                                     (deq_size) ); // Calculate # of deqs to get full value out

        //// 4 BYTES OF TAG
        // Write first part of tag, leaving only 1 byte
        FAPI_TRY(o_stream.put((l_diags_tag.flags << 24 & 0xFF000000) |
                              ((l_diags_tag.diags_flags << 8) & 0x00FFFF00) |
                              ((l_diags_tag.dict_def >> 8) & 0x000000FF)));

        //// 1 BYTE OF TAG, 3 BYTES OF LENGTH
        // Write it back out through the FIFO w/ the rest of the ring address
        // NOTE: length should not be more than 3 bytes worth of data
        FAPI_TRY(o_stream.put((l_diags_tag.dict_def << 24 & 0xFF000000) | (l_deq_length & 0x00FFFFFF)));

        // VALUE
        // Write out the struct in 32-bit chunks to output stream
        for (uint32_t i = 0; i < (uint32_t) l_deq_length; i++)
        {
            FAPI_TRY(o_stream.put((hwp_data_unit) *l_struct_ptr));
            l_struct_ptr += 1;
        }

        i_struct_ptr = NULL; // End dangling pointer
        l_struct_ptr = NULL; // End dangling pointer
    }
fapi_try_exit:
    FAPI_INF("Exiting ...");
    return current_err;
}
