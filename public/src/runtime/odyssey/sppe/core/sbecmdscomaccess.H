/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/odyssey/sppe/core/sbecmdscomaccess.H $     */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2022                             */
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
#ifndef __SBECMDSCOMACCESS_H
#define __SBECMDSCOMACCESS_H

#include <stdint.h>

/**
  * @brief structure for GetScom Chipop (0xA201) contents.
  *
  */
typedef struct
{
    uint32_t hiAddr;
    uint32_t lowAddr;
}sbeGetScomReqMsg_t;

/**
  * @brief structure for PutScom Chipop (0xA202) contents.
  *
  */
typedef struct
{
    uint32_t hiAddr;
    uint32_t lowAddr;
    uint32_t hiInputData;
    uint32_t lowInputData;

    /**
      * @brief return 64-bit Scom data
      *
      * @return 64-bit Scom data
      */
    uint64_t getScomData()
    {
        uint64_t data = ((uint64_t)hiInputData << 32) | lowInputData;
        return data;
    }
}sbePutScomReqMsg_t;

/*
 * @brief sbeDownFifoGetStatus : Write data into Downstream FIFO
 *
 * @param[in] i_pArg Buffer to be passed to the function (not used as of now)
 *
 * @return    Rc from the FIFO access utility
 */
uint32_t sbeGetScom (uint8_t *i_pArg);


/**
 * @brief sbeDownFifoGetStatus : Write data into Downstream FIFO
 *
 * @param[in] i_pArg Buffer to be passed to the function (not used as of now)
 *
 * @return    Rc from the FIFO access utility
 */
uint32_t sbePutScom (uint8_t *i_pArg);

#endif /* __SBEFW_SBECMDSCOMACCESS_H */
