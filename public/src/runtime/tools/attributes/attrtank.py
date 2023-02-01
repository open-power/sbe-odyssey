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
from typing import NamedTuple

class ArgumentError(Exception):
    def __init__(self, message="Argument Exception occured"):
        self.message = "Argument Error :\n"+message
        super().__init__(self.message)

class DumpRecord(NamedTuple):
    name:str
    targ:str
    value:str

class _AttrIntValueType(object):
    def __init__(self, typestr:str):
        self._type = typestr
        self.size = struct.calcsize(typestr)

    def get(self, image:bytearray, offset:int) -> int:
        return struct.unpack_from(self._type, image, offset)[0]

    def valuestr(self, image:bytearray, offset:int) -> str:
        return "0x%x" % struct.unpack_from(self._type, image, offset)[0]

    def dump_record_row(self,
                        image:bytearray, offset:int,
                        partial_row:DumpRecord) -> 'list[DumpRecord]':

        value = "0x%x" % self.get(image, offset)
        return [DumpRecord(partial_row.name, partial_row.targ, value)]

    def set(self, image:bytearray, offset:int, value:list):
        if (len(value) != 1):
            raise ValueError("Invalid array attribute value - expected len=1, but len=%d" % (len(value)))
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

    def dump_record_row(self,
                        image:bytearray, offset:int,
                        partial_row:DumpRecord) -> 'list[DumpRecord]':

        value = str(self.get(image, offset)) + \
                "("+str(self._base.get(image, offset))+")"
        return [DumpRecord(partial_row.name, partial_row.targ, value)]

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
    def __init__(self, base_type:_AttrIntValueType, dim:int, is_targ_dim:bool = False):
        self._base = base_type
        self._dim = dim
        self.size = base_type.size * dim
        self._is_targ_dim = is_targ_dim

    def get(self, image:bytearray, offset:int) -> list:
        return [self._base.get(image, offset + self._base.size * i) for i in \
                range(self._dim)]

    def dump_record_row(self,
                        image:bytearray, offset:int,
                        partial_row:DumpRecord) -> 'list[DumpRecord]':

        ret_list:list[DumpRecord] = []
        for i in range(self._dim):
            if(self._is_targ_dim):
                targ_append = "[" + str(i) + "]"
                name_append = ""
            else:
                name_append = "[" + str(i) + "]"
                targ_append = ""

            new_partial_row = DumpRecord(
                partial_row.name + name_append, partial_row.targ + targ_append, '')

            ret_list += self._base.dump_record_row(image,
                                    offset + self._base.size * i,
                                    new_partial_row)

        return ret_list

    def set_element(self, image:bytearray, offset:int, value:int):
        self._base.set_element(image, offset, value)

    def set_element_by_index(
        self, image:bytearray, base_offset:int, value:int, index:list, cur_dim:int=0):

        if(len(index) == 0):
            raise ArgumentError("Expected index")
        if(index[0] >= self._dim):
            vprint("index (%d) at dimension %d, is out of range (%d)"
                    %(index[0], cur_dim, self._dim))
            raise ArgumentError("Index(%d) out of range(%d)" % (index[0],self._dim))
        base_offset = base_offset + index[0] * self._base.size
        if(isinstance(self._base, _ArrayValueType)):
            return self._base.set_element_by_index(
                image, base_offset, value, index[1:], cur_dim + 1)
        else:
            # this the 0th dimension
            assert (len(index) == 1), "more dimension than expected"
            return self.set_element(image, base_offset, value)

    def set(self, image:bytearray, offset:int, value:list):
        if (len(value) % self._dim != 0):
            raise ValueError(
                "Invalid array attribute value - array dim %d, value dim %d" %
                (self._dim, len(value)))

        sub_list_len = int(len(value) / self._dim)
        for i in range(self._dim):
            sub_list = [value[j] for j in range(i * sub_list_len, (i + 1) * sub_list_len)]
            self._base.set(image, offset + self._base.size * i, sub_list)


