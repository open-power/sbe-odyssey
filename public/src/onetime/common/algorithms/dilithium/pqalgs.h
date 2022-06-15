/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/onetime/common/algorithms/dilithium/pqalgs.h $     */
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


#ifndef PQALGS_H
#define PQALGS_H

/*--------------------------------------
 * Generate keypair.
 *
 * (algid, ibytes)  selects the key algorithm; must point to a suitable
 *                  object identifier (OID).
 *
 * (prv, prvbytes)  contains the private key, at the start, upon success
 * (pub, *pubbytes) contains the public key, at the start, upon success;
 *                  *pubbytes contains the available size when calling.
 *
 * Returns number of bytes written to start of (prv, prvbytes); size
 * query with NULL 'prv'.
 *
 * The returned keys are in a provider-specific, possibly nonstandard
 * format.  See pqca_key2wire() for standardized serialization.
 */
int pqca_generate(unsigned char *prv,   size_t prvbytes,
                  unsigned char *pub,   size_t *pubbytes,
            const unsigned char *algid, size_t ibytes) ;


/*--------------------------------------
 * Sign data: generates signature for (msg, mbytes) as message, using
 * private key (prv, pbytes).
 *
 * Returns number of bytes written to start of (sig, sbytes); size
 * query with NULL 'sig'.  Returns <0 for failure.
 *
 * Key has been returned by an earlier call to pqcr_generate().
 *
 * (algid, ibytes)  selects the key algorithm.  If (NULL, 0), a
 * key(type)-specific default is selected; see algorithm-specific definitions.
 */
int pqca_sign(unsigned char *sig,   size_t sbytes,
        const unsigned char *msg,   size_t mbytes,
        const unsigned char *prv,   size_t pbytes,
        const unsigned char *algid, size_t ibytes) ;


/*--------------------------------------
 * Verify signature: validates signature (sig, sbytes) corresponding to
 * (msg, mbytes), using the public key (pub, pbytes).
 *
 * Returns >0  if signature has been verified
 *         0   if signature is invalid
 *         <0  other errors, such as invalid key or mode
 *
 * Public key has been returned by an earlier call to pqcr_generate().
 *
 * (algid, ibytes)  selects the key algorithm.  If (NULL, 0), a
 * key(type)-specific default is selected; see algorithm-specific definitions.
 */
int pqca_verify(const unsigned char *sig,   size_t sbytes,
                const unsigned char *msg,   size_t mbytes,
                const unsigned char *pub,   size_t pbytes,
                const unsigned char *algid, size_t ibytes) ;


/*--------------------------------------
 * Serialize key: encode [possibly] provider-internal structure to
 * standardized PKCS#8 (private key) or SPKI (public keys).
 *
 * Returns number of bytes written to start of (wire, wbytes); size
 * query with NULL 'wire'.
 *
 * (algid, ibytes) selects encoding algorithm.  If (NULL, 0), a key(type)-
 * specific default is selected; see algorithm-specific definitions.
 *
 * See also: pqca_wire2key()
 */
int pqca_key2wire(unsigned char *wire,  size_t wbytes,
            const unsigned char *key,   size_t kbytes,
            const unsigned char *algid, size_t ibytes) ;


/*--------------------------------------
 * Decode key: import a standardized structure into a [possibly]
 * provider-internal format.
 *
 * Returns number of bytes written to start of (key, kbytes); size
 * query with NULL 'key'.
 *
 * (algid, ibytes) forces use of a specific the serialization algorithm.
 * With NULL 'algid' or 0 'ibytes', all context is derived from the
 * self-describing standardized structure.
 *
 * See also: pqca_key2wire()
 */
int pqca_wire2key(unsigned char *key,   size_t kbytes,
            const unsigned char *wire,  size_t wbytes,
            const unsigned char *algid, size_t ibytes) ;


/*-----  extension notes  ----------------------------------------------------
 * As an alternative to object identifiers (OIDs), an append-only
 * list for algorithm/size/etc. selectors have been defined; see
 * PQCA_ID_t for a full list. These constants must be supplied as
 * (NULL, ...constant...) instead of a non-NULL OID, or (NULL, 0)
 * where the latter implies defaults.
 *
 * Implementations MAY use handles instead of raw key structures;
 * the API is not expected to change for such indirection-addresses
 * providers.
 */

typedef enum {
		/* all reserved values are >0
		 * mapped values have some internal structure, do
		 * not change them
		 */

		/* round 2 Dilithium, NIST strength categories,
		 * implies IBM-specified private+public key formats when
 		 * used in serialization context.
		 */
	PQCA_ID_DIL3_R2 = 0x0105,
	PQCA_ID_DIL4_R2 = 0x0106,
	PQCA_ID_DIL5_R2 = 0x0108,

		/* round 2 Dilithium, NIST strength categories,
		 * implies ref.impl-derived, 'raw' private+public keys when
 		 * used in serialization context, such as generated/used
		 * by liboqs.
		 *
		 * when used as key(generate) or signature type,
		 * DIL<n>_R2_RAW is identical to the corresponding DIL<n>_R2.
		 */
	PQCA_ID_DIL3_R2_RAW = 0x0205,
	PQCA_ID_DIL4_R2_RAW = 0x0206,
	PQCA_ID_DIL5_R2_RAW = 0x0208,

		/* round 3 Dilithium, NIST strength categories,
		 * compressed round 3 signatures
		 *
		 * when used as key type, DIL<n>_R2 is identical
		 * to the corresponding DIL<n>_R2.
		 */
	PQCA_ID_DIL3_R3 = 0x0305,
	PQCA_ID_DIL4_R3 = 0x0306,
	PQCA_ID_DIL5_R3 = 0x0308,

	PQCA_ID_MAX = PQCA_ID_DIL5_R3
} PQCA_ID_t ;


/*-----  error codes  ----------------*/
typedef enum {
	PQCA_EPARAM    = -1,  /* missing/NULL parameter; non-NULL expected */
	PQCA_ESTRENGTH = -2,  /* parameters are strength/policy-restricted */
	PQCA_ESTRUCT   = -3,  /* key(structure) is not recognized */
	PQCA_EKEYTYPE  = -4,  /* object ID/key-object type not recognized */
	PQCA_EKEYMODE  = -5,  /* key is incompatible with requested function */
	PQCA_EMODE     = -6,  /* operation incompatible with requested
	                         function/mode */
	PQCA_ETOOSMALL = -7,  /* insufficient output buffer */
	PQCA_ERNG      = -8,  /* call to random-number generator failed */
	PQCA_EINTERN   = -9   /* CSP internal consistency error */
} PQCA_ERR_t ;

/* TODO: defines for raw-binary OIDs */

#endif
