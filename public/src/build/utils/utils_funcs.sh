#!/bin/sh
# IBM_PROLOG_BEGIN_TAG
# This is an automatically generated prolog.
#
# $Source: public/src/build/utils/utils_funcs.sh $
#
# OpenPOWER sbe Project
#
# Contributors Listed Below - COPYRIGHT 2023
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

#######################################
# Check whether the given environment variable is defined or not.
#
# Arguments:
#   Arg1: The environment variable to check.
#
# Outputs:
#   Exit if the given environment variable is not defined.
#######################################
check_var ()
{
    # Arguments:
    local var="$1"

    eval value=\$${var}
    if [ -z "$value" ] ; then
        echo "'$var' environment variable is not defined"
        exit 1
    fi
}
