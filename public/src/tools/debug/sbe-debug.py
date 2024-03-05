#!/usr/bin/env python3
# IBM_PROLOG_BEGIN_TAG
# This is an automatically generated prolog.
#
# $Source: public/src/tools/debug/sbe-debug.py $
#
# OpenPOWER sbe Project
#
# Contributors Listed Below - COPYRIGHT 2024
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


'''@package sbe-debug
Tool compatible for Hardware, SIMICS and FILE. Tool to extract/parse the given
DUMP

'''

#standard library imports
import os
import argparse
import pathlib
import gzip
import struct
import subprocess
from tabulate import tabulate
from shutil import copyfileobj
from typing import Optional, TextIO

from icecream.icecream import ic

'''
! Tool version
'''
TOOL_VERSION = 1.1

'''
! Memory map
'''
memory_map = {
    'ody' : # Odyssey Chip type
    {
        'sppe':  # SBE Unit type
        {
            'pibmem' : (0xFFF80000, 0x00080000),  # PIBMEM start Image
            'img'    : (0xFFF80000, 0x00080000)   # SPPE Image
        },
        'bldr': # Bootloader Unit type
        {
            'pibmem' : (0xFFF80000, 0x00080000),  # PIBMEM start Image
            'img'    : (0xFFFEE600, 0x00010000) # Bootloader Image
        }
    },
    'pst' : # PST Chip type
    {
        # TODO have to update fro PST
    }

}


class ParseError(Exception):
    '''
    A new type of Exception that can be thrown for parsing related error.
    '''
    pass


class AddressRangeError(Exception):
    """Custom exception class for address range errors."""
    pass


class metadata():
    '''
    ! Metadata parsing class
    '''

    '''
    ! CONST variable
    '''
    IMG_ASCII = b'\x49\x4D\x47' # 0x494D47
    TRA_ASCII = b'\x54\x52\x41' # 0x545241
    LDA_ASCII = b'\x4c\x44\x41' # 0x4c4441
    GIT_ASCII = b'\x47\x49\x54' # 0x474954
    DAT_ASCII = b'\x44\x41\x54' # 0x444154
    HEA_ASCII = b'\x48\x45\x41' # 0x484541
    BLD_ASCII = b'\x42\x4c\x44' # 0x424c44
    TPE_ASCII = b'\x54\x50\x45' # 0x545045
    IID_ASCII = b'\x49\x49\x44' # 0x494944
    ATR_ASCII = b'\x41\x54\x52' # 0x415452
    META_CHAR_LIST = [IMG_ASCII, TRA_ASCII, IID_ASCII, LDA_ASCII, GIT_ASCII, DAT_ASCII, HEA_ASCII, BLD_ASCII, ATR_ASCII, TPE_ASCII]

    def __init__(self, pibmemFile: str, pibmemStart: int, pibmemSize: int) -> None:
        self.pibmemFile  = pibmemFile
        self.pibmemStart = pibmemStart
        self.pibmemSize  = pibmemSize
        print (f"Metadata file: {pibmemFile}, size: {os.path.getsize(pibmemFile)}")

    def parseMetadata(self, data: bytes, index: int) -> dict:
        '''
        ! Parse metadata
          @param data pibmem data
          @param index start index of meta
          @return dict of metadata { tag: value, tag: value .... }
                    hint: { tag(bytes): value(bytes), tag(bytes): value(bytes), .... }
        '''
        metadata = dict()

        while True:
            tag    = data[index : index+3]
            # length = int.from_bytes( data[index+3], "big" )
            length = data[index+3]

            if self.isMetaTag(tag):
                index += 4

                value = data[ index : index+(length*4) ]
                index += (length*4)

                metadata[tag] = value
            else:
                break
        return metadata

    def isMetaTag(self, data: bytes) -> bool:
        for pattern in self.META_CHAR_LIST:
            if data == pattern:
                return True
        return False

    def getMetaBaseAddr(self, data: bytes) -> int:
        metabaseoffset = self.pibmemSize - 4
        medataddr = data[metabaseoffset:(metabaseoffset+4)]

        medataddr = int.from_bytes( medataddr, 'big' )
        return medataddr

    def getMetadata(self) -> dict:
        metadata = None
        with open(self.pibmemFile, 'rb') as f:
            data = f.read()

            if self.pibmemSize != len(data):
                print (f"Invalid file size {len(data):08X}, expected {self.pibmemSize:08X}")
                exit(1)

            metaAddr = self.getMetaBaseAddr(data)
            metadata = self.parseMetadata(data, (metaAddr - self.pibmemStart))

        self.metadata = metadata
        self.display()

        return metadata

    def display(self):
        def c_int(data: bytes) -> int:
            return int.from_bytes(data, 'big' )

        def s_int(data: bytes) -> str:
            return ('0x'+''.join(format(byte, '02X') for byte in data))

        def s_str (data: bytes) -> str:
            null_byte_index = data.find(b'\x00')
            string_data = data[:null_byte_index].decode('utf-8')
            return string_data

        m = self.metadata
        print ("-"*80)
        for key in self.metadata:
            if key in [self.TRA_ASCII, self.HEA_ASCII, self.ATR_ASCII]:
                print (f"TAG: {str(key, 'utf-8')}, Value: addr- {s_int(m[key][:4])} len- {s_int(m[key][4:])}")
            elif key in [self.IMG_ASCII]:
                print (f"TAG: {str(key, 'utf-8')}, Value: {c_int(m[key])}")
            elif key in [self.BLD_ASCII, self.IID_ASCII]:
                print (f"TAG: {str(key, 'utf-8')}, Value: {s_str(m[key])}")
            else:
                print (f"TAG: {str(key, 'utf-8')}, Value: {s_int(m[key])}")
        print ("-"*80)


