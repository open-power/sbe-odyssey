#!/usr/bin/env python3
# IBM_PROLOG_BEGIN_TAG
# This is an automatically generated prolog.
#
# $Source: public/src/runtime/tools/attributes/attrdb.py $
#
# OpenPOWER sbe Project
#
# Contributors Listed Below - COPYRIGHT 2021,2022
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
from xml.dom.minidom import Element
import xml.etree.ElementTree as etree
from textwrap import dedent
from collections import namedtuple
from attrtoolutils import *

TargetTypeInfo = namedtuple("TargetTypeInfo", "ntargets")

# TODO: Move this data into a file in image specific folder,
#   and load into this tool, by passing file path as an argument.
TARGET_TYPES = {
    "TARGET_TYPE_OCMB_CHIP": TargetTypeInfo(1),
    "TARGET_TYPE_SYSTEM": TargetTypeInfo(1),
    "TARGET_TYPE_PERV": TargetTypeInfo(9),
    "TARGET_TYPE_MC": TargetTypeInfo(1),
}

class ParseError(Exception):
    pass

class RealAttribute(object):
    """
    Class to parse attribute from XML which need a real storage location
    """

    def __init__(self, db:"AttributeDB", node:Element):
        self.db = db
        self.sbe_entry:"SBEEntry" = None
        self.unsupported_attribute = False

        name, target_type, value_type, description, enum_values, array_dims = (
            node.find(tag) for tag in ("id", "targetType", "valueType", "description", "enum", "array"))

        if name is None:
            raise ParseError("Unnamed attribute")
        self.name = name.text

        if target_type is None or value_type is None:
            raise ParseError("Attribute %s is missing a target type or value type" % self.name)
        self.target_type = target_type.text
        self.value_type = value_type.text

        target_type_str_list = [targ.strip().upper() for targ in self.target_type.split(',')]
        self.sbe_target_type:list[TargetTypeInfo] = []
        # workaround to support POZ_ANY_CHIP attributes
        self.ekb_target_type:list[str] = []

        for targ in target_type_str_list:
            targ_type = TARGET_TYPES.get(targ, None)
            if targ_type != None:
                self.sbe_target_type.append(targ)
            self.ekb_target_type.append(targ)

        if len(self.sbe_target_type) == 0:
            self.unsupported_attribute = True
            return

        self.description = dedent(description.text.strip("\n")) if description else None

        self.enum_values = enum_values

        if array_dims is None:
            self.array_dims = []
        else:
            # TODO: Be strict about commas
            self.array_dims = [int(dim, 0) for dim in array_dims.text.replace(",", " ").split()]

        if node.find("writeable") != None:
            self.writeable = True
        else:
            self.writeable = False

        if node.find("platInit") != None:
            self.platinit = True
        else:
            self.platinit = False

        # TODO: Support tags <default> and <initToZero/>

    def validate_and_return_sbe_attribute(self, sbe_value:"SBEEntry") -> "SBEEntry":
        if sbe_value != None:
            vprint("Validating attribute: " + self.name)

            if self.unsupported_attribute == True:
                raise ParseError(self.name + " have no supported targets")

            if sbe_value.virtual == False:
                if len(self.sbe_target_type) > 1:
                    raise ParseError(self.name + " have multiple supported targets")
            else:
                if(self.writeable == True):
                    raise ParseError(self.name + " is writeable but virtual")

            if(len(self.array_dims) > 1):
                # this is not supported now
                raise ParseError(self.name + " have multi dimensional array")

            sbe_value.validated = True

        return sbe_value

