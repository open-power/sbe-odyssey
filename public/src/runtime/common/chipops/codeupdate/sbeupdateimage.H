/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/common/chipops/codeupdate/sbeupdateimage.H $ */
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
#ifndef __SBEUPDATEIMAGE_H
#define __SBEUPDATEIMAGE_H

#include "sbeFifoMsgUtils.H"
#include "sbecmdcodeupdate.H"
#include "codeupdateutils.H"

#define WITH_ECC(x)                     (((x)*9)/8)

/*
 * @brief updateImage : Get image updated
 *
 * @param[in] updateImageCmdMsg_t msg : structure for update image
 * @param[in] sbeFifoType type : fifo type
 * @param[out] sbeRespGenHdr_t hdr : response header message
 * @param[out] uint32_t fifoRc : rc for fifo access failure
 * @param[out] bool ackEOT : flag for EOT received or not
 *
 * @return    fapiRc
 */
fapi2::ReturnCode updateImage (const CU::updateImageCmdMsg_t *i_msg,
                               const sbeFifoType i_type,
                               sbeRespGenHdr_t *o_hdr,
                               uint32_t &o_fifoRc,
                               bool &o_ackEOT);

/*
 * @brief getImageEntryFromPartitionTable : Get image details from partition table
 *                                          [ From NOR: incoming image other than bootloader
 *                                            From incoming data: if image is bootloader ]
 *
 * @param[in] i_sideNumber : running/ non-running side number in device
 * @param[in] i_imageType : image type received
 * @param[in] i_pakStartAddrInPibMem : pointer to image pak start address in incoming binary file.
 *            Valid for incoming pak file in pibmem buffer
 * @param[inout] io_codeUpdateCtrlStruct struct used to get/set/use various
 *               context needed for code-update
 *
 * @return rc
 */
uint32_t getImageEntryFromPartitionTable(const uint8_t i_sideNumber,
                                         const CU_IMAGES i_imageType,
                                         void *i_pakStartAddrInPibMem,
                                         codeUpdateCtrlStruct_t &io_codeUpdateReqStruct);

/*
 * @brief validateImageSize : Validate incoming image size
 *
 * @param[in] i_codeUpdateCtrlStruct struct used to contains various
 *            parameters needed for code-update
 *
 * @return rc
 */
uint32_t validateImageSize(codeUpdateCtrlStruct_t &i_codeUpdateReqStruct);


/*
 * @brief validateImageType : Validate if incoming image type is updateable
 *
 * @param[in] i_imageType : image type received
 *
 * @return rc
 */
uint32_t validateImageType(const CU_IMAGES i_imageType);

/*
 * @brief checkNUpdatePakMarkerNSize : This API can be used to check and update
 *                                     pak marker and size
 *
 * @param[in] i_pakStartAddrInPibMem : pointer to image pak start address in incoming binary file.
 *            Valid for incoming pak file in pibmem buffer
 * @param[in] i_length : last chunk of data remaining to write to device
 * @param[in] i_codeUpdateCtrlStruct struct used to contains various
 *            parameters needed for code-update
 *
 * @return rc
 */
uint32_t checkNUpdatePakMarkerNSize(void *i_pakStartAddrInPibMem,
                                    uint32_t i_length,
                                    codeUpdateCtrlStruct_t &i_codeUpdateReqStruct);

#endif
