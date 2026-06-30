/*
 * SPDX-FileCopyrightText: 2025-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include "include/psa_crypto_driver_esp_rsa_ds_utilities.h"
#include "mbedtls/asn1.h"
#include "mbedtls/psa_util.h"
#include "esp_log.h"
#include "mbedtls/constant_time.h"
#include "constant_time_internal.h"

typedef struct {
    psa_algorithm_t md_alg;
    const char *oid;
    size_t oid_len;
} oid_md_mapping_t;

static const oid_md_mapping_t oid_md_table[] = {
#if defined(PSA_WANT_ALG_MD5)
    { PSA_ALG_MD5, "\x2a\x86\x48\x86\xf7\x0d\x02\x05", 8 },
#endif
#if defined(PSA_WANT_ALG_SHA_1)
    { PSA_ALG_SHA_1, "\x2b\x0e\x03\x02\x1a", 5 },
#endif
#if defined(PSA_WANT_ALG_SHA_224)
    { PSA_ALG_SHA_224, "\x60\x86\x48\x01\x65\x03\x04\x02\x04", 9 },
#endif
#if defined(PSA_WANT_ALG_SHA_256)
    { PSA_ALG_SHA_256, "\x60\x86\x48\x01\x65\x03\x04\x02\x01", 9 },
#endif
#if defined(PSA_WANT_ALG_SHA_384)
    { PSA_ALG_SHA_384, "\x60\x86\x48\x01\x65\x03\x04\x02\x02", 9 },
#endif
#if defined(PSA_WANT_ALG_SHA_512)
    { PSA_ALG_SHA_512, "\x60\x86\x48\x01\x65\x03\x04\x02\x03", 9 },
#endif
#if defined(PSA_WANT_ALG_RIPEMD160)
    { PSA_ALG_RIPEMD160, "\x2b\x24\x03\x02\x01", 5 },
#endif
    { PSA_ALG_NONE, NULL, 0 }
};

static int esp_rsa_ds_get_oid_by_psa_alg(psa_algorithm_t md_alg, const char **oid, size_t *olen)
{
    for (size_t i = 0; oid_md_table[i].md_alg != PSA_ALG_NONE; i++) {
        if (oid_md_table[i].md_alg == md_alg) {
            *oid = oid_md_table[i].oid;
            *olen = oid_md_table[i].oid_len;
            return 0;
        }
    }
    return PSA_ERROR_NOT_SUPPORTED;
}

psa_status_t esp_rsa_ds_pad_v15_encode(psa_algorithm_t alg, unsigned int hashlen,
                                            const unsigned char *hash,
                                            size_t dst_len,
                                            unsigned char *dst)
{
    size_t oid_size  = 0;
    size_t nb_pad    = dst_len;
    unsigned char *p = dst;
    const char *oid  = NULL;

    /* Are we signing hashed or raw data? */
    if (alg != PSA_ALG_NONE) {
        if (esp_rsa_ds_get_oid_by_psa_alg(alg, &oid, &oid_size) != PSA_SUCCESS) {
            return PSA_ERROR_INVALID_ARGUMENT;
        }

        hashlen = PSA_HASH_LENGTH(alg);

        /* Double-check that 8 + hashlen + oid_size can be used as a
        * 1-byte ASN.1 length encoding and that there's no overflow. */
        if ( 8 + hashlen + oid_size  >= 0x80         ||
                10 + hashlen            <  hashlen      ||
                10 + hashlen + oid_size <  10 + hashlen ) {
            return ( PSA_ERROR_INVALID_ARGUMENT );
        }

        /*
        * Static bounds check:
        * - Need 10 bytes for five tag-length pairs.
        *   (Insist on 1-byte length encodings to protect against variants of
        *    Bleichenbacher's forgery attack against lax PKCS#1v1.5 verification)
        * - Need hashlen bytes for hash
        * - Need oid_size bytes for hash alg OID.
        */
        if ( nb_pad < 10 + hashlen + oid_size ) {
            return ( PSA_ERROR_INVALID_ARGUMENT );
        }
        nb_pad -= 10 + hashlen + oid_size;
    } else {
        if ( nb_pad < hashlen ) {
            return ( PSA_ERROR_INVALID_ARGUMENT );
        }

        nb_pad -= hashlen;
    }

    /* Need space for signature header and padding delimiter (3 bytes),
     * and 8 bytes for the minimal padding */
    if ( nb_pad < 3 + 8 ) {
        return ( PSA_ERROR_INVALID_ARGUMENT );
    }
    nb_pad -= 3;

    /* Now nb_pad is the amount of memory to be filled
     * with padding, and at least 8 bytes long. */

    /* Write signature header and padding */
    *p++ = 0;
    *p++ = 1; //MBEDTLS_RSA_SIGN;
    memset( p, 0xFF, nb_pad );
    p += nb_pad;
    *p++ = 0;

    /* Are we signing raw data? */
    if ( alg == PSA_ALG_NONE ) {
        memcpy( p, hash, hashlen );
        return ( PSA_SUCCESS );
    }

    /* Signing hashed data, add corresponding ASN.1 structure
     *
     * DigestInfo ::= SEQUENCE {
     *   digestAlgorithm DigestAlgorithmIdentifier,
     *   digest Digest }
     * DigestAlgorithmIdentifier ::= AlgorithmIdentifier
     * Digest ::= OCTET STRING
     *
     * Schematic:
     * TAG-SEQ + LEN [ TAG-SEQ + LEN [ TAG-OID  + LEN [ OID  ]
     *                                 TAG-NULL + LEN [ NULL ] ]
     *                 TAG-OCTET + LEN [ HASH ] ]
     */

    *p++ = MBEDTLS_ASN1_SEQUENCE | MBEDTLS_ASN1_CONSTRUCTED;
    *p++ = (unsigned char)( 0x08 + oid_size + hashlen );
    *p++ = MBEDTLS_ASN1_SEQUENCE | MBEDTLS_ASN1_CONSTRUCTED;
    *p++ = (unsigned char)( 0x04 + oid_size );
    *p++ = MBEDTLS_ASN1_OID;
    *p++ = (unsigned char) oid_size;
    memcpy( p, oid, oid_size );
    p += oid_size;
    *p++ = MBEDTLS_ASN1_NULL;
    *p++ = 0x00;
    *p++ = MBEDTLS_ASN1_OCTET_STRING;
    *p++ = (unsigned char) hashlen;
    memcpy( p, hash, hashlen );
    p += hashlen;

    /* Just a sanity-check, should be automatic
     * after the initial bounds check. */
    if ( p != dst + dst_len ) {
        mbedtls_platform_zeroize( dst, dst_len );
        return ( PSA_ERROR_INVALID_ARGUMENT );
    }

    return PSA_SUCCESS;
}

