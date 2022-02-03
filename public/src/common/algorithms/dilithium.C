/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/common/algorithms/dilithium.C $                    */
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
/* Based on the public domain implementation in
 * crypto_hash/keccakc512/simple/ from http://bench.cr.yp.to/supercop.html
 * by Ronny Van Keer
 * and the public domain "TweetFips202" implementation
 * from https://twitter.com/tweetfips202
 * by Gilles Van Assche, Daniel J. Bernstein, and Peter Schwabe */

/* cross-compliance against ref.impl verified at:
 *    497a98bc7efe48cb0d18fd20d6d9a4b5629406ef  2020-07-21
 *
 * Dilithium variants supported (K/L):
 *   Dil III   Dil-5/4
 *   Dil IV    Dil-6/5
 *   Dil V     Dil-8/7
 *
 * includes test stub if compiled with -DSTANDALONE (see end)
 */

#include <stdint.h>
#include <stddef.h>
#include <ppe42_string.h>
#ifdef STANDALONE
#include <stdlib.h>        /* debug only */
#include <stdio.h>         // diags only
#endif

#include "pqalgs.h"
#include "common-base.h"


#if 1       //-----  delimiter: pseudo-RNG  ----------------------------------
// restricted to 8n bytes; all our calls are such
//
/*--------------------------------------
 * 64-bit mixer fn; ensure input is not 0 in the reasonable future, as it
 * is preserved to output
 *
 * the 'nasam()' construct ("Not Another Strange Acronym Mixer"), from
       mostlymangling.blogspot.com/2020/01/
 *     nasam-not-another-strange-acronym-mixer.html  (Pelle Evensen)
 * see also:
 *    Chris Wellons, nullprogram.com/blog/2018/07/31
 *    "Prospecting for hash functions"
 */
#define  CU__RR64(v, n)  (((v) >> (n)) | ((v) << (64-(n))))


static uint64_t rng_seed;


//--------------------------------------
static inline uint64_t cu_mix64(uint64_t seed)
{
    seed ^= CU__RR64(seed, 25) ^ CU__RR64(seed, 47);
    seed *= UINT64_C(0x9e6c63d0676a9a99);

    seed ^= (seed >> 23) ^ (seed >> 51);
    seed *= UINT64_C(0x9e6d62d06f6a9a9b);

    return seed ^ (seed >> 23) ^ (seed >> 51);
}


//--------------------------------------
// writes 8N-byte units, do not use with other granularity
static void randombytes(unsigned char *r, size_t rbytes)
{
    if (r && rbytes) {
        unsigned int i = 0;

        while ((i+1)*8 <= rbytes) {
            uint64_t v = cu_mix64(++rng_seed);

            MSBF8_WRITE(r +i*8, v);
            ++i;
        }
    }
}
#endif     //-----  pseudo-RNG  ---------------------------------------------


#if 1       /*-----  delimiter: Keccak  ------------------------------------*/

// #include "fips202.h"
#define SHAKE128_RATE 168
#define SHAKE256_RATE 136
#define SHA3_256_RATE 136
#define SHA3_512_RATE 72
//
typedef struct {
  uint64_t s[25];
  unsigned int pos;
} Keccak_state;


#define NROUNDS 24
#define ROL(a, offset) ((a << offset) ^ (a >> (64-offset)))

/*************************************************
* Name:        load64
*
* Description: Load 8 bytes into uint64_t in little-endian order
*
* Arguments:   - const uint8_t *x: pointer to input byte array
*
* Returns the loaded 64-bit unsigned integer
**************************************************/
static uint64_t load64(const uint8_t x[8]) {
  unsigned int i;
  uint64_t r = 0;

  for (i=0;i<8;i++)
    r |= (uint64_t)x[i] << 8*i;

  return r;
}


/*************************************************
* Name:        store64
*
* Description: Store a 64-bit integer to array of 8 bytes in little-endian order
*
* Arguments:   - uint8_t *x: pointer to the output byte array (allocated)
*              - uint64_t u: input 64-bit unsigned integer
**************************************************/
static void store64(uint8_t x[8], uint64_t u) {
  unsigned int i;

  for (i=0;i<8;i++)
    x[i] = u >> 8*i;
}


/* Keccak round constants */
static const uint64_t KeccakF_RoundConstants[NROUNDS] = {
  (uint64_t)0x0000000000000001ULL,
  (uint64_t)0x0000000000008082ULL,
  (uint64_t)0x800000000000808aULL,
  (uint64_t)0x8000000080008000ULL,
  (uint64_t)0x000000000000808bULL,
  (uint64_t)0x0000000080000001ULL,
  (uint64_t)0x8000000080008081ULL,
  (uint64_t)0x8000000000008009ULL,
  (uint64_t)0x000000000000008aULL,
  (uint64_t)0x0000000000000088ULL,
  (uint64_t)0x0000000080008009ULL,
  (uint64_t)0x000000008000000aULL,
  (uint64_t)0x000000008000808bULL,
  (uint64_t)0x800000000000008bULL,
  (uint64_t)0x8000000000008089ULL,
  (uint64_t)0x8000000000008003ULL,
  (uint64_t)0x8000000000008002ULL,
  (uint64_t)0x8000000000000080ULL,
  (uint64_t)0x000000000000800aULL,
  (uint64_t)0x800000008000000aULL,
  (uint64_t)0x8000000080008081ULL,
  (uint64_t)0x8000000000008080ULL,
  (uint64_t)0x0000000080000001ULL,
  (uint64_t)0x8000000080008008ULL
};

/*************************************************
* Name:        KeccakF1600_StatePermute
*
* Description: The Keccak F1600 Permutation
*
* Arguments:   - uint64_t *state: pointer to input/output Keccak state
**************************************************/
static void KeccakF1600_StatePermute(uint64_t state[25])
{
        int round;

        uint64_t Aba, Abe, Abi, Abo, Abu;
        uint64_t Aga, Age, Agi, Ago, Agu;
        uint64_t Aka, Ake, Aki, Ako, Aku;
        uint64_t Ama, Ame, Ami, Amo, Amu;
        uint64_t Asa, Ase, Asi, Aso, Asu;
        uint64_t BCa, BCe, BCi, BCo, BCu;
        uint64_t Da, De, Di, Do, Du;
        uint64_t Eba, Ebe, Ebi, Ebo, Ebu;
        uint64_t Ega, Ege, Egi, Ego, Egu;
        uint64_t Eka, Eke, Eki, Eko, Eku;
        uint64_t Ema, Eme, Emi, Emo, Emu;
        uint64_t Esa, Ese, Esi, Eso, Esu;

        //copyFromState(A, state)
        Aba = state[ 0];
        Abe = state[ 1];
        Abi = state[ 2];
        Abo = state[ 3];
        Abu = state[ 4];
        Aga = state[ 5];
        Age = state[ 6];
        Agi = state[ 7];
        Ago = state[ 8];
        Agu = state[ 9];
        Aka = state[10];
        Ake = state[11];
        Aki = state[12];
        Ako = state[13];
        Aku = state[14];
        Ama = state[15];
        Ame = state[16];
        Ami = state[17];
        Amo = state[18];
        Amu = state[19];
        Asa = state[20];
        Ase = state[21];
        Asi = state[22];
        Aso = state[23];
        Asu = state[24];

        for ( round = 0; round < NROUNDS; round += 2 )
        {
            //    prepareTheta
            BCa = Aba^Aga^Aka^Ama^Asa;
            BCe = Abe^Age^Ake^Ame^Ase;
            BCi = Abi^Agi^Aki^Ami^Asi;
            BCo = Abo^Ago^Ako^Amo^Aso;
            BCu = Abu^Agu^Aku^Amu^Asu;

            //thetaRhoPiChiIotaPrepareTheta(round  , A, E)
            Da = BCu^ROL(BCe, 1);
            De = BCa^ROL(BCi, 1);
            Di = BCe^ROL(BCo, 1);
            Do = BCi^ROL(BCu, 1);
            Du = BCo^ROL(BCa, 1);

            Aba ^= Da;
            BCa = Aba;
            Age ^= De;
            BCe = ROL(Age, 44);
            Aki ^= Di;
            BCi = ROL(Aki, 43);
            Amo ^= Do;
            BCo = ROL(Amo, 21);
            Asu ^= Du;
            BCu = ROL(Asu, 14);
            Eba =   BCa ^((~BCe)&  BCi );
            Eba ^= (uint64_t)KeccakF_RoundConstants[round];
            Ebe =   BCe ^((~BCi)&  BCo );
            Ebi =   BCi ^((~BCo)&  BCu );
            Ebo =   BCo ^((~BCu)&  BCa );
            Ebu =   BCu ^((~BCa)&  BCe );

            Abo ^= Do;
            BCa = ROL(Abo, 28);
            Agu ^= Du;
            BCe = ROL(Agu, 20);
            Aka ^= Da;
            BCi = ROL(Aka,  3);
            Ame ^= De;
            BCo = ROL(Ame, 45);
            Asi ^= Di;
            BCu = ROL(Asi, 61);
            Ega =   BCa ^((~BCe)&  BCi );
            Ege =   BCe ^((~BCi)&  BCo );
            Egi =   BCi ^((~BCo)&  BCu );
            Ego =   BCo ^((~BCu)&  BCa );
            Egu =   BCu ^((~BCa)&  BCe );

            Abe ^= De;
            BCa = ROL(Abe,  1);
            Agi ^= Di;
            BCe = ROL(Agi,  6);
            Ako ^= Do;
            BCi = ROL(Ako, 25);
            Amu ^= Du;
            BCo = ROL(Amu,  8);
            Asa ^= Da;
            BCu = ROL(Asa, 18);
            Eka =   BCa ^((~BCe)&  BCi );
            Eke =   BCe ^((~BCi)&  BCo );
            Eki =   BCi ^((~BCo)&  BCu );
            Eko =   BCo ^((~BCu)&  BCa );
            Eku =   BCu ^((~BCa)&  BCe );

            Abu ^= Du;
            BCa = ROL(Abu, 27);
            Aga ^= Da;
            BCe = ROL(Aga, 36);
            Ake ^= De;
            BCi = ROL(Ake, 10);
            Ami ^= Di;
            BCo = ROL(Ami, 15);
            Aso ^= Do;
            BCu = ROL(Aso, 56);
            Ema =   BCa ^((~BCe)&  BCi );
            Eme =   BCe ^((~BCi)&  BCo );
            Emi =   BCi ^((~BCo)&  BCu );
            Emo =   BCo ^((~BCu)&  BCa );
            Emu =   BCu ^((~BCa)&  BCe );

            Abi ^= Di;
            BCa = ROL(Abi, 62);
            Ago ^= Do;
            BCe = ROL(Ago, 55);
            Aku ^= Du;
            BCi = ROL(Aku, 39);
            Ama ^= Da;
            BCo = ROL(Ama, 41);
            Ase ^= De;
            BCu = ROL(Ase,  2);
            Esa =   BCa ^((~BCe)&  BCi );
            Ese =   BCe ^((~BCi)&  BCo );
            Esi =   BCi ^((~BCo)&  BCu );
            Eso =   BCo ^((~BCu)&  BCa );
            Esu =   BCu ^((~BCa)&  BCe );

            //    prepareTheta
            BCa = Eba^Ega^Eka^Ema^Esa;
            BCe = Ebe^Ege^Eke^Eme^Ese;
            BCi = Ebi^Egi^Eki^Emi^Esi;
            BCo = Ebo^Ego^Eko^Emo^Eso;
            BCu = Ebu^Egu^Eku^Emu^Esu;

            //thetaRhoPiChiIotaPrepareTheta(round+1, E, A)
            Da = BCu^ROL(BCe, 1);
            De = BCa^ROL(BCi, 1);
            Di = BCe^ROL(BCo, 1);
            Do = BCi^ROL(BCu, 1);
            Du = BCo^ROL(BCa, 1);

            Eba ^= Da;
            BCa = Eba;
            Ege ^= De;
            BCe = ROL(Ege, 44);
            Eki ^= Di;
            BCi = ROL(Eki, 43);
            Emo ^= Do;
            BCo = ROL(Emo, 21);
            Esu ^= Du;
            BCu = ROL(Esu, 14);
            Aba =   BCa ^((~BCe)&  BCi );
            Aba ^= (uint64_t)KeccakF_RoundConstants[round+1];
            Abe =   BCe ^((~BCi)&  BCo );
            Abi =   BCi ^((~BCo)&  BCu );
            Abo =   BCo ^((~BCu)&  BCa );
            Abu =   BCu ^((~BCa)&  BCe );

            Ebo ^= Do;
            BCa = ROL(Ebo, 28);
            Egu ^= Du;
            BCe = ROL(Egu, 20);
            Eka ^= Da;
            BCi = ROL(Eka, 3);
            Eme ^= De;
            BCo = ROL(Eme, 45);
            Esi ^= Di;
            BCu = ROL(Esi, 61);
            Aga =   BCa ^((~BCe)&  BCi );
            Age =   BCe ^((~BCi)&  BCo );
            Agi =   BCi ^((~BCo)&  BCu );
            Ago =   BCo ^((~BCu)&  BCa );
            Agu =   BCu ^((~BCa)&  BCe );

            Ebe ^= De;
            BCa = ROL(Ebe, 1);
            Egi ^= Di;
            BCe = ROL(Egi, 6);
            Eko ^= Do;
            BCi = ROL(Eko, 25);
            Emu ^= Du;
            BCo = ROL(Emu, 8);
            Esa ^= Da;
            BCu = ROL(Esa, 18);
            Aka =   BCa ^((~BCe)&  BCi );
            Ake =   BCe ^((~BCi)&  BCo );
            Aki =   BCi ^((~BCo)&  BCu );
            Ako =   BCo ^((~BCu)&  BCa );
            Aku =   BCu ^((~BCa)&  BCe );

            Ebu ^= Du;
            BCa = ROL(Ebu, 27);
            Ega ^= Da;
            BCe = ROL(Ega, 36);
            Eke ^= De;
            BCi = ROL(Eke, 10);
            Emi ^= Di;
            BCo = ROL(Emi, 15);
            Eso ^= Do;
            BCu = ROL(Eso, 56);
            Ama =   BCa ^((~BCe)&  BCi );
            Ame =   BCe ^((~BCi)&  BCo );
            Ami =   BCi ^((~BCo)&  BCu );
            Amo =   BCo ^((~BCu)&  BCa );
            Amu =   BCu ^((~BCa)&  BCe );

            Ebi ^= Di;
            BCa = ROL(Ebi, 62);
            Ego ^= Do;
            BCe = ROL(Ego, 55);
            Eku ^= Du;
            BCi = ROL(Eku, 39);
            Ema ^= Da;
            BCo = ROL(Ema, 41);
            Ese ^= De;
            BCu = ROL(Ese, 2);
            Asa =   BCa ^((~BCe)&  BCi );
            Ase =   BCe ^((~BCi)&  BCo );
            Asi =   BCi ^((~BCo)&  BCu );
            Aso =   BCo ^((~BCu)&  BCa );
            Asu =   BCu ^((~BCa)&  BCe );
        }

        //copyToState(state, A)
        state[ 0] = Aba;
        state[ 1] = Abe;
        state[ 2] = Abi;
        state[ 3] = Abo;
        state[ 4] = Abu;
        state[ 5] = Aga;
        state[ 6] = Age;
        state[ 7] = Agi;
        state[ 8] = Ago;
        state[ 9] = Agu;
        state[10] = Aka;
        state[11] = Ake;
        state[12] = Aki;
        state[13] = Ako;
        state[14] = Aku;
        state[15] = Ama;
        state[16] = Ame;
        state[17] = Ami;
        state[18] = Amo;
        state[19] = Amu;
        state[20] = Asa;
        state[21] = Ase;
        state[22] = Asi;
        state[23] = Aso;
        state[24] = Asu;
}


/*************************************************
* Name:        keccak_init
*
* Description: Initializes the Keccak state.
*
* Arguments:   - Keccak_state *state: pointer to Keccak state
**************************************************/
static void keccak_init(Keccak_state *state)
{
  unsigned int i;
  for (i=0;i<25;i++)
    state->s[i] = 0;
  state->pos = 0;
}


/*************************************************
* Name:        keccak_absorb
*
* Description: Absorb step of Keccak; incremental.
*
* Arguments:   - uint64_t *s:      pointer to Keccak state
*              - unsigned int r:   rate in bytes (e.g., 168 for SHAKE128)
*              - unsigned int pos: position in current block to be absorbed
*              - const uint8_t *m: pointer to input to be absorbed into s
*              - size_t mlen:      length of input in bytes
*
* Returns new position pos in current block
**************************************************/
static unsigned int keccak_absorb(uint64_t s[25],
                                  unsigned int r,
                                  unsigned int pos,
                                  const uint8_t *m,
                                  size_t mlen)
{
  unsigned int i;
  uint8_t t[8] = {0};

  if (pos & 7) {
    i = pos & 7;
    while(i < 8 && mlen > 0) {
      t[i++] = *m++;
      mlen--;
      pos++;
    }
    s[(pos-i)/8] ^= load64(t);
  }

  if (pos && mlen >= r-pos) {
    for (i=0;i<(r-pos)/8;i++)
      s[pos/8+i] ^= load64(m+8*i);
    m += r-pos;
    mlen -= r-pos;
    pos = 0;
    KeccakF1600_StatePermute(s);
  }

  while(mlen >= r) {
    for (i=0;i<r/8;i++)
      s[i] ^= load64(m+8*i);
    m += r;
    mlen -= r;
    KeccakF1600_StatePermute(s);
  }

  for (i=0;i<mlen/8;i++)
    s[pos/8+i] ^= load64(m+8*i);
  m += 8*i;
  mlen -= 8*i;
  pos += 8*i;

  if (mlen) {
    for (i=0;i<8;i++)
      t[i] = 0;
    for (i=0;i<mlen;i++)
      t[i] = m[i];
    s[pos/8] ^= load64(t);
    pos += mlen;
  }

  return pos;
}


