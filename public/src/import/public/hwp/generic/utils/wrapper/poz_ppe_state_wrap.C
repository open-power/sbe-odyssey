/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/hwp/generic/utils/wrapper/poz_ppe_state_wrap.C $ */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2023                             */
/* [+] International Business Machines Corp.                              */
/*                                                                        */
/*                                                                        */
/* Licensed under the Apache License, Version 2.0 (the "License");        */
/* you may not use this file except in compliance with the License.       */
/* You may obtain a copy of the License at                                */
/*                                                                        */
/*     http://www.apache.org/licenses/LICENSE-2.0                         */
/*                                                                        */
/* Unless required by applicable law or agreed to in writing, software    */
/* distributed under the License is distributed on an "AS IS" BASIS,      */
/* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or        */
/* implied. See the License for the specific language governing           */
/* permissions and limitations under the License.                         */
/*                                                                        */
/* IBM_PROLOG_END_TAG                                                     */
/********************************************************************
///
/// @file  poz_ppe_state_wrap.C
/// @brief Wrapper for Dump PPE State
///
/// *HWP HW Owner        : Greg Still <stillgs.@us.ibm.com>
/// *HWP HW Backup Owner : Yue Du <daviddu@us.ibm.com>
/// *HWP FW Owner        : Prasad BG Ranganath <prasadbgr@in.ibm.com>
/// *HWP Team            : PM
/// *HWP Level           : 2
/// *HWP Consumed by     : SBE, Cronus

/// @verbatim
///
/// Procedure Summary:
///   - Dump out PPE's internal state
///
/// @endverbatim
*********************************************************************/

// Includes
#include <fapi2.H>
#include <fapi2ClientCapi.H>
#include <ecmdClientCapi.H>
#include <ecmdSharedUtils.H>
#include <ecmdUtils.H>
#include <fstream>
#include <iostream>
#include <sstream>
#include <map>

#include <poz_ppe_state.H>
#include <poz_mib_state.H>
#include <poz_ppe_other_regs.H>
#include <poz_ppe_utils.H>

static std::string procedureName = "poz_ppe_state";

void help()
{
    char helpstr[256];
    sprintf(helpstr, "\nThis is the help text for the procedure %s: \n\n",
            procedureName.c_str());
    ecmdOutput("helpstr\n");
    ecmdOutput("Usage: poz_ppe_state_wrap chip [-sbe] [-gpe <0 ..3> ][-qme <0 ..7> ][-ce <0 ..3> ] [-ioppe <0 ..3> ][-snapshot] [-halt] [-force_halt] [-verbose] [-verbose+]  [-verif]\n");
    ecmdOutput("  Option flags are:\n");
    ecmdOutput("       chip         Chip type to access. Use -pX to choose which chips.\n");
    ecmdOutput("                      ph   - P11 Spinal Hub chip. Engines: SPPE, SBE, IOPPE, OCC GPEs\n");
    ecmdOutput("                      pc   - P11 Tap Compute chip. Engines: SBE, IOPPE, QME, TCC CEs\n");
    ecmdOutput("                      ody  - Odyssey Memory Controller. Engines: SPPE, IOPPE\n");
    ecmdOutput("      -sbe          Dump the SBE state registers.\n");
    ecmdOutput("      -sppe         Dump the SPPE state registers.\n");
    ecmdOutput("      -pme          Dump the PME state registers.\n");
    ecmdOutput("      -ioppe <0..3> Dump the respective IOPPE state registers.\n");
    ecmdOutput("      -gpe <0..3>   Dump the respective GPE state registers.\n");
    ecmdOutput("      -ce <0..3>    Dump the respective CE state registers.\n");
    ecmdOutput("      -qme <0..7>   Dump the QME state registers specified by QME index(0 to 7).\n");
    ecmdOutput("      -vdrs         Display the VDR contents\n");
    ecmdOutput("      -mib          Dump the contents of respective Memory bolton \n");
    ecmdOutput("      -other_state  Dump the contents of all other important state registers \n");
    ecmdOutput("                      EG : If QMEs,  Dump the contents stop state , control regs, interrupt regs \n");
    ecmdOutput("                         If GPEs,  Dump the contents of debug and status registers \n");
    ecmdOutput("      -snapshot    Dump contents of all registers of the engine.Engine will be halted and resumed if it was already running \n");
    ecmdOutput("      -halt        Dump contents of all registers after halting the engine. No resume will be done.\n");
    ecmdOutput("      -force_halt  Dump contents of all registers after force halting the engine. No resume will be done.\n");
    ecmdOutput("      -xirs        Dump contents of only xirs .\n");
    ecmdOutput("      -verbose     Verbose logging.\n");
    ecmdOutput("      -verbose+    Verbose plus logging : more details on register bit description.\n");
    ecmdOutput("      -verif       Run procedure in sim verification mode (default = false).\n");
    ecmdOutput("      -input_file  Optional input file to read the data instead of doing actual scoms on a target \n");
    ecmdOutput("                   EG : The input file is the text file with register names and addresses \n");
    ecmdOutput("      -p# - Proc postition default = 0).\n");
    ecmdOutput("      -n# - Node postition default = 0).\n");
}

std::vector<RegField_t>  v_empty_DESC = {};

