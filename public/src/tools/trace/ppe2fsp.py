#!/usr/bin/env python3
# IBM_PROLOG_BEGIN_TAG
# This is an automatically generated prolog.
#
# $Source: public/src/tools/trace/ppe2fsp.py $
#
# OpenPOWER sbe Project
#
# Contributors Listed Below - COPYRIGHT 2020,2024
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

# Akhilesh S - created 07/21/2021
# Python module to convert ppe trace to bmc/fsp trace format

############################################################
# Imports - Imports - Imports - Imports - Imports - Imports
############################################################
#Python Provided
import sys
import os
import argparse
import textwrap
import struct

############################################################
# Variables - Variables - Variables - Variables - Variables
############################################################

#Tool version
toolVersion = 1.1

#PK Trace Version supported by this tool
supportedPkTraceVersions = [2, 4]

#Maximum Input size
maxInputSize = 0x2040       #8k

#Maximum Output size
maxOutputSize = 4 * maxInputSize

#Size of word/uint32_t
sizeofWord = 4

#Size of byte/uint8_t
sizeofByte = 1

#Verbose mode
verbose = False

#FSP trace bin file endian format
fspTraceEndianFormat = ""

#Trace formats that are supported
traceFormats = {
    0x0 : "PK_TRACE_FORMAT_EMPTY", # will be treated as PK_TRACE_FORMAT_SPECIAL in v4
    0x1 : "PK_TRACE_FORMAT_TINY",
    0x2 : "PK_TRACE_FORMAT_BIG",
    0x3 : "PK_TRACE_FORMAT_BINARY"
    }

# special tags in v4 format
traceFormatsSpecialTags = {
    "EMPTY"         : 0x0,
    "TIME_BUMP"     : 0x1,
    "THREAD_CHANGE" : 0x2,
}

#FSP Trace time flag options
fspTraceTimeFlagOptions = { "TRAC_TIME_REAL"   : 0x00,
                            "TRAC_TIME_50MHZ"  : 0x01,
                            "TRAC_TIME_200MHZ" : 0x02,
                            "TRAC_TIME_167MHZ" : 0x03
        }

#FSP Trace formats
fspTraceFormats = { "TRACE_FIELDTRACE" : 0x4654,       #Field Trace - "FT"
                    "TRACE_FIELDBIN"   : 0x4644        #Binary Field Trace - "FD"
        }

