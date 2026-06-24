/*
 * SPDX-FileCopyrightText: 2015-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "mbedtls/bignum.h"
#include "mbedtls/esp_mbedtls_random.h"

#include "utils/includes.h"
#include "utils/common.h"
#include "crypto.h"
#include "common/defs.h"
#include "tls/asn1.h"
#include "tls/rsa.h"
#include "tls/pkcs1.h"

#ifdef CONFIG_CRYPTO_MBEDTLS
// #include "mbedtls/entropy.h"
// #include "mbedtls/ctr_drbg.h"

#include <mbedtls/error.h>
#include <mbedtls/x509_crt.h>
#include <mbedtls/platform.h>
// #include <mbedtls/sha256.h>

#include "psa/crypto.h"
#include <mbedtls/psa_util.h>

#define WPA_HEX_ERR(err) ((err) < 0 ? "-" : ""), (unsigned int) ((err) < 0 ? -(err) : (err))

/* Dummy structures; these are just typecast to struct crypto_rsa_key */
struct crypto_public_key;
struct crypto_private_key;

#ifdef DEBUG_PRINT
static void crypto_dump_verify_info(u32 flags)
{
    char dump_buffer[1024];

    mbedtls_x509_crt_verify_info(dump_buffer, 1024, "  ! ", flags);
    wpa_printf(MSG_ERROR, "%s", dump_buffer);
}
#else
static void crypto_dump_verify_info(u32 flags) { }
#endif

int crypto_verify_cert(const u8 *cert_start, int certlen, const u8 *ca_cert_start, int ca_certlen)
{
    int ret;
    u32 flags = 0;

    mbedtls_x509_crt *cert = os_zalloc(sizeof(mbedtls_x509_crt));
    mbedtls_x509_crt *ca_cert = os_zalloc(sizeof(mbedtls_x509_crt));

    if (!cert || !ca_cert) {
        if (cert) {
            os_free(cert);
        }
        if (ca_cert) {
            os_free(ca_cert);
        }
        wpa_printf(MSG_ERROR, "%s: memory allocation failed", __func__);
        return -1;
    }
    mbedtls_x509_crt_init(cert);
    mbedtls_x509_crt_init(ca_cert);
    ret = mbedtls_x509_crt_parse(cert, cert_start, certlen);
    if (ret < 0) {
        wpa_printf(MSG_ERROR, "peer cert parsing failed");
        goto cleanup;
    }
    ret = mbedtls_x509_crt_parse(ca_cert, ca_cert_start, ca_certlen);
    if (ret < 0) {
        wpa_printf(MSG_ERROR, "CA cert parsing failed");
        goto cleanup;
    }

    ret = mbedtls_x509_crt_verify(cert, ca_cert, NULL, NULL, &flags, NULL, NULL);

    /* Certification is failed, try to get some more info */
    if (ret != 0) {
        crypto_dump_verify_info(flags);
    }

cleanup:
    mbedtls_x509_crt_free(cert);
    mbedtls_x509_crt_free(ca_cert);

    os_free(cert);
    os_free(ca_cert);

    return ret;
}

struct crypto_public_key *  crypto_public_key_import(const u8 *key, size_t len)
{
    int ret;
    mbedtls_pk_context *pkey = os_zalloc(sizeof(*pkey));

    if (!pkey) {
        return NULL;
    }

    mbedtls_pk_init(pkey);
    ret = mbedtls_pk_parse_public_key(pkey, key, len);

    if (ret < 0) {
        wpa_printf(MSG_ERROR, "failed to parse public key");
        os_free(pkey);
        return NULL;
    }

    return (struct crypto_public_key *)pkey;
}

struct crypto_private_key *  crypto_private_key_import(const u8 *key,
                                                       size_t len,
                                                       const char *passwd)
{
    int ret;
    mbedtls_pk_context *pkey = os_zalloc(sizeof(mbedtls_pk_context));
    if (!pkey) {
        return NULL;
    }

    mbedtls_pk_init(pkey);

    ret = mbedtls_pk_parse_key(pkey, key, len, (const unsigned char *)passwd,
                               passwd ? os_strlen(passwd) : 0);

    if (ret < 0) {
        wpa_printf(MSG_ERROR, "failed to parse private key");
        os_free(pkey);
        pkey = NULL;
    }

    return (struct crypto_private_key *)pkey;
}

