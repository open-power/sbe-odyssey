/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/memory/explorer/hwp/wrapper/exp_fw_log_data_wrap.C $ */
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

/// @file exp_fw_log_data_wrap.C
/// @brief Wrapper for reading Explorer FW logs
///
/// *HWP HWP Owner: Louis Stermole <stermole@us.ibm.com>
/// *HWP HWP Backup: Stephen Glancy <sglancy@us.ibm.com>
/// *HWP Team: Memory
/// *HWP Level: 2
/// *HWP Consumed by: HB

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------
#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>
#include <fstream>
#include <prcdUtils.H>
#include <croClientCapi.H>
#include <ecmdClientCapi.H>
#include <ecmdDataBuffer.H>
#include <ecmdUtils.H>
#include <ecmdSharedUtils.H>
#include <fapi2.H>
#include <fapi2ClientCapi.H>
#include <fapi2SharedUtils.H>
#include <exp_fw_log_data.H>
#include <generic/memory/mss_git_data.H>

///
/// @brief Operation modes
///
enum read_mode
{
    ACTIVE,
    SAVED,
    RAM,
    I2C,
    CLR_ACTIVE,
    CLR_SAVED,
};

//------------------------------------------------------------------------------
// Function definitions
//------------------------------------------------------------------------------

///
/// @brief Display help message
///
static const std::string PROCEDURE = "exp_fw_log_data_wrap";
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
    ecmdOutput("        [-h] [-k#] [-n#] [-s#] [-p#] [-d] [-quiet] [-verif] [-mode <mode>]\n");
    ecmdOutput("        [-image #] [-offset #] [-size #] [-start #] [-hexdump]\n");
    ecmdOutput("Options:\n");
    ecmdOutput("      -h                           Display this help message.\n");

    ecmdOutput("      -k#                          Specify which cage to act on (default = 0).\n");
    ecmdOutput("      -n#                          Specify which node to act on (default = 0).\n");
    ecmdOutput("      -s#                          Specify which slot to act on (default = 0).\n");
    ecmdOutput("      -p#                          Specify which chip position to act on (default = 0).\n");
    ecmdOutput("      -quiet                       Suppress printing of eCMD DLL/procedure information (default = false).\n");
    ecmdOutput("      -verif                       Run procedure in sim verification mode (default = false).\n");

    ecmdOutput("      -m [mode]                    Specify the mode to read the logs.\n");
    ecmdOutput("                 mode values:\n");
    ecmdOutput("                       active      Read the active log using inband commands (default)\n");
    ecmdOutput("                       saved       Read the saved log using inband commands\n");
    ecmdOutput("                       ram         Read the active log directly from RAM\n");
    ecmdOutput("                       i2c         Read the active log using i2c (sideband) commands.\n");
    ecmdOutput("                       clrActive   Clear active log using inband commands\n");
    ecmdOutput("                       clrSaved    Clear saved log using inband commands\n");
    ecmdOutput("      -image 0/1                   Which image to read from (0 = image A, 1 = image B\n");
    ecmdOutput("      -offset <value>              Offset in image to start grabbing data (0-0x03FFFF)\n");
    ecmdOutput("      -size <value>                Amount of log data to gather (0-0x40000)\n");
    ecmdOutput("      -start <value>               Start address in RAM to start reading in -ram mode (default = 0x0101C520)\n");
    ecmdOutput("      -hexdump                     Data outputted in hexdump format\n");
    ecmdOutput("\n");
    ecmdOutput("NOTE: For -ram mode, -start needs to be set specific to the Explorer FW version.\n");
    ecmdOutput("      These are the known start addresses for different FW versions:\n");
    ecmdOutput("      CL397559 : 0101C520\n");
    ecmdOutput("      CL407134 : 0101CDE0\n");
    ecmdOutput("Example\n");
    snprintf(outstr, sizeof(outstr), "  %s\n", PROCEDURE.c_str());
    ecmdOutput(outstr);

    return;
}