############################################################
# Classes - Classes - Classes - Classes - Classes - Classes
############################################################
class pkTraceBuffer():
    """
    Python usable representation of pk trace buffer.
    /ppe-p10/src/import/chips/p10/common/ppe/ppetrace/pk_trace.h
    """

    def __init__(self):

        self.version = 0
        self.rsvd = 0
        self.imageStr = ""
        self.instanceId = 0
        self.partialTraceHash = 0
        self.hashPrefix = 0
        self.size = 0
        self.maxTimeChange = 0
        self.hz = 0
        self.pad = 0
        self.timeAdj64 = 0
        self.tbu32 = 0
        self.offset = 0
        self.cb = 0                                          #PK Circular trace buffer

    def populateParams(self, ppeTraceBinFile):
        #2 Bytes of version
        self.version = int.from_bytes(ppeTraceBinFile[:2], "big")
        assert self.version in supportedPkTraceVersions, "PPE to FSP invalid version: %s" % self.version

        #2 Byte rsvd
        self.rsvd = int.from_bytes(ppeTraceBinFile[2:4], "big")

        #16 Byte imageStr
        self.imageStr = ppeTraceBinFile[4:20]

        #2 Byte instanceId
        self.instanceId = int.from_bytes(ppeTraceBinFile[20:22], "big")

        #2 Byte partialTraceHash
        self.partialTraceHash = int.from_bytes(ppeTraceBinFile[22:24], "big")

        #2 Byte hashPrefix
        self.hashPrefix = int.from_bytes(ppeTraceBinFile[24:26], "big")

        #2 Byte size
        self.size = int.from_bytes(ppeTraceBinFile[26:28], "big")

        if(self.version == 2):
            #4 byte maxTimeChange
            self.maxTimeChange = int.from_bytes(ppeTraceBinFile[28:32], "big")
            self.threadId = None
        else:
            self.maxTimeChange = None
            self.threadId = int.from_bytes(ppeTraceBinFile[28:29], "big")

        #4 Byte hz
        self.hz = int.from_bytes(ppeTraceBinFile[32:36], "big")

        #4 Byte pad
        self.pad = int.from_bytes(ppeTraceBinFile[36:40], "big")

        self.timeAdj64 = int.from_bytes(ppeTraceBinFile[40:48], "big")

        #4Bytes of tbu32
        self.tbu32 = int.from_bytes(ppeTraceBinFile[48:52], "big")

        #4bytes of offset
        self.offset = int.from_bytes(ppeTraceBinFile[52:56], "big")

        #Rest all data is actual trace info. Lets not convert it to any form . We
        #will keep as it is.
        self.cb = memoryview(ppeTraceBinFile[56:])

        assert len(self.cb) <= maxInputSize, "Input stream exceeds max size of %s bytes" % maxInputSize
        assert len(ppeTraceBinFile) > (len(ppeTraceBinFile) - len(self.cb) ), "Input Buffer too small for ppe trace buffer: %s" % len(ppeTraceBinFile)
        assert self.size != 0, "Input trace buff size is zero %s" % self.size
        assert (self.size & (self.size-1)) == 0, "Trace buffer Size is not power of two: %s" % self.size
        assert (self.offset & 0x7) == 0, "Invalid PPE offset: %s" % self.offset

    def printParams(self):
        print("Version: %s" % hex(self.version))
        print("rsvd: %s" % hex(self.rsvd))
        print("imageStr: %s " % self.imageStr)
        print("instanceId: %s" % hex(self.instanceId))
        print("partialTraceHash: %s" % hex(self.partialTraceHash))
        print("hashPrefix: %s" %  hex(self.hashPrefix))
        print("size: %s" % hex(self.size))
        print("maxTimeChange: %s" % hex(self.maxTimeChange))
        print("hz: %s" % hex(self.hz))
        print("pad: %s" % hex(self.pad))
        print("timeAdj64: %s" % hex(self.timeAdj64))
        print("tbu32: %s" % hex(self.tbu32))
        print("offset: %s" % hex(self.offset))
        #print("cb(Circular Buffer Content): %s" % self.cb)
        print("======================================")

