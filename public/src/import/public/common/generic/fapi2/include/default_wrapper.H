/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/common/generic/fapi2/include/default_wrapper.H $ */
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
 * @file default_wrapper.H
 * @brief Boilerplate code for procedure wrappers
 *
 * Create a FAPI2 procedure wrapper in very few lines of code!
 *
 * This header and its associate .C file contain the entire boilerplate
 * for a simple wrapper that loops over targets and calls a procedure
 * on each one. A wrapper implementation only needs to specify the
 * procedure name and target type, the code in the .C file will take
 * care of the rest via some preprocessor and linker magic.
 *
 * The trivial case of "loop over all targets and call the procedure
 * with only the target as parameter" is handled by the default code;
 * if customization is required (e.g. for procedures that take additional
 * parameters) there are three hooks for a wrapper to supply:
 *
 *  * parse_extra_args(argc, argv) for parsing additional cmdline arguments
 *  * extra_help() for printing help text for your additional arguments
 *  * loop(target) to override what happens to each target inside the loop
 *
 * We also provide a few convenience functions here to save typing on
 * repeating tasks, see below.
 *
 * @author Joachim Fenkes <fenkes@de.ibm.com>
 */

/*
 * Here's a template for a simple wrapper - this is all you need!

// First include the boilerplate header
#include <default_wrapper.H>

// Then include your procedure header
#include <p10_pre_poweron.H>

// Define the procedure name
#define PROCEDURE_NAME p10_pre_poweron

// Define the target type. Leave CHIP_UNIT_TYPE at NULL if you want to
// iterate over entire chips, specify a unit type (like "perv" or "c")
// if you want to iterate over units inside chips too.
const char *CHIP_TYPE = "pu";
const char *CHIP_UNIT_TYPE = NULL;

// The FAPI target type; must match the procedure's parameter type.
const fapi2::TargetType FAPI_TARGET_TYPE = fapi2::TARGET_TYPE_PROC_CHIP;

// Last, include the main boilerplate code
#include <default_wrapper.C>

*/

/*
 * And here's a template for a more complicated wrapper, in case you need
 * to customize parameters or procedure invocation.

// First the usual stuff
#include <default_wrapper.H>

#include <p10_start_cbs.H>

#define PROCEDURE_NAME p10_start_cbs
const char* CHIP_TYPE = "pu";
const char* CHIP_UNIT_TYPE = NULL;
const fapi2::TargetType FAPI_TARGET_TYPE = fapi2::TARGET_TYPE_PROC_CHIP;

// Now, if you want your wrapper to understand extra arguments,
// implement extra_help() and parse_extra_args().
void extra_help()
{
    output("      -i_sbe_start      sets input for boolean procedure argument i_sbe_start to true (default is false).\n");
}

// Parsed arguments go into global variables
bool g_sbe_start = false;

uint64_t parse_extra_args(int* argc, char** argv[])
{
    g_sbe_start = ecmdParseOption(argc, argv, "-i_sbe_start");
    return ECMD_SUCCESS;
}

// If your HwP takes different parameters than just the FAPI target,
// you have to implement the call_hwp() function and call FAPI_EXEC_HWP()
// as needed.
void call_hwp(fapi2::Target<FAPI_TARGET_TYPE> i_fapi_target, fapi2::ReturnCode& o_rc)
{
    FAPI_EXEC_HWP(o_rc, p10_start_cbs, i_fapi_target, g_sbe_start);
}

// Last of all, again include the rest of the boilerplate.
#include <default_wrapper.C>
*/

// Includes
#include <fapi2.H>
#include <fapi2ClientCapi.H>
#include <ecmdClientCapi.H>
#include <ecmdSharedUtils.H>
#include <ecmdUtils.H>
#include <stdio.h>
#include <stdarg.h>

/**
 * @brief Implement this function if you have extra command line parameters
 *        you'd like to handle.
 *
 * You can use the standard ecmd functions like ecmdParseOption(argc, argv)
 * in here. Store your parameters in global variables so your custom loop
 * can access them.
 *
 * If after this there are any arguments left, the wrapper will abort.
 *
 * @param[in] argc, argv Command line arguments left over after standard
 *                       arguments and eCmd arguments have been parsed.
 *
 * @return ECMD_SUCCESS if arguments could be parsed, anything else to
 *         abort the wrapper.
 */
uint64_t parse_extra_args(int* argc, char** argv[]) __attribute__((weak));

/**
 * @brief Implement this function to display your extra parameter help
 *        in the wrapper's help output. It will be called after the
 *        default help text and should use ecmdOutput() or output()
 *        to print your extra help text.
 */
void extra_help() __attribute__((weak));

/**
 * @brief Implement this function if you'd like to customize the way
 *        your procedure gets called.
 *
 * If this function is defined, it will get called by the default loop
 * implementation (see below) when it's time to call the HWP.
 * The default loop will take care of creating the FAPI target and handling
 * the return code; your implementation will usually just consist of a
 * single FAPI_EXEC_HWP(...) line.
 *
 * @param[in]  i_fapi_target The FAPI target to run the HWP on
 * @param[out] o_rc          The FAPI return code from the procedure
 */
#if 0
    // FAPI_TARGET_TYPE is not defined yet at this point yet so the function prototype
    // will actually be declared in default_wrapper.C - just copying it here for documentation.
    void call_hwp(fapi2::Target<FAPI_TARGET_TYPE> i_fapi_target, fapi2::ReturnCode& o_rc) __attribute__((weak));
#endif

/**
 * @brief Implement this function if you need to customize the way your
 *        procedure is being called, including FAPI target creation and
 *        return code handling. This will replace the default implementation
 *        provided by default_wrapper.C.
 *        It will be called exactly once for each target in the
 *        targeting loop. It needs to take care of translating the
 *        target into a FAPI target and calling the procedure.
 *        If this returns anything other than ECMD_SUCCESS the loop
 *        will be aborted.
 *
 * @param[in] i_target The eCmd target for the current loop iteration
 *
 * @return ECMD_SUCCESS if successful, anything else will abort the wrapper
 */
uint64_t loop(ecmdChipTarget& i_target) __attribute__((weak));

/**
 * @brief Similar to FAPI_TRY(), will jump to a label "out" on failure
 *        and expects an "rc" variable in scope.
 */
#define TRY(stuff, msg, args...) {              \
        rc = stuff;                                 \
        if (rc != ECMD_SUCCESS)                     \
        {                                           \
            output("%s_wrap: " msg, procedureName, ##args); \
            goto out;                               \
        }                                           \
    }
extern const char* procedureName; // generated further down in default_wrapper.C

/**
 * @brief Similar to FAPI_TRY(), will jump to a label "out" on failure
 *        and expects an "rc" variable in scope.
 */
#define TRY_NOMSG(stuff) {                      \
        rc = stuff;                                 \
        if (rc != ECMD_SUCCESS)                     \
        {                                           \
            goto out;                               \
        }                                           \
    }

/**
 * @brief Calls ecmdOutput() but applies string formatting and adds a newline
 */
#define output(msg, args...) _output(msg "\n", ##args)
void _output(const char* msg, ...);
