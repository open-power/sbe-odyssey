/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/emb/p11/kernels/ppe/powmanlib/pminit_common.h $ */
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
#ifndef __PMINIT_COMMON_H__
#define __PMINIT_COMMON_H__

#include <stdint.h>
#include <iota_trace.h>
#include <ppe42_spr.h>


//
// verify_ppe_image()
//
// Description:
//   Retrieves the PPE type ID and instance ID from the PIR of the actual PPE HW instance this
//   code is running on. It then compares this against the caller's targeted PPE type and
//   instance ID. If the verification fails, it will create an error log and halt the PPE.
//
// Args:
//   uint32_t i_this_img_ppe_type; // The targeted PPE type. See ppe42_spr.h for PPE_TYPE choices
//   uint32_t i_this_img_ppe_inst; // The targeted instance ID. (CE/GPE: 0..3, QME: 0..1)
//
static inline void verify_ppe_image(uint32_t i_this_img_ppe_type, uint32_t i_this_img_ppe_inst)
{
    uint32_t this_ppe_pir  = mfspr(SPRN_PIR);

    uint32_t this_ppe_type = this_ppe_pir & PIR_PPE_TYPE_MASK;

    if (this_ppe_type != i_this_img_ppe_type)
    {
        //CMO-TBD/TODO: Create error log
        PK_TRACE_ERR("VRFYPPE: Image running on wrong PPE type. Details:");
        PK_TRACE_ERR("  Image's PPE type: 0x%x", i_this_img_ppe_type);
        PK_TRACE_ERR("  This PPE's PIR type: 0x%x", this_ppe_type);
        PK_TRACE_ERR("  This PPE's PIR content: 0x%08x", this_ppe_pir);
        PK_TRACE_ERR("Creating error log and return command to caller.");

        APPCFG_PANIC(PMHW_PPE_TYPE_MISMATCH);
    }

    uint32_t this_ppe_inst = this_ppe_pir & PIR_PPE_INSTANCE_MASK;

    if (this_ppe_inst != i_this_img_ppe_inst)
    {
        //CMO-TBD/TODO: Create error log
        PK_TRACE_ERR("VRFYPPE: Image running on wrong PPE instance. Details:");
        PK_TRACE_ERR("  Image's instance ID: 0x%x", i_this_img_ppe_inst);
        PK_TRACE_ERR("  This PPE's PIR instance ID: 0x%x", this_ppe_inst);
        PK_TRACE_ERR("  This PPE's PIR content: 0x%08x", this_ppe_pir);
        PK_TRACE_ERR("Creating error log and return command to caller.");

        APPCFG_PANIC(PMHW_PPE_INST_MISMATCH);
    }
}

#endif /* __PMINIT_COMMON_H__ */
