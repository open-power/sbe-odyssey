# IBM_PROLOG_BEGIN_TAG
# This is an automatically generated prolog.
#
# $Source: public/src/test/simics/utils/sbeIstepAuto.py $
#
# OpenPOWER sbe Project
#
# Contributors Listed Below - COPYRIGHT 2016,2024
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
import sys
import copy
from sim_commands import *

import sbeSimUtils

EXPDATA = [0xc0,0xde,0xa1,0x01,
           0x0,0x0,0x0,0x0,
           0x00,0x0,0x0,0x03];
gIstepArray = {
        1:[13, 23], # istep 1.13 to 1.23
        2:[],       # no istep 2 in odyssey
        3:[1, 24],  # istep 3.1 to 3.24
               }
# MAIN Test Run Starts Here...
#-------------------------------------------------
def sbe_istep_func( inum1:str, inum2:str, proc=0, node=0):
    # Interpretation:
    # (Single iStep)    inum1=2 inum2=2     => startMajor = 2; startMinor = 2;
    #                                          endMajor = 0; endMinor = 0;
    # (Range of iSteps) inum1=2.3 inum2=3.7 => startMajor = 2; startMinor = 3;
    #                                          endMajor = 3; endMinor = 7;

    # remove '' around the string, which might be present when this function
    #   function is called from ci script.
    inum1 = inum1.strip('\'')
    inum2 = inum2.strip('\'')

    # check whether input strings are valid float number or not
    try:
        float(inum1)
        float(inum2)
    except ValueError:
        print("Invalid argument ", inum1, inum2)
        raise

    # if input is a integer (which will be the case if only one isetep is requested)
    #       then append another 0 to work succeeding logic.
    inum1Split = inum1.split('.')
    if(len(inum1Split) == 1):
        inum1Split.append("0")

    inum2Split = inum2.split('.')
    if(len(inum2Split) == 1):
        inum2Split.append("0")

    startMajor = 0
    startMinor = 0
    endMajor = 0
    endMinor = 0
    # FIFO type will be 0 by default.
    i_fifoType = 0
    startMajor = int(inum1Split[0])
    # Check for single istep condition, where inum1=x.0 and inum2=x.0
    # If not extract the start and end isteps range
    if(int(inum1Split[1]) != 0):
        startMinor = int(inum1Split[1])
    if(int(inum2Split[1]) != 0):
        endMajor = int(inum2Split[0])
        endMinor = int(inum2Split[1])
    else:
        startMinor = int(inum2Split[0])

    # Make sure array is a deep copy,
    # as we are modifying and setting up the local array
    # based on ranges requested
    lIstepArray = copy.deepcopy(gIstepArray)
    lIstepArray[startMajor][0] = startMinor
    if endMajor != 0:
        lIstepArray[endMajor][1] = endMinor
    else:
        endMajor = startMajor
        lIstepArray[startMajor][1] = lIstepArray[startMajor][0]
    for major in range(startMajor, endMajor+1):

        # We may have some hole in istep major number.
        #       like no minor istep in istep-2 of odyssey
        if(len(lIstepArray[major]) == 0):
            continue

        for minor in range(lIstepArray[major][0], lIstepArray[major][1] + 1):
            print("Running:"+str(major)+"."+str(minor))

            try:
                TESTDATA = [0,0,0,3,
                             0,0,0xA1,0x01,
                            0,major,0,minor ]
                sbeSimUtils.runCycles( 10000000 )
                sbeSimUtils.writeUsFifo( TESTDATA, i_fifoType, node, proc)
                sbeSimUtils.writeEot( i_fifoType, node, proc)
                sbeSimUtils.runCycles( 1000000 )
                sbeSimUtils.readDsFifo( EXPDATA, i_fifoType, node, proc)
                sbeSimUtils.runCycles( 1000000 )
                sbeSimUtils.readEot( i_fifoType, node, proc)

            except:
                print ("\nTest completed with error(s). Raise error")
                sbeSimUtils.collectFFDC()
                # TODO via RTC 142706
                # Currently simics commands created using hooks always return
                # success. Need to check from simics command a way to return
                # Calling non existant command to return failure
                run_command("Command Failed");
                raise
    print ("\nTest completed with no errors")
        #sys.exit(0);
