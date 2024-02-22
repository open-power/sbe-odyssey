/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/odyssey/sppe/core/hwpWrapper.C $           */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2023,2024                        */
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
#include "istepincludes.H"
#include "sbestreampaktohwp.H"
#include "pakwrapper.H"
#include "globals.H"
#include "heap.H"
#include "sbestates.H"
#include "sbestatesutils.H"
#include "mss_odyssey_attribute_getters.H"
#include "mss_generic_attribute_getters.H"
#include "plat_i2c_access.H"
#include "odysseylink.H"
#include "sha3.H"
#include "sbeutil.H"
#include "sbeglobals.H"
#include "archive.H"

#define MEM_PAKNAME_MAX_CHAR  20 // ddr/ddimm/dmem.bin

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

///
/// @brief Gets the directory of the binary for a given package
/// @param[in,out] io_packname the base pakage name directory
/// @return fapi2::ReturnCode SUCCESS iff procedure is successful
///
ReturnCode getBinaryDirectory( char (&io_packname)[MEM_PAKNAME_MAX_CHAR])
{
    // Get ATTR_MSS_ODY_PHY_IMAGE_SELECT
    uint8_t imageType = 0;
    const Target<TARGET_TYPE_OCMB_CHIP > l_ocmb_chip = g_platTarget->plat_getChipTarget();

    // The IMEM/DMEM are different for RDIMM's, so get the DIMM type to see if this is an RDIMM
    constexpr uint8_t DIMM_PER_PORT = 2;
    uint8_t dimmType[DIMM_PER_PORT] = {};
    const auto& l_mem_ports = l_ocmb_chip.getChildren<fapi2::TARGET_TYPE_MEM_PORT>();

    // If there are no mem ports, use a default and exit out
    if(l_mem_ports.empty())
    {
        strcpy(io_packname, "ddr/ddimm/");
        return fapi2::FAPI2_RC_SUCCESS;
    }

    FAPI_TRY(FAPI_ATTR_GET(fapi2::ATTR_MSS_ODY_PHY_IMAGE_SELECT, l_ocmb_chip, imageType), "get_ody_phy_image_select failed");
    FAPI_TRY(mss::attr::get_dimm_type(l_mem_ports[0], dimmType), "get_dimm_type failed");

    // Use the ATE image if ATE is selected
    if(imageType  == fapi2::ENUM_ATTR_MSS_ODY_PHY_IMAGE_SELECT_ATE_IMAGE)
    {
        strcpy(io_packname, "ddr/ate/");
    }
    // RDIMM? grab the RDIMM image
    // Note: only DIMM0 on the port matters
    else if(dimmType[0] == fapi2::ENUM_ATTR_MEM_EFF_DIMM_TYPE_RDIMM)
    {
        strcpy(io_packname, "ddr/rdimm/");
    }
    // Otherwise, grab the DDIMM image
    else
    {
        strcpy(io_packname, "ddr/ddimm/");
    }

fapi_try_exit:
    return fapi2::current_err;
}

ReturnCode istepLoadIMEMwithOcmb( voidfuncptr_t i_hwp)
{
    #define SBE_FUNC " istepLoadIMEMwithOcmb "
    SBE_INFO(SBE_FUNC);
    ReturnCode rc = FAPI2_RC_SUCCESS;
    do
    {
        PakWrapper pak((void *)g_partitionOffset, (void *)(g_partitionOffset + g_partitionSize));

        char pakname[MEM_PAKNAME_MAX_CHAR] = "";

        rc = getBinaryDirectory(pakname);
        if (rc)
        {
            SBE_ERROR(SBE_FUNC " getBinaryDirectory failed with rc 0x%08X", rc);
            break;
        }

        // Now append the imem.bin
        strcat (pakname,"imem.bin");

        HwpStreamReceiver rec(0, i_hwp, DDR_IMEM_IMAGE);
        rc = sbestreampaktohwp(&pak, pakname, rec);
        if (rc)
        {
            SBE_ERROR(SBE_FUNC " sbestreampaktohwp failed with rc 0x%08X", rc);
        }
    }while(0);
    return rc;
    #undef SBE_FUNC
}

