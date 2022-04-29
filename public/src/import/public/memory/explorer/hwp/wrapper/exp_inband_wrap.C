/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/memory/explorer/hwp/wrapper/exp_inband_wrap.C $ */
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

/// @file exp_inband_wrap.C
/// @brief Wrapper for exp_inband
//
// *HWP HWP Owner: Ben Gass bgass@us.ibm.com
// *HWP FW Owner: Daniel Crowell dcrowell@us.ibm.com
// *HWP Team: Nest
// *HWP Level: 2
// *HWP Consumed by:


//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------
#include <croClientCapi.H>
#include <ecmdClientCapi.H>
#include <ecmdDataBuffer.H>
#include <ecmdUtils.H>
#include <ecmdSharedUtils.H>
#include <fapi2.H>
#include <fapi2ClientCapi.H>
#include <fapi2SharedUtils.H>
#include <cstdlib>
#include <lib/inband/exp_inband.H>
#include <explorer_scom_addresses_fixes.H>
#include <explorer_scom_addresses_fld_fixes.H>

enum ib_mode
{
    MMIO32,
    MMIO64,
    CFG,
    SCOM,
    CMD,
    RSP,
    SENSOR
};

// Not aware we needs these in a common header file for resuse
constexpr uint64_t OPSW_PCBI_GENERAL_PURPOSE_OUTPUT_REG = 0x03000084ull;
constexpr uint8_t OPSW_PCBI_GPO = 63;

void help()
{
    ecmdOutput("Help for exp_inband_wrap\n");
    ecmdOutput("\n");
    ecmdOutput("Parameters:\n");
    ecmdOutput("      <address>       Target real address (right-justified hex).\n");
    ecmdOutput("                      The address type should match the mode set by -m\n");
    ecmdOutput("\n");
    ecmdOutput("Options:\n");
    ecmdOutput("     -h               Print this\n");
    ecmdOutput("     -m [mode]        Sets the type of address used.\n");
    ecmdOutput("                      mode values:\n");
    ecmdOutput("                           mmio32  :   Generic 32 bit mmio to MSCC space\n");
    ecmdOutput("                           mmio64  :   Generic 64 bit mmio to MSCC space\n");
    ecmdOutput("                           scom    :   IBM Scom address\n");
    ecmdOutput("                           cfg     :   OpenCAPI configuration address\n");
    ecmdOutput("                           cmd     :   Write-Only MSCC command structure\n");
    ecmdOutput("                           rsp     :   Read-Only MSCC response structure\n");
    ecmdOutput("                           sensor  :   Read-Only Sensor Cache structure\n");
    ecmdOutput("\n");
    ecmdOutput("     -w [data]        Specify a write operation.  Valid values are determined by mode\n");
    ecmdOutput("                           mmio32  :   4 bytes of hex data\n");
    ecmdOutput("                           mmio64  :   8 bytes of hex data\n");
    ecmdOutput("                           scom    :   8 bytes of hex data\n");
    ecmdOutput("                           cfg     :   4 bytes of hex data\n");
    ecmdOutput("                           cmd     :   comma separated command\n");
    ecmdOutput("\n");
    ecmdOutput("        Command Header                                                                                                      \n");
    ecmdOutput("        uint8_t  cmd_id;                  // Command type                                                                   \n");
    ecmdOutput("        uint8_t  cmd_flags;               // Various flags associated with the command                                      \n");
    ecmdOutput("        uint16_t request_identifier;      // The request identifier of this transport request                               \n");
    ecmdOutput("        uint32_t cmd_length;              // Number of bytes following the UI header                                        \n");
    ecmdOutput("        uint32_t cmd_crc;                 // CRC of command data buffer, if used                                            \n");
    ecmdOutput("        uint32_t host_work_area;          // Scratchpad area for Host, FW returns this value as a reponse                   \n");
    ecmdOutput("        uint32_t cmd_work_area;           // Scratchpad area for Firmware, can be used for tracking command progress etc.   \n");
    ecmdOutput("        uint32_t  padding[3];             // Fill up to the size of one cache line                                          \n");
    ecmdOutput("        uint8_t  command_argument[28];    // Additional parameters associated with the command                              \n");
    ecmdOutput("        uint32_t cmd_header_crc;          // CRC of 63 bytes of command header                                              \n");
    ecmdOutput("\n");
    ecmdOutput("  Example for -m cmd -w \n");
    ecmdOutput("id,fg,idnt,cmd_len, crc,     hst_wrk, cmd_wrk, pad[0],  pad[1],  pad[2],  pad[3]\n");
    ecmdOutput("01,01,0123,01234567,01234567,01234567,01234567,01234567,01234567,01234567,01234567,\\\n");
    ecmdOutput("00,01,02,03,04,05,06,07,08,09,0A,0B,0C,0D,0E,0F,10,11,12,13,14,15,16,17,18,19,1A,1B,01234567\n");
    ecmdOutput("cmd_arg[0..27],                                                                     crc \n");
}


