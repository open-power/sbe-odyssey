/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/hwp/generic/perv/poz_perv_mod_chiplet_clocking.C $ */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2022                             */
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
/// @file  poz_perv_mod_chiplet_clocking.C
/// @brief Chiplet clocking related modules
//------------------------------------------------------------------------------
// *HWP HW Maintainer   : Sreekanth Reddy (skadapal@in.ibm.com)
// *HWP FW Maintainer   : Raja Das (rajadas2@in.ibm.com)
//------------------------------------------------------------------------------

#include <poz_perv_mod_chiplet_clocking.H>
#include <poz_perv_mod_chiplet_clocking_regs.H>

using namespace fapi2;

enum POZ_PERV_MOD_CHIPLET_CLOCKING_Private_Constants
{
    NS_DELAY = 100000,      // unit in nano seconds
    DELAY_10us = 10000,      // unit in nano seconds
    SIM_CYCLE_DELAY = 1000, // unit in cycles
    CPLT_ALIGN_CHECK_POLL_COUNT = 10, // count to wait for chiplet aligned
    CPLT_OPCG_DONE_DC_POLL_COUNT = 10,    // count to wait for chiplet opcg done
    OPCG_DONE_SCAN0_POLL_COUNT = 200, // Scan0 Poll count
    OPCG_DONE_SCAN0_HW_NS_DELAY = 16000, // unit is nano seconds [min : 8k cycles x 4 = 8000/2 x 4 = 16000 x 10(-9) = 16 us
    //                       max : 8k cycles  =  (8000/25) x 10 (-6) = 320 us]
    OPCG_DONE_SCAN0_SIM_CYCLE_DELAY = 800000, // unit is cycles, to match the poll count change ( 10000 * 8 )
    OPCG_DONE_ARRAYINIT_HW_NS_DELAY = 200000, // unit is nano seconds [min : 400k/2 = 200k ns = 200 us
    //                       max : 200k /25 = 8000 us = 8 ms]
    OPCG_DONE_ARRAYINIT_POLL_COUNT = 400, // Arrayinit Poll count
    OPCG_DONE_ARRAYINIT_SIM_CYCLE_DELAY = 1120000, // unit is cycles,to match the poll count change ( 280000 * 4 )
};

ReturnCode poll_opcg_done(
    const Target < TARGET_TYPE_PERV | TARGET_TYPE_MULTICAST, MULTICAST_AND > & i_target,
    uint32_t i_hw_delay,
    uint32_t i_sim_delay,
    uint32_t i_poll_count,
    bool i_poll_abist_done)
{
    CPLT_STAT0_t CPLT_STAT0;

    while (i_poll_count != 0)
    {
        FAPI_INF("Getting CPLT_STAT0 register value");
        FAPI_TRY(CPLT_STAT0.getScom(i_target));

        if (CPLT_STAT0.get_CC_CTRL_OPCG_DONE_DC() || (i_poll_abist_done && CPLT_STAT0.get_ABIST_DONE_DC()))
        {
            break;
        }

        FAPI_TRY(fapi2::delay(i_hw_delay, i_sim_delay));
        --i_poll_count;
    }

    FAPI_DBG("Loop Count :%d", i_poll_count);

    FAPI_ASSERT(i_poll_count > 0,
                fapi2::POZ_OPCG_DONE_NOT_SET_ERR()
                .set_PERV_CPLT_STAT0(CPLT_STAT0)
                .set_POLL_COUNT(i_poll_count)
                .set_HW_DELAY(i_hw_delay)
                .set_PROC_TARGET(i_target),
                "ERROR:OPCG DONE BIT NOT SET");

fapi_try_exit:
    return current_err;
}

