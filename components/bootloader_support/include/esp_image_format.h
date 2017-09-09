// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#pragma once

#include <stdbool.h>
#include <esp_err.h>
#include "esp_flash_partitions.h"

#define ESP_ERR_IMAGE_BASE       0x2000
#define ESP_ERR_IMAGE_FLASH_FAIL (ESP_ERR_IMAGE_BASE + 1)
#define ESP_ERR_IMAGE_INVALID    (ESP_ERR_IMAGE_BASE + 2)

/* Support for app/bootloader image parsing
   Can be compiled as part of app or bootloader code.
*/

/* SPI flash mode, used in esp_image_header_t */
typedef enum {
    ESP_IMAGE_SPI_MODE_QIO,
    ESP_IMAGE_SPI_MODE_QOUT,
    ESP_IMAGE_SPI_MODE_DIO,
    ESP_IMAGE_SPI_MODE_DOUT,
    ESP_IMAGE_SPI_MODE_FAST_READ,
    ESP_IMAGE_SPI_MODE_SLOW_READ
} esp_image_spi_mode_t;

/* SPI flash clock frequency */
enum {
    ESP_IMAGE_SPI_SPEED_40M,
    ESP_IMAGE_SPI_SPEED_26M,
    ESP_IMAGE_SPI_SPEED_20M,
    ESP_IMAGE_SPI_SPEED_80M = 0xF
} esp_image_spi_freq_t;

/* Supported SPI flash sizes */
typedef enum {
    ESP_IMAGE_FLASH_SIZE_1MB = 0,
    ESP_IMAGE_FLASH_SIZE_2MB,
    ESP_IMAGE_FLASH_SIZE_4MB,
    ESP_IMAGE_FLASH_SIZE_8MB,
    ESP_IMAGE_FLASH_SIZE_16MB,
    ESP_IMAGE_FLASH_SIZE_MAX
} esp_image_flash_size_t;

#define ESP_IMAGE_HEADER_MAGIC 0xE9

/* Main header of binary image */
typedef struct {
    uint8_t magic;
    uint8_t segment_count;
    /* flash read mode (esp_image_spi_mode_t as uint8_t) */
    uint8_t spi_mode;
    /* flash frequency (esp_image_spi_freq_t as uint8_t) */
    uint8_t spi_speed: 4;
    /* flash chip size (esp_image_flash_size_t as uint8_t) */
    uint8_t spi_size: 4;
    uint32_t entry_addr;
    /* WP pin when SPI pins set via efuse (read by ROM bootloader, the IDF bootloader uses software to configure the WP
     * pin and sets this field to 0xEE=disabled) */
    uint8_t wp_pin;
    /* Drive settings for the SPI flash pins (read by ROM bootloader) */
    uint8_t spi_pin_drv[3];
    /* Reserved bytes in ESP32 additional header space, currently unused */
    uint8_t reserved[11];
    /* If 1, a SHA256 digest "simple hash" (of the entire image) is appended after the checksum. Included in image length. This digest
     * is separate to secure boot and only used for detecting corruption. For secure boot signed images, the signature
     * is appended after this (and the simple hash is included in the signed data). */
    uint8_t hash_appended;
} __attribute__((packed))  esp_image_header_t;

_Static_assert(sizeof(esp_image_header_t) == 24, "binary image header should be 24 bytes");

/* Header of binary image segment */
typedef struct {
    uint32_t load_addr;
    uint32_t data_len;
} esp_image_segment_header_t;

#define ESP_IMAGE_MAX_SEGMENTS 16

/* Structure to hold on-flash image metadata */
typedef struct {
  uint32_t start_addr;   /* Start address of image */
  esp_image_header_t image; /* Header for entire image */
  esp_image_segment_header_t segments[ESP_IMAGE_MAX_SEGMENTS]; /* Per-segment header data */
  uint32_t segment_data[ESP_IMAGE_MAX_SEGMENTS]; /* Data offsets for each segment */
  uint32_t image_len; /* Length of image on flash, in bytes */
} esp_image_metadata_t;

/* Mode selection for esp_image_load() */
typedef enum {
    ESP_IMAGE_VERIFY,        /* Verify image contents, load metadata. Print errorsors. */
    ESP_IMAGE_VERIFY_SILENT, /* Verify image contents, load metadata. Don't print errors. */
#ifdef BOOTLOADER_BUILD
    ESP_IMAGE_LOAD,          /* Verify image contents, load to memory. Print errors. */
#endif
} esp_image_load_mode_t;

/**
 * @brief Verify and (optionally, in bootloader mode) load an app image.
 *
 * If encryption is enabled, data will be transparently decrypted.
 *
 * @param mode Mode of operation (verify, silent verify, or load).
 * @param part Partition to load the app from.
 * @param[inout] data Pointer to the image metadata structure which is be filled in by this function. 'start_addr' member should be set (to the start address of the image.) Other fields will all be initialised by this function.
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
esp_err_t esp_image_load(esp_image_load_mode_t mode, const esp_partition_pos_t *part, esp_image_metadata_t *data);

/**
 * @brief Verify the bootloader image.
 *
 * @param[out] If result is ESP_OK and this pointer is non-NULL, it
 * will be set to the length of the bootloader image.
 *
 * @return As per esp_image_load_metadata().
 */
esp_err_t esp_image_verify_bootloader(uint32_t *length);

typedef struct {
    uint32_t drom_addr;
    uint32_t drom_load_addr;
    uint32_t drom_size;
    uint32_t irom_addr;
    uint32_t irom_load_addr;
    uint32_t irom_size;
} esp_image_flash_mapping_t;