// main function
int
main(int argc, char* argv[])
{
    // from prcdUtils
    extern bool GLOBAL_SIM_MODE;
    extern bool GLOBAL_VERIF_MODE;

    // flow/control variables
    uint32_t rc = ECMD_SUCCESS;
    ecmdDllInfo DLLINFO;
    ecmdLooperData looper;
    ecmdChipTarget target;
    bool valid_pos_found = false;
    uint64_t base_address = 0x0ull;
    char* argdata;
    std::vector<uint8_t> data;
    fapi2::ReturnCode rc_fapi(fapi2::FAPI2_RC_SUCCESS);

    do
    {
        // load and initialize the eCMD Dll
        // if left NULL, which DLL to load is determined by the ECMD_DLL_FILE
        // environment variable if set to a specific value, the specified DLL
        // will be loaded
        rc = ecmdLoadDll("");

        if (rc)
        {
            ecmdOutputError("Error initializing eCMD DLL!\n");
            return rc;
        }

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

        // run procedure in sim verification mode
        if (ecmdParseOption(&argc, &argv, "-verif"))
        {
            GLOBAL_VERIF_MODE = true;
        }

        ib_mode mode = MMIO64;
        argdata = ecmdParseOptionWithArgs(&argc, &argv, "-m");

        if (argdata != NULL)
        {
            std::string mstr(argdata);

            if (mstr == "mmio32")
            {
                mode = MMIO32;
            }

            if (mstr == "mmio64")
            {
                mode = MMIO64;
            }

            if (mstr == "cfg")
            {
                mode = CFG;
            }

            if (mstr == "scom")
            {
                mode = SCOM;
            }

            if (mstr == "cmd")
            {
                mode = CMD;
            }

            if (mstr == "rsp")
            {
                mode = RSP;
            }

            if (mstr == "sensor")
            {
                mode = SENSOR;
            }
        }

        // show help message
        if (ecmdParseOption(&argc, &argv, "-h"))
        {
            help();
            break;
        }

        bool rnw = true;
        fapi2::buffer<uint32_t> m32_data = 0;
        fapi2::buffer<uint64_t> m64_data = 0;
        host_fw_command_struct cmd_data;
        host_fw_response_struct rsp_data;
        sensor_cache_struct sensor_data;
        argdata = ecmdParseOptionWithArgs(&argc, &argv, "-w");

        if (argdata != NULL)
        {
            rnw = false;

            switch(mode)
            {
                case CMD:
                    {
                        std::vector<char*> fields;
                        fields.push_back(argdata);
                        auto l_origlen = strlen(argdata);

                        for (uint32_t l_ii = 0; l_ii < l_origlen; l_ii++)
                        {
                            if (argdata[l_ii] == ',')
                            {
                                argdata[l_ii] = '\0';
                                printf("d: %s\n", fields[static_cast<uint32_t>(fields.size()) - 1]);
                                fields.push_back(&argdata[l_ii + 1]);
                            }
                        }

                        if (fields.size() != 40)
                        {
                            printf("Yikes..  You need exactly 40 values for a command. %d\n", static_cast<uint32_t>(fields.size()));
                            help();
                            rc = 1;
                            break;
                        }

                        int l_i = 0;
                        cmd_data.cmd_id = strtoull(fields[l_i], NULL, 16);
                        l_i++;              //uint8_t  cmd_id;                  // Command type
                        cmd_data.cmd_flags = strtoull(fields[l_i], NULL, 16);
                        l_i++;           //uint8_t  cmd_flags;               // Various flags associated with the command
                        cmd_data.request_identifier = strtoull(fields[l_i], NULL, 16);
                        l_i++;  //uint16_t request_identifier;      // The request identifier of this transport request
                        cmd_data.cmd_length = strtoull(fields[l_i], NULL, 16);
                        l_i++;          //uint32_t cmd_length;              // Number of bytes following the UI header
                        cmd_data.cmd_crc = strtoull(fields[l_i], NULL, 16);
                        l_i++;             //uint32_t cmd_crc;                 // CRC of command data buffer, if used
                        cmd_data.host_work_area = strtoull(fields[l_i], NULL, 16);
                        l_i++;      //uint32_t host_work_area;          // Scratchpad area for Host, FW returns this value as a reponse
                        cmd_data.cmd_work_area = strtoull(fields[l_i], NULL, 16);
                        l_i++;       //uint32_t cmd_work_area;           // Scratchpad area for Firmware, can be used for tracking command progress etc.

                        for (int l_ii = 0; l_ii < CMD_PADDING_SIZE; l_ii++)
                        {
                            cmd_data.padding[l_ii] = strtoull(fields[l_i], NULL, 16);
                            l_i++;   //uint32_t padding[3];              // Fill up to the size of one cache line
                        }

                        for (int l_ii = 0; l_ii < 28; l_ii++)
                        {
                            cmd_data.command_argument[l_ii] = strtoull(fields[l_i], NULL, 16);
                            l_i++;//uint8_t  command_argument[28];    // Additional parameters associated with the command
                        }

                        cmd_data.cmd_header_crc = strtoull(fields[l_i], NULL, 16);
                        l_i++;      //uint32_t cmd_header_crc;          // CRC of 63 bytes of command header
                    }
                    break;

                case RSP:
                    printf("You cannot write a response.\n");
                    help();
                    rc = 1;
                    break;

                case SENSOR:
                    printf("You cannot write to the sensor cache.\n");
                    help();
                    rc = 1;
                    break;

                case CFG:
                case MMIO32:
                    m32_data = strtoull(argdata, NULL, 16);
                    break;

                case SCOM:
                case MMIO64:
                default:
                    m64_data = strtoull(argdata, NULL, 16);
                    break;
            }


        }

        if (rc != 0)
        {
            break;
        }

        // parse out common eCMD args like -p0, -c0, -coe, etc..
        // any found args will be removed from arg list upon return
        rc = ecmdCommandArgs(&argc, &argv);

        if (rc)
        {
            ecmdOutputError("Error parsing eCMD arguments\n");
            help();
            break;
        }

        // CMD, SENSOR, and RSP don't take an address, so don't
        // require one on the command line
        if (mode != CMD && mode != SENSOR && mode != RSP)
        {
            if (argc < 2)
            {
                ecmdOutputError("You need to specify an address\n");
                help();
                rc = 1;
                break;
            }

            base_address = strtoull(argv[1], NULL, 16);
        }

        // configure looper to iterate over specified configured positions
        target.chipType      = "ocmb";
        target.chipTypeState = ECMD_TARGET_FIELD_VALID;
        target.cageState     = ECMD_TARGET_FIELD_WILDCARD;
        target.nodeState     = ECMD_TARGET_FIELD_WILDCARD;
        target.slotState     = ECMD_TARGET_FIELD_WILDCARD;
        target.posState      = ECMD_TARGET_FIELD_WILDCARD;
        target.threadState   = ECMD_TARGET_FIELD_UNUSED;
        target.chipUnitTypeState = ECMD_TARGET_FIELD_UNUSED;
        target.chipUnitNumState = ECMD_TARGET_FIELD_UNUSED;

        rc = ecmdConfigLooperInit(target, ECMD_SELECTED_TARGETS_LOOP_DEFALL, looper);

        if (rc)
        {
            ecmdOutputError("Error initializing looper!\n");
            break;
        }

        // loop over specified configured positions
        while ((rc == ECMD_SUCCESS) && ecmdConfigLooperNext(target, looper))
        {
            valid_pos_found = true;
            fapi2::Target<fapi2::TARGET_TYPE_OCMB_CHIP> fapi_target(&target);

            if (rnw)
            {
                switch (mode)
                {
                    case CFG:
                        {
                            fapi2::buffer<uint32_t> l_cfg;
                            rc_fapi = mss::exp::ib::getOCCfg(fapi_target, base_address, l_cfg);
                            printf("OC CFG Data: 0x%08X\n", static_cast<uint32_t>(l_cfg));
                        }
                        break;

                    case SCOM:
                        {
                            fapi2::buffer<uint64_t> l_scom;
                            rc_fapi = mss::exp::ib::getScom(fapi_target, base_address, l_scom);
                            printf("SCOM Data:   0x%016lX\n", static_cast<uint64_t>(l_scom));
                        }
                        break;

                    case MMIO32:
                        {
                            fapi2::buffer<uint32_t> l_data;
                            rc_fapi = mss::exp::ib::getMMIO32(fapi_target, base_address, l_data);
                            printf("MMIO32 Data: 0x%08X\n", static_cast<uint32_t>(l_data));
                        }
                        break;

                    case MMIO64:
                        {
                            fapi2::buffer<uint64_t> l_data;
                            rc_fapi = mss::exp::ib::getMMIO64(fapi_target, base_address, l_data);
                            printf("MMIO64 Data: 0x%016lX\n", static_cast<uint64_t>(l_data));
                        }
                        break;

                    case RSP:
                        {
                            fapi2::buffer<uint64_t> l_scom;
                            rc_fapi = fapi2::getScom(fapi_target, EXPLR_MIPS_TO_OCMB_INTERRUPT_REGISTER1, l_scom);
                            printf("SCOM Register:  0x%016lX. Data: 0x%016lX\n",
                                   EXPLR_MIPS_TO_OCMB_INTERRUPT_REGISTER1,
                                   static_cast<uint64_t>(l_scom));

                            if( rc_fapi)
                            {
                                ecmdOutputError("Error calling getScom!\n");
                                break;
                            }

                            // If the doorbell isn't set, let's manually do it.
                            // Set the doorbell so getRSP doesn't exit due to a polling timeout
                            // before populating the response struct.  Debug workaround.
                            if(!l_scom.getBit<EXPLR_MIPS_TO_OCMB_INTERRUPT_REGISTER1_DOORBELL>())
                            {
                                printf("Manually setting doorbell register 0x%016lX.\n",
                                       OPSW_PCBI_GENERAL_PURPOSE_OUTPUT_REG);

                                // The doorbell looks for a rising edge
                                l_scom.setBit<OPSW_PCBI_GPO>();
                                rc_fapi = fapi2::putScom(fapi_target, OPSW_PCBI_GENERAL_PURPOSE_OUTPUT_REG, l_scom);

                                if( rc_fapi)
                                {
                                    ecmdOutputError("Error calling putScom!\n");
                                    break;
                                }

                                // That register gets set back to 0 to prepare for the next time MIPS wants to set the doorbell itself
                                l_scom.clearBit<OPSW_PCBI_GPO>();
                                rc_fapi = fapi2::putScom(fapi_target, OPSW_PCBI_GENERAL_PURPOSE_OUTPUT_REG, l_scom);

                                if( rc_fapi)
                                {
                                    ecmdOutputError("Error calling putScom!\n");
                                    break;
                                }
                            }

                            // As the user just requested the polling for the response, we do not know the CMD ID
                            // We need the CMD ID to pass in to getRSP, so we just pass in a 0 for unknown response ID
                            std::vector<uint8_t> l_data;
                            constexpr uint8_t CMD_ID_UNKNOWN = 0;
                            rc_fapi = mss::exp::ib::getRSP(fapi_target, CMD_ID_UNKNOWN, rsp_data, l_data);

                            if( rc_fapi)
                            {
                                ecmdOutputError("Error calling getRSP!\n");
                                break;
                            }

                            printf("response_id          : 0x%02X\n",
                                   rsp_data.response_id            );           //uint8_t  response_id;             // Response ID - same as Command ID
                            printf("response_flags       : 0x%02X\n",
                                   rsp_data.response_flags         );        //uint8_t  response_flags;          // Various flags associated with the response
                            printf("request_identifier   : 0x%04X\n",
                                   rsp_data.request_identifier     );    //uint16_t request_identifier;      // The request identifier of this transport request
                            printf("response_length      : 0x%08X\n",
                                   rsp_data.response_length        );       //uint32_t response_length;         // Number of bytes following the response header
                            printf("response_crc         : 0x%08X\n",
                                   rsp_data.response_crc           );          //uint32_t response_crc;            // CRC of response data buffer, if used
                            printf("host_work_area       : 0x%08X\n",
                                   rsp_data.host_work_area         );        //uint32_t host_work_area;          // Scratchpad area for Host, FW returns this value as a reponse

                            for (int l_i = 0; l_i < 4; l_i++)
                            {
                                printf("padding[%d]           : 0x%08X\n", l_i,
                                       rsp_data.padding[l_i] );            //uint32_t padding[4];              // Fill up to the size of one cache line
                            }

                            for (int l_i = 0; l_i < 28; l_i++)
                            {
                                printf("response_argument[%d]: 0x%02X\n", l_i,
                                       rsp_data.response_argument[l_i] ); //uint8_t  response_argument[28];   // Additional parameters associated with the response
                            }

                            printf("response_header_crc  : 0x%08X\n",
                                   rsp_data.response_header_crc    );   //uint32_t response_header_crc;     // CRC of 63 bytes of reponse header

                            // output some response data
                            if (l_data.size() > 0)
                            {
                                // set a max output to avoid a huge data dump
                                uint32_t max_data_print = 20;

                                if (max_data_print > l_data.size())
                                {
                                    max_data_print = l_data.size();
                                }

                                printf("Response buffer data (%d out of %d bytes): ",
                                       max_data_print, (int)l_data.size());

                                for (uint32_t i = 0; i < max_data_print; ++i)
                                {
                                    printf("%02X ", l_data[i]);
                                }

                                printf("\n");
                            }
                        }
                        break;

                    case SENSOR:
                        {
                            rc_fapi = mss::exp::ib::getSensorCache(fapi_target, sensor_data);
                            printf("status                 : 0x%04X\n",
                                   sensor_data.status                );              // uint16_t status;                 // Responses
                            printf("ocmb_dts               : 0x%04X\n",
                                   sensor_data.ocmb_dts              );              // uint16_t ocmb_dts;               // On chip thermal sensor
                            printf("mem_dts0               : 0x%04X\n",
                                   sensor_data.mem_dts0              );              // uint16_t mem_dts0;               // External DIMM thermal sensor 0
                            printf("mem_dts1               : 0x%04X\n",
                                   sensor_data.mem_dts1              );              // uint16_t mem_dts1;               // External DIMM thermal sensor 1
                            printf("mba_reads              : 0x%08X\n",
                                   sensor_data.mba_reads             );              // uint32_t mba_reads;              // The number of reads that the sequencer has seen; wraps
                            printf("mba_writes             : 0x%08X\n",
                                   sensor_data.mba_writes            );              // uint32_t mba_writes;             // The number of writes that the sequencer has seen; wraps
                            printf("mba_activations        : 0x%08X\n",
                                   sensor_data.mba_activations       );              // uint32_t mba_activations;        // The number of bank activates seen; wraps
                            printf("mba_powerups           : 0x%08X\n",
                                   sensor_data.mba_powerups          );              // uint32_t mba_powerups;           // Counts the number of rising edges for a CKE; wraps
                            printf("self_timed_refresh     : 0x%02X\n",
                                   sensor_data.self_timed_refresh    );              // uint8_t self_timed_refresh;      // The number of times that the sequencer exited self-timed refresh

                            for (int l_i = 0; l_i < SENSOR_CACHE_PADDING_SIZE_0; l_i++)
                            {
                                printf("reserved0[%d]           : 0x%02X\n", l_i,
                                       sensor_data.reserved0[l_i]         );         // uint8_t reserved0[SENSOR_CACHE_PADDING_SIZE_0];
                            }

                            printf("frame_count            : 0x%08X\n",
                                   sensor_data.frame_count           );              // uint32_t frame_count;            // Free running counter that is used as denominator for performance counts
                            printf("mba_arrival_histo_base : 0x%08X\n",
                                   sensor_data.mba_arrival_histo_base);              // uint32_t mba_arrival_histo_base; // Increments every MBA Idle Cycle
                            printf("mba_arrival_histo_low  : 0x%08X\n",
                                   sensor_data.mba_arrival_histo_low );              // uint32_t mba_arrival_histo_low;  // Counts the number of times the low idle threshold was met
                            printf("mba_arrival_histo_med  : 0x%08X\n",
                                   sensor_data.mba_arrival_histo_med );              // uint32_t mba_arrival_histo_med;  // Counts the number of times the med idle threshold was met
                            printf("mba_arrival_histo_high : 0x%08X\n",
                                   sensor_data.mba_arrival_histo_high);              // uint32_t mba_arrival_histo_high; // Counts the number of times the high idle threshold was met
                            printf("initial_packet1        : 0x%02X\n",
                                   sensor_data.initial_packet1        );             // uint8_t initial_packet1;         // initial_packet1[0] '1' on first packet1 return, otherwise '0'

                            //                                                                                           // initial_packet1[1:7] Reserved

                            for (int l_i = 0; l_i < SENSOR_CACHE_PADDING_SIZE_1; l_i++)
                            {
                                printf("reserved1[%d]           : 0x%02X\n", l_i,
                                       sensor_data.reserved1[l_i]         );         // uint8_t reserved1[SENSOR_CACHE_PADDING_SIZE_1];
                            }
                        }
                        break;

                    default:
                        ecmdOutputError ("Mode does not have a proper value! Run the help script and try again\n");
                }

                rc = (uint64_t) rc_fapi;

                if (rc)
                {
                    ecmdOutputError ("Error reading.\n");
                    break;
                }
            }
            else
            {
                switch (mode)
                {
                    case CFG:
                        rc_fapi = mss::exp::ib::putOCCfg(fapi_target, base_address, m32_data);
                        break;

                    case SCOM:
                        rc_fapi = mss::exp::ib::putScom(fapi_target, base_address, m64_data);
                        break;

                    case MMIO32:
                        rc_fapi = mss::exp::ib::putMMIO32(fapi_target, base_address, m32_data);
                        break;

                    case MMIO64:
                        rc_fapi = mss::exp::ib::putMMIO64(fapi_target, base_address, m64_data);
                        break;

                    case CMD:
                        rc_fapi = mss::exp::ib::putCMD(fapi_target, cmd_data);
                        break;

                    default:
                        ecmdOutputError("Mode does not have a proper value! Run the help script and try again\n");
                }

                rc = (uint64_t) rc_fapi;

                if (rc)
                {
                    ecmdOutputError ("Error writing.\n");
                    break;
                }
            }

        }

        // check that a valid target was found
        if (rc == ECMD_SUCCESS && !valid_pos_found)
        {
            ecmdOutputError("No valid targets found!\n");
            ecmdUnloadDll();
            return ECMD_TARGET_NOT_CONFIGURED;
        }
        // error occurred in loop, exit
        else if (rc)
        {
            break;
        }

    }
    while(0);

    // unload DLL
    ecmdUnloadDll();
    return rc;

}
