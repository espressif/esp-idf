/*
 * SPDX-FileCopyrightText: 2020-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#define MBEDTLS_ALLOW_PRIVATE_ACCESS

#ifdef ESP_PLATFORM
#include "esp_system.h"
#endif
#include "sdkconfig.h"
#include <errno.h>
#include "utils/includes.h"
#include "utils/common.h"
#include "crypto.h"
#include "random.h"
#include "sha256.h"

#include "mbedtls/ecp.h"
#include "mbedtls/md.h"
#include "mbedtls/bignum.h"
#include "mbedtls/nist_kw.h"

#include "common.h"
#include "utils/wpabuf.h"
#include "dh_group5.h"
#include "md5.h"
#include "sha1.h"
#include "sha256.h"
#include "sha384.h"
#include "aes_wrap.h"
#include "crypto.h"
#include "mbedtls/esp_config.h"

#include "psa/crypto.h"
#include "mbedtls/psa_util.h"

#ifdef CONFIG_FAST_PBKDF2
#include "fastpbkdf2.h"
#include "fastpsk.h"
#endif

struct crypto_hash {
    union {
        psa_hash_operation_t *hash_operation;
        psa_mac_operation_t *mac_operation;
    } u;
    int is_mac;
    psa_key_id_t key_id;
};

static int digest_vector(psa_algorithm_t alg, size_t num_elem,
                         const u8 *addr[], const size_t *len, u8 *mac)
{
    psa_hash_operation_t operation = PSA_HASH_OPERATION_INIT;
    psa_status_t status;
    int ret = -1;

    status = psa_hash_setup(&operation, alg);
    if (status != PSA_SUCCESS) {
        goto cleanup;
    }

    for (size_t i = 0; i < num_elem; i++) {
        status = psa_hash_update(&operation, addr[i], len[i]);
        if (status != PSA_SUCCESS) {
            goto cleanup;
        }
    }

    size_t mac_len;
    status = psa_hash_finish(&operation, mac, PSA_HASH_LENGTH(alg), &mac_len);
    if (status != PSA_SUCCESS) {
        goto cleanup;
    }

    ret = 0;

cleanup:
    psa_hash_abort(&operation);
    return ret;
}

int sha256_vector(size_t num_elem, const u8 *addr[], const size_t *len,
                  u8 *mac)
{
    return digest_vector(PSA_ALG_SHA_256, num_elem, addr, len, mac);
}

int sha384_vector(size_t num_elem, const u8 *addr[], const size_t *len,
                  u8 *mac)
{
    return digest_vector(PSA_ALG_SHA_384, num_elem, addr, len, mac);
}

int sha512_vector(size_t num_elem, const u8 *addr[], const size_t *len,
                  u8 *mac)
{
    return digest_vector(PSA_ALG_SHA_512, num_elem, addr, len, mac);
}

#if CONFIG_MBEDTLS_SHA1_C || CONFIG_MBEDTLS_HARDWARE_SHA
int sha1_vector(size_t num_elem, const u8 *addr[], const size_t *len, u8 *mac)
{
    return digest_vector(PSA_ALG_SHA_1, num_elem, addr, len, mac);
}
#endif

int md5_vector(size_t num_elem, const u8 *addr[], const size_t *len, u8 *mac)
{
    return digest_vector(PSA_ALG_MD5, num_elem, addr, len, mac);
}

#ifdef MBEDTLS_MD4_C
int md4_vector(size_t num_elem, const u8 *addr[], const size_t *len, u8 *mac)
{
    return digest_vector(MBEDTLS_MD_MD4, num_elem, addr, len, mac);
}
#endif

struct crypto_hash * crypto_hash_init(enum crypto_hash_alg alg, const u8 *key,
                                      size_t key_len)
{
    struct crypto_hash *ctx = os_zalloc(sizeof(struct crypto_hash));
    if (ctx == NULL) {
        return NULL;
    }

    psa_algorithm_t psa_alg;
    int is_hmac = 0;

    switch (alg) {
    case CRYPTO_HASH_ALG_MD5:
    case CRYPTO_HASH_ALG_HMAC_MD5:
        psa_alg = PSA_ALG_MD5;
        break;
    case CRYPTO_HASH_ALG_SHA1:
    case CRYPTO_HASH_ALG_HMAC_SHA1:
        psa_alg = PSA_ALG_SHA_1;
        break;
    case CRYPTO_HASH_ALG_SHA256:
    case CRYPTO_HASH_ALG_HMAC_SHA256:
        psa_alg = PSA_ALG_SHA_256;
        break;
    case CRYPTO_HASH_ALG_SHA384:
        psa_alg = PSA_ALG_SHA_384;
        break;
    case CRYPTO_HASH_ALG_SHA512:
        psa_alg = PSA_ALG_SHA_512;
        break;
    default:
        os_free(ctx);
        return NULL;
    }

    switch (alg) {
    case CRYPTO_HASH_ALG_HMAC_MD5:
    case CRYPTO_HASH_ALG_HMAC_SHA1:
    case CRYPTO_HASH_ALG_HMAC_SHA256:
        is_hmac = 1;
        break;
    default:
        break;
    }

    // If is_hmac is set, then it is HMAC mode
    if (is_hmac) {
        if (key == NULL || key_len == 0) {
            os_free(ctx);
            return NULL;
        }

        psa_mac_operation_t *operation = os_zalloc(sizeof(psa_mac_operation_t));
        if (operation == NULL) {
            os_free(ctx);
            return NULL;
        }

        psa_key_id_t key_id = 0;
        psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
        psa_set_key_usage_flags(&attributes, PSA_KEY_USAGE_SIGN_MESSAGE);
        psa_set_key_algorithm(&attributes, PSA_ALG_HMAC(psa_alg));
        psa_set_key_type(&attributes, PSA_KEY_TYPE_HMAC);
        psa_set_key_bits(&attributes, 8 * key_len);

        psa_status_t status = psa_import_key(&attributes, key, key_len, &key_id);
        psa_reset_key_attributes(&attributes);
        if (status != PSA_SUCCESS) {
            os_free(operation);
            os_free(ctx);
            return NULL;
        }

        status = psa_mac_sign_setup(operation, key_id, PSA_ALG_HMAC(psa_alg));
        if (status != PSA_SUCCESS) {
            psa_destroy_key(key_id);
            os_free(operation);
            os_free(ctx);
            return NULL;
        }
        ctx->is_mac = 1;
        ctx->key_id = key_id;
        ctx->u.mac_operation = operation;
    } else {
        psa_hash_operation_t *operation = os_zalloc(sizeof(psa_hash_operation_t));
        if (operation == NULL) {
            os_free(ctx);
            return NULL;
        }

        psa_status_t status = psa_hash_setup(operation, psa_alg);
        if (status != PSA_SUCCESS) {
            os_free(operation);
            os_free(ctx);
            return NULL;
        }
        ctx->is_mac = 0;
        ctx->key_id = 0;
        ctx->u.hash_operation = operation;
    }

    return ctx;
}

void crypto_hash_update(struct crypto_hash *crypto_ctx, const u8 *data, size_t len)
{
    if (data == NULL || len == 0) {
        return;
    }

    psa_status_t status = PSA_ERROR_GENERIC_ERROR;

    if (crypto_ctx->is_mac) {
        status = psa_mac_update(crypto_ctx->u.mac_operation, data, len);
    } else {
        status = psa_hash_update(crypto_ctx->u.hash_operation, data, len);
    }

    if (status != PSA_SUCCESS) {
        wpa_printf(MSG_ERROR, "%s: psa_hash_update failed", __func__);
    }

}

int crypto_hash_finish(struct crypto_hash *crypto_ctx, u8 *mac, size_t *len)
{
    int ret = 0;

    if (crypto_ctx == NULL) {
        return -2;
    }

    if (mac == NULL || len == NULL) {
        ret = -1;
        goto err;
    }

    psa_status_t status = PSA_ERROR_GENERIC_ERROR;

    size_t mac_len = 0;

    if (crypto_ctx->is_mac) {
        status = psa_mac_sign_finish(crypto_ctx->u.mac_operation, mac, *len, &mac_len);
        if (status != PSA_SUCCESS) {
            ret = -1;
            goto err;
        }
    } else {
        status = psa_hash_finish(crypto_ctx->u.hash_operation, mac, *len, &mac_len);
        if (status != PSA_SUCCESS) {
            ret = -1;
            goto err;
        }
    }

    *len = mac_len;

err:
    if (crypto_ctx->is_mac) {
        if (crypto_ctx->u.mac_operation) {
            psa_mac_abort(crypto_ctx->u.mac_operation);
            os_free(crypto_ctx->u.mac_operation);
        }
    } else {
        if (crypto_ctx->u.hash_operation) {
            psa_hash_abort(crypto_ctx->u.hash_operation);
            os_free(crypto_ctx->u.hash_operation);
        }
    }

    if (crypto_ctx->key_id) {
        psa_destroy_key(crypto_ctx->key_id);
    }
    os_free(crypto_ctx);
    return ret;
}

static int hmac_vector(psa_algorithm_t alg,
                       const u8 *key, size_t key_len,
                       size_t num_elem, const u8 *addr[],
                       const size_t *len, u8 *mac)
{
    int ret = 0;
    psa_key_id_t key_id = 0;
    if (key == NULL || key_len == 0 || num_elem == 0 || addr == NULL || len == NULL || mac == NULL) {
        return -1;
    }

    psa_status_t status;

    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_set_key_usage_flags(&attributes, PSA_KEY_USAGE_SIGN_MESSAGE);
    psa_set_key_algorithm(&attributes, PSA_ALG_HMAC(alg));
    psa_set_key_type(&attributes, PSA_KEY_TYPE_HMAC);
    psa_set_key_bits(&attributes, 8 * key_len);

    psa_mac_operation_t operation = PSA_MAC_OPERATION_INIT;

    status = psa_import_key(&attributes, key, key_len, &key_id);
    if (status != PSA_SUCCESS) {
        ret = -1;
        goto err;
    }

    status = psa_mac_sign_setup(&operation, key_id, PSA_ALG_HMAC(alg));
    if (status != PSA_SUCCESS) {
        ret = -1;
        goto err;
    }

    for (size_t i = 0; i < num_elem; i++) {
        status = psa_mac_update(&operation, addr[i], len[i]);
        if (status != PSA_SUCCESS) {
            ret = -1;
            goto err;
        }
    }

    size_t mac_len;
    /* For HMAC, the MAC length equals the hash output length */
    size_t expected_mac_len = PSA_HASH_LENGTH(alg);
    status = psa_mac_sign_finish(&operation, mac, expected_mac_len, &mac_len);
    if (status != PSA_SUCCESS) {
        ret = -1;
        goto err;
    }

    status = psa_destroy_key(key_id);
    if (status != PSA_SUCCESS) {
        ret = -1;
    }

    return ret;

