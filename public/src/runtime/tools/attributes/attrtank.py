#!/usr/bin/env python3
# IBM_PROLOG_BEGIN_TAG
# This is an automatically generated prolog.
#
# $Source: public/src/runtime/tools/attributes/attrtank.py $
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
import hashlib
from collections import namedtuple
import struct
from attrdb import *
import copy
import numpy as np

class ArgumentError(Exception):
    def __init__(self, message="Argument Exception occured"):
        self.message = "Argument Error :\n"+message
        super().__init__(self.message)

class _AttrIntValueType(object):
    def __init__(self, typestr:str):
        self._type = typestr
        self.size = struct.calcsize(typestr)

    def get(self, image:bytearray, offset:int) -> int:
        return struct.unpack_from(self._type, image, offset)[0]

    def valuestr(self, image:bytearray, offset:int) -> str:
        return "0x%x" % struct.unpack_from(self._type, image, offset)[0]

    def set(self, image:bytearray, offset:int, value:list):
        for idx in range (0,len(value)):
            struct.pack_into(self._type, image, offset+(idx*self.size), value[idx])

    def set_element(self, image:bytearray, offset:int, value:int):
        struct.pack_into(self._type, image, offset, value)

class _EnumValueType(object):
    def __init__(self, base_type:_AttrIntValueType, enum_values:dict):
        self._base = base_type
        self._values = enum_values
        self._inverse = {v: k for k, v in enum_values.items()}
        self.size = base_type.size

    def get(self, image:bytearray, offset:int):
        value = self._base.get(image, offset)
        try:
            return self._inverse[value]
        except KeyError:
            return value

    def valuestr(self, image:bytearray, offset:int) -> str:
        return self.get(image, offset)+"("+str(self._base.get(image, offset))+")"

    def set(self, image:bytearray, offset:int, value:list):
        if isinstance(value, str):
            self._base.set_element(image, offset, self._values[value[0]])
        else:
            self._base.set_element(image, offset, value[0])

    def set_element(self, image:bytearray, offset:int, value:'int|str'):
        if isinstance(value, str):
            if value in self._values:
                self._base.set_element(image, offset, self._values[value])
            else:
                raise ArgumentError('Value is not an enum')
        else:
            if value in self._values.values():
                self._base.set_element(image, offset, value)
            else:
                raise ArgumentError('Integer value passed does not map to any\
 enum')

class _ArrayValueType(object):
    def __init__(self, base_type:_AttrIntValueType, dim:list):
        self._base = base_type
        self._dim = dim
        self.size = base_type.size * dim

    def get(self, image:bytearray, offset:int) -> list:
        return [self._base.get(image, offset + self._base.size * i) for i in \
                range(self._dim)]

    def valuestr(self, image:bytearray, offset:int) -> list:
        return [self._base.valuestr(image, offset + self._base.size * i) for i in range(self._dim)]

    def set_element(self, image:bytearray, offset:int, value:int):
        self._base.set_element(image, offset, value)

    def set_element_by_index(
        self, image:bytearray, base_offset:int, value:int, index:list, cur_dim:int=0):

        if(len(index) == 0):
            raise ArgumentError("Expected index")
        if(index[0] > self._dim):
            vprint("index (%d) at dimension %d, is out of range (%d)"
                    %(index[0], cur_dim, self._dim))
            raise ArgumentError("Index out of range")
        base_offset = base_offset + index[0] * self._base.size
        if(isinstance(self._base, _ArrayValueType)):
            return self._base.set_element_by_index(
                image, base_offset, value, index[1:], cur_dim + 1)
        else:
            # this the 0th dimension
            assert (len(index) == 1), "more dimension than expected"
            return self.set_element(image, base_offset, value)

    def set(self, image:bytearray, offset:int, value:list):
        if len(value) != self._dim:
            raise ValueError("Invalid array attribute value - array dim %d, value dim %d" % (self._dim, len(value)))
        for i in range(self._dim):
            self._base.set_element(image, offset + self._base.size * i, value[i])


