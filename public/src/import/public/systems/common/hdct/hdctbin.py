#!/usr/bin/env python3
# IBM_PROLOG_BEGIN_TAG
# This is an automatically generated prolog.
#
# $Source: public/src/import/public/systems/common/hdct/hdctbin.py $
#
# OpenPOWER sbe Project
#
# Contributors Listed Below - COPYRIGHT 2023,2024
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
# Program to generate hdct binary(hdct.bin) file

# General comment for this file.
# 1. We are not sure if there will be multiple HDCT.txt for all the sbe modules.
#    I feel that is very unlikely. This script will be called multiple times for
#    POZ (unlikely Z) with the different HDCT.txt from buildiHDCTBin.py.
#    buildiHDCTBin.py will pass the required parameters like scandef,
#    tracearrayIDs, etc based on the chip. Based on the chip type,
#    buildiHDCTBin.py. will populate the above params.
#
# 2. Incase of single HDCT.txt file. Makefile will run this script for POZ
#    (unlikely Z). buildiHDCTBin.py should add a new parameter chipType to
#    collect the entries for that chip. Based on the chip type,buildiHDCTBin.py.
#    will populate the params like scandef, tracearrayIds.H etc

############################################################
# Imports - Imports - Imports - Imports - Imports - Imports
############################################################
# Python provided
import os
import sys
import shutil
import argparse
import textwrap
import time
import datetime
import atexit
import traceback
import subprocess
import fileinput
import struct
import pickle
import ecmd
import json
# Add the common modules to the path
sys.path.append(os.path.join(os.path.dirname(os.path.realpath(__file__)), "pymod"))
from output import out
import dumpParser.base as dumpParserBase
import dumpUtils as utils
import dumpConstants
import customEnv


# supported targets
supportedTargets = ["odyssey", "pst"]

# Dict of ec level and file descriptors.
fdDict = dict()

# Dict of string ID and its hash Values.
strHashDict = dict()

# Dict to maintain all ring name,address and length collected from .scandef
ringDataDictDD10 = dict()

# Dict to maintain all trace array ID's
traceArrayIdDict = dict()

# Create the filename.
filenameBase = "hdct"
filenames = dict()

projectroot = os.environ['PROJECT_ROOT']

class ArgParser:
    def __init__(self):
        ################################################
        # Command line options
        # Create the argparser object
        # We'll do it in a way so we can create required/optional cmdline arg groups
        self.parser = argparse.ArgumentParser(description="Hdct Binary creation tool",
                                        add_help=False,
                                        formatter_class=argparse.RawDescriptionHelpFormatter,
                                        epilog=textwrap.dedent('''
        Version: 1.0

        Hdct Binary creation tool, Convert HDCT.txt to optimized binary (hdct.bin)
        Examples:  > hdctbin
                > hdctbin -j <CONFIG_Filename> -o <output_dir>
                > hdctbin -f MYCONFIG
        '''))

        # Create argument groups
        requiredGroup = self.parser.add_argument_group('Required Arguments')
        optionalGroup = self.parser.add_argument_group('Optional Arguments')

        # Add arguments to the respective groups
        requiredGroup.add_argument('-j', '--jsonfile', required=True, help="Json configuration file")
        requiredGroup.add_argument('-t', '--target', required=True, choices=['odyssey', 'pst'], help="The generation we are building for.")
        requiredGroup.add_argument('-e', '--ekbcommit', required=True, help="EKB commit")

        # Create our group of optional cmdline args
        optionalGroup = self.parser.add_argument_group('Optional Arguments')
        optionalGroup.add_argument('-h', '--help', action="help", help="Show this help message and exit")
        optionalGroup.add_argument('-o', '--output', help="Directory to place output")
        optionalGroup.add_argument('-l', '--log-level', default=out.levels.INFO, type=out.levels.argparse, choices=list(out.levels),
                            help="The output level to send to send to the log.  INFO is the default")
        optionalGroup.add_argument('-c', '--console-level', default=out.levels.BASE, type=out.levels.argparse, choices=list(out.levels),
                            help="The output level to send to send to the console.  BASE is the default")


    def parseArg(self):
        ''' Args Parser '''
        self.args = self.parser.parse_args()
        return self.args

    def getTarget(self):
        ''' Get the target from input args '''
        if self.args.target in supportedTargets:
            return self.args.target
        else:
            out.critical("Unsupported target type passed in args")
            sys.exit(1)

    def getJsonFile(self):
        ''' Get the json file from input args '''
        return self.args.jsonfile

    def getEkbCommit(self):
        ''' Get the ekb commit from input args '''
        return self.args.ekbcommit

    def getOutputPath(self):
        self.outputpath = None
        if self.args.output:
            self.outputpath = self.args.output
        return self.outputpath


