/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/hwp/generic/perv/poz_scan_via_scom.H $ */
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
#pragma once

#include <fapi2.H>
#include <hwp_data_stream.H>
#include <poz_scom_perv_tpchip.H>

namespace
{
/**
 * @brief Wait for OPCG_DONE after a long rotate
 * @param[in] The target chiplet to wait on
 * @return FAPI2_RC_SUCCESS          if all went well
 *         RC_SCAN_VIA_SCOM_TIMEOUT  if OPCG_DONE didn't turn on within 100ms
 *         another FAPI2 error       if any of the SCOM operations failed
 */
inline fapi2::ReturnCode _svs_wait_opcg_done(
    const fapi2::Target<fapi2::TARGET_TYPE_PERV>& i_target)
{
    fapi2::buffer<uint64_t> buf;
    uint32_t timeout = 10000;

    while (timeout--)
    {
        fapi2::delay(10000, 20000);
        FAPI_TRY(fapi2::getScom(i_target, scomt::poz::CPLT_STAT0, buf), "Failed to read chiplet status 0");

        if (buf.getBit<scomt::poz::CPLT_STAT0_OPCG_DONE>())
        {
            return fapi2::FAPI2_RC_SUCCESS;
        }
    }

    FAPI_ASSERT(false, fapi2::SCAN_VIA_SCOM_TIMEOUT().set_TARGET(i_target).set_CPLT_STAT0(scomt::poz::CPLT_STAT0),
                "Timed out waiting for OPCG_DONE");

fapi_try_exit:
    return fapi2::current_err;
}

/**
 * @brief Initiate a long rotate operation and wait for it to complete.
 * @param[in] i_target The target chiplet
 * @param[in] i_rotate Amount of bits to rotate by; min 0, max LONG_ROTATE_MAX()
 * @return FAPI2_RC_SUCCESS or error code of a sub-operation
 */
inline fapi2::ReturnCode _svs_long_rotate(
    const fapi2::Target<fapi2::TARGET_TYPE_PERV>& i_target,
    uint32_t i_rotate)
{
    fapi2::buffer<uint64_t> buf = 0;
    buf.insertFromRight<12, 20>(i_rotate);
    FAPI_TRY(fapi2::putScom(i_target, 0x039000, buf), "Failed to initiate long rotate");
    FAPI_TRY(_svs_wait_opcg_done(i_target));

fapi_try_exit:
    return fapi2::current_err;
}
} // anon. namespace

