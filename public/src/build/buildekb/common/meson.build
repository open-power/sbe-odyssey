# IBM_PROLOG_BEGIN_TAG
# This is an automatically generated prolog.
#
# $Source: public/src/build/buildekb/common/meson.build $
#
# OpenPOWER sbe Project
#
# Contributors Listed Below - COPYRIGHT 2022,2023
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

subdir('ppebaselib')
subdir('ppetrace')
subdir('pkkernel')
subdir('boltonlib')
subdir('ppe42')

if (get_option('srom').enabled())
    subdir('paksrom')
endif

if (get_option('sppe').enabled() or get_option('bldr').enabled() or
    get_option('hsbe').enabled() or get_option('tsbe').enabled() or
    get_option('sbe').enabled()  or get_option('pme').enabled())
    subdir('pak')
endif

if (get_option('sppe').enabled() or
    get_option('tsbe').enabled() or get_option('hsbe').enabled() or
    get_option('sbe').enabled() or get_option('pme').enabled())

    parseErrorInfo = find_program('../../../import/public/common/generic/fapi2/tools/parseErrorInfo.pl')
    attrTool = find_program('../../../runtime/tools/attributes/attrtool')

    subdir('errorxml')
    subdir('hwp')
    subdir('attributes')
    subdir('rings')
endif