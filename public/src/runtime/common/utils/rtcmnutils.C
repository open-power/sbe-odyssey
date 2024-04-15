/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/common/utils/rtcmnutils.C $                */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2024                             */
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
#include "rtcmnutils.H"
#include "heap.H"
#include "sbeffdc.H"


namespace SBE
{
    fapi2::ReturnCode allocScratch(void *& o_ptr, uint32_t i_size)
    {
        fapi2::current_err = fapi2::FAPI2_RC_SUCCESS;
        void *scratchArea  = NULL;

        // Allocate the scratch space based on i_size
        scratchArea = (void*)Heap::get_instance().scratch_alloc(i_size);
        PLAT_FAPI_ASSERT( (scratchArea != NULL),
                        fapi2::POZ_SCRATCH_ALLOC_FAILED().
                        set_REQUIRED_SPACE(i_size).
                        set_AVAILABLE_SPACE(Heap::get_instance().getFreeHeapSize()),
                        "scratch allocation failed.");
        o_ptr = scratchArea;
    fapi_try_exit:
        return fapi2::current_err;
    }
};