psa_status_t esp_rsa_ds_pad_v15_unpad(unsigned char *input,
    size_t ilen,
    unsigned char *output,
    size_t output_max_len,
    size_t *olen)
{
    /* This implementation mirrors mbedtls_ct_rsaes_pkcs1_v15_unpadding()
     * in upstream rsa.c. Below the public-input length check, every
     * operation must be constant-time w.r.t. the plaintext contents,
     * the position of the 0x00 separator, and padding validity. Failing
     * that opens a Bleichenbacher-style padding oracle. The function is
     * deliberately longer than the project guideline because each step
     * carries an audit comment that has to survive intact. */

    /* ilen and output_max_len are public; this branch is safe. */
    if (ilen < MIN_V15_PADDING_LEN) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }
#if defined(MBEDTLS_PKCS1_V15) && defined(MBEDTLS_RSA_C)
    size_t pad_count = 0;
    size_t plaintext_size = 0;
    size_t plaintext_max_size;
    mbedtls_ct_condition_t bad;
    mbedtls_ct_condition_t pad_done;
    mbedtls_ct_condition_t output_too_large;

    plaintext_max_size = (output_max_len > ilen - MIN_V15_PADDING_LEN)
                            ? ilen - MIN_V15_PADDING_LEN : output_max_len;

    /* EME-PKCS1-v1_5: 0x00 || 0x02 || PS (>= 8 non-zero) || 0x00 || M */
    bad = mbedtls_ct_bool(input[0]);
    bad = mbedtls_ct_bool_or(bad, mbedtls_ct_uint_ne(input[1], 2 /* MBEDTLS_RSA_CRYPT */));

    /* Scan the full buffer; pad_done latches at the first 0x00 found. */
    pad_done = MBEDTLS_CT_FALSE;
    for (size_t i = 2; i < ilen; i++) {
        mbedtls_ct_condition_t found = mbedtls_ct_uint_eq(input[i], 0);
        pad_done   = mbedtls_ct_bool_or(pad_done, found);
        pad_count += mbedtls_ct_uint_if_else_0(mbedtls_ct_bool_not(pad_done), 1);
    }

    /* No separator found, or PS too short. */
    bad = mbedtls_ct_bool_or(bad, mbedtls_ct_bool_not(pad_done));
    bad = mbedtls_ct_bool_or(bad, mbedtls_ct_uint_gt(8, pad_count));

    /* If invalid, substitute plaintext_max_size so the remaining cache
     * and timing trace matches the good case. */
    plaintext_size = mbedtls_ct_uint_if(bad,
                                        (unsigned) plaintext_max_size,
                                        (unsigned) (ilen - pad_count - 3));
    output_too_large = mbedtls_ct_uint_gt(plaintext_size, plaintext_max_size);
    plaintext_size = mbedtls_ct_uint_if(output_too_large,
                                        (unsigned) plaintext_max_size,
                                        (unsigned) plaintext_size);

    /* On any failure path (bad padding, or plaintext doesn't fit) zero
     * the post-header region of `input` BEFORE the memmove_left + memcpy
     * below. Those two operations execute unconditionally to keep the
     * memory access trace fixed; this step ensures they propagate zeros
     * rather than a failed-decryption plaintext attempt into the
     * caller-visible output buffer. Mirrors upstream rsa.c. */
    mbedtls_ct_zeroize_if(mbedtls_ct_bool_or(bad, output_too_large),
                          input + MIN_V15_PADDING_LEN,
                          ilen - MIN_V15_PADDING_LEN);

    /* Slide the plaintext to a fixed in-buffer position, then read
     * from that fixed position. The slide is CT in the secret offset. */
    mbedtls_ct_memmove_left(input + ilen - plaintext_max_size,
                            plaintext_max_size,
                            plaintext_max_size - plaintext_size);
    if (output_max_len != 0) {
        /* memmove handles input/output aliasing (callers may pass the
         * same buffer for both). The length is the public bound, so
         * the access pattern reveals nothing secret. */
        memmove(output, input + ilen - plaintext_max_size, plaintext_max_size);
    }

    *olen = plaintext_size;

    /* Collapse both error conditions into the single status we already
     * return (PSA_ERROR_INVALID_ARGUMENT); distinguishing them would
     * give a Bleichenbacher attacker a finer oracle. */
    return (psa_status_t) mbedtls_ct_error_if_else_0(
                mbedtls_ct_bool_or(bad, output_too_large),
                PSA_ERROR_INVALID_ARGUMENT);
