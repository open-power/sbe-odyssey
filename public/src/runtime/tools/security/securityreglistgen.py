#!/usr/bin/env python3
# IBM_PROLOG_BEGIN_TAG
# This is an automatically generated prolog.
#
# $Source: public/src/runtime/tools/security/securityreglistgen.py $
#
# OpenPOWER sbe Project
#
# Contributors Listed Below - COPYRIGHT 2023,2024
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

"""!
@file securityreglistgen.py
@brief The file incorporates a specialized algorithm designed to enhance
       security list of allow and deny addresses, as well as chiplet data, into
       a C++ table, commonly known as an array.
"""

# Imports
import getopt
import sys
import os
import csv

# Exit codes
SUCCESS       = 0
INVALID_USAGE = 1
PRINT_AND_EXIT= 2

#Variable for storing user input file path.

# Global Constants
DEFAULT_FILE_NAME = "Security_Allow_Deny_list.csv"
DEBUG             = False
VERBOSE           = False

#Output file name
GEN_FILE      = "sbesecuritygen.H"

# csv tags
TAG_BASE_ADDR     = 'Base Address'
TAG_CHIPLET       = 'Chiplet'
TAG_CHIPLET_RANGE = 'Chiplet Id - range'
TAG_VERSION       = 'Version'
TAG_TYPE          = 'Type'
TAG_BIT_MASK      = 'Bit Mask'

TAG_NAME_ALLOWLIST = 'write_allowlist'
TAG_NAME_PARTIALALLOWLIST = 'write_partialallowlist'
TAG_NAME_DENYLIST = 'read_denylist'

# Functions
def usage():
    """! Function for print the file usage on console.
         How to execute on console?
            securityreglistgen.py -h
    """

    print(
'''usage: securityreglistgen.py [-h] [-f <security_list_path>] [-o <output directory] [i] [-d] [-v]
arguments:\n\
-h, --help               show this help message and exit
-f, --file               path to the security list csv file
-o, --output             output directory
-w, --allowlist          print allow list read from csv
-b, --denylist           print deny list read from csv
-g, --partiallist        print partial allow read from csv
-i, --info               get version info of the security list
-d, --debug              enable debug traces
-v, --verbose            enable verbose traces''')


def exit(error, msg = ''):
    """! Function to throw error and print error's
    @param error The exit code
    @param msg   The message to print

    @return none
    """
    if(error == SUCCESS):
        return 0
    elif(error == INVALID_USAGE):
        print(msg)
        usage()
    elif(error == PRINT_AND_EXIT):
        print(msg)
    else:
        if(DEBUG):
            print("unknown error:exiting")
    sys.exit(1)


def remove_duplicates(xlist):
    """! @brief Remove duplicates from list
    @param xlist   expected the list

    @return none duplicated data
    """
    xlist = list(set(xlist))
    return xlist


def remove_zeroes(i_list):
    """! @brief remove zeros in list
    @param i_list expected list

    @return zero removed list
    """
    out_list = []
    for a in i_list:
        if not a == 0x00:
            out_list += [a]
    return out_list

