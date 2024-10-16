/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/common/generic/fapi2/tools/templates/hwp_return_codes.H.t $ */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2024                             */
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

/**
 * hwp_return_codes.H
 *
 * This file is generated from a python script - {{toolname}}.
 *
 * This file contain enum of all HWP RCs defined in error xmls.
 *
 * Integer value of each RC enum is calculated by last 24-bits of
 *  md5 checksum
 */

#pragma once

namespace fapi2
{

/**
 * @brief Enumeration of HWP return codes
 */
enum HwpReturnCode
{
{% for rc in hwpErrorDB.hwp_errors %}
    {{rc}} = {{hwpErrorDB.hwp_errors[rc].hash_hex}},
{% endfor %}
};

} // namespace fapi2
