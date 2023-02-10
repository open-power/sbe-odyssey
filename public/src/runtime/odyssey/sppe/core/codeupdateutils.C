/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/odyssey/sppe/core/codeupdateutils.C $      */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2016,2023                        */
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
#include "cmnglobals.H"
#include "sbetrace.H"
#include "odysseylink.H"
#include "sberegaccess.H"
#include "codeupdateutils.H"

void getPartitionInfo(uint8_t &o_runningPartition,
                      uint8_t &o_nonRunningPartition)
{
    #define SBE_FUNC " getPartitionInfo "
    SBE_ENTER(SBE_FUNC);

    //to get running partition
    o_runningPartition=SbeRegAccess::theSbeRegAccess().getBootSelection();

    // to get non-running partition
    if (o_runningPartition == PARTITION_0_INDEX)
    {
        // if booting from primary then making non-running is secondary
        o_nonRunningPartition = PARTITION_1_INDEX;
    }
    else if (o_runningPartition == PARTITION_1_INDEX)
    {
        // if booting from secondary then making non-running is primary
        o_nonRunningPartition = PARTITION_0_INDEX;
    }
    else if (o_runningPartition == GOLDEN_PARTITION_INDEX)
    {
        // if booting from golden then making non-running is primary
        o_nonRunningPartition = PARTITION_0_INDEX;
    }

    SBE_EXIT(SBE_FUNC);
    #undef SBE_FUNC
}

void getPartitionAddress(const uint8_t i_partition, uint32_t &o_partitionStartAddress)
{
    #define SBE_FUNC " getPartitionAddress "
    SBE_ENTER(SBE_FUNC);

    // Get the partition start offset
    o_partitionStartAddress = getAbsPartitionAddr(i_partition);
    SBE_INFO(SBE_FUNC "Partition Start Address:[0x%08x]",
             o_partitionStartAddress);

    SBE_EXIT(SBE_FUNC);
    #undef SBE_FUNC
}

