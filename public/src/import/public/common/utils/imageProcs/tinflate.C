/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/common/utils/imageProcs/tinflate.C $ */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2022,2024                        */
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
 * tinflate - tiny inflate
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

#include "tinflate.H"

#include <limits.h>
#include <assert.h>
#include <endian.h>

#define TINF_OK         ARC_OPERATION_SUCCESSFUL
#define TINF_DATA_ERROR ARC_FILE_CORRUPTED
#define TINF_BUF_ERROR  ARC_INPUT_BUFFER_OVERFLOW

#define TINF_RET_t   ARC_RET_t

#if defined(UINT_MAX) && (UINT_MAX) < 0xFFFFFFFFUL
    #  error "tinf requires unsigned int to be at least 32-bit"
#endif

// Chosen to equal the size of one hash block
#define READ_BUF_SIZE 72
#define READ_BUF_WORDS (READ_BUF_SIZE >> 3)
static_assert((READ_BUF_SIZE % 8) == 0, "READ_BUF_SIZE must be a multiple of 8");

/* ------------------------------ *
 * -- internal data structures -- *
 * ------------------------------ */

struct tinf_tree
{
    unsigned short table[16]; /* table of code length counts */
    unsigned short trans[288]; /* code -> symbol translation table */
};

struct tinf_data
{
    struct
    {
        uint64_t rollback;
        union
        {
            uint8_t  b[READ_BUF_SIZE];
            uint32_t d[READ_BUF_WORDS * 2];
            uint64_t q[READ_BUF_WORDS];
        };
    } read_buf;

    // volatile forces the compiler to implement reads as lvd instructions
    // which is what we want for performance, especially when reading from
    // slow SPI attached memory.
    const volatile uint64_t* source;
    unsigned int source_remaining;
    int read_buf_ptr;

    uint32_t tag, refill;
    int bitcount, refill_bitcount;

    unsigned char* dest;
    int destSpace;

    unsigned char* dict;
    FileArchive::StreamReceiver* receiver;

#ifdef __USE_SHA3__
    sha3_ctx_t hash_ctx;
    sha3_t* hash;
#endif

    bool ppc_filter;
    uint32_t filter_offset;

    struct tinf_tree ltree; /* literal/length tree */
    struct tinf_tree dtree; /* distance tree */
};

/* ----------------------- *
 * -- utility functions -- *
 * ----------------------- */

static void refill_read_buf(struct tinf_data* d)
{
    /* Since the bit reader code greedily preloads, it might end up reading beyond
     * the end of compressed data. Stubbornly refuse to refill the read buffer in
     * this case so we don't accidentally screw up the hash */
    if (d->source_remaining == 0)
    {
        return;
    }

    const bool whole_buffer = d->source_remaining >= READ_BUF_SIZE;
    const int nwords = whole_buffer ? READ_BUF_WORDS : (d->source_remaining + 7) >> 3;

    // refill read buffer, update hash context if needed
    for (int i = 0; i < nwords; i++)
    {
        uint64_t tmp = *(d->source++);
        d->read_buf.q[i] = tmp;
    }

#ifdef __USE_SHA3__

    if (d->hash)
    {
        if(d->source_remaining >= READ_BUF_SIZE)
        {
            sha3_update(&d->hash_ctx, d->read_buf.b, READ_BUF_SIZE);
        }
        else
        {
            sha3_update(&d->hash_ctx, d->read_buf.b, d->source_remaining);
        }
    }

#endif

    if (d->source_remaining >= READ_BUF_SIZE)
    {
        d->source_remaining -= READ_BUF_SIZE;
    }
    else
    {
        d->source_remaining = 0;
    }

#ifdef __USE_SHA3__

    // At the end of the input stream, finalize the hash
    if (d->source_remaining == 0 && d->hash)
    {
        sha3_final(d->hash, &d->hash_ctx);
    }

#endif

    d->read_buf_ptr = 0;
}

