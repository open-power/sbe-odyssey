/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/common/algorithms/polyvec-include.h $              */
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
/* generated code follows, do not modify */

typedef struct {
    poly vec[ 4 ];
} polyvec4 ;


typedef struct {
    poly vec[ 5 ];
} polyvec5 ;


typedef struct {
    poly vec[ 6 ];
} polyvec6 ;


typedef struct {
    poly vec[ 7 ];
} polyvec7 ;


typedef struct {
    poly vec[ 8 ];
} polyvec8 ;


/*------------------------------------*/
static void
polyvec4_freeze(polyvec4 *pv)
{
    unsigned int i;

    for (i = 0; i < 4; ++i) {
        poly_freeze(&( pv->vec[i] ));
    }
}


/*------------------------------------*/
static void
polyvec4_add(polyvec4 *r, const polyvec4 *u, const polyvec4 *v)
{
    unsigned int i;

    for (i = 0; i < 4; ++i) {
        poly_add(&( r->vec[i] ),
                 &( u->vec[i] ),
                 &( v->vec[i] ));
    }
}


/*------------------------------------*/
static void
polyvec4_ntt(polyvec4 *v)
{
    unsigned int i;

    for (i = 0; i < 4; ++i) {
        poly_ntt256(&( v->vec[i] ));
    }
}


/*------------------------------------*/
ATTR_PURE__ static unsigned int
polyvec4_chknorm(const polyvec4 *v, uint32_t bound)
{
    unsigned int i;

    for (i = 0; i < 4; ++i) {
        if (poly_chknorm(&( v->vec[i] ), bound))
            return 1;
    }

    return 0;
}


/*------------------------------------*/
static void
polyvec5_freeze(polyvec5 *pv)
{
    unsigned int i;

    for (i = 0; i < 5; ++i) {
        poly_freeze(&( pv->vec[i] ));
    }
}


/*------------------------------------*/
static void
polyvec5_add(polyvec5 *r, const polyvec5 *u, const polyvec5 *v)
{
    unsigned int i;

    for (i = 0; i < 5; ++i) {
        poly_add(&( r->vec[i] ),
                 &( u->vec[i] ),
                 &( v->vec[i] ));
    }
}


/*------------------------------------*/
static void
polyvec5_ntt(polyvec5 *v)
{
    unsigned int i;

    for (i = 0; i < 5; ++i) {
        poly_ntt256(&( v->vec[i] ));
    }
}


/*------------------------------------*/
ATTR_PURE__ static unsigned int
polyvec5_chknorm(const polyvec5 *v, uint32_t bound)
{
    unsigned int i;

    for (i = 0; i < 5; ++i) {
        if (poly_chknorm(&( v->vec[i] ), bound))
            return 1;
    }

    return 0;
}


/*------------------------------------*/
static void
polyvec6_freeze(polyvec6 *pv)
{
    unsigned int i;

    for (i = 0; i < 6; ++i) {
        poly_freeze(&( pv->vec[i] ));
    }
}


/*------------------------------------*/
static void
polyvec6_add(polyvec6 *r, const polyvec6 *u, const polyvec6 *v)
{
    unsigned int i;

    for (i = 0; i < 6; ++i) {
        poly_add(&( r->vec[i] ),
                 &( u->vec[i] ),
                 &( v->vec[i] ));
    }
}


/*------------------------------------*/
static void
polyvec6_ntt(polyvec6 *v)
{
    unsigned int i;

    for (i = 0; i < 6; ++i) {
        poly_ntt256(&( v->vec[i] ));
    }
}


/*------------------------------------*/
ATTR_PURE__ static unsigned int
polyvec6_chknorm(const polyvec6 *v, uint32_t bound)
{
    unsigned int i;

    for (i = 0; i < 6; ++i) {
        if (poly_chknorm(&( v->vec[i] ), bound))
            return 1;
    }

    return 0;
}


/*------------------------------------*/
static void
polyvec7_freeze(polyvec7 *pv)
{
    unsigned int i;

    for (i = 0; i < 7; ++i) {
        poly_freeze(&( pv->vec[i] ));
    }
}


