/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/common/generic/fapi2/tools/templates/hwp_reg_ffdc.H.t $ */
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
 * hwp_reg_ffdc.H
 *
 * This file is generated from a python script - {{toolname}}.
 *
 * This file contain declaration of arrays of scom address for various
 *  register-ffdc. This file is required for platform which has compile
 *  flag 'MINIMUM_REG_COLLECTION' (mostly SBE platform).

 * These constants are defining as extern in this header files,
 *  such that full scom def header files are only required to add in
 *  corresponding source file only.
 *  Hence compilation performance will improve.
 *
 */

#pragma once



namespace fapi2
{
{% for (id, reg_ffdc) in hwpErrorDB.register_ffdcs.items() %}
    extern const uint32_t CONST_REG_FFDC_{{id}}[{{reg_ffdc.scom_list_len}}];
{% endfor %}
} // namespace fapi2
