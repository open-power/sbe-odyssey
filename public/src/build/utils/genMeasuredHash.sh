#!/bin/sh
# IBM_PROLOG_BEGIN_TAG
# This is an automatically generated prolog.
#
# $Source: public/src/build/utils/genMeasuredHash.sh $
#
# OpenPOWER sbe Project
#
# Contributors Listed Below - COPYRIGHT 2016,2023
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

# Exit if any command fails
set -e

toolName=$(basename $0)

# @brief Checks whether the file that is the input argument exists on the file
# system. The function
#        exits with an error status if the file DNE.
#
# @param[in] Full path to the file to check
check_path()
{
    if [ ! -f $1 ]; then
        echo "***ERROR | SBE | $toolName : $1 DNE!"
        exit -1
    fi
}

# @brief Print the help/usage info to the console
usage()
{
    echo "Usage"
    echo "!!!RUN ON A RHEL7 & RHEL8 MACHINE!!!"
    echo "Generate Image Hash"
    echo "$toolName -i <secure_hdr> -o <img_hash> [-h ]"
    echo ""
    echo "Reguired Options:"
    echo "  -i: secure header. "
    echo "  -o: Output path for image hash"
    echo "  -f: Output file name "
    echo "Optional Options:"
    echo "  -h: print this message."
}

while getopts "h?i:o:f:" opt;
do
    case $opt in

        h)
            usage
            exit 0
        ;;

        i)
            SECURE_HDR=$OPTARG
        ;;

        f)
            FILE_NAME=$OPTARG
        ;;

        o)
            IMAGE_HASH_PATH=$OPTARG
        ;;

        *)
            echo "Bad option: $OPTARG"
            usage
            exit -1
        ;;

    esac
done

#Check if user has passed all required arguments
if [ -z $SECURE_HDR ]; then
    echo "***ERROR | SBE | $toolName : Secure Header not passed. Please supply the -i option."
    usage
    exit -1
fi

if [ -z $FILE_NAME ]; then
    echo "***ERROR | SBE | $toolName : File name base not passed. Please supply the -f option."
    usage
    exit -1
fi

SIGNING_DIR=
DEV_KEY_DIR=
SIGNTOOL_ARGS=

# Determine if our path setup has been provided by the environment
if [ ! -z $SIGNING_BASE_DIR ]; then
    echo "***INFO | SBE | $toolName : Fetching signing tools from provided environment"
    RH_DIR=`sed "s/^.*release \([0-9]*\).*$/rh\1/" /etc/redhat-release`
    SIGNING_DIR=$SIGNING_BASE_DIR/$RH_DIR/$SIGNING_UTILS_DIR

elif [ -z $SBE_IMG_DIR_OP ]; then
    echo "***INFO | SBE | $toolName : Not OP_build. Fetching keys and signing scripts from /gsa/..."
    #RH_DIR=`sed 's/^.*release \([0-9]*\)\..*$/rh\1/' /etc/redhat-release`
    #TODO: Update below paths
    if [ -n $SIGNING_RHEL_PATH ]; then
        SIGNING_DIR=${SIGNING_RHEL_PATH}/bin/
    else
        echo "***ERROR | SBE | $toolName : SIGNING_RHEL_PATH env var is not defined"
        exit -1
    fi

else
    echo "***INFO | SBE | $toolName : OP_build. Fetching keys and signing scripts from OP-Build Tree"
    SIGNING_DIR=${SBE_IMG_DIR_OP}/../../../host/usr/bin/
fi

# Adjust paths to pick up the signing tools
LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$SIGNING_DIR
PATH=$SIGNING_DIR:$PATH

#TODO: Change below path once available on GFW machines
# open ssl path default installed in RH8 machine but RH7 machine is quit different
OPENSSL_1_1_PATH=$OPEN_SSL_PATH

# Do some sanity checks on things we expect the script to consume
check_path ${SIGNING_DIR}/print-container
check_path ${OPENSSL_1_1_PATH}
check_path ${SECURE_HDR}

printf "\nGenerate Image Hash\n"

#Generate Image Hash
# SHA3-Hash(HW PUB keys Hash | FW PUB Keys Hash | Payload Hash)

#Hash of the HW keys. Fetch the value from any of the secure container which is created.
#We need to filter out the HW keys from the print container output and convert the output to .bin format
hwFwPayloadhashFile="${IMAGE_HASH_PATH}/hwFwPayloadhash.bin"
echo "***INFO | SBE | $toolName : Fetch Hash of HW keys A and D..."
print-container -w0 --imagefile ${SECURE_HDR} | grep -A1 "HW keys hash" | \
tail -1 | awk '{print $1}' | xxd -r -p > $hwFwPayloadhashFile

echo "***INFO | SBE | $toolName : Fetch Hash of FW keys P and S..."
print-container -w0 --imagefile ${SECURE_HDR} | grep -A1 "payload_hash" | \
head -1 | awk '{print $2}' |  xxd -r -p >> $hwFwPayloadhashFile

echo "***INFO | SBE | $toolName : Fetch the Payload hash..."
print-container -w0 --imagefile ${SECURE_HDR} | grep -A1 "unprotected" | \
tail -1 | awk '{print $2}' |  xxd -r -p >> $hwFwPayloadhashFile

echo "***INFO | SBE | $toolName : Calculating Image Hash..."
${OPENSSL_1_1_PATH} dgst -sha3-512 $hwFwPayloadhashFile | \
awk '{print $2}' |  xxd -r -p > ${IMAGE_HASH_PATH}/${FILE_NAME}.hash

# Remove created hwFwPayloadhash file that used to store key to measure
rm -f $hwFwPayloadhashFile

echo ""

exit 0