/*------------------------------------*/
static void
polyvec7_add(polyvec7 *r, const polyvec7 *u, const polyvec7 *v)
{
    unsigned int i;

    for (i = 0; i < 7; ++i) {
        poly_add(&( r->vec[i] ),
                 &( u->vec[i] ),
                 &( v->vec[i] ));
    }
}


/*------------------------------------*/
static void
polyvec7_ntt(polyvec7 *v)
{
    unsigned int i;

    for (i = 0; i < 7; ++i) {
        poly_ntt256(&( v->vec[i] ));
    }
}


/*------------------------------------*/
ATTR_PURE__ static unsigned int
polyvec7_chknorm(const polyvec7 *v, uint32_t bound)
{
    unsigned int i;

    for (i = 0; i < 7; ++i) {
        if (poly_chknorm(&( v->vec[i] ), bound))
            return 1;
    }

    return 0;
}


/*------------------------------------*/
static void
polyvec8_freeze(polyvec8 *pv)
{
    unsigned int i;

    for (i = 0; i < 8; ++i) {
        poly_freeze(&( pv->vec[i] ));
    }
}


/*------------------------------------*/
static void
polyvec8_add(polyvec8 *r, const polyvec8 *u, const polyvec8 *v)
{
    unsigned int i;

    for (i = 0; i < 8; ++i) {
        poly_add(&( r->vec[i] ),
                 &( u->vec[i] ),
                 &( v->vec[i] ));
    }
}


/*------------------------------------*/
static void
polyvec8_ntt(polyvec8 *v)
{
    unsigned int i;

    for (i = 0; i < 8; ++i) {
        poly_ntt256(&( v->vec[i] ));
    }
}


/*------------------------------------*/
ATTR_PURE__ static unsigned int
polyvec8_chknorm(const polyvec8 *v, uint32_t bound)
{
    unsigned int i;

    for (i = 0; i < 8; ++i) {
        if (poly_chknorm(&( v->vec[i] ), bound))
            return 1;
    }

    return 0;
}


/*------------------------------------*/
static void
expand_matrix_5x4(polyvec_max mat[ 5 ], const unsigned char rho[ DIL_SEEDBYTES ])
{
    unsigned int k, l;

    for (k = 0; k < 5; ++k) {
        for (l = 0; l < 4; ++l) {
            poly_uniform(&( mat[k].vec[l] ), rho, (k <<8) +l);
        }
    }
}


/*------------------------------------*/
static unsigned int
polyvec5_make_hint(polyvec5 *h,
             const polyvec5 *v0,
             const polyvec5 *v1)
{
    unsigned int i, nr = 0;

    for (i = 0; i < 5; ++i) {
        nr += poly_make_hint(&(  h->vec[i] ),
                             &( v0->vec[i] ),
                             &( v1->vec[i] ));
    }

    return nr;
}


/*------------------------------------*/
static void
polyvec5_use_hint(polyvec5 *r,
            const polyvec5 *u,
            const polyvec5 *v)
{
    unsigned int i;

    for (i = 0; i < 5; ++i) {
        poly_use_hint(&( r->vec[i] ),
                      &( u->vec[i] ),
                      &( v->vec[i] ));
    }
}


/*------------------------------------*/
static void
polyvec5_decompose(polyvec5 *v1,
                   polyvec5 *v0,
             const polyvec5 *v)
{
    unsigned int i;

    for (i = 0; i < 5; ++i) {
        poly_decompose(&( v1->vec[i] ),
                       &( v0->vec[i] ),
                       &(  v->vec[i] ));
    }
}


/*------------------------------------*/
static void
polyvec5_power2round(polyvec5 *v1,
                     polyvec5 *v0,
               const polyvec5 *v)
{
    unsigned int i;

    for (i = 0; i < 5; ++i) {
        poly_power2round(&( v1->vec[i] ),
                         &( v0->vec[i] ),
                         &(  v->vec[i] ));
    }
}


/*------------------------------------*/
static void
polyvec5_shiftl(polyvec5 *v)
{
    unsigned int i;

    for (i = 0; i < 5; ++i) {
        poly_shiftl(&( v->vec[i] ));
    }
}


/*------------------------------------*/
static void
polyvec5_sub(polyvec5 *r, const polyvec5 *u, const polyvec5 *v)
{
    unsigned int i;

    for (i = 0; i < 5; ++i) {
        poly_sub(&( r->vec[i] ),
                 &( u->vec[i] ),
                 &( v->vec[i] ));
    }
}