class AttrFieldInfo(object):
    has_storage = False
    has_ec = False

    def __init__(self,
                 name: str,
                 hash: int,
                 sbe_target_type:list,
                 ekb_target_type:list,
                 value_type: str,
                 enum_values: str = None,
                 writeable: bool = False,
                 platinit: bool = False) -> None:

        self.name = name
        self._hash = hash
        self.sbe_target_type = copy.deepcopy(sbe_target_type)
        self.ekb_target_type = " | ".join(ekb_target_type)
        self.value_type = value_type + '_t'
        self.writeable = writeable
        self.platinit = platinit
        if enum_values is not None:
            self.enum_values = dict()
            elements = enum_values.text.split(",")
            if not elements[-1].strip():
                del elements[-1] # tolerate trailing comma
            for elem in elements:
                parts = elem.split("=")
                if len(parts) != 2:
                    raise ParseError("Attribute %s: Incorrect enum value syntax" % self.name)

                try:
                    name, value = (x.strip() for x in parts)
                    self.enum_values[name] = int(value, 0)
                except ValueError:
                    raise ParseError("Attribute %s: Invalid enum value '%s' for %s" % (self.name, value, name))
        else:
            self.enum_values = None

    def createDumpRecord(self, attr_list, image, image_base):
        raise NotImplementedError("Dumping this level attribute is not implemented")

    @property
    def hash(self) -> str:
        return hex(self._hash)

    @property
    def type_dims(self):
        return ""

    @property
    def dataType(self):
        return ""

    internal_dims = type_dims


