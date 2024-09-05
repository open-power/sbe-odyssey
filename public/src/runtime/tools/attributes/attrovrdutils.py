#!/usr/bin/env python3
# IBM_PROLOG_BEGIN_TAG
# This is an automatically generated prolog.
#
# $Source: public/src/runtime/tools/attributes/attrovrdutils.py $
#
# OpenPOWER sbe Project
#
# Contributors Listed Below - COPYRIGHT 2022,2024
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
import json

class Utils:
    def _getAttrHash(id):
        attr_hash16bytes = hashlib.md5(id.encode()).digest()
        attr_hash32bits = int.from_bytes(attr_hash16bytes[0:4], "big")
        attr_hash28bits = attr_hash32bits >> 4
        return attr_hash28bits


class Const:
    FORMAT_MAJOR_VERSION  = 0
    FORMAT_MINOR_VERSION  = 2
    ATTR_FILE_TARGET_MAGIC_WORD = 0x54415247


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
        value    : 'int|list[int]'


    class AttributeRc(Enum):
        AttrOverrideRc_SUCCESS                  = 0
        AttrOverrideRc_TGT_TYPE_NOT_FOUND       = 1
        AttrOverrideRc_ATTR_NOT_FOUND           = 2
        AttrOverrideRc_SIZE_NOT_MATCHING        = 3
        AttrOverrideRc_TARGET_INST_OUT_RANGE    = 4
        AttrOverrideRc_ARRAY_INDEX_OUT_RANGE    = 5
        AttrOverrideRc_ATTR_NOT_ARRAY           = 6
        AttrOverrideRc_OVRD_NOT_ALLOWED         = 7

        #Note : These enum values must match with the enum values defined in
        #       hwpf/sbe_utils/include/sbe_attribute_utils.H in the ekb repository.
        #       Hostboot/Cronus refer the enums defined in sbe_attribute_utils.H
        #       available in the ekb.
        AttrOverrideRc_DENIED_UPDATE            = 11


    class AttributeOvrdResponse(NamedTuple):
        attr_name: str
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
        # Target magic word
        self.byte_buffer.extend(Const.ATTR_FILE_TARGET_MAGIC_WORD.to_bytes(4, "big"))

    def generateAttributeFile(
        self,
        i_chip_type : Fapi2.ChipType) -> bytearray :
        '''
        Will generate a pak file containing attr.ovrd or attr.list for the chip_type passed.
        '''
        pass

