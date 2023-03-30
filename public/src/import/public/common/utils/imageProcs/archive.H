/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/common/utils/imageProcs/archive.H $  */
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
#pragma once

#include <stdint.h>
#include <endian.h>
#include "archive_config.H"

#ifdef __USE_HWP_STREAM__
    #include "hwp_data_stream.H"
#endif

static const uint32_t PAK_START = 0x50414B21; // PAK!
static const uint32_t PAK_PAD = 0x50414B50; // PAKP
static const uint32_t PAK_END = 0x2F50414B; // /PAK
static const uint16_t PAK_VERSION = 1;

// The header is comprised of two parts, the core and extended
// Must read the core first to check magic and get extended header size
struct PakFileHeaderCore
{
    uint32_t iv_magic;   //0
    union
    {
        // Depending on the magic value this is either a file header or an end marker.
        // If it's an end marker the next 4 bytes denote the total size of the archive.
        uint32_t iv_asize;  // total archive length in bytes, including end marker

        // If it's a file header the next four bytes indicate format version and
        // the size of the extended header that follows.
        struct
        {
            uint16_t iv_version; //4
            uint16_t iv_hesize;  //6   Size of extended header
        };

        // If it's a padding header the next 4 bytes indicate the amount of bytes to
        // skip after the core header.
        uint32_t iv_padsize;  // Size of padding (not including core header)
    };
} __attribute__((packed));
static_assert(sizeof(PakFileHeaderCore) == 8, "PakFileHeaderCore defined incorrectly, must be 8 bytes");

/**
 * @brief Append one archive to another
 *
 * Given two existing valid PAK archives in memory, append the second to the first so
 * that the first archive becomes a valid PAK archive which contains the union of the
 * files in both source archives.
 *
 * This _may_ be used by user code but the main purpose is to provide a template for
 * user code to re-implement this concatenation using its own data move operations,
 * e.g. if archive data is being streamed through a FIFO.
 *
 * @param[inout] io_end_of_archive  Points to the _end_ of the first archive coming into
 *                                  the function. Will be updated to point to the end of
 *                                  the combined archive afterwards. There must be ample
 *                                  memory space available after the end of the first archive
 *                                  to accommodate the second archive.
 * @param[in]    i_start_of_second_archive  Points to the beginning of the second archive
 *                                          which is to be appeneded to the first.
 * @param[in]    i_size_of_second_archive   Size of the second archive in bytes.
 * @return  ARC_OPERATION_SUCCESSFUL or ARC_FILE_CORRUPTED
 */
static inline ARC_RET_t append_archive(void*& io_end_of_archive,
                                       void* i_start_of_second_archive,
                                       size_t i_size_of_second_archive)
{
    // Rewind to the beginning of the first archive's end marker
    PakFileHeaderCore* l_first_end_marker = ((PakFileHeaderCore*)io_end_of_archive) - 1;

    // Check that we really got handed the end of a valid archive
    if (l_first_end_marker->iv_magic != htobe32(PAK_END))
    {
        return ARC_FILE_CORRUPTED;
    }

    // Store the size of the first archive, minus the end marker
    const uint32_t l_first_size = l_first_end_marker->iv_asize - sizeof(PakFileHeaderCore);

    // Write the second archive, overwriting the first one's end marker
    memcpy(l_first_end_marker, i_start_of_second_archive, i_size_of_second_archive);

    // Determine location of new end marker
    uint8_t* l_new_end_of_archive = (uint8_t*)l_first_end_marker + i_size_of_second_archive;
    PakFileHeaderCore* l_new_end_marker = ((PakFileHeaderCore*)l_new_end_of_archive) - 1;

    // Check that we really got handed a valid second archive
    if (l_new_end_marker->iv_magic != htobe32(PAK_END))
    {
        return ARC_FILE_CORRUPTED;
    }

    // Update archive size and end pointer
    l_new_end_marker->iv_asize += l_first_size;
    io_end_of_archive = l_new_end_of_archive;

    // Great Success!  d-(^.^)-b
    return ARC_OPERATION_SUCCESSFUL;
}

