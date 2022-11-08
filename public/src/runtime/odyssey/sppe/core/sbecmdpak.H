/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/odyssey/sppe/core/sbecmdpak.H $            */
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
/*
 * @file: ppe/sbe/sbefw/sbecmdscomaccess.H
 *
 * @brief This file contains the Interfaces for the SCOM Access chip-ops
 *
 */

#ifndef __SBEFW_SBECMDPAKLOAD_H
#define __SBEFW_SBECMDPAKLOAD_H

#include <stdint.h>

/**
  * @brief sbePakPush : Write data from Upstream FIFO into pakspace of heap shared memory
  *
  * @return    Rc from the FIFO access utility
  */

uint32_t sbePakPush (uint8_t *i_pArg);

typedef struct
{
    uint32_t size; // size in bits
} sbePakPushMsgHdr_t;

/**
  * @brief sbePakPop : Pop previous pak(s) from the pakstack of heap shared memory
  *
  * @return    Rc from the FIFO access utility
  */

uint32_t sbePakPop (uint8_t *i_pArg);

typedef struct
{
    uint16_t reserved1;
    uint8_t reserved2;
    uint8_t pop_count; // number of paks to pop
} sbePakPopMsgHdr_t;


#endif /* __SBEFW_SBECMDPAKLOAD_H */
