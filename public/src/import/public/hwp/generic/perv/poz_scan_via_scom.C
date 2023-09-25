/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/hwp/generic/perv/poz_scan_via_scom.C $ */
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
//------------------------------------------------------------------------------
/// @file  poz_scan_via_scom.C
/// @brief Implementations for scan-via-scom functions
//------------------------------------------------------------------------------
// *HWP HW Owner        : Joachim Fenkes <fenkes@de.ibm.com>
// *HWP HW Backup Owner : Joe McGill <jmcgill@us.ibm.com>
//------------------------------------------------------------------------------

#include <poz_scan_via_scom.H>
#include <poz_scom_perv_tpchip.H>

using namespace fapi2;
using namespace scomt::poz;
using namespace svs;

enum scan_via_scom_constants
{
    SCAN_HEADER             = 0xA5A55A5AA5A55A5A,
};

template <typename T>
inline ReturnCode scan(const fapi2::Target<fapi2::TARGET_TYPE_PERV>& i_target, uint32_t i_nbits, T& io_stream);

template <>
inline ReturnCode scan(const fapi2::Target<fapi2::TARGET_TYPE_PERV>& i_target, uint32_t i_nbits,
                       hwp_bit_istream& i_stream)
{
    return svs::scan_in(i_target, i_nbits, i_stream);
}

template <>
inline ReturnCode scan(const fapi2::Target<fapi2::TARGET_TYPE_PERV>& i_target, uint32_t i_nbits,
                       hwp_bit_ostream& o_stream)
{
    return svs::scan_out(i_target, i_nbits, o_stream);
}

/**
 * @brief Dump/Load a scan ring
 * @param[in]  i_scan_chiplet     The chiplet that the capture latches can be scanned into/out of
 * @param[in]  i_scan_region_type The value for the SCAN_REGION_TYPE register that will select the right rings
 * @param[in]  i_bits_of_interest A definition in RS5 format of which bits from/to the farr chain to return/load and which to drop
 * @param[inout] io_array_bits    A pre-sized buffer that will be filled with/loaded from the bits of interest
 * @return FAPI2_RC_SUCCESS if success, else error code.
 */
template<typename T>
static ReturnCode sparse_ringaccess(
    const Target<TARGET_TYPE_PERV>& i_scan_chiplet,
    uint64_t                        i_scan_region_type,
    hwp_bit_istream&                i_bits_of_interest,
    T&                              io_array_bits)
{
    buffer<uint64_t> l_data;

    /* Set up scan region & type */
    FAPI_TRY(putScom(i_scan_chiplet, SCAN_REGION_TYPE, i_scan_region_type),
             "Failed to set up scan region/type");

    /* Scan in header */
    FAPI_TRY(svs::scan64_put(i_scan_chiplet, 0, SCAN_HEADER), "Failed to write ring header");

    while (true)
    {
        uint32_t l_nbits = 0;
        /* Rotate / 'don't care' bits */
        FAPI_TRY(i_bits_of_interest.get_var_len_int(l_nbits));

        FAPI_TRY(svs::rotate(i_scan_chiplet, l_nbits));

        if (!l_nbits)
        {
            break;
        }

        /* Scan-out / 'do care' bits */
        FAPI_TRY(i_bits_of_interest.get_var_len_int(l_nbits));

        if (!l_nbits)
        {
            break;
        }

        FAPI_TRY(scan<T>(i_scan_chiplet, l_nbits, io_array_bits));
    }

    FAPI_TRY(svs::scan64_get(i_scan_chiplet, 0, l_data), "Failed to read ring header");

    /* Header check -- the last svs::rotate() conveniently read it into l_data for us :3 */
    FAPI_ASSERT(l_data == SCAN_HEADER, FASTARRAY_HEADER_CHECK_FAILED()
                .set_TARGET(i_scan_chiplet).set_EXPECTED(SCAN_HEADER).set_ACTUAL(l_data),
                "Header check failed! Read header: 0x%08X%08X", l_data >> 32, l_data & 0xFFFFFFFF);

fapi_try_exit:
    ReturnCode l_final_rc = current_err;

    /* Clear scan region & type */
    ReturnCode l_rc = putScom(i_scan_chiplet, SCAN_REGION_TYPE, 0);

    if (l_rc != FAPI2_RC_SUCCESS && l_final_rc == FAPI2_RC_SUCCESS)
    {
        FAPI_ERR("Failed to clear scan region/type");
        l_final_rc = l_rc;
    }

    return l_final_rc;
}