class RealAttrFieldInfo(AttrFieldInfo):
    has_storage = True
    _VALUE_TYPES = {
        "int8":   _AttrIntValueType(">b"),
        "uint8":  _AttrIntValueType(">B"),
        "int16":  _AttrIntValueType(">h"),
        "uint16": _AttrIntValueType(">H"),
        "int32":  _AttrIntValueType(">i"),
        "uint32": _AttrIntValueType(">I"),
        "int64":  _AttrIntValueType(">q"),
        "uint64": _AttrIntValueType(">Q"),
    }

    def __init__(self,
                 name: str,
                 hash: int,
                 sbe_target_type: list,
                 ekb_target_type: list,
                 value_type: str,
                 enum_values: str,
                 writeable: bool,
                 platinit: bool,
                 sbe_entry : SBEEntry,
                 array_dims: list = []) -> None:
        super(RealAttrFieldInfo, self).__init__(
            name, hash, sbe_target_type, ekb_target_type, value_type, enum_values,
                    writeable, platinit)

        self.sbe_entry = copy.deepcopy(sbe_entry)
        self.array_dims = array_dims
        self.sbe_address = dict()
        try:
            self._type = self._VALUE_TYPES[value_type.lower()]
        except KeyError:
            raise ParseError("Attribute %s: Unknown value type '%s'" % (self.name, self.value_type))

        if self.enum_values is not None:
            self._type = _EnumValueType(self._type, self.enum_values)

        #The output of np.prod() is numpy.float64 though the
        #values are int. Hence, using int() to convert from
        #numpy.float64 to int
        #Note : If the size of the list is 0, then np.prod() will return 1
        array_size = int(np.prod(self.array_dims))

        self.attr_size_bytes = (array_size * self._type.size)

    def __str__(self):
        return self.name +" "+ self.typestr() +" "+", ".join(self.sbe_target_type)

    def set(self, image, image_base, TARGET_TYPES):
        #Check if the initializer list is same for each of the supported
        #target type and its instances
        #ie. All the instances of all the supported target type for this attribute
        #    will be initialized with the same initializer list
        if ( (len(self.sbe_entry.target_entries) == 1) and
             (self.sbe_entry.target_entries[0].name == "ALL") ):
            for target_type in self.sbe_target_type:
                vprint("%s : Target type [%s] values [%s]" % (self.name,
                        self.sbe_entry.target_entries[0].name,
                        str(self.sbe_entry.target_entries[0].values)))
                self.update(image, image_base, target_type,
                            range(0, TARGET_TYPES[target_type].ntargets),
                            self.sbe_entry.target_entries[0].values)
        else:
            for target_entry in self.sbe_entry.target_entries:
                vprint("%s : Target type [%s] values [%s]" %
                        (self.name, target_entry.name, str(target_entry.values)))
                if (target_entry.instance == 0xFF):
                    instances = range(0, TARGET_TYPES[target_entry.name].ntargets)
                else:
                    instances = range(target_entry.instance,
                                    (target_entry.instance + 1))

                self.update(image, image_base, target_entry.name,
                            instances, target_entry.values)

    def set_value(self, image, image_base, target_type, value,
                                    in_instance, subscript, TARGET_TYPES) -> None:
        if ( len(subscript) != len(self.array_dims) ):
            raise ValueError("You are trying to update [%d] dimension attribute "
                             "of %s by specifying [%d] dimension coordinate" %
                        (len(self.array_dims), str(self.array_dims), len(subscript)))

        for idx in range(0,len(self.array_dims)):
            if ( subscript[idx] >= self.array_dims[idx] ):
                raise ValueError("The attribute is of dimension %s and you are "
                                 "trying to update %s" % (str(self.array_dims),
                                 str(subscript)))

        qual_attr_name = target_type+"::"+self.name
        sbe_addr = self.sbe_address.get(qual_attr_name,None)
        if (sbe_addr == None):
            raise ValueError("Address of [%s] is not found" % qual_attr_name)

        ntargets = TARGET_TYPES[target_type].ntargets
        if ((in_instance == 0xFF) or (in_instance is None)):
            instances = range(0, ntargets)
        else:
            if (in_instance >= ntargets):
                raise ValueError(("%s : %s : instance=%d exceeds " +
                                 "max instance [%d]") %
                               (self.name, target_type, in_instance, (ntargets-1)))
            else:
                instances = range(in_instance,(in_instance + 1))

        array_dims = self.array_dims.copy()

        #Check if it is a multi instance target
        #In case of multi instance target, the first subscript refers
        #to the instance number
        if (ntargets > 1):
            array_dims.insert(0,ntargets)

        for instance in instances:
            coordinate = subscript.copy()
            if (ntargets > 1):
                coordinate.insert(0,instance)

            if ( len(array_dims) > 0 ):
                try:
                    index = np.ravel_multi_index(coordinate,array_dims,
                                mode='raise', order='C')
                except Exception as e:
                    print("Exception occured. Message is " + str(e))
                    raise e

                offset = (sbe_addr + (index * self._type.size) - image_base)
            else:
                offset = sbe_addr - image_base

            try:
                self._type.set_element(image,offset,value)
            except Exception as e:
                vprint("Failed to set the value for the attribute [%s]" %  self.name)
                vprint("For the target type [%s]" % target_type)
                vprint("value=" + str(value))
                vprint("typeofvalue=", type(value))
                vprint("dimension="+ str(self.array_dims))
                raise e

    def update(self, image, image_base, target_type, instances, values) -> None:
        qual_attr_name = target_type+"::"+self.name
        sbe_addr = self.sbe_address.get(qual_attr_name,None)
        if (sbe_addr == None):
            raise ValueError("Address of [%s] is not found" % qual_attr_name)

        for instance in instances:
            offset = (sbe_addr - image_base) +  (instance * self.attr_size_bytes)
            try:
                self._type.set(image,offset,values)
            except Exception as e:
                vprint("Failed to set the value for the attribute [%s]" %  self.name)
                vprint("For the target type [%s]" % target_type)
                vprint("values=" + str(values))
                vprint("typeofvalue=", type(values))
                vprint("dimension="+ str(self.array_dims))
                raise e

    def typestr(self):
        """
        # Function to get the attribute type in human readable format
        # TODO: Need to support enums also. (Not tested this yet)
        """
        return self.value_type + "".join("[%d]" % dim for dim in self.array_dims)

    def createDumpRecord(self, attr_list, image, image_base, TARGET_TYPES):
        attr_name = self.name
        for target_type in self.sbe_target_type:
            qual_attr_name = target_type+"::"+attr_name
            sbe_addr = self.sbe_address.get(qual_attr_name,None)
            if ( sbe_addr == None ):
                my_attr_fields = [attr_name, target_type, '* ERROR *',
                                    '* ERROR *']
                attr_list.append(my_attr_fields)
                continue
            ntargets = TARGET_TYPES[target_type].ntargets

            #The output of np.prod() is numpy.float64 though the
            #values are int. Hence, using int() to convert from
            #numpy.float64 to int
            array_size = int(np.prod(self.array_dims))
            number_of_elements = ntargets * array_size
            if ( number_of_elements == 1 ):
                value = self._type.get(image,  sbe_addr - image_base)
                my_attr_fields = [attr_name, target_type, self.value_type,
                                    str(value)]
                attr_list.append(my_attr_fields)
                continue
            array_dims = list()
            if ( ntargets > 1 ):
                array_dims.append(ntargets)
            if ( len(self.array_dims) > 1 ):
                array_dims.extend(self.array_dims)
            for i in range(0, number_of_elements):
                value = self._type.get(image,  sbe_addr - image_base)
                sbe_addr += self._type.size
                subscript = list(np.unravel_index(i,array_dims))
                target_type_s = target_type
                if ( ntargets > 1 ):
                    target_type_s = target_type + "".join("[%d]" % subscript.pop(0))
                attr_name_s = attr_name
                if ( len(subscript) > 0 ):
                    attr_name_s = attr_name +  \
                        "".join("[%d]" % dim for dim in subscript)

                my_attr_fields = [attr_name_s, target_type_s, self.value_type,
                                    str(value)]
                attr_list.append(my_attr_fields)
        return attr_list

    def get(self, image, image_base):
        return self._type.get(image, self.sbe_address - image_base)

    @property
    def type_dims(self):
        return "".join("[%d]" % dim for dim in self.array_dims)

    #@property
    def internal_dims(self,ntargets):
        retval = ""
        if ntargets > 1:
            retval = "[%d]" % ntargets
        retval += self.type_dims
        return retval

    def array_size(self):
        #The output of np.prod() is numpy.float64 though the
        #values are int. Hence, using int() to convert from
        #numpy.float64 to int
        #Note : If the size of the list is 0, then np.prod() will return 1
        return int(np.prod(self.array_dims))

    def var_name(self):
        var_name = "fapi2::ATTR::" + self.name
        if(self.target == 'TARGET_TYPE_PERV'):
            var_name += "[TARGET.get().getChipletNumber()]"
        elif self.num_targ_inst > 1:
            var_name += "[TARGET.get().getTargetInstance()]"
        return var_name

    @property
    def getter(self):
        return "ATTR::get_" + self.name + "(TARGET,VAL)"

    @property
    def setter(self):
        return "ATTR::set_" + self.name + "(TARGET,VAL)"

    def get_var_definition(self,target_type,ntargets):
        retval = "namespace " + target_type
        retval += "\n{\n"
        retval += self.value_type + " " + self.name + self.internal_dims(ntargets) + " "
        retval += '__attribute__((section(".attrs")));'
        retval += "\n}"
        return retval

    def get_var_declaration(self,target_type,ntargets):
        retval = "namespace " + target_type
        retval += "\n{\n"
        retval += "extern " + self.value_type + " " + self.name + \
                    self.internal_dims(ntargets) + " "
        retval += '__attribute__((section(".attrs")));'
        retval += "\n}"
        return retval

    def get_template_definition(self):
        retval = "template <TargetType T>\n"
        if (self.array_size() == 1):
            retval += "fapi2::ReturnCode get_" + self.name + \
                 "(const fapi2::Target<T> & i_target, " + self.name + "_Type &o_val)"
        else:
            retval += "fapi2::ReturnCode get_" + self.name + \
                 "(const fapi2::Target<T> & i_target, " + self.name + "_Type o_val)"

        retval += "\n{\n"
        retval += "return fapi2::FAPI2_RC_SUCCESS;"
        retval += "\n}"
        return retval

    def set_template_definition(self):
        retval = "template <TargetType T>\n"
        retval += "fapi2::ReturnCode set_" + self.name + \
              "(const fapi2::Target<T> & i_target, const " + self.name + "_Type o_val)"

        retval += "\n{\n"
        retval += "return fapi2::FAPI2_RC_SUCCESS;"
        retval += "\n}"
        return retval

    def get_template_specialization(self,target_type,ntargets):

        if (ntargets > 1):
            if (self.array_size() == 1):
                retval = "template <>\n"
                retval += "inline fapi2::ReturnCode get_" + self.name + \
                        "(const fapi2::Target<fapi2::" + target_type + "> & i_target, " + \
                            self.name + "_Type & o_val)\n"
                retval += "{\n"
                retval += "o_val = \n"
                retval += "fapi2::ATTR::" + target_type + "::" + self.name + \
                          "[i_target.get().getChipletNumber()];\n"
                retval += "return fapi2::FAPI2_RC_SUCCESS;"
                retval += "}\n"
            else:
                retval = "template <>\n"
                retval += "inline fapi2::ReturnCode  get_" + self.name + \
                        "(const fapi2::Target<fapi2::"  + target_type + "> & i_target, " + \
                                 self.name + "_Type  o_val)\n"
                retval += "{\n"
                retval += "memcpy(o_val,fapi2::ATTR::" + target_type + "::" + self.name + \
                          "[i_target.get().getChipletNumber()], sizeof(" + self.name + \
                          "_Type));\n"
                retval += "return fapi2::FAPI2_RC_SUCCESS;"
                retval += "}\n"
        else:
            if (self.array_size() == 1):
                retval = "template <>\n"
                retval += "inline fapi2::ReturnCode get_" + self.name + \
                        "(const fapi2::Target<fapi2::" + target_type + "> & i_target, " + \
                            self.name + "_Type & o_val)\n"
                retval += "{\n"
                retval += "o_val = \n"
                retval += "fapi2::ATTR::" + target_type + "::" + self.name + ";\n"
                retval += "return fapi2::FAPI2_RC_SUCCESS;"
                retval += "}\n"
            else:
                retval = "template <>\n"
                retval += "inline fapi2::ReturnCode get_" + self.name + \
                        "(const fapi2::Target<fapi2::" + target_type + "> & i_target, " + \
                            self.name + "_Type o_val)\n"
                retval += "{\n"
                retval += "memcpy(o_val,fapi2::ATTR::" + target_type + "::" + self.name + \
                          ", sizeof(" + self.name + "_Type));\n"
                retval += "return fapi2::FAPI2_RC_SUCCESS;"
                retval += "}\n"
        return retval

    def set_template_specialization(self,target_type,ntargets):

        if (ntargets > 1):
            if (self.array_size() == 1):
                retval = "template <>\n"
                retval += "inline fapi2::ReturnCode set_" + self.name + \
                        "(const fapi2::Target<fapi2::" + target_type + \
                        "> & i_target, const " + self.name + "_Type i_val)\n"
                retval += "{\n"
                retval += "fapi2::ATTR::" + target_type + "::" + self.name + \
                          "[i_target.get().getChipletNumber()] = i_val;\n"
                retval += "return fapi2::FAPI2_RC_SUCCESS;"
                retval += "}\n"
            else:
                retval = "template <>\n"
                retval += "inline fapi2::ReturnCode set_" + self.name + \
                        "(const fapi2::Target<fapi2::" + target_type + \
                            "> & i_target, const " + self.name + "_Type  i_val)\n"
                retval += "{\n"
                retval += "memcpy(fapi2::ATTR::" + target_type + "::" + self.name + \
                          "[i_target.get().getChipletNumber()], i_val, sizeof(" + \
                           self.name + "_Type));\n"
                retval += "return fapi2::FAPI2_RC_SUCCESS;"
                retval += "}\n"
        else:
            if (self.array_size() == 1):
                retval = "template <>\n"
                retval += "inline fapi2::ReturnCode set_" + self.name + \
                        "(const fapi2::Target<fapi2::" + target_type + \
                            "> & i_target, const " + self.name + "_Type  i_val)\n"
                retval += "{\n"
                retval += "fapi2::ATTR::" + target_type + "::" + self.name + " = i_val;\n"
                retval += "return fapi2::FAPI2_RC_SUCCESS;"
                retval += "}\n"
            else:
                retval = "template <>\n"
                retval += "inline fapi2::ReturnCode set_" + self.name + \
                        "(const fapi2::Target<fapi2::" + target_type + \
                        "> & i_target, const " + self.name + "_Type i_val)\n"
                retval += "{\n"
                retval += "memcpy(fapi2::ATTR::" + target_type + "::" + self.name + \
                          ", i_val, sizeof(" + self.name + "_Type));\n"
                retval += "return fapi2::FAPI2_RC_SUCCESS;"
                retval += "}\n"
        return retval

