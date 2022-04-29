/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/memory/odyssey/hwp/lib/phy/ody_snps_ate_msg_blk.H $ */
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

///
/// @file ody_snps_ate_msg_blk.H
/// @brief Odyssey ATE Message block strucure
///

// *HWP HWP Owner: Geetha Pisapati <Geetha.Pisapati@ibm.com>
// *HWP HWP Backup: Stephen Glancy <sglancy@us.ibm.com>
// *HWP Team: Memory
// *HWP Level: 2
// *HWP Consumed by: FSP:HB

#ifndef _ODY_SNPS_ATE_MSG_BLK_H_
#define _ODY_SNPS_ATE_MSG_BLK_H_

//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
// General defines
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------

// Basic type typedefs

#include <stdint.h>

// ATE firmware revision

#define PMU_ATE_INTERNAL_REV0 0x0101
#define PMU_ATE_INTERNAL_REV1 0x2022


//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
// Message block related defines
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------

// TestsToRun encoding

#define REVISION_CHECK_TEST   0x0001
#define IMPEDANCE_CAL_TEST    0x0002
#define PLL_LOCK_TEST         0x0004
#define LCDL_LINEARITY_TEST   0x0008
#define AC_LOOPBACK_TEST      0x0010
#define DAT_1D_LOOPBACK_TEST  0x0020
#define DAT_2D_LOOPBACK_TEST  0x0040
#define BURN_IN_TEST          0x0080
#define CHARACTERIZATION_TEST 0x8000

// TestOptions encoding

#define TEST_OPTION_PLL_LOCK_INCREASE              0x0001
#define TEST_OPTION_PLL_RESET_INCREASE             0x0002
#define TEST_OPTION_PWR_SAVE_DISABLE               0x0004
#define TEST_OPTION_AC_PINS_PRBS                   0x0008
#define TEST_OPTION_SINGLE_BYTE_BIT                0x0010
#define TEST_OPTION_SKIP_SAVE_RESTORE              0x0020
#define TEST_OPTION_BYPASS_FOURTAPDFE_BROADCASTING 0x0040
#define TEST_OPTION_ENABLE_ALL_ANIBS               0x0080
#define TEST_OPTION_DISABLE_BDL_TEST               0x0100
#define TEST_OPTION_DISABLE_DFE_SUMMER_TEST        0x0200
#define TEST_OPTION_DELAY_SET_DISABLE              0x8000




//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
// Message Block definition for the ATE firmware
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------

