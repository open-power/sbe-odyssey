#!/usr/bin/env python3
# IBM_PROLOG_BEGIN_TAG
# This is an automatically generated prolog.
#
# $Source: public/src/import/public/systems/common/hdct/hdcttxt.py $
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
# Program to verify hdct binary(hdct.bin) file

############################################################
# Imports - Imports - Imports - Imports - Imports - Imports
############################################################
# Python provided
import os
import sys
import argparse
import textwrap
import time
import datetime
import struct
import binascii
import pickle
import filecmp
import re

# Add the common modules to the path
sys.path.append(os.path.join(os.path.dirname(os.path.realpath(__file__)), "pymod"))
from output import out
# Import remaining dump modules
import dumpUtils as utils
import createHdctTxtUtils as hdctTxtUtils

# Tool version
toolVersion = 1.0

# Dict of  file descriptors.
fdDict = dict()

# Create a generic time stamp we can use throughout the program
timestamp = datetime.datetime.now().strftime("%Y/%m/%d-%H:%M:%S")

# Dict of string ID and its hash Values.
strHashDict = dict()

# Maintain a dict of entries/command/dumptypes etc that is not used by sbe
# Items in this list will be stripped off from original HDCT.txt before
# comparision.These are also not written into HDCT.bin
# Few items are not written into HDCT.bin which are not used because of memory constraint
unusedSbeContent = {"dumpTypes" : ["CHCRCLK"], "lineWithTag" : "notsbe"}

# Alignment format for original HDCT.txt entries.
alignmentFormat = {"addr":"{:>08}", "value":"{:<016}", "len":"{:>016}"}

def processesHdctTxt(orignalHdctTxtFileName, ecLevel):

    origHdctTxt = open(orignalHdctTxtFileName, "r")
    tempHdctTxt = open(filenames["tempHdct_txt"], "w")

    out.debug("processesHdctTxt: ")
    # Remove all Comment line,empty lines from original HDCT.txt and write the output in a temp file.
    for line in origHdctTxt:
        line = line.split("*")[0].rstrip()
        if line:

            line = line.split("#")[0].rstrip()
            out.debug("line", line)

            # Lets not write or parse any line with notSbe tag
            # Such lines are not supported by sbe
            if unusedSbeContent["lineWithTag"] in line:
                continue

            line = line.replace(" exp " , " ocmb ")

            line = line.split()

            out.debug("line", line)

            # Align all address,values and length as word, double word,double word format
            for i in range(len(line)):
                if line[i].startswith("putscom"):
                    line[i+2] = alignmentFormat["addr"].format(line[i+2])
                    line[i+3] = alignmentFormat["value"].format(line[i+3])
                    break
                if line[i].startswith("getscom"):
                    line[i+2] = alignmentFormat["addr"].format(line[i+2])
                    break
                if line[i].startswith("getsram"):
                    line[i+2] = alignmentFormat["addr"].format(line[i+2])
                    line[i+3] = alignmentFormat["len"].format(line[i+3])
                    break
                if line[i].startswith("getmempba"):
                    line[i+1] = alignmentFormat["addr"].format(line[i+1])
                    line[i+2] = alignmentFormat["len"].format(line[i+2])
                    break

            # Filter out lines based on EC level.
            # Skip the EC Level check for START_LOOP 16 and END_LOOP line entries in HDCT.txt
            if len(line) > 2:
                tempEcLevel = int(line[-1].replace("+",""))

                if not((ecLevel >= tempEcLevel) if line[-1][-1] == "+" else (ecLevel == tempEcLevel)):
                    continue

                # Remove the ec level and + at end.
                line[-1] = ""

            line = "".join(map(str, line))

            # Remove all unused content from the original HDCT.txt file
            for i in range(len(unusedSbeContent["dumpTypes"])):
                line=line.replace(unusedSbeContent["dumpTypes"][i] + "|", "")

            # Write the modified line into file for comparision
            tempHdctTxt.write(line + "\n")

    origHdctTxt.close()
    tempHdctTxt.close()

################################################
# Command line options
# Create the argparser object
# We'll do it in a way so we can create required/optional cmdline arg groups
argparser = argparse.ArgumentParser(description="Hdct Binary verification tool",
                                 add_help=False,
                                 formatter_class=argparse.RawDescriptionHelpFormatter,
                                 epilog=textwrap.dedent('''
Version: ''' + str(toolVersion) + '''

Examples:  > createHdctTxt
           > createHdctTxt -f <HDCT_Filename> -o <output_dir>
           > createHdctTxt -f MYHDCT
'''))

