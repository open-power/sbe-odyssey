/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/common/utils/imageProcs/archive.C $  */
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
#include "archive.H"
#include <endian.h>

#ifdef __USE_COMPRESSION__
    #include "tinflate.H"
#endif


static const uint32_t PAK_START = 0x50414B21; // PAK!
static const uint32_t PAK_END = 0x2F50414B; // /PAK
static const uint8_t PAK_METHOD_STORE = 1;
static const uint8_t PAK_METHOD_ZLIB = 2;
static const uint8_t PAK_METHOD_ZLIB_FAST = 3;
static const uint8_t PAK_METHOD_ZLIB_PPC = 4;

// The header is comprised of two parts, the core and extended
// Must read the core first to check magic and get extended header size
struct PakFileHeaderCore
{
    uint32_t iv_magic;   //0
    uint16_t iv_version; //4
    uint16_t iv_hesize;  //6
} __attribute__((packed));
static_assert(sizeof(PakFileHeaderCore) == 8, "PakFileHeaderCore defined incorrectly, must be 8 bytes");

struct PakFileHeaderExtended
{
    uint8_t iv_flags;    //0
    uint8_t iv_method;   //1
    uint16_t iv_nsize;   //2
    uint32_t iv_crc;     //4
    uint32_t iv_csize;   //8
    uint32_t iv_dsize;   //12
    uint32_t iv_psize;   //16
    char* iv_name;       //20
} __attribute__((packed));

#ifdef __USE_SHA3__
ARC_RET_t FileArchive::Entry::stream_uncompressed_with_hash(StreamReceiver& i_receiver, void* i_scratch, sha3_t* o_hash)
{
    // If hashing is requested, we have to be paranoid and ensure that we hash exactly
    // what we're handing to the receiver, so we first copy the data into RAM
    // before we hash and process it. It's wasteful but safe.
    uint32_t remaining = iv_uncompressedSize;
    uint8_t* read_ptr = (uint8_t*)iv_compressedData;
    sha3_ctx_t ctx;

    sha3_init(&ctx);

    while (remaining)
    {
        uint32_t chunk_size = (remaining < DICTIONARY_SIZE) ? remaining : DICTIONARY_SIZE;

        memcpy(i_scratch, read_ptr, chunk_size);
        sha3_update(&ctx, i_scratch, chunk_size);
        ARC_RET_t rc = i_receiver.consume(i_scratch, chunk_size);

        if (rc)
        {
            return rc;
        }

        read_ptr += chunk_size;
        remaining -= chunk_size;
    }

    sha3_final(o_hash, &ctx);

    return ARC_OPERATION_SUCCESSFUL;
}
#endif

ARC_RET_t FileArchive::Entry::stream_decompress(StreamReceiver& i_receiver, void* i_scratch, sha3_t* o_hash)
{
    if (iv_method != PAK_METHOD_STORE)
    {
#ifdef __USE_COMPRESSION__
        const int filter = (iv_method == PAK_METHOD_ZLIB_PPC) ? TINF_FILTER_PPC : TINF_FILTER_NONE;
        return tinf_uncompress(i_scratch, STREAM_SCRATCH_SIZE, iv_compressedData, iv_compressedSize, &i_receiver, o_hash,
                               filter);
#else
        return ARC_FUNCTIONALITY_NOT_SUPPORTED;
#endif
    }
    else
    {
        if (o_hash)
        {
#ifdef __USE_SHA3__
            // Keep this in a separate function so we don't incur the stack space cost
            // of the SHA3 context unless we need to.
            return stream_uncompressed_with_hash(i_receiver, i_scratch, o_hash);
#else
            return ARC_FUNCTIONALITY_NOT_SUPPORTED;
#endif
        }
        else
        {
            return i_receiver.consume(iv_compressedData, iv_uncompressedSize);
        }
    }
}

#ifdef __USE_SHA3__

// Make sure this function is not inlined so we don't add the SHA3 context to each potential caller's stack
static __attribute__((noinline)) void hash_block(const void* i_data, uint32_t i_size, sha3_t* o_hash)
{
    sha3_ctx_t ctx;
    sha3_init(&ctx);
    sha3_update(&ctx, i_data, i_size);
    sha3_final(o_hash, &ctx);
}
#endif