struct crypto_public_key *crypto_public_key_from_cert(const u8 *buf,
                                                      size_t len)
{
    int ret;
    mbedtls_x509_crt *cert;
    mbedtls_pk_context *kctx = os_zalloc(sizeof(*kctx));

    if (!kctx) {
        wpa_printf(MSG_ERROR, "failed to allocate memory");
        return NULL;
    }

    cert = os_zalloc(sizeof(mbedtls_x509_crt));
    if (!cert) {
        wpa_printf(MSG_ERROR, "failed to allocate memory");
        goto fail;
    }
    mbedtls_x509_crt_init(cert);

    ret = mbedtls_x509_crt_parse(cert, buf, len);
    if (ret < 0) {
        wpa_printf(MSG_ERROR, "cert parsing failed");
        goto fail;
    }

    psa_key_id_t key_id = 0;
    psa_key_attributes_t key_attributes = PSA_KEY_ATTRIBUTES_INIT;
    ret = mbedtls_pk_get_psa_attributes(&cert->pk, PSA_KEY_USAGE_VERIFY_HASH, &key_attributes);
    if (ret != 0) {
        wpa_printf(MSG_ERROR, "%s:Failed to get key attributes, returned %s0x%X", __func__, WPA_HEX_ERR(ret));
        goto fail;
    }

    ret = mbedtls_pk_import_into_psa(&cert->pk, &key_attributes, &key_id);
    if (ret != 0) {
        wpa_printf(MSG_ERROR, "Failed to import key, returned %s0x%X", WPA_HEX_ERR(ret));
        goto fail;
    }

    mbedtls_pk_init(kctx);

    // Load the key from PSA into mbedTLS pk context
    ret = mbedtls_pk_copy_from_psa(key_id, kctx);
    if (ret != 0) {
        wpa_printf(MSG_ERROR, "Failed to copy key from PSA, returned %s0x%X", WPA_HEX_ERR(ret));
        goto fail;
    }

    psa_destroy_key(key_id);
    psa_reset_key_attributes(&key_attributes);

cleanup:
    mbedtls_x509_crt_free(cert);
    os_free(cert);
    return (struct crypto_public_key *)kctx;
fail:
    os_free(kctx);
    kctx = NULL;
    goto cleanup;
}

int crypto_public_key_encrypt_pkcs1_v15(struct crypto_public_key *key,
                                        const u8 *in, size_t inlen,
                                        u8 *out, size_t *outlen)
{
    int ret = 0;

    mbedtls_pk_context *pkey = (mbedtls_pk_context *)key;

    // Load the key into PSA
    psa_key_id_t key_id = 0;
    psa_key_attributes_t key_attributes = PSA_KEY_ATTRIBUTES_INIT;

    psa_status_t status = mbedtls_pk_get_psa_attributes(pkey, PSA_KEY_USAGE_ENCRYPT, &key_attributes);
    if (status != PSA_SUCCESS) {
        wpa_printf(MSG_ERROR, "%s:Failed to get key attributes, returned %s0x%X", __func__, WPA_HEX_ERR(status));
        ret = -1;
        goto cleanup;
    }

    // If we have a private key but need public key for encryption, modify attributes
    psa_key_type_t key_type = psa_get_key_type(&key_attributes);
    if (PSA_KEY_TYPE_IS_KEY_PAIR(key_type)) {
        // We have a key pair, but encryption needs the public key
        psa_set_key_type(&key_attributes, PSA_KEY_TYPE_PUBLIC_KEY_OF_KEY_PAIR(key_type));
        wpa_printf(MSG_DEBUG, "Converting key pair to public key for encryption");
    }

    ret = mbedtls_pk_import_into_psa(pkey, &key_attributes, &key_id);
    if (ret != 0) {
        wpa_printf(MSG_ERROR, "Failed to import key, returned %s0x%X", WPA_HEX_ERR(ret));
        ret = -1;
        goto cleanup;
    }

    size_t output_len = 0;
    status = psa_asymmetric_encrypt(key_id, PSA_ALG_RSA_PKCS1V15_CRYPT, in, inlen, NULL, 0, out, *outlen, &output_len);
    if (status != PSA_SUCCESS) {
        wpa_printf(MSG_ERROR, "Failed to encrypt data, returned %s0x%X", WPA_HEX_ERR((int) status));
        ret = -1;
        goto cleanup;
    }

    *outlen = output_len;

cleanup:
    psa_reset_key_attributes(&key_attributes);
    if (key_id) {
        psa_destroy_key(key_id);
    }
    return ret;
}

