/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/hwp/generic/perv/poz_common_stopclocks.C $ */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2022,2023                        */
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
//------------------------------------------------------------------------------
/// @file  poz_common_stopclocks.C
/// @brief Common module for stopclocks
//------------------------------------------------------------------------------
// *HWP HW Maintainer   : Sreekanth Reddy (skadapal@in.ibm.com)
// *HWP FW Maintainer   : Raja Das (rajadas2@in.ibm.com)
// *HWP Consumed by     : FSP:HB
//------------------------------------------------------------------------------

#include <poz_common_stopclocks.H>
#include <poz_perv_mod_chiplet_clocking.H>
#include <poz_perv_utils.H>
#include <poz_scom_perv.H>
#include <poz_perv_mod_misc.H>

using namespace fapi2;
using namespace scomt::poz;

SCOMT_PERV_USE_NET_CTRL0;
SCOMT_PERV_USE_CPLT_CTRL0;
SCOMT_PERV_USE_FSXCOMP_FSXLOG_CBS_STAT;
SCOMT_PERV_USE_FSXCOMP_FSXLOG_ROOT_CTRL0;
SCOMT_PERV_USE_FSXCOMP_FSXLOG_PERV_CTRL0;
SCOMT_PERV_USE_CLOCK_STAT_ARY;
SCOMT_PERV_USE_CLOCK_STAT_NSL;
SCOMT_PERV_USE_CLOCK_STAT_SL;

enum POZ_COMMON_STOPCLOCKS_Private_Constants
{
    CBS_POLL_COUNT = 20,
    CBS_POLL_HW_NS_DELAY = 16000,
    CBS_POLL_SIM_CYCLE_DELAY = 800000,
};

fapi2::ReturnCode poz_stopclocks_pre_check(
    const fapi2::Target < fapi2::TARGET_TYPE_ANY_POZ_CHIP >& i_target,
    bool& stop_chiplet_clocks_possible,
    bool& stop_tp_clocks_possible)
{
    FSXCOMP_FSXLOG_ROOT_CTRL0_t ROOT_CTRL0;
    FSXCOMP_FSXLOG_PERV_CTRL0_t PERV_CTRL0;
    CLOCK_STAT_SL_t CLOCK_STAT_SL;
    CLOCK_STAT_NSL_t CLOCK_STAT_NSL;
    CLOCK_STAT_ARY_t CLOCK_STAT_ARY;
    fapi2::Target<fapi2::TARGET_TYPE_PERV>  l_target_tp = get_tp_chiplet_target(i_target);

    bool pcb_is_bypassed      = false;
    bool pcb_clks_are_off     = false;

    bool tp_cplt_en           = false;
    bool tp_ep_rst            = true;
    bool tp_vitl_clk_off      = true;

    uint8_t perv_net_region_3_not_4;
    int perv_net_region;

    FAPI_INF("Entering ...");

    FAPI_TRY(FAPI_ATTR_GET(ATTR_CHIP_EC_FEATURE_PERV_NET_REGION_3_NOT_4, i_target, perv_net_region_3_not_4));
    perv_net_region = perv_net_region_3_not_4 ? 3 : 4;

    FAPI_DBG("Reading PERV_CTRL0 register to observe CHIPLET_EN, PCB_EP_RESET & VITL_CLKOFF");

    if (fapi2::is_platform<fapi2::PLAT_SBE>())
    {
        FAPI_TRY(PERV_CTRL0.getScom(i_target));
    }
    else
    {
        FAPI_TRY(PERV_CTRL0.getCfam(i_target));
    }

    tp_cplt_en         = PERV_CTRL0.get_CHIPLET_EN();
    tp_ep_rst          = PERV_CTRL0.get_PCB_EP_RESET();
    tp_vitl_clk_off    = PERV_CTRL0.get_VITL_CLKOFF();

    FAPI_DBG("Read PERV_CTRL0 register and observed CHIPLET_EN = %d, PCB_EP_RESET = %d, VITL_CLKOFF = %d", tp_cplt_en,
             tp_ep_rst, tp_vitl_clk_off);

    FAPI_ASSERT((!tp_vitl_clk_off),
                fapi2::TP_VITL_CLK_OFF_ERR()
                .set_PERV_CTRL0_READ_DATA(PERV_CTRL0)
                .set_PROC_TARGET(i_target),
                "Perv vital clocks are off, stopclocks cant go ahead");

    if(tp_cplt_en && !tp_ep_rst)
    {
        FAPI_DBG("Reading ROOT_CTRL0 register to check if the PCB network is being bypassed");

        if (fapi2::is_platform<fapi2::PLAT_SBE>())
        {
            FAPI_TRY(ROOT_CTRL0.getScom(i_target));
        }
        else
        {
            FAPI_TRY(ROOT_CTRL0.getCfam(i_target));
        }

        pcb_is_bypassed = ROOT_CTRL0.get_PIB2PCB()
                          || ROOT_CTRL0.get_FSI2PCB()
                          || ROOT_CTRL0.getBit(I2C2PCB); //I2C2PCB - bit 24

        if(pcb_is_bypassed)
        {
            FAPI_DBG("PCB is bypassed, so only the TP chiplet is accessible.");
        }
        else
        {
            FAPI_DBG("Reading clock status registers(SL/NSL/ARY) of TP chiplet to see if SBE,PIB and NET clocks are running. Bits 5, 6 & 8 should be zero.");

            FAPI_DBG("Reading CLOCK_STAT_SL");
            FAPI_TRY(CLOCK_STAT_SL.getScom(l_target_tp));

            FAPI_DBG("Reading CLOCK_STAT_NSL");
            FAPI_TRY(CLOCK_STAT_NSL.getScom(l_target_tp));

            FAPI_DBG("Reading CLOCK_STAT_ARY");
            FAPI_TRY(CLOCK_STAT_ARY.getScom(l_target_tp));

            CLOCK_STAT_SL |= CLOCK_STAT_NSL | CLOCK_STAT_ARY;

            if (CLOCK_STAT_SL.getBits<5, 2>() ||    // sbe, pib
                CLOCK_STAT_SL.getBit(4 + perv_net_region))
            {
                FAPI_ERR("At least one of the SBE/PIB/NET clocks NOT running, can't use the PCB fabric to access chiplets.");
                pcb_clks_are_off      = true;
            }
        }
    }
    else
    {
        FAPI_ERR("TP chiplet dont have favourable conditions to access Clock Controller registers.");
        pcb_clks_are_off        = true;
        stop_tp_clocks_possible = false;
    }

    FAPI_DBG("\n\t pcb_is_bypassed = %s \n\tpcb_clks_are_off = %s \t\n ", btos(pcb_is_bypassed), btos(pcb_clks_are_off));
    stop_chiplet_clocks_possible =  !pcb_clks_are_off && !pcb_is_bypassed;

fapi_try_exit:
    FAPI_INF("Exiting ...");
    return fapi2::current_err;
}

