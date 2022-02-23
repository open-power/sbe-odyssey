#!/usr/bin/env python3
# IBM_PROLOG_BEGIN_TAG
# This is an automatically generated prolog.
#
# $Source: public/src/onetime/common/otprom/build/utils/fusemapgen.py $
#
# OpenPOWER sbe Project
#
# Contributors Listed Below - COPYRIGHT 2022
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

############################################################
# Imports - Imports - Imports - Imports - Imports - Imports
############################################################
#Python Provided
import sys
import os
import argparse
import textwrap
import json
import struct
from jinja2 import Template

#User Provided
pyUtilsPath = os.path.expandvars("$SBEROOT")+ "/public/src/tools/utils/py/"
sys.path.append(os.path.join(os.path.dirname(os.path.realpath(__file__)), pyUtilsPath))
import templateUtil

############################################################
# Variables - Variables - Variables - Variables - Variables
############################################################

# Tool version
toolVersion = 1.0

# Register Size Max in bits
regSize = 64

# OTPROM size without auto ECC
#i.e Data is 1Kb
#    No ECC
otpromSizeWithoutEcc = 1024

# OTPROM size with auto ECC
#i.e Data is 896 bytes
#    ECC is 128 bytes
otpromSizeWithEcc = 896

############################################################
# Template - Template - Template - Template - Template
############################################################
# Template for Header File
headerFileTemplate = """
/**
* @brief This file gives us info about OTPROM Fuse Map.
*        This is a auto generated file basis OTPROM Fuse Map JSON file
*/

#ifndef __OTPROM_FUSE_MAP_H
#define __OTPROM_FUSE_MAP_H

#include <stdint.H>

/**
* @brief Macros defining OTPROM FUSE Reg Addresse's
*/
$$$@@@  for key, value in regJsonDump.items()  @@@$$$
#define $@ value[0]['scom_reg_name'] @$ $@ key @$    // $@ value[0]['description'] @$
$$$@@@ endfor @@@$$$

$$$@@@  for key, value in regJsonDump.items()  @@@$$$
/**
* @brief $@ value[0]['description'] @$
*/
typedef union $@ value[0]['scom_reg_name'].lower() @$
{
    uint64_t $@ value[0]['scom_reg_name'].lower() @$;

    struct
    {
        $$$@@@  for data in value[1:]:  @@@$$$
        uint64_t $@ data['field'] @$ : $@ data['field_size'] @$;     // $@ data['description'] @$
        $$$@@@ endfor @@@$$$
    }$@ value[0]['scom_reg_name'].lower() + "_val" @$;

}$@ value[0]['scom_reg_name'].lower() + "_t" @$;
$$$@@@ endfor @@@$$$

#endif /***** __OTPROM_FUSE_MAP_H *********
"""
############################################################
# Function - Functions - Functions - Functions - Functions
############################################################
# Extract ‘k’ bits from a given position in a number
def extractedBits(no:int, k:int, pos:int) -> int:
   bi = bin(no)[2:].zfill(regSize)
   e = len(bi) - pos
   s = e - k + 1
   substr = bi[s : e+1]
   return int(substr,2)

#Function to generate OTPROM fuse binary basis JSON file
def generateFuseBin(args:argparse.Namespace):

    print("Generating OTPROM Fuse Binary...")

    # Read the JSON file
    with open(args.otpromFuseMapJsonFile, "r") as otpromFuseMap:
        jsonData = json.load(otpromFuseMap)

    # Create our fuse map binary file
    fuseMapBin = open(args.outputBinFilePath, "wb")

    for key in jsonData:
        value = ""
        for ofset in range (1, len(jsonData[key]), 1):
            if jsonData[key][ofset]['value'] == None:
                data = int("0x00",16)
            else:
                assert not jsonData[key][ofset]['value'][:2] != "0x", \
                    ("Enter value in hex string format \"0x12345678\" for %s reg -> field : %s" %
                        (key,jsonData[key][ofset]['field']))
                data = int(jsonData[key][ofset]['value'],16)

            #Convert integer data to bin format and zero fill based on field size
            data = bin(data)[2:].zfill(jsonData[key][ofset]['field_size'])

             # Check if the value is exceeding the field size
            assert not jsonData[key][ofset]['field_size'] != len(data), \
                ("Field size is %s which is not equal to size of value %s for %s reg -> field : %s" %
                        (jsonData[key][ofset]['field_size'],len(data),key,jsonData[key][ofset]['field']))

            # Concatenate the data if its bit filed's
            value = data + value

        # Check if length of value exceeds 64 bits
        assert not len(value) != regSize, \
            ("Total length of value per register should be %s bits. Total Value length of %s register is %s" %
                        (regSize,key,len(value)))

        # Write the 64 bit value into binary in big endian format
        fuseMapBin.write(struct.pack(">Q",int(value,2)))

    fuseMapBin.close()

    # Get the file size at this point as we have already written all data.
    # Lets zero pad to OTPROM size
    binFileSize = os.path.getsize(args.outputBinFilePath)
    padding = [0x0 for _ in range(otpromSizeWithEcc - binFileSize)]  # Create bytearray
    print(otpromSizeWithEcc - binFileSize)
    with open(args.outputBinFilePath, "ab") as fuseMapBin:
        fuseMapBin.write(bytearray(padding))  # Append bytes to the end

