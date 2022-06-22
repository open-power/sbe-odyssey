/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/common/utils/imageProcs/archive-tests/util.C $ */
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
#include "util.H"

void* loadFile(string fname, size_t* size)
{
    FILE* f = fopen(fname.c_str(), "rb");

    if (!f)
    {
        error(1, errno, "Failed to open %s", fname.c_str());
    }

    fseek(f, 0, SEEK_END);
    size_t filesize = ftell(f);
    fseek(f, 0, SEEK_SET);

    if (size)
    {
        *size = filesize;
    }

    void* data = malloc(filesize);
    fread(data, filesize, 1, f);
    fclose(f);

    return data;
}

void saveFile(void* data, size_t size, string fname)
{
    FILE* f = fopen(fname.c_str(), "wb");

    if (!f)
    {
        error(1, errno, "Failed to open %s", fname.c_str());
    }

    fwrite(data, size, 1, f);
    fclose(f);
}

void checkFile(string fname, void* unpacked, FileArchive::Entry& entry, sha3_t* hash)
{
    size_t expect_size, size = entry.get_size();
    void* expect = loadFile(fname, &expect_size);

    if (expect_size != size)
    {
        error(1, 0, "File size mismatch: exp %d, got %d", expect_size, size);
    }

    if (memcmp(expect, unpacked, size))
    {
        saveFile(unpacked, size, fname + ".bad");
        error(1, 0, "File content mismatch, unpacked contents saved to %s.bad", fname.c_str());
    }

    if (hash)
    {
        sha3_ctx_t ctx;
        sha3_t expect_hash;

        sha3_init(&ctx);
        sha3_update(&ctx, entry.iv_compressedData, entry.iv_compressedSize);
        sha3_final(&expect_hash, &ctx);

        if (memcmp(hash, expect_hash, sizeof(expect_hash)))
        {
            error(1, 0, "Hash mismatch");
        }
    }
}
