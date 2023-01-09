/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/odyssey/sppe/core/sbe_sp_plat_intf.H $     */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2015,2023                        */
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
#ifndef SBE_SP_PLAT_INTF_H
#define SBE_SP_PLAT_INTF_H

/*
* @brief Enum for image types supported
*/
enum class CU_IMAGES : uint16_t
{
    IMG_TYPE_INVALID=0,
    BOOTLOADER = 1,    // Updatable
    RUNTIME,           // Updatable
    BMC_OVRD,          // Updatable
    HOST_OVRD,         // Updatable
    SROM,              // Not-updatable
    CUST,              // Not-updatable
    SPPE_RAM,          // Updatable
    IOPPE_BIN,         // Updatable
    SYNOPSYS_BIN,      // Updatable
    ARC_BIN,           // Updatable
    RSVD,              // Reserved
    END_OF_IMG_LIST
};

#endif /* SBE_SP_PLAT_INTF_H */