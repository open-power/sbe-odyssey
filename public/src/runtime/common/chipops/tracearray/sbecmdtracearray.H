/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/common/chipops/tracearray/sbecmdtracearray.H $ */
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

#ifndef __SBECMDTRACEARRAY_H_
#define __SBECMDTRACEARRAY_H_

#include <stdint.h>
#include "plat_hwp_data_stream.H"
#include "poz_gettracearray.H"


typedef enum : uint16_t
{
    SBE_TA_OPERATION_RESET              = 0x0001, ///< Reset the debug logic after dumping
    SBE_TA_OPERATION_RESTART            = 0x0002, ///< Start the trace array after dumping
    SBE_TA_OPERATION_STOP               = 0x0004, ///< Stop the trace array before starting the dump
    SBE_TA_OPERATION_COLLECT_DUMP       = 0x0008, ///< Do dump the trace array; useful if you just want to start/stop
    SBE_TA_OPERATION_IGNORE_MUX_SETTING = 0x0010, ///< Do not fail if the primary trace mux is set to a different bus
}sbeTraceArrayOperationBitMap_t;

/**
 * @brief Control Trace Array chipop
 */
typedef struct
{
    uint32_t reserved;
    uint32_t iv_traceArrayId   :16;
    uint32_t iv_operation      :16;

    /**
     * @brief Get the trace array HWP Arg object
     *
     * @return poz_gettracearray_args return @ref poz_gettracearray_args
     */
    poz_gettracearray_args getArg( void );

    /**
     * @brief execute hardware procedure platform specific
     *
     * @param[out] o_stream o_stream Trace array stream. Will contain all trace
     *                      entries read from the array concatenated, starting
     *                      with the oldest trace entry and ending with the
     *                      newest.
     *
     * @return fapi return from HWP
    */
    fapi2::ReturnCode executeHwp( fapi2::sbefifo_hwp_data_ostream& o_stream );

} sbeControlTraceArrayCmd_t;


/**
 * @brief Control Trace Array Command (0xA602)
 *
 * @param[in] i_pArg Buffer to be passed to the function (not used as of now)
 *
 * @return    RC from the FIFO access utility
 */
uint32_t sbeControlTraceArray(uint8_t *i_pArg);


/**
  * @brief sbeControlTraceArrayWrap : write the tracearray data Buffer
  *
  * @param[in]  i_getStream      fifo down stream interface
  * @param[in]  i_putStream      fifo up stream interface
  *
  * @return    Rc from the FIFO access utility
  */
uint32_t sbeControlTraceArrayWrap(  fapi2::sbefifo_hwp_data_istream& i_getStream,
                                    fapi2::sbefifo_hwp_data_ostream& i_putStream);


#endif