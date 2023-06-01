/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/hwp/generic/perv/poz_sbe_spi_cmd.C $ */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2022,2023                        */
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
/*
 * @file: poz_sbe_spi_cmd.C
 *
 * @brief This file contains and handle for SPI operations
 *
 */

#include "poz_sbe_spi_cmd.H"
#include "endian.h"

// This is required for PPE-simics compile environment
#ifdef __PPE__
    #include "sbeutil.H"
#endif

// Memcpy functionality
#include <string.h>

// SPI commands
#define SPI_SLAVE_WR_CMD   0x0200000000000000ULL
#define SPI_SLAVE_RD_CMD   0x0300000000000000ULL
#define SPI_SLAVE_RD_STAT  0x0500000000000000ULL
#define SPI_SLAVE_WR_EN    0x0600000000000000ULL
#define SPI_SLAVE_ID_CMD   0x9F00000000000000ULL
#define SPI_SLAVE_FLG_STAT 0x7000000000000000ULL
#define SPI_SLAVE_FLG_CLR  0x5000000000000000ULL

#define SECTOR_NUM_CHECK(sector, address) ((address >= (sector * NOR_FLASH_SECTOR_SIZE)) && (address < ((sector + 1) * NOR_FLASH_SECTOR_SIZE)))
#define SECTOR_00 0x00
#define SECTOR_72 0x48

// Timeout values so not stuck in wait loops forever
#ifdef __PPE__
    //SBE will use below delay. This is to reduce the SBE boot time.
    constexpr uint64_t SPI_TIMEOUT_DELAY_NS            = 1000;       // 1 micro sec
    constexpr uint64_t SPI_TIMEOUT_DELAY_NS_SIM_CYCLES = 1000;       // 1 micro sec
    constexpr uint64_t SPI_TIMEOUT_MAX_WAIT_COUNT      = 10000000;   // Max wait count = 10 seconds
#else
    constexpr uint64_t SPI_TIMEOUT_DELAY_NS            = 1000000; // 1 msec
    constexpr uint64_t SPI_TIMEOUT_DELAY_NS_SIM_CYCLES = 1000000; // 1 msec
    constexpr uint64_t SPI_TIMEOUT_MAX_WAIT_COUNT      = 10000;   // Max wait count = 10 seconds
#endif

using namespace fapi2;

// TPM SPI command
// From Trusted Computing Group(TCG) 2.0 spec for SPI Bit Protocol
// https://trustedcomputinggroup.org/wp-content/uploads/PC-Client-Specific-Platform-TPM-Profile-for-TPM-2p0-v1p04_r0p37_pub-1.pdf
typedef union
{
    struct
    {
        uint32_t readNotWrite : 1;
        uint32_t reserved : 1;
        uint32_t len : 6;
        uint32_t addr : 24;
        uint32_t reserved2: 32;
    } cmd_bits;
    uint64_t val;
} tpmSpiCmd_t;

///////////////////////////////////////////
///////// poz_spi_clock_init //////////////
///////////////////////////////////////////
fapi2::ReturnCode poz_spi_clock_init (
    const SpiControlHandle& i_spiHandle)
{
    fapi2::buffer<uint64_t> data64 = 0;
    fapi2::buffer<uint16_t> attr_tpm_spi_bus_div = 0;

    const uint32_t clockRegAddr = i_spiHandle.base_addr + SPIM_CLOCKCONFIGREG;
    FAPI_INF("poz_spi_clock_init: clockRegAddr = [0x%08X]", clockRegAddr);
    FAPI_TRY(getScom(i_spiHandle.target_chip, clockRegAddr, data64));
    FAPI_INF("poz_spi_clock_init: data64 = [0x%08X%08X]", data64 >> 32, data64 & 0xFFFFFFFF);

    // For now, just update FSI SPI clock divider and SCK receive delay if
    // applicable.  The SPI clock divider is a ratio of unit logic to SCK, with
    // a minimum value of 0x004.  Receive delay is given in units of unit clock
    // and is used to compensate for internal and external delays.
    if(i_spiHandle.pibAccess) // i.e. FSI SPI engine
    {
        // Clear the mmSPIsm_enable
        data64.clearBit(31);

        FAPI_INF("poz_spi_clock_init: data64 = [0x%08X%08X]", data64 >> 32, data64 & 0xFFFFFFFF);
        FAPI_TRY(putScom(i_spiHandle.target_chip, clockRegAddr, data64));
    }
    else
    {
        // TODO RTC:260631 Support dynamic calculation of SPI TPM clock config
        // For PIB access of engine 4 (i.e. TPM), the following clock setup register is
        // known to work.
        if (i_spiHandle.engine == SPI_ENGINE_TPM)
        {
            // Default Value
            // SCK_CLOCK_DIVIDER: 0x015
            //      PAU_freq = 0x0855 MHz in Denali MRW
            //      spi_clock_freq = 12 MHz
            //      PIB_frequency = PAU_freq / 4
            //      divider = ( PIB_frequency / (spi_clock_freq * 2) ) - 1
            // SCK_RECEIVE_DELAY: 0x01 (7 clock cycles delay)
            // SCK_ECC_SPIMM_ADDR_CORR_DIS: 0x1  no_ecc_address_correction
            // SCK_ECC_CONTROL: 0x01  transparent_read
            auto default_data64 = 0x0150100A00000000ULL;

            // But also grab the attribute that might have updated values
            fapi2::buffer<uint16_t> attr_tpm_spi_bus_div = 0x0157;
            // TODO:P11_cleanup
            /*
            FAPI_TRY(FAPI_ATTR_GET(fapi2::ATTR_TPM_SPI_BUS_DIV,
                                   i_spiHandle.target_chip,
                                   attr_tpm_spi_bus_div),
                     "Error from FAPI_ATTR_GET (ATTR_TPM_SPI_BUS_DIV");
            */

            // Start with default_data64 and then map in ATTR_TPM_SPI_BUS_DIV values:
            // - SCK_CLOCK_DIVIDER is bits 0:11 in attribute and config reg
            // - SCK_RECEIVE_DELAY is bits 12:15 in attribute, but gets
            //   translated to bits 12:19 in the config reg
            data64 = default_data64;

            // Set SCK_CLOCK_DIVIDER
            data64.insertFromRight< 0, 12 >(attr_tpm_spi_bus_div.getBits< 0, 12>());

            // Set SCK_RECEIVE_DELAY
            data64.insertFromRight< 12, 8 >(0x80 >> attr_tpm_spi_bus_div.getBits< 12, 4>());

            FAPI_INF("Applied attr_tpm_spi_bus_div 0x%.4X to default_data="
                     "0x%.16llX to get data64=0x%.16llX ",
                     attr_tpm_spi_bus_div, default_data64, data64);

            // Write the config reg
            FAPI_TRY(putScom(i_spiHandle.target_chip, clockRegAddr, data64));
        }
    }

fapi_try_exit:
    return fapi2::current_err;
}


///////////////////////////////////////////
//////////// spi_master_lock //////////////
///////////////////////////////////////////
fapi2::ReturnCode
spi_master_lock(SpiControlHandle& i_handle, uint64_t i_pib_master_id)
{
    // The value written to the reg doesn't matter, only that bit zero is set
    // If someone else is currently holding the lock this access will fail
    fapi2::buffer<uint64_t> data64 = 0x8000000000000000ULL;

    FAPI_TRY(putScom( i_handle.target_chip,
                      i_handle.base_addr | SPIM_CONFIGREG1, data64));

fapi_try_exit:
    return fapi2::current_err;
}


///////////////////////////////////////////
///////// spi_master_unlock ///////////////
///////////////////////////////////////////
fapi2::ReturnCode
spi_master_unlock(SpiControlHandle& i_handle, uint64_t i_pib_master_id)
{
    fapi2::buffer<uint64_t> data64 = 0;

    if (!fapi2::is_platform<fapi2::PLAT_CRONUS>())
    {
        // Figure out who owns it now so we can take it away
        FAPI_TRY(getScom( i_handle.target_chip,
                          i_handle.base_addr | SPIM_CONFIGREG1, data64));

        // Clear the lock bit
        data64.clearBit(0);
    }

    FAPI_TRY(putScom( i_handle.target_chip,
                      i_handle.base_addr | SPIM_CONFIGREG1, data64) );

fapi_try_exit:
    return fapi2::current_err;
}


///////////////////////////////////////////
////////////// is_ecc_on //////////////////
///////////////////////////////////////////
// ECC methods for SPI slave status reads
static fapi2::ReturnCode
is_ecc_on(SpiControlHandle& i_handle, bool& o_ecc)
{
    fapi2::buffer<uint64_t> l_data = 0;
    uint32_t l_ecc_cntl = 0;

    FAPI_TRY(getScom(i_handle.target_chip, i_handle.base_addr + SPIM_CLOCKCONFIGREG, l_data));

    l_data.extractToRight(l_ecc_cntl, 29, 2);

    if( l_ecc_cntl % 2 == 0)
    {
        o_ecc = true;
    }
    else
    {
        o_ecc = false;
    }

fapi_try_exit:
    return fapi2::current_err;
}


///////////////////////////////////////////
///////////// spi_set_ecc_on //////////////
///////////////////////////////////////////
static fapi2::ReturnCode
spi_set_ecc_on(SpiControlHandle& i_handle)
{
    fapi2::buffer<uint64_t> l_data = 0;
    FAPI_TRY(getScom(i_handle.target_chip, i_handle.base_addr + SPIM_CLOCKCONFIGREG, l_data));

    l_data.clearBit<30>();

    FAPI_TRY(putScom(i_handle.target_chip, i_handle.base_addr + SPIM_CLOCKCONFIGREG, l_data));

fapi_try_exit:
    return fapi2::current_err;
}


///////////////////////////////////////////
////////// spi_set_ecc_off ////////////////
///////////////////////////////////////////
static fapi2::ReturnCode
spi_set_ecc_off(SpiControlHandle& i_handle)
{
    fapi2::buffer<uint64_t> l_data = 0;
    FAPI_TRY(getScom(i_handle.target_chip, i_handle.base_addr + SPIM_CLOCKCONFIGREG, l_data));

    l_data.setBit<30>();

    FAPI_TRY(putScom(i_handle.target_chip, i_handle.base_addr + SPIM_CLOCKCONFIGREG, l_data));

fapi_try_exit:
    return fapi2::current_err;
}


///////////////////////////////////////////
/////// spi_wait_for_tdr_empty ////////////
///////////////////////////////////////////
// Waits for transmit-data-register empty
static fapi2::ReturnCode
spi_wait_for_tdr_empty(SpiControlHandle& i_handle)
{
    fapi2::buffer<uint64_t> data64 = 0;

    uint64_t timeout = SPI_TIMEOUT_MAX_WAIT_COUNT;

    while(timeout)
    {
        FAPI_TRY(getScom( i_handle.target_chip,
                          i_handle.base_addr + SPIM_STATUSREG, data64));

        //checking for multiplexing error
        FAPI_ASSERT( (data64.getBit<50>() == 0),
                     fapi2::SBE_SPI_INVALID_PORT_MULTIPLEX_SET()
                     .set_CHIP_TARGET(i_handle.target_chip)
                     .set_SPI_ENGINE(i_handle.engine)
                     .set_BASE_ADDRESS(i_handle.base_addr + SPIM_STATUSREG)
                     .set_STATUS_REGISTER(data64),
                     "Port multiplexer setting error set in spi_wait_for_tdr_empty");

        if(!(data64.getBit<4>()))
        {
            break; //Wait until TX Buffer is empty
        }

        fapi2::delay(SPI_TIMEOUT_DELAY_NS, SPI_TIMEOUT_DELAY_NS_SIM_CYCLES);
        --timeout;
    }

    FAPI_ASSERT( timeout != 0,
                 fapi2::SBE_SPI_HANG_TIMEOUT()
                 .set_CHIP_TARGET(i_handle.target_chip)
                 .set_SPI_ENGINE(i_handle.engine)
                 .set_BASE_ADDRESS(i_handle.base_addr + SPIM_STATUSREG)
                 .set_STATUS_REGISTER(data64)
                 .set_TIMEOUT_MSEC(SPI_TIMEOUT_MAX_WAIT_COUNT),
                 "spi_wait_for_tdr_empty wait timeout" );

fapi_try_exit:
    return fapi2::current_err;
}


