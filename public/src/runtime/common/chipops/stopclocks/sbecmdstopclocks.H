/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/common/chipops/stopclock/sbecmdstopclocks.H $ */
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

#ifndef __SBECMDSTOPCLOCK_H_
#define __SBECMDSTOPCLOCK_H_

#include <stdint.h>
#include "plat_hwp_data_stream.H"
#include <fapi2.H>


/**
  * @brief Stop Clocks chipops message header
  *
  */
typedef struct
{
    uint32_t       reserved1        : 8;
    uint32_t       iv_logTargetType : 8;
    uint32_t       reserved2        : 8;
    uint32_t       iv_instanceId    : 8;

    /**
     * @brief Validate the parameter which are received from target type and
     *        Instance ID
     *
     * @return sbeSecondaryResponse Secondary RC
     */
    sbeSecondaryResponse validateParams(void);

    /**
     * @brief Platform specific stop clock function
     *        function pointer will be assigned by platform
     */
    /**
     * @brief execute hardware procedure platform specific
     *
     * @param[out] fapi return from HWP
    */
    fapi2::ReturnCode executeHwp( void );

}sbeStopClocksReqMsgHdr_t;


/**
  * @brief  Handles Sbe Stop Clocks chip-op (0xA903)
  *
  * @param[in] i_pArg Buffer to be passed to the function
  * @return    Rc from the FIFO access utility
  */
uint32_t sbeStopClocks(uint8_t *i_pArg);

/**
 * @brief sbeStopClocksWrap Sbe Stop Clocks function
 *
 * @param[in]  i_getStream      up-stream fifo for chip-op
 * @param[in]  io_putStream     down-stream fifo for chip-op
 * @return  RC from the underlying FIFO utility
 *
 * NOTE: This function will be called from stopcock chipop handler and from dump
 *       chipop handler also
 *
 * TODO: JIRA: PFSBE-352 - "Need to support FIFO/SROM in stream class"
 *       Need to support FIFO/SROM(files), need to refactor by creating
 *       sbe_hwp_data_stream class
 */
uint32_t sbeStopClocksWrap (fapi2::sbefifo_hwp_data_istream& i_getStream,
                            fapi2::sbefifo_hwp_data_ostream& io_putStream);

#endif //__SBECMDSTOPCLOCK_H_