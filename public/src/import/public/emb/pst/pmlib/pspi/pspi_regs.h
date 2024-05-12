/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/emb/pst/pmlib/pspi/pspi_regs.h $     */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2022,2024                        */
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

#ifdef __OCC_PLAT
    #define __OCI_BASE 0xC0060000
#endif

#ifdef __TCC_PLAT
    #define __OCI_BASE 0xC0060000
#endif

#define PSPI_LINK_REG(base, target) \
    (__OCI_BASE + ((base)<<3) + (target * TAP_OFST))

#define PSPI_DB_REG(base, db, target) \
    (__OCI_BASE + ((base)<<3) + (db<<3) + (target * TAP_OFST))

#define PSPI_PCOM_REG(base, target) \
    (__OCI_BASE + ((base)<<3) + (target * TAP_OFST))

#define PSPI_VC_REG(op, vc, target) \
    (__OCI_BASE + ((op)<<3) + ((vc-1) * VC_OFST) + (target * TAP_OFST))

#define PSPI_VC_BROADCAST_REG(op, vc) \
    (__OCI_BASE + ((op)<<3) + ((vc-1) * VC_BROADCAST_OFST))

#define PSPI_SUMMARY_REG(type, regop, ch) \
    (__OCI_BASE + ((type + regop) << 3) + ((ch - ((type==PUSH_RECV_SUMMARY)?1:0)) * SUM_OFST))

#define TAP_OFST               (0x020 << 3)
#define VC_OFST                (0x008 << 3)
#define VC_BROADCAST_OFST      (0x001 << 3)
#define SUM_OFST               (0x004 << 3)

/// Interrupts (not supported via this API) ///

// OCC PSPI Interrupt Type n TAP a (OPITnTa)  (n=0:7, a=0:7)
//   Details 28 bits of pspi_intr_payload
//   Read before clearing OPITnPR bits
// OCC PSPI Interrupt Type n Summary Vector a (OPITnSVa) (n=A:B, a=0:3)
//   Details 4 bits of pspi_intr_payload for each Tap
//   Read before clearing OPITnPR bits
// OCC PSPI Interrupt Type n Summary Vector (OPITnSV) (n=C,D)
//   Details 4 bits of pspi_intr_payload for each Tap
//   Read before clearing OPITnPR bits
// OCC PSPI Interrupt Type n Summary Vector (OPITnSV) (n=E,F)
//   Details 4 bits of pspi_intr_payload for each Tap
//   Read before clearing OPITnPR bits
// OCC PSPI Interrupt Type n Pending a Register (OPITnPRa)  (n=0:7)
//   Details PSPI interrupt pending for each Tap
//   Bits need to be write-cleared
// OCC PSPI Interrupt Type n Pending c Register (OPITnPRc)  (n=A,B,C,D)
//   Details PSPI interrupt pending for each Tap
//   Bits need to be write-cleared
// OCC PSPI Interrupt Type n Pending d Register (OPITnPRd)  (n=E,F)
//   Details PSPI interrupt pending for each Tap
// OCC PSPI Interrupt Pending Group [n] Mask (OPIPGMn) (n=0:3)
//   sets the mask to route PSPI Interrupt Types pending to pspi_intr_pending_group_n (for each GPE)
//   Bits need to be write-cleared

//#define INTERRUPT_TYPE
//#define INTERRUPT_TYPE_RESET
//#define INTERRUPT_SUMMARY_VEC
//#define INTERRUPT_SUMMARY_RESET

#define PSPI_TPIG              0x913

/// Summary (OCC Only) ///

// OCC PSPI Push Receive Complete Summary VCn Register (OPPRCSVn) (n=1:3)
//   Details PUSH_RCV_COMP for each Tap

// OCC PSPI Doorbell Summary n Register (OPDBSRn) (n=0:3)
//   Details received doorbells for each Tap

#define PUSH_RECV_SUMMARY      0x0d4
#define MSG_DB_SUMMARY         0x0e0
#define DB_SUMMARY             0x0f0

/// DoorBell and Message DoorBell ///

// OCC PSPI Doorbell Send n TAPn Register (OPDBSnTa)(d=0:3, n=0:7)
//   Broadcast register also (OPDBSn)
// OCC PSPI Doorbell Receive n TAPn Register (OPDBRnTa)(d=0:3, n=0:7)