class AttrFieldInfo(object):
    has_storage = False
    has_ec = False

    def __init__(self,
                 name: str,
                 hash: int,
                 ekb_target_type:list,
                 value_type: str,
                 enum_values: str = None,
                 writeable: bool = False,
                 platinit: bool = False) -> None:

        self.name = name
        self._hash = hash
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
                 sbe_target_type: str,
                 ekb_target_type: list,
                 value_type: str,
                 enum_values: str,
                 writeable: bool,
                 platinit: bool,
                 targ_entry : TargetEntry,
                 array_dims: list = []) -> None:
        super(RealAttrFieldInfo, self).__init__(
            name, hash, ekb_target_type, value_type, enum_values,
                    writeable, platinit)

        self.sbe_targ_type = sbe_target_type
        self.targ_entry = copy.deepcopy(targ_entry)
        self.array_dims = array_dims
        self.sbe_address = 0
        self.num_targ_inst = AttributeDB.TARGET_TYPES[sbe_target_type].ntargets
        try:
            self._type = self._VALUE_TYPES[value_type.lower()]
        except KeyError:
            raise ParseError("Attribute %s: Unknown value type '%s'" % (self.name, self.value_type))

        if self.enum_values is not None:
            self._type = _EnumValueType(self._type, self.enum_values)

        for dim in reversed(self.array_dims):
            self._type = _ArrayValueType(self._type, dim)
        if(self.num_targ_inst > 1):
            self._type = _ArrayValueType(self._type, self.num_targ_inst, True)
        self.tot_size = self._type.size

    def set(self, image, image_base, value:list):
        try:
            self._type.set(image, self.sbe_address - image_base, value)
        except Exception as e:
            vprint("failed set " + self.name)
            vprint("value=" + str(value))
            vprint("typeofvalue=", type(value))
            vprint("dimension="+ str(self.array_dims))
            raise e

    def setfixed(self, image, image_base):
        vprint("Setting values for attribute", self.name, self.sbe_targ_type)

        values = []

        if(self.targ_entry.comm_values):
            values = self.targ_entry.values[0xFF] * self.num_targ_inst
        else:
            for i in range(self.num_targ_inst):
                values += self.targ_entry.values[i]

        vprint("values:", values)

        try:
            self._type.set(image, self.sbe_address - image_base, values)
        except Exception as e:
            vprint("failed set " + self.name)
            vprint("value=" + str(values))
            vprint("typeofvalue=", type(values))
            vprint("dimension="+ str(self.array_dims))
            raise e

    def set_value(self, image, image_base, value, instance, index:list):

        offset = self.sbe_address - image_base
        if(self.num_targ_inst > 1):
            index.insert(0, instance)
        try:
            if isinstance(self._type, _ArrayValueType):
                self._type.set_element_by_index(image, offset, value, index)
            else:
                self._type.set_element(image, offset, value)
        except Exception as e:
            vprint("failed set " + self.name)
            vprint("value=" + str(value))
            vprint("index= ", str(index))
            raise e

    def typestr(self):
        """
        # Function to get the attribute type in human readable format
        # TODO: Need to support enums also. (Not tested this yet)
        """
        return self.value_type + "".join("[%d]" % dim for dim in self.array_dims)

    def createDumpRecord(self, attr_list, image, image_base):
        partial_row = DumpRecord(self.name, self.sbe_targ_type, '')
        dump_record_list = self._type.dump_record_row(
                                    image,
                                    self.sbe_address - image_base,
                                    partial_row)

        for dump_rec in dump_record_list:
            attr_list.append([dump_rec.name, dump_rec.targ, self.value_type, dump_rec.value])

        return attr_list

    def get(self, image, image_base):
        return self._type.get(image, self.sbe_address - image_base)

    @property
    def type_dims(self) -> str:
        retval = ""
        for dim in self.array_dims:
             retval += "[%d]" % dim
        return retval

    @property
    def internal_dims(self) -> str:
        retval = self.type_dims
        if self.num_targ_inst > 1:
            retval = "[%d]" % self.num_targ_inst + retval
        return retval

    def targ_inst(self, targ_var:str):
        inst_index = ""
        if self.num_targ_inst > 1:
            inst_index += "[" + targ_var + ".get()."
            if(self.sbe_targ_type == 'TARGET_TYPE_PERV'):
                inst_index += "getChipletNumber()"
            else:
                inst_index += "getTargetInstance()"
            inst_index += "]"
        return inst_index

    @property
    def getter(self):
        return "ATTR::get_" + self.name + "(TARGET,VAL)"

    @property
    def setter(self):
        return "ATTR::set_" + self.name + "(TARGET,VAL)"

    @property
    def first_attribute(self):
        return (self.ekb_target_type != '')

    @property
    def support_composite_target(self):
        if(self.ekb_target_type == ''):
            return False

        targ_list = self.ekb_target_type.split('|')
        if(len(targ_list) == 1):
            return False

        # check whether POZ chip type
        # TODO: (JIRA: PFSBE-268)
        #     remove this check, once we have function implemented for any target
        poz_chip_list = ['TARGET_TYPE_HUB_CHIP',
                        'TARGET_TYPE_COMPUTE_CHIP',
                        'TARGET_TYPE_PROC_CHIP',
                        'TARGET_TYPE_OCMB_CHIP']
        fapi2_targ = ''
        self.ekb_target_with_fapi2 = ''
        for targ in targ_list:
            if(targ.strip() in poz_chip_list):
                if(fapi2_targ != ''):
                    fapi2_targ += ' | '
                fapi2_targ += "fapi2::" + targ.strip()
            else:
                return False

        # this property will be called subsequent, if current function return True
        self.ekb_target_with_fapi2 = fapi2_targ

        return True