def gen_file(allowlist_tables, denylist_tables, partialallowlist):
    """! @brief Generate Header file from allow, deny, partial list
                This function will generate the securityGen.H header file from
                allow, deny, partial list.
                Each list in the header file contain three table (T1, T2, T3)
                which is sorted.
                T1 contains bit 0-7 of the 32-bit address
                T2 contains bit 8-15 of the 32-bit address
                T3 contains bit 16-31 of the 32-bit address

    @param allowlist_tables  Expected list of allowed data
    @param denylist_tables   Expected list of denied data
    @param partialallowlist  Expected list of partial data

    @return none
    """
    global GEN_FILE

    # Generative header file
    header = ("#pragma once\n\n"+
              "#include \"sbesecurity.H\"\n\n"+
              "#include \"securityalgo.H\"\n\n"+
              "using namespace SBE_SECURITY;\n\n")

    tables = (('ALLOWLIST', 'allowlist', allowlist_tables),
              ('DENYLIST', 'denylist', denylist_tables))
    body = ''
    # table 1 range and running count type
    table1_range_type = "uint8_t"
    table1_index_type = "uint8_t"
    # table 2 value and running count type
    table2_value_type = "uint8_t"
    table2_index_type = "uint16_t"
    # table 3 value type
    table3_value_type = "uint16_t"
    for namespace, tablename, table in tables:
        body += ("""
namespace """+namespace+"""
{
    /*
    table 1: ['0x0000:0x04', '0x0202:0x05'] ->
                                          {0x00, 0x00, 0x04}, {0x02, 0x02, 0x05}
       keys (1st 2byte )  = first 2byte - (start , end) of
                            the ranges in bit 0-7 of the 32-bit address
       values (3rd Bytes) = 3rd byte of an array, running count of the paths to
                            table2

    for example - if ranges are 0x20-0x37, 0x01-0x01, 0x10-0x17 and has
                  1, 2 and 3 paths respectively to table 2
    then table 1 will have {0x20,0x37} = 01, {0x01,0x01} = 3, {0x10,0x17} = 7

    1 byte for running count - we are good with uint8_t till the
    total paths are less than 256
    */
    _t1_t _t1[]  = {
    // length of the table = """+s_list_len(table[0])+"""
"""+s_table1_gen(tablename, table[0])+"""
                                                          };

    /*
    table 2: ['0x04:0x14', '0x06:0x17',] -> {0x04, 0x14}, {0x06, 0x17}
       keys (1st bytes)   = unique 1 byte numbers having same prefix for each
                            range in table 1 key - bit 8-15 from a 32-bit
                            address
       values (2nd bytes) = running count of paths from each of the keys

    for example - if element a has 1 path, b has 0 and c has 3 paths
    then table 1 will have a = 1, b = 1, c = 4

    1 byte for key
    2 byte for number of paths
    We are good with uint16_t,
    till the number of paths to table 3 from each key is less than 65536
    */
    _t2_t _t2[] = {
    // length of the table = """+s_list_len(table[1])+"""
"""+s_table2_gen(tablename, table[1])+"""
                                      };
    /*
    table 3: ['0x0000', '0x0001', '0x0002] -> {0x0000, 0x0001, 0x0002}
       values = 2 byte value bit 16-31 of the 32-bit address
    */
    _t3_t _t3[] = {
    // length of the table = """+s_list_len(table[2])+"""
"""+s_table3_gen(tablename, table[2])+"""
                      };
    _t1_table_t t1 =
            {sizeof(_t1)/sizeof(_t1_t),
             0xFF000000,
             _t1};
    _t2_table_t t2 =
            {sizeof(_t2)/sizeof(_t2_t),
             0x00FF0000,
             _t2};
    _t3_table_t t3 =
            {sizeof(_t3)/sizeof(_t3_t),
             0x0000FFFF,
             _t3};

    bool isPresent(uint32_t i_addr)
    {
        return _is_present(t1, t2, t3, i_addr);
    }
}""")

    partialallowlist_addr_type = "uint32_t"
    partialallowlist_mask_type = "uint64_t"
    body += ("""
namespace PARTIALALLOWLIST
{
    /*
    table 1:
       Address   = 4 byte
       Mask      = 8 byte
    */
    _pl_t1_t _t1[] = {
"""+s_partialallowlist_table_gen(partialallowlist)+"""
                                      };

    _pl_t1_table_t t1 =
            {sizeof(_t1)/sizeof(_pl_t1_t),
             0xFFFFFFFF,
             _t1};

    bool isPresent(uint32_t i_addr, uint64_t i_mask)
    {
        return _is_present(t1, i_addr, i_mask);
    }
}""")

    with open(GEN_FILE, 'w') as o_f:
        o_f.write(header)
        o_f.write(body)


def get_chiplet(addr):
    chiplet = (addr & 0xFF000000) >> 24
    return chiplet

def set_chiplet(addr, chiplet):
    rAddr = ((addr & 0x00FFFFFF) | (chiplet << 24)) & 0xFFFFFFFF
    return rAddr

