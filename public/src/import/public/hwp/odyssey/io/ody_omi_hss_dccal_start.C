/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/hwp/odyssey/io/ody_omi_hss_dccal_start.C $ */
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
/// @file ody_omi_hss_dccal_start.C
/// @brief Odyssey DCCAL start HWP
///
/// *HWP HW Maintainer : Chris Steffen (cwsteffen@us.ibm.com)
/// *HWP FW Maintainer :
/// *HWP Consumed by: HB, Cronus, SBE
///------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------
#include <ody_scom_omi.H>
#include <ody_omi_hss_dccal_start.H>
#include <ody_io_ppe_common.H>

//------------------------------------------------------------------------------
// Function definitions
//------------------------------------------------------------------------------
SCOMT_OMI_USE_OMI0_RX_GRP0_CTL_REGS_MODE15_PG
SCOMT_OMI_USE_OMI0_RX_GRP0_CTL_REGS_MODE16_PG
SCOMT_OMI_USE_OMI0_RX_GRP0_CTL_REGS_MODE17_PG
SCOMT_OMI_USE_OMI0_RX_GRP0_CTL_REGS_MODE20_PG
SCOMT_OMI_USE_OMI0_RX_GRP0_CTL_REGS_MODE4_PG
SCOMT_OMI_USE_OMI0_RX_GRP0_CTL_REGS_MODE9_PG
SCOMT_OMI_USE_OMI0_RX_GRP0_CTL_REGS_MODE5_PG
SCOMT_OMI_USE_OMI0_RX_GRP0_CTL_REGS_MODE6_PG
SCOMT_OMI_USE_OMI0_RX_GRP0_CTL_REGS_MODE8_PG

fapi2::ReturnCode ody_omi_sim_fast_mode(const fapi2::Target<fapi2::TARGET_TYPE_OCMB_CHIP>& i_target)
{
    FAPI_DBG("Start");
    using namespace scomt::omi;

    OMI0_RX_GRP0_CTL_REGS_MODE15_PG_t l_rx_ctl_mode15_pg;
    OMI0_RX_GRP0_CTL_REGS_MODE16_PG_t l_rx_ctl_mode16_pg;
    OMI0_RX_GRP0_CTL_REGS_MODE17_PG_t l_rx_ctl_mode17_pg;
    OMI0_RX_GRP0_CTL_REGS_MODE20_PG_t l_rx_ctl_mode20_pg;
    OMI0_RX_GRP0_CTL_REGS_MODE4_PG_t l_rx_ctl_mode4_pg;
    OMI0_RX_GRP0_CTL_REGS_MODE9_PG_t l_rx_ctl_mode9_pg;
    OMI0_RX_GRP0_CTL_REGS_MODE5_PG_t l_rx_ctl_mode5_pg;
    OMI0_RX_GRP0_CTL_REGS_MODE6_PG_t l_rx_ctl_mode6_pg;
    OMI0_RX_GRP0_CTL_REGS_MODE8_PG_t l_rx_ctl_mode8_pg;
    fapi2::buffer<uint64_t> l_data;

    l_rx_ctl_mode15_pg.set_0(1);
    l_rx_ctl_mode15_pg.set_1(1);
    l_rx_ctl_mode15_pg.set_2(1);
    l_rx_ctl_mode15_pg.set_3(2);
    FAPI_TRY(l_rx_ctl_mode15_pg.putScom(i_target));

    l_rx_ctl_mode16_pg.set_INC_DEC_AMT0(5);
    l_rx_ctl_mode16_pg.set_INC_DEC_AMT1(4);
    l_rx_ctl_mode16_pg.set_THRESH1(1);
    l_rx_ctl_mode16_pg.set_THRESH2(1);
    FAPI_TRY(l_rx_ctl_mode16_pg.putScom(i_target));

    l_rx_ctl_mode17_pg.set_INC_DEC_AMT2(4);
    l_rx_ctl_mode17_pg.set_INC_DEC_AMT3(3);
    l_rx_ctl_mode17_pg.set_THRESH3(1);
    l_rx_ctl_mode17_pg.set_THRESH4(5);
    FAPI_TRY(l_rx_ctl_mode17_pg.putScom(i_target));

    FAPI_TRY(l_rx_ctl_mode20_pg.getScom(i_target));
    l_rx_ctl_mode20_pg.set_AMP_HYST_START(3);
    l_rx_ctl_mode20_pg.set_LOFF_HYST_START(3);
    FAPI_TRY(l_rx_ctl_mode20_pg.putScom(i_target));

    l_rx_ctl_mode4_pg.set_INC0(1);
    l_rx_ctl_mode4_pg.set_DEC0(1);
    l_rx_ctl_mode4_pg.set_INC1(1);
    l_rx_ctl_mode4_pg.set_DEC1(1);
    FAPI_TRY(l_rx_ctl_mode4_pg.putScom(i_target));

    l_rx_ctl_mode9_pg.set_INC2(1);
    l_rx_ctl_mode9_pg.set_DEC2(1);
    l_rx_ctl_mode9_pg.set_INC3(1);
    l_rx_ctl_mode9_pg.set_DEC2(4);
    FAPI_TRY(l_rx_ctl_mode9_pg.putScom(i_target));

    l_rx_ctl_mode5_pg.set_INC_DEC_AMT0(5);
    l_rx_ctl_mode5_pg.set_INC_DEC_AMT1(4);
    l_rx_ctl_mode5_pg.set_THRESH1(1);
    l_rx_ctl_mode5_pg.set_THRESH2(1);
    FAPI_TRY(l_rx_ctl_mode5_pg.putScom(i_target));

    l_rx_ctl_mode6_pg.set_INC_DEC_AMT2(4);
    l_rx_ctl_mode6_pg.set_INC_DEC_AMT3(3);
    l_rx_ctl_mode6_pg.set_THRESH3(1);
    l_rx_ctl_mode6_pg.set_THRESH4(5);
    FAPI_TRY(l_rx_ctl_mode6_pg.putScom(i_target));

    FAPI_TRY(l_rx_ctl_mode8_pg.getScom(i_target));
    l_rx_ctl_mode8_pg.set_BO_TIME(1);
    FAPI_TRY(l_rx_ctl_mode8_pg.putScom(i_target));

    for (uint64_t l_lane = 0; l_lane < 8; l_lane++)
    {
        FAPI_TRY(getScomLane(i_target, OMI0_TX_LANE0_DD_BIT_REGS_MODE3_PL, l_lane, l_data));
        l_data.insertFromRight<OMI0_TX_LANE0_DD_BIT_REGS_MODE3_PL_DCC_CMP_HIGH_SEL, \
        OMI0_TX_LANE0_DD_BIT_REGS_MODE3_PL_DCC_CMP_HIGH_SEL_LEN>(0);
        l_data.insertFromRight<OMI0_TX_LANE0_DD_BIT_REGS_MODE3_PL_DCC_CMP_LOW_SEL, \
        OMI0_TX_LANE0_DD_BIT_REGS_MODE3_PL_DCC_CMP_LOW_SEL_LEN>(0);
        FAPI_TRY(putScomLane(i_target, OMI0_TX_LANE0_DD_BIT_REGS_MODE3_PL, l_lane, l_data));
    }

fapi_try_exit:
    FAPI_DBG("End");
    return fapi2::current_err;
}

