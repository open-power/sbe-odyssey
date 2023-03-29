# IBM_PROLOG_BEGIN_TAG
# This is an automatically generated prolog.
#
# $Source: public/src/import/public/systems/common/hdct/pymod/dumpConstants.py $
#
# OpenPOWER sbe Project
#
# Contributors Listed Below - COPYRIGHT 2023
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
# Jason Albert - created 11/26/2019
# Python module to define shared constants

############################################################
# Imports - Imports - Imports - Imports - Imports - Imports
############################################################

############################################################
# Variables - Variables - Variables - Variables - Variables
############################################################

#########
# global
#########
# This is a global pointer to the endProgram function
# It is set by the initiating program for any modules that need access to it
# The endProgram function is unique to the initiator, so can't be
# generically common
endProgram = None

# Define the valid dump types for each type of dump
# Used to validate the cmdline --type option
# Also used to valid the dump types given in the HDCT
##########
# crondump
##########
# Constants used by crondump to encode dump data or validate the HDCT
validDumpTypes = {"ebmc" :
                  {"SCS" : 0, "CCS" : 1, "CSCX" : 2, "CCR" : 3, "CHCR" : 4, "MPIPL" : 5,
                   "HB" : 6, "PM": 7, "PERF" : 8, "CCRCLK": 9, "CHCRCLK": 10},
                  "fsp" :
                   {"HWPROC" : 0, "CS" : 1, "CC" : 2, "CCRN" : 3, "OTHER" : 4, "SAPH" : 5,
                   "HB" : 6, "SW" : 7, "SH" : 8, "USER" : 9, "HWCPLCL" : 10, "HWPERF0" : 11}}

# Dictionary to encode chipUnitType values to numerical representation in the dump
chipUnitTypeToEncode = { None : 0x0,
                         "" : 0x01,
                         "mcs" : 0x02,
                         "core" : 0x03,
                         "c" : 0x03, # Same as core intentionally
                         "eq" : 0x04,
                         "ex" : 0x05,
                         "perv" : 0x06,
                         "mcbist" : 0x07,
                         "system" : 0x08,
                         "phb" : 0x09,
                         "mi" : 0x0A,
                         "mc" : 0x0B,
                         "pauc" : 0x0C,
                         "iohs" : 0X0D,
                         "nmmu" : 0x0E,
                         "capp" : 0x0F,
                         "xbus" : 0x10,
                         "mca" : 0x11,
                         "obus" : 0x12,
                         "pec" : 0x13,
                         "mba" : 0x14,
                         "dmi" : 0x15,
                         "thread" : 0x16,
                         "mcc" : 0x17,
                         "omic" : 0x18,
                         "pau" : 0x19,
                         "omi" : 0x20,
                         "ocmb" : 0x21,
                         "mp" : 0x22,
                         }

# Dictionary to encode command values to byte representation in the dump
commandToEncode = { None : 0x00,
                    "getscom" : 0x01,
                    "putscom" : 0x02,
                    "getcfam" : 0x03,
                    "putcfam" : 0x04,
                    "getring" : 0x05,
                    "getblock" : 0x06,
                    "stopclocks" : 0x07,
                    "getgpr" : 0x08,
                    "getspr" : 0x09,
                    "getfpr" : 0x0A,
                    "gettracearray" : 0x0B,
                    "getfastarray" : 0x0C,
                    "getmempba" : 0x0D,
                    "getsram" : 0x0E,
                  }

# Dictionary to encode the real string names of our chip into their
# byte representation in the dump file
chipTypeToEncode = { None : 0x00,
                     "p9n": 0x01,
                     "p9c": 0x02,
                     "centaur": 0x03,
                     "p9a": 0x04,
                     "p10": 0x05,
                     "explorer": 0x06,
                     "exp" : 0x06,
                     "odyssey": 0x07,
                     "ody" : 0x07,
                     }

# Define a list of the common chip names we may encounter
# We then later translate that to the actual chip name
commonChipTypeNames = {"pu", "memb", "ocmb", "ody"}

# Dictionary to encode the system generations into their byte
# representations in the dump file
generationToEncode = { None : 0x00,
                       "p9" : 0x01,
                       "p10" : 0x02,
                       "p11" : 0x03,
                     }

##########
# makedump
##########
# Constants used by makedump to decode sbe dump for system dump creation

