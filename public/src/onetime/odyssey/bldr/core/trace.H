/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/onetime/odyssey/bldr/core/trace.H $                */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2015,2022                        */
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
#ifndef __TRACE_H
#define __TRACE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "pk_api.h"
#include "trac_interface.h"

#ifdef __cplusplus
}
#endif

#define  OBLDR_ENTER_MRK ">>"
#define  OBLDR_EXIT_MRK "<<"
#define  OBLDR_ERR_MRK "E>"
#define  OBLDR_INF_MRK "I>"
#define  OBLDR_DEBUG_MRK "D>"

#define OBLDR_ERROR(args...)
#define OBLDR_INFO(args...)
#define OBLDR_ENTER(args...)
#define OBLDR_EXIT(args...)
#define OBLDR_DEBUG(args...)

//Levels of logging
// 0 - No tracing
// 1 - Error
// 2 - Error, info
// 3 - Error, info, entry/exit
// 4 - Error, info, entry/exit, debug
#if (OBLDR_TRACE_LEVEL >= 1)
#undef OBLDR_ERROR
#define OBLDR_ERROR(args...) PK_TRACE(OBLDR_ERR_MRK"" args)
#endif
#if (OBLDR_TRACE_LEVEL >= 2)
#undef OBLDR_INFO
#define OBLDR_INFO(args...) PK_TRACE(OBLDR_INF_MRK"" args)
#endif
#if (OBLDR_TRACE_LEVEL >= 3)
#undef OBLDR_ENTER
#undef OBLDR_EXIT
#define OBLDR_ENTER(args...) PK_TRACE(OBLDR_ENTER_MRK"" args)
#define OBLDR_EXIT(args...) PK_TRACE(OBLDR_EXIT_MRK"" args)
#endif
#if (OBLDR_TRACE_LEVEL >= 4)
#undef OBLDR_DEBUG
#define OBLDR_DEBUG(args...) PK_TRACE(OBLDR_DEBUG_MRK"" args)
#endif
#define OBLDR_TRACE(args...) PK_TRACE(args)

#endif // __TRACE_H
