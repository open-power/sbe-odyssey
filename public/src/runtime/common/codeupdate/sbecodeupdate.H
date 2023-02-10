/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/common/codeupdate/sbecodeupdate.H $        */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2015,2023                        */
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
#ifndef SBE_CODE_UPDATE_H
#define SBE_CODE_UPDATE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "pk_api.h"
#include "filenames.H"

#ifdef __cplusplus
}
#endif
#include "sbe_sp_intf.H"

#define NOT_USED 0xFFFFFFFF

// CU: Code Update
namespace CU
{
    /*
     * @brief Structure for images that are updatable
     */
    typedef struct
    {
        CU_IMAGES imageType;
        // hash of entire pak file (incl header minus image.hash file)
        uint64_t  imageHashSHA3_512[8];
    }imageUpdate_t;

    /*
     * @brief Structure for image properties
     */
    typedef struct __attribute__((__packed__))
    {
        CU_IMAGES  imageType;
        uint32_t   buildTime; // Format- YYYYMMDD
        uint32_t   commidId;
        char       buildTag[20];
        uint32_t   reserved[2];
    }imageProperties_t;

} // namespace CU

/*@brief - sbeGetCodeLevels : Implement SBE Get Code Levels Chipop (0xAE01)
 *
 * @param[in] - i_pArg Buffer to be passed to the function
 *
 * @return - RC from the FIFO access utility
 */
uint32_t sbeGetCodeLevels(uint8_t *i_pArg);

/*@brief - sbeUpdateImage : Implement SBE Update Image Chipop (0xAE02)
 *
 * @param[in] - i_pArg Buffer to be passed to the function
 *
 * @return - RC from the FIFO access utility
 */
uint32_t sbeUpdateImage(uint8_t *i_pArg);

/*@brief - sbeSyncPartition : Implement SBE Sync Partition Chipop (0xAE03)
 *
 * @param[in] - i_pArg Buffer to be passed to the function
 *
 * @return - RC from the FIFO access utility
 */
uint32_t sbeSyncPartition(uint8_t *i_pArg);

/*@brief - Get image hash for an image in NOR
 *
 * @param[in]  -const CU_IMAGES i_imageType :i_imageType passed to the function
 * @param[in]  -uint8_t i_Partition : passed to the function
 * @param[out] -uint8_t* o_hashArrayPtr :64-bytes image hash
 *
 * @return - success/ fail
 */
uint32_t getImageHash(const CU_IMAGES i_imageType,
                      const uint8_t i_Partition,
                      uint8_t* o_hashArrayPtr);

#endif //SBE_CODE_UPDATE_H
