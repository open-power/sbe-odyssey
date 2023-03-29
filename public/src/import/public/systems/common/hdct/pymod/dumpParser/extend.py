# IBM_PROLOG_BEGIN_TAG
# This is an automatically generated prolog.
#
# $Source: public/src/import/public/systems/common/hdct/pymod/dumpParser/extend.py $
#
# OpenPOWER sbe Project
#
# Contributors Listed Below - COPYRIGHT 2023
# [+] International Business Machines Corp.
#
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
# implied. See the License for the specific language governing
# permissions and limitations under the License.
#
# IBM_PROLOG_END_TAG
# Jason Albert - created 09/22/2018
# Python module to define HDCT parse function

# Defines the HDCTEntry class which holds the parsed representation
# of an HDCT line.  Those are then stored in a list to represent
# the whole HDCT
# Also included is the HDCT parser along with all the supporting
# functions that requires

############################################################
# Imports - Imports - Imports - Imports - Imports - Imports
############################################################
import os
import sys
import re
from output import out
import ecmd
import hwFunctions
import copy

############################################################
# Variables - Variables - Variables - Variables - Variables
############################################################

############################################################
# Function - Functions - Functions - Functions - Functions
############################################################

# Supporting functions for the createEntryTargets function
def setTargetNumState(field, hdctEntry, targets):
    """
    Takes in a field value and creates targets with that value set
    """
    fieldState = field + "State"
    newTargets = list() # List to store our newly updated targets
    # Loop through the targets, expanding it at the field passed in
    for target in targets:
        # Extract the field from the HDCTEntry and check if it is valid
        hdctTargetValue = getattr(hdctEntry, field)
        if (hdctTargetValue):
            # Valid entry, handle the different ways we might setup the target
            if ((hdctTargetValue == "all") or (hdctTargetValue == "defall")):
                setattr(target, fieldState, ecmd.ECMD_TARGET_FIELD_WILDCARD)
                newTargets.append(ecmd.ecmdChipTarget(target))
            elif (".." in hdctTargetValue):
                # Dealing with a range.  Get the numbers on either end
                # Our usecase is end to end inclusive, but python range is not - handle that
                rangeVals = hdctTargetValue.split("..")
                rangeStart = int(rangeVals[0])
                rangeEnd = int(rangeVals[1]) + 1
                for i in range(rangeStart, rangeEnd):
                    setattr(target, fieldState, ecmd.ECMD_TARGET_FIELD_VALID)
                    setattr(target, field, i)
                    newTargets.append(ecmd.ecmdChipTarget(target))
            else:
                # This should be a single valid number
                setattr(target, field, int(hdctTargetValue))
                setattr(target, fieldState, ecmd.ECMD_TARGET_FIELD_VALID)
                newTargets.append(ecmd.ecmdChipTarget(target))
        else:
            # Field not valid, so set it to unused
            setattr(target, fieldState, ecmd.ECMD_TARGET_FIELD_UNUSED)
            newTargets.append(ecmd.ecmdChipTarget(target))

    return newTargets

def setTargetStringState(field, hdctEntry, targets):
    """
    Takes in a field value and creates targets with that value set
    """
    fieldState = field + "State"
    newTargets = list() # List to store our newly updated targets
    # Loop through the targets, expanding it at the field passed in
    for target in targets:
        # We will either have a chipType or chipUnitType that is or is not valid
        hdctTargetValue = getattr(hdctEntry, field)
        if (hdctTargetValue):
            setattr(target, field, hdctTargetValue)
            state = ecmd.ECMD_TARGET_FIELD_VALID
        else:
            state = ecmd.ECMD_TARGET_FIELD_UNUSED

        setattr(target, fieldState, state)
        newTargets.append(ecmd.ecmdChipTarget(target))

    return newTargets