/*************************************************
* Name:        keccak_finalize
*
* Description: Finalize absorb step.
*
* Arguments:   - uint64_t *s:      pointer to Keccak state
*              - unsigned int r:   rate in bytes (e.g., 168 for SHAKE128)
*              - unsigned int pos: position in current block to be absorbed
*              - uint8_t p:        domain separation byte
**************************************************/
static void keccak_finalize(uint64_t s[25], unsigned int r, unsigned int pos, uint8_t p)
{
  unsigned int i,j;

  i = pos >> 3;
  j = pos & 7;
  s[i] ^= (uint64_t)p << 8*j;
  s[r/8-1] ^= 1ULL << 63;
}


/*************************************************
* Name:        keccak_squeezeblocks
*
* Description: Squeeze step of Keccak. Squeezes full blocks of r bytes each.
*              Modifies the state. Can be called multiple times to keep
*              squeezing, i.e., is incremental. Assumes zero bytes of current
*              block have already been squeezed.
*
* Arguments:   - uint8_t *out:   pointer to output blocks
*              - size_t nblocks: number of blocks to be squeezed (written to out)
*              - uint64_t *s:    pointer to input/output Keccak state
*              - unsigned int r: rate in bytes (e.g., 168 for SHAKE128)
**************************************************/
static void keccak_squeezeblocks(uint8_t *out,
                                 size_t nblocks,
                                 uint64_t s[25],
                                 unsigned int r)
{
  unsigned int i;

  while(nblocks > 0) {
    KeccakF1600_StatePermute(s);
    for (i=0;i<r/8;i++)
      store64(out + 8*i, s[i]);
    out += r;
    nblocks--;
  }
}


/*************************************************
* Name:        keccak_squeeze
*
* Description: Squeeze step of Keccak. Squeezes arbitratrily many bytes.
*              Modifies the state. Can be called multiple times to keep
*              squeezing, i.e., is incremental.
*
* Arguments:   - uint8_t *out:     pointer to output
*              - size_t outlen:    number of bytes to be squeezed (written to out)
*              - uint64_t *s:      pointer to input/output Keccak state
*              - unsigned int r:   rate in bytes (e.g., 168 for SHAKE128)
*              - unsigned int pos: number of bytes in current block already squeezed
*
* Returns new position pos in current block
**************************************************/
static unsigned int keccak_squeeze(uint8_t *out,
                                   size_t outlen,
                                   uint64_t s[25],
                                   unsigned int r,
                                   unsigned int pos)
{
  unsigned int i;
  uint8_t t[8];

  if (pos & 7) {
    store64(t,s[pos/8]);
    i = pos & 7;
    while(i < 8 && outlen > 0) {
      *out++ = t[i++];
      outlen--;
      pos++;
    }
  }

  if (pos && outlen >= r-pos) {
    for (i=0;i<(r-pos)/8;i++)
      store64(out+8*i,s[pos/8+i]);
    out += r-pos;
    outlen -= r-pos;
    pos = 0;
  }

  while(outlen >= r) {
    KeccakF1600_StatePermute(s);
    for (i=0;i<r/8;i++)
      store64(out+8*i,s[i]);
    out += r;
    outlen -= r;
  }

  if (!outlen)
    return pos;
  else if (!pos)
    KeccakF1600_StatePermute(s);

  for (i=0;i<outlen/8;i++)
    store64(out+8*i,s[pos/8+i]);
  out += 8*i;
  outlen -= 8*i;
  pos += 8*i;

  store64(t,s[pos/8]);
  for (i=0;i<outlen;i++)
    out[i] = t[i];
  pos += outlen;
  return pos;
}


/*************************************************
* Name:        shake128_init
*
* Description: Initilizes Keccak state for use as SHAKE128 XOF
*
* Arguments:   - Keccak_state *state: pointer to (uninitialized)
*                                     Keccak state
**************************************************/
static
void shake128_init(Keccak_state *state)
{
  keccak_init(state);
}


/*************************************************
* Name:        shake128_absorb
*
* Description: Absorb step of the SHAKE128 XOF; incremental.
*
* Arguments:   - Keccak_state *state: pointer to (initialized) output
*                                     Keccak state
*              - const uint8_t *in:   pointer to input to be absorbed into s
*              - size_t inlen:        length of input in bytes
**************************************************/
static
void shake128_absorb(Keccak_state *state, const uint8_t *in, size_t inlen)
{
  state->pos = keccak_absorb(state->s, SHAKE128_RATE, state->pos, in, inlen);
}


/*************************************************
* Name:        shake128_finalize
*
* Description: Finalize absorb step of the SHAKE128 XOF.
*
* Arguments:   - Keccak_state *state: pointer to Keccak state
**************************************************/
static
void shake128_finalize(Keccak_state *state)
{
  keccak_finalize(state->s, SHAKE128_RATE, state->pos, 0x1F);
  state->pos = 0;
}


/*************************************************
* Name:        shake128_squeezeblocks
*
* Description: Squeeze step of SHAKE128 XOF. Squeezes full blocks of
*              SHAKE128_RATE bytes each. Can be called multiple times
*              to keep squeezing. Assumes zero bytes of current block
*              have already been squeezed (state->pos = 0).
*
* Arguments:   - uint8_t *out:    pointer to output blocks
*              - size_t nblocks:  number of blocks to be squeezed
*                                 (written to output)
*              - Keccak_state *s: pointer to input/output Keccak state
**************************************************/
static
void shake128_squeezeblocks(uint8_t *out, size_t nblocks, Keccak_state *state)
{
  keccak_squeezeblocks(out, nblocks, state->s, SHAKE128_RATE);
}


/*************************************************
* Name:        shake128_squeeze
*
* Description: Squeeze step of SHAKE128 XOF. Squeezes arbitraily many
*              bytes. Can be called multiple times to keep squeezing.
*
* Arguments:   - uint8_t *out:    pointer to output blocks
*              - size_t outlen :  number of bytes to be squeezed
*                                 (written to output)
*              - Keccak_state *s: pointer to input/output Keccak state
**************************************************/
static
void shake128_squeeze(uint8_t *out, size_t outlen, Keccak_state *state)
{
  state->pos = keccak_squeeze(out, outlen, state->s, SHAKE128_RATE, state->pos);
}


/*************************************************
* Name:        shake256_init
*
* Description: Initilizes Keccak state for use as SHAKE256 XOF
*
* Arguments:   - Keccak_state *state: pointer to (uninitialized)
*                                     Keccak state
**************************************************/
static
void shake256_init(Keccak_state *state)
{
  keccak_init(state);
}


/*************************************************
* Name:        shake256_absorb
*
* Description: Absorb step of the SHAKE256 XOF; incremental.
*
* Arguments:   - Keccak_state *state: pointer to (initialized) output
*                                     Keccak state
*              - const uint8_t *in:   pointer to input to be absorbed into s
*              - size_t inlen:        length of input in bytes
**************************************************/
static
void shake256_absorb(Keccak_state *state, const uint8_t *in, size_t inlen)
{
  state->pos = keccak_absorb(state->s, SHAKE256_RATE, state->pos, in, inlen);
}


/*************************************************
* Name:        shake256_finalize
*
* Description: Finalize absorb step of the SHAKE256 XOF.
*
* Arguments:   - Keccak_state *state: pointer to Keccak state
**************************************************/
static
void shake256_finalize(Keccak_state *state)
{
  keccak_finalize(state->s, SHAKE256_RATE, state->pos, 0x1F);
  state->pos = 0;
}


/*************************************************
* Name:        shake256_squeezeblocks
*
* Description: Squeeze step of SHAKE256 XOF. Squeezes full blocks of
*              SHAKE256_RATE bytes each. Can be called multiple times
*              to keep squeezing. Assumes zero bytes of current block
*              have already been squeezed (state->pos = 0).
*
* Arguments:   - uint8_t *out:    pointer to output blocks
*              - size_t nblocks:  number of blocks to be squeezed
*                                 (written to output)
*              - Keccak_state *s: pointer to input/output Keccak state
**************************************************/
static
void shake256_squeezeblocks(uint8_t *out, size_t nblocks, Keccak_state *state)
{
  keccak_squeezeblocks(out, nblocks, state->s, SHAKE256_RATE);
}


/*************************************************
* Name:        shake256_squeeze
*
* Description: Squeeze step of SHAKE256 XOF. Squeezes arbitraily many
*              bytes. Can be called multiple times to keep squeezing.
*
* Arguments:   - uint8_t *out:    pointer to output blocks
*              - size_t outlen :  number of bytes to be squeezed
*                                 (written to output)
*              - Keccak_state *s: pointer to input/output Keccak state
**************************************************/
static
void shake256_squeeze(uint8_t *out, size_t outlen, Keccak_state *state)
{
  state->pos = keccak_squeeze(out, outlen, state->s, SHAKE256_RATE, state->pos);
}


/*************************************************
* Name:        shake128
*
* Description: SHAKE128 XOF with non-incremental API
*
* Arguments:   - uint8_t *out:      pointer to output
*              - size_t outlen:     requested output length in bytes
*              - const uint8_t *in: pointer to input
*              - size_t inlen:      length of input in bytes
**************************************************/
static
void shake128(uint8_t *out, size_t outlen, const uint8_t *in, size_t inlen)
{
  Keccak_state state;

  shake128_init(&state);
  shake128_absorb(&state, in, inlen);
  shake128_finalize(&state);
  shake128_squeeze(out, outlen, &state);
}


/*************************************************
* Name:        shake256
*
* Description: SHAKE256 XOF with non-incremental API
*
* Arguments:   - uint8_t *out:      pointer to output
*              - size_t outlen:     requested output length in bytes
*              - const uint8_t *in: pointer to input
*              - size_t inlen:      length of input in bytes
**************************************************/
static
void shake256(uint8_t *out, size_t outlen, const uint8_t *in, size_t inlen)
{
  Keccak_state state;

  shake256_init(&state);
  shake256_absorb(&state, in, inlen);
  shake256_finalize(&state);
  shake256_squeeze(out, outlen, &state);

  // TODO: clear state
}


#if 0
    // VT: not referenced
/*************************************************
* Name:        sha3_256
*
* Description: SHA3-256 with non-incremental API
*
* Arguments:   - uint8_t *h:        pointer to output (32 bytes)
*              - const uint8_t *in: pointer to input
*              - size_t inlen:      length of input in bytes
**************************************************/
static
void sha3_256(uint8_t h[32], const uint8_t *in, size_t inlen)
{
  uint64_t s[25] = {0};
  unsigned int pos;

  pos = keccak_absorb(s, SHA3_256_RATE, 0, in, inlen);
  keccak_finalize(s, SHA3_256_RATE, pos, 0x06);
  keccak_squeeze(h, 32, s, SHA3_256_RATE, 0);
}


/*************************************************
* Name:        sha3_512
*
* Description: SHA3-512 with non-incremental API
*
* Arguments:   - uint8_t *h:        pointer to output (64 bytes)
*              - const uint8_t *in: pointer to input
*              - size_t inlen:      length of input in bytes
**************************************************/
static
void sha3_512(uint8_t h[64], const uint8_t *in, size_t inlen)
{
  uint64_t s[25] = {0};
  unsigned int pos;

  pos = keccak_absorb(s, SHA3_512_RATE, 0, in, inlen);
  keccak_finalize(s, SHA3_512_RATE, pos, 0x06);
  keccak_squeeze(h, 64, s, SHA3_512_RATE, 0);
}
#endif
#endif         /*-----  /delimiter: Keccak  --------------------------------*/


#if 1       /*-----  delimiter: Dilithium core  ----------------------------*/
#if 1       /*-----  delimiter: reduce  ------------------------------------*/
#define  DIL_D       14
#define  DIL_Q       8380417
#define  DIL_MONT    4193792U       /* 2^32 % DIL_Q */
#define  DIL_QINV    4236238847U    /* -q^(-1) mod 2^32 */
#define  DIL_GAMMA1  ((DIL_Q - 1)/16)
#define  DIL_GAMMA2  (DIL_GAMMA1 /2)
#define  DIL_ALPHA   (2* DIL_GAMMA2)

#define  DIL_SEEDBYTES  ((unsigned int) 256/8)
#define  DIL_CRHBYTES   ((unsigned int) 384/8)

#define  DIL_POLYT0_PACKEDBYTES  ((unsigned int) 448)
#define  DIL_POLYT1_PACKEDBYTES  ((unsigned int) 288)
#define  DIL_POLYW1_PACKEDBYTES  ((unsigned int) 128)
#define  DIL_POLYZ_PACKEDBYTES   ((unsigned int) 640)

/* K/L-dependent values for (5,4), (6,5), (8,7) */
/**/
#define  DIL_PUB_BYTES(k)  (DIL_SEEDBYTES + (k)* DIL_POLYT1_PACKEDBYTES)
/* #define  DIL_PUB4x3_BYTES  DIL_PUB_BYTES(4) */
#define  DIL_PUB5x4_BYTES  DIL_PUB_BYTES(5)
#define  DIL_PUB6x5_BYTES  DIL_PUB_BYTES(6)
#define  DIL_PUB8x7_BYTES  DIL_PUB_BYTES(8)
/*
 * raw bytecounts, excl. any ASN.1/BER framing
 * hardwired since formula contains conditionals; do not expect
 * compiler constant expansion
 */
#define  DIL_PRV5x4_BYTES  ((size_t) 3504)
#define  DIL_PRV6x5_BYTES  ((size_t) 3856)
#define  DIL_PRV8x7_BYTES  ((size_t) 5136)
/* see also dil_prv_wirebytes() */

/* ETA <= 3 decides */
#define  DIL_POLYETA5x4_PACKEDBYTES  ((size_t) 128)
#define  DIL_POLYETA6x5_PACKEDBYTES  ((size_t)  96)
#define  DIL_POLYETA8x7_PACKEDBYTES  ((size_t)  96)

#define  DIL_VECT_MAX   ((unsigned int) 8)   /* MAX(K, L) for any config */


#if 1   /*-----  delimiter: SHA-3 PRF  -------------------------------------*/
/* SHA-3 PRF only: */
#define  DIL_STREAM128_BLOCKBYTES  SHAKE128_RATE
#define  DIL_STREAM256_BLOCKBYTES  SHAKE256_RATE
typedef  Keccak_state              stream128_state ;
typedef  Keccak_state              stream256_state ;
/**/
#define stream128_init(STATE, SEED, NONCE) \
        shake128_stream_init(STATE, SEED, NONCE)

#endif  /*-----  /delimiter: SHA-3 PRF  ------------------------------------*/

/* /header constants from params.h */


/*************************************************
* Name:        montgomery_reduce
*
* Description: For finite field element a with 0 <= a <= Q*2^32,
*              compute r \equiv a*2^{-32} (mod Q) such that 0 <= r < 2*Q.
*
* Arguments:   - uint64_t: finite field element a
*
* Returns r.
**************************************************/
static
uint32_t montgomery_reduce(uint64_t a) {
    uint64_t t;

    t =  a * DIL_QINV;
    t &= ((uint64_t)1 << 32) - 1;
    t *= DIL_Q;
    t += a;

    t >>= 32;

    return t;
}


/*************************************************
* Name:        reduce32
*
* Description: For finite field element a, compute r \equiv a (mod Q)
*              such that 0 <= r < 2*Q.
*
* Arguments:   - uint32_t: finite field element a
*
* Returns r.
**************************************************/
static
uint32_t reduce32(uint32_t a) {
    uint32_t t;

    t = a & 0x7FFFFF;
    a >>= 23;
    t += (a << 13) - a;

    return t;
}


/*************************************************
* Name:        csubq
*
* Description: Subtract Q if input coefficient is bigger than Q.
*
* Arguments:   - uint32_t: finite field element a
*
* Returns r.
**************************************************/
static
uint32_t csubq(uint32_t a) {
    a -= DIL_Q;

    a += ((int32_t)a >> 31) & DIL_Q;

    return a;
}


/*************************************************
* Name:        freeze
*
* Description: For finite field element a, compute standard
*              representative r = a mod Q.
*
* Arguments:   - uint32_t: finite field element a
*
* Returns r.
**************************************************/
static
uint32_t freeze(uint32_t a) {
    a = reduce32(a);
    a = csubq(a);
    return a;
}

#endif      /*-----  /delimiter: reduce  -----------------------------------*/


#if 1       /*-----  delimiter: NTT  ---------------------------------------*/
#define  DIL_N  ((unsigned int) 256)

