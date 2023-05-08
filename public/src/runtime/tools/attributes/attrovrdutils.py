#!/usr/bin/env python3
# IBM_PROLOG_BEGIN_TAG
# This is an automatically generated prolog.
#
# $Source: public/src/runtime/tools/attributes/attrovrdutils.py $
#
# OpenPOWER sbe Project
#
# Contributors Listed Below - COPYRIGHT 2022,2023
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
from abc import ABC,abstractmethod
from enum import Enum
from typing import NamedTuple
import typing
import os,sys
import hashlib
import pickle

class Utils:
    def _getAttrHash(id):
        attr_hash16bytes = hashlib.md5(id.encode()).digest()
        attr_hash32bits = int.from_bytes(attr_hash16bytes[0:4], "big")
        attr_hash28bits = attr_hash32bits >> 4
        return attr_hash28bits


class Const:
    FORMAT_MAJOR_VERSION  = 0
    FORMAT_MINOR_VERSION  = 1


class Fapi2(ABC):
    @abstractmethod
    def isAbstract():
        pass

    class FileType(Enum):
        OVERRIDE = 1
        RESPONSE = 2
        LIST = 3
        DUMP = 4
        RAW_DUMP = 5

    class TargetType(Enum):
        LOG_TARGET_TYPE_SYSTEM              =  0 #< System type
        LOG_TARGET_TYPE_DIMM                =  1 #< DIMM type
        LOG_TARGET_TYPE_PROC_CHIP           =  2 #< Processor type
        LOG_TARGET_TYPE_MEMBUF_CHIP         =  3#< Membuf type
        LOG_TARGET_TYPE_EX                  =  4 #< EX - 2x Core, L2, L3 - can be deconfigured
        LOG_TARGET_TYPE_MBA                 =  5 #< MBA type
        LOG_TARGET_TYPE_MCS                 =  6 #< MCS type
        LOG_TARGET_TYPE_XBUS                =  7 #< XBUS type
        LOG_TARGET_TYPE_ABUS                =  8 #< ABUS type
        LOG_TARGET_TYPE_L4                  =  9 #< L4 type
        LOG_TARGET_TYPE_CORE                = 10 #< Core
        LOG_TARGET_TYPE_EQ                  = 11 #< EQ - 4x core, 2x L2, 2x L3 - can be deconfigured
        LOG_TARGET_TYPE_MCA                 = 12 #< MCA type
        LOG_TARGET_TYPE_MCBIST              = 13 #< MCBIST type
        LOG_TARGET_TYPE_MI                  = 14 #< MI Memory Interface (Cumulus)
        LOG_TARGET_TYPE_CAPP                = 15 #< CAPP target
        LOG_TARGET_TYPE_DMI                 = 16 #< DMI type
        LOG_TARGET_TYPE_OBUS                = 17 #< OBUS type
        LOG_TARGET_TYPE_OBUS_BRICK          = 18 #< OBUS BRICK type
        LOG_TARGET_TYPE_SBE                 = 19 #< SBE type
        LOG_TARGET_TYPE_PPE                 = 20 #< PPE type
        LOG_TARGET_TYPE_PERV                = 21 #< Pervasive type
        LOG_TARGET_TYPE_PEC                 = 22 #< PEC type
        LOG_TARGET_TYPE_PHB                 = 23 #< PHB type
        LOG_TARGET_TYPE_MC                  = 24 #< MC type
        LOG_TARGET_TYPE_OMI                 = 25 #< OMI type
        LOG_TARGET_TYPE_OMIC                = 26 #< OMIC type
        LOG_TARGET_TYPE_MCC                 = 27 #< MCC type
        LOG_TARGET_TYPE_OCMB_CHIP           = 28 #< OCMB type
        LOG_TARGET_TYPE_MEM_PORT            = 29 #< MEM_PORT type
        LOG_TARGET_TYPE_NMMU                = 30 #< NEST MMU type
        LOG_TARGET_TYPE_RESERVED            = 31 #< Reserved for Cronus (Z)
        LOG_TARGET_TYPE_PAU                 = 32 #< PAU type
        LOG_TARGET_TYPE_IOHS                = 33 #< IOHS type
        LOG_TARGET_TYPE_FC                  = 34 #< Fused Core type
        LOG_TARGET_TYPE_PMIC                = 35 #< PMIC type
        LOG_TARGET_TYPE_PAUC                = 36 #< PAUC type
        LOG_TARGET_TYPE_GENERICI2CRESPONDER = 37 #< GENERICI2CRESPONDER type
        LOG_TARGET_TYPE_IOLINK              = 38 #< IOLINK type
        LOG_TARGET_TYPE_MDS_CTLR            = 39 #< MDS Media Controller type
        LOG_TARGET_TYPE_COMPUTE_CHIP        = 40 #< Compute chip type
        LOG_TARGET_TYPE_HUB_CHIP            = 41 #< Hub chip type
        LOG_TARGET_TYPE_TBUSC               = 42 #< TBUS chiplet type
        LOG_TARGET_TYPE_TBUSL               = 43 #< TBUS link type
        LOG_TARGET_TYPE_L3CACHE             = 44 #< L3CACHE type
        LOG_TARGET_TYPE_INT                 = 45 #< INT type
        LOG_TARGET_TYPE_NX                  = 46 #< NX type
        LOG_TARGET_TYPE_PAX                 = 47 #< PAX type
        LOG_TARGET_TYPE_PAXO                = 48 #< PAXO type
        LOG_TARGET_TYPE_SMPLINK             = 59 #< SMPLINK type
        LOG_TARGET_TYPE_PEC6P               = 50 #< PEC6P type
        LOG_TARGET_TYPE_PEC2P               = 51 #< PEC2P type
        LOG_TARGET_TYPE_PHB248X             = 52 #< PHB248X type
        LOG_TARGET_TYPE_PHB16X              = 53 #< PHB16X type
        LOG_TARGET_TYPE_POWER_IC            = 54 #< POWER_IC type
        LOG_TARGET_TYPE_TEMP_SENSOR         = 55 #< TEMP_SENSOR type
        LOG_TARGET_TYPE_MULTICAST           = 63 #< MULTICAST type

    class ChipType(Enum):
        CHIP_TYPE_ANY        = 0
        CHIP_TYPE_ODYSSEY_00 = 1
        CHIP_TYPE_ODYSSEY_01 = 2
        CHIP_TYPE_ODYSSEY_02 = 3
        CHIP_TYPE_ODYSSEY_03 = 4
        CHIP_TYPE_ODYSSEY_04 = 5
        CHIP_TYPE_ODYSSEY_05 = 6
        CHIP_TYPE_ODYSSEY_06 = 7
        CHIP_TYPE_ODYSSEY_07 = 8
        CHIP_TYPE_ODYSSEY_08 = 9
        CHIP_TYPE_ODYSSEY_09 = 10
        CHIP_TYPE_ODYSSEY_10 = 11
        CHIP_TYPE_ODYSSEY_11 = 12
        CHIP_TYPE_ODYSSEY_12 = 13
        CHIP_TYPE_ODYSSEY_13 = 14
        CHIP_TYPE_ODYSSEY_14 = 15
        CHIP_TYPE_ODYSSEY_15 = 16

