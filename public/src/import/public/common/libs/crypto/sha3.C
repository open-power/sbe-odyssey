/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/common/libs/crypto/sha3.C $          */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2023                             */
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

#include "sha3.H"
#include <string.h>
#include <algorithm>

void sha3_keccakf(uint64_t st[25])
{
    // constants
    static const uint64_t keccakf_rndc[24] =
    {
        0x0000000000000001, 0x0000000000008082, 0x800000000000808a,
        0x8000000080008000, 0x000000000000808b, 0x0000000080000001,
        0x8000000080008081, 0x8000000000008009, 0x000000000000008a,
        0x0000000000000088, 0x0000000080008009, 0x000000008000000a,
        0x000000008000808b, 0x800000000000008b, 0x8000000000008089,
        0x8000000000008003, 0x8000000000008002, 0x8000000000000080,
        0x000000000000800a, 0x800000008000000a, 0x8000000080008081,
        0x8000000000008080, 0x0000000080000001, 0x8000000080008008
    };
    static const int keccakf_rotc[24] =
    {
        1,  3,  6,  10, 15, 21, 28, 36, 45, 55, 2,  14,
        27, 41, 56, 8,  25, 43, 62, 18, 39, 61, 20, 44
    };
    static const int keccakf_piln[24] =
    {
        10, 7,  11, 17, 18, 3, 5,  16, 8,  21, 24, 4,
        15, 23, 19, 13, 12, 2, 20, 14, 22, 9,  6,  1
    };

    // variables
    unsigned int i, j, r;
    uint64_t t, bc[5];


    uint8_t* v;

    // endianess conversion. this is redundant on little-endian targets
    for (i = 0; i < 25; i++)
    {
        v = (uint8_t*) &st[i];
        st[i] = ((uint64_t) v[0])     | (((uint64_t) v[1]) << 8) |
                (((uint64_t) v[2]) << 16) | (((uint64_t) v[3]) << 24) |
                (((uint64_t) v[4]) << 32) | (((uint64_t) v[5]) << 40) |
                (((uint64_t) v[6]) << 48) | (((uint64_t) v[7]) << 56);
    }


    // actual iteration
    for (r = 0; r < KECCAKF_ROUNDS; r++)
    {

        // Theta
        for (i = 0; i < 5; i++)
        {
            bc[i] = st[i] ^ st[i + 5] ^ st[i + 10] ^ st[i + 15] ^ st[i + 20];
        }

        for (i = 0; i < 5; i++)
        {
            t = bc[(i + 4) % 5] ^ ROTL64(bc[(i + 1) % 5], 1);

            for (j = 0; j < 25; j += 5)
            {
                st[j + i] ^= t;
            }
        }

        // Rho Pi
        t = st[1];

        for (i = 0; i < 24; i++)
        {
            j = keccakf_piln[i];
            bc[0] = st[j];
            st[j] = ROTL64(t, keccakf_rotc[i]);
            t = bc[0];
        }

        //  Chi
        for (j = 0; j < 25; j += 5)
        {
            for (i = 0; i < 5; i++)
            {
                bc[i] = st[j + i];
            }

            for (i = 0; i < 5; i++)
            {
                st[j + i] ^= (~bc[(i + 1) % 5]) & bc[(i + 2) % 5];
            }
        }

        //  Iota
        st[0] ^= keccakf_rndc[r];
    }

    for (i = 0; i < 25; i++)
    {
        v = (uint8_t*) &st[i];
        t = st[i];
        v[0] = t & 0xFF;
        v[1] = (t >> 8) & 0xFF;
        v[2] = (t >> 16) & 0xFF;
        v[3] = (t >> 24) & 0xFF;
        v[4] = (t >> 32) & 0xFF;
        v[5] = (t >> 40) & 0xFF;
        v[6] = (t >> 48) & 0xFF;
        v[7] = (t >> 56) & 0xFF;
    }
}

// Initialize the context for SHA3

int sha3_init(sha3_ctx_t* c)
{
    int i;

    for (i = 0; i < 25; i++)
    {
        c->st.q[i] = 0;
    }

    c->mdlen = 64;
    c->rsiz = 200 - 2 * 64;
    c->pt = 0;

    return 1;
}

// update state with more data

int sha3_update(sha3_ctx_t* c, const void* data, size_t len)
{
    size_t i;
    int j;

    j = c->pt;

    for (i = 0; i < len; i++)
    {
        c->st.b[j++] ^= ((const uint8_t*) data)[i];

        if (j >= c->rsiz)
        {
            sha3_keccakf(c->st.q);
            j = 0;
        }
    }

    c->pt = j;

    return 1;
}

// finalize and output a hash

int sha3_final(sha3_t* result, sha3_ctx_t* c)
{
    int i;

    c->st.b[c->pt] ^= 0x06;
    c->st.b[c->rsiz - 1] ^= 0x80;
    sha3_keccakf(c->st.q);

    for (i = 0; i < c->mdlen; i++)
    {
        ((uint8_t*) result)[i] = c->st.b[i];
    }

    return 1;
}

// compute a sha3 hash (md) of given byte length from "in"
void* sha3(const void* in, size_t inlen, void* md, int mdlen)
{
    sha3_ctx_t ctx;
    sha3_t digest;
    sha3_init(&ctx);
    sha3_update(&ctx, in, inlen);
    sha3_final(&digest, &ctx);

    memcpy(md, digest, std::min((size_t)mdlen, sizeof(digest)));
    return md;
}