class VirtualAttrFieldInfo(AttrFieldInfo):
    VIRTUAL_FUNCTION = {
        "ATTR_NAME": "_getAttrName",
        "ATTR_EC": "_getAttrEC",
        "ATTR_CHIP_UNIT_POS": "_getAttrChipUnitPos"
    }

    def __init__(self,
                 name: str,
                 hash: int,
                 sbe_target_type: list,
                 ekb_target_type: list,
                 value_type: str,
                 enum_values: str) -> None:
        super().__init__(name, hash, sbe_target_type, ekb_target_type, value_type, enum_values)

    def set(self, image, image_base, value):
        raise NotImplementedError("Cannot modify a virtual attribute")

    def set_value(self, image, image_base, value, target, index=0):
        raise NotImplementedError("Cannot modify value for virtual attribute")

    def get(self, image, image_base):
        raise NotImplementedError("Querying virtual attributes is not implemented")

    @property
    def getter(self):
        return self.VIRTUAL_FUNCTION[self.name] + "(TARGET, VAL)"

class EcAttrFieldInfo(AttrFieldInfo):
    has_ec = True

    def __init__(self,
                 name: str,
                 hash: int,
                 sbe_target_type: list,
                 ekb_target_type: list,
                 value_type: str,
                 chip_name: str,
                 ec_value: str,
                 ec_test: str) -> None:

        super().__init__(name, hash, sbe_target_type, ekb_target_type, value_type)
        self.chip_name = chip_name
        self.ec_value = ec_value
        self.ec_test = ec_test

    def set(self, image, image_base, value):
        raise NotImplementedError("Cannot modify an EC level attribute")

    def get(self, image, image_base):
        raise NotImplementedError("Querying EC level attributes is not implemented")

    def set_value(self, image, image_base, value, target, index=0):
        raise NotImplementedError("Cannot modify value for EC level attribute")

    @property
    def getter(self):
        return "queryChipEcFeature(fapi2::int2Type<ID>(), TARGET, VAL)"