class AttributeFile(ABC):

    class Target(NamedTuple):
        targ_type : Fapi2.TargetType
        targ_inst : int  # should be same as fapi2::ATTR_CHIP_UNIT_NUM


    class AttributeInfo(NamedTuple):
        id    : str
        index : 'list[int]'
        size  : int

    class AttributeOverride(NamedTuple):
        attr_inf : 'AttributeFile.AttributeInfo'
        value    : int


    class AttributeRc(Enum):
        AttrOverrideRc_SUCCESS                  = 0
        AttrOverrideRc_ID_NOT_FOUND             = 1
        AttrOverrideRc_SIZE_NOT_MATCHING        = 2
        AttrOverrideRc_TARGET_INST_OUT_RANGE    = 3
        AttrOverrideRc_ARRAY_INDEX_OUT_RANGE    = 4
        AttrOverrideRc_OVRD_NOT_ALLOWED         = 5


    class AttributeOvrdResponse(NamedTuple):
        attr_inf : 'AttributeFile.AttributeInfo'
        rc       : 'AttributeFile.AttributeRc'


    class AttributeReadResponse(NamedTuple):
        attr_inf : 'AttributeFile.AttributeInfo'
        value    : int
        rc       : 'AttributeFile.AttributeRc'


class AttributeFileGenerator(AttributeFile):

    def __init__(self):
        self.byte_buffer = bytearray()

    def writeHeader(self,
                    i_chip_type:Fapi2.ChipType,
                    file_type:Fapi2.FileType,
                    number_target_sections:int):
        self.byte_buffer.extend(Const.FORMAT_MAJOR_VERSION.to_bytes(1, "big"))
        self.byte_buffer.extend(Const.FORMAT_MINOR_VERSION.to_bytes(1, "big"))
        self.byte_buffer.extend(i_chip_type.value.to_bytes(1, "big"))
        self.byte_buffer.extend(self.iv_file_type.value.to_bytes(1, "big"))
        self.byte_buffer.extend(number_target_sections.to_bytes(4, "big"))

    def writeTarget(self,
                    fapi_target_type:Fapi2.TargetType,
                    inst_num:int,
                    num_attributes:int):
        l =len(self.byte_buffer)
        self.byte_buffer.extend(fapi_target_type.value.to_bytes(1, "big"))
        self.byte_buffer.extend(inst_num.to_bytes(1, "big"))
        self.byte_buffer.extend(num_attributes.to_bytes(2, "big"))
        # Reserved 4 bytes
        self.byte_buffer.extend(int(0).to_bytes(4, "big"))

    def generateAttributeFile(
        self,
        i_chip_type : Fapi2.ChipType) -> bytearray :
        '''
        Will generate a pak file containing attr.ovrd or attr.list for the chip_type passed.
        '''
        pass


