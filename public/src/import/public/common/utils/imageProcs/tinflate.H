/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/common/utils/imageProcs/tinflate.H $ */
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
/*
 * Adapted for PPE from tinflate v1.1.0 by Joergen Ibsen
 * Original adaptation by Joachim Fenkes <fenkes@de.ibm.com>
 * Original comment below.
 */
/*
 * tinf - tiny inflate library (inflate, gzip, zlib)
 *
 * Copyright (c) 2003-2019 Joergen Ibsen
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 *   1. The origin of this software must not be misrepresented; you must
 *      not claim that you wrote the original software. If you use this
 *      software in a product, an acknowledgment in the product
 *      documentation would be appreciated but is not required.
 *
 *   2. Altered source versions must be plainly marked as such, and must
 *      not be misrepresented as being the original software.
 *
 *   3. This notice may not be removed or altered from any source
 *      distribution.
 */

#ifndef TINF_H_INCLUDED
#define TINF_H_INCLUDED

#include "archive.H"

#define TINF_VER_MAJOR 1        /**< Major version number */
#define TINF_VER_MINOR 1        /**< Minor version number */
#define TINF_VER_PATCH 0        /**< Patch version number */
#define TINF_VER_STRING "1.1.0" /**< Version number as a string */

#define TINF_FILTER_NONE 0      /**< Don't apply any filter post decompression */
#define TINF_FILTER_PPC  1      /**< Apply reverse PowerPC instruction filter post decompression */

/**
 * Decompress `sourceLen` bytes of deflate data from `source` to `dest`.
 *
 * @param dest pointer to where to place decompressed data
 * @param destLen size of `dest`
 * @param source pointer to compressed data
 * @param sourceLen size of compressed data
 * @param receiver NULL if pure decompression, stream receiver if streamed decompression
 * @param hash NULL if no hashing required, space for final hash value if compressed data shall be hashed
 */
int tinf_uncompress(void* dest, unsigned int destLen,
                    const void* source, unsigned int sourceLen,
                    FileArchive::StreamReceiver* receiver,
                    sha3_t* hash, int filter);

#endif /* TINF_H_INCLUDED */
