# IBM_PROLOG_BEGIN_TAG
# This is an automatically generated prolog.
#
# $Source: public/src/tools/debug/ffdcparser.py $
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
"""! @brief FFDC parser Support parser FFDC and print the truncated trace as
            well as full trace. But it not parse any HWP local data nor HWP reg
            data.
"""
from collections.abc import Iterable
import copy
from ctypes import (
    c_uint32, c_uint64,
    BigEndianStructure, sizeof
)
from typing import Any

MAGIC_CODE_BYTES                = 0xFBAD
RESPONSE_CODE_BYTES             = 0xC0DE
FAPI2_RC_PLAT_ERROR_SEE_DATA    = 0x2000001
DUMP_FIELD_SBE_FFDC_TRACE_DATA  = 0x0004


# ref: public/src/runtime/common/core/sbeSpMsg.H, struct: sbeResponseFfdc_t
class FfdcPackageHeader_t(BigEndianStructure):
    _pack_ = 1
    _fields_ = [
        ("magicBytes" , c_uint32, 16),
        ("len"        , c_uint32, 16),
        ("seqId"      , c_uint32, 16),
        ("cmdClass"   , c_uint32,  8),
        ("cmd"        , c_uint32,  8),
        ("slid"       , c_uint32, 16),
        ("severity"   , c_uint32,  8),
        ("chipId"     , c_uint32,  8),
        ("fapiRc"     , c_uint32    ),
    ]


# ref: public/src/runtime/common/plat/ffdc/sbeffdctype.H, struct: sbePlatHeader_t
class FfdcPlatHeader_t(BigEndianStructure):
    _pack_ = 1
    _fields_ = [
        ("priRc"      , c_uint32, 16),
        ("SecRc"      , c_uint32, 16),
        ("fwCommitId" , c_uint32    ),
        ("reserved"   , c_uint32,  8),
        ("ddMajor"    , c_uint32,  8),
        ("ddMinor"    , c_uint32,  8),
        ("threadId"   , c_uint32,  8),
    ]


# ref: public/src/runtime/common/plat/ffdc/sbeffdctype.H, struct: packageBlobField_t
class FfdcBlobField_t(BigEndianStructure):
    _pack_ = 1
    _fields_ = [
        ("fieldId",  c_uint32, 16),
        ("fieldLen", c_uint32, 16),
    ]


# ref: public/src/import/public/common/generic/fapi2/include/error_info_defs.H, struct: sbeFfdc_t
class FfdcHwpLocalVar_t(BigEndianStructure):
    _pack_ = 1
    _fields_ = [
        ("size", c_uint32),
        ("data", c_uint64),
    ]

class FfdcPackage(object):
    def __init__(
            self,
            i_ffdc_hdr: FfdcPackageHeader_t,
            i_package_buffer: bytearray) -> None:

        if(i_ffdc_hdr.magicBytes != MAGIC_CODE_BYTES):
            raise Exception("Invalid FFDC magic code recieved")

        self.header = i_ffdc_hdr
        self.raw_buffer = i_package_buffer
        self.offset = sizeof(FfdcPackageHeader_t)
        self.expected_blobs: dict[int,str] = dict()

        self.blobs: dict[str, bytearray] = dict()

    def _parseNextBlob(self) -> int:

        blob = FfdcBlobField_t.from_buffer(
            self.raw_buffer, self.offset)
        self.offset += sizeof(FfdcBlobField_t)

        self.blobs[self.expected_blobs[blob.fieldId]] = self.raw_buffer[
                self.offset : (self.offset + blob.fieldLen)]

        self.offset += blob.fieldLen

        return blob.fieldId

    def _parseAllBlobs(self) -> None:
        # actually dump field is not required to parse the ffdc.
        #  so its only used to validate sanity at the end of this function
        dump_field = int.from_bytes(
            self.raw_buffer[self.offset : (self.offset + 4)],
            'big')
        self.offset += 4

        expected_dump_field = 0

        while(self.offset < len(self.raw_buffer)):
            expected_dump_field |= self._parseNextBlob()

        if(dump_field != expected_dump_field):
            raise Exception("Invalid ffdc package")


class HwpFfdcPackage(FfdcPackage):
    def __init__(
            self,
            i_ffdc_hdr: FfdcPackageHeader_t,
            i_package_buffer: bytearray) -> None:
        super().__init__(i_ffdc_hdr, i_package_buffer)

        self.expected_blobs[0x1] = 'hwpLocal'
        self.expected_blobs[0x2] = 'hwpReg'

        self._parseAllBlobs()

        self._parseLocalVarFfdc()
        self._parseAllRegisterFfdcs()

    def _parseLocalVarFfdc(self) -> None:
        if('hwpLocal' in self.blobs):
            num_of_ffdcs = len(self.blobs['hwpLocal']) // sizeof(FfdcHwpLocalVar_t)
            self.hwp_lv_ffdc = (FfdcHwpLocalVar_t * num_of_ffdcs).from_buffer(self.blobs['hwpLocal'])

    def _parseNextRegisterFfdc(
            self,
            i_reg_ffdc_buf: bytearray) -> None:

        num_of_scoms = (len(i_reg_ffdc_buf) - 12) // 8
        class FfdcHwpRegFfdc_t(BigEndianStructure):
            _pack_ = 1
            _fields_ = [
                ("id",          c_uint32),
                ("target_pos",  c_uint32),
                ("scoms",       c_uint64 * num_of_scoms),
                ("size",        c_uint32),
            ]

        reg_ffdc = FfdcHwpRegFfdc_t.from_buffer(i_reg_ffdc_buf)
        self.hwp_reg_ffdcs.append(reg_ffdc)


    def _parseAllRegisterFfdcs(self) -> None:
        if('hwpReg' in self.blobs):
            self.hwp_reg_ffdcs = []

            buffer = copy.deepcopy(self.blobs['hwpReg'])
            while(True):
                # parse in backward direction
                size_of_last_reg_ffdc = int.from_bytes(buffer[-4:], 'big') * 4

                self._parseNextRegisterFfdc(buffer[-size_of_last_reg_ffdc:])

                if(size_of_last_reg_ffdc == len(buffer)):
                    break
                buffer = buffer[:-size_of_last_reg_ffdc]


