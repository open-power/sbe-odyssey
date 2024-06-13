/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/emb/p11/hwpf/assert.h $              */
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
/*
 *  @file assert.h
 *
 *  @brief This file contains the assert macro for QME
 */

//PLAT NOTE//headers for traces and halts in pool.C
extern "C"
{
#include "iota.h"
#include "iota_trace.h"
#include "trac_interface.h"
}
#ifndef QME_ASSERT_H
#define QME_ASSERT_H

//        PKTRACE("assertion failed: "#expr);
//        iota_halt();

#ifndef NDEBUG
#define assert(expr) \
    if( !(expr ))  \
    {  \
        IOTA_PANIC(0);\
    } \

#else
#define assert(expr)
#endif  //NDEBUG

#endif // QME_ASSERT_H
