/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/common/generic/fapi2/include/default_wrapper.C $ */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2021,2022                        */
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
/**
 * @file default_wrapper.C
 * @brief Boilerplate code for procedure wrappers
 *
 * @author Joachim Fenkes <fenkes@de.ibm.com>
 */

// Stringify the procedure name
#define __STR(x) #x
#define _STR(x) __STR(x)
#define PROCEDURE_NAME_STR _STR(PROCEDURE_NAME)
#define OUTPUT_PREFIX PROCEDURE_NAME_STR "_wrap: "
const char* procedureName = PROCEDURE_NAME_STR;

// Determine the procedure function pointer type
#define __FP_T(x) x##_FP_t
#define _FP_T(x) __FP_T(x)
#define PROCEDURE_FP_T _FP_T(PROCEDURE_NAME)

void call_hwp(fapi2::Target<FAPI_TARGET_TYPE> i_fapi_target, fapi2::ReturnCode& o_rc) __attribute__((weak));

// Default loop implementation that only calls the procedure with no extra parameters
uint64_t default_loop(ecmdChipTarget& i_target)
{
    output("%s", ecmdWriteTarget(i_target).c_str());

    // set up fapi target from an ecmd target
    fapi2::Target<FAPI_TARGET_TYPE>fapi_target(&i_target);
    fapi2::ReturnCode rc_fapi;

    if (call_hwp)
    {
        call_hwp(fapi_target, rc_fapi);
    }
    else
    {
        /*
         * This needs some explanation. Schlimme Dinge ahead!
         *
         * Even if call_hwp is implemented this block is always being compiled, and if
         * the HWP has a different signature than target-only the compilation of
         * FAPI_EXEC_HWP below would fail even though the block would be optimized away.
         *
         * So we have to trick the compiler into assuming the HWP is always target-only
         * by overriding the ..._FP_t type. Of course now that opens us up to awful runtime
         * bugs, so we have to check the type by hand and emit a runtime error instead of
         * accidentally calling the HWP using an invalid prototype.
         */
        typedef fapi2::ReturnCode (*_test_fp_t)(const fapi2::Target<FAPI_TARGET_TYPE>&);

        if (!std::is_same<_test_fp_t, PROCEDURE_FP_T>::value)
        {
            output(OUTPUT_PREFIX "BUG: Attempting to call a HWP with more than only a "
                   "target parameter. Please implement call_hwp() in your wrapper.");
            return ECMD_FUNCTION_NOT_SUPPORTED;
        }

        // Now we can safely override the HWP type
        typedef fapi2::ReturnCode (*PROCEDURE_FP_T)(const fapi2::Target<FAPI_TARGET_TYPE>&);
        FAPI_EXEC_HWP(rc_fapi, PROCEDURE_NAME, fapi_target);
    }

    uint64_t rc = (uint64_t)rc_fapi;

    if (rc)
    {
        output(OUTPUT_PREFIX "Error calling procedure " PROCEDURE_NAME_STR ".");
    }

    return rc;
}

void _output(const char* msg, ...)
{
    char str[1024];
    va_list args;
    va_start(args, msg);
    vsnprintf(str, sizeof(str), msg, args);
    va_end(args);
    ecmdOutput(str);
}

void help()
{
    output("\nThis is the help text for the procedure " PROCEDURE_NAME_STR ": \n");
    output("Usage: " PROCEDURE_NAME_STR "_wrap [-h] [-k#] [-n#] [-s#] [-p#] [-quiet] [-verif]");
    output("  Option flags are:");
    output("      -h       Display this help message.");
    output("      -k#      Specify which cage to act on (default = all).");
    output("      -n#      Specify which node to act on (default = all).");
    output("      -s#      Specify which slot to act on (default = all).");
    output("      -p#      Specify which chip position to act on (default = all).");
    output("      -c#      Specify which chip unit position to act on (default = all).");
    output("      -quiet   Suppress printing of eCMD DLL/procedure informational messages (default = false).");
    output("      -verif   Run procedure in sim verification mode (default = false).");

    // extra_help is a weak symbol and will be NULL if nobody bothered to implement it
    if (extra_help)
    {
        extra_help();
    }
}