class pkTraceEntry():
    """
    Python usable representation of PkTraceGeneric.
    /ppe-p10/src/import/chips/p10/common/ppe/ppetrace/pk_trace.h
    """

    def __init__(self):

        self.pkTraceHash = 0
        self.param = 0
        self.complete = 0
        self.bytes_or_parms_count = 0
        self.timestamp = 0
        self.traceFormat  = 0
        self.params = list()

        #For internal use
        self.traceNumber = 0
        self.bytes_or_parms_count_roundoff = 0
        self.ppe_time64 = 0x0
        self.ppe_timeAdjusted = 0x0
        self.totalTraceEntrySize = 0x0
        self.isTraceComplete = 0x01          #By Default trace is complete

    def populateParams(self, pkTraceHeader, traceBin, offset, pkTraceBuffSize, traceEntryNumber):

        self.header : pkTraceBuffer = pkTraceHeader

        #Store it for returing
        actualOffset = offset

        #PkTraceGeneric is total of 8 bytes. Lets move 8bytes behind to grab
        #this information.
        PkTraceGenericSize = 8
        offset -= PkTraceGenericSize

        #Mask the offset to trace buffer size
        offset &= (pkTraceBuffSize - 1)

        #2 Bytes of pkTraceHash
        self.pkTraceHash = int.from_bytes(traceBin[offset:(offset + 2)], "big")

        #2 Byte of param in case of tiny trace . For other trace formats this is
        #broken down into 1 byte of complete and 1 byte of bytes_or_parms_count
        offset += 2
        self.param = int.from_bytes(traceBin[offset:(offset + 2)], "big")

        #1 Byte of complete
        self.complete = int.from_bytes(traceBin[offset:(offset + 1)], "big")

        #1 Byte of bytes_or_parms_count
        offset += 1
        self.bytes_or_parms_count = int.from_bytes(traceBin[offset:(offset + 1)], "big")

        #30 Bits of timestamp
        offset += 1
        temp = int.from_bytes(traceBin[offset:(offset + 4)], "big")
        self.timestamp = temp & 0xFFFFFFFC

        #2 Bits of traceFormat
        self.traceFormat = temp & 0x00000003

        #Now we are back to the original offset
        offset += 4

        #Update the trace number
        self.traceNumber = traceEntryNumber

        self.tbu32Change = 0
        self.threadSwitch = None

        #Get Parameter Values based on trace format

        if (traceFormats[self.traceFormat] == "PK_TRACE_FORMAT_TINY"):

            #If trace format is tiny, it can accmodate single 16bit param.
            #This is already a part of PkTraceGeneric
            self.params.append(self.param)

            #Update the total trace entry size
            self.totalTraceEntrySize = PkTraceGenericSize

            #Return the offset by pointing it to next trace entry
            return (actualOffset - self.totalTraceEntrySize)

        elif (traceFormats[self.traceFormat] == "PK_TRACE_FORMAT_BIG"):

            #If trace format is big, it can accmodate max 4 words(32 bits)
            #The number of params can be obtained from self.bytes_or_parms_count
            #If self.bytes_or_parms_count is odd roundoff to nearest even
            #number, to ensure its 8 byte aligned
            assert self.bytes_or_parms_count <= 4, "PK_TRACE_FORMAT_BIG exceding max param's count of 4"

            if ((self.bytes_or_parms_count % 2) != 0):
                self.bytes_or_parms_count_roundoff = self.bytes_or_parms_count + 1         #Odd
            else:
                self.bytes_or_parms_count_roundoff = self.bytes_or_parms_count             #Even

            paramsCount = self.bytes_or_parms_count_roundoff
            paramOffset = ((actualOffset - PkTraceGenericSize - (paramsCount * sizeofWord)) & (pkTraceBuffSize - 1))

            #Check if there is enough bytes left to copy from offset.This check
            #is needed when there is a roll over and part of the params are on
            #other side of the circular buffer
            bytesLeft = pkTraceBuffSize - paramOffset

            while(paramsCount and bytesLeft):
                param = int.from_bytes(traceBin[paramOffset:(paramOffset + sizeofWord)], "big")
                self.params.append(param)
                paramsCount -= 1
                paramOffset += sizeofWord
                bytesLeft -= sizeofWord

            #Now copy the rest of the data starting from the beginning of the
            #circular buffer if there was a roll over and part of params was at
            #beginning of circular buffer
            if paramsCount:
                paramOffset = 0

                while(paramsCount):
                    param = int.from_bytes(traceBin[paramOffset:(paramOffset + sizeofWord)], "big")
                    self.params.append(param)
                    paramsCount -= 1
                    paramOffset += sizeofWord

            #Update the total trace entry size
            self.totalTraceEntrySize = PkTraceGenericSize + (self.bytes_or_parms_count_roundoff * sizeofWord)

            #Return the offset by pointing it to next trace entry
            return (actualOffset - self.totalTraceEntrySize)

        elif (traceFormats[self.traceFormat] == "PK_TRACE_FORMAT_BINARY"):

            paramsCountRounded = (self.bytes_or_parms_count + 7) & (~0x00000007)
            self.bytes_or_parms_count_roundoff = paramsCountRounded
            paramOffset = ((actualOffset - PkTraceGenericSize - (paramsCountRounded)) & (pkTraceBuffSize - 1))

            #Check if there is enough bytes left to copy from offset.This check
            #is needed when there is a roll over and part of the params are on
            #other side of the circular buffer
            bytesLeft = pkTraceBuffSize - paramOffset

            while(paramsCountRounded and bytesLeft):
                param = int.from_bytes(traceBin[paramOffset:(paramOffset + sizeofByte)], "big")
                self.params.append(param)
                paramsCountRounded -= 1
                paramOffset += sizeofByte
                bytesLeft -= sizeofByte

            #Now copy the rest of the data starting from the beginning of the
            #circular buffer if there was a roll over and part of params was at
            #beginning of circular buffer
            if paramsCountRounded:
                paramOffset = 0

                while(paramsCountRounded):
                    param = int.from_bytes(traceBin[paramOffset:(paramOffset + sizeofByte)], "big")
                    self.params.append(param)
                    paramsCountRounded -= 1
                    paramOffset += sizeofByte

            #Update the total trace entry size
            self.totalTraceEntrySize = PkTraceGenericSize + self.bytes_or_parms_count_roundoff

            #Return the offset by pointing it to next trace entry
            return (actualOffset - self.totalTraceEntrySize)

        else: # PK_TRACE_FORMAT_EMPTY or PK_TRACE_FORMAT_SPECIAL
            # get type of special tag
            specialTagType = (self.pkTraceHash & 0xFF00) >> 8

            if((self.header.version == 2) or (specialTagType == traceFormatsSpecialTags["EMPTY"])):
                #Return zero which implies all trace entries are parsed.There cannot
                #be a empty trace entry
                return 0

            if(specialTagType == traceFormatsSpecialTags["TIME_BUMP"]):
                self.tbu32Change = ((self.pkTraceHash & 0x00FF) << 16) | self.param
            elif(specialTagType == traceFormatsSpecialTags["THREAD_CHANGE"]):
                # we can assume thread index will not be more than 2 bytes
                self.threadSwitch = self.param
            else : # TODO PFSBE-690 thread number support
                assert False, "Invalid special trace tag"

            #Update the total trace entry size
            self.totalTraceEntrySize = PkTraceGenericSize

            #Return the offset by pointing it to next trace entry
            return (actualOffset - self.totalTraceEntrySize)

    def calculate64bitTime(self, pkTraceHdr, previousEntry):
        if(self.header.version == 2):
            #Only for first entry
            if self.traceNumber == 1:

                self.ppe_time64 = ((pkTraceHdr.tbu32 & 0xffffffff) << 32)
                self.ppe_time64 |= self.timestamp

            else:

                time_diff32 = previousEntry.timestamp - self.timestamp
                time_diff32 &= 0x00000000ffffffff

                if (time_diff32 > pkTraceHdr.maxTimeChange):
                    time_diff32 = self.timestamp - previousEntry.timestamp
                    self.ppe_time64 = previousEntry.ppe_time64 + time_diff32
                else:
                    self.ppe_time64 = previousEntry.ppe_time64 - time_diff32
        else:
            if self.tbu32Change != 0:
                pkTraceHdr.tbu32 -= self.tbu32Change

            self.ppe_time64 = ((pkTraceHdr.tbu32 & 0xffffffff) << 32)
            self.ppe_time64 |= self.timestamp

        #Adjust the time with timeAdj64 parameter from header
        self.ppe_timeAdjusted = self.ppe_time64 + pkTraceHdr.timeAdj64
        #Keep the result to 64bit
        self.ppe_timeAdjusted &= 0xffffffffffffffff

    def setThreadId(self, pkTraceHdr:pkTraceBuffer):
        if self.threadSwitch != None:
            pkTraceHdr.threadId = self.threadSwitch

        self.threadId = pkTraceHdr.threadId

    def printParams(self):
        print("-----------------------------------")
        print("Trace Number : %s" % self.traceNumber)
        print("pkTraceHash: %s" % hex(self.pkTraceHash))
        print("param: %s" % hex(self.param))
        print("tbu32Change: %s" % hex(self.tbu32Change))
        print("complete: %s" % hex(self.complete))
        print("bytes_or_parms_count: %s" % hex(self.bytes_or_parms_count))
        print("Internal Use: bytes_or_parms_count_roundoff: %s" % hex(self.bytes_or_parms_count_roundoff))
        print("timestamp: %s" % hex(self.timestamp))
        print("Internal Use: ppe_time64: %s ==> %s" % (hex(self.ppe_time64), self.ppe_time64))
        print("Internal Use: ppe_timeAdjusted: %s ==> %s" % (hex(self.ppe_timeAdjusted), self.ppe_timeAdjusted))
        print("traceFormat: %s : %s" % (hex(self.traceFormat), traceFormats[self.traceFormat]))
        print("Param: %s ==> %s" % ([hex(x) for x in self.params], self.params))
        print("Internal Use: totalTraceEntrySize: %s" % self.totalTraceEntrySize)
        print("Internal Use: isTraceComplete: %s" % self.isTraceComplete)