ARC_RET_t FileArchive::Entry::decompress(void* o_buffer, uint32_t i_size, sha3_t* o_hash)
{
    if ((uintptr_t)o_buffer & 3)
    {
        ARC_ERROR("FileArchive::Entry::decompress: Output buffer is not 4-byte aligned - o_buffer=%p", o_buffer);
        return ARC_INVALID_PARAMS;
    }

    if (i_size < iv_uncompressedSize)
    {
        ARC_ERROR("FileArchive::Entry::decompress: Output buffer too small - i_size=%d iv_uncompressedSize=%d",
                  i_size, iv_uncompressedSize);
        return ARC_INPUT_BUFFER_OVERFLOW;
    }

    if (iv_method != PAK_METHOD_STORE)
    {
#ifdef __USE_COMPRESSION__
        const int filter = (iv_method == PAK_METHOD_ZLIB_PPC) ? TINF_FILTER_PPC : TINF_FILTER_NONE;
        return tinf_uncompress(o_buffer, i_size, iv_compressedData, iv_compressedSize, NULL, o_hash, filter);
#else
        return ARC_FUNCTIONALITY_NOT_SUPPORTED;
#endif
    }
    else
    {
        memcpy(o_buffer, iv_compressedData, iv_uncompressedSize);

        if (o_hash)
        {
#ifdef __USE_SHA3__
            // Keep this in a separate function so we don't incur the stack space cost
            // of the SHA3 context unless we need to.
            hash_block(o_buffer, iv_uncompressedSize, o_hash);
#else
            return ARC_FUNCTIONALITY_NOT_SUPPORTED;
#endif
        }

        return ARC_OPERATION_SUCCESSFUL;
    }
}

ARC_RET_t FileArchive::Entry::get_stored_data_ptr(const void*& o_buffer)
{
    if (iv_method != PAK_METHOD_STORE)
    {
        ARC_ERROR("Attempting to get stored data pointer for compressed file");
        return ARC_FUNCTIONALITY_NOT_SUPPORTED;
    }

    o_buffer = iv_compressedData;
    return ARC_OPERATION_SUCCESSFUL;
}

#ifdef __USE_HWP_STREAM__
ARC_RET_t FileArchive::Entry::get_stored_data_stream(fapi2::hwp_array_istream& o_stream)
{
    if (iv_method != PAK_METHOD_STORE)
    {
        ARC_ERROR("Attempting to get stored data pointer for compressed file");
        return ARC_FUNCTIONALITY_NOT_SUPPORTED;
    }

    o_stream = fapi2::hwp_array_istream(
                   (fapi2::hwp_data_unit*)iv_compressedData,
                   iv_compressedSize / sizeof(fapi2::hwp_data_unit));
    return ARC_OPERATION_SUCCESSFUL;
}
#endif