# Translate the byte defintions of the command to the string name
commandIdToString = { b'\x01' : "getscom",
                      b'\x02' : "putscom",
                      b'\x03' : "getcfam",
                      b'\x04' : "putcfam",
                      b'\x05' : "getring",
                      b'\x06' : "getblock",
                      b'\x07' : "stopclocks",
                      b'\x08' : "getgpr",
                      b'\x09' : "getspr",
                      b'\x0A' : "getfpr",
                      b'\x0B' : "gettracearray",
                      b'\x0C' : "getfastarray",
                      b'\x0D' : "getmempba",
                      b'\x0E' : "getsram",
                      }

# The byte alignment of each command type
# This is necessary to make sure we read all the data the sbe wrote
commandAlignment = { b'\x01' : 8,
                     b'\x02' : 1,
                     b'\x03' : 8,
                     b'\x04' : 1,
                     b'\x05' : 8,
                     b'\x06' : 128,
                     b'\x07' : 1,
                     b'\x08' : 1,
                     b'\x09' : 1,
                     b'\x0A' : 1,
                     b'\x0B' : 128,
                     b'\x0C' : 8,
                     b'\x0D' : 128,
                     b'\x0E' : 1,
                     }

# Translate the byte definition of the chipUnit to a string
chipUnitTypeIdToString = { b'\x00' : "chip",
                           b'\x01' : "chip",
                           b'\x02' : "mcs",
                           b'\x03' : "c",
                           b'\x04' : "eq",
                           b'\x05' : "ex",
                           b'\x06' : "perv",
                           b'\x07' : "mcbist",
                           b'\x08' : "system",
                           b'\x09' : "phb",
                           b'\x0A' : "mi",
                           b'\x0B' : "mc",
                           b'\x0C' : "pauc",
                           b'\x0D' : "iohs",
                           b'\x0E' : "nmmu",
                           b'\x0F' : "capp",
                           b'\x10' : "xbus",
                           b'\x11' : "mca",
                           b'\x12' : "obus",
                           b'\x13' : "pec",
                           b'\x14' : "mba",
                           b'\x15' : "dmi",
                           b'\x16' : "thread",
                           b'\x17' : "mcc",
                           b'\x18' : "omic",
                           b'\x19' : "pau",
                           b'\x20' : "omi",
                           b'\x21' : "ocmb",
                           b'\x22' : "mp",
                         }

# Translate the byte definition of the chipType to a string
chipTypeIdToString = { b'\x00' : None,
                     b'\x01' : "p9n",
                     b'\x02' : "p9c",
                     b'\x03' : "centaur",
                     b'\x04' : "p9a",
                     b'\x05' : "p10",
                     b'\x06' : "explorer",
                     b'\x07' : "odyssey",
                     }

# Translate generation to string
generationIdToString = { b'\x00' : None,
                         b'\x01' : "p9",
                         b'\x02' : "p10",
                         b'\x03' : "p11",
                       }

# Translate environment to string
environmentIdToString = { b'\x00' : None,
                          b'\x01' : "hw",
                          b'\x02' : "sim",
                          b'\x03' : "offline",
                        }

# Translate the byte definition of dumpType to string
dumpTypeIDToString = {"ebmc" :
                  {b'\x00' :"SCS", b'\x01' :"CCS", b'\x02' :"CSCX",
                  b'\x03':"CCR", b'\x04' :"CHCR", b'\x05' :"MPIPL",
                   b'\x06' :"HB", b'\x07' :"PM", b'\x08' :"PERF", b'\x09'
                   :"CCRCLK", b'\x0A' :"CHCRCLK"},
                  "fsp" :
                   {b'\x00' :"HWPROC", b'\x01' :"CS", b'\x02' :"CC", b'\x03'
                   :"CCRN", b'\x04' :"OTHER", b'\x05' :"SAPH",
                   b'\x06' :"HB", b'\x07' :"SW", b'\x08' :"SH", b'\x09' :"USER",
                   b'\x0A' :"HWCPLCL", b'\x0B' :"HWPERF0"}}