/* Roots of unity in order needed by forward ntt */
static const uint32_t dil_zetas[DIL_N] = {0, 25847, 5771523, 7861508, 237124,
7602457, 7504169, 466468, 1826347, 2353451, 8021166, 6288512, 3119733,
5495562, 3111497, 2680103, 2725464, 1024112, 7300517, 3585928, 7830929,
7260833, 2619752, 6271868, 6262231, 4520680, 6980856, 5102745, 1757237,
8360995, 4010497, 280005, 2706023, 95776, 3077325, 3530437, 6718724,
4788269, 5842901, 3915439, 4519302, 5336701, 3574422, 5512770, 3539968,
8079950, 2348700, 7841118, 6681150, 6736599, 3505694, 4558682, 3507263,
6239768, 6779997, 3699596, 811944, 531354, 954230, 3881043, 3900724,
5823537, 2071892, 5582638, 4450022, 6851714, 4702672, 5339162, 6927966,
3475950, 2176455, 6795196, 7122806, 1939314, 4296819, 7380215, 5190273,
5223087, 4747489, 126922, 3412210, 7396998, 2147896, 2715295, 5412772,
4686924, 7969390, 5903370, 7709315, 7151892, 8357436, 7072248, 7998430,
1349076, 1852771, 6949987, 5037034, 264944, 508951, 3097992, 44288,
7280319, 904516, 3958618, 4656075, 8371839, 1653064, 5130689, 2389356,
8169440, 759969, 7063561, 189548, 4827145, 3159746, 6529015, 5971092,
8202977, 1315589, 1341330, 1285669, 6795489, 7567685, 6940675, 5361315,
4499357, 4751448, 3839961, 2091667, 3407706, 2316500, 3817976, 5037939,
2244091, 5933984, 4817955, 266997, 2434439, 7144689, 3513181, 4860065,
4621053, 7183191, 5187039, 900702, 1859098, 909542, 819034, 495491,
6767243, 8337157, 7857917, 7725090, 5257975, 2031748, 3207046, 4823422,
7855319, 7611795, 4784579, 342297, 286988, 5942594, 4108315, 3437287,
5038140, 1735879, 203044, 2842341, 2691481, 5790267, 1265009, 4055324,
1247620, 2486353, 1595974, 4613401, 1250494, 2635921, 4832145, 5386378,
1869119, 1903435, 7329447, 7047359, 1237275, 5062207, 6950192, 7929317,
1312455, 3306115, 6417775, 7100756, 1917081, 5834105, 7005614, 1500165,
777191, 2235880, 3406031, 7838005, 5548557, 6709241, 6533464, 5796124,
4656147, 594136, 4603424, 6366809, 2432395, 2454455, 8215696, 1957272,
3369112, 185531, 7173032, 5196991, 162844, 1616392, 3014001, 810149,
1652634, 4686184, 6581310, 5341501, 3523897, 3866901, 269760, 2213111,
7404533, 1717735, 472078, 7953734, 1723600, 6577327, 1910376, 6712985,
7276084, 8119771, 4546524, 5441381, 6144432, 7959518, 6094090, 183443,
7403526, 1612842, 4834730, 7826001, 3919660, 8332111, 7018208, 3937738,
1400424, 7534263, 1976782};

/* Roots of unity in order needed by inverse ntt */
static const uint32_t dil_zetas_inv[DIL_N] = {6403635, 846154, 6979993, 4442679,
1362209, 48306, 4460757, 554416, 3545687, 6767575, 976891, 8196974,
2286327, 420899, 2235985, 2939036, 3833893, 260646, 1104333, 1667432,
6470041, 1803090, 6656817, 426683, 7908339, 6662682, 975884, 6167306,
8110657, 4513516, 4856520, 3038916, 1799107, 3694233, 6727783, 7570268,
5366416, 6764025, 8217573, 3183426, 1207385, 8194886, 5011305, 6423145,
164721, 5925962, 5948022, 2013608, 3776993, 7786281, 3724270, 2584293,
1846953, 1671176, 2831860, 542412, 4974386, 6144537, 7603226, 6880252,
1374803, 2546312, 6463336, 1279661, 1962642, 5074302, 7067962, 451100,
1430225, 3318210, 7143142, 1333058, 1050970, 6476982, 6511298, 2994039,
3548272, 5744496, 7129923, 3767016, 6784443, 5894064, 7132797, 4325093,
7115408, 2590150, 5688936, 5538076, 8177373, 6644538, 3342277, 4943130,
4272102, 2437823, 8093429, 8038120, 3595838, 768622, 525098, 3556995,
5173371, 6348669, 3122442, 655327, 522500, 43260, 1613174, 7884926,
7561383, 7470875, 6521319, 7479715, 3193378, 1197226, 3759364, 3520352,
4867236, 1235728, 5945978, 8113420, 3562462, 2446433, 6136326, 3342478,
4562441, 6063917, 4972711, 6288750, 4540456, 3628969, 3881060, 3019102,
1439742, 812732, 1584928, 7094748, 7039087, 7064828, 177440, 2409325,
1851402, 5220671, 3553272, 8190869, 1316856, 7620448, 210977, 5991061,
3249728, 6727353, 8578, 3724342, 4421799, 7475901, 1100098, 8336129,
5282425, 7871466, 8115473, 3343383, 1430430, 6527646, 7031341, 381987,
1308169, 22981, 1228525, 671102, 2477047, 411027, 3693493, 2967645,
5665122, 6232521, 983419, 4968207, 8253495, 3632928, 3157330, 3190144,
1000202, 4083598, 6441103, 1257611, 1585221, 6203962, 4904467, 1452451,
3041255, 3677745, 1528703, 3930395, 2797779, 6308525, 2556880, 4479693,
4499374, 7426187, 7849063, 7568473, 4680821, 1600420, 2140649, 4873154,
3821735, 4874723, 1643818, 1699267, 539299, 6031717, 300467, 4840449,
2867647, 4805995, 3043716, 3861115, 4464978, 2537516, 3592148, 1661693,
4849980, 5303092, 8284641, 5674394, 8100412, 4369920, 19422, 6623180,
3277672, 1399561, 3859737, 2118186, 2108549, 5760665, 1119584, 549488,
4794489, 1079900, 7356305, 5654953, 5700314, 5268920, 2884855, 5260684,
2091905, 359251, 6026966, 6554070, 7913949, 876248, 777960, 8143293,
518909, 2608894, 8354570};


/*************************************************
* Name:        ntt
*
* Description: Forward NTT, in-place. No modular reduction is performed after
*              additions or subtractions. If input coefficients are below 2*Q,
*              then output coefficients are below 18*Q.
*              Output vector is in bitreversed order.
*
* Arguments:   - uint32_t p[N]: input/output coefficient array
**************************************************/
static
void ntt256(uint32_t p[ DIL_N ]) {
    unsigned int len, start, j, k;
    uint32_t zeta, t;

    k = 1;
    for (len = 128; len > 0; len >>= 1) {
        for (start = 0; start < DIL_N; start = j + len) {
            zeta = dil_zetas[k++];

            for (j = start; j < start + len; ++j) {
                t = montgomery_reduce((uint64_t)zeta *
                                      p[j + len]);
                p[j + len] = p[j] + 2*DIL_Q - t;
                p[j] = p[j] + t;
            }
        }
    }
}


/*************************************************
* Name:        invntt_tomont
*
* Description: Inverse NTT and multiplication by Montgomery factor 2^32.
*              In-place. No modular reductions after additions or
*              subtractions. Input coefficient need to be smaller than 2*Q.
*              Output coefficient are smaller than 2*Q.
*
* Arguments:   - uint32_t p[N]: input/output coefficient array
**************************************************/
static
void invntt_tomont256(uint32_t p[ DIL_N ]) {
    unsigned int start, len, j, k;
    uint32_t t, zeta;
    const uint32_t f = (((uint64_t)DIL_MONT*DIL_MONT % DIL_Q) *
                        (DIL_Q-1) % DIL_Q) *
                        ((DIL_Q-1) >> 8) % DIL_Q;

    k = 0;
    for (len = 1; len < DIL_N; len <<= 1) {
        for (start = 0; start < DIL_N; start = j + len) {
            zeta = dil_zetas_inv[k++];
            for (j = start; j < start + len; ++j) {
                t = p[j];

                p[j]       = t + p[j + len];
                p[j + len] = t + 256*DIL_Q - p[j + len];
                p[j + len] = montgomery_reduce((uint64_t)zeta *
                             p[j + len]);
            }
        }
    }

    for (j = 0; j < DIL_N; ++j) {
        p[j] = montgomery_reduce((uint64_t)f * p[j]);
    }
}

#endif      /*-----  /delimiter: NTT  --------------------------------------*/


#if 1       /*-----  delimiter: rounding  ----------------------------------*/
/*************************************************
* Name:        power2round
*
* Description: For finite field element a, compute a0, a1 such that
*              a mod Q = a1*2^D + a0 with -2^{D-1} < a0 <= 2^{D-1}.
*              Assumes a to be standard representative.
*
* Arguments:   - uint32_t a: input element
*              - uint32_t *a0: pointer to output element Q + a0
*
* Returns a1.
**************************************************/
static
uint32_t power2round(uint32_t a, uint32_t *a0)  {
    int32_t t;

    /* Centralized remainder mod 2^D */
    t =  a & ((1U << DIL_D) - 1);
    t -= (1U << (DIL_D-1)) + 1;
    t += (t >> 31) & (1U << DIL_D);
    t -= (1U << (DIL_D-1)) - 1;

    *a0 = DIL_Q + t;
    a = (a - t) >> DIL_D;

    return a;
}


/*************************************************
* Name:        decompose
*
* Description: For finite field element a, compute high and low bits a0, a1 such
*              that a mod Q = a1*ALPHA + a0 with -ALPHA/2 < a0 <= ALPHA/2 except
*              if a1 = (Q-1)/ALPHA where we set a1 = 0 and
*              -ALPHA/2 <= a0 = a mod Q - Q < 0. Assumes a to be standard
*              representative.
*
* Arguments:   - uint32_t a: input element
*              - uint32_t *a0: pointer to output element Q + a0
*
* Returns a1.
**************************************************/
static
uint32_t decompose(uint32_t a, uint32_t *a0) {
#if DIL_ALPHA != (DIL_Q-1)/16
#error "decompose assumes ALPHA == (Q-1)/16"
#endif
    int32_t t, u;

    /* Centralized remainder mod ALPHA */
    t = a & 0x7FFFF;
    t += (a >> 19) << 9;
    t -= DIL_ALPHA/2 + 1;
    t += (t >> 31) & DIL_ALPHA;
    t -= DIL_ALPHA/2 - 1;
    a -= t;

    /* Divide by ALPHA (possible to avoid) */
    u = a - 1;
    u >>= 31;
    a = (a >> 19) + 1;
    a -= u & 1;

    /* Border case */
    *a0 = DIL_Q + t - (a >> 4);
    a &= 0xF;
    return a;
}


/*************************************************
* Name:        make_hint
*
* Description: Compute hint bit indicating whether the low bits of the
*              input element overflow into the high bits. Inputs assumed to be
*              standard representatives.
*
* Arguments:   - uint32_t a0: low bits of input element
*              - uint32_t a1: high bits of input element
*
* Returns 1 if high bits of a and b differ and 0 otherwise.
**************************************************/
static
unsigned int make_hint(uint32_t a0, uint32_t a1) {
    if ((a0 <= DIL_GAMMA2) || (a0 > DIL_Q - DIL_GAMMA2) ||
        ((a0 == DIL_Q - DIL_GAMMA2) && !a1))
        return 0;

    return 1;
}


/*************************************************
* Name:        use_hint
*
* Description: Correct high bits according to hint.
*
* Arguments:   - uint32_t a: input element
*              - unsigned int hint: hint bit
*
* Returns corrected high bits.
**************************************************/
static
uint32_t use_hint(uint32_t a, unsigned int hint) {
    uint32_t a0, a1;

    a1 = decompose(a, &a0);
    if (!hint) {
        return a1;

    } else if (a0 > DIL_Q) {
        return (a1 + 1) & 0xF;

    } else {
        return (a1 - 1) & 0xF;
    }

/* note: dead code in ref.impl? */
#if 0
    /* If decompose does not divide out ALPHA:
    if (!hint) {
        return a1;
    } else if (a0 > DIL_Q)
        return (a1 + DIL_ALPHA) % (DIL_Q - 1);
    else
        return (a1 - DIL_ALPHA) % (DIL_Q - 1);
    */
#endif
}

#endif      /*-----  /delimiter: rounding  ---------------------------------*/


#if 1       /*-----  delimiter: poly -> symmetric.h  -----------------------*/
static
void shake128_stream_init(Keccak_state *state,
                         const uint8_t seed[ DIL_SEEDBYTES ],
                              uint16_t nonce)
{
  uint8_t t[2];

  t[0] = nonce;
  t[1] = nonce >> 8;

  shake128_init(state);
  shake128_absorb(state, seed, DIL_SEEDBYTES);
  shake128_absorb(state, t, 2);
  shake128_finalize(state);
}


static
void shake256_stream_init(Keccak_state *state,
                         const uint8_t seed[ DIL_CRHBYTES ],
                              uint16_t nonce)
{
  uint8_t t[2];

  t[0] = nonce;
  t[1] = nonce >> 8;

  shake256_init(state);
  shake256_absorb(state, seed, DIL_CRHBYTES);
  shake256_absorb(state, t, 2);
  shake256_finalize(state);
}

#define stream128_squeezeblocks(OUT, OUTBLOCKS, STATE) \
        shake128_squeezeblocks(OUT, OUTBLOCKS, STATE)

#define stream256_init(STATE, SEED, NONCE) \
        shake256_stream_init(STATE, SEED, NONCE)

#define stream256_squeezeblocks(OUT, OUTBLOCKS, STATE) \
        shake256_squeezeblocks(OUT, OUTBLOCKS, STATE)


//--------------------------------------
static inline size_t dil_crh(unsigned char *res,  size_t rbytes,
                       const unsigned char *seed, size_t sbytes)
{
    shake256(res, rbytes, seed, sbytes);

    return rbytes;
}

#endif      /*-----  /delimiter: poly -> symmetric.h  ----------------------*/


#if 1       /*-----  delimiter: poly  --------------------------------------*/
typedef struct {
    uint32_t coeffs[ DIL_N ];
} poly;


/*************************************************
* Name:        poly_reduce
*
* Description: Inplace reduction of all coefficients of polynomial to
*              representative in [0,2*Q[.
*
* Arguments:   - poly *a: pointer to input/output polynomial
**************************************************/
static
void poly_reduce(poly *a) {
    unsigned int i;

    for (i = 0; i < DIL_N; ++i)
        a->coeffs[i] = reduce32(a->coeffs[i]);
}


/*************************************************
* Name:        poly_csubq
*
* Description: For all coefficients of in/out polynomial subtract Q if
*              coefficient is bigger than Q.
*
* Arguments:   - poly *a: pointer to input/output polynomial
**************************************************/
static
void poly_csubq(poly *a) {
    unsigned int i;

    for (i = 0; i < DIL_N; ++i)
        a->coeffs[i] = csubq(a->coeffs[i]);
}


/*************************************************
* Name:        poly_freeze
*
* Description: Inplace reduction of all coefficients of polynomial to
*              standard representatives.
*
* Arguments:   - poly *a: pointer to input/output polynomial
**************************************************/
static
void poly_freeze(poly *a) {
    unsigned int i;

    for (i = 0; i < DIL_N; ++i)
        a->coeffs[i] = freeze(a->coeffs[i]);
}


/*************************************************
* Name:        poly_add
*
* Description: Add polynomials. No modular reduction is performed.
*
* Arguments:   - poly *c: pointer to output polynomial
*              - const poly *a: pointer to first summand
*              - const poly *b: pointer to second summand
**************************************************/
static
void poly_add(poly *c, const poly *a, const poly *b)  {
    unsigned int i;

    for (i = 0; i < DIL_N; ++i)
        c->coeffs[i] = a->coeffs[i] + b->coeffs[i];
}


/*************************************************
* Name:        poly_sub
*
* Description: Subtract polynomials. Assumes coefficients of second input
*              polynomial to be less than 2*Q. No modular reduction is
*              performed.
*
* Arguments:   - poly *c: pointer to output polynomial
*              - const poly *a: pointer to first input polynomial
*              - const poly *b: pointer to second input polynomial to be
*                               subtraced from first input polynomial
**************************************************/
static
void poly_sub(poly *c, const poly *a, const poly *b) {
    unsigned int i;

    for (i = 0; i < DIL_N; ++i)
        c->coeffs[i] = a->coeffs[i] + 2*DIL_Q - b->coeffs[i];
}


/*************************************************
* Name:        poly_shiftl
*
* Description: Multiply polynomial by 2^D without modular reduction. Assumes
*              input coefficients to be less than 2^{32-D}.
*
* Arguments:   - poly *a: pointer to input/output polynomial
**************************************************/
static
void poly_shiftl(poly *a) {
    unsigned int i;

    for (i = 0; i < DIL_N; ++i)
        a->coeffs[i] <<= DIL_D;
}


/*************************************************
* Name:        poly_ntt
*
* Description: Inplace forward NTT. Output coefficients can be up to
*              16*Q larger than input coefficients.
*
* Arguments:   - poly *a: pointer to input/output polynomial
**************************************************/
static
void poly_ntt256(poly *a) {
    ntt256(a->coeffs);
}


/*************************************************
* Name:        poly_invntt_tomont
*
* Description: Inplace inverse NTT and multiplication by 2^{32}.
*              Input coefficients need to be less than 2*Q.
*              Output coefficients are less than 2*Q.
*
* Arguments:   - poly *a: pointer to input/output polynomial
**************************************************/
static
void poly_invntt_tomont(poly *a) {
    invntt_tomont256(a->coeffs);
}


/*************************************************
* Name:        poly_pointwise_montgomery
*
* Description: Pointwise multiplication of polynomials in NTT domain
*              representation and multiplication of resulting polynomial
*              by 2^{-32}. Output coefficients are less than 2*Q if input
*              coefficient are less than 22*Q.
*
* Arguments:   - poly *c: pointer to output polynomial
*              - const poly *a: pointer to first input polynomial
*              - const poly *b: pointer to second input polynomial
**************************************************/
static
void poly_pointwise_montgomery(poly *c, const poly *a, const poly *b) {
    unsigned int i;

    for (i = 0; i < DIL_N; ++i) {
        c->coeffs[i] = montgomery_reduce((uint64_t) a->coeffs[i] *
                                                    b->coeffs[i]);
    }
}


