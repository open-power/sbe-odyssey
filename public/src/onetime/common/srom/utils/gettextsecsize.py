#!/usr/bin/env python3
# IBM_PROLOG_BEGIN_TAG
# This is an automatically generated prolog.
#
# $Source: public/src/onetime/common/srom/utils/gettextsecsize.py $
#
# OpenPOWER sbe Project
#
# Contributors Listed Below - COPYRIGHT 2022
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
import subprocess
import argparse
import textwrap
import json
import struct

from jinja2 import Template

try:
    SBE_ROOT_PATH = os.environ['SBEROOT']
except KeyError as err:
    print("SBE Workon not set. Run sbe workon and re-run the tool...")
    exit()

TOOL_PATH  = os.path.dirname(os.path.realpath(__file__))
#User Provided
pyUtilsPath = os.path.expandvars("$SBEROOT")+ "/public/src/tools/utils/py/"
sys.path.append(
    os.path.join(os.path.dirname(os.path.realpath(__file__)), pyUtilsPath))
import templateUtil

############################################################
# Variables - Variables - Variables - Variables - Variables
############################################################

# Tool version
toolVersion = 1.0

# Header global list
headerDataList = []


############################################################
# Template - Template - Template - Template - Template
############################################################
# Template for Header File
headerFileTemplate = """\
/**
* @brief This file gives us info about lib layout start offset.
*        This is a auto generated file basis liblayout path JSON file
*        JSON path: public/src/onetime/odyssey/srom/build/utils/liblayout.json
*/
#ifndef __$@ tHeaderName.upper() @$_H
#define __$@ tHeaderName.upper() @$_H

$$$@@@  for value in listdata  @@@$$$
/**
* @brief Macros defining $@ value['libname'] @$ start offset
*
* $@ value['libname'] @$_lib_size : $@ value['libsize'] @$
* Buffer size : $@ value['buffersize'] @$
* $@ value['libname'] @$_lib_size_with_buffer : $@ value['libbufsize'] @$
* $@ value['libname'] @$_lib_size_with_buffer_aligned: $@ value['alignedbuf']@$
* $@ value['libname'].upper() @$_START_OFFSET =  \
$@ value['prevlibname'].upper() @$_START_OFFSET + \
$@ value['prevlibname'] @$_lib_size_with_buffer_aligned
*/
#define $@ value['libname'].upper() @$_START_OFFSET $@ value['liboffset'] @$
$$$@@@ endfor @@@$$$

#endif /***** __$@tHeaderName.upper()@$_H *********/
"""

############################################################
# Function - Functions - Functions - Functions - Functions
############################################################
# Append data into list
def addToHeaderDataList(libName, libSize, bufferSize, libBufSize,
                        alignedBuf, libOffset, prevLibName):
    distHashDefineTemplate = {
        'libname': libName,
        'libsize': libSize,
        'buffersize' : bufferSize,
        'libbufsize': libBufSize,
        'alignedbuf': alignedBuf,
        'liboffset' : libOffset,
        'prevlibname' : prevLibName
    }

    headerDataList.append(distHashDefineTemplate)
    return headerDataList


# Function to Parse subprocess cmd 'size' to get sum of '.text' size
def parseObjectFileGetTextSection(file):
    try:
        textSize = 0
        # print("file name: ", file)
        p = subprocess.Popen("size "+file, stdout=subprocess.PIPE, shell=True)
        for line in p.stdout:
            token = line.split()
            if(token[0] != b'text'):
                textSize = int(token[0]) + textSize
        return textSize
    except:
        print("Oops! func: parseObjectFileGetTextSection ", sys.exc_info()[0],
        "occurred.")
        return 0


# Function to Parse .map file and get required file .text size
def parseMapFileGetTextSection(mapFileName, libName):
    strSize = 0
    # Ignore the first 3 bytes in libName
    lib =  '.' + libName[3:len(libName)]

    # creating the cmd
    # example: cat mapfile.map | grep -w -A 1".cmnsromcore"
    # Some cases section size not in one line, it will extend upto 2 lines
    cmdStr = 'cat ' + mapFileName + ' | grep -w -A 1 "' + lib + '"'
    p = subprocess.Popen(cmdStr, stdout=subprocess.PIPE, shell=True)
    stdout, stderr = p.communicate()

    if (len(stdout) < 1):
        print('.'+libName+
                  " doesn't exit's, Please check the map file and re-run")
        print("Oops! func: parseMapFileGetTextSection ", sys.exc_info()[0],
        "occurred.")
        sys.exit()

    lineSplitData = stdout.split()

    strSize =  int(lineSplitData[2], 16)
    return strSize


# Fucntion to validate JSON format
def validateJSON(jsonData):
    try:
        json.loads(jsonData)
    except ValueError as err:
        return False
    return True


# Function to read file
def getDataFromFile(fileName):
    try:
        with open(fileName, 'r+') as f:
            data = f.read()
            f.close()
            return data
    except:
        print("Oops! func: getJsonDataFromFile | Json File missing",
        sys.exc_info()[0])
        exit()

