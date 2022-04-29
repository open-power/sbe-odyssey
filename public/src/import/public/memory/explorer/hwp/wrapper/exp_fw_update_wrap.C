/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/memory/explorer/hwp/wrapper/exp_fw_update_wrap.C $ */
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

/// @file exp_twi_update_wrap.C
/// @brief Wrapper for using the explorer flash loader program to update explorer firmware
/// @note exp_twi_update_wrap needs to run first to upload the flash loader program
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
#include <pthread.h>
#include <stdexcept>
#include <croClientCapi.H>
#include <ecmdClientCapi.H>
#include <ecmdDataBuffer.H>
#include <ecmdUtils.H>
#include <ecmdSharedUtils.H>
#include <fapi2.H>
#include <fapi2ClientCapi.H>
#include <fapi2SharedUtils.H>
#include <exp_fw_update.H>
#include <generic/memory/mss_git_data.H>
#include <generic/memory/lib/utils/mss_file_io.H>

//------------------------------------------------------------------------------
// Function definitions
//------------------------------------------------------------------------------

///
/// @brief Display help message
///
static const std::string PROCEDURE = "exp_fw_update_wrap";
void help()
{
    // build help message
    char outstr[256];
    snprintf(outstr, sizeof(outstr), "\nThis is the help text for the procedure %s \n",
             PROCEDURE.c_str());
    ecmdOutput(outstr);
    snprintf(outstr, sizeof(outstr), "Git commit-id %s \n", GIT_COMMIT_ID.c_str());
    ecmdOutput(outstr);
    snprintf(outstr, sizeof(outstr), "Syntax: %s\n", PROCEDURE.c_str());
    ecmdOutput(outstr);
    ecmdOutput("        [-h] [-k#] [-n#] [-s#] [-p#] [-d] [-quiet] [-verif] [-parallel] image_file\n");
    ecmdOutput("Parameters:\n");
    ecmdOutput("      image_file      The exploer firmware image binary file\n");
    ecmdOutput("\n");
    ecmdOutput("Options:\n");
    ecmdOutput("      -h                           Display this help message.\n");

    ecmdOutput("      -k#                          Specify which cage to act on (default = 0).\n");
    ecmdOutput("      -n#                          Specify which node to act on (default = 0).\n");
    ecmdOutput("      -s#                          Specify which slot to act on (default = 0).\n");
    ecmdOutput("      -p#                          Specify which chip position to act on (default = 0).\n");
    ecmdOutput("      -quiet                       Suppress printing of eCMD DLL/procedure information (default = false).\n");
    ecmdOutput("      -verif                       Run procedure in sim verification mode (default = false).\n");
    ecmdOutput("      -parallel                    Run multiple OCMB updates in parallel (default = false).\n");
    ecmdOutput("NOTES:\n");
    ecmdOutput("  1. In parallel mode, the ecmd targeting options k/n/s/p operate on the processor chip\n");
    ecmdOutput("     and not the OCMB chip. All OCMB chips under the selected processor(s) will have their\n");
    ecmdOutput("     firmware updated in parallel mode.\n");
    ecmdOutput("  2. To run in parallel mode via OMI:\n");
    ecmdOutput("     istep -s0..12 (boot through OMI training and initialization)\n");
    ecmdOutput("     set 'USE_SBE_FIFO = on' in the Cronus config\n");
    ecmdOutput("     run this tool\n");
    ecmdOutput("     set USE_SBE_FIFO back to the original setting\n");
    ecmdOutput("\n");
    ecmdOutput("Example\n");
    snprintf(outstr, sizeof(outstr), "  %s exp_fw.bin\n", PROCEDURE.c_str());
    ecmdOutput(outstr);

    return;
}

///
/// @brief Structure representing the node target and parameters for exp_fw_update
///
typedef struct hwp_thread_struct
{
    uint8_t* l_image_data = NULL;
    size_t l_image_size = 0;

    ecmdChipTarget target;
    pthread_t thread;
} hwp_thread_t;

