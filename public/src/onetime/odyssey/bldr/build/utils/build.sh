# IBM_PROLOG_BEGIN_TAG
# This is an automatically generated prolog.
#
# $Source: public/src/onetime/odyssey/bldr/build/utils/build.sh $
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

#This scrpit does all packaging(zip),secure header addition etc.
#We will not add these functionalities into meson
#The input to this script is meson current build directory where the image is
#built and stored before we install.
#This script can use all the files created in the path.

#Note:Do not change name of any files while modifications.
#Meson will pick up files from same builddir during install phase.

# Exit if any command fails
set -e

#Test code
echo "Packaging..."
