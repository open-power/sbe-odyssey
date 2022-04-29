/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/memory/explorer/hwp/wrapper/exp_check_for_ready_wrap.C $ */
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

/// @file exp_check_for_ready_wrap.C
/// @brief Wrapper for calling exp_check_for_ready hwp
///
/// *HWP HWP Owner: Louis Stermole <stermole@us.ibm.com>
/// *HWP HWP Backup: Stephen Glancy <sglancy@us.ibm.com>
/// *HWP Team: Memory
/// *HWP Level: 2
/// *HWP Consumed by: HB


//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------
#include <prcdUtils.H>
#include <croClientCapi.H>
#include <ecmdClientCapi.H>
#include <ecmdDataBuffer.H>
#include <ecmdUtils.H>
#include <ecmdSharedUtils.H>
#include <fapi2.H>
#include <fapi2ClientCapi.H>
#include <fapi2SharedUtils.H>
#include <lib/i2c/exp_i2c.H>
#include <generic/memory/mss_git_data.H>


//------------------------------------------------------------------------------
// Function definitions
//------------------------------------------------------------------------------

// display help message
static const std::string PROCEDURE = "exp_check_for_ready_wrap";
void help()
{
    // procedure constants

    // build help message
    char outstr[256];
    snprintf(outstr, sizeof(outstr), "\nThis is the help text for the procedure %s \n",
             PROCEDURE.c_str());
    ecmdOutput(outstr);
    snprintf(outstr, sizeof(outstr), "Git commit-id %s \n",
             GIT_COMMIT_ID.c_str());
    ecmdOutput(outstr);
    ecmdOutput("        [-h] [-k#] [-n#] [-s#] [-p#] [-d] [-quiet] [-verif]\n");
    ecmdOutput("\n");
    ecmdOutput("Additional options:\n");
    ecmdOutput("      -h                           Display this help message.\n");
    ecmdOutput("      -delay [#]                   Specify delay in ns between fw_status commands (default = 1000000, or 1ms).\n");
    ecmdOutput("      -attempts [#]                Specify number of fw_status commands for each poll (default = 6000).\n");

    ecmdOutput("      -k#                          Specify which cage to act on (default = 0).\n");
    ecmdOutput("      -n#                          Specify which node to act on (default = 0).\n");
    ecmdOutput("      -s#                          Specify which slot to act on (default = 0).\n");
    ecmdOutput("      -p#                          Specify which chip position to act on (default = 0).\n");
    ecmdOutput("      -quiet                       Suppress printing of eCMD DLL/procedure information (default = false).\n");
    ecmdOutput("      -verif                       Run procedure in sim verification mode (default = false).\n");
    return;
}