// OCC PSPI Message Doorbell Send n TAPa Register (OPMDBSnTa)(n=0:3)(a=0:7)
//   Broadcast register also (OPMDBSn)
// OCC PSPI Message Doorbell Receive n TAPa Register (OPMDBRnTa)(n=0:3)(a=0:7)

// Array of DB indexes and chip target
#define MSG_DB_SND             0x900 // [6c900,6c901,6c902,6c903] [C0064800,C0064808,C0064810,C0064818]
#define DB_SND                 0x904 // [6c904,6c905,6c906,6c907] [C0064820,C0064828,C0064830,C0064838]
#define MSG_DB_RCV             0x908 // [6c908,6c909,6c90a,6c90b] [C0064840,C0064848,C0064850,C0064858]
#define DB_RCV                 0x90c // [6c90c,6c90d,6c90e,6c90f] [C0064860,C0064868,C0064870,C0064878]

// Absolute addresses per chip target
#define MSG_DB_SND_BCST        0xb00
#define DB_SND_BCST            0xb04

/// PCOM ///

// OCC PSPI PCOM Data TAPn Register (OPPDTn)(n=0:7)
//   Broadcast register also (OPPD)
// OCC PSPI PCOM Write Address TAPn Register (OPPWATn)(n=0:7)
//   Broadcast register also (OPPWA)
// OCC PSPI PCOM Read Address TAPn Register (OPPRATn)(n=0:7)
//   Broadcast register also (OPPRA)

// Absolute addresses per chip target
#define PCOM_DATA              0x910 // [6c910] [C0064880]
#define PCOM_WADDR             0x911 // [6c911] [C0064888]
#define PCOM_RADDR             0x912 // [6c912] [C0064890]

// Absolute addresses per chip target
#define PCOM_DATA_RCV          0x918
#define PCOM_WADDR_RCV         0x919 // [6c919] [C00648C8]
#define PCOM_RADDR_RCV         0x91a // [6c91A] [C00648D0]

// Absolute addresses per chip target
#define PCOM_DATA_BCST         0xb10
#define PCOM_WADDR_BCST        0xb11
#define PCOM_RADDR_BCST        0xb12

/// VC 0 Config and Error ///

// VC0 Register writes are using direct addresses defined below
//   as these registers are not arrayed therefore no offset assembly needed

// OCC PSPI Config VC0 TAPn Register (OPCFGV0Tn)(n=0:7)
//   Set VC0_ENABLE, TIMEOUT_ENABLE, BROADCAST_ENABLE, FSM_FREEZE_ENABLE, and TIMEOUT_VALUE
// OCC PSPI ERR VC0 TAPn Register (OPERRV0Tn)(n=0:7)
//   Add to RTX error checking
//   Bits need to be write-cleared

// Absolute addresses per chip target
#define PERR_VC0               0x91e // [6c91e] [C00648F0]
#define PCFG_VC0               0x91f // [6c91f] [C00648F8]

enum PSPI_VC0_INTERFACE
{
    PSPI_VC0_EN            = BIT32(0),
    PSPI_VC0_TIMEOUT_EN    = BIT32(2),
    PSPI_VC0_RESET         = BIT32(3),
    PSPI_VC0_BROADCAST_EN  = BIT32(4),
    PSPI_VC0_PCOM_LOOPBACK = BIT32(5),
    PSPI_VC0_FSM_FREEZE_EN = BIT32(7)
};

/// VC 1-3 Config and Action ///

// The following encodings provide combinations to assemble the addresses
//   of the registers below. TCC shares same offset as OCC ones

enum PSPI_CFG_OP_TO_ADDR
{
    PSPI_CNFG             = 0xa00,
    PSPI_GO               = 0xa04,

    PSPI_CTRL_PUSH        = 0x000,
    PSPI_CTRL_PULL        = 0x002,
    PSPI_CTRL_SEND        = 0x000,
    PSPI_CTRL_RECV        = 0x001,
};

