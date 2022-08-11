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

#ifdef __USE_COMPRESSION__
    #include "tinflate.H"
#endif


static const uint8_t PAK_METHOD_STORE = 1;
static const uint8_t PAK_METHOD_ZLIB = 2;
static const uint8_t PAK_METHOD_ZLIB_FAST = 3;
static const uint8_t PAK_METHOD_ZLIB_PPC = 4;

struct PakFileHeaderExtended
{
    uint8_t iv_flags;    //0
    uint8_t iv_method;   //1
    uint16_t iv_nsize;   //2    name size
    uint32_t iv_crc;     //4
    uint32_t iv_csize;   //8    Compressed size
    uint32_t iv_dsize;   //12   Uncompressed size
    uint32_t iv_psize;   //16   Payload size
    char     iv_name[0];       //20
} __attribute__((packed));

const size_t PakHeaderExtendedSize = sizeof(PakFileHeaderExtended);

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
    uint8_t* ptr = static_cast<uint8_t*>(iv_firstFile);
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
        union
        {
            // Make this a union of the actual header struct and a uint64_t so we can
            // leverage the PPE's lvd/stvd instructions to fetch the header from flash.
            PakFileHeaderCore h;
            uint64_t u;
        } hdrc __attribute__((aligned(8)));

        // Using a volatile pointer forces the PPE compiler to emit lvd/stvd
        hdrc.u = *(volatile uint64_t*)ptr;

        // The pak layout is big endian
        hdrc.h.iv_magic = be32toh(hdrc.h.iv_magic);

        // Check the magic to make sure we are at the start of an entry
        if (hdrc.h.iv_magic == PAK_END)
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

        if (hdrc.h.iv_magic == PAK_PAD)
        {
            hdrc.h.iv_padsize = be32toh(hdrc.h.iv_padsize);
            ptr += hdrc.h.iv_padsize + 8;
            continue;
        }

        if (hdrc.h.iv_magic != PAK_START)
        {
            ARC_ERROR("Incorrect file header magic value: 0x%08X, expected: 0x%08X", hdrc.h.iv_magic, PAK_START);
            o_ptr = ptr;
            return ARC_FILE_CORRUPTED;
        }

        hdrc.h.iv_version = be16toh(hdrc.h.iv_version);
        hdrc.h.iv_hesize = be16toh(hdrc.h.iv_hesize);

        // The header core checked out, advance to the extended data
        ptr += 8;

        // Safe to read in the header extended
        // If the version were to change - that would be handled here
        // Do this in two pieces:
        // 1) Copy in the values for the fixed length vars
        // 2) Set name to name start in ptr to avoid allocating a new copy of the name
        PakFileHeaderExtended hdre;
        memcpy(&hdre, ptr, PakHeaderExtendedSize);
        char* name = (char*)(ptr + PakHeaderExtendedSize);
        // The pak layout is big endian
        hdre.iv_nsize = be16toh(hdre.iv_nsize);
        hdre.iv_crc = be32toh(hdre.iv_crc);
        hdre.iv_csize = be32toh(hdre.iv_csize);
        hdre.iv_dsize = be32toh(hdre.iv_dsize);
        hdre.iv_psize = be32toh(hdre.iv_psize);

        // Advance over the extended header to the data
        ptr += hdrc.h.iv_hesize;

        /*
         * The file name in the pak header is not zero-terminated
         * Use the length first and then memcmp to find the file
         */
        if (i_fname && (hdre.iv_nsize == fname_len)
            && !memcmp(name, i_fname, fname_len))
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

ARC_RET_t FileArchive::initialize(void)
{
    ARC_RET_t rc = ARC_OPERATION_SUCCESSFUL;

    if (uintptr_t(iv_firstFile) & 7)
    {
        ARC_ERROR("initialize: Unaligned file header: %p", iv_firstFile);
        return ARC_FILE_CORRUPTED;
    }

    PakFileHeaderCore* hdrc = reinterpret_cast<PakFileHeaderCore*>(iv_firstFile);
    hdrc->iv_magic = htobe32(PAK_END);
    hdrc->iv_asize = sizeof(PakFileHeaderCore);
    return rc;
}

ARC_RET_t FileArchive::_append_find_end(void*& io_end, size_t i_archiveMaxSize)
{
    if(io_end == iv_firstFile)
    {
        // uninitialized archive, point behind end marker
        io_end = static_cast<uint8_t*>(io_end) + sizeof(PakFileHeaderCore);
        return ARC_OPERATION_SUCCESSFUL;
    }
    else if(io_end == NULL)
    {
        // locate the end
        ARC_RET_t rc = _locate_file(NULL, NULL, io_end);

        if(rc != ARC_OPERATION_SUCCESSFUL)
        {
            return rc;
        }

        if(static_cast<uint8_t*>(io_end) == iv_firstFile)
        {
            return ARC_FILE_CORRUPTED;
        }
    }
    else
    {
        if(io_end < iv_firstFile ||
           io_end > (static_cast<uint8_t*>(iv_firstFile) + i_archiveMaxSize))
        {
            ARC_ERROR("FileArchive::append: io_end is not within archive");
            return ARC_INVALID_PARAMS;
        }
    }

    return ARC_OPERATION_SUCCESSFUL;
}