err:
    if (ret != 0) {
        if (key_id) {
            psa_destroy_key(key_id);
        }
        psa_mac_abort(&operation);
    }
    psa_reset_key_attributes(&attributes);

    return ret;
}

int hmac_sha384_vector(const u8 *key, size_t key_len, size_t num_elem,
                       const u8 *addr[], const size_t *len, u8 *mac)
{
    return hmac_vector(PSA_ALG_SHA_384, key, key_len, num_elem, addr,
                       len, mac);
}

int hmac_sha384(const u8 *key, size_t key_len, const u8 *data,
                size_t data_len, u8 *mac)
{
    return hmac_sha384_vector(key, key_len, 1, &data, &data_len, mac);
}

int hmac_sha256_vector(const u8 *key, size_t key_len, size_t num_elem,
                       const u8 *addr[], const size_t *len, u8 *mac)
{
    return hmac_vector(PSA_ALG_SHA_256, key, key_len, num_elem, addr,
                       len, mac);
}

int hmac_sha256(const u8 *key, size_t key_len, const u8 *data,
                size_t data_len, u8 *mac)
{
    return hmac_sha256_vector(key, key_len, 1, &data, &data_len, mac);
}

int hmac_md5_vector(const u8 *key, size_t key_len, size_t num_elem,
                    const u8 *addr[], const size_t *len, u8 *mac)
{
    return hmac_vector(PSA_ALG_MD5, key, key_len,
                       num_elem, addr, len, mac);
}

