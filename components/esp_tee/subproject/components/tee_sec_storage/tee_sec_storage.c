/*
 * SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include "esp_cpu.h"

#include "mbedtls/aes.h"
#include "mbedtls/gcm.h"
#include "mbedtls/sha256.h"

#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/ecdsa.h"
#include "mbedtls/error.h"

#include "esp_flash.h"
#include "esp_efuse.h"
#include "soc/efuse_reg.h"

#include "esp_random.h"
#include "esp_tee_flash.h"
#include "esp_tee_sec_storage.h"

#include "secure_service_num.h"
#include "esp_rom_sys.h"
#include "esp_log.h"
#include "spi_flash_mmap.h"

#define ALIGN_UP(num, align) (((num) + ((align) - 1)) & ~((align) - 1))

#define SECURE_STORAGE_SIZE                 2048

#define AES256_GCM_KEY_LEN                  32
#define AES256_GCM_KEY_BITS                 (AES256_GCM_KEY_LEN * 8)
#define AES256_GCM_IV_LEN                   12
#define AES256_GCM_TAG_LEN                  16
#define AES256_GCM_AAD_LEN                  16

#define ECDSA_SECP256R1_KEY_LEN             32
#define ECDSA_SECP192R1_KEY_LEN             24

/* Structure to hold metadata for secure storage slots */
typedef struct {
    uint16_t owner_id;                /* Identifier for the owner of this slot */
    uint16_t slot_id;                 /* Unique identifier for this storage slot */
    uint8_t reserved;                 /* Reserved for future use */
    uint8_t iv[AES256_GCM_IV_LEN];    /* Initialization vector for AES-GCM */
    uint8_t tag[AES256_GCM_TAG_LEN];  /* Authentication tag for AES-GCM */
    uint8_t data_type;                /* Type of data stored in this slot */
    uint16_t data_len;                /* Length of the data stored in this slot */
} __attribute__((aligned(4))) __attribute__((__packed__)) sec_stg_metadata_t;

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

/* Structure to hold AES-256 GCM key and IV */
typedef struct {
    uint8_t key[AES256_GCM_KEY_LEN];  /* Key for AES-256 GCM */
    uint8_t iv[AES256_GCM_IV_LEN];    /* Initialization vector for AES-256 GCM */
} __attribute__((aligned(4))) __attribute__((__packed__)) sec_stg_aes256_gcm_t;

/* Union to hold different types of cryptographic keys */
typedef union {
    sec_stg_ecdsa_secp256r1_t ecdsa_secp256r1;  /* ECDSA SECP256R1 key pair */
    sec_stg_ecdsa_secp192r1_t ecdsa_secp192r1;  /* ECDSA SECP192R1 key pair */
    sec_stg_aes256_gcm_t aes256_gcm;            /* AES-256 GCM key and IV */
} __attribute__((aligned(4))) __attribute__((__packed__)) sec_stg_key_t;

_Static_assert(sizeof(sec_stg_metadata_t) == 36, "Incorrect sec_stg_metadata_t size");
_Static_assert(sizeof(sec_stg_key_t) == 96, "Incorrect sec_stg_key_t size");

// Need this buffer to read the flash data and then modify and write it back
// esp_rom_spiflash_write requires that we erase the region before writing to it
// TODO: IDF-7586
static uint8_t tmp_buf[SECURE_STORAGE_SIZE];

// AAD buffer
static uint8_t aad_buf[AES256_GCM_AAD_LEN];

// Partition for the secure storage partition
static esp_partition_pos_t part_pos;

static const char *TAG = "secure_storage";

/* ---------------------------------------------- Helper APIs ------------------------------------------------- */
#if CONFIG_SECURE_TEE_SEC_STG_KEY_EFUSE_BLK > 9
#error "TEE Secure Storage: Configured eFuse block for encryption key out of range! (see CONFIG_SECURE_TEE_SEC_STG_KEY_EFUSE_BLK)"
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

