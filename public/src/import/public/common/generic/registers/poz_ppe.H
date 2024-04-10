/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/common/generic/registers/poz_ppe.H $ */
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

#ifndef __poz_ppe_H_
#define __poz_ppe_H_


#ifndef __PPE_HCODE__
namespace scomt
{
namespace poz_ppe
{
#endif


//>> [TP_TPCHIP_PIB_SBE_SBEPM_LCL_DBG]
static const uint32_t TP_TPCHIP_PIB_SBE_SBEPM_LCL_DBG = 0x00000120ull;

static const uint32_t TP_TPCHIP_PIB_SBE_SBEPM_LCL_DBG_EN_DBG = 0;
static const uint32_t TP_TPCHIP_PIB_SBE_SBEPM_LCL_DBG_HALT_ON_XSTOP = 1;
static const uint32_t TP_TPCHIP_PIB_SBE_SBEPM_LCL_DBG_HALT_ON_TRIG = 2;
static const uint32_t TP_TPCHIP_PIB_SBE_SBEPM_LCL_DBG_RESERVED3 = 3;
static const uint32_t TP_TPCHIP_PIB_SBE_SBEPM_LCL_DBG_EN_INTR_ADDR = 4;
static const uint32_t TP_TPCHIP_PIB_SBE_SBEPM_LCL_DBG_EN_TRACE_EXTRA = 5;
static const uint32_t TP_TPCHIP_PIB_SBE_SBEPM_LCL_DBG_EN_TRACE_STALL = 6;
static const uint32_t TP_TPCHIP_PIB_SBE_SBEPM_LCL_DBG_EN_WAIT_CYCLES = 7;
static const uint32_t TP_TPCHIP_PIB_SBE_SBEPM_LCL_DBG_EN_FULL_SPEED = 8;
static const uint32_t TP_TPCHIP_PIB_SBE_SBEPM_LCL_DBG_RESERVED9 = 9;
static const uint32_t TP_TPCHIP_PIB_SBE_SBEPM_LCL_DBG_TRACE_MODE_SEL = 10;
static const uint32_t TP_TPCHIP_PIB_SBE_SBEPM_LCL_DBG_TRACE_MODE_SEL_LEN = 2;
static const uint32_t TP_TPCHIP_PIB_SBE_SBEPM_LCL_DBG_FIR_TRIGGER = 16;
static const uint32_t TP_TPCHIP_PIB_SBE_SBEPM_LCL_DBG_MIB_GPIO = 17;
static const uint32_t TP_TPCHIP_PIB_SBE_SBEPM_LCL_DBG_MIB_GPIO_LEN = 3;
static const uint32_t TP_TPCHIP_PIB_SBE_SBEPM_LCL_DBG_TRACE_DATA_SEL = 20;
static const uint32_t TP_TPCHIP_PIB_SBE_SBEPM_LCL_DBG_TRACE_DATA_SEL_LEN = 4;
//<< [TP_TPCHIP_PIB_SBE_SBEPM_LCL_DBG]

//>> [TP_TPCHIP_PIB_SBE_SBEPM_LCL_EIMR]
static const uint32_t TP_TPCHIP_PIB_SBE_SBEPM_LCL_EIMR_RW = 0x00000020ull;
static const uint32_t TP_TPCHIP_PIB_SBE_SBEPM_LCL_EIMR_WO_CLEAR = 0x00000038ull;
static const uint32_t TP_TPCHIP_PIB_SBE_SBEPM_LCL_EIMR_WO_OR = 0x00000030ull;

static const uint32_t TP_TPCHIP_PIB_SBE_SBEPM_LCL_EIMR_SBE_START0 = 0;
static const uint32_t TP_TPCHIP_PIB_SBE_SBEPM_LCL_EIMR_SBE_START1 = 1;
static const uint32_t TP_TPCHIP_PIB_SBE_SBEPM_LCL_EIMR_SBE_INTR0 = 2;
static const uint32_t TP_TPCHIP_PIB_SBE_SBEPM_LCL_EIMR_SBE_INTR1 = 3;
static const uint32_t TP_TPCHIP_PIB_SBE_SBEPM_LCL_EIMR_DRTM_REQ = 4;
static const uint32_t TP_TPCHIP_PIB_SBE_SBEPM_LCL_EIMR_SBEFIFO_RESET = 5;
static const uint32_t TP_TPCHIP_PIB_SBE_SBEPM_LCL_EIMR_SBEFIFO_DATA = 6;
static const uint32_t TP_TPCHIP_PIB_SBE_SBEPM_LCL_EIMR_SPARE = 7;
static const uint32_t TP_TPCHIP_PIB_SBE_SBEPM_LCL_EIMR_TRIG = 8;
static const uint32_t TP_TPCHIP_PIB_SBE_SBEPM_LCL_EIMR_XSTOP = 9;
static const uint32_t TP_TPCHIP_PIB_SBE_SBEPM_LCL_EIMR_SBEHFIFO_RESET = 10;
static const uint32_t TP_TPCHIP_PIB_SBE_SBEPM_LCL_EIMR_SBEHFIFO_DATA = 11;
//<< [TP_TPCHIP_PIB_SBE_SBEPM_LCL_EIMR]

//>> [TP_TPCHIP_PIB_SBE_SBEPM_LCL_EINR]
static const uint32_t TP_TPCHIP_PIB_SBE_SBEPM_LCL_EINR = 0x000000a0ull;

static const uint32_t TP_TPCHIP_PIB_SBE_SBEPM_LCL_EINR_SBE_START0 = 0;
static const uint32_t TP_TPCHIP_PIB_SBE_SBEPM_LCL_EINR_SBE_START1 = 1;
static const uint32_t TP_TPCHIP_PIB_SBE_SBEPM_LCL_EINR_SBE_INTR0 = 2;
static const uint32_t TP_TPCHIP_PIB_SBE_SBEPM_LCL_EINR_SBE_INTR1 = 3;
static const uint32_t TP_TPCHIP_PIB_SBE_SBEPM_LCL_EINR_DRTM_REQ = 4;
static const uint32_t TP_TPCHIP_PIB_SBE_SBEPM_LCL_EINR_SBEFIFO_RESET = 5;
static const uint32_t TP_TPCHIP_PIB_SBE_SBEPM_LCL_EINR_SBEFIFO_DATA = 6;
static const uint32_t TP_TPCHIP_PIB_SBE_SBEPM_LCL_EINR_SPARE = 7;
static const uint32_t TP_TPCHIP_PIB_SBE_SBEPM_LCL_EINR_TRIG = 8;
static const uint32_t TP_TPCHIP_PIB_SBE_SBEPM_LCL_EINR_XSTOP = 9;
static const uint32_t TP_TPCHIP_PIB_SBE_SBEPM_LCL_EINR_SBEHFIFO_RESET = 10;
static const uint32_t TP_TPCHIP_PIB_SBE_SBEPM_LCL_EINR_SBEHFIFO_DATA = 11;
//<< [TP_TPCHIP_PIB_SBE_SBEPM_LCL_EINR]

//>> [TP_TPCHIP_PIB_SBE_SBEPM_LCL_EIPR]
static const uint32_t TP_TPCHIP_PIB_SBE_SBEPM_LCL_EIPR_RW = 0x00000040ull;
static const uint32_t TP_TPCHIP_PIB_SBE_SBEPM_LCL_EIPR_WO_CLEAR = 0x00000058ull;
static const uint32_t TP_TPCHIP_PIB_SBE_SBEPM_LCL_EIPR_WO_OR = 0x00000050ull;

static const uint32_t TP_TPCHIP_PIB_SBE_SBEPM_LCL_EIPR_SBE_START0 = 0;
static const uint32_t TP_TPCHIP_PIB_SBE_SBEPM_LCL_EIPR_SBE_START1 = 1;
static const uint32_t TP_TPCHIP_PIB_SBE_SBEPM_LCL_EIPR_SBE_INTR0 = 2;
static const uint32_t TP_TPCHIP_PIB_SBE_SBEPM_LCL_EIPR_SBE_INTR1 = 3;
static const uint32_t TP_TPCHIP_PIB_SBE_SBEPM_LCL_EIPR_DRTM_REQ = 4;
static const uint32_t TP_TPCHIP_PIB_SBE_SBEPM_LCL_EIPR_SBEFIFO_RESET = 5;
static const uint32_t TP_TPCHIP_PIB_SBE_SBEPM_LCL_EIPR_SBEFIFO_DATA = 6;
static const uint32_t TP_TPCHIP_PIB_SBE_SBEPM_LCL_EIPR_SPARE = 7;
static const uint32_t TP_TPCHIP_PIB_SBE_SBEPM_LCL_EIPR_TRIG = 8;
static const uint32_t TP_TPCHIP_PIB_SBE_SBEPM_LCL_EIPR_XSTOP = 9;
static const uint32_t TP_TPCHIP_PIB_SBE_SBEPM_LCL_EIPR_SBEHFIFO_RESET = 10;
static const uint32_t TP_TPCHIP_PIB_SBE_SBEPM_LCL_EIPR_SBEHFIFO_DATA = 11;
//<< [TP_TPCHIP_PIB_SBE_SBEPM_LCL_EIPR]

//>> [TP_TPCHIP_PIB_SBE_SBEPM_LCL_EISR]
static const uint32_t TP_TPCHIP_PIB_SBE_SBEPM_LCL_EISR_RW = 0x00000000ull;
static const uint32_t TP_TPCHIP_PIB_SBE_SBEPM_LCL_EISR_WO_CLEAR = 0x00000018ull;
static const uint32_t TP_TPCHIP_PIB_SBE_SBEPM_LCL_EISR_WO_OR = 0x00000010ull;

static const uint32_t TP_TPCHIP_PIB_SBE_SBEPM_LCL_EISR_SBE_START0 = 0;
static const uint32_t TP_TPCHIP_PIB_SBE_SBEPM_LCL_EISR_SBE_START1 = 1;
static const uint32_t TP_TPCHIP_PIB_SBE_SBEPM_LCL_EISR_SBE_INTR0 = 2;
static const uint32_t TP_TPCHIP_PIB_SBE_SBEPM_LCL_EISR_SBE_INTR1 = 3;
static const uint32_t TP_TPCHIP_PIB_SBE_SBEPM_LCL_EISR_DRTM_REQ = 4;
static const uint32_t TP_TPCHIP_PIB_SBE_SBEPM_LCL_EISR_SBEFIFO_RESET = 5;
static const uint32_t TP_TPCHIP_PIB_SBE_SBEPM_LCL_EISR_SBEFIFO_DATA = 6;
static const uint32_t TP_TPCHIP_PIB_SBE_SBEPM_LCL_EISR_SPARE = 7;
static const uint32_t TP_TPCHIP_PIB_SBE_SBEPM_LCL_EISR_TRIG = 8;
static const uint32_t TP_TPCHIP_PIB_SBE_SBEPM_LCL_EISR_XSTOP = 9;
static const uint32_t TP_TPCHIP_PIB_SBE_SBEPM_LCL_EISR_SBEHFIFO_RESET = 10;
static const uint32_t TP_TPCHIP_PIB_SBE_SBEPM_LCL_EISR_SBEHFIFO_DATA = 11;
//<< [TP_TPCHIP_PIB_SBE_SBEPM_LCL_EISR]

//>> [TP_TPCHIP_PIB_SBE_SBEPM_LCL_EISTR]
static const uint32_t TP_TPCHIP_PIB_SBE_SBEPM_LCL_EISTR = 0x00000080ull;

static const uint32_t TP_TPCHIP_PIB_SBE_SBEPM_LCL_EISTR_INTERRUPT_STATUS = 0;
static const uint32_t TP_TPCHIP_PIB_SBE_SBEPM_LCL_EISTR_INTERRUPT_STATUS_LEN = 10;
//<< [TP_TPCHIP_PIB_SBE_SBEPM_LCL_EISTR]

//>> [TP_TPCHIP_PIB_SBE_SBEPM_LCL_EITR]
static const uint32_t TP_TPCHIP_PIB_SBE_SBEPM_LCL_EITR_RW = 0x00000060ull;
static const uint32_t TP_TPCHIP_PIB_SBE_SBEPM_LCL_EITR_WO_CLEAR = 0x00000078ull;
static const uint32_t TP_TPCHIP_PIB_SBE_SBEPM_LCL_EITR_WO_OR = 0x00000070ull;

static const uint32_t TP_TPCHIP_PIB_SBE_SBEPM_LCL_EITR_SBE_START0 = 0;
static const uint32_t TP_TPCHIP_PIB_SBE_SBEPM_LCL_EITR_SBE_START1 = 1;
static const uint32_t TP_TPCHIP_PIB_SBE_SBEPM_LCL_EITR_SBE_INTR0 = 2;
static const uint32_t TP_TPCHIP_PIB_SBE_SBEPM_LCL_EITR_SBE_INTR1 = 3;
static const uint32_t TP_TPCHIP_PIB_SBE_SBEPM_LCL_EITR_DRTM_REQ = 4;
static const uint32_t TP_TPCHIP_PIB_SBE_SBEPM_LCL_EITR_SBEFIFO_RESET = 5;
static const uint32_t TP_TPCHIP_PIB_SBE_SBEPM_LCL_EITR_SBEFIFO_DATA = 6;
static const uint32_t TP_TPCHIP_PIB_SBE_SBEPM_LCL_EITR_SPARE = 7;
static const uint32_t TP_TPCHIP_PIB_SBE_SBEPM_LCL_EITR_TRIG = 8;
static const uint32_t TP_TPCHIP_PIB_SBE_SBEPM_LCL_EITR_XSTOP = 9;
static const uint32_t TP_TPCHIP_PIB_SBE_SBEPM_LCL_EITR_SBEHFIFO_RESET = 10;
static const uint32_t TP_TPCHIP_PIB_SBE_SBEPM_LCL_EITR_SBEHFIFO_DATA = 11;
//<< [TP_TPCHIP_PIB_SBE_SBEPM_LCL_EITR]

//>> [TP_TPCHIP_PIB_SBE_SBEPM_LCL_TBR]
static const uint32_t TP_TPCHIP_PIB_SBE_SBEPM_LCL_TBR = 0x00000140ull;

static const uint32_t TP_TPCHIP_PIB_SBE_SBEPM_LCL_TBR_SBE_LCL_TBR_TIMEBASE = 0;
static const uint32_t TP_TPCHIP_PIB_SBE_SBEPM_LCL_TBR_SBE_LCL_TBR_TIMEBASE_LEN = 32;
//<< [TP_TPCHIP_PIB_SBE_SBEPM_LCL_TBR]

//>> [TP_TPCHIP_PIB_SBE_SBEPM_LCL_TSEL]
static const uint32_t TP_TPCHIP_PIB_SBE_SBEPM_LCL_TSEL = 0x00000100ull;

static const uint32_t TP_TPCHIP_PIB_SBE_SBEPM_LCL_TSEL_FIT_SEL = 0;
static const uint32_t TP_TPCHIP_PIB_SBE_SBEPM_LCL_TSEL_FIT_SEL_LEN = 4;
static const uint32_t TP_TPCHIP_PIB_SBE_SBEPM_LCL_TSEL_WATCHDOG_SEL = 4;
static const uint32_t TP_TPCHIP_PIB_SBE_SBEPM_LCL_TSEL_WATCHDOG_SEL_LEN = 4;
//<< [TP_TPCHIP_PIB_SBE_SBEPM_LCL_TSEL]

//>> [TP_TPCHIP_PIB_SBE_SBEPRV_LCL_IVPR]
static const uint32_t TP_TPCHIP_PIB_SBE_SBEPRV_LCL_IVPR = 0x00000160ull;

static const uint32_t TP_TPCHIP_PIB_SBE_SBEPRV_LCL_IVPR_SBE_LCL_IVPR = 0;
static const uint32_t TP_TPCHIP_PIB_SBE_SBEPRV_LCL_IVPR_SBE_LCL_IVPR_LEN = 23;
//<< [TP_TPCHIP_PIB_SBE_SBEPRV_LCL_IVPR]

//>> [TP_TPCHIP_PIB_SBE_SBEPRV_LCL_LFR_SCRATCH]
static const uint32_t TP_TPCHIP_PIB_SBE_SBEPRV_LCL_LFR_SCRATCH_PPE1 = 0xc0002050ull;
static const uint32_t TP_TPCHIP_PIB_SBE_SBEPRV_LCL_LFR_SCRATCH_PPE2 = 0xc0002058ull;
static const uint32_t TP_TPCHIP_PIB_SBE_SBEPRV_LCL_LFR_SCRATCH_RW = 0xc0002040ull;

static const uint32_t TP_TPCHIP_PIB_SBE_SBEPRV_LCL_LFR_SCRATCH_SBE_LCL_LFR_SCRATCH = 0;
static const uint32_t TP_TPCHIP_PIB_SBE_SBEPRV_LCL_LFR_SCRATCH_SBE_LCL_LFR_SCRATCH_LEN = 64;
//<< [TP_TPCHIP_PIB_SBE_SBEPRV_LCL_LFR_SCRATCH]

//>> [TP_TPCHIP_PIB_SBE_SBEPRV_LCL_TPM_RESET]
static const uint32_t TP_TPCHIP_PIB_SBE_SBEPRV_LCL_TPM_RESET_PPE1 = 0xc0002030ull;
static const uint32_t TP_TPCHIP_PIB_SBE_SBEPRV_LCL_TPM_RESET_PPE2 = 0xc0002038ull;
static const uint32_t TP_TPCHIP_PIB_SBE_SBEPRV_LCL_TPM_RESET_RW = 0xc0002020ull;

static const uint32_t TP_TPCHIP_PIB_SBE_SBEPRV_LCL_TPM_RESET_SBE_LCL_TPM_RESET_TPMRST = 0;
//<< [TP_TPCHIP_PIB_SBE_SBEPRV_LCL_TPM_RESET]

//>> [TP_TPCHIP_PIB_SBE_SBEPRV_PRV_MISC]
static const uint32_t TP_TPCHIP_PIB_SBE_SBEPRV_PRV_MISC_RW = 0xc0002000ull;
static const uint32_t TP_TPCHIP_PIB_SBE_SBEPRV_PRV_MISC_WO_CLEAR = 0xc0002018ull;
static const uint32_t TP_TPCHIP_PIB_SBE_SBEPRV_PRV_MISC_WO_OR = 0xc0002010ull;

static const uint32_t TP_TPCHIP_PIB_SBE_SBEPRV_PRV_MISC_PRV_MISC_TPSBE_TPBR_SBE_INTR = 0;
static const uint32_t TP_TPCHIP_PIB_SBE_SBEPRV_PRV_MISC_PRV_MISC_RESERVED_19 = 1;
static const uint32_t TP_TPCHIP_PIB_SBE_SBEPRV_PRV_MISC_PRV_MISC_RESERVED_18 = 2;
static const uint32_t TP_TPCHIP_PIB_SBE_SBEPRV_PRV_MISC_PRV_MISC_RESERVED_18_LEN = 2;
static const uint32_t TP_TPCHIP_PIB_SBE_SBEPRV_PRV_MISC_PRV_MISC_SBE_EXTERNAL_FIRS = 4;
static const uint32_t TP_TPCHIP_PIB_SBE_SBEPRV_PRV_MISC_PRV_MISC_SBE_EXTERNAL_FIRS_LEN = 4;
static const uint32_t TP_TPCHIP_PIB_SBE_SBEPRV_PRV_MISC_SBE_SPICTL_HARD_RESET = 8;
static const uint32_t TP_TPCHIP_PIB_SBE_SBEPRV_PRV_MISC_SBE_SPICTL_HARD_RESET_LEN = 5;
static const uint32_t TP_TPCHIP_PIB_SBE_SBEPRV_PRV_MISC_PRV_MISC_TPSBE_TPOCC_HALT_COMPLEX = 13;
static const uint32_t TP_TPCHIP_PIB_SBE_SBEPRV_PRV_MISC_PRV_MISC_RESERVED_16 = 14;
static const uint32_t TP_TPCHIP_PIB_SBE_SBEPRV_PRV_MISC_PRV_MISC_RESERVED_16_LEN = 2;
static const uint32_t TP_TPCHIP_PIB_SBE_SBEPRV_PRV_MISC_PRV_MISC_I2C_TIMEOUT_VALUE = 16;
static const uint32_t TP_TPCHIP_PIB_SBE_SBEPRV_PRV_MISC_PRV_MISC_I2C_TIMEOUT_VALUE_LEN = 32;
//<< [TP_TPCHIP_PIB_SBE_SBEPRV_PRV_MISC]

//>>THE END<<

#ifndef __PPE_HCODE__
}
}
#endif
#endif