///////////////////////////////////////////
/////// spi_wait_for_rdr_full /////////////
///////////////////////////////////////////
// Waits for receive-data-register full
static fapi2::ReturnCode
spi_wait_for_rdr_full(SpiControlHandle& i_handle)
{
    fapi2::buffer<uint64_t> status_reg = 0;

    uint64_t timeout = SPI_TIMEOUT_MAX_WAIT_COUNT;

    while(timeout)
    {
        FAPI_TRY(getScom( i_handle.target_chip,
                          i_handle.base_addr + SPIM_STATUSREG, status_reg));

        //checking for multiplexing error
        FAPI_ASSERT( (status_reg.getBit<50>() == 0),
                     fapi2::SBE_SPI_INVALID_PORT_MULTIPLEX_SET()
                     .set_CHIP_TARGET(i_handle.target_chip)
                     .set_SPI_ENGINE(i_handle.engine)
                     .set_BASE_ADDRESS(i_handle.base_addr + SPIM_STATUSREG)
                     .set_STATUS_REGISTER(status_reg),
                     "Port multiplexer setting error set in spi_wait_for_rdr_full");

        if(status_reg.getBit<0>()) //Wait until RX Buffer is full
        {
            break;
        }

        fapi2::delay(SPI_TIMEOUT_DELAY_NS, SPI_TIMEOUT_DELAY_NS_SIM_CYCLES);
        --timeout;
    }

    FAPI_ASSERT( timeout != 0,
                 fapi2::SBE_SPI_HANG_TIMEOUT()
                 .set_CHIP_TARGET(i_handle.target_chip)
                 .set_SPI_ENGINE(i_handle.engine)
                 .set_BASE_ADDRESS(i_handle.base_addr + SPIM_STATUSREG)
                 .set_STATUS_REGISTER(status_reg)
                 .set_TIMEOUT_MSEC(SPI_TIMEOUT_MAX_WAIT_COUNT),
                 "spi_wait_for_rdr_full wait timeout" );

fapi_try_exit:
    return fapi2::current_err;
}


///////////////////////////////////////////
///////// spi_wait_for_idle ///////////////
///////////////////////////////////////////
// Waits for the fsm of the spi-master to be idle
static fapi2::ReturnCode
spi_wait_for_idle(SpiControlHandle& i_handle)
{
    fapi2::buffer<uint64_t> data64 = 0;

    uint64_t timeout = SPI_TIMEOUT_MAX_WAIT_COUNT;

    while(timeout)
    {
        FAPI_TRY(getScom( i_handle.target_chip,
                          i_handle.base_addr + SPIM_STATUSREG, data64));

        //checking for multiplexing error
        FAPI_ASSERT( (data64.getBit<50>() == 0),
                     fapi2::SBE_SPI_INVALID_PORT_MULTIPLEX_SET()
                     .set_CHIP_TARGET(i_handle.target_chip)
                     .set_SPI_ENGINE(i_handle.engine)
                     .set_BASE_ADDRESS(i_handle.base_addr + SPIM_STATUSREG)
                     .set_STATUS_REGISTER(data64),
                     "Port multiplexer setting error set in spi_wait_for_idle");

        if(data64.getBit<15>())  //seq fsm Idle
        {
            break;
        }

        fapi2::delay(SPI_TIMEOUT_DELAY_NS, SPI_TIMEOUT_DELAY_NS_SIM_CYCLES);
        --timeout;
    }

    FAPI_ASSERT( timeout != 0,
                 fapi2::SBE_SPI_HANG_TIMEOUT()
                 .set_CHIP_TARGET(i_handle.target_chip)
                 .set_SPI_ENGINE(i_handle.engine)
                 .set_BASE_ADDRESS(i_handle.base_addr + SPIM_STATUSREG)
                 .set_STATUS_REGISTER(data64)
                 .set_TIMEOUT_MSEC(SPI_TIMEOUT_MAX_WAIT_COUNT),
                 "spi_wait_for_idle wait timeout" );

fapi_try_exit:
    return fapi2::current_err;
}


///////////////////////////////////////////
/////// spi_restore_ecc_status ////////////
///////////////////////////////////////////
static fapi2::ReturnCode
spi_restore_ecc_status(SpiControlHandle& i_handle, fapi2::ReturnCode i_rc, bool i_ecc)
{
    if(i_ecc)
    {
        FAPI_TRY(spi_set_ecc_on(i_handle));
    }

fapi_try_exit:

    if (fapi2::current_err == fapi2::FAPI2_RC_SUCCESS)
    {
        // If no new error reported, send out the original error i_rc if any
        fapi2::current_err = i_rc;
    }

    return fapi2::current_err;
}


///////////////////////////////////////////
/////// spi_read_flag_status //////////////
///////////////////////////////////////////
static fapi2::ReturnCode
spi_read_flag_status(SpiControlHandle& i_handle, fapi2::buffer<uint64_t>& o_status)
{
    bool l_ecc = false;
    uint64_t SEQ = 0x1031411000000000ULL;
    SEQ |= ((uint64_t)((i_handle.slave)) << 56);

    FAPI_DBG("Enter spi_read_flag_status...");

    is_ecc_on(i_handle, l_ecc);

    if (l_ecc)
    {
        spi_set_ecc_off(i_handle);
    }

    //Send the read flag status register command(0x70)
    FAPI_TRY(putScom(i_handle.target_chip, i_handle.base_addr + SPIM_SEQREG, SEQ));
    FAPI_TRY(putScom( i_handle.target_chip, i_handle.base_addr + SPIM_TDR, SPI_SLAVE_FLG_STAT));
    FAPI_TRY(spi_wait_for_tdr_empty(i_handle));

    FAPI_TRY(spi_wait_for_rdr_full(i_handle));  //Wait for response

    FAPI_TRY(getScom(i_handle.target_chip, i_handle.base_addr + SPIM_RDR, o_status));

fapi_try_exit:
    return spi_restore_ecc_status(i_handle, fapi2::current_err, l_ecc);
}


///////////////////////////////////////////
/////// spi_clear_flag_status /////////////
///////////////////////////////////////////
static fapi2::ReturnCode
spi_clear_flag_status(SpiControlHandle& i_handle, fapi2::buffer<uint64_t>& o_status)
{
    bool l_ecc = false;
    uint8_t l_status = 0;
    o_status = 0;
    uint64_t SEQ = 0x1031100000000000ULL;
    SEQ |= (static_cast<uint64_t>((i_handle.slave)) << 56);

    FAPI_DBG("Enter spi_clear_flag_status...");

    is_ecc_on(i_handle, l_ecc);

    if (l_ecc)
    {
        spi_set_ecc_off(i_handle);
    }

    //Send the clear flag status register command(0x50)
    FAPI_TRY(putScom(i_handle.target_chip, i_handle.base_addr + SPIM_SEQREG, SEQ));
    FAPI_TRY(putScom(i_handle.target_chip, i_handle.base_addr + SPIM_TDR, SPI_SLAVE_FLG_CLR));

    FAPI_TRY(spi_wait_for_tdr_empty(i_handle));

    FAPI_TRY(spi_wait_for_idle(i_handle));

    FAPI_TRY(spi_read_flag_status(i_handle, o_status));

    FAPI_DBG("spi_clear_flag_status: 0x%08X%08X",
             (uint64_t(o_status) >> 32), static_cast<uint32_t>(uint64_t(o_status) & 0xFFFFFFFF));

    // check for flag status register value is reset for all error bits
    // bit 7: pgm/erase controller = 0:busy 1:ready
    // bit 6: erase suspend        = 0:clear 1:suspend
    // bit 5: erase                = 0:clear 1:failure or protection error
    // bit 4: program              = 0:clear 1:failure or protection error
    // bit 3: reserve              = 0
    // bit 2: program suspend      = 0:clear 1:suspend
    // bit 1: protection           = 0:clear 1:failure or protection error
    // bit 0: reserve              = 0
    o_status.extractToRight(l_status, 56, 8);

    FAPI_ASSERT( (l_status == 0x80),
                 fapi2::SBE_SPI_OPR_COMPLETION_CHECK_ERROR()
                 .set_CHIP_TARGET(i_handle.target_chip)
                 .set_SPI_ENGINE(i_handle.engine)
                 .set_BASE_ADDRESS(i_handle.base_addr + SPIM_RDR)
                 .set_STATUS_REGISTER(o_status),
                 "spi_clear_flag_status" );

fapi_try_exit:
    return spi_restore_ecc_status(i_handle, fapi2::current_err, l_ecc);
}


////////////////////////////////////////////////////////////
/////// spi_chk_addr_n_lock_status_clr_flag_error_bits /////
////////////////////////////////////////////////////////////
static fapi2::ReturnCode
spi_chk_addr_n_lock_status_clr_flag_error_bits(SpiControlHandle& i_handle,
        uint32_t i_address,
        fapi2::buffer<uint64_t>& io_status)
{
    // check for failing address is in sector 0 for side-0 or sector 72 in side-1
    // also if the protection bit is set. If set, clear off flag status register
    if ((SECTOR_NUM_CHECK(SECTOR_00, i_address) || SECTOR_NUM_CHECK(SECTOR_72 , i_address)) && io_status.getBit<62>())
    {
        FAPI_TRY(spi_clear_flag_status(i_handle, io_status));
    }
    else
    {
        // for all addresses (incl sector 0 & 72 with no protection bit and incl
        // all other sectors w/wo protection bit) clear off flag status register
        // but retain and return the original failed status value
        fapi2::buffer<uint64_t> data64 = 0;
        FAPI_TRY(spi_clear_flag_status(i_handle, data64));
    }

fapi_try_exit:
    return fapi2::current_err;
}


//@nlandi Note: in order for SPI slave status reads to not cause an ECC error, ECC must be turned off for
//              the duration of the read
//waits for write complete flag of the spi-slave
static fapi2::ReturnCode
spi_wait_for_write_complete(SpiControlHandle& i_handle,
                            uint32_t i_address,
                            fapi2::buffer<uint64_t>& o_status)
{
    bool l_ecc = false;

    uint64_t timeout = SPI_TIMEOUT_MAX_WAIT_COUNT;

    is_ecc_on(i_handle, l_ecc);

    if (l_ecc)
    {
        spi_set_ecc_off(i_handle);
    }

    uint64_t SEQ = 0x1031411000000000ULL;
    SEQ |= ((uint64_t)((i_handle.slave)) << 56);
    FAPI_TRY(putScom(i_handle.target_chip, i_handle.base_addr + SPIM_SEQREG, SEQ));

    while(timeout)
    {
        //Send the read flag status register command(0x70)
        FAPI_TRY(putScom( i_handle.target_chip,
                          i_handle.base_addr + SPIM_TDR, SPI_SLAVE_FLG_STAT));

        FAPI_TRY(spi_wait_for_tdr_empty(i_handle));

        FAPI_TRY(spi_wait_for_rdr_full(i_handle));  //Wait for response

        FAPI_TRY(getScom(i_handle.target_chip, i_handle.base_addr + SPIM_RDR, o_status));

        if(o_status.getBit<56>())
        {
            break; //Check for  RDY/BSY bit in the slave flag status register
        }

        FAPI_TRY(spi_wait_for_idle(i_handle));

        fapi2::delay(SPI_TIMEOUT_DELAY_NS, SPI_TIMEOUT_DELAY_NS_SIM_CYCLES);
        timeout--;
    }

    FAPI_ASSERT( timeout != 0,
                 fapi2::SBE_SPI_HANG_TIMEOUT()
                 .set_CHIP_TARGET(i_handle.target_chip)
                 .set_SPI_ENGINE(i_handle.engine)
                 .set_BASE_ADDRESS(i_handle.base_addr + SPIM_RDR)
                 .set_STATUS_REGISTER(o_status)
                 .set_TIMEOUT_MSEC(SPI_TIMEOUT_MAX_WAIT_COUNT),
                 "spi_wait_for_write_complete timeout");

    // Check for input NOR address do not fall in first 64K sector
    // into any of side-0/1 in NOR as the sector would be locked
    // Note: check for flag status bits definition in func spi_clear_flag_status
    // bit 58: erase                = 0:clear 1:failure or protection error
    // bit 59: program              = 0:clear 1:failure or protection error
    if ((o_status.getBit<58>() == 1) || (o_status.getBit<59>() == 1))
    {
        FAPI_INF("spi_wait_for_write_complete: Erase/Write operation unsuccessful"\
                 " address:[0x%08X] status:[0x%08X%08X]", i_address,
                 (uint64_t(o_status) >> 32), static_cast<uint32_t>(uint64_t(o_status) & 0xFFFFFFFF));
        FAPI_TRY(spi_chk_addr_n_lock_status_clr_flag_error_bits(i_handle, i_address, o_status));
    }

fapi_try_exit:
    return spi_restore_ecc_status(i_handle, fapi2::current_err, l_ecc);
}


