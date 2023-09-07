# IBM_PROLOG_BEGIN_TAG
# This is an automatically generated prolog.
#
# $Source: setup.py $
#
# OpenPOWER sbe Project
#
# Contributors Listed Below - COPYRIGHT 2021,2024
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

import os.path
import os

"""
 Setuptools is an open source package to distribute python modules.
 Documentation on setuptools can be found on the web.

 This particular setup tool used in the OpenBMC to pack as part of the BMC image
 to parse the SBE FFDC that will be added in the PEL user data section
 for the SBE failure.
"""

from setuptools import setup

package_directories = {
    # User Data packages.
    # Component package names must be in the form of: `udparsers.xyyyy`.
    # Where 'x' is PEL creator system, for example 'o' for the BMC and
    # 'b' for the Hostboot. 'yyyy' is the 4 digit component ID (lowercase).
    # Refer: https://github.com/openbmc/phosphor-logging/tree/master/extensions/openpower-pels#adding-python3-modules-for-pel-userdata-and-src-parsing

    "udparsers.o4500":          "public/src/tools/debug/plugins/ebmc/",
    "udparsers.poztraceutils":  "public/src/tools/trace/",
    "udpparsers.pozdebugutils": "public/src/tools/debug/"
}

setup(
    name          = "poz-pel-parser",
    version       = os.getenv('PELTOOL_VERSION', '0.1'),
    packages      = package_directories.keys(),
    package_dir   = package_directories,
)