fapi2::ReturnCode poz_common_sbe_stopclocks(
    const fapi2::Target<fapi2::TARGET_TYPE_ANY_POZ_CHIP>& i_target)
{
    FSXCOMP_FSXLOG_ROOT_CTRL0_t ROOT_CTRL0;

    FAPI_INF("Entering ...");

    FAPI_DBG("Reading ROOT_CTRL0 register to check CFAM_PROTECTION_1 & CBS_REQ ");
    FAPI_TRY(ROOT_CTRL0.getCfam(i_target));

    if(ROOT_CTRL0.get_CFAM_PROTECTION_1())
    {
        FAPI_DBG("Clear FSI Fence1 to open CBS-CC interface");
        ROOT_CTRL0.set_CFAM_PROTECTION_1(0);
        FAPI_TRY(ROOT_CTRL0.putCfam(i_target));
    }

    if(ROOT_CTRL0.get_FSI_CC_CBS_REQ())
    {
        FAPI_DBG("CBS_REQ is high - calling poz_common_poll_cbs_cmd_complete function");
        FAPI_TRY(poz_common_poll_cbs_cmd_complete(i_target));
    }
    else
    {
        FAPI_DBG("CBS_REQ is Low - Hence CBS is idle");
    }

    FAPI_DBG("Set CBS_CMD to stop SBE clocks");
    FAPI_TRY(ROOT_CTRL0.getCfam(i_target));
    ROOT_CTRL0.set_FSI_CC_CBS_CMD(0x4);
    FAPI_TRY(ROOT_CTRL0.putCfam(i_target));

    FAPI_DBG("Set CBS_REQ to 1 to start CBS_CMD");
    ROOT_CTRL0.set_FSI_CC_CBS_REQ(1);
    FAPI_TRY(ROOT_CTRL0.putCfam(i_target));

    FAPI_DBG("Calling poz_common_poll_cbs_cmd_complete function");
    FAPI_TRY(poz_common_poll_cbs_cmd_complete(i_target));

fapi_try_exit:
    FAPI_INF("Exiting ...");
    return fapi2::current_err;
}