///////////////////////////////////////////
/////// spi_restore_mmio_config ///////////
///////////////////////////////////////////
static fapi2::ReturnCode
spi_restore_mmio_config(SpiControlHandle& i_handle, fapi2::ReturnCode i_rc)
{
    // Restore the default counter and seq used by the side band path
    FAPI_TRY(putScom(i_handle.target_chip, i_handle.base_addr + SPIM_COUNTERREG, 0x0ULL));

    FAPI_TRY(putScom(i_handle.target_chip, i_handle.base_addr + SPIM_SEQREG, SPI_DEFAULT_SEQ));

fapi_try_exit:

    if (fapi2::current_err == fapi2::FAPI2_RC_SUCCESS)
    {
        // If no putscom error reported, send out the original error i_rc
        fapi2::current_err = i_rc;
    }

    return fapi2::current_err;
}


///////////////////////////////////////////
/////// spi_check_write_enable ////////////
///////////////////////////////////////////
// Poll SPI slave to see if a write has been enabled yet
static fapi2::ReturnCode
spi_check_write_enable(SpiControlHandle& i_handle)
{
    fapi2::buffer<uint64_t> data64 = 0;
    uint64_t SEQ = 0x1031411000000000ULL;
    SEQ |= ((uint64_t)((i_handle.slave)) << 56);
    bool l_ecc;

    uint64_t timeout = SPI_TIMEOUT_MAX_WAIT_COUNT;

    FAPI_DBG("Enter spi_check_write_enable...");

    FAPI_TRY(putScom(i_handle.target_chip, i_handle.base_addr + SPIM_SEQREG, SEQ));

    is_ecc_on(i_handle, l_ecc);

    if (l_ecc)
    {
        spi_set_ecc_off(i_handle);
    }

    while(timeout != 0)
    {
        //Send the read status register command
        FAPI_TRY(putScom(i_handle.target_chip,
                         i_handle.base_addr + SPIM_TDR, SPI_SLAVE_RD_STAT));

        FAPI_TRY(spi_wait_for_rdr_full(i_handle));

        FAPI_TRY(getScom(i_handle.target_chip, i_handle.base_addr + SPIM_RDR, data64));

        if(data64.getBit<62>())
        {
            break; //Check for write enable latch bit
        }

        FAPI_TRY(spi_wait_for_idle(i_handle));

        fapi2::delay(SPI_TIMEOUT_DELAY_NS, SPI_TIMEOUT_DELAY_NS_SIM_CYCLES);
        timeout--;
    }

    FAPI_ASSERT( timeout != 0,
                 fapi2::SBE_SPI_HANG_TIMEOUT()
                 .set_CHIP_TARGET(i_handle.target_chip)
                 .set_SPI_ENGINE(i_handle.engine)
                 .set_BASE_ADDRESS(i_handle.base_addr + SPIM_RDR)
                 .set_STATUS_REGISTER(data64)
                 .set_TIMEOUT_MSEC(SPI_TIMEOUT_MAX_WAIT_COUNT),
                 "spi_check_write_enable timeout");

fapi_try_exit:
    return spi_restore_ecc_status(i_handle, fapi2::current_err, l_ecc);
}


///////////////////////////////////////////
/////// spi_set_write_enable //////////////
///////////////////////////////////////////
// Enables SPI-slave write
static fapi2::ReturnCode
spi_set_write_enable(SpiControlHandle& i_handle)
{
    uint64_t SEQ = 0x1031100000000000ULL;
    SEQ |= (static_cast<uint64_t>((i_handle.slave)) << 56);
    uint64_t TDR = SPI_SLAVE_WR_EN;

    FAPI_DBG("Enter spi_set_write_enable...");

    FAPI_TRY(putScom(i_handle.target_chip, i_handle.base_addr + SPIM_SEQREG, SEQ));
    FAPI_TRY(putScom(i_handle.target_chip, i_handle.base_addr + SPIM_TDR, TDR));

    FAPI_TRY(spi_wait_for_tdr_empty(i_handle));

    FAPI_TRY(spi_wait_for_idle(i_handle));

    FAPI_TRY(spi_check_write_enable(i_handle));

fapi_try_exit:
    return fapi2::current_err;
}


///////////////////////////////////////////
////////////// spi_precheck ///////////////
///////////////////////////////////////////
//Check the h/w is in the expected state
fapi2::ReturnCode
spi_precheck(SpiControlHandle& i_handle)
{
    fapi2::buffer<uint64_t> status_reg = 0;
    uint64_t temp, flags;

    FAPI_TRY(getScom(i_handle.target_chip,
                     i_handle.base_addr + SPIM_STATUSREG, status_reg));
    status_reg.extract<0, 64>(temp);
    flags = SPI_RDR_FULL | SPI_RDR_OVERRUN |
            SPI_RDR_UNDERRUN | SPI_TDR_FULL |
            SPI_TDR_OVERRUN /*| SPI_TDR_UNDERRUN*/;

    // Check the h/w is in the expected state
    FAPI_ASSERT( !(temp & flags),
                 fapi2::SBE_SPI_CMD_STATUS_REG_UNSUPPORTED_STATE()
                 .set_CHIP_TARGET(i_handle.target_chip)
                 .set_SPI_ENGINE(i_handle.engine)
                 .set_BASE_ADDRESS(i_handle.base_addr + SPIM_STATUSREG)
                 .set_STATUS_REGISTER(temp)
                 .set_CHECK_FLAGS(flags),
                 "SPI status register state bits check validation failed.");

fapi_try_exit:
    return fapi2::current_err;
}


///////////////////////////////////////////
///// spi_wait_for_seq_index_pass /////////
///////////////////////////////////////////
static fapi2::ReturnCode
spi_wait_for_seq_index_pass(SpiControlHandle& i_handle, const uint32_t i_index)
{
    fapi2::buffer<uint64_t> status_reg = 0;

    uint64_t timeout = SPI_TIMEOUT_MAX_WAIT_COUNT;

    while(timeout--)
    {
        FAPI_TRY(getScom( i_handle.target_chip,
                          i_handle.base_addr + SPIM_STATUSREG, status_reg));

        //checking for multiplexing error
        FAPI_ASSERT( (status_reg.getBit<50>() == 0),
                     fapi2::SBE_SPI_INVALID_PORT_MULTIPLEX_SET()
                     .set_CHIP_TARGET(i_handle.target_chip)
                     .set_SPI_ENGINE(i_handle.engine)
                     .set_BASE_ADDRESS(i_handle.base_addr + SPIM_STATUSREG)
                     .set_STATUS_REGISTER(status_reg),
                     "Port multiplexer setting error set in wait_for_seq_index_pass");

        // 28 to 31 Sequencer index - Sequence index currently being executed
        status_reg = (status_reg >> 32) & 0x000000000000000FULL;

        if(status_reg > i_index)
        {
            break;
        }

        fapi2::delay(SPI_TIMEOUT_DELAY_NS, SPI_TIMEOUT_DELAY_NS_SIM_CYCLES);
    }

    FAPI_DBG("wait_for_seq_index_pass(%d) timeout %lld msec", i_index, timeout);
    FAPI_ASSERT( timeout != 0,
                 fapi2::SBE_SPI_HANG_TIMEOUT()
                 .set_CHIP_TARGET(i_handle.target_chip)
                 .set_SPI_ENGINE(i_handle.engine)
                 .set_BASE_ADDRESS(i_handle.base_addr + SPIM_STATUSREG)
                 .set_STATUS_REGISTER(status_reg)
                 .set_TIMEOUT_MSEC(SPI_TIMEOUT_MAX_WAIT_COUNT),
                 "wait_for_seq_index_pass(%d) wait timeout", i_index);

fapi_try_exit:
    return fapi2::current_err;
}


///////////////////////////////////////////
///// spi_tpm_write_with_wait_internal ////
///////////////////////////////////////////
fapi2::ReturnCode spi_tpm_write_with_wait_internal( SpiControlHandle& i_handle,
        const uint32_t i_locality,
        const uint32_t i_address,
        const uint8_t i_length,
        const uint8_t* i_buffer )
{
    uint64_t SEQ = 0;
    uint64_t CNT = 0;
    fapi2::buffer<uint64_t> data64 = 0;

    // Trusted Computing Group (TCG) standard requires
    // 3-byte addressing for SPI TPM operations
    // Change into TPM address on SPI ( D4_[locality]xxxh )
    uint32_t l_address = 0x00D40000 + (i_locality << 12) + (i_address & 0x0FFF);
    fapi2::ReturnCode rc = fapi2::FAPI2_RC_SUCCESS;
    tpmSpiCmd_t startWriteCmd = {0};

    // Looking for last byte being a 0x01 which indicates end of wait period
    uint64_t memory_mapping_reg = 0x00000000FF01FF00;

    if(i_length <= 8)
    {
        // Sequencer Basic Operations
        //    0x1X = Select_Slave X -Select slave X (X = handle.slave)
        //        34 = Shift_N1 - M = 4 bytes of data being sent in TDR
        //          41 = Shift_N2 - M = 1 byte to receive
        //            62 = Branch if Not Equal RDR   -- use memory_mapping_reg
        //              3M = Shift_N1 - M = i_length bytes of data to send
        //                10 = select_slave 0 - deselect any slave
        //                  00 = STOP
        SEQ = 0x1034416230100000ULL | (static_cast<uint64_t>(i_length) << 24) |
              (static_cast<uint64_t>(i_handle.slave) << 56);
        CNT = 0x0;
    }
    else if((i_length % 8) == 0)
    {
        // NON-SECURE OPERATION - 0xEx cmd op not allowed
        // Sequencer Basic Operations
        //    0x1X = Select_Slave X -Select slave X (X = handle.slave)
        //        34 = Shift_N1 - M = 4 bytes of data being sent in TDR
        //          41 = Shift_N2 - M = 1 byte to receive
        //            62 = Branch if Not Equal RDR   -- use memory_mapping_reg
        //              38 = Shift_N1 - M = 8 bytes of data to sendg
        //                E4 = Branch to index 4 if not Equal and increment loop counter
        //                  10 = select_slave 0 - deselect any slave
        //                    00 = STOP
        SEQ = 0x1034416238E41000ULL | (static_cast<uint64_t>(i_handle.slave) << 56);
        CNT = (static_cast<uint64_t>((i_length / 8) - 1) << 32) | ((0x65) << 8);
    }
    else
    {
        // NON-SECURE OPERATION - 0xEx cmd op not allowed
        // Sequencer Basic Operations
        //    0x1X = Select_Slave X -Select slave X (X = handle.slave)
        //        34 = Shift_N1 - M = 4 bytes of data being sent in TDR
        //          41 = Shift_N2 - M = 1 byte to receive
        //            62 = Branch if Not Equal RDR   -- use memory_mapping_reg
        //              38 = Shift_N1 - M = 8 bytes of data to send
        //                E4 = Branch to index 4 if not Equal and increment loop counter
        //                  3M = Shift_N1 - M = i_length%8 of remaining data sent in TDR
        //                    10 = select_slave 0 - deselect any slave
        SEQ = 0x1034416238E43010ULL | (static_cast<uint64_t>((i_length) % 8) << 8) |
              (static_cast<uint64_t>(i_handle.slave) << 56);
        CNT = (static_cast<uint64_t>((i_length / 8) - 1) << 32) | ((0x65) << 8);
    }

    // Initial TDR command
    startWriteCmd.cmd_bits.readNotWrite = 0;
    startWriteCmd.cmd_bits.len = ((i_length - 1) & 0x3F);
    startWriteCmd.cmd_bits.addr = l_address;


    FAPI_DBG("Address: 0x%08X, SEQ: 0x%016X", i_handle.base_addr + SPIM_SEQREG, SEQ);
    FAPI_TRY(putScom(i_handle.target_chip, i_handle.base_addr + SPIM_SEQREG, SEQ));
    FAPI_DBG("Address: 0x%08X, MEMORY_MAPPING_REG: 0x%016X", i_handle.base_addr + SPIM_MMSPISMREG, memory_mapping_reg);
    FAPI_TRY(putScom(i_handle.target_chip, i_handle.base_addr + SPIM_MMSPISMREG, memory_mapping_reg));
    FAPI_DBG("Address: 0x%08X, CNT: 0x%016X", i_handle.base_addr + SPIM_COUNTERREG, CNT);
    FAPI_TRY(putScom(i_handle.target_chip, i_handle.base_addr + SPIM_COUNTERREG, CNT));
    FAPI_DBG("Address: 0x%08X, TDR: 0x%016X", i_handle.base_addr + SPIM_TDR, startWriteCmd.val);
    FAPI_TRY(putScom(i_handle.target_chip, i_handle.base_addr + SPIM_TDR, startWriteCmd.val));


    // Wait until sequence index 3 has executed (TPM finished
    // inserting wait states)
    rc = spi_wait_for_seq_index_pass(i_handle, 3);

    if (rc)
    {
        FAPI_ERR("Error in wait_for_seq_index_pass, rc: 0x%08X", static_cast<uint32_t>(rc));
        fapi2::current_err = rc;
        goto fapi_try_exit;
    }

    data64 = 0;

    // break data up into 8-byte sections
    for(int i = 0; i < i_length; i++)
    {
        data64 = (data64 << 8) | ((uint8_t)i_buffer[i]);

        if( ((i % 8) == 7) || (i == (i_length - 1)) )
        {
            // left-justify data if not full 8-bytes added
            if ( (i_length < 8) ||
                 ((i == (i_length - 1)) && ((i_length % 8) != 0)) )
            {
                data64 = data64 << (8 * (8 - i_length % 8));
            }

            rc = spi_wait_for_tdr_empty(i_handle);

            if (rc)
            {
                FAPI_ERR("Error in spi_wait_for_tdr_empty ");
                fapi2::current_err = rc;
                goto fapi_try_exit;
            }

            FAPI_DBG("tpm_write() TDR: 0x%016X", data64);
            FAPI_TRY(putScom(i_handle.target_chip, i_handle.base_addr + SPIM_TDR, data64));
        }
    }

    rc = spi_wait_for_idle(i_handle);

    if (rc)
    {
        FAPI_ERR("Multiplexing error or timeout from spi_wait_for_idle ");
        fapi2::current_err = rc;
        goto fapi_try_exit;
    }

fapi_try_exit:
    FAPI_DBG("spi_tpm_write_with_wait_internal() exit. RC: 0x%08X", static_cast<uint32_t>(rc));
    return rc;
}

