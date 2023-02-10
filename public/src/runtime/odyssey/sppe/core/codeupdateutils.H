/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/odyssey/sppe/core/codeupdateutils.H $      */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2015,2023                        */
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
#ifndef CODE_UPDATE_UTILS_H
#define CODE_UPDATE_UTILS_H

/* @brief : This API is used to find out the running partition
 *          [Primary , secondary or golden] and Non-running partition
 *
 * @param[out] : o_runningPartition used to pass running partition information
 * @param[out] : o_nonRunningPartition used to pass non-running partition
 *               information.
 *
 * @return : void
 */
void getPartitionInfo(uint8_t &o_runningPartition,
                      uint8_t &o_nonRunningPartition);

#endif //CODE_UPDATE_UTILS_H
