/*
 * SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>

#include "esp_log.h"
#include "esp_cpu.h"
#include "esp_fault.h"
#include "esp_flash.h"
#include "esp_efuse.h"
#include "esp_efuse_chip.h"
#include "esp_hmac.h"
#include "esp_random.h"
#include "spi_flash_mmap.h"

#include "mbedtls/aes.h"
#include "mbedtls/gcm.h"
#include "mbedtls/sha256.h"
#include "mbedtls/ecdsa.h"
#include "mbedtls/error.h"
#include "esp_hmac_pbkdf2.h"

#include "esp_rom_sys.h"
#include "nvs.h"
#include "nvs_flash.h"

#include "esp_tee.h"
#include "esp_tee_flash.h"
#include "esp_tee_sec_storage.h"
#include "secure_service_num.h"

#define AES256_KEY_LEN                      32
#define AES256_KEY_BITS                     (AES256_KEY_LEN * 8)
#define AES256_DEFAULT_IV_LEN               16
#define AES256_GCM_IV_LEN                   12
#define ECDSA_SECP256R1_KEY_LEN             32
#define ECDSA_SECP192R1_KEY_LEN             24

#define SHA256_DIGEST_SZ                    32

#define EKEY_SEED 0xAEBE5A5A
#define TKEY_SEED 0xCEDEA5A5

#define PBKDF2_HMAC_ITER 2048

/* Structure to hold ECDSA SECP256R1 key pair */
typedef struct {
    uint8_t priv_key[ECDSA_SECP256R1_KEY_LEN];     /* Private key for ECDSA SECP256R1 */
    uint8_t pub_key[2 * ECDSA_SECP256R1_KEY_LEN];  /* Public key for ECDSA SECP256R1 (X and Y coordinates) */
} __attribute__((aligned(4))) __attribute__((__packed__)) sec_stg_ecdsa_secp256r1_t;

/* Structure to hold ECDSA SECP192R1 key pair */
typedef struct {
    uint8_t priv_key[ECDSA_SECP192R1_KEY_LEN];     /* Private key for ECDSA SECP192R1 */
    uint8_t pub_key[2 * ECDSA_SECP192R1_KEY_LEN];  /* Public key for ECDSA SECP192R1 (X and Y coordinates) */
} __attribute__((aligned(4))) __attribute__((__packed__)) sec_stg_ecdsa_secp192r1_t;

/* Structure to hold AES-256 key and IV */
typedef struct {
    uint8_t key[AES256_KEY_LEN];          /* Key for AES-256 */
    uint8_t iv[AES256_DEFAULT_IV_LEN];    /* Initialization vector for AES-256 */
} __attribute__((aligned(4))) __attribute__((__packed__)) sec_stg_aes256_t;

/* Structure to hold the cryptographic keys in NVS */
typedef struct {
    const esp_tee_sec_storage_type_t type;          /* Type of the key */
    uint32_t flags;                                 /* Flags associated with the key */
    union {
        sec_stg_ecdsa_secp256r1_t ecdsa_secp256r1;  /* ECDSA SECP256R1 key pair */
        sec_stg_ecdsa_secp192r1_t ecdsa_secp192r1;  /* ECDSA SECP192R1 key pair */
        sec_stg_aes256_t aes256;                    /* AES-256 key and IV */
    };
    uint32_t reserved[38];                          /* Reserved space for future use */
} __attribute__((aligned(4))) __attribute__((__packed__)) sec_stg_key_t;

_Static_assert(sizeof(sec_stg_key_t) == 256, "Incorrect sec_stg_key_t size");

static nvs_handle_t tee_nvs_hdl;

static const char *TAG = "secure_storage";

/* ---------------------------------------------- Helper APIs ------------------------------------------------- */
#if CONFIG_SECURE_TEE_SEC_STG_EFUSE_HMAC_KEY_ID < 0
#error "TEE Secure Storage: Configured eFuse block (CONFIG_SECURE_TEE_SEC_STG_EFUSE_HMAC_KEY_ID) out of range!"
#endif

