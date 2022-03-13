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
polyvec8_ntt(polyvec8 *v)
{
    unsigned int i;

    for (i = 0; i < 8; ++i) {
        poly_ntt256(&( v->vec[i] ));
    }
}

/*------------------------------------*/
static void
expand_matrix_8x7(polyvec_max *mat01,polyvec_max *mat234,polyvec_max *mat567, const unsigned char rho[ DIL_SEEDBYTES ])
{
    unsigned int k, l;

    for (k = 0; k < 8; ++k) {
        for (l = 0; l < 7; ++l) {
            if(k<2){
              poly_uniform(&( mat01[k].vec[l] ), rho, (k <<8) +l);
            }
            if((k>=2)&&(k<=4)){
              poly_uniform(&( mat234[k-2].vec[l] ), rho, (k <<8) +l);
            }
            if(k>=5){
              poly_uniform(&( mat567[k-5].vec[l] ), rho, (k <<8) +l);
            }
        }
    }
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

/* /generated code */