/* build the fixed huffman trees */
static void tinf_build_fixed_trees(struct tinf_tree* lt, struct tinf_tree* dt)
{
    int i;

    /* build fixed length tree */
    for (i = 0; i < 16; ++i)
    {
        lt->table[i] = 0;
    }

    lt->table[7] = 24;
    lt->table[8] = 152;
    lt->table[9] = 112;

    for (i = 0; i < 24; ++i)
    {
        lt->trans[i] = 256 + i;
    }

    for (i = 0; i < 144; ++i)
    {
        lt->trans[24 + i] = i;
    }

    for (i = 0; i < 8; ++i)
    {
        lt->trans[24 + 144 + i] = 280 + i;
    }

    for (i = 0; i < 112; ++i)
    {
        lt->trans[24 + 144 + 8 + i] = 144 + i;
    }

    /* build fixed distance tree */
    for (i = 0; i < 16; ++i)
    {
        dt->table[i] = 0;
    }

    dt->table[5] = 32;

    for (i = 0; i < 32; ++i)
    {
        dt->trans[i] = i;
    }
}

/* given an array of code lengths, build a tree */
static TINF_RET_t tinf_build_tree(struct tinf_tree* t, const unsigned char* lengths,
                                  unsigned int num)
{
    unsigned short offs[16];
    unsigned int i, sum;

    assert(num < 288);

    /* clear code length count table */
    for (i = 0; i < 16; ++i)
    {
        t->table[i] = 0;
    }

    /* scan symbol lengths, and sum code length counts */
    for (i = 0; i < num; ++i)
    {
        t->table[lengths[i]]++;
    }

    t->table[0] = 0;

    /* compute offset table for distribution sort */
    for (sum = 0, i = 0; i < 16; ++i)
    {
        offs[i] = sum;
        sum += t->table[i];
    }

    /* create code->symbol translation table (symbols sorted by code) */
    for (i = 0; i < num; ++i)
    {
        if (lengths[i])
        {
            t->trans[offs[lengths[i]]++] = i;
        }
    }

    return TINF_OK;
}

/* ---------------------- *
 * -- decode functions -- *
 * ---------------------- */

/*
 * The bit reader code attempts to go through the read buffer in 32-bit aligned reads.
 * Since the beginning of the stream may not be on a 4-byte boundary the first read
 * may only partially fill the bit buffer.
 */
static void tinf_bitread_start(struct tinf_data* d)
{
    d->bitcount = (4 - (d->read_buf_ptr & 3)) * 8;
    int read_buf_ptr = d->read_buf_ptr & ~3;
    d->tag = le32toh(d->read_buf.d[read_buf_ptr / 4]) >> (32 - d->bitcount);
    d->refill_bitcount = 0;
    d->read_buf_ptr = read_buf_ptr + 4;

    if (d->read_buf_ptr >= READ_BUF_SIZE)
    {
        refill_read_buf(d);
    }
}

/*
 * If we have to switch to bytewise reading for uncompressed blocks, we must return all
 * the bits we have not yet consumed.
 */
static void tinf_bitread_stop(struct tinf_data* d)
{
    /* In most cases we can just rewind the read pointer to the first unused byte
     * and pretend nothing happened. */
    d->read_buf_ptr -= (d->bitcount + d->refill_bitcount) / 8;

    if (d->read_buf_ptr < 0)
    {
        /* If we buffered too greedily and have to return to a position in the previous
         * buffered hash block, reconstruct that tail from the tag and refill buffer */
        uint64_t rollback = d->refill << d->bitcount | (d->tag & ((1 << d->bitcount) - 1));
        d->read_buf.rollback = htole64(rollback << (64 - d->bitcount - d->refill_bitcount));
    }
}

/* Refill the bit reservoir from the read buffer.
 * After this 16 <= d->bitcount <= 32 */
