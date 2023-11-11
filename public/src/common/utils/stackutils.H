/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/common/utils/stackutils.H $                        */
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

#include "pk.h"

#define THRESHOLD_STACK_LIMIT_PERCENT 90

#define CRITICAL_STACK_LIMIT_PERCENT  95

/**
 * @brief Get the percentage of stack utilized
 *
 * @param stack_base Stack start addr
 * @param stack_limit Stack end add
 * @return uint8_t percentage of stack utilized
 */
uint8_t getStackUtilPercent(uint32_t stack_base, uint32_t stack_limit);

/**
 * @brief Get the percentage of stack utilized by current thread
 *
 * @return uint8_t percentage of stack utilized
 */
uint8_t getCurThreadStackUtilPercent();

/**
 * @brief Get the percentage of stack utilized by kernel thread
 *
 * @return uint8_t percentage of stack utilized
 */
uint8_t getKernelStackUtilPercent();

/**
 * @brief Check if current thread stack limit has crossed beyond:
 *        THRESHOLD_STACK_LIMIT_PERCENT --> Create a RE
 *        CRITICAL_STACK_LIMIT_PERCENT  --> Assert
 *
 */
void checkCurThreadStackLimit();

/**
 * @brief Check if kernel thread stack limit has crossed beyond:
 *        THRESHOLD_STACK_LIMIT_PERCENT --> Create a RE
 *        CRITICAL_STACK_LIMIT_PERCENT  --> Assert
 *
 */
void checkKernelStackLimit();