ReturnCode mod_abist_setup(
    const Target < TARGET_TYPE_PERV | TARGET_TYPE_MULTICAST > & i_target,
    uint16_t i_regions,
    uint64_t i_runn_cycles,
    uint64_t i_abist_start_at,
    uint64_t i_abist_start_stagger,
    const uint16_t* i_chiplets_regions,
    const bool i_skip_first_clock,
    const bool i_skip_last_clock)
{
    CPLT_CTRL0_t CPLT_CTRL0;
    BIST_t BIST;
    CLK_REGION_t CLK_REGION;
    OPCG_REG0_t OPCG_REG0;
    OPCG_REG1_t OPCG_REG1;

    // Trigger infinite mode if loop count is greater than max possible value
    const bool l_opcg_infinite_mode = i_runn_cycles > 0x7FFFFFFFFFF;
    uint64_t l_idle_count = i_abist_start_at;
    auto l_chiplets_uc = i_target.getChildren<TARGET_TYPE_PERV>();
    uint16_t l_chiplet_regions;

    FAPI_INF("Entering ...");
    FAPI_INF("Switch dual-clocked arrays to ABIST clock domain.");
    CPLT_CTRL0.flush<0>();
    CPLT_CTRL0.set_CTRL_CC_ABSTCLK_MUXSEL_DC(1);
    FAPI_TRY(CPLT_CTRL0.putScom_SET(i_target));

    FAPI_INF("Set up BISTed regions.");
    BIST = 0;
    BIST.set_TC_SRAM_ABIST_MODE_DC(1);
    BIST.insertFromRight<BIST_REGION_PERV, 16>(i_regions);
    FAPI_DBG("BIST buffer value (i_regions) : %#018lX", BIST);
    FAPI_TRY(BIST.putScom(i_target));

    FAPI_INF("Set up clocking.");
    CLK_REGION = 0;
    CLK_REGION.set_SEL_THOLD_SL(1);
    CLK_REGION.set_SEL_THOLD_NSL(1);
    CLK_REGION.set_SEL_THOLD_ARY(1);
    CLK_REGION.set_SUPPRESS_FIRST_EVEN_CLK(i_skip_first_clock);
    CLK_REGION.set_SUPPRESS_LAST_ODD_CLK(i_skip_last_clock);
    CLK_REGION.insertFromRight<CLK_REGION_CLOCK_REGION_PERV, 16>(i_regions);
    FAPI_DBG("CLK_REGION buffer value (i_regions) : %#018lX", CLK_REGION);
    FAPI_TRY(CLK_REGION.putScom(i_target));

    // If we know there are custom regions by chiplet, unicast them
    if (i_chiplets_regions != NULL)
    {
        for (auto& targ : l_chiplets_uc)
        {
            l_chiplet_regions = i_chiplets_regions[targ.getChipletNumber()];

            // Any regions mask of zero will opt to the default one
            if (l_chiplet_regions)
            {
                BIST.insertFromRight<BIST_REGION_PERV, 16>(l_chiplet_regions);
                FAPI_DBG("Custom BIST buffer value for chiplet %d : %#018lX",
                         targ.getChipletNumber(), BIST);
                FAPI_TRY(BIST.putScom(targ));

                CLK_REGION.insertFromRight<CLK_REGION_CLOCK_REGION_PERV, 16>(l_chiplet_regions);
                FAPI_DBG("Custom CLK_REGION buffer value for chiplet %d : %#018lX",
                         targ.getChipletNumber(), CLK_REGION);
                FAPI_TRY(CLK_REGION.putScom(targ));
            }
        }
    }

    FAPI_INF("Configure idle count in OPCG_REG1.");
    OPCG_REG1 = 0;

    if (l_opcg_infinite_mode)
    {
        FAPI_INF("Loop count value triggers OPCG infinite mode.");
        OPCG_REG1.set_INFINITE_MODE(1);
    }

    if (i_abist_start_stagger)
    {
        for (auto& targ : l_chiplets_uc)
        {
            OPCG_REG1.insertFromRight<0, 36>(l_idle_count);
            FAPI_TRY(OPCG_REG1.putScom(targ));
            l_idle_count += i_abist_start_stagger;
        }
    }
    else
    {
        OPCG_REG1.insertFromRight<0, 36>(l_idle_count);
        FAPI_TRY(OPCG_REG1.putScom(i_target));
    }

    FAPI_INF("Configure loop count and prep OPCG.");
    OPCG_REG0 = 0;
    OPCG_REG0.set_RUNN_MODE(1);
    OPCG_REG0.set_OPCG_STARTS_BIST(1);

    if (!l_opcg_infinite_mode)
    {
        OPCG_REG0.set_LOOP_COUNT(i_runn_cycles);
    }

    FAPI_TRY(OPCG_REG0.putScom(i_target));

fapi_try_exit:
    FAPI_INF("Exiting ...");
    return current_err;
}