/*------------------------------------*/
static void
polyvec5_reduce(polyvec5 *v)
{
    unsigned int i;

    for (i = 0; i < 5; ++i) {
        poly_reduce(&( v->vec[i] ));
    }
}


/*------------------------------------*/
static void
polyvec5_csubq(polyvec5 *v)
{
    unsigned int i;

    for (i = 0; i < 5; ++i) {
        poly_csubq(&( v->vec[i] ));
    }
}


/*------------------------------------*/
static void
polyvec5_invntt_tomont(polyvec5 *v)
{
    unsigned int i;

    for (i = 0; i < 5; ++i) {
        poly_invntt_tomont(&( v->vec[i] ));
    }
}


/*------------------------------------*/
static void
pack_pk5(unsigned char pk [ DIL_PUB5x4_BYTES ],
   const unsigned char rho[ DIL_SEEDBYTES ],
        const polyvec5 *t1)
{
    unsigned int i;

    memmove(pk, rho, DIL_SEEDBYTES);
    pk += DIL_SEEDBYTES;

    for (i = 0; i < 5; ++i) {
        polyt1_pack(pk +i *DIL_POLYT1_PACKEDBYTES, &(t1->vec[i]));
    }
}


/*------------------------------------*/
static void
unpack_pk5(unsigned char rho[ DIL_SEEDBYTES ],
                polyvec5 *t1,
     const unsigned char pk[ DIL_PUB5x4_BYTES ])
{
    unsigned int i;

    memmove(rho, pk, DIL_SEEDBYTES);
    pk += DIL_SEEDBYTES;

    for (i = 0; i < 5; ++i) {
        polyt1_unpack(&(t1->vec[i]), pk +i *DIL_POLYT1_PACKEDBYTES);
    }
}


/*------------------------------------*/
static void
polyvec4_pointwise_acc_montgomery(poly *w,
                        const polyvec4 *u,
                        const polyvec4 *v)
{
    unsigned int i;
    poly tmp;

    poly_pointwise_montgomery(w, &(u->vec[0]), &(v->vec[0]));

    for (i = 1; i < 4; ++i) {
        poly_pointwise_montgomery(&tmp, &(u->vec[i]), &(v->vec[i]));
        poly_add(w, w, &tmp);
    }
}


/*------------------------------------*/
static void
pack_prv5(unsigned char prv[ DIL_PRV5x4_BYTES ],
    const unsigned char rho[ DIL_SEEDBYTES ],
    const unsigned char key[ DIL_SEEDBYTES ],
    const unsigned char tr [ DIL_CRHBYTES ],
         const polyvec4 *s1,
         const polyvec5 *s2,
         const polyvec5 *t0)
{
    unsigned int i;

    memmove(prv, rho, DIL_SEEDBYTES);
    prv += DIL_SEEDBYTES;

    memmove(prv, key, DIL_SEEDBYTES);
    prv += DIL_SEEDBYTES;

    memmove(prv, tr,  DIL_CRHBYTES);
    prv += DIL_CRHBYTES;

    for (i = 0; i < 4; ++i) {
        polyeta_pack(prv +i *DIL_POLYETA5x4_PACKEDBYTES, &(s1->vec[i]),
                     5 /* eta(k=5) */);
    }
    prv += 4 * DIL_POLYETA5x4_PACKEDBYTES;  /*L*/

    for (i = 0; i < 5; ++i) {
        polyeta_pack(prv +i *DIL_POLYETA5x4_PACKEDBYTES, &(s2->vec[i]),                      5 /* eta(k=5) */);
    }
    prv += 5 * DIL_POLYETA5x4_PACKEDBYTES;  /*K*/

    for (i = 0; i < 5; ++i) {
        polyt0_pack(prv +i *DIL_POLYT0_PACKEDBYTES, &(t0->vec[i]));
    }
}