int hmac_md5(const u8 *key, size_t key_len, const u8 *data, size_t data_len,
             u8 *mac)
{
    return hmac_md5_vector(key, key_len, 1, &data, &data_len, mac);
}

#if (defined(MBEDTLS_SHA1_C) || defined(PSA_WANT_ALG_SHA_1))
int hmac_sha1_vector(const u8 *key, size_t key_len, size_t num_elem,
                     const u8 *addr[], const size_t *len, u8 *mac)
{
    return hmac_vector(PSA_ALG_SHA_1, key, key_len, num_elem, addr,
                       len, mac);
}

int hmac_sha1(const u8 *key, size_t key_len, const u8 *data, size_t data_len,
              u8 *mac)
{
    return hmac_sha1_vector(key, key_len, 1, &data, &data_len, mac);
}
#endif

static void *aes_crypt_init(int mode, const u8 *key, size_t len)
{
    psa_status_t status;
    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_key_id_t *key_id = os_malloc(sizeof(psa_key_id_t));

    if (key_id == NULL) {
        return NULL;
    }

    if (mode == MBEDTLS_ENCRYPT) {
        psa_set_key_usage_flags(&attributes, PSA_KEY_USAGE_ENCRYPT);
    } else if (mode == MBEDTLS_DECRYPT) {
        psa_set_key_usage_flags(&attributes, PSA_KEY_USAGE_DECRYPT);
    }

    psa_set_key_algorithm(&attributes, PSA_ALG_ECB_NO_PADDING);
    psa_set_key_type(&attributes, PSA_KEY_TYPE_AES);
    psa_set_key_bits(&attributes, len * 8);

    status = psa_import_key(&attributes, key, len, key_id);
    psa_reset_key_attributes(&attributes);
    if (status != PSA_SUCCESS) {
        wpa_printf(MSG_ERROR, "%s: psa_import_key failed", __func__);
        os_free(key_id);
        return NULL;
    }

    return (void *) key_id;
}

static int aes_crypt(void *ctx, int mode, const u8 *in, u8 *out)
{
    psa_status_t status;
    psa_key_id_t *key_id = (psa_key_id_t *) ctx;
    psa_cipher_operation_t operation = PSA_CIPHER_OPERATION_INIT;
    size_t output_len;
    int ret = -1;

    if (mode == MBEDTLS_ENCRYPT) {
        status = psa_cipher_encrypt_setup(&operation, *key_id, PSA_ALG_ECB_NO_PADDING);
    } else if (mode == MBEDTLS_DECRYPT) {
        status = psa_cipher_decrypt_setup(&operation, *key_id, PSA_ALG_ECB_NO_PADDING);
    } else {
        wpa_printf(MSG_ERROR, "%s: invalid mode", __func__);
        return -1;
    }
    if (status != PSA_SUCCESS) {
        wpa_printf(MSG_ERROR, "%s: psa_cipher_encrypt_setup failed", __func__);
        psa_cipher_abort(&operation);
        return -1;
    }

    status = psa_cipher_update(&operation, in, 16, out, 16, &output_len);
    if (status != PSA_SUCCESS) {
        wpa_printf(MSG_ERROR, "%s: psa_cipher_update failed", __func__);
        goto cleanup;
    }

    status = psa_cipher_finish(&operation, out + output_len, 16 - output_len, &output_len);
    if (status != PSA_SUCCESS) {
        wpa_printf(MSG_ERROR, "%s: psa_cipher_finish failed", __func__);
        goto cleanup;
    }

    ret = 0;

cleanup:
    psa_cipher_abort(&operation);
    return ret;
}

static void aes_crypt_deinit(void *ctx)
{
    psa_key_id_t *key_id = (psa_key_id_t *) ctx;
    psa_status_t status = psa_destroy_key(*key_id);
    if (status != PSA_SUCCESS) {
        wpa_printf(MSG_ERROR, "%s: psa_destroy_key failed", __func__);
    }
    os_free(ctx);
}

void *aes_encrypt_init(const u8 *key, size_t len)
{
    return aes_crypt_init(MBEDTLS_ENCRYPT, key, len);
}

int aes_encrypt(void *ctx, const u8 *plain, u8 *crypt)
{
    return aes_crypt(ctx, MBEDTLS_ENCRYPT, plain, crypt);
}

void aes_encrypt_deinit(void *ctx)
{
    return aes_crypt_deinit(ctx);
}

void * aes_decrypt_init(const u8 *key, size_t len)
{
    return aes_crypt_init(MBEDTLS_DECRYPT, key, len);
}

int aes_decrypt(void *ctx, const u8 *crypt, u8 *plain)
{
    return aes_crypt(ctx, MBEDTLS_DECRYPT, crypt, plain);
}

void aes_decrypt_deinit(void *ctx)
{
    return aes_crypt_deinit(ctx);
}