ReturnCode mod_opcg_go(
    const Target < TARGET_TYPE_PERV | TARGET_TYPE_MULTICAST, MULTICAST_AND > &i_target)
{
    OPCG_REG0_t OPCG_REG0;
    FAPI_TRY(OPCG_REG0.getScom(i_target));
    OPCG_REG0.set_OPCG_GO(1);
    FAPI_TRY(OPCG_REG0.putScom(i_target));

fapi_try_exit:
    FAPI_INF("Exiting ...");
    return current_err;
}

ReturnCode mod_abist_start(
    const Target < TARGET_TYPE_PERV | TARGET_TYPE_MULTICAST > & i_target,
    uint16_t i_regions,
    uint64_t i_runn_cycles,
    uint64_t i_abist_start_at,
    uint64_t i_abist_start_stagger)
{
    FAPI_TRY(mod_abist_setup(i_target,
                             i_regions,
                             i_runn_cycles,
                             i_abist_start_at,
                             i_abist_start_stagger));

    FAPI_TRY(mod_opcg_go(i_target));

fapi_try_exit:
    FAPI_INF("Exiting ...");
    return current_err;
}


ReturnCode mod_abist_poll(
    const Target < TARGET_TYPE_PERV | TARGET_TYPE_MULTICAST, MULTICAST_AND > &i_target,
    bool i_poll_abist_done)
{
    CPLT_STAT0_t CPLT_STAT0;
    FAPI_INF("Entering ...");
    FAPI_DBG("Poll OPCG done bit to check for run-N completeness.");
    FAPI_TRY(poll_opcg_done(i_target, OPCG_DONE_ARRAYINIT_HW_NS_DELAY, OPCG_DONE_ARRAYINIT_SIM_CYCLE_DELAY,
                            OPCG_DONE_ARRAYINIT_POLL_COUNT, i_poll_abist_done));

    FAPI_TRY(CPLT_STAT0.getScom(i_target));
    FAPI_DBG("Checking sram abist done.");
    FAPI_ASSERT(CPLT_STAT0.get_ABIST_DONE_DC() == 1,
                fapi2::POZ_SRAM_ABIST_DONE_BIT_ERR()
                .set_PERV_CPLT_STAT0(CPLT_STAT0)
                .set_SELECT_SRAM(true)
                .set_PROC_TARGET(i_target),
                "ERROR: SRAM_ABIST_DONE_BIT_NOT_SET");

fapi_try_exit:
    FAPI_INF("Exiting ...");
    return current_err;
}


ReturnCode mod_abist_cleanup(
    const Target < TARGET_TYPE_PERV | TARGET_TYPE_MULTICAST > & i_target,
    bool i_clear_sram_abist_mode)
{
    CPLT_CTRL0_t CPLT_CTRL0;
    BIST_t BIST;
    CLK_REGION_t CLK_REGION;
    OPCG_REG0_t OPCG_REG0;

    FAPI_INF("Entering ...");
    FAPI_INF("Clear OPCG_REG0.");
    OPCG_REG0 = 0;
    FAPI_TRY(OPCG_REG0.putScom(i_target));
    FAPI_INF("Clear CLK_REGION register.");
    CLK_REGION = 0;
    FAPI_TRY(CLK_REGION.putScom(i_target));

    FAPI_INF("Clear CPLT_CTRL0 register.");
    CPLT_CTRL0.flush<0>();
    CPLT_CTRL0.set_CTRL_CC_ABSTCLK_MUXSEL_DC(1);
    FAPI_TRY(CPLT_CTRL0.putScom_CLEAR(i_target));

    FAPI_INF("Clear BIST register.");
    BIST = 0;
    BIST.set_TC_SRAM_ABIST_MODE_DC(!i_clear_sram_abist_mode);
    FAPI_TRY(BIST.putScom(i_target));

fapi_try_exit:
    FAPI_INF("Exiting ...");
    return current_err;
}

