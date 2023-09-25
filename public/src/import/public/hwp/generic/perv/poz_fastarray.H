/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/hwp/generic/perv/poz_fastarray.H $   */
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
/// @file poz_fastarray.H
///
/// @brief SBE part of fast array dump: Sets up the ABIST engines, clocks ABIST
///        cycles and dumps the interesting bits of the FARR ring back to the
///        service processor.
///        The information about which bits are interesting during which ABIST
///        cycle is supplied by the caller.
//------------------------------------------------------------------------------
// *HWP HW Owner        : Joachim Fenkes <fenkes@de.ibm.com>
// *HWP HW Backup Owner : Joe McGill <jmcgill@us.ibm.com>
// *HWP FW Owner        : Raja Das <rajadas2@in.ibm.com>
//------------------------------------------------------------------------------

#pragma once

#include <fapi2.H>
#include <hwp_data_stream.H>

/*
 * Helper functions to interpret a control blob
 *
 * The fastarray control blobs are mostly an opaque blob of data, but contain a few
 * elements that are of interest to outside code calling fastarray. For flexibility
 * the control blobs use a Tag-Length-Value (TLV) format, where the blob is organized
 * into sections, and each section has a small header that describes the type of data
 * (the tag) and its length. Code wanting to extract information from a control blob
 * can look for sections with known tags and skip over all sections of unknown type
 * because their length is known.
 *
 * The smallest unit of a control blob is a 32-bit word in big-endian byte order.
 * Each TLV header is a single word, and the blob begins with a TLV header. Use the
 * functions below to interpret the TLV header.
 */

/**
 * @brief Extract the tag from a TLV header
 * @param[in] i_tlv the TLV header. The caller is responsible for conversion from big endian to native byte order.
 * @return The tag that identifies the value following the header.
 */
static inline uint8_t poz_fastarray_chunk_tag(uint32_t i_tlv)
{
    return i_tlv >> 24;
}

/**
 * @brief Extract the length from a TLV header
 * @param[in] i_tlv the TLV header. The caller is responsible for conversion from big endian to native byte order.
 * @return The length of the value following the header, in 32-bit words, not including the header itself.
 */
static inline uint32_t poz_fastarray_chunk_length(uint32_t i_tlv)
{
    return i_tlv & 0x00FFFFFF;
}

/**
 * @brief Tag for a poz_fastarray_control_info structure
 */
static const uint8_t POZ_FASTARRAY_TAG_CONTROL_INFO = 0x1F;

/**
 * @brief Information about the fastarray control blob.
 *
 * This struct may be extended in the future by appending new fields,
 * so users are advised to check the length from the TLV header to make sure
 * a given structure contains all the fields they expect.
 */
struct poz_fastarray_control_info
{
    /*
     * @brief Expected length of return blob, in 32-bit words.
     *
     * Not including SBE headers or footers - pure payload size.
     */
    uint32_t return_data_nwords;
} __attribute__((packed));

/**
 * @brief Locate the info tag in a fastarray control blob and return it
 *
 * @param[in]  i_control_data         Pointer to the control data blob
 * @param[in]  i_control_data_nwords  Size of control data blob, in 32-bit words
 * @param[out] o_control_info         The info record will be copied here
 *
 * @return FAPI2_RC_SUCCESS if the record was found, otherwise FAPI2_RC_FALSE
 */
fapi2::ReturnCode poz_fastarray_get_control_info(
    const uint32_t* i_control_data,
    const uint32_t i_control_data_nwords,
    poz_fastarray_control_info& o_control_info);


typedef fapi2::ReturnCode (*poz_fastarray_FP_t)(
    const fapi2::Target < fapi2::TARGET_TYPE_PERV | fapi2::TARGET_TYPE_CORE > & i_target,
    fapi2::hwp_data_istream&  i_instructions,
    fapi2::hwp_data_ostream&  o_dump_data);


/**
 * @brief Extract a raw fast array dump out of a given chiplet
 *
 * This will go through the entire process of setting up the chiplet's ABIST engines for fast array readout,
 * clocking ABIST cycles and dumping out the interesting bits of the FARR ring. It will also clean up after
 * itself, so that after successful return from this procedure the same chiplet can be dumped again and
 * should yield the exact same data.
 *
 * The data returned by this procedure is raw ring data as it was scanned out of the FARR ring, with no
 * inversion mask applied yet and array data still in the order the observation latches are arranged on
 * the ring in. It will need to be post processed to yield usable array content.
 *
 * The procedure needs some control data to perform its duty; that control data is streamed into the
 * procedure, either from static data compiled into the SBE binary, or from outside the SBE if custom
 * control is required. Likewise, the data that is dumped out of the chiplet is returned in another stream.
 *
 * @param[in]  i_target            The chiplet to be dumped
 * @param[in]  i_instructions      Control data for the procedure
 * @param[out] o_dump_data         Raw fast array dump data that will require offline post processing
 *
 * @return the usual FAPI return codes
 */
extern "C"
{
    fapi2::ReturnCode poz_fastarray(
        const fapi2::Target < fapi2::TARGET_TYPE_PERV | fapi2::TARGET_TYPE_CORE > & i_target,
        fapi2::hwp_data_istream&  i_instructions,
        fapi2::hwp_data_ostream&  o_dump_data);
}
