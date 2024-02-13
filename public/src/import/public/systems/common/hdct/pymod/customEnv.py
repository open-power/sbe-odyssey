# IBM_PROLOG_BEGIN_TAG
# This is an automatically generated prolog.
#
# $Source: public/src/import/public/systems/common/hdct/pymod/customEnv.py $
#
# OpenPOWER sbe Project
#
# Contributors Listed Below - COPYRIGHT 2023,2024
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
#
# Python module to copy stats file
# It also register the system into stela DB
#

############################################################
# Imports - Imports - Imports - Imports - Imports - Imports
############################################################
import os
import sys
import time
import datetime
import subprocess
import shutil
from output import out
import gzip
import bz2

############################################################
# Function - Functions - Functions - Functions - Functions
############################################################

###########################
# utility functions
###########################
def copyStatsFile(outputDir, statsFile, creator, timeStamp, product, dllProductType, systemName):
    """
    Copy the stats file from output dir to remote destination on gsa
    """

    # Create the path to save to
    gsaPath = '/gsa/rchgsa/projects/s/smartdump/raw-' + creator + "/" + product

    # When we copy the file over, we want to save it with the timestamp in the name
    # This is in place of the user selected suffix.  The code that processes the
    # data files counts on the date being in the file name
    savePath = os.path.join(gsaPath, systemName)
    saveFile = os.path.join(savePath, ("%s-%s.stats" % (creator, timeStamp)))
    if (statsFile.endswith(".bz2")):
        saveFile += ".bz2"

    # Now see if the directory exists - if not, create it
    if not os.path.exists(savePath):
        try:
            os.makedirs(savePath, exist_ok=True)
            os.chmod(savePath, 0o777)
        except Exception as e:
            out.error("==== Exception ====")
            out.error(format(e))
            out.error("Unable to create the directory: %s" % savePath)
            return # Bail because of fail

    # Get the path for the stats file to copy
    statsFilePath = os.path.join(outputDir, statsFile)
    if not os.path.isfile(statsFilePath):
        # try the filename directly.
        if os.path.isfile(statsFile):
            statsFilePath = statsFile
        else:
            out.error("Unable to find the statsFile: {}".format(statsFile))
            return  # bail because of fail

    # Copy the file from output dir to gsa dir for stela to pick up
    try:
        shutil.copy(statsFilePath, saveFile)
        os.chmod(saveFile, 0o666)
    except Exception as e:
        out.error("==== Exception ====")
        out.error(format(e))
        out.error("Unable to save the stats: %s" % saveFile)
        return # Bail because of fail

    # Log where we put it
    out.info("stats file saved: %s" % saveFile)

def copyHdctFile(filenames, generation, hashType, hashValue, saveLookup):
    """
    This function can be used to copy the hdct file to the save location
    This will also save the lookup file if it was generated

    # filenames  -- The dict of filenames
    # generation -- The product generation we are on
    # hashType   -- The type of hash value we have
    # hashValue  -- The actual hash value
    # saveLookup -- Did we also generate the SBE data
    """

    # Get the base path to save to
    gsaPath = '/gsa/rchgsa/projects/s/smartdump/hdct-save/' + generation

    # For the ekb, all firmware stores is the first 16 characters of the hash
    # That is all we need for the dir hashvalue
    hashDir = hashValue

    # Add the type and value to the gsaPath
    gsaPath = os.path.join(gsaPath, hashType, hashDir[0], hashDir)

    # Now see if the directory exists - if not, create it and copy files over
    # If it exists, assume it has all the data it needs and don't do anything
    # This saves us from permission problems, etc.. if one user tries to copy over another
    if not os.path.exists(gsaPath):
        try:
            os.makedirs(gsaPath, exist_ok=True)
            os.chmod(gsaPath, 0o777)
        except Exception as e:
            out.error("==== Exception ====")
            out.error(format(e))
            out.error("Unable to create the directory: %s" % gsaPath)
            return # Bail because of fail

        # The hdct file to save
        # If already compressed, copy over as is
        # If not, compress and copy
        hdctSave = os.path.join(gsaPath, "hdct.bz2")
        if (filenames["hdct"].endswith(".bz2")):
            try:
                shutil.copy(filenames["hdct"], hdctSave)
                os.chmod(hdctSave, 0o666)
            except Exception as e:
                out.error("==== Exception ====")
                out.error(format(e))
                out.error("Unable to save the file: %s" % hdctSave)
                return # Bail because of fail
        else:
            try:
                with open(filenames['hdct'], 'rb') as src, bz2.open(hdctSave, 'wb') as dst:
                    dst.writelines(src)
                os.chmod(hdctSave, 0o666)
            except Exception as e:
                out.error("==== Exception ====")
                out.error(format(e))
                out.error("Unable to save the file: %s" % hdctSave)
                return # Bail because of fail

        out.info("hdct file saved: %s" % hdctSave)

        # We only do this if passed the save lookup indication
        if (saveLookup):
            # The lookup file to save
            lookupSave = os.path.join(gsaPath, "lookup")

            try:
                # Copy the file from output dir to gsa dir for save
                shutil.copy(filenames["lookup"], lookupSave)
                os.chmod(lookupSave, 0o666)
            except Exception as e:
                out.error("==== Exception ====")
                out.error(format(e))
                out.error("Unable to save the file: %s" % lookupSave)
                return # Bail because of fail

            # Log where we put it
            out.info("lookup file saved: %s" % lookupSave)

def makeOutputDir():
    """
    This function can be used to create the log fail directory that is neither
    default nor the default, the location can be derived or customized to a
    specific environment
    """

    # Set the default path to None
    outputPath = None

    # Check if we are in lab lcb environment
    if ((os.getenv("LABTS")) and (os.getenv("LABCPU"))):
        # Check if mklogfaildir is being called by BTF which will set LI_LOGDIR
        outputPath = os.getenv("LI_LOGDIR")
        if (not outputPath):
            # Grab the script to be invoked
            mklogDir = os.path.join( os.getenv("CTEPATH"), "tools", "gsisymlink", "mklogfaildir")
            # Invoke the script to create the dir to log stuff out
            logDir = subprocess.Popen([mklogDir,  os.getenv("ECMD_TARGET")], stdout=subprocess.PIPE,  universal_newlines=True)
            # Read the line from the stdout of subprocess and strip all the white spaces
            outputPath = logDir.stdout.readline().rstrip()

    return outputPath
