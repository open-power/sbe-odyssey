/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/common/core/sbeFifoMsgUtils.C $            */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2015,2023                        */
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
 * @file: ppe/sbe/sbefw/sbeFifoMsgUtils.C
 *
 * @brief This file contains the SBE FIFO Access Common Utility Functions
 *
 */

#include "sbefifo.H"
#include "sbetrace.H"
#include "sbe_sp_intf.H"
#include "sbeFifoMsgUtils.H"
#include "sbeglobals.H"
#include "sbeffdc.H"
#include "sbeerrorcodes.H"
#include "assert.h"
#include "return_code_defs.H"
#include "error_info_defs.H"
#include <ffdc.H>
#include <error_info.H>
#include <hwp_error_info.H>

// If we can not perform FIFO operation ( FIFO FULL while writing
// or EMPTY while reading ) we will sleep for FIFO_WAIT_SLEEP_TIME
// ms so that FIFO can be ready.
static const uint32_t FIFO_WAIT_SLEEP_TIME = 1;

extern fapi2::pozFfdcData_t g_FfdcData;

// Contents of below array are indexed based on sbeFifoType
const uint64_t g_Fifo_baseAddresses[] =
{
               SBE_FIFO_BASE,
               SBE_HB_FIFO_BASE,
               SBE_PIPE1_BASE,
               SBE_PIPE2_BASE,
               SBE_PIPE3_BASE,
               SBE_PIPE4_BASE,
               SBE_PIPE5_BASE,
               SBE_PIPE6_BASE,
               SBE_PIPE7_BASE,
               SBE_PIPE8_BASE
};

using namespace fapi2;

inline uint32_t sbeBuildRespHeaderStatusWordGlobal (void)
{
    return ( (((uint32_t)SBE_GLOBAL->sbeCmdRespHdr.prim_status)<<16) |
                        (SBE_GLOBAL->sbeCmdRespHdr.sec_status) );
}
//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
uint32_t sbeUpFifoDeq_mult (uint32_t    &io_len,
                            uint32_t    *o_pData,
                            const bool  i_isEotExpected,
                            const bool  i_flush,
                            sbeFifoType i_type)
{
    #define SBE_FUNC " sbeUpFifoDeq_mult "
    SBE_DEBUG(SBE_FUNC "Lenght:[0x%08X], isEotExpected:[%02X], flush:[%02X],"
        "Fifo Type is:[%02X]", io_len, i_isEotExpected, i_flush, i_type);

    uint32_t l_rc = SBE_SEC_OPERATION_SUCCESSFUL;
    uint32_t l_len = 0;

    // If Caller didn't request flush operation
    // and passed a non-zero valid length, we
    // would expect a valid buffer
    if ((!i_flush) && (io_len > 0))
    {
        assert ( NULL != o_pData)
    }

    do
    {
        sbeFifoEntry_t l_data = {0};

        // Read Double word from the Upstream FIFO;
        // The DW data represents the first 32 bits of data word entry
        // followed by the status bits.

        // Bit 0-31    : Data
        // Bit 32      : Data valid flag
        // Bit 33      : EOT flag
        // Bit 34-63   : Status (2-31)
        // Valid : EOT
        //    1  : 0   -> data=message
        //    0  : 1   -> data=dummy_data of EOT operation
        //    0  : 0   -> data=dummy_data
        //    1  : 1   -> Not used

        l_rc = sbeUpFifoDeq ( reinterpret_cast<uint64_t*>(&l_data), i_type);
        if (l_rc)
        {
            // Error while dequeueing from upstream FIFO
            SBE_ERROR(SBE_FUNC "sbeUpFifoDeq failed, "
                "l_rc=[0x%08X], Fifo Type is:[%02X]", l_rc, i_type);
            // @TODO RTC via : 132295
            //       RC refactoring - reserve 3 bits in SBE RC for PCBPIB
            l_rc = SBE_SEC_FIFO_ACCESS_FAILURE;
            break;
        }

        SBE_DEBUG(SBE_FUNC"sbeUpFifoDeq - fifo_data:0x%08X, status:0x%08X, "
            "Fifo Type is:[%02X]", l_data.fifo_data, l_data.status, i_type);

        // If FIFO reset is requested
        if(l_data.statusOrReserved.req_upfifo_reset)
        {
            // @TODO via RTC : 126147
            //       Review reset loop flow in here.
            //       Received a FIFO reset request
            l_rc = SBE_FIFO_RESET_RECEIVED;
            break;
        }

        // if EOT flag is set, clear EOT and
        // set the RC accordingly
        if (l_data.statusOrReserved.eot_flag)
        {
            l_rc = sbeUpFifoAckEot(i_type);
            if (l_rc)
            {
                // Error while ack'ing EOT in upstream FIFO
                SBE_ERROR(SBE_FUNC"sbeUpFifoAckEot failed,"
                    "l_rc=[0x%08X], Fifo Type is:[%02X]", l_rc, i_type);

                // Collect FFDC and save off the l_rc
                l_rc = SBE_SEC_FIFO_ACCESS_FAILURE;
                break;
            }
            // Successfully Ack'ed the EOT in upstream FIFO
            if ( ((!i_isEotExpected) || (l_len != io_len))
                     && (!i_flush) )
            {
                SBE_ERROR(SBE_FUNC" Actual length:0x%08X Expected len:0x%08X, "
                    "Fifo Type is:[%02X]", l_len, io_len, i_type);
                if (l_len < io_len)
                {
                    // Unexpected EOT, got insufficient data
                    l_rc = SBE_SEC_UNEXPECTED_EOT_INSUFFICIENT_DATA ;
                }
                else
                {
                    // Unexpected EOT, got excess data
                    l_rc = SBE_SEC_UNEXPECTED_EOT_EXCESS_DATA ;
                }
            }
            break;
        }
        // Check valid flag
        if ( !l_data.statusOrReserved.valid_flag )
        {
            if( l_data.statusOrReserved.parity_err )
            {
                SBE_ERROR(SBE_FUNC"Parity error while reading FIFO."
                          " FIFO status: 0x%08X, Fifo Type is:[%02X]",
                          l_data.statusOrReserved.parity_err, i_type);
                l_rc = SBE_SEC_FIFO_PARITY_ERROR;
                break;
            }
            // We can reach here because FIFO was empty. We can not trust
            // empty flag because empty flag tells the status of FIFO after
            // operation not at the time of operation
            if( SBE::isSimicsRunning() )
            {
                // sleep if simics is running. Otherwise simics becomes
                // 99 % busy and fsp does not get a chance to do operation
                // over FIFO.
                pk_sleep(PK_MILLISECONDS(FIFO_WAIT_SLEEP_TIME));
            }
            continue;
        }

        if ((!i_flush) && (l_len < io_len))
        {
            o_pData[l_len] = l_data.fifo_data;
        }

        ++l_len;

    } while(i_flush || i_isEotExpected || (l_len < io_len));

    // Return the length of entries dequeued.
    io_len = l_len;
    return l_rc;

    #undef SBE_FUNC
}