/*------------------------------------*/
static void
unpack_prv5(unsigned char rho[ DIL_SEEDBYTES ],
            unsigned char key[ DIL_SEEDBYTES ],
            unsigned char tr [ DIL_CRHBYTES ],
                 polyvec4 *s1,
                 polyvec5 *s2,
                 polyvec5 *t0,
      const unsigned char prv[ DIL_PRV5x4_BYTES ])
{
    unsigned int i;

    memmove(rho, prv, DIL_SEEDBYTES);
    prv += DIL_SEEDBYTES;

    memmove(key, prv, DIL_SEEDBYTES);
    prv += DIL_SEEDBYTES;

    memmove(tr, prv,  DIL_CRHBYTES);
    prv += DIL_CRHBYTES;

    for (i = 0; i < 4; ++i) {
        polyeta_unpack(&( s1->vec[i] ),
                       prv +i *DIL_POLYETA5x4_PACKEDBYTES, 5);
    }
    prv += 4 * DIL_POLYETA5x4_PACKEDBYTES;  /*L*/

    for (i = 0; i < 5; ++i) {
        polyeta_unpack(&( s2->vec[i] ),
                       prv +i *DIL_POLYETA5x4_PACKEDBYTES, 5);
    }
    prv += 5 * DIL_POLYETA5x4_PACKEDBYTES;  /*K*/

    for (i = 0; i < 5; ++i) {
        polyt0_unpack(&( t0->vec[i] ), prv +i *DIL_POLYT0_PACKEDBYTES);
    }
}


/*------------------------------------*/
static void
expand_matrix_6x5(polyvec_max mat[ 6 ], const unsigned char rho[ DIL_SEEDBYTES ])
{
    unsigned int k, l;

    for (k = 0; k < 6; ++k) {
        for (l = 0; l < 5; ++l) {
            poly_uniform(&( mat[k].vec[l] ), rho, (k <<8) +l);
        }
    }
}


/*------------------------------------*/
static unsigned int
polyvec6_make_hint(polyvec6 *h,
             const polyvec6 *v0,
             const polyvec6 *v1)
{
    unsigned int i, nr = 0;

    for (i = 0; i < 6; ++i) {
        nr += poly_make_hint(&(  h->vec[i] ),
                             &( v0->vec[i] ),
                             &( v1->vec[i] ));
    }

    return nr;
}


/*------------------------------------*/
static void
polyvec6_use_hint(polyvec6 *r,
            const polyvec6 *u,
            const polyvec6 *v)
{
    unsigned int i;

    for (i = 0; i < 6; ++i) {
        poly_use_hint(&( r->vec[i] ),
                      &( u->vec[i] ),
                      &( v->vec[i] ));
    }
}


/*------------------------------------*/
static void
polyvec6_decompose(polyvec6 *v1,
                   polyvec6 *v0,
             const polyvec6 *v)
{
    unsigned int i;

    for (i = 0; i < 6; ++i) {
        poly_decompose(&( v1->vec[i] ),
                       &( v0->vec[i] ),
                       &(  v->vec[i] ));
    }
}


/*------------------------------------*/
static void
polyvec6_power2round(polyvec6 *v1,
                     polyvec6 *v0,
               const polyvec6 *v)
{
    unsigned int i;

    for (i = 0; i < 6; ++i) {
        poly_power2round(&( v1->vec[i] ),
                         &( v0->vec[i] ),
                         &(  v->vec[i] ));
    }
}


/*------------------------------------*/
static void
polyvec6_shiftl(polyvec6 *v)
{
    unsigned int i;

    for (i = 0; i < 6; ++i) {
        poly_shiftl(&( v->vec[i] ));
    }
}


/*------------------------------------*/
static void
polyvec6_sub(polyvec6 *r, const polyvec6 *u, const polyvec6 *v)
{
    unsigned int i;

    for (i = 0; i < 6; ++i) {
        poly_sub(&( r->vec[i] ),
                 &( u->vec[i] ),
                 &( v->vec[i] ));
    }
}


/*------------------------------------*/
static void
polyvec6_reduce(polyvec6 *v)
{
    unsigned int i;

    for (i = 0; i < 6; ++i) {
        poly_reduce(&( v->vec[i] ));
    }
}


/*------------------------------------*/
static void
polyvec6_csubq(polyvec6 *v)
{
    unsigned int i;

    for (i = 0; i < 6; ++i) {
        poly_csubq(&( v->vec[i] ));
    }
}


