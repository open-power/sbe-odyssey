/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/common/utils/imageProcs/archive.H $  */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2022                             */
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

#include <stdint.h>
#include "archive_config.H"

#ifdef __USE_HWP_STREAM__
    #include "hwp_data_stream.H"
#endif

/**
 * @brief Class managing an SBE data archive
 */
class FileArchive
{
    public:
        /**
         * @brief Compression constants; must match external compressor traits
         */
        static const uint32_t DICTIONARY_SIZE = 32768;
        static const uint32_t MAX_MATCH_LEN   = 258;

        /**
         * @brief Required scratch buffer size for streaming decompression (in bytes)
         */
        static const uint32_t STREAM_SCRATCH_SIZE = DICTIONARY_SIZE + 2 * MAX_MATCH_LEN;

        /**
         * Abstract base class for receivers of streamed decompression
         */
        class StreamReceiver
        {
            public:
                /**
                 * @brief Consume decompressed bytes
                 *
                 * This method is called for every DICTIONARY_SIZE bytes of decompressed data, or
                 * at the end of the stream for the remaining data.
                 * The receiver must consume all data.
                 *
                 * @param[in] i_data  Start of decompressed data
                 * @param[in] i_size  Number of bytes available
                 *
                 * @return ARC_OPERATION_SUCCESSFUL or any SBE_SEC_* error code.
                 *         If an error is returned, decompression will be aborted and the
                 *         error will be returned to the caller.
                 */
                virtual ARC_RET_t consume(const void* i_data, uint32_t i_size)
#ifdef __SBE_PPE__
                    = 0;
#else
                {
                    return ARC_FUNCTIONALITY_NOT_SUPPORTED;
                }
#endif
        };

        /**
         * @brief One file entry in an archive
         */
        class Entry
        {
            public:
                /**
                 * @brief The uncompressed size of the file entry
                 */
                __attribute__((always_inline))
                uint32_t get_size()
                {
                    return iv_uncompressedSize;
                }

                /**
                 * @brief Decompress the file data into a buffer with given maximum size.
                 *
                 * If the file is stored uncompressed, the file data is simply copied.
                 *
                 * @param[out] o_buffer Buffer that will receive the file data
                 * @param[in]  i_size   Size of the buffer
                 * @param[out] o_hash   If non-NULL, the SHA3 hash of the compressed file data will be
                 *                      calculated and placed here. If NULL, the hash will not be calculated.
                 *
                 * @return     ARC_OPERATION_SUCCESSFUL   if the decompression was successful
                 *             ARC_INPUT_BUFFER_OVERFLOW  if the decompressed size exceeds the buffer size
                 *             ARC_FILE_CORRUPTED         if the compressed data turned out to be corrupted
                 */
                ARC_RET_t decompress(void* o_buffer, uint32_t i_size, sha3_t* o_hash = NULL);

                /**
                 * @brief Decompress the file data in a streaming fashion
                 *
                 * @param[in]  i_receiver Receiver object that will receive the uncompressed data stream
                 * @param[in]  i_scratch  Scratch space for decompression, must be STREAM_SCRATCH_SIZE bytes in size
                 * @param[out] o_hash     If non-NULL, the SHA3 hash of the compressed file data will be
                 *                        calculated and placed here. If NULL, the hash will not be calculated.
                 *
                 * @return     ARC_OPERATION_SUCCESSFUL   if the decompression was successful
                 *             ARC_FILE_CORRUPTED         if the compressed data turned out to be corrupted
                 *             something else             if the stream receiver returned an error
                 */
                ARC_RET_t stream_decompress(StreamReceiver& i_receiver, void* i_scratch, sha3_t* o_hash = NULL);

                /**
                 * @brief Retrieve the direct pointer to the stored data inside the archive for uncompressed files
                 *
                 * This asserts that the file has been stored in the archive uncompressed, and fails if
                 * the file has been compressed. Use this instead of get_compressed_data() if you'd like
                 * to stream files directly from SEEPROM.
                 *
                 * @param[out] o_buffer Will receive a pointer to the uncompressed file data.
                 *
                 * @return     ARC_OPERATION_SUCCESSFUL        if no problem occurred
                 *             ARC_FUNCTIONALITY_NOT_SUPPORTED if the file is compressed
                 */
                ARC_RET_t get_stored_data_ptr(const void*& o_buffer);
#ifdef __USE_HWP_STREAM__
                /**
                 * @brief Retrieve a stream of uncompressed data directly from the archive
                 *
                 * The file must be stored uncompressed for this to work. If the file is compressed this will fail.
                 *
                 * @param[out] o_stream Stream that will be initialized from the uncompressed file data
                 *
                 * @return     ARC_OPERATION_SUCCESSFUL        if no problem occurred
                 *             ARC_FUNCTIONALITY_NOT_SUPPORTED if the file is compressed
                 */
                ARC_RET_t get_stored_data_stream(fapi2::hwp_array_istream& o_stream);
#endif

#ifndef ARCHIVE_TEST_FIXTURE
            protected:
#endif
                friend class FileArchive;
                uint16_t iv_method;
                void*    iv_compressedData;
                uint32_t iv_compressedSize;
                uint32_t iv_uncompressedSize;

            private:
                ARC_RET_t stream_uncompressed_with_hash(StreamReceiver& i_receiver, void* i_scratch, sha3_t* o_hash);
        };

        /**
         * @brief Initialize a file archive from a known start address
         *
         * @param[in] i_address Location of the beginning of the archive in addressable memory
         */
        FileArchive(void* i_address) : iv_firstFile(i_address) {};

        /**
         * @brief Locate a file inside the archive
         *
         * @param[in]  i_fname Name of the file to locate
         * @param[out] o_entry Entry that will receive the file information if found
         *
         * @return     ARC_OPERATION_SUCCESSFUL        if the file was found and o_entry is valid
         *             ARC_FILE_NOT_FOUND              if the file could not be found in the archive
         *             ARC_FILE_CORRUPTED              if the archive appears corrupted
         *             ARC_FUNCTIONALITY_NOT_SUPPORTED if the compression method used for the file is not supported
         */
        ARC_RET_t locate_file(const char* i_fname, Entry& o_entry);

        /**
         * @brief Locate the end of the archive
         *
         * @return An 8-byte aligned pointer to the first memory location after the end of the archive
         */
        void* archive_end();

        /**
         * @brief Append an uncompressed file to the end of the archive
         *
         * @param[in] i_fname    Image filename
         * @param[in] i_data     Image pointer
         * @param[in] i_dataSize Image size in bytes
         * @param[in] i_archiveMaxSize FileArchive max size in bytes
         * @param[in/out] io_end ptr to end of archive + 1 (next free space)
         *                If NULL then will seek the end from the beginning
         * @return  ARC_...
         * @note No compression support at this time... Store only
         */
        ARC_RET_t append(const char* i_fname,
                         const void* i_data,
                         uint32_t i_dataSize,
                         size_t   i_archiveMaxSize,
                         void*& io_end);

        /**
         * @brief Initailize/reset the archive buffer to empty
         * @return ARC_OPERATION_SUCCESSFUL || ARC_FILE_CORRUPTED if internal buffer is not 8 byte aligned
         */
        ARC_RET_t initialize(void);

    private:
        void* iv_firstFile;

        /**
         * @brief Internal function used for both locate_file and archive_end
         */
        ARC_RET_t _locate_file(const char* i_fname, Entry* o_entry, void*& o_ptr);
};