# Json Parser class definition
class JsonParser:
    def __init__(self, filepath):
        self.filepath = filepath

        ''' json file parser '''
        try:
            with open(self.filepath) as file:
                jsondata = file.read()
            self.jsonParsedData = json.loads(jsondata)
        except FileNotFoundError:
            out.error("Error: File not found:", self.filepath)
            sys.exit(1)
        except json.JSONDecodeError as e:
            out.error("Error parsing JSON:", str(e))
            sys.exit(1)


    def findKeyRecursive(self, json_data, target_key):
        '''
            Find the value for given target_key in loaded json
        '''
        if isinstance(json_data, dict):
            if target_key in json_data:
                return json_data[target_key]
            for value in json_data.values():
                result = self.findKeyRecursive(value, target_key)
                if result is not None:
                    return result
        elif isinstance(json_data, list):
            for item in json_data:
                result = self.findKeyRecursive(item, target_key)
                if result is not None:
                    return result
        return None

    def raiseExp(self, msg):
        out.critical(msg)
        out.critical("Exception: %s" % sys.exc_info()[0])
        sys.exit(1)

    # Add getter here
    def getJsonParsedData(self):
        return self.jsonParsedData

    def getEcLevel(self):
        ''' Get EC level from json file '''
        self.eclevel = []
        if self.jsonParsedData:
            if (self.jsonParsedData["eclevel"]):
                self.eclevel = [int(hexString, 16) for hexString in self.jsonParsedData["eclevel"]]
                return self.eclevel
            else:
                self.raiseExp("Loaded JSON data doesn't have ecLevel value, exiting...")
        else:
            self.raiseExp("JSON data not loaded. Call 'parseJsonFile()' first.")

    def getHdctFile(self):
        ''' Get HDCT file from json file '''
        if self.jsonParsedData:
            if (self.jsonParsedData["hdctfile"]):
                self.hdctfile = self.jsonParsedData["hdctfile"]
                if "$PROJECT_ROOT" in self.hdctfile:
                    self.hdctfile = self.hdctfile.replace("$PROJECT_ROOT", projectroot)
                return self.hdctfile
            else:
                self.raiseExp("Can't able to find hdctfile in loaded JSON data")
        else:
            self.raiseExp("JSON data not loaded. Call 'parseJsonFile()' first.")

    def getDefaultDump(self):
        if self.jsonParsedData:
            if (self.jsonParsedData["defaultdump"]):
                self.defaultdump = self.jsonParsedData["defaultdump"]
                return self.defaultdump
            else:
                self.raiseExp("Loaded JSON data doesn't have dump format value, exiting...")
                return self.getDefaultDump
        else:
            self.raiseExp("JSON data not loaded. Call 'parseJsonFile()' first.")

    def getScanFile(self):
        self.scanfile = None
        if self.jsonParsedData:
            if (self.jsonParsedData["scanfile"]):
                self.scanfile = self.jsonParsedData["scanfile"]
            # Return None in case scandef file key not found
            return self.scanfile
        else:
            self.raiseExp("JSON data not loaded. Call 'parseJsonFile()' first.")

    def getTracearrayConfig(self):
        self.tracearrayConfig = None
        if self.jsonParsedData:
            if (self.jsonParsedData["tracearray"]):
                self.tracearrayConfig = self.jsonParsedData["tracearray"]
                return self.tracearrayConfig
            else:
                return self.tracearrayConfig
        else:
            self.raiseExp("JSON data not loaded. Call 'parseJsonFile()' first.")

    def getStopClockConfig(self):
        self.stopclockConfig = None
        if self.jsonParsedData:
            if (self.jsonParsedData["stopClocksTgtType"]):
                self.stopclockConfig = self.jsonParsedData["stopClocksTgtType"]
                return self.stopclockConfig
            else:
                return self.stopclockConfig
        else:
            self.raiseExp("JSON data not loaded. Call 'parseJsonFile()' first.")