class fspTraceHeader():
    """
    Python usable representation of trace_buf_head_t
    /ppe-p10/src/tools/trace/trac_interface.h
    """
    def __init__(self):

        self.ver = 0x0
        self.hdr_len = 0x0
        self.time_flg = 0x0
        self.endian_flg = 0x0
        self.comp = 0x0
        self.size = 0x0
        self.times_wrap = 0x0
        self.next_free = 0x0
        self.te_count = 0x0
        self.extracted = 0x0

    def populateParams(self, pkTraceBuffHdr):

        #1Byte: Trace buffer version
        self.ver = 0x01

        #1Byte: size of this struct(trace_buf_head_t) in bytes
        self.hdr_len = 0x28

        #1Byte: meaning of timestamp entry field
        self.time_flg = fspTraceTimeFlagOptions["TRAC_TIME_REAL"]

        #1Byte: flag for big ('B') or little ('L') endian
        self.endian_flg = ord(fspTraceEndianFormat[0].upper())

        #16Bytes: the buffer name as specified in init call
        self.comp = pkTraceBuffHdr.imageStr

        #4Bytes: size of buffer, including this struct.Will be updated at end
        #With the right value
        self.size = self.hdr_len

        #4Bytes: how often the buffer wrapped
        self.times_wrap = 0x01

        #4Bytes: offset of the byte behind the latest entry
        self.next_free = self.hdr_len

        #4Bytes: Updated each time a trace is done
        self.te_count = 0x0

        #4Bytes: Not currently used
        self.extracted = 0x0

    def printParams(self):

        print("")
        print("FSP Header")
        print("ver: %s" % hex(self.ver))
        print("hdr_len: %s" % hex(self.hdr_len))
        print("time_flg: %s " % hex(self.time_flg))
        print("endian_flg: %s : %s" % (hex(self.endian_flg), chr(self.endian_flg)))
        print("comp: %s" % self.comp)
        print("size: %s" % hex(self.size))
        print("times_wrap: %s" % hex(self.times_wrap))
        print("next_free: %s" % hex(self.next_free))
        print("te_count: %s" % hex(self.te_count))
        print("extracted: %s" % hex(self.extracted))

