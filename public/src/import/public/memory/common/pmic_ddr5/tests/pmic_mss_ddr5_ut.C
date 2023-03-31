/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/memory/common/pmic_ddr5/tests/pmic_mss_ddr5_ut.C $ */
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
/// @file chips/ocmb/common/procedures/hwp/pmic_ddr5/tests/pmic_mss_ddr5_ut.C
/// @brief Unit tests for memory apis
///
// *HWP HWP Owner: Sneha Kadam <sneha.kadam1@ibm.com>
// *HWP HWP Backup: Louis Stermole <stermole@us.ibm.com>
// *HWP Team: Memory
// *HWP Level: 4
// *HWP Consumed by: CI

#define CATCH_CONFIG_RUNNER

#include <string>
#include <sstream>
#include <vector>

#include <prcdUtils.H>

#include <fapi2.H>
#include <fapi2ClientCapi.H>
#include <fapi2SharedUtils.H>
#include <generic/memory/tests/mss_run_unit_tests.H>
#include <generic/memory/tests/target_fixture.H>
#include <generic/memory/lib/utils/find.H>
#include <generic/memory/tests/mss_lab_tools.H>
#include <generic/memory/proc_specific/tests/ody_mss_reset_attrs_ut.H>

// main function
int main(int i_argc, char* i_argv[])
{
    //This forces mss logging to be console only
    setenv("MSS_LOG_OUTPUT_TARGETS", "CONSOLE", 1);

    //initialize ecmd/fapi2 interface
    fapi2::ReturnCode l_rc;
    mss::lab::tool_init lab_tool(l_rc, i_argc, i_argv);

    //Verify lab_tool initialized everythign ok.
    mss::lab::is_ok(l_rc, "Failed initializing ecmd");

    // Load the current state of the attributes in to the current process's attributes
    // We can't call a unit test here as catch isn't setup yet. There doesn't appear to
    // be a way to force a test to always run first, so we build what ammounts to part of
    // and ecmd wrapper to handle things for us.
    mss::ecmd::ecmd_config_looper l_looper("pu", "");
    mss::lab::is_ok(!l_looper.status(), "ECMD looper didn't initialize" );

    bool l_found_targets = false;

    std::vector<std::unique_ptr<ecmdChipTarget>> l_ecmd_targets;
    l_looper.get_ecmd_targets(l_ecmd_targets);

    for( const auto& ecmd_target : l_ecmd_targets )
    {
        fapi2::Target<fapi2::TARGET_TYPE_PROC_CHIP> l_proc(ecmd_target.get());
        fapi2::ReturnCode l_rc = mss::configure_attrs(l_proc);
        mss::lab::is_ok(l_rc, "Failed initializing attributes");

        l_found_targets = true;
    }

    mss::lab::is_ok(l_found_targets == true, "ECMD looper didn't give us any targets");

    RUN_MSS_CATCH_SESSION(i_argc, i_argv, l_rc);

    return l_rc;
}