def get_ring(addr):
    ring = (addr & 0x00003C00) >> 10
    return ring

def set_ring(addr, ring):
    rAddr = ((addr & 0xFFFFC3FF) | (ring << 10)) & 0xFFFFFFFF
    return rAddr

def get_satId(addr):
    satId = (addr & 0x000003C0) >> 6
    return satId

def set_satId(addr, satId):
    rAddr = ((addr & 0xFFFFFC3F) | (satId << 6)) & 0xFFFFFFFF
    return rAddr

def s_list_hex(prefix, list, num_chars):
    fmt_string = '0x%0'+str(num_chars)+'x'
    return prefix+str([fmt_string % ele for ele in list])

def s_list_len(list):
    return str(len(list))


def get_tables(id, xlist):
    """! @brief Generate the table from list
                This function has special algo which is optimize the list of
                address to three table
                T1 contains bit 0-7 of the 32-bit address
                T2 contains bit 8-15 of the 32-bit address
                T3 contains bit 16-31 of the 32-bit address
                which is sorted and remove duplicate data in each table.

    @param id    which is type of list (allow, deny, partial)
    @param xlist list of address

    @return three optimized table (T1, T2, T3)
    """
    # -----------------------------------------------------------------#
    # Step 1:             [register list]                              #
    #                           ||                                     #
    #                           \/                                     #
    #         {base_address1 : [chiplet range1, chiplet range2]}       #
    #..................................................................#
    # Eg: [01ABCDEF, 02ABCDEF, 01AB1234, 02AB1234, 04ABCDEF, 05ABCDEF, #
    #      10UVWXYZ, 11UVWXYZ, 04UVWXYZ, 05UVWXYZ]                     #
    #                           ||                                     #
    #                           \/                                     #
    #  {ABCDEF : [0102, 0405], AB1234:[0102], UVWXYZ : [1011, 0405]}   #
    # -----------------------------------------------------------------#
    table_base_to_range = {}
    # get a list of unique base addressess
    base_addr = [ele & 0x00FFFFFF for ele in xlist] # Get the base addr
    base_addr = remove_duplicates(base_addr)       # remove duplicates

    if(DEBUG):
        print ("########################## Gen Table #############################")
        print ("ID: " , id)
        print ("All address: " , ", ".join("0x{:02x}".format(num) for num in xlist))
        print ("------------------------------------------------------------------")
        print ("Base addr list: " , ", ".join("0x{:02x}".format(num) for num in base_addr))
        print ("########################## Step 1 ################################")
        print (
            """
                # -----------------------------------------------------------------#
                # Step 1:             [register list]                              #
                #                           ||                                     #
                #                           \/                                     #
                #         {base_address1 : [chiplet range1, chiplet range2]}       #
                #..................................................................#
                # Eg: [01ABCDEF, 02ABCDEF, 01AB1234, 02AB1234, 04ABCDEF, 05ABCDEF, #
                #      10UVWXYZ, 11UVWXYZ, 04UVWXYZ, 05UVWXYZ]                     #
                #                           ||                                     #
                #                           \/                                     #
                #  {ABCDEF : [0102, 0405], AB1234:[0102], UVWXYZ : [1011, 0405]}   #
                # -----------------------------------------------------------------#
            """
        )
    # for each base address, find the chiplet ranges
    table1_range_keys = []
    for base_ele in base_addr:
        # get a list of chiplet ids for the base address
        ele_list = []
        for list_ele in xlist:
            if((list_ele & 0x00FFFFFF) == base_ele):        # IF base addr match with given list
                ele_list += [(list_ele & 0xFF000000) >> 24] # get the chiplet ID --> ele_list
        # remove duplicates
        ele_list = remove_duplicates(ele_list)
        ele_list.sort()
        # prepare a list of unique ranges
        range_val = [ele_list[0]<<8 | ele_list[0]]

        for ele_list_ele in ele_list:
            # each consecutive number found expands the existing range
            # and the non consecutive number adds a new range

            # This condition checks if the current element is one greater than
            # the rightmost 8 bits of the last element in range_val
            if(ele_list_ele == (range_val[-1]&0xFF)+1):
                range_val[-1] = (range_val[-1] & 0xFF00) | ele_list_ele
            # This condition checks if the current element is not equal to the
            # leftmost 8 bits of the last element in range_val shifted 8 bits to the right.
            elif(ele_list_ele != (range_val[-1]&0xFF00 >> 8)):
                range_val += [ele_list_ele<<8 | ele_list_ele]
        table_base_to_range[base_ele] = range_val

    if(DEBUG):
        print ("------------------------------------------------------------------")
        print ("Step 1 table: " , table_base_to_range)
        print ("------------------------------------------------------------------")
        for key, values in table_base_to_range.items():
            print ("Base address: ", hex(key) , " Chiplet list: ", ", ".join("0x{:02x}".format(num) for num in values))
        print ("------------------------------------------------------------------")
    # -----------------------------------------------------------------#
    # Step 2: {base_address1 : [chiplet range1, chiplet range2]}       #
    #                           ||                                     #
    #                           \/                                     #
    #         {chiplet range1 : [base_address1, base_address2]}        #
    #..................................................................#
    # Eg:{ABCDEF : [0102, 0405], AB1234:[0102], UVWXYZ : [1011, 0405]} #
    #                           ||                                     #
    #                           \/                                     #
    # {0102 :[ABCDEF, AB1234], 0405 :[ABCDEF, UVWXYZ], 1011 :[UVWXYZ]} #
    # -----------------------------------------------------------------#
    if(DEBUG):
        print ("########################## Step 2 ################################")
        print (
        """
            # -----------------------------------------------------------------#
            # Step 2: {base_address1 : [chiplet range1, chiplet range2]}       #
            #                           ||                                     #
            #                           \/                                     #
            #         {chiplet range1 : [base_address1, base_address2]}        #
            #..................................................................#
            # Eg:{ABCDEF : [0102, 0405], AB1234:[0102], UVWXYZ : [1011, 0405]} #
            #                           ||                                     #
            #                           \/                                     #
            # {0102 :[ABCDEF, AB1234], 0405 :[ABCDEF, UVWXYZ], 1011 :[UVWXYZ]} #
            # -----------------------------------------------------------------#
        """
        )
    table_range_to_base = {}
    # get unique ranges
    for key, values in table_base_to_range.items():
        for val in values:
            if(val not in table1_range_keys):
                table1_range_keys += [val]

    # for each range find the base addressess
    for range_ele in table1_range_keys:
        table_range_to_base[range_ele] = []
        for key, values in table_base_to_range.items():
            if(range_ele in values):
                table_range_to_base[range_ele] += [key]
        table_range_to_base[range_ele] = remove_duplicates(table_range_to_base[range_ele])
        table_range_to_base[range_ele].sort()
    if(DEBUG):
        print ("------------------------------------------------------------------")
        print ("Step 2 table: ", table_range_to_base)
        print ("------------------------------------------------------------------")
        for key, values in table_range_to_base.items():
            print ("chiplet range: ", hex(key) , "base address list: ", ", ".join("0x{:02x}".format(num) for num in values))
        print ("------------------------------------------------------------------")

    # -----------------------------------------------------------------#
    # Step 3: {chiplet range1 : [base_address1, base_address2]}        #
    #                           ||                                     #
    #                           \/                                     #
    #         {chiplet range1 : {key1 : [base_addr1, base_Addr2]}}     #
    #..................................................................#
    # Eg:{0102 :[ABCDEF,AB1234], 0405 :[ABCDEF,UVWXYZ], 1011 :[UVWXYZ]}#
    #                           ||                                     #
    #                           \/                                     #
    #               {0102 : {AB : [CDEF, 1234]},                       #
    #                0405 : {AB : [CDEF], UV : [WXYZ]},                #
    #                1011 : {UV : [WXYZ]}}                             #
    # -----------------------------------------------------------------#
    if(DEBUG):
        print ("########################## Step 3 ################################")
        print (
            """
                # -----------------------------------------------------------------#
                # Step 3: {chiplet range1 : [base_address1, base_address2]}        #
                #                           ||                                     #
                #                           \/                                     #
                #         {chiplet range1 : {key1 : [base_addr1, base_Addr2]}}     #
                #..................................................................#
                # Eg:{0102 :[ABCDEF,AB1234], 0405 :[ABCDEF,UVWXYZ], 1011 :[UVWXYZ]}#
                #                           ||                                     #
                #                           \/                                     #
                #               {0102 : {AB : [CDEF, 1234]},                       #
                #                0405 : {AB : [CDEF], UV : [WXYZ]},                #
                #                1011 : {UV : [WXYZ]}}                             #
                # -----------------------------------------------------------------#
            """
        )
    table_range_to_key_to_base = []
    for key, values in table_range_to_base.items():
        # prepare a list of table 2 keys for each range
        temp_keys = [(val & 0x00FF0000)>> 16 for val in values]
        temp_keys = remove_duplicates(temp_keys)
        temp_keys.sort()
        l = []
        for temp_key in temp_keys:
            # for each key associated with range get a list of base addressess
            a = []
            for val in values:
                if(((val & 0x00FF0000)>>16) == temp_key):
                    a += [val & 0x0000FFFF]
            l.append((temp_key, a))
        table_range_to_key_to_base.append((key, l))

    if(DEBUG):
        print ("------------------------------------------------------------------")
        print ("step 3 table: ", table_range_to_key_to_base)
        print ("------------------------------------------------------------------")
        for key_m, tree in table_range_to_key_to_base:
            for key, values in tree:
                print ("chiplet range: ", hex (key_m))
                print("1St byte of Base address: ", hex (key))
                print("last 2 bytes of base address: ", ", ".join("0x{:02x}".format(num) for num in values))
                print ("------------------------------------------------------------------")
        print ("------------------------------------------------------------------")

    # -----------------------------------------------------------------#
    # Step 4: {chiplet range1 : {key1 : [base_addr1, base_Addr2]}}     #
    #                           ||                                     #
    #                           \/                                     #
    #                  table1, table2, table3                          #
    #..................................................................#
    # Eg:           {0102 : {AB : [CDEF, 1234]},                       #
    #                0405 : {AB : [CDEF], UV : [WXYZ]},                #
    #                1011 : {UV : [WXYZ]}}                             #
    #                           ||                                     #
    #                           \/                                     #
    #         table1: {0102 : 1, 0405 : 3, 1011 : 4}                   #
    #         table2: {AB : 2, AB : 3, UV : 4, UV : 5}                 #
    #         table3: {CDEF, 1234, CDEF, WXYZ, WXYZ}                   #
    # -----------------------------------------------------------------#
    if(DEBUG):
        print ("########################## Step 4 ################################")
        print (
            """
                # -----------------------------------------------------------------#
                # Step 4: {chiplet range1 : {key1 : [base_addr1, base_Addr2]}}     #
                #                           ||                                     #
                #                           \/                                     #
                #                  table1, table2, table3                          #
                #..................................................................#
                # Eg:           {0102 : {AB : [CDEF, 1234]},                       #
                #                0405 : {AB : [CDEF], UV : [WXYZ]},                #
                #                1011 : {UV : [WXYZ]}}                             #
                #                           ||                                     #
                #                           \/                                     #
                #         table1: {0102 : 1, 0405 : 3, 1011 : 4}                   #
                #         table2: {AB : 2, AB : 3, UV : 4, UV : 5}                 #
                #         table3: {CDEF, 1234, CDEF, WXYZ, WXYZ}                   #
                # -----------------------------------------------------------------#
            """
        )
    """
    table 1:
       keys   = 2byte - (start , end) of
                the ranges in bit 0-7 of the 32-bit address
       values = running count of the paths to table2

    for example - if ranges are 0x20-0x37, 0x01-0x01, 0x10-0x17 and has
                  1, 2 and 3 paths respectively to table 2
    then table 1 will have {0x20,0x37} = 01, {0x01,0x01} = 3, {0x10,0x17} = 7

    1 byte for running count - we are good with uint8_t till the
    total paths are less than 256
    """
    table1 = []

    """
    table 2
       keys   = unique 1 byte numbers having same prefix for each
                range in table 1 key - bit 8-15 from a 32-bit address
       values = running count of paths from each of the keys

    for example - if element a has 1 path, b has 0 and c has 3 paths
    then table 1 will have a = 1, b = 1, c = 4

    1 byte for key
    1 byte for number of paths
    We are good with uint8_t,
    till the number of paths to table 3 from each key is less than 256
    """
    table2 = []

    """
    table 3
       values = 2 byte value bit 16-31 of the 32-bit address
    """
    table3 = []

    running_count_table1 = 0
    running_count_table2 = 0
    for key_m, tree in table_range_to_key_to_base:
        running_count_table1 += len(tree)
        table1.append((key_m, running_count_table1))
        for key, values in tree:
            temp_keys = values
            temp_keys = remove_duplicates(temp_keys)
            temp_keys.sort()
            running_count_table2 += len(temp_keys)
            table2.append((key, running_count_table2))
            # table 3 values will just be the base addresses
            # for each range and key combination in order
            table3 += temp_keys

    if(VERBOSE):
        print(id+" table3 keys len ["+s_list_len(table3)+"]")

    if(DEBUG):
        print(id,"table1:", ['0x%04x:0x%02x' % ele for ele in table1])
        print(id,"table2:", ['0x%02x:0x%02x' % ele for ele in table2])
        print(id,"table3:", ['0x%04x' % ele for ele in table3])
    if(DEBUG):
        print(id,"table1 len ["+s_list_len(table1)+"]")
        print(id,"table2 len ["+s_list_len(table2)+"]")
        print(id+" table3 len ["+s_list_len(table3)+"]")

    return (table1, table2, table3)