static esp_err_t get_sec_stg_encr_key(uint8_t *key_buf, size_t key_buf_len)
{
    // NOTE: Key should strictly be of 256-bits
    if (!key_buf || key_buf_len != AES256_GCM_KEY_LEN) {
        return ESP_ERR_INVALID_ARG;
    }

    esp_err_t err = ESP_OK;

#if CONFIG_SECURE_TEE_SEC_STG_MODE_RELEASE
    esp_efuse_block_t blk = (esp_efuse_block_t)(CONFIG_SECURE_TEE_SEC_STG_KEY_EFUSE_BLK);

    if (blk < EFUSE_BLK_KEY0 || blk >= EFUSE_BLK_KEY_MAX) {
        ESP_LOGE(TAG, "Invalid eFuse block - %d", blk);
        return ESP_ERR_INVALID_ARG;
    }

    esp_efuse_purpose_t blk_purpose = esp_efuse_get_key_purpose(blk);
    if (blk_purpose != ESP_EFUSE_KEY_PURPOSE_USER) {
        ESP_LOGE(TAG, "Invalid eFuse block purpose - %d", blk_purpose);
        return ESP_ERR_INVALID_STATE;
    }

    memset(key_buf, 0x00, key_buf_len);
    err = esp_efuse_read_block(blk, key_buf, 0, AES256_GCM_KEY_BITS);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to read eFuse block (err - %d)", err);
        return err;
    }
#else
    memset(key_buf, 0xA5, key_buf_len);
#endif

    // Check if eFuse is empty
    uint8_t empty_key_buf[AES256_GCM_KEY_LEN] = {0};
    if (memcmp(empty_key_buf, key_buf, key_buf_len) == 0) {
        ESP_LOGE(TAG, "All-zeroes key read from eFuse");
        return ESP_FAIL;
    }

    return err;
}

static int rand_func(void *rng_state, unsigned char *output, size_t len)
{
    esp_fill_random(output, len);
    return 0;
}

static int secure_storage_write(uint16_t slot_id, uint8_t *data, size_t len, uint8_t type)
{
    uint8_t iv[AES256_GCM_IV_LEN];
    uint8_t tag[AES256_GCM_TAG_LEN];
    uint8_t key[AES256_GCM_KEY_LEN];
    uint8_t out_data[256] = {0};

    buffer_hexdump("Plaintext data", data, len);

    mbedtls_gcm_context gcm;
    mbedtls_gcm_init(&gcm);

    esp_err_t err = get_sec_stg_encr_key(key, sizeof(key));
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to fetch key from eFuse!");
        goto exit;
    }

    int ret = mbedtls_gcm_setkey(&gcm, MBEDTLS_CIPHER_ID_AES, key, AES256_GCM_KEY_BITS);
    if (ret != 0) {
        ESP_LOGE(TAG, "Error in setting key: %d", ret);
        err = ESP_FAIL;
        goto exit;
    }

    // Generate different IV every time GCM encrypt is called
    esp_fill_random(iv, AES256_GCM_IV_LEN);

    ret = mbedtls_gcm_crypt_and_tag(&gcm, MBEDTLS_GCM_ENCRYPT, len, iv, AES256_GCM_IV_LEN,
                                    aad_buf, AES256_GCM_AAD_LEN, data, out_data, AES256_GCM_TAG_LEN, tag);
    if (ret != 0) {
        ESP_LOGE(TAG, "Error in encrypting data: %d", ret);
        err = ESP_FAIL;
        goto exit;
    }

    buffer_hexdump("Encrypted data", out_data, len);
    buffer_hexdump("TAG data", tag, sizeof(tag));

    // Currently keeping the owner ID as 0
    sec_stg_metadata_t metadata;
    metadata.owner_id = 0;
    metadata.slot_id = slot_id;
    memcpy(metadata.iv, iv, AES256_GCM_IV_LEN);
    memcpy(metadata.tag, tag, AES256_GCM_TAG_LEN);
    metadata.data_type = type;
    metadata.data_len = len;

    uint32_t slot_offset = (sizeof(sec_stg_metadata_t) + sizeof(sec_stg_key_t)) * slot_id;

    /* ROM flash APIs require the region to be erased before writing to it.
     * For that, we read the entire sector, make changes in read buffer, and then write
     * the entire data back in flash.
     *
     * This opens up a small window when the sector has been erased but the device resets before writing the
     * data back in flash. This can lead to loss of data.
     *
     * TODO: IDF-7586
     */
    ret = esp_tee_flash_read(part_pos.offset, (uint32_t *)tmp_buf, SECURE_STORAGE_SIZE, false);
    if (ret != 0) {
        ESP_LOGE(TAG, "Error reading flash contents: %d", ret);
        err = ESP_ERR_FLASH_OP_FAIL;
        goto exit;
    }

    memcpy(&tmp_buf[slot_offset], &metadata, sizeof(sec_stg_metadata_t));
    memcpy(&tmp_buf[slot_offset + sizeof(sec_stg_metadata_t)], out_data, len);

    ret = esp_tee_flash_erase_range(part_pos.offset, ALIGN_UP(SECURE_STORAGE_SIZE, FLASH_SECTOR_SIZE));
    ret |= esp_tee_flash_write(part_pos.offset, (uint32_t *)tmp_buf, SECURE_STORAGE_SIZE, false);
    if (ret != 0) {
        ESP_LOGE(TAG, "Error writing encrypted data: %d", ret);
        err = ESP_ERR_FLASH_OP_FAIL;
        goto exit;
    }
    err = ESP_OK;

