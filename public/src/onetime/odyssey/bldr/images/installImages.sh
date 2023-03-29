#!/bin/sh
# IBM_PROLOG_BEGIN_TAG
# This is an automatically generated prolog.
#
# $Source: public/src/onetime/odyssey/bldr/images/installImages.sh $
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

echo "Installing production bootloader image..."
# TODO: JIRA: 325: Untar files only based on the checksum instead of
#                  checking the file existence
if [ \( ! -d "${MESON_INSTALL_PREFIX}/odyssey/onetime/bldr" \) ] ;
then
    tar -xzvf \
        ${MESON_SOURCE_ROOT}/public/src/onetime/odyssey/bldr/images/bldr.tar.gz \
        -C ${MESON_INSTALL_PREFIX}/odyssey/onetime/ || exit 1
fi
