#!/usr/bin/env python3
# IBM_PROLOG_BEGIN_TAG
# This is an automatically generated prolog.
#
# $Source: public/src/tools/debug/plugins/ebmc/o4500.py $
#
# OpenPOWER sbe Project
#
# Contributors Listed Below - COPYRIGHT 2021,2024
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

import json
import os
import sys
from udparsers.helpers.miscUtils import getLid
from udparsers.poztraceutils.ppe2fsp import get_sbe_trace_data_as_string
from udparsers.pozdebugutils.ffdcparser import PlatFfdcPackage, FfdcPackageHeader_t
from udparsers.helpers.hostfw_trace import get_binary_trace_data_as_string

def getTraceStrData(i_traceBinDataFile, i_stringFile):
    traces = None

    with open(i_traceBinDataFile, "rb") as traceBinDataFileFD:
        traceBinData = traceBinDataFileFD.read()

    # Get the human understandable trace data from the trace binary data
    startingPosition = 0
    printNumberOfTraces = -1 # -1 means to get all traces
    (retVal, traceDataString, warningMessages) = \
            get_binary_trace_data_as_string(traceBinData, startingPosition,
                                            printNumberOfTraces, i_stringFile)
    traces = traceDataString.split("\n")

    return traces

def getTraces(i_pkTraceBinFile, stringFileCommitId):
    # Get trace binary data from the PK trace
    traceBinDataFile = "/tmp/pozTraceBin"
    #If we don't pass the endianness to the below function, then it will 
    #default to sys.byteorder, which is little-endian on eBMC systems. 
    #Hence, numerical values printed in the SBE trace will be incorrect. 
    #To avoid this, specify that numerical values printed in SBE trace 
    #are stored in "big" endian format.
    get_sbe_trace_data_as_string(i_pkTraceBinFile, traceBinDataFile, "big")
    lidMap = {
        "RT_STRING_FILE":"81e006bd.lid",
        "GLDN_STRING_FILE":"81e006be.lid"
    }
    stringFileLid = lidMap["RT_STRING_FILE"]
    if (stringFileCommitId == 0x35b89840):
        stringFileLid = lidMap["GLDN_STRING_FILE"]

    stringFile = getLid(stringFileLid)
    if stringFile == "":
        errMsg = "String file [" + stringFileLid + "] is not found"
        errMsg += " to get traces"
        return errMsg

    # Get the human understandable trace data from the trace binary data
    traces = getTraceStrData(traceBinDataFile, stringFile)

    # Remove traceBinDataFile
    if os.path.exists(traceBinDataFile):
        os.remove(traceBinDataFile)

    return traces

# This is like a callback function that will be invoked from the BMC PEL infra
# based on the component id.
# Refer: https://github.com/openbmc/phosphor-logging/tree/master/extensions/openpower-pels#adding-python3-modules-for-pel-userdata-and-src-parsing
def parseUDToJson(subType, ver, data):
    pkTraceBinFile = "/tmp/pozPkTraceBin"
    userData = dict()
    try:
        #  - fwDetails
        #  - traces
        # create dummy header and pass that
        ffdcHeader = FfdcPackageHeader_t(magicBytes=0xFBAD, len=0, seqId=0,
                                        cmdClass=0, cmd=0, slid=0, severity=0,
                                        chipId=0, fapiRc=0x2000001)
        ffdcHdrByteArr = bytearray(ffdcHeader)
        ffdcHdrByteArr.extend(bytearray(data))
        platFfdcPackage = PlatFfdcPackage(ffdcHeader, ffdcHdrByteArr)
        platPkgHeader = platFfdcPackage.plat_header
        userData["fwDetails"]= {
                            "PrimaryRc":hex(platPkgHeader.priRc),
                            "SecondaryRc":hex(platPkgHeader.SecRc),
                            "FWCommitId":hex(platPkgHeader.fwCommitId),
                            "DD Major":hex(platPkgHeader.ddMajor),
                            "DD Minor":hex(platPkgHeader.ddMinor),
                            "Thread Id":hex(platPkgHeader.threadId)
                        }
        sbeFwCommitId = platPkgHeader.fwCommitId
        traceBlob =[]
        if "trace" in platFfdcPackage.blobs:
            traceBlob = platFfdcPackage.blobs['trace']
        with open(pkTraceBinFile, 'wb') as traceBinFile:
            traceBinFile.write(platFfdcPackage.blobs['trace'])
        if os.path.exists(pkTraceBinFile):
            trace = getTraces(pkTraceBinFile, sbeFwCommitId)
            if trace is not None:
                userData["traces"] = trace
            os.remove(pkTraceBinFile)
    except Exception as e:
        userData["Exception_Error"] = e
    userDataJsonStr = str()
    if userData is not None:
        userDataJsonStr = json.dumps(userData)

    return userDataJsonStr
