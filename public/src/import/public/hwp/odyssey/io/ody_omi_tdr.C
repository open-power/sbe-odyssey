/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/hwp/odyssey/io/ody_omi_tdr.C $       */
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
///------------------------------------------------------------------------------
/// @file ody_omi_tdr.C
/// @brief Odyssey HWP that runs TDR on links
///
/// *HWP HW Maintainer : Josh Chica <josh.chica@ibm.com>
/// *HWP FW Maintainer :
/// *HWP Consumed by: SBE
///------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------
#include <cmath>
#include <ody_omi_tdr.H>
#include <common_io_tdr.H>
#include <ody_io_ppe_common.H>
#include <ody_scom_omi.H>
#include <io_scom_lib.H>

SCOMT_OMI_USE_D_REG_DL0_STATUS

enum PRECURSOR_DATA
{
    PRE1_REG = 0x4640,
    PRE1_BIT = 48,
    PRE1_LEN = 8,
    PRE2_REG = 0x45C0,
    PRE2_BIT = 48,
    PRE2_LEN = 5,
    POST_REG = 0x4BC0,
    POST_BIT = 48,
    POST_LEN = 8,
};

//------------------------------------------------------------------------------
// Function definitions
//------------------------------------------------------------------------------
inline fapi2::ReturnCode getPrecursor(const fapi2::Target<fapi2::TARGET_TYPE_OCMB_CHIP>& i_target,
                                      const uint8_t i_lane,
                                      uint8_t& o_pre1,
                                      uint8_t& o_pre2,
                                      uint8_t& o_post)
{
    FAPI_DBG("Start OMI TDR::getPrecursor");

    constexpr uint8_t c_thread = 0;

    fapi2::buffer<uint64_t> l_buffer = 0;
    fapi2::buffer<uint64_t> l_addr = 0;

    // Get the precursor
    l_addr = buildAddr(PHY_ODY_OMI_BASE, c_thread, i_lane, PRE1_REG);
    FAPI_TRY(fapi2::getScom(i_target, l_addr, l_buffer));
    l_buffer.extractToRight<PRE1_BIT, PRE1_LEN, uint8_t>(o_pre1);
    l_buffer.flush<0>();

    l_addr = buildAddr(PHY_ODY_OMI_BASE, c_thread, i_lane, PRE2_REG);
    FAPI_TRY(fapi2::getScom(i_target, l_addr, l_buffer));
    l_buffer.extractToRight<PRE2_BIT, PRE2_LEN, uint8_t>(o_pre2);
    l_buffer.flush<0>();

    l_addr = buildAddr(PHY_ODY_OMI_BASE, c_thread, i_lane, POST_REG);
    FAPI_TRY(fapi2::getScom(i_target, l_addr, l_buffer));
    l_buffer.extractToRight<POST_BIT, POST_LEN, uint8_t>(o_post);

fapi_try_exit:
    FAPI_DBG("End OMI TDR::getPrecursor");
    return fapi2::current_err;
}

inline fapi2::ReturnCode setPrecursor(const fapi2::Target<fapi2::TARGET_TYPE_OCMB_CHIP>& i_target,
                                      const uint8_t i_lane,
                                      const uint8_t i_pre1,
                                      const uint8_t i_pre2,
                                      const uint8_t i_post)
{
    FAPI_DBG("Start OMI TDR::setPrecursor");

    constexpr uint8_t c_thread = 0;

    fapi2::buffer<uint64_t> l_buffer = 0;
    fapi2::buffer<uint64_t> l_addr = 0;

    // Set the precursor
    l_addr = buildAddr(PHY_ODY_OMI_BASE, c_thread, i_lane, PRE1_REG);
    FAPI_TRY(fapi2::getScom(i_target, l_addr, l_buffer));
    l_buffer.insertFromRight<PRE1_BIT, PRE1_LEN, uint8_t>(i_pre1);
    FAPI_TRY(fapi2::putScom(i_target, l_addr, l_buffer));
    l_buffer.flush<0>();

    l_addr = buildAddr(PHY_ODY_OMI_BASE, c_thread, i_lane, PRE2_REG);
    FAPI_TRY(fapi2::getScom(i_target, l_addr, l_buffer));
    l_buffer.insertFromRight<PRE2_BIT, PRE2_LEN, uint8_t>(i_pre2);
    FAPI_TRY(fapi2::putScom(i_target, l_addr, l_buffer));
    l_buffer.flush<0>();

    l_addr = buildAddr(PHY_ODY_OMI_BASE, c_thread, i_lane, POST_REG);
    FAPI_TRY(fapi2::getScom(i_target, l_addr, l_buffer));
    l_buffer.insertFromRight<POST_BIT, POST_LEN, uint8_t>(i_post);
    FAPI_TRY(fapi2::putScom(i_target, l_addr, l_buffer));

fapi_try_exit:
    FAPI_DBG("End OMI TDR::setPrecursor");
    return fapi2::current_err;
}

