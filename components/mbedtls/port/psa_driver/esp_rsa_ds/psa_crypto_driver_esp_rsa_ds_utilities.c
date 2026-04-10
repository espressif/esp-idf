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
    if (ilen < MIN_V15_PADDING_LEN) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    unsigned char bad = 0;
    size_t msg_len = 0;
    size_t msg_max_len = 0;
    unsigned char pad_done = 0;
    size_t pad_count = 0;

    msg_max_len = (output_max_len > ilen - MIN_V15_PADDING_LEN) ? ilen - MIN_V15_PADDING_LEN : output_max_len;

    /* Check the first byte (0x00) */
    bad |= input[0];

    /* Check the padding type */
    bad |= input[1] ^ 2; // MBEDTLS_RSA_CRYPT;

    /* Scan for separator (0x00) and count padding bytes in constant time */
    for (size_t i = 2; i < ilen; i++) {
        unsigned char found = (input[i] == 0x00);
        pad_done = pad_done | found;
        pad_count += (pad_done == 0) ? 1 : 0;
    }

    /* Check if we found a separator and padding is long enough */
    bad |= (pad_done == 0);  /* No separator found */
    bad |= (pad_count < 8);  /* Padding too short (need at least 8 non-zero bytes) */

    /* Calculate message length */
    msg_len = ilen - pad_count - 3;

    /* Check if separator is not at the very end */
    bad |= (msg_len > output_max_len);
    if (bad) {
        msg_len = msg_max_len;
    }

    /* Verify padding bytes are non-zero in constant time */
#if defined(__clang__) && defined(__xtensa__)
    #pragma clang loop vectorize(disable)
#endif
    for (size_t i = 2; i < ilen; i++) {
        unsigned char in_padding = (i < pad_count + 2);
        unsigned char is_zero = (input[i] == 0x00);
        bad |= in_padding & is_zero;
    }

    if (bad) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    *olen = msg_len;
    if (*olen > 0) {
        memcpy(output, input + ilen - msg_len, msg_len);
    }
    return PSA_SUCCESS;
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
    unsigned int hlen = PSA_HASH_LENGTH(hash_alg);
    unsigned char bad = 0;
    size_t msg_len = 0;

    /* Validate input length */
    bad |= (ilen < 2 * hlen + 2);

    /* Apply MGF masks */
    bad |= esp_rsa_ds_mgf_mask(input + 1, hlen, input + hlen + 1, ilen - hlen - 1, hash_alg) != PSA_SUCCESS;

    bad |= esp_rsa_ds_mgf_mask(input + hlen + 1, ilen - hlen - 1, input + 1, hlen, hash_alg) != PSA_SUCCESS;

    /* Check first byte (should be 0x00) */
    bad |= input[0];

    /* Skip the first byte and maskSeed */
    unsigned char *db = input + 1 + hlen;
    size_t db_len = ilen - hlen - 1;

    /* Compute hash, label is NULL and label_len is 0 */
    unsigned char lhash[PSA_HASH_MAX_SIZE];
    memset(lhash, 0, sizeof(lhash));
    size_t lhen = 0;
    bad |= psa_hash_compute(hash_alg, NULL, 0, lhash, sizeof(lhash), &lhen) != PSA_SUCCESS;

    bad |= (lhen != hlen);

    /* Verify hash portion of db against lhash */
    for (size_t i = 0; i < hlen && i < db_len; i++) {
        bad |= db[i] ^ lhash[i];
    }

    /* Skip past lhash in DB */
    unsigned char *p = db + hlen;
    size_t remaining = db_len - hlen;

    /*
     * Scan PS || 0x01 || M
     */
    unsigned char seen_one = 0;
    size_t msg_index = 0;

    for (size_t i = 0; i < remaining; i++) {
        unsigned char is_zero = (p[i] == 0);
        unsigned char is_one  = (p[i] == 1);

        /* Before delimiter, only 0x00 allowed */
        bad |= (seen_one == 0) & !(is_zero | is_one);

        /* Record first 0x01 */
        msg_index |= (seen_one == 0 && is_one) * i;
        seen_one  |= is_one;
    }

    /* Must see exactly one delimiter */
    bad |= (seen_one == 0);

    /* Calculate message length */
    msg_len = remaining - msg_index - 1;
    bad |= (msg_len == 0);

    /* Check output buffer size */
    bad |= (output_max_len < msg_len);

    if (bad) {
        *olen = 0;
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    /* Copy message in constant time */
    *olen = msg_len;
    if (*olen > 0) {
        memcpy(output, p + msg_index + 1, msg_len);
    }

    return PSA_SUCCESS;
}
#endif /* CONFIG_MBEDTLS_SSL_PROTO_TLS1_3 */