class AttributeStructure(object):

    def __init__(self, db: AttributeDB) -> None:
        # Using list, since we need same order while iterating
        self.field_list: list["AttrFieldInfo"] = []
        self.hash_set : set[int] = set()

        self.TARGET_TYPES = copy.deepcopy(AttributeDB.TARGET_TYPES)
        for attr in db.attributes.values():
            if (not attr.sbe_entry) or (attr.sbe_entry is None):
                continue
            vprint("Finalizing sbe fields for attribute: " + attr.name)

            attr_hash16bytes = hashlib.md5(attr.name.encode()).digest()
            attr_hash32bits = int.from_bytes(attr_hash16bytes[0:4], "big")
            attr_hash28bit = attr_hash32bits >> 4;
            if attr_hash28bit in self.hash_set:
                raise ParseError("Hash for attribute " + attr.name +
                        " already used")
            else:
                self.hash_set.add(attr_hash28bit)

            if isinstance(attr, ECAttribute):
                self.field_list.append(EcAttrFieldInfo(
                    attr.name,
                    attr_hash28bit,
                    attr.sbe_target_type,
                    attr.ekb_target_type,
                    attr.value_type,
                    attr.chip_name,
                    attr.ec_value,
                    attr.ec_test))
            elif attr.sbe_entry.virtual:
                self.field_list.append(VirtualAttrFieldInfo(
                    attr.name,
                    attr_hash28bit,
                    attr.sbe_target_type,
                    attr.ekb_target_type,
                    attr.value_type,
                    attr.enum_values))
            else:
                self.field_list.append(RealAttrFieldInfo(
                    attr.name,
                    attr_hash28bit,
                    attr.sbe_target_type,
                    attr.ekb_target_type,
                    attr.value_type,
                    attr.enum_values,
                    attr.writeable,
                    attr.platinit,
                    attr.sbe_entry,
                    attr.array_dims))