enum PSPI_CFG_OP_INTERFACE
{
    PSPI_CFG_BUF_EN       = BIT32(0),
    PSPI_CFG_PP_EN        = BIT32(1),
    PSPI_CFG_BROADCAST_EN = BIT32(2), // Push Send or Pull Recv only
    PSPI_CFG_BW_SAVER     = BIT32(3),
    PSPI_CFG_LOCK_DIS     = BIT32(4),
    PSPI_CFG_HW_LOCK_DIS  = BIT32(5),
    PSPI_CFG_OCI_BLK_DIS  = BIT32(6),
    PSPI_CFG_WR_BLK_DIS   = BIT32(7),
    PSPI_CFG_RD_BLK_DIS   = BIT32(8),

    PSPI_OP_START         = BIT32(0),
    PSPI_OP_IN_PROG       = BIT32(1)
};

// OCC PSPI Push Send Config VCn TAPa Register (OPSSCVnTa)(n=1:3, a=0:7)
//   Set BUFFER_ENABLE, PING_PONG_ENABLE (for ping pong mode testing),
//       BROADCAST_ENABLE (for broadcast testing), BANDWIDTH_SAVER (for pull loop test),
//       TIMEOUT_VALUE, BUFFER_SIZE, and BUFFER_BAR
// OCC PSPI Push Rcv Config VCn TAPa Register (OPSRCVnTa)(n=1:3, a=0:7)
//   Set BUFFER_ENABLE, PING_PONG_ENABLE (for ping pong mode testing),
//       BROADCAST_ENABLE (for broadcast testing), BANDWIDTH_SAVER (for pull loop test),
//       TIMEOUT_VALUE, BUFFER_SIZE, and BUFFER_BAR
// OCC PSPI Pull Send Config VCn TAPa Register (OPLSCVnTa)(n=1:3, a=0:7)
//   Set BUFFER_ENABLE, PING_PONG_ENABLE (for ping pong mode testing),
//       BROADCAST_ENABLE (for broadcast testing), BANDWIDTH_SAVER (for pull loop test),
//       TIMEOUT_VALUE, BUFFER_SIZE, and BUFFER_BAR
// OCC PSPI Pull Rcv Config VCn TAPa Register (OPLRCVnTa)(n=1:3, a=0:7)
//   Set BUFFER_ENABLE, PING_PONG_ENABLE (for ping pong mode testing),
//       BROADCAST_ENABLE (for broadcast testing), BANDWIDTH_SAVER (for pull loop test),
//       TIMEOUT_VALUE, BUFFER_SIZE, and BUFFER_BAR

#define PUSH_SND_CFG_VC        0xa00
#define PUSH_RCV_CFG_VC        0xa01
#define PULL_SND_CFG_VC        0xa02
#define PULL_RCV_CFG_VC        0xa03

// OCC PSPI Push Send Op VCa TAPn Register (OPSSOVaTn) (n=0:7, a=1:3)
//   Set START bit
//   Use status/error fields
//   Broadcast register also (OPSSOBVn) only START bit
// OCC PSPI Push Rcv Op VCa TAPn Register (OPSROVaTn) (n=0:7, a=1:3)
//   Use status/error fields
// OCC PSPI Pull Send Op VCa TAPn Register (OPLSOVaTn) (n=0:7, a=1:3)
//   Poll on NEW_DATA
//   Use status/error fields
// OCC PSPI Pull Rcv Op VCa TAPn Register (OPLROVaTn) (n=0:7, a=1:3)
//   Set START bit
//   Poll on IN_PROGRESS
//   Use status/error fields
//   Broadcast register also (OPLROBVn) only START bit

#define PUSH_SND_OP_VC         0xa04
#define PUSH_RCV_OP_VC         0xa05
#define PULL_SND_OP_VC         0xa06
#define PULL_RCV_OP_VC         0xa07

// Absolute addresses per chip target
#define PUSH_SND_OP_BCST_VC    0xb18
#define PULL_RCV_OP_BCST_VC    0xb1c

/// VC 0-3 Status (OCC Only) ///

enum PSPI_VC_STATUS
{
    PSPI_VC_STATUS_TRAN     = 0,
    PSPI_VC_STATUS_RECV     = 1,
    PSPI_VC0_STATUS_LOC     = SHIFT32(7),
    PSPI_VC1_STATUS_LOC     = SHIFT32(15),
    PSPI_VC2_STATUS_LOC     = SHIFT32(23),
    PSPI_VC3_STATUS_LOC     = SHIFT32(31),
    PSPI_VC_STATUS_ALL_TAPS = 0xFF
};

