#!/bin/sh
# IBM_PROLOG_BEGIN_TAG
# This is an automatically generated prolog.
#
# $Source: public/src/build/utils/installImages.sh $
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

# Install golden image for the odyssey project
if [ \( $SBE_PLATFORM == "odyssey" \) -o \
     \( $SBE_PLATFORM == "odysseylab" \) ] ; then
    if [ $SBE_IMAGE == "pnor" ] ; then
        # TODO: JIRA: 325: Untar files only based on the checksum instead of
        #                  checking the file existence.
        # Get golden image file name.
        # For example, "golden_odyssey_nor_DD1.img.tar.gz"
        goldenImgName="$(basename $(readlink ${GOLDEN_IMG}))"

        # Get golden image file name without tar file extension to check
        # the file existence. For example "golden_odyssey_nor_DD1.img"
        goldenImgName="${goldenImgName%.*.*}"

        if [ ! -f "${MESON_INSTALL_PREFIX}/${SBE_PLATFORM}/$goldenImgName" ] ;
        then
            echo "Installing ${SBE_PLATFORM} golden image from ${GOLDEN_IMG}"
            tar -xzvf ${GOLDEN_IMG} \
                -C ${MESON_INSTALL_PREFIX}/${SBE_PLATFORM}/ \
                || exit 1
        fi
    fi
fi
