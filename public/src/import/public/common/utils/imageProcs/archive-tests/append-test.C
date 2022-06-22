/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/common/utils/imageProcs/archive-tests/append-test.C $ */
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

void check(FileArchive& arc, const char* arcname, const char* fname)
{
    FileArchive::Entry entry;
    int rc = arc.locate_file(fname, entry);

    if (rc)
    {
        error(1, 0, "%s:%s: locateFile() failed: 0x%X", arcname, fname, rc);
    }

    void* unpacked = malloc(entry.get_size());
    rc = entry.decompress(unpacked, entry.get_size(), NULL);

    if (rc)
    {
        error(1, 0, "%s:%s: append then decompress() failed: 0x%X", arcname, fname, rc);
    }

    checkFile(fname, unpacked, entry, NULL);
    free(unpacked);
}

int main(int argc, char* argv[])
{
    if (argc < 5)
    {
        printf("Usage: %s basePak appendPak appendFile checkFile [checkFile...]\n", argv[0]);
        return 1;
    }

    const char* baseArchive = argv[1];
    const char* appendPak = argv[2];
    const char* appendFile = argv[3];

    char buf[2 * 1024 * 1024];
    size_t size;
    void* arcdata = loadFile(baseArchive, &size);
    memcpy(buf, arcdata, size);
    FileArchive arc(buf);

    // append entire archive
    void* appenddata = loadFile(appendPak, &size);
    ARC_RET_t rc;
    void* end = NULL;

    rc = arc.append_archive(appenddata, size, sizeof(buf), end);

    if (rc != ARC_OPERATION_SUCCESSFUL)
    {
        error(1, 0, "append_archive failed: 0x%X", rc);
    }

    // append single file
    appenddata = loadFile(appendFile, &size);
    rc = arc.append_file(appendFile, appenddata, size, sizeof(buf), end);

    if (rc != ARC_OPERATION_SUCCESSFUL)
    {
        error(1, 0, "append_file failed: 0x%X", rc);
    }

    for (int i = 4; i < argc; i++)
    {
        check(arc, baseArchive, argv[i]);
    }

    printf("append test OK\n");
    return 0;
}