#ifdef CONFIG_MBEDTLS_CIPHER_MODE_CBC
int aes_128_cbc_encrypt(const u8 *key, const u8 *iv, u8 *data, size_t data_len)
{
    psa_status_t status;
    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_key_id_t key_id;

    psa_set_key_usage_flags(&attributes, PSA_KEY_USAGE_ENCRYPT);
    psa_set_key_algorithm(&attributes, PSA_ALG_CBC_NO_PADDING);
    psa_set_key_type(&attributes, PSA_KEY_TYPE_AES);
    psa_set_key_bits(&attributes, 128);

    status = psa_import_key(&attributes, key, 16, &key_id);
    psa_reset_key_attributes(&attributes);
    if (status != PSA_SUCCESS) {
        wpa_printf(MSG_ERROR, "%s: psa_import_key failed", __func__);
        return -1;
    }

    psa_cipher_operation_t operation = PSA_CIPHER_OPERATION_INIT;

    status = psa_cipher_encrypt_setup(&operation, key_id, PSA_ALG_CBC_NO_PADDING);
    if (status != PSA_SUCCESS) {
        wpa_printf(MSG_ERROR, "%s: psa_cipher_encrypt_setup failed", __func__);
        psa_destroy_key(key_id);
        return -1;
    }

    status = psa_cipher_set_iv(&operation, iv, 16);
    if (status != PSA_SUCCESS) {
        wpa_printf(MSG_ERROR, "%s: psa_cipher_set_iv failed", __func__);
        psa_cipher_abort(&operation);
        psa_destroy_key(key_id);
        return -1;
    }

    size_t output_length = 0;
    status = psa_cipher_update(&operation, data, data_len, data, data_len, &output_length);
    if (status != PSA_SUCCESS) {
        wpa_printf(MSG_ERROR, "%s: psa_cipher_update failed", __func__);
        psa_cipher_abort(&operation);
        psa_destroy_key(key_id);
        return -1;
    }

    status = psa_cipher_finish(&operation, data + output_length, data_len - output_length, &output_length);
    if (status != PSA_SUCCESS) {
        wpa_printf(MSG_ERROR, "%s: psa_cipher_finish failed", __func__);
        psa_cipher_abort(&operation);
        psa_destroy_key(key_id);
        return -1;
    }

    psa_cipher_abort(&operation);

    psa_destroy_key(key_id);

    return 0;
}

int aes_128_cbc_decrypt(const u8 *key, const u8 *iv, u8 *data, size_t data_len)
{
    psa_status_t status;
    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_key_id_t key_id;

    psa_set_key_usage_flags(&attributes, PSA_KEY_USAGE_DECRYPT);
    psa_set_key_algorithm(&attributes, PSA_ALG_CBC_NO_PADDING);
    psa_set_key_type(&attributes, PSA_KEY_TYPE_AES);
    psa_set_key_bits(&attributes, 128);

    status = psa_import_key(&attributes, key, 16, &key_id);
    psa_reset_key_attributes(&attributes);
    if (status != PSA_SUCCESS) {
        wpa_printf(MSG_ERROR, "%s: psa_import_key failed", __func__);
        return -1;
    }

    psa_cipher_operation_t operation = PSA_CIPHER_OPERATION_INIT;

    status = psa_cipher_decrypt_setup(&operation, key_id, PSA_ALG_CBC_NO_PADDING);
    if (status != PSA_SUCCESS) {
        wpa_printf(MSG_ERROR, "%s: psa_cipher_decrypt_setup failed", __func__);
        psa_destroy_key(key_id);
        return -1;
    }

    status = psa_cipher_set_iv(&operation, iv, 16);
    if (status != PSA_SUCCESS) {
        wpa_printf(MSG_ERROR, "%s: psa_cipher_set_iv failed", __func__);
        psa_cipher_abort(&operation);
        psa_destroy_key(key_id);
        return -1;
    }

    size_t output_length = 0;

    status = psa_cipher_update(&operation, data, data_len, data, data_len, &output_length);
    if (status != PSA_SUCCESS) {
        wpa_printf(MSG_ERROR, "%s: psa_cipher_update failed", __func__);
        psa_cipher_abort(&operation);
        psa_destroy_key(key_id);
        return -1;
    }

    status = psa_cipher_finish(&operation, data + output_length, data_len - output_length, &output_length);
    if (status != PSA_SUCCESS) {
        wpa_printf(MSG_ERROR, "%s: psa_cipher_finish failed", __func__);
        psa_cipher_abort(&operation);
        psa_destroy_key(key_id);
        return -1;
    }

    psa_cipher_abort(&operation);

    psa_destroy_key(key_id);

    return 0;

}
#endif /* CONFIG_MBEDTLS_CIPHER_MODE_CBC */

#ifdef CONFIG_TLS_INTERNAL_CLIENT
struct crypto_cipher {
    void *ctx_enc;
    void *ctx_dec;
    psa_key_id_t key_id;
};

static uint32_t alg_to_psa_cipher(enum crypto_cipher_alg alg)
{
    switch (alg) {
    case CRYPTO_CIPHER_ALG_AES:
    case CRYPTO_CIPHER_ALG_3DES:
    case CRYPTO_CIPHER_ALG_DES:
        return PSA_ALG_CBC_NO_PADDING;
    default:
        break;
    }

    return 0;
}

static uint32_t alg_to_psa_key_type(enum crypto_cipher_alg alg)
{
    switch (alg) {
    case CRYPTO_CIPHER_ALG_AES:
        return PSA_KEY_TYPE_AES;
    case CRYPTO_CIPHER_ALG_3DES:
    case CRYPTO_CIPHER_ALG_DES:
        return PSA_KEY_TYPE_DES;
    default:
        break;
    }

    return 0;
}

