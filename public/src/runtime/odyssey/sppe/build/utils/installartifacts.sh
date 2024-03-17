#!/bin/sh
# IBM_PROLOG_BEGIN_TAG
# This is an automatically generated prolog.
#
# $Source: public/src/runtime/odyssey/sppe/build/utils/installartifacts.sh $
#
# OpenPOWER sbe Project
#
# Contributors Listed Below - COPYRIGHT 2023,2024
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
   ${MESON_SOURCE_ROOT}/public/src/onetime/odyssey/srom/build/images/odyssey_srom_DD1.bin \
   ${MESON_SOURCE_ROOT}/public/src/onetime/odyssey/otprom/build/images/v2.0/odyssey_otprom_DD1.bin \
   ${MESON_SOURCE_ROOT}/internal/src/tools/debug/ppe2fsp `# Keep copying x86 executable as a backup` \
   ${MESON_SOURCE_ROOT}/internal/src/tools/debug/fsp-trace \
   ${MESON_SOURCE_ROOT}/public/src/tools/trace/ppe2fsp.py \
   ${MESON_SOURCE_ROOT}/public/src/runtime/tools/attributes/attr* \
   ${MESON_SOURCE_ROOT}/public/src/tools/install/debugtoolrequirements.txt \
   ${MESON_BUILD_ROOT}/${SBE_PLATFORM}_debug_files_tools/ || exit 1

# Copy simics related tools and utils into simics dir (This is required for HB)
mkdir -p  ${MESON_BUILD_ROOT}/${SBE_PLATFORM}_debug_files_tools/simics/
cp ${MESON_SOURCE_ROOT}/internal/src/tools/simics/pozsbeinitstandalone.simics \
   ${MESON_SOURCE_ROOT}/internal/src/tools/simics/sbestandaloneenv.simics \
   ${MESON_SOURCE_ROOT}/internal/src/test/utils/simTargets.py \
   ${MESON_SOURCE_ROOT}/internal/src/tools/simics/poz-simics-debug-framework.py \
   ${MESON_SOURCE_ROOT}/internal/src/tools/simics/pozmsgregparser.py \
   ${MESON_SOURCE_ROOT}/internal/src/test/utils/toolwrappers.py \
   ${MESON_SOURCE_ROOT}/internal/src/test/utils/sbeSimUtils.py \
   ${MESON_SOURCE_ROOT}/internal/src/test/utils/sbeIstepAuto.py \
   ${MESON_BUILD_ROOT}/${SBE_PLATFORM}_debug_files_tools/simics/ || exit 1

# Copy parser plugins on FSP based systems
mkdir -p  ${MESON_BUILD_ROOT}/${SBE_PLATFORM}_debug_files_tools/parserplugins
cp ${MESON_SOURCE_ROOT}/public/src/runtime/common/plat/ffdc/sbeffdctypecommon.H \
   ${MESON_SOURCE_ROOT}/public/src/runtime/odyssey/sppe/external/odySbeUserDataParser.C \
   ${MESON_BUILD_ROOT}/${SBE_PLATFORM}_debug_files_tools/parserplugins || exit 1

tar -czvf ${MESON_INSTALL_PREFIX}/$SBE_PLATFORM/${SBE_PLATFORM}_sbe_debug_DD1.tar.gz \
    -C ${MESON_BUILD_ROOT} \
    ${SBE_PLATFORM}_debug_files_tools || exit 1