class fspTraceEntry():
    """
    Python usable representation of a single fsp entry
    /ppe-p10/src/tools/trace/trac_interface.h
    """

    def __init__(self):

        #trace_entry_stamp_t
        self.tbh = 0x0
        self.tbl = 0x0
        self.tid = 0x0

        #trace_entry_head_t
        self.length = 0x0
        self.tag = 0x0
        self.hashId = 0x0
        self.line = 0x0

        self.data = list()

        self.size = 0x0

        #Internal use
        self.fspEntryCount = 0

    def populateParams(self, pkTraceBuff, pkTraceEntry:pkTraceEntry):

        #size of total entry excluding data as it varies from entry to entry
        entrySizeExcludingData = 28

        #Convert PPE time to FSP time format and popylate tbh and tbl
        self.convertPpeTimeToFspTimeFormat(pkTraceBuff.hz, pkTraceEntry.ppe_timeAdjusted)

        if(pkTraceBuff.version == 2):
            #use the ppe instance id as the thread id
            self.tid = pkTraceBuff.instanceId
        else:
            self.tid = pkTraceEntry.threadId

        #merge the hash prefix and the string_id fields together for a 32 bit hash value
        self.hashId = int((format(pkTraceBuff.hashPrefix, "04x") + format(pkTraceEntry.pkTraceHash, "04x")) , 16)

        #determine the FSP trace format
        if (traceFormats[pkTraceEntry.traceFormat] == "PK_TRACE_FORMAT_BINARY"):
            self.tag = fspTraceFormats["TRACE_FIELDBIN"]
        else:
            self.tag = fspTraceFormats["TRACE_FIELDTRACE"]

        #Populate length and data based on trace format
        if (traceFormats[pkTraceEntry.traceFormat] == "PK_TRACE_FORMAT_TINY"):

            self.length = sizeofWord
            self.data = pkTraceEntry.params

        elif (traceFormats[pkTraceEntry.traceFormat] == "PK_TRACE_FORMAT_BIG"):

            if pkTraceEntry.isTraceComplete == 0x1:
                #If PK trace is complete

                if (pkTraceEntry.bytes_or_parms_count % 2) != 0:                            #If the params count is odd
                    self.length = pkTraceEntry.bytes_or_parms_count_roundoff * sizeofWord

                    #Last param is not the data, so we can delete it
                    self.data = pkTraceEntry.params
                    del self.data[-1]
                else:                                                                       #If the params count is even
                    self.length = (pkTraceEntry.bytes_or_parms_count_roundoff * sizeofWord) + sizeofWord

                    #All params are actual params/data
                    self.data = pkTraceEntry.params

            else:
                #If PK trace is in-complete(not all parm data had been written at the time the trace was
                #captured) then we will write a trace to the fsp buffer that says
                #"PARTIAL TRACE ENTRY.  HASH_ID = %d"

                self.length = sizeofWord
                self.data.append(self.hashId)

                #Update With partial trace hash
                self.hashId = int((format(pkTraceBuff.hashPrefix, "04x") + format(pkTraceBuff.partialTraceHash, "04x")) , 16)

        elif (traceFormats[pkTraceEntry.traceFormat] == "PK_TRACE_FORMAT_BINARY"):

            if pkTraceEntry.isTraceComplete == 0x1:
                #If PK trace is complete
                self.length = pkTraceEntry.bytes_or_parms_count_roundoff

                #All params are actual params/data
                self.data = pkTraceEntry.params

            else:
                #If PK trace is in-complete(not all parm data had been written at the time the trace was
                #captured) then we will write a trace to the fsp buffer that says
                #"PARTIAL TRACE ENTRY.  HASH_ID = %d"

                self.length = sizeofWord
                self.data.append(self.hashId)

                #Update With partial trace hash
                self.hashId = int((format(pkTraceBuff.hashPrefix, "04x") + format(pkTraceBuff.partialTraceHash, "04x")) , 16)

        #set the line number to 1
        self.line = 0x01

        #Total size of one entry
        self.size = entrySizeExcludingData + self.length

        #Add a extra word specifying the size of one entry excluding the word size in params.
        #Applicable only for Big trace if trace is complete
        #This has no significance although. Can be any junk as well.
        if ((traceFormats[pkTraceEntry.traceFormat] == "PK_TRACE_FORMAT_BIG") and
                    pkTraceEntry.isTraceComplete == 0x1):
            self.data.append(self.size - sizeofWord)

        #Update the fsp entry count with pkTrace entry count
        self.fspEntryCount = pkTraceEntry.traceNumber

    def convertPpeTimeToFspTimeFormat(self, pkKernalFrequency, pkTraceEntry64BitTime):

        #convert from ppe ticks to seconds and nanoseconds
        seconds = int(pkTraceEntry64BitTime / pkKernalFrequency)
        remainder = pkTraceEntry64BitTime - (seconds * pkKernalFrequency)
        nseconds = int((remainder * 1000000000) / pkKernalFrequency)
        fsp_time64 = ((seconds << 32) | nseconds)
        fsp_time64 &= 0xffffffffffffffff                      #Keep the result to 64bit

        #Populate FSP trace time parameters(LSB, MSB)
        self.tbh = fsp_time64 >> 32
        self.tbl = fsp_time64 & 0x00000000ffffffff

    def printParams(self):

        print("")
        print("Fsp Entry: %s" % self.fspEntryCount)
        print("tbh: %s" % hex(self.tbh))
        print("tbl: %s" % hex(self.tbl))
        print("tid: %s" % hex(self.tid))
        print("length: %s" % hex(self.length))
        print("tag: %s" % hex(self.tag))
        print("hashId: %s" % hex(self.hashId))
        print("line: %s" % hex(self.line))
        print("data(Last word is not significant.Not part of data): %s ==> %s" % ([hex(x) for x in self.data], self.data))
        print("size %s" % hex(self.size))

