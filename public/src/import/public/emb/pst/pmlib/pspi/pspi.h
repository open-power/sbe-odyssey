/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/emb/pst/pmlib/pspi/pspi.h $          */
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
#ifndef __PSPI_H__
#define __PSPI_H__

#include "pspi_util.h"
#include "pspi_regs.h"

#ifdef __SSX__   //This is for the 405
    #include <common_types.h>
    #include <stddef.h>
    //CMO-20230301: To get traceout working requires including <trac.h> and define PSPI_TRACE
    //              TRAC_INFO. However, there's a problem with the trace_adal_hash() function
    //              which, as I understand it, is being built during the build of tracepp. So
    //              somehow this needs to be weaved into the build of the libpspi.a lib.
    //              So, since we're not tracing out from any of the other libraries in OCC,
    //              namely ./pmlib/ipc, ./lib/ppc405lib, ./lib/occlib and ./lib/commonlib, I
    //              have null'ed out traceout when building PSPI lib for the OCC.
    #define NO_TRACE(...)
    #define PSPI_TRACE     NO_TRACE
    #define PSPI_TRACE_ERR NO_TRACE
#else            //This is for the PPEs
    #include <ppe42_string.h>
    #include <iota_trace.h>
    #define PSPI_TRACE     PK_TRACE
    #define PSPI_TRACE_ERR PK_TRACE_ERR
#endif

enum PSPI_TARGETS
{
    PSPI_MAX_TAPS  = 8,
    PSPI_TAP0      = 0x0,
    PSPI_TAP1      = 0x1,
    PSPI_TAP2      = 0x2,
    PSPI_TAP3      = 0x3,
    PSPI_TAP4      = 0x4,
    PSPI_TAP5      = 0x5,
    PSPI_TAP6      = 0x6,
    PSPI_TAP7      = 0x7,
    PSPI_HUB       = 0x0,
    PSPI_BROADCAST = 0x0  // Internal use only
};

enum PSPI_DOORBELLS
{
    PSPI_DB0     = 0x0,
    PSPI_DB1     = 0x1,
    PSPI_DB2     = 0x2,
    PSPI_DB3     = 0x3,
    PSPI_MSG_DB0 = 0x0,
    PSPI_MSG_DB1 = 0x1,
    PSPI_MSG_DB2 = 0x2,
    PSPI_MSG_DB3 = 0x3
};

enum PSPI_VCHANNELS
{
    PSPI_VC0 = 0x0,
    PSPI_VC1 = 0x1,
    PSPI_VC2 = 0x2,
    PSPI_VC3 = 0x3
};

enum PSPI_REG_OPERATION
{
    PSPI_REG_OP_RD  = 0x0, // Read
    PSPI_REG_OP_CLR = 0x1, // Clear
    PSPI_REG_OP_OR  = 0x2, // OR (set)
};

enum PSPI_CHIP_TYPE
{
    PSPI_CT_HUB  = 0x1, // Hub chip
    PSPI_CT_TAP  = 0x2, // Tap chip
};

enum PSPI_RETURN_CODES
{
    PSPI_RC_SUCCESS                = 0x0,
    PSPI_RC_CODE_BUG               = 0x1,
    PSPI_RC_LINK_DISABLED          = 0x2,
    PSPI_RC_UNSUPPORTED_CHIP_TYPE  = 0x3,
};

#define VC_BUFFER_CONFIG_DEFAULT 0xB000FF00

typedef union vc_buffer_config
{

    uint64_t value;
    struct
    {
        uint32_t upper;
        uint32_t lower;
    } words;
    struct
    {
        uint64_t buffer_enable         : 1; //0 default 1
        uint64_t ping_pong_mode        : 1; //1 default 0
        uint64_t broadcast_enable      : 1; //2 default 1
        uint64_t bandwidth_saver       : 1; //3 default 1
        uint64_t lock_disable          : 1; //4 default 0
        uint64_t auto_hw_lock_disable  : 1; //5 default 0
        uint64_t oci_blocking_disable  : 1; //6 default 0
        uint64_t write_lock_disable    : 1; //7 default 0
        uint64_t read_lock_disable     : 1; //8 default 0
        uint64_t reserved9to15         : 7; //9-15
        uint64_t timeout_value         : 8; //16-23 default 0xFF
        uint64_t reserved24to27        : 4; //24-27
        uint64_t buffer_size           : 13;//28-40
        uint64_t reserved41to42        : 2; //41-42
        uint64_t buffer_bar            : 18;//43-60
        uint64_t reserved61to63        : 3; //61-63
    } fields;
} vc_buffer_config_t;

typedef struct pspi_buffer
{
    uint32_t current;
    uint32_t new_data[2];
    uint32_t write_lock[2];
    uint32_t write_ptr[2];
    uint32_t read_lock[2];
    uint32_t read_ptr[2];
    vc_buffer_config_t config;
} pspi_buffer_t;

void     pspi_vc0_config(uint32_t, uint32_t);
uint32_t pspi_vc0_error(uint32_t);
uint32_t pspi_vc_status(uint32_t, uint32_t, uint32_t);
uint32_t pspi_summary(uint32_t, uint32_t, uint32_t);
void     pspi_summary_clear(uint32_t, uint32_t, uint32_t);

void pspi_doorbell_receive(uint32_t, uint32_t, uint32_t*);
void pspi_doorbell_send(uint32_t, uint32_t, uint32_t);
void pspi_doorbell_sendall(uint32_t, uint32_t, uint32_t);

void pspi_msg_doorbell_receive(uint32_t, uint32_t, uint64_t*);
void pspi_msg_doorbell_send(uint32_t, uint32_t, uint64_t);
void pspi_msg_doorbell_sendall(uint32_t, uint32_t, uint64_t);

void pspi_pcom_get(uint32_t, uint32_t, uint64_t*);
void pspi_pcom_put(uint32_t, uint32_t, uint64_t);

void pspi_pcom_getall(uint32_t, uint64_t*);
void pspi_pcom_putall(uint32_t, uint64_t);

void pspi_vc_buffer_init(pspi_buffer_t*, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t);
void pspi_vc_buffer_config(pspi_buffer_t*, uint32_t, uint32_t, uint32_t);

uint32_t pspi_vc_buffer_write_setup(pspi_buffer_t*);
void     pspi_vc_buffer_write_fill(pspi_buffer_t*, uint32_t);
void     pspi_vc_buffer_write_done(pspi_buffer_t*);

void pspi_vc_put(uint32_t, uint32_t, uint32_t, uint64_t);
void pspi_vc_get(uint32_t, uint32_t, uint32_t, uint64_t*);

void     pspi_vc_unicast_go(uint32_t, uint32_t, uint32_t);
void     pspi_vc_broadcast_go(uint32_t, uint32_t, uint32_t);
uint32_t pspi_vc_in_progress(uint32_t, uint32_t, uint32_t);

void     pspi_link_config(uint32_t, uint32_t);
uint32_t pspi_link_status_common(uint32_t);
uint32_t pspi_link_status_fsm(uint32_t, uint32_t);
void     pspi_link_err_inj(uint32_t, uint32_t, uint32_t);
void     pspi_link_ping(uint32_t, uint32_t);
void     pspi_link_ping_write(uint32_t, uint32_t, uint64_t);
uint32_t pspi_link_ping_read(uint32_t, uint64_t*);

int  pspi_link_init_enable(uint32_t, uint32_t, uint32_t*);
int  pspi_link_init_verif(uint32_t, uint32_t, uint32_t*);
void pspi_link_init(uint32_t, uint32_t);

void pspi_test();


#endif // __PSPI_H__