class VirtualAttrFieldInfo(AttrFieldInfo):
    VIRTUAL_FUNCTION = {
        "ATTR_NAME": "_getAttrName",
        "ATTR_EC": "_getAttrEC",
        "ATTR_CHIP_UNIT_POS": "_getAttrChipUnitPos"
    }

    def __init__(self,
                 name: str,
                 hash: int,
                 ekb_target_type: list,
                 value_type: str,
                 enum_values: str) -> None:
        super().__init__(name, hash, ekb_target_type, value_type, enum_values)

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
                 ekb_target_type: list,
                 value_type: str,
                 chip_name: str,
                 ec_value: str,
                 ec_test: str) -> None:

        super().__init__(name, hash, ekb_target_type, value_type)
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
                    attr.ekb_target_type,
                    attr.value_type,
                    attr.chip_name,
                    attr.ec_value,
                    attr.ec_test))
            elif attr.sbe_entry.virtual:
                self.field_list.append(VirtualAttrFieldInfo(
                    attr.name,
                    attr_hash28bit,
                    attr.ekb_target_type,
                    attr.value_type,
                    attr.enum_values))
            else:
                ekb_target_list = attr.ekb_target_type
                for sbe_targ in attr.sbe_target_type:
                    targ_entry = attr.sbe_entry.get_target_entry(sbe_targ)
                    if(targ_entry == None):
                        raise Exception("Invalid target entry list for attribute %s for target %s" % {attr.name, sbe_targ})
                    self.field_list.append(RealAttrFieldInfo(
                        attr.name,
                        attr_hash28bit,
                        sbe_targ,
                        ekb_target_list,
                        attr.value_type,
                        attr.enum_values,
                        attr.writeable,
                        attr.platinit,
                        targ_entry,
                        attr.array_dims))

                    # ekb_target_type is only required for the attribute structure of the first target
                    ekb_target_list = []

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

            qual_attr_name = attr.sbe_targ_type + "::" +attr.name

            if qual_attr_name in self.symbols:
                attr.sbe_address = self.symbols[qual_attr_name].offset
            else:
                raise ParseError("Address is not present in symbol file for "
                                    +attr.name + " for the target " +
                                    attr.sbe_targ_type)