# Function to JSON dump OTPROM Fuse binary
def printFusebin(args:argparse.Namespace):

    # Read the JSON file
    with open(args.otpromFuseMapJsonFile, "r") as otpromFuseMap:
        jsonData = json.load(otpromFuseMap)

    # Check if binary size is as expected wrt to JSON
    binFileSize = os.path.getsize(args.fuseBinFile)
    assert not otpromSizeWithEcc != binFileSize, \
        ("Expected binary file size as per JSON: %s, Actual binary file size: %s. Pass right .bin/JSON" %
            (otpromSizeWithEcc, binFileSize))

    # Open FUSE map binary file
    fuseMapBin = open(args.fuseBinFile, "rb")

    for key in jsonData:
        value = int.from_bytes(fuseMapBin.read(8),"big")
        position = 1
        for ofset in range (1, len(jsonData[key]), 1):
            bitFieldValue = extractedBits(value, jsonData[key][ofset]['field_size'], position)
            position += jsonData[key][ofset]['field_size']
            jsonData[key][ofset]['value'] = hex(bitFieldValue).upper()

    fuseMapBin.close()

    # Dump the parsed Binary data into JSON file
    with open(args.outputJsonFilePath, 'w') as jsonDataDump:
        json.dump(jsonData, jsonDataDump, indent=4)

    print("Dumping JSON... to %s" % args.outputJsonFilePath)

# Function to generate OTPROM Fuse Header file basis JSON
def generateFuseHdr(args:argparse.Namespace):

    print("Generating OTPROM Fuse Header...")

    # Read the JSON file
    with open(args.otpromFuseMapJsonFile, "r") as otpromFuseMap:
        jsonData = json.load(otpromFuseMap)

    # Render the Custom Template basis JSON Data
    customHdrFileTemplate = templateUtil.customTemplate(headerFileTemplate)
    renderedHdrFile = customHdrFileTemplate.render(regJsonDump=jsonData)

    # Write the rendered template data into a file
    with open(args.outputHdrFilePath, "w") as  fuseHdrFile:
        fuseHdrFile.write(renderedHdrFile)

################################## Main ###############################################
# Command line options
# Create the argparser object
# We'll do it in a way so we can create required/optional cmdline arg groups
argparser = argparse.ArgumentParser(description="Tool to generate OTPROM Fuse Map .bin and .H",
                                    add_help=False,
                                    formatter_class=argparse.RawDescriptionHelpFormatter,
                                    epilog=textwrap.dedent('''
Version: ''' + str(toolVersion) + '''

'''))

# Create our group of required cmdline args
reqgroup = argparser.add_argument_group('Required Arguments')
reqgroup.add_argument('-i', '--otpromFuseMapJsonFile', required=True, help="OTPROM Fuse Map JSON File")

# Create our group of optional cmdline args
optgroup = argparser.add_argument_group('Optional Arguments')
optgroup.add_argument('-h', '--help', action="help", help="Show this help message and exit")

# Create our group of subparser cmdline args
subparsers = argparser.add_subparsers(dest='Check --help')
subparsers.required = True

# Subparser for binary generation
bingenerator = subparsers.add_parser("bingenerator", help="Generate OTPROM Fuse Map binary from JSON File")
bingenerator.set_defaults(func = generateFuseBin)
bingenerator.add_argument('-o', '--outputBinFilePath', required=True, help="Output file name path of otprom fuse map binary")

# Subparser for parsing/printing generated binary
printbin = subparsers.add_parser("printbin", help="Dump/Parse OTPROM Fuse bin into JSON")
printbin.set_defaults(func = printFusebin)
printbin.add_argument('-b', '--fuseBinFile', required=True, help="The OTPROM Fuse binary to to printed")
printbin.add_argument('-o', '--outputJsonFilePath',  required=True, help="Output file name path to dump parsed binary as JSON")

# Subparser for .H file generation that can be used by SROM
hdrgenerator = subparsers.add_parser("hdrgenerator", help="Generate OTPROM Fuse Map Header from JSON File")
hdrgenerator.set_defaults(func = generateFuseHdr)
hdrgenerator.add_argument('-o', '--outputHdrFilePath', required=True, help="Output file name path of otprom fuse map header")

# cmdline loaded up, now parse for it and handle what is found
args = argparser.parse_args()

# Make sure the file exists
if not os.path.isfile(args.otpromFuseMapJsonFile):
    print("The JSON File does not exists. Please check the file path and re-run")
    sys.exit(1)

# Run the appropriate function
args.func(args)