def validateAttrOvrd(attrOvrd:dict,i_attr_db)->bool:
    import attrdatatype

    if (i_attr_db is None):
        return True

    attr_db_dict = {attr.name: attr for attr in i_attr_db.field_list if attr.has_storage}

    for target,attributes in attrOvrd.items():
        log_target_name = Fapi2.TargetType(target.targ_type).name
        target_name     = log_target_name[4:]
        target_inst     = target.targ_inst

        if (target_name not in i_attr_db.target_types):
            print(f"The target type [{target_name}] is not supported by this instance of self boot engine.")
            return False

        if (target_inst >= i_attr_db.target_types[target_name].ntargets):
            print(f"The target instance [{target_inst}] is not valid for the target type [{target_name}].")
            return False

        for attribute in attributes:
            attr_name = attribute.attr_inf.id
            if (attr_name not in attr_db_dict.keys()):
                print(f"The attribute name [{attr_name}] is not supported by this instance of self boot engine.")
                return False

            attr_list = [attrs.name for attrs in i_attr_db.field_list
                                    if attrs.has_storage and attrs.name == attr_name and attrs.sbe_targ_type == target_name]
            if (len(attr_list) == 0):
                print(f"The attribute name [{attr_name}] is not supported for the target type [{target_name}] by this "
                        f"instance of self boot engine.")
                return False

            # Remove _t for lookup in the attrdatatype.VALUE_TYPES
            value_type = attr_db_dict[attr_name].value_type[:-2].lower()
            value_size = attrdatatype.VALUE_TYPES[value_type].size
            if (attribute.attr_inf.size != value_size):
                print(f"The attribute size [{attribute.attr_inf.size}] as specified in the attribute override file does not "
                      f"match with the size [{value_size}] specified in the attribute definition xml.")
                return False

            array_dims = attr_db_dict[attr_name].array_dims
            if (len(array_dims) == 0 and attribute.attr_inf.index != [255,255,255]):
                print(f"The attribute [{attr_name}] is not an array attribute as per the attribute definition xml. So, the "
                      f"index must be [255,255,255]")
                return False

            if (len(array_dims) != 0 and len(array_dims) != len(attribute.attr_inf.index)):
                print(
                  f"The attribute [{attr_name}] is {array_dims} dimension(s) as per the attribute definition xml. But, you "
                  f"have specified [{len(attribute.attr_inf.index)}] dimension(s) having value {attribute.attr_inf.index}")
                return False

            if (len(array_dims) > 0):
                for i in range(0,len(array_dims)):
                    if (attribute.attr_inf.index[i] >= array_dims[i]):
                        print(f"The attribute [{attr_name}] index {attribute.attr_inf.index} exceeds the array "
                              f"dimension {array_dims} specified in the attribute definition xml.")
                        return False
    return True

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
        or json file
        [
            {
                "target":"LOG_TARGET_TYPE_SYSTEM",
                "instance":"0",
                "attributes":[
                    {
                        "id":"ATTR_IS_SIMULATION",
                        "index":[255, 255, 255],
                        "size":1,
                        "value":["0x01"]
                    },
                    {
                        "id":"ATTR_HOTPLUG",
                        "size":1,
                        "value":["0x03"]
                    }
                ]
            }
        ]
    '''
    @classmethod
    def attributeJsonLoader(cls, io_attrDict:dict, **kwarg : dict):
        if('target' in kwarg):
            target = AttributeFile.Target(Fapi2.TargetType[kwarg['target']],
                                    int(kwarg['instance'],10))
            io_attrDict[target]=[]
            io_attrDict[target].extend( kwarg['attributes'] )
        elif('id' in kwarg):
            if('index' in kwarg):
                index = kwarg['index']
            else:
                index = [0xff, 0xff, 0xff]
            index = [int(i, 16) if isinstance(i, str) else i for i in index]
            size = kwarg['size']
            if(isinstance(size,str)):
                size = int(size, 16)
            attrInfo = AttributeFile.AttributeInfo(
                                                kwarg['id'],
                                                index, size)
            value = [int(i, 16) if isinstance(i, str) \
                    else i for i in kwarg['value']]
            attrOvrd = AttributeFile.AttributeOverride(attrInfo, value)
            return attrOvrd
        else:
            raise Exception("Key mismatch, 'target', or 'id' not found")

    @classmethod
    def fromAttrJson(cls, i_jsonfile):
        try:
            attrDict={}
            target=None
            with open(i_jsonfile,'r') as fp:
                data = json.load(fp,
                                object_hook= lambda d:
                        AttributeUpdateFileGenerator.attributeJsonLoader(attrDict,
                                                        **d))
            return attrDict
        except KeyError:
            raise Exception("Expected keys not found in json file")
        except FileNotFoundError:
            raise Exception("Expected json file {} not found".format(i_jsonfile))

    def __init__(
        self,
        i_chip_type : Fapi2.ChipType,
        i_attr_list,
        i_attr_db = None ):

        self.iv_file_type = Fapi2.FileType.OVERRIDE
        self.iv_chip_type= i_chip_type
        self.iv_attr_list = {}
        if(isinstance(i_attr_list, str)):
            if(os.path.isfile(i_attr_list)):
                self.iv_attr_list = AttributeUpdateFileGenerator.fromAttrJson(i_attr_list)
            else:
                raise Exception("{} is not a valid file path".format(i_attr_list))

            if (i_attr_db is not None):
                validOvrd = validateAttrOvrd(self.iv_attr_list,i_attr_db)
                if (validOvrd == False):
                    raise Exception(f"Validation of attribute override file [{i_attr_list}] failed. Please also check "
                            "whether you have passed the correct attribute database file.")
        else:
            for target, attrovrdlist in i_attr_list.items():
                newAttrOvrd = []
                for attrovrd in attrovrdlist:
                    attrovrd = AttributeFile.AttributeOverride(
                        AttributeFile.AttributeInfo(attrovrd.attr_inf.id,
                        attrovrd.attr_inf.index, attrovrd.attr_inf.size), attrovrd.value)
                    newAttrOvrd.append(attrovrd)
                self.iv_attr_list[target] = newAttrOvrd
        super().__init__()

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
                attr_hash28bits = Utils._getAttrHash(attr_inf.id)
                self.writeAttribute(attr_hash28bits,
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
        self.iv_attr_db = pickle.loads(self.utilOpen(i_attr_db))
        self.iv_attr_fields = {field.name: field for field in self.iv_attr_db.field_list}
        self.iv_AttrId_NameMap = {}
        for attr in self.iv_attr_db.field_list:
            attr_hash28bits = Utils._getAttrHash(attr.name)
            self.iv_AttrId_NameMap[attr_hash28bits] = attr.name

    def utilOpen(self, filename:str)->bytes:
        try:
            return open(filename, "rb").read()
        except FileNotFoundError:
            raise Exception("ERR -- '{}' FILE NOT FOUND ".format(filename))

    def getAttrName(self, id)->str:
        try:
            return self.iv_AttrId_NameMap[id]
        except KeyError:
            raise KeyError("Attribute hash not found: " + hex(id))

    def _get_bytes(self, nbytes):
        new_ofs = self.iv_offset + nbytes
        value = self.iv_attr_file[self.iv_offset : new_ofs]
        self.iv_offset = new_ofs
        return value

    def _get(self, nbytes):
        return int.from_bytes(self._get_bytes(nbytes), "big", signed=False)

    def _align(self, alignment):
        self.iv_offset += -self.iv_offset % alignment

    def _getAttrId(self):
        return self._get(4) & 0x0FFFFFFF

    def _getIndex(self):
        return [self._get(1) for _ in range(4)]

    def readHeader(self):
        format_major_version = self._get(1)
        format_minor_version = self._get(1)
        if format_major_version!=Const.FORMAT_MAJOR_VERSION:
            raise Exception("Format major version mismatch")
        elif format_minor_version!=Const.FORMAT_MINOR_VERSION:
            raise Exception("Format minor version mismatch")

        chip_type = Fapi2.ChipType(self._get(1)).name
        file_type = Fapi2.FileType(self._get(1))
        if self.iv_file_type != file_type:
            raise Exception("File Type Mismatch Expected - \
            {}\n Got - {}".format(self.iv_file_type, file_type))

        number_target_sections = self._get(4)
        return number_target_sections

    def readTarget(self) -> 'list[int, Fapi2.Target]':
        fapi_target_type = Fapi2.TargetType(self._get(1)).name
        inst_num = self._get(1)
        num_attribute_rows = self._get(2)
        magic_word = self._get(4)

        if magic_word != Const.ATTR_FILE_TARGET_MAGIC_WORD:
            raise Exception("Input data is corrupted, target magic word expected")

        return num_attribute_rows, AttributeFile.Target(fapi_target_type, inst_num)

    def getResponse(self):
        response={}
        num_target_sections = self.readHeader()
        for i in range(num_target_sections):
            num_attribute_rows, target = self.readTarget()
            response[target] = list()
            if(num_attribute_rows == 0):
                raise Exception("Invalid Target section, num attributes = 0")
            for k in range(num_attribute_rows):
                attr = self.readAttribute()
                if attr is not None:
                    response[target].append(attr)
        return response


class AttributeOverrideFileParser(AttributeFileParser):
    '''
    Parse an attribute override file
    '''
    def __init__(
        self,
        i_attr_file : bytearray, # a pak file containing one or more attribute ovrd resp file
        i_attr_db) -> None:
        super().__init__(i_attr_file, i_attr_db)
        self.iv_file_type = Fapi2.FileType.OVERRIDE

    def readAttribute(self):
        attr_id = self._getAttrId()
        size = self._get(2)
        index = self._getIndex()
        value = self._get_bytes(size)
        self._align(8)

        try:
            attr_name = self.getAttrName(attr_id)
        except KeyError as e:
            print(e.msg)
            return None

        attr_info = self.iv_attr_fields[attr_name]
        ndims = len(attr_info.array_dims)

        return AttributeFile.AttributeOverride(
            AttributeFile.AttributeInfo(attr_name, index[:ndims], size),
            value)

class AttributeUpdateRespFileParser(AttributeFileParser):
    '''
    Parse the response recieved from attrUpdate chipop
    '''
    def __init__(
        self,
        i_attr_file : bytearray, # a pak file containing one or more attribute ovrd resp file
        i_attr_db) -> None:
        super().__init__(i_attr_file, i_attr_db)
        self.iv_file_type = Fapi2.FileType.RESPONSE

    def readAttribute(self):
        attr_id = self._getAttrId()
        attr_name = self.getAttrName(attr_id)
        rc = self._get(4)
        attr_ovrd = AttributeFile.AttributeOvrdResponse(
            attr_name=attr_name,
            rc = AttributeFile.AttributeRc(rc))
        return attr_ovrd


class AttributeListRespFileParser(AttributeFileParser):
    def __init__(self, i_attr_file, i_attr_db) -> None:
        super().__init__(i_attr_file, i_attr_db)
        self.iv_file_type = Fapi2.FileType.LIST

    def readAttribute(self):
        attr_id = self._getAttrId()
        attr_name = self.getAttrName(attr_id)
        size = self._get(2)
        index = self._getIndex()
        data = self._get_bytes(size)
        attr_inf = AttributeFile.AttributeInfo(
            id = attr_name, index = index, size=size)
        attr_ovrd = AttributeFile.AttributeReadResponse(
            attr_inf = attr_inf, value = data,
            rc = AttributeFile.AttributeRc["AttrOverrideRc_SUCCESS"])

        self._align(8)
        return attr_ovrd