#if CONFIG_SECURE_TEE_SEC_STG_EFUSE_HMAC_KEY_ID == CONFIG_SECURE_TEE_PBKDF2_EFUSE_HMAC_KEY_ID
#error "TEE Secure Storage: Configured eFuse block for storage encryption keys (CONFIG_SECURE_TEE_SEC_STG_EFUSE_HMAC_KEY_ID) and PBKDF2 key derivation (CONFIG_SECURE_TEE_PBKDF2_EFUSE_HMAC_KEY_ID) cannot be the same!"
#endif

static int buffer_hexdump(const char *label, const void *buffer, size_t length)
{
#if CONFIG_SECURE_TEE_LOG_LEVEL >= 4
    if (label == NULL || buffer == NULL || length == 0) {
        return -1;
    }

    const uint8_t *bytes = (const uint8_t *)buffer;
    const size_t max_bytes_per_line = 16;
    char hexbuf[max_bytes_per_line * 3];

    ESP_LOGD(TAG, "%s -", label);

    for (size_t i = 0; i < length; i += max_bytes_per_line) {
        size_t chunk_len = MIN(max_bytes_per_line, length - i);
        size_t hexbuf_idx = 0;

        for (size_t j = 0; j < chunk_len; j++) {
            uint8_t byte = bytes[i + j];
            hexbuf[hexbuf_idx++] = (byte >> 4) < 10 ? '0' + (byte >> 4) : 'a' + (byte >> 4) - 10;
            hexbuf[hexbuf_idx++] = (byte & 0x0F) < 10 ? '0' + (byte & 0x0F) : 'a' + (byte & 0x0F) - 10;

            hexbuf[hexbuf_idx++] = ' ';
            if ((j + 1) % 8 == 0 && j + 1 < chunk_len) {
                hexbuf[hexbuf_idx++] = ' ';
            }
        }
        hexbuf[hexbuf_idx] = '\0';
        esp_rom_printf("%s\n", hexbuf);
    }
#else
    (void) label;
    (void) buffer;
    (void) length;
#endif

    return 0;
}

static int rand_func(void *rng_state, unsigned char *output, size_t len)
{
    esp_fill_random(output, len);
    return 0;
}

#if CONFIG_SECURE_TEE_SEC_STG_MODE_RELEASE
static esp_err_t compute_nvs_keys_with_hmac(hmac_key_id_t hmac_key_id, nvs_sec_cfg_t *cfg)
{
    uint32_t ekey_seed[8] = {[0 ... 7] = EKEY_SEED};
    uint32_t tkey_seed[8] = {[0 ... 7] = TKEY_SEED};

    memset(cfg, 0x00, sizeof(nvs_sec_cfg_t));

    esp_err_t err = ESP_FAIL;
    err = esp_hmac_calculate(hmac_key_id, ekey_seed, sizeof(ekey_seed), (uint8_t *)cfg->eky);
    err |= esp_hmac_calculate(hmac_key_id, tkey_seed, sizeof(tkey_seed), (uint8_t *)cfg->tky);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to calculate seed HMAC");
        return ESP_FAIL;
    }
    ESP_FAULT_ASSERT(err == ESP_OK);

    /* NOTE: If the XTS E-key and T-key are the same, we have a hash collision */
    ESP_FAULT_ASSERT(memcmp(cfg->eky, cfg->tky, NVS_KEY_SIZE) != 0);

    return ESP_OK;
}
#endif

