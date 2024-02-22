/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/common/core/sbestreampaktohwp.H $          */
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

#ifndef __SBESTREAMPAKTOHWP_H
#define __SBESTREAMPAKTOHWP_H

#include "fapi2.H"
#include "hwptable.H"
#include "pakwrapper.H"

/**
 * @brief Implementation of StreamReceiver interface class for stream
 *        decompression.
 */
class HwpStreamReceiver : public FileArchive::StreamReceiver
{
    public:
        /**
         * @brief Constructor for HwpStreamReceiver class.
         *
         * @param[in] i_destinationDataPtr Pointer to destination buffer.
         * @param[in] i_decompressedSize Size of decompressed data.
         */
        HwpStreamReceiver(uint32_t i_offset, voidfuncptr_t i_hwp, uint8_t i_Image) :
                        iv_offset(i_offset),
                        iv_hwp(i_hwp),
                        iv_image(i_Image)
                        {
                        }
        // Each platfrom will have its own implementation of consume.
        int consume(const void* i_data, uint32_t i_size);

        uint32_t getEndOffset() { return iv_offset; }

    protected:
        uint32_t iv_offset;
        voidfuncptr_t iv_hwp;
        uint8_t iv_image;
};

/**
 * @brief Extension of HwpStreamReceiver which extracts specific metadata
 *        from the first block of a PPE image
 */
class PpeImageReceiver : public HwpStreamReceiver
{
    public:
        PpeImageReceiver(uint32_t i_offset, voidfuncptr_t i_hwp, uint8_t i_Image) :
            HwpStreamReceiver(i_offset, i_hwp, i_Image) {};
        int consume(const void* i_data, uint32_t i_size);

        /// @brief Heap start address of loaded image
        uint32_t iv_heapStart;
        /// @brief Heap size of loaded image
        uint32_t iv_heapSize;
        /// @brief Abbreviated git commit hash of loaded image
        uint32_t iv_gitId;
        /// @brief Secure Pak boundary
        uint32_t iv_trustedPakEndPtr;
};

/// @brief Wrapper that loads pak into scratch area in SRAM.
///
/// @param[in]    i_pak          Pointer to PakWrapper
/// @param[in]    i_pakname      Name of pak to load
/// @param[in]    i_receiver     StreamReceiver to receive the image data
/// @return  FAPI2_RC_SUCCESS on success

fapi2::ReturnCode sbestreampaktohwp(
    PakWrapper *i_pak, const char * i_pakname,
    HwpStreamReceiver &i_receiver);

/// @brief Wrapper that loads pak into scratch area in SRAM.
/// Does _NOT_ verify the hash, for use in case a file is loaded from
/// the PIBMEM embedded archive which has already been verified.
///
/// @param[in]    i_pak          Pointer to PakWrapper
/// @param[in]    i_pakname      Name of pak to load
/// @param[in]    i_receiver     StreamReceiver to receive the image data
/// @return  FAPI2_RC_SUCCESS on success

fapi2::ReturnCode sbestreampaktohwp_unverified(
    PakWrapper *i_pak, const char * i_pakname,
    HwpStreamReceiver &i_receiver);

/**
 * @brief Execute HWP on pak stream
 *
 * @param[in] i_hwp          Function pointer to HWP
 * @param[in] i_bin_data     Pointer to binary data
 * @param[in] i_bin_size     Binary size
 * @param[in] i_bin_offset   Binary offset
 * @param[in] i_image        Image type
 *
 * @return FAPI2_RC_SUCCESS on success
 */
fapi2::ReturnCode sbeexecutehwponpak(
    voidfuncptr_t i_hwp, uint8_t* const i_bin_data,
    const uint32_t i_bin_size, const uint32_t i_bin_offset,
    uint8_t i_image);


/**
 * @brief Check file hash and validate
 *
 * @param[in] i_pakname             Name of pak to check hash and validate
 * @param[in] i_hashData            Hash data
 *
 * @return ReturnCode
 */
fapi2::ReturnCode check_file_hash_and_validate(const char * i_pakname, const sha3_t &i_hashData);


#endif