fapi2::ReturnCode ody_omi_tdr(const fapi2::Target<fapi2::TARGET_TYPE_OCMB_CHIP>& i_target,
                              const uint32_t i_link_mask,
                              fapi2::hwp_bit_ostream& o_ostream)
{
    FAPI_DBG("Start - OMI TDR Isolation");

    constexpr uint64_t c_start_cmd = ody_io::POWER_ON_PL | ody_io::TX_FIFO_INIT_PL | ody_io::TX_FFE_PL;
    constexpr uint64_t c_stop_cmd = ody_io::POWER_OFF_PL | ody_io::TX_FFE_PL;
    constexpr uint8_t c_lane_pos = 48;
    constexpr uint8_t c_status_pos = 40;
    constexpr uint8_t c_max_lanes = 8;
    constexpr uint8_t c_thread = 0;

    io_ppe_regs<fapi2::TARGET_TYPE_OCMB_CHIP> l_ppe_regs(PHY_ODY_OMI_BASE);

    ody_io::io_ppe_common<fapi2::TARGET_TYPE_OCMB_CHIP> l_ppe_common(&l_ppe_regs);

    scomt::omi::D_REG_DL0_STATUS_t l_dl0_status;

    fapi2::ATTR_FREQ_OMI_MHZ_Type l_freq;
    uint64_t l_data = 0;
    uint32_t l_group = 0x0;
    uint32_t l_training_state = 0;
    uint32_t l_disabled_lanes = 0;
    uint32_t l_fail = 0;
    uint8_t l_done = 0;
    uint8_t l_tx_pre1 = 0;
    uint8_t l_tx_pre2 = 0;
    uint8_t l_tx_post = 0;
    uint8_t l_pos = 0;

    fapi2::ATTR_MFG_FLAGS_Type l_mfg_flags = {0};
    TdrResult l_status = TdrResult::None;
    uint32_t l_length_ps = 0;

    FAPI_TRY(FAPI_ATTR_GET(fapi2::ATTR_BUS_POS, i_target, l_pos));
    FAPI_TRY(FAPI_ATTR_GET(fapi2::ATTR_FREQ_OMI_MHZ, i_target, l_freq));
    FAPI_TRY(FAPI_ATTR_GET(fapi2::ATTR_MFG_FLAGS, fapi2::Target<fapi2::TARGET_TYPE_SYSTEM>(), l_mfg_flags));

    FAPI_TRY(l_dl0_status.getScom(i_target));
    l_training_state = l_dl0_status.get_TRAINING_STATE_MACHINE();
    l_disabled_lanes = l_dl0_status.get_LANES_DISABLED();

    FAPI_TRY(l_ppe_common.ext_cmd_start(i_target, c_thread, c_max_lanes, 0, ody_io::CLEAR))
    FAPI_TRY(l_ppe_common.ext_cmd_poll(i_target, c_thread, ody_io::CLEAR, l_done, l_fail));

    /*
        This HWP will run TDR on the lanes to determine their status. It saves:
        [00:07]: Group
        [08:15]: Lane
        [16:23]: Status
        [24:31]: Reserved (Backup)
        [32:63]: Delay Location
        It writes this information to a file in /tmp.
    */

    // Check if link is trained
    if (l_training_state == 0x7) // Trained / Link up
    {
        for (uint8_t l_lane = 0; l_lane < c_max_lanes; l_lane++)
        {
            l_data = l_data | (static_cast<uint64_t>(l_lane) << c_lane_pos);

            // If the lane is not being actively used run TDR
            if ((0x01 << l_lane) & l_disabled_lanes) // Lane is disabled
            {
                FAPI_DBG("ody_omi_tdr::power on lane %d calling l_ppe.ext_cmd_start", l_lane);

                // Save the precursor
                getPrecursor(i_target, l_lane, l_tx_pre1, l_tx_pre2, l_tx_post);

                // Clear the precursor
                setPrecursor(i_target, l_lane, 0, 0, 0);

                // Power on lane
                FAPI_TRY(l_ppe_common.ext_cmd_start(i_target, c_thread, l_lane, 0, c_start_cmd));
                FAPI_TRY(l_ppe_regs.flushCache(i_target));
                FAPI_TRY(l_ppe_common.ext_cmd_poll(i_target, c_thread, c_start_cmd, l_done, l_fail));

                // Run TDR
                FAPI_TRY(common_io_tdr(i_target, PHY_ODY_OMI_BASE, l_group, l_lane, l_freq, l_status, l_length_ps));

                // Set data
                FAPI_DBG("Checking on lane %d with status %d.", l_lane, l_status);
                l_data |= (static_cast<uint64_t>(l_status) << c_status_pos);
                l_data |= l_length_ps;

                // Reset the precursor
                setPrecursor(i_target, l_lane, l_tx_pre1, l_tx_pre2, l_tx_post);

                // Power off lane
                FAPI_DBG("ody_omi_tdr::power off lanes calling l_ppe.ext_cmd_start");
                FAPI_TRY(l_ppe_common.ext_cmd_start(i_target, c_thread, l_lane, 0, c_stop_cmd));
                FAPI_TRY(l_ppe_regs.flushCache(i_target));
                FAPI_TRY(l_ppe_common.ext_cmd_poll(i_target, c_thread, c_stop_cmd, l_done, l_fail));
            }
            // Lane in use, TDR not run
            else
            {
                FAPI_DBG("Lane %d is up & trained.", l_lane);
                l_data |= (static_cast<uint64_t>(TdrResult::DidNotRun) << c_status_pos);
            }

            o_ostream.put64(l_data);
            // Clear the lane & other data without touching the group
            // Not necessary here, as group will always be 0 for Ody
            l_data &= 0xFF00000000000000;
        }
    }
    else // Not trained / Link down
    {
        for (uint8_t l_lane = 0; l_lane < 8; l_lane++)
        {

            // Save the precursor
            getPrecursor(i_target, l_lane, l_tx_pre1, l_tx_pre2, l_tx_post);

            // Clear the precursor
            setPrecursor(i_target, l_lane, 0, 0, 0);

            FAPI_DBG("ody_omi_tdr::power on lanes calling l_ppe.ext_cmd_start");
            FAPI_TRY(l_ppe_common.ext_cmd_start(i_target, c_thread, l_lane, 0, c_start_cmd));
            FAPI_TRY(l_ppe_regs.flushCache(i_target));
            FAPI_TRY(l_ppe_common.ext_cmd_poll(i_target, c_thread, c_start_cmd, l_done, l_fail));

            FAPI_ASSERT(l_done && !l_fail,
                        fapi2::IO_EXT_CMD_POLL_FAILED()
                        .set_POS(l_pos)
                        .set_FAIL(l_fail)
                        .set_DONE(l_done)
                        .set_TARGET(i_target),
                        "IO Ext Cmd Poll (0x%08X) Done Fail on %d :: Done(%d), Fail(0x%04X)",
                        c_start_cmd, l_pos, l_done, l_fail);

            l_data |= (static_cast<uint64_t>(l_lane) << c_lane_pos);

            FAPI_TRY(common_io_tdr(i_target, PHY_ODY_OMI_BASE, l_group, l_lane, l_freq, l_status, l_length_ps));

            FAPI_DBG("Checking on lane %d with status %d.", l_lane, l_status);

            l_data |= (static_cast<uint64_t>(l_status) << c_status_pos);
            l_data |= l_length_ps;
            o_ostream.put64(l_data);
            // Clear the lane & other data without touching the group
            // Not necessary here, as group will always be 0 for Ody
            l_data &= 0xFF00000000000000;

            // Reset the precursor
            setPrecursor(i_target, l_lane, l_tx_pre1, l_tx_pre2, l_tx_post);

            FAPI_DBG("ody_omi_tdr::power off lanes calling l_ppe.ext_cmd_start");
            FAPI_TRY(l_ppe_common.ext_cmd_start(i_target, c_thread, l_lane, 0, c_stop_cmd));
            FAPI_TRY(l_ppe_regs.flushCache(i_target));
            FAPI_TRY(l_ppe_common.ext_cmd_poll(i_target, c_thread, c_stop_cmd, l_done, l_fail));

            FAPI_ASSERT(l_done && !l_fail,
                        fapi2::IO_EXT_CMD_POLL_FAILED()
                        .set_POS(l_pos)
                        .set_FAIL(l_fail)
                        .set_DONE(l_done)
                        .set_TARGET(i_target),
                        "IO Ext Cmd Poll (0x%08X) Done Fail on %d :: Done(%d), Fail(0x%04X)",
                        c_stop_cmd, l_pos, l_done, l_fail);
        }
    }

    FAPI_TRY(o_ostream.flush());

fapi_try_exit:
    FAPI_DBG("End");
    return fapi2::current_err;
}
