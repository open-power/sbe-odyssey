/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/odyssey/sppe/hwpf/plat_ring_utils.C $      */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2023                             */
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

#include "plat_ring_utils.H"
#include "target.H"
#include "hw_access.H"
#include "ppe42_scom.h"
#include "hwp_return_codes.H"
#include "plat_error_scope.H"
#include "hwp_ffdc_classes.H"
#include "error_info_defs.H"
#include "ffdc.H"
#include <error_info.H>
#include "plat_target_base.H"
#include "sbe_sp_intf.H"
#include "poz_putRingUtils.H"

namespace fapi2
{

struct restoreOpcgRegisters g_opcgData;

ReturnCode plat_putringutil( const void *i_target,
                             const void *i_scanImage,
                             const uint32_t i_modifiedRingAddress,
                             const fapi2::RingMode i_ringMode)

{
    fapi2::ReturnCode l_rc = FAPI2_RC_SUCCESS;
    const fapi2::Target < fapi2::TARGET_TYPE_PERV | fapi2::TARGET_TYPE_MULTICAST>
                         target(*(plat_target_handle_t*)i_target);
    FAPI_INF("plat_putringutil:  modifiedRingAddress[%08X], ringMode[%08X], "
             "target[0x%08X]", i_modifiedRingAddress, i_ringMode, target.get());

    l_rc = poz_putRingUtils( target, (uint8_t*)i_scanImage,
                             i_modifiedRingAddress, i_ringMode);
    if( l_rc != FAPI2_RC_SUCCESS )
    {
        FAPI_ERR("plat_putringutil failed with RC[%08X]", l_rc);
    }
    return l_rc;
}
ReturnCode getRing_setup(const uint32_t i_ringAddress,
                             const RingMode i_ringMode)
{
    fapi2::ReturnCode l_rc = FAPI2_RC_SUCCESS;

    uint64_t l_scanRegion = 0;
    Target<SBE_ROOT_CHIP_TYPE> l_target =  g_platTarget->plat_getChipTarget();
    auto l_hndl = l_target.getChildren<fapi2::TARGET_TYPE_PERV>(fapi2::TARGET_FILTER_MC,TARGET_STATE_FUNCTIONAL)[0];
    do
    {
        l_scanRegion = decodeScanRegionData(l_hndl, i_ringAddress, i_ringMode);
        // Prep clock controller for ring scan with modified type
        l_rc =  setupClockController( l_hndl, i_ringMode,
                                      l_scanRegion, g_opcgData);
        if(l_rc != fapi2::FAPI2_RC_SUCCESS)
        {
            FAPI_ERR("setupClockController failed. Rc:0x%08X", l_rc);
            break;
        }

        // Write a 64 bit value for header.
        const uint64_t l_header = 0xa5a5a5a5a5a5a5a5ull;
        uint32_t l_scomAddress = 0x0003E000 |  (i_ringAddress & 0xFF000000);
        l_rc = fapi2::putScom(l_hndl, l_scomAddress, l_header);

        if(l_rc != fapi2::FAPI2_RC_SUCCESS)
        {
            FAPI_ERR("setupClockController ring header failed.Rc:0x%08X", l_rc);
            break;
        }
    } while(0);

    return l_rc;
}

ReturnCode getRing_verifyAndcleanup(const uint32_t i_ringAddress,
                                    const RingMode i_ringMode)
{
    fapi2::ReturnCode l_rc = FAPI2_RC_SUCCESS;

    Target<SBE_ROOT_CHIP_TYPE> l_target =  g_platTarget->plat_getChipTarget();
    auto l_hndl = l_target.getChildren<fapi2::TARGET_TYPE_PERV>(fapi2::TARGET_FILTER_MC, TARGET_STATE_FUNCTIONAL)[0];
    do
    {
        uint64_t l_scanRegion = 0;
        l_scanRegion = decodeScanRegionData(l_hndl, i_ringAddress, i_ringMode);
        // Verify header
        l_rc = verifyHeader( l_hndl, l_scanRegion, 0) ;
        if(l_rc != fapi2::FAPI2_RC_SUCCESS)
        {
            FAPI_ERR("verifyHeader failed. Rc:0x%08X", l_rc);
            break;
        }
        l_rc =cleanupClockController( l_hndl,
                                      i_ringMode, l_scanRegion, g_opcgData);
        if(l_rc != fapi2::FAPI2_RC_SUCCESS)
        {
            FAPI_ERR("cleanupClockController failed. Rc:0x%08X", l_rc);
            break;
        }

    }while(0);
    return l_rc;
}

};