class AttributeUpdateFileGenerator(AttributeFileGenerator):
    '''
    class which encapsulate all functions for generating an attribute override
    file, from a dict. Example of dict is
        {
            Target(LOG_TARGET_TYPE_SYSTEM, 0):
                [
                    AttributeOverride(AttributeInfo(ATTR_IS_SIMULATION, [], 1), 5),
                    AttributeOverride(AttributeInfo(ATTR_HOTPLUG, [], 1), 2)
                ],
            Target(LOG_TARGET_TYPE_OCMB, 0):
                [
                    AttributeOverride(AttributeInfo(ATTR_ENABLE_LBIST, [], 1), 1),
                    AttributeOverride(AttributeInfo(ATTR_BUS_POS, [], 1), 5)
                ],
        }
    '''
    def __init__(
        self,
        i_chip_type : Fapi2.ChipType,
        i_attr_list: typing.Dict[AttributeFile.Target, 'list[AttributeFile.AttributeOverride]']
        ):
        self.iv_file_type = Fapi2.FileType.OVERRIDE
        self.iv_chip_type= i_chip_type
        self.iv_attr_list = {}
        super().__init__()
        for target, attrovrdlist in i_attr_list.items():
            newAttrOvrd = []
            for attrovrd in attrovrdlist:
                attr_hash28bits = Utils._getAttrHash(attrovrd.attr_inf.id)
                attrovrd = AttributeFile.AttributeOverride(
                    AttributeFile.AttributeInfo(attr_hash28bits,
                    attrovrd.attr_inf.index, attrovrd.attr_inf.size), attrovrd.value)
                newAttrOvrd.append(attrovrd)
            self.iv_attr_list[target] = newAttrOvrd

    def writeAttribute(self,
                        attr_id:int,
                        size:int,
                        index:list,
                        data:'int|list[int]'):
        pad = 0
        l = len(self.byte_buffer)
        self.byte_buffer.extend(attr_id.to_bytes(4,"big"))
        self.byte_buffer.extend(size.to_bytes(2, "big"))
        for elem in index:
            self.byte_buffer.extend(elem.to_bytes(1,"big"))
        self.byte_buffer.extend(pad.to_bytes(4-len(index), "big"))
        if(isinstance(data,list)):
            for elem in data:
                self.byte_buffer.extend(elem.to_bytes(1, "big"))
        else:
            self.byte_buffer.extend(data.to_bytes(size, "big"))
        alignment = 8-(len(self.byte_buffer)%8)
        if(alignment!=8):
            self.byte_buffer.extend(pad.to_bytes(alignment, "big"))

    def generateAttributeFile(
        self,
        i_chip_type : Fapi2.ChipType) -> bytearray :
        '''
        Will generate a pak file containing attr.ovrd or attr.list for the chip_type passed.
        '''
        num_attributes   = len(self.iv_attr_list.values())
        chip_type        = self.iv_chip_type
        num_tgt_sections = len(self.iv_attr_list.items())
        self.writeHeader(chip_type, self.iv_file_type, num_tgt_sections)

        for target, attrovrdlist in self.iv_attr_list.items():
            self.writeTarget(target.targ_type,
                            target.targ_inst, len(attrovrdlist))
            for attrovrd in attrovrdlist:
                attr_inf = attrovrd.attr_inf
                self.writeAttribute(attr_inf.id,
                                    attr_inf.size, attr_inf.index,
                attrovrd.value)
        return self.byte_buffer