/*************************************************
* Name:        poly_power2round
*
* Description: For all coefficients c of the input polynomial,
*              compute c0, c1 such that c mod Q = c1*2^D + c0
*              with -2^{D-1} < c0 <= 2^{D-1}. Assumes coefficients to be
*              standard representatives.
*
* Arguments:   - poly *a1: pointer to output polynomial with coefficients c1
*              - poly *a0: pointer to output polynomial with coefficients Q + c0
*              - const poly *v: pointer to input polynomial
**************************************************/
static
void poly_power2round(poly *a1, poly *a0, const poly *a) {
  unsigned int i;

  for (i = 0; i < DIL_N; ++i)
    a1->coeffs[i] = power2round(a->coeffs[i], &a0->coeffs[i]);
}

/*************************************************
* Name:        poly_decompose
*
* Description: For all coefficients c of the input polynomial,
*              compute high and low bits c0, c1 such c mod Q = c1*ALPHA + c0
*              with -ALPHA/2 < c0 <= ALPHA/2 except c1 = (Q-1)/ALPHA where we
*              set c1 = 0 and -ALPHA/2 <= c0 = c mod Q - Q < 0.
*              Assumes coefficients to be standard representatives.
*
* Arguments:   - poly *a1: pointer to output polynomial with coefficients c1
*              - poly *a0: pointer to output polynomial with coefficients Q + c0
*              - const poly *c: pointer to input polynomial
**************************************************/
static
void poly_decompose(poly *a1, poly *a0, const poly *a) {
  unsigned int i;

  for (i = 0; i < DIL_N; ++i)
    a1->coeffs[i] = decompose(a->coeffs[i], &a0->coeffs[i]);
}


/*************************************************
* Name:        poly_make_hint
*
* Description: Compute hint polynomial. The coefficients of which indicate
*              whether the low bits of the corresponding coefficient of
*              the input polynomial overflow into the high bits.
*
* Arguments:   - poly *h: pointer to output hint polynomial
*              - const poly *a0: pointer to low part of input polynomial
*              - const poly *a1: pointer to high part of input polynomial
*
* Returns number of 1 bits.
**************************************************/
static
unsigned int poly_make_hint(poly *h, const poly *a0, const poly *a1) {
    unsigned int i, s = 0;

    for (i = 0; i < DIL_N; ++i) {
        h->coeffs[i] = make_hint(a0->coeffs[i], a1->coeffs[i]);
        s += h->coeffs[i];
    }

    return s;
}


/*************************************************
* Name:        poly_use_hint
*
* Description: Use hint polynomial to correct the high bits of a polynomial.
*
* Arguments:   - poly *b: pointer to output polynomial with corrected high bits
*              - const poly *a: pointer to input polynomial
*              - const poly *h: pointer to input hint polynomial
**************************************************/
static
void poly_use_hint(poly *b, const poly *a, const poly *h) {
    unsigned int i;

    for (i = 0; i < DIL_N; ++i)
        b->coeffs[i] = use_hint(a->coeffs[i], h->coeffs[i]);
}


/*************************************************
* Name:        poly_chknorm
*
* Description: Check infinity norm of polynomial against given bound.
*              Assumes input coefficients to be standard representatives.
*
* Arguments:   - const poly *a: pointer to polynomial
*              - uint32_t B: norm bound
*
* Returns 0 if norm is strictly smaller than B and 1 otherwise.
**************************************************/
ATTR_PURE__
static
/**/
int poly_chknorm(const poly *a, uint32_t B) {
    unsigned int i;
    uint32_t t;

    /* It is ok to leak which coefficient violates the bound since
     the probability for each coefficient is independent of secret
     data but we must not leak sign of the centralized representative. */

    for (i = 0; i < DIL_N; ++i) {
        /* Absolute value of centralized representative */

        t =  (DIL_Q-1)/2 - a->coeffs[i];
        t ^= (int32_t)t >> 31;
        t =  (DIL_Q-1)/2 - t;

        if (t >= B) {
            return 1;
        }
    }

    return 0;
}


/*************************************************
* Name:        rej_uniform
*
* Description: Sample uniformly random coefficients in [0, Q-1] by
*              performing rejection sampling using array of random bytes.
*
* Arguments:   - uint32_t *a: pointer to output array (allocated)
*              - unsigned int len: number of coefficients to be sampled
*              - const uint8_t *buf: array of random bytes
*              - unsigned int buflen: length of array of random bytes
*
* Returns number of sampled coefficients. Can be smaller than len if not enough
* random bytes were given.
**************************************************/
static unsigned int rej_uniform(uint32_t *a,
                                unsigned int len,
                                const uint8_t *buf,
                                unsigned int buflen)
{
    unsigned int ctr = 0, pos = 0;
    uint32_t t;

    while ((ctr < len) && (pos + 3 <= buflen)) {
        t  = buf[pos++];

        t |= (uint32_t)buf[pos++] << 8;
        t |= (uint32_t)buf[pos++] << 16;

        t &= 0x7FFFFF;

        if (t < DIL_Q)
            a[ctr++] = t;
    }

    return ctr;
}

/*************************************************
* Name:        poly_uniform
*
* Description: Sample polynomial with uniformly random coefficients
*              in [0,Q-1] by performing rejection sampling using the
*              output stream of SHAKE256(seed|nonce) or AES256CTR(seed,nonce).
*
* Arguments:   - poly *a: pointer to output polynomial
*              - const uint8_t seed[]: byte array with seed of length SEEDBYTES
*              - uint16_t nonce: 2-byte nonce
**************************************************/
#define POLY_UNIFORM_NBLOCKS \
       ((768 +DIL_STREAM128_BLOCKBYTES -1) / DIL_STREAM128_BLOCKBYTES)

static
void poly_uniform(poly *a,
                  const uint8_t seed[ DIL_SEEDBYTES ],
                  uint16_t nonce)
{
    unsigned int i, ctr, off;
    unsigned int buflen = POLY_UNIFORM_NBLOCKS * DIL_STREAM128_BLOCKBYTES;
    uint8_t buf[ POLY_UNIFORM_NBLOCKS * DIL_STREAM128_BLOCKBYTES +2 ];
    stream128_state state;

    stream128_init(&state, seed, nonce);
    stream128_squeezeblocks(buf, POLY_UNIFORM_NBLOCKS, &state);

    ctr = rej_uniform(a->coeffs, DIL_N, buf, buflen);

    while(ctr < DIL_N) {
        off = buflen % 3;
        for (i = 0; i < off; ++i)
            buf[i] = buf[buflen - off + i];

        buflen = DIL_STREAM128_BLOCKBYTES + off;

        stream128_squeezeblocks(buf + off, 1, &state);

        ctr += rej_uniform(a->coeffs + ctr, DIL_N - ctr, buf, buflen);
    }
}


/*************************************************
* Name:        rej_eta
*
* Description: Sample uniformly random coefficients in [-ETA, ETA] by
*              performing rejection sampling using array of random bytes.
*
* Arguments:   - uint32_t *a: pointer to output array (allocated)
*              - unsigned int len: number of coefficients to be sampled
*              - const uint8_t *buf: array of random bytes
*              - unsigned int buflen: length of array of random bytes
*
* Returns number of sampled coefficients. Can be smaller than len if not enough
* random bytes were given.
**************************************************/
static unsigned int rej_eta(uint32_t *a,
                            unsigned int len,
                            const uint8_t *buf,
                            unsigned int buflen,
                            unsigned int eta)
{
    unsigned int ctr = 0, pos = 0;
    uint32_t t0, t1;

    while ((ctr < len) && (pos < buflen)) {
        if (eta <= 3) {
            t0 = buf[pos  ] &  0x07;
            t1 = buf[pos++] >> 5;
        } else {
            t0 = buf[pos  ] &  0x0F;
            t1 = buf[pos++] >> 4;
        }

        if (t0 <= 2*eta)
            a[ctr++] = DIL_Q + eta - t0;

        if ((t1 <= 2*eta) && (ctr < len))
            a[ctr++] = DIL_Q + eta - t1;
    }

    return ctr;
}


/*************************************************
* Name:        poly_uniform_eta
*
* Description: Sample polynomial with uniformly random coefficients
*              in [-ETA,ETA] by performing rejection sampling using the
*              output stream from SHAKE256(seed|nonce) or AES256CTR(seed,nonce).
*
* Arguments:   - poly *a: pointer to output polynomial
*              - const uint8_t seed[]: byte array with seed of length SEEDBYTES
*              - uint16_t nonce: 2-byte nonce
**************************************************/
#define POLY_UNIFORM_ETA_NBLOCKS ((192 + DIL_STREAM128_BLOCKBYTES - 1) \
                                  /DIL_STREAM128_BLOCKBYTES)

static
void poly_uniform_eta(poly *a,
                      const uint8_t seed[ DIL_SEEDBYTES ],
                       unsigned int eta,
                      uint16_t nonce)
{
    unsigned int ctr;
    unsigned int buflen = POLY_UNIFORM_ETA_NBLOCKS * DIL_STREAM128_BLOCKBYTES;
    uint8_t buf[ POLY_UNIFORM_ETA_NBLOCKS * DIL_STREAM128_BLOCKBYTES ];
    stream128_state state;

    stream128_init(&state, seed, nonce);
    stream128_squeezeblocks(buf, POLY_UNIFORM_ETA_NBLOCKS, &state);

    ctr = rej_eta(a->coeffs, DIL_N, buf, buflen, eta);

    while (ctr < DIL_N) {
        stream128_squeezeblocks(buf, 1, &state);

        ctr += rej_eta(a->coeffs + ctr, DIL_N - ctr, buf,
                       DIL_STREAM128_BLOCKBYTES, eta);
    }
}

/*************************************************
* Name:        rej_gamma1m1
*
* Description: Sample uniformly random coefficients
*              in [-(GAMMA1 - 1), GAMMA1 - 1] by performing rejection sampling
*              using array of random bytes.
*
* Arguments:   - uint32_t *a: pointer to output array (allocated)
*              - unsigned int len: number of coefficients to be sampled
*              - const uint8_t *buf: array of random bytes
*              - unsigned int buflen: length of array of random bytes
*
* Returns number of sampled coefficients. Can be smaller than len if not enough
* random bytes were given.
**************************************************/
static unsigned int rej_gamma1m1(uint32_t *a,
                                 unsigned int len,
                                 const uint8_t *buf,
                                 unsigned int buflen)
{
#if DIL_GAMMA1 > (1 << 19)
#error "rej_gamma1m1() assumes GAMMA1 - 1 fits in 19 bits"
#endif
    unsigned int ctr, pos;
    uint32_t t0, t1;

    ctr = pos = 0;

    while(ctr < len && pos + 5 <= buflen) {
        t0  = buf[pos];
        t0 |= (uint32_t)buf[pos + 1] << 8;
        t0 |= (uint32_t)buf[pos + 2] << 16;
        t0 &= 0xFFFFF;

        t1  = buf[pos + 2] >> 4;
        t1 |= (uint32_t)buf[pos + 3] << 4;
        t1 |= (uint32_t)buf[pos + 4] << 12;

        pos += 5;

        if (t0 <= 2*DIL_GAMMA1 - 2)
            a[ctr++] = DIL_Q + DIL_GAMMA1 - 1 - t0;

        if (t1 <= 2*DIL_GAMMA1 - 2 && ctr < len)
            a[ctr++] = DIL_Q + DIL_GAMMA1 - 1 - t1;
    }

    return ctr;
}


/*************************************************
* Name:        poly_uniform_gamma1m1
*
* Description: Sample polynomial with uniformly random coefficients
*              in [-(GAMMA1 - 1), GAMMA1 - 1] by performing rejection
*              sampling on output stream of SHAKE256(seed|nonce)
*              or AES256CTR(seed,nonce).
*
* Arguments:   - poly *a: pointer to output polynomial
*              - const uint8_t seed[]: byte array with seed of length CRHBYTES
*              - uint16_t nonce: 16-bit nonce
**************************************************/
#define POLY_UNIFORM_GAMMA1M1_NBLOCKS  \
       ((640 + DIL_STREAM256_BLOCKBYTES - 1) / DIL_STREAM256_BLOCKBYTES)

static
void poly_uniform_gamma1m1(poly *a,
                           const uint8_t seed[ DIL_CRHBYTES ],
                           uint16_t nonce)
{
    unsigned int i, ctr, off;
    unsigned int buflen = POLY_UNIFORM_GAMMA1M1_NBLOCKS *
                                                DIL_STREAM256_BLOCKBYTES;
    uint8_t buf[POLY_UNIFORM_GAMMA1M1_NBLOCKS * DIL_STREAM256_BLOCKBYTES + 4];
    stream256_state state;

    stream256_init(&state, seed, nonce);
    stream256_squeezeblocks(buf, POLY_UNIFORM_GAMMA1M1_NBLOCKS, &state);

    ctr = rej_gamma1m1(a->coeffs, DIL_N, buf, buflen);

    while(ctr < DIL_N) {
        off = buflen % 5;
        for (i = 0; i < off; ++i)
            buf[i] = buf[buflen - off + i];

        buflen = DIL_STREAM256_BLOCKBYTES + off;

        stream256_squeezeblocks(buf + off, 1, &state);

        ctr += rej_gamma1m1(a->coeffs + ctr, DIL_N - ctr, buf, buflen);
    }
}


/*************************************************
* Description: Bit-pack polynomial with coefficients in [-ETA,ETA].
*              Input coefficients are assumed to lie in [Q-ETA,Q+ETA].
* Arguments:   - uint8_t *r: pointer to output byte array with at least
*                            POLETA_SIZE_PACKED bytes
*              - const poly *a: pointer to input polynomial
**************************************************/
static
void polyeta_pack(uint8_t *r, const poly *a, unsigned int eta) {
    unsigned int i;
    uint8_t t[8];

    if (2*eta <= 7) {
        for (i = 0; i < DIL_N/8; ++i) {
            t[0] = DIL_Q + eta - a->coeffs[8*i+0];
            t[1] = DIL_Q + eta - a->coeffs[8*i+1];
            t[2] = DIL_Q + eta - a->coeffs[8*i+2];
            t[3] = DIL_Q + eta - a->coeffs[8*i+3];
            t[4] = DIL_Q + eta - a->coeffs[8*i+4];
            t[5] = DIL_Q + eta - a->coeffs[8*i+5];
            t[6] = DIL_Q + eta - a->coeffs[8*i+6];
            t[7] = DIL_Q + eta - a->coeffs[8*i+7];

            r[3*i+0]  = (t[0] >> 0) | (t[1] << 3) | (t[2] << 6);
            r[3*i+1]  = (t[2] >> 2) | (t[3] << 1) |
                        (t[4] << 4) | (t[5] << 7);
            r[3*i+2]  = (t[5] >> 1) | (t[6] << 2) | (t[7] << 5);
        }

    } else {
        for (i = 0; i < DIL_N/2; ++i) {
            t[0] = DIL_Q + eta - a->coeffs[2*i+0];
            t[1] = DIL_Q + eta - a->coeffs[2*i+1];
            r[i] = t[0] | (t[1] << 4);
        }
    }
}


/*************************************************
* Name:        polyeta_unpack
*
* Description: Unpack polynomial with coefficients in [-ETA,ETA].
*              Output coefficients lie in [Q-ETA,Q+ETA].
*
* Arguments:   - poly *r: pointer to output polynomial
*              - const uint8_t *a: byte array with bit-packed polynomial
**************************************************/
static
void polyeta_unpack(poly *r, const uint8_t *a, unsigned int eta) {
  unsigned int i;

  if (eta <= 3) {
    for (i = 0; i < DIL_N/8; ++i) {
      r->coeffs[8*i+0] =  a[3*i+0] & 0x07;
      r->coeffs[8*i+1] = (a[3*i+0] >> 3) & 0x07;
      r->coeffs[8*i+2] = ((a[3*i+0] >> 6) | (a[3*i+1] << 2)) & 0x07;
      r->coeffs[8*i+3] = (a[3*i+1] >> 1) & 0x07;
      r->coeffs[8*i+4] = (a[3*i+1] >> 4) & 0x07;
      r->coeffs[8*i+5] = ((a[3*i+1] >> 7) | (a[3*i+2] << 1)) & 0x07;
      r->coeffs[8*i+6] = (a[3*i+2] >> 2) & 0x07;
      r->coeffs[8*i+7] = (a[3*i+2] >> 5) & 0x07;

      r->coeffs[8*i+0] = DIL_Q + eta - r->coeffs[8*i+0];
      r->coeffs[8*i+1] = DIL_Q + eta - r->coeffs[8*i+1];
      r->coeffs[8*i+2] = DIL_Q + eta - r->coeffs[8*i+2];
      r->coeffs[8*i+3] = DIL_Q + eta - r->coeffs[8*i+3];
      r->coeffs[8*i+4] = DIL_Q + eta - r->coeffs[8*i+4];
      r->coeffs[8*i+5] = DIL_Q + eta - r->coeffs[8*i+5];
      r->coeffs[8*i+6] = DIL_Q + eta - r->coeffs[8*i+6];
      r->coeffs[8*i+7] = DIL_Q + eta - r->coeffs[8*i+7];
    }

  } else {
    for (i = 0; i < DIL_N/2; ++i) {
      r->coeffs[2*i+0] = a[i] & 0x0F;
      r->coeffs[2*i+1] = a[i] >> 4;

      r->coeffs[2*i+0] = DIL_Q + eta - r->coeffs[2*i+0];
      r->coeffs[2*i+1] = DIL_Q + eta - r->coeffs[2*i+1];
    }
  }
}