// OCC PSPI VC Transfer Status Register (OPVCTS)
//   XFER_ONGOING status
// OCC PSPI VC Receive Status Register (OPVCRS)
//   RCV_ONGOING status

// Absolute addresses per chip target
#define PVC_TRAN_STATUS        0xb14
#define PVC_RECV_STATUS        0xb15

/// VC 1-3 Data Input/Output ///

// The following encodings provide combinations to assemble the addresses
//   of the registers below. TCC shares same offset as OCC ones

enum PSPI_DATA_IO_TO_ADDR
{
    PSPI_PUSH          = 0xc00,
    PSPI_PULL          = 0xd00,
    PSPI_SEND          = 0x000,
    PSPI_RECV          = 0x004,
    PSPI_START         = 0x000,
    PSPI_NEXT          = 0x001,
    PSPI_LAST          = 0x002,
};

// OCC PSPI Push Send Write Start VCa TAPn Register (OPSSWSVaTn) (n=0:7, a=1:3)
//   For 4B accesses, the correct order is to write 0:31, then 32:63.
//   The initial write sets up write pointers and lock
// OCC PSPI Push Send Write Next VCa TAPn Register (OPSSWNVaTn) (n=0:7, a=1:3)
//   For 4B accesses, the correct order is to write 0:31, then 32:63.
//   The initial write sets up write pointers and lock
// OCC PSPI Push Send Write Complete VCa TAPn Register (OPSSWCVaTn) (n=0:7,a=1:3)
//   For 4B accesses, the correct order is to write 0:31, then 32:63.
//   The initial write sets up write pointers and lock

#define PUSH_SND_WR_START_VC   0xc00
#define PUSH_SND_WR_NEXT_VC    0xc01
#define PUSH_SND_WR_LAST_VC    0xc02

// OCC PSPI Push Rcv Read Start VCa TAPn Register (OPSRRSVaTn) (n=0:7, a=1:3)
//   For 4B accesses, the correct order is to read 0:31, then 32:63.
//   The initial read sets up read pointers and locks
// OCC PSPI Push Rcv Read Next VCa TAPn Register (OPSRRNVaTn) (n=0:7, a=1:3)
//   For 4B accesses, the correct order is to read 0:31, then 32:63.
//   The initial read sets up read pointers and locks
// OCC PSPI Push Rcv Read Complete VCa TAPn Register (OPSRRCVaTn) (n=0:7, a=1:3)
//   For 4B accesses, the correct order is to read 0:31, then 32:63.
//   The initial read sets up read pointers and locks

#define PUSH_RCV_RD_START_VC   0xc04
#define PUSH_RCV_RD_NEXT_VC    0xc05
#define PUSH_RCV_RD_LAST_VC    0xc06

// OCC PSPI Pull Send Write Start VCa TAPn Register (OPLSWSVaTn) (n=0:7, a=1:3)
//   For 4B accesses, the correct order is to write 0:31, then 32:63.
//   The initial write sets up write pointers and lock
// OCC PSPI Pull Send Write Next VCa TAPn Register (OPLSWNVaTn) (n=0:7, a=1:3)
//   For 4B accesses, the correct order is to write 0:31, then 32:63.
//   The initial write sets up write pointers and lock
// OCC PSPI Pull Send Write Complete VCa TAPn Register (OPLSWCVaTn) (n=0:7, a=1:3)
//   For 4B accesses, the correct order is to write 0:31, then 32:63.
//   The initial write sets up write pointers and lock

#define PULL_SND_WR_START_VC   0xd00
#define PULL_SND_WR_NEXT_VC    0xd01
#define PULL_SND_WR_LAST_VC    0xd02

// OCC PSPI Pull Rcv Read Start VCa TAPn Register (OPLRRSVaTn) (n=0:7, a=1:3)
//   For 4B accesses, the correct order is to read 0:31, then 32:63.
//   The initial read sets up read pointers and locks
// OCC PSPI Pull Rcv Read Next VCa TAPn Register (OPLRRNVaTn) (n=0:7, a=1:3)
//   For 4B accesses, the correct order is to read 0:31, then 32:63.
//   The initial read sets up read pointers and locks
// OCC PSPI Pull Rcv Read Complete VCa TAPn Register (OPLRRCVaTn) (n=0:7, a=1:3)
//   For 4B accesses, the correct order is to read 0:31, then 32:63.
//   The initial read sets up read pointers and locks