class utilsTools:
    def executeCommand (self, command):
        '''
        ! Execute Command function
          @param command command need to execute on console
          @return command execute result
        '''
        try:
            # Execute the command and capture the result
            result = subprocess.run(command, shell=True)
            if result.returncode != 0:
                print ("Command failed")
                print ("Error: ", result.stderr)
            return result
        except Exception as e:
            print ("Error occurred: ", str(e))
            exit (1)

    def getFile(self, filePath: TextIO, fileHint: str):
        '''
        ! select file from given directory using file name hint
            @param filePath   file path
            @param fileHint   file name hint
            @return           file / None for not found
        '''

        print ("getFile: ", fileHint)
        # Iterate over files in the directory
        for filepath in filePath.iterdir():
            if filepath.is_file():
                filename = filepath.name.lower()

                if fileHint in filename:
                    return filepath
        return None

    def parseSbeTrace(self, toolPath, traceBinPath, traceStringFilePath, outFilePath, printTrace = True):
        pp2fspFile   = self.getFile (toolPath, 'ppe2fsp')
        fsptraceFile = self.getFile (toolPath, 'fsp-trace' )

        outfile = str(outFilePath / 'trace.txt')
        sbetracefile = str(outFilePath / 'sbetrace.bin')
        cmd_parse_to_fsp_format = str(pp2fspFile) + ' -i ' + str(traceBinPath) + ' -o ' + sbetracefile
        cmd_parse_fsp_trace = str(fsptraceFile) + ' -s ' + str(traceStringFilePath) + ' ' + sbetracefile + ' > ' + outfile
        cmd_print_output = 'cat ' + outfile

        print (cmd_parse_to_fsp_format)
        print (cmd_parse_fsp_trace)

        self.executeCommand ( cmd_parse_to_fsp_format )
        self.executeCommand ( cmd_parse_fsp_trace )
        if printTrace == True:
            self.executeCommand(cmd_print_output)


