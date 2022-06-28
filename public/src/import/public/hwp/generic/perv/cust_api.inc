/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/hwp/generic/perv/cust_api.inc $      */
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
// This is the API for calls to cust.fsm.
// It's basically just a bunch of well-known entry points into cust.fsm.

#include "cmdtable_hwp_errors.H"

// -----------------------------------------------------------------------
// Required APIs. If called from main.fsm but not implemented in cust.fsm
// these will result in a fatal error.
// These are supposed to be used for pre-GA APIs that must be implemented
// on all modules.
// -----------------------------------------------------------------------

// Scan in PIB repairs
#define CUST_SCAN_PIB_REPR 0
//#define CUST_REQUIRED_API_1 1
//#define CUST_REQUIRED_API_2 2
//#define CUST_REQUIRED_API_3 3
//#define CUST_REQUIRED_API_4 4
//#define CUST_REQUIRED_API_5 5
//#define CUST_REQUIRED_API_6 6
//#define CUST_REQUIRED_API_7 7


// -----------------------------------------------------------------------
// Optional APIs. If called from main.fsm but not implemented in cust.fsm
// these will silently return and do nothing.
// These are supposed to be used for post-GA fixes which may not be
// implemented on all modules.
// -----------------------------------------------------------------------

//#define CUST_OPTIONAL_API_0 8
//#define CUST_OPTIONAL_API_1 9
//#define CUST_OPTIONAL_API_2 10
//#define CUST_OPTIONAL_API_3 11
//#define CUST_OPTIONAL_API_4 12
//#define CUST_OPTIONAL_API_5 13
//#define CUST_OPTIONAL_API_6 14
//#define CUST_OPTIONAL_API_7 15


// -----------------------------------------------------------------------
// Dispatcher macro for cust.fsm
// Accepts labels for all supported APIs, and reserves space for
// future expansion.
// -----------------------------------------------------------------------

#define CUST_DISPATCHER(scan_pib_repr)   \
        b scan_pib_repr;                 \
        fail ERR_UNSUPPORTED_CUST_API_1; \
        fail ERR_UNSUPPORTED_CUST_API_2; \
        fail ERR_UNSUPPORTED_CUST_API_3; \
        fail ERR_UNSUPPORTED_CUST_API_4; \
        fail ERR_UNSUPPORTED_CUST_API_5; \
        fail ERR_UNSUPPORTED_CUST_API_6; \
        fail ERR_UNSUPPORTED_CUST_API_7; \
        return; \
        return; \
        return; \
        return; \
        return; \
        return; \
        return; \
        return