//FOR PPE
std::vector<RegField_t>  v_XSR_DESC =
{
    {false, 0, 0, "HS", {"PPE Not Halted", "PPE Halted"}},
    {false, 1, 3, "HC", {"None", "XCR[CMD] Written '111'", "WDT", "Unmaskable Interrupt", "Debug", "DBCR", "Halt_Req", "Hardware Failure"}},
    {false, 4, 4, "HCP", {"No Halt Condition", "Halt Condition Present"}},
    {true, 5, 5, "RIP", {"", "Ramming In Progress"}},
    {true, 6, 6, "SIP", {"", "Single-step In Progress"}},
    {true, 7, 7, "TRAP", {"", "Trap Instruction Debug Event Occurred"}},
    {true, 8, 8, "IAC", {"", "Instruction Address Compare Event Occurred"}},
    {true, 12, 12, "DACR", {"", "Data Address Compare (Read) Event Occurred"}},
    {true, 13, 13, "DACW", {"", "Data Address Compare (Write) Event Occurred"}},
    {true, 14, 14, "WS", {"", "PPE Is In Wait State"}},
    {false, 15, 15, "TRH", {"Timers Freeze When Halted", "Timers Run When Halted"}},
    {true, 16, 19, "SMS", {"State Machine State"}},
    {true, 20, 20, "LP", {"Low Priority"}},
    {true, 21, 21, "EP", {"", "Maskable Event Pending"}},
    {false, 24, 24, "PTR", {"Program Interrupt Caused By Illegal Instruction", "Program Interrupt From Trap"}},
    {false, 25, 25, "ST", {"Datainterrupt Caused by A Load", "Data Interrupt Caused By A Store"}},
    {true, 28, 28, "MFE", {"", "Multiple Fault Error"}},
    {
        false, 29, 31, "MCS", {"Instruction Mck",
            "Data Machine Check - Load",
            "Data Machine Check - Precise Store",
            "Data Machine Check - Imprecise Store",
            "Program Interrupt, Promoted",
            "Instruction Storage Interrupt, Promoted",
            "Alignment Interrupt, Promoted",
            "Data Storage Interrupt, Promoted"
        }
    }
};

std::vector<RegField_t>  v_ISR_DESC =
{
    {true, 21, 21, "EP", {"", "MSR[EE] maskable interrupt pending ; Any of these :Ext Interrupt/Dec/FIT/Watchdog"}},
    {false, 24, 24, "PTR", {"Program interrupt caused by illegal instruction", "Program interrupt from trap"}}, //special case
    {false, 25, 25, "ST", {"Datainterrupt caused by a load", "Datainterrupt caused by a store"}}, //special case
    {true, 28, 28, "MFE", {"", "Multiple Fault Error"}},
    {
        false, 29, 31, "MCS", {"Instruction Mck", //special case (as default can be Instruction MCK)
            "Data Machine Check - Load",
            "Data Machine Check - Precise Store",
            "Data Machine Check - Imprecise Store",
            "Program Interrupt, Promoted",
            "Instruction Storage Interrupt, Promoted",
            "Alignment Interrupt, Promoted",
            "Data Storage Interrupt, Promoted"
        }
    }
};


std::vector<RegField_t>  v_MSR_DESC =
{
    {true, 1, 7, "SEM", {"SIB Error Mask"}},
    {true, 8, 8, "IS0", {"", "Instance-specific Field 0-Data cache populate(Applicable for GPE only)"}},
    //{true, 9, 11, "SIBRC", {"Last Sib Return Code"}},
    {
        false, 9, 11, "SIBRC", {"No Error",
            "XSCOM command blocked due to pending errors, resource in use. Retry later or try to unlock",
            "Chiplet Offline",
            "Partial Good",
            "Invalid Address, Address Error, Access Error (bad access type)",
            "Clock Error",
            "Address Parity Error, Data Parity Error, Unexpected Packet, Wrong Packet Number, Protocol Two-phase Handshake Error",
            "Time-out"
        }
    },
    {true, 13, 13, "WE", {"", "PPE Is In Wait State"}},
    {true, 14, 14, "IS1", {"", "Instance-specific Field 1-store gather enablement(Also needs imprecise mode to be set)"}},
    {true, 15, 15, "UIE", { "Unmaskable Interrupt Enable"}},
    {true, 16, 16, "EE", { "External Enable"}},
    {true, 19, 19, "ME", { "Machine Check Enable"}},
    {true, 20, 20, "IS2", {"", "Instance-specific Field 2 -Icache Prefetch(Applicable for GPE only)"}},
    {true, 21, 21, "IS3", {"", "Instance-specific Field 3 -Unused"}},
    {false, 23, 23, "IPE", {"Imprecise Mode is disabled", "Imprecise Mode Is Enabled"}},
    //{true, 24, 31, "SIBRCA", {"Last Sib Return Code Accumulator"}}
    {true, 24, 24, "SIBRCA[0]", {"", "No Error"}},
    {true, 25, 25, "SIBRCA[1]", {"", "XSCOM command blocked due to pending errors, resource in use. Retry later or try to unlock"}},
    {true, 26, 26, "SIBRCA[2]", {"", "Chiplet Offline"}},
    {true, 27, 27, "SIBRCA[3]", {"", "Partial Good"}},
    {true, 28, 28, "SIBRCA[4]", {"", "Invalid Address, Address Error, Access Error (bad access type)"}},
    {true, 29, 29, "SIBRCA[5]", {"", "Clock Error"}},
    {true, 30, 30, "SIBRCA[6]", {"", "Address Parity Error, Data Parity Error, Unexpected Packet, Wrong Packet Number, Protocol Two-phase Handshake Error"}},
    {true, 31, 31, "SIBRCA[7]", {"", "Time-out"}},

};


std::vector<RegField_t>  v_TSR_DESC =
{
    {true, 0, 0, "ENW", {"", "Enable Next Watchdog =1"}},
    {true, 1, 1, "WIS-Watchdog Timer Interrupt Status", {"", "Watchdog timer event has occurred"}},
    {false, 2, 3, "WRS-Watchdog Timer Reset Status", {"No reset", "Soft reset", "hard reset", "halted" }},
    {true, 4, 4, "DIS-Decrementer Interrupt Status" , {"", "Decrementer event has occurred"}},
    {true, 5, 5, "FIS-FIT Interrupt Status" , {"", "FIT event has occurred"}},
    {
        false, 2, 3, "WRS Watchdog Timer Reset Status", {"No Watchdog timer reset has occurred",
            "Last Watchdog action was a soft reset",
            "Last Watchdog action was a hard reset",
            "Last Watchdog action halted the processor"
        }
    }
};