#else
    /* PKCS#1 v1.5 padding is not configured; the driver should not be
     * dispatched for this algorithm in the first place. */
    (void) input; (void) output; (void) output_max_len; (void) olen;
    return PSA_ERROR_NOT_SUPPORTED;
#endif /* MBEDTLS_PKCS1_V15 && MBEDTLS_RSA_C */
}

#if CONFIG_MBEDTLS_SSL_PROTO_TLS1_3
static psa_status_t esp_rsa_ds_mgf_mask(unsigned char *dst, size_t dlen, unsigned char *src,
                    size_t slen, psa_algorithm_t hash_alg)
{
    unsigned char counter[4];
    unsigned char *p;
    unsigned int hlen;
    size_t i, use_len;
    unsigned char mask[PSA_HASH_MAX_SIZE];
    psa_hash_operation_t hash_op = PSA_HASH_OPERATION_INIT;
    psa_status_t status = PSA_SUCCESS;

    hlen = PSA_HASH_LENGTH(hash_alg);

    memset(mask, 0, sizeof(mask));
    memset(counter, 0, 4);

    /* Generate and apply dbMask */
    p = dst;

    while (dlen > 0) {
        use_len = hlen;
        if (dlen < hlen) {
            use_len = dlen;
        }

        status = psa_hash_setup(&hash_op, hash_alg);
        if (status != PSA_SUCCESS) {
            goto exit;
        }
        status = psa_hash_update(&hash_op, src, slen);
        if (status != PSA_SUCCESS) {
            goto exit;
        }
        status = psa_hash_update(&hash_op, counter, 4);
        if (status != PSA_SUCCESS) {
            goto exit;
        }
        size_t out_len = 0;
        status = psa_hash_finish(&hash_op, mask, PSA_HASH_LENGTH(hash_alg), &out_len);
        if (status != PSA_SUCCESS) {
            goto exit;
        }

        for (i = 0; i < use_len; ++i) {
            *p++ ^= mask[i];
        }

        counter[3]++;

        dlen -= use_len;
    }

exit:
    psa_hash_abort(&hash_op);
    mbedtls_platform_zeroize(mask, sizeof(mask));

    return status;
}