//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
uint32_t sbeDownFifoEnq_mult (uint32_t        &io_len,
                              const uint32_t *i_pData,
                              sbeFifoType     i_type)
{
    #define SBE_FUNC " sbeDownFifoEnq_mult "
    SBE_DEBUG(SBE_FUNC " sbeDownFifoEnq_mult FIFO_TYPE[0x%02X]", i_type);
    uint32_t  l_rc   = SBE_SEC_OPERATION_SUCCESSFUL;
    uint32_t   l_len = 0;

    do
    {
        sbeDownFifoStatusReg_t l_status = {0};

        // Read the down stream FIFO status
        l_rc = sbeDownFifoGetStatus (reinterpret_cast<uint64_t *>(&l_status),
                                     i_type);
        if (l_rc)
        {
            // Error while reading downstream FIFO status
            SBE_ERROR(SBE_FUNC"sbeDownFifoGetStatus failed, l_rc=[0x%08X] "
                "Fifo Type is:[%02X]", l_rc, i_type);
            l_rc = SBE_SEC_FIFO_ACCESS_FAILURE;
            break;
        }

        // Check if there was a FIFO reset request from SP
        if (l_status.downfifo_status.req_upfifo_reset)
        {
            // @TODO via RTC : 126147
            //       Review reset loop flow in here.
            //       Received an upstream FIFO reset request
            SBE_ERROR(SBE_FUNC"Received reset request and the "
                "Fifo Type is:[%02X]", i_type);
            l_rc = SBE_FIFO_RESET_RECEIVED;
            break;
        }

        // Check if downstream FIFO is full
        if (l_status.downfifo_status.fifo_full)
        {
            // Downstream FIFO is full
            if( SBE::isSimicsRunning() )
            {
                // sleep if simics is running. Otherwise simics becomes
                // 99 % busy and fsp does not get a chance to do operation
                // over FIFO.
                pk_sleep(PK_MILLISECONDS(FIFO_WAIT_SLEEP_TIME));
            }
            continue;
        }

        // PIB write data format:
        // Bit 0 - 31  : Data
        // Bit 32 - 63 : Unused

        sbeFifoEntry_t l_data = {0};

        l_data.fifo_data   = *(i_pData+l_len);

        SBE_DEBUG(SBE_FUNC"Downstream fifo data entry[0x%08X]"
            "Fifo Type is:[%02X]", l_data.fifo_data, i_type);

        // Write the data into the downstream FIFO
        uint64_t * tp = reinterpret_cast<uint64_t*>(&l_data);
        l_rc = sbeDownFifoEnq ( *tp, i_type );
        if (l_rc)
        {
            SBE_ERROR(SBE_FUNC"sbeDownFifoEnq failed, l_rc[0x%08X], "
                "Fifo Type is:[%02X]", l_rc, i_type);
            // @TODO RTC via : 132295
            //       RC refactoring - reserve 3 bits in SBE RC for PCBPIB
            l_rc = SBE_SEC_FIFO_ACCESS_FAILURE;
            break;
        }

        ++l_len;

    } while(l_len<io_len);

    io_len = l_len;
    return l_rc;
    #undef SBE_FUNC
}