static void tinf_refill(struct tinf_data* d)
{
    assert(d->bitcount < 16);
    assert(d->refill_bitcount < 16);

    /* refill tag from refill buffer */
    if (d->refill_bitcount)
    {
        d->tag |= d->refill << d->bitcount;;
        d->bitcount += d->refill_bitcount;
        d->refill_bitcount = 0;
    }

    /* iff tag still has less than 16 bits, refill from memory */
    if (d->bitcount < 16)
    {
        const uint32_t memword = le32toh(d->read_buf.d[d->read_buf_ptr / 4]);
        d->tag |= memword << d->bitcount;
        d->refill = memword >> (32 - d->bitcount);
        d->refill_bitcount = d->bitcount;
        d->bitcount = 32;

        d->read_buf_ptr += 4;

        if (d->read_buf_ptr >= READ_BUF_SIZE)
        {
            refill_read_buf(d);
        }
    }
}

#define likely(x) __builtin_expect((x),1)
#define unlikely(x) __builtin_expect((x),0)

/* get num bits from source stream */
static inline __attribute__((always_inline)) unsigned int tinf_getbits(struct tinf_data* d, int num)
{
    assert(num >= 0 && num <= 16);

    /* Refill if not enough bits available */
    if (unlikely(d->bitcount < num))
    {
        tinf_refill(d);
    }

    /* get bits from tag */
    uint32_t bits = d->tag & ((1UL << num) - 1);

    /* remove bits from tag */
    d->tag >>= num;
    d->bitcount -= num;

    return bits;
}

/* read a num bit value from stream and add base */
static inline __attribute__((always_inline)) unsigned int tinf_getbits_base(struct tinf_data* d, int num, int base)
{
    return base + (num ? tinf_getbits(d, num) : 0);
}

/* given a data stream and a tree, decode a symbol */
static TINF_RET_t tinf_decode_symbol(struct tinf_data* d, const struct tinf_tree* t)
{
    int sum = 0, cur = 0, len = 0;

    /* Make sure we have the maximum number of bits available */
    if (unlikely(d->bitcount < 15))
    {
        tinf_refill(d);
    }

    uint32_t tag = d->tag;
    unsigned int bitcount = d->bitcount;

    /* get more bits while code value is above sum */
    do
    {
        cur = 2 * cur + (tag & 1);
        tag >>= 1;
        bitcount--;

        ++len;

        assert(len <= 15);

        sum += t->table[len];
        cur -= t->table[len];
    }
    while (cur >= 0);

    d->tag = tag;
    d->bitcount = bitcount;

    assert(sum + cur >= 0 && sum + cur < 288);

    return t->trans[sum + cur];
}

/* given a data stream, decode dynamic trees from it */
static TINF_RET_t tinf_decode_trees(struct tinf_data* d,
                                    struct tinf_tree* lt,
                                    struct tinf_tree* dt)
{
    unsigned char lengths[288 + 32];

    /* special ordering of code length codes */
    static const unsigned char clcidx[19] =
    {
        16, 17, 18, 0,  8, 7,  9, 6, 10, 5,
        11,  4, 12, 3, 13, 2, 14, 1, 15
    };

    unsigned int hlit, hdist, hclen;
    unsigned int i, num, length;
    int res;

    /* get 5 bits HLIT (257-286) */
    hlit = tinf_getbits_base(d, 5, 257);

    /* get 5 bits HDIST (1-32) */
    hdist = tinf_getbits_base(d, 5, 1);

    /* get 4 bits HCLEN (4-19) */
    hclen = tinf_getbits_base(d, 4, 4);

    for (i = 0; i < 19; ++i)
    {
        lengths[i] = 0;
    }

    /* read code lengths for code length alphabet */
    for (i = 0; i < hclen; ++i)
    {
        /* get 3 bits code length (0-7) */
        unsigned int clen = tinf_getbits(d, 3);

        lengths[clcidx[i]] = clen;
    }

    /* build code length tree (in literal/length tree to save space) */
    res = tinf_build_tree(lt, lengths, 19);

    if (res != TINF_OK)
    {
        return res;
    }