int  crypto_private_key_decrypt_pkcs1_v15(struct crypto_private_key *key,
                                          const u8 *in, size_t inlen,
                                          u8 *out, size_t *outlen)
{
    int ret = 0;
    mbedtls_pk_context *pkey = (mbedtls_pk_context *)key;

    // Load the key into PSA
    psa_key_id_t key_id = 0;
    psa_key_attributes_t key_attributes = PSA_KEY_ATTRIBUTES_INIT;

    psa_status_t status = mbedtls_pk_get_psa_attributes(pkey, PSA_KEY_USAGE_DECRYPT, &key_attributes);
    if (status != PSA_SUCCESS) {
        wpa_printf(MSG_ERROR, "%s:Failed to get key attributes, returned %s0x%X", __func__, WPA_HEX_ERR(status));
        ret = -1;
        goto cleanup;
    }

    ret = mbedtls_pk_import_into_psa(pkey, &key_attributes, &key_id);
    if (ret != 0) {
        wpa_printf(MSG_ERROR, "Failed to import key, returned %s0x%X", WPA_HEX_ERR(ret));
        ret = -1;
        goto cleanup;
    }

    size_t output_len = 0;
    status = psa_asymmetric_decrypt(key_id, PSA_ALG_RSA_PKCS1V15_CRYPT, in, inlen, NULL, 0, out, *outlen, &output_len);
    if (status != PSA_SUCCESS) {
        wpa_printf(MSG_ERROR, "Failed to decrypt data, returned %s0x%X", WPA_HEX_ERR((int) status));
        ret = -1;
        goto cleanup;
    }
    *outlen = output_len;

cleanup:
    psa_reset_key_attributes(&key_attributes);
    if (key_id) {
        psa_destroy_key(key_id);
    }
    return ret;
}

int crypto_private_key_sign_pkcs1(struct crypto_private_key *key,
                                  const u8 *in, size_t inlen,
                                  u8 *out, size_t *outlen)
{
    int ret = 0;

    mbedtls_pk_context *pkey = (mbedtls_pk_context *)key;

    // Load the key into PSA
    psa_key_id_t key_id = 0;
    psa_key_attributes_t key_attributes = PSA_KEY_ATTRIBUTES_INIT;

    psa_status_t status = mbedtls_pk_get_psa_attributes(pkey, PSA_KEY_USAGE_SIGN_HASH, &key_attributes);
    if (status != PSA_SUCCESS) {
        wpa_printf(MSG_ERROR, "%s:Failed to get key attributes, returned %s0x%X", __func__, WPA_HEX_ERR(status));
        ret = -1;
        goto cleanup;
    }

    ret = mbedtls_pk_import_into_psa(pkey, &key_attributes, &key_id);
    if (ret != 0) {
        wpa_printf(MSG_ERROR, "Failed to import key, returned %s0x%X", WPA_HEX_ERR(ret));
        ret = -1;
        goto cleanup;
    }

    size_t output_len = 0;
    status = psa_sign_hash(key_id, PSA_ALG_RSA_PKCS1V15_SIGN_RAW, in, inlen, out, *outlen, &output_len);
    if (status != PSA_SUCCESS) {
        wpa_printf(MSG_ERROR, "Failed to sign data, returned %s0x%X", WPA_HEX_ERR((int) status));
        ret = -1;
        goto cleanup;
    }

    *outlen = output_len;

cleanup:
    psa_reset_key_attributes(&key_attributes);
    if (key_id) {
        psa_destroy_key(key_id);
    }
    return ret;
}

struct crypto_public_key *crypto_public_key_from_private_key(struct crypto_private_key *priv_key)
{
    if (!priv_key) {
        return NULL;
    }

    mbedtls_pk_context *priv_ctx = (mbedtls_pk_context *)priv_key;
    mbedtls_pk_context *pub_ctx = os_zalloc(sizeof(mbedtls_pk_context));

    if (!pub_ctx) {
        wpa_printf(MSG_ERROR, "Failed to allocate memory for public key");
        return NULL;
    }

    // Import the private key into PSA temporarily to extract the public key
    psa_key_id_t key_id = 0;
    psa_key_attributes_t key_attributes = PSA_KEY_ATTRIBUTES_INIT;

    // Use a valid usage flag - mbedtls_pk_get_psa_attributes will automatically add EXPORT
    // We use DECRYPT as a generic private key operation to get the key attributes
    int ret = mbedtls_pk_get_psa_attributes(priv_ctx, PSA_KEY_USAGE_DECRYPT, &key_attributes);
    if (ret != 0) {
        wpa_printf(MSG_ERROR, "%s:Failed to get key attributes, returned %s0x%X", __func__, WPA_HEX_ERR(ret));
        os_free(pub_ctx);
        return NULL;
    }

    ret = mbedtls_pk_import_into_psa(priv_ctx, &key_attributes, &key_id);
    if (ret != 0) {
        wpa_printf(MSG_ERROR, "Failed to import private key, returned %s0x%X", WPA_HEX_ERR(ret));
        psa_reset_key_attributes(&key_attributes);
        os_free(pub_ctx);
        return NULL;
    }

