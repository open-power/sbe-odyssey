/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/memory/common/pmic/wrapper/pmic_n_mode_detect_wrap.C $ */
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
/// @file pmic_n_mode_detect_wrap.C
/// @brief Wrapper for calling pmic_n_mode_detect
///
// *HWP HWP Owner: Stephen Glancy <sglancy@us.ibm.com>
// *HWP HWP Backup: Louis Stermole <stermole@us.ibm.com>
// *HWP Team: Memory
// *HWP Level: 2
// *HWP Consumed by: FSP:HB


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
#include <wrapper/pmic_n_mode_detect_wrap.H>
#include <generic/memory/mss_git_data.H>


//------------------------------------------------------------------------------
// Function definitions
//------------------------------------------------------------------------------

// display help message
static const std::string PROCEDURE = "pmic_n_mode_detect_wrap";
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
    ecmdOutput("      -k#                          Specify which cage to act on (default = 0).\n");
    ecmdOutput("      -n#                          Specify which node to act on (default = 0).\n");
    ecmdOutput("      -s#                          Specify which slot to act on (default = 0).\n");
    ecmdOutput("      -p#                          Specify which chip position to act on (default = 0).\n");
    ecmdOutput("      -quiet                       Suppress printing of eCMD DLL/procedure information (default = false).\n");
    ecmdOutput("      -verif                       Run procedure in sim verification mode (default = false).\n");
    return;
}

void print_pmic_telem(pmic_telemetry& i_data)
{
    printf("R08-STATUS_0: 0x%02X\n", i_data.iv_r08);
    printf("R09-STATUS_1: 0x%02X\n", i_data.iv_r09);
    printf("R0A-STATUS_2: 0x%02X\n", i_data.iv_r0a);
    printf("R0B-STATUS_3: 0x%02X\n", i_data.iv_r0b);

    printf("SWA Current: %umA\n", i_data.iv_swa_current_mA);
    printf("SWB Current: %umA\n", i_data.iv_swb_current_mA);
    printf("SWC Current: %umA\n", i_data.iv_swc_current_mA);
    printf("SWD Current: %umA\n", i_data.iv_swd_current_mA);

    printf("VIN_BULK:         %umV\n", i_data.iv_vin_bulk_mV);
    printf("Temperature:      %uC\n", i_data.iv_temp_c);
    printf("R33-TEMP_STATUS:  0x%02X\n", i_data.iv_r33);

    printf("R1B-OV_OT_THRESH: 0x%02X\n", i_data.iv_r1b);
    printf("R2E-OTF_THRESH:   0x%02X\n", i_data.iv_r2e);
    printf("R2F-PMIC_CONFIG:  0x%02X\n", i_data.iv_r2f);
    printf("R32-PMIC_ENABLE:  0x%02X\n", i_data.iv_r32);
    printf("R9C-ON_OFF_CONFIG_GLOBAL: 0x%02X\n", i_data.iv_r9c);
}

