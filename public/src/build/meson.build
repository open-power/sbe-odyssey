# IBM_PROLOG_BEGIN_TAG
# This is an automatically generated prolog.
#
# $Source: public/src/build/meson.build $
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

subdir('buildekb')

# Install tools thats will expect from the SBE for the external components.
meson.add_install_script('utils/installTools.sh')

# Install the SBE project specific image if required.
# For example, golden image (aka, side 2 image) for the odyssey project.
# FIXME: In future if the SBE project specific scope is increasing to install
#        some images then, introduce a project specific installation script and
#        meson target rules for the same.
meson.add_install_script('utils/installImages.sh')