static esp_err_t read_security_cfg_hmac(nvs_sec_cfg_t *cfg)
{
    if (cfg == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

#if CONFIG_SECURE_TEE_SEC_STG_MODE_RELEASE
    esp_efuse_block_t hmac_key_blk = (esp_efuse_block_t)(EFUSE_BLK_KEY0 + (esp_efuse_block_t)CONFIG_SECURE_TEE_SEC_STG_EFUSE_HMAC_KEY_ID);
    esp_efuse_purpose_t hmac_efuse_blk_purpose = esp_efuse_get_key_purpose(hmac_key_blk);
    if (hmac_efuse_blk_purpose != ESP_EFUSE_KEY_PURPOSE_HMAC_UP) {
        ESP_LOGE(TAG, "HMAC key is not burnt in eFuse block");
        return ESP_ERR_NOT_FOUND;
    }

    esp_err_t err = compute_nvs_keys_with_hmac((hmac_key_id_t)CONFIG_SECURE_TEE_SEC_STG_EFUSE_HMAC_KEY_ID, cfg);
    if (err != ESP_OK) {
        return err;
    }
#else
    memset(&cfg->eky, 0x33, sizeof(cfg->eky));
    memset(&cfg->tky, 0xCC, sizeof(cfg->tky));
#endif

    return ESP_OK;
}

static esp_err_t secure_storage_find_key(const char *key_id)
{
    nvs_type_t out_type;
    return nvs_find_key(tee_nvs_hdl, key_id, &out_type);
}

static esp_err_t secure_storage_write(const char *key_id, const void *data, size_t len)
{
    esp_err_t err = nvs_set_blob(tee_nvs_hdl, key_id, data, len);
    if (err != ESP_OK) {
        return err;
    }

    err = nvs_commit(tee_nvs_hdl);
    return err;
}

static esp_err_t secure_storage_read(const char *key_id, void *data, size_t *len)
{
    return nvs_get_blob(tee_nvs_hdl, key_id, data, len);
}

/* ---------------------------------------------- Interface APIs ------------------------------------------------- */

esp_err_t esp_tee_sec_storage_init(void)
{
    nvs_sec_cfg_t cfg = {};
    esp_err_t err = read_security_cfg_hmac(&cfg);
    if (err != ESP_OK) {
        return err;
    }

    err = nvs_flash_secure_init_partition(ESP_TEE_SEC_STG_PART_LABEL, &cfg);
    if (err != ESP_OK) {
        return err;
    }

    err = nvs_open_from_partition(ESP_TEE_SEC_STG_PART_LABEL, ESP_TEE_SEC_STG_NVS_NAMESPACE, NVS_READWRITE, &tee_nvs_hdl);
    if (err != ESP_OK) {
        return err;
    }

#if CONFIG_SECURE_TEE_SEC_STG_MODE_DEVELOPMENT
    ESP_LOGW(TAG, "TEE Secure Storage enabled in insecure DEVELOPMENT mode");
#endif

    return ESP_OK;
}

esp_err_t esp_tee_sec_storage_clear_key(const char *key_id)
{
    if (secure_storage_find_key(key_id) != ESP_OK) {
        return ESP_ERR_NOT_FOUND;
    }

    sec_stg_key_t keyctx;
    size_t keyctx_len = sizeof(keyctx);

    esp_err_t err = secure_storage_read(key_id, (void *)&keyctx, &keyctx_len);
    if (err != ESP_OK) {
        return err;
    }

    if (keyctx.flags & SEC_STORAGE_FLAG_WRITE_ONCE) {
        ESP_LOGE(TAG, "Key is write-once only and cannot be cleared!");
        return ESP_ERR_INVALID_STATE;
    }

    err = nvs_erase_key(tee_nvs_hdl, key_id);
    if (err != ESP_OK) {
        return err;
    }

    err = nvs_commit(tee_nvs_hdl);
    return err;
}

static int generate_ecdsa_key(sec_stg_key_t *keyctx, esp_tee_sec_storage_type_t key_type)
{
    if (keyctx == NULL) {
        return -1;
    }

    mbedtls_ecp_group_id curve_id = MBEDTLS_ECP_DP_SECP256R1;
    size_t key_len = ECDSA_SECP256R1_KEY_LEN;

    if (key_type == ESP_SEC_STG_KEY_ECDSA_SECP192R1) {
#if CONFIG_SECURE_TEE_SEC_STG_SUPPORT_SECP192R1_SIGN
        curve_id = MBEDTLS_ECP_DP_SECP192R1;
        key_len = ECDSA_SECP192R1_KEY_LEN;
#else
        ESP_LOGE(TAG, "Unsupported key-type!");
        return -1;
#endif
    }

    ESP_LOGD(TAG, "Generating ECDSA key for curve %d...", curve_id);

    mbedtls_ecdsa_context ctxECDSA;
    mbedtls_ecdsa_init(&ctxECDSA);

    int ret = mbedtls_ecdsa_genkey(&ctxECDSA, curve_id, rand_func, NULL);
    if (ret != 0) {
        goto exit;
    }

    uint8_t *priv_key = (key_type == ESP_SEC_STG_KEY_ECDSA_SECP256R1) ?
                        keyctx->ecdsa_secp256r1.priv_key :
#if CONFIG_SECURE_TEE_SEC_STG_SUPPORT_SECP192R1_SIGN
                        keyctx->ecdsa_secp192r1.priv_key;
#else
                        NULL;
#endif

    uint8_t *pub_key = (key_type == ESP_SEC_STG_KEY_ECDSA_SECP256R1) ?
                       keyctx->ecdsa_secp256r1.pub_key :
#if CONFIG_SECURE_TEE_SEC_STG_SUPPORT_SECP192R1_SIGN
                       keyctx->ecdsa_secp192r1.pub_key;
#else
                       NULL;
#endif

    ret = mbedtls_mpi_write_binary(&(ctxECDSA.MBEDTLS_PRIVATE(Q).MBEDTLS_PRIVATE(X)), pub_key, key_len);
    if (ret != 0) {
        goto exit;
    }

    ret = mbedtls_mpi_write_binary(&(ctxECDSA.MBEDTLS_PRIVATE(Q).MBEDTLS_PRIVATE(Y)), pub_key + key_len, key_len);
    if (ret != 0) {
        goto exit;
    }

    ret = mbedtls_mpi_write_binary(&ctxECDSA.MBEDTLS_PRIVATE(d), priv_key, key_len);
    if (ret != 0) {
        goto exit;
    }

    buffer_hexdump("Private key", priv_key, key_len);
    buffer_hexdump("Public key", pub_key, key_len * 2);

exit:
    mbedtls_ecdsa_free(&ctxECDSA);
    return ret;
}

static int generate_aes256_key(sec_stg_key_t *keyctx)
{
    if (keyctx == NULL) {
        return -1;
    }

    ESP_LOGD(TAG, "Generating AES-256 key...");

    esp_fill_random(&keyctx->aes256.key, AES256_KEY_LEN);
    esp_fill_random(&keyctx->aes256.iv, AES256_DEFAULT_IV_LEN);

    return 0;
}

esp_err_t esp_tee_sec_storage_gen_key(const esp_tee_sec_storage_key_cfg_t *cfg)
{
    if (cfg == NULL || cfg->id == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    if (secure_storage_find_key(cfg->id) == ESP_OK) {
        ESP_LOGE(TAG, "Key ID already exists");
        return ESP_ERR_INVALID_STATE;
    }

    sec_stg_key_t keyctx = {
        .type = cfg->type,
        .flags = cfg->flags,
    };

    switch (cfg->type) {
    case ESP_SEC_STG_KEY_ECDSA_SECP256R1:
#if CONFIG_SECURE_TEE_SEC_STG_SUPPORT_SECP192R1_SIGN
    case ESP_SEC_STG_KEY_ECDSA_SECP192R1:
#endif
        if (generate_ecdsa_key(&keyctx, cfg->type) != 0) {
            ESP_LOGE(TAG, "Failed to generate ECDSA keypair");
            return ESP_FAIL;
        }
        break;
    case ESP_SEC_STG_KEY_AES256:
        if (generate_aes256_key(&keyctx) != 0) {
            ESP_LOGE(TAG, "Failed to generate AES key");
            return ESP_FAIL;
        }
        break;
    default:
        ESP_LOGE(TAG, "Unsupported key-type!");
        return ESP_ERR_NOT_SUPPORTED;
    }

    return secure_storage_write(cfg->id, (void *)&keyctx, sizeof(keyctx));
}

esp_err_t esp_tee_sec_storage_ecdsa_sign(const esp_tee_sec_storage_key_cfg_t *cfg, const uint8_t *hash, size_t hlen, esp_tee_sec_storage_ecdsa_sign_t *out_sign)
{
    if (cfg == NULL || cfg->id == NULL || hash == NULL || out_sign == NULL || hlen == 0) {
        return ESP_ERR_INVALID_ARG;
    }

#if !CONFIG_SECURE_TEE_SEC_STG_SUPPORT_SECP192R1_SIGN
    if (cfg->type == ESP_SEC_STG_KEY_ECDSA_SECP192R1) {
        ESP_LOGE(TAG, "Unsupported key-type!");
        return ESP_ERR_NOT_SUPPORTED;
    }
#endif

    esp_err_t err = secure_storage_find_key(cfg->id);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Key ID not found");
        return err;
    }

    sec_stg_key_t keyctx;
    size_t keyctx_len = sizeof(keyctx);
    err = secure_storage_read(cfg->id, (void *)&keyctx, &keyctx_len);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to fetch key from storage");
        return err;
    }

    if (keyctx.type != cfg->type) {
        ESP_LOGE(TAG, "Key type mismatch");
        return ESP_ERR_INVALID_STATE;
    }

    mbedtls_mpi r, s;
    mbedtls_ecp_keypair priv_key;
    mbedtls_ecdsa_context sign_ctx;

    mbedtls_mpi_init(&r);
    mbedtls_mpi_init(&s);
    mbedtls_ecp_keypair_init(&priv_key);
    mbedtls_ecdsa_init(&sign_ctx);

    size_t key_len = 0;
    int ret = -1;

    if (cfg->type == ESP_SEC_STG_KEY_ECDSA_SECP256R1) {
        ret = mbedtls_ecp_read_key(MBEDTLS_ECP_DP_SECP256R1, &priv_key, keyctx.ecdsa_secp256r1.priv_key, sizeof(keyctx.ecdsa_secp256r1.priv_key));
        key_len = ECDSA_SECP256R1_KEY_LEN;
#if CONFIG_SECURE_TEE_SEC_STG_SUPPORT_SECP192R1_SIGN
    } else if (cfg->type == ESP_SEC_STG_KEY_ECDSA_SECP192R1) {
        ret = mbedtls_ecp_read_key(MBEDTLS_ECP_DP_SECP192R1, &priv_key, keyctx.ecdsa_secp192r1.priv_key, sizeof(keyctx.ecdsa_secp192r1.priv_key));
        key_len = ECDSA_SECP192R1_KEY_LEN;
#endif
    }

    if (ret != 0) {
        err = ESP_FAIL;
        goto exit;
    }

    ret = mbedtls_ecdsa_from_keypair(&sign_ctx, &priv_key);
    if (ret != 0) {
        err = ESP_FAIL;
        goto exit;
    }

    ESP_LOGD(TAG, "Generating ECDSA signature...");

    ret = mbedtls_ecdsa_sign(&sign_ctx.MBEDTLS_PRIVATE(grp), &r, &s, &sign_ctx.MBEDTLS_PRIVATE(d), hash, hlen,
                             rand_func, NULL);
    if (ret != 0) {
        ESP_LOGE(TAG, "Error generating signature: %d", ret);
        err = ESP_FAIL;
        goto exit;
    }

    memset(out_sign, 0x00, sizeof(esp_tee_sec_storage_ecdsa_sign_t));
    ret = mbedtls_mpi_write_binary(&r, out_sign->sign_r, key_len);
    if (ret == 0) {
        ret = mbedtls_mpi_write_binary(&s, out_sign->sign_s, key_len);
    }

    if (ret != 0) {
        memset(out_sign, 0x00, sizeof(esp_tee_sec_storage_ecdsa_sign_t));
        err = ESP_FAIL;
        goto exit;
    }

    err = ESP_OK;

exit:
    mbedtls_ecdsa_free(&sign_ctx);
    mbedtls_ecp_keypair_free(&priv_key);
    mbedtls_mpi_free(&s);
    mbedtls_mpi_free(&r);

    return err;
}

