/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/common/odyssey/include/odyssey_dynamic_init_features.H $ */
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

// set of Odyssey-specific dynamic init features
// - once assigned, the feature number/meaning is intended to never change
//   for backwards compatibility reasons
// - this file will be included in the PAK image manifest as a reference
//   of the dynXX folder meanings
// - HWP code may use this header to manage the runtime setting of
//   ATTR_DYNAMIC_INIT_FEATURE_VEC (the bit-vector of active dynamic inits,
//   which will trigger the scan of the associated dynXX content)
enum ODYSSEY_DYN_FEATURES
{
};
