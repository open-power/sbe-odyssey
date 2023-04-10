/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/memory/common/pmic/lab/tools/pmic_bias_nmode_tool.C $ */
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
///
/// @file pmic_bias_nmode_tool.C
/// @brief Bias PMIC device with given parameters while the secondary device is disabled
///
// *HWP HWP Owner: Stephen Glancy <sglancy@us.ibm.com>
// *HWP HWP Backup: Louis Stermole <stermole@us.ibm.com>
// *HWP Team: Memory
// *HWP Level: 2

#include <fapi2.H>
#include <generic/memory/lab/mss_lab_tools.H>
#include <generic/memory/lib/utils/find.H>
#include <generic/memory/mss_git_data.H>
#include <pmic_bias/pmic_bias_tool_utils.H>
#include <pmic_bias/pmic_bias_utils.H>
#include <lib/i2c/i2c_pmic.H>

///
/// @brief main test function
///
int main(int i_argc, char* i_argv[])
{
    fapi2::ReturnCode rc;

    // Memory lab tools initialization
    mss::lab::tool_init tool_init(rc, i_argc, i_argv);
    mss::lab::is_ok(rc, "Failed to initialize lab tool!");
    printf("Git commit ID: %s Procedure: pmic_bias_nmode_tool.exe \n",
           GIT_COMMIT_ID.c_str());

    // Struct for user input
    mss::lab::pmic::bias::inputs l_inputs;

    rc = mss::lab::pmic::bias::process_inputs(i_argc, i_argv, l_inputs);

    // Enforces the open power toolkit requirement that help screens always return success
    if(mss::lab::check_if_helpscreen_called(rc))
    {
        return fapi2::FAPI2_RC_SUCCESS;
    }

    mss::lab::is_ok(rc, "Error processing inputs");

    if (l_inputs.iv_id == mss::lab::pmic::id::ALL)
    {
        FAPI_ERR("ID cannot be PMIC all for this tool!");
        return fapi2::FAPI2_RC_INVALID_PARAMETER;
    }

    // Parse out common eCMD args like -p0, -c0, -coe, etc..
    // Do this after parsing off tool specific arguments
    uint32_t l_rc = ecmdCommandArgs(&i_argc, &i_argv);
    mss::lab::is_ok((l_rc == ECMD_SUCCESS), "Failed parsing ecmd command args");

    // Get ocmb list from Cronus
    const auto& ecmd_target_list = mss::lab::get_ecmd_targets(ECMD_SELECTED_TARGETS_LOOP_DEFALL, "", "ocmb");
    mss::logf(mss::INFO, "Found %d OCMB targets", ecmd_target_list.size());

    for (const auto& ecmd_ocmb_target : ecmd_target_list)
    {
        // Create FAPI2 Target
        fapi2::Target<fapi2::TARGET_TYPE_OCMB_CHIP> fapi2_ocmb_target;

        // associate the ocmb ecmdChiptTarget with the fapi2 ocmb Target
        l_rc = mss::lab::get_fapi2_target(ecmd_ocmb_target, fapi2_ocmb_target);
        mss::lab::is_ok(l_rc == ECMD_SUCCESS, "Failed to get fapi2 ocmb");

        // retrieve the Cronus string representing the fapi2 target
        mss::logf(mss::DEBUG, "OCMB: %s", mss::c_str(fapi2_ocmb_target));


        // Either PMIC0 or PMIC1, can cast to mss::pmic::id and use the common_lab helper
        const auto l_pmics = mss::pmic::get_valid_pmics_for_id(fapi2_ocmb_target, static_cast<mss::pmic::id>(l_inputs.iv_id));

        // If we do not have at least 2 PMIC's, exit with an error
        if(l_pmics.size() != 2)
        {
            FAPI_ERR("%s has %u PMIC's. 2 are required!", mss::c_str(fapi2_ocmb_target), l_pmics.size());
            return fapi2::FAPI2_RC_INVALID_PARAMETER;
        }

        const auto& l_pmic_target = l_pmics[1];
        static constexpr auto J = mss::pmic::product::JEDEC_COMPLIANT;
        using REGS = pmicRegs<J>;
        using FIELDS = pmicFields<J>;

        // Disable the redundant PMIC
        fapi2::buffer<uint8_t> l_orig_vr_enable;
        rc = mss::pmic::i2c::reg_read_reverse_buffer(l_pmic_target, REGS::R32, l_orig_vr_enable);
        mss::lab::is_ok(rc, "Failed reading/writing explorer registers");

        // Disable VR Enable (1 --> Bit 7)
        auto l_vr_enable = l_orig_vr_enable;
        l_vr_enable.clearBit<FIELDS::R32_VR_ENABLE>();

        rc = mss::pmic::i2c::reg_write_reverse_buffer(l_pmic_target, REGS::R32, l_vr_enable);
        mss::lab::is_ok(rc, "Failed reading/writing explorer registers");


        // Run procedure
        rc = mss::lab::pmic::bias_chip(
                 fapi2_ocmb_target,
                 l_inputs.iv_id,
                 l_inputs.iv_setting,
                 l_inputs.iv_amount,
                 l_inputs.iv_unit,
                 l_inputs.iv_force);

        mss::lab::is_ok(rc, "Failed reading/writing explorer registers");

        rc = mss::pmic::i2c::reg_write_reverse_buffer(l_pmic_target, REGS::R32, l_orig_vr_enable);
        mss::lab::is_ok(rc, "Failed reading/writing explorer registers");
    }

    mss::log(mss::DEBUG, "Finished");
    return 0;
}