class PlatFfdcPackage(FfdcPackage):
    def __init__(
            self,
            i_ffdc_hdr: FfdcPackageHeader_t,
            i_package_buffer: bytearray) -> None:
        super().__init__(i_ffdc_hdr, i_package_buffer)

        self.plat_header = FfdcPlatHeader_t.from_buffer(
            self.raw_buffer,
            self.offset);
        self.offset += sizeof(FfdcPlatHeader_t)

        self.expected_blobs[0x4] = 'trace'
        self.expected_blobs[0x8] = 'attr'

        self._parseAllBlobs()


class FfdcParser():
    '''
        After instantiating an object with a buffer, this object will have
          an object tree as described belwo. Please note that only high
          level tree is documented.

        Assume input buffer has 4 ffdc packages like below.
                HWP_PACKAGE  - SLID=3
                PLAT_APCKAGE - SLID=3
                HWP_PACKAGE  - SLID=4
                PLAT_APCKAGE - SLID=4

        Then this object will have following structure

        ffdc_packages (dict)
            |- ffdc_packages[3] (list)
            |       |- obj_of_HwpFfdcPackage (HwpFfdcPackage)
            |       |       |- header (FfdcPackageHeader_t)
            |       |       |       |- magicBytes
            |       |       |       |- len
            |       |       |       |- seqId
            |       |       |       |- cmdClass
            |       |       |       |- cmd
            |       |       |       |- slid
            |       |       |       |- severity
            |       |       |       |- chipId
            |       |       |       |- fapiRc
            |       |       |
            |       |       |- blobs['hwpLocal'] (bytearray) (optional)
            |       |       |- blobs['hwpReg'] (bytearray)   (optional)
            |       |
            |       |- obj_of_PlatFfdcPackage (PlatFfdcPackage)
            |               |- header (FfdcPackageHeader_t)
            |               |       |- magicBytes
            |               |       |- len
            |               |       |- seqId
            |               |       |- cmdClass
            |               |       |- cmd
            |               |       |- slid
            |               |       |- severity
            |               |       |- chipId
            |               |       |- fapiRc
            |               |
            |               |- plat_header (FfdcPlatHeader_t)
            |               |       |- priRc
            |               |       |- SecRc
            |               |       |- fwCommitId
            |               |       |- reserved
            |               |       |- ddMajor
            |               |       |- ddMinor
            |               |       |- threadId
            |               |
            |               |- blobs['trace'] (bytearray) (optional)
            |               |- blobs['attr'] (bytearray)  (optional)
            |
            |- ffdc_packages[4] (list) (similar as above)
    '''
    class StrictSet(set):
        def add(self, __element: Any) -> None:
            if(__element in self):
                raise KeyError(f'Value already exists')

            return super().add(__element)

        def update(self, *s: Iterable) -> None:
            raise NotImplementedError("Update is not supported")

    def __init__(self, i_ffdc_raw:bytearray):
        self.ffdc_packages: 'dict[int, list[FfdcPackage]]' = dict()
        # To detect whether we are getting more than one hwp ffdc with
        #  same slid or not.
        self.hwp_ffdc_slid: set[int] = FfdcParser.StrictSet()
        self.raw_ffdc_buffer = i_ffdc_raw
        self.offset = 0

        while(self.offset < len(self.raw_ffdc_buffer)):
            self._parseNextPackage()

    def _addNextPackageToDict(
            self,
            i_package: FfdcPackage) -> None:
        slid = i_package.header.slid
        if(type(i_package) == HwpFfdcPackage):
            self.hwp_ffdc_slid.add(slid)

        if(slid in self.ffdc_packages):
            self.ffdc_packages[slid].append(i_package)
        else:
            # Add new list into the dictionary
            self.ffdc_packages[slid] = [i_package]


    def _parseNextPackage(self) -> None:
        ffdc_header = FfdcPackageHeader_t.from_buffer(
            self.raw_ffdc_buffer,
            self.offset)

        # since both child class has same set of parameters,
        #  we call it in one place by changing class name conditionally
        #  using a variable.
        if(ffdc_header.fapiRc == FAPI2_RC_PLAT_ERROR_SEE_DATA):
            child_class = PlatFfdcPackage
        else:
            child_class = HwpFfdcPackage

        ffdc_package = child_class(
                ffdc_header,
                self.raw_ffdc_buffer[self.offset : self.offset+ffdc_header.len * 4])

        self._addNextPackageToDict(ffdc_package)
        self.offset += ffdc_header.len * 4
