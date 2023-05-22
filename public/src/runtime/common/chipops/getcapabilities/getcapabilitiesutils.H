/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/common/chipops/getcapabilities/getcapabilitiesutils.H $ */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2022,2023                        */
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

#ifndef GETCAPABILITIES_UTILS_H
#define GETCAPABILITIES_UTILS_H

#include "sbeCmdGetCapabilities.H"

/**
 * @brief Used to get images details to send in the GetCapabilities response
 *
 * @param[out] o_capMsg The getcapability response buffer
 *
 * @return  RC
 */
uint32_t fillImagesDetails(GetCapabilityResp_t &o_capMsg);

/**
 * @brief Used to get capabilities details to send in the GetCapabilities
 *        response
 *
 * @param[out] o_capability The pointer to capability array
 *
 * @return void
 */
void fillCapabilitiesDetails(uint32_t* o_capability);

#endif