static psa_status_t esp_rsa_ds_hash_mprime(const unsigned char *hash, size_t hlen,
                       const unsigned char *salt, size_t slen,
                       unsigned char *out, psa_algorithm_t hash_alg)
{
    const unsigned char zeros[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };

    psa_hash_operation_t hash_op = PSA_HASH_OPERATION_INIT;
    psa_status_t status = psa_hash_setup(&hash_op, hash_alg);
    if (status != PSA_SUCCESS) {
        goto exit;
    }
    status = psa_hash_update(&hash_op, zeros, sizeof(zeros));
    if (status != PSA_SUCCESS) {
        goto exit;
    }
    status = psa_hash_update(&hash_op, hash, hlen);
    if (status != PSA_SUCCESS) {
        goto exit;
    }
    status = psa_hash_update(&hash_op, salt, slen);
    if (status != PSA_SUCCESS) {
        goto exit;
    }
    size_t out_len = 0;
    status = psa_hash_finish(&hash_op, out, PSA_HASH_LENGTH(hash_alg), &out_len);
    if (status != PSA_SUCCESS) {
        goto exit;
    }
exit:
    psa_hash_abort(&hash_op);
    return status;
}

psa_status_t esp_rsa_ds_pad_v21_encode(psa_algorithm_t hash_alg,
                                            unsigned int hashlen,
                                            const unsigned char *hash,
                                            int saltlen,
                                            unsigned char *sig,
                                            size_t dst_len)
{
    size_t olen;
    unsigned char *p = sig;
    unsigned char *salt = NULL;
    size_t slen, min_slen, hlen, offset = 0;
    psa_status_t ret = PSA_ERROR_INVALID_ARGUMENT;
    size_t msb;

    if ((hash_alg != PSA_ALG_NONE || hashlen != 0) && hash == NULL) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    olen = dst_len;

    if (hash_alg != PSA_ALG_NONE) {
        /* Gather length of hash to sign */
        size_t exp_hashlen = PSA_HASH_LENGTH(hash_alg);
        if (exp_hashlen != hashlen) {
            return PSA_ERROR_INVALID_ARGUMENT;
        }
    }

    hlen = PSA_HASH_LENGTH(hash_alg);

    if (saltlen == -1) {
        /* Calculate the largest possible salt length, up to the hash size.
        * Normally this is the hash length, which is the maximum salt length
        * according to FIPS 185-4 �5.5 (e) and common practice. If there is not
        * enough room, use the maximum salt length that fits. The constraint is
        * that the hash length plus the salt length plus 2 bytes must be at most
        * the key length. This complies with FIPS 186-4 �5.5 (e) and RFC 8017
        * (PKCS#1 v2.2) �9.1.1 step 3. */
        min_slen = hlen - 2;
        if (olen < hlen + min_slen + 2) {
            return PSA_ERROR_INVALID_ARGUMENT;
        } else if (olen >= hlen + hlen + 2) {
            slen = hlen;
        } else {
            slen = olen - hlen - 2;
        }
    } else if ((saltlen < 0) || (saltlen + hlen + 2 > olen)) {
        return PSA_ERROR_INVALID_ARGUMENT;
    } else {
        slen = (size_t) saltlen;
    }

    memset(sig, 0, olen);

    /* Note: EMSA-PSS encoding is over the length of N - 1 bits */
    msb = dst_len * 8 - 1;
    p += olen - hlen - slen - 2;
    *p++ = 0x01;

    /* Generate salt of length slen in place in the encoded message */
    salt = p;
    if ((ret = mbedtls_psa_get_random(MBEDTLS_PSA_RANDOM_STATE, salt, slen)) != 0) {
        return PSA_ERROR_INSUFFICIENT_ENTROPY;
    }
    p += slen;

    /* Generate H = Hash( M' ) */
    ret = esp_rsa_ds_hash_mprime(hash, hashlen, salt, slen, p, hash_alg);
    if (ret != 0) {
        return ret;
    }

    /* Compensate for boundary condition when applying mask */
    if (msb % 8 == 0) {
        offset = 1;
    }

    /* maskedDB: Apply dbMask to DB */
    ret = esp_rsa_ds_mgf_mask(sig + offset, olen - hlen - 1 - offset, p, hlen, hash_alg);
    if (ret != 0) {
        return ret;
    }

    msb = dst_len * 8 - 1;
    sig[0] &= 0xFF >> (olen * 8 - msb);

    p += hlen;
    *p++ = 0xBC;
    return ret;
}