/**
 * @brief enum of archive flags the caller can specify for different behaviors
 * @note  should be a one-hot bitfield when new flags are added.
 */
enum ArchiveFlags_t : uint32_t
{
    ARCHIVE_FLAGS_NONE            = 0x00000000,  //< Default, no flags
    ARCHIVE_FLAGS_NOT_FOUND_QUIET = 0x00000001,  //< Don't log ARC_ERROR message when ARC_FILE_NOT_FOUND
};

static const uint8_t PAK_METHOD_STORE = 1;
static const uint8_t PAK_METHOD_ZLIB = 2;
static const uint8_t PAK_METHOD_ZLIB_FAST = 3;
static const uint8_t PAK_METHOD_ZLIB_PPC = 4;

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
                 * @brief Returns true if the file is compressed
                 */
                __attribute__((always_inline))
                bool is_compressed()
                {
                    return iv_method != PAK_METHOD_STORE;
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
                uint32_t iv_uncompressedSize;
                void*    iv_compressedData;
                uint32_t iv_compressedSize;

            private:
                ARC_RET_t stream_uncompressed_with_hash(StreamReceiver& i_receiver, void* i_scratch, sha3_t* o_hash);
        };

        /**
         * @brief Initialize a file archive from a known start address
         *
         * @param[in] i_address Location of the beginning of the archive in addressable memory
         * @param[in] i_limit   Maximum expected address range for this archive.
         */
        FileArchive(void* i_address, void* i_limit = NULL) : iv_firstFile(i_address), iv_archiveLimit(i_limit) {};

        /**
         * @brief Locate a file inside the archive
         *
         * @param[in]  i_fname Name of the file to locate
         * @param[out] o_entry Entry that will receive the file information if found
         * @param[in]  i_flags [optional] Flags that can change archive behavior - defaults to none
         *
         * @return     ARC_OPERATION_SUCCESSFUL        if the file was found and o_entry is valid
         *             ARC_FILE_NOT_FOUND              if the file could not be found in the archive
         *             ARC_FILE_CORRUPTED              if the archive appears corrupted
         *             ARC_FUNCTIONALITY_NOT_SUPPORTED if the compression method used for the file is not supported
         */
        ARC_RET_t locate_file(const char* i_fname, Entry& o_entry, uint32_t i_flags = 0);

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
        ARC_RET_t append_file(const char* i_fname,
                              const void* i_data,
                              uint32_t i_dataSize,
                              size_t   i_archiveMaxSize,
                              void*& io_end);

        /**
         * @brief Append a full archive to this one
         *
         * Append an entire PAK archive to this one so that in the end this archive contains
         * the union of its own and the second archive's files.
         * Note that if both archives contain files with the same name,
         * the first archive wins and the appended archive's file will be ignored.
         *
         * @param[in]    i_archive        Beginning of the archive to be appended.
         * @param[in]    i_archiveSize    Size of the archive to be appended in bytes.
         * @param[in]    i_archiveMaxSize Max size for the combined archive.
         * @param[inout] io_end           Pointer to end of archive + 1 (next free space).
         *                                If NULL then will seek the end from the beginning.
         * @return  ARC_...
         */
        ARC_RET_t append_archive(void* i_archive, size_t i_archiveSize,
                                 size_t i_archiveMaxSize, void*& io_end);

        /**
         * @brief Initialize/reset the archive buffer to empty
         * @return ARC_OPERATION_SUCCESSFUL || ARC_FILE_CORRUPTED if internal buffer is not 8 byte aligned
         */
        ARC_RET_t initialize(void);

    private:
        void* iv_firstFile;
        void* iv_archiveLimit;

        /**
         * @brief Internal function used for both locate_file and archive_end
         */
        ARC_RET_t _locate_file(const char* i_fname, Entry* o_entry, void*& o_ptr, uint32_t i_flags = 0);

        /**
         * @brief Internal function used by append_*
         */
        ARC_RET_t _append_find_end(void*& io_end, size_t i_archiveMaxSize);
};
