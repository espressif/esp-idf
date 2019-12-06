// Copyright 2015-2019 Espressif Systems (Shanghai) PTE LTD
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

/**
 * @brief ESP chip ID
 *
 */
typedef enum {
    ESP_CHIP_ID_ESP32 = 0x0000,  /*!< chip ID: ESP32 */
    ESP_CHIP_ID_INVALID = 0xFFFF /*!< Invalid chip ID (we defined it to make sure the esp_chip_id_t is 2 bytes size) */
} __attribute__((packed)) esp_chip_id_t;

/** @cond */
_Static_assert(sizeof(esp_chip_id_t) == 2, "esp_chip_id_t should be 16 bit");
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
 * @brief SPI flash clock frequency
 */
typedef enum {
    ESP_IMAGE_SPI_SPEED_40M,        /*!< SPI clock frequency 40 MHz */
    ESP_IMAGE_SPI_SPEED_26M,        /*!< SPI clock frequency 26 MHz */
    ESP_IMAGE_SPI_SPEED_20M,        /*!< SPI clock frequency 20 MHz */
    ESP_IMAGE_SPI_SPEED_80M = 0xF   /*!< SPI clock frequency 80 MHz */
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
    uint8_t min_chip_rev;       /*!< Minimum chip revision supported by image */
    uint8_t reserved[8];       /*!< Reserved bytes in additional header space, currently unused */
    uint8_t hash_appended;      /*!< If 1, a SHA256 digest "simple hash" (of the entire image) is appended after the checksum.
                                 * Included in image length. This digest
                                 * is separate to secure boot and only used for detecting corruption.
                                 * For secure boot signed images, the signature
                                 * is appended after this (and the simple hash is included in the signed data). */
} __attribute__((packed))  esp_image_header_t;

/** @cond */
_Static_assert(sizeof(esp_image_header_t) == 24, "binary image header should be 24 bytes");
/** @endcond */


/**
 * @brief Header of binary image segment
 */
typedef struct {
    uint32_t load_addr;     /*!< Address of segment */
    uint32_t data_len;      /*!< Length of data */
} esp_image_segment_header_t;

#define ESP_IMAGE_MAX_SEGMENTS 16           /*!< Max count of segments in the image. */

#define ESP_APP_DESC_MAGIC_WORD 0xABCD5432  /*!< The magic word for the esp_app_desc structure that is in DROM. */

/**
 * @brief Description about application.
 */
typedef struct {
    uint32_t magic_word;        /*!< Magic word ESP_APP_DESC_MAGIC_WORD */
    uint32_t secure_version;    /*!< Secure version */
    uint32_t reserv1[2];        /*!< reserv1 */
    char version[32];           /*!< Application version */
    char project_name[32];      /*!< Project name */
    char time[16];              /*!< Compile time */
    char date[16];              /*!< Compile date*/
    char idf_ver[32];           /*!< Version IDF */
    uint8_t app_elf_sha256[32]; /*!< sha256 of elf file */
    uint32_t reserv2[20];       /*!< reserv2 */
} esp_app_desc_t;

/** @cond */
_Static_assert(sizeof(esp_app_desc_t) == 256, "esp_app_desc_t should be 256 bytes");
/** @endcond */
