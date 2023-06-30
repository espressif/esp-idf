/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <inttypes.h>
#include "esp_assert.h"

/**
 * @brief ESP chip ID
 *
 */
typedef enum {
    ESP_CHIP_ID_ESP32 = 0x0000,  /*!< chip ID: ESP32 */
    ESP_CHIP_ID_ESP32S2 = 0x0002,  /*!< chip ID: ESP32-S2 */
    ESP_CHIP_ID_ESP32C3 = 0x0005, /*!< chip ID: ESP32-C3 */
    ESP_CHIP_ID_ESP32S3 = 0x0009, /*!< chip ID: ESP32-S3 */
    ESP_CHIP_ID_ESP32C2 = 0x000C, /*!< chip ID: ESP32-C2 */
    ESP_CHIP_ID_ESP32C6 = 0x000D, /*!< chip ID: ESP32-C6 */
    ESP_CHIP_ID_ESP32H2 = 0x0010, /*!< chip ID: ESP32-H2 */
    ESP_CHIP_ID_INVALID = 0xFFFF /*!< Invalid chip ID (we defined it to make sure the esp_chip_id_t is 2 bytes size) */
} __attribute__((packed)) esp_chip_id_t;

/** @cond */
ESP_STATIC_ASSERT(sizeof(esp_chip_id_t) == 2, "esp_chip_id_t should be 16 bit");
/** @endcond */

/**
 * @brief SPI flash mode, used in esp_image_header_t
 */
typedef enum {
    ESP_IMAGE_SPI_MODE_QIO,         /*!< SPI mode QIO */
    ESP_IMAGE_SPI_MODE_QOUT,        /*!< SPI mode QOUT */
    ESP_IMAGE_SPI_MODE_DIO,         /*!< SPI mode DIO */
    ESP_IMAGE_SPI_MODE_DOUT,        /*!< SPI mode DOUT */
    ESP_IMAGE_SPI_MODE_FAST_READ,   /*!< SPI mode FAST_READ */
    ESP_IMAGE_SPI_MODE_SLOW_READ    /*!< SPI mode SLOW_READ */
} esp_image_spi_mode_t;

/**
 * @brief SPI flash clock division factor.
 */
typedef enum {
    ESP_IMAGE_SPI_SPEED_DIV_2,        /*!< The SPI flash clock frequency is divided by 2 of the clock source */
    ESP_IMAGE_SPI_SPEED_DIV_3,        /*!< The SPI flash clock frequency is divided by 3 of the clock source */
    ESP_IMAGE_SPI_SPEED_DIV_4,        /*!< The SPI flash clock frequency is divided by 4 of the clock source */
    ESP_IMAGE_SPI_SPEED_DIV_1 = 0xF   /*!< The SPI flash clock frequency equals to the clock source */
} esp_image_spi_freq_t;

/**
 * @brief Supported SPI flash sizes
 */
typedef enum {
    ESP_IMAGE_FLASH_SIZE_1MB = 0,   /*!< SPI flash size 1 MB */
    ESP_IMAGE_FLASH_SIZE_2MB,       /*!< SPI flash size 2 MB */
    ESP_IMAGE_FLASH_SIZE_4MB,       /*!< SPI flash size 4 MB */
    ESP_IMAGE_FLASH_SIZE_8MB,       /*!< SPI flash size 8 MB */
    ESP_IMAGE_FLASH_SIZE_16MB,      /*!< SPI flash size 16 MB */
    ESP_IMAGE_FLASH_SIZE_32MB,      /*!< SPI flash size 32 MB */
    ESP_IMAGE_FLASH_SIZE_64MB,      /*!< SPI flash size 64 MB */
    ESP_IMAGE_FLASH_SIZE_128MB,     /*!< SPI flash size 128 MB */
    ESP_IMAGE_FLASH_SIZE_MAX        /*!< SPI flash size MAX */
} esp_image_flash_size_t;

#define ESP_IMAGE_HEADER_MAGIC 0xE9 /*!< The magic word for the esp_image_header_t structure. */

/**
 * @brief Main header of binary image
 */
typedef struct {
    uint8_t magic;              /*!< Magic word ESP_IMAGE_HEADER_MAGIC */
    uint8_t segment_count;      /*!< Count of memory segments */
    uint8_t spi_mode;           /*!< flash read mode (esp_image_spi_mode_t as uint8_t) */
    uint8_t spi_speed: 4;       /*!< flash frequency (esp_image_spi_freq_t as uint8_t) */
    uint8_t spi_size: 4;        /*!< flash chip size (esp_image_flash_size_t as uint8_t) */
    uint32_t entry_addr;        /*!< Entry address */
    uint8_t wp_pin;            /*!< WP pin when SPI pins set via efuse (read by ROM bootloader,
                                * the IDF bootloader uses software to configure the WP
                                * pin and sets this field to 0xEE=disabled) */
    uint8_t spi_pin_drv[3];     /*!< Drive settings for the SPI flash pins (read by ROM bootloader) */
    esp_chip_id_t chip_id;      /*!< Chip identification number */
    uint8_t min_chip_rev;       /*!< Minimal chip revision supported by image
                                 * After the Major and Minor revision eFuses were introduced into the chips, this field is no longer used.
                                 * But for compatibility reasons, we keep this field and the data in it.
                                 * Use min_chip_rev_full instead.
                                 * The software interprets this as a Major version for most of the chips and as a Minor version for the ESP32-C3.
                                 */
    uint16_t min_chip_rev_full; /*!< Minimal chip revision supported by image, in format: major * 100 + minor */
    uint16_t max_chip_rev_full; /*!< Maximal chip revision supported by image, in format: major * 100 + minor */
    uint8_t reserved[4];        /*!< Reserved bytes in additional header space, currently unused */
    uint8_t hash_appended;      /*!< If 1, a SHA256 digest "simple hash" (of the entire image) is appended after the checksum.
                                 * Included in image length. This digest
                                 * is separate to secure boot and only used for detecting corruption.
                                 * For secure boot signed images, the signature
                                 * is appended after this (and the simple hash is included in the signed data). */
} __attribute__((packed))  esp_image_header_t;

/** @cond */
ESP_STATIC_ASSERT(sizeof(esp_image_header_t) == 24, "binary image header should be 24 bytes");
/** @endcond */


/**
 * @brief Header of binary image segment
 */
typedef struct {
    uint32_t load_addr;     /*!< Address of segment */
    uint32_t data_len;      /*!< Length of data */
} esp_image_segment_header_t;

#define ESP_IMAGE_MAX_SEGMENTS 16           /*!< Max count of segments in the image. */