    // Export the public key
    unsigned char pub_key_buf[PSA_EXPORT_PUBLIC_KEY_MAX_SIZE];
    size_t pub_key_len = 0;
    psa_status_t status = psa_export_public_key(key_id, pub_key_buf, sizeof(pub_key_buf), &pub_key_len);

    psa_destroy_key(key_id);
    psa_reset_key_attributes(&key_attributes);

    if (status != PSA_SUCCESS) {
        wpa_printf(MSG_ERROR, "Failed to export public key, returned %s0x%X", WPA_HEX_ERR((int) status));
        os_free(pub_ctx);
        return NULL;
    }

    // Parse the public key into a new pk context
    mbedtls_pk_init(pub_ctx);
    ret = mbedtls_pk_parse_public_key(pub_ctx, pub_key_buf, pub_key_len);
    if (ret != 0) {
        wpa_printf(MSG_ERROR, "Failed to parse public key, returned %s0x%X", WPA_HEX_ERR(ret));
        mbedtls_pk_free(pub_ctx);
        os_free(pub_ctx);
        return NULL;
    }

    return (struct crypto_public_key *)pub_ctx;
}

void  crypto_public_key_free(struct crypto_public_key *key)
{
    mbedtls_pk_context *pkey = (mbedtls_pk_context *)key;
    if (!pkey) {
        return;
    }

    mbedtls_pk_free(pkey);
    os_free(pkey);
}

void  crypto_private_key_free(struct crypto_private_key *key)
{
    mbedtls_pk_context *pkey = (mbedtls_pk_context *)key;
    if (!pkey) {
        return;
    }

    mbedtls_pk_free(pkey);
    os_free(pkey);
}

int  crypto_public_key_decrypt_pkcs1(struct crypto_public_key *key,
                                     const u8 *crypt, size_t crypt_len,
                                     u8 *plain, size_t *plain_len)
{
    mbedtls_pk_context *pkey = (mbedtls_pk_context *)key;
    struct crypto_rsa_key *rsa_key = NULL;
    struct asn1_hdr hdr;
    struct asn1_hdr bitstring;
    const u8 *spki_pos, *spki_end;
    int ret;
    /* SubjectPublicKeyInfo DER for RSA-4096 is comfortably below this size. */
    unsigned char pubkey_der[1024];
    const u8 *pubkey = NULL;
    size_t pubkey_len = 0;

    if (!pkey || !crypt || !plain || !plain_len) {
        return -1;
    }

    ret = mbedtls_pk_write_pubkey_der(pkey, pubkey_der, sizeof(pubkey_der));
    if (ret <= 0) {
        wpa_printf(MSG_ERROR, "%s: Failed to export public key in DER form, returned %s0x%X", __func__, WPA_HEX_ERR(ret));
        return -1;
    }
    pubkey_len = (size_t) ret;
    pubkey = pubkey_der + sizeof(pubkey_der) - pubkey_len;

    /*
     * mbedtls_pk_write_pubkey_der() returns SubjectPublicKeyInfo.
     * crypto_rsa_import_public_key() expects PKCS#1 RSAPublicKey,
     * i.e. the BIT STRING payload inside SubjectPublicKeyInfo.
     */
    if (asn1_get_next(pubkey, pubkey_len, &hdr) < 0 || !asn1_is_sequence(&hdr)) {
        wpa_printf(MSG_ERROR, "%s: Failed to parse SubjectPublicKeyInfo sequence", __func__);
        return -1;
    }
    spki_pos = hdr.payload;
    spki_end = spki_pos + hdr.length;

    if (asn1_get_next(spki_pos, spki_end - spki_pos, &hdr) < 0 || !asn1_is_sequence(&hdr)) {
        wpa_printf(MSG_ERROR, "%s: Failed to parse SubjectPublicKeyInfo algorithm", __func__);
        return -1;
    }
    spki_pos = hdr.payload + hdr.length;

    if (asn1_get_next(spki_pos, spki_end - spki_pos, &bitstring) < 0 || !asn1_is_bitstring(&bitstring) ||
            bitstring.length < 1 || bitstring.payload[0] != 0) {
        wpa_printf(MSG_ERROR, "%s: Failed to parse SubjectPublicKey BIT STRING", __func__);
        return -1;
    }

    pubkey = bitstring.payload + 1;
    pubkey_len = bitstring.length - 1;
    rsa_key = crypto_rsa_import_public_key(pubkey, pubkey_len);
    if (!rsa_key) {
        wpa_printf(MSG_ERROR, "%s: Failed to import public key for PKCS#1 verification", __func__);
        return -1;
    }
    ret = pkcs1_decrypt_public_key(rsa_key, crypt, crypt_len, plain, plain_len);
    crypto_rsa_free(rsa_key);
    return ret;
}
#endif
