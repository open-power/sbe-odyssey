/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/hwp/odyssey/io/ody_omi_unload.C $    */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2023,2024                        */
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
///------------------------------------------------------------------------------
/// @file ody_omi_unload.C
/// @brief Streams IO Data back in payload for report
///
/// *HWP HW Maintainer : Josh Chica <josh.chica@ibm.com>
/// *HWP FW Maintainer :
/// *HWP Consumed by: SBE
///------------------------------------------------------------------------------

#include <ody_omi_unload.H>
#include <common_unload.H>
#include <ody_io_ppe_common.H>

enum section_header_numbers : uint64_t
{
    DL_Regs = 0,
    Flat_Scom_Regs = 1,
    Mem_Regs_Per_PPE = 2,
    Mem_Regs_Per_Thread = 3,
    Tx_Pl_Hw_Regs = 4,
    Rx_Pl_Hw_Regs = 5,
    Tx_Pg_Hw_Regs = 6,
    Rx_Pg_Hw_Regs = 7
};

/// @brief                      Stream DL data to file
/// @param[in] i_target         Target Chip
/// @param[out] o_ostream       Streaming object to write data to
void stream_ody_dl_data(const fapi2::Target<fapi2::TARGET_TYPE_OCMB_CHIP>& i_target,
                        fapi2::hwp_bit_ostream& o_ostream)
{
    const uint64_t c_base_addr = 0x08012400;
    uint64_t l_offsets[] = {0x00, 0x02, 0x08, 0x0E, 0x10, 0x11, 0x12, 0x13, 0x15, 0x16, 0x17, 0x1C, 0x1D, 0x1E, 0x1F};

    fapi2::buffer<uint64_t> l_data = 0;
    uint64_t l_addr = 0;
    uint32_t l_rc = 0; // FAPI_RC_SUCCESS

    for (uint8_t i = 0; i < sizeof(l_offsets) / sizeof(l_offsets[0]); i++)
    {
        l_addr = c_base_addr | l_offsets[i];
        l_rc = fapi2::getScom(i_target, l_addr, l_data); // FIR Bits

        if (l_rc)
        {
            l_data = 0xDEADDEADDEADDEAD;
        }

        o_ostream.put64(l_data);
    }

    o_ostream.flush();
}

fapi2::ReturnCode ody_omi_unload(const fapi2::Target<fapi2::TARGET_TYPE_OCMB_CHIP>& i_target,
                                 const uint32_t i_chipunit_mask,
                                 fapi2::hwp_bit_ostream& o_ostream)
{
    FAPI_DBG("Starting ody_omi_unload");

    constexpr uint64_t c_section_mark = 0xAC1D000000000000;
    constexpr uint32_t l_groups = 1;
    constexpr uint32_t l_lanes = 8;
    constexpr uint32_t l_threads = 1;
    constexpr uint16_t c_REVISION = 0x02;
    constexpr uint8_t c_section_number_shift = 40;

    io_ppe_regs<fapi2::TARGET_TYPE_OCMB_CHIP> l_ppe_regs(PHY_ODY_OMI_BASE);
    ody_io::io_ppe_common<fapi2::TARGET_TYPE_OCMB_CHIP> l_ppe_common(&l_ppe_regs);

    uint64_t l_section_header = 0xAC1D000000000000;

    FAPI_DBG("HWP: I/O UNLOAD: Base Addr(0x%08X) Groups(%d) Lanes(%d)", PHY_ODY_OMI_BASE, l_groups, l_lanes);

    // 1. DL Regs
    // 2. Flat Scom Regs (64b reads)
    // 3. Mem Regs (64b reads, only need the mem-regs sections)
    // 4. Tx Hardware Regs (16bit reads, will pack them to 32b fifo entries)
    // 5. Rx Hardware Regs (16bit reads, will pack them to 32b fifo entries)

    o_ostream.put8(c_REVISION);
    o_ostream.put8(0x01);   // System number Ody = 0x01
    o_ostream.put8(0);      // FBC Drawer
    o_ostream.put8(0);      // FBC DCM
    o_ostream.put8(0);      // FBC Chip

    o_ostream.put8(0);      // Reserved
    o_ostream.put16(0);     // Reserved
    o_ostream.put32(i_chipunit_mask);

    // 1. DL regs
    l_section_header = c_section_mark | (DL_Regs << c_section_number_shift) | PHY_ODY_OMI_BASE;
    o_ostream.put64(l_section_header);
    stream_ody_dl_data(i_target, o_ostream);

    // 2. Flat Scom Regs (64b reads): 18 * 2 = 36(32B)
    l_section_header = c_section_mark | (Flat_Scom_Regs << c_section_number_shift) | PHY_ODY_OMI_BASE;
    o_ostream.put64(l_section_header);
    stream_scom_data(i_target, o_ostream, PHY_ODY_OMI_BASE);

    // 3. Mem Regs (64b reads, only need the mem-regs sections)
    //  Image Regs (32B)
    l_section_header = c_section_mark | (Mem_Regs_Per_PPE << c_section_number_shift) | PHY_ODY_OMI_BASE;
    o_ostream.put64(l_section_header);
    // Stop the threads so it doesn't corrupt the data
    FAPI_TRY(l_ppe_common.stop_thread(i_target, PHY_ODY_NUM_THREADS, true));
    stream_mem_data_pp(i_target, PHY_ODY_OMI_BASE, o_ostream);
    l_section_header = c_section_mark | (Mem_Regs_Per_Thread << c_section_number_shift) | PHY_ODY_OMI_BASE;
    o_ostream.put64(l_section_header);
    stream_mem_data_pt(i_target, PHY_ODY_OMI_BASE, l_threads, o_ostream);
    // Restart the threads
    FAPI_TRY(l_ppe_common.stop_thread(i_target, PHY_ODY_NUM_THREADS, false));

    // 4-5. Tx Hardware Regs (16bit reads, will pack them to 32b fifo entries)

    // RX PL Regs (0x000 - 0x07C)
    // TX PL Regs (0x080 - 0x0A5)
    // RX PG Regs (0x100 - 0x16F)
    // TX PG Regs (0x180 - 0x1AD)

    // Rx/Tx PL Regs

    l_section_header = c_section_mark | (Tx_Pl_Hw_Regs << c_section_number_shift) | PHY_ODY_OMI_BASE;
    o_ostream.put64(l_section_header);
    stream_hw_data(i_target, o_ostream, PHY_ODY_OMI_BASE, l_groups, l_lanes, true, true);
    l_section_header = c_section_mark | (Rx_Pl_Hw_Regs << c_section_number_shift) | PHY_ODY_OMI_BASE;
    o_ostream.put64(l_section_header);
    stream_hw_data(i_target, o_ostream, PHY_ODY_OMI_BASE, l_groups, l_lanes, false, true);

    // Rx/Tx PG Regs
    l_section_header = c_section_mark | (Tx_Pg_Hw_Regs << c_section_number_shift) | PHY_ODY_OMI_BASE;
    o_ostream.put64(l_section_header);
    stream_hw_data(i_target, o_ostream, PHY_ODY_OMI_BASE, l_groups, 1, true, false);

    l_section_header = c_section_mark | (Rx_Pg_Hw_Regs << c_section_number_shift) | PHY_ODY_OMI_BASE;
    o_ostream.put64(l_section_header);
    stream_hw_data(i_target, o_ostream, PHY_ODY_OMI_BASE, l_groups, 1, false, false);


fapi_try_exit:
    FAPI_DBG("End ody_omi_unload");
    return fapi2::current_err;
}