############################################################
# Function - Functions - Functions - Functions - Functions
############################################################
#Parse the pk trace buffer and extract header and all trace info
def parsePkTraceBuff(ppeTraceBinFile):

    #PK Trace Buffer object, lets return this as it has generic data as well
    pkTraceBuff = pkTraceBuffer()

    #Return parameter containing list of all trace entries
    pkTraceEntryAll = list()

    #Store total number of traces
    totalTraces = 0

    file = open(ppeTraceBinFile, "rb")

    #Read PK Trace Buffer
    pkTraceBuff.populateParams(file.read())

    file.close()

    #Store the current offset
    curOffset = pkTraceBuff.offset

    #Lets keep a count of total bytes left to parse
    bytesLeft = pkTraceBuff.size

    #Read PK Trace Entry
    while(curOffset > 0 and bytesLeft > 0):

        traceEntry = pkTraceEntry()
        totalTraces += 1
        curOffset = traceEntry.populateParams(pkTraceBuff, pkTraceBuff.cb, curOffset, pkTraceBuff.size, totalTraces)

        #Update the timing info parameter for the trace
        traceEntry.calculate64bitTime(pkTraceBuff, pkTraceEntryAll[-1] if traceEntry.traceNumber != 1 else "")

        if(pkTraceBuff.version != 2):
            traceEntry.setThreadId(pkTraceBuff)

        #Update bytes left
        bytesLeft -= traceEntry.totalTraceEntrySize

        pkTraceEntryAll.append(traceEntry)

    #Check if bytes left was less than the actual entry size. If so mark the
    #trace as incomplete
    if bytesLeft < 0:
        pkTraceEntryAll[-1].isTraceComplete = 0x0

    return (pkTraceBuff,pkTraceEntryAll)

