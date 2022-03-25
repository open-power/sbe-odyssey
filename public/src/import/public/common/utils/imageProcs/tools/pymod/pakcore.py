#!/usr/bin/env python3
# IBM_PROLOG_BEGIN_TAG
# This is an automatically generated prolog.
#
# $Source: public/src/import/public/common/utils/imageProcs/tools/pymod/pakcore.py $
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

############################################################
# Imports - Imports - Imports - Imports - Imports - Imports
############################################################
import zlib
import os
import sys
import struct
from collections import UserList
from enum import IntEnum
from pakconstants import out
import ast

############################################################
# Classes - Classes - Classes - Classes - Classes - Classes
############################################################
class CM(IntEnum):
    '''
    Define the compression methods

    Note: I too believe defines should be in CAPS
    However, by defining them lowercase the convenience parts all just works much smoother
    i.e.. it matches the input in the manifest and on the cmdline
    '''
    store = 1
    zlib = 2
    zlib_fast = 3
    zlib_ppc = 4

    # Needed to make choices in argparse work
    def argparse(self):
        try:
            return CM[self]
        except KeyError:
            return self

    # Removes the CM. off prints in argparse
    def __str__(self):
        return self.name

    # Prints an easy to read list of options
    def __repr__(self):
        return str(self)

class ArchiveError(Exception):
    '''
    Put all program errors through exceptions instead of return codes
    Give us hooks into the messages should it be needed
    '''
    def __init__(self, message):
        self.message = message
        super().__init__(self.message)

    def __str__(self):
        return self.message

##############################
# All archive related info
##############################
# Magic at the start of each entry
PAK_START = 0x50414B21 # PAK!
# The total length (4 bytes) will follow the end magic to make it an 8 byte block
PAK_END = 0x2F50414B # /PAK
PAK_VERSION = 1

# If a file is added with a fixed size, store that in the header flags
# This would preserve the psize should the archive be rewritten
PAK_FLAG_FIXED_SIZE = 0x20

