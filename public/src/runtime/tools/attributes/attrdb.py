#!/usr/bin/env python3
# IBM_PROLOG_BEGIN_TAG
# This is an automatically generated prolog.
#
# $Source: public/src/runtime/tools/attributes/attrdb.py $
#
# OpenPOWER sbe Project
#
# Contributors Listed Below - COPYRIGHT 2021,2023
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
import json
import copy

TargetTypeInfo = namedtuple("TargetTypeInfo", "ntargets")

def targetTypeInfoLoader(**kwarg : dict):
    if '__targInfo__' in kwarg:
        return TargetTypeInfo(kwarg['ntargets'])
    return kwarg

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
        self.sbe_target_type:list[str] = []
        # workaround to support POZ_ANY_CHIP attributes
        self.ekb_target_type:list[str] = []

        for targ in target_type_str_list:
            targ_type = AttributeDB.TARGET_TYPES.get(targ, None)
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

            if ((self.writeable == True) and (sbe_value.virtual == True)):
                raise ParseError(self.name + " is writeable but virtual")

            array_size = 1
            for dim in self.array_dims:
                array_size *= dim

            for target_entry in sbe_value.target_entries:
                if ( (target_entry.name == "ALL")  and
                     (len(sbe_value.target_entries) > 1) ):
                    raise ParseError(self.name + ":" + \
                    "More than one target_entry with value 'ALL'")

                #Check if the target_type is supported for the attribute
                if ( (target_entry.name != "ALL") and
                     (target_entry.name not in self.sbe_target_type) ):
                    raise ParseError(self.name + ":" + \
                    "Trying to initialize values for the unsupported target type " + \
                    target_entry.name)

                if ( len(target_entry.values) != array_size ):
                    raise ParseError(("%s : Number of <value> not "
                            "enough to initialize") % (self.name))

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
        self.sbe_target_type:list[str] = []
        for targ in target_type_str_list:
            targ_type = AttributeDB.TARGET_TYPES.get(targ, None)
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
    TARGET_TYPES : 'dict[str, TargetTypeInfo]' = dict()

    def __init__(self, I_target_json_path : str):
        with open(I_target_json_path, 'r') as fp:
            AttributeDB.TARGET_TYPES =json.load(
                    fp, object_hook= lambda d : targetTypeInfoLoader(**d))
        self.attributes : 'dict[str, RealAttribute|ECAttribute]' = dict()

    def load_xml(self, fname):
        root = etree.parse(fname).getroot()
        for elem in root.iter("attribute"):
            attr = ECAttribute(self, elem) if elem.find("chipEcFeature") else RealAttribute(self, elem)
            if attr.name in self.attributes:
                raise ParseError("Duplicate attribute " + attr.name)
            self.attributes[attr.name] = attr

class TargetEntry(object):
    """
    Parse target_type element of sbe attribute xml (which will define SBE related
        properties)
    """
    def __init__(self) -> None:
        self.name = "ALL"
        self.instance = 0xFF
        self.values = []

    def set_values(self, values:list):
        self.values = values

    def set_name(self, name:str):
        self.name = name

    def set_instance(self, instance:int):
        self.instance = instance

class SBEEntry(object):
    """
    Parse an entry inside sbe attribute xml (which will define SBE related
        properties)
    """
    def __init__(self) -> None:
        self.is_empty = True
        self.target_entries:list[TargetEntry] = list()
        self.virtual = False
        self.overridable = False
        self.validated = False

    def set_overridable(self):
        self.is_empty = False
        self.overridable = True

    def set_virtual(self, img_type:str="GENERIC"):
        self.is_empty = False
        self.virtual = True

    def add_target_entry(self, target_entry:TargetEntry):
        self.is_empty = False
        self.target_entries.append(target_entry)

    def update_sbe_entry(self, in_entry:"SBEEntry") -> None:
        if (in_entry == None) or in_entry.is_empty:
            return
        self.target_entries.clear()
        self.target_entries = copy.deepcopy(in_entry.target_entries)
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
        self.attributes : dict[str,SBEEntry]  = dict()
        elem_count = 0
        for elem in root.iter("entry"):
            elem_count += 1
            vprint("Processing XML element <entry> ..." + str(elem_count))
            name = elem.attrib.get("name")
            target_types = elem.findall("target_type")
            virtual = elem.find("virtual")
            overridable = elem.find("Overridable")
            values = elem.findall("value")

            if name is None:
                raise ParseError("Unnamed SBE attribute")

            if name in self.attributes:
                raise ParseError("Duplicate entry for attribute " + name)

            entry = SBEEntry()

            if virtual != None:
                entry.set_virtual()

            if overridable != None:
                entry.set_overridable()

            #<value> can also be direct children of <entry>
            #In that case, that attribute will be initialized
            #with the value for all the targets and their instances
            if values:
                target_entry = TargetEntry()
                target_entry.set_values([int(value.text, 0) for value in values])
                entry.add_target_entry(target_entry)
                if ( len(target_types) > 0 ):
                    raise ParseError(
                      "If <value> element is present as a child of <attribute> "
                      "then <target_entry> element is not supported")

            # Look for the XML element <target_type> with the same instance-id
            # and throw error
            tgt_ins : dict[str,list[int]] = dict()

            for target_type in target_types:
                target_entry = TargetEntry()
                target_name = target_type.attrib.get("name",None)
                instance_str = target_type.attrib.get("instance",None)
                values = target_type.findall("value")

                if target_name is None:
                    raise ParseError("%s : Unnamed XML element <target_type> " % name)

                if target_name not in tgt_ins.keys():
                    tgt_ins[target_name] = list()

                target_entry.set_name(target_name)

                if instance_str is not None:
                    try:
                        instance = int(instance_str,0)
                    except ValueError as ex:
                        vprint(('"%s" cannot be converted to an int : %s') %
                                (instance_str,ex))
                        raise ParseError(("%s : %s : instance [%s] is not valid") %
                                    (name, target_name, instance_str))

                    if (instance != 0xFF):
                        ntargets = AttributeDB.TARGET_TYPES[target_name].ntargets
                        if (instance >= ntargets):
                            raise ParseError(("%s : %s : instance=%d exceeds " +
                                      "max instance [%d]") %
                                      (name, target_name, instance, (ntargets-1)))
                else:
                    instance = 0xFF

                if ( tgt_ins[target_name].count(instance) > 0 ):
                    raise ParseError(("%s : %s : instance [%d] alreay exists") %
                                    (name, target_name, instance))

                tgt_ins[target_name].append(instance)

                target_entry.set_instance(instance)

                if values:
                    target_entry.set_values([int(value.text, 0) for value in values])
                else:
                    raise ParseError(("%s : %s : For the instance [%d], <value> element(s) "
                                    "are not present") % (name, target_name, instance))

                entry.add_target_entry(target_entry)

            if not entry.is_empty:
                self.attributes[name] = entry

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
