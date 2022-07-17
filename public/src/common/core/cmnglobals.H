/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/common/core/cmnglobals.H $                         */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2017,2022                        */
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
#ifndef __CMN_GLOBALS_H
#define __CMN_GLOBALS_H

#include "sbeutil.H"

#define CMN_GLOBAL cmnGlobal

enum sbeRole
{
    SBE_ROLE_MASTER = 0,
    SBE_ROLE_SLAVE = 1,
    SBE_ROLE_ALT_MASTER = 2,
};

class CMNGlobalsSingleton
{
    public:
    // Disable copy contructor and assingment operator
    CMNGlobalsSingleton(const CMNGlobalsSingleton&) = delete;
    CMNGlobalsSingleton& operator=(const CMNGlobalsSingleton&) = delete;
    static CMNGlobalsSingleton& getInstance();
    sbeRole SBERole;

    // SBE Frequency.
    uint32_t sbefreq;

    // Hreset flow.
    static bool isHreset;

    private:
    // Constructor
    CMNGlobalsSingleton():
    SBERole(SBE_ROLE_MASTER),
    sbefreq(( 133 * 1000 * 1000)/SBE::SBE_TO_NEST_FREQ_FACTOR)
    {
    }
};

extern CMNGlobalsSingleton* cmnGlobal;
//TODO: P11SBE Porting
//This will be changed in P11.

//This is a struct defintion for the local register 0xC0002040.
//This local register will be used to save the SPI clock divider and round trip
//delay settings
//MPIPL in progress setting
//What kind of reset is triggered in 0x50008, since otprom clears the reset bit
// ipl_reset which is the cbs_start flow
// hreset which is the runtime reset flow
// hreset_done bit to indicate SBE finished Hreset flow and back up to receive
// commands over fifo.
typedef struct sbe_local_LFR {
    uint32_t spi_clock_divider: 12;       //Bit0-11 : odyssey,p11 : clock divider bits
    //TODO: For p11 need to map below bits
    uint32_t boot_selection : 2;          //Bit 12-13 : odyssey,p11 : select if primary or seconday or golden
                                          //            (golden supported only for odyssey)
    uint32_t mpipl : 1;                   //Bit 14 : p11 : Set if mpipl is triggered
    uint32_t ipl_reset : 1;               //Bit 15 : p11 :  Set if SBE is reset via 50008:bit12
    uint32_t hreset : 1;                  //Bit 16 : odyssey,p11 :  Set if SBE is reset via 50008:bit13(HRESET)
    uint32_t reserved : 1;                //Bit 17 : reserved
    uint32_t mpipl_reset_done : 1;        //Bit 18 : p11 : Set if Mpipl flow is done in FW
    uint32_t secure_mode : 1;             //Bit 19  : odyssey,p11 :  Secure mode bit indication
    uint32_t round_trip_delay : 4;        //Bit 20-23 : p11 : round trip delay
    uint32_t tpm_spi_clock_freq_Mhz : 8;  //Bit 24-31 : p11 : TPM Spi clock frequency in MHz
    uint32_t tpm_spi_clock_delay : 4;     //Bit 32-35 : p11 : TPM Spi clock delay encoded
    uint32_t disable_scom_filtering : 1;   //Bit 36 : odyssey,p11 : Set if scom filtering needs to be disabled
    uint32_t disable_invalid_scom_addr_check : 1 ; //Bit 37 : odyssey,p11 : Set if invalid address check needs to be disabled
    uint32_t bitsreserved : 10;           //Bit 38-47 Reserved
    uint32_t pau_freq_in_mhz : 16;        //Bit 48-63 : odyssey,p11 :  PAU system frequency in MHz

    sbe_local_LFR()
    {
        spi_clock_divider = 0;
        boot_selection = 0;
        mpipl = 0;
        ipl_reset = 0;
        hreset = 0;
        reserved = 0;
        mpipl_reset_done = 0;
        round_trip_delay = 0;
        secure_mode = 0;
        pau_freq_in_mhz = 0;
        tpm_spi_clock_freq_Mhz = 0;
        tpm_spi_clock_delay = 0;
        disable_scom_filtering = 0;
        disable_invalid_scom_addr_check = 0;
        // This is required else these gets initialised with dummy data
        bitsreserved = 0;
    }
}sbe_local_LFR;

// This is struct definition for scratch register 13, scom 50184, cfam 2984
// This contains the TPM SPI Clock divider and clock delay setting from the
// user as part of SBE config update istep. SBE should always check the valid
// bit for this register in scratch reg8 to initialise SPI4 engine with these
typedef struct sbe_scratch_reg13 {
    uint32_t tpm_spi_clock_divider: 12;   // Bit0-11 clock divider Bits
    uint32_t tpm_spi_clock_delay: 4;      // Bit12-15 round trip delay
    uint32_t unused: 16;                  // unused
    uint32_t reserved;                    // reserved

    sbe_scratch_reg13()
    {
        tpm_spi_clock_divider = 0;
        tpm_spi_clock_delay = 0;
        unused = 0;
        reserved = 0;
    }
}sbe_scratch_reg13;

// This is the struct definition for scratch register 13, to be reused in the
// hreset path to support Mpipl related use-case.
typedef struct sbe_scratch_reg13_reuse {
    uint32_t unused :16;                  // unused
    uint32_t pau_freq_in_mhz :16;         // PAU frequency in MHz
    uint32_t reserved;                    // reserved

    sbe_scratch_reg13_reuse()
    {
        unused = 0;
        pau_freq_in_mhz = 0;
        reserved = 0;
    }
}sbe_scratch_reg13_reuse;

// This is the struct definition for scratch register 16, to be used in the
// CBS path to support Hreset use-case
typedef struct sbe_scratch_reg16 {
    uint32_t spi_clock_divider :12;       // Bit0-11 clock divider Bits
    uint32_t spi_clock_delay :4;          // Bit12-15 round trip delay
    uint32_t tpm_spi_clock_freq_Mhz :8;   // Bit16-23 tpm clock divider
    uint32_t tpm_spi_clock_delay :4;      // Bit24-27 tpm clock delay
    uint32_t unused :4;                   // Bit28-31 unused
    uint32_t reserved;                    // reserved

    sbe_scratch_reg16()
    {
        spi_clock_divider = 0;
        spi_clock_delay = 0;
        tpm_spi_clock_freq_Mhz = 0;
        tpm_spi_clock_delay = 0;
        unused = 0;
        reserved = 0;
    }
}sbe_scratch_reg16;

#endif //__SBE_GLOBALS_H
