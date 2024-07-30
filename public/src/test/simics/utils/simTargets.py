#!/usr/bin/env python3
# IBM_PROLOG_BEGIN_TAG
# This is an automatically generated prolog.
#
# $Source: public/src/test/simics/utils/simTargets.py $
#
# OpenPOWER sbe Project
#
# Contributors Listed Below - COPYRIGHT 2023,2024
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

"""This modules helps to get the simics objects for the targets.

- This module helps to collect all targets simics objects from the simics
  based on the class name and use it in the test case framework based on the
  needs.
"""

from sim_commands import *

def getTargetInstanceNum(target):
    """Used to get the instance number of the given target.

    This function used to get the target instance number that will be
    available in the simics object name.

    Args:
        target (simics class type) : Used to pass the target to get instance
                                     number.

    Returns:
        Return the instance number of the given target from its name.
    """

    # FIXME: This is logic is not correct to get the instance number from
    # the object name because if the name contains "digits" in unexpected place
    # then we will consider that also as a instance number.
    # For example, backplane2.dimm[10].odyssey => "210" but, actually instance
    # number is "10" so we need some other member from the simics object.
    instanceNum = int(''.join(filter(lambda c: c.isdigit(), target.name)))
    return instanceNum

# Get all targets objects that loaded by the simics
targetsObjs = SIM_get_all_objects()

# Collect all below objects from the targetsObjs list to use in the test case
# framework
odysseys = []
dimms = []
for targetsObj in targetsObjs:
    # Currently, we are looking for only odyssey dimm model so using
    # the "odyssey_cmp" class to get odyssey dimm objects.
    if targetsObj.classname == "odyssey_cmp":
        odysseys.append(targetsObj)
    if ((targetsObj.classname == "ddimm_cmp") or
        (targetsObj.classname == "ddimm_ody")):
        dimms.append(targetsObj)

# Sort always the retrieved objects to use the object by using the instance
# number.
odysseys.sort(key=getTargetInstanceNum)
dimms.sort(key=getTargetInstanceNum)