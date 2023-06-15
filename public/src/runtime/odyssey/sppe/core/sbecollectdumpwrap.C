/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/odyssey/sppe/core/sbecollectdumpwrap.C $   */
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
#include "sbecollectdump.H"

using namespace fapi2;

#define DUMP_HDR_CHIP_TYPE_TO_ENCODE 0x7

/****************** getDumpHdrChipTypeToEncode *****************/
uint8_t sbeCollectDump::getDumpHdrChipTypeToEncode()
{
    return (uint8_t)DUMP_HDR_CHIP_TYPE_TO_ENCODE;
}

/************************ getTargetList ***********************/
void sbeCollectDump::getTargetList(std::vector<fapi2::plat_target_handle_t> &o_targetList)
{
    #define SBE_FUNC "getTargetList"
    SBE_ENTER(SBE_FUNC);
    fapi2::Target<fapi2::TARGET_TYPE_OCMB_CHIP> l_chip =  g_platTarget->plat_getChipTarget();
    switch(iv_tocRow.hdctHeader.chipUnitType)
    {
        case CHIP_UNIT_TYPE_CHIP:
        case CHIP_UNIT_TYPE_UNKNOWN:
        {
            o_targetList.push_back(l_chip);
            SBE_DEBUG(SBE_FUNC "PROC: [0x%08X]", l_chip.get());
            break;
        }
        case CHIP_UNIT_TYPE_PERV:
        {
            for( auto& target : l_chip.getChildren<fapi2::TARGET_TYPE_PERV>(
                static_cast<fapi2::TargetFilter>(
                        fapi2::TARGET_FILTER_TP     |
                        fapi2::TARGET_FILTER_MC))
                )
            {
                o_targetList.push_back(target);
                SBE_DEBUG(SBE_FUNC " PERV: [0x%08X]", target.get());
            }
            break;
        }
        case CHIP_UNIT_TYPE_MEM_PORT:
        {
            for( auto& target : l_chip.getChildren<fapi2::TARGET_TYPE_MEM_PORT>())
            {
                o_targetList.push_back(target);
            }
            break;
        }
        default:
        {
            SBE_ERROR(SBE_FUNC " PROC: [0x%08X] command Id [0x%02X] Type[0x%02X] not supported",
                        l_chip.get(),
                        (uint8_t)iv_tocRow.hdctHeader.chipUnitType,
                        (uint8_t)iv_tocRow.hdctHeader.cmdType);
            break;
        }
    }
    #undef SBE_FUNC
}

void sbeCollectDump::getAbsoluteAddressForRing(
                                const fapi2::plat_target_handle_t i_tgtHndl,
                                const uint32_t i_relativeAddr,
                                uint32_t &o_absoluteAddr)
{
    #define SBE_FUNC " getAbsoluteAddressForRing "
    SBE_ENTER(SBE_FUNC);
    do
    {
        switch(i_tgtHndl.getTargetType())
        {
            case LOG_TARGET_TYPE_MC:
            {
                uint32_t chipletNum = i_tgtHndl.getChipletNumber();
                chipletNum = chipletNum << 24;
                o_absoluteAddr = (i_relativeAddr & 0x00FFFFFF) | chipletNum;
                break;
            }

            default:
            {
                o_absoluteAddr = i_relativeAddr;
                break;
            }
        }
        SBE_DEBUG(SBE_FUNC "Target, Relative Addr, Absolute Addr is 0x%08X, 0x%08X 0x%08X",
                           i_tgtHndl.getPlatTargetType(),
                           i_relativeAddr,
                           o_absoluteAddr);
    }while(0);
    SBE_EXIT(SBE_FUNC);
    #undef SBE_FUNC
}