////////////////////////////////////////////////////////
////////////////////////////////////////////////////////
uint32_t sbeDownFifoSignalEot ( sbeFifoType i_type )
{
    uint32_t l_rc = 0;
    #define SBE_FUNC "sbeDownFifoSignalEot "
    SBE_ENTER(SBE_FUNC);
    SBE_DEBUG(SBE_FUNC "sbeDownFifoSignalEot FIFO_TYPE[0x%02X]", i_type);
    sbeDownFifoStatusReg_t l_status = {0};
    do
    {
        // Read the down stream FIFO status
        l_rc = sbeDownFifoGetStatus (reinterpret_cast<uint64_t *>(&l_status),
                                     i_type);
        if (l_rc)
        {
            // Error while reading downstream FIFO status
            SBE_ERROR(SBE_FUNC"sbeDownFifoGetStatus failed, l_rc=[0x%08X], "
                "Fifo Type is:[%02X]", l_rc, i_type);
            l_rc = SBE_SEC_FIFO_ACCESS_FAILURE;
            break;
        }

        // Check if downstream FIFO is full
        if (l_status.downfifo_status.fifo_full)
        {
            if( SBE::isSimicsRunning() )
            {
                // sleep if simics is running. Otherwise simics becomes
                // 99 % busy and fsp does not get a chance to do operation
                // over FIFO.
                pk_sleep(PK_MILLISECONDS(FIFO_WAIT_SLEEP_TIME));
            }
            continue;
        }

        l_rc = sbeDownFifoWriteEot(i_type);
        if(l_rc)
        {
            SBE_ERROR(SBE_FUNC " Write EOT failed, rc=[0x%08X] "
                "Fifo Type is:[%02X]", l_rc, i_type);
        }
        break;
    } while(1);


    SBE_EXIT(SBE_FUNC);
    return l_rc;
    #undef SBE_FUNC
}

