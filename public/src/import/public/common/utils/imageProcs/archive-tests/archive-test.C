/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/common/utils/imageProcs/archive-tests/archive-test.C $ */
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

#define FLAG_STREAM 1
#define FLAG_HASH   2

class TestReceiver : public FileArchive::StreamReceiver
{
    public:
        TestReceiver(void* buffer, size_t size) : ptr((uint8_t*)buffer), space(size) {}
        virtual int consume(const void* i_data, uint32_t i_size);
    private:
        uint8_t* ptr;
        size_t space;
};

ARC_RET_t TestReceiver::consume(const void* i_data, uint32_t i_size)
{
//    printf("consume %p %d <- %p %d\n", ptr, space, i_data, i_size);
    if (i_size > space)
    {
        error(1, 0, "Stream buffer overflow: %d > %d", i_size, space);
    }

    memcpy(ptr, i_data, i_size);
    ptr += i_size;
    space -= i_size;

    return ARC_OPERATION_SUCCESSFUL;
}

int main(int argc, char* argv[])
{
    const char* archiveName = argv[1];
    const char* fileName = argv[2];
    const int flags = (argc < 4) ? 0 : atoi(argv[3]);

    if (argc < 3)
    {
        printf("Usage: %s zipfile subfile [flags]\n", argv[0]);
        printf("  flags are:\n");
        printf("  1 - stream (otherwise decompress in one go)\n");
        printf("  2 - hash\n");
        return 1;
    }

    void* arcdata = loadFile(argv[1]);
    FileArchive arc(arcdata);

    FileArchive::Entry entry;
    int rc = arc.locate_file(argv[2], entry);

    if (rc)
    {
        error(1, 0, "%s:%s: locateFile() failed: 0x%X", argv[1], argv[2], rc);
    }

    sha3_t hash;
    sha3_t* phash = (flags & FLAG_HASH) ? &hash : NULL;

    void* unpacked = malloc(entry.get_size());

    if (flags & FLAG_STREAM)
    {
        TestReceiver rcv(unpacked, entry.get_size());
        uint8_t scratch[FileArchive::STREAM_SCRATCH_SIZE];
        rc = entry.stream_decompress(rcv, scratch, phash);

        if (rc)
        {
            error(1, 0, "%s:%s: stream_decompress() failed: 0x%X", argv[1], argv[2], rc);
        }
    }
    else
    {
        rc = entry.decompress(unpacked, entry.get_size(), phash);

        if (rc)
        {
            error(1, 0, "%s:%s: decompress() failed: 0x%X", argv[1], argv[2], rc);
        }
    }

    checkFile(argv[2], unpacked, entry, phash);

    printf("%s:%s OK\n", argv[1], argv[2]);
    return 0;
}
