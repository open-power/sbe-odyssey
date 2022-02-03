/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/common/algorithms/common-base.h $                  */
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
/*----------------------------------------------------------------------
 * (C) COPYRIGHT INTERNATIONAL BUSINESS MACHINES CORPORATION 2005--2020
 *                        ALL RIGHTS RESERVED
 *                       IBM Research - Zurich
 *----------------------------------------------------------------------
 *  Author: Visegrady, Tamas (tvi@zurich.ibm.com)
 *----------------------------------------------------------------------*/

/* shared macros for public distribution; no typedefs
 *
 * this file may define static-inline functions, if environment permits
 * (these operate effectively as macros, under recent gcc/xlc/clang)
 *
 * note that this file uses identifiers containing double-underscores
 * (but not starting with them). Such identifiers are reserved in C++
 * (ISO/IEC 14882:2011, 17.6.4.3.2, Global names), but not in C (ISO/IEC
 * 9899:2011, 7.1.3, Reserved identifiers). You MAY need to convince
 * your compiler to prevent compilation as C++.
 */

#if !defined(COMMON_BASE_H__)
#define COMMON_BASE_H__ 1

/* note: a minimized version used within IBM prod environments */


#define  ARRAY_ELEMS(arr)  (sizeof(arr) / sizeof((arr)[0]))


#if (__GNUC__ >= 3)       /* note: pre-gcc3 support is basically irrelevant */
#define ATTR_PURE__     __attribute__ ((pure))
#define ATTR_CONST__    __attribute__ ((const))

#else
#define ATTR_PURE__     /**/
#define ATTR_CONST__    /**/
#endif      /* gcc >= 3 */


/*--------------------------------------
 * endianness-conversion 'macros'
 * assuming ntohl() etc. are real functions, not static-inline macros
 */

static inline uint32_t MSBF4_READ(const void *p)
{
        const unsigned char *pb = (const unsigned char *) p;

        return ((((uint32_t)  (pb)[3])       ) |
                (((uint32_t) ((pb)[2])) <<  8) |
                (((uint32_t) ((pb)[1])) << 16) |
                (((uint32_t) ((pb)[0])) << 24)) ;
}


/*--------------------------------------
 * assume this gets inlined, possibly through bswap() or equivalent
 * recent gcc/clang and some xlc's tend to do so
 */
static inline void MSBF8_WRITE(void *p, uint64_t v)
{
        unsigned char *pb = (unsigned char *) p;

        pb[7] = (unsigned char)  v;
        pb[6] = (unsigned char) (v >> 8);
        pb[5] = (unsigned char) (v >>16);
        pb[4] = (unsigned char) (v >>24);
        pb[3] = (unsigned char) (v >>32);
        pb[2] = (unsigned char) (v >>40);
        pb[1] = (unsigned char) (v >>48);
        pb[0] = (unsigned char) (v >>56);
}

#ifdef STANDALONE
//--------------------------------------
static inline
void cu_hexprint(const char *prefix, const void *data, size_t dbytes)
{
	const unsigned char *b = (const unsigned char *) data;

	if ((NULL == b) || (0 == dbytes) || (b+dbytes < b))
		return;
	if (NULL != prefix)
		printf("%s", prefix);

	while (0 < dbytes--)
		printf("%02x", *(b++));

	if (NULL != prefix)
		printf("\n");
	fflush(stdout);
}
#endif

#endif      /* !defined(COMMON_BASE_H__) */
