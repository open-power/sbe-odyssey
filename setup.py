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
from setuptools.command.build_py import build_py

package_directories = {
    # User Data packages.
    # Component package names must be in the form of: `udparsers.xyyyy`.
    # Where 'x' is PEL creator system, for example 'o' for the BMC and
    # 'b' for the Hostboot. 'yyyy' is the 4 digit component ID (lowercase).
    # Refer: https://github.com/openbmc/phosphor-logging/tree/master/extensions/openpower-pels#adding-python3-modules-for-pel-userdata-and-src-parsing

    "udparsers.o4500":          "public/src/tools/debug/plugins/ebmc/",
    "udparsers.poztraceutils":  "public/src/tools/trace/",
    "udparsers.pozdebugutils": "public/src/tools/debug/"
}
# TODO: add golden image stringfiles
# currently need to run simics and then untar and find the gldnstringfile
custom_data_files = [
                        ('ody_data',['images/odyssey/runtime/sppe/odysseySppeStringFile_DD1',])
                    ]

def check_environment_files():
    """
    Check the environment for the needed files

    SBE setup.py is invoked in two contexts:
    1 - op-build, where the images file exists, post build
    2 - OpenBMC, where the images file does NOT exist
        OpenBMC clones a clean SBE PPE repo (source only)

    setup.py will fail if data_files do not exist,
    so if we encounter a missing file, clear the
    expectation and only populate the wheel with
    the usual python source files.
    """
    for i in custom_data_files:
        for x in i[1]:
            if not os.path.isfile(x):
                custom_data_files.clear()
                return

class BuildCommand(build_py):
    """
    Subclass the build_py command

    This allows the capability to add custom build
    steps.
    """
    def run(self):
        # First run the regular build_py
        build_py.run(self)
        # Now run the custom step we need
        check_environment_files()

setup(
    name          = "poz-pel-parser",
    cmdclass      = {"build_py":BuildCommand},
    version       = os.getenv('PELTOOL_VERSION', '0.1'),
    packages      = package_directories.keys(),
    data_files    = custom_data_files,
    package_dir   = package_directories,
)