def s_table1_gen(id, table):
    """! @brief Generate table 1 for given table (list)
                table: ['0x0000:0x04', '0x0202:0x05'] which is convert to
                array {{0x00, 0x00, 0x04}, {0x02, 0x02, 0x05}} format which is
                compatible for cpp header
    @param id    which is type of list (allow, deny, partial)
    @param table expected dictionary which is called as table

    @return header formatted array each element should not exceed 256 range
            which is fit in uint8_t

    """
    # write table 1 string
    str_table1 = ""
    for i,(key, value) in enumerate(table):
        str_table1 += '{0x%02x, 0x%02x, 0x%02x}, ' % (((key & 0xFF00) >> 8),
                                                        (key & 0x00FF),
                                                        value)
        if(0 == ((i+1) % 4)):
            str_table1 = str_table1[:-1]
            str_table1 += '\n'
    str_table1 = str_table1[:-1]
    if(VERBOSE):
        print(id+" generated table1")
        print(str_table1)
    return str_table1


def s_table2_gen(id, table):
    """! @brief Generate table 2 for given table (list)
                ['0x04:0x14', '0x06:0x17',] which is convert to
                array {{0x04, 0x14}, {0x06, 0x17}} format which is
                compatible for cpp header
    @param id     which is type of list (allow, deny, partial)
    @param table  expected dictionary which is called as table

    @return header formatted array each element should not exceed 256 range
            which is fit in uint8_t

    """
    # write table 2 string
    str_table2 = ""
    for i,(key, value) in enumerate(table):
        str_table2 += '{0x%02x, 0x%02x}, ' % (key, value)
        if(0 == ((i+1) % 4)):
            str_table2 = str_table2[:-1]
            str_table2 += '\n'
    str_table2 = str_table2[:-1]
    if(VERBOSE):
        print(id+" generated table2")
        print(str_table2)
    return str_table2


