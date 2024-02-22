/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/emb/pst/kernels/ppe/powmanlib/hcode_common_api.h $ */
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


#ifndef __HCODE_COMMON_API_H__
#define __HCODE_COMMON_API_H__


#ifdef __cplusplus
extern "C" {
#endif

//
// Hcode IPC Return Codes
//
enum IPC_MESSAGE_RCS
{
    IPC_RC_ERRLOG_FAILED            = 0x00000100,
    IPC_RC_ERRLOG_NULL_POINTER      = 0x00000110,
    IPC_RC_DOPT_OFLD_FAILED         = 0x00000200,
    IPC_RC_DOPT_OFLD_NULL_POINTER   = 0x00000210,
    IPC_RC_PMSUSPEND_FAILED         = 0x00000300,
};


typedef struct ipcmsg_base
{
    int32_t   rc;
} ipcmsg_base_t;

//
// Error Logging
//

typedef struct ipcmsg_errlog
{
    ipcmsg_base_t   msg_cb;
    void*           errl_ptr;
} ipcmsg_errlog_t;

//
// Deep Operational Trace Offloading
//
typedef struct ipcmsg_doptrace_ofl
{
    ipcmsg_base_t   msg_cb;
    void*           doptrc_ptr;
} ipcmsg_doptrace_ofl_t;

//
// PM Suspend
//
typedef struct ipcmsg_pmsuspend
{
    ipcmsg_base_t   msg_cb;
} ipcmsg_pmsuspend_t;


#ifdef __cplusplus
} // end extern C
#endif

#endif    /* __HCODE_COMMON_API_H__ */