exit:
    mbedtls_gcm_free(&gcm);
    return err;
}

static esp_err_t secure_storage_read(uint16_t slot_id, uint8_t *data, size_t len, uint8_t type)
{
    esp_err_t err;

    sec_stg_metadata_t metadata;
    uint32_t slot_offset = (sizeof(sec_stg_metadata_t) + sizeof(sec_stg_key_t)) * slot_id;

    uint8_t key[AES256_GCM_KEY_BITS / 8];
    uint8_t flash_data[256] = {0};

    int ret = esp_tee_flash_read(part_pos.offset + slot_offset, (uint32_t *)&metadata, sizeof(sec_stg_metadata_t), false);
    if (ret != 0) {
        ESP_LOGE(TAG, "Error reading metadata: %d", ret);
        err = ESP_ERR_FLASH_OP_FAIL;
        goto exit;
    }

    if (metadata.data_type != type || metadata.data_len != len) {
        ESP_LOGE(TAG, "Data type/length mismatch");
        err = ESP_ERR_NOT_FOUND;
        goto exit;
    }

    ret = esp_tee_flash_read(part_pos.offset + slot_offset + sizeof(sec_stg_metadata_t), (uint32_t *)flash_data, metadata.data_len, false);
    if (ret != 0) {
        ESP_LOGE(TAG, "Error reading data: %d", ret);
        err = ESP_ERR_FLASH_OP_FAIL;
        goto exit;
    }

    buffer_hexdump("Encrypted data", flash_data, len);
    buffer_hexdump("TAG data", metadata.tag, AES256_GCM_TAG_LEN);

    mbedtls_gcm_context gcm;
    mbedtls_gcm_init(&gcm);

    err = get_sec_stg_encr_key(key, sizeof(key));
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to fetch key from eFuse!");
        goto cleanup;
    }

    ret = mbedtls_gcm_setkey(&gcm, MBEDTLS_CIPHER_ID_AES, key, AES256_GCM_KEY_BITS);
    if (ret != 0) {
        err = ESP_FAIL;
        goto cleanup;
    }

    ret = mbedtls_gcm_auth_decrypt(&gcm, metadata.data_len, metadata.iv, AES256_GCM_IV_LEN,
                                   aad_buf, AES256_GCM_AAD_LEN, metadata.tag, AES256_GCM_TAG_LEN, flash_data, data);
    if (ret != 0) {
        ESP_LOGE(TAG, "Error in decrypting data: %d", ret);
        err = ESP_FAIL;
        goto cleanup;
    }

    buffer_hexdump("Decrypted data", data, len);
    err = ESP_OK;

cleanup:
    mbedtls_gcm_free(&gcm);
exit:
    return err;
}

/* ---------------------------------------------- Interface APIs ------------------------------------------------- */

esp_err_t esp_tee_sec_storage_init(void)
{
    ESP_LOGI(TAG, "Initializing secure storage...");
    esp_partition_info_t part_info = {};
    esp_err_t err = esp_tee_flash_find_partition(PART_TYPE_DATA, PART_SUBTYPE_DATA_TEE_SEC_STORAGE, NULL, &part_info);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "No secure storage partition found (0x%08x)", err);
        return err;
    } else {
#if CONFIG_SECURE_TEE_SEC_STG_MODE_DEVELOPMENT
        ESP_LOGW(TAG, "TEE Secure Storage enabled in insecure DEVELOPMENT mode");
#endif
        // Take backup of the partition for future usage
        part_pos = part_info.pos;
    }

    return ESP_OK;
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
        ESP_LOGE(TAG, "Unsupported key type!");
        return -1;
