/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/hwp/generic/perv/poz_perv_mod_cbs.C $ */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2024                             */
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
/// @file  poz_perv_mod_cbs.C
/// @brief odule definitions for CBS
//------------------------------------------------------------------------------
// *HWP HW Maintainer   : Sreekanth Reddy (skadapal@in.ibm.com)
// *HWP FW Maintainer   : Raja Das (rajadas2@in.ibm.com)
//------------------------------------------------------------------------------

#include "poz_perv_mod_cbs.H"
#include "poz_scom_perv.H"

using namespace scomt::poz;

SCOMT_PERV_USE_FSXCOMP_FSXLOG_ROOT_CTRL0;
SCOMT_PERV_USE_FSXCOMP_FSXLOG_ROOT_CTRL1;
SCOMT_PERV_USE_FSXCOMP_FSXLOG_CBS_ENVSTAT;
SCOMT_PERV_USE_FSXCOMP_FSXLOG_ROOT_CTRL0_COPY;
SCOMT_PERV_USE_FSXCOMP_FSXLOG_ROOT_CTRL1_COPY;
SCOMT_PERV_USE_FSXCOMP_FSXLOG_CBS_CS;
SCOMT_PERV_USE_FSXCOMP_FSXLOG_SB_CS;
SCOMT_PERV_USE_TP_CFAM_FSI_W_SBE_FIFO_FSB_DOWNFIFO_RESET;
SCOMT_PERV_USE_TP_CFAM_FSI_W_FSI2PIB_STATUS;
SCOMT_PERV_USE_FSXCOMP_FSXLOG_SB_MSG;

using ROOT_CTRL0_t = FSXCOMP_FSXLOG_ROOT_CTRL0_t;
using ROOT_CTRL1_t = FSXCOMP_FSXLOG_ROOT_CTRL1_t;
using CBS_ENVSTAT_t = FSXCOMP_FSXLOG_CBS_ENVSTAT_t;
using ROOT_CTRL0_COPY_t = FSXCOMP_FSXLOG_ROOT_CTRL0_COPY_t;
using ROOT_CTRL1_COPY_t = FSXCOMP_FSXLOG_ROOT_CTRL1_COPY_t;
using CBS_CS_t = FSXCOMP_FSXLOG_CBS_CS_t;
using SB_CS_t = FSXCOMP_FSXLOG_SB_CS_t;
using SBE_FIFO_FSB_DOWNFIFO_RESET_t = TP_CFAM_FSI_W_SBE_FIFO_FSB_DOWNFIFO_RESET_t;
using FSI2PIB_STATUS_t = TP_CFAM_FSI_W_FSI2PIB_STATUS_t;
using SB_MSG_t = FSXCOMP_FSXLOG_SB_MSG_t;

static constexpr int CFAM_CBS_POLL_COUNT =
    200;         // Observed Number of times CBS read for CBS_INTERNAL_STATE_VECTOR
static constexpr int CBS_IDLE_VALUE = 0x002;            // Read the value of CBS_CS_INTERNAL_STATE_VECTOR
static constexpr int CBS_IDLE_HW_NS_DELAY =
    640000;     // unit is nano seconds [min : 64k x (1/100MHz) = 64k x 10(-8) = 640 us
//                       max : 64k x (1/50MHz) = 128k x 10(-8) = 1280 us]
static constexpr int CBS_IDLE_SIM_CYCLE_DELAY =
    750000; // unit is sim cycles,to match the poll count change ( 250000 * 30 )

using namespace fapi2;