class AttributeListFileGenerator(AttributeFileGenerator):
    '''
    class which encapsulate all functions for generating an attribute list
    file, from a dict. Example of dict is
        {
            Target(LOG_TARGET_TYPE_SYSTEM, 0):
                [
                    AttributeInfo(ATTR_IS_SIMULATION, [], 0),
                    AttributeInfo(ATTR_HOTPLUG, [], 0)
                ],
            Target(LOG_TARGET_TYPE_OCMB, 0):
                [
                    AttributeInfo(ATTR_ENABLE_LBIST, [], 0),
                    AttributeInfo(ATTR_BUS_POS, [], 0)
                ],
        }
    '''
    def __init__(
        self,
        i_chip_type : Fapi2.ChipType,
        i_attr_list: typing.Dict[AttributeFile.Target, 'list[AttributeFile.AttributeInfo]']
        ) -> None:
        pass


class AttributeFileParser(AttributeFile):
    def __init__(self, i_attr_file, i_attr_db)->None:
        self.iv_attr_file = i_attr_file
        self.iv_offset = 0
        self.iv_attr_db = i_attr_db
        self.iv_AttrId_NameMap = {}
        dbfile = pickle.loads(self.utilOpen(self.iv_attr_db))
        for attr in dbfile.field_list:
            attr_hash28bits = Utils._getAttrHash(attr.name)
            self.iv_AttrId_NameMap[attr_hash28bits] = attr.name

    def utilOpen(self, filename:str)->bytes:
        try:
            return open(filename, "rb").read()
        except FileNotFoundError:
            raise Exception("ERR -- '{}' FILE NOT FOUND ".format(filename))

    def getAttrName(self, id)->str:
        return self.iv_AttrId_NameMap.get(id, 'Not Found Attr id '+ str(hex(id)))

    def _getAttrId(self):
        return int.from_bytes(self.iv_attr_file[self.iv_offset:self.iv_offset+4],
                "big", signed=False)&0xFFFFFFFF

    def readHeader(self):
        offset = self.iv_offset
        format_major_version = self.iv_attr_file[0]
        format_minor_version = self.iv_attr_file[1]
        if(format_major_version!=Const.FORMAT_MAJOR_VERSION):
            raise Exception("Format major version mismatch")
        elif(format_minor_version!=Const.FORMAT_MINOR_VERSION):
            raise Exception("Format minor version mismatch")
        chip_type = Fapi2.ChipType(self.iv_attr_file[2]).name
        if(self.iv_file_type != Fapi2.FileType(self.iv_attr_file[3])):
            raise Exception("File Type Mismatch Expected - \
            {}\n Got - {}".format(self.iv_file_type,
            Fapi2.FileType(self.iv_attr_file[3])))
        number_target_sections = int.from_bytes(self.iv_attr_file[4:8],
                                                "big",signed=False)
        self.iv_offset += 8
        return number_target_sections

    def readTarget(self)-> 'list[int, Fapi2.Target]':
        offset = self.iv_offset
        fapi_target_type = Fapi2.TargetType(int.from_bytes(
            self.iv_attr_file[offset:offset+1], "big", signed=False)).name
        inst_num = int.from_bytes(self.iv_attr_file[offset+1:offset+2],
                                "big", signed=False)
        num_attribute_rows = int.from_bytes(self.iv_attr_file[offset+2:offset+4],
                                            "big", signed=False)
        self.iv_offset+=8
        target = AttributeFile.Target(fapi_target_type, inst_num)
        return num_attribute_rows, target

    def getResponse(self):
        response={}
        num_target_sections = self.readHeader()
        for i in range(num_target_sections):
            num_attribute_rows, target = self.readTarget()
            response[target] = list()
            if(num_attribute_rows == 0):
                print("Invalid Target section, num attributes = 0")
                raise Exception()
            for k in range(num_attribute_rows):
                response[target].append(self.readAttribute())
        return response