    /* decode code lengths for the dynamic trees */
    for (num = 0; num < hlit + hdist; )
    {
        int sym = tinf_decode_symbol(d, lt);

        switch (sym)
        {
            case 16:
                /* copy previous code length 3-6 times (read 2 bits) */
                sym = lengths[num - 1];
                length = tinf_getbits_base(d, 2, 3);
                break;

            case 17:
                /* repeat code length 0 for 3-10 times (read 3 bits) */
                sym = 0;
                length = tinf_getbits_base(d, 3, 3);
                break;

            case 18:
                /* repeat code length 0 for 11-138 times (read 7 bits) */
                sym = 0;
                length = tinf_getbits_base(d, 7, 11);
                break;

            default:
                /* values 0-15 represent the actual code lengths */
                length = 1;
                break;
        }

        while (length--)
        {
            lengths[num++] = sym;
        }
    }

    /* build dynamic trees */
    res = tinf_build_tree(lt, lengths, hlit);

    if (res != TINF_OK)
    {
        return res;
    }

    res = tinf_build_tree(dt, lengths + hlit, hdist);

    if (res != TINF_OK)
    {
        return res;
    }

    return TINF_OK;
}

/*
 * This filter treats the entire file content as a stream of PowerPC instructions
 * and replaces any relative branch-and-link instructions with absolute ones. This
 * causes calls to the same function to be identical across the code, improving
 * compression rate.
 *
 * Of course there will be false positives, but since we undo the transformation
 * after decompression the false positives will be undone as well. They will
 * harm the compression ratio though, so only files that consist mostly of PPC code
 * should be treated by this filter.
 */
static void ppc_filter(struct tinf_data* d, uint32_t size)
{
    if (!d->ppc_filter)
    {
        return;
    }

    size &= ~3;

    for (uint32_t i = 0; i < size; i += sizeof(uint32_t))
    {
        uint32_t* insn = (uint32_t*)(d->dict + i);
        uint32_t value = be32toh(*insn);

        if ((value & 0xFC000003) == 0x48000001)
        {
            *insn = htobe32(((value + (d->filter_offset + i)) & 0x03FFFFFC) | 0x48000001);
        }
    }
}

static void ppc_filter_inverse(struct tinf_data* d, uint32_t size)
{
    if (!d->ppc_filter)
    {
        return;
    }

    size &= ~3;

    for (uint32_t i = 0; i < size; i += sizeof(uint32_t))
    {
        uint32_t* insn = (uint32_t*)(d->dict + i);
        uint32_t value = be32toh(*insn);

        if ((value & 0xFC000003) == 0x48000001)
        {
            *insn = htobe32(((value - (d->filter_offset + i)) & 0x03FFFFFC) | 0x48000001);
        }
    }
}

/*
 * Behind the dictionary we maintain a tail of the maximum match length which is
 * identical to the beginning of the dictionary at all times, so we can copy
 * matches without bounds checking each byte.
 * To achieve this we write beyond the end of the dictionary for at least the
 * max match length. As soon as we cross that boundary, we copy the entire
 * tail to the beginning of the dictionary and adjust our pointers.
 */
static const uint32_t DICTIONARY_SIZE = FileArchive::DICTIONARY_SIZE;
static const uint32_t DICTIONARY_TAIL = FileArchive::MAX_MATCH_LEN;

static TINF_RET_t tinf_stream_wrap(struct tinf_data* d)
{
    /* Undo the PPC transformation before we hand the block to the receiver */
    ppc_filter_inverse(d, DICTIONARY_SIZE);

    TINF_RET_t rc = d->receiver->consume(d->dict, DICTIONARY_SIZE);

    if (rc)
    {
        return rc;
    }

    /* Since the buffer is also our dictionary we now have to re-apply the PPC
     * transformation to prevent corruption - sucks but what can you do */
    ppc_filter(d, DICTIONARY_SIZE);
    d->filter_offset += DICTIONARY_SIZE;

    unsigned char* dict_end = d->dict + DICTIONARY_SIZE;
    uint32_t tail_length = d->dest - dict_end;
    memcpy(d->dict, dict_end, tail_length);
    d->dest -= DICTIONARY_SIZE;
    d->destSpace += DICTIONARY_SIZE;

    return TINF_OK;
}

/* ----------------------------- *
 * -- block inflate functions -- *
 * ----------------------------- */