#Convert pk trace header and all trace info into fsp trace format
def ppe2fsp(pkTraceBuff, pkTraceEntryAll):

    #Return param
    fspTraceHdr = fspTraceHeader()

    #Return Param consisting of list of all ppe2fsp entries
    ppe2fspEntriesAll = list()

    fspTraceHdr.populateParams(pkTraceBuff)

    if verbose:
        pkTraceBuff.printParams()
        fspTraceHdr.printParams()

    for entry in pkTraceEntryAll:

        if (traceFormats[entry.traceFormat] == "PK_TRACE_FORMAT_EMPTY"):
            continue

        singleFspTraceEntry = fspTraceEntry()
        singleFspTraceEntry.populateParams(pkTraceBuff, entry)

        #Keep updating the total size in fsp header
        fspTraceHdr.size += singleFspTraceEntry.size

        #update the fsp header to reflect the true size and entry count
        fspTraceHdr.te_count = singleFspTraceEntry.fspEntryCount

        if verbose:
            entry.printParams()
            singleFspTraceEntry.printParams()

        ppe2fspEntriesAll.append(singleFspTraceEntry)

    if verbose:
        print("")
        print("Updated FSP Trace Header")
        fspTraceHdr.printParams()

    return (fspTraceHdr, ppe2fspEntriesAll)

#Create the fsp trace bin file based on fsp header and all fsp trace entries
def fspTraceBin(fspTraceBinFile, fspTraceHeader, ppe2fspEntriesAll):

    file = open(fspTraceBinFile, "wb")

    #Basis endian format select struct pack endian char
    endianChar = "<" if fspTraceEndianFormat == "little" else ">"

    #Reverse the ppe2fspEntriesAll list as we will start writing the last fsp
    #entry in first
    ppe2fspEntriesAll.reverse()

    #Write the FSP header into bin file
    file.write(struct.pack("B", fspTraceHeader.ver))
    file.write(struct.pack("B", fspTraceHeader.hdr_len))
    file.write(struct.pack("B", fspTraceHeader.time_flg))
    file.write(struct.pack("B", fspTraceHeader.endian_flg))
    file.write(struct.pack("16s", fspTraceHeader.comp))
    file.write(struct.pack(endianChar + "I", fspTraceHeader.size))
    file.write(struct.pack(endianChar + "I", fspTraceHeader.times_wrap))
    file.write(struct.pack(endianChar + "I", fspTraceHeader.next_free))
    file.write(struct.pack(endianChar + "I", fspTraceHeader.te_count))
    file.write(struct.pack(endianChar + "I", fspTraceHeader.extracted))

    for entry in ppe2fspEntriesAll:

        #Write fsp entrie into bin file
        file.write(struct.pack(endianChar + "I", entry.tbh))
        file.write(struct.pack(endianChar + "I", entry.tbl))
        file.write(struct.pack(endianChar + "I", entry.tid))

        file.write(struct.pack(endianChar + "H", entry.length))
        file.write(struct.pack(endianChar + "H", entry.tag))
        file.write(struct.pack(endianChar + "I", entry.hashId))
        file.write(struct.pack(endianChar + "I", entry.line))

        for data in entry.data:
            if(entry.tag == fspTraceFormats["TRACE_FIELDBIN"]):
                file.write(struct.pack("B", data))
            else:
                file.write(struct.pack(endianChar + "I", data))

        file.write(struct.pack(endianChar + "I", entry.size))

    file.close()

    #Lets do some checks on the output file
    fspTraceBinFileSize = os.path.getsize(fspTraceBinFile)
    assert fspTraceBinFileSize == fspTraceHeader.size, "Mismatch in FSP Trace Bin file size"
    assert fspTraceBinFileSize <= maxOutputSize, "FSP trace bin file exceeds max size %s" % maxOutputSize