/*************************************************
* Name:        polyt1_pack
*
* Description: Bit-pack polynomial t1 with coefficients fitting in 9 bits.
*              Input coefficients are assumed to be standard representatives.
*
* Arguments:   - uint8_t *r: pointer to output byte array with at least
*                            POLT1_SIZE_PACKED bytes
*              - const poly *a: pointer to input polynomial
**************************************************/
static
void polyt1_pack(uint8_t *r, const poly *a) {
    unsigned int i;

    for (i = 0; i < DIL_N/8; ++i) {
        r[9*i+0] = (a->coeffs[8*i+0] >> 0);
        r[9*i+1] = (a->coeffs[8*i+0] >> 8) | (a->coeffs[8*i+1] << 1);
        r[9*i+2] = (a->coeffs[8*i+1] >> 7) | (a->coeffs[8*i+2] << 2);
        r[9*i+3] = (a->coeffs[8*i+2] >> 6) | (a->coeffs[8*i+3] << 3);
        r[9*i+4] = (a->coeffs[8*i+3] >> 5) | (a->coeffs[8*i+4] << 4);
        r[9*i+5] = (a->coeffs[8*i+4] >> 4) | (a->coeffs[8*i+5] << 5);
        r[9*i+6] = (a->coeffs[8*i+5] >> 3) | (a->coeffs[8*i+6] << 6);
        r[9*i+7] = (a->coeffs[8*i+6] >> 2) | (a->coeffs[8*i+7] << 7);
        r[9*i+8] = (a->coeffs[8*i+7] >> 1);
    }
}

/*************************************************
* Name:        polyt1_unpack
*
* Description: Unpack polynomial t1 with 9-bit coefficients.
*              Output coefficients are standard representatives.
*
* Arguments:   - poly *r: pointer to output polynomial
*              - const uint8_t *a: byte array with bit-packed polynomial
**************************************************/
static
void polyt1_unpack(poly *r, const uint8_t *a) {
  unsigned int i;

  for (i = 0; i < DIL_N/8; ++i) {
    r->coeffs[8*i+0] = ((a[9*i+0] >> 0) | ((uint32_t)a[9*i+1] << 8)) & 0x1FF;
    r->coeffs[8*i+1] = ((a[9*i+1] >> 1) | ((uint32_t)a[9*i+2] << 7)) & 0x1FF;
    r->coeffs[8*i+2] = ((a[9*i+2] >> 2) | ((uint32_t)a[9*i+3] << 6)) & 0x1FF;
    r->coeffs[8*i+3] = ((a[9*i+3] >> 3) | ((uint32_t)a[9*i+4] << 5)) & 0x1FF;
    r->coeffs[8*i+4] = ((a[9*i+4] >> 4) | ((uint32_t)a[9*i+5] << 4)) & 0x1FF;
    r->coeffs[8*i+5] = ((a[9*i+5] >> 5) | ((uint32_t)a[9*i+6] << 3)) & 0x1FF;
    r->coeffs[8*i+6] = ((a[9*i+6] >> 6) | ((uint32_t)a[9*i+7] << 2)) & 0x1FF;
    r->coeffs[8*i+7] = ((a[9*i+7] >> 7) | ((uint32_t)a[9*i+8] << 1)) & 0x1FF;
  }
}

/*************************************************
* Name:        polyt0_pack
*
* Description: Bit-pack polynomial t0 with coefficients in ]-2^{D-1}, 2^{D-1}].
*              Input coefficients are assumed to lie in ]Q-2^{D-1}, Q+2^{D-1}].
*
* Arguments:   - uint8_t *r: pointer to output byte array with at least
*                            POLT0_SIZE_PACKED bytes
*              - const poly *a: pointer to input polynomial
**************************************************/
static
void polyt0_pack(uint8_t *r, const poly *a) {
#if DIL_D != 14
#error "polyt0_pack() assumes D == 14"
#endif
    unsigned int i;
    uint32_t t[4];

    for (i = 0; i < DIL_N/4; ++i) {
        t[0] = DIL_Q + (1U << (DIL_D -1)) - a->coeffs[4*i+0];
        t[1] = DIL_Q + (1U << (DIL_D -1)) - a->coeffs[4*i+1];
        t[2] = DIL_Q + (1U << (DIL_D -1)) - a->coeffs[4*i+2];
        t[3] = DIL_Q + (1U << (DIL_D -1)) - a->coeffs[4*i+3];

        r[7*i+0]  =  t[0];
        r[7*i+1]  =  t[0] >> 8;
        r[7*i+1] |=  t[1] << 6;
        r[7*i+2]  =  t[1] >> 2;
        r[7*i+3]  =  t[1] >> 10;
        r[7*i+3] |=  t[2] << 4;
        r[7*i+4]  =  t[2] >> 4;
        r[7*i+5]  =  t[2] >> 12;
        r[7*i+5] |=  t[3] << 2;
        r[7*i+6]  =  t[3] >> 6;
    }
}


/*************************************************
* Name:        polyt0_unpack
*
* Description: Unpack polynomial t0 with coefficients in ]-2^{D-1}, 2^{D-1}].
*              Output coefficients lie in ]Q-2^{D-1},Q+2^{D-1}].
*
* Arguments:   - poly *r: pointer to output polynomial
*              - const uint8_t *a: byte array with bit-packed polynomial
**************************************************/
static
void polyt0_unpack(poly *r, const uint8_t *a) {
  unsigned int i;

  for (i = 0; i < DIL_N/4; ++i) {
    r->coeffs[4*i+0]  = a[7*i+0];
    r->coeffs[4*i+0] |= (uint32_t)a[7*i+1] << 8;
    r->coeffs[4*i+0] &= 0x3FFF;

    r->coeffs[4*i+1]  = a[7*i+1] >> 6;
    r->coeffs[4*i+1] |= (uint32_t)a[7*i+2] << 2;
    r->coeffs[4*i+1] |= (uint32_t)a[7*i+3] << 10;
    r->coeffs[4*i+1] &= 0x3FFF;

    r->coeffs[4*i+2]  = a[7*i+3] >> 4;
    r->coeffs[4*i+2] |= (uint32_t)a[7*i+4] << 4;
    r->coeffs[4*i+2] |= (uint32_t)a[7*i+5] << 12;
    r->coeffs[4*i+2] &= 0x3FFF;

    r->coeffs[4*i+3]  = a[7*i+5] >> 2;
    r->coeffs[4*i+3] |= (uint32_t)a[7*i+6] << 6;

    r->coeffs[4*i+0] = DIL_Q + (1U << (DIL_D-1)) - r->coeffs[4*i+0];
    r->coeffs[4*i+1] = DIL_Q + (1U << (DIL_D-1)) - r->coeffs[4*i+1];
    r->coeffs[4*i+2] = DIL_Q + (1U << (DIL_D-1)) - r->coeffs[4*i+2];
    r->coeffs[4*i+3] = DIL_Q + (1U << (DIL_D-1)) - r->coeffs[4*i+3];
  }
}


/*************************************************
* Name:        polyz_pack
*
* Description: Bit-pack polynomial z with coefficients
*              in [-(GAMMA1 - 1), GAMMA1 - 1].
*              Input coefficients are assumed to be standard representatives.
*
* Arguments:   - uint8_t *r: pointer to output byte array with at least
*                            POLZ_SIZE_PACKED bytes
*              - const poly *a: pointer to input polynomial
**************************************************/
static
void polyz_pack(uint8_t *r, const poly *a) {
#if DIL_GAMMA1 > (1 << 19)
#error "polyz_pack() assumes GAMMA1 - 1 fits in 19 bits"
#endif
    unsigned int i;
    uint32_t t[2];

    for (i = 0; i < DIL_N/2; ++i) {
        /* Map to {0,...,2*GAMMA1 - 2} */
        t[0] =  DIL_GAMMA1 - 1 - a->coeffs[2*i+0];
        t[0] += ((int32_t)t[0] >> 31) & DIL_Q;

        t[1] =  DIL_GAMMA1 - 1 - a->coeffs[2*i+1];
        t[1] += ((int32_t)t[1] >> 31) & DIL_Q;

        r[5*i+0]  = t[0];
        r[5*i+1]  = t[0] >> 8;
        r[5*i+2]  = t[0] >> 16;
        r[5*i+2] |= t[1] << 4;
        r[5*i+3]  = t[1] >> 4;
        r[5*i+4]  = t[1] >> 12;
    }
}


/*************************************************
* Name:        polyz_unpack
*
* Description: Unpack polynomial z with coefficients
*              in [-(GAMMA1 - 1), GAMMA1 - 1].
*              Output coefficients are standard representatives.
*
* Arguments:   - poly *r: pointer to output polynomial
*              - const uint8_t *a: byte array with bit-packed polynomial
**************************************************/
static
void polyz_unpack(poly *r, const unsigned char *a) {
    unsigned int i;

    for (i = 0; i < DIL_N/2; ++i) {
        r->coeffs[2*i+0]  = a[5*i+0];
        r->coeffs[2*i+0] |= (uint32_t)a[5*i+1] << 8;
        r->coeffs[2*i+0] |= (uint32_t)a[5*i+2] << 16;
        r->coeffs[2*i+0] &= 0xFFFFF;

        r->coeffs[2*i+1]  = a[5*i+2] >> 4;
        r->coeffs[2*i+1] |= (uint32_t)a[5*i+3] << 4;
        r->coeffs[2*i+1] |= (uint32_t)a[5*i+4] << 12;

        r->coeffs[2*i+0] =  DIL_GAMMA1 - 1 - r->coeffs[2*i+0];
        r->coeffs[2*i+0] += ((int32_t)r->coeffs[2*i+0] >> 31) & DIL_Q;
        r->coeffs[2*i+1] =  DIL_GAMMA1 - 1 - r->coeffs[2*i+1];
        r->coeffs[2*i+1] += ((int32_t)r->coeffs[2*i+1] >> 31) & DIL_Q;
    }
}


/*************************************************
* Name:        polyw1_pack
*
* Description: Bit-pack polynomial w1 with coefficients in [0, 15].
*              Input coefficients are assumed to be standard representatives.
*
* Arguments:   - uint8_t *r: pointer to output byte array with at least
*                            POLW1_SIZE_PACKED bytes
*              - const poly *a: pointer to input polynomial
**************************************************/
static
void polyw1_pack(uint8_t *r, const poly *a) {
    unsigned int i;

    for (i = 0; i < DIL_N/2; ++i)
        r[i] = a->coeffs[2*i+0] | (a->coeffs[2*i+1] << 4);
}
#endif      /*-----  /delimiter: poly  -------------------------------------*/


#if 1       /*-----  delimiter: polyvec, some of packing.c  ----------------*/
/* the largest polyvecl, polyveck possible
 * safe to cast to any valid, smaller size
 */
typedef struct {
    poly vec[ DIL_VECT_MAX ];
} polyvec_max ;


//--------------------------------------
// 0 if K is not a valid predefined category
static unsigned int dil_eta(unsigned int k)
{
    switch (k) {
//  case 4: return 6;
    case 5: return 5;
    case 6: return 3;        /* Category IV */
    case 8: return 2;        /* Category V? */

    default:
        return 0;
    }
}


//--------------------------------------
// raw bytecount, excluding ASN framing
static size_t dil_prv_wirebytes(unsigned int k)
{
    return DIL_SEEDBYTES *2                            +
           DIL_CRHBYTES                                +
           (k + k -1) * ((dil_eta(k) <= 3) ? 96 : 128) +   /* (K+L)* ... */
           k * DIL_POLYT0_PACKEDBYTES ;
}


//--------------------------------------
// raw bytecount, excluding ASN framing
static size_t dil_pub_wirebytes(unsigned int k)
{
    switch (k) {
//  case 4: return DIL_PUB4x3_BYTES;
    case 5: return DIL_PUB5x4_BYTES;
    case 6: return DIL_PUB6x5_BYTES;
    case 8: return DIL_PUB8x7_BYTES;

    default:
        return 0;
    }
}


//--------------------------------------
static unsigned dil_omega(unsigned int k)
{
    switch (k) {
//  case 4: return 80;
    case 5: return 96;
    case 6: return 120;
    case 8: return 140;

    default:
        return 0;
    }
}


//--------------------------------------
// TODO: named consts
//
static unsigned dil_beta(unsigned int k)
{
    switch (k) {
//  case 4: return 325;
    case 5: return 275;
    case 6: return 175;
    case 8: return 120;

    default:
        return 0;
    }
}


//--------------------------------------
// raw bytecount, excluding ASN framing
// returns 0  if parameter choice is not supported
//
// ref.impl:
// CRYPTO_BYTES == (L*POLYZ_PACKEDBYTES + OMEGA + K + N/8 + 8)
// restricted to L==K-1
//
static size_t dil_signature_bytes(unsigned int k)
{
    switch (k) {
//  case 4: return 2044;
    case 5: return 2701;      /* double-check; do we pad to 2n? */
    case 6: return 3366;
    case 8: return 4668;

    default:
        return 0;
    }
}


/*--------------------------------------
 * OID stubs for Dilithium
 *
 * highly regular:
 *     1.3.6.1.4.1.2.267 .X .Y.Z  ->  060b 2b0601040102820b <X> <Y> <Z>
 *
 * all valid X/Y/Z are single-byte, so sizeof(OID) == sizeof(...stub) +3
 */
static const unsigned char dil_oidstub[] = {
    0x06,0x0b,                                     // OID{
         0x2b,0x06,0x01,0x04,0x01, 0x02,0x82,0x0b, //     ...}
} ;

#define DIL_OIDTAIL_BYTES  ((size_t) 3)


/*--------------------------------------
 * returns  >0  if Dilithium variant is a recognized one, see PQCA_ID_t
 *           0  not a recognized OID
 *
 * OIDs are highly regular, so we just verify these stubs
 * see also dil_oid2wire(), the inverse
 */
static unsigned int dil_oid2type(const unsigned char *oid, size_t obytes)
{
    unsigned int rc = 0;

    if (oid && (obytes == sizeof(dil_oidstub) +DIL_OIDTAIL_BYTES) &&
        !memcmp(oid, dil_oidstub, obytes -DIL_OIDTAIL_BYTES))
    {
        switch (MSBF4_READ(oid +obytes -4) & 0xffffff) {
                        /* (.1) Dilithium round2 */
        case 0x010504: rc = PQCA_ID_DIL3_R2;     break;
        case 0x010605: rc = PQCA_ID_DIL4_R2;     break;
        case 0x010807: rc = PQCA_ID_DIL5_R2;     break;

                /* (.6) Dilithium round2, 'raw' key objects */
        case 0x060504: rc = PQCA_ID_DIL3_R2_RAW; break;
        case 0x060605: rc = PQCA_ID_DIL4_R2_RAW; break;
        case 0x060807: rc = PQCA_ID_DIL5_R2_RAW; break;

            /* (.7) Dilithium round3, compressed signatures */
        case 0x070504: rc = PQCA_ID_DIL3_R3;     break;
        case 0x070605: rc = PQCA_ID_DIL4_R3;     break;
        case 0x070807: rc = PQCA_ID_DIL5_R3;     break;

        default:
            break;
        }
    }

    return rc;
}


/*--------------------------------------
 * returns written bytecount
 *         0  if type is unknown
 *        ~0  if output buffer is insufficient
 */
static size_t dil_oid2wire(unsigned char *wire, size_t wbytes,
                            unsigned int type)
{
    uint32_t tail = 0;
    size_t wr;

    switch (type) {
        case PQCA_ID_DIL3_R2:     tail = 0x010504; break;
        case PQCA_ID_DIL4_R2:     tail = 0x010605; break;
        case PQCA_ID_DIL5_R2:     tail = 0x010807; break;
        case PQCA_ID_DIL3_R2_RAW: tail = 0x060504; break;
        case PQCA_ID_DIL4_R2_RAW: tail = 0x060605; break;
        case PQCA_ID_DIL5_R2_RAW: tail = 0x060807; break;
        case PQCA_ID_DIL3_R3:     tail = 0x070504; break;
        case PQCA_ID_DIL4_R3:     tail = 0x070605; break;
        case PQCA_ID_DIL5_R3:     tail = 0x070807; break;
        default:
            break;
    }

    if (!tail)
        return 0;

    wr = sizeof(dil_oidstub) + DIL_OIDTAIL_BYTES;
    if (wire && (wr > wbytes))
        return ~((size_t) 0);              // insufficient output

    if (wire) {
        memcpy(wire, dil_oidstub, sizeof(dil_oidstub));

        wire[ wr -3 ] = (unsigned char) (tail >> 16);
        wire[ wr -2 ] = (unsigned char) (tail >>  8);
        wire[ wr -1 ] = (unsigned char)  tail;
    }

    return wr;
}