def createEntryTargets(hdctEntries, cmdlineTargets):
    """
    This function expands the HDCTEntry target params into actual ecmdTargets
    """
    errorCount = 0
    targetCount = 0
    # Track all the chip level target ECs read so we can write them in the stats file
    targetData = dict()
    noTargets = list()

    for hdctEntry in hdctEntries:
        # Setup the right function to call for each of the HDCT entries
        # This is straight forward, get it from the dictionary
        hdctEntry.function = hwFunctions.names[hdctEntry.command]

        # Create the correct ecmd data structure in the funcargs based on the command
        if (hdctEntry.command[0:3] == "get"):
            if ((hdctEntry.command == "getblock") or (hdctEntry.command == "getfastarray")):
                hdctEntry.funcargs["o_data"] = ecmd.ecmdDataBufferVector()
            elif (hdctEntry.command == "gettracearray"):
                # Do stop start default value - this is extra for gettracearray
                hdctEntry.funcargs["i_doTraceStopStart"] = ecmd.ECMD_TRACE_IGNORE_MUX
                hdctEntry.funcargs["o_data"] = ecmd.ecmdNameVectorEntry()
            elif (hdctEntry.command == "getsram"):
                # The name of the arg in the function prototype changed to better represent what it is in >= 15.0
                # This is what the main parser supports.  For ecmd 14.x, we need to change the name back
                if (not ecmd.ecmdQueryVersionGreater("15.0")):
                    hdctEntry.funcargs["i_channel"] = hdctEntry.funcargs.pop("i_mode")
                hdctEntry.funcargs["o_data"] = ecmd.ecmdDataBuffer()
            else:
                hdctEntry.funcargs["o_data"] = ecmd.ecmdDataBuffer()
        elif (hdctEntry.command[0:3] == "put"):
            # For put commands, we need to take the data value from the HDCT
            # Then create an ecmdDataBuffer and insert that data into it
            if (hdctEntry.command == "putscom"):
                i_data = hdctEntry.funcargs["i_data"]
                hdctEntry.funcargs["i_data"] = ecmd.ecmdDataBuffer(64)
                hdctEntry.funcargs["i_data"].insertFromHexLeft(i_data)
            elif (hdctEntry.command == "putcfam"):
                i_data = hdctEntry.funcargs["i_data"]
                hdctEntry.funcargs["i_data"] = ecmd.ecmdDataBuffer(32)
                hdctEntry.funcargs["i_data"].insertFromHexLeft(i_data)
        else:
            # Nothing to do.. commands like stopclocks which don't need data creation
            pass

        # Setup the target state based on the values in the HDCT entry
        # What this does is start with a single undefined target in the list
        # With each successive call, it updates the level of the target passed in
        # Each time it returns a new list created in the function
        # So if states like unused or wildcard are set, the size of the list coming
        # out will equal the size of the list going in.
        # If the field has a range value like 0..5, there would be new entries coming
        # out for each of the individual values in that range
        targets = [ecmd.ecmdChipTarget()]
        targets = setTargetNumState("cage", hdctEntry, targets)
        targets = setTargetNumState("node", hdctEntry, targets)
        targets = setTargetNumState("slot", hdctEntry, targets)
        targets = setTargetStringState("chipType", hdctEntry, targets)
        targets = setTargetNumState("pos", hdctEntry, targets)
        targets = setTargetStringState("chipUnitType", hdctEntry, targets)
        targets = setTargetNumState("chipUnitNum", hdctEntry, targets)
        targets = setTargetNumState("thread", hdctEntry, targets)

        # Loop over the targets established above
        # This will determine if the values from the HDCT above are actually configured in this system
        for baseTarget in targets:
            # Create and intialize the looper
            looperData = ecmd.ecmdLooperData()
            target = ecmd.ecmdChipTarget(baseTarget)
            rc = ecmd.ecmdConfigLooperInit(target, ecmd.ECMD_ALL_TARGETS_LOOP, looperData)
            if (rc):
                out.error("Problem calling ecmdConfigLooperInit with %s" % ecmd.ecmdWriteTarget(target, ecmd.ECMD_DISPLAY_TARGET_HYBRID))
                errorCount += 1
                continue

            # Loop through all the configured targets
            while (ecmd.ecmdConfigLooperNext(target, looperData)):

                # If the user gave target filter list, only try to add if it exists in the list
                if (cmdlineTargets) and (target not in cmdlineTargets):
                    continue

                # Create the target string for use throughout
                targetString = ecmd.ecmdWriteTarget(target, ecmd.ECMD_DISPLAY_TARGET_HYBRID)

                # If we are a chipunit level target, we need to also get the chip level target data
                if (target.chipUnitTypeState == ecmd.ECMD_TARGET_FIELD_VALID):
                    chipTarget = ecmd.ecmdChipTarget(target)
                    chipTarget.chipUnitTypeState = ecmd.ECMD_TARGET_FIELD_UNUSED
                    chipTarget.chipUnitType = ""
                    chipTarget.chipUnitNumState = ecmd.ECMD_TARGET_FIELD_UNUSED
                    chipTarget.chipUnitNum = 0
                    chipTarget.threadState = ecmd.ECMD_TARGET_FIELD_UNUSED
                    chipTarget.thread = 0
                    try:
                        chipData = targetData[chipTarget]
                    except:
                        chipData = ecmd.ecmdChipData()
                        rc = ecmd.ecmdGetChipData(chipTarget, chipData)
                        targetData[ecmd.ecmdChipTarget(chipTarget)] = chipData

                # The actual target is done second so if a chip is added above it will be
                # first in the list
                # Check the EC level of the chip to determine if it should be included
                # Get the chipData if we don't have it, use the cache if we do
                try:
                    chipData = targetData[target]
                except:
                    chipData = ecmd.ecmdChipData()
                    rc = ecmd.ecmdGetChipData(target, chipData)
                    if (rc):
                        out.error("Problem retrieving the chipData for %s on target: %s rc: %d" %
                                  (hdctEntry.command, targetString, rc))
                        errorCount += 1
                        continue
                    targetData[ecmd.ecmdChipTarget(target)] = chipData

                # Do we have ec+ in the entry
                if (hdctEntry.ecPlus):
                    # Make sure that we are not running on a downlevel system
                    if (hdctEntry.ec > chipData.chipEc):
                        out.info("EC in hdct entry: %X, is higher than hw EC: %X. Skipping the entry for %s %s on target: %s" %
                                 (hdctEntry.ec, chipData.chipEc, hdctEntry.command, hdctEntry.commandArg, targetString))
                        continue
                # If only ec without + is specified in hdctEntry, we need to make sure that it matches
                elif (hdctEntry.ec != chipData.chipEc):
                    out.info("EC in hdct entry: %X, doesn't match with allowed hw EC: %X. Skipping the entry for %s %s on target: %s" %
                             (hdctEntry.ec, chipData.chipEc, hdctEntry.command, hdctEntry.commandArg, targetString))
                    continue

                # If we made it here, it is okay to add
                hdctEntry.targets.append(ecmd.ecmdChipTarget(target))

        # If we found no targets for this command, log that enty.  Some callers might care.
        if not hdctEntry.targets:
            noTargets.append(hdctEntry)

        # Accumulate total number of targets for stats down the road
        targetCount += len(hdctEntry.targets)

    return (errorCount, targetCount, targetData, noTargets)