struct crypto_cipher *crypto_cipher_init(enum crypto_cipher_alg alg,
                                         const u8 *iv, const u8 *key,
                                         size_t key_len)
{
    struct crypto_cipher *ctx;

    ctx = (struct crypto_cipher *)os_zalloc(sizeof(*ctx));
    if (!ctx) {
        return NULL;
    }

    psa_status_t status;
    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_key_id_t key_id = 0;
    psa_cipher_operation_t *enc_operation = NULL;
    psa_cipher_operation_t *dec_operation = NULL;

    psa_set_key_usage_flags(&attributes, PSA_KEY_USAGE_ENCRYPT | PSA_KEY_USAGE_DECRYPT);
    uint32_t psa_alg = alg_to_psa_cipher(alg);
    if (psa_alg == 0) {
        wpa_printf(MSG_ERROR, "%s: invalid cipher algorithm", __func__);
        goto cleanup;
    }
    psa_set_key_algorithm(&attributes, psa_alg);

    uint32_t psa_key_type = alg_to_psa_key_type(alg);
    if (psa_key_type == 0) {
        wpa_printf(MSG_ERROR, "%s: invalid key type", __func__);
        goto cleanup;
    }
    psa_set_key_type(&attributes, psa_key_type);
    psa_set_key_bits(&attributes, key_len * 8);

    status = psa_import_key(&attributes, key, key_len, &key_id);
    if (status != PSA_SUCCESS) {
        wpa_printf(MSG_ERROR, "%s: psa_import_key failed", __func__);
        goto cleanup;
    }

    psa_reset_key_attributes(&attributes);

    enc_operation = os_zalloc(sizeof(psa_cipher_operation_t));
    if (!enc_operation) {
        wpa_printf(MSG_ERROR, "%s: os_zalloc failed", __func__);
        goto cleanup;
    }

    ctx->ctx_enc = (void *)enc_operation;

    status = psa_cipher_encrypt_setup(enc_operation, key_id, psa_alg);
    if (status != PSA_SUCCESS) {
        wpa_printf(MSG_ERROR, "%s: psa_cipher_encrypt_setup failed", __func__);
        goto cleanup;
    }

    status = psa_cipher_set_iv(enc_operation, iv, 16);
    if (status != PSA_SUCCESS) {
        wpa_printf(MSG_ERROR, "%s: psa_cipher_set_iv failed", __func__);
        goto cleanup;
    }

    dec_operation = os_zalloc(sizeof(psa_cipher_operation_t));
    if (!dec_operation) {
        wpa_printf(MSG_ERROR, "%s: os_zalloc failed", __func__);
        goto cleanup;
    }

    ctx->ctx_dec = (void *)dec_operation;

    status = psa_cipher_decrypt_setup(dec_operation, key_id, psa_alg);
    if (status != PSA_SUCCESS) {
        wpa_printf(MSG_ERROR, "%s: psa_cipher_decrypt_setup failed", __func__);
        goto cleanup;
    }

    status = psa_cipher_set_iv(dec_operation, iv, 16);
    if (status != PSA_SUCCESS) {
        wpa_printf(MSG_ERROR, "%s: psa_cipher_set_iv failed", __func__);
        goto cleanup;
    }

    ctx->key_id = key_id;

    return ctx;

cleanup:
    if (key_id) {
        psa_destroy_key(key_id);
    }
    if (enc_operation) {
        psa_cipher_abort(enc_operation);
        os_free(enc_operation);
    }
    if (dec_operation) {
        psa_cipher_abort(dec_operation);
        os_free(dec_operation);
    }
    psa_reset_key_attributes(&attributes);
    os_free(ctx);
    return NULL;
}

int crypto_cipher_encrypt(struct crypto_cipher *ctx, const u8 *plain,
                          u8 *crypt, size_t len)
{
    psa_status_t status;
    psa_cipher_operation_t *operation = (psa_cipher_operation_t *)ctx->ctx_enc;

    size_t output_length = 0;

    status = psa_cipher_update(operation, plain, len, crypt, len, &output_length);
    if (status != PSA_SUCCESS) {
        wpa_printf(MSG_ERROR, "%s: psa_cipher_update failed", __func__);
        return -1;
    }

    status = psa_cipher_finish(operation, crypt + output_length, len - output_length, &output_length);
    if (status != PSA_SUCCESS) {
        wpa_printf(MSG_ERROR, "%s: psa_cipher_finish failed", __func__);
        return -1;
    }

    return 0;
}

int crypto_cipher_decrypt(struct crypto_cipher *ctx, const u8 *crypt,
                          u8 *plain, size_t len)
{
    psa_status_t status;
    psa_cipher_operation_t *operation = (psa_cipher_operation_t *)ctx->ctx_dec;

    size_t output_length = 0;

    status = psa_cipher_update(operation, crypt, len, plain, len, &output_length);
    if (status != PSA_SUCCESS) {
        wpa_printf(MSG_ERROR, "%s: psa_cipher_update failed", __func__);
        return -1;
    }

    status = psa_cipher_finish(operation, plain + output_length, len - output_length, &output_length);
    if (status != PSA_SUCCESS) {
        wpa_printf(MSG_ERROR, "%s: psa_cipher_finish failed", __func__);
        return -1;
    }

    return 0;
}

void crypto_cipher_deinit(struct crypto_cipher *ctx)
{
    psa_status_t status;
    psa_cipher_abort((psa_cipher_operation_t *)ctx->ctx_enc);
    psa_cipher_abort((psa_cipher_operation_t *)ctx->ctx_dec);
    status = psa_destroy_key(ctx->key_id);
    if (status != PSA_SUCCESS) {
        wpa_printf(MSG_ERROR, "%s: psa_destroy_key failed", __func__);
    }
    os_free(ctx->ctx_enc);
    os_free(ctx->ctx_dec);
    os_free(ctx);
}
#endif

