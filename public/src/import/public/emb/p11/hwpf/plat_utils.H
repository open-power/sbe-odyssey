/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/emb/p11/hwpf/plat_utils.H $          */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2022,2023                        */
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
/**
 * @file plat_utils.H
 * @brief platform definitions for fapi2 targets
 */

#ifndef __FAPI2_PLAT_UTILS__
#define __FAPI2_PLAT_UTILS__

#include <stdint.h>
#include <plat_types.H>
#include <fapi2_hw_access.H>

namespace fapi2
{

ReturnCode plat_putRingUtils( const void* i_target,
                              const void* i_scanImage,
                              const uint32_t i_modifiedRingAddress,
                              const RingMode i_ringMode);

ReturnCode mod_multicast_setup(
    const fapi2::Target<fapi2::TARGET_TYPE_ANY_POZ_CHIP>&,
    uint8_t i_group_id,
    uint64_t i_chiplets,
    TargetState);


#ifdef __PPE_QME
    static const PlatformType PLATFORM = PLAT_QME;
#else
    static const PlatformType PLATFORM = PLAT_SBE;
#endif

//PLAT NOTE//
//like we ever going to need such special wakeup injection routine
//as this platform is the processer of the special wakeup

//F2//template<TargetType T>
//F2//inline ReturnCode platSpecialWakeup( const Target<T>& i_target, bool i_enable)
//F2//{
//F2//    static_cast<void>(i_enable);
//F2//    static_cast<void>(i_target);
//F2//
//F2//    if(is_same<T, TARGET_TYPE_COMPUTE_CHIP>() )
//F2//    {
//F2//        FAPI_INF("do proc special wakeup stuff");
//F2//    }
//F2//    else if(is_same<T, TARGET_TYPE_CORE>() )
//F2//    {
//F2//        FAPI_INF("do core special wakeup stuff");
//F2//    }
//F2//    else if(is_same<T, TARGET_TYPE_EX>() )
//F2//    {
//F2//        FAPI_INF("do ex speical wakeup stuff");
//F2//    }
//F2//    else if(is_same<T, TARGET_TYPE_EQ>() )
//F2//    {
//F2//        FAPI_INF("do eq special wakeup stuff");
//F2//    }
//F2//    else
//F2//    {
//F2//        FAPI_INF("should never get here");
//F2//    }
//F2//
//F2//    return FAPI2_RC_SUCCESS;
//F2//}

} // fapi2 namespace

#ifdef __ASSEMBLER__

    #ifndef ULL
        #define ULL(x) x
    #endif

#else

    #ifndef ULL
        #define ULL(x) x##ull

    #endif

#endif // __ASSEMBLER

#ifdef _BIG_ENDIAN

    #define revle16(x) x
    #define revle32(x) x
    #define revle64(x) x

#else

    uint16_t revle16(uint16_t i_x);
    uint32_t revle32(uint32_t i_x);
    uint64_t revle64(uint64_t i_x);

#endif

#endif
