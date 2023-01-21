/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/common/utils/metadata.H $                          */
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
#pragma once

/**
 * @brief SBE image metadata declarations
 *
 * This header declares all the possible metadata fields for
 * SBE project images. Note that a given image may (and likely will)
 * not contain all of these tags, so it is advised to _always_ check
 * the return value of GET_META_xxx() for NULL.
 */

#include "metadata_base.H"

/*
 * Metadata rules:
 *
 *  * Once a tag is used in production, never change it again.
 *    Tools will depend on the field layout.
 *  * If you need to add new data, consider creating a new field/tag
 *    over changing an existing field. That way old tools will
 *    still understand the old values.
 *  * If you need to update a field, create a new version with a
 *    new tag_int value, e.g. DECLARE_METADATA(IMGv2, 0x494D48)
 */

#define IMG_ASCII 0x494D47
#define TRA_ASCII 0x545241
#define LDA_ASCII 0x4c4441
#define GIT_ASCII 0x474954
#define DAT_ASCII 0x444154
#define HEA_ASCII 0x484541
#define BLD_ASCII 0x424c44

// Image type information
DECLARE_METADATA(IMG, IMG_ASCII) {
    uint32_t imageType;
};

// Trace buffer information
DECLARE_METADATA(TRA, TRA_ASCII) {
    uint32_t traceAddr; // Address of the trace buffer
    uint32_t traceSize; // Size of the trace buffer
};

// Attribute tank information
DECLARE_METADATA(ATR, 0x415452) {
    uint32_t addr; // Address of the attribute tank
    uint32_t size; // Size of the attribute tank
};

// Intended load address for the image
DECLARE_METADATA(LDA, LDA_ASCII) {
    uint32_t loadAddr;
};

// Git commit id for the image
DECLARE_METADATA(GIT, GIT_ASCII) {
    uint32_t commitId;
};

// Git build timestamp MMDDYYYY for the image
DECLARE_METADATA(DAT, DAT_ASCII) {
    uint32_t timeStamp;
};

// Heap information
DECLARE_METADATA(HEA, HEA_ASCII) {
    uint32_t heapAddr; // Address of the heap space
    uint32_t heapSize; // Size of heap space area
};

// String metadata needs no field declaration
DECLARE_METADATA_STR(BLD, BLD_ASCII);

extern const struct metadata_t g_image_metadata;
