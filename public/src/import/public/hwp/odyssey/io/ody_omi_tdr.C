/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/hwp/odyssey/io/ody_omi_tdr.C $       */
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
#include <ody_omi_tdr.H>
#include <ody_io_tdr_utils.H>
#include <ody_io_ppe_common.H>
#include <ody_scom_omi.H>
#include <io_scom_lib.H>

SCOMT_OMI_USE_D_REG_DL0_STATUS

#define TX_PSAVE_FORCE_REQ_0_15_0 0x19C8
#define TX_PSAVE_FENCE_REG_DL_IO_0_15 0x1988
#define TX_PSAVE_START 48
#define TX_PSAVE_LENGTH 16

enum STREAM_DATA
{
    CLEAR_CMD = 0,
    OMI_START_CMD = ody_io::TX_FIFO_INIT_PL,
    LANE_POS = 48,
    GROUP_POS = 56,
    STATUS_POS = 40,
};
enum PRECURSOR_DATA
{
    PRE1_REG = 0x4640,
    PRE2_REG = 0x45C0,
    POST_REG = 0x4BC0,
};

//------------------------------------------------------------------------------
// Function definitions
//------------------------------------------------------------------------------
fapi2::ReturnCode run_ext_cmd(const fapi2::Target<fapi2::TARGET_TYPE_OCMB_CHIP>& i_target,
                              const uint32_t i_ext_cmd,
                              const uint32_t i_phy_lane)
{
    io_ppe_regs<fapi2::TARGET_TYPE_OCMB_CHIP> l_ppe_regs(PHY_ODY_OMI_BASE);

    ody_io::io_ppe_common<fapi2::TARGET_TYPE_OCMB_CHIP> l_ppe_common(&l_ppe_regs);

    uint32_t l_fail = 0;
    uint8_t l_done = 0;
    uint8_t l_pos = 99;
    uint32_t l_phy_tx_lane_mask = 0x80000000 >> i_phy_lane;

    FAPI_TRY(FAPI_ATTR_GET(fapi2::ATTR_BUS_POS, i_target, l_pos));
    FAPI_INF("ody_hss_tdr::run_ext_cmd group(%d) lane_mask(0x%08X) running ExtCmd(0x%08X)", 0, l_phy_tx_lane_mask,
             i_ext_cmd);
    FAPI_TRY(l_ppe_common.ext_cmd_start(i_target, 0, 0, l_phy_tx_lane_mask, i_ext_cmd));
    FAPI_TRY(l_ppe_common.iv_regs->flushCache(i_target));
    FAPI_TRY(l_ppe_common.ext_cmd_poll(i_target, 0, i_ext_cmd, l_done, l_fail));
    FAPI_ASSERT(l_done && !l_fail,
                fapi2::IO_PPE_DONE_POLL_FAILED()
                .set_POS(l_pos)
                .set_FAIL(l_fail)
                .set_DONE(l_done)
                .set_TARGET(i_target),
                "IO Ext Cmd Poll (0x%08X) Done Fail on %d :: Done(%d), Fail(0x%08X)",
                i_ext_cmd, l_pos, l_done, l_fail);

fapi_try_exit:
    return fapi2::current_err;
}

fapi2::ReturnCode lane_power_on(const fapi2::Target<fapi2::TARGET_TYPE_OCMB_CHIP>& i_target,
                                const uint32_t i_phy_lane)
{
    fapi2::buffer<uint64_t> l_buffer;
    uint64_t l_addr = 0;

    l_buffer.flush<0>();
    l_buffer.insertFromRight<TX_PSAVE_START, TX_PSAVE_LENGTH>(0x8000 >> i_phy_lane);
    l_addr = buildAddr(PHY_ODY_OMI_BASE, 0, 0, TX_PSAVE_FORCE_REQ_0_15_0);
    FAPI_TRY(fapi2::putScom(i_target, l_addr, l_buffer));
    l_addr = buildAddr(PHY_ODY_OMI_BASE, 0, 0, TX_PSAVE_FENCE_REG_DL_IO_0_15);
    FAPI_TRY(fapi2::putScom(i_target, l_addr, l_buffer));

    // Tx Fifo
    FAPI_TRY(run_ext_cmd(i_target, CLEAR_CMD, i_phy_lane));
    FAPI_TRY(run_ext_cmd(i_target, OMI_START_CMD, i_phy_lane));

fapi_try_exit:
    return fapi2::current_err;
}

fapi2::ReturnCode lane_power_off(const fapi2::Target<fapi2::TARGET_TYPE_OCMB_CHIP>& i_target,
                                 const uint32_t i_phy_lane)
{
    fapi2::buffer<uint64_t> l_buffer;
    uint64_t l_addr = 0;

    l_buffer.flush<0>();
    l_addr = buildAddr(PHY_ODY_OMI_BASE, 0, 0, TX_PSAVE_FORCE_REQ_0_15_0);
    FAPI_TRY(fapi2::putScom(i_target, l_addr, l_buffer));
    l_addr = buildAddr(PHY_ODY_OMI_BASE, 0, 0, TX_PSAVE_FENCE_REG_DL_IO_0_15);
    FAPI_TRY(fapi2::putScom(i_target, l_addr, l_buffer));

fapi_try_exit:
    return fapi2::current_err;
}