def get_sbe_trace_data_as_string(ppeTraceBinFile, o_fspTraceBinFile, i_fspTraceEndianFormat=sys.byteorder):

    global fspTraceEndianFormat
    fspTraceEndianFormat = i_fspTraceEndianFormat
    #Parse contents of PK trace bufer
    (pkTraceBuff, pkTraceEntryAll) = parsePkTraceBuff(ppeTraceBinFile)

    #Convert ppe trace to fsp trace format
    (fspTraceHdr, ppe2fspEntriesAll) = ppe2fsp(pkTraceBuff, pkTraceEntryAll)

    #Create the FSP trace bin file
    fspTraceBin(o_fspTraceBinFile, fspTraceHdr, ppe2fspEntriesAll)

################################## Main ###############################################

if __name__ == '__main__':
    ################################################
    # Command line options
    # Create the argparser object
    # We'll do it in a way so we can create required/optional cmdline arg groups
    argparser = argparse.ArgumentParser(description="Tool To convert PPE trace to FSP trace format",
                                        add_help=False,
                                        formatter_class=argparse.RawDescriptionHelpFormatter,
                                        epilog=textwrap.dedent('''
    Version: ''' + str(toolVersion) + '''

    Examples:  > ppe2fsp.py -i <ppeTraceBinFile> -o <fspTraceBinFile>

    '''))

    # Create our group of required cmdline args
    reqgroup = argparser.add_argument_group('Required Arguments')
    reqgroup.add_argument('-i', '--ppeTraceBinFile', required=True, help="PPE Trace Bin File which needs to be converted to FSP trace format")
    reqgroup.add_argument('-o', '--fspTraceBinFile', required=True, help="Output file in FSP trace format")

    # Create our group of optional cmdline args
    optgroup = argparser.add_argument_group('Optional Arguments')
    optgroup.add_argument('-h', '--help', action="help", help="Show this help message and exit")
    optgroup.add_argument('-v', '--verbose', action='store_true', help="Show all traces")
    optgroup.add_argument('-e', '--endianFormat', help="Endian format of FSP trace", choices=["big","little","auto"], default = "big")

    # cmdline loaded up, now parse for it and handle what is found
    args = argparser.parse_args()

    # Get the PPE Trace Bin file path
    ppeTraceBinFile = args.ppeTraceBinFile

    # Get the FSP Trace Bin file name and output path
    fspTraceBinFile = args.fspTraceBinFile

    # Get the verbose flag
    verbose = args.verbose

    # Get the fsp trace bin file endian format.
    # By default this is always auto, based on machine script will internally
    # find endian format and create the fsp trace bin.
    # But an option has been provided to create fsp trace bin file as per user choice.
    fspTraceEndianFormat = args.endianFormat

    #If endian format is auto lets find out the endian format based on the machine
    if fspTraceEndianFormat == "auto":
        fspTraceEndianFormat = sys.byteorder

    get_sbe_trace_data_as_string(ppeTraceBinFile, fspTraceBinFile, fspTraceEndianFormat)

    print(textwrap.dedent(''' \n Converted ppe trace to fsp trace successfully.
          Output path: %s''' % fspTraceBinFile ))
