# IBM_PROLOG_BEGIN_TAG
# This is an automatically generated prolog.
#
# $Source: public/src/runtime/odyssey/sppe/build/utils/build.sh $
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

SPPE_MESON_IMAGE_DIR_PATH=$1
SPPE_BASE_IMAGE_NAME=$2

SPPE_IMAGE_ABS=$1/$2

#Tools
ATTR_TOOL=$3
PAK_TOOL_PATH=$4
PAK_BUILD_TOOL_PATH=$5
SBE_SIGN_TOOL=$6

$ATTR_TOOL setfixed $SPPE_IMAGE_ABS.attr.db $SPPE_IMAGE_ABS.bin -v

# Create fake embedded archive for now
echo "test" > test.bin
$PAK_TOOL_PATH add ${SPPE_MESON_IMAGE_DIR_PATH}/embedded_archive.pak test.bin
rm test.bin

#SPPE manifest file path
SPPE_MANIFEST_PATH=$SBEROOT/public/src/runtime/odyssey/sppe/build/utils/manifest

# Create the pak based on SPPE manifest file
$PAK_BUILD_TOOL_PATH $SPPE_MANIFEST_PATH -o ${SPPE_MESON_IMAGE_DIR_PATH} -n ${SPPE_BASE_IMAGE_NAME}

#Lets generate the hash list
mkdir -p ${SPPE_MESON_IMAGE_DIR_PATH}/rt
$PAK_TOOL_PATH hash ${SPPE_MESON_IMAGE_DIR_PATH}/${SPPE_BASE_IMAGE_NAME}.pak ${SPPE_MESON_IMAGE_DIR_PATH}/rt/hash.list

#Sign the Hash List
$SBE_SIGN_TOOL -s ${SPPE_MESON_IMAGE_DIR_PATH}/scratch -i ${SPPE_MESON_IMAGE_DIR_PATH}/rt/hash.list -o ${SPPE_MESON_IMAGE_DIR_PATH}/rt/ -c RUN_TIME

#Change dir into meson image dir path(builddir where output images are stored)
#and then add the files into pak so that we dont endup adding the complete file
#path as file name
cd ${SPPE_MESON_IMAGE_DIR_PATH}

#Add the hash list and secure header into the pak
$PAK_TOOL_PATH add ${SPPE_BASE_IMAGE_NAME}.pak rt --method zlib
#we need to store the hash.list in uncompressed method, since we will store
#   the hash of uncompressed file in secure.hdr, and code will be matching these two
#   And current archive library is returning hash of data before compression.
$PAK_TOOL_PATH add ${SPPE_BASE_IMAGE_NAME}.pak rt/hash.list --method store