fapi2::ReturnCode ody_omi_hss_dccal_start(const fapi2::Target<fapi2::TARGET_TYPE_OCMB_CHIP>& i_target)
{
    FAPI_DBG("Start");

    fapi2::buffer<uint64_t> l_data = 0;
    const fapi2::Target<fapi2::TARGET_TYPE_SYSTEM> l_sys;
    uint8_t l_sim = 0;

    io_ppe_regs<fapi2::TARGET_TYPE_OCMB_CHIP> l_ppe_regs(scomt::omi::PHY_PPE_WRAP0_ARB_CSAR,
            scomt::omi::PHY_PPE_WRAP0_ARB_CSDR,
            scomt::omi::PHY_PPE_WRAP0_XIXCR);

    ody_io::io_ppe_common<fapi2::TARGET_TYPE_OCMB_CHIP> l_ppe_common(&l_ppe_regs);

    const fapi2::buffer<uint64_t> l_rx_lanes = 0xF0000000; // 4 lanes
    const fapi2::buffer<uint64_t> l_tx_lanes = 0xFF000000; // 8 lanes
    const fapi2::buffer<uint64_t> l_num_threads = 1;

    FAPI_TRY(FAPI_ATTR_GET(fapi2::ATTR_IS_SIMULATION, l_sys, l_sim));

    if (l_sim)
    {
        FAPI_TRY(ody_omi_sim_fast_mode(i_target));
        FAPI_TRY(l_ppe_common.fast_mode(i_target, l_num_threads));
    }

    FAPI_TRY(l_ppe_common.dccal_start(i_target, l_num_threads, l_rx_lanes, l_tx_lanes, 1));
    FAPI_TRY(l_ppe_regs.flushCache(i_target));

fapi_try_exit:
    FAPI_DBG("End");
    return fapi2::current_err;
}