static ReturnCode check_clock_status(
    const Target < TARGET_TYPE_PERV | TARGET_TYPE_MULTICAST, MULTICAST_OR > & i_target,
    uint16_t i_clock_regions,
    uint16_t i_clock_type,
    bool i_start_stop)
{
    CLOCK_STAT_SL_t CLOCK_STAT_SL;
    CLOCK_STAT_NSL_t CLOCK_STAT_NSL;
    CLOCK_STAT_ARY_t CLOCK_STAT_ARY;

    fapi2::buffer<uint64_t> clock_stat_getscom_value;
    fapi2::buffer<uint64_t> clock_stat_expect;
    fapi2::buffer<uint64_t> clock_regions64;
    uint32_t addr = 0;

    fapi2::Target < fapi2::TARGET_TYPE_PERV | fapi2::TARGET_TYPE_MULTICAST,
          fapi2::MULTICAST_AND > l_mcast_and_target = i_target;
    clock_regions64.flush<0>().insertFromRight<4, 16>(i_clock_regions);

    if (i_clock_type == CLOCK_TYPE_SL)
    {
        addr = CLOCK_STAT_SL.addr;
        FAPI_INF("Reading CLOCK_STAT_SL register");
    }
    else if (i_clock_type == CLOCK_TYPE_NSL)
    {
        addr = CLOCK_STAT_NSL.addr;
        FAPI_INF("Reading CLOCK_STAT_NSL register");
    }
    else if (i_clock_type == CLOCK_TYPE_ARY)
    {
        addr = CLOCK_STAT_ARY.addr;
        FAPI_INF("Reading CLOCK_STAT_ARY register");
    }
    else
    {
        FAPI_ASSERT(false,
                    fapi2::POZ_INVALID_CLOCK_TYPE()
                    .set_CLOCK_TYPE_VALUE(i_clock_type)
                    .set_PROC_TARGET(i_target),
                    "ERROR: Invalid clock_type passed to check_clock_status function.");
    }

    FAPI_DBG("CLOCK_STAT SL/NSL/ARY register addr : %#018lX", addr);

    if (i_start_stop)
    {
        FAPI_TRY(fapi2::getScom(i_target, addr, clock_stat_getscom_value));
        clock_stat_expect = 0;
    }
    else
    {
        FAPI_TRY(fapi2::getScom(l_mcast_and_target, addr, clock_stat_getscom_value));
        clock_stat_expect = clock_regions64;
    }

    FAPI_DBG("CLOCK_STAT expect : 0x%08X%08X",
             (clock_stat_expect >> 32) & 0xFFFFFFFF,
             clock_stat_expect & 0xFFFFFFFF);
    FAPI_DBG("CLOCK_STAT getscom value : 0x%08X%08X",
             (clock_stat_getscom_value >> 32) & 0xFFFFFFFF,
             clock_stat_getscom_value & 0xFFFFFFFF);

    FAPI_ASSERT(((clock_regions64 & clock_stat_getscom_value) == clock_stat_expect),
                fapi2::POZ_THOLD_ERR()
                .set_CLOCK_CMD(i_start_stop)
                .set_CLOCK_TYPE(i_clock_type)
                .set_REGIONS(clock_regions64)
                .set_READ_CLK(clock_stat_getscom_value)
                .set_PROC_TARGET(i_target),
                "ERROR: CLOCK STATUS NOT MATCHING WITH EXPECTED REGIONS VALUE");

fapi_try_exit:
    return current_err;
}