/*------------------------------------*/
static void
polyvec6_invntt_tomont(polyvec6 *v)
{
    unsigned int i;

    for (i = 0; i < 6; ++i) {
        poly_invntt_tomont(&( v->vec[i] ));
    }
}


/*------------------------------------*/
static void
pack_pk6(unsigned char pk [ DIL_PUB6x5_BYTES ],
   const unsigned char rho[ DIL_SEEDBYTES ],
        const polyvec6 *t1)
{
    unsigned int i;

    memmove(pk, rho, DIL_SEEDBYTES);
    pk += DIL_SEEDBYTES;

    for (i = 0; i < 6; ++i) {
        polyt1_pack(pk +i *DIL_POLYT1_PACKEDBYTES, &(t1->vec[i]));
    }
}


/*------------------------------------*/
static void
unpack_pk6(unsigned char rho[ DIL_SEEDBYTES ],
                polyvec6 *t1,
     const unsigned char pk[ DIL_PUB6x5_BYTES ])
{
    unsigned int i;

    memmove(rho, pk, DIL_SEEDBYTES);
    pk += DIL_SEEDBYTES;

    for (i = 0; i < 6; ++i) {
        polyt1_unpack(&(t1->vec[i]), pk +i *DIL_POLYT1_PACKEDBYTES);
    }
}


/*------------------------------------*/
static void
polyvec5_pointwise_acc_montgomery(poly *w,
                        const polyvec5 *u,
                        const polyvec5 *v)
{
    unsigned int i;
    poly tmp;

    poly_pointwise_montgomery(w, &(u->vec[0]), &(v->vec[0]));

    for (i = 1; i < 5; ++i) {
        poly_pointwise_montgomery(&tmp, &(u->vec[i]), &(v->vec[i]));
        poly_add(w, w, &tmp);
    }
}


/*------------------------------------*/
static void
pack_prv6(unsigned char prv[ DIL_PRV6x5_BYTES ],
    const unsigned char rho[ DIL_SEEDBYTES ],
    const unsigned char key[ DIL_SEEDBYTES ],
    const unsigned char tr [ DIL_CRHBYTES ],
         const polyvec5 *s1,
         const polyvec6 *s2,
         const polyvec6 *t0)
{
    unsigned int i;

    memmove(prv, rho, DIL_SEEDBYTES);
    prv += DIL_SEEDBYTES;

    memmove(prv, key, DIL_SEEDBYTES);
    prv += DIL_SEEDBYTES;

    memmove(prv, tr,  DIL_CRHBYTES);
    prv += DIL_CRHBYTES;

    for (i = 0; i < 5; ++i) {
        polyeta_pack(prv +i *DIL_POLYETA6x5_PACKEDBYTES, &(s1->vec[i]),
                     3 /* eta(k=6) */);
    }
    prv += 5 * DIL_POLYETA6x5_PACKEDBYTES;  /*L*/

    for (i = 0; i < 6; ++i) {
        polyeta_pack(prv +i *DIL_POLYETA6x5_PACKEDBYTES, &(s2->vec[i]),                      3 /* eta(k=6) */);
    }
    prv += 6 * DIL_POLYETA6x5_PACKEDBYTES;  /*K*/

    for (i = 0; i < 6; ++i) {
        polyt0_pack(prv +i *DIL_POLYT0_PACKEDBYTES, &(t0->vec[i]));
    }
}


/*------------------------------------*/
static void
unpack_prv6(unsigned char rho[ DIL_SEEDBYTES ],
            unsigned char key[ DIL_SEEDBYTES ],
            unsigned char tr [ DIL_CRHBYTES ],
                 polyvec5 *s1,
                 polyvec6 *s2,
                 polyvec6 *t0,
      const unsigned char prv[ DIL_PRV6x5_BYTES ])
{
    unsigned int i;

    memmove(rho, prv, DIL_SEEDBYTES);
    prv += DIL_SEEDBYTES;

    memmove(key, prv, DIL_SEEDBYTES);
    prv += DIL_SEEDBYTES;

    memmove(tr, prv,  DIL_CRHBYTES);
    prv += DIL_CRHBYTES;

    for (i = 0; i < 5; ++i) {
        polyeta_unpack(&( s1->vec[i] ),
                       prv +i *DIL_POLYETA6x5_PACKEDBYTES, 3);
    }
    prv += 5 * DIL_POLYETA6x5_PACKEDBYTES;  /*L*/

    for (i = 0; i < 6; ++i) {
        polyeta_unpack(&( s2->vec[i] ),
                       prv +i *DIL_POLYETA6x5_PACKEDBYTES, 3);
    }
    prv += 6 * DIL_POLYETA6x5_PACKEDBYTES;  /*K*/

    for (i = 0; i < 6; ++i) {
        polyt0_unpack(&( t0->vec[i] ), prv +i *DIL_POLYT0_PACKEDBYTES);
    }
}


