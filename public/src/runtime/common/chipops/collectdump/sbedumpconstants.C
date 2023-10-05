/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/common/chipops/collectdump/sbedumpconstants.C $ */
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
#include "sbedumpconstants.H"
#include "target_types.H"

DUMPTYPEMAP dumpTypeMap[5] = {
    {SBE_DUMP_TYPE_SCS   , DUMP_TYPE_SCS  },
    {SBE_DUMP_TYPE_MPIPL , DUMP_TYPE_MPIPL},
    {SBE_DUMP_TYPE_PERF  , DUMP_TYPE_PERF },
    {SBE_DUMP_TYPE_HB    , DUMP_TYPE_HB   }
};

/**
 * @brief Chip Unit type map with Log target type
 *
 * @note: In case user try the changes @ref enum chipUnitTypes should changes
 *        respective log target type here
*/
uint8_t chipUnitTypeToLogTargetTypeMap[ ] = {
    INVALID_DUMP_TARGET,
    fapi2::LOG_TARGET_TYPE_PROC_CHIP,
    fapi2::LOG_TARGET_TYPE_MCS,
    fapi2::LOG_TARGET_TYPE_CORE,
    fapi2::LOG_TARGET_TYPE_EQ,
    fapi2::LOG_TARGET_TYPE_EX,
    fapi2::LOG_TARGET_TYPE_PERV,
    fapi2::LOG_TARGET_TYPE_MCBIST,
    fapi2::LOG_TARGET_TYPE_SYSTEM,
    fapi2::LOG_TARGET_TYPE_PHB,
    fapi2::LOG_TARGET_TYPE_MI,
    fapi2::LOG_TARGET_TYPE_MC,
    fapi2::LOG_TARGET_TYPE_PAUC,
    fapi2::LOG_TARGET_TYPE_IOHS,
    fapi2::LOG_TARGET_TYPE_NMMU,
    fapi2::LOG_TARGET_TYPE_CAPP,
    fapi2::LOG_TARGET_TYPE_XBUS,
    fapi2::LOG_TARGET_TYPE_MCA,
    INVALID_DUMP_TARGET,
    fapi2::LOG_TARGET_TYPE_PEC,
    fapi2::LOG_TARGET_TYPE_MBA,
    fapi2::LOG_TARGET_TYPE_DMI,
    INVALID_DUMP_TARGET,
    fapi2::LOG_TARGET_TYPE_MCC,
    fapi2::LOG_TARGET_TYPE_OMIC,
    fapi2::LOG_TARGET_TYPE_PAU,
    fapi2::LOG_TARGET_TYPE_OMI,
    fapi2::LOG_TARGET_TYPE_OCMB_CHIP,
    fapi2::LOG_TARGET_TYPE_MEM_PORT,
};

//Index mapped table wrt commandTypes enum
//for storing the size of the genericHdctRow_t
//for a particular command type.
//Write NULL for commands that are not used in HDCT.txt
size_t genericHdctRowSize_table[NO_OF_CMD_TYPES] = {
    CMD_TYPE_NOT_USED,
    genericHdctRowSize(genericHdctRow_t, cmdGetScom),   // 0x01
    genericHdctRowSize(genericHdctRow_t, cmdPutScom),   // 0x02
    CMD_TYPE_NOT_USED,                                  // 0x03
    CMD_TYPE_NOT_USED,                                  // 0x04
    genericHdctRowSize(genericHdctRow_t,cmdGetRing),    // 0x05
    CMD_TYPE_NOT_USED,                                  // 0x06
    genericHdctRowSize(genericHdctRow_t,cmdStopClocks), // 0x07
    CMD_TYPE_NOT_USED,                                  // 0x08
    CMD_TYPE_NOT_USED,                                  // 0x09
    CMD_TYPE_NOT_USED,                                  // 0x0A
    genericHdctRowSize(genericHdctRow_t,cmdTraceArray), // 0x0B
    genericHdctRowSize(genericHdctRow_t,cmdFastArray),  // 0x0C
    CMD_TYPE_NOT_USED,                                  // 0x0D
    genericHdctRowSize(genericHdctRow_t,cmdGetSram)     // 0x0E
};

uint16_t getEquivDumpType(uint8_t reqDumpType)
{
    #define SBE_FUNC " getEquivDumpType "

    uint8_t count = (sizeof(dumpTypeMap)/sizeof(dumpTypeMap[0]));
    uint16_t hdctDumpTypeMap = 0;
    for(uint8_t i = 0; i < count; i++)
    {
       if( dumpTypeMap[i].dumpRequestReceived == reqDumpType)
       {
            //Return as per HDCT.bin dump type format.
            hdctDumpTypeMap = (1 << dumpTypeMap[i].hdctDumpType);
       }
    }

    return hdctDumpTypeMap;
    #undef SBE_FUNC
}
