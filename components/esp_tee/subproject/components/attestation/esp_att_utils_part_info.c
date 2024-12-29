/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <inttypes.h>

#include <sys/param.h>
#include <sys/queue.h>

#include "esp_log.h"
#include "esp_err.h"
#include "hal/efuse_hal.h"

#include "esp_app_format.h"
#include "esp_bootloader_desc.h"
#include "esp_image_format.h"
#include "esp_app_desc.h"

#if ESP_TEE_BUILD
#include "esp_flash_partitions.h"
#include "bootloader_utility_tee.h"
#include "esp_tee_flash.h"
#else
#include "esp_partition.h"
#include "esp_ota_ops.h"
#endif

#if CONFIG_SECURE_BOOT_V2_ENABLED
#include "esp_secure_boot.h"
#include "bootloader_utility.h"
#if CONFIG_IDF_TARGET_ESP32C6
#include "esp32c6/rom/secure_boot.h"
#endif
#endif

#include "mbedtls/sha256.h"

#include "bootloader_flash_priv.h"
#include "esp_attestation_utils.h"

#define SECURE_BOOT_V2       (0x02)
#define ALIGN_UP(num, align) (((num) + ((align)-1)) & ~((align)-1))

static const char *TAG = "esp_att_utils";

/* Forward declaration */
static esp_err_t read_partition(uint32_t offset, void *buf, size_t size);
esp_err_t get_flash_contents_sha256(uint32_t flash_offset, uint32_t len, uint8_t *digest);
static esp_err_t get_active_app_part_pos(esp_partition_pos_t *pos);
static esp_err_t get_active_tee_part_pos(esp_partition_pos_t *pos);

/* ---------------------------------------------- Helper APIs ------------------------------------------------- */

static size_t digest_type_to_len(esp_att_part_digest_type_t digest)
{
    size_t digest_len = 0;

    switch (digest) {
    case ESP_ATT_DIGEST_TYPE_SHA256:
        digest_len = SHA256_DIGEST_SZ;
        break;
    default:
        break;
    }

    return digest_len;
}

#if ESP_TEE_BUILD

static esp_err_t read_partition(uint32_t offset, void *buf, size_t size)
{
    return (esp_err_t)esp_tee_flash_read(offset, buf, size, true);
}

