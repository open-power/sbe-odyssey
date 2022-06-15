/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/onetime/common/algorithms/dilithium/dilithium.H $  */
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

#ifndef _DILITHIUM_H
#define _DILITHIUM_H

#define DILITHIUM_PUB_KEY_SIZE 2336
#define DILITHIUM_SIG_SIZE 4668

#define DIL_N ((unsigned int)256)
#define DIL_VECT_MAX ((unsigned int)8) /* MAX(K, L) for any config */

typedef uint8_t dilithium_key_t[DILITHIUM_PUB_KEY_SIZE];
typedef uint8_t dilithium_signature_t[DILITHIUM_SIG_SIZE];

typedef struct
{
    uint32_t coeffs[DIL_N];
} poly;

/**
 * @brief the largest polyvecl, polyveck possible safe to cast to any valid, smaller size
 *
 */
typedef struct
{
    poly vec[DIL_VECT_MAX];
} polyvec_max;

/**
 * @brief Verifies dilithium signature
 *
 * @param *m: pointer to input signature
 *
 * @param siglen: length of signature
 *
 * @param *m: pointer to message
 *
 * @param mlen: length of message
 *
 * @return >0 if signature could be verified correctly and 0 otherwise
 */
int __attribute__((noinline)) ref_verify2(const uint8_t *sig,
                                          size_t siglen,
                                          const uint8_t *m,
                                          size_t mlen,
                                          const uint8_t *pk,
                                          size_t pkbytes, polyvec_max *w1, polyvec_max *mat01, polyvec_max *mat234, polyvec_max *mat567,
                                          polyvec_max *z, polyvec_max *h);

#endif //----------_DILITHIUM_H-------