///
/// @brief Function to run when a thread executes, to set up a looper then execute the function for each chipunit
/// @param[in] arg hwp_thread_t structure representing the node target and parameters
/// @return fapi2::ReturnCode from exp_fw_update function
/// @note Parameters are passed in via the hwp_thread_t structure
///
void* hwp_thread_func(void* arg)
{
    hwp_thread_t* l_thread = (hwp_thread_t*) arg;

    const std::string my_hwp = "exp_fw_update_all";

    char printStr[fapi2::MAX_ECMD_STRING_LEN] = "";
    fapi2::ReturnCode l_fapi2_rc = fapi2::FAPI2_RC_SUCCESS;

    if (!ecmdGetGlobalVar(ECMD_GLOBALVAR_QUIETMODE))
    {
        snprintf(printStr, sizeof(printStr), "Going to call %s on %s\n",
                 PROCEDURE.c_str(),
                 ecmdWriteTarget(l_thread->target).c_str());
        ecmdOutput(printStr);
    }

    fapi2::Target<fapi2::TARGET_TYPE_PROC_CHIP> fapi_target(&l_thread->target);

    l_fapi2_rc = exp_fw_update_all( fapi_target,
                                    l_thread->l_image_data,
                                    l_thread->l_image_size );

    if (l_fapi2_rc != fapi2::FAPI2_RC_SUCCESS)
    {
        ecmdOutputError("-----------------------------------------------------------\n");
        snprintf(printStr, sizeof(printStr), "          Procedure: %s  exited early ...\n", my_hwp.c_str());
        ecmdOutputError(printStr);
        snprintf(printStr, sizeof(printStr), "          on ecmdChipTarget = %s\n", ecmdWriteTarget(l_thread->target).c_str());
        ecmdOutputError(printStr);
        snprintf(printStr, sizeof(printStr), "          Return code = 0x%08x \n", static_cast<uint32_t>(l_fapi2_rc));
        ecmdOutputError(printStr);
        ecmdOutputError("-----------------------------------------------------------\n");
    }

    pthread_exit((void*)&l_fapi2_rc);
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
    char outstr[256];
    char* l_image_file = NULL;
    uint8_t* l_image_data = NULL;
    size_t l_image_size = 0;
    bool l_parallel_mode = false;
    pthread_attr_t threadAttributes;
    std::list<hwp_thread_t*> thread_list;

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

        // run procedure in sim verification mode
        if (ecmdParseOption(&argc, &argv, "-verif"))
        {
            GLOBAL_VERIF_MODE = true;
        }

        // run procedure in parallel mode
        if (ecmdParseOption(&argc, &argv, "-parallel"))
        {
            l_parallel_mode = true;
        }

        // parse out common eCMD args like -p0, -c0, -coe, etc..
        // any found args will be removed from arg list upon return
        rc = ecmdCommandArgs(&argc, &argv);

        if (rc)
        {
            ecmdOutputError("Error parsing eCMD arguments\n");
            break;
        }

        // check the image_file parameter
        if (argc != 2)
        {
            ecmdOutputError("You need to specify an image file\n");
            help();
            rc = ECMD_INVALID_ARGS;
            break;
        }

        // Get the image filename
        l_image_file = argv[1];

        // Read data from file
        rc = mss::read_file(l_image_file, l_image_data, l_image_size) ;

        if (rc)
        {
            snprintf(outstr, sizeof(outstr), "Can't read data from file %s\n", l_image_file);
            ecmdOutputError(outstr);
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

        if (l_parallel_mode)
        {
            ecmdLooperData pu_looper;
            ecmdChipTarget l_proc_target;

            l_proc_target.chipType = "pu";
            l_proc_target.chipTypeState = ECMD_TARGET_FIELD_VALID;
            l_proc_target.cageState   = ECMD_TARGET_FIELD_WILDCARD;
            l_proc_target.nodeState   = ECMD_TARGET_FIELD_WILDCARD;
            l_proc_target.slotState   = ECMD_TARGET_FIELD_WILDCARD;
            l_proc_target.posState    = ECMD_TARGET_FIELD_WILDCARD;
            l_proc_target.chipUnitNumState   = ECMD_TARGET_FIELD_UNUSED;
            l_proc_target.threadState = ECMD_TARGET_FIELD_UNUSED;

            rc = ecmdConfigLooperInit(l_proc_target, ECMD_SELECTED_TARGETS_LOOP_DEFALL, pu_looper);


            if (rc)
            {
                ecmdOutputError("Error initializing chip looper!\n");
                break;
            }

            // loop over configured explorers inside current node
            while(ecmdConfigLooperNext(l_proc_target, pu_looper))
            {
                // Set an attribute to reduce the stack size of our threads from the default.
                pthread_attr_init(&threadAttributes);
                size_t stacksize = 0x400000;
                pthread_attr_setstacksize(&threadAttributes, stacksize);

                // Create the data structure to store the target and params
                hwp_thread_t* new_hwp_thread = new (std::nothrow) hwp_thread_t;

                new_hwp_thread->target = l_proc_target;
                new_hwp_thread->l_image_data = l_image_data;
                new_hwp_thread->l_image_size = l_image_size;

                // Start the new thread, and add it to our thread list
                rc = pthread_create(&(new_hwp_thread->thread), &threadAttributes, hwp_thread_func, (void*) new_hwp_thread);

                if (rc)
                {
                    snprintf(outstr, sizeof(outstr), "**** Error creating pthread for ecmdChipTarget = %s\n",
                             ecmdWriteTarget(l_proc_target).c_str());
                    ecmdOutputError(outstr);
                    snprintf(outstr, sizeof(outstr), "**** Error code = %d\n", rc);
                    ecmdOutputError(outstr);
                    break;
                }
                else
                {
                    thread_list.push_back(new_hwp_thread);
                }
            }

            // Join threads and track any error status
            // Note that this could hang if any of the threads do not return
            while (thread_list.empty() == false)
            {
                fapi2::ReturnCode* l_fapi2_rc;
                hwp_thread_t* hwp_thread_ptr = thread_list.front();
                pthread_join(hwp_thread_ptr->thread, (void**)&l_fapi2_rc);

                if (*l_fapi2_rc != fapi2::FAPI2_RC_SUCCESS)
                {
                    rc = ECMD_FAILURE;
                }

                delete hwp_thread_ptr;
                thread_list.pop_front();
            }

            // Cleanup the thread attribute
            pthread_attr_destroy(&threadAttributes);
        }
        else // serial mode
        {
            ecmdLooperData ocmb_looper;
            ecmdChipTarget l_ocmb_target;

            l_ocmb_target.chipType = "ocmb";
            l_ocmb_target.chipTypeState = ECMD_TARGET_FIELD_VALID;
            l_ocmb_target.cageState   = ECMD_TARGET_FIELD_WILDCARD;
            l_ocmb_target.nodeState   = ECMD_TARGET_FIELD_WILDCARD;
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

            // loop over configured explorers inside current node
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
                rc = static_cast<uint32_t>(exp_fw_update(fapi_target, l_image_data, l_image_size));

                if (rc)
                {
                    snprintf(outstr, sizeof(outstr), "ERROR: %s FAPI call exited with bad return code = %s 0x%08x\n",
                             PROCEDURE.c_str(),
                             ecmdParseReturnCode(rc).c_str(), rc);
                    ecmdOutputError(outstr);
                    break;
                }
            }
        }

        if (rc)
        {
            break;
        }
    }
    while(0);

    // Free l_image_data
    if(l_image_data != NULL)
    {
        free(l_image_data);
    }

    if (!rc)
    {
        snprintf(outstr, sizeof(outstr), "%s  completed successfully!\n", PROCEDURE.c_str());
        ecmdOutput(outstr);
    }
    else
    {
        snprintf(outstr, sizeof(outstr), "%s  did not complete successfully.\n", PROCEDURE.c_str());
        ecmdOutputError(outstr);
    }

    ecmdUnloadDll();

    return rc;
}
