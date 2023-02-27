/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/odyssey/sppe/hwpf/plat_hw_access.C $       */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2016,2023                        */
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

#include "target.H"
#include "hw_access.H"
#include "plat_hw_access.H"
#include "ppe42_scom.h"
#include "hwp_return_codes.H"
#include "plat_error_scope.H"
#include "hwp_ffdc_classes.H"
#include "error_info_defs.H"
#include "ffdc.H"
#include <error_info.H>
#include "plat_target_base.H"
#include "sbe_sp_intf.H"

namespace fapi2
{

/**
 * @brief Indirect SCOM Status
 */
union IndirectScom_t
{
    uint64_t data64;
    struct
    {
        uint64_t :12;        //0:11
        uint64_t addr:20;    //12:31
        uint64_t done:1;     //32
        uint64_t piberr:3;   //33:35
        uint64_t userstat:4; //36:39
        uint64_t :8;         //40:47
        uint64_t data:16;    //48:63
    };

};
// Wait time slice to check indirect scom status register
static const uint32_t SBE_INDIRECT_SCOM_WAIT_TIME_NS = 10000;
// Indirect scom timeout
static const uint32_t MAX_INDSCOM_TIMEOUT_NS = 100000; //0.1 ns

static const uint64_t DIRECT_SCOM_ADDR_MASK = 0x8000000000000000;
static const uint64_t INDIRECT_SCOM_NEW_ADDR_MASK = 0x9000000000000000;
// Allowed Address first 3 bits
// 0b000 - Allowed
// 0b10X - Not Allowed
// 0b110 - Allowed
// 0b111 - Allowed
static const uint64_t SCOM_SBE_ADDR_MASK = 0xE0000000; // 0x01, 0x11, 0x00 is allowed
static const uint64_t SCOM_MASTER_ID_MASK = 0x00F00000;
static const uint64_t PIBMEM_SCOM_MASK = 0xFFF80000;

// Scom types
enum sbeScomType
{
    SBE_SCOM_TYPE_DIRECT = 0,      // Direct scom
};


static uint32_t getEffectiveAddress(const uint32_t *i_target, const uint32_t i_addr, bool isIndirectScom = false)
{
    uint32_t translatedAddr = 0;
    const plat_target_sbe_handle* l_targetBase =
            reinterpret_cast<const plat_target_sbe_handle*>(i_target);
    uint8_t targetInstance = 0;
    uint32_t BITSHIFT = 0;
    switch(l_targetBase->getTargetType())
    {
        case PPE_TARGET_TYPE_CHIP:
            translatedAddr = (l_targetBase->getPIBAddress() | i_addr);
            break;
        case PPE_TARGET_TYPE_MEMPORT:
            // Get the target instance.
            targetInstance = l_targetBase->getTargetInstance();
            // Check for Indirect scom.
            if(isIndirectScom) // Left most bit is set for indirect scom address.
            {
                // Base Addr  0x800070550801303F
                // Trans Addr 0x800070550801303F ---> Mem port 0
                // Trans Addr 0x800070550801343F ---> Mem port 1
                BITSHIFT = 10;
                translatedAddr = i_addr;
            }
            else
            {
                //Base Addr  0x08011810
                //Trans Addr 0x08011810 ---> Mem port 0
                //Trans Addr 0x08012810 ---> Mem port 1
                BITSHIFT = 13;
                // Set bit 12 to 0.
                translatedAddr = targetInstance ? (i_addr & 0xFFFFEFFF) : i_addr;
            }
            translatedAddr = targetInstance ? translatedAddr | ( 1 << BITSHIFT ) : translatedAddr;
            break;
        default: //For all the chiplet types
            {
                translatedAddr = l_targetBase->getPIBAddress() | (i_addr & 0x00FFFFFF);
                break;
            }
    }
    SBE_DEBUG("getEffectiveAddress Target is 0x%08X, i_addr is 0x%08X, translatedAddr is 0x%08X", uint32_t(*l_targetBase), i_addr, translatedAddr);
    return translatedAddr;
}

fapi2::ReturnCode pibRcToFapiRc(const uint32_t i_pibRc)
{
    fapi2::ReturnCode l_fapiRc = FAPI2_RC_SUCCESS;
    switch(i_pibRc)
    {
        case PIB_XSCOM_ERROR:
            l_fapiRc = RC_SBE_PIB_XSCOM_ERROR;
            break;
        case PIB_OFFLINE_ERROR:
            l_fapiRc = RC_SBE_PIB_OFFLINE_ERROR;
            break;
        case PIB_PARTIAL_ERROR:
            l_fapiRc = RC_SBE_PIB_PARTIAL_ERROR;
            break;
        case PIB_ADDRESS_ERROR:
            l_fapiRc = RC_SBE_PIB_ADDRESS_ERROR;
            break;
        case PIB_CLOCK_ERROR:
            l_fapiRc = RC_SBE_PIB_CLOCK_ERROR;
            break;
        case PIB_PARITY_ERROR:
            l_fapiRc = RC_SBE_PIB_PARITY_ERROR;
            break;
        case PIB_TIMEOUT_ERROR:
            l_fapiRc = RC_SBE_PIB_TIMEOUT_ERROR;
            break;
        case PIB_NO_ERROR:
        default:
            break;
    }
    return l_fapiRc;
}

fapi2::ReturnCode handle_scom_error(const uint32_t i_addr, uint8_t i_pibRc)
{
    SBE_ERROR("handle_scom_error : pibrc=0x%X, i_addr=0x%8X", i_pibRc, i_addr);
    PLAT_FAPI_ASSERT( false,
                      SBE_SCOM_FAILURE().
                      set_address(i_addr).
                      set_pcb_pib_rc(i_pibRc),
                      "SCOM : pcb pib error, pibRc[0x%08X] Translated_ScomAddr[0x%08X]",
                      i_pibRc, i_addr);
    fapi_try_exit:
        // Override FAPI RC based on PIB RC
        fapi2::current_err = pibRcToFapiRc(i_pibRc);
        fapi2::g_FfdcData.fapiRc = fapi2::current_err;
    return fapi2::current_err;
}

fapi2::ReturnCode getscom_abs_wrap(const void *i_target,
                                   const uint32_t i_addr, uint64_t *o_data,
                                   bool isIndirectScom)
{
    uint32_t l_pibRc = 0;
    uint32_t l_addr = i_addr;
    l_addr = getEffectiveAddress((uint32_t *)i_target, i_addr, isIndirectScom);
    l_pibRc = getscom_abs(l_addr, o_data);

    SBE_DEBUG("SCOMOUT  %08X %08X%08X",
                    l_addr,
                    *((uint32_t*)o_data),
                    *(((uint32_t*)o_data) + 1));

    return (l_pibRc == PIB_NO_ERROR) ? fapi2::ReturnCode(FAPI2_RC_SUCCESS) :
                 handle_scom_error(l_addr, l_pibRc);
}

fapi2::ReturnCode putscom_abs_wrap(const void *i_target,
                                    const uint32_t i_addr, uint64_t i_data,
                                    bool isIndirectScom)
{
    uint32_t l_pibRc = 0;
    uint32_t l_addr = i_addr;
    l_addr = getEffectiveAddress((uint32_t *)i_target, i_addr, isIndirectScom);

    SBE_DEBUG("SCOMIN   %08X %08X%08X",
                    l_addr,
                    *((uint32_t*)&i_data),
                    *(((uint32_t*)&i_data) + 1));

    l_pibRc = putscom_abs(l_addr, i_data);
    return (l_pibRc == PIB_NO_ERROR) ? fapi2::ReturnCode(FAPI2_RC_SUCCESS) :
                 handle_scom_error(l_addr, l_pibRc);
}

static ReturnCode doIndirectScom(const bool i_isRead,
                                 const void *i_target,
                                 const uint64_t i_addr,
                                 uint64_t & io_data)
{
    #define SBE_FUNC " doIndirectScom "
    ReturnCode fapiRc = FAPI2_RC_SUCCESS;

    do {
        // We are performing an indirect scom.
        // Zero out the indirect address location.. leave the 16bits of data
        // Get the 31-bits indirect scom address
        // OR in the 31-bits indirect address
        uint64_t tempBuffer = ( io_data & 0x000000000000FFFF) |
            ( i_addr & 0x7FFFFFFF00000000 );

        // zero out the indirect address from the buffer..
        // bit 0-31 - indirect area..
        // bit 32 - always 0
        // bit 33-47 - bcast/chipletID/port
        // bit 48-63 - local addr
        uint32_t tempAddr = i_addr & 0x7FFFFFFF;

        // If we are doing a read. We need to do a write first..
        if( i_isRead)
        {
            // turn the read bit on.
            tempBuffer |= 0x8000000000000000;
        }

        // perform write before the read with the new
        // IO_buffer with the imbedded indirect scom addr.
        fapiRc = putscom_abs_wrap (i_target, tempAddr, tempBuffer, true);
        if( fapiRc != FAPI2_RC_SUCCESS )
            break;

        // Need to check loop on read until we see done, error,
        //  or we timeout
        IndirectScom_t scomout;
        uint32_t elapsedIndScomTimeNs = 0;
        while (true)
        {
            // Now perform the op requested using the passed in
            // IO_Buffer to pass the read data back to caller.
            fapiRc = getscom_abs_wrap (i_target, tempAddr, &(scomout.data64), true);
            if( fapiRc != FAPI2_RC_SUCCESS )
                break;

            // if bit 32 is on indicating a complete bit
            //  or we saw an error, then we're done
            if (scomout.piberr)
            {
                SBE_ERROR(SBE_FUNC "pib error [%d] reading status register", uint32_t(scomout.piberr));
                fapiRc = pibRcToFapiRc(scomout.piberr);
                break;
            }
            if (scomout.done )
            {
                io_data = scomout.data;
                break;
            }
            if (elapsedIndScomTimeNs > MAX_INDSCOM_TIMEOUT_NS)
            {
                SBE_ERROR(SBE_FUNC "Indirect scom timeout.");
                fapiRc = RC_SBE_PIB_TIMEOUT_ERROR;
                break;
            }

            pk_sleep(PK_NANOSECONDS(SBE_INDIRECT_SCOM_WAIT_TIME_NS));
            elapsedIndScomTimeNs += SBE_INDIRECT_SCOM_WAIT_TIME_NS;
        }
    } while (false);

    return fapiRc;
    #undef SBE_FUNC
}

uint32_t platcheckIndirectAndDoScom( const bool i_isRead,
                                 const void *i_target,
                                 const uint64_t i_addr,
                                 uint64_t & io_data,
                                 ReturnCode & io_fapiRc)
{
    #define SBE_FUNC " checkIndirectAndDoScom "
    io_fapiRc = FAPI2_RC_SUCCESS;

    // If the indirect scom bit is 0, then doing a regular scom
    if( (i_addr & DIRECT_SCOM_ADDR_MASK) == 0)
    {
        SBE_DEBUG(SBE_FUNC "Performing Direct scom.");
        if( i_isRead )
        {
            io_fapiRc = getscom_abs_wrap (i_target, (uint32_t)i_addr,
                                          & io_data);
        }
        else
        {
            io_fapiRc = putscom_abs_wrap (i_target, (uint32_t)i_addr,
                                          io_data);
        }
    }
    else
    {
        SBE_DEBUG(SBE_FUNC "Performing Indirect scom");
        io_fapiRc = doIndirectScom(i_isRead, i_target, i_addr, io_data);
        if (io_fapiRc != FAPI2_RC_SUCCESS)
        {
            SBE_ERROR(SBE_FUNC "Indirect SCOM failed. isRead [%d] addr [0x%08X%08X] data [%08X]",
                      i_isRead, i_addr, io_data);
        }
    }
    #undef SBE_FUNC

    return (io_fapiRc == FAPI2_RC_SUCCESS) ? SBE_SEC_OPERATION_SUCCESSFUL : SBE_SEC_PCB_PIB_ERR;
}

};
