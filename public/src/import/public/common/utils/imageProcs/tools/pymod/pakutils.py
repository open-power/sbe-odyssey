# IBM_PROLOG_BEGIN_TAG
# This is an automatically generated prolog.
#
# $Source: public/src/import/public/common/utils/imageProcs/tools/pymod/pakutils.py $
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
# Jason Albert - created 02/02/2022
# Python module to define common utility functions

############################################################
# Imports - Imports - Imports - Imports - Imports - Imports
############################################################
import datetime
from pakconstants import out

############################################################
# Function - Functions - Functions - Functions - Functions
############################################################
def formatTime(timePassed, fraction=True):
    '''
    Handles time formatting in common function
    '''
    # The time comes out as 0:00:45.3482..
    # We find the break from the full seconds to the fractional seconds
    timeString = str(datetime.timedelta(seconds=timePassed))
    decIdx = timeString.find(".")

    if (fraction):
        # The second half of this is a bit of a mess
        # Convert the decimal string to a float, then round it to two places, then turn it back into a string
        # It has to be a formatted string conversion, a simple str() would turn .10 into .1.  Then remove the "0."
        return timeString[0:decIdx] + ("%.2f" % round(float(timeString[decIdx:]), 2))[1:]
    else:
        return timeString[0:decIdx]

# This is a modified version of code here
# https://stackoverflow.com/questions/12523586/python-format-size-application-converting-b-to-kb-mb-gb-tb
def humanBytes(B):
    '''
    Return the given bytes as a human friendly KB, MB, GB, or TB string
    '''
    B = int(B)
    KB = float(1024)
    MB = float(KB ** 2) # 1,048,576
    GB = float(KB ** 3) # 1,073,741,824
    TB = float(KB ** 4) # 1,099,511,627,776

    if B < KB:
        return '{0}B'.format(B)
    elif KB <= B < MB:
        return '{0:.2f}K'.format(B/KB)
    elif MB <= B < GB:
        return '{0:.2f}M'.format(B/MB)
    elif GB <= B < TB:
        return '{0:.2f}G'.format(B/GB)
    elif TB <= B:
        return '{0:.2f}T'.format(B/TB)
