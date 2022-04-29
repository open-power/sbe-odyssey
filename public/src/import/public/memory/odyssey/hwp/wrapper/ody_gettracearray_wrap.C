/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/memory/odyssey/hwp/wrapper/ody_gettracearray_wrap.C $ */
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

// Includes
#include <fapi2.H>
#include <fapi2ClientCapi.H>
#include <ecmdClientCapi.H>
#include <ecmdSharedUtils.H>
#include <ecmdUtils.H>

#include "ody_gettracearray.H"

#define procedureName "ody_gettracearray"
#define REVISION "1.0"

#define PRINT_ERR(msg) ecmdOutput(procedureName "_wrap: " msg "\n")

#define WARN(msg) PRINT_ERR("WARNING: " msg)

#define ERR(retval, msg) { \
        rc = retval; \
        PRINT_ERR(msg); \
        goto fail; \
    }

#define TRY(action, msg) \
    rc = action; \
    if (rc != ECMD_SUCCESS) \
        ERR(rc, msg);

struct bus_def
{
    std::string name;
    ody_tracearray_id trace_array;
};

static const bus_def bus_defs[] =
{
    {"TRACE0", ODY_TA_TRACE0},
    {"TRACE1", ODY_TA_TRACE1},
};

static const int NUM_BUS_DEFS = ARRAY_SIZE(bus_defs);

void help()
{
    ecmdOutput("\nThis is the help text for the procedure " procedureName " v" REVISION "\n\n");
    ecmdOutput("Usage: " procedureName "_wrap [options ...] <trace_bus> [more options ...]\n");
    ecmdOutput("  <trace_bus>  Name of the trace bus; use -list to find out available trace buses");
    ecmdOutput("  Option flags are:\n");
    ecmdOutput("      -stop      Stop the trace array before dumping.\n");
    ecmdOutput("      -reset     Reset the debug logic after dumping.\n");
    ecmdOutput("      -start     Start the trace array after dumping.\n");
    ecmdOutput("      -nodump    Skip the array dumping step -- use if you only want to start/stop/reset.\n");
    ecmdOutput("      -list      List available trace buses.\n");
    ecmdOutput("      -trace88   Dumps the content of first 88 bits of traces.\n");
    ecmdOutput("      -h         Display this help message.\n");
    ecmdOutput("      -k#        Specify which cage to act on.\n");
    ecmdOutput("      -n#        Specify which node to act on.\n");
    ecmdOutput("      -s#        Specify which slot to act on.\n");
    ecmdOutput("      -p#        Specify which chip position to act on.\n");
    ecmdOutput("      -c#        Specify chip unit number for OBUS/MC/EX/EC trace buses.\n");
    ecmdOutput("      -quiet     Suppress printing of eCMD DLL/procedure informational messages (default = false).\n");
    ecmdOutput("      -verif     Run procedure in sim verification mode (default = false).\n");

}