fapi2::ReturnCode poll_for_cbs_ack(
    const fapi2::Target<fapi2::TARGET_TYPE_ANY_POZ_CHIP>& i_target,
    bool poll_data)
{
    FSXCOMP_FSXLOG_CBS_STAT_t CBS_STAT;
    int l_timeout = 0;

    FAPI_INF("Entering ...");

    l_timeout = CBS_POLL_COUNT;

    while (l_timeout != 0)
    {
        FAPI_TRY(CBS_STAT.getCfam(i_target));

        if (CBS_STAT.get_TP_TPFSI_CBS_ACK() == poll_data)
        {
            break;
        }

        fapi2::delay(CBS_POLL_HW_NS_DELAY, CBS_POLL_SIM_CYCLE_DELAY);
        --l_timeout;
    }

    FAPI_ASSERT(l_timeout > 0, fapi2::CBS_ACK_NOT_SEEN_ERR()
                .set_PROC_TARGET(i_target)
                .set_EXPECTED_ACK_VALUE(poll_data)
                .set_CBS_STAT_READ_DATA(CBS_STAT)
                .set_TIMEOUT(CBS_POLL_COUNT)
                .set_DELAY(CBS_POLL_HW_NS_DELAY),
                "ERROR: CBS_ACK not reached expected value");

fapi_try_exit:
    FAPI_INF("Exiting ...");
    return fapi2::current_err;
}

fapi2::ReturnCode poz_common_poll_cbs_cmd_complete(
    const fapi2::Target<fapi2::TARGET_TYPE_ANY_POZ_CHIP>& i_target)
{
    FSXCOMP_FSXLOG_ROOT_CTRL0_t ROOT_CTRL0;

    FAPI_INF("Entering ...");

    FAPI_DBG("Wait for CBS_ACK to go to 1");
    FAPI_TRY(poll_for_cbs_ack(i_target, true));

    FAPI_DBG("Lower CBS_REQ");
    ROOT_CTRL0 = 0;
    ROOT_CTRL0.set_FSI_CC_CBS_REQ(1);
    FAPI_TRY(ROOT_CTRL0.putCfam_CLEAR(i_target));

    FAPI_DBG("Poll for CBS_ACK to go to 0");
    FAPI_TRY(poll_for_cbs_ack(i_target, false));

fapi_try_exit:
    FAPI_INF("Exiting ...");
    return fapi2::current_err;
}

fapi2::ReturnCode poz_common_stopclocks_chiplet_accessible(
    const fapi2::Target<fapi2::TARGET_TYPE_PERV>& i_target_chiplet)
{
    NET_CTRL0_t NET_CTRL0;

    FAPI_INF("Entering ...");
    FAPI_DBG("Reading NET_CTRL0 register to observe CHIPLET_ENABLE, PCB_EP_RESET, VITL_CLKOFF & FENCE_EN");
    FAPI_TRY(NET_CTRL0.getScom(i_target_chiplet));

    if ((NET_CTRL0.get_CHIPLET_EN())
        && !(NET_CTRL0.get_PCB_EP_RESET())
        && !(NET_CTRL0.get_VITL_CLKOFF())
        && !(NET_CTRL0.get_FENCE_EN()))
    {
        FAPI_DBG("Required bits of NET_CTRL0 are met for chiplet access.\n"
                 "CHIPLET_ENABLE=1, PCB_EP_RESET=0, VITL_CLKOFF=0 & FENCE_EN=0.");
        return fapi2::FAPI2_RC_SUCCESS;
    }
    else
    {
        FAPI_DBG("Minimum one of the required bits of NET_CTRL0 are not met for chiplet access.\n"
                 "Either CHIPLET_ENABLE!=1 or PCB_EP_RESET!=0 or VITL_CLKOFF!=0 or FENCE_EN!=0.");
        return fapi2::FAPI2_RC_FALSE;
    }

fapi_try_exit:
    FAPI_INF("Exiting ...");
    return fapi2::current_err;
}