#define PULL_RCV_RD_START_VC   0xd04
#define PULL_RCV_RD_NEXT_VC    0xd05
#define PULL_RCV_RD_LAST_VC    0xd06

/// Link and Ping ///

// Link Register writes are using direct addresses defined below
//   as these registers are not arrayed therefore no offset assembly needed

// OCC PSPI Link Configuration TAPn Register  (OPLCTn) (n=0:7)
//   Set pspi_link_enable, pspi_clock_divider = 1 (can randomized1-10),
//       Pspi_ecc_gen_en, Pspi_ecc_check_en
// OCC PSPI Link Status TAPn Register (OPLSTn) (n=0:7)

#define LINK_CONFIG            0xe00
#define LINK_PING              0xe01
#define LINK_STATUS            0xe02
#define LINK_ECC               0xe03
#define LINK_PING_WRITE_HEADER 0xe04
#define LINK_PING_WRITE_DATA   0xe05
#define LINK_PING_READ_HEADER  0xe06
#define LINK_PING_READ_DATA    0xe07

enum PSPI_CONFIGS_LINK_PING
{
    PSPI_LINK_ENABLE            = BIT32(0),
    PSPI_LINK_RESET             = BIT32(1),
    PSPI_CLK_DIV_DEFAULT        = BIT32(15),
    PSPI_LINK_WRAP_MODE         = BIT32(16),
    PSPI_LINK_NOT_WRAP_MODE     = 0,
    // enable pkt_capture, ecc_gen/check, ue_freeze
    PSPI_LINK_DEBUG_DEFAULT     = (BIT32(18) | BITS32(20, 3)),
    PSPI_LINK_DDR_DIS           = BIT32(17),
    PSPI_LINK_PKT_CAPTURE       = BIT32(18), // OCC Only
    PSPI_LINK_ECC_GEN_EN        = BIT32(20),
    PSPI_LINK_ECC_CHECK_EN      = BIT32(21),
    PSPI_LINK_RX_FEM_FREEZE     = BIT32(22),

    PSPI_PING_SEND              = BIT32(0),
    PSPI_PING_MODE_SINGLE       = 0,
    PSPI_PING_MODE_CONTINUOUS   = BIT32(1),
    PSPI_PING_GENERATOR         = BIT32(2),
    PSPI_PING_RESPONDER         = BIT32(3),
    PSPI_PING_DETECT_CLEAR      = BIT32(4),

    PSPI_PING_DATAOP_NULL       = 0,
    PSPI_PING_DATAOP_INC        = BIT32(7),
    PSPI_PING_DATAOP_INVERT     = BIT32(6),
    PSPI_PING_DATAOP_ROTATE     = BITS32(6, 2),
    PSPI_PING_DATAOP_INC_ROT    = BIT32(5),
    PSPI_PING_DATAOP_INVERT_ROT = (BIT32(5) | BIT32(7)),

    //Request Header only:  0b11xx00yy where xx=1-3   pull, vc1-3
    //                                   and yy=0-2         start/next/done
    //Request 8b Data:      0b110000xx where xx=0-3   db0-3
    //Request 32b Data:     0b110010xx where xx=0,2,3 pspi/pcom_rd/pcom_wr
    //Request 64b Data:     0b11001111                scom wr
    //                      0b110001xx where xx=0-3   db0-3
    //                      0b11xx11yy where xx=1-3   push, vc1-3
    //                                   and yy=0-2         start/next/done
    //Response Header only: 0b00xxyyyy where xx=0-3,
    //                                       yy != 0b1000
    //Response 64b Data:    0b00xx1000 where xx=0-3
    PSPI_PING_HDR_REQ_VC1L_ST_DAT64 = 0xD0000000,
    PSPI_PING_HDR_REQ_VC1L_NX_DAT64 = 0xD1000000,
    PSPI_PING_HDR_REQ_VC1L_DN_DAT64 = 0xD2000000,