ReturnCode mod_scan0(
    const Target < TARGET_TYPE_PERV | TARGET_TYPE_MULTICAST > & i_target,
    uint16_t i_clock_regions,
    uint16_t i_scan_types)
{
    CLK_REGION_t CLK_REGION;
    OPCG_REG0_t OPCG_REG0;
    SCAN_REGION_TYPE_t SCAN_REGION_TYPE;

    FAPI_INF("Entering ...");
    FAPI_INF("Set up clock regions for NSL fill.")
    CLK_REGION = 0;
    CLK_REGION.insertFromRight<CLK_REGION_CLOCK_REGION_PERV, 16>(i_clock_regions)
    .setBit<CLK_REGION_SEL_THOLD_NSL>()
    .setBit<CLK_REGION_SEL_THOLD_ARY>();
    FAPI_TRY(CLK_REGION.putScom(i_target));

    FAPI_INF("Set up scan regions for scan0.")
    SCAN_REGION_TYPE = 0;
    SCAN_REGION_TYPE.insertFromRight<SCAN_REGION_TYPE_SCAN_REGION_PERV, 16>(i_clock_regions)
    .insertFromRight<SCAN_REGION_TYPE_SCAN_TYPE_FUNC, 12>(i_scan_types);
    FAPI_TRY(SCAN_REGION_TYPE.putScom(i_target));

    FAPI_INF("Trigger scan0")
    OPCG_REG0 = 0;
    OPCG_REG0.set_RUN_SCAN0(1);
    FAPI_TRY(OPCG_REG0.putScom(i_target));

    FAPI_INF("Wait for scan0 to complete. Polling for OPCG_DONE.")
    FAPI_TRY(poll_opcg_done(i_target, OPCG_DONE_SCAN0_HW_NS_DELAY, OPCG_DONE_SCAN0_SIM_CYCLE_DELAY,
                            OPCG_DONE_SCAN0_POLL_COUNT));

    FAPI_INF("Clean up scan0.")
    CLK_REGION = 0;
    FAPI_TRY(CLK_REGION.putScom(i_target));
    SCAN_REGION_TYPE = 0;
    FAPI_TRY(SCAN_REGION_TYPE.putScom(i_target));

fapi_try_exit:
    FAPI_INF("Exiting ...");
    return current_err;
}

ReturnCode mod_start_stop_clocks(
    const Target < TARGET_TYPE_PERV | TARGET_TYPE_MULTICAST, MULTICAST_OR > & i_target,
    uint16_t i_clock_regions,
    uint16_t i_clock_types,
    bool i_start_not_stop)
{
    CLK_REGION_t CLK_REGION;
    SCAN_REGION_TYPE_t SCAN_REGION_TYPE;
    CPLT_CTRL1_t CPLT_CTRL1;
    CPLT_CTRL1 = 0;
    CPLT_CTRL1.insertFromRight<CPLT_CTRL1_TC_REGION0_FENCE_DC, 16>(i_clock_regions);

    FAPI_INF("Entering ...");
    FAPI_INF("Clear SCAN_REGION_TYPE register");
    SCAN_REGION_TYPE = 0;
    FAPI_TRY(SCAN_REGION_TYPE.putScom(i_target));

    if(i_start_not_stop)
    {
        FAPI_INF("Drop fences before starting clocks.");
        FAPI_TRY(CPLT_CTRL1.putScom_CLEAR(i_target));
    }

    FAPI_INF("Issue clock start/stop command with required clock_regions and clock_types");
    CLK_REGION = 0;
    CLK_REGION.set_CLOCK_CMD(i_start_not_stop ? 1 : 2);
    CLK_REGION.insertFromRight<CLK_REGION_CLOCK_REGION_PERV, 16>(i_clock_regions)
    .insertFromRight<CLK_REGION_SEL_THOLD_SL, 4>(i_clock_types);
    FAPI_DBG("CLK_REGION buffer value : %#018lX", CLK_REGION);
    FAPI_TRY(CLK_REGION.putScom(i_target));

    FAPI_INF("Wait for clock start/stop command to be done. Polling for OPCG_DONE");
    FAPI_TRY(poll_opcg_done(i_target, NS_DELAY, SIM_CYCLE_DELAY, CPLT_OPCG_DONE_DC_POLL_COUNT));

    for(auto check_type :
        (uint32_t[])
{
    CLOCK_TYPE_SL, CLOCK_TYPE_NSL, CLOCK_TYPE_ARY
})
    {
        FAPI_TRY(check_clock_status(i_target, i_clock_regions, i_clock_types & check_type, i_start_not_stop));
    }

    if( !i_start_not_stop )
    {
        FAPI_INF("Raise fences after clocks are stopped.")
        FAPI_TRY(CPLT_CTRL1.putScom_SET(i_target));
    }

fapi_try_exit:
    FAPI_INF("Exiting ...");
    return current_err;
}