//std::vector<RegField_t>  v_IR_DESC =
//{
//             {false,0,0, "IR contains the last instruction fetched (but not necessarily executed).", {}}
//};
//FOR MIB
std::vector<RegField_t>  v_SIB_INFO_DESC =
{

    {
        true, 0, 31, "MIB_XISIB_PIB_ADDR", {
            "PIB transaction buffer current or previous transaction byte address"
        }
    },

    {
        true, 32, 32, "MIB_XISIB_PIB_R_NW", {
            "PIB transaction buffer current or previous transaction type, read if '1' and write if '0'."
        }
    },

    {
        true, 33, 33, "MIB_XISIB_PIB_BUSY", {
            "Indicates if the transaction buffer is occupied with an ongoing transaction.  Busy is cleared when the transaction is completed."
        }
    },

    {
        true, 34, 34, "MIB_XISIB_PIB_IMPRECISE_ERROR_PENDING", {
            "Indicates that the current or previous transaction had an imprecise error. Cleared when it is reported back to PPE. Note: The address of the erroneous\n\
                    transaction is copied into the SGB address latches such that the PIB interface can continue to service I-fetches (only in case of SBE instance)."
        }
    },

    // {
    //     true, 49, 51, "MIB_XISIB_PIB_RSP_INFO", {
    //         "PIB transaction buffer response info. Current or previous transaction got an error on the PIB interface when non-zero."
    //     }
    // },
    {
        false, 49, 51, "MIB_XISIB_PIB_RSP_INFO", {"No Error",
            "XSCOM command blocked due to pending errors, resource in use. Retry later or try to unlock",
            "Chiplet Offline",
            "Partial Good",
            "Invalid Address, Address Error, Access Error (bad access type)",
            "Clock Error",
            "Address Parity Error, Data Parity Error, Unexpected Packet, Wrong Packet Number, Protocol Two-phase Handshake Error",
            "Time-out"
        }
    },


    {
        true, 62, 62, "MIB_XISIB_PIB_IFETCH_PENDING", {
            "Indicates a instruction fetch is pending on the PIB interface when set to '1'."
        }
    },

    {
        true, 63, 63, "MIB_XISIB_PIB_DATAOP_PENDING", {
            "Indicates a data transaction is pending on the PIB interface when set to '1'."
        }
    },
};


bool is_interrupt_handler(uint32_t iar, uint32_t ivpr, uint32_t type)
{

    return ( (iar == (ivpr | type))
//       ||( (iar > (ivpr | type)) && (iar < ((ivpr | type) + 0x20)) )
// Uncomment above line if we also want to check if Iar is within interrupt routine range(32 bytes i.e, 0x20 )
           );
}

//PPE Reg decode function
std::string decode_ppe_REG( Reg32Value_t& i_reg_val,
                            uint8_t GOT_MCHK,
                            uint8_t GOT_PRG,
                            uint8_t PRG_PROMOTED,
                            uint8_t GOT_DSI,
                            uint8_t DSI_PROMOTED,
                            uint8_t GOT_ALIG,
                            uint8_t ALIG_PROMOTED,
                            uint8_t GOT_ANY_INTR
                          )
{

    std::string o_str;
    fapi2::buffer<uint32_t> tmp_data = 0;
    fapi2::buffer<uint16_t> l_data16;
    char l_buff[255];
    //For every iteration, define an empty vector(that will be the defaultcase value)
    //std::vector<RegField_t>* l_ppe_reg = new std::vector<RegField_t>();
    std::vector<RegField_t>* l_ppe_reg ;
    l_ppe_reg = &v_empty_DESC;

    if (i_reg_val.number == v_ppe_reg_number[PPE_XSR])
    {
        l_ppe_reg = &v_XSR_DESC;
    }
    else if (i_reg_val.number == v_ppe_reg_number[PPE_MSR])
    {
        l_ppe_reg = &v_MSR_DESC;
    }
    else if (i_reg_val.number == v_ppe_reg_number[PPE_ISR])
    {
        l_ppe_reg = &v_ISR_DESC;
    }
    else if (i_reg_val.number == v_ppe_reg_number[PPE_SRR1])
    {
        if(GOT_ANY_INTR)
        {
            l_ppe_reg = &v_MSR_DESC;  //If any interrupt MSR value during that interrupt  will be copied into SRR1
        }
    }
    else if (i_reg_val.number == v_ppe_reg_number[PPE_TSR])
    {
        l_ppe_reg = &v_TSR_DESC;
    }



    o_str = "";

    for (auto it : *l_ppe_reg)
    {
        // Extract the field
        tmp_data = i_reg_val.value;
        tmp_data.extractToRight(l_data16.flush<0>(), it.startBit, it.endBit - it.startBit + 1);

        //Handling special cases here
        if(!strcmp(it.description, "MCS") && l_data16 == 0)
        {
            if(GOT_MCHK == 1)
            {
                snprintf(l_buff, 256, "       %s[%d:%d] = %s(value=0x%X  -%s)   \n", i_reg_val.name, it.startBit, it.endBit,
                         it.description, l_data16(), it.enumDescription[l_data16()]);
            }

            o_str += l_buff;
            memset(l_buff, '\0', sizeof(l_buff));
            continue;
        }

        if(!strcmp(it.description, "PTR") && l_data16 == 0)
        {
            if(GOT_PRG | PRG_PROMOTED)
            {
                snprintf(l_buff, 256, "       %s[%d:%d] = %s(value=0x%X  -%s)   \n", i_reg_val.name, it.startBit, it.endBit,
                         it.description, l_data16(), it.enumDescription[l_data16()]);
            }

            o_str += l_buff;
            memset(l_buff, '\0', sizeof(l_buff));

            continue;
        }

        if(!strcmp(it.description, "ST") && l_data16 == 0  )
        {
            if(GOT_DSI | DSI_PROMOTED | GOT_ALIG | ALIG_PROMOTED)
            {
                snprintf(l_buff, 256, "       %s[%d:%d] = %s(value=0x%X  -%s)   \n", i_reg_val.name, it.startBit, it.endBit,
                         it.description, l_data16(), it.enumDescription[l_data16()]);
            }

            o_str += l_buff;
            memset(l_buff, '\0', sizeof(l_buff));

            continue;
        }

        //IF Enum Descr size is > 1 and check is true then check for that bit and then only print
        //This is applicable for only single bit checking(Reg table will only have "true" switch for single bit )
        if ((it.enumDescription.size() > 1 ) and
            (it.check_one && tmp_data.getBit(it.startBit, 1))  )
        {

            snprintf(l_buff, 1024, "       %s[%d:%d] = %s(value=0x%X  -%s)   \n", i_reg_val.name, it.startBit,
                     it.endBit,
                     it.description, l_data16(), it.enumDescription[l_data16()]);

        }
        //IF enum Desc size is > 1 and check is false then print respective description
        else if((it.enumDescription.size() > 1) and !(it.check_one)) //Just print the description

        {
            snprintf(l_buff, 1024, "       %s[%d:%d] = %s(value=0x%X  -%s)   \n", i_reg_val.name, it.startBit,
                     it.endBit,
                     it.description, l_data16(), it.enumDescription[l_data16()]);

        }
        //If enum Desc size = 1 and check is true then print that description along with value
        else if((it.enumDescription.size() == 1) and (it.check_one))
        {
            snprintf(l_buff, 1024, "       %s[%d:%d] = %s(value=0x%X  -%s)   \n", i_reg_val.name, it.startBit,
                     it.endBit,
                     it.description, l_data16(), it.enumDescription[0]);
        }
        //If enum Desc size = 1 and check is false then print that description without value(used for register descriptions)
        else if((it.enumDescription.size() == 1) and !(it.check_one))
        {
            snprintf(l_buff, 1024, "       %s(: %s)   \n", it.description, it.enumDescription[0]);
        }

        o_str += l_buff;
        memset(l_buff, '\0', sizeof(l_buff));
    }


    return o_str;
}