############################################################
# Function - Functions - Functions - Functions - Functions
############################################################
def getRingInfo(scandefFilePath):
    # Dict to maintain all ring name,address and length collected from scandef
    # which will be returned by this function
    ringDataDict = {}

    if scandefFilePath == None:
        return None

    with open(scandefFilePath,"r") as scandef:

        # List containing all ring names
        ringNameList = []
        # List containing all ring addresses
        ringAddrList = []
        # List containing all ring Length's
        ringLengthList = []

        for line in scandef:
            if line.startswith("Name = "):
                ringNameList.append(line[7:-1])
            if line.startswith("Address = "):
                ringAddrList.append(int(line[11:21], base = 16))
            if line.startswith("Length = "):
                ringLengthList.append(int(line[9:-1]))
        scandef.close()

        # Lets create a dict {ringName:{"addr":ringAddr,"len":ringLength}} containing
        # all ring info from scandef.
        for (ringName,addr,length) in zip(ringNameList,ringAddrList,ringLengthList):
            ringDataLineDict = {ringName:{"addr":addr,"len":length}}
            ringDataDict.update(ringDataLineDict)

    return ringDataDict


def getTraceArrayIdInfo(traceArrayFileName, config):
    traceArrayEnumStart =  config["enumname"] # "enum pst_tracearray_bus_id"
    traceArrayEumVarStartWith = config["enumdatamemberstartwith"]
    traceArrayEnumEnd = "};"
    traceArrayEnumFound = False
    currentTraceArrayId = 0
    traceArrayFile = open(traceArrayFileName, "r")
    # Lets create a Dict {traceArrayName:traceArrayID} containing all trace array
    # ID info from tracearray_defs.H.
    traceArrayIdDict = {}

    for line in traceArrayFile:
        if not traceArrayEnumFound:
            if line.startswith(traceArrayEnumStart):
                traceArrayEnumFound = True
        elif line.startswith(traceArrayEnumEnd):
            traceArrayEnumFound = False
            break
        else:
            if line.find("=") != -1:
                temp = line.split("=")[1]
                temp = temp.replace(",","")
                temp = temp.replace(" ","")
                currentTraceArrayId = int(temp,16)
            if line.find(traceArrayEumVarStartWith) != -1:
                temp = line.split(traceArrayEumVarStartWith + "_")[1]
                if line.find(",") != -1:
                    temp = temp.split(",")[0]
                if line.find(" ") != -1:
                    temp = temp.split(" ")[0]
                if line.find("\n") != -1:
                    temp = temp.split("\n")[0]
                if not (line.find("=") != -1):
                    currentTraceArrayId += 1

                traceArrayIdMap = {temp:currentTraceArrayId}
                traceArrayIdDict.update(traceArrayIdMap)

    out.debug(traceArrayIdDict)
    out.debug("Number of trace array ID's found: %s " % len(traceArrayIdDict.keys()))

    traceArrayFile.close()
    return traceArrayIdDict


def createSBEFormattedDumpType(dumpTypes):
    out.debug("validDumpTypes: %s" % dumpConstants.validDumpTypes["ebmc"])
    out.debug("dumpTypes: %s" % dumpTypes)
    dumpType = 0

    for i in dumpTypes:
        dumpType = dumpType | (1 << dumpConstants.validDumpTypes["ebmc"][i])

    # Return the dumpTypes in bit mapped format.
    dumpType = dumpType & 0x3FF  # Safe side consider only 10 bits for dump type.
    return(dumpType)


def createSBEFormattedCommandType(cmdType):
    out.debug("validCommandTypes: %s" % dumpConstants.commandToEncode)
    out.debug("cmdType: %s" % cmdType)
    cmd = dumpConstants.commandToEncode[cmdType]

    # Return the enum of command type.
    cmd = cmd & 0xF   # Safe side consider only 4 bits for command.
    return(cmd)


