/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/common/utils/metadata.H $                          */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2022                             */
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

// Image type information
DECLARE_METADATA(IMG, 0x494D47) {
    uint32_t imageType;
};

// Trace buffer information
DECLARE_METADATA(TRA, 0x545241) {
    uint32_t traceAddr; // Address of the trace buffer
    uint32_t traceSize; // Size of the trace buffer
};

// Intended load address for the image
DECLARE_METADATA(LDA, 0x4c4441) {
    uint32_t loadAddr;
};

// Git commit id for the image
DECLARE_METADATA(GIT, 0x474954) {
    uint32_t commitId;
};

// Git build timestamp MMDDYYYY for the image
DECLARE_METADATA(DAT, 0x444154) {
    uint32_t timeStamp;
};

// String metadata needs no field declaration
DECLARE_METADATA_STR(BLD, 0x424c44);

extern const struct metadata_t g_image_metadata;