uint32_t sbeDsSendRespHdr(const sbeRespGenHdr_t &i_hdr,
                          sbeResponseFfdc_t *i_ffdc, sbeFifoType i_type )
{
    #define SBE_FUNC "sbeDsSendRespHdr "
    uint32_t rc = SBE_SEC_OPERATION_SUCCESSFUL;
    do
    {
        uint32_t distance = 1; //initialise by 1 for entry count itself.
        uint32_t len = sizeof( i_hdr )/sizeof(uint32_t);
        // sbeDownFifoEnq_mult.
        rc = sbeDownFifoEnq_mult ( len, ( uint32_t *) &i_hdr, i_type);
        if (rc)
        {
            break;
        }
        distance += len;
        uint32_t dumpFieldsConfig = 0;

        // If no ffdc , exit;
        if( (i_ffdc != NULL) && (i_ffdc->getRc() != FAPI2_RC_SUCCESS))
        {
            SBE_ERROR( SBE_FUNC" FAPI RC:0x%08X", i_ffdc->getRc());
            dumpFieldsConfig |= SBE_FFDC_ALL_HW_DATA;
        }
        // If there is a SBE internal failure
        if((i_hdr.primaryStatus() != SBE_PRI_OPERATION_SUCCESSFUL) ||\
           (i_hdr.secondaryStatus() != SBE_SEC_OPERATION_SUCCESSFUL))
        {
            SBE_ERROR( SBE_FUNC" PriStatus:0x%08X SecStatus:0x%08X"
                " Fifo Type is:[%02X]", (uint32_t)i_hdr.primaryStatus(),
                (uint32_t)i_hdr.secondaryStatus(), i_type);
            dumpFieldsConfig |= SBE_FFDC_ALL_PLAT_DATA;
        }
        rc = sendFFDCOverFIFO(dumpFieldsConfig, len, true);
        CHECK_SBE_RC_AND_BREAK_IF_NOT_SUCCESS(rc);

        distance += len;
        rc = sbeDownFifoEnq_mult ( len, &distance, i_type);
        if (rc)
        {
            break;
        }

    }while(0);
    SBE_EXIT(SBE_FUNC);
    return rc;
    #undef SBE_FUNC
}
////////////////////////////////////////////////////////
////////////////////////////////////////////////////////
sbeFifoType sbeFifoGetSource (bool reset)
{
    #define SBE_FUNC "sbeFifoGetSource"
    uint32_t rc = SBE_SEC_OPERATION_SUCCESSFUL;
    sbeFifoType type = SBE_FIFO_UNKNOWN;

    do
    {
        // First check on FIFO, from status register
        sbeUpFifoStatusReg_t status = {0};
        rc = sbeUpFifoGetStatus (reinterpret_cast<uint64_t *>(&status),
                                 SBE_FIFO);
        if (rc)
        {
            SBE_ERROR (SBE_FUNC "Reading fifo status, rc: 0x%08X", rc);
            //rc = SBE_SEC_FIFO_ACCESS_FAILURE;
            // This is a fatal error
            pk_halt ();
            break;
        }

        if ((!reset && !(status.upfifo_status.fifo_empty)) ||
            (reset && status.upfifo_status.req_upfifo_reset))
        {
            type = SBE_FIFO;
            SBE_INFO (SBE_FUNC "0x%04X on FIFO %d",
                      (reset)? 0xDEAD:0xDA7A, type);
            break;
        }

        // SBE_HB_FIFO:
	rc = sbeUpFifoGetStatus (reinterpret_cast<uint64_t *>(&status),
                                 SBE_HB_FIFO);
        if (rc)
        {
            SBE_ERROR (SBE_FUNC "Reading fifo status, rc: 0x%08X", rc);
            //rc = SBE_SEC_FIFO_ACCESS_FAILURE;
            // This is a fatal error
            pk_halt ();
            break;
        }

        if ((!reset && !(status.upfifo_status.fifo_empty)) ||
            (reset && status.upfifo_status.req_upfifo_reset))
        {
            type = SBE_HB_FIFO;
            SBE_INFO (SBE_FUNC "0x%04X on FIFO %d",
                      (reset)? 0xDEAD:0xDA7A, type);
            break;
        }

        // Check pending interrupt on Pipes
        type = sbePipeGetSource ();

        // Fatal error is no interrupt source was found
        assert(type != SBE_FIFO_UNKNOWN);
    }   while (0);

    SBE_EXIT(SBE_FUNC " FIFO Type - %d", type);
    return type;
    #undef SBE_FUNC
}