namespace svs
{
/**
 * @brief Limits of the clock controller
 * The rotate amount can go up to FFF, but we'd run into PIB timeouts at too high values if the core is running slowly
 * Looking at the rotate amounts generated on the P9DD1 core, a limit of ~1K will make almost no difference versus
 * 4K or 8K, so pick that.
 *
 * TODO Convert to `inline constexpr` variables if/when we decide to compile with c++17.
 *      The `inline constexpr` function thing is a c++11 smell to prevent a unique instance
 *      of these variables per translation-unit.
 */
inline constexpr uint32_t SCAN_ROTATE_MAX()
{
    return 0x380;
}
inline constexpr uint32_t LONG_ROTATE_MAX()
{
    return ((1 << 20) - 1);
}

/**
 * @brief Low-level scan-via-scom read: get the 64bit scan buffer and rotate
 * @param[in]  i_target The target to operate on
 * @param[in]  i_rotate Rotate by this many bits; min 0, max SCAN_ROTATE_MAX()
 * @param[out] o_data   Resulting scan buffer data
 * @return the FAPI2 return code of the SCOM operation
 */
inline fapi2::ReturnCode scan64_get(
    const fapi2::Target<fapi2::TARGET_TYPE_PERV>& i_target,
    uint32_t i_rotate, fapi2::buffer<uint64_t>& o_data)
{
    return fapi2::getScom(i_target, 0x03F000 | i_rotate, o_data);
}

/**
 * @brief Low-level scan-via-scom write: set the 64bit scan buffer and rotate
 * @param[in] i_target The target to operate on
 * @param[in] i_rotate Rotate by this many bits; min 0, max SCAN_ROTATE_MAX()
 * @param[in] i_data   Data to write into the scan buffer. If i_rotate is < 64, only the first i_rotate bits will be written
 * @return the FAPI2 return code of the SCOM operation
 */
inline fapi2::ReturnCode scan64_put(
    const fapi2::Target<fapi2::TARGET_TYPE_PERV>& i_target,
    uint32_t i_rotate, const fapi2::buffer<uint64_t> i_data)
{
    return fapi2::putScom(i_target, 0x03F000 | i_rotate, i_data);
}

/*
 * To minimize the amount of necessary SCOMs, scan_out() and rotate() work
 * in lockstep -- scan_out() doesn't do the last rotate but returns the extra
 * number of bits to rotate in the next rotate operation. Conversely, rotate()
 * returns the scan buffer after rotation, which already contains the first 64
 * bits of the data scan_out() is interested in.
 */

/**
 * @brief Scan out N bits of data
 * @param[in]  i_target       The target chiplet
 * @param[in]  i_nbits        Amount of bits to scan out
 * @param[out] o_stream       Bit stream to append scanned data to
 * @return FAPI2 return code
 */
inline fapi2::ReturnCode scan_out(
    const fapi2::Target<fapi2::TARGET_TYPE_PERV>& i_target,
    uint32_t i_nbits, fapi2::hwp_bit_ostream& o_stream)
{
    fapi2::buffer<uint64_t> l_data;

    while (i_nbits)
    {
        uint32_t l_bits_to_scan = std::min(i_nbits, (uint32_t)64);
        FAPI_TRY(scan64_get(i_target, l_bits_to_scan, l_data), "Failed to perform a scan read operation");

        FAPI_TRY(o_stream.put(l_data, l_bits_to_scan, true),
                 "Failed to place scanned data into the output buffer");

        i_nbits -= l_bits_to_scan;
    }

fapi_try_exit:
    return fapi2::current_err;
}

/**
 * @brief Rotate ring by N bits
 * @param[in]  i_target The target chiplet
 * @param[in]  i_nbits  Amount of bits to rotate by
 * @return FAPI2 return code
 */
inline fapi2::ReturnCode rotate(
    const fapi2::Target<fapi2::TARGET_TYPE_PERV>& i_target,
    uint32_t i_nbits)
{
    /* Get the # remaining bits down to a sensible range */
    while (i_nbits > LONG_ROTATE_MAX())
    {
        FAPI_TRY(_svs_long_rotate(i_target, LONG_ROTATE_MAX()));
        i_nbits -= LONG_ROTATE_MAX();
    }

    /* If we're outside the range of normal scan, do long rotate else do a non-blocking scan */
    if (i_nbits > SCAN_ROTATE_MAX())
    {
        FAPI_TRY(_svs_long_rotate(i_target, i_nbits));
    }
    else
    {
        fapi2::buffer<uint64_t> l_data;
        FAPI_TRY(scan64_get(i_target, i_nbits, l_data), "Failed to read scan data");
    }

    return fapi2::FAPI2_RC_SUCCESS;

fapi_try_exit:
    return fapi2::current_err;
}

/**
 * @brief Scan in N bits of data
 * @param[in]  i_target       The target chiplet
 * @param[in]  i_nbits        Amount of bits to scan in
 * @param[in]  i_stream       Bit stream of data to stream in sequentially
 * @return FAPI2 return code
 */
inline fapi2::ReturnCode scan_in(
    const fapi2::Target<fapi2::TARGET_TYPE_PERV>& i_target,
    uint32_t i_nbits, fapi2::hwp_bit_istream& i_stream)
{
    uint64_t tmp = 0;

    while (i_nbits)
    {
        uint32_t l_bits_to_scan = std::min(i_nbits, (uint32_t)64);
        FAPI_TRY(i_stream.get(tmp, l_bits_to_scan, true),
                 "Failed to read scan data from input buffer");

        FAPI_TRY(scan64_put(i_target, l_bits_to_scan, tmp), "Failed to perform scan put operation");

        i_nbits -= l_bits_to_scan;
    }

fapi_try_exit:
    return fapi2::current_err;
}


/**
 * @brief expand a 32bit ring address into a 64bit value for the clock controller's SCAN_REGION_TYPE register
 *
 * If the ring address belongs to a core ring, the result will be shifted to match a core target's core select.
 *
 * @param[in] i_target a PERV or CORE target; used for adapting the result to a core select value
 * @param[in] i_ring_address the 32bit ring address
 * @return The 64bit SCAN_REGION_TYPE value
 */
uint64_t expand_ring_address(
    const fapi2::Target < fapi2::TARGET_TYPE_PERV | fapi2::TARGET_TYPE_CORE > & i_target,
    const uint32_t i_ring_address);

/**
 * @brief Dump a scan ring and into an array
 * @param[in]  i_scan_chiplet     The chiplet that the capture latches can be scanned out of
 * @param[in]  i_scan_region_type The value for the SCAN_REGION_TYPE register that will select the right rings
 * @param[in]  i_bits_of_interest A definition in RS5 format of which bits from the farr chain to return and which to drop
 * @param[out] o_array_bits       A pre-sized buffer that will be filled with the bits of interest
 * @return FAPI2_RC_SUCCESS if success, else error code.
 */
fapi2::ReturnCode sparse_getring(
    const fapi2::Target<fapi2::TARGET_TYPE_PERV>& i_scan_chiplet,
    uint64_t                                      i_scan_region_type,
    fapi2::hwp_bit_istream&                       i_bits_of_interest,
    fapi2::hwp_bit_ostream&                       o_array_bits);

/**
 * @brief Restore a scan ring from an array
 * @param[in]  i_scan_chiplet     The chiplet that the capture latches can be scanned out of
 * @param[in]  i_scan_region_type The value for the SCAN_REGION_TYPE register that will select the right rings
 * @param[in]  i_bits_of_interest A definition in RS5 format of which bits from the farr chain to return and which to drop
 * @param[out] i_array_bits       A pre-sized buffer that is be filled with the bits to be restored
 * @return FAPI2_RC_SUCCESS if success, else error code.
 */
fapi2::ReturnCode sparse_putring(
    const fapi2::Target<fapi2::TARGET_TYPE_PERV>& i_scan_chiplet,
    uint64_t                                      i_scan_region_type,
    fapi2::hwp_bit_istream&                       i_bits_of_interest,
    fapi2::hwp_bit_istream&                       i_array_bits);


} // namespace svs

