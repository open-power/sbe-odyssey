/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/common/algorithms/sha3/sha3.H $                    */
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
#ifndef SHA3_H
#define SHA3_H

#include <stddef.h>
#include <stdint.h>

#ifndef KECCAKF_ROUNDS
#define KECCAKF_ROUNDS 24
#endif

#ifndef ROTL64
#define ROTL64(x, y) (((x) << (y)) | ((x) >> (64 - (y))))
#endif

#define SHA3_DIGEST_LENGTH 64

typedef uint8_t sha3_t[SHA3_DIGEST_LENGTH];

// state context
typedef struct {
    union {                                 // state:
        uint8_t b[200];                     // 8-bit bytes
        uint64_t q[25];                     // 64-bit words
    } st;
    int pt, rsiz, mdlen;                    // these don't overflow
} sha3_ctx_t;

typedef uint8_t  sha3_byte;     /* Exactly 1 byte */
typedef uint32_t sha3_word32;   /* Exactly 4 bytes */
typedef uint64_t sha3_word64;   /* Exactly 8 bytes */

// Compression function.
void sha3_keccakf(uint64_t st[25]);

// OpenSSL - like interfece
int sha3_init(sha3_ctx_t *c);    // mdlen = hash output in bytes
int sha3_update(sha3_ctx_t *c, const void *data, size_t len);
int sha3_final(sha3_t *md, sha3_ctx_t *c);    // digest goes to md

/**
 * @brief compute a sha3 hash (md) of given byte length from "data"
 *
 * @param data Data on which sha3 needs to be calculated
 * @param len Length of data
 * @param result sha3 result store location ptr
 */
void sha3_Hash(const sha3_byte *data, size_t len, sha3_t *result);

uint32_t make32Bit(uint8_t *ptr);

//Print SHA3-512 result
void SHA3_dump(sha3_t *result);

#endif