class pibmemMisc:
    """
    ! Common tools class
    """
    def __init__(self, pibmemFile: str, startAddr: int, imgSize: int) -> None:
        '''
        ! Common Tool initialization function
          @param pibmemDumpFile pibmem dump file
        '''
        self.pibmemDumpFile = open(pibmemFile, "rb")
        self.imgStartAddr   = startAddr
        self.imgSize        = imgSize
        print ("Size of file ", hex(os.path.getsize(pibmemFile)))

    def pibmemDumpRead (self, addr: int, length: int) -> "bytes":
        '''
        ! Read PIBMEM Dump file
          @param addr    address to read data
          @param len     length
          @return        file data
        '''
        if (self.imgStartAddr + self.imgSize) < (addr + length):
            raise AddressRangeError(f"Address range exceeds the maximum allowed range of {self.imgStartAddr + self.imgSize}.")

        offset = addr - self.imgStartAddr
        print (f"pibmemDumpRead, Addr: {addr:08X}, len: {length}, offset: {offset:08X} ")
        # Move file pointer to offset location
        self.pibmemDumpFile.seek ( offset )
        fileData = self.pibmemDumpFile.read( length )
        # reset file pointer
        self.pibmemDumpFile.seek ( 0, 0 )
        return fileData

    def pibmemDumpReadWord (self, addr: int) -> "bytes":
        '''
        ! Read PIBMEM Dump file
          @param addr    address to read data
          @param len     length
          @return        file data
        '''
        if (self.imgStartAddr + self.imgSize) < (addr + length):
            raise AddressRangeError(f"Address range exceeds the maximum allowed range of {self.imgStartAddr + self.imgSize}.")

        offset = addr - self.imgStartAddr
        print (f"pibmemDumpReadWord, Addr: {addr:08X}, offset: {offset:08X} ")
        # Move file pointer to offset location
        self.pibmemDumpFile.seek ( offset )
        fileData = self.pibmemDumpFile.read( 4 )

        print (f"pibmemDumpReadWord, Addr: {addr:08X}, offset: {offset:08X}, Data: {fileData} ")
        # reset file pointer
        self.pibmemDumpFile.seek ( 0 )
        return fileData


# TODO Hardware related tool will cover in phase2
class hwTools:
    pass


def hw_parse_dump(args):
    pass


