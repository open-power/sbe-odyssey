#!/bin/sh
# IBM_PROLOG_BEGIN_TAG
# This is an automatically generated prolog.
#
# $Source: public/src/runtime/odyssey/sppe/build/utils/installartifacts.sh $
#
# OpenPOWER sbe Project
#
# Contributors Listed Below - COPYRIGHT 2023
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

if [ $SBE_PLATFORM == "odyssey" ]; then
   BASE_PATH="public"
else
   BASE_PATH="internal"
fi

mkdir -p ${MESON_BUILD_ROOT}/${SBE_PLATFORM}_debug_files_tools
cp ${MESON_BUILD_ROOT}/$BASE_PATH/src/runtime/$SBE_PLATFORM/sppe/odyssey_sppe_DD1.dis \
   ${MESON_BUILD_ROOT}/$BASE_PATH/src/runtime/$SBE_PLATFORM/sppe/odyssey_sppe_DD1.map \
   ${MESON_BUILD_ROOT}/$BASE_PATH/src/runtime/$SBE_PLATFORM/sppe/odyssey_sppe_DD1.syms \
   ${MESON_BUILD_ROOT}/$BASE_PATH/src/runtime/$SBE_PLATFORM/sppe/odyssey_sppe_DD1.attr.db \
   ${MESON_BUILD_ROOT}/$BASE_PATH/src/runtime/$SBE_PLATFORM/sppe/odysseySppeStringFile_DD1 \
   ${MESON_INSTALL_PREFIX}/odyssey/onetime/bldr/odyssey_bldr_DD1.dis \
   ${MESON_INSTALL_PREFIX}/odyssey/onetime/bldr/odyssey_bldr_DD1.map \
   ${MESON_INSTALL_PREFIX}/odyssey/onetime/bldr/odyssey_bldr_DD1.syms \
   ${MESON_INSTALL_PREFIX}/odyssey/onetime/bldr/odysseyBldrStringFile_DD1 \
   ${MESON_SOURCE_ROOT}/public/src/onetime/odyssey/srom/build/images/odysseySromStringFile_DD1 \
   ${MESON_SOURCE_ROOT}/public/src/onetime/odyssey/srom/build/images/odyssey_srom_DD1.syms \
   ${MESON_SOURCE_ROOT}/public/src/onetime/odyssey/srom/build/images/odyssey_srom_DD1.map \
   ${MESON_SOURCE_ROOT}/public/src/onetime/odyssey/srom/build/images/odyssey_srom_DD1.dis \
   ${MESON_BUILD_ROOT}/${SBE_PLATFORM}_debug_files_tools/ || exit 1

# Copy simics related tools and utils into simics dir (This is required for HB)
mkdir -p  ${MESON_BUILD_ROOT}/${SBE_PLATFORM}_debug_files_tools/simics/
cp ${MESON_SOURCE_ROOT}/internal/src/runtime/odyssey/sppe/test/testcases/testUtil.py \
   ${MESON_SOURCE_ROOT}/internal/src/tools/simics/sbestartupodystandalone.simics \
   ${MESON_BUILD_ROOT}/${SBE_PLATFORM}_debug_files_tools/simics/ || exit 1

tar -czvf ${MESON_INSTALL_PREFIX}/$SBE_PLATFORM/${SBE_PLATFORM}_sbe_debug.tar.gz \
    -C ${MESON_BUILD_ROOT} \
    ${SBE_PLATFORM}_debug_files_tools || exit 1