typedef struct _PMU_SMB_ATE_t
{

    /////////////////////////////////////////
    // General ATE Firmware Inputs / Outputs
    /////////////////////////////////////////

    uint16_t TestsToRun;  // Byte Offset: 0x0 / CSR Address: 0x58000
    // TestsToRun[0]    = DMEM / IMEM revision check
    // TestsToRun[1]    = Impedance Calibration
    // TestsToRun[2]    = PLL Lock
    // TestsToRun[3]    = LCDL Linearity
    // TestsToRun[4]    = Address / Command Loopback
    // TestsToRun[5]    = Data Loopback 1D
    // TestsToRun[6]    = Data Loopback 2D
    // TestsToRun[7]    = Burn-In
    // TestsToRun[15:8] = Reserved (must be written to 0)
    uint16_t PassFailResults;  // Byte Offset: 0x2 / CSR Address: 0x58001
    // Bit value of 0 = fail / 1 = pass
    // PassFailResults[0]    = DMEM / IMEM revision check
    // PassFailResults[1]    = Impedance Calibration
    // PassFailResults[2]    = PLL Lock
    // PassFailResults[3]    = LCDL Linearity
    // PassFailResults[4]    = Address / Command Loopback
    // PassFailResults[5]    = Data Loopback 1D
    // PassFailResults[6]    = Data Loopback 2D
    // PassFailResults[7]    = Burn-In
    // PassFailResults[15:8] = Reserved (will be 0)
    uint16_t TestOptions;  // Byte Offset: 0x4 / CSR Address: 0x58002
    // TestOptions[0]    = This value must be set to 0, unless directed by Synopsys.
    //                     For some technologies, the PLL locking behaviour may need to be modified.
    // TestOptions[1]    = PLL reset time increase (must be written to 0 unless directed by Synopsys)
    // TestOptions[2]    = 0 - Power Savings features enabled; 1 - Power Savings features disabled.
    // TestOptions[3]    = Requires TestOptions[2] to be set to 1;
    //                     0 - Normal A/C command signals on AC pins; 1 - PRBS signals on AC pins
    //
    // TestOptions[4]    = 0 - All DBYTEs are exercised during data loopback (1D/2D), and all DBYTEs must pass for the test to pass.
    //                     1 - All DBYTEs are execised during data loopback (1D/2D), but (1) or ALL the lanes in (1) DBYTE, are checked.
    //                         Only the lane(s) checked must pass for the test to pass.
    //                         The MsgBlock fields 'DatLoopByte' and 'DatLoopBit' are used to select the DBYTE/Lane(s).
    //
    //                           - 1D Data Loopback
    //                              - DatLoopByte must be [0:NumDbytes-1]
    //                              - DatLoopBit must be either:
    //                              - Inside [0:NumBitsPerDbyte-1], which will check (1) specific lane in 'DatLoopByte'
    //                              - 0xFF, which  will result in ALL lanes being checked
    //
    //                           - 2D Data Loopback:
    //                              - DatLoopByte must be [0:NumDbytes-1]
    //                              - DatLoopBit must be [0:NumBitsPerDbyte-1]
    //
    //                           - NOTE: If this test-option is enabled, and DatLoopByte/DatLoopBit are configured
    //                                   illegally, the test will fail.
    //
    //
    // TestOptions[5]    = 0 - CSRs modified by ATE FW are restored to their initial state at the end of the test.
    //                     1 - ATE FW does not restore any CSRs
    //
    // TestOptions[6]    = 0 -Broadcast the FourTapDfe message block settings(csrRxDFEMisc/csrRxDFETapCtrl/csrRxDFECalibCtrl) to the corresponding CSRs in all DBYTES when in 4 tap DFE mode.
    //                     1 -Firmware does not write FourTapDfe message block inputs (csrRxDFEMisc/csrRxDFETapCtrl/csrRxDFECalibCtrl) to the corresponding CSRs. It is up to the customer to program all the CSRs
    // TestOptions[7]    = 0 - Make sure only those Anibs that drive clocks to the Dbytes are powered, shut all the others down. And tristate the IO to save power during the test.
    //                     1 - Enable all Anibs for data loopback test.
    // TestOptions[8]    = 0 - Enable  the BDL tests in 1D loopback tests when the PHY support 4-tap DFE and running pad loopback.
    //                     1 - Disable the BDL tests in 1D loopback tests.
    // TestOptions[14:9] = Reserved (must be written to 0)
    // TestOptions[15]   = Delay control - Causes the data loopback tests to not set delays for
    //                     elements that are not scanned as part of the test. Leaves these values
    //                     at user settings
    uint16_t Technology;  // Byte Offset: 0x6 / CSR Address: 0x58003
    // Must be set to 0x0000 unless told otherwise for a given technology

    uint32_t AteImemRevision;  // Byte Offset: 0x8 / CSR Address: 0x58004
    // PMU firmware revision ID of the IMEM image
    // After ATE firmware is run, this address will contain
    // the revision ID of the ATE firmware's IMEM image.
    // Please reference this revision ID when filing support
    // cases with Synopsys.
    uint32_t AteDmemRevision;  // Byte Offset: 0xc / CSR Address: 0x58006
    // PMU firmware revision ID of the DMEM image
    // After ATE firmware is run, this address will contain
    // the revision ID of the ATE firmware's DMEM image.
    // Please reference this revision ID when filing support
    // cases with Synopsys.
    uint8_t  NumDbytes;  // Byte Offset: 0x10 / CSR Address: 0x58008
    // Number of Dbytes in the PHY
    uint8_t  NumAnibs;  // Byte Offset: 0x11 / CSR Address: 0x58008
    // Number of Anibs in the PHY
    uint8_t  NumBitsPerDbyte;  // Byte Offset: 0x12 / CSR Address: 0x58009
    // Number of physical lanes per Dbyte.
    uint8_t  X4Mode;  // Byte Offset: 0x13 / CSR Address: 0x58009
    // x4 Mode
    //    0x0 = x8 mode
    //    0x1 = x4 mode

    uint8_t  PhyMode;  // Byte Offset: 0x14 / CSR Address: 0x5800a
    // Phy Mode
    //    0x0 = DDR4 mode
    //    0x1 = DDR5 mode

    uint8_t  ARdPtrInitVal;  // Byte Offset: 0x15 / CSR Address: 0x5800a
    // See CSR ARdPtrInitVal for description of contents.
    uint16_t CalDrvStr0;  // Byte Offset: 0x16 / CSR Address: 0x5800b
    // See CSR CalDrvStr0 for description of contents.
    uint8_t  Vref;  // Byte Offset: 0x18 / CSR Address: 0x5800c
    // Vref[6:0] is used to set CSR subfield GlobalVrefInDAC[9:3],
    // Vref[7] are not used, should be set to 0.
    // The Vref range(csrGlobalVrefInSel[2:0]) could be set by register
    // programming before executing ATE firmware.
    uint8_t  Alignment0RSVD;  // Byte Offset: 0x19 / CSR Address: 0x5800c
    // Reserved field for alignment, will not be written/read.
    uint16_t DfiClkFreq;  // Byte Offset: 0x1a / CSR Address: 0x5800d
    // DFI Clock frequency in MHz
    // For example enter 0x0320 for 800MHz.
    uint16_t TxImpedanceCtrl0;  // Byte Offset: 0x1c / CSR Address: 0x5800e
    // See CSR TxImpedanceCtrl0 for description of contents.
    uint16_t TxImpedanceCtrl1;  // Byte Offset: 0x1e / CSR Address: 0x5800f
    // See CSR TxImpedanceCtrl1 for description of contents.
    uint16_t TxImpedanceCtrl2;  // Byte Offset: 0x20 / CSR Address: 0x58010
    // See CSR TxImpedanceCtrl2 for description of contents.
    uint16_t ATxImpedance;  // Byte Offset: 0x22 / CSR Address: 0x58011
    // See CSR of the same name for description of contents.
    uint16_t MinLoopbackPower;  // Byte Offset: 0x24 / CSR Address: 0x58012
    // Used by Data and AC Loopback tests to serialize testing of the dbytes/acx4 instances.
    // That is, it tests one dbyte/acx4 at a time and configures unused dbytes/acx4 for
    // lower power. This trades off power for test execution time.


    ///////////////////////////////////////////////
    // Impedance Calibration Test Inputs
    ///////////////////////////////////////////////

    uint8_t  ContinuousCal;  // Byte Offset: 0x26 / CSR Address: 0x58013
    // Continuous Calibration
    //     0x0 = Do not do continuous calibration
    //     0x1 = Do continuous calibration
    uint8_t  CalInterval;  // Byte Offset: 0x27 / CSR Address: 0x58013
    // CalInterval of CalRate CSR used in continuous cal
    //     See CalRate CSR description for more information.


    ///////////////////////////////////////////////
    // Pll Lock Test Inputs
    ///////////////////////////////////////////////

    uint16_t MemClkToggle;  // Byte Offset: 0x28 / CSR Address: 0x58014
    // MEMCLK toggle
    //      0x0 = MEMCLK does not toggle
    //      0x1 = MEMCLK toggles at 2*DFICLK
    //      0x2 = MEMCLK toggles at DFICLK / 2
    //      0x3 = MEMCLK toggles at DFICLK / 4
    //      0x4 = MEMCLK toggles at DFICLK / 8
    //      0x5 = MEMCLK toggles at DFICLK / 16
    //      0x6 = MEMCLK toggles at DFICLK / 24
    //            All other values reserved.
    uint16_t MemClkTime;  // Byte Offset: 0x2a / CSR Address: 0x58015
    // Time in microseconds to toggle MEMCLK
    // Setting to 0xFFFF will cause
    // the clock to run infinitely. The uC must be reset
    // to stop if in the infinite mode.


    /////////////////////////////////////////
    // LCDL Linearity Test Inputs
    /////////////////////////////////////////

    uint16_t LcdlClksToRun;  // Byte Offset: 0x2c / CSR Address: 0x58016
    // Clock cycles to run for each measurement
    uint16_t LcdlNumTestLoops;  // Byte Offset: 0x2e / CSR Address: 0x58017
    // Number of loops to use in testing the LCDLs
    // (1/LcdlNumTestLoops) of the LCDLs will be
    // tested each loop.
    uint16_t LcdlStartPhase;  // Byte Offset: 0x30 / CSR Address: 0x58018
    // Start Phase (0-127 decimal)
    uint16_t LcdlEndPhase;  // Byte Offset: 0x32 / CSR Address: 0x58019
    // End Phase (0-1023 decimal)
    uint8_t  LcdlStride;  // Byte Offset: 0x34 / CSR Address: 0x5801a
    // Stride 1 to 100, 7 recommended
    // 0x00 = Use recommended value of 7
    // 0x01-0x64 = Use this value for stride
    // All other values reserved
    uint8_t  LcdlPassPercent;  // Byte Offset: 0x35 / CSR Address: 0x5801a
    // Percentage of spec values to consider pass / fail
    // 100 (decimal) = 100% = matches spec
    // Numbers smaller than 100 tighten the tolerance
    // Numbers larger than 100 loosen the tolerance

    uint16_t LcdlObserveCfg[4];  // Byte Offset: 0x36 / CSR Address: 0x5801b
    // LCDL Linearity Plot - LCDL selection inputs
    //
    // LcdlObserveCfg[i][15:8] - DBYTE/ANIB Instance Select
    // LcdlObserveCfg[i][7:0]  - LCDL Type Select:
    //
    // -- ANIB LCDL Type Select --
    //
    //    255-28 = RESERVED
    //        27 = Anib TuDll
    //        26 = Anib MdDll
    //        25 = Anib TxDll
    //
    // -- DBYTE LCDL Type Select --
    //
    //        24 = Dbyte BDL RxLn8 (dbi)
    //        23 = Dbyte BDL RxLn7 (dq7)
    //        22 = Dbyte BDL RxLn6 (dq6)
    //        21 = Dbyte BDL RxLn5 (dq5)
    //        20 = Dbyte BDL RxLn4 (dq4)
    //        19 = Dbyte BDL RxLn3 (dq3)
    //        18 = Dbyte BDL RxLn2 (dq2)
    //        17 = Dbyte BDL RxLn1 (dq1)
    //        16 = Dbyte BDL RxLn0 (dq0)
    //        15 = Dbyte RxClk1C
    //        14 = Dbyte RxClk0C
    //        13 = Dbyte RxClk1T
    //        12 = Dbyte RxClk0T
    //        11 = Dbyte RxEn1
    //        10 = Dbyte RxEn0
    //         9 = Dbyte TxLn9 (dqs-lower)
    //         8 = Dbyte TxLn8 (dm/dqs-upper)
    //         7 = Dbyte TxLn7 (dq7)
    //         6 = Dbyte TxLn6 (dq6)
    //         5 = Dbyte TxLn5 (dq5)
    //         4 = Dbyte TxLn4 (dq4)
    //         3 = Dbyte TxLn3 (dq3)
    //         2 = Dbyte TxLn2 (dq2)
    //         1 = Dbyte TxLn1 (dq1)
    //         0 = Dbyte TxLn0 (dq0)
    //


    /////////////////////////////////////////
    // AC Loopback Test Inputs
    /////////////////////////////////////////

    uint8_t  AcLoopLaneMask[7];  // Byte Offset: 0x3e / CSR Address: 0x5801f
    // Mask for disabling check on any of the AC lanes.
    // Setting a bit to 1 disables the check of that lane.
    // Setting a bit to 0 enables the check of that lane.
    uint8_t  AcLoopIncrement;  // Byte Offset: 0x45 / CSR Address: 0x58022
    // Value to increment delay by for the Ac loopback search
    // This value is in fine delay values.
    uint16_t ATxSlewRate;  // Byte Offset: 0x46 / CSR Address: 0x58023
    // See CSR of the same name for description of contents.
    uint16_t AcLoopClksToRun;  // Byte Offset: 0x48 / CSR Address: 0x58024
    // Clock cycles to run AC loopback
    uint8_t  AcLoopDataRate;  // Byte Offset: 0x4a / CSR Address: 0x58025
    // Data Rate
    //    0x0 = Single data rate pattern
    //    0x1 = Dual data rate pattern
    uint8_t  AcLoopCoreLoopBk;  // Byte Offset: 0x4b / CSR Address: 0x58025
    // Pad / Core loopback
    //    0x0 = Pad loopback
    //    0x1 = Core loopback
    uint8_t  AcMinEyeWidth;  // Byte Offset: 0x4c / CSR Address: 0x58026
    // Required Eye width in the units of 1/64 UI


    /////////////////////////////////////////
    // Data Loopback Test Inputs
    /////////////////////////////////////////

    uint8_t  Alignment1RSVD;  // Byte Offset: 0x4d / CSR Address: 0x58026
    // Reserved field for alignment, will not be written/read
    uint16_t DatLoopClksToRun;  // Byte Offset: 0x4e / CSR Address: 0x58027
    // Pattern length
    uint16_t DatLoopCoreLoopBk;  // Byte Offset: 0x50 / CSR Address: 0x58028
    // Pad / Core loopback
    //    0x0 = Pad loopback
    //    0x1 = Core loopback
    uint16_t TxSlewRate;  // Byte Offset: 0x52 / CSR Address: 0x58029
    // See CSR of the same name for description of contents.
    uint16_t DqDqsRcvCntrl;  // Byte Offset: 0x54 / CSR Address: 0x5802a
    // See CSR of the same name for description of contents.
    uint16_t TxDqsDly;  // Byte Offset: 0x56 / CSR Address: 0x5802b
    uint16_t RxEnDly;  // Byte Offset: 0x58 / CSR Address: 0x5802c
    uint16_t RxPBDly;  // Byte Offset: 0x5a / CSR Address: 0x5802d
    uint8_t  VrefDAC0;  // Byte Offset: 0x5c / CSR Address: 0x5802e
    // Vref used during the 1D data loopback measurement
    // See CSR of the same name for description of contents.
    uint8_t  Alignment2RSVD;  // Byte Offset: 0x5d / CSR Address: 0x5802e
    // Reserved field for alignment, will not be written/read
    uint8_t  DatLoopCoarseStart;  // Byte Offset: 0x5e / CSR Address: 0x5802f
    // Starting coarse value for the Data loopback search
    uint8_t  DatLoopCoarseEnd;  // Byte Offset: 0x5f / CSR Address: 0x5802f
    // Ending coarse value for the Data loopback search
    uint8_t  DatLoopIncrement;  // Byte Offset: 0x60 / CSR Address: 0x58030
    // Value to increment delay by for the Data loopback search
    // This value is in fine delay values.
    uint8_t  DatLoopMinEyeWidth;  // Byte Offset: 0x61 / CSR Address: 0x58030
    // Minimum Eye width for data loopback tests
    // Required Eye width in the units of 1/64 UI
    uint8_t  DatLoopByte;  // Byte Offset: 0x62 / CSR Address: 0x58031
    // Byte to test for 2D loopback, and Single DBYTE testing mode
    uint8_t  DatLoopBit;  // Byte Offset: 0x63 / CSR Address: 0x58031
    // Bit to test for 2D loopback, and Single DBYTE testing mode

    uint8_t  DatLoop2dVrefStart;  // Byte Offset: 0x64 / CSR Address: 0x58032
    // Initial value of Vref used during the 2D loopback measurement
    uint8_t  DatLoop2dVrefEnd;  // Byte Offset: 0x65 / CSR Address: 0x58032
    // Upper bound of Vref used during the 2D loopback measurement
    uint8_t  DatLoop2dVrefIncr;  // Byte Offset: 0x66 / CSR Address: 0x58033
    // Vref Increment size (1-63) for 2D loopback measurement
    uint8_t  RxEnbOffset;  // Byte Offset: 0x67 / CSR Address: 0x58033
    // Positive MemClk offset applied to RxEnb after training. Setting this
    // to zero results in RxEnb being trained as it would in a real system.
    // Non-zero value result in the initial DQS pulses being skipped. The
    // number of pulses skipped is equal to the value set here. Setting this to
    // non-zero values can improve the data loopback eye by skipping the initial
    // DQS pulses, which can be narrower than normal because the bus is at its
    // maximum idle value rather than active steady state.;
    uint16_t FourTapDfe_RxDFETapCtrl;  // Byte Offset: 0x68 / CSR Address: 0x58034
    // Only Used for 4-tap DFE setting.
    // See CSR RxDFETapCtrl for description of contents.
    uint16_t FourTapDfe_RxDFECalibCtrl;  // Byte Offset: 0x6a / CSR Address: 0x58035
    // Only Used for 4-tap DFE setting.
    // See CSR RxDFECalibCtrl for description of contents.
    uint8_t  FourTapDfe_RxDFEMisc;  // Byte Offset: 0x6c / CSR Address: 0x58036
    // Only Used for 4-tap DFE setting.
    // See CSR RxDFEMisc for description of contents.
    uint8_t  VrefDAC1;  // Byte Offset: 0x6d / CSR Address: 0x58036
    // For 4-tap DFE, VrefDAC1 is only used when either 1-tap, 2-tap, 3-tap, or 4-tap mode is selected.
    // For 2-tap DFE, VrefDAC1 is only used when either 1-tap or 2-tap mode is selected.
    // Vref used during the 1D data loopback measurement.
    // See CSR of the same name for description of contents.
    uint8_t  VrefDAC2;  // Byte Offset: 0x6e / CSR Address: 0x58037
    // For 4-tap DFE, VrefDAC2 is not used.
    // For 2-tap DFE, VrefDAC2 is only used when 2-tap mode is selected.
    // Vref used during the 1D data loopback measurement.
    // See CSR of the same name for description of contents.
    uint8_t  VrefDAC3;  // Byte Offset: 0x6f / CSR Address: 0x58037
    // For 4-tap DFE, VrefDAC3 is not used.
    // For 2-tap DFE, VrefDAC3 is only used when 2-tap mode is selected.
    // Vref used during the 1D data loopback measurement.
    // See CSR of the same name for description of contents.
    uint16_t Alignment3RSVD;  // Byte Offset: 0x70 / CSR Address: 0x58038
    // Reserved field for alignment, will not be written/read
    uint16_t Alignment4RSVD;  // Byte Offset: 0x72 / CSR Address: 0x58039
    // Reserved field for alignment, will not be written/read


    /////////////////////////////////////////
    // Impedance Calibration Test Results
    /////////////////////////////////////////

    uint16_t TxCalBinP;  // Byte Offset: 0x74 / CSR Address: 0x5803a
    // Output from Impedance Calibration
    uint16_t TxCalBinN;  // Byte Offset: 0x76 / CSR Address: 0x5803b
    // Output from Impedance Calibration
    // See CSR of the same name for description of contents.
    uint16_t CalPExt;  // Byte Offset: 0x78 / CSR Address: 0x5803c
    // Output from Impedance Calibration
    // See CSR of the same name for description of contents.
    uint16_t CalNInt;  // Byte Offset: 0x7a / CSR Address: 0x5803d
    // Output from Impedance Calibration
    // See CSR of the same name for description of contents.
    uint16_t CalCmpr5;  // Byte Offset: 0x7c / CSR Address: 0x5803e
    // Output from Impedance Calibration
    // See CSR of the same name for description of contents.


    /////////////////////////////////////////
    // PLL Lock / LCDL Calibration Test Results
    /////////////////////////////////////////

    uint16_t PllResults;  // Byte Offset: 0x7e / CSR Address: 0x5803f
    // Pll lock results.
    //  -0x1 = PLL lock succeeded
    //  -0x0 = PLL lock failed

    uint16_t LcdlResultsDbyte[10];  // Byte Offset: 0x80 / CSR Address: 0x58040
    // If a DBYTE LCDL 1UI lock code is outside the expected range (caused the test to fail),
    // the cooresponding bit will be a (1).
    // There are 16 LCDLs per DBYTE (bits [15:0]).
    // NOTE: BDLs are not included, because they do not self-calibrate
    // Example of analyzing a DBYTE LCDL:
    //    LcdlErrcntDbyte[DBYTE] & (1<<DBYTE_LCDL_NUM)
    // where LcdlNumber is encoded as:
    //
    //   15 = Dbyte RxClk1T
    //   14 = Dbyte RxClk1C
    //   13 = Dbyte RxClk0T
    //   12 = Dbyte RxClk0C
    //   11 = Dbyte RxEn1
    //   10 = Dbyte RxEn0
    //    9 = Dbyte TxLn9 (dqs-lower)
    //    8 = Dbyte TxLn8 (dm/dqs-upper)
    //    7 = Dbyte TxLn7 (dq7)
    //    6 = Dbyte TxLn6 (dq6)
    //    5 = Dbyte TxLn5 (dq5)
    //    4 = Dbyte TxLn4 (dq4)
    //    3 = Dbyte TxLn3 (dq3)
    //    2 = Dbyte TxLn2 (dq2)
    //    1 = Dbyte TxLn1 (dq1)
    //    0 = Dbyte TxLn0 (dq0)
    //

    uint8_t  LcdlResultsAnib[14];  // Byte Offset: 0x94 / CSR Address: 0x5804a
    // If an ANIB LCDL 1UI lock code is outside the expected range (caused the test to fail),
    // the cooresponding bit will be a (1).
    // There are 3 LCDLs per ANIB (bits [2:0]. Bits [7:3] are RESERVED. Refer to 'LcdlErrcntAnib' for ANIB LCDL numbers
    // into array entry
    // Example of analyzing an ANIB LCDL:
    //    LcdlErrcntAnib[ANIB] & (1<<ANIB_LCDL_NUM)


    /////////////////////////////////////////
    // LCDL Linearity Test Results
    /////////////////////////////////////////

    uint16_t LcdlErrcntAnib[3][14];  // Byte Offset: 0xa2 / CSR Address: 0x58051
    // Defined as: LcdlErrCntByLcdlNumAndAnib[LcdlNumber][AnibNum]
    // Contains how many points failed linearity for each LCDL
    //
    // where LcdlNumber is encoded as:
    //
    //   2 = Anib TuDll
    //   1 = Anib MdDll
    //   0 = Anib TxDll

    uint16_t LcdlErrcntDbyte[25][10];  // Byte Offset: 0xf6 / CSR Address: 0x5807b
    //
    // Defined as: LcdlErrCntByLcdlNumAndDbyte[LcdlNumber][DbyteNum]
    // Contains how many points failed linearity for each LCDL
    //
    // where LcdlNumber is encoded as:
    //
    //   24 = Dbyte BDL RxLn8 (dbi)
    //   23 = Dbyte BDL RxLn7 (dq7)
    //   22 = Dbyte BDL RxLn6 (dq6)
    //   21 = Dbyte BDL RxLn5 (dq5)
    //   20 = Dbyte BDL RxLn4 (dq4)
    //   19 = Dbyte BDL RxLn3 (dq3)
    //   18 = Dbyte BDL RxLn2 (dq2)
    //   17 = Dbyte BDL RxLn1 (dq1)
    //   16 = Dbyte BDL RxLn0 (dq0)
    //   15 = Dbyte RxClk1C
    //   14 = Dbyte RxClk0C
    //   13 = Dbyte RxClk1T
    //   12 = Dbyte RxClk0T
    //   11 = Dbyte RxEn1
    //   10 = Dbyte RxEn0
    //    9 = Dbyte TxLn9 (dqs-lower)
    //    8 = Dbyte TxLn8 (dm/dqs-upper)
    //    7 = Dbyte TxLn7 (dq7)
    //    6 = Dbyte TxLn6 (dq6)
    //    5 = Dbyte TxLn5 (dq5)
    //    4 = Dbyte TxLn4 (dq4)
    //    3 = Dbyte TxLn3 (dq3)
    //    2 = Dbyte TxLn2 (dq2)
    //    1 = Dbyte TxLn1 (dq1)
    //    0 = Dbyte TxLn0 (dq0)
    //

    uint16_t Lcdl_count_values[4][1024];  // Byte Offset: 0x2ea / CSR Address: 0x58175
    // LCDL Linearity Plotting  Ring Oscillator Edge Count Values


    /////////////////////////////////////////
    // AC Loopback Test Results
    /////////////////////////////////////////

    uint16_t Alignment5RSVD;  // Byte Offset: 0x22ea / CSR Address: 0x59175
    // Reserved field for alignment, will not be written/read
    uint32_t Alignment6RSVD;  // Byte Offset: 0x22ec / CSR Address: 0x59176
    // Reserved field for alignment, will not be written/read
    uint64_t AcLoopbackStuckatResults;  // Byte Offset: 0x22f0 / CSR Address: 0x59178
    // Stuckat results. Each bit indicates whether that
    // lane failed stuckat testing. For example, if bit 4
    // is set to 1, it means that lane 4 (Acx4 instance 1,
    // lane 0) failed stuckat testing.

    uint64_t AcLoopbackBitmap[56][2];  // Byte Offset: 0x22f8 / CSR Address: 0x5917c
    // Address / Command loopback eye bitmap indexed by lane
    // AcLoopbackBitmapByLane[LaneNumber] and by UI


    /////////////////////////////////////////
    // Data Loopback 1D Test Results
    /////////////////////////////////////////

    uint8_t  LoopbackResults;  // Byte Offset: 0x2678 / CSR Address: 0x5933c
    // The Result of loopback test, used for 1D + DFE test;
    // This will be set to 1 if the loopback test passed, and 0 if the loopback test failed.
    uint8_t  BDLResults;  // Byte Offset: 0x2679 / CSR Address: 0x5933c
    // BDL test is only Used for 4-tap DFE ;
    // This will be set to 1 if the BDL test passed, and 0 if the BDL test failed.
    // If the BDL test is not enabled, the value of the BDLResults would be 1.
    uint16_t DFEResults[10];  // Byte Offset: 0x267a / CSR Address: 0x5933d
    // DFE test results indexed by Dbyte and Lane.
    // Defined as DFEResults[DbyteNumber].
    // DFEResults[DbyteNumber][7:0] are DQ[7:0] for DbyteNumber;
    // DFEResults[DbyteNumber][8] is DM/DBI for DbyteNumber;
    // DFEResults[DbyteNumber][15:9] are reserved.
    // The corresponding bit will be set to 1 if DFE testing passed, and 0 if DFE testing failed.
    // If the DFE test is not enabled , the value of the DFEResults[DbyteNumber] would be 0xFFFF.

    uint16_t DataLoopbackRxEnbVal[10][2];  // Byte Offset: 0x268e / CSR Address: 0x59347
    // Data loopback RxEnable value indexed by Dbyte and Nibble
    // Defined as DataLoopbackRxEnbVal[DbyteNumber][NibbleNumber]
    // DataLoopbackRxEnbVal[DbyteNumber][0] is for DQ[3:0] for DbyteNumber
    // DataLoopbackRxEnbVal[DbyteNumber][1] is for DQ[7:4] and DM/DBI for DbyteNumber

    uint8_t  DataLoopbackCoarse[10][9];  // Byte Offset: 0x26b6 / CSR Address: 0x5935b
    // Data loopback eye bitmap starting coarse value indexed by Dbyte and Lane
    // Defined as DataLoopbackCoarse[DbyteNumber][LaneNumber]
    // DataLoopbackCoarse[DbyteNumber][7:0] are DQ[7:0] for DbyteNumber
    // DataLoopbackCoarse[DbyteNumber][8] is DM/DBI for DbyteNumber
    uint64_t Alignment7RSVD;  // Byte Offset: 0x2710 / CSR Address: 0x59388
    // Reserved field for alignment, will not be written/read.
    uint64_t DataLoopbackBitmap[10][9][2];  // Byte Offset: 0x2718 / CSR Address: 0x5938c
    // Data loopback eye bitmap indexed by Dbyte and Lane
    // Defined as DataLoopbackBitmap[DbyteNumber][LaneNumber][UI_Number]
    // DataLoopbackBitmap[DbyteNumber][7:0] are DQ[7:0] for DbyteNumber
    // DataLoopbackBitmap[DbyteNumber][8] is DM/DBI for DbyteNumber


    /////////////////////////////////////////
    // Data Loopback 2D Test Results
    /////////////////////////////////////////

    uint8_t  DatLoop2dVrefCoarse[128];  // Byte Offset: 0x2cb8 / CSR Address: 0x5965c
    // Data Loopback bitmaps coarse starting value per Vref for specified DQ
    // with DatLoop2dByte / DatLoop2dBit / DatLoop2dVrefIncr
    // Indexed by Vref stride index

    uint64_t DatLoop2dVrefBitmap[128][2];  // Byte Offset: 0x2d38 / CSR Address: 0x5969c
    // Data Loopback bitmaps per Vref for specified DQ with
    // DatLoop2dByte / DatLoop2dBit / DatLoop2dVrefIncr
    // Indexed by Vref stride index

    uint16_t DatLoop2dDbyteBitFail[10];  // Byte Offset: 0x3538 / CSR Address: 0x59a9c
    // Array indicates if DBYTE bits have failed the 'MinEyeWidth' check at any VREF value.
    // Index of array is the DBYTE, and bits [8:0] coorespond to the bits of a particular DBYTE.
    // Bits [15:9] are RESERVED.
    // If (DatLoop2dDbyteBitFail[DBYTE] & (1 << bit)) equals:
    //     0 - The cooresponding DBYTE bit passed all eye width checks accross tested VREFs
    //     1 - The cooresponding DBYTE bit failed an eye width check on one or more tested VREFs


    /////////////////////////////////////////
    // Data Loopback 1D Test Results for Analog Summers(4-tap DFE) test
    /////////////////////////////////////////

    uint16_t FourTapDfe_DFEResults[10][4];  // Byte Offset: 0x354c / CSR Address: 0x59aa6
    // Analog Summers(4-tap DFE) test results indexed by Dbyte and Lane.
    // Defined as FourTapDfe_DFEResults[DbyteNumber][SummerNumber].
    // FourTapDfe_DFEResults[DbyteNumber][7:0] are DQ[7:0] for DbyteNumber;
    // FourTapDfe_DFEResults[DbyteNumber][8] is DM/DBI for DbyteNumber;
    // FourTapDfe_DFEResults[DbyteNumber][15:9] are not care.
    // The corresponding bit will be set to 1 if Analog Summers(4-tap DFE) test passed, and 0 if Analog Summers(4-tap DFE) tests failed.
    // If the Analog Summers(4-tap DFE) test is not enabled , the value of the FourTapDfe_DFEResults[DbyteNumber] would be 0xFFFF.
    uint8_t  Vref_dfe_0[10][9][4];  // Byte Offset: 0x359c / CSR Address: 0x59ace
    // Analog Summers(4-tap DFE) test debug information indexed by Dbyte ,Lane and DFE tap summer.
    // Defined as Vref_dfe_0[DbyteNumber][LaneNumber][SummerNumber].
    // Only used for 4-tap DFE.
    uint8_t  Vref_dfe_pos[10][9][4];  // Byte Offset: 0x3704 / CSR Address: 0x59b82
    // Analog Summers(4-tap DFE) test debug information indexed by Dbyte ,Lane and DFE tap summer.
    // Defined as Vref_dfe_pos[DbyteNumber][LaneNumber][SummerNumber].
    // Only used for 4-tap DFE.
    uint8_t  Vref_dfe_neg[10][9][4];  // Byte Offset: 0x386c / CSR Address: 0x59c36
    // Analog Summers(4-tap DFE) test debug information indexed by Dbyte, Lane and DFE tap summer.
    // Defined as Vref_dfe_neg[DbyteNumber][LaneNumber][SummerNumber].
    // Only used for 4-tap DFE.
    uint32_t Alignment8RSVD;  // Byte Offset: 0x39d4 / CSR Address: 0x59cea
    // Reserved field for alignment, will not be written/read.

} __attribute__ ((packed)) PMU_SMB_ATE_t; //Structure size = 14808 Bytes

#endif
