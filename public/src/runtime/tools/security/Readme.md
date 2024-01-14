# Readme

## Security Register list generator

[securityreglistgen](./securityreglistgen.py) incorporates a specialized
algorithm designed to store the addresses provided in a csv file to an array, thereby enhancing search operation and reducing memory footprint.

![Security reg list gen allow deny list flow chart](<AllowDenyListFlow.png>)


## How to segregate allow list, deny list & partial list?

Allow list, Deny list and partial list are part of the security validation of any
register. The script will parse the data from the given csv file. Below step are followed
to parse:
* Read data line by line(row by row) from CSV
* Segregate the ALLOW, DENY & PARTIAL lsit data based on the TAGS added for each row.
* Remove all duplicates
* sort the ALLOW, DENY & PARTIAL data for binary search


## How are the tables created?


Allow list , Deny List and Partial Allow List are part of the Security validation
of any register addresses. Each list has been sub divided into 3 tables [t1, t2 and
t3] as mentioned bellow. Table data will be used on security algorithm to
validates register address is part of Allow list, Deny List and Partial Allow List.

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


**Table T1**, contains the 3 element in each array `{{0x00, 0x00, 0x04}, {0x02, 0x02, 0x05}}`
First two element is called key and last element is called value.
       keys   = 2byte - (start , end) of
                the ranges in bit 0-7 of the 32-bit address
       values = running count of the paths to table2
```
    for example - if ranges are 0x20-0x37, 0x01-0x01, 0x10-0x17 and has
                  1, 2 and 3 paths respectively to table 2
    then table 1 will have {0x20,0x37} = 01, {0x01,0x01} = 3, {0x10,0x17} = 7

    1 byte for running count - we are good with uint8_t till the
    total paths are less than 256
```

**Table T2**, contains the 2 element in each array `{{0x04, 0x14}, {0x06, 0x17}}`
First one element is called key and last element is called value.
       keys   = unique 1 byte numbers having same prefix for each
                range in table 1 key - bit 8-15 from a 32-bit address
       values = running count of paths from each of the values to table3
```
    for example - if element a has 1 path, b has 0 and c has 3 path
    then table 1 will have `a = 1, b = 2, c = 4`

    1 byte for key
    2 byte for number of paths
    We are good with uint16_t,
    till the number of paths to table 3 from each key is less than 65536
```

**Table T3**, one dimensional array which contains only values `{0x0000, 0x0001,
 0x0002, 0x0003}`
```
 values = 2 byte value bit 16-31 of the 32-bit address
```

`Note: Values in table which is not contain repeatable address and all values
are sorted, which make ease to search (binary search) values in Cpp code`

### Example 1
Data pass to the gen_table: `[0x1abcdef, 0x2abcdef, 0x4abcdef, 0x5abcdef,
0x10456789, 0x11456789]`
Cmd used: `./public/src/runtime/tools/security/securityreglistgen.py -f
../P10_Security_Allow_Deny_list.csv --output ./`
output:
```log
########################## Step 1 ################################

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

ID:  allowlist
All address:  0x1abcdef, 0x2abcdef, 0x4abcdef, 0x5abcdef, 0x10456789, 0x11456789
------------------------------------------------------------------
Base addr list:  0x456789, 0xabcdef
------------------------------------------------------------------
Step 1 table:  {4548489: [4113], 11259375: [258, 1029]}
------------------------------------------------------------------
Base address:  0x456789  Chiplet list:  0x1011
Base address:  0xabcdef  Chiplet list:  0x102, 0x405
------------------------------------------------------------------
########################## Step 2 ################################

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

------------------------------------------------------------------
Step 2 table:  {4113: [4548489], 258: [11259375], 1029: [11259375]}
------------------------------------------------------------------
chiplet range:  0x1011 base address list:  0x456789
chiplet range:  0x102 base address list:  0xabcdef
chiplet range:  0x405 base address list:  0xabcdef
------------------------------------------------------------------
########################## Step 3 ################################

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

------------------------------------------------------------------
step 3 table:  [(4113, [(69, [26505])]), (258, [(171, [52719])]),
                (1029, [(171, [52719])])]
------------------------------------------------------------------
chiplet range:  0x1011
1St byte of Base address:  0x45
last 2 bytes of base address:  0x6789
------------------------------------------------------------------
chiplet range:  0x102
1St byte of Base address:  0xab
last 2 bytes of base address:  0xcdef
------------------------------------------------------------------
chiplet range:  0x405
1St byte of Base address:  0xab
last 2 bytes of base address:  0xcdef
------------------------------------------------------------------
------------------------------------------------------------------
########################## Step 4 ################################

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

------------------------------------------------------------------
allowlist table3 keys len [3]
------------------------------------------------------------------
allowlist table1: ['0x1011:0x01', '0x0102:0x02', '0x0405:0x03']
allowlist table2: ['0x45:0x01', '0xab:0x02', '0xab:0x03']
allowlist table3: ['0x6789', '0xcdef', '0xcdef']
------------------------------------------------------------------
allowlist table1 len [3]
allowlist table2 len [3]
allowlist table3 len [3]
```


### Example 2:
Data passed: `[0x1abcdef, 0x2abcdef, 0x4abcdef, 0x5abcdef, 0x4456789, 0x5456789,
0x10456789, 0x11456789]`

