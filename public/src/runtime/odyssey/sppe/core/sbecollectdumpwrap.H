/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/odyssey/sppe/core/sbecollectdumpwrap.H $   */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2023                             */
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

#pragma once

/**
 * @brief fastarray file name mapping with "fastArrayControlSet"
*/
typedef enum
{
    mc_core = 0x00000001,
    mc_pub0 = 0x00000002,
    mc_pub1 = 0x00000003,
    mc_io   = 0x00000004,
    mc_perv = 0x00000005
}FastarrayFileNameMapping_t;


/**
 * @brief fastarray binary name mapping
 *
 * Note: This array needs to correspond to the json key "fastArrayControlSet"
 *       in the ekb hdct configuration.json file.
 *
*/
const char const_fastarrayFileNameMapping[][16] = {
    { NULL },      // null in 0 index
    { "mc_core" }, // mc_core
    { "mc_pub0" }, // mc_pub0
    { "mc_pub1" }, // mc_pub1
    { "mc_io"   }, // mc_io
    { "mc_perv" }  // mc_perv
};