class fileTools(utilsTools):
    """
    ! Files tools class
    """

    '''
    ! CONST variable
    '''
    PLAT_DUMP_FILE_PIBMEM_REG       = "pibms_reg_dump"
    PLAT_DUMP_FILE_SBE_LOCAL_REG    = "sbe_localreg_dump"
    PLAT_DUMP_FILE_PPE_STATE        = "ppe_state"
    PLAT_DUMP_FILE_PIBMEM_DUMP      = "pibmem_dump"

    def __init__(self, file: str, outputPath:Optional[pathlib.Path] = None, isSysDumpFile: bool = True) -> None:
        '''
        ! Class constructor
        '''
        self.isSysDumpFile = isSysDumpFile
        if isSysDumpFile == True:
            self.file  = file
            if (self.file.endswith(".gz")):
                print("gunzip on file: %s" % self.file)
                gzfileTmp=gzip.open(self.file, 'rb')
                fileObj=open(self.file[:-3], 'wb+')

                #Copy the unzipped file content into a new file
                copyfileobj(gzfileTmp, fileObj)
                gzfileTmp.close()
                fileObj.close()
                #Use the unzipped file else where
                self.file=self.file[:-3]

            self.structEndian, self.endianness = self.getEndian()
        else:
            self.file = file
            self.endianness = 'big'

        # Set default output path if not provided
        if outputPath is None:
            outputPath = pathlib.Path('./')
        self.outputpath = outputPath

        # Create the directory if it does not exist
        self.outputpath.mkdir(parents=True, exist_ok=True)
        self.platDumpPath = self.outputpath / 'plat_dump'

    def getEndian(self):
        '''
        Get Endian's of a system basis bit 0x290 in dump header.
        If set implies BMC system => little endian
        If not set implies FSP system => big endian

        @return "<" BMC system dump file (little endian file)
                ">" FSP system dump file (big endian file)
        '''
        print ("# Checking Endianness for given file")
        if(self.file == ''):
            raise NameError("untared dump bin file name empty")
            exit(-1)

        fileObj=open(self.file, 'rb')

        print("Parsing the Dump header")
        dumpSummaryOffset = 0xD0
        missingSecIndicatorOffset = 0x83

        # If the missing section indicator is on, then adjust the dump summary location
        fileObj.seek(missingSecIndicatorOffset, 0)
        oneByteChunk = fileObj.read(1)
        missingSecIndicator = struct.unpack('b', oneByteChunk)[0]
        print("Missing section indicator is %s" %hex(missingSecIndicator))
        if (hex(missingSecIndicator) == 0x01):
            print("Adjusting the dump summary location")
            dumpSummaryOffset -= 0x30

        isBmcSystemOffset = dumpSummaryOffset + 0x290
        fileObj.seek(isBmcSystemOffset, 0)
        isBmcSystem = fileObj.read(1)
        isBmcSystem = struct.unpack('B',isBmcSystem)
        if(isBmcSystem[0] == 1):
            print("BMC System: %s" % isBmcSystem[0])
            endianChar = "<"
            endianness = "little"
        else:
            print("Not BMC System: %s" % isBmcSystem[0])
            endianChar = ">"
            endianness = "big"

        fileObj.close()
        return endianChar, endianness

    def sbeState(self):
        '''
        ! Parse SBE state for given dump file
        '''
        print ("# Parse sbe state -")
        print ("self.isSysDumpFile: ", self.isSysDumpFile)
        if self.isSysDumpFile:
            sbestatefile = self.getFile( self.platDumpPath, self.PLAT_DUMP_FILE_PIBMEM_REG )
        else:
            sbestatefile = self.file
        print("File path: ", str(sbestatefile))
        fileHandle = open(sbestatefile, "rb")
        l_cnt = 0

        tableName = "- SBE STATE"
        print ("-" * 80)
        print (tableName)
        print ("-" * 80)

        headers = ["Reg Number", "Reg Value", "Reg String"]
        regList = []
        try:
            while(l_cnt < os.path.getsize(sbestatefile)):
                str1 = int.from_bytes( fileHandle.read(8), byteorder=self.endianness )
                str2 = fileHandle.read(32).decode()
                str3 = int.from_bytes( fileHandle.read(4), byteorder=self.endianness )
                str4 = int.from_bytes( fileHandle.read(8), byteorder=self.endianness )
                l_cnt = l_cnt + 52

                regList.append ( [f"{str1:016X}", f"{str4:016X}", f"{str2:}"] )
        except:
            raise ValueError (f"Parsing given file failed, Please check "+str(sbestatefile)+" format")

        # Print data using tabulate
        print(tabulate(regList, headers=headers, tablefmt="grid"))
        fileHandle.close()

    def sbeLocalRegister(self):
        '''
        ! Parse SBE Local register for given dump file
        '''
        print ("# Parse sbe local register -")
        if self.isSysDumpFile:
            sbelocalreg = self.getFile(self.platDumpPath, self.PLAT_DUMP_FILE_SBE_LOCAL_REG)
        else:
            sbelocalreg = self.file
        print("File path: ", str(sbelocalreg))
        fileHandle = open(sbelocalreg, "rb")
        l_cnt = 0

        tableName = "- SBE LOCAL REGISTER"
        print ("-" * 80)
        print (tableName)
        print ("-" * 80)

        headers = ["Reg Number", "Reg Value", "Reg String"]
        regList = []
        try:
            while(l_cnt < os.path.getsize(sbelocalreg)):
                str1 = int.from_bytes( fileHandle.read(2), byteorder=self.endianness  )
                str2 = fileHandle.read(32).decode()
                str3 = int.from_bytes( fileHandle.read(8), byteorder=self.endianness  )
                l_cnt = l_cnt + 42
                regList.append ( [f"{str1:08X}", f"{str3:016X}", f"{str2}"] )
        except:
            raise ValueError (f"Parsing given file failed, Please check " + str(sbelocalreg) + " format")

        # Print data using tabulate
        print(tabulate(regList, headers=headers, tablefmt="fancy_grid"))
        fileHandle.close()

    def ppeState(self):
        '''
        ! Parse PPE state for given dump file
        '''
        print ("# Parse PPE state register -")

        # register name
        regNameMapSPR = {
                         # SPRs and XIR
                         9      : ["CTR" , False],
                         316    : ["DACR", False],
                         308    : ["DBCR", False],
                         22     : ["DEC" , False],
                         61     : ["EDR" , False],
                         3      : ["IR"  , False],
                         62     : ["ISR" , False],
                         63     : ["IVPR", False],
                         8      : ["LR"  , False],
                         286    : ["PIR" , False],
                         287    : ["PVR" , False],
                         272    : ["SPRG0",False],
                         26     : ["SRR0", False],
                         27     : ["SRR1", False],
                         340    : ["TCR" , False],
                         336    : ["TSR" , False],
                         1      : ["XER" , False],
                         42     : ["MSR" , False],
                         420    : ["CR"  , False],
                         1003   : ["XSR" , False],
                         1000   : ["IAR" , False],
                         1001   : ["IR"  , False],
                         1002   : ["XCR" , False],
                         }

        regNameMapGPR = {
                         # GPRs
                         0  : ["R0" , False] ,
                         1  : ["R1" , False] ,
                         2  : ["R2" , False] ,
                         3  : ["R3" , False] ,
                         4  : ["R4" , False] ,
                         5  : ["R5" , False] ,
                         6  : ["R6" , False] ,
                         7  : ["R7" , False] ,
                         8  : ["R8" , False] ,
                         9  : ["R9" , False] ,
                         10 : ["R10", False],
                         13 : ["R13", False],
                         28 : ["R28", False],
                         29 : ["R29", False],
                         30 : ["R30", False],
                         31 : ["R31", False]
                     }

        if self.isSysDumpFile:
            ppeStateFileName = self.getFile( self.platDumpPath, self.PLAT_DUMP_FILE_PPE_STATE )
        else:
            ppeStateFileName = self.file

        print("File path: ", str(ppeStateFileName))
        fileHandle = open(ppeStateFileName, "rb")

        # Table header
        tableName = "- PPE State "
        print ("-" * 80)
        print (tableName)
        print ("-" * 80)

        headers = ["Register", "Reg Value"]
        regList = []

        l_cnt = 0
        try:
            while(l_cnt < os.path.getsize(ppeStateFileName)):
                regNum = int.from_bytes( fileHandle.read(2), self.endianness  )

                str1 = ''
                try:
                    if((regNum in regNameMapSPR.keys())
                        and (regNameMapSPR[regNum][1] == False)):
                        str1 = regNameMapSPR[regNum][0]
                        regNameMapSPR[regNum][1] = True
                    else:
                        str1 = regNameMapGPR[regNum][0]
                        regNameMapGPR[regNum][1] = True
                except:
                    str1 = hex(regNum)

                str3 = int.from_bytes( fileHandle.read(4), self.endianness  )
                str4 = fileHandle.read(4)
                #2Bytes(SPR/GPR number) + 4Bytes(Value) + 4Bytes(Name)
                l_cnt = l_cnt + 10

                regList.append( [str1, f"{str3:08X}"] )

        except:
            raise ValueError (f"Parsing given file failed, Please check " + str(ppeStateFileName) + " format")

        # Print data using tabulate
        print(tabulate(regList, headers=headers, tablefmt="grid"))
        fileHandle.close()

    def collectTrace (self, args):
        '''
        ! Collect trace parse level
        '''
        print ("\r\n\r\n", "-" * 80)
        def checkAndCorrectEndianness(inputfile, outputfile):
            if (self.endianness == 'little'):
                self.executeCommand("hexdump -v -e \'1/8 \"%016x\"\' -e \'\"\\n\"\' " + inputfile + "| xxd -r -p > " + outputfile)
            else:
                outputfile = inputfile


        if self.isSysDumpFile:
            pibmemDump = self.getFile(self.platDumpPath, self.PLAT_DUMP_FILE_PIBMEM_DUMP)
        else:
            pibmemDump = self.file

        pibmemDumpInfile = str( pibmemDump )
        pibmemDumpOutfile = '/'.join(pibmemDumpInfile.rsplit('/', 1)[0:1] + ["pibmemdump_out.bin"])

        checkAndCorrectEndianness(pibmemDumpInfile, pibmemDumpOutfile)
        pibmemDump = pibmemDumpOutfile

        tmpEndianness = self.endianness
        self.endianness = 'big'

        print("File path: ", str(pibmemDump))

        pibmemStart = memory_map[args.chip][args.unit]['pibmem'][0]
        pibemeSize  = memory_map[args.chip][args.unit]['pibmem'][1]
        metaobj = metadata( pibmemDump, pibmemStart, pibemeSize)
        meta    = metaobj.getMetadata()

        pibmem = pibmemMisc (pibmemDump, pibmemStart, pibemeSize)

        traceAddr = int.from_bytes( meta[metaobj.TRA_ASCII][:4], self.endianness )
        traceLen  = int.from_bytes( meta[metaobj.TRA_ASCII][4:], self.endianness )

        traceData = pibmem.pibmemDumpRead (traceAddr, traceLen)

        self.outputpath.mkdir (mode=0o777, parents=True, exist_ok=True)
        ffdcTraceFileName = str(self.outputpath / 'ffdc_plat_trace_response_blob.bin')

        # Print and parsing trace
        if os.path.exists(ffdcTraceFileName):
            os.remove(ffdcTraceFileName)

        with open(ffdcTraceFileName, "wb") as traceFile:
            traceFile.write ( traceData )
            traceFile.close()

        ffdcTraceFile = pathlib.Path( ffdcTraceFileName )

        self.parseSbeTrace (args.tracetoolpath, ffdcTraceFile, args.stringfile, self.outputpath, True )
        self.endianness = tmpEndianness


    def extract (self):
        '''
        ! Extract DUMP
            create compressed extracted dump file which is output.binary along
            with created the plat_dump directory for store the plat dump like
            localreg_dump, pibmem_reg_dump, pibmem_dump, ppe_state
        '''

        print("Parsing the Dump header")
        fileObj=open(self.file, 'rb')
        dumpSummaryOffset = 0xD0
        missingSecIndicatorOffset = 0x83

        # If the missing section indicator is on, then adjust the dump summary location
        fileObj.seek(missingSecIndicatorOffset, 0)
        oneByteChunk = fileObj.read(1)
        missingSecIndicator = struct.unpack('b', oneByteChunk)[0]
        print("Missing section indicator is %s" %hex(missingSecIndicator))
        if (hex(missingSecIndicator) == 0x01):
            print("Adjusting the dump summary location")
            dumpSummaryOffset -= 0x30

        #Just for info print lets parse header and check system type
        self.getEndian()
        #By default we read data as big endian for FSP and BMC system
        endianChar = ">"

        #Read the dump content type
        dumpContentOffset = dumpSummaryOffset + 0x260
        fileObj.seek(dumpContentOffset, 0)
        fourByteChunk = fileObj.read(4)
        dumpContentType = struct.unpack(endianChar + 'i', fourByteChunk)[0]
        print("Dump Content Type is %s" %hex(dumpContentType))

        hwDataLenOffest = 0x40 + 0x30 + 0x18 #FileHeaderLength + summarySectionLen + HWSectionLenOffset
        hwDataOffset = 0x4D0

        #Read the sysdatalength.
        fileObj.seek(hwDataLenOffest, 0)
        fourByteChunk = fileObj.read(4)
        sysDataSize = struct.unpack(endianChar + 'i', fourByteChunk)[0]
        print("Sys data size is %s" %hex(sysDataSize))

        #Read the hwdatalength.
        fileObj.seek(hwDataLenOffest + 4, 0)
        fourByteChunk = fileObj.read(4)
        hwDataSize = struct.unpack(endianChar + 'i', fourByteChunk)[0]
        print("HW data size is %s" %hex(hwDataSize))

        length = hwDataSize - sysDataSize
        print("Length of tar is %s" %hex(length))

        if self.outputpath.exists():
            outputPath = str(self.outputpath / 'output.binary')
        else:
            outputPath = "./output.binary"
        cmd = "dd skip=" + str(hwDataOffset) + " count=" + str(length) + " if=" + self.file  + " of="+ outputPath + " bs=1"
        print("Command is %s" %cmd)
        self.executeCommand(cmd)

        # Untar the HW data
        if self.outputpath.exists():
            cmd = str(self.outputpath)
        else:
            cmd = "./"
        cmd = "tar -xvf " + outputPath +" -C " + cmd
        print("Command is %s" %cmd)
        self.executeCommand(cmd)

        #Close the fileObj.
        fileObj.close()


