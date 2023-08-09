#!/usr/bin/env python2
# IBM_PROLOG_BEGIN_TAG
# This is an automatically generated prolog.
#
# $Source: public/src/import/public/systems/common/hdct/verifyhdct.py $
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
# Program to verify the HDCT binary for SBE

##############################################################

import os, sys, glob

#-------------------------
#Global Var for reference
#-------------------------

#-------------------------
# Main Function
#-------------------------

def main():

    # Get the latest commit id.
    gitCommand = "git rev-parse HEAD"
    commitId = os.popen(gitCommand).read()

    # Accessing the command-line argument
    if len(sys.argv) > 1:
        arg = sys.argv[1]
    else:
        # Default value as odyssey
        arg = "ody"

    if not (arg == 'odyssey' or arg == 'p11'):
        print("Target is not supporting, exiting...")
        sys.exit(1)

    # Verify HDCT.bin
    cmd = os.environ['PATH_PYTHON3'] + " " + "$PROJECT_ROOT/public/systems/common/hdct/hdcttxt.py -f $PROJECT_ROOT/output/images/$ECMD_ARCH/hdct_"+arg+"_10/hdct_"+arg+"_10.bin -o $PROJECT_ROOT/output/images/$ECMD_ARCH/ -s $PROJECT_ROOT/output/images/$ECMD_ARCH/hdct_"+arg+"_common/hdct.lookup -r $PROJECT_ROOT/public/systems/p11/HDCT.txt -g "+arg+" -c critical -l critical -e " + commitId
    rc = os.system( cmd )

    if rc:
        sys.exit("HDCT.bin verification failed")

main()
