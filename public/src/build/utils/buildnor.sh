#!/bin/sh
# IBM_PROLOG_BEGIN_TAG
# This is an automatically generated prolog.
#
# $Source: public/src/build/utils/buildnor.sh $
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

# Exit if any command fails
set -e

# @brief Checks whether the file that is the input argument exists on the file
# system. The function exits with an error status if the file DNE.
#
# @param[in] Full path to the file to check
check_path()
{
    if [ ! -f $1 ]; then
        echo "***ERROR | SBE | buildnor : $1 DNE!"
        exit -1
    fi
}

# @brief Print the help/usage info to the console
usage()
{
    echo "Usage"
    echo "Build NOR Image"
    echo "buildnor -p <partition0_img> -o <output_file_path> [-s partition1_img]
            [-g golden_partition_img] [-e ecc_tool_path] [-h ]"
    echo " "
    echo " Required Args:"
    echo "  -p: partition zero image / primary partition"
    echo "  -o:  output image path "
    echo " "
    echo " Optional Args:"
    echo "  -h: print this message "
    echo "  -s: partition one image / secondary partition. Default: partition zero image"
    echo "  -g:  golden partition image. Default: partition zero image "
    echo "  -e:  ecc tool path. Default: ECC tool in sbe repo "
}

while getopts "h?p:s:o:g:e:" opt;
do
    case $opt in

        h)
            usage
            exit 0
        ;;

        p)
            PARTITION_0_IMAGE_PATH=$OPTARG
        ;;

        s)
            PARTITION_1_IMAGE_PATH=$OPTARG
        ;;

        g)
            GOLDEN_PARTITION_IMAGE_PATH=$OPTARG
        ;;

        e)
            ECC_TOOL_PATH=$OPTARG
        ;;

        o)
            OUTPUT_IMAGE_PATH=$OPTARG
        ;;

        *)
            echo "Bad option: $OPTARG"
            usage
            exit -1
        ;;

    esac
done

#Check if user has passed all required arguments
if [ -z $PARTITION_0_IMAGE_PATH ]; then
    echo "***ERROR | SBE | buildnor : Supply Partition 0 image path. Please supply the -p option."
    usage
    exit -1
fi

if [ -z $OUTPUT_IMAGE_PATH ]; then
    echo "***ERROR | SBE | buildnor : Supply output image path. Please supply the -o option."
    usage
    exit -1
fi

# If partition 1 image path is not passed default it to partition 0 path
if [ -z $PARTITION_1_IMAGE_PATH ]; then
    echo "***INFO | SBE | buildnor : Partition 1 image path not passed. Defaulting to partition 0 image path..."
    PARTITION_1_IMAGE_PATH=$PARTITION_0_IMAGE_PATH
fi

# If golden partition image path is not passed default it to partition 0 path
if [ -z $GOLDEN_PARTITION_IMAGE_PATH ]; then
    echo "***INFO | SBE | buildnor : Golden partition image path not passed. Defaulting to partition 0 image path..."
    GOLDEN_PARTITION_IMAGE_PATH=$PARTITION_0_IMAGE_PATH
fi

# If ECC tool path is not passed default it to use ecc tool present in SBE repo
if [ -z $ECC_TOOL_PATH ]; then
    echo "***INFO | SBE | buildnor : ECC tool path not passed."
    ECC_TOOL_PATH=$SBEROOT/public/src/build/utils/ecc
    echo "***INFO | SBE | buildnor : Defaulting to $ECC_TOOL_PATH"
fi

#Check if file paths exists
check_path $PARTITION_0_IMAGE_PATH
check_path $PARTITION_1_IMAGE_PATH
check_path $GOLDEN_PARTITION_IMAGE_PATH
check_path $ECC_TOOL_PATH

# Currently single partition image generated is of size 1Mb.
# Each partition is of size 4MB.
# We will have to pad 3Mb zeros for every partition
# NOTE: This can change as per design in odysseylink.H. Manual changes are required to be done here.
PAD_BYTES_SIZE=3145728

#Lets create a temprorary scratch dir inside output dir for storing intermediate files.
TEMP_SCRATCH_PATH=$OUTPUT_IMAGE_PATH/temp_scratch
mkdir -p $TEMP_SCRATCH_PATH

#Copy the image into scratch
cp $PARTITION_0_IMAGE_PATH $TEMP_SCRATCH_PATH/odyssey_nor_DD1.img.part0
cp $PARTITION_1_IMAGE_PATH $TEMP_SCRATCH_PATH/odyssey_nor_DD1.img.part1
cp $GOLDEN_PARTITION_IMAGE_PATH $TEMP_SCRATCH_PATH/odyssey_nor_DD1.img.golden

#TODO: Add logic to automatically calculate current size and pad required
# Pad partition zero with pad bytes
echo "***INFO | SBE | buildnor : Padding partition zero..."
head -c $PAD_BYTES_SIZE /dev/zero >> $TEMP_SCRATCH_PATH/odyssey_nor_DD1.img.part0

# Pad partition one with pad bytes
echo "***INFO | SBE | buildnor : Padding partition one..."
head -c $PAD_BYTES_SIZE /dev/zero >> $TEMP_SCRATCH_PATH/odyssey_nor_DD1.img.part1

# Pad golden partition with pad bytes
echo "***INFO | SBE | buildnor : Padding golden partition..."
head -c $PAD_BYTES_SIZE /dev/zero >> $TEMP_SCRATCH_PATH/odyssey_nor_DD1.img.golden

# Append all images to create a nor image
echo "***INFO | SBE | buildnor : Creating nor..."
cat $TEMP_SCRATCH_PATH/odyssey_nor_DD1.img.part0 \
    $TEMP_SCRATCH_PATH/odyssey_nor_DD1.img.part1 \
    $TEMP_SCRATCH_PATH/odyssey_nor_DD1.img.golden > $TEMP_SCRATCH_PATH/odyssey_nor_DD1.img

# Inject ecc into the image
echo "***INFO | SBE | buildnor : Inserting ECC..."
$ECC_TOOL_PATH --inject $TEMP_SCRATCH_PATH/odyssey_nor_DD1.img --output $OUTPUT_IMAGE_PATH/odyssey_nor_DD1.img.ecc --p8

# Remove the scratch dir
rm -rf $TEMP_SCRATCH_PATH