esp_err_t esp_tee_sec_storage_ecdsa_get_pubkey(const esp_tee_sec_storage_key_cfg_t *cfg, esp_tee_sec_storage_ecdsa_pubkey_t *out_pubkey)
{
    if (cfg == NULL || cfg->id == NULL || out_pubkey == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    esp_err_t err = secure_storage_find_key(cfg->id);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Key ID not found");
        return err;
    }

    sec_stg_key_t keyctx;
    size_t keyctx_len = sizeof(keyctx);
    uint8_t *pub_key_src = NULL;
    size_t pub_key_len = 0;

    switch (cfg->type) {
    case ESP_SEC_STG_KEY_ECDSA_SECP256R1:
        pub_key_src = keyctx.ecdsa_secp256r1.pub_key;
        pub_key_len = ECDSA_SECP256R1_KEY_LEN;
        break;
#if CONFIG_SECURE_TEE_SEC_STG_SUPPORT_SECP192R1_SIGN
    case ESP_SEC_STG_KEY_ECDSA_SECP192R1:
        pub_key_src = keyctx.ecdsa_secp192r1.pub_key;
        pub_key_len = ECDSA_SECP192R1_KEY_LEN;
        break;
#endif
    default:
        ESP_LOGE(TAG, "Unsupported key-type");
        return ESP_ERR_INVALID_ARG;
    }

    err = secure_storage_read(cfg->id, (void *)&keyctx, &keyctx_len);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to read key from secure storage");
        return err;
    }

    if (keyctx.type != cfg->type) {
        ESP_LOGE(TAG, "Key type mismatch");
        return ESP_ERR_INVALID_STATE;
    }

    memcpy(out_pubkey->pub_x, pub_key_src, pub_key_len);
    memcpy(out_pubkey->pub_y, pub_key_src + pub_key_len, pub_key_len);

    return ESP_OK;
}