#ifdef CONFIG_MBEDTLS_CIPHER_MODE_CTR
int aes_ctr_encrypt(const u8 *key, size_t key_len, const u8 *nonce,
                    u8 *data, size_t data_len)
{
    psa_status_t status;
    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_key_id_t key_id = 0;
    psa_cipher_operation_t operation = PSA_CIPHER_OPERATION_INIT;
    int ret = -1;
    u8 *temp_buf = NULL;

    psa_set_key_usage_flags(&attributes, PSA_KEY_USAGE_ENCRYPT);
    psa_set_key_algorithm(&attributes, PSA_ALG_CTR);
    psa_set_key_type(&attributes, PSA_KEY_TYPE_AES);
    psa_set_key_bits(&attributes, key_len * 8);

    status = psa_import_key(&attributes, key, key_len, &key_id);
    if (status != PSA_SUCCESS) {
        wpa_printf(MSG_ERROR, "%s: psa_import_key failed", __func__);
        goto cleanup;
    }

    psa_reset_key_attributes(&attributes);

    status = psa_cipher_encrypt_setup(&operation, key_id, PSA_ALG_CTR);
    if (status != PSA_SUCCESS) {
        wpa_printf(MSG_ERROR, "%s: psa_cipher_encrypt_setup failed", __func__);
        goto cleanup;
    }

    status = psa_cipher_set_iv(&operation, nonce, 16);
    if (status != PSA_SUCCESS) {
        wpa_printf(MSG_ERROR, "%s: psa_cipher_set_iv failed", __func__);
        goto cleanup;
    }

    /* Use temporary buffer only when data length is not a multiple of 16 bytes
     * to avoid driver restrictions on in-place encryption */
    const size_t AES_BLOCK_SIZE = 16;
    const int need_temp_buf = (data_len % AES_BLOCK_SIZE != 0);

    if (need_temp_buf) {
        temp_buf = os_malloc(data_len);
        if (temp_buf == NULL) {
            wpa_printf(MSG_ERROR, "%s: os_malloc failed", __func__);
            goto cleanup;
        }
    }

    size_t output_length = 0;
    size_t total_output = 0;
    u8 *output_buf = need_temp_buf ? temp_buf : data;

    status = psa_cipher_update(&operation, data, data_len, output_buf, data_len, &output_length);
    if (status != PSA_SUCCESS) {
        wpa_printf(MSG_ERROR, "%s: psa_cipher_update failed", __func__);
        goto cleanup;
    }
    total_output += output_length;

    size_t finish_output = 0;
    status = psa_cipher_finish(&operation, output_buf + total_output, data_len - total_output, &finish_output);
    if (status != PSA_SUCCESS) {
        wpa_printf(MSG_ERROR, "%s: psa_cipher_finish failed", __func__);
        goto cleanup;
    }
    total_output += finish_output;

    /* Copy result back to original buffer if we used temporary buffer */
    if (need_temp_buf) {
        os_memcpy(data, temp_buf, data_len);
    }

    ret = 0;

cleanup:
    if (temp_buf) {
        os_free(temp_buf);
    }
    psa_cipher_abort(&operation);
    if (key_id) {
        psa_destroy_key(key_id);
    }

    return ret;
}
#endif /* CONFIG_MBEDTLS_CIPHER_MODE_CTR */

int aes_128_ctr_encrypt(const u8 *key, const u8 *nonce,
                        u8 *data, size_t data_len)
{
    return aes_ctr_encrypt(key, 16, nonce, data, data_len);
}

#ifdef MBEDTLS_NIST_KW_C
int aes_wrap(const u8 *kek, size_t kek_len, int n, const u8 *plain, u8 *cipher)
{
    mbedtls_nist_kw_context ctx;
    size_t olen;
    int ret = 0;
    mbedtls_nist_kw_init(&ctx);

    ret = mbedtls_nist_kw_setkey(&ctx, MBEDTLS_CIPHER_ID_AES,
                                 kek, kek_len * 8, 1);
    if (ret != 0) {
        return ret;
    }

    ret = mbedtls_nist_kw_wrap(&ctx, MBEDTLS_KW_MODE_KW, plain,
                               n * 8, cipher, &olen, (n + 1) * 8);

    mbedtls_nist_kw_free(&ctx);
    return ret;
}

int aes_unwrap(const u8 *kek, size_t kek_len, int n, const u8 *cipher,
               u8 *plain)
{
    mbedtls_nist_kw_context ctx;
    size_t olen;
    int ret = 0;
    mbedtls_nist_kw_init(&ctx);

    ret = mbedtls_nist_kw_setkey(&ctx, MBEDTLS_CIPHER_ID_AES,
                                 kek, kek_len * 8, 0);
    if (ret != 0) {
        return ret;
    }

    ret = mbedtls_nist_kw_unwrap(&ctx, MBEDTLS_KW_MODE_KW, cipher,
                                 (n + 1) * 8, plain, &olen, (n * 8));

    mbedtls_nist_kw_free(&ctx);
    return ret;
}
#endif

int crypto_mod_exp(const uint8_t *base, size_t base_len,
                   const uint8_t *power, size_t power_len,
                   const uint8_t *modulus, size_t modulus_len,
                   uint8_t *result, size_t *result_len)
{
    mbedtls_mpi bn_base, bn_exp, bn_modulus, bn_result, bn_rinv;
    int ret = 0;

    mbedtls_mpi_init(&bn_base);
    mbedtls_mpi_init(&bn_exp);
    mbedtls_mpi_init(&bn_modulus);
    mbedtls_mpi_init(&bn_result);
    mbedtls_mpi_init(&bn_rinv);

    MBEDTLS_MPI_CHK(mbedtls_mpi_read_binary(&bn_base, base, base_len));
    MBEDTLS_MPI_CHK(mbedtls_mpi_read_binary(&bn_exp, power, power_len));
    MBEDTLS_MPI_CHK(mbedtls_mpi_read_binary(&bn_modulus, modulus, modulus_len));

    MBEDTLS_MPI_CHK(mbedtls_mpi_exp_mod(&bn_result, &bn_base, &bn_exp, &bn_modulus,
                                        &bn_rinv));

    ret = mbedtls_mpi_write_binary(&bn_result, result, *result_len);

cleanup:
    mbedtls_mpi_free(&bn_base);
    mbedtls_mpi_free(&bn_exp);
    mbedtls_mpi_free(&bn_modulus);
    mbedtls_mpi_free(&bn_result);
    mbedtls_mpi_free(&bn_rinv);

    return ret;
}

