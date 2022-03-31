# IBM_PROLOG_BEGIN_TAG
# This is an automatically generated prolog.
#
# $Source: public/src/build/buildekb/common/meson.build $
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

#List containing all EKB common lib paths
ekb_cmn_lib_paths = []

#List containing all lib names for seeprom image
ekb_cmn_libs = []

#List of all dependent libs from ekb common. (For custom target depends input)
#Image generation will not start unless all the libs in below list are built
#This list is basically custom target lib generation output object
ekb_cmn_dep_libs = []

subdir('ppebaselib')
subdir('ppetrace')
subdir('pkkernel')
subdir('boltonlib')
subdir('ppe42')

if (get_option('srom').enabled())
    subdir('paksrom')
endif

if (get_option('sppe').enabled()) or (get_option('tsbe').enabled()) or
                            (get_option('hsbe').enabled() or bldr.enabled())
    subdir('pak')
endif

if (get_option('sppe').enabled()) or (get_option('tsbe').enabled()) or
                                     (get_option('hsbe').enabled())
    subdir('errorxml')
    subdir('hwp')
    subdir('attributes')
endif