```log
########################## Step 1 ################################

    # -----------------------------------------------------------------#
    # Step 1:             [register list]                              #
    #                           ||                                     #
    #                           \/                                     #
    #         {base_address1 : [chiplet range1, chiplet range2]}       #
    #..................................................................#
    # Eg: [01ABCDEF, 02ABCDEF, 04ABCDEF, 05ABCDEF, 10UVWXYZ, 11UVWXYZ, #
    #      04UVWXYZ, 05UVWXYZ]                                         #
    #                           ||                                     #
    #                           \/                                     #
    #         {ABCDEF : [0102, 0405], UVWXYZ : [0405, 1011]}           #
    # -----------------------------------------------------------------#

ID:  allowlist
All address:  0x1abcdef, 0x2abcdef, 0x4abcdef, 0x5abcdef, 0x4456789, 0x5456789,
              0x10456789, 0x11456789
------------------------------------------------------------------
Base addr list:  0x456789, 0xabcdef
------------------------------------------------------------------
Step 1 table:  {4548489: [1029, 4113], 11259375: [258, 1029]}
------------------------------------------------------------------
Base address:  0x456789  Chiplet list:  0x405, 0x1011
Base address:  0xabcdef  Chiplet list:  0x102, 0x405
------------------------------------------------------------------
########################## Step 2 ################################

    # -----------------------------------------------------------------#
    # Step 2: {base_address1 : [chiplet range1, chiplet range2]}       #
    #                           ||                                     #
    #                           \/                                     #
    #         {chiplet range1 : [base_address1, base_address2]}        #
    #..................................................................#
    # Eg:     {ABCDEF : [0102, 0405], UVWXYZ : [0405, 1020]}           #
    #                           ||                                     #
    #                           \/                                     #
    #     {0102 : [ABCDEF], 0405 : [ABCDEF, UVWXYZ], 1011 : [UVWXYZ]}  #
    # -----------------------------------------------------------------#

------------------------------------------------------------------
Step 2 table:  {1029: [4548489, 11259375], 4113: [4548489], 258: [11259375]}
------------------------------------------------------------------
chiplet range:  0x405 base address list:  0x456789, 0xabcdef
chiplet range:  0x1011 base address list:  0x456789
chiplet range:  0x102 base address list:  0xabcdef
------------------------------------------------------------------
########################## Step 3 ################################

    # -----------------------------------------------------------------#
    # Step 3: {chiplet range1 : [base_address1, base_address2]}        #
    #                           ||                                     #
    #                           \/                                     #
    #         {chiplet range1 : {key1 : [base_addr1, base_Addr2]}}     #
    #..................................................................#
    # Eg: {0102 : [ABCDEF], 0405 : [ABCDEF, UVWXYZ], 1011 : [UVWXYZ]}  #
    #                           ||                                     #
    #                           \/                                     #
    #               {0102 : {AB : [CDEF]},                             #
    #                0405 : {AB : [CDEF], UV: [WXYZ]},                 #
    #                1011 : {UV : [WXYZ]}}                             #
    # -----------------------------------------------------------------#

------------------------------------------------------------------
step 3 table:  [(1029, [(69, [26505]), (171, [52719])]),
                (4113, [(69, [26505])]), (258, [(171, [52719])])]
------------------------------------------------------------------
chiplet range:  0x405
1St byte of Base address:  0x45
last 2 bytes of base address:  0x6789
------------------------------------------------------------------
chiplet range:  0x405
1St byte of Base address:  0xab
last 2 bytes of base address:  0xcdef
------------------------------------------------------------------
chiplet range:  0x1011
1St byte of Base address:  0x45
last 2 bytes of base address:  0x6789
------------------------------------------------------------------
chiplet range:  0x102
1St byte of Base address:  0xab
last 2 bytes of base address:  0xcdef
------------------------------------------------------------------
------------------------------------------------------------------
########################## Step 4 ################################

    # -----------------------------------------------------------------#
    # Step 4: {chiplet range1 : {key1 : [base_addr1, base_Addr2]}}     #
    #                           ||                                     #
    #                           \/                                     #
    #                  table1, table2, table3                          #
    #..................................................................#
    # Eg:           {0102 : {AB : [CDEF]},                             #
    #                0405 : {AB : [CDEF], UV: [WXYZ]},                 #
    #                1011 : {UV : [WXYZ]}}                             #
    #                           ||                                     #
    #                           \/                                     #
    #         table1: {0102 : 1, 0405 : 3, 1011 : 4}                   #
    #         table2: {AB : 1, AB : 2, UV : 3, UV : 4}                 #
    #         table3: {CDEF, CDEF, WXYZ, WXYZ}                         #
    # -----------------------------------------------------------------#

------------------------------------------------------------------
allowlist table3 keys len [4]
------------------------------------------------------------------
allowlist table1: ['0x0405:0x02', '0x1011:0x03', '0x0102:0x04']
allowlist table2: ['0x45:0x01', '0xab:0x02', '0x45:0x03', '0xab:0x04']
allowlist table3: ['0x6789', '0xcdef', '0x6789', '0xcdef']
------------------------------------------------------------------
allowlist table1 len [3]
allowlist table2 len [4]
allowlist table3 len [4]
------------------------------------------------------------------
```