inline fapi2::ReturnCode clear_ffe_selects(const fapi2::Target<fapi2::TARGET_TYPE_OCMB_CHIP>& i_target,
        const uint64_t i_base_addr,
        const uint8_t i_lane)
{
    uint64_t l_addr = 0;
    l_addr = buildAddr(i_base_addr, 0, i_lane, PRE1_REG);
    FAPI_TRY(fapi2::putScom(i_target, l_addr, 0));
    l_addr = buildAddr(i_base_addr, 0, i_lane, PRE2_REG);
    FAPI_TRY(fapi2::putScom(i_target, l_addr, 0));
    l_addr = buildAddr(i_base_addr, 0, i_lane, POST_REG);
    FAPI_TRY(fapi2::putScom(i_target, l_addr, 0));

fapi_try_exit:
    return fapi2::current_err;
}

fapi2::ReturnCode ody_omi_tdr(const fapi2::Target<fapi2::TARGET_TYPE_OCMB_CHIP>& i_target,
                              const uint32_t i_link_mask,
                              const uint32_t i_lane_on,
                              fapi2::hwp_bit_ostream& o_ostream)
{
    FAPI_DBG("Start - OMI TDR Isolation");

    constexpr uint8_t c_thread = 0;

    io_ppe_regs<fapi2::TARGET_TYPE_OCMB_CHIP> l_ppe_regs(PHY_ODY_OMI_BASE);

    ody_io::io_ppe_common<fapi2::TARGET_TYPE_OCMB_CHIP> l_ppe_common(&l_ppe_regs);

    scomt::omi::D_REG_DL0_STATUS_t l_dl0_status;

    fapi2::ATTR_FREQ_OMI_MHZ_Type l_freq;
    uint64_t l_data = 0;
    uint32_t l_group = 0x0;
    uint32_t l_disabled_lanes = 0;
    uint32_t l_fail = 0;
    uint8_t l_done = 0;
    uint8_t l_pos = 0;

    fapi2::ATTR_MFG_FLAGS_Type l_mfg_flags = {0};
    TdrResult l_status = TdrResult::None;
    uint32_t l_length_ps = 0;

    FAPI_TRY(FAPI_ATTR_GET(fapi2::ATTR_BUS_POS, i_target, l_pos));
    FAPI_TRY(FAPI_ATTR_GET(fapi2::ATTR_FREQ_OMI_MHZ, i_target, l_freq));
    FAPI_TRY(FAPI_ATTR_GET(fapi2::ATTR_MFG_FLAGS, fapi2::Target<fapi2::TARGET_TYPE_SYSTEM>(), l_mfg_flags));

    FAPI_TRY(l_dl0_status.getScom(i_target));
    l_disabled_lanes = l_dl0_status.get_LANES_DISABLED();

    FAPI_TRY(l_ppe_common.ext_cmd_start(i_target, c_thread, PHY_ODY_NUM_LANES, 0, ody_io::CLEAR))
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

    for (uint8_t l_lane = 0; l_lane < PHY_ODY_NUM_LANES; l_lane++)
    {
        l_status = TdrResult::DidNotRun;
        l_length_ps = 0;

        // If the lane is not being actively used run TDR
        if ((0x80000000 >> l_lane) & l_disabled_lanes) // Lane is disabled
        {
            FAPI_TRY(clear_ffe_selects(i_target, PHY_ODY_OMI_BASE, l_lane));

            // Power on lane
            FAPI_TRY(lane_power_on(i_target, l_lane));

            // Run TDR
            FAPI_TRY(ody_io_tdr(i_target, PHY_ODY_OMI_BASE, l_group, l_lane, l_freq, l_status, l_length_ps));

            // Set data
            FAPI_DBG("Checking on lane %d with status %d.", l_lane, l_status);

            if (i_lane_on == 0)
            {
                // Power off lane
                FAPI_TRY(lane_power_off(i_target, l_lane));
            }
        }
        // Lane in use, TDR not run
        else
        {
            FAPI_DBG("Lane %d is up & trained.", l_lane);
        }

        l_data = l_data | (static_cast<uint64_t>(l_lane) << LANE_POS);
        l_data |= (static_cast<uint64_t>(l_status) << STATUS_POS);
        l_data |= l_length_ps;

        o_ostream.put64(l_data);
        // Clear the lane & other data without touching the group
        // Not necessary here, as group will always be 0 for Ody
        l_data &= 0xFF00000000000000;
    }

    FAPI_TRY(o_ostream.flush());

fapi_try_exit:
    FAPI_DBG("End");
    return fapi2::current_err;
}