class ArchiveEntry(object):
    def __init__(self):
        '''
        Define the structure of a .pak file entry
        The contents is created by unpacking data into it or through ingest
        '''
        ################
        # Start Format #
        ################

        #################
        # Header Core
        # A single 8 byte block that describes core info about the entry
        #
        # 4 bytes - sanity check we are at the start
        self.magic = PAK_START
        # 2 byte - the version
        self.version = PAK_VERSION
        # 2 bytes - the size header extended, to guide the next read
        #self.hesize - a derived property function

        #################
        # Header Extended
        # The extended 8 byte aligned header containing aditional info and the name
        #
        # 1 byte - defined bit flags to describe the entry
        self.flags = 0x0
        # 1 byte - the storage method of the data
        self.method = None
        # 2 bytes - the size of the name
        #self.nsize - a derived property function
        # 4 bytes - the size crc of the actual data, not the payload
        #self.crc - a derived property function
        # 4 bytes - the size of the compressed data
        #self.csize - a derived property function
        # 4 bytes - the size of the decompressed data
        #self.dsize - a derived property function
        # 4 bytes - the size of the payload data
        #self.psize - a derived property function
        # N bytes - the name of the entry
        self.name = None

        #################
        # Payload
        #
        # Goes directly into the image, create from the cdata/ddata
        # N bytes - the payload data
        self.pdata = None

        ################
        # End   Format #
        ################

        #################
        # The Data
        # What it's all about!
        # N bytes - the decompressed data
        self.ddata = None
        # N bytes - the compressed data
        self.cdata = None

        #################
        # Image components
        # 8 bytes - the bytearray header core
        self.hdrc = None
        # N bytes - the bytearray header extended
        self.hdre = None
        # N bytes - the bytearray payload data
        #self.pdata = None - defined in the payload section above
        # N bytes - the fully assembled image
        #self.idata - a derived property function

        #################
        # Extended Info
        # Track where the entry starts when read from a file
        self.offset = None
        # Define the entry to a fixed size defined here
        self.fixed_size = None

    # The property functions for the derived values
    @property
    def hesize(self):
        # 20 is the number of fixed bytes in the extended header
        return self.align(20 + self.nsize)

    @property
    def nsize(self):
        return len(self.name)

    @property
    def csize(self):
        return len(self.cdata)

    @property
    def dsize(self):
        return len(self.ddata)

    @property
    def psize(self):
        return len(self.pdata)

    @property
    def crc(self):
        return zlib.crc32(self.ddata)

    @property
    def idata(self):
        return (self.hdrc + self.hdre + self.pdata)

    @property
    def isize(self):
        return len(self.idata)

    def packHdrc(self):
        '''
        Create the bytearray hdrc data that can be written
        '''
        # magic - 4 bytes
        # version - 2 bytes
        # hesize - 2 bytes
        self.hdrc = struct.pack('>IHH', self.magic, self.version, self.hesize)

        return

    def unpackHdrc(self, i_hdrc):
        '''
        Decode the bytearray hdrc data into the variables we can use
        '''
        # Save original value passed in, would get updated with a call to build
        self.hdrc = i_hdrc

        # Any @property variables are unpacked into local vars for use
        # magic - 4 bytes
        # version - 2 bytes
        # hesize - 2 bytes
        self.magic, self.version, l_hesize = struct.unpack('>IHH', i_hdrc)

        # Error checks
        if self.magic != PAK_START:
            raise ArchiveError("Unable to unpack hdrc, invalid magic: 0x%08X!" % self.magic)

        # Only 1 version for now, direct check
        if self.version != PAK_VERSION:
            raise ArchiveError("Unable to unpack hdrc, invalid version: %d!" % self.version)

        return (l_hesize)

    def packHdre(self):
        '''
        Create the bytearray hdre data that can be written
        '''
        # flags - 1 byte
        # method - 1 byte
        # nsize - 2 bytes
        # crc - 4 bytes
        # csize - 4 bytes
        # dsize - 4 bytes
        # psize - 4 bytes
        self.hdre = struct.pack('>BBHIIII', self.flags, self.method, self.nsize,
                                self.crc, self.csize, self.dsize, self.psize)
        # name - n bytes
        self.hdre += self.name.encode()
        # Add alignment data
        self.hdre += bytes(self.hesize - len(self.hdre))

        return

    def unpackHdre(self, i_hdre):
        '''
        Decode the bytearray hdrc data into the variables we can use
        '''
        # Save original value passed in, would get updated with a call to build
        self.hdre = i_hdre

        # Any @property variables are unpacked into local vars for use
        # flags - 1 byte
        # method - 1 byte
        # nsize - 2 bytes
        self.flags, self.method, l_nsize = struct.unpack('>BBH', i_hdre[0:4])
        # Force the method into the enum
        self.method = CM(self.method)

        # crc - 4 bytes
        # csize - 4 bytes
        # dsize - 4 bytes
        # psize - 4 bytes
        l_crc, l_csize, l_dsize, l_psize = struct.unpack('>IIII', i_hdre[4:20])
        # Name field
        self.name = i_hdre[20:(20 + l_nsize)].decode('utf-8')

        return (l_crc, l_csize, l_dsize, l_psize)

    def packPayload(self):
        '''
        Handle compression of the data and then create 
        the bytearray payload data that can be written
        '''
        # Set the level of zlib compression based on the method
        # Also use it as an indicator to flow through zlib code path
        zlibLevel = None
        zdata = None

        # Set the compression level based on method
        if self.method == CM.zlib:
            # For default zlib we want max compression
            zlibLevel = 9
            zdata = self.ddata
        if self.method == CM.zlib_fast:
            # In fast, pack a medium level of compression tilted towards speed
            zlibLevel = 5
            zdata = self.ddata
        if self.method == CM.zlib_ppc:
            zlibLevel = 9
            # Do an extra layer of data prep on ppc instructions
            zdata = self.powerpc_filter(self.ddata, True)

        # First try to compress the data if set
        if zlibLevel is not None:
            compressor = zlib.compressobj(level=zlibLevel, wbits=-15)
            self.cdata = compressor.compress(zdata) + compressor.flush(zlib.Z_FINISH)

            # If the compressed data is larger than the original data, just store that
            if len(self.cdata) >= len(self.ddata):
                self.method = CM.store

        # If the entry was originally store or the decompress turned it on
        if self.method == CM.store:
            self.cdata = self.ddata

        # Calculate the payload size
        # If a fixed_size is given, use that
        # Otherwise, just pad out to 8B for alignment
        if (self.fixed_size):
            # Make sure fixed_size isn't smaller than the data
            # If not checked for, the cdata would force the pdata larger and have bad downstream effects
            if (self.fixed_size < self.csize):
                raise ArchiveError("On %s, given size: 0x%X smaller than data size: 0x%X" % (self.name, self.fixed_size, self.csize))
            self.pdata = bytearray(self.fixed_size)
        else:
            self.pdata = bytearray(self.align(self.csize))

        # Insert the data
        self.pdata[0:self.csize] = self.cdata

        return

    def unpackPayload(self, i_pdata, i_crc, i_csize, i_dsize):
        '''
        Uncompress the payload data and set the proper internal variables
        '''
        # Save original value passed in, would get updated with a call to build
        self.pdata = i_pdata

        # Get out the compresssed data
        self.cdata = i_pdata[:i_csize]
        if (self.method in [CM.zlib, CM.zlib_fast, CM.zlib_ppc]):
            self.ddata = zlib.decompress(self.cdata, -15, i_csize)
        else:
            self.ddata = i_pdata[:i_dsize]

        # If zlib_ppc compressed, do an extra layer of unwinding
        if self.method == CM.zlib_ppc:
            self.ddata = self.powerpc_filter(self.ddata, False)

        # Validate the data is valid
        if zlib.crc32(self.ddata) != i_crc:
            raise ArchiveError("File %s is corrupted, CRC mismatch" % self.name)

        # If the fixed size flag was on, save that away
        if (self.flags & PAK_FLAG_FIXED_SIZE):
            self.fixed_size = len(i_pdata)

        return

    def powerpc_filter(self, i_data, i_encode):
        '''
        Optimization for ppc instruction compression
        '''
        uint32be_t = struct.Struct(">I")
        retdata = bytearray(i_data)
        for offset in range(0, len(retdata) & ~3, uint32be_t.size):
            value = uint32be_t.unpack_from(retdata, offset)[0]
            if (value & 0xFC000003) == 0x48000001:
                if i_encode:
                    value = ((value + offset) & 0x03FFFFFC) | 0x48000001
                else:
                    value = ((value - offset) & 0x03FFFFFC) | 0x48000001
                uint32be_t.pack_into(retdata, offset, value)

        return retdata

    def build(self):
        '''
        Trigger all the functions required to build a complete image to write
        Also useful to update entry state
        '''
        # Pack all the data segments
        # Do them in reverse layout order because they inform the prior data segment
        self.packPayload()
        self.packHdre()
        self.packHdrc()

        return

    def ingest(self, i_name, i_method, i_data, i_size=None, i_flags=0x0):
        '''
        Used by build from manifest to ingest the critical info for an entry
        '''
        # Set some base values from data read in and then build to fill in the rest
        self.name = i_name
        self.method = i_method
        self.ddata = i_data
        if i_size:
            self.fixed_size = i_size
            self.flags |= PAK_FLAG_FIXED_SIZE
        self.flags |= i_flags
        self.build()

        return

    def display(self):
        '''
        Display debug data about the entry
        '''
        out.print("file: %s" % self.name)
        try:
            ratio = (self.dsize/self.csize)
        except ZeroDivisionError:
            ratio = 0
        out.print("  offset: 0x%06X, size: %6d, flags: 0x%02X, hesize: %2d, nsize: %2d, method: %9s, crc: %04X, csize: %6d: dsize: %6d: ratio: %4.1fx, psize: %6d" %
                  (self.offset, self.isize, self.flags, self.hesize, self.nsize, self.method, self.crc, self.csize, self.dsize, ratio, self.psize))

    def align(self, value):
        '''
        Helper function to 8B align data
        '''
        result = value
        if (value & 0x7):
            result = value + (8 - (value & 0x7))

        return result

