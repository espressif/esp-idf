/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>

#include "esp_err.h"
#include "esp_flash_partitions.h"
#include "esp_image_format.h"

#ifdef __cplusplus
extern "C" {
#endif

// TEE otadata magic is derived from sha256 of "tee_ota" string
#define TEE_OTADATA_MAGIC 0x4337e1e1

/* TEE OTA selection structure (two copies in the TEE OTA data partition) */
typedef struct {
    uint32_t magic;          // A magic byte for otadata structure
    uint8_t version;         // OTA image version
    uint8_t boot_partition;  // Default boot partition
    uint8_t ota_state;       // OTA_DATA states for checking operability of the app
    uint8_t reserved_1;      // Reserved field 1
    uint32_t reserved_2[5];  // Reserved fields 2
    uint32_t crc;            // CRC32 of all fields in the structure
} __attribute__((packed)) esp_tee_ota_select_entry_t;

ESP_STATIC_ASSERT(offsetof(esp_tee_ota_select_entry_t, crc) == sizeof(esp_tee_ota_select_entry_t) - sizeof(uint32_t));

// OTA_DATA states for checking operability of the app.
typedef enum {
    ESP_TEE_OTA_IMG_NEW             = 0x00U,  /*!< Monitor the first boot - the bootloader changes the state to PENDING_VERIFY. */
    ESP_TEE_OTA_IMG_PENDING_VERIFY  = 0x33U,  /*!< If encountered during the second boot, the bootloader changes the state to INVALID. */
    ESP_TEE_OTA_IMG_INVALID         = 0x55U,  /*!< App was confirmed as workable - can boot and work without limits. */
    ESP_TEE_OTA_IMG_VALID           = 0xAAU,  /*!< App was confirmed as non-workable - will not selected to boot at all. */
    ESP_TEE_OTA_IMG_UNDEFINED       = 0xFFU,  /*!< Undefined. */
} esp_tee_ota_img_states_t;

#ifdef __cplusplus
}
#endif
