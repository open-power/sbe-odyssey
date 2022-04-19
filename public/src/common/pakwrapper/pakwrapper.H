/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/common/pakwrapper/pakwrapper.H $                   */
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
/**
 * @brief Wrapper class for one time and stream decompression and hash
 * calculation.
 */
class PakWrapper
{
    public:
        /**
         * @brief Initialize a Pak updater class with partition address.
         *
         * @param[in] i_partitionAddress Partition address to update from.
         */
        PakWrapper(void* i_partitionAddress) :
                    iv_fileArchive(i_partitionAddress),
                    fileArchiveEntry()
        {
        };

        /**
         * @brief Constructor for PakWrapper class.
         *
         * @param[in] i_fileName Pointer to file name to look for.
         * @param[in] i_destinationAddr Destination buffer address.
         * @param[in] i_destinationBufferSize Size of destination buffer.
         * @param[out] o_hash Pointer to sha3_t, if NULL no hash is returned else hash is returned.
         * @param[out] o_uncompressedSize Pointer to uncompressed file size.
         */
        ARC_RET_t read_file(const char* i_fileName, void* i_destinationAddr, uint32_t i_destinationBufferSize, void* o_hash, uint32_t *o_uncompressedSize);


        __attribute__((always_inline))
        uint32_t get_size()
        {
            return fileArchiveEntry.get_size();
        }

        __attribute__((always_inline))
        ARC_RET_t get_stored_data_ptr(const void*& o_buffer)
        {
            return fileArchiveEntry.get_stored_data_ptr(o_buffer);
        }

#ifndef __SROM_IMAGE__
        /**
         * @brief Wrapper function to stream decompress pak data into
         *        destination address.
         *
         * @param[in] i_fileName Pointer to file name to look for.
         * @param[in] i_destinationAddr Destination buffer address.
         * @param[in] i_destinationBufferSize Size of destination buffer.
         * @param[out] o_hash Pointer to sha3_t, if NULL no hash is returned
         *             else hash is returned.
         */
        uint32_t stream_file(const char* i_fileName, void* i_destinationAddr,
                            uint32_t i_destinationBufferSize, void* o_hash);
#endif
    private:
        FileArchive iv_fileArchive;
        FileArchive::Entry fileArchiveEntry;
};

#ifndef __SROM_IMAGE__
/**
 * @brief Implementation of StreamReceiver interface class for stream
 *        decompression.
 */
class PakStreamReceiver : public FileArchive::StreamReceiver
{
    public:
        /**
         * @brief Constructor for PakStreamReceiver class.
         *
         * @param[in] i_destinationDataPtr Pointer to destination buffer.
         * @param[in] i_decompressedSize Size of decompressed data.
         */
        PakStreamReceiver(void* i_destinationDataPtr,
                                size_t i_decompressedSize) :
                        iv_destinationDataPtr((uint8_t*)i_destinationDataPtr),
                        iv_decompressedSize(i_decompressedSize)
                        {
                        }

        int consume(const void* i_data, uint32_t i_size);
    private:
        uint8_t* iv_destinationDataPtr;
        size_t iv_decompressedSize;
};
#endif
