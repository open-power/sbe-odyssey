# IBM_PROLOG_BEGIN_TAG
# This is an automatically generated prolog.
#
# $Source: public/src/import/public/systems/common/hdct/pymod/dumpParser/base.py $
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
import copy
from dumpConstants import validDumpTypes
from dumpConstants import chipUnitTypeToEncode
from dumpConstants import chipTypeToEncode
from dumpConstants import commonChipTypeNames

out.setConsoleLevel(out.levels.DEBUG)

############################################################
# Variables - Variables - Variables - Variables - Variables
############################################################

############################################################
# Classes - Classes - Classes - Classes - Classes - Classes
############################################################
class HDCTEntry():
    """
    Python usable representation of a line from HDCT
    """
    def __init__(self, dumpFormat):
        self.dumpFormat   = dumpFormat # For determining some default values
        if (self.dumpFormat == "ebmc"):
            tgtDefault = "defall" # Default all - to differentiate from a given "all"
        else:
            tgtDefault = None

        self.dumpTypes    = None
        self.command      = None
        self.ec           = None
        self.ecPlus       = None
        self.system       = None
        self.notSbe       = False
        self.maxOnly      = False
        self.cage         = tgtDefault
        self.node         = tgtDefault
        # The HDCT doesn't require slot be specified
        # Set the default to all so we setup the loopers properly later
        self.slot         = "all"
        self.chipType     = None
        self.pos          = tgtDefault
        self.chipUnitType = None
        self.chipUnitNum  = tgtDefault
        self.thread       = None
        # These args are used later in the run for actual execution of the dump
        self.function     = None    # Name of function to invoke to do the dump
        self.funcargs     = dict()  # All of the args to go into the function
        self.targets      = list()  # All of the targets the function will be called against
        # This arg is used to track what item from funcargs was the address/ring the command was against
        # This is used for stats gathering
        self.commandArg = None
        # The original line from the HDCT for reference
        self.line = None

    def dump(self):
        """
        Print all the values entry to the screen for debug, etc..
        """
        out.debug("dumpTypes: %s" % self.dumpTypes)
        out.debug("command: %s" % self.command)
        out.debug("ec: %s" % self.ec)
        out.debug("system: %s" % self.system)
        out.debug("notsbe: %s" % self.notSbe)
        out.debug("maxonly: %s" % self.maxOnly)
        out.debug("cage: %s" % self.cage)
        out.debug("node: %s" % self.node)
        out.debug("slot: %s" % self.slot)
        out.debug("chipType: %s" % self.chipType)
        out.debug("pos: %s" % self.pos)
        out.debug("chipUnitType: %s" % self.chipUnitType)
        out.debug("chipUnitNum: %s" % self.chipUnitNum)
        out.debug("thread: %s" % self.thread)

    def verify(self):
        """
        Verify all the entries to ensure its been set properly
        """
        # Error setups
        errorMsgs = list()

        # Before we verify, lets cleanup a couple state things based on the dump type
        # We will use the presence or not of the chipUnitType to determine if chipUnitNum
        # should be set.  If it's still in the default all, will reset
        # If set to "all", that means it was specified and assume the user was right
        if (self.dumpFormat == "ebmc"):
            if ((self.chipUnitType == None) and (self.chipUnitNum == "defall")):
                self.chipUnitNum = None

        if not self.dumpTypes:
            errorMsgs.append("No dump types set!")
        if not self.command:
            errorMsgs.append("No command set!")
        if not self.funcargs:
            errorMsgs.append("No funcargs set!")
        if not self.ec:
            errorMsgs.append("No ec set!")
        #if not self.system:
        #    errorMsgs.append("No system set!")
        if not self.cage:
            errorMsgs.append("No cage arg (-k) found!")
        if not self.node:
            errorMsgs.append("No node arg (-n) found!")
        if not self.slot:
            errorMsgs.append("No slot arg (-s) found!")
        if not self.chipType:
            errorMsgs.append("No chip found!")
        if (self.chipType and ((self.chipType not in chipTypeToEncode) and (self.chipType not in commonChipTypeNames))):
            errorMsgs.append("chipType '%s' not valid!" % self.chipType)
        if not self.pos:
            errorMsgs.append("No pos arg (-p) found!")
        if (self.chipUnitType and (self.chipUnitType not in chipUnitTypeToEncode)):
            errorMsgs.append("chipUnitType '%s' not valid!" % self.chipUnitType)

        # Some additional conditional checks at the end
        # chipUnitType/chipUnitNum are not required to be set, since it could be just pos level
        # Put in check to ensure if one is set, the other is too
        if (not self.chipUnitType and self.chipUnitNum):
            errorMsgs.append("A chipUnitType was not given, but -c was!")
        if (self.chipUnitType and not self.chipUnitNum):
            errorMsgs.append("A chipUnitType was given, but no -c was!")

        return errorMsgs

