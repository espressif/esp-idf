/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "esp_err.h"
#include "esp_types.h"
#include "sdkconfig.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ESP_PARTITION_MAGIC 0x50AA
#define ESP_PARTITION_MAGIC_MD5 0xEBEB

#define PART_TYPE_APP 0x00
#define PART_SUBTYPE_FACTORY  0x00
#define PART_SUBTYPE_OTA_FLAG 0x10
#define PART_SUBTYPE_OTA_MASK 0x0f
#define PART_SUBTYPE_TEST     0x20

#define PART_TYPE_DATA 0x01
#define PART_SUBTYPE_DATA_OTA 0x00
#define PART_SUBTYPE_DATA_RF  0x01
#define PART_SUBTYPE_DATA_WIFI 0x02
#define PART_SUBTYPE_DATA_NVS_KEYS 0x04
#define PART_SUBTYPE_DATA_EFUSE_EM 0x05

#define PART_TYPE_BOOTLOADER 0x02
#define PART_SUBTYPE_BOOTLOADER_PRIMARY 0x00
#define PART_SUBTYPE_BOOTLOADER_OTA 0x01

#define PART_TYPE_PARTITION_TABLE 0x03
#define PART_SUBTYPE_PARTITION_TABLE_PRIMARY 0x00
#define PART_SUBTYPE_PARTITION_TABLE_OTA 0x01

#define PART_TYPE_END 0xff
#define PART_SUBTYPE_END 0xff

#define PART_FLAG_ENCRYPTED (1<<0)
#define PART_FLAG_READONLY (1<<1)

/* The md5sum value is found this many bytes after the ESP_PARTITION_MAGIC_MD5 offset */
#define ESP_PARTITION_MD5_OFFSET 16

/* Pre-partition table fixed flash offsets */
#define ESP_BOOTLOADER_DIGEST_OFFSET 0x0
#define ESP_BOOTLOADER_OFFSET CONFIG_BOOTLOADER_OFFSET_IN_FLASH  /* Offset of bootloader image. Has matching value in bootloader KConfig.projbuild file. */
#define ESP_PRIMARY_BOOTLOADER_OFFSET CONFIG_BOOTLOADER_OFFSET_IN_FLASH  /* Offset of Primary bootloader image. */
#define ESP_PARTITION_TABLE_OFFSET CONFIG_PARTITION_TABLE_OFFSET /* Offset of partition table. Backwards-compatible name.*/
#define ESP_PRIMARY_PARTITION_TABLE_OFFSET CONFIG_PARTITION_TABLE_OFFSET /* Offset of partition table. */
#define ESP_PARTITION_TABLE_SIZE (0x1000) /* The partition table occupies 1 sector of flash (SPI_FLASH_SEC_SIZE) */
#define ESP_BOOTLOADER_SIZE (ESP_PARTITION_TABLE_OFFSET - ESP_BOOTLOADER_OFFSET)

#define ESP_PARTITION_TABLE_MAX_LEN 0xC00 /* Maximum length of partition table data */
#define ESP_PARTITION_TABLE_MAX_ENTRIES (ESP_PARTITION_TABLE_MAX_LEN / sizeof(esp_partition_info_t)) /* Maximum length of partition table data, including terminating entry */

/// OTA_DATA states for checking operability of the app.
typedef enum {
    ESP_OTA_IMG_NEW             = 0x0U,         /*!< Monitor the first boot. In bootloader this state is changed to ESP_OTA_IMG_PENDING_VERIFY. */
    ESP_OTA_IMG_PENDING_VERIFY  = 0x1U,         /*!< First boot for this app was. If while the second boot this state is then it will be changed to ABORTED. */
    ESP_OTA_IMG_VALID           = 0x2U,         /*!< App was confirmed as workable. App can boot and work without limits. */
    ESP_OTA_IMG_INVALID         = 0x3U,         /*!< App was confirmed as non-workable. This app will not selected to boot at all. */
    ESP_OTA_IMG_ABORTED         = 0x4U,         /*!< App could not confirm the workable or non-workable. In bootloader IMG_PENDING_VERIFY state will be changed to IMG_ABORTED. This app will not selected to boot at all. */
    ESP_OTA_IMG_UNDEFINED       = 0xFFFFFFFFU,  /*!< Undefined. App can boot and work without limits. */
} esp_ota_img_states_t;

/* OTA selection structure (two copies in the OTA data partition.)
   Size of 32 bytes is friendly to flash encryption */
typedef struct {
    uint32_t ota_seq;
    uint8_t  seq_label[20];
    uint32_t ota_state;
    uint32_t crc; /* CRC32 of ota_seq field only */
} esp_ota_select_entry_t;


typedef struct {
    uint32_t offset;
    uint32_t size;
} esp_partition_pos_t;

/* Structure which describes the layout of partition table entry.
 * See docs/partition_tables.rst for more information about individual fields.
 */
typedef struct {
    uint16_t magic;
    uint8_t  type;
    uint8_t  subtype;
    esp_partition_pos_t pos;
    uint8_t  label[16];
    uint32_t flags;
} esp_partition_info_t;

/* @brief Verify the partition table
 *
 * @param partition_table Pointer to at least ESP_PARTITION_TABLE_MAX_ENTRIES of potential partition table data. (ESP_PARTITION_TABLE_MAX_LEN bytes.)
 * @param log_errors Log errors if the partition table is invalid.
 * @param num_partitions If result is ESP_OK, num_partitions is updated with total number of partitions (not including terminating entry).
 *
 * @return ESP_OK on success, ESP_ERR_INVALID_STATE if partition table is not valid.
 */
esp_err_t esp_partition_table_verify(const esp_partition_info_t *partition_table, bool log_errors, int *num_partitions);

/**
 * Check whether the region on the main flash is not read-only.
 *
 * @param addr Start address of the region
 * @param size Size of the region
 *
 * @return true if the region is safe to write, otherwise false.
 */
bool esp_partition_is_flash_region_writable(size_t addr, size_t size);

/**
 * Check whether the region on the main flash is safe to write.
 *
 * @param addr Start address of the region
 * @param size Size of the region
 *
 * @return true if the region is safe to write, otherwise false.
 */
bool esp_partition_main_flash_region_safe(size_t addr, size_t size);

#ifdef __cplusplus
}
#endif