    PSPI_PING_HDR_REQ_VC2L_ST_DAT64 = 0xE0000000,
    PSPI_PING_HDR_REQ_VC2L_NX_DAT64 = 0xE1000000,
    PSPI_PING_HDR_REQ_VC2L_DN_DAT64 = 0xE2000000,

    PSPI_PING_HDR_REQ_VC3L_ST_DAT64 = 0xF0000000,
    PSPI_PING_HDR_REQ_VC3L_NX_DAT64 = 0xF1000000,
    PSPI_PING_HDR_REQ_VC3L_DN_DAT64 = 0xF2000000,

    PSPI_PING_HDR_REQ_DB0_DAT8      = 0xC0000000,
    PSPI_PING_HDR_REQ_DB1_DAT8      = 0xC1000000,
    PSPI_PING_HDR_REQ_DB2_DAT8      = 0xC2000000,
    PSPI_PING_HDR_REQ_DB3_DAT8      = 0xC3000000,

    PSPI_PING_HDR_REQ_INTRP_DAT32   = 0xC8000000,
    PSPI_PING_HDR_REQ_PCMRD_DAT32   = 0xCA000000,
    PSPI_PING_HDR_REQ_PCMWR_DAT32   = 0xCB000000,

    PSPI_PING_HDR_REQ_SCOM_DAT64    = 0xCF000000,
    PSPI_PING_HDR_REQ_DB0_DAT64     = 0xC4000000,
    PSPI_PING_HDR_REQ_DB1_DAT64     = 0xC5000000,
    PSPI_PING_HDR_REQ_DB2_DAT64     = 0xC6000000,
    PSPI_PING_HDR_REQ_DB3_DAT64     = 0xC7000000,

    PSPI_PING_HDR_REQ_VC1S_ST_DAT64 = 0xDC000000,
    PSPI_PING_HDR_REQ_VC1S_NX_DAT64 = 0xDD000000,
    PSPI_PING_HDR_REQ_VC1S_DN_DAT64 = 0xDE000000,

    PSPI_PING_HDR_REQ_VC2S_ST_DAT64 = 0xEC000000,
    PSPI_PING_HDR_REQ_VC2S_NX_DAT64 = 0xED000000,
    PSPI_PING_HDR_REQ_VC2S_DN_DAT64 = 0xEE000000,

    PSPI_PING_HDR_REQ_VC3S_ST_DAT64 = 0xFC000000,
    PSPI_PING_HDR_REQ_VC3S_NX_DAT64 = 0xFD000000,
    PSPI_PING_HDR_REQ_VC3S_DN_DAT64 = 0xFE000000,

    PSPI_PING_HDR_RSP_VC0_NOCODE    = 0x00000000,
    PSPI_PING_HDR_RSP_VC1_NOCODE    = 0x10000000,
    PSPI_PING_HDR_RSP_VC2_NOCODE    = 0x20000000,
    PSPI_PING_HDR_RSP_VC3_NOCODE    = 0x30000000,

    PSPI_PING_HDR_RSP_CODE_DAT64    = 0x08000000,
    PSPI_PING_HDR_RSP_CODE_NOERR    = 0x00000000,
    PSPI_PING_HDR_RSP_CODE_RETRY    = 0x01000000,
    PSPI_PING_HDR_RSP_CODE_INVLD_OP = 0x02000000,
    PSPI_PING_HDR_RSP_CODE_INVLD_AD = 0x04000000,
    PSPI_PING_HDR_RSP_CODE_CRCERR   = 0x06000000,
    PSPI_PING_HDR_RSP_CODE_TIMEOUT  = 0x07000000,
    PSPI_PING_HDR_RSP_CODE_DATA     = 0x08000000, //same as DATA 64
    PSPI_PING_HDR_RSP_CODE_NODATA   = 0x09000000,
    PSPI_PING_HDR_RSP_CODE_NONEWDAT = 0x0a000000,

    PSPI_FSM_SEND_LOC   = SHIFT64SH(35),
    PSPI_FSM_RECV_LOC   = SHIFT64SH(39),
    PSPI_FSM_MASTER_LOC = SHIFT64SH(43),
    PSPI_FSM_SLAVE_LOC  = SHIFT64SH(47)
};