#if 1       /*-----  delimiter: ASN.1/BER  ---------------------------------*/
/* all encoding is DER
 * we rely on DER as input, since only minimal framing is supported
 *
 * public keys:
 *   [3]   0 1794: SEQUENCE {                               -- 30 82 xx yy
 *   [1]   4   15:   SEQUENCE {                             -- 30 nn
 *   [1]   6   11:     OBJECT IDENTIFIER '...2 267 1 6 5'   -- 06 ...
 *   [4]  19    0:     NULL                                 -- 05 00
 *               :     }
 *   [3]  21 1773:   BIT STRING, encapsulates {             -- 03 82 xx yy 00
 *   [3]  26 1768:     SEQUENCE {                           -- 30 82 xx yy
 *   [2]  30   33:       BIT STRING                         -- 03 21 00 ...
 *               :         59 3D BC 08 17 DB 66 9F 67 0D 61 1F DD 27 A5 46
 *               :         88 D7 A3 B9 0D C7 82 CA F8 8A 8E 2F 1A 2A 5E 0B
 *   [3]  65 1729:       BIT STRING                         -- 30 82 xx yy 00
 *               :         53 C7 B4 9C 2B 7B BB 92 F7 A4 EC 36 88 0B F1 EE
 *               :         C7 7E C1 15 BB D8 F3 F8 30 DE CA 04 BD C4 A7 D8 ...
 *
 * exploiting the following specialized information:
 *   [1]  OID sizes are fixed; tags are single-byte
 *   [2]  nonce/bit string sizes are fixed; tags are single byte
 *   [3]  public keys' T1 vector sizes, their BIT STRING etc.
 *        encapsulation and any integrating tag uses/needs 2-byte
 *        Length sizes (82 xx yy)
 *   [4]  NULL parameters are supplied and ignored [legacy compatibility]
 */
#define  DIL__ASN_NULL_BYTES  2
#define  DIL_SPKI_ADDL_BYTES                          \
         4 /*SEQ*/ +2 /*SEQ*/ +DIL__ASN_NULL_BYTES  + \
         5 /*BIT*/ +4 /*SEQ*/ +3 /*BIT*/  + 5 /*BIT*/
//
// excludes OID; OIDs include their Tag+Len fields, not separately
// framed, therefore missing from above

    /* Tag+Len+fixed Value byte for SEEDBYTES-sized BIT STRINGs */
#define DIL__ASN1_BITSTR_SEED    0x032100
#define DIL__ASN1_BITSTR_SEED_B  ((size_t) 3)     /* sizeof(..._BITSTR_SEED) */

#define DIL__ASN1_SEQUENCE   0x30
#define DIL__ASN1_BITSTRING  0x03
#define DIL__ASN1_NULL       0x05

//--------------------------------------
// writes BIT STRING frame to end of (wire, wbytes)
// returns written bytecount
//
// opportunistic; assume result fits; restricted to 82 xx yy or single-byte Len
//
static size_t dil_asn_bitstr(unsigned char *wire, size_t wbytes,
                                    size_t bstring_net_bytes)
{
    if (bstring_net_bytes < 0x80) {          // 03 ...len... 00
        if (wire && (wbytes >= 3)) {
            wire[ wbytes-3 ] = DIL__ASN1_BITSTRING;
            wire[ wbytes-2 ] = (unsigned char) bstring_net_bytes +1;
            wire[ wbytes-1 ] = 0;
        }
        return 3;

    } else {                                 // assume  03 82 xx yy 00
        if (wire && (wbytes >= 5)) {
            wire[ wbytes-5 ] = DIL__ASN1_BITSTRING;
            wire[ wbytes-4 ] = 0x82;

            wire[ wbytes-3 ] =
                (unsigned char) ((bstring_net_bytes +1) >>8);
            wire[ wbytes-2 ] =
                (unsigned char)   bstring_net_bytes +1;

            wire[ wbytes-1 ] = 0;
        }
        return 5;
    }
}


//--------------------------------------
static size_t dil_asn_sequence(unsigned char *wire, size_t wbytes,
                                      size_t seq_net_bytes)
{
    if (seq_net_bytes < 0x80) {              // 30 ...len... 00
        if (wire && (wbytes >= 2)) {
            wire[ wbytes-2 ] = DIL__ASN1_SEQUENCE;
            wire[ wbytes-1 ] = (unsigned char) seq_net_bytes;
        }
        return 2;

    } else {                                 // assume  30 82 xx yy
        if (wire && (wbytes >= 4)) {
            wire[ wbytes-4 ] = DIL__ASN1_SEQUENCE;
            wire[ wbytes-3 ] = 0x82;

            wire[ wbytes-2 ] =
                (unsigned char) (seq_net_bytes >>8);
            wire[ wbytes-1 ] =
                (unsigned char)  seq_net_bytes;
        }
        return 4;
    }
}


//--------------------------------------
static size_t dil_asn_null(unsigned char *wire, size_t wbytes)
{
    if (wire && (wbytes >= DIL__ASN_NULL_BYTES)) {
        wire[ wbytes-2 ] = DIL__ASN1_NULL;
        wire[ wbytes-1 ] = 0x00;
    }

    return DIL__ASN_NULL_BYTES;
}


//--------------------------------------
static size_t dil_pub2wire(unsigned char *wire,  size_t wbytes,
                     const unsigned char *pub,   size_t pbytes,
                     const unsigned char *algid, size_t ibytes)
{
    size_t wr = 0, offs = 0, oidb;
    unsigned int type;

// TODO: deduplicate
    switch (pub ? pbytes : 0) {
    case DIL_PUB5x4_BYTES: type = PQCA_ID_DIL3_R2; break;
    case DIL_PUB6x5_BYTES: type = PQCA_ID_DIL4_R2; break;
    case DIL_PUB8x7_BYTES: type = PQCA_ID_DIL5_R2; break;
    default:
        return 0;
    }
    if (!type)
        return 0;

    (void) algid;
    (void) ibytes;

    wr = DIL_SPKI_ADDL_BYTES +sizeof(dil_oidstub) +DIL_OIDTAIL_BYTES +
         pbytes;

    if (!wire)
        return wr;
    if (wr > wbytes)
        return ~0;

                /* concatenate fields back to front */

    pbytes -= DIL_SEEDBYTES;
    offs   =  wr -pbytes;

        // seed is (pub, DIL_SEEDBYTES)
        // raw T1 is rest

    memmove(&( wire[ offs ] ), &(pub[ DIL_SEEDBYTES ]), pbytes);

    offs -= dil_asn_bitstr(wire, offs, pbytes);
                    // BIT STRING { t1 }

            // TODO: offset sanity checks

    offs -= DIL_SEEDBYTES;

    memmove(&( wire[ offs ] ), pub, DIL_SEEDBYTES);
    offs -= dil_asn_bitstr(wire, offs, DIL_SEEDBYTES);
                    // BIT STRING { seed }

    offs -= dil_asn_sequence(wire, offs, wr -offs);
                    // SEQ { BIT { seed } BIT { t1 } }

    offs -= dil_asn_bitstr(wire, offs, wr -offs);
                // BIT { SEQ { BIT { seed } BIT { t1 } } }

    offs -= dil_asn_null(wire, offs);

                // TODO: write OID to end; saves indirection

    oidb = dil_oid2wire(NULL, ~0, type);

    if (oidb <= offs)
        oidb = dil_oid2wire(&( wire[ offs -oidb ] ), oidb, type);

    // TODO: ...other sanity-checks...

    offs -= oidb;

    offs -= dil_asn_sequence(wire, offs, oidb +DIL__ASN_NULL_BYTES);
                // SEQ { OID { ... } NULL }

    offs -= dil_asn_sequence(wire, offs, wr -offs);

    return wr;
}


//--------------------------------------
typedef enum {
    DIL_SPKI_SEEDFRAME  = 1,  // seed[32] in its own BIT STRING frame
    DIL_SPKI_T1FRAME    = 2,  // T1[var-len] in its own BIT STRING frame
    DIL_SPKI_FULLFRAME  = 4   // seed || T1 in a single BIT STRING
} DIL_SPKIflags_t ;


/*--------------------------------------
 * DER SPKI frames
 * identify enough
 *   1) check size
 *   2) check for OID in known position
 *   3) pick up type, nonce[32] and T1[size-dependent] offsets if valid
 */
static const struct {       /* field names "...b" all abbreviate "...bytes" */
    size_t totalb;
    const char *descr;

    unsigned int type;

    size_t oidoffs;    /* full 06(...) frame incl. Tag+Len */
    size_t oidb;

    size_t seedoffs;   /* raw value, w/o any ASN frame excl. 00 start
                          byte of BIT STRING; see also flags */
    size_t seedb;

    size_t t1offs;    /* raw T1 value, w/o any ASN frame, see also flags */
    size_t t1b;

    unsigned int flags;           /* see DIL_SPKIflags_t */
} dil_spkis_der[] = {
                        /* round2, standard SPKIs */

    { 1510, "Dil, round2 std. SPKI, Cat. III [Dil-5-4]",
      PQCA_ID_DIL3_R2,
      6, 13, /*OID*/ 33, 32, /*DIL_SEEDBYTES*/ 70, 1440,
      DIL_SPKI_SEEDFRAME | DIL_SPKI_T1FRAME,
    },
    { 1798, "Dil, round2 std. SPKI, Cat. IV [Dil-6-5]",
      PQCA_ID_DIL4_R2,
      6, 13, /*OID*/ 33, 32, /*DIL_SEEDBYTES*/ 70, 1728,
      DIL_SPKI_SEEDFRAME | DIL_SPKI_T1FRAME,
    },
    { 2374, "Dil, round2 std. SPKI, Cat. V [Dil-8-7]",
      PQCA_ID_DIL5_R2,
      6, 13, /*OID*/ 33, 32, /*DIL_SEEDBYTES*/ 70, 2304,
      DIL_SPKI_SEEDFRAME | DIL_SPKI_T1FRAME,
    },

} ;


/*--------------------------------------
 * skeleton, identifying OID through minimal template
 * returns >0  if successful: nr. of bytes written to start of (pub, pbytes)
 *          0  if framing is not recognized
 *
 * sets non-NULL 'type' to recognized PQCA_ID_t constant, if known
 *
 * TODO: proper DER deframe
 */
static size_t dil_wire2pub(unsigned char *pub,   size_t pbytes,
                           unsigned int  *type,
                     const unsigned char *wire,  size_t wbytes,
                     const unsigned char *algid, size_t ibytes)
{
    size_t seedoffs = 0, seedb = 0, t1offs = 0, t1b = 0;
                        // any valid offset is >0
    unsigned int idx = 0, i;
    size_t wr = 0;

    if (type)
        *type = 0;

    if (!wire || !wbytes)
        return 0;

    for (i=0; i<ARRAY_ELEMS(dil_spkis_der); ++i) {
        unsigned int otype;

        if (dil_spkis_der[i].totalb != wbytes)
            continue;

        //-----  retrieve <seed || T1> into (pub, pbytes)  -----------

            // OID retrieval+verification is unconditional

        otype = dil_oid2type(wire +dil_spkis_der[i].oidoffs,
                                   dil_spkis_der[i].oidb);
            //
        if (!otype || (otype != dil_spkis_der[i].type))
            continue;


            // TODO: check seed offs >= _SEED_B for all entries

                /* is this a seed-sized BIT STRING Tag+Len? */
        if (DIL_SPKI_SEEDFRAME & dil_spkis_der[i].flags) {
            uint32_t v =
                (wire[ dil_spkis_der[i].seedoffs -3 ] << 16) +
                (wire[ dil_spkis_der[i].seedoffs -2 ] <<  8) +
                 wire[ dil_spkis_der[i].seedoffs -1 ] ;
                            // i.e., MSBF3_READ()

            if (v != DIL__ASN1_BITSTR_SEED)
                continue;

            seedoffs = dil_spkis_der[i].seedoffs;
            seedb    = dil_spkis_der[i].seedb;
        }

            /* is this "03 82 xx yy 00" (xxyy == sizeof(T1)+1)? */

        if (DIL_SPKI_T1FRAME & dil_spkis_der[i].flags) {
            uint32_t v =
                MSBF4_READ(wire + dil_spkis_der[i].t1offs -5);

                // TODO: const
            if (((((uint16_t) DIL__ASN1_BITSTRING) <<8) +0x82) !=
                (v >> 16))
                continue;

            /* BIT STRING { 00 ...T1... }, so +1 byte */

            if ((uint16_t) v != dil_spkis_der[i].t1b +1)
                continue;

                /* verify 00 (no unused BIT STRING bits) */

            if (wire[ dil_spkis_der[i].t1offs -1 ] != 0)
                continue;

                /* BIT STRING { .... } covers rest of data */

            t1offs = dil_spkis_der[i].t1offs;
            t1b    = dil_spkis_der[i].t1b;
        }

        if (seedoffs && t1offs) {
            idx = i;
            break;
        }
    }

            /* fields are in the same order
             * writing seed, then T1 with memmove works in-place
             */
    if (seedoffs && seedb && t1offs && t1b) {
        wr = seedb +t1b;

        if (type)
            *type = dil_spkis_der[ idx ].type;

        if (pub && (pbytes >= wr)) {
            memmove(pub,       wire +seedoffs, seedb);
            memmove(pub +seedb, wire +t1offs,  t1b);
        }
    }

    (void) algid;
    (void) ibytes;

    return wr;
}
#endif      /*-----  /delimiter: ASN.1/BER  --------------------------------*/


/*--------------------------------------
 * does not check 'type' validity; call only after verification
 *
 * expect this 'function' to be cheap, no need to cache etc.
 */
static unsigned int dil_type2k(unsigned int type)
{
// TODO: build-assert checks

    return (type & 0xff);
}


/*--------------------------------------
 * does not check 'type' validity; call only after verification
 */
static unsigned int dil_type2eta(unsigned int type)
{
    switch (type) {
    case PQCA_ID_DIL3_R2:
    case PQCA_ID_DIL3_R3:
    case PQCA_ID_DIL3_R2_RAW:
        return 5;

    case PQCA_ID_DIL4_R2:
    case PQCA_ID_DIL4_R3:
    case PQCA_ID_DIL4_R2_RAW:
        return 3;

    case PQCA_ID_DIL5_R2:
    case PQCA_ID_DIL5_R3:
    case PQCA_ID_DIL5_R2_RAW:
        return 2;

    default:
        return 0;
    }
}


//--------------------------------------
#include "polyvec-include.h"            /* size-specialized fn set */


/*************************************************
* Bit-pack signature sig = (z, h, c).
* Arguments:   - uint8_t sig[]: output byte array
*              - const polyvecl *z: pointer to vector z
*              - const polyveck *h: pointer to hint vector h
*              - const poly *c: pointer to challenge polynomial
*
* accesses only necessary number of elements of z[] and h[]
**************************************************/
size_t dil_sig2wire(unsigned char *sig, size_t sbytes,
                const polyvec_max *z,
                const polyvec_max *h,
                       const poly *c,
                     unsigned int dil_k)
{
    unsigned int i, j, k = 0, omega = dil_omega(dil_k);
    size_t sb = dil_signature_bytes(dil_k);
    uint64_t signs, mask;

    if (!sb || !omega)
        return 0;
    if (sbytes < sb)
        return 0;
    memset(sig, 0, sb);

    for (i = 0; i < (dil_k -1); ++i)                          /* ... < L */
        polyz_pack(sig + i * DIL_POLYZ_PACKEDBYTES, &( z->vec[i] ));

    sig += (dil_k -1) * DIL_POLYZ_PACKEDBYTES;                /* L * ... */

                        /* Encode h */
    for (i = 0; i < dil_k; ++i) {
        for (j = 0; j < DIL_N; ++j) {
            if(h->vec[i].coeffs[j] != 0)
                sig[k++] = j;
        }

        sig[ omega + i ] = k;
    }

#if 0
    while (k < omega)
        sig[k++] = 0;       /* was cleared at start */
#endif

    sig += omega + dil_k;

                             /* Encode c */
    signs = 0;
    mask  = 1;

    for (i = 0; i < DIL_N/8; ++i) {
        sig[i] = 0;

        for (j = 0; j < 8; ++j) {
            if (c->coeffs[ 8*i+j ] != 0) {
                sig[i] |= (1U << j);

                if (c->coeffs[ 8*i+j ] == (DIL_Q - 1))
                    signs |= mask;

                mask <<= 1;
            }
        }
    }

    sig += DIL_N/8;

    for (i = 0; i < 8; ++i)
        sig[i] = signs >> 8*i;                     // TODO: LSBF8_WRITE

    return sb;
}


/*------------------------------------*/
/*************************************************
* Unpack signature sig = (z, h, c).
* Arguments:   - polyvecl *z: pointer to output vector z
*              - polyveck *h: pointer to output hint vector h
*              - poly *c: pointer to output challenge polynomial
*              - const uint8_t sig[]: byte array containing
*                bit-packed signature
*
* Returns >0 in case of malformed signature; otherwise 0.
*
* accesses only necessary number of elements of z[] and h[],
* any of the polyvec<...> vectors MAY be used with proper dil_k
**************************************************/
int dil_wire2sig(polyvec_max *z,
                 polyvec_max *h,
                        poly *c,
                unsigned int dil_k,
         const unsigned char *sig, size_t sbytes)
{
    unsigned int i, j, k, omega = dil_omega(dil_k);
    size_t sb = dil_signature_bytes(dil_k);
    uint64_t signs;

    if (sb != sbytes)
        return 1;

    for (i = 0; i < dil_k-1; ++i)                                    /* L */
        polyz_unpack(&( z->vec[i] ), sig + i* DIL_POLYZ_PACKEDBYTES);

    sig += (dil_k -1) * DIL_POLYZ_PACKEDBYTES;                /* L * ... */

  /* Decode h */
    k = 0;

    for (i = 0; i < dil_k; ++i) {
        for (j = 0; j < DIL_N; ++j)
            h->vec[i].coeffs[j] = 0;

        if ((sig[ omega +i ] < k) || (sig[ omega +i ] > omega))
            return 1;

        for (j = k; j < sig[ omega +i ]; ++j) {
            /* Coefficients are ordered for strong unforgeability */

            if ((j > k) && (sig[j] <= sig[ j-1 ]))
                return 1;

            h->vec[i].coeffs[ sig[j] ] = 1;
        }

        k = sig[ omega +i ];
    }

            /* Extra indices are zero for strong unforgeability */
    for (j = k; j < omega; ++j) {
        if (sig[j])
            return 1;
    }

    sig += omega + dil_k;

                            /* Decode c */
    for (i = 0; i < DIL_N; ++i)
        c->coeffs[i] = 0;

    signs = 0;
    for (i = 0; i < 8; ++i)
        signs |= ((uint64_t) sig[ DIL_N/8 +i ]) << 8*i;

        // TODO: LSBF8_READ

        /* Extra sign bits are zero for strong unforgeability */

    if (signs >> 60)
        return 1;

    for (i = 0; i < DIL_N/8; ++i) {
        for (j = 0; j < 8; ++j) {
            if ((sig[i] >> j) & 0x01) {
                c->coeffs[ 8*i+j ] =  1;
                c->coeffs[ 8*i+j ] ^= -(signs & 1) &
                                       (1 ^ (DIL_Q -1));

                signs >>= 1;
            }
        }
    }

    return 0;
}

