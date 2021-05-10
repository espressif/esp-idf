/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdbool.h>
#include <esp_err.h>
#include "esp_flash_partitions.h"
#include "esp_app_format.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ESP_ERR_IMAGE_BASE       0x2000
#define ESP_ERR_IMAGE_FLASH_FAIL (ESP_ERR_IMAGE_BASE + 1)
#define ESP_ERR_IMAGE_INVALID    (ESP_ERR_IMAGE_BASE + 2)

/* Support for app/bootloader image parsing
   Can be compiled as part of app or bootloader code.
*/

#define ESP_IMAGE_HASH_LEN 32 /* Length of the appended SHA-256 digest */

/* Structure to hold on-flash image metadata */
typedef struct {
  uint32_t start_addr;   /* Start address of image */
  esp_image_header_t image; /* Header for entire image */
  esp_image_segment_header_t segments[ESP_IMAGE_MAX_SEGMENTS]; /* Per-segment header data */
  uint32_t segment_data[ESP_IMAGE_MAX_SEGMENTS]; /* Data offsets for each segment */
  uint32_t image_len; /* Length of image on flash, in bytes */
  uint8_t image_digest[32]; /* appended SHA-256 digest */
} esp_image_metadata_t;

typedef enum {
    ESP_IMAGE_VERIFY,            /* Verify image contents, not load to memory, load metadata. Print errors. */
    ESP_IMAGE_VERIFY_SILENT,     /* Verify image contents, not load to memory, load metadata. Don't print errors. */
#ifdef BOOTLOADER_BUILD
    ESP_IMAGE_LOAD,              /* Verify image contents, load to memory, load metadata. Print errors. */
    ESP_IMAGE_LOAD_NO_VALIDATE,  /* Not verify image contents, load to memory, load metadata. Print errors. */
#endif
} esp_image_load_mode_t;

typedef struct {
    esp_partition_pos_t partition;  /*!< Partition of application which worked before goes to the deep sleep. */
    uint16_t reboot_counter;        /*!< Reboot counter. Reset only when power is off. */
    uint16_t reserve;               /*!< Reserve */
#ifdef CONFIG_BOOTLOADER_CUSTOM_RESERVE_RTC
    uint8_t custom[CONFIG_BOOTLOADER_CUSTOM_RESERVE_RTC_SIZE]; /*!< Reserve for custom propose */
#endif
    uint32_t crc;                   /*!< Check sum crc32 */
} rtc_retain_mem_t;

#ifdef CONFIG_BOOTLOADER_CUSTOM_RESERVE_RTC
_Static_assert(CONFIG_BOOTLOADER_CUSTOM_RESERVE_RTC_SIZE % 4 == 0, "CONFIG_BOOTLOADER_CUSTOM_RESERVE_RTC_SIZE must be a multiple of 4 bytes");
#endif

#if defined(CONFIG_BOOTLOADER_SKIP_VALIDATE_IN_DEEP_SLEEP) || defined(CONFIG_BOOTLOADER_CUSTOM_RESERVE_RTC)
_Static_assert(CONFIG_BOOTLOADER_RESERVE_RTC_SIZE % 4 == 0, "CONFIG_BOOTLOADER_RESERVE_RTC_SIZE must be a multiple of 4 bytes");
#endif

#ifdef CONFIG_BOOTLOADER_CUSTOM_RESERVE_RTC
#define ESP_BOOTLOADER_RESERVE_RTC (CONFIG_BOOTLOADER_RESERVE_RTC_SIZE + CONFIG_BOOTLOADER_CUSTOM_RESERVE_RTC_SIZE)
#elif defined(CONFIG_BOOTLOADER_SKIP_VALIDATE_IN_DEEP_SLEEP)
#define ESP_BOOTLOADER_RESERVE_RTC (CONFIG_BOOTLOADER_RESERVE_RTC_SIZE)
#endif

#if defined(CONFIG_BOOTLOADER_SKIP_VALIDATE_IN_DEEP_SLEEP) || defined(CONFIG_BOOTLOADER_CUSTOM_RESERVE_RTC)
_Static_assert(sizeof(rtc_retain_mem_t) <= ESP_BOOTLOADER_RESERVE_RTC, "Reserved RTC area must exceed size of rtc_retain_mem_t");
#endif