def createSBEFormattedChipType(chipType, chipUnitType, chipNum):
    out.debug("chipType: %s" %chipType)
    out.debug("chipUnitType: %s" %chipUnitType)
    out.debug("chipNum: %s" %chipNum)

    # ChipType is OCMB then add chipUnitType should be OCMB
    # Convert an explorer chipType to OCMB chipUnitType
    if (chipType == "ocmb" or chipType == "exp"):
        chipUnitType = "ocmb"
        out.debug("Updated ocmb in chipUnitType: %s" %chipUnitType)

    chip = dumpConstants.chipUnitTypeToEncode[chipUnitType]
    out.debug("chiplet number %s" %chip)
    #chip = chipDict['None'];

    # |      6 BITS       | 6 BITS | 6 BITS |
    # |   CHIPLET OFFSET  | START  |   END  |
    chip = chip << 12
    out.debug("chiplet number %s" %chip)
    if ((chipNum == None) or (chipNum == 'defall')):
        fstChiplet = 0
        lstChiplet = 0
    else:
        if (".." in chipNum):
            parts = chipNum.split("..")
            fstChiplet = int(parts[0]) << 6
            out.debug("fst chiplet %s" %fstChiplet)
            lstChiplet = int(parts[1])
            out.debug("lst chiplet %s" %lstChiplet)
        else :
            fstChiplet = int(chipNum) << 6
            out.debug("fst chiplet %s" %fstChiplet)
            lstChiplet = 0
            out.debug("lst chiplet %s" %lstChiplet)

    chip = chip | fstChiplet | lstChiplet
    # Return the enum of command type.
    chip = chip & 0x3FFFF    # Safe side consider only 18 bits for chip/chiplet info.
    return(chip)


def createSBEFormattedAddrOrId(cmdType, addrID):
    addr = 0
    out.debug("cmdType: %s" %cmdType)
    out.debug("addrID: %s" %addrID)
    if ((cmdType == 'getscom') or (cmdType == 'putscom')or (cmdType == 'getmempba') or (cmdType == 'getsram')):
        addr = int(addrID, 16)
    elif(cmdType == 'gettracearray') or (cmdType == 'getring') or (cmdType == 'stopclocks') or (cmdType == 'getfastarray'):
        if addrID in strHashDict.keys():
            addr = strHashDict[addrID]
        else:
            addr = ecmd.ecmdHashString32(addrID, 0)
            strHashDict[addrID] = addr

    # Return the enum of command type.
    if((cmdType != 'getmempba') and (cmdType != 'getsram')):
        addr = addr & 0xFFFFFFFF  # Safe side consider only 32 bits for address.
    return(addr)


def getScandefFilePath(target, ecs, jsonparse):
    # parse existing JSON config file entry, if it exists
    try:
        if jsonparse.getScanFile():
            scandefFilePath = [jsonparse.getScanFile()[key] for key in jsonparse.getScanFile()]

        # Iterate over the list, and replace environment variables
        for i in range(len(scandefFilePath)):
            if "$PROJECT_ROOT" in scandefFilePath[i]:
                scandefFilePath[i] = scandefFilePath[i].replace("$PROJECT_ROOT", projectroot)

    # if not, use existing EKB defaults to attempt to find data
    except KeyError:
        scandefFilePath = [None] * len(ecs)

        for i in range(len(ecs)):
            chipdef_base = "%s_%02x_chipdef_base" % (target, ecs[i])
            chipdef_ver = "%s_%02x_chipdef_ver" % (target, ecs[i])

            scandefFilePath[i] = "%s/%s/%s.scandef" % (os.environ[chipdef_base], os.environ[chipdef_ver], target)

    # validating file exist in given path, in case not found raise error
    for scandeffile in scandefFilePath:
        if (not os.path.exists(scandeffile)):
            out.critical("SCANDEF file (%s) not found..." % scandeffile)
            sys.exit(1)

    return scandefFilePath


def getTraceIDFilePath(jsonparse):
    traceArrayIdFilePath = None
    if jsonparse.getTracearrayConfig() != None:
        if "$PROJECT_ROOT" in jsonparse.getTracearrayConfig()["file"]:
            jsonparse.getTracearrayConfig()["file"] = jsonparse.getTracearrayConfig()["file"].replace("$PROJECT_ROOT", projectroot)
        if (os.path.exists(jsonparse.getTracearrayConfig()["file"])):
            traceArrayIdFilePath = jsonparse.getTracearrayConfig()["file"]
    return traceArrayIdFilePath


def createOutputDirectory(outputPath, target):
    ''' create output directory to strore .lookup, .hdct, .log, .console'''
    outputPathCommon = outputPath + "/" + filenameBase + "_" + target + "_common"

    # Make sure the path exists
    if (not os.path.exists(outputPathCommon)):
        # Create the common output dir
        try:
            os.makedirs(outputPathCommon)
        except:
            out.critical("The output path does not exist.  Please check the path and re-run")
            out.critical("Problem creating: %s" % outputPathCommon)
            out.critical("Exception: %s" % sys.exc_info()[0])
            sys.exit(1)
    return outputPathCommon