class Archive(UserList):
    '''
    The class representing the archive to be created
    '''
    def __init__(self, filename):
        super().__init__()
        # The file to use when load is invoked or to write to for save
        self.filename = filename
        # The contents of file from a load or the in memory image to write on save
        self.image = None

    def validate_append(self, add):
        '''
        Override list append functions to prevent duplicate addition of the same file
        When a duplicate is found, instead replace it with the new entry at the same location
        '''
        if any(add.name == x.name for x in self.data):
            self.data = [add if add.name == x.name else x for x in self.data]
        else:
            self.data.append(add)

    def append(self, add):
        '''
        Override list append and send through the validated append
        '''
        self.validate_append(add)

    def extend(self, adds):
        '''
        Override list extend and send through the validated append
        '''
        for add in adds:
            self.validate_append(add)

    def __add__(self, adds):
        '''
        Override list + operator and send through the validated append
        '''
        for add in adds:
            self.validate_append(add)
        return self

    def __iadd__(self, adds):
        '''
        Override list += operator and send through the validated append
        '''
        for add in adds:
            self.validate_append(add)
        return self

    def load(self):
        '''
        Load the given archive and process it into the ArchiveEntry classes
        '''
        # See if the input archive already exists as a file
        # If it doesn't, throw an error.. maybe make that supressable
        if not os.path.isfile(self.filename):
            out.error("The given archive file \'%s\' does not exist!" % self.filename)
            return None

        # Read the entire file into the image for reference
        with open(self.filename, "rb") as f:
            self.image = f.read()

        # Loop through bytearray image in memory and pull out the data
        offset = 0
        while (offset < len(self.image)):
            # Create the entry we are going to fill in
            entry = ArchiveEntry()
            entry.offset = offset

            # Check for the start or end marker before trying to process
            if (int.from_bytes(self.image[offset:(offset + 4)], 'big') == PAK_START):
                pass # Do nothing, this looks like a valid block that will get processed further
            elif (int.from_bytes(self.image[offset:(offset + 4)], 'big') == PAK_END):
                # Hit the end, get out of here
                break
            else:
                raise ArchiveError("Unknown magic block found at offset: %d" % offset)

            # Not at the end, process it as good
            # Unpack the hdrc block
            readsz = 8
            l_hesize = entry.unpackHdrc(self.image[offset:(offset + readsz)])
            offset += readsz

            # Unpack the hdre block
            readsz = l_hesize
            (l_crc, l_csize, l_dsize, l_psize) = entry.unpackHdre(self.image[offset:(offset + readsz)])
            offset += readsz

            # Unpack the payload
            readsz = l_psize
            entry.unpackPayload(self.image[offset:(offset + readsz)], l_crc, l_csize, l_dsize)
            offset += readsz

            # The entry is created without error, add it to the list
            self.append(entry)

        return

    def save(self, manifest=None):
        '''
        Write out the ArchiveEntry information into a useable archive
        '''
        # If a manifest is not passed in, create an empty one
        # This gets us the defaults for manifest globals
        if (not manifest):
            manifest = Manifest()

        # Build all the contents for each entry before we try to write it
        for entry in self:
            entry.build()

        # Build the full image from all the parts
        self.image = bytearray()
        offset = 0
        for entry in self:
            entry.offset = offset
            self.image += entry.idata
            offset += entry.isize

        # Add the end marker if requested
        if manifest.end_marker:
            # First 4 bytes are the end magic
            self.image += PAK_END.to_bytes(4, 'big')
            # Last 4 bytes are the total length of the archive
            self.image += (len(self.image) + 4).to_bytes(4, 'big')

        # We've got the image, write it out
        with open(self.filename, "wb") as f:
            f.write(self.image)

