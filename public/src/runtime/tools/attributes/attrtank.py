#!/usr/bin/env python3
# IBM_PROLOG_BEGIN_TAG
# This is an automatically generated prolog.
#
# $Source: public/src/runtime/tools/attributes/attrtank.py $
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
import hashlib
from collections import namedtuple
import struct
from attrdb import *

class _AttrIntValueType(object):
    def __init__(self, typestr:str):
        self._type = typestr
        self.size = struct.calcsize(typestr)

    def get(self, image:bytearray, offset:int) -> int:
        return struct.unpack_from(self._type, image, offset)[0]

    def set(self, image:bytearray, offset:int, value:list):
        struct.pack_into(self._type, image, offset, value[0])

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

    def set(self, image:bytearray, offset:int, value:list):
        if isinstance(value, str):
            self._base.set_element(image, offset, self._values[value[0]])
        else:
            self._base.set_element(image, offset, value[0])

    def set_element(self, image:bytearray, offset:int, value:int):
        self._base.set_element(image, offset, value)

class _ArrayValueType(object):
    def __init__(self, base_type:_AttrIntValueType, dim:list):
        self._base = base_type
        self._dim = dim
        self.size = base_type.size * dim

    def get(self, image:bytearray, offset:int) -> list:
        return [self._base.get(image, offset + self._base.size * i) for i in range(self._dim)]

    def set_element(self, image:bytearray, offset:int, value:int):
        self._base.set_element(image, offset, value)

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
                 target: str,
                 target_type:list,
                 value_type: str,
                 enum_values: str = None,
                 writeable: bool = False,
                 platinit: bool = False) -> None:

        self.name = name
        self._hash = hash
        self.target = target
        self.target_type = " | ".join(target_type)
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

    @property
    def hash(self) -> str:
        return hex(self._hash)

    @property
    def type_dims(self):
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
                 target: str,
                 target_type:list,
                 value_type: str,
                 enum_values: str,
                 writeable: bool,
                 platinit: bool,
                 values: list,
                 num_targ_inst: int = 0,
                 array_dims: list = []) -> None:

        super(RealAttrFieldInfo, self).__init__(
            name, hash, target, target_type, value_type, enum_values, writeable, platinit)

        self.values = values
        self.num_targ_inst = num_targ_inst
        self.array_dims = array_dims

        try:
            self._type = self._VALUE_TYPES[value_type.lower()]
        except KeyError:
            raise ParseError("Attribute %s: Unknown value type '%s'" % (self.name, self.value_type))

        if self.enum_values is not None:
            self._type = _EnumValueType(self._type, self.enum_values)

        for dim in reversed(self.array_dims):
            self._type = _ArrayValueType(self._type, dim)

        self.tot_size = self._type.size

    def set(self, image, image_base, value):
        try:
            self._type.set(image, self.sbe_address - image_base, value)
        except Exception as e:
            vprint("failed set " + self.name)
            vprint("value=" + str(value))
            vprint("typeofvalue=", type(value))
            vprint("dimension="+ str(self.array_dims))
            raise e

    def get(self, image, image_base):
        return self._type.get(image, self.sbe_address - image_base)

    @property
    def type_dims(self):
        retval = ""
        if len(self.array_dims) == 1:
            retval += "[%d]" % self.array_dims[0]
        elif len(self.array_dims) > 1:
            raise ValueError("Multidimensional array attributs not supportd on SBE")
        return retval

    @property
    def internal_dims(self):
        retval = self.type_dims
        if self.num_targ_inst > 1:
            retval += "[%d]" % self.num_targ_inst
        return retval

    def var_name(self):
        var_name = "fapi2::ATTR::" + self.name
        if self.num_targ_inst > 1:
            var_name += "[TARGET.get().getTargetInstance()]"
        return var_name

    @property
    def getter(self):
        return "VAL = " + self.var_name() if not self.array_dims else ("memcpy(VAL, %s, %d)" % (self.var_name(), self.tot_size))

    @property
    def setter(self):
        return self.var_name() + " = VAL" if not self.array_dims else ("memcpy(%s, VAL, %d)" % (self.var_name(), self.tot_size))


class VirtualAttrFieldInfo(AttrFieldInfo):
    VIRTUAL_FUNCTION = {
        "ATTR_NAME": "fapi2::_getAttrName",
        "ATTR_EC": "fapi2::_getAttrEC",
        "ATTR_CHIP_UNIT_POS": "fapi2::_getAttrChipUnitPos"
    }

    def __init__(self,
                 name: str,
                 hash: int,
                 target: str,
                 target_type:list,
                 value_type: str,
                 enum_values: str) -> None:
        super().__init__(name, hash, target, target_type, value_type, enum_values)

    def set(self, image, image_base, value):
        raise NotImplementedError("Cannot modify a virtual attribute")

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
                 target: str,
                 target_type:list,
                 value_type: str,
                 chip_name: str,
                 ec_value: str,
                 ec_test: str) -> None:

        super().__init__(name, hash, target, target_type, value_type)
        self.chip_name = chip_name
        self.ec_value = ec_value
        self.ec_test = ec_test

    def set(self, image, image_base, value):
        raise NotImplementedError("Cannot modify an EC level attribute")

    def get(self, image, image_base):
        raise NotImplementedError("Querying EC level attributes is not implemented")

    @property
    def getter(self):
        return "fapi2::queryChipEcFeature(fapi2::int2Type<ID>(), TARGET, VAL)"


class AttributeStructure(object):

    def __init__(self, db: AttributeDB) -> None:
        # Using list, since we need same order while iterating
        self.field_list: list["AttrFieldInfo"] = []
        self.hash_set : set[int] = set()

        for attr in db.attributes.values():
            if (not attr.sbe_entry) or (attr.sbe_entry is None):
                continue
            vprint("Finallising sbe fields for attribute: " + attr.name)

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
                    attr.sbe_target_type[0],
                    attr.sbe_target_type,
                    attr.value_type,
                    attr.chip_name,
                    attr.ec_value,
                    attr.ec_test))
            elif attr.sbe_entry.virtual:
                self.field_list.append(VirtualAttrFieldInfo(
                    attr.name,
                    attr_hash28bit,
                    attr.sbe_target_type[0],
                    attr.ekb_target_type,
                    attr.value_type,
                    attr.enum_values))
            else:
                self.field_list.append(RealAttrFieldInfo(
                    attr.name,
                    attr_hash28bit,
                    attr.sbe_target_type[0],
                    attr.ekb_target_type,
                    attr.value_type,
                    attr.enum_values,
                    attr.writeable,
                    attr.platinit,
                    attr.sbe_entry.values,
                    TARGET_TYPES[attr.sbe_target_type[0]].ntargets,
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
                        self.symbols[attr_name] = self.Symbol(parts[2], parts[1], int(parts[0], 16), size)
                    else:
                        self.symbols[parts[2]] = self.Symbol(parts[2], parts[1], int(parts[0], 16), size)

    def update_attrdb(self, attr_tank:AttributeStructure):
        attr_tank.image_base = self.symbols["__vectors"].offset
        attr_tank.start_address = self.symbols["_attrs_start_"].offset
        attr_tank.end_address = self.symbols["_attrs_end_"].offset
        for attr in attr_tank.field_list:
            if not isinstance(attr, RealAttrFieldInfo):
                continue

            if attr.name in self.symbols:
                attr.sbe_address = self.symbols[attr.name].offset
            else:
                raise ParseError("address is not present in symbol file for " + attr.name)
