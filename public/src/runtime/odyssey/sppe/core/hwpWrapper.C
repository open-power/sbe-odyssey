/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/odyssey/sppe/core/hwpWrapper.C $           */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2023                             */
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
#include "hwpWrapper.H"
#include "istep_includes.H"

using namespace fapi2;

//----------------------------------------------------------------------------

ReturnCode istepWithOcmb( voidfuncptr_t i_hwp)
{
    #define SBE_FUNC " istepWithOcmb "
    SBE_ENTER(SBE_FUNC);
    ReturnCode rc = FAPI2_RC_SUCCESS;
    Target<TARGET_TYPE_OCMB_CHIP > l_ocmb_chip = g_platTarget->plat_getChipTarget();
    assert( NULL != i_hwp );
    SBE_EXEC_HWP(rc, reinterpret_cast<sbeIstepHwpOcmb_t>( i_hwp ), l_ocmb_chip);
    SBE_EXIT(SBE_FUNC);
    return rc;
    #undef SBE_FUNC
}

ReturnCode istepBistWithOcmb( voidfuncptr_t i_hwp)
{
    #define SBE_FUNC " istepBistWithOcmb "
    SBE_ENTER(SBE_FUNC);
    ReturnCode rc = FAPI2_RC_SUCCESS;
    Target<TARGET_TYPE_OCMB_CHIP > l_ocmb_chip = g_platTarget->plat_getChipTarget();
    assert( NULL != i_hwp );
    if (fapi2::ATTR::TARGET_TYPE_OCMB_CHIP::ATTR_ENABLE_ABIST ||
        fapi2::ATTR::TARGET_TYPE_OCMB_CHIP::ATTR_ENABLE_LBIST)
    {
        SBE_EXEC_HWP(rc, reinterpret_cast<sbeIstepHwpOcmb_t>( i_hwp ), l_ocmb_chip);
    }
    else
    {
        SBE_INFO("Skipping istep since ATTR_ENABLE_ABIST nor ATTR_ENABLE_LBIST not set");
    }
    SBE_EXIT(SBE_FUNC);
    return rc;
    #undef SBE_FUNC
}

ReturnCode istepAbistWithOcmb( voidfuncptr_t i_hwp)
{
    #define SBE_FUNC " istepAbistWithOcmb "
    SBE_ENTER(SBE_FUNC);
    ReturnCode rc = FAPI2_RC_SUCCESS;
    Target<TARGET_TYPE_OCMB_CHIP > l_ocmb_chip = g_platTarget->plat_getChipTarget();
    assert( NULL != i_hwp );
    if(fapi2::ATTR::TARGET_TYPE_OCMB_CHIP::ATTR_ENABLE_ABIST)
    {
        SBE_EXEC_HWP(rc, reinterpret_cast<sbeIstepHwpOcmb_t>( i_hwp ), l_ocmb_chip);
    }
    else
    {
        SBE_INFO("Skipping istep since ATTR_ENABLE_ABIST not set");
    }
    SBE_EXIT(SBE_FUNC);
    return rc;
    #undef SBE_FUNC
}

ReturnCode istepLbistWithOcmb( voidfuncptr_t i_hwp)
{
    #define SBE_FUNC " istepLbistWithOcmb "
    SBE_ENTER(SBE_FUNC);
    ReturnCode rc = FAPI2_RC_SUCCESS;
    Target<TARGET_TYPE_OCMB_CHIP > l_ocmb_chip = g_platTarget->plat_getChipTarget();
    assert( NULL != i_hwp );
    if(fapi2::ATTR::TARGET_TYPE_OCMB_CHIP::ATTR_ENABLE_LBIST)
    {
        SBE_EXEC_HWP(rc, reinterpret_cast<sbeIstepHwpOcmb_t>( i_hwp ), l_ocmb_chip);
    }
    else
    {
        SBE_INFO("Skipping istep since ATTR_ENABLE_LBIST not set");
    }
    SBE_EXIT(SBE_FUNC);
    return rc;
    #undef SBE_FUNC
}