class SymbolTable(object):
    Symbol = namedtuple("Symbol", "name type offset size")

    def __init__(self, fname):
        self.symbols = dict()
        with open(fname, "r") as symtab:
            for line in symtab:
                parts = line.strip().split()
                size = None
                if len(parts) == 4:
                    size = int(parts[1], 16)
                    del parts[1]
                if len(parts) == 3:
                    # Collect only attribute variables, and remove fapi2::ATTR::
                    if parts[2].startswith("fapi2::ATTR::"):
                        attr_name = parts[2][13:]
                        self.symbols[attr_name] = self.Symbol(attr_name, parts[1], int(parts[0], 16), size)
                    else:
                        self.symbols[parts[2]] = self.Symbol(parts[2], parts[1], int(parts[0], 16), size)

    def update_attrdb(self, attr_tank:AttributeStructure):
        attr_tank.image_base = self.symbols["__vectors"].offset
        attr_tank.start_address = self.symbols["_attrs_start_"].offset
        attr_tank.end_address = self.symbols["_attrs_end_"].offset
        for attr in attr_tank.field_list:
            if not isinstance(attr, RealAttrFieldInfo):
                continue

            for target_type in attr.sbe_target_type:
                qual_attr_name = target_type+"::"+attr.name
                if qual_attr_name in self.symbols:
                    attr.sbe_address[qual_attr_name] = self.symbols[qual_attr_name].offset
                else:
                    raise ParseError("Address is not present in symbol file for " + attr.name + " for the target " + target_type)
