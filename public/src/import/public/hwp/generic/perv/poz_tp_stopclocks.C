/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/hwp/generic/perv/poz_tp_stopclocks.C $ */
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
/// @file  poz_tp_stopclocks.C
/// @brief Stop clocks for TP chiplet
//------------------------------------------------------------------------------
// *HWP HW Maintainer   : Sreekanth Reddy (skadapal@in.ibm.com)
// *HWP FW Maintainer   : Raja Das (rajadas2@in.ibm.com)
// *HWP Consumed by     : FSP:HB
//------------------------------------------------------------------------------

#include <poz_tp_stopclocks.H>
#include <poz_perv_common_params.H>
#include <poz_perv_mod_chiplet_clocking.H>
#include <poz_perv_mod_misc.H>
#include <poz_perv_utils.H>
#include <poz_scom_perv.H>
#include <multicast_defs.H>

using namespace fapi2;
using namespace scomt::poz;

SCOMT_PERV_USE_CPLT_CTRL0;
SCOMT_PERV_USE_NET_CTRL0;
SCOMT_PERV_USE_FSXCOMP_FSXLOG_ROOT_CTRL0;
SCOMT_PERV_USE_FSXCOMP_FSXLOG_PERV_CTRL0;

enum POZ_TP_STOPCLOCKS_Private_Constants
{
};

fapi2::ReturnCode poz_tp_stopclocks(
    const fapi2::Target<fapi2::TARGET_TYPE_ANY_POZ_CHIP>& i_target,
    const StopClocksFlags i_stopclocks_flags,
    bool i_pcb_accessible)
{
    fapi2::ReturnCode l_rc;
    NET_CTRL0_t NET_CTRL0;
    FSXCOMP_FSXLOG_PERV_CTRL0_t PERV_CTRL0;
    auto l_chiplets_mc_NO_TP   = i_target.getMulticast<TARGET_TYPE_PERV>(MCGROUP_GOOD_NO_TP);

    FAPI_INF("Entering ...");

    FAPI_INF("Drop TP chiplet fence");

    if(i_pcb_accessible)
    {
        NET_CTRL0 = 0;
        NET_CTRL0.set_PERV2CHIPLET_CHIPLET_FENCE(1);
        FAPI_TRY(NET_CTRL0.putScom_SET(l_chiplets_mc_NO_TP));
    }

    PERV_CTRL0 = 0;
    PERV_CTRL0.setBit<17>(); // bit 17: PERV_CHIPLET_FENCE required for Odyssey
    FAPI_TRY(PERV_CTRL0.putScom_SET(i_target));

    if(i_stopclocks_flags & STOP_REGION_TP)
    {
        l_rc = poz_tp_stopclocks_all_regions_except_sbe(i_target);
        FAPI_ASSERT_NOEXIT(l_rc == fapi2::FAPI2_RC_SUCCESS,
                           fapi2::TP_STOPCLOCKS_ERR()
                           .set_PROC_TARGET(i_target),
                           "poz_tp_stopclocks returned error when stopping clocks for all regions except SBE");
    }

    if(i_stopclocks_flags & STOP_REGION_SBE)
    {
        l_rc = poz_sbe_region_stopclocks(i_target);
        FAPI_ASSERT_NOEXIT(l_rc == fapi2::FAPI2_RC_SUCCESS,
                           fapi2::SBE_STOPCLOCKS_ERR()
                           .set_PROC_TARGET(i_target),
                           "poz_sbe_region_stopclocks returned error ");
    }

fapi_try_exit:
    FAPI_INF("Exiting ...");
    return fapi2::current_err;
}

fapi2::ReturnCode poz_tp_stopclocks_all_regions_except_sbe(
    const fapi2::Target<fapi2::TARGET_TYPE_ANY_POZ_CHIP>& i_target)
{
    CPLT_CTRL0_t CPLT_CTRL0;
    fapi2::Target<fapi2::TARGET_TYPE_PERV>  l_tpchiplet = get_tp_chiplet_target(i_target);

    FAPI_INF("Entering ...");

    FAPI_DBG("Switch mux to PIB2PCB path");
    FAPI_TRY(mod_switch_pcbmux(i_target, mux::PIB2PCB));

    FAPI_DBG("Call module start stop clocks");
    FAPI_TRY(mod_start_stop_clocks(l_tpchiplet, REGION_ALL_BUT_PERV_SBE, CLOCK_TYPE_ALL, false));

    FAPI_DBG("Set FLUSHMODE_INH, ABISTCLK_MUXSEL & SYNCCLK_MUXSEL");
    CPLT_CTRL0 = 0;
    CPLT_CTRL0.set_FLUSHMODE_INH(1);
    CPLT_CTRL0.set_ABSTCLK_MUXSEL(1);
    CPLT_CTRL0.set_SYNCCLK_MUXSEL(1);
    FAPI_TRY(CPLT_CTRL0.putScom_SET(l_tpchiplet));

fapi_try_exit:
    FAPI_INF("Exiting ...");
    return fapi2::current_err;
}

fapi2::ReturnCode poz_sbe_region_stopclocks(
    const fapi2::Target<fapi2::TARGET_TYPE_ANY_POZ_CHIP>& i_target)
{
    FSXCOMP_FSXLOG_ROOT_CTRL0_t ROOT_CTRL0;

    FAPI_INF("Entering ...");

    if (fapi2::is_platform<fapi2::PLAT_SBE>())
    {
        FAPI_ERR("Calling TP stopclocks for SBE regions in SBE mode is INVALID\n\t --> Skipping TP Stopclocks for SBE regions..! <--");
        fapi2::current_err = fapi2::FAPI2_RC_INVALID_PARAMETER;
        goto fapi_try_exit;
    }
    else
    {
        FAPI_DBG("Switch mux to FSI2PCB path");
        FAPI_TRY(mod_switch_pcbmux_cfam(i_target, mux::FSI2PCB));

        //Using CBS interface to stop clock
        FAPI_DBG("Call module clock start stop for SBE only");
        FAPI_TRY(poz_common_sbe_stopclocks(i_target));

        // RC0 bit0 : cfam_protection0,  bit8 : cfam_protection1, bit9 : cfam_protection2
        // Not raising ROOT_CTRL0_CFAM_PROTECTION_1_DC because we need the SHIFT <- TPvitl path for toad mode
        // Not raising ROOT_CTRL0_CFAM_PROTECTION_2_DC because we need FSI2PIB <- PERV EPS connectivity
        FAPI_DBG("Assert CFAM fences");
        ROOT_CTRL0 = 0;
        ROOT_CTRL0.setBit<8, 7>();
        ROOT_CTRL0.set_CFAM_PROTECTION_1(0);
        ROOT_CTRL0.set_CFAM_PROTECTION_2(0);
        FAPI_TRY(ROOT_CTRL0.putCfam_SET(i_target));

        FAPI_DBG("Assert cfam_protection_0 and raise OOB Mux");
        ROOT_CTRL0 = 0;
        ROOT_CTRL0.set_CFAM_PROTECTION_0(1);
        ROOT_CTRL0.set_OOB_MUX(1);
        FAPI_TRY(ROOT_CTRL0.putCfam_SET(i_target));
    }

fapi_try_exit:
    FAPI_INF("Exiting ...");
    return fapi2::current_err;
}
