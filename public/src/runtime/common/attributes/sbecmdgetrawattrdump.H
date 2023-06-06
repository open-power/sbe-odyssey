/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/common/attributes/sbecmdgetrawattrdump.H $ */
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
#ifndef __SBE_CMDGETRAWATTRDUMP_H
#define __SBE_CMDGETRAWATTRDUMP_H

#include <stdint.h>

/**
 * @brief sbeGetRawAttrDump: GetRawAttrDump chipop handler.
 *
 * @param[in] i_pArg Buffer to be passed to the function
 *
 * @return    Rc from the FIFO access utility
 */
uint32_t sbeGetRawAttrDump(uint8_t *i_pArg);

#endif /* __SBE_CMDGETRAWATTRDUMP_H */