///////////////////////////////////////////
/////// spi_tpm_write_with_wait ///////////
///////////////////////////////////////////
// Force the secure mode path by breaking up data writes of length greater than 8-bytes into
// 8-byte chunks or less
fapi2::ReturnCode spi_tpm_write_with_wait( SpiControlHandle& i_handle,
        const uint32_t i_locality,
        const uint32_t i_address,
        const uint8_t i_length,
        const uint8_t* i_buffer )
{
    fapi2::ReturnCode rc = fapi2::FAPI2_RC_SUCCESS;

    // We're only allowing secure TPM writes, which are of length TPM_SECURE_DATA_LEN or less.
    // l_lengthTraversed will increase from 0 up to i_length in TPM_SECURE_DATA_LEN intervals or less.
    // l_lengthTraversed will be used to know from what offset in the buffer we're writing to the TPM.

    uint8_t l_lengthTraversed = 0;

    while (l_lengthTraversed < i_length)
    {
        uint8_t l_minTravelLength = std::min(TPM_SECURE_DATA_LEN, (i_length - l_lengthTraversed));

        // TODO RTC: 268091 Current SPI issue when writing 3 bytes. Must be broken down into smaller write ops.
        if (l_minTravelLength == 3)
        {
            l_minTravelLength = 2;
        }

        rc = spi_tpm_write_with_wait_internal(i_handle, i_locality, i_address, l_minTravelLength,
                                              i_buffer + l_lengthTraversed);

        if (rc)
        {
            break;
        }

        l_lengthTraversed += l_minTravelLength;
    }

    if (rc)
    {
        FAPI_ERR("spi_tpm_write_with_wait: Error doing secure write.");
        fapi2::current_err = rc;
        goto fapi_try_exit;
    }

fapi_try_exit:
    FAPI_DBG("spi_tpm_write_with_wait() exit. RC: 0x%08X", static_cast<uint32_t>(rc));
    return rc;
}

///////////////////////////////////////////
///// spi_tpm_read_internal_with_wait /////
///////////////////////////////////////////
// Secure TPM reads will call this with i_length <= 8
// Non-secure TPM reads can call this with i_length > 8
fapi2::ReturnCode spi_tpm_read_internal_with_wait( SpiControlHandle& i_handle,
        const uint32_t i_address,
        uint8_t i_length,
        uint8_t* o_buffer )
{
    uint64_t SEQ;
    uint64_t CNT;
    fapi2::buffer<uint64_t> data64 = 0;
    uint64_t temp = 0;
    fapi2::ReturnCode rc = fapi2::FAPI2_RC_SUCCESS;
    tpmSpiCmd_t startReadCmd = {0};

    // Looking for last byte being a 0x01 which indicates end of wait period
    uint64_t memory_mapping_reg = 0x00000000FF01FF00;

    if(i_length <= 8)
    {
        // Sequencer Basic Operations
        //    0x1X = Select_Slave X -Select slave X (X = handle.slave)
        //        34 = Shift_N1 - M = 4 bytes of data being sent in TDR
        //          41 = Shift_N2 - M = 1 byte to receive
        //            62 = Branch if Not Equal RDR   -- use memory_mapping_reg
        //              4X = Shift_N2 - X bytes to receive (X=length)
        //                10 = select_slave 0 - deselect any slave
        //                  00 = STOP
        // Shift out 4 bytes of TDR (the read/write bit, the size, the address),
        // then switch to reading and if the next bytes is not 1, then go to
        // sequence opcode (2), and keep doing that until the data is 0x1.
        // (that covers the flow control), then start reading the actual # of
        // bytes the TPM is trying to send back
        SEQ = 0x1034416240100000ULL | (static_cast<uint64_t>(i_length) << 24) |
              (static_cast<uint64_t>(i_handle.slave) << 56);
        CNT = 0;
    }
    else if((i_length % 8) == 0)
    {
        // NON-SECURE OPERATION - 0xEx cmd op not allowed
        // Sequencer Basic Operations
        //    0x1X = Select_Slave X -Select slave X (X = handle.slave)
        //        34 = Shift_N1 - M = 4 bytes of data being sent in TDR
        //          41 = Shift_N2 - M = 1 byte to receive
        //            62 = Branch if Not Equal RDR   -- use memory_mapping_reg
        //              48 = Shift_N2 - 8 bytes to receive
        //                E4 = Branch if Not Equal and Increment - SEQ opcode (4)
        //                  10 = select_slave 0 - deselect any slave
        //                    00 = STOP
        SEQ = 0x1034416248E41000ULL | (static_cast<uint64_t>(i_handle.slave) << 56);
        CNT = (static_cast<uint64_t>((i_length / 8) - 1) << 32) | ((0x6F) << 8);
    }
    else
    {
        // NON-SECURE OPERATION - 0xEx cmd op not allowed
        // Sequencer Basic Operations
        //    0x1X = Select_Slave X -Select slave X (X = handle.slave)
        //        34 = Shift_N1 - M = 4 bytes of data being sent in TDR
        //          41 = Shift_N2 - M = 1 byte to receive
        //            62 = Branch if Not Equal RDR   -- use memory_mapping_reg
        //              48 = Shift_N2 - 8 bytes to receive
        //                E4 = Branch if Not Equal and Increment - SEQ opcode (4)
        //                  40 = Shift_N2 - 0 bytes to receive
        //                    10 = select_slave 0 - deselect any slave
        //                      00 = STOP
        SEQ = 0x1034416248E44010ULL | (static_cast<uint64_t>((i_length) % 8) << 8)
              | (static_cast<uint64_t>(i_handle.slave) << 56);
        CNT = (static_cast<uint64_t>((i_length / 8) - 1) << 32) | ((0x6F) << 8);
    }

    // Initial TDR command
    startReadCmd.cmd_bits.readNotWrite = 1;
    startReadCmd.cmd_bits.len = ((i_length - 1) & 0x3F);
    startReadCmd.cmd_bits.addr = i_address;

    FAPI_DBG("Address: 0x%08X, SEQ: 0x%016X", i_handle.base_addr + SPIM_SEQREG, SEQ);
    FAPI_TRY(putScom(i_handle.target_chip, i_handle.base_addr + SPIM_SEQREG, SEQ));
    FAPI_DBG("Address: 0x%08X, MMSPIMREG: 0x%016X", i_handle.base_addr + SPIM_MMSPISMREG, memory_mapping_reg);
    FAPI_TRY(putScom(i_handle.target_chip, i_handle.base_addr + SPIM_MMSPISMREG, memory_mapping_reg));
    FAPI_DBG("Address: 0x%08X, COUNTERREG: 0x%016X", i_handle.base_addr + SPIM_COUNTERREG, CNT);
    FAPI_TRY(putScom(i_handle.target_chip, i_handle.base_addr + SPIM_COUNTERREG, CNT));
    FAPI_DBG("Address: 0x%08X, TDR: 0x%016X", i_handle.base_addr + SPIM_TDR, startReadCmd.val);
    FAPI_TRY(putScom(i_handle.target_chip, i_handle.base_addr + SPIM_TDR, startReadCmd.val));
    FAPI_DBG("spi_wait_for_tdr_empty()");
    rc = spi_wait_for_tdr_empty(i_handle);

    if (rc)
    {
        FAPI_ERR("Error in spi_wait_for_tdr_empty");
        fapi2::current_err = rc;
        goto fapi_try_exit;
    }

    if (i_length > 8)
    {
        FAPI_DBG("spi_wait_for_tdr_empty() done, TDR 0");
        FAPI_TRY(putScom(i_handle.target_chip, i_handle.base_addr + SPIM_TDR, 0x0ULL));
        FAPI_DBG("TDR 0 done");
    }

    // Wait until sequence index 3 has executed (TPM finished
    // inserting wait states)
    rc = spi_wait_for_seq_index_pass(i_handle, 3);

    if (rc)
    {
        FAPI_ERR("Error in wait_for_seq_index_pass, rc: 0x%08X",
                 static_cast<uint32_t>(rc));
        fapi2::current_err = rc;
        goto fapi_try_exit;
    }

    //receive data
    if(i_length <= 8)
    {
        rc = spi_wait_for_rdr_full(i_handle);  //Wait for response

        if (rc)
        {
            FAPI_ERR("Error in spi_wait_for_rdr_full");
            fapi2::current_err = rc;
            goto fapi_try_exit;
        }

        FAPI_TRY(getScom(i_handle.target_chip, i_handle.base_addr + SPIM_RDR, data64));
        data64.extract<0, 64>(temp);

        FAPI_DBG("spi_wait_for_rdr_full finished, data read from rdr: 0x%016X", temp);

        // The value read from this RDR register is right-aligned.
        // Only copy the requested bytes of data
        temp = temp << ((8 - i_length) * 8);
#ifndef __PPE__
        memcpy(o_buffer, &temp, i_length);
#else
        uint8_t* tempPtr = (uint8_t*)&temp;

        for(uint32_t i = 0; i < i_length; i++)
        {
            *(o_buffer + i) = *(tempPtr + i);
            FAPI_DBG("Output buffer is 0x%02X", *(o_buffer + i));
        }

#endif
    }
    else
    {

        for (uint32_t i = 0; i < static_cast<uint32_t>(i_length - 7); i += 8)
        {
            rc = spi_wait_for_rdr_full(i_handle);  //Wait for response

            if (rc)
            {
                FAPI_ERR("Error in spi_wait_for_rdr_full");
                fapi2::current_err = rc;
                goto fapi_try_exit;
            }

            FAPI_TRY(getScom(i_handle.target_chip, i_handle.base_addr + SPIM_RDR, data64));
            data64.extract<0, 64>(temp);
            FAPI_DBG("%d) spi_wait_for_rdr_full finished, data read from rdr: 0x%016X", i, temp);

            // add the full 8 bytes to buffer
            reinterpret_cast<uint64_t*>(o_buffer)[i / 8] = temp;
        }

        if ((i_length % 8 != 0) && (i_length > 8))
        {
            rc = spi_wait_for_rdr_full(i_handle);  //Wait for response

            if (rc)
            {
                FAPI_ERR("Error in spi_wait_for_rdr_full");
                fapi2::current_err = rc;
                goto fapi_try_exit;
            }

            FAPI_TRY(getScom(i_handle.target_chip, i_handle.base_addr + SPIM_RDR, data64));
            data64.extract<0, 64>(temp);
            FAPI_DBG("Read RDR data: 0x%016X", temp);

            // The value read from this RDR register is right-aligned.
            // Only copy the remaining requested bytes of data
            temp = temp << ((8 - (i_length % 8)) * 8);
            FAPI_DBG("Copy %d bytes of right-aligned shifted RDR: 0x%016X",
                     i_length % 8, temp);
#ifndef __PPE__
            memcpy(&o_buffer[i_length - (i_length % 8)], &temp, i_length % 8);
#else
            uint8_t* tempPtr = (uint8_t*)&temp;

            for(uint32_t i = 0; i < (i_length % 8); i++)
            {
                *(o_buffer + i_length - (i_length % 8) + i) = *(tempPtr + i);
                FAPI_DBG("Output buffer is 0x%02X", *(o_buffer + i));
            }

#endif
        }
    }

    FAPI_DBG("spi_tpm_read_with_wait: spi_wait_for_idle");
    rc = spi_wait_for_idle(i_handle);
    FAPI_DBG("spi_tpm_read_with_wait: spi_wait_for_idle done");

    if (rc)
    {
        FAPI_ERR("Multiplexing error in spi_wait_for_idle");
        fapi2::current_err = rc;
        goto fapi_try_exit;
    }

fapi_try_exit:
    FAPI_DBG("spi_tpm_read_with_wait() exit. RC: 0x%02X", static_cast<int>(rc));
    return rc;
}


