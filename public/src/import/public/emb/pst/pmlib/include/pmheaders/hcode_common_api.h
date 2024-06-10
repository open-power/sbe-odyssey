/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/emb/pst/pmlib/include/pmheaders/hcode_common_api.h $ */
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
/// @file  hcode_common_api.h
/// @brief Common structures used for IPC communication
///
// *HWP HW Owner        : Prasad Bg Ranganath <prasadbgr@in.ibm.com>
// *HWP HW Owner        : Greg Still <stillgs@us.ibm.com>
// *HWP Team            : PM
// *HWP Level           : 3
// *HWP Consumed by     : DCE:OCE:PCE:XCE:PGPE:XGPE:OCC

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/// @addtogroup pm_hcode_lib
/// @{

///
/// Hcode IPC Return Codes
///
enum IPC_MESSAGE_RCS
{
    IPC_RC_ERRLOG_FAILED            = 0x00000100,
    IPC_RC_ERRLOG_NULL_POINTER      = 0x00000110,
    IPC_RC_DOPT_OFLD_FAILED         = 0x00000200,
    IPC_RC_DOPT_OFLD_NULL_POINTER   = 0x00000210,
    IPC_RC_PMSUSPEND_FAILED         = 0x00000300,
};

///
/// Base contents
///
typedef struct
{
    int32_t   rc;                   ///< Return code
} ipcmsg_base_t;

///
/// Error Logging
///
typedef struct
{
    ipcmsg_base_t   msg_cb;         ///< Message control block
    void*           errl_ptr;       ///< Error log pointer
} ipcmsg_errlog_t;

///
/// Deep Operational Trace Offloading
///
typedef struct
{
    ipcmsg_base_t   msg_cb;         ///< Message control block
    void*           doptrc_ptr;     ///< Deep operational trace pointer
} ipcmsg_doptrace_ofl_t;

///
/// PM Suspend
///
typedef struct
{
    ipcmsg_base_t   msg_cb;         ///< Message control block
} ipcmsg_pmsuspend_t;


#ifdef __cplusplus
} // end extern C
#endif

/// @} end addtogroup