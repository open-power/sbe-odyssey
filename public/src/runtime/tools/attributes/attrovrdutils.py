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
        TARGET_TYPE_SYSTEM              =  0 #< System type
        TARGET_TYPE_DIMM                =  1 #< DIMM type
        TARGET_TYPE_PROC_CHIP           =  2 #< Processor type
        TARGET_TYPE_MEMBUF_CHIP         =  3 #< Membuf type
        TARGET_TYPE_EX                  =  4 #< EX - 2x Core, L2, L3 - can be deconfigured
        TARGET_TYPE_MBA                 =  5 #< MBA type
        TARGET_TYPE_MCS                 =  6 #< MCS type
        TARGET_TYPE_XBUS                =  7 #< XBUS type
        TARGET_TYPE_ABUS                =  8 #< ABUS type
        TARGET_TYPE_L4                  =  9 #< L4 type
        TARGET_TYPE_CORE                = 10 #< Core
        TARGET_TYPE_EQ                  = 11 #< EQ - 4x core, 2x L2, 2x L3 - can be deconfigured
        TARGET_TYPE_MCA                 = 12 #< MCA type
        TARGET_TYPE_MCBIST              = 13 #< MCBIST type
        TARGET_TYPE_MI                  = 14 #< MI Memory Interface (Cumulus)
        TARGET_TYPE_CAPP                = 15 #< CAPP target
        TARGET_TYPE_DMI                 = 16 #< DMI type
        TARGET_TYPE_OBUS                = 17 #< OBUS type
        TARGET_TYPE_OBUS_BRICK          = 18 #< OBUS BRICK type
        TARGET_TYPE_SBE                 = 19 #< SBE type
        TARGET_TYPE_PPE                 = 20 #< PPE type
        TARGET_TYPE_PERV                = 21 #< Pervasive type
        TARGET_TYPE_PEC                 = 22 #< PEC type
        TARGET_TYPE_PHB                 = 23 #< PHB type
        TARGET_TYPE_MC                  = 24 #< MC type
        TARGET_TYPE_OMI                 = 25 #< OMI type
        TARGET_TYPE_OMIC                = 26 #< OMIC type
        TARGET_TYPE_MCC                 = 27 #< MCC type
        TARGET_TYPE_OCMB_CHIP           = 28 #< OCMB type
        TARGET_TYPE_MEM_PORT            = 29 #< MEM_PORT type
        TARGET_TYPE_NMMU                = 30 #< NEST MMU type
        TARGET_TYPE_RESERVED            = 31 #< Reserved for Cronus (Z)
        TARGET_TYPE_PAU                 = 32 #< PAU type
        TARGET_TYPE_IOHS                = 33 #< IOHS type
        TARGET_TYPE_FC                  = 34 #< Fused Core type
        TARGET_TYPE_PMIC                = 35 #< PMIC type
        TARGET_TYPE_PAUC                = 36 #< PAUC type
        TARGET_TYPE_GENERICI2CRESPONDER = 37 #< GENERICI2CRESPONDER type
        TARGET_TYPE_IOLINK              = 38 #< IOLINK type
        TARGET_TYPE_MDS_CTLR            = 39 #< MDS Media Controller type
        TARGET_TYPE_COMPUTE_CHIP        = 40 #< Compute chip type
        TARGET_TYPE_HUB_CHIP            = 41 #< Hub chip type
        TARGET_TYPE_TBUSC               = 42 #< TBUS chiplet type
        TARGET_TYPE_TBUSL               = 43 #< TBUS link type
        TARGET_TYPE_L3CACHE             = 44 #< L3CACHE type
        TARGET_TYPE_INT                 = 45 #< INT type
        TARGET_TYPE_NX                  = 46 #< NX type
        TARGET_TYPE_PAX                 = 47 #< PAX type
        TARGET_TYPE_PAXO                = 48 #< PAXO type
        TARGET_TYPE_SMPLINK             = 59 #< SMPLINK type
        TARGET_TYPE_PEC6P               = 50 #< PEC6P type
        TARGET_TYPE_PEC2P               = 51 #< PEC2P type
        TARGET_TYPE_PHB248X             = 52 #< PHB248X type
        TARGET_TYPE_PHB16X              = 53 #< PHB16X type
        TARGET_TYPE_POWER_IC            = 54 #< POWER_IC type
        TARGET_TYPE_TEMP_SENSOR         = 55 #< TEMP_SENSOR type
        TARGET_TYPE_MULTICAST           = 63 #< MULTICAST type

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
            Target(TARGET_TYPE_SYSTEM, 0):
                [
                    AttributeOverride(AttributeInfo(ATTR_IS_SIMULATION, []), 5),
                    AttributeOverride(AttributeInfo(ATTR_HOTPLUG, []), 2)
                ],
            Target(TARGET_TYPE_OCMB, 0):
                [
                    AttributeOverride(AttributeInfo(ATTR_ENABLE_LBIST, []), 1),
                    AttributeOverride(AttributeInfo(ATTR_BUS_POS, []), 5)
                ],
        }
    '''
    def __init__(
        self,
        i_chip_type : Fapi2.ChipType,
        i_attr_list: typing.Dict[AttributeFile.Target, 'list[AttributeFile.AttributeOverride]']
        ) -> None:

        pass


class AttributeListFileGenerator(AttributeFileGenerator):
    '''
    class which encapsulate all functions for generating an attribute list
    file, from a dict. Example of dict is
        {
            Target(TARGET_TYPE_SYSTEM, 0):
                [
                    AttributeInfo(ATTR_IS_SIMULATION, [], 0),
                    AttributeInfo(ATTR_HOTPLUG, [], 0)
                ],
            Target(TARGET_TYPE_OCMB, 0):
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
        pass


class AttributeUpdateRespFileParser(AttributeFileParser):
    '''
    Parse the response recieved from attrUpdate chipop
    '''
    def __init__(
        self,
        i_attr_file : bytearray, # a pak file containing one or more attribute ovrd resp file
        i_attr_db) -> None:
        pass

    def getResponse(self) -> 'list[AttributeFile.AttributeOvrdResponse]':
        pass


class AttributeListRespParser(AttributeFileParser):
    '''
    Parse the response recieved from attrRead chipop
    '''
    def __init__(
        self,
        i_attr_file : bytearray, # a pak file containing one or more attribute read resp file
        i_attr_db) -> None:

        pass

    def getResponse(self) -> 'list[AttributeFile.AttributeReadResponse]':
        pass