///////////////////////////////////////////
/////////// spi_tpm_read_secure ///////////
///////////////////////////////////////////
fapi2::ReturnCode spi_tpm_read_secure( SpiControlHandle& i_handle,
                                       const uint32_t i_locality,
                                       const uint32_t i_address,
                                       const uint8_t i_length,
                                       uint8_t* o_buffer )
{
    fapi2::ReturnCode rc = fapi2::FAPI2_RC_SUCCESS;
    uint8_t readlen = i_length;  // try to read full length if possible

    // Trusted Computing Group (TCG) standard requires
    // 3-byte addressing for SPI TPM operations
    // Change into TPM address on SPI ( D4_[locality]xxxh )
    uint32_t l_address = 0x00D40000 + (i_locality << 12) + (i_address & 0x0FFF);

    // 0xEX Op code is not allowed in secure mode so need to split
    // read into multiple transactions of TPM_SECURE_DATA_LEN max size
    do
    {
        if (i_length <= TPM_SECURE_DATA_LEN)
        {
            rc = spi_tpm_read_internal_with_wait( i_handle, l_address, readlen, o_buffer );
            break;
        }

        for(uint8_t i = 0; i < i_length; i += TPM_SECURE_DATA_LEN)
        {
            readlen = (i_length - i) < TPM_SECURE_DATA_LEN ?
                      (i_length - i) : TPM_SECURE_DATA_LEN;

            if (readlen == 0)
            {
                break;
            }

            rc = spi_tpm_read_internal_with_wait( i_handle, l_address, readlen, o_buffer );

            if (rc != fapi2::FAPI2_RC_SUCCESS)
            {
                FAPI_ERR( "spi_tpm_read_secure: "
                          "Failed address 0x%04X read at %d bytes out of %d total",
                          l_address, i, i_length);
                break;
            }

            o_buffer += readlen;
        }
    }
    while(0);

    return rc;
}


///////////////////////////////////////////
//////// spi_read_manufacturer_id /////////
///////////////////////////////////////////
fapi2::ReturnCode spi_read_manufacturer_id(SpiControlHandle& i_handle, uint8_t* o_buffer)
{
    fapi2::buffer<uint64_t> data64;
    uint32_t length = 32;
    // The sequence that will write the op code and then read five bytes of manufacturer id.
    uint64_t SEQ = 0x1231484848481000ULL | ((uint64_t)((i_handle.slave)) << 56);
    // Ensure the Counter Config Reg is cleared as this function doesn't require loop logic and default settings
    // will be sufficient enough.
    uint64_t CNT = 0x0ULL;
    // The value to write into the TDR, this is just the op code that requests the manufacturer id
    // from the slave device.
    uint64_t TDR = SPI_SLAVE_ID_CMD;

    fapi2::ReturnCode rc = fapi2::FAPI2_RC_SUCCESS;

    //TODO:Do check for ecc?? zA is not performing any check
    // If ECC is on then it will need to be off to avoid errors.
    bool l_ecc = false;
    is_ecc_on(i_handle, l_ecc);

    if (l_ecc)
    {
        spi_set_ecc_off(i_handle);
    }

    // Check the state of the h/w
    rc = spi_precheck(i_handle);

    if (rc != fapi2::FAPI2_RC_SUCCESS)
    {
        return rc;
    }

    // Set the sequence
    FAPI_TRY(putScom(i_handle.target_chip, i_handle.base_addr + SPIM_SEQREG, SEQ));

    // Write the op code to the TDR, this executes the sequence.
    FAPI_TRY(putScom(i_handle.target_chip, i_handle.base_addr + SPIM_TDR, TDR));

    //one time zeros to trigger read
    rc = spi_wait_for_tdr_empty(i_handle);

    if (rc)
    {
        FAPI_ERR("Error in spi_wait_for_tdr_empty ");
        fapi2::current_err = rc;
        goto fapi_try_exit;
    }

    //TODO: Is this SPIM_TDR correct or it should be SPIM_COUNTERREG??
    // Clear the Counter Config Reg
    FAPI_TRY(putScom(i_handle.target_chip, i_handle.base_addr + SPIM_TDR, CNT));

    for (uint32_t i = 0; i < length; i += 8)
    {
        // Wait for the RDR to be full
        rc = spi_wait_for_rdr_full(i_handle);

        if (rc)
        {
            FAPI_ERR("Error in spi_wait_for_rdr_full");
            fapi2::current_err = rc;
            goto fapi_try_exit;
        }

        // Read the manufacturer id from the RDR.
        FAPI_TRY(getScom(i_handle.target_chip, i_handle.base_addr + SPIM_RDR, data64));
        FAPI_INF("Read word 0x%08X%08X", (uint64_t(data64) >> 32), static_cast<uint32_t>(uint64_t(data64) & 0xFFFFFFFF));

        for (uint32_t j = 0; j < 8 && j < (length - i); j++)
        {
            if (length < 8)
            {
                data64.extractToRight(o_buffer[i + j],  (j + (8 - length))  * 8, 8);
            }
            else
            {
                data64.extractToRight(o_buffer[i + j],  j  * 8, 8);
            }
        }
    }

    rc = spi_wait_for_idle(i_handle);

    if (rc)
    {
        FAPI_ERR("Multiplexing Error in spi_wait_for_idle ");
        fapi2::current_err = rc;
        goto fapi_try_exit;
    }

fapi_try_exit:
    return spi_restore_ecc_status(i_handle, fapi2::current_err, l_ecc);
}


/**
 * @brief This function is used to read data from the device which use SPI
 *        as a interface and store into the given buffer.
 *
 * @description
 *
 * - This API expects the read length should be equal or greater than
 *   the MIN_READ_LENGTH.
 * - This API expects the read length should be equal or less than
 *   the MAX_READ_LENGTH.
 * - This API supports to read max MAX_READ_LENGTH per attempt by using
 *   the counter.
 * - The length and address should be physical (with adding ECC bytes)
 *   if the SPI_ECC_CONTROL_STATUS is the RAW_BYTE_ACCESS, and
 *   length and address should be logical (without adding ECC bytes) otherwise.
 *
 * Steps to read from device (aka, SEEPROM or NOR) which using interface as SPI.
 * 1. Send the READ command along with the address.
 * 2. Configure count control
 * 3. The device will start sending the data from that offset
 *    continuously.
 * 4. Read the data. Use the loop mode if more than 8 bytes reads
 * 5. Deselect the slave to stop it from sending any more data
 *
 * @param[in] i_handle Used to pass SPI handler to get the SPI port and other
 *                     necessary inputs.
 * @param[in] i_address Used to pass the address to say from where needs to
 *                      read.
 * @param[in] i_length Used to pass the length to read required data from
 *                     the device.
 * @param[out] o_buffer Used to pass the buffer address to store the read data.
 * @param[in] i_eccStatus Used to pass the ecc mode to read back from the device.
 *
 * @return FAPIRC if any failure otherwise success FAPIRC.
 */
static fapi2::ReturnCode
spi_read_secure(SpiControlHandle& i_handle, uint32_t i_address, uint32_t i_length,
                uint8_t* o_buffer, SPI_ECC_CONTROL_STATUS i_eccStatus)
{
    fapi2::buffer<uint64_t> data64;
    uint64_t SEQ;
    uint64_t CNT = 0;
    uint64_t TDR;
    bool l_ecc = false;

    FAPI_ASSERT(((i_length <= MAX_READ_LENGTH) && (i_length >= MIN_READ_LENGTH)),
                fapi2::SBE_SPI_INVALID_LENGTH()
                .set_CHIP_TARGET(i_handle.target_chip)
                .set_SPI_ENGINE(i_handle.engine)
                .set_LENGTH(i_length)
                .set_MIN_LENGTH(MIN_READ_LENGTH)
                .set_MAX_LENGTH(MAX_READ_LENGTH)
                .set_ECC_STATUS(i_eccStatus)
                .set_ADDRESS(i_address),
                "SPI: The given length to read is not in the required range.");

    // Check the state of the h/w
    FAPI_TRY(spi_precheck(i_handle),
             "spi_precheck failed before executing read operation");

    // Check whether the ecc is on or not
    FAPI_TRY(is_ecc_on(i_handle, l_ecc),
             "is_ecc_on failed before executing read operation");

    if ((i_eccStatus == RAW_BYTE_ACCESS) && (l_ecc))
    {
        spi_set_ecc_off(i_handle);
    }

    if((i_eccStatus == STANDARD_ECC_ACCESS) ||
       (i_eccStatus == DISCARD_ECC_ACCESS)   )
    {
        FAPI_ASSERT(((i_length % 8) == 0),
                    fapi2::SBE_SPI_LENGTH_IS_NOT_ALIGNED()
                    .set_CHIP_TARGET(i_handle.target_chip)
                    .set_SPI_ENGINE(i_handle.engine)
                    .set_LENGTH(i_length)
                    .set_ECC_STATUS(i_eccStatus),
                    "SPI: The given length to read is not aligned by 8 bytes");

        FAPI_ASSERT(((i_address % 8) == 0),
                    fapi2::SBE_SPI_ADDRESS_IS_NOT_ALIGNED()
                    .set_CHIP_TARGET(i_handle.target_chip)
                    .set_SPI_ENGINE(i_handle.engine)
                    .set_ADDRESS(i_length)
                    .set_ECC_STATUS(i_eccStatus),
                    "SPI: The given address to read is not aligned by 8 bytes");

        if (i_length == 8)
        {
            // select slave0, TX4, RX9, deselect slave0
            SEQ = 0x1034491000000000ULL | (static_cast<uint64_t>(i_handle.slave) << 56);
            CNT = 0x0;
        }
        else
        {
            // select slave0, TX4, RX9, Loop, deselect slave0
            SEQ = 0x103449E210000000ULL | (static_cast<uint64_t>(i_handle.slave) << 56);
            // LoopCount (No of the loop iteration to run the specified index
            // op-code in the sequence command thats is "xxxx49E2xxxxxxxx",
            // index start with 0 from the MSB in the sequence command)
            CNT = ((uint64_t)((i_length / 8) - 1) << 32);
            // N2_control (0xf - 1111, bit 0: counter N2 reload,
            // bit 1: force N2 count implicit, bit 2: use N2 counter for data
            // transmit, bit 3: use N2 counter for data receive)
            CNT |= ((uint64_t)(0xf) << 8);
        }

        // Need to covert logical to physical address for ecc mode
        i_address = i_address * 9 / 8;
    }
    else
    {
        if (i_length < 8)
        {
            // select slave0, TX4, RX(i_length), deselect slave0
            SEQ = 0x1034401000000000ULL | (static_cast<uint64_t>(i_handle.slave) << 56);
            SEQ |= (static_cast<uint64_t>(i_length) << 40);
            CNT = 0x0;
        }
        else if (i_length == 8)
        {
            // select slave0, TX4, RX8, deselect slave0
            SEQ = 0x1034481000000000ULL | (static_cast<uint64_t>(i_handle.slave) << 56);
            CNT = 0x0;
        }
        else if (i_length % 8)
        {
            // select slave0, TX4, RX8, Loop, RX(i_length % 8), deselect slave0
            SEQ = 0x103448E240100000ULL | (static_cast<uint64_t>(i_handle.slave) << 56);
            SEQ |= ((uint64_t)(i_length % 8) << 24);
            // LoopCount (No of the loop iteration to run the specified index
            // op-code in the sequence command thats is "xxxx48E2xxxxxxxx",
            // index start with 0 from the MSB in the sequence command)
            CNT = ((uint64_t)((i_length / 8) - 1) << 32);
            // N2_control (0xf - 1111, bit 0: counter N2 reload,
            // bit 1: force N2 count implicit, bit 2: use N2 counter for data
            // transmit, bit 3: use N2 counter for data receive)
            CNT |= ((uint64_t)(0xf) << 8);
        }
        else
        {
            // select slave0, TX4, RX8, Loop, deselect slave0
            SEQ = 0x103448E210000000ULL | (static_cast<uint64_t>(i_handle.slave) << 56);
            // LoopCount (No of the loop iteration to run the specified index
            // op-code in the sequence command thats is "xxxx48E2xxxxxxxx",
            // index start with 0 from the MSB in the sequence command)
            CNT = ((uint64_t)((i_length / 8) - 1) << 32) | ((uint64_t)(0xf) << 8);
        }
    }

    // Read command (0x3)|| address in TDR to be sent to the slave
    TDR = SPI_SLAVE_RD_CMD | ((uint64_t)i_address << 32);
    FAPI_DBG("spi_read_secure: SEQ = [0x%08X%08X]", SEQ >> 32, SEQ & 0xFFFFFFFF);
    FAPI_DBG("spi_read_secure: CNT = [0x%08X%08X]", CNT >> 32, CNT & 0xFFFFFFFF);
    FAPI_DBG("spi_read_secure: TDR = [0x%08X%08X]", TDR >> 32, TDR & 0xFFFFFFFF);

    FAPI_TRY(putScom(i_handle.target_chip, i_handle.base_addr + SPIM_SEQREG, SEQ));
    FAPI_TRY(putScom(i_handle.target_chip, i_handle.base_addr + SPIM_COUNTERREG, CNT));
    FAPI_TRY(putScom(i_handle.target_chip, i_handle.base_addr + SPIM_TDR, TDR));

    if (i_length > 8)
    {
        //one time zeros to trigger read
        FAPI_TRY(spi_wait_for_tdr_empty(i_handle));

        FAPI_TRY(putScom(i_handle.target_chip, i_handle.base_addr + SPIM_TDR, 0x0ULL));
    }

    // Copying data from the RDR buffer into the given output buffer.
    for (uint32_t i = 0; i < i_length; i += 8)
    {
        FAPI_TRY(spi_wait_for_rdr_full(i_handle));

        FAPI_TRY(getScom(i_handle.target_chip, i_handle.base_addr + SPIM_RDR, data64));
        FAPI_DBG("spi_read_secure: data64 = [0x%08X%08X]", data64 >> 32, data64 & 0xFFFFFFFF);

        if ( (i_length % 8) && (i_length < (i + 8)))
        {
            // If the length is unaligned then, the last of chunk data will be
            // less than 8 bytes so we need to do the left shift to get the
            // correct data else we will get previous chunk data from
            // the 0th index.
            ((uint64_t*)o_buffer)[i / 8] = data64 << ((8 - i_length % 8) * 8);
        }
        else
        {
            ((uint64_t*)o_buffer)[i / 8] = data64();
        }
    }

    FAPI_TRY(spi_wait_for_idle(i_handle));

    FAPI_DBG("spi_read_secure: ((uint32_t*)o_buffer)[0] = [0x%08X]", ((uint32_t*)o_buffer)[0]);

    // Restore the default counter and seq used by the side band path
    FAPI_TRY(putScom(i_handle.target_chip, i_handle.base_addr + SPIM_COUNTERREG, 0x0ULL));
    FAPI_TRY(putScom(i_handle.target_chip,
                     i_handle.base_addr + SPIM_SEQREG, SPI_DEFAULT_SEQ));

fapi_try_exit:
    return spi_restore_ecc_status(i_handle, fapi2::current_err, l_ecc);
}