/*------------------------------------*/
static void
expand_matrix_8x7(polyvec_max mat[ 8 ], const unsigned char rho[ DIL_SEEDBYTES ])
{
    unsigned int k, l;

    for (k = 0; k < 8; ++k) {
        for (l = 0; l < 7; ++l) {
            poly_uniform(&( mat[k].vec[l] ), rho, (k <<8) +l);
        }
    }
}


/*------------------------------------*/
static unsigned int
polyvec8_make_hint(polyvec8 *h,
             const polyvec8 *v0,
             const polyvec8 *v1)
{
    unsigned int i, nr = 0;

    for (i = 0; i < 8; ++i) {
        nr += poly_make_hint(&(  h->vec[i] ),
                             &( v0->vec[i] ),
                             &( v1->vec[i] ));
    }

    return nr;
}


/*------------------------------------*/
static void
polyvec8_use_hint(polyvec8 *r,
            const polyvec8 *u,
            const polyvec8 *v)
{
    unsigned int i;

    for (i = 0; i < 8; ++i) {
        poly_use_hint(&( r->vec[i] ),
                      &( u->vec[i] ),
                      &( v->vec[i] ));
    }
}


/*------------------------------------*/
static void
polyvec8_decompose(polyvec8 *v1,
                   polyvec8 *v0,
             const polyvec8 *v)
{
    unsigned int i;

    for (i = 0; i < 8; ++i) {
        poly_decompose(&( v1->vec[i] ),
                       &( v0->vec[i] ),
                       &(  v->vec[i] ));
    }
}


/*------------------------------------*/
static void
polyvec8_power2round(polyvec8 *v1,
                     polyvec8 *v0,
               const polyvec8 *v)
{
    unsigned int i;

    for (i = 0; i < 8; ++i) {
        poly_power2round(&( v1->vec[i] ),
                         &( v0->vec[i] ),
                         &(  v->vec[i] ));
    }
}


/*------------------------------------*/
static void
polyvec8_shiftl(polyvec8 *v)
{
    unsigned int i;

    for (i = 0; i < 8; ++i) {
        poly_shiftl(&( v->vec[i] ));
    }
}


/*------------------------------------*/
static void
polyvec8_sub(polyvec8 *r, const polyvec8 *u, const polyvec8 *v)
{
    unsigned int i;

    for (i = 0; i < 8; ++i) {
        poly_sub(&( r->vec[i] ),
                 &( u->vec[i] ),
                 &( v->vec[i] ));
    }
}


/*------------------------------------*/
static void
polyvec8_reduce(polyvec8 *v)
{
    unsigned int i;

    for (i = 0; i < 8; ++i) {
        poly_reduce(&( v->vec[i] ));
    }
}


/*------------------------------------*/
static void
polyvec8_csubq(polyvec8 *v)
{
    unsigned int i;

    for (i = 0; i < 8; ++i) {
        poly_csubq(&( v->vec[i] ));
    }
}


/*------------------------------------*/
static void
polyvec8_invntt_tomont(polyvec8 *v)
{
    unsigned int i;

    for (i = 0; i < 8; ++i) {
        poly_invntt_tomont(&( v->vec[i] ));
    }
}


/*------------------------------------*/
static void
pack_pk8(unsigned char pk [ DIL_PUB8x7_BYTES ],
   const unsigned char rho[ DIL_SEEDBYTES ],
        const polyvec8 *t1)
{
    unsigned int i;

    memmove(pk, rho, DIL_SEEDBYTES);
    pk += DIL_SEEDBYTES;

    for (i = 0; i < 8; ++i) {
        polyt1_pack(pk +i *DIL_POLYT1_PACKEDBYTES, &(t1->vec[i]));
    }
}