#endif
    }

    ESP_LOGI(TAG, "Generating ECDSA key for curve %d...", curve_id);

    mbedtls_ecdsa_context ctxECDSA;
    mbedtls_ecdsa_init(&ctxECDSA);

    int ret = mbedtls_ecdsa_genkey(&ctxECDSA, curve_id, rand_func, NULL);
    if (ret != 0) {
        ESP_LOGE(TAG, "Failed to generate ECDSA key");
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

    ret = mbedtls_mpi_write_binary(&ctxECDSA.MBEDTLS_PRIVATE(d), priv_key, key_len);
    if (ret != 0) {
        goto exit;
    }

    ret = mbedtls_mpi_write_binary(&(ctxECDSA.MBEDTLS_PRIVATE(Q).MBEDTLS_PRIVATE(X)), pub_key, key_len);
    if (ret != 0) {
        goto exit;
    }

    ret = mbedtls_mpi_write_binary(&(ctxECDSA.MBEDTLS_PRIVATE(Q).MBEDTLS_PRIVATE(Y)), pub_key + key_len, key_len);
    if (ret != 0) {
        goto exit;
    }

    buffer_hexdump("Private key", priv_key, key_len);
    buffer_hexdump("Public key", pub_key, key_len * 2);

exit:
    mbedtls_ecdsa_free(&ctxECDSA);
    return ret;
}

static int generate_aes256_gcm_key(sec_stg_key_t *keyctx)
{
    if (keyctx == NULL) {
        return -1;
    }

    ESP_LOGI(TAG, "Generating AES-256-GCM key...");

    esp_fill_random(&keyctx->aes256_gcm.key, AES256_GCM_KEY_LEN);
    esp_fill_random(&keyctx->aes256_gcm.iv, AES256_GCM_IV_LEN);

    return 0;
}

esp_err_t esp_tee_sec_storage_gen_key(uint16_t slot_id, esp_tee_sec_storage_type_t key_type)
{
    if (slot_id > MAX_SEC_STG_SLOT_ID) {
        ESP_LOGE(TAG, "Invalid slot ID");
        return ESP_ERR_INVALID_ARG;
    }

    if (!esp_tee_sec_storage_is_slot_empty(slot_id)) {
        ESP_LOGE(TAG, "Slot already occupied - clear before reuse");
        return ESP_ERR_INVALID_STATE;
    }

    int ret = -1;
    sec_stg_key_t keyctx;

    switch (key_type) {
    case ESP_SEC_STG_KEY_ECDSA_SECP256R1:
#if CONFIG_SECURE_TEE_SEC_STG_SUPPORT_SECP192R1_SIGN
    case ESP_SEC_STG_KEY_ECDSA_SECP192R1:
#endif
        if (generate_ecdsa_key(&keyctx, key_type) != 0) {
            ESP_LOGE(TAG, "Failed to generate ECDSA keypair (%d)", ret);
            return ESP_FAIL;
        }
        break;
    case ESP_SEC_STG_KEY_AES256:
        if (generate_aes256_gcm_key(&keyctx) != 0) {
            ESP_LOGE(TAG, "Failed to generate AES key (%d)", ret);
            return ESP_FAIL;
        }
        break;
    default:
        ESP_LOGE(TAG, "Unsupported key-type!");
        return ESP_ERR_NOT_SUPPORTED;
    }

    return secure_storage_write(slot_id, (uint8_t *)&keyctx, sizeof(keyctx), key_type);
}