def s_table3_gen(id, table):
    """! @brief Generate table 3 for given table (list)
                table: ['0x0000', '0x0001', '0x0002] which is convert to
                array {0x0000, 0x0001, 0x0002} format which is
                compatible for cpp header
                data of array consist bit 16-31 of the 32-bit address
    @param id    which is type of list (allow, deny, partial)
    @param table expected dictionary which is called as table

    @return header formatted array each element should not exceed 65,535 range
            which is fit in uint16_t

    """
    # write table 3 string
    str_table3 = ""
    for i,value in enumerate(table):
        str_table3 += '0x%04x, ' % (value)
        if(0 == ((i+1) % 8)):
            str_table3 = str_table3[:-1]
            str_table3 += '\n'
    str_table3 = str_table3[:-1]
    if(VERBOSE):
        print(id+" generated table3")
        print(str_table3)
    return str_table3


def s_partialallowlist_table_gen( partialallowlist):
    """! @brief Generate partial for given table (list)
    @param partialallowlist    partial list
    @return formatted array
    """
    # write partialallowlist string
    str_table = ""
    for ele in partialallowlist:
        str_table += '{0x%08x, 0x%016xull}, ' % (ele[0], ele[1])
    str_table = str_table[:-1]
    if(VERBOSE):
        print(" partialallowlist table")
        print(str_table)
    return str_table


