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

struct uint16le
{
    uint16_t v;
    uint16_t value() const
    {
        return le16toh(v);
    }
};

struct uint32le
{
    uint32_t v;
    uint32_t value() const
    {
        return le32toh(v);
    }
};

struct unaligned_uint32le
{
    uint16_t lo, hi;
    uint32_t value() const
    {
        return (uint32_t(le16toh(hi)) << 16) | le16toh(lo);
    }
};

static const uint32_t ZIPFILE_MAGIC = 0x04034B50;
static const uint32_t END_MAGIC = 0xFFFE4B50;
static const uint16_t METHOD_STORE = 0;
static const uint16_t METHOD_DEFLATE = 8;
static const uint16_t METHOD_DEFLATEPPC = 0x0108;

struct ZipFileHeader
{
    uint32le iv_magic;  //0
    uint16le iv_version; //4
    uint16le iv_flags;   //6
    uint16le iv_method;  //8
    uint16le iv_mtime;   //10
    uint16le iv_mdate;   //12
    unaligned_uint32le iv_crc32;    //14
    unaligned_uint32le iv_compsize; //18
    unaligned_uint32le iv_uncompsize; //22
    uint16le iv_fnlen; //26
    uint16le iv_exlen; //28
    char iv_fname[0];  //30
} __attribute__((packed));
static_assert(sizeof(ZipFileHeader) == 30, "ZipFileHeader defined incorrectly, must be 30 bytes");

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
    if (iv_method != METHOD_STORE)
    {
#ifdef __USE_COMPRESSION__
        const int filter = (iv_method == METHOD_DEFLATEPPC) ? TINF_FILTER_PPC : TINF_FILTER_NONE;
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

    if (iv_method != METHOD_STORE)
    {
#ifdef __USE_COMPRESSION__
        const int filter = (iv_method == METHOD_DEFLATEPPC) ? TINF_FILTER_PPC : TINF_FILTER_NONE;
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
    if (iv_method != METHOD_STORE)
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
    if (iv_method != METHOD_STORE)
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

        const ZipFileHeader* hdr = (ZipFileHeader*)ptr;
        const uint32_t magic = hdr->iv_magic.value();

        if (magic == END_MAGIC)
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
                o_ptr = ptr + 4;
                return ARC_OPERATION_SUCCESSFUL;
            }
        }

        if (magic != ZIPFILE_MAGIC)
        {
            ARC_ERROR("Incorrect file header magic value: 0x%08X", magic);
            o_ptr = ptr;
            return ARC_FILE_CORRUPTED;
        }

        const uint32_t hdr_len = sizeof(*hdr) + hdr->iv_fnlen.value() + hdr->iv_exlen.value();
        const uint32_t compsize = hdr->iv_compsize.value();
        ptr += hdr_len;

        /*
         * The file name in the ZIP header is not zero-terminated,
         * so we first compare the name length and if that matches
         * we use memcmp.
         */
        if (i_fname && (hdr->iv_fnlen.value() == fname_len)
            && !memcmp(hdr->iv_fname, i_fname, fname_len))
        {
            const uint16_t method = hdr->iv_method.value();
#ifndef __USE_COMPRESSION__

            if(method == METHOD_DEFLATE || method == METHOD_DEFLATEPPC)
            {
                ARC_ERROR("Archive is compressed, "
                          "but compression support is not enabled");
                return ARC_FUNCTIONALITY_NOT_SUPPORTED;
            }

            if (method != METHOD_STORE)
#else
            if (method != METHOD_STORE && method != METHOD_DEFLATE
                && method != METHOD_DEFLATEPPC)
#endif
            {
                ARC_ERROR("Unsupported compression format: 0x%04x", method);
                return ARC_FUNCTIONALITY_NOT_SUPPORTED;
            }

            if (uintptr_t(ptr) & 7)
            {
                ARC_ERROR("Unaligned compressed data: %p", ptr);
                return ARC_FILE_CORRUPTED;
            }

            if (compsize & 7)
            {
                ARC_ERROR("Unaligned compressed size: %d", compsize);
                return ARC_FILE_CORRUPTED;
            }

            o_entry->iv_method = method;
            o_entry->iv_compressedData = ptr;
            o_entry->iv_compressedSize = compsize;
            o_entry->iv_uncompressedSize = hdr->iv_uncompsize.value();
            return ARC_OPERATION_SUCCESSFUL;
        }

        // Not the right file, skip to the next header
        ptr += compsize;
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