/**
 * @brief Display  register content on stdout
 * @param[in]  v_ppe_minor_sprs_value   input PPE regs value
 * @param[in]  v_ppe_major_sprs_value   input PPE regs value
 * @param[in]  v_ppe_xirs_value   input PPE regs value
 * @param[in]  v_ppe_special_sprs_value   input PPE regs value
 */
fapi2::ReturnCode
ppe_state_print_verbose(
    std::vector<Reg32Value_t>& v_ppe_sprs_value,
    std::vector<Reg32Value_t>& v_ppe_xirs_value)
{


    uint32_t    l_saved_iar = 0;
    bool l_xir_ava = false;
    bool l_ivpr_ava = false;
    uint32_t    l_saved_ivpr = 0;
    uint8_t GOT_MCHK = 0;
    uint8_t GOT_SRST = 0;
    uint8_t GOT_DSI = 0;
    uint8_t GOT_ALIG = 0;
    uint8_t GOT_PRG = 0;
    uint8_t PRG_PROMOTED = 0;
    uint8_t GOT_DATAMCHK = 0;
    uint8_t DSI_PROMOTED = 0;
    uint8_t ALIG_PROMOTED = 0;
    uint8_t GOT_ISI = 0;
    uint8_t GOT_EXT = 0;
    uint8_t GOT_DEC = 0;
    uint8_t GOT_FIT = 0;
    uint8_t GOT_WDT = 0;
    uint8_t GOT_ANY_INTR = 0;
    fapi2::buffer<uint32_t> tmp_field = 0;
    uint32_t tmp_chk = 0;
    fapi2::buffer<uint32_t> tmp_data = 0;

    //Extract out IAR
    if (!v_ppe_xirs_value.empty())
    {
        for (auto it : v_ppe_xirs_value)
        {
            if(!strcmp(it.name, "IAR"))
            {
                l_saved_iar  = it.value;
                l_xir_ava = true;
            }

            if(!strcmp(it.name, "XSR"))   //Extract MCS
            {
                tmp_data = it.value;
                tmp_data.extractToRight(tmp_field, 29, 3);
                tmp_chk = tmp_field;
            }
        }
    }

    //Extract out  IVPR values
    if (!v_ppe_sprs_value.empty())
    {
        for (auto it : v_ppe_sprs_value)
        {
            if(!strcmp(it.name, "IVPR"))
            {
                l_saved_ivpr  = it.value;
                l_ivpr_ava = true;
            }
        }
    }

    //Decode MCS for further use

    switch (tmp_chk)
    {
        case 1 :
            GOT_DATAMCHK = 1;
            break;

        case 2 :
            GOT_DATAMCHK = 1;
            break;

        case 3 :
            GOT_DATAMCHK = 1;
            break;

        case 4 :
            PRG_PROMOTED = 1;
            break;

        //case 5 :
        //ISI_PROMOTED = 1;
        // break;
        case 6 :
            ALIG_PROMOTED = 1;
            break;

        case 7 :
            DSI_PROMOTED = 1;
            break;
    }

    std::string  info;

    //Identify interrupt (Using only if statements as all cases are mutually exclusive)
    if(is_interrupt_handler(l_saved_iar, l_saved_ivpr, MCHK_VEC) && l_xir_ava && l_ivpr_ava)
    {
        GOT_MCHK = 1;
        info =   "Exception summary: PPE is at the Machine Check interrupt vector\n";
        info += "      ISR(29:31) is valid and contains the reason for the Machine Check\n";
        info += "      IAR contains the Machine Check ISR address\n";
        info += "      SRR0 has current instruction address and the instruction did not complete\n";
        info += "      SRR1 written with the content of the MSR\n";

        if(DSI_PROMOTED | ALIG_PROMOTED | GOT_DATAMCHK )
        {
            info += "      EDR contains Data address resposible for error.\n";
        }

        if(PRG_PROMOTED)
        {
            info += "      EDR contains 32bit value being decoded as an instruction.\n";
        }
    }

    if(is_interrupt_handler(l_saved_iar, l_saved_ivpr, SRST_VEC))
    {
        GOT_SRST = 1;
        info =   "Exception summary: PPE is at the Reset interrupt vector\n";
        info += "      IAR contains the Reset ISR address\n";
        info += "      SRR0 has current instruction address interrupted by reset\n";
        info += "      SRR1 written with the content of the MSR\n";

    }

    if(is_interrupt_handler(l_saved_iar, l_saved_ivpr, DSI_VEC))
    {
        GOT_DSI = 1;
        info =   "Exception summary: PPE is at the Data Storage interrupt vector\n";
        info += "      IAR contains the Data Storage ISR address\n";
        info += "      SRR0 has the address of the instruction responsible for the data storage interrupt\n";
        info += "      SRR1 written with the content of the MSR\n";
        info += "      EDR contains Data address resposible for error.\n";

    }

    if(is_interrupt_handler(l_saved_iar, l_saved_ivpr, ISI_VEC))
    {
        GOT_ISI = 1;
        info =   "Exception summary: PPE is at the Instruction Storage interrupt vector\n";
        info += "      IAR contains the Instruction storage  ISR address\n";
        info += "      SRR0 has the address of the instruction responsible for the instruction storage interrupt \n";
        info += "      SRR1 written with the content of the MSR\n";


    }

    if(is_interrupt_handler(l_saved_iar, l_saved_ivpr, EXT_VEC))
    {
        GOT_EXT = 1;
        info =   "Exception summary: PPE is at the External interrupt vector\n";
        info += "      IAR contains the External  ISR address\n";
        info += "      SRR0 written with the address of the next sequential instruction\n";
        info += "      SRR1 written with the content of the MSR\n";

    }

    if(is_interrupt_handler(l_saved_iar, l_saved_ivpr, ALIG_VEC))
    {
        GOT_ALIG = 1;
        info =   "Exception summary: PPE is at the Alignment interrupt vector\n";
        info += "      ISR(ST) = '1' for stores and dcbz , ISR(ST)='0' otherwise\n";
        info += "      IAR contains the Alignement ISR address\n";
        info += "      SRR0 has the address of the instruction causing the alignment interrupt\n";
        info += "      SRR1 written with the content of the MSR\n";
        info += "      EDR contains Data address resposible for error.\n";

    }

    if(is_interrupt_handler(l_saved_iar, l_saved_ivpr, PRG_VEC))
    {
        GOT_PRG = 1;
        info =   "Exception summary: PPE is at the Program interrupt vector\n";
        info += "      ISR(PTR)is valid and contains the reason for the Error\n";
        info += "      IAR contains the Program ISR address\n";
        info += "      SRR0 has the address of the instruction responsible for the program interrupt\n";
        info += "      SRR1 written with the content of the MSR\n";
        info += "      EDR contains 32bit instruction responsible for the program interrupt\n";


    }

    if(is_interrupt_handler(l_saved_iar, l_saved_ivpr, DEC_VEC))
    {
        GOT_DEC = 1;
        info =   "Exception summary: PPE is at the Decrementer interrupt vector\n";
        info += "      IAR contains the Decrementer ISR address\n";
        info += "      SRR0 written with the address of the next sequential instruction \n";
        info += "      SRR1 written with the content of the MSR\n";
    }

    if(is_interrupt_handler(l_saved_iar, l_saved_ivpr, FIT_VEC))
    {
        GOT_FIT = 1;
        info =   "Exception summary: PPE is at the FIT interrupt vector\n";
        info += "      IAR contains the FIT ISR address\n";
        info += "      SRR0 written with the address of the next sequential instruction \n";
        info += "      SRR1 written with the content of the MSR\n";

    }

    if(is_interrupt_handler(l_saved_iar, l_saved_ivpr, WDT_VEC))
    {
        GOT_WDT = 1;
        info =   "Exception summary: PPE is at the WDT interrupt vector\n";
        info += "      IAR contains the WDT ISR address\n";
        info += "      SRR0 written with the address of the next sequential instruction \n";
        info += "      SRR1 written with the content of the MSR\n";

    }

    GOT_ANY_INTR  = (GOT_MCHK | GOT_SRST | GOT_DSI | GOT_ISI | GOT_EXT | GOT_ALIG
                     | GOT_PRG | GOT_DEC | GOT_FIT | GOT_WDT);


    if (!v_ppe_xirs_value.empty())
    {
        printf("----------------------------------------------------------------------------------------------------------\n");
        printf ("PPE :Verbose details of XIR Values \n");

        for (auto it : v_ppe_xirs_value)
        {
            printf("   %s: %08X \n%s ", it.name, it.value,
                   decode_ppe_REG(it, GOT_MCHK, GOT_PRG, PRG_PROMOTED, GOT_DSI, DSI_PROMOTED, GOT_ALIG, ALIG_PROMOTED ,
                                  GOT_ANY_INTR).c_str());
        }
    }

    if (!v_ppe_sprs_value.empty())
    {
        printf("----------------------------------------------------------------------------------------------------------\n");
        printf ("PPE : Verbose details of SPR Values\n");

        for (auto it : v_ppe_sprs_value)
        {
            printf("   %s: %08X \n%s ", it.name, it.value,
                   decode_ppe_REG(it, GOT_MCHK, GOT_PRG, PRG_PROMOTED, GOT_DSI, DSI_PROMOTED, GOT_ALIG, ALIG_PROMOTED,
                                  GOT_ANY_INTR ).c_str());
        }
    }

    printf("----------------------------------------------------------------------------------------------------------\n\n");
    printf("  %s", info.c_str());

    return fapi2::current_err;
}