/**
 * @brief Verify an app image.
 *
 * If encryption is enabled, data will be transparently decrypted.
 *
 * @param mode Mode of operation (verify, silent verify, or load).
 * @param part Partition to load the app from.
 * @param[inout] data Pointer to the image metadata structure which is be filled in by this function.
 *                    'start_addr' member should be set (to the start address of the image.)
 *                    Other fields will all be initialised by this function.
 *
 * Image validation checks:
 * - Magic byte.
 * - Partition smaller than 16MB.
 * - All segments & image fit in partition.
 * - 8 bit image checksum is valid.
 * - SHA-256 of image is valid (if image has this appended).
 * - (Signature) if signature verification is enabled.
 *
 * @return
 * - ESP_OK if verify or load was successful
 * - ESP_ERR_IMAGE_FLASH_FAIL if a SPI flash error occurs
 * - ESP_ERR_IMAGE_INVALID if the image appears invalid.
 * - ESP_ERR_INVALID_ARG if the partition or data pointers are invalid.
 */
esp_err_t esp_image_verify(esp_image_load_mode_t mode, const esp_partition_pos_t *part, esp_image_metadata_t *data);

/**
 * @brief Get metadata of app
 *
 * If encryption is enabled, data will be transparently decrypted.
 *
 * @param part Partition to load the app from.
 * @param[out] metadata Pointer to the image metadata structure which is be filled in by this function.
 *                      Fields will all be initialised by this function.
 *
 * @return
 * - ESP_OK if filling of metadata was successful
 */
esp_err_t esp_image_get_metadata(const esp_partition_pos_t *part, esp_image_metadata_t *metadata);

/**
 * @brief Verify and load an app image (available only in space of bootloader).
 *
 * If encryption is enabled, data will be transparently decrypted.
 *
 * @param part Partition to load the app from.
 * @param[inout] data Pointer to the image metadata structure which is be filled in by this function.
 *                    'start_addr' member should be set (to the start address of the image.)
 *                    Other fields will all be initialised by this function.
 *
 * Image validation checks:
 * - Magic byte.
 * - Partition smaller than 16MB.
 * - All segments & image fit in partition.
 * - 8 bit image checksum is valid.
 * - SHA-256 of image is valid (if image has this appended).
 * - (Signature) if signature verification is enabled.
 *
 * @return
 * - ESP_OK if verify or load was successful
 * - ESP_ERR_IMAGE_FLASH_FAIL if a SPI flash error occurs
 * - ESP_ERR_IMAGE_INVALID if the image appears invalid.
 * - ESP_ERR_INVALID_ARG if the partition or data pointers are invalid.
 */
esp_err_t bootloader_load_image(const esp_partition_pos_t *part, esp_image_metadata_t *data);

/**
 * @brief Load an app image without verification (available only in space of bootloader).
 *
 * If encryption is enabled, data will be transparently decrypted.
 *
 * @param part Partition to load the app from.
 * @param[inout] data Pointer to the image metadata structure which is be filled in by this function.
 *                    'start_addr' member should be set (to the start address of the image.)
 *                    Other fields will all be initialised by this function.
 *
 * @return
 * - ESP_OK if verify or load was successful
 * - ESP_ERR_IMAGE_FLASH_FAIL if a SPI flash error occurs
 * - ESP_ERR_IMAGE_INVALID if the image appears invalid.
 * - ESP_ERR_INVALID_ARG if the partition or data pointers are invalid.
 */
esp_err_t bootloader_load_image_no_verify(const esp_partition_pos_t *part, esp_image_metadata_t *data);

/**
 * @brief Verify the bootloader image.
 *
 * @param[out] If result is ESP_OK and this pointer is non-NULL, it
 * will be set to the length of the bootloader image.
 *
 * @return As per esp_image_load_metadata().
 */
esp_err_t esp_image_verify_bootloader(uint32_t *length);

/**
 * @brief Verify the bootloader image.
 *
 * @param[out] Metadata for the image. Only valid if result is ESP_OK.
 *
 * @return As per esp_image_load_metadata().
 */
esp_err_t esp_image_verify_bootloader_data(esp_image_metadata_t *data);

/**
 * @brief Get the flash size of the image
 *
 * @param app_flash_size The value configured in the image header
 * @return Actual size, in bytes.
 */
int esp_image_get_flash_size(esp_image_flash_size_t app_flash_size);


typedef struct {
    uint32_t drom_addr;
    uint32_t drom_load_addr;
    uint32_t drom_size;
    uint32_t irom_addr;
    uint32_t irom_load_addr;
    uint32_t irom_size;
} esp_image_flash_mapping_t;

#ifdef __cplusplus
}
#endif