esp_err_t get_flash_contents_sha256(uint32_t flash_offset, uint32_t len, uint8_t *digest)
{
    if (digest == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    uint32_t mmu_free_pages_count = esp_tee_flash_mmap_get_free_pages();
    uint32_t partial_image_len = mmu_free_pages_count * CONFIG_MMU_PAGE_SIZE;

    mbedtls_sha256_context ctx;
    mbedtls_sha256_init(&ctx);

    int ret = mbedtls_sha256_starts(&ctx, false);
    if (ret != 0) {
        mbedtls_sha256_free(&ctx);
        return ESP_FAIL;
    }

    while (len > 0) {
        uint32_t mmap_len = MIN(len, partial_image_len);
        const void *image = esp_tee_flash_mmap(flash_offset, mmap_len);
        if (image == NULL) {
            mbedtls_sha256_finish(&ctx, NULL);
            return ESP_FAIL;
        }
        mbedtls_sha256_update(&ctx, image, mmap_len);
        esp_tee_flash_munmap(image);

        flash_offset += mmap_len;
        len -= mmap_len;
    }

    mbedtls_sha256_finish(&ctx, digest);
    mbedtls_sha256_free(&ctx);
    return ESP_OK;
}

static esp_err_t get_active_app_part_pos(esp_partition_pos_t *pos)
{
    if (pos == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    esp_partition_info_t *running_app = esp_tee_flash_get_running_ree_partition();
    if (running_app->magic != ESP_PARTITION_MAGIC) {
        return ESP_ERR_NOT_FOUND;
    }

    memcpy(pos, &running_app->pos, sizeof(esp_partition_pos_t));
    return ESP_OK;
}

static esp_err_t get_active_tee_part_pos(esp_partition_pos_t *pos)
{
    uint8_t tee_active_part = bootloader_utility_tee_get_boot_partition(NULL);
    if (tee_active_part > PART_SUBTYPE_TEE_1) {
        return ESP_ERR_NOT_FOUND;
    }

    esp_partition_info_t part_info = {};
    esp_err_t err = esp_tee_flash_find_partition(PART_TYPE_APP, tee_active_part, NULL, &part_info);
    if (err != ESP_OK) {
        return err;
    }

    memcpy(pos, &part_info.pos, sizeof(esp_partition_pos_t));
    return ESP_OK;
}

#else

static esp_err_t read_partition(uint32_t offset, void *buf, size_t size)
{
    return esp_flash_read(NULL, buf, offset, size);
}

esp_err_t get_flash_contents_sha256(uint32_t flash_offset, uint32_t len, uint8_t *digest)
{
    if (digest == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    esp_err_t err =  ESP_FAIL;

    uint32_t mmu_free_pages_count = bootloader_mmap_get_free_pages();
    uint32_t partial_image_len = mmu_free_pages_count * CONFIG_MMU_PAGE_SIZE;

    mbedtls_sha256_context sha256_ctx;
    mbedtls_sha256_init(&sha256_ctx);

    if (mbedtls_sha256_starts(&sha256_ctx, false) != 0) {
        goto exit;
    }

    while (len > 0) {
        uint32_t mmap_len = MIN(len, partial_image_len);
        const void *image = bootloader_mmap(flash_offset, mmap_len);
        if (image == NULL) {
            goto exit;
        }
        if (mbedtls_sha256_update(&sha256_ctx, image, mmap_len) != 0) {
            goto exit;
        }
        bootloader_munmap(image);

        flash_offset += mmap_len;
        len -= mmap_len;
    }

    if (mbedtls_sha256_finish(&sha256_ctx, digest) != 0) {
        goto exit;
    }

    err = ESP_OK;
exit:
    mbedtls_sha256_free(&sha256_ctx);
    return err;
}

static esp_err_t get_active_app_part_pos(esp_partition_pos_t *pos)
{
    if (pos == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    const esp_partition_t *running_part = esp_ota_get_running_partition();
    if (running_part == NULL) {
        return ESP_ERR_NOT_FOUND;
    }

    pos->offset = running_part->address;
    pos->size = running_part->size;

    return ESP_OK;
}

static esp_err_t get_active_tee_part_pos(esp_partition_pos_t *pos)
{
    return ESP_ERR_NOT_SUPPORTED;
}

#endif

static esp_err_t get_active_part_pos(esp_att_part_type_t part_type, esp_partition_pos_t *active_pos)
{
    if (active_pos == NULL || part_type > ESP_ATT_PART_TYPE_APP) {
        return ESP_ERR_INVALID_ARG;
    }

    esp_err_t err;
    esp_partition_pos_t pos = {};

    switch (part_type) {
    case ESP_ATT_PART_TYPE_BOOTLOADER:
        pos.offset = CONFIG_BOOTLOADER_OFFSET_IN_FLASH;
        pos.size = CONFIG_PARTITION_TABLE_OFFSET - CONFIG_BOOTLOADER_OFFSET_IN_FLASH;
        break;
    case ESP_ATT_PART_TYPE_APP:
        err = get_active_app_part_pos(&pos);
        if (err != ESP_OK) {
            return err;
        }
        break;
    case ESP_ATT_PART_TYPE_TEE:
        err = get_active_tee_part_pos(&pos);
        if (err != ESP_OK) {
            return err;
        }
        break;
    default:
        ESP_LOGE(TAG, "Unsupported partition type!");
        return ESP_ERR_NOT_FOUND;
    }

    memcpy(active_pos, &pos, sizeof(esp_partition_pos_t));
    return ESP_OK;
}

static esp_err_t get_part_digest(const esp_partition_pos_t *pos, esp_att_part_digest_info_t *part_digest)
{
    if (pos == NULL || part_digest == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    esp_image_metadata_t metadata = {};
    esp_err_t err = esp_image_get_metadata(pos, &metadata);
    if (err != ESP_OK) {
        return err;
    }

    memset(part_digest, 0x00, sizeof(esp_att_part_digest_info_t));

    part_digest->type = ESP_ATT_DIGEST_TYPE_SHA256;
    size_t digest_len = digest_type_to_len(part_digest->type);
    size_t image_len = metadata.image_len - digest_len;

    uint8_t *digest = calloc(digest_len, sizeof(uint8_t));
    if (digest == NULL) {
        return ESP_ERR_NO_MEM;
    }

    err = get_flash_contents_sha256(pos->offset, image_len, digest);
    if (err != ESP_OK) {
        goto exit;
    }

    if (!memcmp(digest, &metadata.image_digest, digest_len)) {
        part_digest->digest_validated = true;
    }

    memset(part_digest->calc_digest, 0x00, sizeof(part_digest->calc_digest));
    memcpy(part_digest->calc_digest, digest, digest_len);

#if CONFIG_SECURE_BOOT_V2_ENABLED
    uint32_t signed_image_len = ALIGN_UP(metadata.image_len, FLASH_SECTOR_SIZE);

    if (signed_image_len % CONFIG_MMU_PAGE_SIZE == 0) {
        part_digest->secure_padding = true;
    } else {
        part_digest->secure_padding = false;
    }

    err = esp_secure_boot_verify_signature(metadata.start_addr, signed_image_len);
    if (err == ESP_OK) {
        part_digest->sign_verified = true;
    }
#endif

    err = ESP_OK;

exit:
    free(digest);
    return err;
}

static esp_err_t get_part_btl_desc(const esp_partition_pos_t *pos, esp_bootloader_desc_t *btl_desc)
{
    if (pos == NULL || btl_desc == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    uint32_t btl_desc_offset = pos->offset + sizeof(esp_image_header_t) + sizeof(esp_image_segment_header_t);
    esp_err_t err = read_partition(btl_desc_offset, btl_desc, sizeof(esp_bootloader_desc_t));
    if (err != ESP_OK) {
        return err;
    }

    if (btl_desc->magic_byte != ESP_BOOTLOADER_DESC_MAGIC_BYTE) {
        return ESP_ERR_NOT_FOUND;
    }

    return ESP_OK;
}

static esp_err_t get_part_app_desc(const esp_partition_pos_t *pos, esp_app_desc_t *app_desc)
{
    if (pos == NULL || app_desc == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    uint32_t app_desc_offset = pos->offset + sizeof(esp_image_header_t) + sizeof(esp_image_segment_header_t);
    esp_err_t err = read_partition(app_desc_offset, app_desc, sizeof(esp_app_desc_t));
    if (err != ESP_OK) {
        return err;
    }

    if (app_desc->magic_word != ESP_APP_DESC_MAGIC_WORD) {
        return ESP_ERR_NOT_FOUND;
    }

    return ESP_OK;
}

static esp_err_t get_part_chip_rev(const esp_partition_pos_t *pos, esp_att_part_chip_rev_t *chip_rev)
{
    if (pos == NULL || chip_rev == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    esp_image_header_t img_hdr = {};
    esp_err_t err = read_partition(pos->offset, &img_hdr, sizeof(esp_image_header_t));
    if (err != ESP_OK) {
        return err;
    }

    if (img_hdr.magic != ESP_IMAGE_HEADER_MAGIC) {
        return ESP_ERR_NOT_FOUND;
    }

    chip_rev->min_chip_rev = img_hdr.min_chip_rev_full;
    chip_rev->max_chip_rev = img_hdr.max_chip_rev_full;

    return ESP_OK;
}

esp_err_t esp_att_utils_hexbuf_to_hexstr(const void *hexbuf, size_t hexbuf_sz, char *hexstr, size_t hexstr_len)
{
    if (hexbuf == NULL || hexstr == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    if (hexbuf_sz == 0 || hexstr_len < (hexbuf_sz * 2 + 1)) {
        return ESP_ERR_INVALID_SIZE;
    }

    const uint8_t *bytes = (const uint8_t *)hexbuf;

    for (size_t i = 0; i < hexbuf_sz; i++) {
        for (int shift = 0; shift < 2; shift++) {
            uint8_t nibble = (bytes[i] >> (shift ? 0 : 4)) & 0x0F;
            if (nibble < 10) {
                hexstr[i * 2 + shift] = '0' + nibble;
            } else {
                hexstr[i * 2 + shift] = 'a' + nibble - 10;
            }
        }
    }
    hexstr[hexbuf_sz * 2] = '\0';

    return ESP_OK;
}

esp_err_t esp_att_utils_get_btl_claim_data(esp_att_part_metadata_t *btl_metadata)
{
    esp_partition_pos_t btl_pos = {};
    esp_err_t err = get_active_part_pos(ESP_ATT_PART_TYPE_BOOTLOADER, &btl_pos);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to get running app partition!");
        return err;
    }

    esp_att_part_digest_info_t btl_digest = {};
    err = get_part_digest(&btl_pos, &btl_digest);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to get bootloader image digest!");
        return err;
    }

    esp_bootloader_desc_t btl_desc = {};
    err = get_part_btl_desc(&btl_pos, &btl_desc);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to get bootloader partition description!");
        return err;
    }

    esp_att_part_chip_rev_t btl_chip_rev = {};
    err = get_part_chip_rev(&btl_pos, &btl_chip_rev);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to get bootloader image chip rev!");
        return err;
    }

    /* Clearing and populating the bootloader FW metadata */
    memset(btl_metadata, 0x00, sizeof(esp_att_part_metadata_t));

    btl_metadata->type = ESP_ATT_PART_TYPE_BOOTLOADER;
    btl_metadata->secure_ver = btl_desc.secure_version;

    err = esp_att_utils_hexbuf_to_hexstr(&btl_desc.version, sizeof(btl_desc.version), btl_metadata->ver, sizeof(btl_metadata->ver));
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to fetch bootloader version!");
        return err;
    }

    memcpy(btl_metadata->idf_ver, &btl_desc.idf_ver, sizeof(btl_metadata->idf_ver));
    memcpy(&btl_metadata->part_digest, &btl_digest, sizeof(esp_att_part_digest_info_t));
    memcpy(&btl_metadata->part_chip_rev, &btl_chip_rev, sizeof(esp_att_part_chip_rev_t));

    return ESP_OK;
}

esp_err_t esp_att_utils_get_app_claim_data(esp_att_part_metadata_t *app_metadata)
{
    esp_partition_pos_t app_pos = {};
    esp_err_t err = get_active_part_pos(ESP_ATT_PART_TYPE_APP, &app_pos);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to get running app partition!");
        return err;
    }

    esp_att_part_digest_info_t app_digest = {};
    err = get_part_digest(&app_pos, &app_digest);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to get app image digest!");
        return err;
    }

    esp_app_desc_t ns_app_desc = {};
    err = get_part_app_desc(&app_pos, &ns_app_desc);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to get app partition description!");
        return err;
    }

    esp_att_part_chip_rev_t app_chip_rev = {};
    err = get_part_chip_rev(&app_pos, &app_chip_rev);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to get app image chip rev!");
        return err;
    }

    /* Clearing and populating the bootloader FW metadata */
    memset(app_metadata, 0x00, sizeof(esp_att_part_metadata_t));

    app_metadata->type = ESP_ATT_PART_TYPE_APP;
    app_metadata->secure_ver = ns_app_desc.secure_version;

    memcpy(app_metadata->ver, &ns_app_desc.version, sizeof(app_metadata->ver));
    memcpy(app_metadata->idf_ver, &ns_app_desc.idf_ver, sizeof(app_metadata->idf_ver));

    memcpy(&app_metadata->part_digest, &app_digest, sizeof(esp_att_part_digest_info_t));
    memcpy(&app_metadata->part_chip_rev, &app_chip_rev, sizeof(esp_att_part_chip_rev_t));

    return ESP_OK;
}