# Create our group of required cmdline args
reqgroup = argparser.add_argument_group('Required Arguments')
reqgroup.add_argument('-f', '--hdctbinfile', required = True, help="The HDCT Binary file to use")
reqgroup.add_argument('-e', '--ekbCommit', required=True, help="EKB commit")
reqgroup.add_argument('-s', '--hashlookup', required=True, help="The HDCT.lookup to be used for fetching hash equivalent strings")
reqgroup.add_argument('-r', '--originalHdctTxt', required=True, help="The original HDCT.txt file with which the HDCT.bin=>HDCT.txt needs to be compared")
reqgroup.add_argument('-g', '--generation', required=True, choices=['p11', 'ody'], help="The generation of directory structure")

# Create our group of optional cmdline args
optgroup = argparser.add_argument_group('Optional Arguments')
optgroup.add_argument('-h', '--help', action="help", help="Show this help message and exit")
optgroup.add_argument('-o', '--output', help="Directory to place output")
optgroup.add_argument('-l', '--log-level', default=out.levels.INFO, type=out.levels.argparse, choices=list(out.levels),
                      help="The output level to send to the log.  INFO is the default")
optgroup.add_argument('-c', '--console-level', default=out.levels.BASE, type=out.levels.argparse, choices=list(out.levels),
                      help="The output level to send to the console.  BASE is the default")

# cmdline loaded up, now parse for it and handle what is found
args = argparser.parse_args()

# See if the user passed in the path to hdct file, we will use that instead
hdctBinFileName = args.hdctbinfile

# Get the path for hdct.lookup file
hdctLookupFile = args.hashlookup

# Get the path for original HDCT.txt file for comparision
hdctTxtFileName = args.originalHdctTxt

# Load data from the pickle file which contains string equivalent hash
strHashDict = pickle.load(open(hdctLookupFile,"rb"))

# Get the SBE ec level from the binary file name
ecLevel = int(args.hdctbinfile[-6:-4])
out.debug("SBE ecLevel: %s" % ecLevel)

# Grab your output location and level args right away so we can setup output and logging
# Setup our output directory
# If the user gave us one, use that
# If not, create one in /tmp
if (args.output == None):
    # If we don't have one, use the default
    outputPath = os.path.join("/tmp", ("hdctTxt-%s" % timestamp))
else:
    outputPath = args.output

# Grab the EKB commit ID.
# Simple now to assign it over.  Could validate length, etc.. in the future
ekbCommitID = args.ekbCommit

# Create the filename.
filenameBase = "verifyHdct"
filenames = dict()
outputPath = outputPath + "/" + filenameBase + "_" + args.generation + "dd" + str(ecLevel)

# Make sure the path exists
if (not os.path.exists(outputPath)):
    # Create the output dir
    try:
        os.mkdir(outputPath)
    except:
        out.critical("The output path does not exist.  Please check the path and re-run")
        out.critical("Problem creating: %s" % outputPath)
        out.critical("Exception: %s" % sys.exc_info()[0])
        sys.exit(1)
    out.debug("Output path is %s" % outputPath)
# File containing HDCT.bin equivalent HDCT.txt content
filenames["binEqvHdct_txt"] = os.path.join(outputPath, filenameBase +"_" + args.generation + "dd" + str(ecLevel) +".txt")

# File containing original HDCT.txt content with all spaces,comments, unsupported
# SBE HDCT content stripped off for comparision with binEqvHdct_txt.
filenames["tempHdct_txt"] = os.path.join(outputPath,"tempOrigHdct.txt")
filenames["log"] = os.path.join(outputPath, filenameBase + ".log")
filenames["console"] = os.path.join(outputPath, filenameBase + ".console")

# Setup our console output level
out.setConsoleLevel(args.console_level)
# out.setConsoleLevel("debug")

# Set our log output level
out.setLogLevel(args.log_level)

# Setup our logging infrastructure
# This has to be done after cmdline args are processed and we know output dirs and suffixes
out.setupLogging(filenames["log"], filenames["console"])

# parseHdct takes dumpFormat argument, so passing as part of it.
# By default the dump format will be embc.
dumpFormat = "ebmc"

# Log the args the program was called with
out.debug("Program Args: " + ' '.join(sys.argv), logOnly=True)

stageStartTime = time.time()
out.debug(">>>> Start: Loading program")
out.setIndent(2)

###############################################

out.setIndent(0)

try:
    y = open(filenames["binEqvHdct_txt"], "w")
    fdDict[ecLevel] = y
except IOError as e :
    out.error( "I/O error File for hdct.txt file: %s " % (filenames[str(hex(x)) + "_txt"]))
    sys.exit(1)

# Parse the HDCT.bin file
(commitId, hdctTxtEntries) = hdctTxtUtils.createHDCTTxt(hdctBinFileName, "ekb", strHashDict)

# Remove comment line,new lines from original HDCT.txt
processesHdctTxt(hdctTxtFileName, ecLevel)

out.setIndent(2)

# Store PERF Entry count so as to get START_LOOP 16 and END_LOOP as in HDCT.txt
perfEntryCount = 0
perfEntriesPerLoop = 15
perfLoopCount = 16

hdctEntryCount = 0

