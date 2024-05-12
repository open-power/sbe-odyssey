/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/emb/pst/pmlib/include/registers/pba_register_addresses.h $ */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2021,2024                        */
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
#ifndef __PBA_REGISTER_ADDRESSES_H__
#define __PBA_REGISTER_ADDRESSES_H__

/// \file pba_register_addresses.h
/// \brief Symbolic addresses for the PBA units

#define PBA_OCI_BASE     0xC0040000
#define PBA_MODE         0xC0040000
#define PBA_PORTRST      0xC0040008
#define PBA_PORTCTLN(n) (PBA_PORTCTL0 + ((PBA_PORTCTL1 - PBA_PORTCTL0) * (n)))
#define PBA_PORTCTL0     0xC0040020
#define PBA_PORTCTL1     0xC0040028
#define PBA_PORTCTL2     0xC0040030
#define PBA_PORTCTL3     0xC0040038
#define PBA_BCDE0_CTL    0xC0040080
#define PBA_BCDE0_SET    0xC0040088
#define PBA_BCDE0_STAT   0xC0040090
#define PBA_BCDE0_DR     0xC0040098
#define PBA_BCDE0_OCIBAR 0xC00400a0
#define PBA_BCUE0_CTL    0xC00400a8
#define PBA_BCUE0_SET    0xC00400b0
#define PBA_BCUE0_STAT   0xC00400b8
#define PBA_BCUE0_DR     0xC00400c0
#define PBA_BCUE0_OCIBAR 0xC00400c8
//CMO-20230209: Here goes a 2nd set of BCEs for Hcode users, BC{D,U}E1. Will implement if needed.
#define PBA_OCRN(n) (PBA_OCR0 + ((PBA_OCR1 - PBA_OCR0) * (n)))
#define PBA_OCR0         0xC00400d0
#define PBA_OCR1         0xC00400d8
#define PBA_OCR2         0xC00400e0
#define PBA_OCR3         0xC00400e8
//CMO-20230209: Here goes four new BC{D,U}E{0,1}_STATX status extend regs. Will implement if needed.
#define PBA_XSNDTX       0xC0040100
#define PBA_XCFG         0xC0040108
#define PBA_XSNDSTAT     0xC0040110
#define PBA_XSNDDAT      0xC0040118
#define PBA_XRCVSTAT     0xC0040120
#define PBA_XSHBRN(n) (PBA_XSHBR0 + ((PBA_XSHBR1 - PBA_XSHBR0) * (n)))
#define PBA_XSHBR0       0xC0040130
#define PBA_XSHBR1       0xC0040150
#define PBA_XSHCSN(n) (PBA_XSHCS0 + ((PBA_XSHCS1 - PBA_XSHCS0) * (n)))
#define PBA_XSHCS0       0xC0040138
#define PBA_XSHCS1       0xC0040158
#define PBA_XSHINCN(n) (PBA_XSHINC0 + ((PBA_XSHINC1 - PBA_XSHINC0) * (n)))
#define PBA_XSHINC0      0xC0040140
#define PBA_XSHINC1      0xC0040160
#define PBA_XISNDTX      0xC0040180
#define PBA_XICFG        0xC0040188
#define PBA_XISNDSTAT    0xC0040190
#define PBA_XISNDDAT     0xC0040198
#define PBA_XIRCVSTAT    0xC00401a0
#define PBA_XISHBRN(n) (PBA_XISHBR0 + ((PBA_XISHBR1 - PBA_XISHBR0) * (n)))
#define PBA_XISHBR0      0xC00401b0
#define PBA_XISHBR1      0xC00401d0
#define PBA_XISHCSN(n) (PBA_XISHCS0 + ((PBA_XISHCS1 - PBA_XISHCS0) * (n)))
#define PBA_XISHCS0      0xC00401b8
#define PBA_XISHCS1      0xC00401d8
#define PBA_XISHINCN(n) (PBA_XISHINC0 + ((PBA_XISHINC1 - PBA_XISHINC0) * (n)))
#define PBA_XISHINC0     0xC00401c0
#define PBA_XISHINC1     0xC00401e0


