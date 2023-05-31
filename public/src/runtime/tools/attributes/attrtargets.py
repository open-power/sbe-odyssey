# IBM_PROLOG_BEGIN_TAG
# This is an automatically generated prolog.
#
# $Source: public/src/runtime/tools/attributes/attrtargets.py $
#
# OpenPOWER sbe Project
#
# Contributors Listed Below - COPYRIGHT 2023
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
from collections import namedtuple
import json

class TargetListParseError(Exception):
    pass

TargetTypeInfo = namedtuple("TargetTypeInfo", "ntargets")

CHIP_TARGETS = [
        'TARGET_TYPE_HUB_CHIP',
        'TARGET_TYPE_COMPUTE_CHIP',
        'TARGET_TYPE_PROC_CHIP',
        'TARGET_TYPE_OCMB_CHIP'
]

def load_target_list_json(i_path:str) -> 'tuple[dict[str, TargetTypeInfo], str]':

    root_chip_name = ''
    def targetTypeInfoLoader(**kwarg : dict):
        nonlocal root_chip_name

        if('__targInfo__' in kwarg):
            if('__rootChip__' in kwarg):
                if(root_chip_name != ''):
                    raise TargetListParseError("Multiple root chip is declared")
                root_chip_name = kwarg['chipName']

            return TargetTypeInfo(kwarg['ntargets'])
        return kwarg

    with open(i_path, 'r') as fp:
        target_types =json.load(
                fp, object_hook= lambda d : targetTypeInfoLoader(**d))

    return target_types, root_chip_name
