# IBM_PROLOG_BEGIN_TAG
# This is an automatically generated prolog.
#
# $Source: public/src/onetime/odyssey/cust/build/utils/build.sh $
#
# OpenPOWER sbe Project
#
# Contributors Listed Below - COPYRIGHT 2022
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
set -e

CUST_MESON_IMAGE_DIR_PATH=$1
CUST_BASE_IMAGE_NAME=$2

#Tools
PAK_TOOL_PATH=$3
PAK_BUILD_TOOL_PATH=$4

#Cust manifest file path
CUST_MANIFEST_PATH=$SBEROOT/public/src/onetime/odyssey/cust/build/utils/manifest

# Create the pak based on vpd manifest file
$PAK_BUILD_TOOL_PATH $CUST_MANIFEST_PATH -o ${CUST_MESON_IMAGE_DIR_PATH} -n ${CUST_BASE_IMAGE_NAME}