def createSeperateDirForBinFile(eclevel, outputpath, target):
    # Create seperate dir for each DD level specific hdct.bin(As per EKB Requirement)
    filename = dict()
    for x in eclevel:
        outputPathBin = outputpath + "/" + filenameBase + "_" + target + "_" + str(hex(x))[2:]

        # Make sure the path exists
        if (not os.path.exists(outputPathBin)):
            # Create the common output dir
            try:
                os.makedirs(outputPathBin)
            except:
                out.critical("The output path does not exist.  Please check the path and re-run")
                out.critical("Problem creating: %s" % outputPathBin)
                out.critical("Exception: %s" % sys.exc_info()[0])
                sys.exit(1)

        filename[str(hex(x)) + "_bin"] = os.path.join(outputPathBin, filenameBase + "_" + target + "_" + str(hex(x))[2:] +".bin")
    return filename


def getStringwithNByteAlign (i_inputString, i_nBytesToAlign):
    alignedString = i_inputString[:i_nBytesToAlign].ljust(i_nBytesToAlign, '\x00')
    alignedBytes = alignedString.encode ('ascii')
    return (alignedBytes)

############################################################
# Main - Main - Main - Main - Main - Main - Main - Main
############################################################

# Create a generic time stamp we can use throughout the program
timestamp = datetime.datetime.now().strftime("%Y%m%d-%H%M%S")