#endif      /*-----  /delimiter: polyvec, some of packing.c  ---------------*/


#if 1       /*-----  delimiter: sign/verify  -------------------------------*/

typedef struct {
    unsigned char seed[ 2*DIL_SEEDBYTES + 3*DIL_CRHBYTES ];
    polyvec_max s1, s2;

    polyvec_max t0, t1;

    polyvec_max w0, w1;

    polyvec_max h, y, z;

    poly c, chat;

    Keccak_state state;
} DilState ;


/*-------------------------------------------------
* Description: Implementation of H. Samples polynomial with 60 nonzero
*              coefficients in {-1,1} using the output stream of
*              SHAKE256(mu|w1).
*
* Arguments:   - poly *c: pointer to output polynomial
*              - const uint8_t mu[]: byte array containing mu
*              - const polyveck *w1: pointer to vector w1
**************************************************/
static
void dil_challenge(poly *c,
           const uint8_t mu[ DIL_CRHBYTES ],
       const polyvec_max *w1,
            unsigned int k)
{
    unsigned char buf[ DIL_CRHBYTES +
                       DIL_VECT_MAX * DIL_POLYW1_PACKEDBYTES ];
    unsigned int i, b, pos;
    uint64_t signs = 0;
    Keccak_state state;

    memmove(buf, mu, DIL_CRHBYTES);

    for (i = 0; i < k; ++i) {
        polyw1_pack(buf + DIL_CRHBYTES + i * DIL_POLYW1_PACKEDBYTES,
                    &( w1->vec[i] ));
    }

    shake256_init(&state);
    shake256_absorb(&state, buf, DIL_CRHBYTES + k *DIL_POLYW1_PACKEDBYTES);
    shake256_finalize(&state);
    shake256_squeezeblocks(buf, 1, &state);

    signs = 0;
    for (i = 0; i < 8; ++i)
        signs |= (uint64_t)buf[i] << 8*i;           // TODO: LSBF8_READ

    pos = 8;

    for (i = 0; i < DIL_N; ++i)
        c->coeffs[i] = 0;

    for (i = 196; i < 256; ++i) {
        do {
            if(pos >= SHAKE256_RATE) {
                shake256_squeezeblocks(buf, 1, &state);
                pos = 0;
            }

            b = buf[pos++];
        } while (b > i);

        c->coeffs[i] = c->coeffs[b];
        c->coeffs[b] = 1;
        c->coeffs[b] ^= -((uint32_t) signs & 1) & (1 ^ (DIL_Q-1));
        signs >>= 1;
    }

    // TODO: wipe
}


/*************************************************
* Description: Computes signature.
* Arguments:   - uint8_t *sig:   pointer to output signature (of length CRYPTO_BYTES)
*              - size_t siglen:  max.available/written signature (in/output)
*              - uint8_t *m:     pointer to message to be signed
*              - size_t mlen:    length of message
*              - uint8_t *sk:    pointer to bit-packed secret key
* Returns 0 (success)
* ...TODO: properly mapped ret.codes...
**************************************************/
static
size_t ref_sign(unsigned char *sig, size_t siglen,
                const uint8_t *m,   size_t mlen,
                const uint8_t *sk,  size_t skbytes,
                 unsigned int type)
{
    unsigned int i, n = 0, K = 0, beta;
    uint8_t seedbuf[ 2*DIL_SEEDBYTES + 3*DIL_CRHBYTES ];
    uint8_t *rho, *tr, *key, *mu, *rhoprime;
    uint16_t nonce = 0;
    poly c, chat;
    polyvec_max mat[ DIL_VECT_MAX ];        /* using only K * L */
    polyvec_max s1, y, z;                   /* L */
    polyvec_max t0, s2, w1, w0, h;          /* K */
    Keccak_state state;

    (void) type;

                // layout: [key || mu] MUST be consecutive

    rho = seedbuf;
    tr  = rho + DIL_SEEDBYTES;
    key = tr  + DIL_CRHBYTES;
    mu  = key + DIL_SEEDBYTES;
    rhoprime = mu + DIL_CRHBYTES;

    switch (skbytes) {
    case DIL_PRV5x4_BYTES: K=5; break;
    case DIL_PRV6x5_BYTES: K=6; break;
    case DIL_PRV8x7_BYTES: K=8; break;
    default:
        return 0;
    }

    beta = dil_beta(K);

        // unpack_sk(rho, key, tr, &s1, &s2, &t0, sk);

    switch (K) {                     /* s1 is L-sized; s2+t0 are K-sized */
    case 5:
        unpack_prv5(rho, key, tr, (polyvec4 *) &s1, (polyvec5 *) &s2,
                    (polyvec5 *) &t0, sk);
        break;
    case 6:
        unpack_prv6(rho, key, tr, (polyvec5 *) &s1, (polyvec6 *) &s2,
                    (polyvec6 *) &t0, sk);
        break;
    case 8:
        unpack_prv8(rho, key, tr, (polyvec7 *) &s1, (polyvec8 *) &s2,
                    (polyvec8 *) &t0, sk);
        break;
    }

                    /* Compute CRH(tr, msg) */
    shake256_init(&state);
    shake256_absorb(&state, tr, DIL_CRHBYTES);
    shake256_absorb(&state, m, mlen);
    shake256_finalize(&state);
    shake256_squeeze(mu, DIL_CRHBYTES, &state);

#if 0 && DILITHIUM_RANDOMIZED_SIGNING
    randombytes(rhoprime, CRHBYTES);
#endif
    dil_crh(rhoprime, DIL_CRHBYTES, key, DIL_SEEDBYTES +DIL_CRHBYTES);

                /* Expand matrix and transform vectors */
    switch (K) {
    case 5:
        expand_matrix_5x4(mat, rho);
        polyvec4_ntt((polyvec4 *) &s1);   /* L */
        polyvec5_ntt((polyvec5 *) &s2);   /* K */
        polyvec5_ntt((polyvec5 *) &t0);
        break;
    case 6:
        expand_matrix_6x5(mat, rho);
        polyvec5_ntt((polyvec5 *) &s1);   /* L */
        polyvec6_ntt((polyvec6 *) &s2);   /* K */
        polyvec6_ntt((polyvec6 *) &t0);
        break;
    case 8:
        expand_matrix_8x7(mat, rho);
        polyvec7_ntt((polyvec7 *) &s1);   /* L */
        polyvec8_ntt((polyvec8 *) &s2);   /* K */
        polyvec8_ntt((polyvec8 *) &t0);
        break;
    }

REJECT:
    /* Sample intermediate vector y */
    for (i = 0; i < K-1; ++i)                                       /* L */
        poly_uniform_gamma1m1(&( y.vec[i] ), rhoprime, nonce++);

                /* Matrix-vector multiplication */
    z = y;

    switch (K) {              // K->L, +1 difference is not a typo
    case 5: polyvec4_ntt((polyvec4 *) &z); break;
    case 6: polyvec5_ntt((polyvec5 *) &z); break;
    case 8: polyvec7_ntt((polyvec7 *) &z); break;
    }

    for (i = 0; i < K; ++i) {
        switch (K) {          // K->L, +1 difference is not a typo
        case 5:
        polyvec4_pointwise_acc_montgomery(&( w1.vec[i] ),
                                   (const polyvec4 *) &( mat[i] ),
                                   (const polyvec4 *) &z);
        break;
        case 6:
        polyvec5_pointwise_acc_montgomery(&( w1.vec[i] ),
                                   (const polyvec5 *) &( mat[i] ),
                                   (const polyvec5 *) &z);
        break;
        case 8:
        polyvec7_pointwise_acc_montgomery(&( w1.vec[i] ),
                                   (const polyvec7 *) &( mat[i] ),
                                   (const polyvec7 *) &z);
        break;
        }

        poly_reduce(&( w1.vec[i] ));
        poly_invntt_tomont(&( w1.vec[i] ));
    }

                /* Decompose w and call the random oracle */

    switch (K) {
    case 5:
        polyvec5_csubq((polyvec5 *) &w1);
        polyvec5_decompose((polyvec5 *) &w1, (polyvec5 *) &w0,
                           (polyvec5 *) &w1);
        break;
    case 6:
        polyvec6_csubq((polyvec6 *) &w1);
        polyvec6_decompose((polyvec6 *) &w1, (polyvec6 *) &w0,
                           (polyvec6 *) &w1);
        break;
    case 8:
        polyvec8_csubq((polyvec8 *) &w1);
        polyvec8_decompose((polyvec8 *) &w1, (polyvec8 *) &w0,
                           (polyvec8 *) &w1);
        break;
    }
    dil_challenge(&c, mu, &w1, K);

    chat = c;
    poly_ntt256(&chat);

                 /* Compute z, reject if it reveals secret */

    for (i = 0; i < K-1; ++i) {                                     /* L */
        poly_pointwise_montgomery(&( z.vec[i] ), &chat, &( s1.vec[i] ));
        poly_invntt_tomont(&( z.vec[i] ));
    }

    {
    unsigned fail = 0;

    switch (K) {          // -> L, so choices are off-by-one
    case 5:
        polyvec4_add((polyvec4 *) &z, (const polyvec4 *) &z,
                     (const polyvec4 *) &y);
        polyvec4_freeze((polyvec4 *) &z);
        fail = !!polyvec4_chknorm((const polyvec4 *) &z,
                                  DIL_GAMMA1 - beta);
        break;
    case 6:
        polyvec5_add((polyvec5 *) &z, (const polyvec5 *) &z,
                     (const polyvec5 *) &y);
        polyvec5_freeze((polyvec5 *) &z);
        fail = !!polyvec5_chknorm((const polyvec5 *) &z,
                                  DIL_GAMMA1 - beta);
        break;
    case 8:
        polyvec7_add((polyvec7 *) &z, (const polyvec7 *) &z,
                     (const polyvec7 *) &y);
        polyvec7_freeze((polyvec7 *) &z);
        fail = !!polyvec7_chknorm((const polyvec7 *) &z,
                                  DIL_GAMMA1 - beta);
        break;
    }

    if (fail)
        goto REJECT;
    }

        /* Check that subtracting cs2 does not change high bits of w
         * and low bits do not reveal secret information */

    for (i = 0; i < K; ++i) {
        poly_pointwise_montgomery(&( h.vec[i] ), &chat, &( s2.vec[i] ));
        poly_invntt_tomont(&( h.vec[i] ));
    }

    {
    unsigned int fail = 0;

    switch (K) {
    case 5:
        polyvec5_sub((polyvec5 *) &w0, (const polyvec5 *) &w0,
                     (const polyvec5 *) &h);
        polyvec5_freeze((polyvec5 *) &w0);
        fail = !!polyvec5_chknorm((const polyvec5 *) &w0,
                                  DIL_GAMMA2 - beta);
        break;
    case 6:
        polyvec6_sub((polyvec6 *) &w0, (const polyvec6 *) &w0,
                     (const polyvec6 *) &h);
        polyvec6_freeze((polyvec6 *) &w0);
        fail = !!polyvec6_chknorm((const polyvec6 *) &w0,
                                  DIL_GAMMA2 - beta);
        break;
    case 8:
        polyvec8_sub((polyvec8 *) &w0, (const polyvec8 *) &w0,
                     (const polyvec8 *) &h);
        polyvec8_freeze((polyvec8 *) &w0);
        fail = !!polyvec8_chknorm((const polyvec8 *) &w0,
                                  DIL_GAMMA2 - beta);
        break;
    default:
        break;
    }
    if (fail)
        goto REJECT;
    }

                        /* Compute hints for w1 */

    for (i = 0; i < K; ++i) {
        poly_pointwise_montgomery(&( h.vec[i] ), &chat, &( t0.vec[i] ));
        poly_invntt_tomont(&( h.vec[i] ));
    }

    {
    unsigned int fail = 0;

    switch (K) {
    case 5:
        polyvec5_csubq((polyvec5 *) &h);
        fail = !!polyvec5_chknorm((const polyvec5 *) &h, DIL_GAMMA2);
        break;
    case 6:
        polyvec6_csubq((polyvec6 *) &h);
        fail = !!polyvec6_chknorm((const polyvec6 *) &h, DIL_GAMMA2);
        break;
    case 8:
        polyvec8_csubq((polyvec8 *) &h);
        fail = !!polyvec8_chknorm((const polyvec8 *) &h, DIL_GAMMA2);
        break;

    default:
        break;
    }
    if (fail)
        goto REJECT;
    }

    switch (K) {
    case 5:
        polyvec5_add((polyvec5 *) &w0, (const polyvec5 *) &w0,
                     (const polyvec5 *) &h);
        polyvec5_csubq((polyvec5 *) &w0);
        n = polyvec5_make_hint((polyvec5 *) &h, (const polyvec5 *) &w0,
                               (const polyvec5 *) &w1);
        break;
    case 6:
        polyvec6_add((polyvec6 *) &w0, (const polyvec6 *) &w0,
                     (const polyvec6 *) &h);
        polyvec6_csubq((polyvec6 *) &w0);
        n = polyvec6_make_hint((polyvec6 *) &h, (const polyvec6 *) &w0,
                               (const polyvec6 *) &w1);
        break;
    case 8:
        polyvec8_add((polyvec8 *) &w0, (const polyvec8 *) &w0,
                     (const polyvec8 *) &h);
        polyvec8_csubq((polyvec8 *) &w0);
        n = polyvec8_make_hint((polyvec8 *) &h, (const polyvec8 *) &w0,
                               (const polyvec8 *) &w1);
        break;

    default:
        break;
    }

    if (n > dil_omega(K))
        goto REJECT;

    siglen = dil_sig2wire(sig, siglen, &z, &h, &c, K);

        /* TODO: cleanup potentially-sensitive stuff */

    return siglen;
}