# Dictionary to encode SPR Name with its Number to extract architected reg info.
# Keep the dictionary in the sorted order.parseMpipl is dependent on that.
sprToEncode ={"XER"      :      1,
              "DSCR_RU"  :      3,
              "LR"       :      8,
              "CTR"      :      9,
              "UAMR"     :      13,
              "DSCR"     :      17,
              "DSISR"    :      18,
              "DAR"      :      19,
              "DEC"      :      22,
              "SRR0"     :      26,
              "SRR1"     :      27,
              "CFAR"     :      28,
              "AMR"      :      29,
              "PIDR"     :      48,
              "IAMR"     :      61,
              "TFHAR"    :      128,
              "TFIAR"    :      129,
              "TEXASR"   :      130,
              "TEXASRU"  :      131,
              "CTRL_RU"  :      136,
              "CTRL"     :      152,
              "FSCR"     :      153,
              "UAMOR"    :      157,
              "GSR"      :      158,
              "PSPB"     :      159,
              "DPDES"    :      176,
              "DAWR0"    :      180,
              "DAWR1"    :      181,
              "RPR"      :      186,
              "CIABR"    :      187,
              "DAWRX0"   :      188,
              "DAWRX1"   :      189,
              "HFSCR"    :      190,
              "VRSAVE"   :      256,
              "SPRG3_RU" :      259,
              "TB"       :      268,
              "TBU_RU"   :      269,
              "SPRG0"    :      272,
              "SPRG1"    :      273,
              "SPRG2"    :      274,
              "SPRG3"    :      275,
              "SPRC"     :      276,
              "SPRD"     :      277,
              "TBL"      :      284,
              "TBU"      :      285,
              "TBU40"    :      286,
              "PVR"      :      287,
              "HSPRG0"   :      304,
              "HSPRG1"   :      305,
              "HDSISR"   :      306,
              "HDAR"     :      307,
              "SPURR"    :      308,
              "PURR"     :      309,
              "HDEC"     :      310,
              "HRMOR"    :      313,
              "HSRR0"    :      314,
              "HSRR1"    :      315,
              "TFMR"     :      317,
              "LPCR"     :      318,
              "LPIDR"    :      319,
              "HMER"     :      336,
              "HMEER"    :      337,
              "PCR"      :      338,
              "HEIR"     :      339,
              "AMOR"     :      349,
              "TIR"      :      446,
              "HDEXCR_RU":      455,
              "PTCR"     :      464,
              "HDEXCR"   :      471,
              "SIERA_RU" :      736,
              "SIERB_RU" :      737,
              "MMCR3_RU" :      738,
              "SIERA"    :      752,
              "SIERB"    :      753,
              "MMCR3"    :      754,
              "SIER_RU"  :      768,
              "MMCR2_RU" :      769,
              "MMCRA_RU" :      770,
              "PMC1_RU"  :      771,
              "PMC2_RU"  :      772,
              "PMC3_RU"  :      773,
              "PMC4_RU"  :      774,
              "PMC5_RU"  :      775,
              "PMC6_RU"  :      776,
              "MMCR0_RU" :      779,
              "SIAR_RU"  :      780,
              "SDAR_RU"  :      781,
              "MMCR1_RU" :      782,
              "SIER"     :      784,
              "MMCR2"    :      785,
              "MMCRA"    :      786,
              "PMC1"     :      787,
              "PMC2"     :      788,
              "PMC3"     :      789,
              "PMC4"     :      790,
              "PMC5"     :      791,
              "PMC6"     :      792,
              "MMCR0"    :      795,
              "SIAR"     :      796,
              "SDAR"     :      797,
              "MMCR1"    :      798,
              "IMC"      :      799,
              "BESCRS"   :      800,
              "BESCRSU"  :      801,
              "BESCRR"   :      802,
              "BESCRRU"  :      803,
              "EBBHR"    :      804,
              "EBBRR"    :      805,
              "BESCR"    :      806,
              "DEXCR_RU" :      812,
              "TAR"      :      815,
              "ASDR"     :      816,
              "PSSCR_SU" :      823,
              "DEXCR"    :      828,
              "IC"       :      848,
              "VTB"      :      849,
              "LDBAR"    :      850,
              "MMCRC"    :      851,
              "PMSR"     :      853,
              "PSSCR"    :      855,
              "L2QOSR"   :      861,
              "TRIG0"    :      880,
              "TRIG1"    :      881,
              "TRIG2"    :      882,
              "PMCR"     :      884,
              "RWMR"     :      885,
              "WORT"     :      895,
              "PPR"      :      896,
              "PPR32"    :      898,
              "TSCR"     :      921,
              "TTR"      :      922,
              "TRACE"    :      1006,
              "HID"      :      1008,
              "PIR"      :      1023,
              "NIA"      :      2000,
              "MSRD"     :      2001,
              "CR"       :      2002,
              "FPSCR"    :      2003,
              "VSCR"     :      2004,
              "MSR"      :      2005,
              "MSR_L1"   :      2006,
              "MSRD_L1"  :      2007,
             }
