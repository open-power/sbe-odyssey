/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/odyssey/sppe/core/sbescom.C $              */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2016,2024                        */
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

#include <stdint.h>
#include "sbe_sp_intf.H"
#include "sbetrace.H"
#include "plat_hw_access.H"
#include "plat_target.H"
#include "sbescom.H"
#include "ffdc.H"
#include "sbesecurity.H"
#include "sberegaccess.H"
#include "odysseylink.H"
#include "sbeglobals.H"
#include "securityutils.H"

using namespace fapi2;

static const uint64_t DIRECT_SCOM_ADDR_MASK = 0x8000000000000000;
static const uint64_t INDIRECT_SCOM_NEW_ADDR_MASK = 0x8000000000000000;
static const uint64_t INDIRECT_BASE_ADDR_MASK = 0x000000007FFFFFFF;
// Allowed Address first 3 bits
// 0b000 - Allowed
// 0b10X - Not Allowed
// 0b110 - Allowed
// 0b111 - Allowed
static const uint64_t SCOM_SBE_ADDR_MASK = 0xE0000000; // 0x01, 0x11, 0x00 is allowed
static const uint64_t SCOM_MASTER_ID_MASK = 0x00F00000;
static const uint64_t PIBMEM_SCOM_MASK = SRAM_ORIGIN;

// Macros Defined for security RC Check, Break if Error
#define CHECK_SBE_SECURITY_RC_AND_BREAK_IF_NOT_SUCCESS(addr, op, resp, fifotype, mask) \
if (g_pSbeSecurityUtils->getScomFilteringCheckLvl() != SOFT_SECURITY_CHECK_DISABLED) \
{ /* Applying scom filtering on untrusted fifo's/PIPEs. */ \
    if(!(SBE_SECURITY::isAllowed(addr,mask,op))) \
    { \
        if (g_pSbeSecurityUtils->getScomFilteringCheckLvl() == \
                                                       SOFT_SECURITY_CHECK_ENABLED) \
        { \
            resp->setStatus(SBE_PRI_UNSECURE_ACCESS_DENIED, \
                            SBE_SEC_DENYLIST_REG_ACCESS_BLOCKED); \
            break; \
        } \
        else \
        { \
            resp->setStatus(SBE_PRI_UNSECURE_ACCESS_DENIED, \
                            SBE_SEC_DENYLIST_REG_ACCESS); \
        } \
    } \
}

void checkIndirectAndDoScom( const bool i_isRead,
                             const uint64_t i_addr,
                             uint64_t & io_data,
                             sbeRespGenHdr_t *const o_hdr,
                             sbeResponseFfdc_t *const o_ffdc,
                             sbeFifoType i_fifoType,
                             uint64_t i_mask )
{

    #define SBE_FUNC " checkIndirectAndDoScom "
    ReturnCode fapiRc = FAPI2_RC_SUCCESS;
    uint32_t rc = SBE_SEC_OPERATION_SUCCESSFUL;
    Target<SBE_ROOT_CHIP_TYPE> l_hndl =  g_platTarget->plat_getChipTarget();

    SBE_DEBUG(SBE_FUNC "checkIndirectAndDoScom: addr = [0x%08X 0x%08X] ",
                        SBE::higher32BWord(i_addr), SBE::lower32BWord(i_addr));
    do
    {
        // Do address validation
        // SBE throws data storage exception if Master ID field is not 0. Also
        // we halt sbe for SBE address space errors. So check these registers
        // at top level so that we do not halt SBE in these cases.
        if(g_pSbeSecurityUtils->getScomFilteringCheckLvl() == SOFT_SECURITY_CHECK_ENABLED)
        {
            if( ( (i_addr & SCOM_SBE_ADDR_MASK) == 0x80000000 ) ||
                ( (i_addr & SCOM_MASTER_ID_MASK) && ((i_addr & SCOM_SBE_ADDR_MASK) == 0x0) ) ||
                ( (i_addr & PIBMEM_SCOM_MASK) == PIBMEM_SCOM_MASK) )
            {
                SBE_ERROR(SBE_FUNC "Invalid scom address [0x%08X 0x%08X] ",
                        SBE::higher32BWord(i_addr), SBE::lower32BWord(i_addr));
                rc = SBE_SEC_INVALID_ADDRESS_PASSED;
                break;
            }
        }

       // If the indirect scom bit is 0, then doing a regular scom
        if( (i_addr & DIRECT_SCOM_ADDR_MASK) == 0)
        {
            SBE_DEBUG(SBE_FUNC "Performing Direct scom security rc");

            CHECK_SBE_SECURITY_RC_AND_BREAK_IF_NOT_SUCCESS(
                        static_cast<uint32_t>(i_addr),
                        (i_isRead ? SBE_SECURITY::READ : SBE_SECURITY::WRITE),
                        o_hdr, i_fifoType, i_mask)
        }
        /* Indirect SCOM */
        else
        {
            // zero out the indirect address from the buffer..
            // bit 0-31 - indirect area..
            // bit 32 - always 0
            // bit 33-47 - bcast/chipletID/port
            // bit 48-63 - local addr
            SBE_DEBUG(SBE_FUNC "Performing In-Direct scom security rc");
            uint64_t tempAddr = i_addr & INDIRECT_BASE_ADDR_MASK;
            CHECK_SBE_SECURITY_RC_AND_BREAK_IF_NOT_SUCCESS(
                            static_cast<uint32_t>(tempAddr),
                            (i_isRead ? SBE_SECURITY::READ : SBE_SECURITY::WRITE),
                            o_hdr, i_fifoType, i_mask)
        }
        rc = platcheckIndirectAndDoScom(i_isRead, &l_hndl, i_addr, io_data, fapiRc);

    } while (0);

    if (fapiRc != FAPI2_RC_SUCCESS)
    {
        o_hdr->setStatus(SBE_PRI_GENERIC_EXECUTION_FAILURE,
                         SBE_SEC_PCB_PIB_ERR);
        if(o_ffdc) o_ffdc->setRc(fapiRc);
        SBE_ERROR(SBE_FUNC "Fapi failure, fapiRc:0x%08X, RC:0x%08X", fapiRc, rc);
    }
    if (rc != SBE_SEC_OPERATION_SUCCESSFUL)
    {
        o_hdr->setStatus(SBE_PRI_GENERIC_EXECUTION_FAILURE,
                         rc);
        SBE_ERROR(SBE_FUNC "Response Failure, fapiRc:0x%08X, RC:0x%08X", fapiRc, rc);
    }

    SBE_DEBUG(SBE_FUNC "Scom data is 0x%08X_%08X", ((io_data >> 32) & 0xFFFFFFFF), (io_data & 0xFFFFFFFF));
    SBE_DEBUG(SBE_FUNC "fapiRc:%u o_hdr->secondaryStatus:0x%08X", fapiRc, o_hdr->secondaryStatus());
    #undef SBE_FUNC
}