def main():
    argParser = ArgParser()
    args = argParser.parseArg()

    jsonparse = JsonParser(argParser.getJsonFile())

    # Set the variable based on the required json data passed in
    hdctfileName = jsonparse.getHdctFile()

    # Get the scandef file path from json data passed in
    scandefFilePaths = getScandefFilePath(args.target, jsonparse.getEcLevel(), jsonparse)

    # Get the tracearray_defs.H file path from commandline arge passed in.
    traceArrayIdFilePath = getTraceIDFilePath(jsonparse)

    # stopclock config file
    stopClocksTgtType = jsonparse.getStopClockConfig()

    # Grab your output location and level args right away so we can setup output and logging
    # Setup our output directory
    # If the user gave us one, use that
    # If not, create one in /tmp
    if (args.output == None):
        # If we don't have one, use the default
        outputPath = os.path.join("/tmp", ("hdctbin-%s" % timestamp))
    else:
        outputPath = args.output

    # Grab the EKB commit ID.
    # Simple now to assign it over.  Could validate length, etc.. in the future
    ekbCommitID = argParser.getEkbCommit()

    # Dir to store .lookup, .hdct, .log, .consol
    outputPathCommon = createOutputDirectory(outputPath, argParser.getTarget())

    # Create seperate dir for each DD level specific hdct.bin(As per EKB Requirement)
    filenames = createSeperateDirForBinFile(jsonparse.getEcLevel(), outputPath, argParser.getTarget())

    filenames["log"] = os.path.join(outputPathCommon, filenameBase + ".log")
    filenames["hdct"] = os.path.join(outputPathCommon, filenameBase + ".hdct")
    filenames["console"] = os.path.join(outputPathCommon, filenameBase + ".console")
    filenames["lookup"] = os.path.join(outputPathCommon, filenameBase + ".lookup")

    # Setup our console output level
    out.setConsoleLevel(args.console_level)

    # Set our log output level
    out.setLogLevel(args.log_level)

    # Setup our logging infrastructure
    # This has to be done after cmdline args are processed and we know output dirs and suffixes
    out.setupLogging(filenames["log"], filenames["console"])

    out.print("Output path is %s" % outputPathCommon)

    # parseHdct takes dumpFormat argument, so passing as part of it.
    # By default the dump format will be embc.
    dumpFormat = jsonparse.getDefaultDump()

    # Log the args the program was called with
    out.print("Program Args: " + ' '.join(sys.argv), logOnly=True)

    stageStartTime = time.time()
    out.print(">>>> Start: Loading program")
    out.moreIndent()

    # Tell the user where all output is going
    # Useful if the program fails in some unexpected way
    out.print("Output going to: %s.*" % os.path.join(outputPath, filenameBase))

    ###############################################

    timePassed = (time.time() - stageStartTime)
    out.lessIndent()
    out.info("<<<< Start: Elapsed Time %s" % (utils.formatTime(timePassed)))

    # We want to continue on error so we parse/dump as much as we can
    # So we accumulate errors as we go and return this value at the end
    totalErrors = 0
    hdctEntries = list()

    ################################################
    # Call the HDCT parser
    stageNum = 1
    stageStartTime = time.time()
    out.print(">>>> Stage %d: Parsing the HDCT file" % stageNum)
    out.moreIndent()

    # Print the hdctfile given
    out.print("The HDCT file is: %s" % (hdctfileName))
    # Save off the HDCT used
    shutil.copyfile(hdctfileName, filenames["hdct"])

    # Print the dumpFormat
    out.print("The dumpFormat is: %s" % (dumpFormat))

    # Parse the HDCT
    (errorCount, entriesGood, entriesBad, allHdctEntries) = dumpParserBase.parseHDCT(hdctfileName, dumpFormat)

    # Parse scandef and collect ring name,addr and length
    ringDataDictDD = []
    count = 0
    for scandefFilePath in scandefFilePaths:
        if(scandefFilePath != None):
            ringDataDictDD.append(getRingInfo(scandefFilePath))
        else:
            out.critical("scandefFilePath for DD10 not entered/incorrect")
        count += 1


    # Parse tracearray_defs.H and collect all trace array Id's
    if(traceArrayIdFilePath != None):
        traceArrayIdDict = getTraceArrayIdInfo(traceArrayIdFilePath, jsonparse.getTracearrayConfig())
    else:
        out.critical("tracearray_defs.H not entered/Incorrect path")

    # Create the hdct.bin into the /tmp folder and open in the write mode.
    try:
        for x in jsonparse.getEcLevel():
            y = open(filenames[str(hex(x)) + "_bin"], "wb")
            fdDict[x] = y
    except IOError as e :
        out.error( "I/O error File for hdct.bin file: %s " % (filenames[str(hex(x)) + "_bin"]))
        sys.exit(1)

    # Now convert each hdct entry into sbe formatted entry.
    entriesSkipped = 0
    entryCount = 0

    # TO store DD specific ring info
    ringDataDict = dict()

    # |      6 BITS       | 6 BITS | 6 BITS |4 bits| 10 Bits |
    # |   CHIPLET OFFSET  | START  |   END  |Cmd  | Content |
    out.info("Generic Packet Structure:")
    out.info("|  6 BITS           | 6 BITS | 6 BITS |4 bits| 10 Bits |")
    out.info("|   CHIPLET OFFSET  | START  |   END  |Cmd   | Content |")

    chiptypemapwithhdct = "ody" if (argParser.getTarget() == 'odyssey') else "pu"

    for entry in allHdctEntries:

        skipped = False
        entryCount += 1

        # Only PST Odyssey regs are dumped
        # Ignore all chip types other than 'ody'
        if entry.chipType == chiptypemapwithhdct:
            out.print("Target Type matched, Processing entry %d/%d" % (entryCount, len(allHdctEntries)))
        else:
            out.print("Skipping entry %d/%d" % (entryCount, len(allHdctEntries)))
            skipped = True

        out.info("dumpTypes: %s" % entry.dumpTypes)
        out.info("command: %s" % entry.command)
        out.info("ec: %s" % entry.ec)
        out.info("chipType: %s" % entry.chipType)
        out.info("chipUnitType: %s" % entry.chipUnitType)
        out.info("chipUnitNum: %s" % entry.chipUnitNum)
        out.info("commandArg %s" % entry.commandArg)
        out.info("ecLevel %s" % hex(entry.ec))
        out.info("ecPlus: %s" % entry.ecPlus)

        if skipped:
            continue

        # Get the sbe Formatted cmd type.
        (cmdType) = createSBEFormattedCommandType(entry.command)
        out.info("cmdType is %s" %cmdType)

        # Get the bit mapped dump types for the given dumpTypes
        (bitMapDumpTypes) = createSBEFormattedDumpType(entry.dumpTypes)
        out.info("bitMapDumpTypes is %s" %bitMapDumpTypes)

        # Get the sbe formatted chip type
        (chipInfo) = createSBEFormattedChipType(entry.chipType, entry.chipUnitType, entry.chipUnitNum)
        out.info("chipType is %s" %chipInfo)

        # Get the address/RingId/TracearrayID/stopclockID/
        (addr) = createSBEFormattedAddrOrId(entry.command, entry.commandArg)
        out.info("addr is %s" %hex(addr))

        #  |--18 bits-|4 bits| 10 Bits |
        #  |----------|------|---------|
        #  | Chip Info| Cmd  | Content |
        chipInfo = (chipInfo << 14)
        cmdType = cmdType << 10
        genericVal = chipInfo | cmdType | bitMapDumpTypes
        out.info("genericVal is %s, %s" % (genericVal, hex(genericVal)))

        parsedData = []

        # Write to the binary according to EC level.
        for key in fdDict:
            if ((key >= entry.ec) if entry.ecPlus else (key == entry.ec)):

                # Check if notsbe is present in HDCT row
                # Such HDCT rows are not supported by SBE
                if entry.notSbe == True:
                    out.info("SBE does not support HDCT row(entry.notSbe): %s" % entry.notSbe)
                    continue

                # Write the generic struct to HDCT binary.
                fdDict[key].write(struct.pack(">I",genericVal))
                parsedData.append(struct.pack(">I",genericVal))

                # Write adress to hdct binary.
                fdDict[key].write(struct.pack(">I",addr))
                parsedData.append(struct.pack(">I",addr))

                # Write the control_set to HDCT binary in case of fast array if not
                # found in dict write 0xffffffff
                if(entry.command == "getfastarray"):
                    fastarrayFilenameSizeMax = 32
                    if entry.commandArg:
                        out.info("Control set: %s " % entry.commandArg)
                        fdDict[key].write(getStringwithNByteAlign(entry.commandArg, fastarrayFilenameSizeMax))
                        parsedData.append(getStringwithNByteAlign(entry.commandArg, fastarrayFilenameSizeMax))
                    else:
                        fdDict[key].write(getStringwithNByteAlign("", fastarrayFilenameSizeMax))
                        parsedData.append(getStringwithNByteAlign("", fastarrayFilenameSizeMax))
                        out.info("Control_set info not found for fast array: %s " % entry.commandArg)

                # Write the data to HDCT incase of getmempba and getsram
                if((entry.command == "getmempba") or (entry.command == "getsram")):
                    out.info("Size is %s" %  entry.funcargs["i_bytes"])
                    fdDict[key].write(struct.pack(">Q",entry.funcargs["i_bytes"]))
                    parsedData.append(struct.pack(">Q",entry.funcargs["i_bytes"]))

                # Write ring address and length to HDCT binary if found else write 0xffffffff
                if (key in jsonparse.getEcLevel()):
                    ringDataDict =  ringDataDictDD[jsonparse.getEcLevel().index(key)-1]

                if(entry.command == "getring"):
                    if entry.commandArg in ringDataDict.keys():
                        fdDict[key].write(struct.pack(">I",ringDataDict[entry.commandArg]["addr"]))
                        out.info("Ring address: %s " % str(hex(ringDataDict[entry.commandArg]["addr"])))
                        fdDict[key].write(struct.pack(">I",ringDataDict[entry.commandArg]["len"]))
                        out.info("Ring Length: %s " % str(ringDataDict[entry.commandArg]["len"]))

                        parsedData.append(struct.pack(">I",ringDataDict[entry.commandArg]["addr"]))
                        parsedData.append(struct.pack(">I",ringDataDict[entry.commandArg]["len"]))
                    else:
                        fdDict[key].write(struct.pack(">I",0xffffffff))
                        fdDict[key].write(struct.pack(">I",0xffffffff))

                        parsedData.append(struct.pack(">I",0xffffffff))
                        parsedData.append(struct.pack(">I",0xffffffff))

                        out.info("Ring info not found: %s " % entry.commandArg)

                # Write the data to HDCT incase of putscom.
                if(entry.command == "putscom"):
                    out.info("funArg %s" %  entry.funcargs["i_data"])
                    fdDict[key].write(struct.pack(">Q",int(entry.funcargs["i_data"], 16)))

                    parsedData.append(struct.pack(">Q",int(entry.funcargs["i_data"], 16)))

                # Write 0x01 if -band and 0x02 if -bor is present for putscom else 0x0
                if(entry.command == "putscom"):
                    if "i_band" in entry.funcargs:
                        fdDict[key].write(struct.pack(">I",0x01))
                        parsedData.append(struct.pack(">I",0x01))
                        out.info("bit modifier: and")
                    elif "i_bor" in entry.funcargs:
                        fdDict[key].write(struct.pack(">I",0x02))
                        parsedData.append(struct.pack(">I",0x02))
                        out.info("bit modifier: or")
                    else:
                        fdDict[key].write(struct.pack(">I",0x00))
                        parsedData.append(struct.pack(">I",0x00))
                        out.info("bit modifier: None")

                # Write channel/mode if present for sram else 0xff
                if (entry.command == "getsram"):
                    if "i_mode" in entry.funcargs:
                        fdDict[key].write(struct.pack(">I",entry.funcargs["i_mode"]))
                        parsedData.append(struct.pack(">I",entry.funcargs["i_mode"]))
                        out.info("channel/mode: %s" % entry.funcargs["i_mode"])
                    else:
                        fdDict[key].write(struct.pack(">I",0xff))
                        parsedData.append(struct.pack(">I",0xff))

                # Write trace array id if found in traceArrayIdDict if not write 0xff
                if(entry.command == "gettracearray"):
                    # Output of dumpParser.py for tracearray is:
                    # PB0:0 L3_0:0
                    # we need to truncate after ':'.
                    traceArray = entry.commandArg.split(":", 1)[0]
                    if traceArray in traceArrayIdDict.keys():
                        fdDict[key].write(struct.pack(">I",traceArrayIdDict[traceArray]))
                        parsedData.append(struct.pack(">I",traceArrayIdDict[traceArray]))
                        out.info("Trace Array ID: %s " % hex(traceArrayIdDict[traceArray]))
                    else:
                        fdDict[key].write(struct.pack(">I",0x000000ff))
                        parsedData.append(struct.pack(">I",0x000000ff))
                        out.info("Trace Array ID info not found for: %s " % traceArray)

                # Write the stop clocks target type to HDCT binary if not
                # found in dict write 0xffffffff
                if(entry.command == "stopclocks"):
                    if entry.commandArg in stopClocksTgtType.keys():
                        out.info("Stop clock Target type: %s " % stopClocksTgtType[entry.commandArg])
                        fdDict[key].write(struct.pack(">I", int(stopClocksTgtType[entry.commandArg], 16) ))
                        parsedData.append(struct.pack(">I", int(stopClocksTgtType[entry.commandArg], 16)))
                    else:
                        fdDict[key].write(struct.pack(">I",0xffffffff))
                        parsedData.append(struct.pack(">I",0xffffffff))
                        out.info("Stop clock target type not found for: %s " % entry.commandArg)


                # Printing binary value for comapre
                out.debug("Packet : +++")
                # |      6 BITS       | 6 BITS | 6 BITS |4 bits| 10 Bits |
                # |   CHIPLET OFFSET  | START  |   END  |Cmd  | Content |
                chipinfoprint = (chipInfo >> 14)
                out.debug("generic Val: %s %s %s %s %s" % ( hex((chipinfoprint >> 12) & 0x3F),
                                            hex((chipinfoprint >> 6) & 0x3F),
                                            hex((chipinfoprint) & 0x3F),
                                            hex((cmdType >> 10 ) & 0xF),
                                            hex((bitMapDumpTypes) & 0x3FF) ))

                out.debug("Addr: %s" % hex(addr) )
                out.debug('PACKET with no comama  ' + ' '.join(' '.join(format(i, '02X') for i in list(j)) for j in parsedData) )
                out.debug('PACKET with with comama' + ',  '.join(''.join(format(i, '02X') for i in list(j)) for j in parsedData) )
                out.debug("Packet : ---")



    # Close the bin files written out
    for key in fdDict:
        fdDict[key].close()

    # Save off the string to hash keys for future makedump use
    # We need invert the strHashDict first so we can do lookups based on keys later
    invertKeys = {v: k for k, v in strHashDict.items()}
    # Now inverted, save that dictionary off as a simple pickle file.
    # We are going with protocol version 2 so as to support unpickling in simics env
    # which supports only python2.7
    pickle.dump(invertKeys, open(filenames["lookup"], "wb"), protocol=2)

    # Save off the hdct file and lookup table tied to this commit id
    customEnv.copyHdctFile(filenames, args.target, "ekb", ekbCommitID, True)

    out.print("")
    out.print("All bin output in: %s/" % outputPath)
    out.print("HDCT File: %s" % filenames["hdct"])
    out.print("Lookup File: %s" % filenames["lookup"])
    out.print("Log File: %s" % filenames["log"])
    out.print("Console File: %s" % filenames["console"])

    out.setIndent(0)
    out.print("")
    out.print("Program ends")



# PYTHON MAIN
if __name__ == "__main__":
    main()
    exit()