esp_err_t esp_att_utils_get_tee_claim_data(esp_att_part_metadata_t *tee_metadata)
{
#if ESP_TEE_BUILD
    esp_partition_pos_t tee_pos = {};
    esp_err_t err = get_active_part_pos(ESP_ATT_PART_TYPE_TEE, &tee_pos);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to get running tee partition!");
        return err;
    }

    esp_att_part_digest_info_t tee_digest = {};
    err = get_part_digest(&tee_pos, &tee_digest);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to get TEE image digest!");
        return err;
    }

    esp_app_desc_t tee_app_desc = {};
    err = get_part_app_desc(&tee_pos, &tee_app_desc);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to get TEE partition description!");
        return err;
    }

    esp_att_part_chip_rev_t tee_chip_rev = {};
    err = get_part_chip_rev(&tee_pos, &tee_chip_rev);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to get TEE image chip rev!");
        return err;
    }

    /* Clearing and populating the TEE FW metadata */
    memset(tee_metadata, 0x00, sizeof(esp_att_part_metadata_t));

    tee_metadata->type = ESP_ATT_PART_TYPE_TEE;
    tee_metadata->secure_ver = tee_app_desc.secure_version;

    memcpy(tee_metadata->ver, &tee_app_desc.version, sizeof(tee_metadata->ver));
    memcpy(tee_metadata->idf_ver, &tee_app_desc.idf_ver, sizeof(tee_metadata->idf_ver));

    memcpy(&tee_metadata->part_digest, &tee_digest, sizeof(esp_att_part_digest_info_t));
    memcpy(&tee_metadata->part_chip_rev, &tee_chip_rev, sizeof(esp_att_part_chip_rev_t));

    return ESP_OK;
#else
    ESP_LOGE(TAG, "TEE not supported!");
    return ESP_ERR_NOT_SUPPORTED;
#endif
}