fapi2::ReturnCode poz_vital_stopclocks(
    const fapi2::Target < fapi2::TARGET_TYPE_ANY_POZ_CHIP >& i_target)
{
    FSXCOMP_FSXLOG_ROOT_CTRL0_t ROOT_CTRL0;
    FSXCOMP_FSXLOG_PERV_CTRL0_t PERV_CTRL0;

    FAPI_INF("Entering ...");

    if (fapi2::is_platform<fapi2::PLAT_SBE>())
    {
        FAPI_ERR("WARNING :: VITAL clocks can't be stopped in SBE mode\n\t --> Skipping VITAL Stopclocks..! <--");
    }
    else
    {
        FAPI_INF("Stopping pervasive VITAL clocks (writing PERV_CTRL0 VITL_CLKOFF=1) ");
        PERV_CTRL0 = 0;
        PERV_CTRL0.set_VITL_CLKOFF(1);
        FAPI_TRY(PERV_CTRL0.putCfam_SET(i_target));

        FAPI_INF("Raising fences (writing ROOT_CTRL0 8:14 bits to raise FSI fences)");
        ROOT_CTRL0 = 0;
        ROOT_CTRL0.setBit<8, 7>();
        FAPI_TRY(ROOT_CTRL0.putCfam_SET(i_target));

        FAPI_DBG("Writing PERV_CTRL0 register to raise fences");
        PERV_CTRL0 = 0;
        PERV_CTRL0.setBit<17>(); // bit 17 : PERV_CHIPLET_FENCE
        PERV_CTRL0.set_PERV2FSI_CHIPLET_FENCE(1);
        FAPI_TRY(PERV_CTRL0.putCfam_SET(i_target));
    }

fapi_try_exit:
    FAPI_INF("Exiting ...");
    return fapi2::current_err;
}

fapi2::ReturnCode poz_chiplet_stopclocks(
    const fapi2::Target < fapi2::TARGET_TYPE_PERV | fapi2::TARGET_TYPE_MULTICAST > & i_target_chiplet)
{
    NET_CTRL0_t NET_CTRL0;
    CPLT_CTRL0_t CPLT_CTRL0;

    FAPI_INF("Entering ...");

    FAPI_DBG("Call mod_start_stop_clocks");
    FAPI_TRY(mod_start_stop_clocks(i_target_chiplet, REGION_ALL, CLOCK_TYPE_ALL, false));

    FAPI_DBG("Raise chiplet fence");
    NET_CTRL0 = 0;
    NET_CTRL0.set_FENCE_EN(1);
    FAPI_TRY(NET_CTRL0.putScom_SET(i_target_chiplet));

    FAPI_INF("Set FLUSHMODE_INH, ABISTCLK_MUXSEL & SYNCCLK_MUXSEL");
    CPLT_CTRL0 = 0;
    CPLT_CTRL0.set_FLUSHMODE_INH(1);
    CPLT_CTRL0.set_ABSTCLK_MUXSEL(1);
    CPLT_CTRL0.set_SYNCCLK_MUXSEL(1);
    FAPI_TRY(CPLT_CTRL0.putScom_SET(i_target_chiplet));

fapi_try_exit:
    FAPI_INF("Exiting ...");
    return fapi2::current_err;
}

fapi2::ReturnCode poz_stop_chiplet_clocks_with_multiacst(
    const fapi2::Target < fapi2::TARGET_TYPE_ANY_POZ_CHIP >& i_target, fapi2::TargetFilter i_target_filter)
{
    uint64_t l_chiplets_to_stop = 0;
    fapi2::ReturnCode l_rc;
    FAPI_INF("Entering ...");

    FAPI_INF("Check which all chiplets can be accessed");

    for (auto l_chiplet : i_target.getChildren<fapi2::TARGET_TYPE_PERV>
         (i_target_filter, fapi2::TARGET_STATE_FUNCTIONAL))
    {
        l_rc = poz_common_stopclocks_chiplet_accessible(l_chiplet);

        if(l_rc == fapi2::FAPI2_RC_SUCCESS)
        {
            l_chiplets_to_stop |= (uint64_t)1 << (63 - l_chiplet.getChipletNumber());
        }
    }

    FAPI_INF("Configure all accessible chiplets as part of multicast group 6");
    FAPI_TRY(mod_multicast_setup(i_target, MCGROUP_6, l_chiplets_to_stop, TARGET_STATE_FUNCTIONAL));
    {
        // Initializing l_stop_chiplets inside a new scope to prevent issues with FAPI_TRY
        auto l_stop_chiplets = i_target.getMulticast<fapi2::TARGET_TYPE_PERV>(fapi2::MCGROUP_6);
        // poz_chiplet_stopclocks will log errors if any
        FAPI_INF("Calling poz_chiplet_stopclocks...");
        FAPI_TRY(poz_chiplet_stopclocks(l_stop_chiplets));
    }

fapi_try_exit:
    FAPI_INF("Exiting ...");
    return fapi2::current_err;
}