/*************************************************
* Description: Verifies signature.
* Arguments:   - uint8_t *m: pointer to input signature
*              - size_t siglen: length of signature
*              - const uint8_t *m: pointer to message
*              - size_t mlen: length of message
*              - const uint8_t *pk: pointer to bit-packed public key
*
* Returns >0 if signature could be verified correctly and 0 otherwise
**************************************************/
static
int ref_verify(const uint8_t *sig,
                  size_t siglen,
                  const uint8_t *m,
                  size_t mlen,
                  const uint8_t *pk,
                  size_t pkbytes)
{
    unsigned int i, K = 0, beta;
    uint8_t rho[ DIL_SEEDBYTES ];
    uint8_t mu[ DIL_CRHBYTES ];
    size_t sigb;
    poly c, cp;
    polyvec_max mat[ DIL_VECT_MAX ], z;        /* L; LxK (mat) */
    polyvec_max t1, h, w1;                     /* K */
    Keccak_state state;

    switch (pkbytes) {
    case DIL_PUB5x4_BYTES: K=5; break;
    case DIL_PUB6x5_BYTES: K=6; break;
    case DIL_PUB8x7_BYTES: K=8; break;
    default:
        return PQCA_EKEYTYPE;
    }

    sigb = dil_signature_bytes(K);
    beta = dil_beta(K);
    if (!sigb || (siglen != sigb))
        return 0;

    switch (K) {
    case 5: unpack_pk5(rho, (polyvec5 *) &t1, pk); break;
    case 6: unpack_pk6(rho, (polyvec6 *) &t1, pk); break;
    case 8: unpack_pk8(rho, (polyvec8 *) &t1, pk); break;
    default:
        return PQCA_EINTERN;
    }

    if (dil_wire2sig(&z, &h, &c, K, sig, siglen))
        return 0;

    {
    unsigned int fail = 0;
    switch (K){              /* check Lx vector, so -1 is not off-by-one */
    case 5:
        fail = !!polyvec4_chknorm((const polyvec4 *) &z,
                                  DIL_GAMMA1 -beta);
        break;
    case 6:
        fail = !!polyvec5_chknorm((const polyvec5 *) &z,
                                  DIL_GAMMA1 -beta);
        break;
    case 8:
        fail = !!polyvec7_chknorm((const polyvec7 *) &z,
                                  DIL_GAMMA1 -beta);
        break;
    default:
        break;
    }
    if (fail)
        return 0;
    }

                /* Compute CRH(CRH(rho, t1), msg) */

    dil_crh(mu, DIL_CRHBYTES, pk, pkbytes);
        //
    shake256_init(&state);
    shake256_absorb(&state, mu, DIL_CRHBYTES);
    shake256_absorb(&state, m, mlen);
    shake256_finalize(&state);
    shake256_squeeze(mu, DIL_CRHBYTES, &state);

            /* Matrix-vector multiplication; compute Az - c2^dt1 */

    switch (K) {                 /* NTT on L-sized vector, K-1 */
    case 5:
        expand_matrix_5x4(mat, rho);
        polyvec4_ntt((polyvec4 *) &z);   /* L */
        break;
    case 6:
        expand_matrix_6x5(mat, rho);
        polyvec5_ntt((polyvec5 *) &z);   /* L */
        break;
    case 8:
        expand_matrix_8x7(mat, rho);
        polyvec7_ntt((polyvec7 *) &z);   /* L */
        break;
    }

    for (i = 0; i < K ; ++i) {
    switch (K) {
    case 5:
        polyvec4_pointwise_acc_montgomery(&( w1.vec[i] ),
                               (const polyvec4 *) &( mat[i] ),
                               (const polyvec4 *) &z);
        break;
    case 6:
        polyvec5_pointwise_acc_montgomery(&( w1.vec[i] ),
                               (const polyvec5 *) &( mat[i] ),
                               (const polyvec5 *) &z);
        break;
    case 8:
        polyvec7_pointwise_acc_montgomery(&( w1.vec[i] ),
                               (const polyvec7 *) &( mat[i] ),
                               (const polyvec7 *) &z);
        break;
    }
    }

    cp = c;
    poly_ntt256(&cp);

    switch (K) {
    case 5:
        polyvec5_shiftl((polyvec5 *) &t1);
        polyvec5_ntt((polyvec5 *) &t1);
        break;
    case 6:
        polyvec6_shiftl((polyvec6 *) &t1);
        polyvec6_ntt((polyvec6 *) &t1);
        break;
    case 8:
        polyvec8_shiftl((polyvec8 *) &t1);
        polyvec8_ntt((polyvec8 *) &t1);
        break;
    }

    for (i = 0; i < K; ++i) {
        poly_pointwise_montgomery(&( t1.vec[i] ), &cp,
                                  &( t1.vec[i] ));
    }

  /* csubq: Reconstruct w1 */
    switch (K) {
    case 5:
        polyvec5_sub((polyvec5 *) &w1, (const polyvec5 *) &w1,
                     (const polyvec5 *) &t1);
        polyvec5_reduce((polyvec5 *) &w1);
        polyvec5_invntt_tomont((polyvec5 *) &w1);
        polyvec5_csubq((polyvec5 *) &w1);
        polyvec5_use_hint((polyvec5 *) &w1, (const polyvec5 *) &w1,
                          (const polyvec5 *) &h);
        break;
    case 6:
        polyvec6_sub((polyvec6 *) &w1, (const polyvec6 *) &w1,
                     (const polyvec6 *) &t1);
        polyvec6_reduce((polyvec6 *) &w1);
        polyvec6_invntt_tomont((polyvec6 *) &w1);
        polyvec6_csubq((polyvec6 *) &w1);
        polyvec6_use_hint((polyvec6 *) &w1, (const polyvec6 *) &w1,
                          (const polyvec6 *) &h);
        break;
    case 8:
        polyvec8_sub((polyvec8 *) &w1, (const polyvec8 *) &w1,
                     (const polyvec8 *) &t1);
        polyvec8_reduce((polyvec8 *) &w1);
        polyvec8_invntt_tomont((polyvec8 *) &w1);
        polyvec8_csubq((polyvec8 *) &w1);
        polyvec8_use_hint((polyvec8 *) &w1, (const polyvec8 *) &w1,
                          (const polyvec8 *) &h);
        break;
    }

                 /* Call random oracle and verify challenge */

    dil_challenge(&cp, mu, &w1, K);

    for (i = 0; i < DIL_N; ++i) {
        if (c.coeffs[i] != cp.coeffs[i])
            return 0;
    }

    return 1;
}


#endif      /*-----  /delimiter: sign/verify  ------------------------------*/




#if defined(STANDALONE)
// devel only
static unsigned long crd_keycnt;
#endif //-----STANDALONE--------------------


#if 1       /*-----  delimiter: key.generate  ------------------------------*/
/*************************************************
* Description: Generates public and private key.
* Arguments:   - uint8_t *pk: pointer to output public key (allocated
*                             array of CRYPTO_PUBLICKEYBYTES bytes)
*              - uint8_t *sk: pointer to output private key (allocated
*                             array of CRYPTO_SECRETKEYBYTES bytes)
*
* Returns >0 upon success; number of bytes written to start of (prv, prbytes)
**************************************************/
int dil_keygen(unsigned char *prv,   size_t prbytes,
               unsigned char *pub,   size_t *pbbytes,
         const unsigned char *algid, size_t ibytes)
{
    unsigned int i, type, K, eta;
    unsigned char seedbuf[ 3*DIL_SEEDBYTES ];
    unsigned char tr[ DIL_CRHBYTES ];
    const unsigned char *rho, *rhoprime, *key;
    uint16_t nonce = 0;
    polyvec_max mat[ DIL_VECT_MAX ];        /* using only K * L */
    polyvec_max s1, s1hat;                  /* L */
    polyvec_max s2, t1, t0;                 /* K */
    size_t wrb;

    if (!pbbytes)
        return PQCA_EPARAM;

    type = dil_oid2type(algid, ibytes);
    K    = dil_type2k  (type);
    eta  = dil_type2eta(type);
    wrb  = dil_prv_wirebytes(K);
    if (!type || !K || !wrb)
        return PQCA_EKEYTYPE;

    if ((wrb > prbytes) || (dil_pub_wirebytes(K) > *pbbytes))
        return PQCA_ETOOSMALL;
    *pbbytes = dil_pub_wirebytes(K);

                /* Get randomness for rho, rhoprime and key */

    randombytes(seedbuf, sizeof(seedbuf));

#if defined(STANDALONE)
    if (getenv("CRDEBUG"))
    {
        ++crd_keycnt;

        for (i=0; i<DIL_SEEDBYTES; ++i) {
            seedbuf[ 3*i   ] = 1 +   crd_keycnt +i +1;
            seedbuf[ 3*i+1 ] = 2 +(((crd_keycnt +i +1) *5) >>  5);
            seedbuf[ 3*i+2 ] = 3 +(((crd_keycnt +i +1) *7) >> 10);
        }
    }
#endif //-----STANDALONE-----

    rho      = seedbuf;
    rhoprime = seedbuf + DIL_SEEDBYTES;
    key      = seedbuf + 2* DIL_SEEDBYTES;

    /* Expand matrix */

    switch (K) {
    case 5: expand_matrix_5x4(mat, rho); break;
    case 6: expand_matrix_6x5(mat, rho); break;
    case 8: expand_matrix_8x7(mat, rho); break;
    }

            /* Sample short vectors s1 and s2 */
    for (i = 0; i < K-1; ++i)                                /* L */
        poly_uniform_eta(&( s1.vec[i] ), rhoprime, eta, nonce++);

    for (i = 0; i < K; ++i)
        poly_uniform_eta(&( s2.vec[i] ), rhoprime, eta, nonce++);

    s1hat = s1;

    switch (K) {
    case 5: polyvec4_ntt((polyvec4 *) &s1hat); break;
    case 6: polyvec5_ntt((polyvec5 *) &s1hat); break;
    case 8: polyvec7_ntt((polyvec7 *) &s1hat); break;
    default:
        break;
    }

    for (i = 0; i < K; ++i) {
        switch (K) {             /* K -> L, so -1 offset is not typo */
        case 5:
        polyvec4_pointwise_acc_montgomery(&t1.vec[i],
                       (const polyvec4 *) &( mat[i] ),
                       (const polyvec4 *) &s1hat);
        break;

        case 6:
        polyvec5_pointwise_acc_montgomery(&t1.vec[i],
                       (const polyvec5 *) &( mat[i] ),
                       (const polyvec5 *) &s1hat);
        break;

        case 8:
        polyvec7_pointwise_acc_montgomery(&t1.vec[i],
                       (const polyvec7 *) &( mat[i] ),
                       (const polyvec7 *) &s1hat);
        break;
        }
    }
    for (i = 0; i < K; ++i) {
        poly_reduce(&( t1.vec[i] ));
        poly_invntt_tomont(&( t1.vec[i] ));
    }

        /* Add error vector s2 (..._add()), then
         * Extract t1 and write public key
         */

    switch (K) {
    case 5:
        polyvec5_add((polyvec5 *) &t1, (polyvec5 *) &t1,
                     (const polyvec5 *) &s2);
        polyvec5_freeze((polyvec5 *) &t1);
        polyvec5_power2round((polyvec5 *) &t1, (polyvec5 *) &t0,
                             (const polyvec5 *) &t1);
        pack_pk5(pub, rho, (const polyvec5 *) &t1);
        break;

    case 6:
        polyvec6_add((polyvec6 *) &t1, (polyvec6 *) &t1,
                     (const polyvec6 *) &s2);
        polyvec6_freeze((polyvec6 *) &t1);
        polyvec6_power2round((polyvec6 *) &t1, (polyvec6 *) &t0,
                             (const polyvec6 *) &t1);
        pack_pk6(pub, rho, (const polyvec6 *) &t1);
        break;

    case 8:
        polyvec8_add((polyvec8 *) &t1, (polyvec8 *) &t1,
                     (const polyvec8 *) &s2);
        polyvec8_freeze((polyvec8 *) &t1);
        polyvec8_power2round((polyvec8 *) &t1, (polyvec8 *) &t0,
                             (const polyvec8 *) &t1);
        pack_pk8(pub, rho, (const polyvec8 *) &t1);
        break;

    default:
        break;
    }

                /* Compute CRH(rho, t1) and write priv. key */

    dil_crh(tr, DIL_CRHBYTES, pub, *pbbytes);

    switch (K) {
    case 5: pack_prv5(prv, rho, key, tr, (const polyvec4 *) &s1,
                      (const polyvec5 *) &s2, (const polyvec5 *) &t0);
            break;
    case 6: pack_prv6(prv, rho, key, tr, (const polyvec5 *) &s1,
                      (const polyvec6 *) &s2, (const polyvec6 *) &t0);
            break;
    case 8: pack_prv8(prv, rho, key, tr, (const polyvec7 *) &s1,
                      (const polyvec8 *) &s2, (const polyvec8 *) &t0);
            break;
    default:
        break;
    }

    return (int) wrb;
}

#endif      /*-----  /delimiter: key.generate  -----------------------------*/
#endif      /*-----  /delimiter: Dilithium core  ---------------------------*/


#if 1       /*-----  delimiter: PKCS11 wrappers  ---------------------------*/
int pqca_generate(unsigned char *prv,   size_t prvbytes,
                  unsigned char *pub,   size_t *pubbytes,
            const unsigned char *algid, size_t ibytes)
{
    int rc;

    if (!prv || !prvbytes || !pub || !pubbytes)
        return 0;

    rc = dil_keygen(prv, prvbytes, pub, pubbytes, algid, ibytes);

        /* placeholder: reference yet-unused internal fns */

    if (0) {
        shake128(NULL, ~0, NULL, ~0);
    }

        // ...log any failure(reason) etc...

    return (size_t) rc;
}


/*------------------------------------*/
int pqca_sign(unsigned char *sig,   size_t sbytes,
        const unsigned char *msg,   size_t mbytes,
        const unsigned char *prv,   size_t pbytes,
        const unsigned char *algid, size_t ibytes)
{
    int v;

    if (!sig || !sbytes || !msg || !mbytes || !prv || !pbytes)
        (void) 0;

        // TODO: currently, picks defaults
    (void) algid;
    (void) ibytes;

    v = ref_sign(sig, sbytes, msg, mbytes, prv, pbytes,
                 0 /* alg.id -> type, currently ignored */);

// TODO: log other, non-verify-indicating errors

    return !!(v > 0);
}


/*------------------------------------*/
int pqca_verify(const unsigned char *sig,   size_t sbytes,
                const unsigned char *msg,   size_t mbytes,
                const unsigned char *pub,   size_t pbytes,
                const unsigned char *algid, size_t ibytes)
{
    int v;

    if (!sig || !sbytes || !msg || !mbytes || !pub || !pbytes)
        (void) 0;

        // TODO: currently, picks defaults
    (void) algid;
    (void) ibytes;

    v = ref_verify(sig, sbytes, msg, mbytes, pub, pbytes);

// TODO: log other, non-verify-indicating errors

    return !!(v > 0);
}


/*------------------------------------*/
int pqca_key2wire(unsigned char *wire,  size_t wbytes,
            const unsigned char *key,   size_t kbytes,
            const unsigned char *algid, size_t ibytes)
{
    size_t wr;

    wr = dil_pub2wire(wire, wbytes, key, kbytes, algid, ibytes);

// TODO: check for prv->pkcs8 conversion too

    return (int) wr;
}


/*------------------------------------*/
int pqca_wire2key(unsigned char *key,   size_t kbytes,
            const unsigned char *wire,  size_t wbytes,
            const unsigned char *algid, size_t ibytes)
{
    unsigned int type;
    size_t wr;

    wr = dil_wire2pub(key, kbytes, &type, wire, wbytes, algid, ibytes);

    (void) type;

// TODO: check for pkcs8->prv conversion too

    return (int) wr;
}
#endif      /*-----  /delimiter: PKCS11 wrappers  --------------------------*/


#if defined(STANDALONE)
#include <stdio.h>

#define  BIG_ENOUGH  ((size_t) 6000)

static const struct {
    const unsigned char *oid;
    size_t obytes;
} dilt_oids[] = {

    {
    (const unsigned char *)
    "\x06\x0b" "\x2b\x06\x01\x04\x01\x02\x82\x0b\x01\x08\x07", 13,
    },         // round2, 8-7

#if 0
    {
    (const unsigned char *)
    "\x06\x0b" "\x2b\x06\x01\x04\x01\x02\x82\x0b\x06\x05\x04", 13,
    },         // round2 raw, 5-4
    {
    (const unsigned char *)
    "\x06\x0b" "\x2b\x06\x01\x04\x01\x02\x82\x0b\x06\x06\x05", 13,
    },         // round2 raw, 6-5
    {
    (const unsigned char *)
    "\x06\x0b" "\x2b\x06\x01\x04\x01\x02\x82\x0b\x06\x08\x07", 13,
    },         // round2 raw, 8-7
#endif

} ;


static unsigned char dilt_msg0[] = {
    0x14,0x15,0x92,0x65,0x35, 0x89,0x79,0x32,0x38,0x46,
    0x26,0x43,0x38,0x32,0x79, 0x50,0x28,0x84,0x19,0x71,
    0x69,0x39,0x93,0x75,0x10, 0x58,0x20,0x97,0x49,0x44,
    0x59,0x23,0x07,0x81,0x64, 0x06,0x28,0x62,0x08,0x99,
    0x86,0x28,0x03,0x48,0x25, 0x34,0x21,0x17,0x06,0x79,
    0x82,0x14,0x80,0x86,0x51, 0x32,0x82,0x30,0x66,0x47,
    0x09,0x38,0x44,0x60};

//--------------------------------------
int main(void)
{
    unsigned char prv[ 5136 ],
                  pub[ 2336 ],
                  sig[ 4668 ];
    unsigned int i, offs = 0;
    size_t prvb, pubb;
    int rc = 0;

    printf("DIL.PRV.B(%u)=%zu\n", 5, dil_prv_wirebytes(5));
    printf("DIL.PRV.B(%u)=%zu\n", 6, dil_prv_wirebytes(6));
    printf("DIL.PRV.B(%u)=%zu\n", 8, dil_prv_wirebytes(8));

    printf("DIL.PUB.B(%u)=%zu\n", 5, dil_pub_wirebytes(5));
    printf("DIL.PUB.B(%u)=%zu\n", 6, dil_pub_wirebytes(6));
    printf("DIL.PUB.B(%u)=%zu\n", 8, dil_pub_wirebytes(8));

    printf("DIL.SIG.B(%u)=%zu\n", 5, dil_signature_bytes(5));
    printf("DIL.SIG.B(%u)=%zu\n", 6, dil_signature_bytes(6));
    printf("DIL.SIG.B(%u)=%zu\n", 8, dil_signature_bytes(8));

    for (i = 0; i < ARRAY_ELEMS(dilt_oids); ++i) {
        unsigned int j;
        size_t asnb;

        if (!dilt_oids[i].oid || !dilt_oids[i].obytes)
            continue;
        ++offs;

        if (getenv("CRDEBUG")) {
            crd_keycnt = 0;
        }

        for (j=0; j<1; ++j) {

        prvb = sizeof(prv);
        pubb = sizeof(pub);

        memset(prv, i+i+offs+j+1, sizeof(prv));
        memset(pub, i+i+offs+j+2, sizeof(pub));

        rc = dil_keygen(prv, sizeof(prv), pub, &pubb,
                        dilt_oids[i].oid, dilt_oids[i].obytes);
        if (rc < 0)
            break;

        if (rc > 0) {
            prvb = (size_t) rc;

            printf("PRV.B=%zu\n", prvb);
            //cu_hexprint("PRV=", prv, prvb);

            printf("PUB.B=%zu\n", pubb);
            //cu_hexprint("PUB=", pub, pubb);
        }


        if (rc < 0)
            break;

        {
        size_t mb, sigb;
        int ver;
        mb=64;
            printf("MSG.B=%zu\n", mb);
            //cu_hexprint("MSG=", dilt_msg0, mb);

            sigb = ref_sign(sig, sizeof(sig), dilt_msg0, mb,
                            prv, prvb, 0);
            if (sigb) {
                printf("SIG.B=%zu\n", sigb);
                //cu_hexprint("SIG=", sig, sigb);
            }

            ver = ref_verify(sig, sigb, dilt_msg0, mb, pub, pubb);
            printf("VER=%d\n", ver);
        }
        }
        if (rc < 0)
            break;
    }

    return (rc < 0) ? -1 : 0;
}
#endif         /* defined(STANDALONE) */

