# IBM_PROLOG_BEGIN_TAG
# This is an automatically generated prolog.
#
# $Source: env.bash $
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


# Apply generci bashrc

if [ -z $SBE_CI_ENV_SETUP ]; then
    if [ -e ${HOME}/.bashrc ]; then
        source ${HOME}/.bashrc
    fi
fi

echo "Setting environment variables..."

source public/src/tools/utils/sbe/venv-python

ROOTDIR=.
export SBEROOT=`pwd`
export SBEROOT_PUB="${SBEROOT}/public"
export SBEROOT_INT="${SBEROOT}/internal"
export SBEROOT_PYTHON_ENV="${SBEROOT}/venv/"

if [ -e ${SBEROOT_PUB}/projectrc ]; then
    source ${SBEROOT_PUB}/projectrc
fi

if [ -e ${SBEROOT_INT}/projectrc ]; then
    source ${SBEROOT_INT}/projectrc
fi

# Apply personal settings
if [ -e ${HOME}/.sbe/customrc ]; then
    source ${HOME}/.sbe/customrc
fi

if [ -e ${SBEROOT}/customrc ]; then
    source ${SBEROOT}/customrc
fi

if [ -z $SBE_CI_ENV_SETUP ]; then
    source public/src/tools/utils/sbe/sbe_complete
fi

#calling function for activate the virtul python environment
activatePythonVenv
#Install required packages
installRequiredPackages

if [ -f "builddir/build.ninja" ]; then
    # Sync with current workon settings
    mesonwrap sync || exit -1
fi
