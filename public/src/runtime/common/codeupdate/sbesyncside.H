/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/common/codeupdate/sbesyncside.H $          */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2022,2023                        */
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

#ifndef SBE_SYNC_SIDE_H
#define SBE_SYNC_SIDE_H

#include "target.H"

using namespace fapi2;

/**
 * @brief structure for sync chip-op
 */
typedef struct __attribute__((__packed__))
{
    uint32_t reserved  : 31;
    uint32_t forceSync : 1;
}sbeSyncReqMsg_t;

/**
 * @brief  syncSide : syncSide is used to sync images(BOOTLOADER/RUNTIME,BMC,HOST).
 *
 * @param[in]  - i_msg: sync chip-op request parameters
 * @param[out] - o_hdr: Response header object
 *
 * @return     - FAPIRC
 */
fapi2::ReturnCode syncSide(const sbeSyncReqMsg_t *i_msg, sbeRespGenHdr_t *o_hdr);

/**
 * @brief syncImage : Syncing the image from runSide to nonRunSide
 *
 * @param[in]  - i_syncSideCtrlStruct struct used to contains various
 *               parameters needed for syn chip-op
 * @param[out] - o_hdr: Response header object
 *
 * @return:      FAPIRC
 */
fapi2::ReturnCode syncImage(codeUpdateCtrlStruct_t &i_syncSideCtrlStruct,
                            sbeRespGenHdr_t *o_hdr);

#endif