///////////////////////////////////////////
////////////// spi_read ///////////////////
///////////////////////////////////////////
fapi2::ReturnCode
spi_read( SpiControlHandle& i_handle, uint32_t i_address, uint32_t i_length,
          SPI_ECC_CONTROL_STATUS i_eccStatus, uint8_t* o_buffer )
{
    FAPI_DBG("spi_read: Entering...");

    do
    {
        for(uint32_t i = 0; i < i_length; i += MAX_READ_LENGTH)
        {
            uint32_t readlen = (i_length - i) < MAX_READ_LENGTH ?
                               (i_length - i) : MAX_READ_LENGTH;

            if (readlen == 0)
            {
                break;
            }

            FAPI_TRY(spi_read_secure(i_handle, i_address, readlen,
                                     o_buffer, i_eccStatus),
                     "SPI: Failed to read, length[%d] address[0x%08X] "
                     "eccStatus[%d]", readlen, i_address, i_eccStatus);

            i_address += readlen;
            o_buffer = (uint8_t*)(reinterpret_cast<uint64_t>(o_buffer) + readlen);
        }
    }
    while(0);

fapi_try_exit:
    FAPI_DBG("spi_read: Exiting...");
    return fapi2::current_err;
}


///////////////////////////////////////////
/////////// spi_write_prep_seq ////////////
///////////////////////////////////////////
fapi2::ReturnCode
spi_write_prep_seq(SpiControlHandle& i_handle, uint64_t i_address, uint32_t i_length)
{
    uint64_t SEQ;
    uint64_t CNT;
    uint64_t TDR = SPI_SLAVE_WR_CMD | (((static_cast<uint64_t>(i_address) << 32) & 0x00ffffffffffffffULL));

    if (i_length < 8)
    {
        SEQ = 0x1034301000000000ULL | ((uint64_t)(i_length % 8) << 40);
        CNT = 0;
    }
    else if (i_length == 8)
    {
        SEQ = 0x1034381000000000ULL;
        CNT = 0;
    }
    else if (i_length % 8 != 0)
    {
        SEQ = 0x103438E230100000ULL | ((uint64_t)(i_length % 8) << 24);
        CNT = ((uint64_t)((i_length / 8) - 1) << 32) | ((uint64_t)(0x6) << 12);
    }
    else
    {
        SEQ = 0x103438E210000000ULL;
        CNT = ((uint64_t)((i_length / 8) - 1) << 32) | ((uint64_t)(0x6) << 12);
    }

    SEQ |= (static_cast<uint64_t>(i_handle.slave) << 56);

    FAPI_TRY(spi_set_write_enable(i_handle));

    FAPI_TRY(spi_wait_for_idle(i_handle));

    FAPI_DBG("spi_write_prep_seq: SEQ = [0x%08X%08X]", SEQ >> 32, SEQ & 0xFFFFFFFF);
    FAPI_DBG("spi_write_prep_seq: CNT = [0x%08X%08X]", CNT >> 32, CNT & 0xFFFFFFFF);
    FAPI_DBG("spi_write_prep_seq: TDR = [0x%08X%08X]", TDR >> 32, TDR & 0xFFFFFFFF);

    // Place sequence and send write command
    FAPI_TRY(putScom(i_handle.target_chip, i_handle.base_addr + SPIM_SEQREG, SEQ));
    FAPI_TRY(putScom(i_handle.target_chip, i_handle.base_addr + SPIM_COUNTERREG, CNT));
    FAPI_TRY(putScom(i_handle.target_chip, i_handle.base_addr + SPIM_TDR, TDR));

    FAPI_TRY(spi_wait_for_tdr_empty(i_handle)); //Wait for previous TDR to be sent

fapi_try_exit:
    return fapi2::current_err;
}


///////////////////////////////////////////
/////////// spi_write_post_seq ////////////
///////////////////////////////////////////
fapi2::ReturnCode
spi_write_post_seq(SpiControlHandle& i_handle, uint32_t i_address)
{
    fapi2::buffer<uint64_t> data64 = 0;
    FAPI_TRY(spi_wait_for_tdr_empty(i_handle)); //Wait for previous TDR to be sent

    // Wait until machine is no longer executing
    FAPI_TRY(spi_wait_for_idle(i_handle));

    FAPI_TRY(spi_wait_for_write_complete(i_handle, i_address, data64));

    // Check for program status bit
    FAPI_ASSERT( (data64.getBit<59>() == 0),
                 fapi2::SBE_SPI_OPR_COMPLETION_CHECK_ERROR()
                 .set_CHIP_TARGET(i_handle.target_chip)
                 .set_SPI_ENGINE(i_handle.engine)
                 .set_BASE_ADDRESS(i_handle.base_addr + SPIM_RDR)
                 .set_STATUS_REGISTER(data64),
                 "spi_write_post_seq error" );

fapi_try_exit:
    return spi_restore_mmio_config(i_handle, fapi2::current_err);
}


///////////////////////////////////////////
////////////// spi_write_secure ///////////
///////////////////////////////////////////
fapi2::ReturnCode
spi_write_secure(SpiControlHandle& i_handle, uint32_t i_address, uint8_t* i_data, uint32_t i_length)
{
    do
    {
        // Get write prep seq
        FAPI_TRY(spi_write_prep_seq(i_handle, i_address, i_length));

        uint64_t l_temp = 0;
        fapi2::buffer<uint64_t> TDR = 0;

        for (uint32_t i = 0; i < i_length; i += 8)
        {
            l_temp = ((uint64_t*)i_data)[i / 8];

            TDR = l_temp;

            FAPI_TRY(putScom(i_handle.target_chip, i_handle.base_addr + SPIM_TDR, TDR));
            FAPI_TRY(spi_wait_for_tdr_empty(i_handle));
            l_temp = 0;
        }

        // Get write post seq
        FAPI_TRY(spi_write_post_seq(i_handle, i_address));

        // Verify the written data by reading it back and compare
        // FIXME: The assumption is,
        //  - The "spi_write_secure" API try to write max
        //    SEEPROM_SECURE_DATA_LEN_MAX per attempt and also
        //    the MAX_READ_LENGTH value will be the same as like
        //    SEEPROM_SECURE_DATA_LEN_MAX so using the given length to write
        //    as a read length too.
        //  - Always read with the "RAW_BYTE_ACCESS" since the caller of
        //    "spi_write_secure" API needs to read with ECC bytes only since
        //    we will write with ECC byte always.
        static_assert(SEEPROM_SECURE_DATA_LEN_MAX == MAX_READ_LENGTH,
                      "Mismatch between SEEPROM_SECURE_DATA_LEN_MAX and MAX_READ_LENGTH");
        uint8_t readData[MAX_READ_LENGTH] = {0};
        FAPI_TRY(spi_read(i_handle, i_address, i_length,
                          SPI_ECC_CONTROL_STATUS::RAW_BYTE_ACCESS,
                          readData),
                 "Failed to read back the written data for the verification.");

        FAPI_ASSERT((memcmp(i_data, readData, i_length) == 0),
                    fapi2::SBE_SPI_WRITTEN_DATA_MISMATCH()
                    .set_CHIP_TARGET(i_handle.target_chip)
                    .set_SPI_ENGINE(i_handle.engine)
                    .set_WRITTEN_DATA_ADDRESS(i_address)
                    .set_WRITTEN_DATA_LENGTH(i_length),
                    "SPI: Verification of data after writing to flash failed.");
    }
    while(0);

fapi_try_exit:
    return fapi2::current_err;
}