ReturnCode istepLoadDMEMwithOcmb( voidfuncptr_t i_hwp)
{
    #define SBE_FUNC " istepLoadDMEMwithOcmb "
    SBE_INFO(SBE_FUNC);
    ReturnCode rc = FAPI2_RC_SUCCESS;
    do
    {
        PakWrapper pak((void *)g_partitionOffset, (void *)(g_partitionOffset + g_partitionSize));

        char pakname[MEM_PAKNAME_MAX_CHAR] = "";

        rc = getBinaryDirectory(pakname);
        if (rc)
        {
            SBE_ERROR(SBE_FUNC " getBinaryDirectory failed with rc 0x%08X", rc);
            break;
        }

        // Now append the dmem.bin
        strcat (pakname,"dmem.bin");

        HwpStreamReceiver rec(0, i_hwp, DDR_DMEM_IMAGE);
        rc = sbestreampaktohwp(&pak, pakname, rec);
        if (rc)
        {
            SBE_ERROR(SBE_FUNC " sbestreampaktohwp failed with rc 0x%08X", rc);
        }
    }while(0);

    return rc;
    #undef SBE_FUNC
}

ReturnCode istepLoadIOPPEwithOcmb( voidfuncptr_t i_hwp)
{
    #define SBE_FUNC " istepLoadIOPPEwithOcmb "
    SBE_INFO(SBE_FUNC);
    ReturnCode rc = FAPI2_RC_SUCCESS;

    do{
        PakWrapper pak((void *)g_partitionOffset, (void *)(g_partitionOffset + g_partitionSize));

        {
            HwpStreamReceiver rec(0, i_hwp, IOPPE_BASE_IMAGE);
            rc = sbestreampaktohwp(&pak, "ioppe/ioo.bin", rec);
            if (rc)
            {
                SBE_ERROR(SBE_FUNC " sbestreampaktohwp failed with rc 0x%08X for ioppe/ioo.bin", rc);
                break;
            }
        }

        {
            HwpStreamReceiver rec(0, i_hwp, IOPPE_MEMREGS_IMAGE);
            rc = sbestreampaktohwp(&pak, "ioppe/ioo_memregs.bin", rec);
            if (rc)
            {
                SBE_ERROR(SBE_FUNC " sbestreampaktohwp failed with rc 0x%08X for ioppe/ioo_memregs.bin", rc);
            }
        }
    }while(0);

    return rc;
    #undef SBE_FUNC
}

ReturnCode sbeexecutehwponpak( voidfuncptr_t i_hwp, uint8_t* const i_bin_data,
                               const uint32_t i_bin_size, const uint32_t i_bin_offset,
                               uint8_t i_image)
{
    #define SBE_FUNC " sbeexecutehwponpak "
    SBE_INFO(SBE_FUNC);
    ReturnCode fapiRc = FAPI2_RC_SUCCESS;

    Target<TARGET_TYPE_OCMB_CHIP > l_ocmb_chip = g_platTarget->plat_getChipTarget();
    SBE_EXEC_HWP(fapiRc, reinterpret_cast<poz_common_image_load_FP_t>( i_hwp ),
                 l_ocmb_chip, (uint8_t *)i_bin_data, i_bin_size, i_bin_offset,
                 static_cast<poz_image_type>(i_image));

    // SBE_EXEC_HWP assigns fapi2::current_err returned by the hardware procedure to
    // fapiRc; In case of failure, fapiRc is committed and data pointer set to NULL;
    // But, fapi2::current_err will continue to hold the RC and the data pointer
    // returned by the hardware procedure; So, set the RC to SUCCESS. No need to worry
    // about the data pointer as it has been already committed via fapiRc.
    fapi2::current_err = FAPI2_RC_SUCCESS;

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
                    (fapi2::hwp_data_unit*)Heap::get_instance().scratch_alloc(SPPE_MEM_TRAINING_DATA_SIZE);

            PLAT_FAPI_ASSERT( (scratchArea != NULL),
                              POZ_SCRATCH_ALLOC_FAILED().
                              set_REQUIRED_SPACE(SPPE_MEM_TRAINING_DATA_SIZE).
                              set_AVAILABLE_SPACE(Heap::get_instance().getFreeHeapSize()),
                              "scratch allocation failed.");

            // Create the stream class pointing to the scratch space.
            fapi2::hwp_array_ostream  logStream( scratchArea,
                                      SPPE_MEM_TRAINING_DATA_SIZE/sizeof(fapi2::hwp_data_unit));

            Target<TARGET_TYPE_OCMB_CHIP > l_ocmb_chip = g_platTarget->plat_getChipTarget();
            SBE_EXEC_HWP(fapiRc, reinterpret_cast<sbeHwpDraminit_t>( i_hwp ),
                          l_ocmb_chip, logStream);

            g_dramDataSizeInWords = (logStream.getLength() * sizeof(hwp_data_unit) / 4);

            // Update DRAM offset with allocate the scratch space.
            g_draminitOffset =  (uint32_t)scratchArea;
        }
    }while(0);

    SBE_EXIT(SBE_FUNC);
    return fapiRc;

