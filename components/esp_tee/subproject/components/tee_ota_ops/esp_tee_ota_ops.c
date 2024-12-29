/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <inttypes.h>
#include <assert.h>

#include "esp_log.h"
#include "esp_err.h"
#include "hal/efuse_hal.h"

#include "spi_flash_mmap.h"
#include "esp_image_format.h"
#include "bootloader_utility_tee.h"

#include "esp_tee.h"
#include "esp_tee_flash.h"

#include "sdkconfig.h"

typedef enum {
    ESP_TEE_OTA_UNDEFINED = 0,
    ESP_TEE_OTA_BEGIN,
    ESP_TEE_OTA_IN_PROGRESS,
    ESP_TEE_OTA_END,
} esp_tee_ota_state_t;

typedef struct {
    esp_tee_ota_state_t tee_ota_state;
    esp_partition_pos_t tee_ota_data;
    esp_partition_info_t tee_next;
} esp_tee_ota_handle_t;

/* Global handle for managing TEE OTA */
static esp_tee_ota_handle_t ota_handle = {};

static const char *TAG = "esp_tee_ota_ops";

static esp_err_t get_tee_otadata_part_pos(esp_partition_pos_t *tee_ota_pos)
{
    if (tee_ota_pos == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    esp_partition_info_t tee_ota_info = {};
    esp_err_t err = esp_tee_flash_find_partition(PART_TYPE_DATA, PART_SUBTYPE_DATA_TEE_OTA, NULL, &tee_ota_info);
    if (err != ESP_OK) {
        return err;
    }

    memcpy(tee_ota_pos, &tee_ota_info.pos, sizeof(esp_partition_pos_t));
    return ESP_OK;
}

esp_err_t esp_tee_ota_begin(void)
{
    /* Initialising the TEE OTA handle */
    memset(&ota_handle, 0x00, sizeof(esp_tee_ota_handle_t));
    ota_handle.tee_ota_state = ESP_TEE_OTA_UNDEFINED;

    esp_err_t err = get_tee_otadata_part_pos(&ota_handle.tee_ota_data);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to fetch TEE OTA data partition!");
        return err;
    }

    ota_handle.tee_ota_state = ESP_TEE_OTA_BEGIN;

    int tee_boot_part = bootloader_utility_tee_get_boot_partition(&ota_handle.tee_ota_data);
    int tee_next_boot_part = bootloader_utility_tee_get_next_update_partition(&ota_handle.tee_ota_data);

    esp_partition_info_t tee_next = {};
    err = esp_tee_flash_find_partition(PART_TYPE_APP, (uint8_t)tee_next_boot_part, NULL, &tee_next);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to find partition!");
        return err;
    }

    ESP_LOGI(TAG, "Running partition - Subtype: 0x%x", (uint8_t)tee_boot_part);
    ESP_LOGI(TAG, "Next partition - Subtype: 0x%x (Offset: 0x%" PRIx32 ")", (uint8_t)tee_next_boot_part, tee_next.pos.offset);

    const uint32_t aligned_erase_size = (tee_next.pos.size + SPI_FLASH_SEC_SIZE - 1) & ~(SPI_FLASH_SEC_SIZE - 1);
    int ret = esp_tee_flash_erase_range(tee_next.pos.offset, aligned_erase_size);
    if (ret != 0) {
        ESP_LOGE(TAG, "Failed to erase partition!");
        return ESP_ERR_FLASH_OP_FAIL;
    }

    memcpy(&ota_handle.tee_next, &tee_next, sizeof(esp_partition_info_t));
    ota_handle.tee_ota_state = ESP_TEE_OTA_IN_PROGRESS;

    return ESP_OK;
}

esp_err_t esp_tee_ota_write(uint32_t rel_offset, const void *data, size_t size)
{
    if (data == NULL || size == 0) {
        ESP_LOGE(TAG, "Data cannot be NULL!");
        return ESP_ERR_INVALID_ARG;
    }

    if (ota_handle.tee_ota_state != ESP_TEE_OTA_IN_PROGRESS) {
        ESP_LOGE(TAG, "TEE OTA found to be in an invalid state!");
        return ESP_ERR_INVALID_STATE;
    }

    if (rel_offset + size > ota_handle.tee_next.pos.size) {
        ESP_LOGE(TAG, "Out of region write not allowed!");
        return ESP_FAIL;
    }

    ESP_LOGD(TAG, "Writing at offset: 0x%"PRIx32" | size: 0x%"PRIx32, rel_offset, size);

    bool write_encrypted = efuse_hal_flash_encryption_enabled();
    int ret = esp_tee_flash_write(ota_handle.tee_next.pos.offset + rel_offset, (void *)data, size, write_encrypted);
    if (ret != 0) {
        ESP_LOGE(TAG, "Failed to write partition!");
        return ESP_ERR_FLASH_OP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_tee_ota_end(void)
{
    if (ota_handle.tee_ota_state != ESP_TEE_OTA_IN_PROGRESS) {
        ESP_LOGE(TAG, "TEE OTA found to be in an invalid state!");
        return ESP_ERR_INVALID_STATE;
    }

    esp_err_t err = bootloader_utility_tee_set_boot_partition(&ota_handle.tee_ota_data, &ota_handle.tee_next);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set TEE boot partition (0x%"PRIx32")", err);
        return err;
    }

    ota_handle.tee_ota_state = ESP_TEE_OTA_END;
    return ESP_OK;
}