def main(argv):
    """! @brief Main """
    try:
        opts, args = getopt.getopt(sys.argv[1:],
                "f:o:wbgidvhW:B:",
                                   ['file=', 'output=', 'allowlist', 'denylist', 'partialallowlist', 'info', 'debug', 'verbose', 'help', 'wt=', 'bt='])
    except getopt.GetoptError as err:
        exit(INVALID_USAGE, str(err))

    # Parse the command line arguments
    global DEBUG, VERBOSE, DEFAULT_FILE_NAME, GEN_FILE

    print_info = None
    wt = -1
    bt = -1
    for opt, arg in opts:
        if opt in ('-h', '--help'):
            exit(INVALID_USAGE)
        elif opt in ('-d', '--debug'):
            DEBUG = True
        elif opt in ('-v', '--verbose'):
            DEBUG = True
            VERBOSE = True
        elif opt in ('-i', '--info'):
            print_info = "version"
        elif opt in ('-w', '--allowlist'):
            print_info = "allowlist"
        elif opt in ('-b', '--denylist'):
            print_info = "denylist"
        elif opt in ('-g', '--partiallist'):
            print_info = "partialallowlist"
        elif opt in ('-f', '--file'):
            assert os.path.exists(arg), "file doesn't exist at:"+str(arg)
            DEFAULT_FILE_NAME = str(arg)
        elif opt in ('-W', '--wt'):
            wt = int(arg)
            print_info = "allowlist_table"
        elif opt in ('-B', '--bt'):
            print_info = "denylist_table"
            bt = int(arg)
        elif opt in ('-o', '--output'):
            assert os.path.exists(arg), "directory doesn't exist at:"+str(arg)
            GEN_FILE = str(arg)+"/"+GEN_FILE

    if(DEBUG):
        print("file ["+str(DEFAULT_FILE_NAME)+"]")
        print("output ["+str(GEN_FILE)+"]")

    # Read the security list file
    version   = 'unknown'
    allowlist = []
    denylist = []
    partialallowlist = []
    # Open given csv file
    with open(DEFAULT_FILE_NAME, 'r',encoding = 'unicode_escape') as f:
        reader = csv.DictReader(f)
        for idx, row in enumerate(reader):
            try:
                if(version.strip().lower() == 'unknown'):
                    version = row[TAG_VERSION]
                base_addr = row[TAG_BASE_ADDR].strip().lower().split('0x')[-1]
                # Append 0s for numbers represented by less than 8 chars
                base_addr = '0'*(8-len(base_addr))+base_addr
                # Extract the least 32 bit number for base address
                base_addr = base_addr[len(base_addr)-8:]
                base_addr = int(base_addr, 16)
                if(VERBOSE):
                    print("base["+'0x%08x' % base_addr + "]")
                bit_mask = row[TAG_BIT_MASK].strip()
                if not bit_mask:
                    bit_mask = 0
                else:
                    bit_mask = int( bit_mask.lower().split('0x')[-1], 16)

                chiplet_range = row[TAG_CHIPLET_RANGE].split('-')
                # Empty range field considered as error
                if(chiplet_range[0] == ''):
                    exit(PRINT_AND_EXIT, "Missing chiplet id range")
                if(chiplet_range[0].strip().lower() != '0x00'):
                    if(chiplet_range[0].strip().lower() != '0x%02x' % (get_chiplet(base_addr))):
                        print("base_addr",hex(base_addr))
                        print("get_chiplet(base_addr)",hex(get_chiplet(base_addr)))
                        print("chiplet_range[0]", chiplet_range[0])
                        exit(PRINT_AND_EXIT, "Base address is not consistent")
                    base_addr = base_addr & 0x00FFFFFF
                chiplet_range = [int(ele, 16) for ele in chiplet_range]
                # Expand base address with ranges
                expanded_range = []
                for ele in range(chiplet_range[0], chiplet_range[-1]+1):
                    expanded_range += [ele*(2**24)]
                expanded_line = [(base_addr + ele) for ele in expanded_range]
                if(VERBOSE):
                    print(s_list_hex("range:", expanded_range, 8))
                if(row[TAG_TYPE].strip().lower() == TAG_NAME_PARTIALALLOWLIST):
                   if(( bit_mask == 0 ) or ( bit_mask == 0xffffffffffffffff)):
                        exit(PRINT_AND_EXIT, "Wrong mask for partialallowlist")
                   partialallowlist_line = expanded_line
                   if(VERBOSE):
                        print(s_list_hex("partialallowlist_line:", partialallowlist_line, 8))
                        print("mask:", bit_mask)
                   for ele in partialallowlist_line:
                        partialallowlist.append((ele, bit_mask))
                elif(row[TAG_TYPE].strip().lower() == TAG_NAME_ALLOWLIST):
                    allowlist_line = expanded_line
                    if(VERBOSE):
                        print(s_list_hex("allowlist_line:", allowlist_line, 8))
                    allowlist += allowlist_line
                elif(row[TAG_TYPE].strip().lower() == TAG_NAME_DENYLIST):
                    denylist_line = expanded_line
                    if(VERBOSE):
                        print(s_list_hex("denylist_line:", denylist_line, 8))
                    denylist += denylist_line

            except:
                print("Error in line ["+str(idx+2)+"]")
                exit(PRINT_AND_EXIT, sys.exc_info()[0])

    allowlist = remove_duplicates(allowlist)
    allowlist = remove_zeroes(allowlist)
    allowlist.sort()
    denylist = remove_duplicates(denylist)
    denylist = remove_zeroes(denylist)
    denylist.sort()
    partialallowlist = remove_duplicates(partialallowlist)
    partialallowlist.sort()

    if(print_info == 'version'):
        exit(PRINT_AND_EXIT, "security list version ["+version+"]")
    if(print_info == 'allowlist'):
        exit(PRINT_AND_EXIT, s_list_hex("allowlist:", allowlist, 8))
    if(print_info == 'denylist'):
        exit(PRINT_AND_EXIT, s_list_hex("denylist:", denylist, 8))
    if(print_info == 'partialallowlist'):
        exit(PRINT_AND_EXIT, partialallowlist)

    if(VERBOSE):
        print(s_list_hex("allowlist:", allowlist, 8))
        print(s_list_hex("denylist:", denylist, 8))
    if(DEBUG):
        print("security list version ["+version+"]")
        print("allowlist len ["+s_list_len(allowlist)+"]")
        print("denylist len ["+s_list_len(denylist)+"]")
        print("partialallowlist len ["+s_list_len(partialallowlist)+"]")

    allowlist_tables = get_tables("allowlist", allowlist)
    denylist_tables = get_tables("denylist", denylist)

    if(print_info == 'allowlist_table'):
        exit(PRINT_AND_EXIT, "allowlist_table["+str(wt-1)+"]" + str(allowlist_tables[wt-1]))
    if(print_info == 'denylist_table'):
        exit(PRINT_AND_EXIT, "denylist_table["+str(bt-1)+"]" + str(denylist_tables[bt-1]))

    # Generate output file
    gen_file(allowlist_tables, denylist_tables, partialallowlist)

    exit(SUCCESS)

if __name__ == "__main__":
    main(sys.argv)