int main(int argc, char* argv[])
{
    extern bool GLOBAL_SIM_MODE;
    extern bool GLOBAL_VERIF_MODE;
    bool bus_found = false;
    bool loop_entered = false;
    bool l_get_88bits = false;
    uint64_t rc = ECMD_SUCCESS;
    fapi2::ReturnCode rc_fapi;
    std::string trace_name;
    ecmdLooperData looper; // Store internal Looper data
    ecmdChipTarget target; // This is the target to operate on
    ecmdDllInfo DLLINFO;
    ody_gettracearray_args gettracearray_args;

    //
    // ------------------------------------
    // Load and initialize the eCMD Dll
    // If left NULL, which DLL to load is determined by the ECMD_DLL_FILE environment variable
    // If set to a specific value, the specified dll will be loaded
    // ------------------------------------
    TRY(ecmdLoadDll(""), "Failed to load eCMD");

    //
    // This is needed if you're running a FAPI procedure from this eCMD procedure
    TRY(fapi2InitExtension(), "Failed to initialize FAPI");

    //
    // establish if this is a simulation run or not
    //
    // NEEDED?????
    TRY(ecmdQueryDllInfo(DLLINFO), "Failed to query eCMD DLL info");

    if (DLLINFO.dllEnv == ECMD_DLL_ENV_SIM)
    {
        GLOBAL_SIM_MODE = true;
    }

    // Parse out user options (excluding -pX, -cX, -coe, -debug, etc
    if (ecmdParseOption(&argc, &argv, "-h"))
    {
        help();
        ecmdUnloadDll();
        return 1;
    }

    // Parse out user options (excluding -pX, -cX, -coe, -debug, etc
    if (ecmdParseOption(&argc, &argv, "-list"))
    {
        int len = 0;

        for (int i = 0; i < NUM_BUS_DEFS; i++)
        {
            ecmdOutput(bus_defs[i].name.c_str());
            len += bus_defs[i].name.length() + 2;

            if (i == NUM_BUS_DEFS - 1)
            {
                ecmdOutput("\n");
            }
            else if (len > 80)
            {
                ecmdOutput(",\n");
                len = 0;
            }
            else
            {
                ecmdOutput(", ");
            }
        }

        ecmdUnloadDll();
        return 1;
    }

    // run procedure in sim verification mode
    if (ecmdParseOption(&argc, &argv, "-verif"))
    {
        GLOBAL_VERIF_MODE = true;
    }

    if(ecmdParseOption(&argc, &argv, "-trace88"))
    {
        l_get_88bits = true;
    }

    gettracearray_args.stop_pre_dump      =  ecmdParseOption(&argc, &argv, "-stop");
    gettracearray_args.collect_dump       = !ecmdParseOption(&argc, &argv, "-nodump");
    gettracearray_args.reset_post_dump    =  ecmdParseOption(&argc, &argv, "-reset");
    gettracearray_args.restart_post_dump  =  ecmdParseOption(&argc, &argv, "-start");

    //
    // -------------------------------------------------------------------------------------------------
    // Parse out common eCMD args like -p0, -c0, -coe, etc..
    // Any found args will be removed from arg list upon return
    // -------------------------------------------------------------------------------------------------
    TRY(ecmdCommandArgs(&argc, &argv), "Failed to parse eCMD arguments");

    // unsupported arguments left over?
    if (argc != 2)
    {
        if (argc > 2)
        {
            PRINT_ERR("Unrecognized arguments specified");
        }
        else
        {
            PRINT_ERR("Missing trace bus name parameter");
        }

        help();
        rc = ECMD_INVALID_ARGS;
        goto fail;
    }

    // Now the only remaining parameter apart from the executable name must be a trace bus name.
    // Go and match it against our list of trace buses.
    for (unsigned int i = 0; i < strlen(argv[1]); i++)
    {
        argv[1][i] = toupper(argv[1][i]);
    }

    for (unsigned int i = 0; i < NUM_BUS_DEFS; i++)
    {
        if (bus_defs[i].name.compare(argv[1]))
        {
            continue;
        }

        gettracearray_args.trace_array = bus_defs[i].trace_array;
        bus_found = true;
        break;
    }

    if (!bus_found)
    {
        ERR(ECMD_INVALID_ARGS, "Unknown trace bus name");
    }

    //
    // always print the dll info to the screen, unless in quiet mode
    if (!ecmdGetGlobalVar(ECMD_GLOBALVAR_QUIETMODE))
    {
        TRY(ecmdDisplayDllInfo(), "Failed to display eCMD DLL info");
    }

    target.chipType          = "odyssey";
    target.chipTypeState     = ECMD_TARGET_FIELD_VALID;
    target.chipUnitTypeState = ECMD_TARGET_FIELD_UNUSED;
    target.chipUnitNumState  = ECMD_TARGET_FIELD_UNUSED;
    target.cageState         = ECMD_TARGET_FIELD_WILDCARD;
    target.nodeState         = ECMD_TARGET_FIELD_WILDCARD;
    target.slotState         = ECMD_TARGET_FIELD_WILDCARD;
    target.posState          = ECMD_TARGET_FIELD_WILDCARD;
    target.threadState       = ECMD_TARGET_FIELD_UNUSED;

    // loop over specified configured positions
    TRY(ecmdConfigLooperInit(target, ECMD_SELECTED_TARGETS_LOOP, looper), "Failed to initialize target loop");

    while (ecmdConfigLooperNext(target, looper))
    {
        loop_entered = true;

        if (!ecmdGetGlobalVar(ECMD_GLOBALVAR_QUIETMODE))
        {
            ecmdOutput(("# " + ecmdWriteTarget(target) + "\n").c_str());
        }

        // set up fapi target from an ecmd target
        fapi2::Target<fapi2::TARGET_TYPE_OCMB_CHIP> fapi_target(&target);
        fapi2::variable_buffer array_data(ODY_TRACEARRAY_NUM_ROWS * ODY_TRACEARRAY_BITS_PER_ROW);
        FAPI_EXEC_HWP(rc_fapi, ody_gettracearray, fapi_target, gettracearray_args, array_data);
        rc = (uint64_t)rc_fapi;

        if (rc)
        {
            ERR(rc, "Procedure " procedureName " failed")
        }

        if (gettracearray_args.collect_dump)
        {
            for (unsigned int i = 0; i < ODY_TRACEARRAY_NUM_ROWS; i++)
            {
                char str[256];

                if(l_get_88bits)
                {
                    snprintf(str, sizeof(str), "%016lX%06lX\n", array_data.get<uint64_t>(i * 2),
                             (array_data.get<uint64_t>(i * 2 + 1) >> 40));
                }
                else
                {
                    snprintf(str, sizeof(str), "0x%016lX%016lX\n", array_data.get<uint64_t>(i * 2), array_data.get<uint64_t>(i * 2 + 1));
                }

                ecmdOutput(str);
            }
        }
    }

    if (!loop_entered)
    {
        WARN("Did not iterate over any targets -- maybe you need to be more specific with the -p#, -c# etc. arguments?");
    }

fail:
    // Unload the eCMD Dll, this should always be the last thing you do
    ecmdUnloadDll();
    return rc;
}
