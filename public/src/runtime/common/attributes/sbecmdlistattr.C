/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/common/attributes/sbecmdlistattr.C $       */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2023,2025                        */
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
#include "sbeFifoMsgUtils.H"
#include "chipop_struct.H"
#include "sbecmdlistattr.H"
#include "sbe_sp_intf.H"
#include "sbeglobals.H"
#include "heap.H"
#include "attribute_list.H"

uint32_t sbeCmdListAttr(uint8_t *i_pArg)
{
    #define SBE_FUNC " sbeCmdListAttr "
    SBE_ENTER(SBE_FUNC);

    uint32_t l_len = 0;
    uint32_t l_fifoRc = SBE_SEC_OPERATION_SUCCESSFUL;
    sbeFifoType type;
    sbeRespGenHdr_t respHdr;
    respHdr.init();
    do
    {
        chipOpParam_t* configStr = (struct chipOpParam*)i_pArg;
        type = static_cast<sbeFifoType>(configStr->fifoType);
        l_fifoRc = sbeUpFifoDeq_mult(l_len, NULL, true, false, type);
        // If FIFO access failure
        if(l_fifoRc)
        {
            SBE_ERROR(SBE_FUNC"l_fifoRc upfifo : 0x%08x", l_fifoRc);
            break;
        }
        SBE_INFO(SBE_FUNC "Fifo Type is:[%02X]",type);

        fapi2::sbefifo_hwp_data_ostream l_outStream(type);

        //Ideally,a separate RC should be used to capture SBE FIFO error and the secondary
        //error. Since listAttribute() ignores all the secondary errors and return only
        //SBE FIFO RC, it is fine here.
        l_fifoRc = fapi2::ATTR::listAttribute(&l_outStream);

    } while(false);

    if (l_fifoRc == SBE_SEC_OPERATION_SUCCESSFUL)
    {
        l_fifoRc = sbeDsSendRespHdr(respHdr, NULL, type);
        if ( l_fifoRc != SBE_SEC_OPERATION_SUCCESSFUL )
        {
            SBE_ERROR(SBE_FUNC"Attribute List chip-op send response "
                        "header failed.RC=0x%08X",l_fifoRc);
        }
    }
    SBE_EXIT(SBE_FUNC);
    return l_fifoRc;
    #undef SBE_FUNC
}
