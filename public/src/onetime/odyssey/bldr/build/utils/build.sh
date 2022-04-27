# IBM_PROLOG_BEGIN_TAG
# This is an automatically generated prolog.
#
# $Source: public/src/onetime/odyssey/bldr/build/utils/build.sh $
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

#This scrpit does all packaging(zip),secure header addition etc.
#We will not add these functionalities into meson
#The input to this script is meson current build directory where the image is
#built and stored before we install.
#This script can use all the files created in the path.

#Note:Do not change name of any files while modifications.
#Meson will pick up files from same builddir during install phase.

# Exit if any command fails
set -e

BLDR_MESON_IMAGE_DIR_PATH=$1
BLDR_BASE_IMAGE_NAME=$2

#Tools
PAK_TOOL_PATH=$3
PAK_BUILD_TOOL_PATH=$4
SBE_SIGN_TOOL=$5

#Files
ODYSSEY_MAIN_FSM_PATH=$6

#Boot Loader manifest file path
BLDR_MANIFEST_PATH=$SBEROOT/public/src/onetime/odyssey/bldr/build/utils/manifest

# Create the pak based on boot loader manifest file
$PAK_BUILD_TOOL_PATH $BLDR_MANIFEST_PATH -o ${BLDR_MESON_IMAGE_DIR_PATH} -n ${BLDR_BASE_IMAGE_NAME}

#Lets generate the hash list
mkdir -p ${BLDR_MESON_IMAGE_DIR_PATH}/boot
$PAK_TOOL_PATH hash ${BLDR_MESON_IMAGE_DIR_PATH}/${BLDR_BASE_IMAGE_NAME}.pak ${BLDR_MESON_IMAGE_DIR_PATH}/boot/hash.list

#Sign the Hash List
$SBE_SIGN_TOOL -s ${BLDR_MESON_IMAGE_DIR_PATH}/scratch -i ${BLDR_MESON_IMAGE_DIR_PATH}/boot/hash.list -o ${BLDR_MESON_IMAGE_DIR_PATH}/boot/secure.hdr -c BOOT_LDR

#Change dir into meson image dir path(builddir where output images are stored)
#and then add the files into pak so that we dont endup adding the complete file
#path as file name
cd ${BLDR_MESON_IMAGE_DIR_PATH}

#Add the hash list and secure header into the pak
$PAK_TOOL_PATH add ${BLDR_BASE_IMAGE_NAME}.pak boot --method store