##############################
# All manifest related info
##############################
class Manifest(object):
    '''
    Store the syntax validated version of the input manifest dict
    '''
    def __init__(self):
        # The compression level
        self.method = CM.zlib
        # Add in the end marker
        self.end_marker = True
        # The list of layout entries
        self.layouts = list()

    def to_dict(self):
        '''
        First attempt at function to turn the manifest class back into a dictionary
        Would be useful to write out the fully resolved manifest that matches the create archive
        It works, but needs some major work
        '''
        # Main dict and top sections
        d = dict()
        d["global"] = dict()
        d["layout"] = dict()

        # Load in each sub section
        # Globals
        if self.method != None:
            d["global"]["method"] = self.method.name
        if self.end_marker != None:
            d["global"]["end-marker"] = self.end_marker

        # Layout
        for layout in self.layouts:
            d["layout"][layout.name] = dict()
            if layout.file != None:
                d["layout"][layout.name]["file"] = layout.file
            if layout.directory != None:
                d["layout"][layout.name]["directory"] = layout.directory
            if layout.pattern != None:
                d["layout"][layout.name]["pattern"] = layout.pattern
            if layout.method != None:
                d["layout"][layout.name]["method"] = layout.method.name
            if layout.location != None:
                d["layout"][layout.name]["location"] = layout.location
            if layout.size != None:
                d["layout"][layout.name]["size"] = layout.size

        return d

    def parse(self, manifestFile, basepath=None):
        '''
        Opens, parses and fully validates a .manifest file
        '''
        errors = 0

        # Open the file, parse it and eval it into the dictionary 'data'
        # Any error is a hard return of the program as we at least need a valid data struct
        with open(manifestFile, "r") as f:
            out.print("Validating manifest syntax")
            # First make sure we can parse the manifest - proper braces, commas, etc..
            try:
                parsedata = ast.parse(f.read(), mode="eval")
            except SyntaxError as e:
                out.critical("Syntax error parsing manifest, line %d. Entry: '%s'" % (e.lineno, e.text.strip()))
                out.critical("Hint - The actual error may be on an earlier line.")
                return 1

            # Now validate all the values are at least valid python expression
            # For example, True and not a typo of Tru
            try:
                data = ast.literal_eval(parsedata)
            except ValueError as e:
                # Get the error data, both the message and location
                exc_type, exc_value, exc_traceback = sys.exc_info()
                # First the error message
                out.critical("%s" % (exc_value))
                # Now get the line number and position
                last_tb = exc_traceback
                while last_tb.tb_next:
                    last_tb = last_tb.tb_next
                out.critical("Found at location: line=%d, col=%d" % (last_tb.tb_frame.f_locals["node"].lineno,
                                                                     last_tb.tb_frame.f_locals["node"].col_offset))
                return 1

        ################################################
        # Validate the values read in

        # Instead of just getting the values we need, instead loop through all values read in
        # This will allow us to check for anything invalid added by the user
        out.print("Validating 'global' keyword values")
        for key, value in data["global"].items():
            if (key == "method"):
                self.method = CM[value]
            elif (key == "end-marker"):
                self.end_marker = value
            else:
                out.error("global keyword '%s' not known!" % key)
                errors += 1

        # Now validate all the layout keywords in the same method
        out.print("Validating 'layout' keyword values")
        for key, value in data["layout"].items():
            entry = ManifestLayout()
            entry.name = key
            # Set the default from the manifest
            entry.method = self.method

            # Make sure every key given for the layout is valild
            for subkey, subvalue in value.items():
                if (subkey == "method"):
                    entry.method = CM[subvalue]
                elif (subkey == "file"):
                    if (basepath):
                        # Create the fully qualified file name
                        entry.file = os.path.join(basepath, subvalue)
                    else:
                        entry.file = subvalue
                elif (subkey == "directory"):
                    if (basepath):
                        # Create the fully qualified director name
                        entry.directory = os.path.join(basepath, subvalue)
                    else:
                        entry.file = subvalue
                elif (subkey == "pattern"):
                    entry.pattern = subvalue
                elif (subkey == "location"):
                    entry.location = subvalue
                elif (subkey == "size"):
                    entry.size = subvalue
                else:
                    out.error("layout keyword '%s' in file '%s' not known!" % (subkey, key))
                    errors += 1

            # Done with our key validation, now make sure we got the right info to build an entry
            if (entry.file and entry.directory):
                out.error("Found both a \'file\' and \'directory\' entry for layout '%s'" % key)
                errors += 1
                continue

            if (not (entry.file or entry.directory)):
                out.error("Found neither a \'file\' or \'directory\' entry for layout '%s'" % key)
                errors += 1
                continue

            if (entry.directory and not entry.pattern):
                out.error("A keyword 'pattern' required to be used with a directory entry")
                errors += 1
                continue

            # Stick the class into the list
            self.layouts.append(entry)

        # All done, return the total count of parsing errors encountered
        return errors

class ManifestLayout(object):
    '''
    Store the manifest dict entries after processing/validation
    '''
    def __init__(self):
        # The name to use in the archive
        self.name = None
        # The file to insert into the archive (maybe actually the contents??)
        self.file = None
        # The directory and pattern to look for
        self.directory = None
        self.pattern = None
        # The method of compression
        self.method = None
        # The fixed location in the archive the data should be placed
        self.location = None
        # The fixed size in the archive
        self.size = None
