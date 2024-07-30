# IBM_PROLOG_BEGIN_TAG
# This is an automatically generated prolog.
#
# $Source: public/src/test/simics/utils/istepArray.py $
#
# OpenPOWER sbe Project
#
# Contributors Listed Below - COPYRIGHT 2024
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
from sim_commands import *
import simTargets

istepArray = {}

def getIstepArray():
    if(simenv.sbe_project_type == "pst"):
        if(simenv.sbe_image_type == "tsbe"):
            istepArray = {
                    1:[],       # no istep 1 for TAP
                    2:[14, 22], # istep 2.14 to 2.22 used for TAP.
                    3:[1, 14],  # istep 3.1 to 3.14 used for TAP.
                        }
        else:
            #TODO: Modify these as per the pst image type.
            istepArray = {
                    1:[], # empty placeholder
                        }
    elif (simenv.sbe_project_type == "odyssey") or (simenv.sbe_project_type == "odysseylab"):
        istepArray = {
                1:[13, 23], # istep 1.13 to 1.23
                2:[],       # no istep 2 for odyssey
                3:[1, 24],  # istep 3.1 to 3.24
                    }
        #TODO:Add istepArray for each platform.
    else:
        #Avoid using generic istepArray and use only platform specific istepArray
        print("Avoid using generic istepArray and use only platform specific istepArray")
        istepArray = {
                    1:[], # empty placeholder
                    }
    return istepArray