ReturnCode mod_cbs_start_prep(
    const Target<TARGET_TYPE_ANY_POZ_CHIP>& i_target,
    const bool i_start_sbe,
    const bool i_scan0_clockstart)
{
    ROOT_CTRL0_t ROOT_CTRL0;
    ROOT_CTRL1_t ROOT_CTRL1;
    CBS_ENVSTAT_t CBS_ENVSTAT;
    ROOT_CTRL0_COPY_t ROOT_CTRL0_COPY;
    ROOT_CTRL1_COPY_t ROOT_CTRL1_COPY;
    CBS_CS_t CBS_CS;
    SBE_FIFO_FSB_DOWNFIFO_RESET_t FSB_DOWNFIFO_RESET;
    FSI2PIB_STATUS_t FSI2PIB_STATUS;
    SB_MSG_t SB_MSG;
    SB_CS_t SB_CS;

    FAPI_DBG("Drop CFAM protection 0 to ungate VDN_PRESENT");
    FAPI_TRY(ROOT_CTRL0.getCfam(i_target));
    ROOT_CTRL0.set_CFAM_PROTECTION_0(0);
    FAPI_TRY(ROOT_CTRL0.putCfam(i_target));
    // not using putCfam_CLEAR scope here since the same value needs to be written into COPY

    ROOT_CTRL0_COPY = ROOT_CTRL0;
    FAPI_TRY(ROOT_CTRL0_COPY.putCfam(i_target));

    FAPI_DBG("Read FSI2PIB_STATUS register and check whether VDN power is on or not(VDD_NEST_OBSERVE).");
    FAPI_TRY(FSI2PIB_STATUS.getCfam(i_target));
    FAPI_ASSERT(FSI2PIB_STATUS.get_VDD_NEST_OBSERVE(),
                fapi2::POZ_VDN_POWER_NOT_ON()
                .set_FSI2PIB_STATUS_READ(FSI2PIB_STATUS)
                .set_PROC_TARGET(i_target),
                "ERROR: VDN power is NOT on. i.e. FSI2PIB_STATUS register bit 16 is NOT set.");

    FAPI_DBG("Clear Selfboot Message Register, clear SBE start bit, reset SBE FIFO.");
    SB_MSG = 0;
    FAPI_TRY(SB_MSG.putCfam(i_target));

    FAPI_TRY(SB_CS.getCfam(i_target));
    SB_CS.set_START_RESTART_VECTOR0(0);
    SB_CS.set_START_RESTART_VECTOR1(0);
    FAPI_TRY(SB_CS.putCfam(i_target));

    FSB_DOWNFIFO_RESET = 0x80000000;
    FAPI_TRY(FSB_DOWNFIFO_RESET.putCfam(i_target));

    FAPI_DBG("Read CBS_ENVSTAT register to check the status of TEST_ENABLE C4 pin");
    FAPI_TRY(CBS_ENVSTAT.getCfam(i_target));

    if (CBS_ENVSTAT.get_CBS_ENVSTAT_C4_TEST_ENABLE())
    {
        FAPI_INF("Test mode, enable TP drivers/receivers for GSD scan out");
        ROOT_CTRL1 = 0;
        ROOT_CTRL1.set_TP_RI_DC_N(1);
        ROOT_CTRL1.set_TP_DI2_DC_N(1);
        FAPI_TRY(ROOT_CTRL1.putCfam_SET(i_target));

        // Don't forget the copy reg
        FAPI_TRY(ROOT_CTRL1_COPY.getCfam(i_target));
        ROOT_CTRL1_COPY.setBit(FSXCOMP_FSXLOG_ROOT_CTRL1_TP_RI_DC_N);
        ROOT_CTRL1_COPY.setBit(FSXCOMP_FSXLOG_ROOT_CTRL1_TP_DI2_DC_N);
        FAPI_TRY(ROOT_CTRL1_COPY.putCfam(i_target));
    }

    FAPI_DBG("Prepare for CBS start.");
    FAPI_TRY(CBS_CS.getCfam(i_target));
    CBS_CS.set_START_BOOT_SEQUENCER(0);
    CBS_CS.set_OPTION_SKIP_SCAN0_CLOCKSTART(not i_scan0_clockstart);
    CBS_CS.set_OPTION_PREVENT_SBE_START(not i_start_sbe);
    FAPI_TRY(CBS_CS.putCfam(i_target));

fapi_try_exit:
    return current_err;
}

ReturnCode mod_cbs_start(
    const Target<TARGET_TYPE_ANY_POZ_CHIP>& i_target,
    const bool i_start_sbe,
    const bool i_scan0_clockstart)
{
    CBS_CS_t CBS_CS;
    SB_CS_t SB_CS;
    fapi2::buffer<uint8_t> l_bootSide;
    int l_timeout = 0;

    FAPI_DBG("Entering ...");

    // Get and set the side to boot the SPPE from
    FAPI_TRY(FAPI_ATTR_GET(fapi2::ATTR_SPPE_BOOT_SIDE, i_target, l_bootSide));
    FAPI_TRY(SB_CS.getCfam(i_target));
    FAPI_DBG("Current boot side set to %d.", (SB_CS.getBits<17, 2>()));
    SB_CS.insertFromRight<17, 2, uint8_t>(l_bootSide);
    FAPI_DBG("New boot side set to %d.", (SB_CS.getBits<17, 2>()));
    FAPI_TRY(SB_CS.putCfam(i_target));

    FAPI_TRY(mod_cbs_start_prep(i_target, i_start_sbe, i_scan0_clockstart));

    FAPI_INF("Start CBS.");
    FAPI_TRY(CBS_CS.getCfam(i_target));
    CBS_CS.set_START_BOOT_SEQUENCER(1);
    FAPI_TRY(CBS_CS.putCfam(i_target));
    // Leave START_BOOT_SEQUENCER at 1 to prevent accidental restarts

    FAPI_DBG("Monitor CBS_CS INTERNAL_STATE_VECTOR to know current state of CBS state machine.");
    l_timeout = CFAM_CBS_POLL_COUNT;

    while (l_timeout != 0)
    {
        FAPI_TRY(CBS_CS.getCfam(i_target));

        if (CBS_CS.get_INTERNAL_STATE_VECTOR() == CBS_IDLE_VALUE)
        {
            break;
        }

        FAPI_TRY(fapi2::delay(CBS_IDLE_HW_NS_DELAY, CBS_IDLE_SIM_CYCLE_DELAY));
        --l_timeout;
    }

    FAPI_DBG("Loop Count :%d", l_timeout);

    // Finding the clock used for starting CBS. TODO
    //FAPI_TRY(FAPI_ATTR_GET(fapi2::ATTR_CP_REFCLOCK_SELECT, i_target_chip, l_cp_refclck_select));

    FAPI_ASSERT(l_timeout > 0,
                fapi2::POZ_CBS_NOT_IN_IDLE_STATE()
                .set_CBS_CS_READ(CBS_CS)
                .set_CBS_CS_IDLE_VALUE(CBS_IDLE_VALUE)
                .set_LOOP_COUNT(CFAM_CBS_POLL_COUNT)
                .set_HW_DELAY(CBS_IDLE_HW_NS_DELAY)
                .set_PROC_TARGET(i_target),
                //.set_CLOCK_POS(l_callout_clock),
                "ERROR: CBS HAS NOT REACHED IDLE STATE VALUE 0x002 ");

fapi_try_exit:
    FAPI_DBG("Exiting ...");
    return current_err;
}
