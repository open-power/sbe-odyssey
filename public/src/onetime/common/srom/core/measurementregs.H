/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/onetime/common/srom/core/measurementregs.H $       */
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

/**
 * @brief This files defines all measurement regs addresses
 *
 */

#ifndef _MEASUREMENTREGS_H_
#define _MEASUREMENTREGS_H_

/**
 * @brief Used for storing SHA3-512 (truncated to 48B)
 * (Hash of boot loader HW keys | Hash of boot loader FW keys | Hash boot loader hash list)
 *
 */
#define MEASUREMENT_REG_0  0x000501A0
#define MEASUREMENT_REG_1  0x000501A1
#define MEASUREMENT_REG_2  0x000501A2
#define MEASUREMENT_REG_3  0x000501A3
#define MEASUREMENT_REG_4  0x000501A4
#define MEASUREMENT_REG_5  0x000501A5
#define MEASUREMENT_REG_6  0x000501A6
#define MEASUREMENT_REG_7  0x000501A7
#define MEASUREMENT_REG_8  0x000501A8
#define MEASUREMENT_REG_9  0x000501A9
#define MEASUREMENT_REG_10 0x000501AA
#define MEASUREMENT_REG_11 0x000501AB

/**
 * @brief Used for storing SHA3-512 (truncated to 48B)
 * (Hash of runtime HW keys | Hash of runtime FW keys | Hash of runtime image hash list)
 *
 */
#define MEASUREMENT_REG_12 0x000501AC
#define MEASUREMENT_REG_13 0x000501AD
#define MEASUREMENT_REG_14 0x000501AE
#define MEASUREMENT_REG_15 0x000501AF
#define MEASUREMENT_REG_16 0x000501B0
#define MEASUREMENT_REG_17 0x000501B1
#define MEASUREMENT_REG_18 0x000501B2
#define MEASUREMENT_REG_19 0x000501B3
#define MEASUREMENT_REG_20 0x000501B4
#define MEASUREMENT_REG_21 0x000501B5
#define MEASUREMENT_REG_22 0x000501B6
#define MEASUREMENT_REG_23 0x000501B7

/**
 * @brief Used to store SROM Secure Boot Control Settings
 *
 */
#define MEASUREMENT_REG_24 0x000501B8

/**
 * @brief Used to store Boot Loader Secure Boot Control Settings
 *
 */
#define MEASUREMENT_REG_25 0x000501B9

#define MEASUREMENT_REG_26 0x000501BA
#define MEASUREMENT_REG_27 0x000501BB
#define MEASUREMENT_REG_28 0x000501BC
#define MEASUREMENT_REG_29 0x000501BD
#define MEASUREMENT_REG_30 0x000501BE
#define MEASUREMENT_REG_31 0x000501BF
#define MEASUREMENT_REG_32 0x000501C0
#define MEASUREMENT_REG_33 0x000501C1
#define MEASUREMENT_REG_34 0x000501C2
#define MEASUREMENT_REG_35 0x000501C3
#define MEASUREMENT_REG_36 0x000501C4
#define MEASUREMENT_REG_37 0x000501C5
#define MEASUREMENT_REG_38 0x000501C6
#define MEASUREMENT_REG_39 0x000501C7

#endif /* _MEASUREMENTREGS_H_ */
