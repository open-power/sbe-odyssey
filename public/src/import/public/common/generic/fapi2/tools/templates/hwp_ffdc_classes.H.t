/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/common/generic/fapi2/tools/templates/hwp_ffdc_classes.H.t $ */
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
 * hwp_ffdc_classes.H
 *
 * This file is generated from a python script - {{toolname}}.
 *
 * This files contain definition of all RC classes, which can be used by
 *  HWP inside a FAPI_ASSERT like,
 *
 *     FAPI_ASSERT(false,
 *         fapi2::MSS_ODY_CCS_READ_MISCOMPARE()
 *         .set_MC_TARGET(i_target_chip),
 *         " CCS FAIL Read Miscompare");
 *
 *  This file provide definition of constructor and other set method (for each
 *  local variable ffdcs), for each RC class.
 *
 * This file will contain other platform specific logics to handle the RC instantiation
 *  call from the HWP like,
 *  1. allocate space for the internal data structure
 *  2. additional data structure to support sending ffdc to host (in case of sbe platform)
 *  etc.
 *
 */

#pragma once

#include <return_code.H>
#include <fapi2_hwp_executor.H>
#include <error_info.H>
#include <buffer.H>
#include <hwp_error_info.H>
#include <error_info_defs.H>
#if !defined(FAPI2_NO_FFDC) && !defined(MINIMUM_FFDC)
#include <utils.H>
#include <ffdc_includes.H>
#endif
#if defined(__SBE__) && defined(MINIMUM_FFDC) && !defined(MINIMUM_FFDC_RE)
#include <sbeffdctype.H>
#endif

/**
 * @brief FFDC gathering classes
 */
namespace fapi2
{

extern pozFfdcData_t g_FfdcData;
#if defined(__SBE__) && defined(MINIMUM_FFDC) && !defined(MINIMUM_FFDC_RE)
extern pozFfdcCtrl_t g_ffdcCtrlSingleton;
#endif

{% for rc in hwpErrorDB.hwp_errors %}
class {{rc[3:]}}
{
  public:
    sbeFfdc_t * iv_localFfdcData = NULL;
    {% for lv_ffdc, index in hwpErrorDB.hwp_errors[rc].sorted_ffdc_with_index %}
    template< typename T >
    inline {{rc[3:]}}& set_{{lv_ffdc}}(const T& i_value)
    {
        if (iv_localFfdcData)
        {
            iv_localFfdcData[{{index}}].data= convertType(i_value);
            iv_localFfdcData[{{index}}].size = fapi2::getErrorInfoFfdcSize(i_value);
        }
        return *this;
    };

    {% endfor %}{# lv_ffdc #}
    void execute()
    {

    }
    {{rc[3:]}}(fapi2::errlSeverity_t i_sev = fapi2::FAPI2_ERRL_SEV_UNRECOVERABLE)
    {
        fapi2::current_err =  {{rc}};
#if defined(MINIMUM_FFDC_RE)
        fapi2::current_err.setDataPtr(0);
#endif
#if !defined(MINIMUM_FFDC)
        FAPI_ERR("{{hwpErrorDB.hwp_errors[rc].description}}");
#endif
#if defined(__SBE__)
  #if defined (MINIMUM_FFDC)
    void* ptr = nullptr;
    uint32_t tempScratchAddr = ffdcConstructor(
                     (uint32_t){{rc}},
                     (uint16_t)({{hwpErrorDB.hwp_errors[rc].ffdc_len}} * sizeof(fapi2::sbeFfdc_t)),
                     ( void *&)iv_localFfdcData,
                     (uint16_t)0,
                     ( void *&)ptr,
                     i_sev
                   );
    #if defined (MINIMUM_FFDC_RE)
        fapi2::current_err.setDataPtr(tempScratchAddr);
    #elif !defined (MINIMUM_FFDC_RE)
        fapi2::g_ffdcCtrlSingleton.setHead(reinterpret_cast<const pozFfdcNode_t*>(tempScratchAddr));
    #endif
  #endif
#endif
    }
};


{% endfor %}{# rc #}

#define MAX_FFDC_LV_SIZE ({{hwpErrorDB.max_ffdc_len}} * sizeof(fapi2::sbeFfdc_t))

} // namespace fapi2
