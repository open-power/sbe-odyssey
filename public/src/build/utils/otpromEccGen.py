#!/usr/bin/env python3
# IBM_PROLOG_BEGIN_TAG
# This is an automatically generated prolog.
#
# $Source: public/src/build/utils/otpromEccGen.py $
#
# OpenPOWER sbe Project
#
# Contributors Listed Below - COPYRIGHT 2016,2024
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

import sys
import argparse

'''
Rearrange ECC bytes:
After extracting the ecc bytes and arranging them required offset in the image,
Word swapping need to be performed so that the each byte will be stored at the 
designated otprom data address to match the hardware layout. (Example below)

Before rearranging ECC bytes:
otprom data address (double word address):  8000    8001    8002    8003    8004    8005    8006    8007
corresponding ecc location (byte address):  8070    8071    8072    8073    8074    8075    8076    8077

Hardware is mapping ECC bytes as shown below:
otprom data address (double word address):  8000    8001    8002    8003    8004    8005    8006    8007
corresponding ecc location (byte address):  8074    8075    8076    8077    8070    8071    8072    8073
'''
def rearrange_ecc_bytes(ecc_list):
    rearrange_ecc_list = []
    for i in range(0, len(ecc_list), 8):
        rearrange_ecc_list += ecc_list[i+4:i+8] + ecc_list[i:i+4]
    return rearrange_ecc_list


'''
seperate_ecc_bytes:
1. Extracting and seperating ecc bytes from the image.
2. Passing the ecc bytes to rearrange_ecc_bytes function to arrange the ecc bytes in required format.
'''
def seperate_ecc_bytes(infile, outfile):
    file_data_list = []

    with open(infile, "rb") as file:
        ecc_byte_list=[]
        # Reading file
        filedata = file.read()
        # convert filedata to list array for processing
        file_data_list = list(filedata)

    # file len divide by 9 because of ecc calculate 8 byte and append
    # ecc byte after that ex: if file size id 896, 896/9 = 112, which
    # means ecc data size are 112 bytes
    ecc_bytes_count =  len(filedata) / 9
    rindex =  8
    total_data_count = len(filedata) - ecc_bytes_count
    while(rindex <= total_data_count):
        ecc_byte_list.append(file_data_list[rindex])
        # delete one item
        del file_data_list[rindex]
        rindex = rindex + 8
    post_arrange_eccbytes = rearrange_ecc_bytes(ecc_byte_list)
    # using naive method to concat append rearrange data to original
    for i in post_arrange_eccbytes :
        file_data_list.append(i)

    with open(outfile, "wb+") as newfile:
        # Store processed data to new file
        newfile.write(bytearray(file_data_list))

def main():
    parser = argparse.ArgumentParser(description="Process ECC bytes.")
    parser.add_argument( '-i', '--input', type=str, help="Input binary image file" ) 
    parser.add_argument( '-o', '--output', type=str, help="Output binary image file" )
    args = parser.parse_args()
    in_file_name = args.input
    out_file_name = args.output
    seperate_ecc_bytes(in_file_name, out_file_name)

if __name__ == "__main__":
    main()
