/*
 * SPDX-FileCopyrightText: 2015-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifdef ESP_PLATFORM
#include "mbedtls/bignum.h"
#include "mbedtls/esp_mbedtls_random.h"
#endif

#include "utils/includes.h"
#include "utils/common.h"
#include "crypto.h"
#include "common/defs.h"

#ifdef CONFIG_CRYPTO_MBEDTLS
// #include "mbedtls/entropy.h"
// #include "mbedtls/ctr_drbg.h"

#include <mbedtls/error.h>
#include <mbedtls/x509_crt.h>
#include <mbedtls/platform.h>
// #include <mbedtls/sha256.h>

#include "psa/crypto.h"
#include <mbedtls/psa_util.h>

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
        wpa_printf(MSG_ERROR, "Failed to get key attributes, returned %d", ret);
        goto fail;
    }

    ret = mbedtls_pk_import_into_psa(&cert->pk, &key_attributes, &key_id);
    if (ret != 0) {
        wpa_printf(MSG_ERROR, "Failed to import key, returned %d", ret);
        goto fail;
    }

    mbedtls_pk_init(kctx);

    // Load the key from PSA into mbedTLS pk context
    ret = mbedtls_pk_copy_from_psa(key_id, kctx);
    if (ret != 0) {
        wpa_printf(MSG_ERROR, "Failed to copy key from PSA, returned %d", ret);
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
        wpa_printf(MSG_ERROR, "Failed to get key attributes, returned %d", (int) status);
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
        wpa_printf(MSG_ERROR, "Failed to import key, returned %d", ret);
        ret = -1;
        goto cleanup;
    }

    size_t output_len = 0;
    status = psa_asymmetric_encrypt(key_id, PSA_ALG_RSA_PKCS1V15_CRYPT, in, inlen, NULL, 0, out, *outlen, &output_len);
    if (status != PSA_SUCCESS) {
        wpa_printf(MSG_ERROR, "Failed to encrypt data, returned %d", (int) status);
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
        wpa_printf(MSG_ERROR, "Failed to get key attributes, returned %d", (int) status);
        ret = -1;
        goto cleanup;
    }

    ret = mbedtls_pk_import_into_psa(pkey, &key_attributes, &key_id);
    if (ret != 0) {
        wpa_printf(MSG_ERROR, "Failed to import key, returned %d", ret);
        ret = -1;
        goto cleanup;
    }

    size_t output_len = 0;
    status = psa_asymmetric_decrypt(key_id, PSA_ALG_RSA_PKCS1V15_CRYPT, in, inlen, NULL, 0, out, *outlen, &output_len);
    if (status != PSA_SUCCESS) {
        wpa_printf(MSG_ERROR, "Failed to decrypt data, returned %d", (int) status);
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
        wpa_printf(MSG_ERROR, "Failed to get key attributes, returned %d", (int) status);
        ret = -1;
        goto cleanup;
    }

    ret = mbedtls_pk_import_into_psa(pkey, &key_attributes, &key_id);
    if (ret != 0) {
        wpa_printf(MSG_ERROR, "Failed to import key, returned %d", ret);
        ret = -1;
        goto cleanup;
    }

    size_t output_len = 0;
    status = psa_sign_hash(key_id, PSA_ALG_RSA_PKCS1V15_SIGN_RAW, in, inlen, out, *outlen, &output_len);
    if (status != PSA_SUCCESS) {
        wpa_printf(MSG_ERROR, "Failed to sign data, returned %d", (int) status);
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
        wpa_printf(MSG_ERROR, "Failed to get key attributes, returned %d", ret);
        os_free(pub_ctx);
        return NULL;
    }

    ret = mbedtls_pk_import_into_psa(priv_ctx, &key_attributes, &key_id);
    if (ret != 0) {
        wpa_printf(MSG_ERROR, "Failed to import private key, returned %d", ret);
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
        wpa_printf(MSG_ERROR, "Failed to export public key, returned %d", (int) status);
        os_free(pub_ctx);
        return NULL;
    }

    // Parse the public key into a new pk context
    mbedtls_pk_init(pub_ctx);
    ret = mbedtls_pk_parse_public_key(pub_ctx, pub_key_buf, pub_key_len);
    if (ret != 0) {
        wpa_printf(MSG_ERROR, "Failed to parse public key, returned %d", ret);
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
    size_t len;
    u8 *pos;
    mbedtls_pk_context *pkey = (mbedtls_pk_context *)key;

    // Load the key into PSA
    psa_key_id_t key_id = 0;
    psa_key_attributes_t key_attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_status_t status;
    int ret = mbedtls_pk_get_psa_attributes(pkey, PSA_KEY_USAGE_DECRYPT, &key_attributes);
    if (ret != 0) {
        wpa_printf(MSG_ERROR, "Failed to get key attributes, returned %d", ret);
        psa_reset_key_attributes(&key_attributes);
        return -1;
    }

    len = psa_get_key_bits(&key_attributes) / 8;
    if (len != crypt_len) {
        psa_reset_key_attributes(&key_attributes);
        return -1;
    }

    ret = mbedtls_pk_import_into_psa(pkey, &key_attributes, &key_id);
    if (ret != 0) {
        wpa_printf(MSG_ERROR, "Failed to import key, returned %d", ret);
        psa_reset_key_attributes(&key_attributes);
        return -1;
    }
    psa_reset_key_attributes(&key_attributes);

    size_t output_len = 0;
    status = psa_asymmetric_decrypt(key_id, PSA_ALG_RSA_PKCS1V15_CRYPT, crypt, crypt_len, NULL, 0, plain, *plain_len, &output_len);
    if (status != PSA_SUCCESS) {
        wpa_printf(MSG_ERROR, "Failed to decrypt data, returned %d", (int) status);
        psa_destroy_key(key_id);
        return -1;
    }

    *plain_len = output_len;

    if (key_id) {
        psa_destroy_key(key_id);
    }

    /*
     * PKCS #1 v1.5, 8.1:
     *
     * EB = 00 || BT || PS || 00 || D
     * BT = 00 or 01
     * PS = k-3-||D|| times (00 if BT=00) or (FF if BT=01)
     * k = length of modulus in octets
     *
     * Based on 10.1.3, "The block type shall be 01" for a signature.
     */

    if (len < 3 + 8 + 16 /* min hash len */ ||
            plain[0] != 0x00 || plain[1] != 0x01) {
        wpa_printf(MSG_INFO, "LibTomCrypt: Invalid signature EB "
                   "structure");
        wpa_hexdump_key(MSG_DEBUG, "Signature EB", plain, len);
        return -1;
    }

    pos = plain + 3;
    /* BT = 01 */
    if (plain[2] != 0xff) {
        wpa_printf(MSG_INFO, "LibTomCrypt: Invalid signature "
                   "PS (BT=01)");
        wpa_hexdump_key(MSG_DEBUG, "Signature EB", plain, len);
        return -1;
    }
    while (pos < plain + len && *pos == 0xff) {
        pos++;
    }

    if (pos - plain - 2 < 8) {
        /* PKCS #1 v1.5, 8.1: At least eight octets long PS */
        wpa_printf(MSG_INFO, "LibTomCrypt: Too short signature "
                   "padding");
        wpa_hexdump_key(MSG_DEBUG, "Signature EB", plain, len);
        return -1;
    }

    if (pos + 16 /* min hash len */ >= plain + len || *pos != 0x00) {
        wpa_printf(MSG_INFO, "LibTomCrypt: Invalid signature EB "
                   "structure (2)");
        wpa_hexdump_key(MSG_DEBUG, "Signature EB", plain, len);
        return -1;
    }
    pos++;
    len -= pos - plain;

    /* Strip PKCS #1 header */
    os_memmove(plain, pos, len);
    *plain_len = len;

    return 0;
}
#endif