///
/// @brief Print the telemetry data
/// @param[in] i_data telemetry data struct
///
void print_telem(telemetry_data& i_data)
{
    // Telem info
    printf("\nTelemetry Data:\n");
    printf("\nPMIC1:\n");
    print_pmic_telem(i_data.iv_pmic1);

    printf("\nPMIC2:\n");
    print_pmic_telem(i_data.iv_pmic2);

    printf("\nPMIC3:\n");
    print_pmic_telem(i_data.iv_pmic3);

    printf("\nPMIC4:\n");
    print_pmic_telem(i_data.iv_pmic4);

    // Print the struct
    printf("\nGPIO Data:\n");
    printf("GPIO1 Input Port:         0x%02X\n", i_data.iv_gpio1_port_state);
    printf("GPIO1 Output Port:        0x%02X\n", i_data.iv_gpio1_r01_efuse_output);
    printf("GPIO1 Polarity Inversion: 0x%02X\n", i_data.iv_gpio1_r02_efuse_polarity);
    printf("GPIO1 Configuration:      0x%02X\n", i_data.iv_gpio1_r03_configuration);

    printf("GPIO2 Input Port:         0x%02X\n", i_data.iv_gpio2_port_state);
    printf("GPIO2 Output Port:        0x%02X\n", i_data.iv_gpio2_r01_efuse_output);
    printf("GPIO2 Polarity Inversion: 0x%02X\n", i_data.iv_gpio2_r02_efuse_polarity);
    printf("GPIO2 Configuration:      0x%02X\n", i_data.iv_gpio2_r03_configuration);

    printf("\nADC1 Data:\n");
    printf("Event Low Flag: 0x%02X\n", i_data.iv_adc1.iv_event_low_flag);
    printf("Recent CH0: %umV\n", i_data.iv_adc1.iv_recent_ch0_mV);
    printf("Recent CH1: %umV\n", i_data.iv_adc1.iv_recent_ch1_mV);
    printf("Recent CH2: %umV\n", i_data.iv_adc1.iv_recent_ch2_mV);
    printf("Recent CH3: %umV\n", i_data.iv_adc1.iv_recent_ch3_mV);
    printf("Recent CH4: %umV\n", i_data.iv_adc1.iv_recent_ch4_mV);
    printf("Recent CH5: %umV\n", i_data.iv_adc1.iv_recent_ch5_mV);
    printf("Recent CH6: %umV\n", i_data.iv_adc1.iv_recent_ch6_mV);
    printf("Recent CH7: %umV\n", i_data.iv_adc1.iv_recent_ch7_mV);

    printf("Max CH0: %umV\n", i_data.iv_adc1.iv_max_ch0_mV);
    printf("Max CH1: %umV\n", i_data.iv_adc1.iv_max_ch1_mV);
    printf("Max CH2: %umV\n", i_data.iv_adc1.iv_max_ch2_mV);
    printf("Max CH3: %umV\n", i_data.iv_adc1.iv_max_ch3_mV);
    printf("Max CH4: %umV\n", i_data.iv_adc1.iv_max_ch4_mV);
    printf("Max CH5: %umV\n", i_data.iv_adc1.iv_max_ch5_mV);
    printf("Max CH6: %umV\n", i_data.iv_adc1.iv_max_ch6_mV);
    printf("Max CH7: %umV\n", i_data.iv_adc1.iv_max_ch7_mV);

    printf("Min CH0: %umV\n", i_data.iv_adc1.iv_min_ch0_mV);
    printf("Min CH1: %umV\n", i_data.iv_adc1.iv_min_ch1_mV);
    printf("Min CH2: %umV\n", i_data.iv_adc1.iv_min_ch2_mV);
    printf("Min CH3: %umV\n", i_data.iv_adc1.iv_min_ch3_mV);
    printf("Min CH4: %umV\n", i_data.iv_adc1.iv_min_ch4_mV);
    printf("Min CH5: %umV\n", i_data.iv_adc1.iv_min_ch5_mV);
    printf("Min CH6: %umV\n", i_data.iv_adc1.iv_min_ch6_mV);
    printf("Min CH7: %umV\n", i_data.iv_adc1.iv_min_ch7_mV);

    printf("System Status:                   0x%02X\n", i_data.iv_adc1.iv_system_status);
    printf("Auto Sequence Channel Selection: 0x%02X\n", i_data.iv_adc1.iv_auto_seq_ch_sel);

    printf("\nADC2 Data:\n");
    printf("Event Low Flag: 0x%02X\n", i_data.iv_adc2.iv_event_low_flag);
    printf("Recent CH0: %umV\n", i_data.iv_adc2.iv_recent_ch0_mV);
    printf("Recent CH1: %umV\n", i_data.iv_adc2.iv_recent_ch1_mV);
    printf("Recent CH2: %umV\n", i_data.iv_adc2.iv_recent_ch2_mV);
    printf("Recent CH3: %umV\n", i_data.iv_adc2.iv_recent_ch3_mV);
    printf("Recent CH4: %umV\n", i_data.iv_adc2.iv_recent_ch4_mV);
    printf("Recent CH5: %umV\n", i_data.iv_adc2.iv_recent_ch5_mV);
    printf("Recent CH6: %umV\n", i_data.iv_adc2.iv_recent_ch6_mV);
    printf("Recent CH7: %umV\n", i_data.iv_adc2.iv_recent_ch7_mV);

    printf("Max CH0: %umV\n", i_data.iv_adc2.iv_max_ch0_mV);
    printf("Max CH1: %umV\n", i_data.iv_adc2.iv_max_ch1_mV);
    printf("Max CH2: %umV\n", i_data.iv_adc2.iv_max_ch2_mV);
    printf("Max CH3: %umV\n", i_data.iv_adc2.iv_max_ch3_mV);
    printf("Max CH4: %umV\n", i_data.iv_adc2.iv_max_ch4_mV);
    printf("Max CH5: %umV\n", i_data.iv_adc2.iv_max_ch5_mV);
    printf("Max CH6: %umV\n", i_data.iv_adc2.iv_max_ch6_mV);
    printf("Max CH7: %umV\n", i_data.iv_adc2.iv_max_ch7_mV);

    printf("Min CH0: %umV\n", i_data.iv_adc2.iv_min_ch0_mV);
    printf("Min CH1: %umV\n", i_data.iv_adc2.iv_min_ch1_mV);
    printf("Min CH2: %umV\n", i_data.iv_adc2.iv_min_ch2_mV);
    printf("Min CH3: %umV\n", i_data.iv_adc2.iv_min_ch3_mV);
    printf("Min CH4: %umV\n", i_data.iv_adc2.iv_min_ch4_mV);
    printf("Min CH5: %umV\n", i_data.iv_adc2.iv_min_ch5_mV);
    printf("Min CH6: %umV\n", i_data.iv_adc2.iv_min_ch6_mV);
    printf("Min CH7: %umV\n", i_data.iv_adc2.iv_min_ch7_mV);

    printf("System Status:                   0x%02X\n", i_data.iv_adc2.iv_system_status);
    printf("Auto Sequence Channel Selection: 0x%02X\n", i_data.iv_adc2.iv_auto_seq_ch_sel);
}

