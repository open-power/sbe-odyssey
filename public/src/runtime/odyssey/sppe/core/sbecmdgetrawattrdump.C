/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/odyssey/sppe/core/sbecmdgetrawattrdump.C $ */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2022                             */
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
#include "sbeutil.H"
#include "sbeFifoMsgUtils.H"
#include "chipop_struct.H"
#include "sbecmdgetrawattrdump.H"
#include "sbe_sp_intf.H"

// align address to next 8 byte boundary
#define ALIGN_8_BYTES(address) (address+0x7)&(~0x7)
extern uint32_t _attrs_start_ __attribute__ ((section (".attr")));
extern uint32_t _attrs_end_ __attribute__((section(".attr")));

uint32_t sbeGetRawAttrDump (uint8_t *i_pArg)
{
    #define SBE_FUNC " sbeGetRawAttrDump "
    SBE_ENTER(SBE_FUNC);
    uint32_t l_rc = SBE_SEC_OPERATION_SUCCESSFUL;
    uint32_t len = 0;
    sbeRespGenHdr_t respHdr;
    respHdr.init();
    sbeFifoType type;

    uint32_t *startAddr = &_attrs_start_;
    uint32_t *endAddr = &_attrs_end_;
    do
    {
        chipOpParam_t* configStr = (struct chipOpParam*)i_pArg;
        type = static_cast<sbeFifoType>(configStr->fifoType);
        SBE_DEBUG(SBE_FUNC "Fifo Type is:[%02X]",type);

        l_rc = sbeUpFifoDeq_mult (len, NULL, true, false, type);
        // If FIFO access failure
        CHECK_SBE_RC_AND_BREAK_IF_NOT_SUCCESS(l_rc);
        // Attributes can be not aligned to 8
        // and we expect data to be 8 bytes aligned in order to read
        len = ALIGN_8_BYTES(endAddr-startAddr);
        SBE_DEBUG(SBE_FUNC "Length is 0x%08x", len);
        l_rc = sbeDownFifoEnq_mult( len,
                    (uint32_t *)startAddr, type );
        CHECK_SBE_RC_AND_BREAK_IF_NOT_SUCCESS(l_rc);
    }while(false);

    if(l_rc != SBE_SEC_OPERATION_SUCCESSFUL){
        respHdr.setStatus( SBE_PRI_GENERIC_EXECUTION_FAILURE,
                           SBE_SEC_GETATTRDUMP_FAILURE );
    }
    l_rc = sbeDsSendRespHdr(respHdr, NULL, type);
    SBE_EXIT(SBE_FUNC);
    return l_rc;
    #undef SBE_FUNC

}
