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
#ifdef MINIMUM_REG_COLLECTION
#include <hwp_reg_ffdc.H>
#endif

/**
 * @brief FFDC gathering classes
 */
namespace fapi2
{

#ifdef MINIMUM_REG_COLLECTION
/**
 * @brief - Collect all register values whose addess is provided.
 *
 *      Platform which supports MINIMUM_REG_COLLECTION has to implement this
 *       function.
 *
 *      Limitation of this implementation:
 *          1. Only scom register collections are supported now
 *          2. register collection for multiple target using <childTarget>
 *             is not supported now.
 *
 *      Store the register collection in array of uint64_t (since its scom),
 *       with target index as a header, such that this array can be directly
 *       logged into the pel by host/sp
 *
 * @param[in] i_target_ffdc     pointer to lv-ffdc which has target info
 * @param[in] i_address_list    pointer to first address of address list
 * @param[in] i_length          length of address list
 * @param[in] io_hwp_reg_ffdc   pointer to the output buffer
 *
 */
void collectRegisters(
    const void *i_target_ffdc,
    const uint32_t* i_address_list,
    const uint32_t i_length,
    uint32_t*& io_hwp_reg_ffdc);
#endif

extern pozFfdcData_t g_FfdcData;
#if defined(__SBE__) && defined(MINIMUM_FFDC) && !defined(MINIMUM_FFDC_RE)
extern pozFfdcCtrl_t g_ffdcCtrlSingleton;
#endif

{% for rc in hwpErrorDB.hwp_errors %}
class {{rc[3:]}}
{
  public:
    sbeFfdc_t * iv_localFfdcData = NULL;
    void* iv_hwRegFfdcData = NULL;

    {% for lv_ffdc in hwpErrorDB.hwp_errors[rc].ffdc %}
    template< typename T >
    inline {{rc[3:]}}& set_{{lv_ffdc}}(const T& i_value)
    {
        if (iv_localFfdcData)
        {
            iv_localFfdcData[{{loop.index-1}}].data= convertType(i_value);
            iv_localFfdcData[{{loop.index-1}}].size = fapi2::getErrorInfoFfdcSize(i_value);
        }
        return *this;
    };
    {% endfor %}{# lv_ffdc #}

    void execute()
    {
#ifdef MINIMUM_REG_COLLECTION
    {% if hwpErrorDB.hwp_errors[rc].hasCollectRegister %}
        auto word_ptr = (uint32_t*)iv_hwRegFfdcData;
    {% endif %}
    {% for crf in hwpErrorDB.hwp_errors[rc].collect_reg_ffdc %}
        *word_ptr = {{crf.id_hash_hex}};
        word_ptr++;
        collectRegisters(
            &iv_localFfdcData[{{crf.target_index}}].data,
            CONST_REG_FFDC_{{crf.id}},
            sizeof(CONST_REG_FFDC_{{crf.id}})/sizeof(CONST_REG_FFDC_{{crf.id}}[0]),
            word_ptr);

    {% endfor %}
#endif
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
#ifdef MINIMUM_REG_COLLECTION
        uint32_t hwRegisterFfdcSize = {{hwpErrorDB.hwp_errors[rc].reg_ffdc_size_in_bytes}};
#else
        uint32_t hwRegisterFfdcSize = 0;
#endif
#if defined(__SBE__)
  #if defined (MINIMUM_FFDC)
    uint32_t tempScratchAddr = ffdcConstructor(
                    {{rc}},
                    ({{hwpErrorDB.hwp_errors[rc].ffdc_len}} * sizeof(fapi2::sbeFfdc_t)),
                    (void*&)iv_localFfdcData,
                    hwRegisterFfdcSize,
                    iv_hwRegFfdcData,
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
#define MAX_REG_FFDC_SIZE ({{hwpErrorDB.max_reg_ffdc_size}})

} // namespace fapi2
