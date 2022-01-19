/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/hwp/generic/perv/poz_perv_mod_misc.C $ */
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
/// @file  poz_perv_mod_misc.C
///
/// @brief  contains definitions for modules CBS start, switch pcbmux
///                                          multicast setup & hangpulse setup
//------------------------------------------------------------------------------
// *HWP HW Maintainer   : Sreekanth Reddy (skadapal@in.ibm.com)
// *HWP FW Maintainer   : Raja Das (rajadas2@in.ibm.com)
//------------------------------------------------------------------------------

#include "poz_perv_mod_misc.H"
#include <p11_scom_perv.H>
#include <p11_scom_pc.H>
#include <target_filters.H>

SCOMT_PERV_USE_FSXCOMP_FSXLOG_CBS_CS;
SCOMT_PC_USE_TP_CFAM_FSI_W_SBE_FIFO_FSB_DOWNFIFO_RESET;
SCOMT_PC_USE_TP_CFAM_FSI_W_FSI2PIB_STATUS;
SCOMT_PERV_USE_FSXCOMP_FSXLOG_ROOT_CTRL0;
SCOMT_PERV_USE_FSXCOMP_FSXLOG_SB_MSG;
SCOMT_PERV_USE_HANG_PULSE_0_REG;
//SCOMT_PERV_USE_FSXCOMP_FSXLOG_CBS_ENVSTAT; TODO

using namespace fapi2;
//using namespace fapi2::p11t;
using namespace scomt::perv;
using namespace scomt::pc;

enum POZ_PERV_MOD_MISC_Private_Constants
{
    P11_CFAM_CBS_POLL_COUNT = 200, // Observed Number of times CBS read for CBS_INTERNAL_STATE_VECTOR
    CBS_IDLE_VALUE = 0x002, // Read the value of CBS_CS_INTERNAL_STATE_VECTOR
    P11_CBS_IDLE_HW_NS_DELAY = 640000, // unit is nano seconds [min : 64k x (1/100MHz) = 64k x 10(-8) = 640 us
    //                       max : 64k x (1/50MHz) = 128k x 10(-8) = 1280 us]
    P11_CBS_IDLE_SIM_CYCLE_DELAY = 750000, // unit is sim cycles,to match the poll count change ( 250000 * 30 )
    MC_GROUP_MEMBERSHIP_BITX_READ = 0x500F0001,
};

ReturnCode mod_cbs_start(
    const Target<TARGET_TYPE_ANY_POZ_CHIP>& i_target,
    bool start_sbe)
{
    FSXCOMP_FSXLOG_CBS_CS_t CBS_CS;
    TP_CFAM_FSI_W_SBE_FIFO_FSB_DOWNFIFO_RESET_t FSB_DOWNFIFO_RESET;
    TP_CFAM_FSI_W_FSI2PIB_STATUS_t FSI2PIB_STATUS;
    FSXCOMP_FSXLOG_SB_MSG_t SB_MSG;
    int l_timeout = 0;

    FAPI_INF("Entering ...");
    FAPI_INF("Read FSI2PIB_STATUS register and check whether VDN power is on or not(VDD_NEST_OBSERVE).");
    FAPI_TRY(FSI2PIB_STATUS.getCfam(i_target));

    FAPI_ASSERT(FSI2PIB_STATUS.get_VDD_NEST_OBSERVE(),
                fapi2::VDN_POWER_NOT_ON()
                .set_FSI2PIB_STATUS_READ(FSI2PIB_STATUS)
                .set_PROC_TARGET(i_target),
                "ERROR: VDN power is NOT on. i.e. FSI2PIB_STATUS register bit 16 is NOT set.");

    FAPI_INF("Clear Selfboot Message Register, Reset SBE FIFO.");
    SB_MSG = 0;
    FAPI_TRY(SB_MSG.putCfam(i_target));
    FSB_DOWNFIFO_RESET = 0x80000000;
    FAPI_TRY(FSB_DOWNFIFO_RESET.putCfam(i_target));

    FAPI_INF("Prepare for CBS start.");
    FAPI_TRY(CBS_CS.getCfam(i_target));
    CBS_CS.set_START_BOOT_SEQUENCER(0);
    CBS_CS.set_OPTION_SKIP_SCAN0_CLOCKSTART(0);
    CBS_CS.set_OPTION_PREVENT_SBE_START(not start_sbe);
    FAPI_TRY(CBS_CS.putCfam(i_target));

    FAPI_INF("Start CBS.");
    CBS_CS.set_START_BOOT_SEQUENCER(1);
    FAPI_TRY(CBS_CS.putCfam(i_target));
    // Leave START_BOOT_SEQUENCER at 1 to prevent accidental restarts

    FAPI_DBG("Monitor CBS_CS INTERNAL_STATE_VECTOR to know current state of CBS state machine.");
    l_timeout = P11_CFAM_CBS_POLL_COUNT;

    while (l_timeout != 0)
    {
        FAPI_TRY(CBS_CS.getCfam(i_target));

        if (CBS_CS.get_INTERNAL_STATE_VECTOR() == CBS_IDLE_VALUE)
        {
            break;
        }

        FAPI_TRY(fapi2::delay(P11_CBS_IDLE_HW_NS_DELAY, P11_CBS_IDLE_SIM_CYCLE_DELAY));
        --l_timeout;
    }

    FAPI_DBG("Loop Count :%d", l_timeout);

    // Finding the clock used for starting CBS. TODO
    //FAPI_TRY(FAPI_ATTR_GET(fapi2::ATTR_CP_REFCLOCK_SELECT, i_target_chip, l_cp_refclck_select));

    FAPI_ASSERT(l_timeout > 0,
                fapi2::CBS_NOT_IN_IDLE_STATE()
                .set_CBS_CS_READ(CBS_CS)
                .set_CBS_CS_IDLE_VALUE(CBS_IDLE_VALUE)
                .set_LOOP_COUNT(P11_CFAM_CBS_POLL_COUNT)
                .set_HW_DELAY(P11_CBS_IDLE_HW_NS_DELAY)
                .set_PROC_TARGET(i_target),
                //.set_CLOCK_POS(l_callout_clock),
                "ERROR: CBS HAS NOT REACHED IDLE STATE VALUE 0x002 ");

fapi_try_exit:
    FAPI_INF("Exiting ...");
    return current_err;
}