// main function
int main(int argc, char* argv[])
{
    // from prcdUtils
    extern bool GLOBAL_SIM_MODE;
    extern bool GLOBAL_VERIF_MODE;

    // flow/control variables
    uint32_t rc = ECMD_SUCCESS;
    ecmdDllInfo DLLINFO;
    ecmdLooperData node_looper;
    ecmdChipTarget node_target;
    char outstr[256];

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

        printf( "// PMIC state bitmap guide:\n"
                "// DECONFIGURED       = 0b10000000\n"
                "// NOT_PRESENT        = 0b00001000\n"
                "// I2C_FAIL           = 0b00000100\n"
                "// CURRENT_IMBALANCE  = 0b00000010\n"
                "// PWR_NOT_GOOD       = 0b00000001\n"
                "// ALL_GOOD           = 0b00000000\n\n");

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
                wrapper_ostream l_stream = wrapper_ostream();

                if (!ecmdGetGlobalVar(ECMD_GLOBALVAR_QUIETMODE))
                {
                    snprintf(outstr, sizeof(outstr), "Going to call %s on %s\n",
                             PROCEDURE.c_str(),
                             ecmdWriteTarget(l_ocmb_target).c_str());
                    ecmdOutput(outstr);
                }

                // invoke FAPI procedure core
                fapi2::Target<fapi2::TARGET_TYPE_OCMB_CHIP> fapi_target(&l_ocmb_target);

                FAPI_EXEC_HWP(rc,
                              pmic_n_mode_detect,
                              fapi_target,
                              l_stream);

                runtime_n_mode_telem_info l_info = l_stream.get_struct();

                printf("PMICS on %s:\n", ecmdWriteTarget(l_ocmb_target).c_str());

                switch (l_info.iv_aggregate_error)
                {
                    case aggregate_state::N_PLUS_1:
                        printf("N Mode State: N_PLUS_1\n");
                        break;

                    case aggregate_state::N_MODE:
                        printf("N Mode State: N_MODE\n");
                        break;

                    case aggregate_state::LOST:
                        printf("N Mode State: LOST\n");
                        break;

                    case aggregate_state::GI2C_I2C_FAIL:
                        printf("N Mode State: I2C_FAIL\n");
                        break;

                    default:
                        printf("N Mode State: NOT 4U\n");
                }

                printf("PMIC1 State: 0x%02X\n", l_info.iv_pmic1_errors);
                printf("PMIC2 State: 0x%02X\n", l_info.iv_pmic2_errors);
                printf("PMIC3 State: 0x%02X\n", l_info.iv_pmic3_errors);
                printf("PMIC4 State: 0x%02X\n\n", l_info.iv_pmic4_errors);

                print_telem(l_info.iv_telemetry_data);
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