def file_extract_dump(args):
    print("\n\n##- Extract dump File -##")
    extractfile = fileTools (args.file, args.outputpath)
    extractfile.extract()


def file_parse_dump(args):
    print("\n\n##- Extract and Parse dump File -##")
    # file arg
    if args.individualFile != None:
        if len ( args.dumptype ) > 1:
            raise ValueError ("For Individual File, accept one level for a parse level file")
        parsefiletools = fileTools (args.individualFile, args.outputpath, False)

    if args.dumpfile != None:
        if 'trace' in args.dumptype:
            # required arg
            if not (hasattr(args, "str") & hasattr(args, "tracetoolpath")):
                raise AttributeError ("trace level requires args are: project, img, sys, str, tracetoolpath")

        parsefiletools = fileTools (args.dumpfile, args.outputpath)
        parsefiletools.extract()

    if 'all' in args.dumptype:
        parsefiletools.ppeState()
        parsefiletools.sbeLocalRegister()
        parsefiletools.sbeState()
        parsefiletools.collectTrace(args)
    else:
        for level in args.dumptype:
            print (f" level: {level}")
            if level == 'ppestate':
                parsefiletools.ppeState()
            elif level == 'localregister':
                parsefiletools.sbeLocalRegister()
            elif level == 'sbestate':
                parsefiletools.sbeState()
            elif level == 'trace':
                parsefiletools.collectTrace(args)