#if defined(CONFIG_MBEDTLS_SHA1_C) || defined(CONFIG_MBEDTLS_HARDWARE_SHA)
int pbkdf2_sha1(const char *passphrase, const u8 *ssid, size_t ssid_len,
                int iterations, u8 *buf, size_t buflen)
{
#ifdef CONFIG_FAST_PBKDF2
    /* For ESP32: Using pbkdf2_hmac_sha1() because esp_fast_psk() utilizes hardware,
     * but for ESP32, the SHA1 hardware implementation is slower than the software implementation.
     */
#if defined(CONFIG_IDF_TARGET_ESP32) || !defined(CONFIG_SOC_SHA_SUPPORTED)
    fastpbkdf2_hmac_sha1((const u8 *) passphrase, os_strlen(passphrase),
                         ssid, ssid_len, iterations, buf, buflen);
    return 0;
#else
    return esp_fast_psk(passphrase, os_strlen(passphrase), ssid, ssid_len, iterations, buf, buflen);
#endif
#else
    int ret = mbedtls_pkcs5_pbkdf2_hmac_ext(MBEDTLS_MD_SHA1, (const u8 *) passphrase,
                                            os_strlen(passphrase), ssid,
                                            ssid_len, iterations, buflen, buf);
    return ret == 0 ? 0 : -1;
#endif
}
#endif /* defined(CONFIG_MBEDTLS_SHA1_C) || defined(CONFIG_MBEDTLS_HARDWARE_SHA) */

#ifdef MBEDTLS_DES_C
int des_encrypt(const u8 *clear, const u8 *key, u8 *cypher)
{
    psa_status_t status;
    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_key_id_t key_id = 0;
    psa_cipher_operation_t operation = PSA_CIPHER_OPERATION_INIT;
    int ret = -1;

    psa_set_key_usage_flags(&attributes, PSA_KEY_USAGE_ENCRYPT);
    psa_set_key_algorithm(&attributes, PSA_ALG_ECB_NO_PADDING);
    psa_set_key_type(&attributes, PSA_KEY_TYPE_DES);
    psa_set_key_bits(&attributes, 128);

    status = psa_import_key(&attributes, key, 8, &key_id);
    psa_reset_key_attributes(&attributes);
    if (status != PSA_SUCCESS) {
        wpa_printf(MSG_ERROR, "%s: psa_import_key failed", __func__);
        return -1;
    }

    status = psa_cipher_encrypt_setup(&operation, key_id, PSA_ALG_ECB_NO_PADDING);
    if (status != PSA_SUCCESS) {
        wpa_printf(MSG_ERROR, "%s: psa_cipher_encrypt_setup failed", __func__);
        goto cleanup;
    }

    size_t output_length = 0;

    status = psa_cipher_update(&operation, clear, 8, cypher, 8, &output_length);
    if (status != PSA_SUCCESS) {
        wpa_printf(MSG_ERROR, "%s: psa_cipher_update failed", __func__);
        goto cleanup;
    }

    status = psa_cipher_finish(&operation, cypher + output_length, 8 - output_length, &output_length);
    if (status != PSA_SUCCESS) {
        wpa_printf(MSG_ERROR, "%s: psa_cipher_finish failed", __func__);
        goto cleanup;
    }

    ret = 0;

cleanup:
    psa_cipher_abort(&operation);
    if (key_id) {
        psa_destroy_key(key_id);
    }

    return ret;
}
#endif

/* Only enable this if all other ciphers are using MbedTLS implementation */
#if defined(MBEDTLS_CCM_C) && defined(MBEDTLS_CMAC_C) && defined(MBEDTLS_NIST_KW_C)
int aes_ccm_ae(const u8 *key, size_t key_len, const u8 *nonce,
               size_t M, const u8 *plain, size_t plain_len,
               const u8 *aad, size_t aad_len, u8 *crypt, u8 *auth)
{
    psa_status_t status;
    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_key_id_t key_id;

    psa_set_key_usage_flags(&attributes, PSA_KEY_USAGE_ENCRYPT);
    psa_set_key_algorithm(&attributes, PSA_ALG_CCM);
    psa_set_key_type(&attributes, PSA_KEY_TYPE_AES);
    psa_set_key_bits(&attributes, key_len * 8);

    status = psa_import_key(&attributes, key, key_len, &key_id);
    if (status != PSA_SUCCESS) {
        wpa_printf(MSG_ERROR, "%s: psa_import_key failed", __func__);
        return -1;
    }

    psa_reset_key_attributes(&attributes);

    psa_aead_operation_t operation = PSA_AEAD_OPERATION_INIT;

    status = psa_aead_encrypt_setup(&operation, key_id, PSA_ALG_CCM);
    if (status != PSA_SUCCESS) {
        wpa_printf(MSG_ERROR, "%s: psa_aead_encrypt_setup failed", __func__);
        psa_destroy_key(key_id);
        return -1;
    }

    status = psa_aead_set_nonce(&operation, nonce, 13);
    if (status != PSA_SUCCESS) {
        wpa_printf(MSG_ERROR, "%s: psa_aead_set_nonce failed", __func__);
        psa_aead_abort(&operation);
        psa_destroy_key(key_id);
        return -1;
    }

    status = psa_aead_set_lengths(&operation, aad_len, plain_len);
    if (status != PSA_SUCCESS) {
        wpa_printf(MSG_ERROR, "%s: psa_aead_set_lengths failed", __func__);
        psa_aead_abort(&operation);
        psa_destroy_key(key_id);
        return -1;
    }

    size_t output_length = 0;
    size_t tag_len = 0;

    status = psa_aead_update_ad(&operation, aad, aad_len);
    if (status != PSA_SUCCESS) {
        wpa_printf(MSG_ERROR, "%s: psa_aead_update_ad failed", __func__);
        psa_aead_abort(&operation);
        psa_destroy_key(key_id);
        return -1;
    }

    status = psa_aead_update(&operation, plain, plain_len, crypt, plain_len, &output_length);
    if (status != PSA_SUCCESS) {
        wpa_printf(MSG_ERROR, "%s: psa_aead_update failed", __func__);
        psa_aead_abort(&operation);
        psa_destroy_key(key_id);
        return -1;
    }

    size_t finish_output = 0;
    status = psa_aead_finish(&operation, crypt + output_length, plain_len - output_length, &finish_output, auth, M, &tag_len);
    if (status != PSA_SUCCESS) {
        wpa_printf(MSG_ERROR, "%s: psa_aead_finish failed, status: %d", __func__, status);
        psa_aead_abort(&operation);
        psa_destroy_key(key_id);
        return -1;
    }

    psa_aead_abort(&operation);

    psa_destroy_key(key_id);

    return 0;
}