ReturnCode mod_align_regions(
    const Target < TARGET_TYPE_PERV | TARGET_TYPE_MULTICAST > &i_target,
    uint16_t i_clock_regions)
{
    CPLT_CTRL0_t CPLT_CTRL0;
    CPLT_CTRL4_t CPLT_CTRL4;
    SYNC_CONFIG_t SYNC_CONFIG;
    CPLT_STAT0_t CPLT_STAT0;
    Target < TARGET_TYPE_PERV | TARGET_TYPE_MULTICAST, MULTICAST_COMPARE > l_mcast_cmp_target = i_target;
    Target < TARGET_TYPE_PERV | TARGET_TYPE_MULTICAST, MULTICAST_AND > l_mcast_and_target = i_target;
    int l_timeout = 0;

    FAPI_INF("Entering ...");

    FAPI_INF("Write region flush mode inhibit value in CPLT_CTRL4 reg.");
    CPLT_CTRL4 = 0;
    CPLT_CTRL4.insertFromRight<CPLT_CTRL4_0_FLUSHMODE_INH, 16>(i_clock_regions);
    FAPI_TRY(CPLT_CTRL4.putScom_SET(i_target));

    FAPI_INF("Enable alignment");
    CPLT_CTRL0 = 0;
    CPLT_CTRL0.set_CTRL_CC_FORCE_ALIGN(1);
    FAPI_TRY(CPLT_CTRL0.putScom_SET(i_target));

    FAPI_INF("Clear 'chiplet is aligned' indication");

    FAPI_TRY(SYNC_CONFIG.getScom(l_mcast_cmp_target));
    SYNC_CONFIG.set_CLEAR_CHIPLET_IS_ALIGNED(1);
    FAPI_TRY(SYNC_CONFIG.putScom(l_mcast_cmp_target));

    SYNC_CONFIG.set_CLEAR_CHIPLET_IS_ALIGNED(0);
    FAPI_TRY(SYNC_CONFIG.putScom(l_mcast_cmp_target));

    FAPI_INF("Wait for chiplets to be aligned");
    FAPI_DBG("Poll OPCG 'CHIPLET_IS_ALIGNED_DC' bit to check for completeness");

    l_timeout = CPLT_ALIGN_CHECK_POLL_COUNT;

    while (l_timeout != 0)
    {
        FAPI_INF("Getting CPLT_STAT0 register value");
        FAPI_TRY(CPLT_STAT0.getScom(l_mcast_and_target));

        if (CPLT_STAT0.get_CC_CTRL_CHIPLET_IS_ALIGNED_DC() == 1)
        {
            break;
        }

        FAPI_TRY(fapi2::delay(NS_DELAY, SIM_CYCLE_DELAY));
        --l_timeout;
    }

    FAPI_DBG("Loop Count :%d", l_timeout);

    FAPI_ASSERT(l_timeout > 0,
                fapi2::POZ_CPLT_NOT_ALIGNED_ERR()
                .set_PERV_CPLT_STAT0(CPLT_STAT0)
                .set_LOOP_COUNT(l_timeout)
                .set_HW_DELAY(NS_DELAY)
                .set_PROC_TARGET(i_target),
                "ERROR : CHIPLET NOT ALIGNED");

    FAPI_INF("Disable alignment");
    CPLT_CTRL0 = 0;
    CPLT_CTRL0.set_CTRL_CC_FORCE_ALIGN(1);
    FAPI_TRY(CPLT_CTRL0.putScom_CLEAR(i_target));

    FAPI_TRY(fapi2::delay(DELAY_10us, SIM_CYCLE_DELAY));

    CPLT_CTRL4 = 0;
    FAPI_TRY(CPLT_CTRL4.putScom(i_target));

fapi_try_exit:
    FAPI_INF("Exiting ...");
    return current_err;
}