psa_status_t esp_rsa_ds_pad_oaep_unpad(unsigned char *input,
    size_t ilen,
    unsigned char *output,
    size_t output_max_len,
    size_t *olen,
    psa_algorithm_t hash_alg)
{
    /* This mirrors mbedtls_rsa_rsaes_oaep_decrypt() in upstream rsa.c.
     * Below the public-input sanity check, the unpadding scan operates
     * only through mbedtls_ct primitives, so its time and memory trace
     * depend solely on ilen and hash_alg. The single branch on `bad`
     * at the end leaks no more than the return value already does,
     * which matches the upstream design and is acceptable for OAEP
     * (the relevant side-channel attack relies on distinguishing
     * failure modes, not on timing the success path). */

    unsigned int hlen = PSA_HASH_LENGTH(hash_alg);
    mbedtls_ct_condition_t bad;
    mbedtls_ct_condition_t in_padding;
    size_t pad_len;
    unsigned char *p;
    unsigned char lhash[PSA_HASH_MAX_SIZE];

    /* All lengths checked here are public. */
    if (hlen == 0 || hlen > PSA_HASH_MAX_SIZE || ilen < 2 * hlen + 2) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    /* Unmask seed and DB. A failure here is an internal hash error,
     * not a ciphertext-dependent condition, so an early return is
     * safe. */
    if (esp_rsa_ds_mgf_mask(input + 1, hlen,
                            input + hlen + 1, ilen - hlen - 1, hash_alg) != PSA_SUCCESS ||
        esp_rsa_ds_mgf_mask(input + hlen + 1, ilen - hlen - 1,
                            input + 1, hlen, hash_alg) != PSA_SUCCESS) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    /* lHash of the empty label, recomputed each call. */
    size_t lhen = 0;
    if (psa_hash_compute(hash_alg, NULL, 0, lhash, sizeof(lhash), &lhen) != PSA_SUCCESS
            || lhen != hlen) {
        mbedtls_platform_zeroize(lhash, sizeof(lhash));
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    /* Constant-time padding check. */
    p   = input;
    bad = mbedtls_ct_bool(*p++);          /* First byte must be 0x00 */
    p  += hlen;                            /* Skip seed */
    bad = mbedtls_ct_bool_or(bad,
            mbedtls_ct_bool(mbedtls_ct_memcmp(lhash, p, hlen)));
    p  += hlen;

    /* Count leading zeros in DB (between lHash and the 0x01 delimiter).
     * The loop scans the full DB region every time; the latch via
     * in_padding keeps pad_len from incrementing once a non-zero byte
     * is seen. */
    pad_len    = 0;
    in_padding = MBEDTLS_CT_TRUE;
    for (size_t i = 0; i < ilen - 2 * hlen - 2; i++) {
        in_padding = mbedtls_ct_bool_and(in_padding, mbedtls_ct_uint_eq(p[i], 0));
        pad_len   += mbedtls_ct_uint_if_else_0(in_padding, 1);
    }
    p  += pad_len;
    bad = mbedtls_ct_bool_or(bad, mbedtls_ct_uint_ne(*p++, 0x01));

    mbedtls_platform_zeroize(lhash, sizeof(lhash));

    /* Single decision point on the accumulated bit. */
    if (bad != MBEDTLS_CT_FALSE) {
        *olen = 0;
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    /* Padding is valid; from here the plaintext length is no longer
     * secret (it is returned to the caller via *olen). */
    size_t plaintext_size = ilen - (size_t) (p - input);
    if (plaintext_size > output_max_len) {
        *olen = 0;
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    *olen = plaintext_size;
    if (plaintext_size != 0) {
        /* memmove (not memcpy): the driver's caller aliases input and
         * output to the same buffer, so the source range [p, p+plaintext_size)
         * overlaps the destination range [output, output+plaintext_size).
         * Both pointers and the length are public, so memmove's access
         * pattern stays fixed by public values -- CT property preserved. */
        memmove(output, p, plaintext_size);
    }
    return PSA_SUCCESS;
}
#endif /* CONFIG_MBEDTLS_SSL_PROTO_TLS1_3 */