class ECAttribute(object):
    """
    Class to parse EC feature realted attribute from xml. (These attribute doesnt
        need a storage location.)
    """
    def __init__(self, db:"AttributeDB", node:Element):
        # TODO: Derive RealAttribute and ECAttribute from a common class to remove code duplication

        self.db = db
        self.sbe_entry:"SBEEntry" = None

        name, target_type, description, enum_values, array_dims = (
            node.find(tag) for tag in ("id", "targetType", "description", "enum", "array"))

        if name is None:
            raise ParseError("Unnamed attribute")
        self.name = name.text

        if target_type is None:
            raise ParseError("Attribute %s is missing a target type" % self.name)
        self.target_type = target_type.text

        target_type_str_list = [targ.strip().upper() for targ in self.target_type.split(',')]
        self.sbe_target_type:list[TargetTypeInfo] = []
        for targ in target_type_str_list:
            targ_type = TARGET_TYPES.get(targ, None)
            if targ_type != None:
                self.sbe_target_type.append(targ)

        if len(self.sbe_target_type) == 0:
            self.unsupported_attribute = True
            return

        self.value_type = "uint8"
        self.array_dims = []
        self.enum_values = None

        self.chip_name = node.find("chipEcFeature").find("chip").find("name").text
        self.ec_value = node.find("chipEcFeature").find("chip").find("ec").find("value").text
        self.ec_test = node.find("chipEcFeature").find("chip").find("ec").find("test").text

    def validate_and_return_sbe_attribute(self, sbe_value:"SBEEntry") -> "SBEEntry":
        #TODO: Enable this while supporting EC_attribute
        if sbe_value != None:
            vprint("Validating attribute: " + self.name)
            '''
            if sbe_value.virtual == False:
                raise ParseError(self.name + " is EC attribute but not virtual")

            if self.target_type != 'TARGET_TYPE_PROC_CHIP':
                raise ParseError(self.name + " is EC attribute but target is not"
                        " valid: " + self.target_type)

            if self.ec_test != "EQUAL":
                raise ParseError(self.name + " is EC attribute but target is not valid")
            '''

            sbe_value.validated = True

        return sbe_value

class AttributeDB(object):
    """
    An object of this class will represent collection of all attributes in
        attribute xmls from EKB.
    """
    def __init__(self):
        self.attributes : dict[str, RealAttribute|ECAttribute] = dict()

    def load_xml(self, fname):
        root = etree.parse(fname).getroot()
        for elem in root.iter("attribute"):
            attr = ECAttribute(self, elem) if elem.find("chipEcFeature") else RealAttribute(self, elem)
            if attr.name in self.attributes:
                raise ParseError("Duplicate attribute " + attr.name)
            self.attributes[attr.name] = attr


class SBEEntry(object):
    """
    Parse an entry inside sbe attribute xml (which will define SBE related
        properties)
    """
    def __init__(self) -> None:
        self.is_empty = True
        self.values = []
        self.virtual = False
        self.overridable = False
        self.validated = False

    def set_values(self, values:list):
        self.is_empty = False
        self.values = values

    def set_overridable(self):
        self.is_empty = False
        self.overridable = True

    def set_virtual(self, img_type:str="GENERIC"):
        self.is_empty = False
        self.virtual = True

    def update_sbe_entry(self, in_entry:"SBEEntry") -> None:
        if (in_entry == None) or in_entry.is_empty:
            return
        if len(in_entry.values) != 0:
            self.values = in_entry.values
        if in_entry.overridable:
            self.overridable = True
        if in_entry.virtual:
            self.virtual = True

class SBEAttributes(object):
    """
    This class represent collection of all SBE entries.
    """
    def __init__(self, fname):
        root = etree.parse(fname).getroot()
        self.attributes = dict()
        for elem in root.iter("entry"):
            name = elem.find("name")
            values = elem.findall("value")
            virtual = elem.find("virtual")
            overridable = elem.find("Overridable")

            if name is None:
                raise ParseError("Unnamed SBE attribute")

            if name.text in self.attributes:
                raise ParseError("Duplicate entry for attribute " + name.text)

            entry = SBEEntry()

            if values:
                entry.set_values([int(value.text, 0) for value in values])

            if virtual != None:
                entry.set_virtual()

            if overridable != None:
                entry.set_overridable()

            if not entry.is_empty:
                self.attributes[name.text] = entry

    def update_attrdb(self, db:AttributeDB) -> None:
        for attr in db.attributes.values():
            entry_value = attr.validate_and_return_sbe_attribute(
                                self.attributes.get(attr.name, None))
            if (not attr.sbe_entry) or (attr.sbe_entry == None):
                attr.sbe_entry = entry_value
            else:
                attr.sbe_entry.update_sbe_entry(entry_value)

        for attr in self.attributes.keys():
            if self.attributes[attr].validated == False:
                raise ParseError("Invalid SBE attribute " + attr)