#define TINF_PRE_WRITE(d, len)                            \
    (d)->destSpace -= len;                                \
    if (((d)->destSpace < 0) && !((d)->receiver)) {       \
        return TINF_BUF_ERROR;                            \
    }

#define TINF_POST_WRITE(d, len)                           \
    (d)->dest += len;                                     \
    if (((d)->destSpace <= 0) && (d)->receiver) {         \
        TINF_RET_t rc = tinf_stream_wrap(d);              \
        if (rc) return rc;                                \
    }

/* given a stream and two trees, inflate a block of data */
static TINF_RET_t tinf_inflate_block_data(struct tinf_data* d,
        struct tinf_tree* lt,
        struct tinf_tree* dt)
{
    /* extra bits and base tables for length codes */
    static const unsigned char length_bits[30] =
    {
        0, 0, 0, 0, 0, 0, 0, 0, 1, 1,
        1, 1, 2, 2, 2, 2, 3, 3, 3, 3,
        4, 4, 4, 4, 5, 5, 5, 5, 0, 127
    };

    static const unsigned short length_base[30] =
    {
        3,  4,  5,   6,   7,   8,   9,  10,  11,  13,
        15, 17, 19,  23,  27,  31,  35,  43,  51,  59,
        67, 83, 99, 115, 131, 163, 195, 227, 258,   0
    };

    /* extra bits and base tables for distance codes */
    static const unsigned char dist_bits[30] =
    {
        0, 0,  0,  0,  1,  1,  2,  2,  3,  3,
        4, 4,  5,  5,  6,  6,  7,  7,  8,  8,
        9, 9, 10, 10, 11, 11, 12, 12, 13, 13
    };

    static const unsigned short dist_base[30] =
    {
        1,    2,    3,    4,    5,    7,    9,    13,    17,    25,
        33,   49,   65,   97,  129,  193,  257,   385,   513,   769,
        1025, 1537, 2049, 3073, 4097, 6145, 8193, 12289, 16385, 24577
    };

    for (;;)
    {
        int sym = tinf_decode_symbol(d, lt);

        /* check for end of block */
        if (sym == 256)
        {
            return TINF_OK;
        }

        if (sym < 256)
        {
            TINF_PRE_WRITE(d, 1);
            *d->dest = sym;
            TINF_POST_WRITE(d, 1);
        }
        else
        {
            const unsigned char* source;
            int length, dist, offs;
            int i;

            sym -= 257;

            /* possibly get more bits from length code */
            length = tinf_getbits_base(d, length_bits[sym],
                                       length_base[sym]);

            dist = tinf_decode_symbol(d, dt);

            /* possibly get more bits from distance code */
            offs = tinf_getbits_base(d, dist_bits[dist],
                                     dist_base[dist]);

            TINF_PRE_WRITE(d, length);

            /* copy match */
            source = d->dest - offs;

            if (source < d->dict)
            {
                if (!d->receiver)
                {
                    // Pointing in front of output buffer is an
                    // error in the non-streaming case.
                    return TINF_DATA_ERROR;
                }

                // In the streaming case the dict is a ring buffer
                source += DICTIONARY_SIZE;
            }

            for (i = 0; i < length; ++i)
            {
                d->dest[i] = source[i];
            }

            TINF_POST_WRITE(d, length);
        }
    }
}

