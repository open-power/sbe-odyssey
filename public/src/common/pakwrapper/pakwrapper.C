/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/common/pakwrapper/pakwrapper.C $                   */
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

#include "pakwrapper.H"

ARC_RET_t PakWrapper::read_file(const char* i_fileName, void* i_destinationAddr, uint32_t i_destinationBufferSize, sha3_t* o_hash, uint32_t *o_uncompressedSize)
{
    ARC_RET_t rc = ARC_INVALID_PARAMS;
    if((i_fileName != nullptr) || (i_destinationAddr != nullptr))
    {
        rc = locate_file(i_fileName, fileArchiveEntry);

        if (rc != ARC_OPERATION_SUCCESSFUL)
        {
            ARC_ERROR(" Pak Read Failed. Rc: %x ", rc);
        }
        else
        {
            rc = fileArchiveEntry.decompress(i_destinationAddr,
                    i_destinationBufferSize, static_cast<sha3_t *>(o_hash));
            if (rc != ARC_OPERATION_SUCCESSFUL)
            {
                ARC_ERROR(" Pak Read Failed. Rc: %x ", rc);
            }
            else
            {
                if(o_uncompressedSize)
                    *o_uncompressedSize = fileArchiveEntry.get_size();
            }
        }
    }
    return rc;
}

ARC_RET_t PakWrapper::get_image_start_ptr_and_size(const char* i_fileName, uint32_t** o_imageStartPtr, uint32_t *o_uncompressedSize)
{
    ARC_RET_t rc = ARC_INVALID_PARAMS;

    if(i_fileName != nullptr)
    {
        //Locate the file
        rc = locate_file(i_fileName, fileArchiveEntry);
        if (rc != ARC_OPERATION_SUCCESSFUL)
        {
            ARC_ERROR(" locate_file failed with Rc: %x ", rc);
        }
        else
        {
            if(o_imageStartPtr)
            {
                //Get the stored pointer (this will be success only if file is uncompressed)
                const void* ptr;
                rc = get_stored_data_ptr(ptr);
                if (rc != ARC_OPERATION_SUCCESSFUL)
                {
                    ARC_ERROR(" get_stored_data_ptr failed with Rc: %x ", rc);
                }
                else
                {
                    *o_imageStartPtr = (uint32_t*)ptr;
                }
            }

            if(o_uncompressedSize)
                *o_uncompressedSize = fileArchiveEntry.get_size();
        }
    }
    return rc;
}

#ifndef __SROM_IMAGE__
uint32_t PakWrapper::stream_file(const char* i_fileName,
            void* i_destinationAddr, uint32_t i_destinationBufferSize, void* o_hash)
{
    ARC_RET_t rc = ARC_INVALID_PARAMS;
    if((i_fileName != nullptr) || (i_destinationAddr != nullptr))
    {
        if ((uintptr_t)(i_destinationAddr) & 0x03)
        {
            ARC_ERROR("Destination buffer is not 4-byte aligned - \
                    i_destinationAddr=%p", i_destinationAddr);
            rc = ARC_INVALID_PARAMS;
        }
        else
        {
            FileArchive::Entry fileArchiveEntry;
            rc = locate_file(i_fileName, fileArchiveEntry);
            if (rc != ARC_OPERATION_SUCCESSFUL)
            {
                ARC_ERROR(" Pak Read Failed. Rc: %x ", rc);
            }
            else
            {
                if(i_destinationBufferSize >= fileArchiveEntry.get_size())
                {
                    uint8_t scratch[FileArchive::STREAM_SCRATCH_SIZE];
                    PakStreamReceiver streamReceiver(i_destinationAddr,
                                                fileArchiveEntry.get_size());
                    rc = fileArchiveEntry.stream_decompress(streamReceiver,
                                                        scratch, static_cast<sha3_t *>(o_hash));
                    if(rc != ARC_OPERATION_SUCCESSFUL)
                    {
                        ARC_ERROR(" Pak Read Failed. Rc: %x ", rc);
                    }
                }
                else
                {
                    rc = ARC_INPUT_BUFFER_OVERFLOW;
                }
            }
        }
    }
    return rc;
}

ARC_RET_t PakStreamReceiver::consume(const void* i_data, uint32_t i_size)
{
    ARC_RET_t rc = ARC_OPERATION_SUCCESSFUL;
    if (i_size > iv_decompressedSize)
    {
        ARC_ERROR("Size is greater than decompressed size =%p =%p",
                                        iv_decompressedSize, i_size);
        rc = ARC_INVALID_PARAMS;
    }
    else
    {
        memcpy(iv_destinationDataPtr, i_data, i_size);
        iv_destinationDataPtr += i_size;
        iv_decompressedSize -= i_size;
    }
    return rc;
}
#endif
