/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/odyssey/sppe/core/hwpWrapper.C $           */
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
#include "hwpWrapper.H"
#include "istep_includes.H"
#include "sbestreampaktohwp.H"
#include "pakwrapper.H"
#include "globals.H"
#include "heap.H"

#define SRAM_SCRATCH_GRANULAR_SIZE 0x10000 // 64 KB

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

ReturnCode istepLoadIMEMwithOcmb( voidfuncptr_t i_hwp)
{
    #define SBE_FUNC " istepLoadIMEMwithOcmb "
    SBE_INFO(SBE_FUNC);
    ReturnCode rc = FAPI2_RC_SUCCESS;

    PakWrapper pak((void *)g_partitionOffset);
    static const char pakname[]   =  "ddr/ddimm/imem.bin";
    rc = sbestreampaktohwp(&pak, pakname, i_hwp);
    if (rc)
    {
        SBE_ERROR(SBE_FUNC " sbestreampaktohwp failed with rc 0x%08X", rc);
    }

    return rc;
    #undef SBE_FUNC
}

ReturnCode istepLoadDMEMwithOcmb( voidfuncptr_t i_hwp)
{
    #define SBE_FUNC " istepLoadDMEMwithOcmb "
    SBE_INFO(SBE_FUNC);
    ReturnCode rc = FAPI2_RC_SUCCESS;

    PakWrapper pak((void *)g_partitionOffset);
    static const char pakname[]   =  "ddr/ddimm/dmem.bin";
    rc = sbestreampaktohwp(&pak, pakname, i_hwp);
    if (rc)
    {
        SBE_ERROR(SBE_FUNC " sbestreampaktohwp failed with rc 0x%08X", rc);
    }

    return rc;
    #undef SBE_FUNC
}

ReturnCode istepLoadIOPPEwithOcmb( voidfuncptr_t i_hwp)
{
    #define SBE_FUNC " istepLoadIOPPEwithOcmb "
    SBE_INFO(SBE_FUNC);
    ReturnCode rc = FAPI2_RC_SUCCESS;

    do{
        PakWrapper pak((void *)g_partitionOffset);
        static const char ioppe[]   =  "ioppe/ioo.bin";
        rc = sbestreampaktohwp(&pak, ioppe, i_hwp);
        if (rc)
        {
            SBE_ERROR(SBE_FUNC " sbestreampaktohwp failed with rc 0x%08X for ioppe/ioo.bin", rc);
            break;
        }

        static const char memregs[]   =  "ioppe/ioo_memregs.bin";
        rc = sbestreampaktohwp(&pak, memregs, i_hwp);
        if (rc)
        {
            SBE_ERROR(SBE_FUNC " sbestreampaktohwp failed with rc 0x%08X for ioppe/ioo_memregs.bin", rc);
        }
    }while(0);

    return rc;
    #undef SBE_FUNC
}

ReturnCode sbeexecutehwponpak( voidfuncptr_t i_hwp, uint8_t* const i_bin_data,
                               const uint32_t i_bin_size, const uint32_t i_bin_offset,
                               uint8_t i_ioppeImage)
{
    #define SBE_FUNC " sbeexecutehwponpak "
    SBE_INFO(SBE_FUNC);
    ReturnCode fapiRc = FAPI2_RC_SUCCESS;

    Target<TARGET_TYPE_OCMB_CHIP > l_ocmb_chip = g_platTarget->plat_getChipTarget();
    if(i_ioppeImage)
    {
        SBE_EXEC_HWP(fapiRc, reinterpret_cast<sbeHwpIOLoadbin_t>( i_hwp ),
                     l_ocmb_chip, (uint8_t *)i_bin_data, i_bin_size, i_bin_offset, i_ioppeImage);
    }
    else
    {
        SBE_EXEC_HWP(fapiRc, reinterpret_cast<sbeHwpMEMLoadbin_t>( i_hwp ),
                     l_ocmb_chip, (uint8_t *)i_bin_data, i_bin_size, i_bin_offset);
    }
    return fapiRc;
    #undef SBE_FUNC
}

ReturnCode istepDraminitWithOcmb( voidfuncptr_t i_hwp)
{
    #define SBE_FUNC " istepDraminitWithOcmb "
    SBE_ENTER(SBE_FUNC);
    ReturnCode fapiRc = FAPI2_RC_SUCCESS;
    uint32_t *scratchArea = NULL;
    do
    {
        if(i_hwp != NULL)
        {
            // Allocate the scratch space of 64 KB for log data.
            scratchArea =
                    (fapi2::hwp_data_unit*)Heap::get_instance().scratch_alloc(SRAM_SCRATCH_GRANULAR_SIZE);

            if(scratchArea == NULL)
            {
                SBE_ERROR(SBE_FUNC "scratch allocation failed.");
                fapiRc = FAPI2_RC_PLAT_ERR_SEE_DATA;
                break;
            }

            // Create the stream class pointing to the scratch space.
            fapi2::hwp_array_ostream  logStream( scratchArea,
                                      SRAM_SCRATCH_GRANULAR_SIZE/sizeof(fapi2::hwp_data_unit));

            Target<TARGET_TYPE_OCMB_CHIP > l_ocmb_chip = g_platTarget->plat_getChipTarget();
            SBE_EXEC_HWP(fapiRc, reinterpret_cast<sbeHwpDraminit_t>( i_hwp ),
                          l_ocmb_chip, logStream);
        }
    }while(0);

    // Update DRAM offset with allocate the scratch space.
    g_draminitOffset =  (uint32_t)scratchArea;

    SBE_EXIT(SBE_FUNC);
    return fapiRc;
    #undef SBE_FUNC
}
