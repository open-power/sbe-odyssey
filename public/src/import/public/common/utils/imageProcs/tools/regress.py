#!/usr/bin/env python3
# IBM_PROLOG_BEGIN_TAG
# This is an automatically generated prolog.
#
# $Source: public/src/import/public/common/utils/imageProcs/tools/regress.py $
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
# Python provided
import sys
import os
import tempfile
import random
 
# Add program modules to the path
sys.path.append(os.path.join(os.path.dirname(os.path.realpath(__file__)), "pymod"))
from output import out
import pakcore as pak

############################################################
# About - About - About - About - About - About - About
############################################################
'''
This is a regression script designed to validate changes to the
python module used to create and manage pak files.

The cmdline tools cover a lot of the code paths, but not all of them.
Especially with teams like DFT directly consuming the python code and
making API like calls to manipulate the archive.

This script should test those code flows to make sure nothing breaks
over the course of updates.

This currently does not test the manifest API paths, but could be updated
to do so.  The biggest 'hurdle' is having the regression test write 
out all the required files a manifest would require, then cleanu up after.
At least the use of tempfile could make cleanup automatic
'''

# Some globals related to tempfiles
# Useful to leave the files after the run for debug
tempFileDelete = True
tempFilePath = '/tmp/'
tempFilePrefix = 'pakr_'
banner = "\n########"

def regress():
    # Create the temporary file we will use
    # The archive will not directly use this tempfile object
    # Instead it will use the name that is created for the file name on disk
    # That way when the program exits, the file will be cleaned up 
    # Also of note: The way archive.save works now is it uses `with open() as f`
    # That automatically calls close after the write, deleting the file before
    # the program even ends.
    archiveFile = tempfile.NamedTemporaryFile(dir=tempFilePath,
                                              prefix=tempFilePrefix,
                                              suffix=".pak",
                                              delete=tempFileDelete)


    # Create an array of random 8 byte values
    # Use those to populate a file entry
    # Intended to walk the line between all zeros and full random
    randarray = list()
    for x in range(64):
        randarray.append(os.urandom(8))

    # Create some files in memory
    files = dict()
    files['dir1/smallfile'] = (pak.CM.zlib, bytearray(os.urandom(1024)))
    files['dir1/largefile'] = (pak.CM.zlib, bytearray(os.urandom(50000)))
    files['dir2/smallfile'] = (pak.CM.zlib, bytearray(os.urandom(2048)))
    files['dir2/largefile'] = (pak.CM.zlib, bytearray(os.urandom(150000)))
    files['PAD-01'] = 32768
    files['top-store'] = (pak.CM.store, bytearray(50000))
    files['top-zlib'] = (pak.CM.zlib, bytearray(5000))
    files['feedb0b0'] = (pak.CM.zlib, bytearray(b'FEEDB0B0' * 1000))
    files['randarray'] = (pak.CM.zlib, b''.join([randarray[random.randrange(0,len(randarray))] for x in range(5000)]))
    expectLength = len(files)

    # Create our archive, using the name of the temp file
    out.print(banner)
    out.print("Creating %s" % archiveFile.name)
    archive = pak.Archive(archiveFile.name)
    out.print("Archive length: %d" % len(archive))

    # Test adding files to the archive
    out.print(banner)
    out.print("Adding files..")
    out.moreIndent()
    # Test adding files
    for file in files:
        out.print(file)
        if file.startswith('PAD'):
            archive.addPad(files[file])
        else:
            archive.add(file, files[file][0], files[file][1])
    out.lessIndent()

    # Validate the results
    out.print("Archive length: %d" % len(archive))
    if (len(archive) != expectLength):
        raise pak.ArchiveError("Archive length mismatch: %d != %d " % (len(archive), expectLength))

    # Print a listing of the files in the archive
    out.print(banner)
    out.print("Listing files..")
    for file in archive:
        file.display()

    # Test extraction of file
    out.print(banner)
    out.print("Extracting and comparing files..")
    out.moreIndent()
    for file in files:
        # Don't do if pad
        if not file.startswith('PAD'):
            out.print(file)
            if files[file][1] != archive.extract(file):
                raise pak.ArchiveError("Data mismatch on %s" % file)
    out.lessIndent()

    # Test extraction of missing file
    out.print(banner)
    out.print("Extracting file that doesn't exist")
    try:
        archive.extract('nofile')
    except:
        # We know this fails
        out.print("Raised expected exception!")
    else:
        raise pak.ArchiveError("Exception not raised for extracting unknown file!")

    # Test extraction of multiple files
    out.print(banner)
    out.print("Extracting multiple files")
    try:
        archive.extract('dir1')
    except:
        # We know this fails
        out.print("Raised expected exception!")
    else:
        raise pak.ArchiveError("Exception not raised for extracting multiple files!")

    # Test remove of a file that doesn't exist
    out.print(banner)
    out.print("Removing file that doesn't exist")
    try:
        archive.remove('nofile')
    except:
        # We know this fails
        out.print("Raised expected exception!")
    else:
        raise pak.ArchiveError("Exception not raised for deleting unknown file!")

    # Now remove a file it knows
    out.print(banner)
    out.print("Removing file that does exist")
    # This should work, so if it throws we want it to fail
    try:
        archive.remove('top-store')
    except:
        # This shouldn't fail
        raise pak.ArchiveError("Exception raised for deleting known file!")
    else:
        out.print("File deleted successfully!")
    out.print("Archive length: %d" % len(archive))
    expectLength -= 1 # Succesfully removed a file
    if (len(archive) != expectLength):
        raise pak.ArchiveError("Archive length mismatch: %d != %d " % (len(archive), expectLength))

    # Finally, try and save the file we created
    # This will delete the tempfile at the end of the save
    # This is due to the save doing `with open() as f`, which automatically does the close()
    # If we wanted to do more work with the actual file after the save, need to modify the save
    archive.save()

try:
    regress()
except pak.ArchiveError as e:
    out.critical(str(e))
    # Make this better and get the line number to print
    # It will be the line of the raise and not the actual check though
    #print(traceback.format_exc())
    sys.exit(1)
else:
    out.print(banner)
    out.print("ALL PASSED!")
    out.print(banner[1:]) # Rip the \n off the front of banner