///////////////////////////////////////////
////////////// spi_write //////////////////
///////////////////////////////////////////
fapi2::ReturnCode
spi_write(SpiControlHandle& i_handle, uint32_t i_address,
          uint32_t i_length,  uint8_t* i_buffer)
{
    uint32_t cur_address   = i_address;
    uint32_t cur_buf_byte  = 0;
    uint32_t remaining_len = i_length;
    uint32_t page_offset   = 0;
    uint32_t write_len;
    uint32_t write_max_len  = SEEPROM_SECURE_DATA_LEN_MAX;
    fapi2::ReturnCode rc = fapi2::FAPI2_RC_SUCCESS;

    rc = spi_precheck(i_handle);

    if (rc != fapi2::FAPI2_RC_SUCCESS)
    {
        return rc;
    }

    // Secure writes can only be done at byte-length of SEEPROM_SECURE_DATA_LEN or less, therefore
    // write length is determined either by:
    // (1) number of bytes left to write
    // (2) place in the current page of SEEPROM
    // (3) SEEPROM_SECURE_DATA_LEN
    // The smallest of these three is written
    do
    {
        if (SECTOR_NUM_CHECK(0, cur_address) || SECTOR_NUM_CHECK(0x48, cur_address))
        {
            write_max_len = SEEPROM_SECURE_DATA_LEN_MIN;
        }
        else
        {
            write_max_len = SEEPROM_SECURE_DATA_LEN_MAX;
        }

        // Min between remaining_len and write_max_len
        write_len = (remaining_len > write_max_len) ? write_max_len : remaining_len;
        page_offset = (cur_address & (SEEPROM_PAGE_SIZE - 1));
        // Min between above and byte-length left to reach a page
        write_len = ((SEEPROM_PAGE_SIZE - page_offset) < write_len) ? (SEEPROM_PAGE_SIZE - page_offset) : write_len;

        FAPI_DBG("spi_write: Address= [0x%08X] CurrBuf=[0x%08X] Len=[0x%08X]", cur_address, cur_buf_byte, write_len);

        FAPI_TRY(spi_write_secure (i_handle, cur_address, &i_buffer[cur_buf_byte], write_len));

        cur_address   += write_len;
        remaining_len -= write_len;
        cur_buf_byte  += write_len;
    }
    while(remaining_len > 0);

    FAPI_TRY(spi_wait_for_idle(i_handle));

    // Restore the default counter and sdeq used by the side band path
    FAPI_TRY(putScom(i_handle.target_chip, i_handle.base_addr + SPIM_COUNTERREG, 0x0ULL));
    FAPI_TRY(putScom( i_handle.target_chip,
                      i_handle.base_addr + SPIM_SEQREG, SPI_DEFAULT_SEQ) );

fapi_try_exit:
    return fapi2::current_err;
}


///////////////////////////////////////////
////////////// spi_master_reset ///////////
///////////////////////////////////////////
fapi2::ReturnCode spi_master_reset(SpiControlHandle i_handle)
{
    fapi2::buffer<uint64_t> buffer = 0;

    // The pervasive spec says that to reset the SPI master and its internal sequencer we must
    // write 0x5 followed by 0xA to the clock configuration register in the 24-27 bit field. The
    // SPI master will then be reset unconditionally and any pending or running operation is
    // discontinued.
    //
    // However, the configuration register values are not changed, the status register is not reset,
    // and the SPI data registers are not reset. The reset also doesn't have any affect on attached
    // slaves.

    // Get the contents of the clock config register
    FAPI_TRY(getScom(i_handle.target_chip, i_handle.base_addr + SPIM_CLOCKCONFIGREG, buffer));
    FAPI_DBG("Clock Configuration Buffer Contents - Initial Read: 0x%.16X", buffer());

    // Write 0x5 to reset control bit field
    buffer.clearBit<24>().setBit<25>().clearBit<26>().setBit<27>();

    // Write the first portion of the reset sequence to the register.
    FAPI_TRY(putScom(i_handle.target_chip, i_handle.base_addr + SPIM_CLOCKCONFIGREG, buffer));

    // Read back the contents of the register.
    FAPI_TRY(getScom(i_handle.target_chip, i_handle.base_addr + SPIM_CLOCKCONFIGREG, buffer));
    FAPI_DBG("Clock Configuration Buffer Contents - 0x5 written: 0x%.16X", buffer());

    // Write 0xA to reset control bit field
    buffer.setBit<24>().clearBit<25>().setBit<26>().clearBit<27>();

    // Finish the reset request sequence by writing the final portion of the reset sequence.
    FAPI_TRY(putScom(i_handle.target_chip, i_handle.base_addr + SPIM_CLOCKCONFIGREG, buffer));

fapi_try_exit:
    FAPI_INF("spi_master_reset: exiting...");
    return fapi2::current_err;
}


///////////////////////////////////////////
////////////// spi_sector_erase ///////////
///////////////////////////////////////////
// Erases 4kb sector in which address is located
fapi2::ReturnCode spi_sector_erase(SpiControlHandle& i_handle, uint32_t i_address)
{
    fapi2::buffer<uint64_t> data64 = 0;
    uint32_t address = i_address;
    uint64_t SEQ;
    uint64_t TDR;

    FAPI_INF("spi_sector_erase: Addr:[0x%08X]", i_address);

    if ((address % 8) != 0)
    {
        FAPI_ERR("The address requested is not byte aligned, exiting");
        return fapi2::FAPI2_RC_INVALID_PARAMETER;
    }

    FAPI_TRY(spi_set_write_enable(i_handle));

    SEQ = SPI_ERASE_SEQ     | ((uint64_t)i_handle.slave << 56);
    TDR = SPI_SEC_ERASE_OP  | (((uint64_t)address << 32) & 0x00ffffffffffffffULL);

    FAPI_TRY(putScom(i_handle.target_chip, i_handle.base_addr + SPIM_SEQREG, SEQ));
    FAPI_TRY(putScom(i_handle.target_chip, i_handle.base_addr + SPIM_TDR, TDR));

    FAPI_TRY(spi_wait_for_tdr_empty(i_handle));

    FAPI_TRY(spi_wait_for_idle(i_handle));

    FAPI_TRY(spi_wait_for_write_complete(i_handle, i_address, data64));

    // Check for erase status bit
    FAPI_ASSERT( (data64.getBit<58>() == 0),
                 fapi2::SBE_SPI_OPR_COMPLETION_CHECK_ERROR()
                 .set_CHIP_TARGET(i_handle.target_chip)
                 .set_SPI_ENGINE(i_handle.engine)
                 .set_BASE_ADDRESS(i_handle.base_addr + SPIM_RDR)
                 .set_STATUS_REGISTER(data64),
                 "spi_sector_erase error" );

fapi_try_exit:
    FAPI_INF("SPI sector erase: Exiting ...");
    return spi_restore_mmio_config(i_handle, fapi2::current_err);
}


///////////////////////////////////////////
////////////// spi_block_erase ////////////
///////////////////////////////////////////
// Erases 32kb block in which address is located
fapi2::ReturnCode spi_block_erase(SpiControlHandle& i_handle, uint32_t i_address)
{
    fapi2::buffer<uint64_t> data64 = 0;
    uint32_t address = i_address;
    uint64_t SEQ;
    uint64_t TDR;

    FAPI_INF("spi_blk_erase: Addr:[0x%08X]", i_address);

    if ((address % 8) != 0)
    {
        FAPI_ERR("The address requested is not byte aligned, exiting");
        return fapi2::FAPI2_RC_INVALID_PARAMETER;
    }

    FAPI_TRY(spi_set_write_enable(i_handle));

    SEQ = SPI_ERASE_SEQ     | ((uint64_t)i_handle.slave << 56);
    TDR = SPI_BLK_ERASE_OP  | (((uint64_t)address << 32) & 0x00ffffffffffffffULL);

    FAPI_TRY(putScom(i_handle.target_chip, i_handle.base_addr +  SPIM_SEQREG, SEQ));
    FAPI_TRY(putScom(i_handle.target_chip, i_handle.base_addr +  SPIM_TDR, TDR));

    FAPI_TRY(spi_wait_for_tdr_empty(i_handle));

    FAPI_TRY(spi_wait_for_idle(i_handle));

    FAPI_TRY(spi_wait_for_write_complete(i_handle, i_address, data64));

    // Check for erase status bit
    FAPI_ASSERT( (data64.getBit<58>() == 0),
                 fapi2::SBE_SPI_OPR_COMPLETION_CHECK_ERROR()
                 .set_CHIP_TARGET(i_handle.target_chip)
                 .set_SPI_ENGINE(i_handle.engine)
                 .set_BASE_ADDRESS(i_handle.base_addr + SPIM_RDR)
                 .set_STATUS_REGISTER(data64),
                 "spi_block_erase error" );

fapi_try_exit:
    FAPI_INF("SPI block erase: Exiting ...");
    return spi_restore_mmio_config(i_handle, fapi2::current_err);
}


static const uint64_t ecc_mask[] =
{
    0xFF0000E8423C0F99ull,
    0x99FF0000E8423C0Full,
    0x0F99FF0000E8423Cull,
    0x3C0F99FF0000E842ull,
    0x423C0F99FF0000E8ull,
    0xE8423C0F99FF0000ull,
    0x00E8423C0F99FF00ull,
    0x0000E8423C0F99FFull
};


///////////////////////////////////////////
////////////// spi_ecc_gen ////////////////
///////////////////////////////////////////
//generates for an 8-byte array one additional ecc-byte
static uint8_t spi_ecc_gen(const uint8_t* bytes)
{
    uint8_t ecc   = 0;
    uint64_t data = 0;

    for (uint32_t i = 0; i < 8; i++)
    {
        data = data << 8 | bytes[i];
    }

    for (int ecc_bit = 0; ecc_bit < 8; ecc_bit++ )
    {
        uint64_t scratch = ecc_mask[ecc_bit] & data;

        /* The ECC bit is the LSB of the hamming weight, which
         * happens to be the XOR across all bits of the value */
        uint32_t scratch2 = scratch ^ (scratch >> 32);
        scratch2 ^= scratch2 >> 16;
        scratch2 ^= scratch2 >> 8;
        scratch2 ^= scratch2 >> 4;
        scratch2 ^= scratch2 >> 2;
        scratch2 ^= scratch2 >> 1;
        ecc = (ecc << 1) | (scratch2 & 1);
    }

    return ecc;
}


///////////////////////////////////////////
////////////// spi_write_ecc //////////////
///////////////////////////////////////////
fapi2::ReturnCode
spi_write_ecc(SpiControlHandle& i_handle, uint32_t i_address,
              uint32_t i_length,  uint8_t* i_buffer, bool i_ecc)
{
    uint32_t cur_address = i_address;
    uint8_t buf[256] = {0};

    if ((i_address % 8 != 0) || (i_length % 8 != 0) || (i_length == 0))
    {
        FAPI_ERR("The write address/length passed is not 8 byte aligned OR length passes is 0, exiting");
        return fapi2::FAPI2_RC_INVALID_PARAMETER;
    }

    if (i_ecc)
    {
        cur_address = (cur_address * 9) / 8;
    }

    FAPI_INF("spi_write_ecc: Device Address (wo ECC):[0x%08X] (with ECC):[0x%08X]", i_address, cur_address);

    for (uint32_t i = 0, j = 0, k = cur_address; i < i_length; i++)
    {
        buf[j] = i_buffer[i];

        if ((k % 256 == 255) || ((i == (i_length - 1U)) && !(i % 8 == 7 && i_ecc)))
        {
            FAPI_TRY(spi_write(i_handle, k - j, j + 1, buf));
            k++;
            j = 0;
        }
        else
        {
            j++;
            k++;
        }

        if (i % 8 == 7 && i_ecc)
        {
            buf[j] = spi_ecc_gen(&(i_buffer[i - 7]));

            if ((k % 256 == 255) || (i == (i_length - 1U)))
            {
                FAPI_TRY(spi_write(i_handle, k - j, j + 1, buf));
                k++;
                j = 0;
            }
            else
            {
                j++;
                k++;
            }
        }
    }

fapi_try_exit:
    FAPI_INF("SPI write with ecc: Exiting ...");
    return fapi2::current_err;
}

