/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/common/hwpf/i2cr.H $                       */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2011,2023                        */
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
#ifndef __I2CR_H
#define __I2CR_H

/** @file  i2cr.H
 *  @brief Provides the interfaces to perform an I2CR Odyssey SCOM/CFAM R/W
 */

#include <fapi2.H>

namespace I2CR
{

  enum OperationType {READ,WRITE};
  enum AccessType {I2CR_CFAM,I2CR_SCOM,CFAM};

  fapi2::ReturnCode i2crPerformOp(OperationType i_opType,
                                  fapi2::Target<fapi2::TARGET_TYPE_OCMB_CHIP>& i_target,
                                  void* io_buffer,
                                  size_t& io_buflen,
                                  AccessType i_accessType,
                                  uint32_t i_i2crAddr);

}; // End namespace

#endif