sbeInterfaceSrc_t sbeFifoGetInstSource (sbeFifoType upFifoType, bool reset)
{
    if (!reset)
    {
        switch(upFifoType)
        {
            case SBE_FIFO:     return SBE_INTERFACE_FIFO;
            case SBE_HB_FIFO:  return SBE_INTERFACE_HFIFO;
            case SBE_PIPE1:    return SBE_INTERFACE_PIPE1;
            case SBE_PIPE2:    return SBE_INTERFACE_PIPE2;
            case SBE_PIPE3:    return SBE_INTERFACE_PIPE3;
            case SBE_PIPE4:    return SBE_INTERFACE_PIPE4;
            case SBE_PIPE5:    return SBE_INTERFACE_PIPE5;
            case SBE_PIPE6:    return SBE_INTERFACE_PIPE6;
            case SBE_PIPE7:    return SBE_INTERFACE_PIPE7;
            case SBE_PIPE8:    return SBE_INTERFACE_PIPE8;
            case SBE_FIFO_UNKNOWN:    return SBE_INTERFACE_UNKNOWN;
       }
    }
    else
    {
        switch(upFifoType)
        {
            case SBE_FIFO:     return SBE_INTERFACE_FIFO_RESET;
            case SBE_HB_FIFO:  return SBE_INTERFACE_HFIFO_RESET;
            case SBE_PIPE1:    return SBE_INTERFACE_PIPE1_RESET;
            case SBE_PIPE2:    return SBE_INTERFACE_PIPE2_RESET;
            case SBE_PIPE3:    return SBE_INTERFACE_PIPE3_RESET;
            case SBE_PIPE4:    return SBE_INTERFACE_PIPE4_RESET;
            case SBE_PIPE5:    return SBE_INTERFACE_PIPE5_RESET;
            case SBE_PIPE6:    return SBE_INTERFACE_PIPE6_RESET;
            case SBE_PIPE7:    return SBE_INTERFACE_PIPE7_RESET;
            case SBE_PIPE8:    return SBE_INTERFACE_PIPE8_RESET;
            case SBE_FIFO_UNKNOWN:    return SBE_INTERFACE_UNKNOWN;
       }
    }

    return SBE_INTERFACE_UNKNOWN;
}

sbeFifoType sbePipeGetSource (void)
{
    #define SBE_FUNC "sbePipeGetSource "
    uint64_t accessCtrlCfg = 0;
    sbeFifoType type = SBE_FIFO_UNKNOWN;

    uint32_t rc = getscom_abs(SBE_PIPE_REG_ACCESS_CTRL, &accessCtrlCfg);
    if (rc)
    {
        SBE_ERROR (SBE_FUNC "Error reading Pipe Access Ctrl: 0x08X", rc);
        // rc = SBE_SEC_FIFO_ACCESS_FAILURE;
        // This is a fatal error
        pk_halt ();
    }
    else
    {
        rc = SBE_SEC_GENERIC_FAILURE_IN_EXECUTION;

        for (uint8_t i=SBE_PIPE1; i<= SBE_PIPE8; i++)
        {   // For Pipes, look at access control reg to find the source
            sbePipeAccessFlags_t cfg = {0};
            cfg.flags = (SBE_PIPE_GET_CFG_BYTE (i, accessCtrlCfg));

            // ctrlID field of read end determines intr routing if RIE is set,
            // independent of RUID setting. So,below flag being set implies SBE
            // got either the new data available or reset request interrupt
            if (cfg.rd_intr_pending)
            {
                rc = SBE_SEC_OPERATION_SUCCESSFUL;
                type = static_cast<sbeFifoType> (i);
                SBE_INFO (SBE_FUNC "Intr received on PIPE# %d", i);
                break;
            }
        }

        if (rc != SBE_SEC_OPERATION_SUCCESSFUL)
        {   // Note: The ctrlID field of read end determines interrupt routing
            // if interrupt enable (RIE) is set, independent of use ctrlID(RUID)
            // setting. Since we got an interrupt (which is not from FIFO, this
            // is an unexpected HW bug .. catch it as soon as detected
            SBE_ERROR (SBE_FUNC "No interrupts pending on any pipe!");
            pk_halt ();
        }
    }

    return type;
    #undef SBE_FUNC
}

