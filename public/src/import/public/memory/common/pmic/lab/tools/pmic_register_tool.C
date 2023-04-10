/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/memory/common/pmic/lab/tools/pmic_register_tool.C $ */
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
/// @file pmic_register_tool.C
/// @brief tool to read and write PMIC registers
///
// *HWP HWP Owner: Michael Pardeik <pardeik@us.ibm.com>
// *HWP HWP Backup: Louis Stermole <stermole@us.ibm.com>
// *HWP Team: Memory
// *HWP Level: 2

#include <fapi2.H>
#include <generic/memory/lab/mss_lab_tools.H>
#include <generic/memory/lib/utils/find.H>
#include <generic/memory/mss_git_data.H>
#include <chips/ocmb/common/procedures/hwp/pmic/lab/tools/pmic_register_tool.H>

///
/// @brief main test function
/// @param[in]  i_argc              number of arguments passed in by the user
/// @param[in]  i_argv              list of arguments passed in by the user
///
int main(int i_argc, char* i_argv[])
{
    fapi2::ReturnCode rc = fapi2::FAPI2_RC_SUCCESS;
    fapi2::ReturnCode error_occurred = fapi2::FAPI2_RC_SUCCESS;
    struct mss::lab::pmic::reg::options_pmic_register_tool pmic_reg_tool_options;

    // Memory lab tools initialization
    mss::lab::tool_init tool_init(rc, i_argc, i_argv);
    mss::lab::is_ok(rc, "Failed to initialize lab tool!");
    printf("Git commit ID: %s Procedure: pmic_register_tool.exe \n",
           GIT_COMMIT_ID.c_str());

    // process tool options
    rc = mss::lab::pmic::reg::process_pmic_register_tool_options(i_argc, i_argv, pmic_reg_tool_options);

    // Enforces the open power toolkit requirement that help screens always return success
    if(mss::lab::check_if_helpscreen_called(rc))
    {
        return fapi2::FAPI2_RC_SUCCESS;
    }

    mss::lab::is_ok(rc, "Error processing inputs");

    // Parse out common eCMD args like -p0, -c0, -coe, etc..
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

        // Run procedure
        FAPI_TRY(mss::lab::pmic::reg::get_set_pmic_register(fapi2_ocmb_target,
                 pmic_reg_tool_options));
    }

    return fapi2::FAPI2_RC_SUCCESS;

fapi_try_exit:
    return fapi2::current_err;
}