class AttributeUpdateRespFileParser(AttributeFileParser):
    '''
    Parse the response recieved from attrUpdate chipop
    '''
    def __init__(
        self,
        i_attr_file : bytearray, # a pak file containing one or more attribute ovrd resp file
        i_attr_db) -> None:
        super().__init__(i_attr_file, i_attr_db)
        self.iv_file_type = Fapi2.FileType.OVERRIDE

    def _getStatus(self):
        return (self.iv_attr_file[self.iv_offset])&0xF0

    def _getAttrRc(self):
        return int.from_bytes(self.iv_attr_file[self.iv_offset+4:self.iv_offset+8],
                "big", signed=False)

    def readAttribute(self):
        status = self._getStatus()
        attr_id = self._getAttrId()
        attr_name = self.getAttrName(attr_id)
        index = []
        size = 4
        rc = self._getAttrRc()
        attr_inf = AttributeFile.AttributeInfo(id = attr_name,
                                               index = index, size=size)
        attr_ovrd = AttributeFile.AttributeOvrdResponse(attr_inf = attr_inf,
        rc = AttributeFile.AttributeRc(rc))
        offset = self.iv_offset+8
        self.iv_offset = offset
        if(offset%8!=0):
            self.iv_offset = offset+ 8-(offset%8)
        return attr_ovrd


class AttributeListRespFileParser(AttributeFileParser):
    def __init__(self, i_attr_file, i_attr_db) -> None:
        super().__init__(i_attr_file, i_attr_db)
        self.iv_file_type = Fapi2.FileType.LIST

    def _getSize(self):
        return int.from_bytes(self.iv_attr_file[self.iv_offset+4:self.iv_offset+6],
                "big", signed=False)

    def _getIndex(self):
        return list(self.iv_attr_file[self.iv_offset+6:self.iv_offset+9])

    def _getData(self, size):
        return list(self.iv_attr_file[self.iv_offset+10:self.iv_offset+10+size])

    def readAttribute(self):
        offset = self.iv_offset
        attr_id = self._getAttrId()
        attr_name = self.getAttrName(attr_id)
        size = self._getSize()
        index = self._getIndex()
        data = self._getData(size)
        attr_inf = AttributeFile.AttributeInfo(id = attr_name,
                                               index = index, size=size)
        attr_ovrd = AttributeFile.AttributeReadResponse(attr_inf = attr_inf,
        value = data, rc = AttributeFile.AttributeRc["AttrOverrideRc_SUCCESS"])
        offset = self.iv_offset+10+size
        self.iv_offset = offset
        if(offset%8!=0):
            self.iv_offset = offset+ 8-(offset%8)
        return attr_ovrd
