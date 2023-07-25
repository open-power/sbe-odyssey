/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/common/plat/attributes/attrutils.H $       */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2023                             */
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

#pragma once

#include "attribute_properties.H"

/**
 * @brief This file contains APIs that are specific to project
 *        Project can implement these APIs in plat/attributes/attrutils.C
 *
 */

/**
 * @brief Set ATTR_SECURITY_LEVEL attribute value for project specific chip type
 *
 * @param i_securityLvl Value to be set for ATTR_SECURITY_LEVEL
 */
void platSetAttrSecurityLvl(fapi2::ATTR_SECURITY_LEVEL_Type i_securityLvl);