fapi_try_exit:
    SBE_EXIT(SBE_FUNC);
    // PLAT_FAPI_ASSERT will not commit the error.So, it is the responsibility of this
    // macro user to commit the error log.
    logFatalError(fapi2::current_err);
    return fapi2::current_err;

    #undef SBE_FUNC
}

ReturnCode istepLoadPIEwithOcmb( voidfuncptr_t i_hwp)
{
    #define SBE_FUNC " istepLoadPIEwithOcmb "
    SBE_ENTER(SBE_FUNC);

    // Variable declaration
    ReturnCode l_rc = FAPI2_RC_SUCCESS;
    uint8_t *l_scratchArea = nullptr;

    do
    {
        // assret if there in no HWP
        assert( NULL != i_hwp );
        Target<TARGET_TYPE_OCMB_CHIP > l_ocmb_chip = g_platTarget->plat_getChipTarget();

        PakWrapper pak((void *)g_partitionOffset, (void *)(g_partitionOffset + g_partitionSize));

        char pakname[MEM_PAKNAME_MAX_CHAR] = "";
        l_rc = getBinaryDirectory(pakname);

        if (l_rc)
        {
            SBE_ERROR(SBE_FUNC " getBinaryDirectory failed with l_rc[0x%08X]", l_rc);
            break;
        }

        // Now append the pie.bin
        strcat (pakname,"pie.bin");

        // Variable declaration
        uint32_t l_imageSize = 0;
        uint32_t* l_imageStartPtr = nullptr;

        l_rc = pak.get_image_start_ptr_and_size(pakname, &l_imageStartPtr, &l_imageSize);

        //Decompress the file
        if ((l_rc == FAPI2_RC_SUCCESS) && (l_imageSize != 0))
        {
            l_scratchArea = (uint8_t *)Heap::get_instance().scratch_alloc(l_imageSize);
            if(l_scratchArea == nullptr)
            {
                l_rc = SBE_SEC_HEAP_BUFFER_ALLOC_FAILED;
                SBE_ERROR(SBE_FUNC " scratch allocation request for [%d] bytes failed " \
                                    " RC[0x%08x]",l_imageSize, l_rc);
                break;
            }

            uint32_t l_size = 0;
            sha3_t l_digest = {0};
            l_rc = pak.read_file(pakname, l_scratchArea, l_imageSize, &l_digest, &l_size);

            if( l_rc != FAPI2_RC_SUCCESS )
            {
                SBE_ERROR(SBE_FUNC "Failed to read image file [%s]. RC=0x%08X", pakname, l_rc);
                break;
            }

            if (l_imageSize != l_size)
            {
                l_rc = SBE_SEC_IMAGE_SIZE_MISMATCH;
                SBE_ERROR(SBE_FUNC "Failed to read, Expected size[%d]" \
                                    "actual size[%d] ", l_imageSize, l_size);
                break;
            }

            // checking the image hash and validate
            l_rc = check_file_hash_and_validate(pakname, l_digest);
            if(l_rc != FAPI2_RC_SUCCESS)
            {
                SBE_ERROR(SBE_FUNC " File hash valiadtion failed ", l_rc);
                break;
            }
        }

        // Pad out image size to 32-bit boundary
        uint8_t l_remainder = (l_imageSize % 4);
        l_imageSize = (l_remainder == 0) ? l_imageSize : l_imageSize + (4 - l_remainder);

        fapi2::hwp_array_istream l_dimm_istream_0((fapi2::hwp_data_unit*)l_scratchArea, (l_imageSize / sizeof(uint32_t)));
        fapi2::hwp_array_istream l_dimm_istream_1((fapi2::hwp_data_unit*)l_scratchArea, (l_imageSize / sizeof(uint32_t)));

        SBE_EXEC_HWP(l_rc, reinterpret_cast<sbeHwpLoadPIE_t>( i_hwp ), l_ocmb_chip,
                     l_dimm_istream_0, l_dimm_istream_1);

    }while(0);

    Heap::get_instance().scratch_free(l_scratchArea);
    SBE_EXIT(SBE_FUNC);
    return l_rc;
    #undef SBE_FUNC
}