static esp_err_t tee_sec_storage_crypt_common(const char *key_id, const uint8_t *input, size_t len, const uint8_t *aad,
                                              size_t aad_len, uint8_t *tag, size_t tag_len, uint8_t *output,
                                              bool is_encrypt)
{
    if (key_id == NULL || input == NULL || output == NULL || tag == NULL) {
        ESP_LOGE(TAG, "Invalid arguments");
        return ESP_ERR_INVALID_ARG;
    }

    if (len == 0 || tag_len == 0) {
        ESP_LOGE(TAG, "Invalid input/tag length");
        return ESP_ERR_INVALID_SIZE;
    }

    esp_err_t err = secure_storage_find_key(key_id);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Key ID not found");
        return err;
    }

    sec_stg_key_t keyctx;
    size_t keyctx_len = sizeof(keyctx);
    err = secure_storage_read(key_id, (void *)&keyctx, &keyctx_len);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to fetch key from storage");
        return err;
    }

    if (keyctx.type != ESP_SEC_STG_KEY_AES256) {
        ESP_LOGE(TAG, "Key type mismatch");
        return ESP_ERR_INVALID_STATE;
    }

    mbedtls_gcm_context gcm;
    mbedtls_gcm_init(&gcm);

    int ret = mbedtls_gcm_setkey(&gcm, MBEDTLS_CIPHER_ID_AES, keyctx.aes256.key, AES256_KEY_BITS);
    if (ret != 0) {
        ESP_LOGE(TAG, "Error in setting key: %d", ret);
        err = ESP_FAIL;
        goto exit;
    }

    if (is_encrypt) {
        ret = mbedtls_gcm_crypt_and_tag(&gcm, MBEDTLS_GCM_ENCRYPT, len, keyctx.aes256.iv, AES256_GCM_IV_LEN,
                                        aad, aad_len, input, output, tag_len, tag);
        if (ret != 0) {
            ESP_LOGE(TAG, "Error in encrypting data: %d", ret);
            err = ESP_FAIL;
            goto exit;
        }
    } else {
        ret = mbedtls_gcm_auth_decrypt(&gcm, len, keyctx.aes256.iv, AES256_GCM_IV_LEN,
                                       aad, aad_len, tag, tag_len, input, output);
        if (ret != 0) {
            ESP_LOGE(TAG, "Error in decrypting data: %d", ret);
            err = ESP_FAIL;
            goto exit;
        }
    }
    err = ESP_OK;