/**
 * append_file - store an uncompressed image(file) at the end of the archive
 * @param[in] Image filename
 * @param[in] Image pointer
 * @param[in] Image size in bytes
 * @param[in] FileArchive max size
 * @param[out] New FileArchive size
 * @returns error code
 */

ARC_RET_t FileArchive::append_file(const char* i_fname,
                                   const void* i_data,
                                   uint32_t i_dataSize,
                                   size_t i_archiveMaxSize,
                                   void*& io_end)
{
    ARC_RET_t rc = ARC_OPERATION_SUCCESSFUL;

    if(i_fname == NULL || i_data == NULL || i_dataSize == 0)
    {
        // Note: There seems to be a weird bug in PPE trace macro expansion,
        // where two of "%p" in sequence causes a problem, but putting
        // something between them (like %x) resolves it.
        // So the order is here deliberate.
        ARC_ERROR("append_file: Invalid parms. i_data: %p i_dataSize: %x i_fname: %p",
                  i_data, i_dataSize, i_fname);
        return ARC_INVALID_PARAMS;
    }

    uint32_t name_len = strlen(i_fname);

    if(name_len == 0)
    {
        ARC_ERROR("append_file: file name length is zero");
        return ARC_INVALID_PARAMS;
    }

    rc = _append_find_end(io_end, i_archiveMaxSize);

    if (rc != ARC_OPERATION_SUCCESSFUL)
    {
        return rc;
    }

    uint8_t* next_entry = static_cast<uint8_t*>(io_end) - sizeof(PakFileHeaderCore);

    // Get sizes
    uint32_t paySize = (i_dataSize + 7) & ~(0x7); // payload size must by multiple of 8

    uint32_t hesize = PakHeaderExtendedSize + name_len;
    hesize = (hesize + 7) & ~(0x7); // multiple of 8
    uint32_t entry_size = hesize + sizeof(PakFileHeaderCore) + paySize;

    // Check for overflow
    if(((next_entry - static_cast<uint8_t*>(iv_firstFile)) + entry_size + sizeof(PakFileHeaderCore) ) > i_archiveMaxSize)
    {
        ARC_ERROR("FileArchive::append_file: Append would overflow archive");
        return ARC_INPUT_BUFFER_OVERFLOW;
    }

    PakFileHeaderCore* hdrc = reinterpret_cast<PakFileHeaderCore*>(next_entry);
    hdrc->iv_magic = htobe32(PAK_START);
    hdrc->iv_version = htobe16(PAK_VERSION);
    hdrc->iv_hesize = htobe16(hesize);

    PakFileHeaderExtended* hdex = reinterpret_cast<PakFileHeaderExtended*>(next_entry + sizeof(PakFileHeaderCore));
    hdex->iv_flags = 0;
    hdex->iv_method = PAK_METHOD_STORE;
    hdex->iv_nsize = htobe16(name_len);
    hdex->iv_crc = 0;  // TBD Can zero mean 'don't check'?
    hdex->iv_csize = htobe32(i_dataSize);
    hdex->iv_dsize = htobe32(i_dataSize);
    hdex->iv_psize = htobe32(paySize);

    reinterpret_cast<uint64_t*>(hdex)[(hesize / 8) - 1] = 0; // padding at end of fname

    memcpy(hdex->iv_name, i_fname, name_len);

    uint8_t* payloadDest = next_entry + hesize + sizeof(PakFileHeaderCore);
    memcpy(payloadDest, i_data, i_dataSize);

    // add new end marker
    hdrc = reinterpret_cast<PakFileHeaderCore*>(payloadDest + paySize);
    hdrc->iv_magic = htobe32(PAK_END);

    io_end = reinterpret_cast<uint8_t*>(hdrc) + sizeof(PakFileHeaderCore);

    hdrc->iv_asize = (uint32_t)(reinterpret_cast<uint8_t*>(io_end) - reinterpret_cast<uint8_t*>(iv_firstFile));

    return rc;
}

ARC_RET_t FileArchive::append_archive(void* i_archive, size_t i_archiveSize,
                                      size_t i_archiveMaxSize, void*& io_end)
{
    ARC_RET_t rc = _append_find_end(io_end, i_archiveMaxSize);

    if (rc != ARC_OPERATION_SUCCESSFUL)
    {
        return rc;
    }

    // Check for overflow
    if(((static_cast<uint8_t*>(io_end) - static_cast<uint8_t*>(iv_firstFile)) + i_archiveSize) > i_archiveMaxSize)
    {
        ARC_ERROR("FileArchive::append_archive: Append would overflow archive");
        return ARC_INPUT_BUFFER_OVERFLOW;
    }

    return ::append_archive(io_end, i_archive, i_archiveSize);
}