int main(int argc, char* argv[])
{
    extern bool GLOBAL_SIM_MODE;
    extern bool GLOBAL_VERIF_MODE;
    uint64_t rc = ECMD_SUCCESS;
    ecmdDllInfo DLLINFO;
    ecmdChipTarget target;
    ecmdLooperData looper;
    bool target_found = false;
    //
    // ------------------------------------
    // Load and initialize the eCMD Dll
    // If left NULL, which DLL to load is determined by the ECMD_DLL_FILE environment variable
    // If set to a specific value, the specified dll will be loaded
    // ------------------------------------
    rc = ecmdLoadDll("");

    if (rc)
    {
        output(OUTPUT_PREFIX "Error calling ecmdLoadDll.");
        return rc;
    }

    //
    // This is needed if you're running a FAPI procedure from this eCMD procedure
    TRY(fapi2InitExtension(), "Error calling fapi2InitExtension.");

    //
    // establish if this is a simulation run or not
    //
    // NEEDED?????
    TRY(ecmdQueryDllInfo(DLLINFO), "Error calling ecmdQueryDllInfo.");

    if (DLLINFO.dllEnv == ECMD_DLL_ENV_SIM)
    {
        GLOBAL_SIM_MODE = true;
    }

    // Parse out user options (excluding -pX, -cX, -coe, -debug, etc
    if (ecmdParseOption(&argc, &argv, "-h"))
    {
        help();
        rc = 1;
        goto out;
    }

    // run procedure in sim verification mode
    if (ecmdParseOption(&argc, &argv, "-verif"))
    {
        GLOBAL_VERIF_MODE = true;
    }

    // parse_extra_args is a weak symbol and will be NULL if nobody bothered to implement it
    if (parse_extra_args)
    {
        TRY_NOMSG(parse_extra_args(&argc, &argv));
    }

    //
    // -------------------------------------------------------------------------------------------------
    // Parse out common eCMD args like -p0, -c0, -coe, etc..
    // Any found args will be removed from arg list upon return
    // -------------------------------------------------------------------------------------------------
    TRY(ecmdCommandArgs(&argc, &argv), "Error calling ecmdCommandArgs.");

    // unsupported arguments left over?
    if (argc != 1)
    {
        output("Unknown/unsupported arguments specified!");
        help();
        rc = ECMD_INVALID_ARGS;
        goto out;
    }

    //
    // always print the dll info to the screen, unless in quiet mode
    if (!ecmdGetGlobalVar(ECMD_GLOBALVAR_QUIETMODE))
    {
        TRY(ecmdDisplayDllInfo(), "Error calling ecmdDisplayDllInfo.");
    }

    target.chipType      = CHIP_TYPE;
    target.chipTypeState = ECMD_TARGET_FIELD_VALID;
    target.cageState     = ECMD_TARGET_FIELD_WILDCARD;
    target.nodeState     = ECMD_TARGET_FIELD_WILDCARD;
    target.slotState     = ECMD_TARGET_FIELD_WILDCARD;
    target.threadState   = ECMD_TARGET_FIELD_UNUSED;

    if (CHIP_UNIT_TYPE == NULL)
    {
        target.chipUnitTypeState  = ECMD_TARGET_FIELD_UNUSED;
        target.chipUnitNumState   = ECMD_TARGET_FIELD_UNUSED;
    }
    else
    {
        target.chipUnitType       = CHIP_UNIT_TYPE;
        target.chipUnitTypeState  = ECMD_TARGET_FIELD_VALID;
        target.chipUnitNumState   = ECMD_TARGET_FIELD_WILDCARD;
    }

    TRY(ecmdConfigLooperInit(target, ECMD_SELECTED_TARGETS_LOOP_DEFALL, looper),
        "Error calling ecmdConfigLooperInit.");

    // loop over specified configured positions
    while (ecmdConfigLooperNext(target, looper))
    {
        target_found = true;

        // loop is a weak symbol and will be NULL if nobody bothered to implement it
        if (loop)
        {
            TRY_NOMSG(loop(target));
        }
        else
        {
            TRY_NOMSG(default_loop(target));
        }
    }

    TRY(!target_found, "No targets found, procedure did not run!");
    output(PROCEDURE_NAME_STR " is done");

out:
    // Unload the eCMD Dll, this should always be the last thing you do
    ecmdUnloadDll();
    return rc;
}
