/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/odyssey/sppe/core/sbecmdgetmemtraindata.H $ */
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

#include <stdint.h>

enum memTrainOperations
{
    MEMTRAINDUMP    = 0x1,
    MEMTRAINPURGE   = 0x2,
    MEMTRAIN_MAX_OPERATION
};

/**
  * @brief Get memport training data request structure
  */
typedef struct
{
    uint32_t reserved :24;
    uint32_t operation:8;

    /**
      * @brief Validate the operations.
      *
      * @param[in] true incase of valid operation, else false.
      */
     bool isValidOperations()
     {
         bool isValid = false;
         if((MEMTRAINDUMP >= operation) && (operation < MEMTRAIN_MAX_OPERATION))
         {
            isValid = true;
         }
         return isValid;
     }

}sbeGetMemTrainDataMsgHdr_t;

/**
 * @brief sbeExecuteHWP: Implement sbeGetMemTrainData chipop. (0xA804)
 *
 * @param[in] i_pArg Buffer to be passed to the function
 *
 * @return    Rc from the FIFO access utility
 */
uint32_t sbeGetMemTrainData(uint8_t *i_pArg);