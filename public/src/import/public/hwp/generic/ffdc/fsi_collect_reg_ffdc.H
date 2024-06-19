/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/hwp/generic/ffdc/fsi_collect_reg_ffdc.H $ */
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
#ifndef __FSI_COLLECT_REG_FFDC_H
#define __FSI_COLLECT_REG_FFDC_H

#include <return_code.H>
#include <error_info_defs.H>

// Function pointer definition for HWP call support
typedef fapi2::ReturnCode (*fsi_collect_reg_ffdc_FP_t)(const fapi2::ffdc_t&,
        fapi2::ReturnCode& );

extern "C"
{

    fapi2::ReturnCode fsi_collect_reg_ffdc(const fapi2::ffdc_t& i_spinal,
                                           fapi2::ReturnCode& o_rc);

}

#endif