#if 0
///////////////////////////////////////////
///////// spi_erase_and_preserve //////////
///////////////////////////////////////////
// Perform SPI erase and preserve data.
// Assumption: Each file (runtime, override etc) size in nor would be greater
// than 32KB (block size in nor) i.e the diff of start and end address supplied
// should always be greater then 32KB else error out.
// The process is divided into three steps:
//
// First: (starting address in sector)
// Read sector in which starting address is located. This sector would be any
// one of eight sectors in a block in nor. Check for both start and end address
// if within same sector. If yes, perform erase followed by writing data back
// to the sector and exit else figure out remaining sectors in this
// block and erase all sectors (including sector in which starting address is
// located). This step is performed using sector erase operation.
// Write back data in sector (which has starting address located) starting
// from sector start address till the starting address location.
//
// Middle:
// Figure out the remaining blocks/sectors (excluding one above) wrt to the end
// address supplied. Perform block erase operation if the diff of end and start
// address expands to more than the block size in nor (excluding the last block
// that contains the end address in one of eight sectors in that block).
// Following to this step, figure out the remaining sectors before and excluding
// the one which contains the end address and erase the same using sector erase
// operation.
//
// End: (end address in sector)
// Read sector in which end address is located. Now write back data in sector
// starting from end address supplied till the end of this sector.
fapi2::ReturnCode spi_erase_and_preserve(SpiControlHandle& i_handle,
        uint32_t i_startAddress, uint32_t i_endAddress)
{
    uint32_t startAddr = i_startAddress;
    uint32_t endAddr   = i_endAddress;
    uint8_t sectorBuffer[NOR_FLASH_SECTOR_SIZE] __attribute__ ((aligned (8))) = {0};

    do
    {
        FAPI_INF("spi_erase_preserve: SAddr:[0x%08X] EAddr:[0x%08X]", startAddr, endAddr);

        // Check on validity of start and and address value received
        if ((endAddr - startAddr) + 1 > NOR_FLASH_DEVICE_SIZE)
        {
            FAPI_ERR("Error: Diff of start and end address is more than NOR device size of 16MB, exiting");
            return fapi2::FAPI2_RC_INVALID_PARAMETER;
        }

        // Start sector to preserve & erase
        // --------------------------------
        // Read sector in which starting address is located
        FAPI_INF("spi_erase_preserve: SAddr:[0x%08X]", (startAddr / NOR_FLASH_SECTOR_SIZE) * NOR_FLASH_SECTOR_SIZE);
        FAPI_TRY(spi_read(i_handle,
                          (startAddr / NOR_FLASH_SECTOR_SIZE) * NOR_FLASH_SECTOR_SIZE,
                          NOR_FLASH_SECTOR_SIZE,
                          SPI_ECC_CONTROL_STATUS::RAW_BYTE_ACCESS,
                          sectorBuffer));

        // Check for end address (if within same sector)
        if (startAddr / NOR_FLASH_SECTOR_SIZE == endAddr / NOR_FLASH_SECTOR_SIZE)
        {
            // Erase the sector
            FAPI_TRY(spi_sector_erase(i_handle, (startAddr / NOR_FLASH_SECTOR_SIZE) *NOR_FLASH_SECTOR_SIZE));

            // Write back data before beginning of start address
            FAPI_TRY(spi_write(i_handle,
                               (startAddr / NOR_FLASH_SECTOR_SIZE) * NOR_FLASH_SECTOR_SIZE,
                               startAddr % NOR_FLASH_SECTOR_SIZE,
                               sectorBuffer));

            // Check end address to write data back after end address if not aligned to sector boundary
            if (endAddr % NOR_FLASH_SECTOR_SIZE)
            {
                // Check for end address alinged to 8-byte boundary
                uint32_t len = endAddr % NOR_FLASH_SECTOR_SIZE;

                if (endAddr % 8)
                {
                    uint8_t i;

                    for (i = 0; i < (endAddr + 1) % 8; i++)
                    {
                        sectorBuffer[len - i] = 0xFF;
                    }

                    endAddr -= (i - 1);
                }

                FAPI_TRY(spi_write(i_handle,
                                   endAddr,
                                   NOR_FLASH_SECTOR_SIZE - (endAddr % NOR_FLASH_SECTOR_SIZE),
                                   &sectorBuffer[endAddr % NOR_FLASH_SECTOR_SIZE]));
            }

            break;
        }

        // Get number of sectors in block where start address is located
        uint16_t numSector = NOR_FLASH_BLOCK_SIZE / NOR_FLASH_SECTOR_SIZE -
                             ((((startAddr / NOR_FLASH_SECTOR_SIZE) * NOR_FLASH_SECTOR_SIZE) -
                               ((startAddr / NOR_FLASH_BLOCK_SIZE) * NOR_FLASH_BLOCK_SIZE)) / NOR_FLASH_SECTOR_SIZE);
        FAPI_INF("spi_erase_preserve: SAddr:[0x%08X] NSector:[0x%08X]",
                 (startAddr / NOR_FLASH_SECTOR_SIZE) * NOR_FLASH_SECTOR_SIZE,
                 numSector);

        uint32_t i = 0, j = 0;

        if (numSector > 0)
        {
            for (i = (startAddr / NOR_FLASH_SECTOR_SIZE) * NOR_FLASH_SECTOR_SIZE, j = 0;
                 j < numSector; i += NOR_FLASH_SECTOR_SIZE, j++)
            {
                if (i == (endAddr / NOR_FLASH_SECTOR_SIZE) * NOR_FLASH_SECTOR_SIZE)
                {
                    break;
                }

                FAPI_TRY(spi_sector_erase(i_handle, i));
            }
        }

        // Perform write data preserving upto start address
        FAPI_INF("spi_erase_preserve: SAddr:[0x%08X] Len:[0x%08X]", (startAddr / NOR_FLASH_SECTOR_SIZE) * NOR_FLASH_SECTOR_SIZE,
                 startAddr - ((startAddr / NOR_FLASH_SECTOR_SIZE) * NOR_FLASH_SECTOR_SIZE));
        FAPI_TRY(spi_write(i_handle,
                           (startAddr / NOR_FLASH_SECTOR_SIZE) * NOR_FLASH_SECTOR_SIZE,
                           startAddr % NOR_FLASH_SECTOR_SIZE,
                           sectorBuffer));
        startAddr += (NOR_FLASH_SECTOR_SIZE * j);
        FAPI_INF("spi_erase_preserve: SAddr:[0x%08X]", (startAddr / NOR_FLASH_SECTOR_SIZE) * NOR_FLASH_SECTOR_SIZE);

        // Middle sectors/blocks to be erased
        // ----------------------------------
        if (((startAddr / NOR_FLASH_SECTOR_SIZE) + 1) <= (endAddr / NOR_FLASH_SECTOR_SIZE))
        {
            uint16_t numBlock = (((endAddr / NOR_FLASH_SECTOR_SIZE) * NOR_FLASH_SECTOR_SIZE) -
                                 ((startAddr / NOR_FLASH_SECTOR_SIZE) * NOR_FLASH_SECTOR_SIZE)) / NOR_FLASH_BLOCK_SIZE;
            FAPI_INF("spi_erase_preserve: SAddr:[0x%08X] EAddr:[0x%08X] NBlock:[0x%08X]",
                     (startAddr / NOR_FLASH_SECTOR_SIZE) * NOR_FLASH_SECTOR_SIZE,
                     (endAddr / NOR_FLASH_SECTOR_SIZE) * NOR_FLASH_SECTOR_SIZE,
                     numBlock);

            if (numBlock > 0)
            {
                // Perform block erase
                for (uint32_t i = (startAddr / NOR_FLASH_SECTOR_SIZE) * NOR_FLASH_SECTOR_SIZE, j = 0;
                     j < numBlock; i += NOR_FLASH_BLOCK_SIZE, j++)
                {
                    FAPI_TRY(spi_block_erase(i_handle, i));
                }

                startAddr += (NOR_FLASH_BLOCK_SIZE * numBlock);
            }

            uint16_t numSector = (((endAddr / NOR_FLASH_SECTOR_SIZE) * NOR_FLASH_SECTOR_SIZE) -
                                  ((startAddr / NOR_FLASH_SECTOR_SIZE) * NOR_FLASH_SECTOR_SIZE)) / NOR_FLASH_SECTOR_SIZE;
            FAPI_INF("spi_erase_preserve: SAddr:[0x%08X] EAddr:[0x%08X] NSector:[0x%08X]",
                     (startAddr / NOR_FLASH_SECTOR_SIZE) * NOR_FLASH_SECTOR_SIZE,
                     (endAddr / NOR_FLASH_SECTOR_SIZE) * NOR_FLASH_SECTOR_SIZE,
                     numSector);

            if (numSector > 0)
            {
                // Perform sector erase
                for (uint32_t i = (startAddr / NOR_FLASH_SECTOR_SIZE) * NOR_FLASH_SECTOR_SIZE, j = 0;
                     j < numSector; i += NOR_FLASH_SECTOR_SIZE, j++)
                {
                    FAPI_TRY(spi_sector_erase(i_handle, i));
                }
            }
        }

        // End sector to preserve and erase
        // --------------------------------
        if ((endAddr % NOR_FLASH_SECTOR_SIZE) != 0)
        {
            FAPI_INF("spi_erase_preserve: EAddr:[0x%08X]", (endAddr / NOR_FLASH_SECTOR_SIZE) * NOR_FLASH_SECTOR_SIZE);
            // Perform sector read
            FAPI_TRY(spi_read(i_handle,
                              (endAddr / NOR_FLASH_SECTOR_SIZE) * NOR_FLASH_SECTOR_SIZE,
                              NOR_FLASH_SECTOR_SIZE,
                              SPI_ECC_CONTROL_STATUS::RAW_BYTE_ACCESS,
                              sectorBuffer));

            // Perform sector erase
            FAPI_TRY(spi_sector_erase(i_handle,
                                      (endAddr / NOR_FLASH_SECTOR_SIZE) * NOR_FLASH_SECTOR_SIZE));

            // If end address is not aligned to 8 bytes. Make sure to correct it
            // by decrementing the address by the factor of 8 bytes aligned
            // address. Correspondingly matchup the sector buffer read values
            // at the respective offset to 0xFF (ref to erased value in nor)
            uint32_t len = endAddr % NOR_FLASH_SECTOR_SIZE;
            FAPI_INF("spi_erase_preserve: EAddr:[0x%08X] Len:[0x%08X] ", endAddr,
                     NOR_FLASH_SECTOR_SIZE - (endAddr % NOR_FLASH_SECTOR_SIZE));

            if (endAddr % 8)
            {
                uint8_t i;

                for (i = 0; i < (endAddr + 1) % 8; i++)
                {
                    sectorBuffer[len - i] = 0xFF;
                }

                endAddr -= (i - 1);
            }

            // Perform sector write
            FAPI_INF("spi_erase_preserve: EAddr:[0x%08X] Len:[0x%08X]", endAddr,
                     NOR_FLASH_SECTOR_SIZE - (endAddr % NOR_FLASH_SECTOR_SIZE));
            FAPI_TRY(spi_write(i_handle,
                               endAddr,
                               NOR_FLASH_SECTOR_SIZE - (endAddr % NOR_FLASH_SECTOR_SIZE),
                               &sectorBuffer[endAddr % NOR_FLASH_SECTOR_SIZE]));
        }
    }
    while (0);

    FAPI_INF("SPI Sector range erase while preserving bytes before the starting and after the ending address in the sectors: Exiting ...");
    return fapi2::FAPI2_RC_SUCCESS;
fapi_try_exit:
    return fapi2::current_err;
}
#endif

//////////////////////////////////////////////
///////// spi_erase_and_no_preserve //////////
//////////////////////////////////////////////
fapi2::ReturnCode spi_erase_and_no_preserve(SpiControlHandle& i_handle,
        uint32_t i_startAddress, uint32_t i_endAddress)
{
    fapi2::current_err = fapi2::FAPI2_RC_SUCCESS;
    uint32_t startAddr = i_startAddress;
    uint32_t endAddr   = i_endAddress;

    do
    {
        FAPI_INF("spi_erase_no_preserve: SAddr:[0x%08X] EAddr:[0x%08X]",
                 startAddr, endAddr);

        if (((startAddr   & NOR_FLASH_SUB_SECTOR_BOUNDARY_CHECK_MASK) != 0) ||
            (((endAddr + 1) & NOR_FLASH_SUB_SECTOR_BOUNDARY_CHECK_MASK) != 0))
        {
            // Address unaligned error
            FAPI_ERR("Address not aligned to 4K boundary");
            fapi2::current_err = fapi2::FAPI2_RC_INVALID_PARAMETER;
            break;
        }

        if (startAddr > endAddr)
        {
            // Address range error
            FAPI_ERR("Address range error: start address is more than end address");
            fapi2::current_err = fapi2::FAPI2_RC_INVALID_PARAMETER;
            break;
        }

        // Erase sectors to reach next block boundary
        while (((startAddr % NOR_FLASH_BLOCK_SIZE) != 0) && (startAddr < endAddr))
        {
            FAPI_TRY(spi_sector_erase(i_handle, startAddr));
            startAddr += NOR_FLASH_SUB_SECTOR_SIZE;
        }

        while (startAddr < endAddr)
        {
            if ( (endAddr - startAddr) >= NOR_FLASH_BLOCK_SIZE )
            {
                // Both start and end address are block size apart
                FAPI_TRY(spi_block_erase(i_handle, startAddr));
                startAddr += NOR_FLASH_BLOCK_SIZE;
            }
            else
            {
                // Both start and end address are sector size apart or
                // within same sector
                FAPI_TRY(spi_sector_erase(i_handle, startAddr));
                startAddr += NOR_FLASH_SUB_SECTOR_SIZE;
            }
        }
    }
    while (0);

    if ( fapi2::current_err == fapi2::FAPI2_RC_SUCCESS )
    {
        FAPI_INF("SPI Sector range erase is successfull");
    }

fapi_try_exit:
    return fapi2::current_err;
}