///
/// @brief Converts log data into human-readable string data
///
/// @param[in/out] io_outss  - string stream for output string data
/// @param[in]     i_data    - explorer log data
/// @param[in]     i_raw_hex - output like hexdump
/// @param[in]     i_log_offset - offset in explorer log where data starts
/// @return int return value
///
void logToString(std::stringstream& io_outss, std::vector<uint8_t>& i_data, const bool i_raw_hex,
                 const uint32_t i_log_offset)
{
    if (i_raw_hex)
    {
        unsigned short l_byte_offset = 0;
        std::stringstream l_right_data;

        for (const auto l_byte : i_data)
        {
            // 16 hex characters per line
            if ((l_byte_offset % 0x10) == 0)
            {
                // already outputted lefthand side of data
                if (l_byte_offset)
                {
                    io_outss << "     |" << l_right_data.str();  // add right hand data
                    l_right_data.str(""); // clear right data
                    io_outss << "|\n";
                }

                io_outss << std::setfill('0') << std::setw(8) << std::hex << static_cast<unsigned int>
                         (l_byte_offset + i_log_offset) << "  ";
            }

            l_byte_offset++;
            io_outss << std::uppercase << std::setfill('0') << std::setw(2) << std::hex << static_cast<unsigned short>
                     (l_byte) << " ";

            if (isprint(l_byte))
            {
                l_right_data << l_byte;
            }
            else
            {
                l_right_data << " ";
            }
        }

        // add last righthand data if exists
        if (!l_right_data.str().empty())
        {
            io_outss << "     |" << l_right_data.str() << "|\n";
        }
    }
    else
    {
        uint8_t   l_same_byte = 0;
        uint16_t l_same_byte_cnt = 0;

        for (const auto l_byte : i_data)
        {
            // if previous character was non-printable
            if (l_same_byte_cnt)
            {
                if (l_byte == l_same_byte)
                {
                    // non-printable character repeated
                    l_same_byte_cnt++;
                    continue;
                }
                else
                {
                    // different character, write out how many matches before this one
                    io_outss << "[repeats for 0x" << l_same_byte_cnt << " bytes] ";
                    l_same_byte_cnt = 0;
                }
            }

            if (isprint(l_byte) || l_byte == '\n' )
            {
                // printable character or LF
                io_outss << l_byte;
            }
            else
            {
                // non-printable character, print in hex if not already printed
                if (l_same_byte_cnt == 0)
                {
                    io_outss << "[0x" << std::uppercase << std::setfill('0') << std::setw(2)
                             << std::hex << static_cast<unsigned short>(l_byte) << "] ";
                    l_same_byte = l_byte;
                    l_same_byte_cnt++;
                }
            }
        }

        // add the byte count, if necessary
        if (l_same_byte_cnt > 1)
        {
            io_outss << "[repeats for 0x" << l_same_byte_cnt << " bytes] ";
        }
    }
}