def validateChipUnit(parser, args):
    '''
    ! Validate chip unit type
     @param parser argparse object
     @param args   argparse args
    '''
    # Odyssey chip unit validation
    odyUnitType = ['bldr', 'sppe']
    if args.chip == 'ody':
        if args.unit not in odyUnitType:
            print (f"[ERROR] - Chip {args.chip} supports unit types {odyUnitType}\n")
            parser.print_help()
            exit(1)

    # PST chip unit validation
    pstUnitType = ['bldr', 'sbe', 'sppe', 'tsbe']
    if args.chip == 'pst':
        if args.unit not in pstUnitType:
            print (f"[ERROR] - Chip {args.chip} supports unit types {pstUnitType}")
            parser.print_help()
        print ('[ERROR] - sbe-debug tool not support "PST" chip unit\n')
        exit(1)


# import argparse
if __name__ == '__main__':
    parser = argparse.ArgumentParser(description="Tools to Parse SBE DUMP V{TOOL_VERSION}")
    parser.add_argument('-v', '--version', action='version',    version=f'%(prog)s {TOOL_VERSION}', help='show the version')
    parser.add_argument("--verbose",       action="store_true", help="Increase verbosity")

    CHIP_UNIT = ['ody', 'pst']
    IMG_TYPE  = ['bldr', 'sppe', 'sbe', 'tsbe']
    parser.add_argument('chip', metavar=f'chip {CHIP_UNIT}', nargs='?', choices=CHIP_UNIT, help='Enter the chip type')
    parser.add_argument('unit', metavar=f'unit {IMG_TYPE}',  nargs='?', choices=IMG_TYPE,  help='Enter the chip unit image type')

    subparsers = parser.add_subparsers()
    fileparser = subparsers.add_parser('FILE', help='Parse DUMP from FILE')
    # trace, ppe-state, local-register, pibmem-register
    filesubparsers = fileparser.add_subparsers()

    extractparser  = filesubparsers.add_parser('extract',     help='Extract DUM')
    extractparser.add_argument('-f', '--file', required=True, help='Dump file')
    extractparser.add_argument("-o","--outputpath", type=pathlib.Path, default='./', help="Extract file path")
    extractparser.set_defaults(func=file_extract_dump)

    parsedumpparser  = filesubparsers.add_parser("parse", help="Parse DUMP")
    DUMP_PARSE_LEVEL = ['ppestate', 'localregister', 'sbestate', 'trace', 'all']
    exclusivegroup = parsedumpparser.add_mutually_exclusive_group(required=True)
    exclusivegroup.add_argument  ("-dumpfile",          help="System Dump file 'SYSDUMP' file")
    exclusivegroup.add_argument  ("-individualFile",    help="Dump parse level file, applies only to a single level for single level")
    parsedumpparser.add_argument ("-dumptype",          nargs='+', required=True, choices=DUMP_PARSE_LEVEL, help='Dump parse type')
    parsedumpparser.add_argument ("-o",  "--outputpath",type=pathlib.Path,               default='./',      help="Extract file path")
    parsedumpparser.add_argument ("-stringfile",                                         help='Required for trace: SBE String file for Trace parse')
    parsedumpparser.add_argument ("-tracetoolpath",     type=pathlib.Path,               help='Required for trace: SBE Trace tool path (ppe2fsp, fsp-trace) for Trace parse')
    parsedumpparser.set_defaults ( func=file_parse_dump )

    args = parser.parse_args()

    if not hasattr(args, "func"):
        raise NotImplementedError ("Function default not implemented")
        exit(1)

    validateChipUnit(parser, args)
    args.func(args)
