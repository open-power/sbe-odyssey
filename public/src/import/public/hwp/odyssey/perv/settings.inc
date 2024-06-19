/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/hwp/odyssey/perv/settings.inc $      */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2022,2023                        */
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
//Variables
#define ATTR_PLL_BYPASS_VALID     BIT(7)
#define ATTR_PLL_BYPASS_REG       SCRATCH_REGISTER_8
#define ATTR_PLL_BYPASS_BIT       BIT(0)
#define ATTR_NEST_FREQ_MHZ_REG    SCRATCH_REGISTER_6
#define ATTR_NEST_FREQ_MHZ(value) BITVALUE(16, 31, value)
#define REF_CLOCK_FREQ_MHZ        133
#define PIB_CLOCK_REGION 0b001000000000000      //perv regions: 0-3, 13
#define ODY_PERV_MCPLL 0xl0000002
#define FORCE_SECURITY_ON_VALID   BIT(10)
#define FORCE_SECURITY_ON_REG     SCRATCH_REGISTER_11
#define FORCE_SECURITY_ON_BIT     BIT(4)
