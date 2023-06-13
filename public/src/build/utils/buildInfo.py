#!/usr/bin/env python3
# IBM_PROLOG_BEGIN_TAG
# This is an automatically generated prolog.
#
# $Source: public/src/build/utils/buildInfo.py $
#
# OpenPOWER sbe Project
#
# Contributors Listed Below - COPYRIGHT 2016,2023
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

# This script will create header file sbe_build_info.H which will have
# buld information required by SBE code.
import os
import sys
from subprocess import Popen, PIPE
import datetime

def buildInfo():
    buildInfoFileName = sys.argv[1]+"/sbe_build_info.H"
    header = \
"#ifndef SBE_BUILD_INFO_H  \n\
#define SBE_BUILD_INFO_H  \n\n"

    footer = "\n#endif  // SBE_BUILD_INFO_H"

    buildTime = "0x" + datetime.datetime.now().strftime('%Y%m%d')
    hexTime   = int(buildTime, 16)

    commitStr = os.popen('git rev-parse --short=8 HEAD').read().rstrip()
    try:
        commitInt = int(commitStr, 16)
    except:
        print("Failed to get a valid SBE_COMMIT_ID")
        sys.exit(1)

    ## Get SBE tag corresponding to commit_id it got included first after merge.
    ## git describe command can be used to find the tag that comes after the commit.
    ## The command finds the most recent tag that is reachable from a commit. If the
    ## tag points to the commit then only the tag is returned. Otherwise, it suffixes (~)
    ## the tag name with the additional commits on top of the tagged object
    buildTag = ""
    p = Popen(["git", 'describe', '--tags', '--contains', 'commitStr'],\
              stdout=PIPE, stderr=PIPE, universal_newlines=True)
    output, error = p.communicate()
    if p.returncode == 0:
        if len(output) > 0:
            separator = '~'
            buildTag = output.split(separator, 1)[0]
            print ("Tag found: " + buildTag)
    else:
        print("Failed to get a tag" + str(p.returncode) + str(error))

    f = open( buildInfoFileName, 'w')

    f.write(header)
    f.write("//Define SBE Commit ID \n")
    f.write("#define SBE_COMMIT_ID " + hex(commitInt) + "\n")
    f.write("//Define SBE BUILD_TIME \n")
    f.write("#define SBE_BUILD_TIME " + hex(hexTime) + "\n")
    f.write("//Define SBE BUILD_TAG \n")
    f.write("#define SBE_BUILD_TAG " + "\"" + buildTag + "\"" + "\n")
    f.write(footer)
    f.close()

# Call buildInfo
buildInfo()