exit:
    mbedtls_gcm_free(&gcm);
    return err;
}

esp_err_t esp_tee_sec_storage_aead_encrypt(const esp_tee_sec_storage_aead_ctx_t *ctx, uint8_t *tag, size_t tag_len, uint8_t *output)
{
    return tee_sec_storage_crypt_common(ctx->key_id, ctx->input, ctx->input_len, ctx->aad, ctx->aad_len, tag, tag_len, output, true);
}

esp_err_t esp_tee_sec_storage_aead_decrypt(const esp_tee_sec_storage_aead_ctx_t *ctx, const uint8_t *tag, size_t tag_len, uint8_t *output)
{
    return tee_sec_storage_crypt_common(ctx->key_id, ctx->input, ctx->input_len, ctx->aad, ctx->aad_len, (uint8_t *)tag, tag_len, output, false);
}

esp_err_t esp_tee_sec_storage_ecdsa_sign_pbkdf2(const esp_tee_sec_storage_pbkdf2_ctx_t *ctx,
                                                const uint8_t *hash, size_t hlen,
                                                esp_tee_sec_storage_ecdsa_sign_t *out_sign,
                                                esp_tee_sec_storage_ecdsa_pubkey_t *out_pubkey)
{
    if (!ctx || !hash || !out_sign || !out_pubkey || !ctx->salt || ctx->salt_len == 0) {
        return ESP_ERR_INVALID_ARG;
    }

    if (hlen != SHA256_DIGEST_SZ) {
        return ESP_ERR_INVALID_SIZE;
    }

    esp_err_t err = ESP_FAIL;
    size_t key_len;
    mbedtls_ecp_group_id curve_id = MBEDTLS_ECP_DP_NONE;

    switch (ctx->key_type) {
    case ESP_SEC_STG_KEY_ECDSA_SECP256R1:
        key_len = ECDSA_SECP256R1_KEY_LEN;
        curve_id = MBEDTLS_ECP_DP_SECP256R1;
        break;
#if CONFIG_SECURE_TEE_SEC_STG_SUPPORT_SECP192R1_SIGN
    case ESP_SEC_STG_KEY_ECDSA_SECP192R1:
        key_len = ECDSA_SECP192R1_KEY_LEN;
        curve_id = MBEDTLS_ECP_DP_SECP192R1;
        break;
#endif
    default:
        ESP_LOGE(TAG, "Unsupported key type");
        return ESP_ERR_INVALID_ARG;
    }

    hmac_key_id_t key_id = (hmac_key_id_t)(CONFIG_SECURE_TEE_PBKDF2_EFUSE_HMAC_KEY_ID);
    esp_efuse_block_t blk = EFUSE_BLK_KEY0 + (esp_efuse_block_t)(key_id);
    if (esp_efuse_get_key_purpose(blk) != ESP_EFUSE_KEY_PURPOSE_HMAC_UP) {
        ESP_LOGE(TAG, "HMAC key is not burnt in the specified eFuse block ID");
        return ESP_ERR_NOT_FOUND;
    }

    uint8_t *derived_key = calloc(1, key_len);
    if (!derived_key) {
        return ESP_ERR_NO_MEM;
    }

    err = esp_hmac_derive_pbkdf2_key(key_id, ctx->salt, ctx->salt_len, PBKDF2_HMAC_ITER, key_len, derived_key);
    if (err != ESP_OK) {
        goto exit;
    }

    mbedtls_ecp_keypair keypair;
    mbedtls_mpi r, s;

    mbedtls_ecp_keypair_init(&keypair);
    mbedtls_mpi_init(&r);
    mbedtls_mpi_init(&s);

    int ret = -1;

    ret = mbedtls_ecp_group_load(&keypair.MBEDTLS_PRIVATE(grp), curve_id);
    if (ret != 0) {
        err = ESP_FAIL;
        goto exit;
    }

    ret = mbedtls_mpi_read_binary(&keypair.MBEDTLS_PRIVATE(d), derived_key, key_len);
    if (ret != 0) {
        err = ESP_FAIL;
        goto exit;
    }

    ret = mbedtls_ecp_check_privkey(&keypair.MBEDTLS_PRIVATE(grp), &keypair.MBEDTLS_PRIVATE(d));
    if (ret != 0) {
        ESP_LOGE(TAG, "Invalid private key!");
        err = ESP_FAIL;
        goto exit;
    }

    ret = mbedtls_ecp_keypair_calc_public(&keypair, rand_func, NULL);
    if (ret != 0) {
        err = ESP_FAIL;
        goto exit;
    }

    ret = mbedtls_ecdsa_sign(&keypair.MBEDTLS_PRIVATE(grp), &r, &s,
                             &keypair.MBEDTLS_PRIVATE(d), hash, hlen,
                             rand_func, NULL);
    if (ret != 0) {
        err = ESP_FAIL;
        goto exit;
    }

    memset(out_sign, 0x00, sizeof(esp_tee_sec_storage_ecdsa_sign_t));
    ret = mbedtls_mpi_write_binary(&r, out_sign->sign_r, key_len);
    if (ret == 0) {
        ret = mbedtls_mpi_write_binary(&s, out_sign->sign_s, key_len);
    }

    if (ret != 0) {
        memset(out_sign, 0x00, sizeof(esp_tee_sec_storage_ecdsa_sign_t));
        err = ESP_FAIL;
        goto exit;
    }

    memset(out_pubkey, 0x00, sizeof(esp_tee_sec_storage_ecdsa_pubkey_t));
    ret = mbedtls_mpi_write_binary(&keypair.MBEDTLS_PRIVATE(Q).MBEDTLS_PRIVATE(X), out_pubkey->pub_x, key_len);
    if (ret == 0) {
        ret = mbedtls_mpi_write_binary(&keypair.MBEDTLS_PRIVATE(Q).MBEDTLS_PRIVATE(Y), out_pubkey->pub_y, key_len);
    }

    if (ret != 0) {
        memset(out_pubkey, 0x00, sizeof(esp_tee_sec_storage_ecdsa_pubkey_t));
        err = ESP_FAIL;
        goto exit;
    }

    err = ESP_OK;

exit:
    if (derived_key) {
        memset(derived_key, 0x00, key_len);
        free(derived_key);
    }
    mbedtls_ecp_keypair_free(&keypair);
    mbedtls_mpi_free(&r);
    mbedtls_mpi_free(&s);
    return err;
}
