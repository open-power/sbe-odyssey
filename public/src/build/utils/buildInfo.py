#!/usr/bin/env python3
# IBM_PROLOG_BEGIN_TAG
# This is an automatically generated prolog.
#
# $Source: public/src/build/utils/buildInfo.py $
#
# OpenPOWER sbe Project
#
# Contributors Listed Below - COPYRIGHT 2016,2024
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
import datetime
import struct

buildDate = "0x" + datetime.datetime.now().strftime('%Y%m%d')
hexDate   = int(buildDate, 16)

# Change to the directory where this script is located since that's
# guaranteed to be inside the git repo, so all git commands will
# query the right repo.
os.chdir(os.path.dirname(__file__))

commitStr = os.popen('git rev-parse --short=8 HEAD').read().rstrip()
try:
    commitInt = int(commitStr, 16)
except:
    print("Failed to get a valid SBE_COMMIT_ID", file=sys.stderr)
    sys.exit(1)

## sbeBuildInfo.bin format: [<commit-Id><build-date><build-tag>]
f = open( sys.argv[1], 'wb' )
bin_commit = struct.pack("!L", commitInt)
bin_date   = struct.pack("!i", hexDate)
f.write(bin_commit)
f.write(bin_date)

tags = []
for proj in ["ody", "zme", "pst"]:
    proc = os.popen('git describe --tags --dirty --long --always --match "sbe*_%s.*"' % proj)
    tags.append((proj, proc.read().strip()))
    rc = proc.close()
    if rc:
        print("Failed to get an SBE tag for " + proj, file=sys.stderr)
        f.close()
        sys.exit(1)

print("#pragma once\n")
print("//Define SBE Commit ID")
print("#define SBE_COMMIT_ID " + hex(commitInt))
print("//Define SBE BUILD_DATE")
print("#define SBE_BUILD_DATE " + hex(hexDate))
for proj, tag in tags:
    print('#define SBE_BUILD_TAG_%s "%s"' % (proj.upper(), tag))
    if proj == "ody":
        int_tag = bytes(tag.encode())
        bin_tag = struct.pack("!20s", int_tag)
        f.write(bin_tag)

f.close()