ARC_RET_t FileArchive::_locate_file(const char* i_fname, Entry* o_entry, void*& o_ptr)
{
    uint8_t* ptr = (uint8_t*)iv_firstFile;
    uint16_t fname_len = i_fname ? strlen(i_fname) : 0;

    while (true)
    {
        if (uintptr_t(ptr) & 7)
        {
            ARC_ERROR("Unaligned file header: %p", ptr);
            o_ptr = ptr;
            return ARC_FILE_CORRUPTED;
        }

        // Read just the first 8 bytes to check magic
        // This prevents any out of bound access just to read the whole header
        PakFileHeaderCore hdrc;
        memcpy(&hdrc, ptr, 8);
        // The pak layout is big endian
        hdrc.iv_magic = be32toh(hdrc.iv_magic);
        hdrc.iv_version = be16toh(hdrc.iv_version);
        hdrc.iv_hesize = be16toh(hdrc.iv_hesize);

        // Check the magic to make sure we are at the start of an entry
        if (hdrc.iv_magic == PAK_END)
        {
            if (i_fname)
            {
#if defined(__SBE_PPE__) || defined(__PPE_QME)
                ARC_ERROR_BIN("File not found:", i_fname, fname_len);
#else
                ARC_ERROR("File not found: %s", i_fname);
#endif
                return ARC_FILE_NOT_FOUND;
            }
            else
            {
                // The end magic is 4 bytes + 4 bytes of total size
                o_ptr = ptr + 8;
                return ARC_OPERATION_SUCCESSFUL;
            }
        }

        if (hdrc.iv_magic != PAK_START)
        {
            ARC_ERROR("Incorrect file header magic value: 0x%08X, expected: 0x%08X", hdrc.iv_magic, PAK_START);
            o_ptr = ptr;
            return ARC_FILE_CORRUPTED;
        }

        // The header core checked out, advance to the extended data
        ptr += 8;

        // Safe to read in the header extended
        // If the version were to change - that would be handled here
        // Do this in two pieces:
        // 1) Copy in the values for the fixed length vars
        // 2) Set iv_name to name start in ptr to avoid allocating a new copy of the name
        PakFileHeaderExtended hdre;
        size_t hdreFixedLen = 20;
        memcpy(&hdre, ptr, hdreFixedLen);
        hdre.iv_name = (char*)(ptr + hdreFixedLen);
        // The pak layout is big endian
        hdre.iv_nsize = be16toh(hdre.iv_nsize);
        hdre.iv_crc = be32toh(hdre.iv_crc);
        hdre.iv_csize = be32toh(hdre.iv_csize);
        hdre.iv_dsize = be32toh(hdre.iv_dsize);
        hdre.iv_psize = be32toh(hdre.iv_psize);

        // Advance over the extended header to the data
        ptr += hdrc.iv_hesize;

        /*
         * The file name in the pak header is not zero-terminated
         * Use the length first and then memcmp to find the file
         */
        if (i_fname && (hdre.iv_nsize == fname_len)
            && !memcmp(hdre.iv_name, i_fname, fname_len))
        {
#ifndef __USE_COMPRESSION__

            if(hdre.iv_method == PAK_METHOD_ZLIB || hdre.iv_method == PAK_METHOD_ZLIB_FAST
               || hdre.iv_method == PAK_METHOD_ZLIB_PPC)
            {
                ARC_ERROR("Archive is compressed, "
                          "but compression support is not enabled");
                return ARC_FUNCTIONALITY_NOT_SUPPORTED;
            }

            if (hdre.iv_method != PAK_METHOD_STORE)
#else
            if (hdre.iv_method != PAK_METHOD_STORE && hdre.iv_method != PAK_METHOD_ZLIB
                && hdre.iv_method != PAK_METHOD_ZLIB_FAST && hdre.iv_method != PAK_METHOD_ZLIB_PPC)
#endif
            {
                ARC_ERROR("Unsupported compression format: 0x%04x", hdre.iv_method);
                return ARC_FUNCTIONALITY_NOT_SUPPORTED;
            }

            if (uintptr_t(ptr) & 7)
            {
                ARC_ERROR("Unaligned compressed data: %p", ptr);
                return ARC_FILE_CORRUPTED;
            }

            if (hdre.iv_psize & 7)
            {
                ARC_ERROR("Unaligned payload size: %d", hdre.iv_dsize);
                return ARC_FILE_CORRUPTED;
            }

            // Everything checks out, setup the return data
            o_entry->iv_method = hdre.iv_method;
            o_entry->iv_compressedData = ptr;
            o_entry->iv_compressedSize = hdre.iv_csize;
            o_entry->iv_uncompressedSize = hdre.iv_dsize;
            // Advance to the end of the entry
            o_ptr = ptr + hdre.iv_psize;
            return ARC_OPERATION_SUCCESSFUL;
        }

        // Not the right file, skip to the next header
        ptr += hdre.iv_psize;
    }
}

ARC_RET_t FileArchive::locate_file(const char* i_fname, Entry& o_entry)
{
    void* dummy;
    return _locate_file(i_fname, &o_entry, dummy);
}

void* FileArchive::archive_end()
{
    void* end;
    _locate_file(NULL, NULL, end);
    return end;
}
