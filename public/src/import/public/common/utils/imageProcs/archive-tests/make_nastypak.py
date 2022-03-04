#!/usr/bin/env python3
# IBM_PROLOG_BEGIN_TAG
# This is an automatically generated prolog.
#
# $Source: public/src/import/public/common/utils/imageProcs/archive-tests/make_nastypak.py $
#
# OpenPOWER sbe Project
#
# Contributors Listed Below - COPYRIGHT 2022
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
Create a pak file with specially constructed compressed data:
30K deflated, followed by 4K stored, followed by 30+K deflated

This is supposed to exercise the streaming code in tinflate.C,
where the stored block crosses over the dictionary boundary and
will be split up during streaming. The following deflated block
is expected to reference parts of the first and second block
in the dictionary.

This stream checks for two things:
 1) The streaming code properly wraps around the edge of the dictionary
 2) The dictionary state is correct when we return to unpacking deflated data
"""
from zlib import *
from collections import namedtuple
from struct import Struct

PakHeader = namedtuple("PakHeader", "magic, version, hesize, flags, method, nsize, crc, csize, dsize, psize")
PakFooterStruct = Struct(">II")
PakHeaderStruct = Struct(">IHHBBHIIII")
PAK_MAGIC = 0x50414B21
PAK_END = 0x2F50414B
PAK_VERSION = 1
METHOD_DEFLATE = 2

def comp(data, zdict, level, flush=Z_SYNC_FLUSH):
    c = compressobj(wbits=-15, level=level, zdict=zdict)
    return c.compress(data) + c.flush(flush)

def craft_pak(pak_name, uncompressed_data, compressed_data, compressed_name):
    # construct the pak header
    fname = compressed_name.encode()
    data_padding = -len(compressed_data) & 7
    hdr_padding = -(PakHeaderStruct.size + len(fname)) & 7
    pak_header = PakHeader(PAK_MAGIC, PAK_VERSION, 20 + len(fname) + hdr_padding, 0, METHOD_DEFLATE, len(fname), crc32(uncompressed_data), len(compressed_data), len(uncompressed_data), len(compressed_data) + data_padding)

    # write pak and bin
    pak_data = PakHeaderStruct.pack(*pak_header) + fname + bytes(hdr_padding) + compressed_data + bytes(data_padding)
    open(pak_name, "wb").write(pak_data + PakFooterStruct.pack(PAK_END, len(pak_data)))
    open(compressed_name, "wb").write(uncompressed_data)

# grab arbitrary compressible data from the test suite for test data
data1 = open("pibmem.bin", "rb").read(30*1024)
data2 = data1[:4096]
data3 = open("test", "rb").read(32*1024)

# craft special compressed blob and uncompressed equivalent
compressed_data = comp(data1, bytes(), 9) + comp(data2, data1, 0) + comp(data3, data1+data2, 9, Z_FINISH)
uncompressed_data = data1 + data2 + data3

craft_pak("nastytest.pak", uncompressed_data, compressed_data, "nastytest.bin")


# Craft a second special pak that tests out hash block boundaries
# Any "errors" in this string are intentional to hit the target compressed size, do not change!
uncompressed_data = b"This string is going to compress down to a little less than a hash hash boundary! "
compressed_data = comp(uncompressed_data, bytes(), 9)
compressed_data = compressed_data * 100 + comp(uncompressed_data, bytes(), 9, Z_FINISH)
uncompressed_data = uncompressed_data * 101
craft_pak("hashblockbndy.pak", uncompressed_data, compressed_data, "hashblockbndy.bin")