/*------------------------------------*/
static void
unpack_pk8(unsigned char rho[ DIL_SEEDBYTES ],
                polyvec8 *t1,
     const unsigned char pk[ DIL_PUB8x7_BYTES ])
{
    unsigned int i;

    memmove(rho, pk, DIL_SEEDBYTES);
    pk += DIL_SEEDBYTES;

    for (i = 0; i < 8; ++i) {
        polyt1_unpack(&(t1->vec[i]), pk +i *DIL_POLYT1_PACKEDBYTES);
    }
}


/*------------------------------------*/
static void
polyvec7_pointwise_acc_montgomery(poly *w,
                        const polyvec7 *u,
                        const polyvec7 *v)
{
    unsigned int i;
    poly tmp;

    poly_pointwise_montgomery(w, &(u->vec[0]), &(v->vec[0]));

    for (i = 1; i < 7; ++i) {
        poly_pointwise_montgomery(&tmp, &(u->vec[i]), &(v->vec[i]));
        poly_add(w, w, &tmp);
    }
}


/*------------------------------------*/
static void
pack_prv8(unsigned char prv[ DIL_PRV8x7_BYTES ],
    const unsigned char rho[ DIL_SEEDBYTES ],
    const unsigned char key[ DIL_SEEDBYTES ],
    const unsigned char tr [ DIL_CRHBYTES ],
         const polyvec7 *s1,
         const polyvec8 *s2,
         const polyvec8 *t0)
{
    unsigned int i;

    memmove(prv, rho, DIL_SEEDBYTES);
    prv += DIL_SEEDBYTES;

    memmove(prv, key, DIL_SEEDBYTES);
    prv += DIL_SEEDBYTES;

    memmove(prv, tr,  DIL_CRHBYTES);
    prv += DIL_CRHBYTES;

    for (i = 0; i < 7; ++i) {
        polyeta_pack(prv +i *DIL_POLYETA8x7_PACKEDBYTES, &(s1->vec[i]),
                     2 /* eta(k=8) */);
    }
    prv += 7 * DIL_POLYETA8x7_PACKEDBYTES;  /*L*/

    for (i = 0; i < 8; ++i) {
        polyeta_pack(prv +i *DIL_POLYETA8x7_PACKEDBYTES, &(s2->vec[i]),                      2 /* eta(k=8) */);
    }
    prv += 8 * DIL_POLYETA8x7_PACKEDBYTES;  /*K*/

    for (i = 0; i < 8; ++i) {
        polyt0_pack(prv +i *DIL_POLYT0_PACKEDBYTES, &(t0->vec[i]));
    }
}


/*------------------------------------*/
static void
unpack_prv8(unsigned char rho[ DIL_SEEDBYTES ],
            unsigned char key[ DIL_SEEDBYTES ],
            unsigned char tr [ DIL_CRHBYTES ],
                 polyvec7 *s1,
                 polyvec8 *s2,
                 polyvec8 *t0,
      const unsigned char prv[ DIL_PRV8x7_BYTES ])
{
    unsigned int i;

    memmove(rho, prv, DIL_SEEDBYTES);
    prv += DIL_SEEDBYTES;

    memmove(key, prv, DIL_SEEDBYTES);
    prv += DIL_SEEDBYTES;

    memmove(tr, prv,  DIL_CRHBYTES);
    prv += DIL_CRHBYTES;

    for (i = 0; i < 7; ++i) {
        polyeta_unpack(&( s1->vec[i] ),
                       prv +i *DIL_POLYETA8x7_PACKEDBYTES, 2);
    }
    prv += 7 * DIL_POLYETA8x7_PACKEDBYTES;  /*L*/

    for (i = 0; i < 8; ++i) {
        polyeta_unpack(&( s2->vec[i] ),
                       prv +i *DIL_POLYETA8x7_PACKEDBYTES, 2);
    }
    prv += 8 * DIL_POLYETA8x7_PACKEDBYTES;  /*K*/

    for (i = 0; i < 8; ++i) {
        polyt0_unpack(&( t0->vec[i] ), prv +i *DIL_POLYT0_PACKEDBYTES);
    }
}


/* /generated code */