// main function
int main(int argc, char* argv[])
{
    // from prcdUtils
    extern bool GLOBAL_SIM_MODE;
    extern bool GLOBAL_VERIF_MODE;

    // flow/control variables
    uint32_t rc = ECMD_SUCCESS;
    fapi2::ReturnCode rc_fapi;
    ecmdDllInfo DLLINFO;
    ecmdLooperData node_looper;
    ecmdChipTarget node_target;
    char outstr[256];
    char* argdata;
    uint64_t l_delay = mss::DELAY_1MS;
    uint64_t l_attempts = 6000;

    // required parameters & optional flags

    // load and initialize the eCMD Dll
    // if left NULL, which DLL to load is determined by the ECMD_DLL_FILE
    // environment variable if set to a specific value, the specified DLL
    // will be loaded
    rc = ecmdLoadDll("");
    snprintf(outstr, sizeof(outstr), "Git commit ID: %s Procedure %s \n",
             GIT_COMMIT_ID.c_str(), PROCEDURE.c_str());
    ecmdOutput(outstr);

    if (rc)
    {
        ecmdOutputError("Failed to load the ecmd DLL\n");
        return rc;
    }

    do
    {
        // initalize FAPI extension
        rc = fapi2InitExtension();

        if (rc)
        {
            ecmdOutputError("Error initializing FAPI extension!\n");
            break;
        }

        // establish if this is a simulation run or not
        rc = ecmdQueryDllInfo(DLLINFO);

        if (rc)
        {
            ecmdOutput("Error querying DLL!\n");
            break;
        }

        if (DLLINFO.dllEnv == ECMD_DLL_ENV_SIM)
        {
            GLOBAL_SIM_MODE = true;
        }

        // show help message
        if (ecmdParseOption(&argc, &argv, "-h"))
        {
            help();
            break;
        }

        argdata = ecmdParseOptionWithArgs(&argc, &argv, "-delay");

        if (argdata != NULL)
        {
            l_delay = strtoull(argdata, NULL, 10);
        }

        argdata = ecmdParseOptionWithArgs(&argc, &argv, "-attempts");

        if (argdata != NULL)
        {
            l_attempts = strtoull(argdata, NULL, 10);
        }

        // run procedure in sim verification mode
        if (ecmdParseOption(&argc, &argv, "-verif"))
        {
            GLOBAL_VERIF_MODE = true;
        }

        // parse out common eCMD args like -p0, -c0, -coe, etc..
        // any found args will be removed from arg list upon return
        rc = ecmdCommandArgs(&argc, &argv);

        if (rc)
        {
            ecmdOutputError("Error parsing eCMD arguments\n");
            break;
        }

        // unsupported arguments left over?
        if (argc != 1)
        {
            ecmdOutputError("Unknown/unsupported arguments specified!\n");
            help();
            rc = ECMD_INVALID_ARGS;
            break;
        }

        // print procedure information header
        if (!ecmdGetGlobalVar(ECMD_GLOBALVAR_QUIETMODE))
        {
            // always print the DLL info to the screen, unless in quiet mode
            rc = ecmdDisplayDllInfo();

            if (rc)
            {
                ecmdOutputError("Error displaying DLL info!");
                break;
            }
        }

        // configure looper to iterate over all nodes
        node_target.cageState   = ECMD_TARGET_FIELD_WILDCARD;
        node_target.nodeState   = ECMD_TARGET_FIELD_WILDCARD;
        node_target.slotState   = ECMD_TARGET_FIELD_UNUSED;
        node_target.posState    = ECMD_TARGET_FIELD_UNUSED;
        node_target.chipUnitNumState   = ECMD_TARGET_FIELD_UNUSED;
        node_target.threadState = ECMD_TARGET_FIELD_UNUSED;
        rc = ecmdConfigLooperInit(node_target, ECMD_SELECTED_TARGETS_LOOP_DEFALL, node_looper);

        if (rc)
        {
            ecmdOutputError("Error initializing node looper!\n");
            break;
        }

        // loop over specified configured nodes
        while (ecmdConfigLooperNext(node_target, node_looper))
        {
            ecmdLooperData ocmb_looper;
            ecmdChipTarget l_ocmb_target;

            if (!ecmdGetGlobalVar(ECMD_GLOBALVAR_QUIETMODE))
            {
                snprintf(outstr, sizeof(outstr), "Processing %s\n",
                         ecmdWriteTarget(node_target).c_str());
                ecmdOutput(outstr);
            }

            l_ocmb_target.chipType = "ocmb";
            l_ocmb_target.chipTypeState = ECMD_TARGET_FIELD_VALID;

            l_ocmb_target.cage = node_target.cage;
            l_ocmb_target.node = node_target.node;

            l_ocmb_target.cageState   = ECMD_TARGET_FIELD_VALID;
            l_ocmb_target.nodeState   = ECMD_TARGET_FIELD_VALID;
            l_ocmb_target.slotState   = ECMD_TARGET_FIELD_WILDCARD;
            l_ocmb_target.posState    = ECMD_TARGET_FIELD_WILDCARD;
            l_ocmb_target.chipUnitNumState   = ECMD_TARGET_FIELD_UNUSED;
            l_ocmb_target.threadState = ECMD_TARGET_FIELD_UNUSED;

            rc = ecmdConfigLooperInit(l_ocmb_target, ECMD_SELECTED_TARGETS_LOOP_DEFALL, ocmb_looper);

            if (rc)
            {
                ecmdOutputError("Error initializing chip looper!\n");
                break;
            }

            // loop over configured positions inside current node
            while(ecmdConfigLooperNext(l_ocmb_target, ocmb_looper))
            {
                if (!ecmdGetGlobalVar(ECMD_GLOBALVAR_QUIETMODE))
                {
                    snprintf(outstr, sizeof(outstr), "Going to call %s on %s\n",
                             PROCEDURE.c_str(),
                             ecmdWriteTarget(l_ocmb_target).c_str());
                    ecmdOutput(outstr);
                }

                // invoke FAPI procedure core
                fapi2::Target<fapi2::TARGET_TYPE_OCMB_CHIP> fapi_target(&l_ocmb_target);
                rc_fapi = mss::exp::i2c::exp_check_for_ready_helper(fapi_target, l_attempts, l_delay);
                rc = (uint32_t) rc_fapi;

                if (rc)
                {
                    snprintf(outstr, sizeof(outstr), "ERROR: %s FAPI call exited with bad return code = %s 0x%08x\n",
                             PROCEDURE.c_str(),
                             ecmdParseReturnCode(rc).c_str(), rc);
                    ecmdOutputError(outstr);
                    break;
                }
            }

            if (rc)
            {
                break;
            }
        }

        if (rc)
        {
            break;
        }
    }
    while(0);

    ecmdUnloadDll();
    return rc;
}