/* inflate an uncompressed block of data */
static TINF_RET_t tinf_inflate_uncompressed_block(struct tinf_data* d)
{
    int length, invlength;

    /* make sure we start next block on a byte boundary */
    tinf_getbits(d, (d->bitcount + d->refill_bitcount) & 7);

    /* get length */
    length = tinf_getbits(d, 16);

    /* get one's complement of length */
    invlength = tinf_getbits(d, 16);

    /* check length */
    if (length != (~invlength & 0x0000FFFF))
    {
        return TINF_DATA_ERROR;
    }

    if (d->destSpace < length && !d->receiver)
    {
        return TINF_BUF_ERROR;
    }

    tinf_bitread_stop(d);

    /* Copy data until exhausted */
    /* This is much more complicated than it needs to be since we have to
     * account for optional hashing and streaming */
    while (length)
    {
        const int read_buf_remaining = READ_BUF_SIZE - d->read_buf_ptr;
        int chunk_size = d->destSpace;

        if (read_buf_remaining < chunk_size)
        {
            chunk_size = read_buf_remaining;
        }

        if (length < chunk_size)
        {
            chunk_size = length;
        }

        TINF_PRE_WRITE(d, chunk_size);
        memcpy(d->dest, d->read_buf.b + d->read_buf_ptr, chunk_size);
        TINF_POST_WRITE(d, chunk_size);

        d->read_buf_ptr += chunk_size;

        if (d->read_buf_ptr >= READ_BUF_SIZE)
        {
            refill_read_buf(d);
        }

        length -= chunk_size;
    }

    tinf_bitread_start(d);

    return TINF_OK;
}

/* inflate a block of data compressed with fixed huffman trees */
static TINF_RET_t tinf_inflate_fixed_block(struct tinf_data* d)
{
    /* build fixed huffman trees */
    tinf_build_fixed_trees(&d->ltree, &d->dtree);

    /* decode block using fixed trees */
    return tinf_inflate_block_data(d, &d->ltree, &d->dtree);
}

/* inflate a block of data compressed with dynamic huffman trees */
static TINF_RET_t tinf_inflate_dynamic_block(struct tinf_data* d)
{
    /* decode trees from stream */
    TINF_RET_t res = tinf_decode_trees(d, &d->ltree, &d->dtree);

    if (res != TINF_OK)
    {
        return res;
    }

    /* decode block using decoded trees */
    return tinf_inflate_block_data(d, &d->ltree, &d->dtree);
}

/* ---------------------- *
 * -- public functions -- *
 * ---------------------- */

/* inflate stream from source to dest */
TINF_RET_t tinf_uncompress(void* dest, unsigned int destLen,
                           const void* source, unsigned int sourceLen,
                           FileArchive::StreamReceiver* receiver,
                           sha3_t* hash, int filter)
{
    struct tinf_data d;
    int bfinal;

    /* initialise data */
    d.source = (const uint64_t*) source;
    d.source_remaining = sourceLen;
    d.tag = 0;
    d.bitcount = 0;
    d.read_buf_ptr = 0;
    d.receiver = receiver;
    d.ppc_filter = filter == TINF_FILTER_PPC;
    d.filter_offset = 0;

#ifdef __USE_SHA3__
    d.hash = hash;

    if (hash)
    {
        sha3_init(&d.hash_ctx);
    }

#else

    if (hash)
    {
        return ARC_FUNCTIONALITY_NOT_SUPPORTED;
    }

#endif

    refill_read_buf(&d);
    tinf_bitread_start(&d);

    d.dict = d.dest = (unsigned char*) dest;
    d.destSpace = receiver ? (DICTIONARY_SIZE + DICTIONARY_TAIL) : destLen;

    do
    {
        unsigned int btype;
        TINF_RET_t res;

        /* read final block flag */
        bfinal = tinf_getbits(&d, 1);

        /* read block type (2 bits) */
        btype = tinf_getbits(&d, 2);

        /* decompress block */
        switch (btype)
        {
            case 0:
                /* decompress uncompressed block */
                res = tinf_inflate_uncompressed_block(&d);
                break;

            case 1:
                /* decompress block with fixed huffman trees */
                res = tinf_inflate_fixed_block(&d);
                break;

            case 2:
                /* decompress block with dynamic huffman trees */
                res = tinf_inflate_dynamic_block(&d);
                break;

            default:
                res = TINF_DATA_ERROR;
                break;
        }

        if (res != TINF_OK)
        {
            ARC_ERROR("tinf_uncompress: failed with error %d", res);
            return res;
        }
    }
    while (!bfinal);

    ppc_filter_inverse(&d, d.dest - d.dict);

    if (d.receiver && d.dest != d.dict)
    {
        d.receiver->consume(d.dict, d.dest - d.dict);
    }

    return TINF_OK;
}