/**
 * @brief Unload a ring, extracting only the care bits according to run length encoded care mask
 *
 * This is an external API for code which is used internally in fastarray, and which is likely
 * useful for other code as well (such as BIST). It accepts a run length encoded stream of care
 * data (encoded by an external Python tool), will scan out the ring and emit only the ring bits
 * marked as "care" back via an output data stream.
 *
 * The ring bits returned by this function are raw ring bits and not corrected for inversion!
 *
 * @param[in]  i_target        The chiplet or core you would like to scan out
 * @param[in]  i_ring_address  The ring address of the ring you would like to scan
 * @param[in]  i_care_data     A stream of care data blob as prepared by the external tool
 * @param[out] o_ring_bits     The care bits, concatenated in big-endian fashion,
 *                             padded to a multiple of 32-bit words by appending zero bits
 *
 * @return the usual FAPI2 return codes
 */
fapi2::ReturnCode poz_sparse_getring(
    const fapi2::Target < fapi2::TARGET_TYPE_PERV | fapi2::TARGET_TYPE_CORE > & i_target,
    const uint32_t                  i_ring_address,
    fapi2::hwp_data_istream&        i_care_data,
    fapi2::hwp_data_ostream&        o_ring_bits);

/**
 * @brief Restore a ring, overwriting only the care bits according to run length encoded care mask
 *
 * This is an external API for code which is used internally in fastarray, and which is likely
 * useful for other code as well (such as BIST). It accepts a run length encoded stream of care
 * data (encoded by an external Python tool), will scan in the ring and change only the ring bits
 * marked as "care" back via an input data stream.
 *
 * The ring bits returned by this function are raw ring bits and not corrected for inversion!
 *
 * @param[in]  i_target        The chiplet or core you would like to scan out
 * @param[in]  i_ring_address  The ring address of the ring you would like to scan
 * @param[in]  i_care_data     A stream of care data blob as prepared by the external tool
 * @param[out] i_ring_bits     The care bits, concatenated in big-endian fashion,
 *                             padded to a multiple of 32-bit words by appending zero bits
 *
 * @return the usual FAPI2 return codes
 */
fapi2::ReturnCode poz_sparse_putring(
    const fapi2::Target < fapi2::TARGET_TYPE_PERV | fapi2::TARGET_TYPE_CORE > & i_target,
    const uint32_t                 i_ring_address,
    fapi2::hwp_data_istream&       i_care_data,
    fapi2::hwp_data_istream&       i_ring_bits);