int aes_ccm_ad(const u8 *key, size_t key_len, const u8 *nonce,
               size_t M, const u8 *crypt, size_t crypt_len,
               const u8 *aad, size_t aad_len, const u8 *auth,
               u8 *plain)
{
    psa_status_t status;
    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_key_id_t key_id;
    u8 *ciphertext_with_tag = NULL;
    size_t plaintext_length = 0;
    int ret = -1;

    psa_set_key_usage_flags(&attributes, PSA_KEY_USAGE_DECRYPT);
    psa_set_key_algorithm(&attributes, PSA_ALG_CCM);
    psa_set_key_type(&attributes, PSA_KEY_TYPE_AES);
    psa_set_key_bits(&attributes, key_len * 8);

    status = psa_import_key(&attributes, key, key_len, &key_id);
    psa_reset_key_attributes(&attributes);
    if (status != PSA_SUCCESS) {
        wpa_printf(MSG_ERROR, "%s: psa_import_key failed", __func__);
        return -1;
    }

    /* psa_aead_decrypt expects the tag to be appended to the ciphertext */
    ciphertext_with_tag = os_malloc(crypt_len + M);
    if (ciphertext_with_tag == NULL) {
        wpa_printf(MSG_ERROR, "%s: os_malloc failed", __func__);
        goto cleanup;
    }
    os_memcpy(ciphertext_with_tag, crypt, crypt_len);
    os_memcpy(ciphertext_with_tag + crypt_len, auth, M);

    status = psa_aead_decrypt(key_id, PSA_ALG_CCM,
                              nonce, 13,
                              aad, aad_len,
                              ciphertext_with_tag, crypt_len + M,
                              plain, crypt_len, &plaintext_length);
    if (status != PSA_SUCCESS) {
        wpa_printf(MSG_ERROR, "%s: psa_aead_decrypt failed, status: %d", __func__, status);
        goto cleanup;
    }

    if (plaintext_length != crypt_len) {
        wpa_printf(MSG_ERROR, "%s: plaintext length mismatch: expected %zu, got %zu", __func__, crypt_len, plaintext_length);
        goto cleanup;
    }

    ret = 0;

cleanup:
    if (ciphertext_with_tag) {
        os_free(ciphertext_with_tag);
    }
    psa_destroy_key(key_id);

    return ret;
}
#endif

#ifdef MBEDTLS_CMAC_C
int omac1_aes_vector(const u8 *key, size_t key_len, size_t num_elem,
                     const u8 *addr[], const size_t *len, u8 *mac)
{
    if (key == NULL || mac == NULL) {
        return -1;
    }

    psa_status_t status;
    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_key_id_t key_id = 0;
    psa_mac_operation_t operation = PSA_MAC_OPERATION_INIT;
    int ret = -1;

    psa_set_key_usage_flags(&attributes, PSA_KEY_USAGE_SIGN_HASH);
    psa_set_key_algorithm(&attributes, PSA_ALG_CMAC);
    psa_set_key_type(&attributes, PSA_KEY_TYPE_AES);
    psa_set_key_bits(&attributes, key_len * 8);

    status = psa_import_key(&attributes, key, key_len, &key_id);
    if (status != PSA_SUCCESS) {
        wpa_printf(MSG_ERROR, "%s: psa_import_key failed", __func__);
        goto cleanup;
    }

    psa_reset_key_attributes(&attributes);

    status = psa_mac_sign_setup(&operation, key_id, PSA_ALG_CMAC);
    if (status != PSA_SUCCESS) {
        wpa_printf(MSG_ERROR, "%s: psa_mac_sign_setup failed", __func__);
        goto cleanup;
    }

    for (int i = 0; i < num_elem; i++) {
        status = psa_mac_update(&operation, addr[i], len[i]);
        if (status != PSA_SUCCESS) {
            wpa_printf(MSG_ERROR, "%s: psa_mac_update failed", __func__);
            goto cleanup;
        }
    }

    size_t output_length = 0;

    status = psa_mac_sign_finish(&operation, mac, 16, &output_length);
    if (status != PSA_SUCCESS) {
        wpa_printf(MSG_ERROR, "%s: psa_mac_sign_finish failed", __func__);
        goto cleanup;
    }

    ret = 0;

cleanup:
    psa_mac_abort(&operation);
    if (key_id != 0) {
        psa_destroy_key(key_id);
    }

    return ret;

}

int omac1_aes_128_vector(const u8 *key, size_t num_elem,
                         const u8 *addr[], const size_t *len, u8 *mac)
{
    return omac1_aes_vector(key, 16, num_elem, addr, len, mac);
}

int omac1_aes_128(const u8 *key, const u8 *data, size_t data_len, u8 *mac)
{
    return omac1_aes_128_vector(key, 1, &data, &data_len, mac);
}
#endif

int crypto_dh_init(u8 generator, const u8 *prime, size_t prime_len, u8 *privkey,
                   u8 *pubkey)
{
    size_t pubkey_len, pad;

    if (os_get_random(privkey, prime_len) < 0) {
        return -1;
    }
    if (os_memcmp(privkey, prime, prime_len) > 0) {
        /* Make sure private value is smaller than prime */
        privkey[0] = 0;
    }

    pubkey_len = prime_len;
    if (crypto_mod_exp(&generator, 1, privkey, prime_len, prime, prime_len,
                       pubkey, &pubkey_len) < 0) {
        return -1;
    }
    if (pubkey_len < prime_len) {
        pad = prime_len - pubkey_len;
        os_memmove(pubkey + pad, pubkey, pubkey_len);
        os_memset(pubkey, 0, pad);
    }

    return 0;
}

int crypto_global_init(void)
{
    return 0;
}

void crypto_global_deinit(void)
{
}
