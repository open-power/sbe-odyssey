#!/bin/sh
# IBM_PROLOG_BEGIN_TAG
# This is an automatically generated prolog.
#
# $Source: public/src/import/public/hwp/generic/perv/convert-to-declare-hwp.sh $
#
# OpenPOWER sbe Project
#
# Contributors Listed Below - COPYRIGHT 2024
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

# Run a big regex across all header files in the current directory to
#  1. Remove all extern "C" wrappers (done within DECLARE_HWP)
#  2. Remove the *_FP_t typedef
#  3. Convert the HWP declaration to DECLARE_HWP

# The results aren't perfect and probably need some follow-up work
# but this takes care of the mind-numbing busywork.

perl -i -0777 -pe 's/extern\s+"C"\s*\{\s*(.*)\s*\}(\s*\/\/\s*extern\s+"C".*)?/\1/s;s/typedef\s+fapi2::Return.*?\);\s*//s;s/fapi2::ReturnCode\s+(\w+)\s*\((.*?)\);/DECLARE_HWP(\1, \2);/s;s|//--[^/]*// Structure def.*HWP call support\s||s;' $(grep -l 'extern "C"' *.H)