esp_err_t esp_tee_sec_storage_get_signature(uint16_t slot_id, esp_tee_sec_storage_type_t key_type, uint8_t *hash, size_t hlen, esp_tee_sec_storage_sign_t *out_sign)
{
    if (slot_id > MAX_SEC_STG_SLOT_ID || hash == NULL || out_sign == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    if (hlen == 0) {
        return ESP_ERR_INVALID_SIZE;
    }

#if !CONFIG_SECURE_TEE_SEC_STG_SUPPORT_SECP192R1_SIGN
    if (key_type == ESP_SEC_STG_KEY_ECDSA_SECP192R1) {
        ESP_LOGE(TAG, "Unsupported key type!");
        return ESP_ERR_NOT_SUPPORTED;
    }
#endif

    sec_stg_key_t keyctx;
    esp_err_t err = secure_storage_read(slot_id, (uint8_t *)&keyctx, sizeof(sec_stg_key_t), key_type);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to fetch key from slot");
        return err;
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
    if (key_type == ESP_SEC_STG_KEY_ECDSA_SECP256R1) {
        ret = mbedtls_ecp_read_key(MBEDTLS_ECP_DP_SECP256R1, &priv_key, keyctx.ecdsa_secp256r1.priv_key, sizeof(keyctx.ecdsa_secp256r1.priv_key));
        key_len = ECDSA_SECP256R1_KEY_LEN;
#if CONFIG_SECURE_TEE_SEC_STG_SUPPORT_SECP192R1_SIGN
    } else if (key_type == ESP_SEC_STG_KEY_ECDSA_SECP192R1) {
        ret = mbedtls_ecp_read_key(MBEDTLS_ECP_DP_SECP192R1, &priv_key, keyctx.ecdsa_secp192r1.priv_key, sizeof(keyctx.ecdsa_secp192r1.priv_key));
        key_len = ECDSA_SECP192R1_KEY_LEN;
#endif
    } else {
        ESP_LOGE(TAG, "Unsupported key type for signature generation");
        err = ESP_ERR_NOT_SUPPORTED;
        goto exit;
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

    ESP_LOGI(TAG, "Generating ECDSA signature...");

    ret = mbedtls_ecdsa_sign(&sign_ctx.MBEDTLS_PRIVATE(grp), &r, &s, &sign_ctx.MBEDTLS_PRIVATE(d), hash, hlen,
                             rand_func, NULL);
    if (ret != 0) {
        ESP_LOGE(TAG, "Error generating signature: %d", ret);
        err = ESP_FAIL;
        goto exit;
    }

    memset(out_sign, 0x00, sizeof(esp_tee_sec_storage_sign_t));

    ret = mbedtls_mpi_write_binary(&r, out_sign->sign_r, key_len);
    if (ret != 0) {
        err = ESP_FAIL;
        goto exit;
    }

    ret = mbedtls_mpi_write_binary(&s, out_sign->sign_s, key_len);
    if (ret != 0) {
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

esp_err_t esp_tee_sec_storage_get_pubkey(uint16_t slot_id, esp_tee_sec_storage_type_t key_type, esp_tee_sec_storage_pubkey_t *pubkey)
{
    if (slot_id > MAX_SEC_STG_SLOT_ID || pubkey == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    sec_stg_key_t keyctx;
    size_t key_len;
    uint8_t *pub_key_src;

    if (key_type == ESP_SEC_STG_KEY_ECDSA_SECP256R1) {
        key_len = ECDSA_SECP256R1_KEY_LEN;
        pub_key_src = keyctx.ecdsa_secp256r1.pub_key;
#if CONFIG_SECURE_TEE_SEC_STG_SUPPORT_SECP192R1_SIGN
    } else if (key_type == ESP_SEC_STG_KEY_ECDSA_SECP192R1) {
        key_len = ECDSA_SECP192R1_KEY_LEN;
        pub_key_src = keyctx.ecdsa_secp192r1.pub_key;
#endif
    } else {
        ESP_LOGE(TAG, "Unsupported key type");
        return ESP_ERR_INVALID_ARG;
    }

    esp_err_t err = secure_storage_read(slot_id, (uint8_t *)&keyctx, sizeof(sec_stg_key_t), key_type);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to fetch key from slot");
        return err;
    }

    // Copy public key components in one shot
    memcpy(pubkey->pub_x, pub_key_src, key_len);
    memcpy(pubkey->pub_y, pub_key_src + key_len, key_len);

    return ESP_OK;
}

bool esp_tee_sec_storage_is_slot_empty(uint16_t slot_id)
{
    if (slot_id > MAX_SEC_STG_SLOT_ID) {
        ESP_LOGE(TAG, "Invalid slot ID");
        return false;
    }

    sec_stg_metadata_t metadata, blank_metadata;
    memset(&blank_metadata, 0xFF, sizeof(sec_stg_metadata_t));

    uint32_t slot_offset = (sizeof(sec_stg_metadata_t) + sizeof(sec_stg_key_t)) * slot_id;
    bool ret = false;

    int err = esp_tee_flash_read(part_pos.offset + slot_offset, (uint32_t *)&metadata, sizeof(sec_stg_metadata_t), false);
    if (err != 0) {
        goto exit;
    }

    if (memcmp(&metadata, &blank_metadata, sizeof(sec_stg_metadata_t)) && metadata.slot_id == slot_id) {
        goto exit;
    }
    ret = true;

exit:
    return ret;
}

esp_err_t esp_tee_sec_storage_clear_slot(uint16_t slot_id)
{
    if (slot_id > MAX_SEC_STG_SLOT_ID) {
        ESP_LOGE(TAG, "Invalid slot ID");
        return ESP_ERR_INVALID_ARG;
    }

    if (esp_tee_sec_storage_is_slot_empty(slot_id)) {
        return ESP_OK;
    }

    sec_stg_key_t blank_data;
    memset(&blank_data, 0xFF, sizeof(blank_data));

    sec_stg_metadata_t blank_metadata;
    memset(&blank_metadata, 0xFF, sizeof(sec_stg_metadata_t));

    uint32_t slot_offset = (sizeof(sec_stg_metadata_t) + sizeof(sec_stg_key_t)) * slot_id;
    esp_err_t err;

    int ret = esp_tee_flash_read(part_pos.offset, (uint32_t *)tmp_buf, SECURE_STORAGE_SIZE, false);
    if (ret != 0) {
        ESP_LOGE(TAG, "Error reading flash contents: %d", ret);
        err = ESP_ERR_FLASH_OP_FAIL;
        goto exit;
    }

    memcpy(&tmp_buf[slot_offset], &blank_metadata, sizeof(sec_stg_metadata_t));
    memcpy(&tmp_buf[slot_offset + sizeof(sec_stg_metadata_t)], &blank_data, sizeof(sec_stg_key_t));

    ret = esp_tee_flash_erase_range(part_pos.offset, ALIGN_UP(SECURE_STORAGE_SIZE, FLASH_SECTOR_SIZE));
    ret |= esp_tee_flash_write(part_pos.offset, (uint32_t *)tmp_buf, SECURE_STORAGE_SIZE, false);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Error clearing slot: %d", ret);
        err = ESP_ERR_FLASH_OP_FAIL;
        goto exit;
    }
    err = ESP_OK;

exit:
    return err;
}

static esp_err_t tee_sec_storage_crypt_common(uint16_t slot_id, uint8_t *input, uint8_t len, uint8_t *aad,
                                              uint16_t aad_len, uint8_t *tag, uint16_t tag_len, uint8_t *output,
                                              bool is_encrypt)
{
    if (slot_id > MAX_SEC_STG_SLOT_ID) {
        ESP_LOGE(TAG, "Invalid slot ID");
        return ESP_ERR_INVALID_ARG;
    }

    if (input == NULL || output == NULL || tag == NULL) {
        ESP_LOGE(TAG, "Invalid input/output/tag buffer");
        return ESP_ERR_INVALID_ARG;
    }

    if (len == 0 || tag_len == 0) {
        ESP_LOGE(TAG, "Invalid length/tag length");
        return ESP_ERR_INVALID_SIZE;
    }

    sec_stg_key_t keyctx;
    esp_err_t err = secure_storage_read(slot_id, (uint8_t *)&keyctx, sizeof(keyctx), ESP_SEC_STG_KEY_AES256);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to fetch key from slot");
        return err;
    }

    mbedtls_gcm_context gcm;
    mbedtls_gcm_init(&gcm);

    int ret = mbedtls_gcm_setkey(&gcm, MBEDTLS_CIPHER_ID_AES, keyctx.aes256_gcm.key, AES256_GCM_KEY_BITS);
    if (ret != 0) {
        ESP_LOGE(TAG, "Error in setting key: %d", ret);
        err = ESP_FAIL;
        goto exit;
    }

    if (is_encrypt) {
        ret = mbedtls_gcm_crypt_and_tag(&gcm, MBEDTLS_GCM_ENCRYPT, len, keyctx.aes256_gcm.iv, AES256_GCM_IV_LEN,
                                        aad, aad_len, input, output, tag_len, tag);
        if (ret != 0) {
            ESP_LOGE(TAG, "Error in encrypting data: %d", ret);
            err = ESP_FAIL;
            goto exit;
        }
    } else {
        ret = mbedtls_gcm_auth_decrypt(&gcm, len, keyctx.aes256_gcm.iv, AES256_GCM_IV_LEN,
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

esp_err_t esp_tee_sec_storage_encrypt(uint16_t slot_id, uint8_t *input, uint8_t len, uint8_t *aad,
                                      uint16_t aad_len, uint8_t *tag, uint16_t tag_len, uint8_t *output)
{
    return tee_sec_storage_crypt_common(slot_id, input, len, aad, aad_len, tag, tag_len, output, true);
}

esp_err_t esp_tee_sec_storage_decrypt(uint16_t slot_id, uint8_t *input, uint8_t len, uint8_t *aad,
                                      uint16_t aad_len, uint8_t *tag, uint16_t tag_len, uint8_t *output)
{
    return tee_sec_storage_crypt_common(slot_id, input, len, aad, aad_len, tag, tag_len, output, false);
}
