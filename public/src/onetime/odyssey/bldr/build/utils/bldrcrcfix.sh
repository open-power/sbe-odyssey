#!/bin/sh
# IBM_PROLOG_BEGIN_TAG
# This is an automatically generated prolog.
#
# $Source: public/src/onetime/odyssey/bldr/build/utils/bldrcrcfix.sh $
#
# OpenPOWER sbe Project
#
# Contributors Listed Below - COPYRIGHT 2024
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

set -e

toolName=$(basename $0)

usage ()
{
    echo "Usage: $0 <output_directory>"
    exit 1
}

check_var ()
{
    var="$1"
    eval value=\$${var}
    if [ -z "$value" ] ; then
        echo "$var is not defined"
        exit 1
    fi
}

trace ()
{
    echo "INFO | $toolName | $@"
}

# Ensure required environment variables are defined
check_var OP_UTILS_COMMIT

# Variables
opUtilsRepoUri="git@github.com:open-power/op-utils.git"
opUtilsRepo="$SBEROOT/builddir/op-utils"
opUtilsRepoBranch="main"
opUtilsRepoCommit=${OP_UTILS_COMMIT}
fixCrcTool="$opUtilsRepo/sbe/scripts/fixcrc.py"

# Get latest code of the op-utils
if [ \( ! -d "$opUtilsRepo" \) -a \( ! -d "$opUtilsRepo/.git" \) ] ;
then
    rm -rf "$opUtilsRepo"
    git clone -b "$opUtilsRepoBranch" "$opUtilsRepoUri" \
                "$opUtilsRepo" || exit 1
else
    cd $opUtilsRepo
    git remote -v update || exit 1
    localsrc=$(git rev-parse @{0})
    remotesrc=$(git rev-parse @{u})
    basesrc=$(git merge-base @{0} @{u})
    if [ $localsrc = $remotesrc ] ; then
        trace "op-utils: Already up-to-date."
    elif [ $localsrc = $basesrc ] ; then
        trace "op-utils is not latest, pull"
        git pull || exit 1
    else
        trace "Unexpected op-utils code level,
                it could be either diverged or local is ahead."
        trace "localsrc: $localsrc"
        trace "remotesrc: $remotesrc"
        trace "basesrc: $basesrc"
        exit 1
    fi
    cd -
fi

isCheckedOut=false
# Get specified commit if not match.
if [ $opUtilsRepoCommit != "HEAD" ] ; then
    cd $opUtilsRepo
    localHead=$(git rev-parse HEAD)
    if [ $opUtilsRepoCommit != $localHead ] ; then
        trace "op-utils local HEAD is not matched with given commit,
                checkout."
        git checkout "$opUtilsRepoCommit" || exit 1
        isCheckedOut=true
    fi
    cd -
fi

if $isCheckedOut ; then
    cd $opUtilsRepo
    git checkout "$opUtilsRepoBranch"
    cd -
fi

# Parameters
inputBinary=$1
outputBinary=$1
desiredCrc="77CCBFDE"

$fixCrcTool "$inputBinary" "$desiredCrc" "$outputBinary" || exit 1