# Function to generate header file from JSON
def genarateHeaderFile(args:argparse.Namespace):
    print('Generate Header file from JSON')

    # Reading Json form file
    fileData = getDataFromFile(args.libLayoutJsonFile)

    # Validating JSON Format
    if(validateJSON(fileData) != True):
        print("Oops! validateJSON: INVALID json format, Exiting...")
        exit()

    # Load data to json
    jsonData = json.loads(fileData)

    # start Offset convert hex to decimal
    nextLibStartOffset = int(args.startOffset, 16)
    currLibTotalSize = 0

    try:
        mapLen = len(args.mapFile)
        mapFile = SBE_ROOT_PATH + "/" + args.mapFile
        if(args.mapFile[0] == '.'):
            mapFile = SBE_ROOT_PATH + args.mapFile[1:len(args.mapFile)]
        print("Overide the .map file")
    except:
        mapFile = SBE_ROOT_PATH + "/" + jsonData['mapfile']

    # Make sure the file exists
    if not os.path.isfile(mapFile):
        print("The ", mapFile , " file does not exists. \
        Please check the file path and re-run")
        sys.exit(1)

    alignedOffset = 0
    alignedBufSize = 0
    prevLibName = 'initial'

    # Parsing JSON data
    for data in jsonData['layoutdata']:
        libName = data['libname']
        extraByte = data['libbuffer']
        libPath = data['libpath']

        textSectionSize = parseMapFileGetTextSection(mapFile, libName)

        #Adding the extra Bytes to the buffer
        currLibTotalSize = textSectionSize + extraByte

        # Align the offset address with double word (8bytes)
        alignDiff = currLibTotalSize % 8
        if(alignDiff > 0):
            alignedBufSize = currLibTotalSize + (8 - alignDiff)
        else:
            alignedBufSize = currLibTotalSize

        # align the input offset address as well verify address are
        # aligned before create the header file
        alignDiff = nextLibStartOffset % 8
        if(alignDiff > 0):
            alignedOffset = nextLibStartOffset + (8 - alignDiff)
        else:
            alignedOffset = nextLibStartOffset

        # Store lib data to list
        addToHeaderDataList(libName, hex(textSectionSize), hex(extraByte),
                            hex(currLibTotalSize), hex(alignedBufSize),
                            hex(alignedOffset), prevLibName)

        #currentlibStartoffset = previouslibstartoffset+previouslibsize
        # Next lib start offset calcualtion
        nextLibStartOffset = nextLibStartOffset + alignedBufSize
        prevLibName = libName


    # Seperating path with file name
    hdrFile = args.outputHdrFilePath
    hdrFileName = hdrFile.split('/')
    hdrFileName = hdrFileName[len(hdrFileName) - 1] #getting headername
    hdrFileName = hdrFileName.split('.')[0]#discard .h

    # creating header file
    customHdrFileTemplate = templateUtil.customTemplate(headerFileTemplate)
    renderedHdrFile = customHdrFileTemplate.render(
        tHeaderName = hdrFileName, listdata = headerDataList)

    # Write the rendered template data into a file
    with open(args.outputHdrFilePath, "w") as  libHeaderFile:
        libHeaderFile.write(renderedHdrFile)
        libHeaderFile.close()


################################## Main #######################################
# Command line options
# Create the argparser object
# We'll do it in a way so we can create required/optional cmdline arg groups
def main():
    print("Tool to generate lib layout .H file using lib layout JSON")

    argparser = argparse.ArgumentParser(
        description="Tool to generate Lib layout start offset .H from JSON",
        prog='Lib Layout Gen',
        add_help=False,
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog=textwrap.dedent('''
        Version: ''' + str(toolVersion) + '''

        '''))

    # Create our group of required cmdline args
    reqgroup = argparser.add_argument_group('Required Arguments')
    reqgroup.add_argument('-i', '--libLayoutJsonFile',
                          required=True,
                          help="Lib Layout JSON File")


    reqgroup.add_argument('-s', '--startOffset',
                          required=True,
                          help="Section Start Offset Address. ex: 0x800")

    # Create our group of optional cmdline args
    optgroup = argparser.add_argument_group('Optional Arguments')
    optgroup.add_argument('-h', '--help',
                          action="help",
                          help="Show this help message and exit")

    # Create our group of optional cmdline args
    # Optional argument used for orverride the .map file in JSON
    optgroup = argparser.add_argument_group('Optional Arguments')
    optgroup.add_argument('-m', '--mapFile',
                    help="Overide option for .map file from given JSON file")

    # Create our group of subparser cmdline args
    subparsers = argparser.add_subparsers(dest='Check --help')
    subparsers.required = True

    # Subparser for .H file generation that can be used by lib layout
    hdrgenerator = subparsers.add_parser(
        "hdrgenerator",
        help="Generate lib layout Header from JSON File"
    )
    hdrgenerator.set_defaults(func = genarateHeaderFile)
    hdrgenerator.add_argument('-o', '--outputHdrFilePath',
                            required=True,
                            help="Output file name path of lib layout header")

    # cmdline loaded up, now parse for it and handle what is found
    args = argparser.parse_args()

    # Make sure the file exists
    if not os.path.isfile(args.libLayoutJsonFile):
        print(textwrap.dedent('''The JSON File does not exist.\
            Please check the file path and re-run'''))
        sys.exit(1)

    # Run the appropriate function
    args.func(args)


# PYTHON MAIN
if __name__ == "__main__":
    main()
