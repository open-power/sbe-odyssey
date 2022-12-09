/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/common/codeupdate/sbecmdupdateimage.H $    */
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
#ifndef __SBECMDUPDATEIMAGE_H
#define __SBECMDUPDATEIMAGE_H

#include "target.H"
#include "sbeFifoMsgUtils.H"

using namespace fapi2;

#define NOR_PARTITION_SIZE              0x00400000 // 4MB
#define NOR_FLASH_SECTOR_BOUNDARY_ALIGN 0xFFFFF000
#define MIN_SPI_WRITE_IN_WORDS          0x100 // 256 Words
#define WITH_ECC(x)                     (((x)*9)/8)
#define IN_BYTES(x)                     ((x)*4)
#define WORD_ALIGNED                    4
#define WORD_LENGTH                     4

/**
 * @brief structure for update image chip-op
 */
typedef struct __attribute__((__packed__))
{
    uint16_t reserved;
    uint16_t imageType;
    uint32_t imageSizeInWords;
}updateImageCmdMsg_t;

/**
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
fapi2::ReturnCode updateImage (const updateImageCmdMsg_t *i_msg,
                               const sbeFifoType i_type,
                               sbeRespGenHdr_t *o_hdr,
                               uint32_t &o_fifoRc,
                               bool &o_ackEOT);

#endif