fapi2::ReturnCode
ppe_state_print_regs(std::vector<Reg32Value_t> v_regs)
{
    printf("----------------------------------------------------------------------------------------------------------\n");

    if (!v_regs.empty())
    {
        uint8_t l_tmp_ctr = 0;

        for (auto it : v_regs)
        {
            printf( " %s: 0x%08X ", it.name, it.value);
            l_tmp_ctr++;

            if(l_tmp_ctr == 6 )
            {
                l_tmp_ctr = 0;
                printf("\n");
            }
        }
    }
    else
    {
        printf(" Not available \n");
    }

    printf("\n----------------------------------------------------------------------------------------------------------\n\n");

    return fapi2::current_err;
}



//-----------------------------------------------------------------------------
/**
 * @brief Display VDR register content on stdout
 * @param[in]   v_ppe_regs       Input GPR Registers vector to display VDRs
 */
fapi2::ReturnCode
ppe_state_print_vdr_regs(std::vector<Reg32Value_t> v_regs)
{
    char l_outstr[128];
    printf("----------------------------------------------------------------------------------------------------------\n");

    if (!v_regs.empty())
    {
        uint8_t l_tmp_ctr = 0;

        for (auto it : v_regs)
        {
            if(!strcmp(it.name, "R13") || !strcmp(it.name, "R10"))
            {

                continue;
            }

            // printf("%-20VDR%d [R%d.R%d] = 0x%08X%08X     ", it.reg.number, it.reg.number, v_ppe_gprs_data[it.reg.number].value,
            // v_ppe_gprs_data[it.reg.number+1].value );
            if(!strcmp(it.name, "R31"))
            {
                printf( "%s: 0x%08X%08X  ", "VDR31[R31.R0]", v_regs[15].value, v_regs[0].value );
                continue;

            }

            sprintf(l_outstr , "VDR%d[R%d.R%d]",  it.number, it.number, it.number + 1);
            printf("%s: 0x%08X%08X  ", l_outstr, v_regs[l_tmp_ctr].value, v_regs[l_tmp_ctr + 1].value);
            l_tmp_ctr++;

            if(!(l_tmp_ctr % 3) )
            {
                printf("\n");
            }
        }
    }
    else
    {
        printf(" Not available \n");
    }

    printf("\n----------------------------------------------------------------------------------------------------------\n\n");
    return fapi2::current_err;
}
//-----------------------------------------------------------------------------