///
/// @brief Main function
///
/// @param[in] argc input argument count
/// @param[in] argv input arguments
/// @return int return value
///
int main(int argc, char* argv[])
{
    constexpr uint32_t MAX_TRANSACTION_SIZE = 0x10000; // 64KB
    constexpr uint32_t MAX_OVERALL_SIZE = 0x040000;    // 256KB

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
    read_mode l_read_mode = ACTIVE;
    char* l_argdata = NULL;
    uint8_t l_image = 0;
    uint32_t l_offset = 0;
    uint32_t l_size = 0x1000; // 4K default size
    bool l_hexdump_data = false;

    // This is the starting address to read the FW log buffer from Explorer RAM
    uint32_t l_start_addr = CL397559_BUFFER_START;

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

        l_argdata = ecmdParseOptionWithArgs(&argc, &argv, "-m");

        if (l_argdata != NULL)
        {
            std::string l_modestr(l_argdata);

            if (l_modestr.compare("active") == 0)
            {
                l_read_mode = ACTIVE;
            }
            else if (l_modestr.compare("saved") == 0)
            {
                l_read_mode = SAVED;
            }
            else if (l_modestr.compare("ram") == 0)
            {
                l_read_mode = RAM;
            }
            else if (l_modestr.compare("i2c") == 0)
            {
                l_read_mode = I2C;
            }
            else if (l_modestr.compare("clrActive") == 0)
            {
                l_read_mode = CLR_ACTIVE;
            }
            else if (l_modestr.compare("clrSaved") == 0)
            {
                l_read_mode = CLR_SAVED;
            }
            else
            {
                l_read_mode = ACTIVE;
            }
        }

        if (l_read_mode == RAM)
        {
            // Since we're reading from RAM, default this to the known size of the
            // circular buffer, but allow for override using -size
            l_size = BUFFER_SIZE;
        }

        // image number to read
        l_argdata = ecmdParseOptionWithArgs(&argc, &argv, "-image");

        if (l_argdata != NULL)
        {
            l_image = strtoull(l_argdata, NULL, 16);

            if ((l_image != 0) && (l_image != 1))
            {
                printf("ERROR: image needs to be 0 or 1\n");
                help();
                rc = 1;
                break;
            }
        }
        else if (l_read_mode == SAVED)
        {
            printf("ERROR: image must be specified with -mode saved\n");
            help();
            rc = 1;
            break;
        }

        // offset to read from
        l_argdata = ecmdParseOptionWithArgs(&argc, &argv, "-offset");

        if (l_argdata != NULL)
        {
            l_offset = strtoull(l_argdata, NULL, 16);

            if (l_offset >= MAX_OVERALL_SIZE)
            {
                printf("ERROR: offset must be in range 0 - 0x%.8X\n", MAX_OVERALL_SIZE - 1);
                help();
                rc = 1;
                break;
            }
        }
        else if (l_read_mode == SAVED)
        {
            printf("ERROR: offset must be specified with -mode saved\n");
            help();
            rc = 1;
            break;
        }

        // size of data to read
        l_argdata = ecmdParseOptionWithArgs(&argc, &argv, "-size");

        if (l_argdata != NULL)
        {
            l_size = strtoull(l_argdata, NULL, 16);

            if (l_size > MAX_OVERALL_SIZE)
            {
                printf("ERROR: size must be in range 0 - 0x%.8X\n", MAX_OVERALL_SIZE);
                help();
                rc = 1;
                break;
            }
        }
        else if (l_read_mode == SAVED)
        {
            printf("ERROR: size must be specified with -mode saved\n");
            help();
            rc = 1;
            break;
        }

        // start address in RAM to read
        l_argdata = ecmdParseOptionWithArgs(&argc, &argv, "-start");

        if (l_argdata != NULL)
        {
            l_start_addr = strtoull(l_argdata, NULL, 16);
        }

        // run procedure in sim verification mode
        if (ecmdParseOption(&argc, &argv, "-verif"))
        {
            GLOBAL_VERIF_MODE = true;
        }

        // raw output in hex
        if (ecmdParseOption(&argc, &argv, "-hexdump"))
        {
            l_hexdump_data = true;
        }

        // parse out common eCMD args like -p0, -c0, -coe, etc..
        // any found args will be removed from arg list upon return
        rc = ecmdCommandArgs(&argc, &argv);

        if (rc)
        {
            ecmdOutputError("Error parsing eCMD arguments\n");
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
        node_target.chipUnitNumState = ECMD_TARGET_FIELD_UNUSED;
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
            l_ocmb_target.chipUnitNumState = ECMD_TARGET_FIELD_UNUSED;
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

                std::vector<uint8_t> l_data;

                if (l_read_mode == CLR_ACTIVE)
                {
                    snprintf(outstr, sizeof(outstr), "Clearing active log on %s\n",
                             ecmdWriteTarget(l_ocmb_target).c_str() );
                    ecmdOutput(outstr);

                    rc = static_cast<uint32_t>(exp_clear_active_log(fapi_target));

                    if (rc)
                    {
                        snprintf(outstr, sizeof(outstr), "ERROR: %s FAPI call exited with bad return code = %s 0x%08x\n",
                                 PROCEDURE.c_str(),
                                 ecmdParseReturnCode(rc).c_str(), rc);
                        ecmdOutputError(outstr);
                    }
                }
                else if (l_read_mode == CLR_SAVED)
                {
                    snprintf(outstr, sizeof(outstr), "Clearing saved log on %s\n",
                             ecmdWriteTarget(l_ocmb_target).c_str() );
                    ecmdOutput(outstr);

                    rc = static_cast<uint32_t>(exp_clear_saved_log(fapi_target));

                    if (rc)
                    {
                        snprintf(outstr, sizeof(outstr), "ERROR: %s FAPI call exited with bad return code = %s 0x%08x\n",
                                 PROCEDURE.c_str(),
                                 ecmdParseReturnCode(rc).c_str(), rc);
                        ecmdOutputError(outstr);
                    }
                }
                else if (l_read_mode == RAM)
                {
                    snprintf(outstr, sizeof(outstr), "Reading active logs directly from RAM on %s, size %d, starting at 0x%08x\n",
                             ecmdWriteTarget(l_ocmb_target).c_str(), l_size, l_start_addr);
                    ecmdOutput(outstr);

                    l_data.resize(l_size, 0);
                    rc = static_cast<uint32_t>(exp_active_log_from_ram(fapi_target, l_start_addr, l_data));

                    if (rc)
                    {
                        snprintf(outstr, sizeof(outstr), "ERROR: %s FAPI call exited with bad return code = %s 0x%08x\n",
                                 PROCEDURE.c_str(),
                                 ecmdParseReturnCode(rc).c_str(), rc);
                        ecmdOutputError(outstr);
                        break;
                    }

                    std::stringstream l_outss;
                    l_outss << ecmdWriteTarget(node_target).c_str();

                    if (l_hexdump_data)
                    {
                        l_outss << " active log data (hex):\n";
                        logToString(l_outss, l_data, true, 0);
                    }
                    else
                    {
                        l_outss << " active log data:\n";
                        l_outss << std::hex << std::setw(2) << std::setfill('0');
                        logToString(l_outss, l_data, false, 0);
                    }

                    l_outss << "\n";
                    ecmdOutput(l_outss.str().c_str());
                }
                else if (l_read_mode == SAVED)
                {
                    snprintf(outstr, sizeof(outstr), "Reading saved log %s on image %s, starting offset: %d, size %d\n",
                             ecmdWriteTarget(l_ocmb_target).c_str(), l_image ? "B" : "A", l_offset, l_size);
                    ecmdOutput(outstr);

                    // use tmp variables so size and offset stay same for each OCMB target
                    uint32_t l_tmp_size = l_size;
                    uint32_t l_tmp_offset = l_offset;

                    while (l_tmp_size > 0)
                    {
                        uint32_t l_transaction_size = (l_tmp_size > MAX_TRANSACTION_SIZE) ? MAX_TRANSACTION_SIZE : l_tmp_size;

                        l_data.resize(l_transaction_size, 0);
                        rc = static_cast<uint32_t>(exp_saved_log(static_cast<mss::exp::ib::exp_image>(l_image), l_tmp_offset, fapi_target,
                                                   l_data));

                        if (rc)
                        {
                            snprintf(outstr, sizeof(outstr), "ERROR: %s FAPI call exited with bad return code = %s 0x%08x\n",
                                     PROCEDURE.c_str(),
                                     ecmdParseReturnCode(rc).c_str(), rc);
                            ecmdOutputError(outstr);
                            break;
                        }

                        std::stringstream l_outss;
                        l_outss << ecmdWriteTarget(node_target).c_str();

                        if (l_hexdump_data)
                        {
                            l_outss << " saved log image " << (l_image ? "B" : "A") << " data (hex):\n";
                            logToString(l_outss, l_data, true, l_tmp_offset);
                        }
                        else
                        {
                            l_outss << " saved log image " << (l_image ? "B" : "A") << " data:\n";
                            l_outss << std::hex << std::setw(2) << std::setfill('0');
                            logToString(l_outss, l_data, false, l_tmp_offset);
                        }

                        l_outss << "\n";
                        ecmdOutput(l_outss.str().c_str());

                        l_tmp_offset += l_transaction_size;
                        l_tmp_size -= l_transaction_size;
                    }
                }
                else if (l_read_mode == I2C)
                {
                    snprintf(outstr, sizeof(outstr), "Reading active logs using I2C on %s\n",
                             ecmdWriteTarget(l_ocmb_target).c_str());
                    ecmdOutput(outstr);

                    rc = static_cast<uint32_t>(exp_active_log_i2c(fapi_target, l_data));

                    if (rc)
                    {
                        snprintf(outstr, sizeof(outstr), "ERROR: %s FAPI call exited with bad return code = %s 0x%08x\n",
                                 PROCEDURE.c_str(),
                                 ecmdParseReturnCode(rc).c_str(), rc);
                        ecmdOutputError(outstr);
                        break;
                    }

                    std::stringstream l_outss;
                    l_outss << ecmdWriteTarget(node_target).c_str();

                    if (l_hexdump_data)
                    {
                        l_outss << " active log data (hex):\n";
                        logToString(l_outss, l_data, true, 0);
                    }
                    else
                    {
                        l_outss << " active log data:\n";
                        l_outss << std::hex << std::setw(2) << std::setfill('0');
                        logToString(l_outss, l_data, false, 0);
                    }

                    l_outss << "\n";
                    ecmdOutput(l_outss.str().c_str());
                }
                else // l_read_mode = ACTIVE
                {
                    snprintf(outstr, sizeof(outstr), "Reading active log on %s, size %d\n",
                             ecmdWriteTarget(l_ocmb_target).c_str(), l_size);
                    ecmdOutput(outstr);

                    l_data.resize(l_size, 0);
                    rc = static_cast<uint32_t>(exp_active_log(fapi_target, l_data));

                    if (rc)
                    {
                        snprintf(outstr, sizeof(outstr), "ERROR: %s FAPI call exited with bad return code = %s 0x%08x\n",
                                 PROCEDURE.c_str(),
                                 ecmdParseReturnCode(rc).c_str(), rc);
                        ecmdOutputError(outstr);
                        break;
                    }

                    std::stringstream l_outss;
                    l_outss << ecmdWriteTarget(node_target).c_str();

                    if (l_hexdump_data)
                    {
                        l_outss << " active log data (hex):\n";
                        logToString(l_outss, l_data, true, 0);
                    }
                    else
                    {
                        l_outss << " active log data:\n";
                        l_outss << std::hex << std::setw(2) << std::setfill('0');
                        logToString(l_outss, l_data, false, 0);
                    }

                    l_outss << "\n";
                    ecmdOutput(l_outss.str().c_str());
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