for entry in hdctTxtEntries:

    hdctEntryCount += 1
    #NOTE: Enable if debug is needed
#    out.print("********************************************************** HDCT ENTRY %s ***************************************************************************" % hdctEntryCount)
#    entry.printSingleEntry()

    for key in fdDict:
        if entry.dumpTypes[0] == "PERF":
            if perfEntryCount <= 0:
                fdDict[key].write("START_LOOP16\n")
            elif perfEntryCount == perfEntriesPerLoop * perfLoopCount - 1:
                fdDict[key].write("END_LOOP\n")
            perfEntryCount = perfEntryCount + 1
            if perfEntryCount > perfEntriesPerLoop:
                continue
        else:
            perfEntryCount = 0

        for x in range(len(entry.dumpTypes)):
            if( x < len(entry.dumpTypes) - 1):
                fdDict[key].write( ''.join(map(str, entry.dumpTypes[x])) + "|")
        fdDict[key].write( ''.join(map(str, entry.dumpTypes[x])))

        fdDict[key].write(entry.command)

        if ( (entry.command != "getmempba") and (entry.chipUnitType != "ocmb") ):
            fdDict[key].write(entry.chipType)

        if entry.chipUnitType == "ocmb":
            fdDict[key].write(entry.chipUnitType)

        if ( (entry.chipUnitType != None) and (entry.chipUnitType != "ocmb") ):
            fdDict[key].write("." + entry.chipUnitType)

        # Output of createHdctTxtUtils.py for tracearray is:
        # PB0:0 L3_0:0
        # we need to truncate after ':'.
        if entry.command == "gettracearray":
            entry.address = entry.address.split(":",1)[0]

        fdDict[key].write(entry.address)

        if entry.command == "putscom":
            fdDict[key].write(entry.funcargs["i_data"])
            if "i_band" in entry.funcargs:
                fdDict[key].write("-band")
            elif "i_bor" in entry.funcargs:
                fdDict[key].write("-bor")

        if((entry.command == "getmempba") or (entry.command == "getsram")):
            fdDict[key].write(entry.funcargs["i_bytes"])

        if(entry.command == "getsram"):
            if "i_channel" in entry.funcargs:
                fdDict[key].write("-m" + entry.funcargs["i_channel"])

        if entry.chipUnitNum != None:
            fdDict[key].write("-c" + entry.chipUnitNum)

        #NOTE: Dont write ec level and + sign at last as we are removing it from original HDCT.txt based on the EC level
        fdDict[key].write("ec" + "\n")

for key in fdDict:
    fdDict[key].close()

out.setIndent(0)
out.debug("Comparing original HDCT.txt with HDCT.txt generated from HDCT.bin")

comp = filecmp.cmp(filenames["tempHdct_txt"], filenames["binEqvHdct_txt"], shallow = False)

stageEndTime = time.time() - stageStartTime

############################################ Report ############################################
out.print(" ")
out.print("************************************HDCT.bin verification report************************************")
out.print(" ")
out.print("HDCT.bin verification tool version: %s " % toolVersion)
out.print("Date and Time: %s" % timestamp + "    " + "Run Duration: %s" % utils.formatTime(stageEndTime))
out.print("EKB Commit ID passed by user: %s" % ekbCommitID)
out.print("EC Level: %s" % ecLevel)
out.print(textwrap.dedent("""Note: Stripping of new lines, spaces, commits.
Alligning all address and values to word and double word format respectively.
Filtering out HDCT.txt entries based on EC Level %s and %s tag.""" % (ecLevel,unusedSbeContent["lineWithTag"]) + """\n"""
"""Stripping out unsupported SBE content %s """ % unusedSbeContent["dumpTypes"] +
"""from original HDCT.bin.\n"""))
out.print(" ")

if comp == True:
    out.print("HDCT.bin verified successfully")
else:
    out.print("HDCT.bin verification failed")
    out.print("Mismatch found between %s" %filenames["binEqvHdct_txt"] + " and\n %s" %filenames["tempHdct_txt"] + "\n Performing delta between files")
    out.print(" ")
    diff = "diff " + filenames["binEqvHdct_txt"] + " " + filenames["tempHdct_txt"]
    sdiff = "sdiff -l " + filenames["binEqvHdct_txt"] + " " + filenames["tempHdct_txt"] + " | cat -n | grep -v -e \'($\'"
    os.system(diff) # Can use sdiff or diff
    out.print(" ")
    out.print("For more info all output in: %s/" % outputPath)
    out.print("HDCT.txt file generated from HDCT.bin: %s" % filenames["binEqvHdct_txt"])
    out.print("Temp HDCT.txt file generated from original HDCT.txt: %s" % filenames["tempHdct_txt"])
    out.print("Log File: %s" % filenames["log"])
    out.print("Console File: %s" % filenames["console"])

out.print(" ")
out.print("****************************************************************************************************")
out.print("")
out.debug("Program ends")
sys.exit(not comp)