############################################################
# Function - Functions - Functions - Functions - Functions
############################################################
def parseChipString(string):
    """
    Parse out a chip string to look for a chip unit
    """
    chip = None
    chipUnitType = None

    if "." not in string:
        chip = string
    else:
        chip = string.split(".")[0]
        chipUnitType = string.split(".")[1]

    return(chip, chipUnitType)

def parseHDCT(fileName, dumpFormat):
    """
    Function to parse the HDCT and return it transformed it
    python data types to operate on
    """
    # Our two return types
    totalErrors = 0
    hdctEntries = list()
    # Track number of good/bad entries found
    entriesGood = 0
    entriesBad = 0

    # We want to track our line numbers so we can provide that to the user
    lineNumber = 0

    # loopCount indicates if we should gather loop entries for unrolling
    loopCount = 0

    # Open the HDCT file
    file = open(fileName, "r")

    # Loop through the file and process it
    # Along the way, turn it into data structures we'll use
    # downstream
    # A sample HDCT line we'll use to show the text processing as we go
    # line: HWPROC | CS | OTHER | HB  getring pu occ_fure  -kall -nall -pall ec 10+ # some comment
    for line in file:
        # Save off the original line, complete with comments
        origLine = line
        # Reset each time thru
        errorMsgs = list()

        # Cleanup the \n at the end of the line
        line = line.strip('\n')

        # Keep the original line for error printing and track line numbers
        origLine = line
        lineNumber += 1

        # Optionally print the line to the screen
        out.debug("HDCT line : %s" % line)

        # Get rid of comments before we process the rest
        # Comments can be a couple different characters, so use regex
        match = re.search('\*|#', line)
        # If we get no match, no comment and nothing to do
        # If we have a match, drop in and save all text befor the comment
        if (match):
            line = line[:match.start()]

        # If our line is now empty, no need to keep going
        if (not line):
            continue

        # Before we process the standard lines, look for special keywords
        if (line.startswith("START_LOOP")):
            # The other part of this line is the count
            loopCount = int(line.split()[1])
            # Define our variable to hold what we find in the loop, ensures clean start
            loopEntries = list()

            # Done processing this line
            continue

        if (line.startswith("END_LOOP")):
            # Flush through what we've accumulated
            for i in range(loopCount):
                for loopEntry in loopEntries:
                    # deepcopy over the loopEntry so we have a unique instance to update
                    newEntry = copy.deepcopy(loopEntry)
                    # If an entry in the list is a block, update the iteration inside it
                    if (loopEntry.command == "getblock"):
                        blockEntry = newEntry.funcargs["i_metadataBlockEntry"].replace("ROCI", str(i))
                        newEntry.funcargs["i_metadataBlockEntry"] = blockEntry
                        newEntry.commandArg = blockEntry
                    elif (loopEntry.command == "gettracearray"):
                        newEntry.commandArg = newEntry.commandArg.replace("ROCI", str(i))
                    hdctEntries.append(newEntry)
                    entriesGood += 1

            # Reset loopCount to 0 so the code knows we are done
            loopCount = 0

            # Done processing this line
            continue

        # Comments are cleaned up, now break up our line to
        # get the dumpTypes and commandParts
        # line: HWPROC | CS | OTHER | HB  getring pu occ_fure  -kall -nall -pall ec 10+
        parts = line.split("|")

        # All but the last array element is a dump types
        dumpTypes = parts[:-1]
        # The last one is a dump type plus the command to do
        # Do some special handling on that to get the dump type
        # back in the right place
        commandParts = parts[-1].split()
        # Put our final dump type into the dump
        dumpTypes.append(commandParts[0])
        # Reset the commandParts to ditch the dumpType
        commandParts = commandParts[1:]

        # Cleanup any stray white space
        # commandParts is in good shape from it's initial split
        # dumpTypes needs some help, remove all white space on the elements
        dumpTypes = ["".join(type.split()) for type in dumpTypes]
        # Remove any empty entries from the list.  This can happen with syntax like:
        # HWPROC | CS |   | OTHER
        dumpTypes = [i for i in dumpTypes if i]

        # From our original line, we now have
        # dumpTypes = HWPROC | CS | OTHER | HB
        # commandParts = getring pu occ_fure  -kall -nall -pall ec 10+
        out.debug("dumpTypes %s" % dumpTypes)
        out.debug("commandParts %s" % commandParts)

        # Now validate the values given for dumpTypes in the HDCT are in the valid list
        # Anything that ends up in the list are the unsupported ones
        badDumpTypeValues = [i for i in dumpTypes if i not in (validDumpTypes[dumpFormat]).keys()]
        if (badDumpTypeValues):
            errorMsgs.append("The following were not valid dump type values: %s" % badDumpTypeValues)

        # Create our hdctEntry we are going to push onto the list
        entry = HDCTEntry(dumpFormat)
        # Save the original line in case someone needs it
        entry.line = origLine

        # The dump types are fully processed above, assign them into the struct
        entry.dumpTypes = dumpTypes

        # Pull out all of the common optional args
        allFound = False
        idx = 0
        while (idx < len(commandParts)):
            if (commandParts[idx] == "-all"):
                allFound = True
                entry.cage = "all"
                entry.node = "all"
                entry.pos = "all"
                entry.chipUnitNum = "all"
                del(commandParts[idx])
                continue
            elif (commandParts[idx].startswith("-k")):
                entry.cage = commandParts[idx][2:]
                del(commandParts[idx])
                continue
            elif (commandParts[idx].startswith("-n")):
                entry.node = commandParts[idx][2:]
                del(commandParts[idx])
                continue
            elif (commandParts[idx].startswith("-p")):
                entry.pos = commandParts[idx][2:]
                del(commandParts[idx])
                continue
            elif (commandParts[idx].startswith("-c")):
                entry.chipUnitNum = commandParts[idx][2:]
                del(commandParts[idx])
                continue
            elif (commandParts[idx].startswith("-t")):
                entry.thread = commandParts[idx][2:]
                del(commandParts[idx])
                continue
            elif (commandParts[idx] == "ec"):
                del(commandParts[idx])
                # Check if we have + in the ec field. eg: ec 10+
                entry.ecPlus = bool(re.search('\+', commandParts[idx]))
                # Grab the ec value from the ec field in hdctEntry
                entry.ec = int(commandParts[idx].split("+")[0], 16)
                del(commandParts[idx])
                continue
            elif (commandParts[idx] == "sys"):
                del(commandParts[idx])
                if (commandParts[idx] == "ne"):
                    del(commandParts[idx])
                    entry.system = commandParts[idx]
                    del(commandParts[idx])
                    continue
            elif (commandParts[idx] == "notsbe"):
                del(commandParts[idx])
                entry.notSbe = True
                continue
            elif (commandParts[idx] == "maxonly"):
                del(commandParts[idx])
                entry.maxOnly = True
                continue

            # Increment our index
            idx += 1

        # The first entry of commandParts will be the command to run
        # From there, parse each set of args specifically for the command
        try:
            entry.command = commandParts.pop(0)
        except IndexError:
            errorMsgs.append("Command to run not found!")

        # If case for all command types, error for unknown at end
        if (entry.command == "getscom"):
            # Chip Type
            try:
                (entry.chipType, entry.chipUnitType) = parseChipString(commandParts.pop(0))
            except IndexError:
                errorMsgs.append("Unable to get chip/chipType for getscom!")
            # Scom address
            try:
                entry.funcargs["i_address"] = int(commandParts.pop(0), 16)
            except IndexError:
                errorMsgs.append("Unable to get address for getscom!")
            entry.commandArg = "%X" % int(entry.funcargs["i_address"])
        elif (entry.command == "putscom"):
            # Chip Type
            try:
                (entry.chipType, entry.chipUnitType) = parseChipString(commandParts.pop(0))
            except IndexError:
                errorMsgs.append("Unable to get chip/chipType for putscom!")
            # Scom address
            try:
                entry.funcargs["i_address"] = int(commandParts.pop(0), 16)
            except IndexError:
                errorMsgs.append("Unable to get address for putscom!")
            entry.commandArg = "%X" % int(entry.funcargs["i_address"])
            try:
                entry.funcargs["i_data"] = commandParts.pop(0)
            except IndexError:
                errorMsgs.append("Unable to get data for putscom!")
            # If there is a next arg, see if the next arg happens to be -band/-bor and setup
            # The forward checks of the list means we don't need to try/except the pops
            if (commandParts and (commandParts[0] == "-band")):
                entry.funcargs["i_band"] = True
                commandParts.pop(0)
            if (commandParts and (commandParts[0] == "-bor")):
                entry.funcargs["i_bor"] = True
                commandParts.pop(0)
        elif ((dumpFormat == "fsp") and (entry.command == "getcfam")):
            # Chip Type
            try:
                (entry.chipType, entry.chipUnitType) = parseChipString(commandParts.pop(0))
            except IndexError:
                errorMsgs.append("Unable to get chip/chipType for getcfam!")
            # cfam address
            try:
                entry.funcargs["i_address"] = int(commandParts.pop(0), 16)
            except IndexError:
                errorMsgs.append("Unable to get address for getcfam!")
            entry.commandArg = "%X" % int(entry.funcargs["i_address"])
        elif ((dumpFormat == "fsp") and (entry.command == "putcfam")):
            # Chip Type
            try:
                (entry.chipType, entry.chipUnitType) = parseChipString(commandParts.pop(0))
            except IndexError:
                errorMsgs.append("Unable to get chip/chipType for putcfam!")
            # cfam address
            try:
                entry.funcargs["i_address"] = int(commandParts.pop(0), 16)
            except IndexError:
                errorMsgs.append("Unable to get address for putcfam!")
            entry.commandArg = "%X" % int(entry.funcargs["i_address"])
            try:
                entry.funcargs["i_data"] = commandParts.pop(0)
            except IndexError:
                errorMsgs.append("Unable to get data for putcfam!")
            # If there is a next arg, see if the next arg happens to be -band/-bor and setup
            # The forward checks of the list means we don't need to try/except the pops
            if (commandParts and (commandParts[0] == "-band")):
                entry.funcargs["i_band"] = True
                commandParts.pop(0)
            if (commandParts and (commandParts[0] == "-bor")):
                entry.funcargs["i_bor"] = True
                commandParts.pop(0)
        elif (entry.command == "getring"):
            # Chip Type
            try:
                (entry.chipType, entry.chipUnitType) = parseChipString(commandParts.pop(0))
            except IndexError:
                errorMsgs.append("Unable to get chip/chipType for getring!")
            # Ring name
            try:
                entry.funcargs["i_ringName"] = commandParts.pop(0)
            except IndexError:
                errorMsgs.append("Unable to get ringname for getring!")
            entry.commandArg = entry.funcargs["i_ringName"]
        elif (entry.command == "stopclocks"):
            # Chip Type
            try:
                (entry.chipType, entry.chipUnitType) = parseChipString(commandParts.pop(0))
            except IndexError:
                errorMsgs.append("Unable to get chip/chipType for stopclocks!")
            # What is the Clock Domain we want to stop
            try:
                entry.funcargs["i_clockDomain"] = commandParts.pop(0)
            except IndexError:
                errorMsgs.append("Unable to get clockdomain for stopclocks!")
            entry.commandArg = entry.funcargs["i_clockDomain"]
            # Do we want to force clocks off regardless of current state?
            # The forward checks of the list means we don't need to try/except the pops
            if (commandParts and (commandParts[0] == "-force")):
                entry.funcargs["i_forceState"] = commandParts.pop(0)
            # Do we need to set any mode?
            #FIXME i_mode has to be set correctly depending on what is passed at command line, eg: skip_iovalid etc
            # The forward checks of the list means we don't need to try/except the pops
            if (commandParts and (commandParts[0] == "-skip_iovalid")):
                entry.funcargs["i_mode"] = commandParts.pop(0)
        elif ((dumpFormat == "fsp") and (entry.command == "getblock")):
            # getblock is only supported on fsp dumps
            # ebmc use getmempba and gettracearray directly
            # Chip Type
            try:
                (entry.chipType, entry.chipUnitType) = parseChipString(commandParts.pop(0))
            except IndexError:
                errorMsgs.append("Unable to get chip/chipType for getblock!")
            # Get the procedure name
            try:
                prcdName = commandParts.pop(0)
            except IndexError:
                errorMsgs.append("Unable to get procedure name for getblock!")
            # Version # of the procedure being called, set it to zero for now
            prcdVer = "0"
            # Get the array name
            try:
                arrayName = commandParts.pop(0)
            except IndexError:
                errorMsgs.append("Unable to get array name for getblock!")
            # Get the # of iterations
            try:
                numIter = commandParts.pop(0)[2:]
            except IndexError:
                # No -i given, set the iterations to 0
                numIter = "0"

            # If we are in a loop, set the numIter to a string we can replace later
            if (loopCount):
                numIter = "ROCI"

            # Put everything together before we set the funcargs
            # prcdName + prcdVer + numIter + arrayName
            blockEntry = prcdName + ":" + prcdVer + ":" + numIter + ":" + str.lower(arrayName)

            # Meta Data Block Name
            entry.funcargs["i_metadataBlockEntry"] = blockEntry
            entry.commandArg = entry.funcargs["i_metadataBlockEntry"]
        elif ((dumpFormat == "ebmc") and (entry.command == "getmempba")):
            # Chip Type
            # Assume generic "pu" and position 0 for the target
            entry.chipType = "pu"
            entry.pos = "0"
            # Address
            try:
                entry.funcargs["i_address"] = int(commandParts.pop(0), 16)
            except IndexError:
                errorMsgs.append("Unable to get address for getmembpa!")
            entry.commandArg = "%X" % int(entry.funcargs["i_address"])
            #entry.commandArg = entry.funcargs["i_address"]
            # Bytes
            try:
                entry.funcargs["i_bytes"] = int(commandParts.pop(0), 10)
            except IndexError:
                errorMsgs.append("Unable to get num bytes for getmembpa!")
        elif ((dumpFormat == "ebmc") and (entry.command == "getgpr")):
            # Setup some target info since it's not part of the command
            entry.chipType = "pu"
            entry.chipUnitType = "c"
            # Threads aren't enabled for most commands
            # If -t0 or the like isn't in the HDCT, enable all
            if (entry.thread == None):
                entry.thread = "defall"
            # Address
            try:
                entry.funcargs["i_gprNum"] = int(commandParts.pop(0), 10)
            except IndexError:
                errorMsgs.append("Unable to get number for getgpr!")
            entry.commandArg = "%d" % int(entry.funcargs["i_gprNum"])
        elif ((dumpFormat == "ebmc") and (entry.command == "getfpr")):
            # Setup some target info since it's not part of the command
            entry.chipType = "pu"
            entry.chipUnitType = "c"
            # Threads aren't enabled for most commands
            # If -t0 or the like isn't in the HDCT, enable all
            if (entry.thread == None):
                entry.thread = "defall"
            # Address
            try:
                entry.funcargs["i_fprNum"] = int(commandParts.pop(0), 10)
            except IndexError:
                errorMsgs.append("Unable to get number for getfpr!")
            entry.commandArg = "%d" % int(entry.funcargs["i_fprNum"])
        elif ((dumpFormat == "ebmc") and (entry.command == "getspr")):
            # Setup some target info since it's not part of the command
            entry.chipType = "pu"
            entry.chipUnitType = "c"
            # Threads aren't enabled for most commands
            # If -t0 or the like isn't in the HDCT, enable all
            if (entry.thread == None):
                entry.thread = "defall"
            # Address
            try:
                entry.funcargs["i_sprName"] = commandParts.pop(0)
            except IndexError:
                errorMsgs.append("Unable to get name for getspr!")
            entry.commandArg = entry.funcargs["i_sprName"]
        elif ((dumpFormat == "ebmc") and (entry.command == "gettracearray")):
            # Chip Type
            try:
                (entry.chipType, entry.chipUnitType) = parseChipString(commandParts.pop(0))
            except IndexError:
                errorMsgs.append("Unable to get chip/chipType for gettracearray!")
            # Array Name
            try:
                entry.funcargs["i_name"] = commandParts.pop(0)
            except IndexError:
                errorMsgs.append("Unable to get array name for gettracearray!")

            # If we are in a loop, set the numIter to a string we can replace later
            if (loopCount):
                numIter = "ROCI"
            else:
                numIter = "0"

            # Create the trace array name with the iteration as part of the name
            entry.commandArg = entry.funcargs["i_name"] + ":" + numIter
        elif (entry.command == "getsram"):
            # Chip Type
            try:
                (entry.chipType, entry.chipUnitType) = parseChipString(commandParts.pop(0))
            except IndexError:
                errorMsgs.append("Unable to get chip/chipType for getsram!")
            # Memory Address
            try:
                entry.funcargs["i_address"] = int(commandParts.pop(0), 16)
            except IndexError:
                errorMsgs.append("Unable to get address for getsram!")
            entry.commandArg = "%X" % int(entry.funcargs["i_address"])
            # Num Bytes
            try:
                entry.funcargs["i_bytes"] = int(commandParts.pop(0), 10)
            except IndexError:
                errorMsgs.append("Unable to get num bytes for getsram!")
            # Get the mode
            # First the -mode arg
            mode = None
            try:
                mode = commandParts.pop(0)
            except IndexError:
                errorMsgs.append("Did not find -mode for getsram!")
            if (mode != "-m"):
                errorMsgs.append("Did not find -mode for getsram!")
            # Now the actual mode
            try:
                entry.funcargs["i_mode"] = int(commandParts.pop(0), 16)
            except IndexError:
                errorMsgs.append("No value for -m given for getsram!")
        elif (entry.command == "getfastarray"):
            # Chip Type
            try:
                (entry.chipType, entry.chipUnitType) = parseChipString(commandParts.pop(0))
            except IndexError:
                errorMsgs.append("Unable to get chip/chipType for getfastarray!")
            # Get the array name
            arrayName = None
            try:
                arrayName = commandParts.pop(0)
            except IndexError:
                errorMsgs.append("Unable to get array name for getfastarray!")
            entry.commandArg = arrayName
            # We need to add the getfastarray onto the front metadata string
            entry.funcargs["i_metadataBlockEntry"] = "fastarray:" + arrayName
        else:
            errorMsgs.append("Unknown command \'%s\' found!" % entry.command)

        # Possibly cleanup chpUnitNum if -all was given since we set it to "all" above
        # If chipUnitType was not given by the command though, reset it to None
        if (allFound and (entry.chipUnitType == None)):
            entry.chipUnitNum = None

        # At this point, all commandParts should be consumed.  If the list still has contents,
        # throw an error with that contents
        if (commandParts):
            errorMsgs.append("Unknown args left: %s" % commandParts)

        # We have everything parsed up, verify our entry
        verifyMsgs = entry.verify()
        errorMsgs += verifyMsgs

        # If we had errors, throw the original line and line numbers to screen
        # If clean, add it to our return list
        if (len(errorMsgs)):
            # Put out a banner to make it easy to group errors to their line
            out.error("===================================================================")
            out.error("[%d]: %s" % (lineNumber, origLine))
            # The messages in the list will have an indent, use that to make the output easier to read
            for msg in errorMsgs:
                out.error("  " + msg)
            entriesBad += 1
        else:
            # If we are in loop, we accumulate the entries to the side for processing at the end of the loop
            # Otherwise, add to the main list
            if (loopCount):
                loopEntries.append(entry)
            else:
                hdctEntries.append(entry)
                entriesGood += 1

        # Keep a running tally of errors
        totalErrors += len(errorMsgs)

    # Done reading the file, close the file
    file.close()

    # Return any errors found and the lines parsed
    return(totalErrors, entriesGood, entriesBad, hdctEntries)