/**
 * @brief Explicit scan type mappings for certain rings
 */
static const uint16_t scan_type_decoder_ring[4] = { 0xDCE0, 0x0820, 0x0440, 0x9000 };

uint64_t svs::expand_ring_address(
    const Target < TARGET_TYPE_PERV | TARGET_TYPE_CORE > & i_target,
    const uint32_t i_ring_address)
{
    const uint32_t l_chiplet_id   =   ((i_ring_address & 0xFF000000) >> 24 );
    const uint32_t l_core_select  =   (l_chiplet_id >= 0x20) ? i_target.getCoreSelect() : 0x8;
    const uint32_t l_scan_region  =   (i_ring_address & 0x0000FFF0) | ((i_ring_address & 0x00F00000) >> 20);
    const uint32_t l_fastinit     =   (i_ring_address & 0x00080000) << 12;
    // Multiplication with l_core_select puts copies of l_core_select everywhere a bit is set in the input region vector
    // Since core 0 is 0x8 we need to shift left by three positions less
    const uint32_t l_reg_upper    =   ((l_scan_region << (13 - 3)) * l_core_select) | l_fastinit;
    const uint32_t l_scan_encode  =   i_ring_address & 0x0000000F;
    const uint32_t l_scan_type    =   (l_scan_encode < 12) ?
                                      (0x8000 >> l_scan_encode) :
                                      scan_type_decoder_ring[l_scan_encode - 12];
    return ((uint64_t)l_reg_upper << 32) | l_scan_type;
}

ReturnCode svs::sparse_getring(
    const Target < TARGET_TYPE_PERV >& i_target,
    uint64_t                        i_scan_region_type,
    hwp_bit_istream&                i_care_bits,
    hwp_bit_ostream&                o_ring_bits)
{
    return sparse_ringaccess<hwp_bit_ostream>(i_target, i_scan_region_type, i_care_bits, o_ring_bits);
}

ReturnCode svs::sparse_putring(
    const Target < TARGET_TYPE_PERV >& i_target,
    uint64_t                        i_scan_region_type,
    hwp_bit_istream&                i_care_bits,
    hwp_bit_istream&                i_ring_bits)
{
    return sparse_ringaccess<hwp_bit_istream>(i_target, i_scan_region_type, i_care_bits, i_ring_bits);
}

ReturnCode poz_sparse_getring(
    const Target < TARGET_TYPE_PERV | TARGET_TYPE_CORE > & i_target,
    const uint32_t                  i_ring_address,
    hwp_data_istream&               i_care_data,
    hwp_data_ostream&               o_ring_data)
{
    const auto l_perv_target = i_target.getParent<TARGET_TYPE_PERV>();
    const uint64_t l_scan_region_type = svs::expand_ring_address(i_target, i_ring_address);
    hwp_bit_istream l_care_bits(i_care_data);
    hwp_bit_ostream l_ring_bits(o_ring_data);
    return svs::sparse_getring(l_perv_target, l_scan_region_type, l_care_bits, l_ring_bits);
}

ReturnCode poz_sparse_putring(
    const Target < TARGET_TYPE_PERV | TARGET_TYPE_CORE > & i_target,
    const uint32_t                  i_ring_address,
    hwp_data_istream&               i_care_data,
    hwp_data_istream&               i_ring_data)
{
    const auto l_perv_target = i_target.getParent<TARGET_TYPE_PERV>();
    const uint64_t l_scan_region_type = svs::expand_ring_address(i_target, i_ring_address);
    hwp_bit_istream l_care_bits(i_care_data);
    hwp_bit_istream l_ring_bits(i_ring_data);
    return svs::sparse_putring(l_perv_target, l_scan_region_type, l_care_bits, l_ring_bits);
}
