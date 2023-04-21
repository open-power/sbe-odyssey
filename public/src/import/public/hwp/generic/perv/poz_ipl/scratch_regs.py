#!/usr/bin/env python3
# IBM_PROLOG_BEGIN_TAG
# This is an automatically generated prolog.
#
# $Source: public/src/import/public/hwp/generic/perv/poz_ipl/scratch_regs.py $
#
# OpenPOWER sbe Project
#
# Contributors Listed Below - COPYRIGHT 2023
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
"""
Parse scratch register definitions from a spreadsheet into a data model,
then emit text based on a Jinja2 template using that data model.

The spreadsheet contains register definitions and field definitions for
each register bit. A field definition may span multiple register bits.
Also, array fields may be declared which combine multiple similar fields
under a common name, with an index to select the individual field.

A field may be either common, in which case it is valid for all chips,
or chip specific, in which case it is only valid for one chip. Which of the
two it is depends on its location in the spreadsheet.

Each field may have a description, the description may have multiple lines.
Enums are also supported, and described as part of the description text.

The data model consists of a list of registers (regs) and a list of fields
(fields). The template should iterate over those as needed.

The rendered template text is output to stdout and can be redirected into
a file or piped into other programs for further processing, e.g. pandoc to
render Markdown into HTML.

Requirements (install via pip): jinja2, openpyxl

Author: Joachim Fenkes <fenkes@de.ibm.com>
"""
from argparse import ArgumentParser
from collections import defaultdict, namedtuple
import openpyxl, sys, jinja2, importlib, re
from os import path

g_verbosity = 0

def vprint(*args, level=1):
    if g_verbosity >= level:
        print(*args, file=sys.stderr)

# ----------------------------------------------
# Classes for internal data model
# ----------------------------------------------

# The spreadsheet defines multiple Regs
# Each Reg has one or more RegFields
# A RegFieldArray combines multiple RegFields into an array with indices

EnumValue = namedtuple("EnumValue", "value name description")

def parse_enums(mnemonic, description):
    if not description:
        return None, None
    enums = []
    parts = description.split("---ENUM---\n")
    if len(parts) > 1:
        description = parts[0].strip()
        for enum in parts[1].strip().split("\n"):
            value, name, comment, *_ = (enum + ":::").split(":")
            if not value or not name:
                raise ValueError("Enum entry for %s needs at least a value and a name" % mnemonic)
            enums.append(EnumValue(value, name, comment))

    return description, enums

class RegField:
    def __init__(self, array, register, bit, mnemonic, attribute, description):
        self.array, self.register, self.mnemonic, self.attribute = array, register, mnemonic, attribute
        self.description, self.enums = parse_enums(mnemonic, description)
        self.bits = range(bit, bit + 1)
        self.register.fields.append(self)

    def add(self, register, bit):
        if register != self.register:
            raise ValueError("Field %s spans multiple registers" % self.mnemonic)
        if bit != self.bits.stop:
            raise ValueError("Field bits for %s are not consecutive" % self.mnemonic)
        self.bits = range(self.bits.start, bit + 1)

    @property
    def single_bit(self):
        return len(self.bits) == 1

class RegFieldArray:
    def __init__(self, register, bit, mnemonic, index, attribute, description):
        self.mnemonic, self.attribute = mnemonic, attribute
        self.description, self.enums = parse_enums(mnemonic, description)
        self.offset = index
        self.components = []
        self.add(register, bit, index)
        self.components[0].attribute = attribute
        self.components[0].description = description

    def add(self, register, bit, index):
        if index == self.offset + len(self.components) - 1:
            self.components[-1].add(register, bit)
        elif index == self.offset + len(self.components):
            self.components.append(RegField(self, register, bit, "%s[%d]" % (self.mnemonic, index), None, None))
        else:
            raise ValueError("Register field arrays must be consecutive")

    @property
    def range(self):
        return range(self.offset, self.offset + len(self.components))

class Reg:
    def __init__(self, mnemonic, cfam, scom, description):
        super().__init__()
        self.mnemonic, self.description = mnemonic, description
        self.cfam = int(cfam, 0)
        self.scom = int(scom, 0)
        self.fields = []

# ----------------------------------------------
# Parse a spreadsheet into the data model
# ----------------------------------------------

def parse(sheet):
    h1 = next(sheet)
    h2 = next(sheet)
    headers = list(zip(h1, h2))
    vprint("Headers:", headers, level=2)

    look_for = (
        ('Register', 'Mnemonic'),
        ('Register', 'CFAM'),
        ('Register', 'SCOM'),
        ('Register', 'Description'),
        ('Bit', None),
        ('Common', 'Mnemonic'),
        ('Common', 'Attribute'),
        ('Common', 'Description'),
        (args.chip, 'Mnemonic'),
        (args.chip, 'Attribute'),
        (args.chip, 'Description')
    )

    columns = [None] * len(look_for)
    for i, h1 in enumerate(headers):
        for j, h2 in enumerate(look_for):
            if h1 == h2:
                columns[j] = i

    vprint("Columns found:", columns, level=2)

    if None in columns:
        raise ValueError("Could not find all required columns in sheet")

    cur_reg = None
    regs = []
    fields = dict()
    for row in sheet:
        row = [row[i] for i in columns]

        if row[0]:
            cur_reg = Reg(*row[0:4])
            regs.append(cur_reg)

        bit = row[4]
        common_field = row[5:8]
        chip_field = row[8:11]
        if bit is None:
            continue

        if common_field[0] and chip_field[0]:
            raise ValueError("A bit cannot have both a common and chip specific definition")

        if not (common_field[0] or chip_field[0]):
            continue

        mnemonic, attribute, description = common_field if common_field[0] else chip_field
        if "[" in mnemonic:
            mnemonic, index = mnemonic.split("[")
            index = int(index.rstrip("]"))
            if mnemonic not in fields:
                fields[mnemonic] = RegFieldArray(cur_reg, bit, mnemonic, index, attribute, description)
            else:
                fields[mnemonic].add(cur_reg, bit, index)
        else:
            if mnemonic not in fields:
                fields[mnemonic] = RegField(None, cur_reg, bit, mnemonic, attribute, description)
            else:
                fields[mnemonic].add(cur_reg, bit)

    return regs, fields

# ----------------------------------------------
# Parse arguments
# ----------------------------------------------

parser = ArgumentParser(description="Generate text from scratch register descriptions in a spreadsheet using Jinja2 templates")
parser.add_argument("input", help="Path to input .xlsx")
parser.add_argument("chip", help="Chip to extract data for")
parser.add_argument("template", help="Jinja2 template for output")
parser.add_argument("--verbose", "-v", action="count", default=0, help="Print more information during processing; repeat to increase verbosity")
args = parser.parse_args()
g_verbosity = args.verbose
vprint("Arguments:", args)

wb = openpyxl.load_workbook(args.input, data_only=True)
sheet = wb["DEFS"].values

try:
    regs, fields = parse(sheet)
except ValueError as e:
    print("Error: " + e.args[0])
    exit(1)

env = jinja2.Environment(
    loader=jinja2.FileSystemLoader("."),
    trim_blocks=True,
    lstrip_blocks=True,
    undefined=jinja2.StrictUndefined)

env.globals = {
    "regs": regs,
    "fields": list(fields.values()),
}

print(env.get_template(args.template).render())