/**
 * @brief Display register content on stdout
 * @param[in]   v_ppe_regs       Input Registers vector to display
 */
fapi2::ReturnCode
print_regs(std::vector<Reg64Value_t> v_ppe_regs)
{
    printf("----------------------------------------------------------------------------------------------------------\n");

    if (!v_ppe_regs.empty())
    {
        uint8_t l_tmp_ctr = 0;

        for (auto it : v_ppe_regs)
        {
            printf( " %s: 0x%016lX ", it.name, it.value);
            l_tmp_ctr++;

            if(l_tmp_ctr == 3 )
            {
                l_tmp_ctr = 0;
                printf("\n");
            }
        }
    }
    else
    {
        printf(" Not available \n");
    }

    printf("\n----------------------------------------------------------------------------------------------------------\n\n");

    return fapi2::current_err;
}
//-----------------------------------------------------------------------------




int main(int argc,
         char* argv[])
{
    extern bool GLOBAL_SIM_MODE;
    extern bool GLOBAL_VERIF_MODE;
    uint64_t rc = ECMD_SUCCESS;
    fapi2::ReturnCode rc_fapi;
    ecmdLooperData looper; // Store internal Looper data
    ecmdChipTarget target; // This is the target to operate on
    ecmdDllInfo DLLINFO;
    ecmdLooperData drawer_looper;
    ecmdChipTarget drawer_target;
    char* testModeValueStr;
    char outstr[128];
    bool isInputFileAvailable =  false;
    std::string filename = "";
    char*    chip_type = (char*)"pc";
    char* argdata;

    bool  dump_vdrs;
    bool  dump_mib;
    bool  dump_others;
    PPE_DUMP_MODE mode = NONE;
    VERBOSE_MODE vmode = NOVERBOSE;
    std::vector<std::pair<enum PPE_TYPES, uint32_t>> v_ppe_types;

    //
    // ------------------------------------
    // Load and initialize the eCMD Dll
    // If left NULL, which DLL to load is determined by the ECMD_DLL_FILE environment variable
    // If set to a specific value, the specified dll will be loaded
    // ------------------------------------
    rc = ecmdLoadDll("");

    if (rc)
    {
        ecmdOutput("poz_ppe_state_wrap: Error calling ecmdLoadDll.");
        return rc;
    }

    do
    {

        // This is needed if you're running a FAPI procedure from this eCMD procedure
        rc = fapi2InitExtension();

        if (rc)
        {
            ecmdOutput("poz_ppe_state_wrap: Error calling fapi2InitExtension.");
            break;
        }

        rc = ecmdQueryDllInfo(DLLINFO);

        if (rc)
        {
            ecmdOutput("poz_ppe_state_wrap: Error calling ecmdQueryDllInfo.");
            break;
        }

        if (DLLINFO.dllEnv == ECMD_DLL_ENV_SIM)
        {
            GLOBAL_SIM_MODE = true;
        }

        // Parse out user options (excluding -pX, -cX, -coe, -debug, etc
        if (ecmdParseOption(&argc, &argv, "-h"))
        {
            help();
            rc = 1;
            break;
        }

        // run procedure in sim verification mode
        if (ecmdParseOption(&argc, &argv, "-verif"))
        {
            GLOBAL_VERIF_MODE = true;
        }

        //\TODO
        //parse command line parameters
        argdata = ecmdParseOptionWithArgs(&argc, &argv, "-input_file");

        //\TODO
        if (argdata != NULL)
        {
            filename = argdata;
            isInputFileAvailable = true;
        }

        if (ecmdParseOption(&argc, &argv, "-sbe"))
        {
            v_ppe_types.push_back(std::make_pair(PPE_TYPE_SBE, 0));
        }

        if (ecmdParseOption(&argc, &argv, "-sppe"))
        {
            v_ppe_types.push_back(std::make_pair(PPE_TYPE_SPPE, 0));
        }

        if (ecmdParseOption(&argc, &argv, "-pme"))
        {
            v_ppe_types.push_back(std::make_pair(PPE_TYPE_PME, 0));
        }

        testModeValueStr = ecmdParseOptionWithArgs(&argc, &argv, "-gpe");

        if (testModeValueStr != NULL)
        {
            if (strcmp(testModeValueStr , "0") == 0)
            {
                ecmdOutput("Preparing for GPE0 dump\n");
                v_ppe_types.push_back(std::make_pair(PPE_TYPE_GPE, 0));
            }

            else if (strcmp(testModeValueStr , "1") == 0)
            {
                ecmdOutput("Preparing for GPE1 dump\n");
                v_ppe_types.push_back(std::make_pair(PPE_TYPE_GPE, 1));
            }

            else if (strcmp(testModeValueStr , "2") == 0)
            {
                ecmdOutput("Preparing for GPE2 dump\n");
                v_ppe_types.push_back(std::make_pair(PPE_TYPE_GPE, 2));
            }

            else if (strcmp(testModeValueStr , "3") == 0)
            {
                ecmdOutput("Preparing for GPE3 dump\n");
                v_ppe_types.push_back(std::make_pair(PPE_TYPE_GPE, 3));
            }
            else
            {
                ecmdOutput("Unknown/unsupported GPE index specified!\n");
                help();
                rc = ECMD_INVALID_ARGS;
                break;
            }
        }

        testModeValueStr = ecmdParseOptionWithArgs(&argc, &argv, "-ce");

        if (testModeValueStr != NULL)
        {
            if (strcmp(testModeValueStr , "0") == 0)
            {
                ecmdOutput("Preparing for CE0 dump\n");
                v_ppe_types.push_back(std::make_pair(PPE_TYPE_GPE, 0));
            }

            else if (strcmp(testModeValueStr , "1") == 0)
            {
                ecmdOutput("Preparing for CE1 dump\n");
                v_ppe_types.push_back(std::make_pair(PPE_TYPE_GPE, 1));
            }

            else if (strcmp(testModeValueStr , "2") == 0)
            {
                ecmdOutput("Preparing for CE2 dump\n");
                v_ppe_types.push_back(std::make_pair(PPE_TYPE_GPE, 2));
            }

            else if (strcmp(testModeValueStr , "3") == 0)
            {
                ecmdOutput("Preparing for CE3 dump\n");
                v_ppe_types.push_back(std::make_pair(PPE_TYPE_GPE, 3));
            }
            else
            {
                ecmdOutput("Unknown/unsupported GPE index specified!\n");
                help();
                rc = ECMD_INVALID_ARGS;
                break;
            }
        }


        testModeValueStr = ecmdParseOptionWithArgs(&argc, &argv, "-ioppe");

        if (testModeValueStr != NULL)
        {
            uint32_t id   = strtoul(testModeValueStr, NULL, 0);

            if (id > 4)
            {
                ecmdOutput("Invalid IOPPE id specified (valid 0:3)\n");
                help();
                rc = ECMD_INVALID_ARGS;
                break;
            }

            v_ppe_types.push_back(std::make_pair(PPE_TYPE_IO, id));
        }


        testModeValueStr = ecmdParseOptionWithArgs(&argc, &argv, "-qme");

        if (testModeValueStr != NULL)
        {
            uint32_t id   = strtoul(testModeValueStr, NULL, 0);

            if (id > 7)
            {
                ecmdOutput("Invalid QME id specified (valid 0:7)\n");
                help();
                rc = ECMD_INVALID_ARGS;
                break;
            }

            //todo Pass QME instance number
            v_ppe_types.push_back(std::make_pair(PPE_TYPE_QME, id));
        }

        if (ecmdParseOption(&argc, &argv, "-snapshot"))
        {
            ecmdOutput("Preparing for -dump option \n");
            mode = SNAPSHOT;
        }

        if (ecmdParseOption(&argc, &argv, "-force_halt"))
        {
            ecmdOutput("Preparing for -force_halt option \n");
            mode = FORCE_HALT;
        }

        if (ecmdParseOption(&argc, &argv, "-halt"))
        {
            ecmdOutput("Preparing for -halt option \n");
            mode = HALT;
        }

        if (ecmdParseOption(&argc, &argv, "-xirs"))
        {
            ecmdOutput("Preparing for -xirs option \n");
            mode = XIRS;
        }

        if (ecmdParseOption(&argc, &argv, "-verbose") )
        {
            ecmdOutput("Preparing for -verbose option \n");
            vmode = VERBOSE;
        }

        if (ecmdParseOption(&argc, &argv, "-verbose+") )
        {
            ecmdOutput("Preparing for -verbose plus option \n");
            vmode = VERBOSEP;

        }


        dump_vdrs = ecmdParseOption(&argc, &argv, "-vdrs");
        dump_mib = ecmdParseOption(&argc, &argv, "-mib");
        dump_others = ecmdParseOption(&argc, &argv, "-other_state");
        //
        // -------------------------------------------------------------------------------------------------
        // Parse out common eCMD args like -p0, -c0, -coe, etc..
        // Any found args will be removed from arg list upon return
        // -------------------------------------------------------------------------------------------------
        rc = ecmdCommandArgs(&argc, &argv);

        if (rc)
        {
            ecmdOutput("poz_ppe_state_wrap: Error calling ecmdCommandArgs.");
            break;
        }

        // We parsed out all optional parameters, now let's parse the positionals
        if (argc < 2)
        {
            ecmdOutputError("No chip option indicated.\n");
            rc = ECMD_INVALID_ARGS;
            break;
        }

        chip_type = argv[1];

        // unsupported arguments left over?
        if (argc > 2)
        {
            ecmdOutput("Unknown/unsupported arguments specified!\n");
            help();
            rc = ECMD_INVALID_ARGS;
            break;
        }

        //
        // always print the dll info to the screen, unless in quiet mode
        if (vmode)
        {
            rc = ecmdDisplayDllInfo();

            if (rc)
            {
                ecmdOutput("poz_ppe_state_wrap: Error calling ecmdDisplayDllInfo.");
                break;
            }
        }

        // configure looper to iterate over all drawer
        drawer_target.cageState   = ECMD_TARGET_FIELD_WILDCARD;
        drawer_target.nodeState   = ECMD_TARGET_FIELD_WILDCARD;
        drawer_target.slotState   = ECMD_TARGET_FIELD_UNUSED;
        drawer_target.posState    = ECMD_TARGET_FIELD_UNUSED;
        drawer_target.chipUnitNumState = ECMD_TARGET_FIELD_UNUSED;
        drawer_target.threadState = ECMD_TARGET_FIELD_UNUSED;

        rc = ecmdConfigLooperInit(drawer_target, ECMD_SELECTED_TARGETS_LOOP_DEFALL, drawer_looper);

        if (rc)
        {
            ecmdOutputError("Error initializing drawer looper!\n");
            break;
        }

        // loop over specified configured drawers
        while (ecmdConfigLooperNext(drawer_target, drawer_looper))
        {
            target.chipType      = chip_type;
            target.cage = drawer_target.cage;
            target.node = drawer_target.node;
            target.chipTypeState = ECMD_TARGET_FIELD_VALID;
            target.chipUnitTypeState = ECMD_TARGET_FIELD_UNUSED;
            target.chipUnitNumState  = ECMD_TARGET_FIELD_UNUSED;
            target.cageState     = ECMD_TARGET_FIELD_WILDCARD;
            target.nodeState     = ECMD_TARGET_FIELD_WILDCARD;
            target.slotState     = ECMD_TARGET_FIELD_WILDCARD;
            target.threadState     = ECMD_TARGET_FIELD_UNUSED;

            rc = ecmdConfigLooperInit(target, ECMD_SELECTED_TARGETS_LOOP_DEFALL, looper);

            if (rc)
            {
                ecmdOutput("poz_ppe_state_wrap: Error calling ecmdConfigLooperInit.");
                break;
            }

            if (vmode)
            {
                ecmdOutput("Entering config looper\n");
            }

            // loop over specified configured positions
            while (ecmdConfigLooperNext(target, looper))
            {
                fapi2::Target<fapi2::TARGET_TYPE_ANY_POZ_CHIP> fapi_target(&target);

                for (auto it : v_ppe_types)
                {
                    sprintf(outstr, "\nDumping %s state. Base Address=0x%08lx\n", ppe_get_name(it.first, it.second),
                            ppe_get_base_address(it.first, it.second));
                    ecmdOutput(outstr);

                    std::vector<Reg32Value_t> v_ppe_sprs_value;
                    std::vector<Reg32Value_t> v_ppe_xirs_value;
                    std::vector<Reg32Value_t> v_ppe_gprs_value;
                    std::vector<Reg64Value_t> v_mib_regs_value;
                    std::vector<Reg64Value_t> v_other_regs_value;
                    Reg32Value_t l_regVal;

                    if (isInputFileAvailable)
                    {

                        // Open the input file
                        std::ifstream myfile;
                        myfile.open(filename.c_str(), std::ifstream::in);

                        bool isLineFound = false;
                        int regname_len = 8, value_len = 16, count = 0;
                        std::string line, token;

                        if (myfile.is_open())
                        {
                            while(!myfile.eof())
                            {
                                getline(myfile, line);

                                if(isLineFound)
                                {
                                    if ((line.find("****", 0) != std::string::npos) || (line.find("--------", 0) != std::string::npos))
                                    {
                                        count++;

                                        if(count >= 2)
                                        {
                                            isLineFound = false;
                                        }
                                    }
                                    else
                                    {
                                        token = line.substr(0, regname_len);
                                        l_regVal.name = token.c_str();

                                        if (l_regVal.value == 1)
                                        {
                                            token = line.substr(16, value_len);
                                            l_regVal.value = stoul(token, NULL, 16);
                                            // sprs
                                            v_ppe_sprs_value.push_back(l_regVal);
                                        }
                                        else if(l_regVal.value == 2)
                                        {
                                            token = line.substr(16, value_len);
                                            l_regVal.value = stoul(token, NULL, 16);
                                            // xirs
                                            v_ppe_xirs_value.push_back(l_regVal);
                                        }
                                        else if(l_regVal.value == 3)
                                        {
                                            token = line.substr(16, value_len);
                                            l_regVal.value = stoul(token, NULL, 16);
                                            // gprs
                                            v_ppe_gprs_value.push_back(l_regVal);
                                        }
                                    }
                                }

                                if(line.find("Reg             Reg Value", 0) != std::string::npos)
                                {
                                    isLineFound = true;
                                }
                            }
                        }
                    }
                    else
                    {
                        FAPI_EXEC_HWP(rc_fapi, poz_ppe_state, fapi_target, it.first, it.second, mode,
                                      v_ppe_gprs_value, v_ppe_sprs_value, v_ppe_xirs_value);
                        rc = uint64_t(rc_fapi);

                        if (rc)
                        {
                            ecmdOutput("poz_ppe_state_wrap: Error calling procedure poz_ppe_state.");
                            break;
                        }
                    }

                    printf ("PPE : XIR Values\n");
                    ppe_state_print_regs(v_ppe_xirs_value);

                    printf ("PPE:  SPR Values\n");
                    ppe_state_print_regs(v_ppe_sprs_value);

                    printf ("PPE: GPR Values\n");
                    ppe_state_print_regs(v_ppe_gprs_value);

                    if(dump_vdrs)
                    {
                        printf ("PPE: VDR Values\n");
                        ppe_state_print_vdr_regs(v_ppe_gprs_value);
                    }

                    //\TODO
                    if((vmode == VERBOSE) || (vmode == VERBOSEP) )
                    {
                        //Then call verbose print function
                        ppe_state_print_verbose(
                            v_ppe_sprs_value,
                            v_ppe_xirs_value);
                    }

                    if(dump_mib)
                    {
                        FAPI_EXEC_HWP(rc_fapi, poz_mib_state, fapi_target, it.first, it.second,
                                      v_mib_regs_value );
                        rc = uint64_t(rc_fapi);

                        if (rc)
                        {
                            ecmdOutput("poz_ppe_state_wrap: Error calling procedure poz_mib_state.");
                            break;
                        }

                        //Printing mib regs
                        printf ("MIB: state register values\n");
                        print_regs(v_mib_regs_value);
                    }

                    //\TODO
                    if(dump_others)
                    {
                        FAPI_EXEC_HWP(rc_fapi, poz_ppe_other_regs, fapi_target, it.first, it.second,
                                      v_other_regs_value);
                        rc = uint64_t(rc_fapi);

                        if (rc)
                        {
                            ecmdOutput("poz_ppe_state_wrap: Error calling procedure poz_ppe_other_regs.");
                            break;
                        }

                        //Printing other regs
                        printf ("Other state register values\n");
                        print_regs(v_other_regs_value);
                    }

                }

            }
        }
    }
    while(0);

    if (vmode)
    {
        ecmdOutput("\npoz_ppe_state is Done\n");
    }

    ecmdUnloadDll();

    return rc;

}
