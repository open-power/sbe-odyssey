/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/common/chipops/fastarray/sbecmdfastarray.H $ */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2016,2023                        */
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

#include <stdint.h>
#include "sbe_sp_intf.H"
#include "plat_hwp_data_stream.H"

enum sbeControlFastArrayFlags {
    FASTARRAY_FLAG_NONE = 0x00,
    FASTARRAY_FLAG_CUSTOM_CONTROL_DATA = 0x01,
};


/*
 * @brief Fast array command structure
 * */
typedef struct
{
    uint32_t flags          : 8;
    uint32_t logTargetType  : 8;
    uint32_t reserved       : 8;
    uint32_t instanceId     : 8;
} sbeControlFastArrayCMD_t;


/**
 * @brief sbeControlFastArrayWrap: write the data Buffer
 *
 * @param[in]  i_getStream  upstream fifo for chip-op /
 *                          memory interface for dump
 * @param[in]  i_putStream  down-stream fifo for chip-op /
 *                          memory interface for dump
 *
 * @return    Rc from the FIFO access utility
 */
uint32_t sbeControlFastArrayWrap(fapi2::sbefifo_hwp_data_istream& i_getStream,
                                 fapi2::sbefifo_hwp_data_ostream& i_putStream);

/**
 * @brief Control Fast Array Command (0xA601)
 *
 * @param[in] i_pArg Buffer to be passed to the function (not used as of now)
 *
 * @return    RC from the FIFO access utility
 */
uint32_t sbeControlFastArray(uint8_t *i_pArg);
