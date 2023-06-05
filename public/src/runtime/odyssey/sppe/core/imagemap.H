/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/odyssey/sppe/core/imagemap.H $             */
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
#ifndef __IMAGE_MAP_H
#define __IMAGE_MAP_H

#include "codeupdateutils.H"
#include "getcapabilitiesutils.H"

#define MAX_PARTITION_TABLE_ENTRY 6

/**
 * @brief - Used to decide no of updatable image section count.
 *        - If the updatable image section count is changed then,
 *          we should add appropriate image section type in the
 *          CodeLevelsRespMsg_t::iv_updateableImagesInfo member,
 *          refer constructor.
 */
#define UPDATABLE_IMG_SECTION_CNT 4

// CU: Code Update
namespace CU
{
    static const imgMap_t g_imgMap[] = {
        {CU_IMAGES::BOOTLOADER, "boot"},
        {CU_IMAGES::RUNTIME,    "rt"  },
        {CU_IMAGES::BMC_OVRD,   "bmc" },
        {CU_IMAGES::HOST_OVRD,  "host"}
    };
} // namespace CU

constexpr CU_IMAGES g_syncableImgs [] __attribute__ ((aligned (8))) =
{
   CU_IMAGES::BOOTLOADER,
   CU_IMAGES::RUNTIME,
   CU_IMAGES::BMC_OVRD,
   CU_IMAGES::HOST_OVRD
};

constexpr capImgMap_t g_getCapabilitiesImages [] =
{
    {CAPABILITY_IMAGES::SROM,       "srom"},
    {CAPABILITY_IMAGES::BOOTLOADER, "boot"},
    {CAPABILITY_IMAGES::RUNTIME,    "rt"  },
    {CAPABILITY_IMAGES::BMC_OVRD,   "bmc" },
    {CAPABILITY_IMAGES::HOST_OVRD,  "host"},
    {CAPABILITY_IMAGES::EKB,        "ekb" }
};
#endif //IMAGE_MAP_H