uint32_t sbeHandleDsPipeCnfg (const sbeFifoType  i_usPipe)
{
    #define SBE_FUNC " sbeHandleDsPipeCnfg "

    uint32_t rc = SBE_SEC_OPERATION_SUCCESSFUL;
    uint64_t accessCtrl = 0;
    uint64_t ctlrId = 0;

    do
    {
        if ((i_usPipe < SBE_PIPE1) || (i_usPipe > SBE_PIPE7))
        {
            rc = SBE_SEC_INVALID_PIPE;
            SBE_ERROR (SBE_FUNC "Invalid Upstream Pipe %d", i_usPipe);
            break;
        }

        uint8_t usPipe = i_usPipe;
        // Respond on the next (consecutive pipe)
        uint8_t dsPipe = usPipe+1;

        rc = getscom_abs (SBE_PIPE_REG_ACCESS_CTRL, &accessCtrl);
        if (rc)
        {
            SBE_ERROR (SBE_FUNC "Error reading Pipe Access Ctrl: 0x08X", rc);
            rc = SBE_SEC_FIFO_ACCESS_FAILURE;
            pk_halt (); // fatal error
            break;
        }

        rc = getscom_abs (SBE_PIPE_REG_CTLR_ID, &ctlrId);
        if (rc)
        {
            SBE_ERROR (SBE_FUNC "Error reading Pipe CtlrId: 0x08X", rc);
            rc = SBE_SEC_FIFO_ACCESS_FAILURE;
            pk_halt (); // fatal error
            break;
        }

        // The fact that SBE got an interrupt implies upstream pipe setup is ok
        // Check the downstream pipe setup and config it if not done by caller
        sbePipeAccessFlags_t dsPipeAccess = {0};
        dsPipeAccess.flags = SBE_PIPE_GET_CFG_BYTE(dsPipe, accessCtrl);

        SBE_INFO (SBE_FUNC "Pipe Access Ctrl: 0x%08X%08X",
            SHIFT_RIGHT(accessCtrl, 32), MASK_ZERO_H32B_UINT64(accessCtrl));

        sbePipeCtlrId_t dsPipeCtlrId = {0}; // downstream
        sbePipeCtlrId_t usPipeCtlrId = {0}; // upstream
        bool updateDsConfig = false;

        dsPipeCtlrId.byte = SBE_PIPE_GET_CFG_BYTE(dsPipe, ctlrId);
        usPipeCtlrId.byte = SBE_PIPE_GET_CFG_BYTE(i_usPipe, ctlrId);

        SBE_INFO (SBE_FUNC "Pipe Ctlr IDs: 0x%08X%08X",
                  SHIFT_RIGHT(ctlrId, 32), MASK_ZERO_H32B_UINT64(ctlrId));

        // Check and config downstream pipe WRITE config if not done
        if ( !( (dsPipeAccess.wr_open) &&
                  ( !dsPipeAccess.wr_use_ctlr_id ||
                    (dsPipeAccess.wr_use_ctlr_id  &&
                      (dsPipeCtlrId.writeend == PIB_CTLR_SPPE)
                    )
                  )
              )
          )
        {
            updateDsConfig = true;
            dsPipeAccess.wr_open = 1;
            dsPipeAccess.wr_use_ctlr_id = 1;
            dsPipeCtlrId.writeend = PIB_CTLR_SPPE;
        }
        // Check and config downstream pipe READ config if not done
        if ( !( (dsPipeAccess.rd_open) &&
                  ( !dsPipeAccess.rd_use_ctlr_id ||
                    (dsPipeAccess.rd_use_ctlr_id  &&
                      (dsPipeCtlrId.readend == usPipeCtlrId.writeend)
                    )
                  )
              )
          )
        {
            updateDsConfig = true;
            dsPipeAccess.rd_open = 1;
            dsPipeAccess.rd_use_ctlr_id = 1;
            dsPipeCtlrId.readend = usPipeCtlrId.writeend;
            // no interrupt routing for non-SBE PIB controllers, they poll
            // for P11 SPPE-SBE this could be enabled
        }

        if (updateDsConfig)
        {
            uint64_t data64 = SBE_PIPE_SET_CFG_BYTE (dsPipe,
                                                        dsPipeAccess.flags );
            SBE_INFO (SBE_FUNC "Updating DS pipe %d access flags: 0x%08X%08X",
                      dsPipe,
                      SHIFT_RIGHT(data64, 32), MASK_ZERO_H32B_UINT64(data64));
            rc = putscom_abs (SBE_PIPE_REG_ACCESS_CTRL_SET, data64);
            if (rc)
            {
                SBE_ERROR (SBE_FUNC "Err writing Ctlr Id Reg: 0x08X", rc);
                rc = SBE_SEC_FIFO_ACCESS_FAILURE;
                pk_halt (); // this is a fatal error
                break;
            }

            data64 = SBE_PIPE_SET_CFG_BYTE ( dsPipe,
                                                dsPipeCtlrId.byte);
            SBE_INFO (SBE_FUNC "Updating DS pipe %d ctlr id: 0x%08X%08X",
                      dsPipe,
                      SHIFT_RIGHT(data64, 32), MASK_ZERO_H32B_UINT64(data64));
            rc = putscom_abs (SBE_PIPE_REG_CTLR_ID_SET, data64);
            if (rc)
            {
                SBE_ERROR (SBE_FUNC "Err writing Access Ctrl Reg: 0x08X", rc);
                rc = SBE_SEC_FIFO_ACCESS_FAILURE;
                pk_halt (); // this is fatal error
                break;
            }
        }
    }   while (0);

    return rc;
    #undef SBE_FUNC
}
