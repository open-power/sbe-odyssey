#!/usr/bin/env python2
# IBM_PROLOG_BEGIN_TAG
# This is an automatically generated prolog.
#
# $Source: public/src/import/public/systems/common/hdct/buildhdct.py $
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

##############################################################
#    @file     buildiHDCTBin.py
#    @brief    Main Module to build HDCT Binary for SBE
#              This tool will be used in hcode makefile to add
#              hdct binary into hcode image.
#
#    @author   Sunil Kumar
#
#    @date     Apr 11, 2020
#
#    ----------------------------------------------------
#    @version  Developer      Date       Description
#    -----------------------------------------------------
#      1.0     skumar8j     11/04/20     Initial create
###############################################################

import os, sys, glob

#-------------------------
#Global Var for reference
#-------------------------

#-------------------------
# Main Functions
#-------------------------

def main():

    #Get the latest commit id.
    gitCommand = "git rev-parse HEAD"
    commitId = os.popen(gitCommand).read()

    # Accessing the command-line argument
    if len(sys.argv) > 1:
        arg = sys.argv[1]
    else:
        # Default value as odyssey
        arg = "ody"
    arg = arg[:3]

    buildCmd = os.environ['PATH_PYTHON3'] + " " + "$PROJECT_ROOT/public/systems/common/hdct/hdctbin.py -j $PROJECT_ROOT/public/systems/common/hdct/"+arg+"/config.json -o $PROJECT_ROOT/output/images/$ECMD_ARCH/ -t "+arg+" -c critical -l critical -e " + commitId
    rc = os.system( buildCmd )

    if rc:
        sys.exit("Failed to execute buildhdctbin.py\n")

    verifyCmd = os.environ['PATH_PYTHON3'] + " " + "$PROJECT_ROOT/public/systems/common/hdct/verifyhdct.py " + arg
    rc = os.system( verifyCmd )

    if rc:
        sys.exit("Failed to verify HDCT.bin\n")

main()