#if defined(__OCC_PLAT)


    //
    // PBAO registers for OCC - 0x010118nn
    //


    #define PBA_PBAO_FIR_RWCLR     0x01011800
    #define PBA_PBAO_FIR_OR        0x01011801
    #define PBA_PBAO_FIRMASK_RWCLR 0x01011802
    #define PBA_PBAO_FIRMASK_OR    0x01011803
    #define PBA_PBAO_CFGXSTOP      0x01011804
    #define PBA_PBAO_CFGRECOV      0x01011805
    #define PBA_PBAO_CFGATTN       0x01011806
    #define PBA_PBAO_CFGLXSTOP     0x01011807
    #define PBA_PBAO_LFIRWOF       0x01011808
    #define PBA_PBAO_OCCACT        0x0101180a
    #define PBA_PBAO_OCFG          0x0101180b // OCI config reg
    //#define PBA_PBAO_ERRRPT0_RWCLR 0x0101180c // Any write clears ERRRPT1/2
    #define PBA_PBAO_ERRRPT1_RO    0x0101180d
    #define PBA_PBAO_ERRRPT2_RO    0x0101180e

    #define PBA_PBAO_RBUFVALN(n) (PBA_RBUFVAL0 + ((PBA_RBUFVAL1 - PBA_RBUFVAL0) * (n)))
    #define PBA_PBAO_RBUFVAL0 0x01011810
    #define PBA_PBAO_RBUFVAL1 0x01011811
    #define PBA_PBAO_RBUFVAL2 0x01011812
    #define PBA_PBAO_RBUFVAL3 0x01011813
    #define PBA_PBAO_RBUFVAL4 0x01011814
    #define PBA_PBAO_RBUFVAL5 0x01011815

    #define PBA_PBAO_WBUFVALN(n) (PBA_PBAO_WBUFVAL0 + ((PBA_PBAO_WBUFVAL1 - PBA_PBAO_WBUFVAL0) * (n)))
    #define PBA_PBAO_WBUFVAL0 0x01011818
    #define PBA_PBAO_WBUFVAL1 0x01011819

    #define PBA_PBAO_BARN(n) (PBA_PBAO_BAR0 + ((PBA_PBAO_BAR1 - PBA_PBAO_BAR0) * (n)))
    #define PBA_PBAO_BAR0     0x0101181a
    #define PBA_PBAO_BAR1     0x0101181b
    #define PBA_PBAO_BAR2     0x0101181c
    #define PBA_PBAO_BAR3     0x0101181d

    #define PBA_PBAO_BARMSKN(n) (PBA_PBAO_BARMSK0 + ((PBA_PBAO_BARMSK1 - PBA_PBAO_BARMSK0) * (n)))
    #define PBA_PBAO_BARMSK0  0x0101181e
    #define PBA_PBAO_BARMSK1  0x0101181f
    #define PBA_PBAO_BARMSK2  0x01011820
    #define PBA_PBAO_BARMSK3  0x01011821

    #define PBA_PBAO_PBTXTN(n) (PBA_PBAO_PBTXT0 + ((PBA_PBAO_PBTXT1 - PBA_PBAO_PBTXT0) * (n)))
    #define PBA_PBAO_PBTXT0   0x01011822
    #define PBA_PBAO_PBTXT1   0x01011823
    #define PBA_PBAO_PBTXT2   0x01011824
    #define PBA_PBAO_PBTXT3   0x01011825


    //
    // PBAF registers for OCC - 0x060114nn
    //
    // Notes:
    // - 20230208: Only the FIR and ERRRPT regs are defined below. The rest can
    //             be defined when needed.
    //

    #define PBA_PBAF_FIR_RWCLR     0x06011400
    #define PBA_PBAF_FIR_OR        0x06011401
    #define PBA_PBAF_FIRMASK_RWCLR 0x06011402
    #define PBA_PBAF_FIRMASK_OR    0x06011403
    #define PBA_PBAF_CFGXSTOP      0x06011404
    #define PBA_PBAF_CFGRECOV      0x06011405
    #define PBA_PBAF_CFGATTN       0x06011406
    #define PBA_PBAF_CFGLXSTOP     0x06011407
    #define PBA_PBAF_LFIRWOF_RWCLR 0x06011408
    #define PBA_PBAF_ERRRPT0_RWCLR 0x0601140c // Any write clears ERRRPT1/2
    #define PBA_PBAF_ERRRPT1_RO    0x0601140d
    #define PBA_PBAF_ERRRPT2_RO    0x0601140e


#elif defined (__TCC_PLAT)


    //
    // Notes:
    // - 20230208: No regs defined below. We can define them when needed.
    //


    //
    // PBAO registers for TCC - 0x030114nn
    //


    //
    // PBAF registers for TCC - 0x020110nn
    //


#else


    #error "Either __OCC_PLAT or __TCC_PLAT must be defined by the application"


#endif // End of ifdef __{OCC,TCC}_PLAT


#endif // __PBA_REGISTER_ADDRESSES_H__
