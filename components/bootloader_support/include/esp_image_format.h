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
#ifndef __ESP32_IMAGE_FORMAT_H
#define __ESP32_IMAGE_FORMAT_H

#include <stdbool.h>
#include <esp_err.h>

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
    uint8_t spi_mode;      /* flash read mode (esp_image_spi_mode_t as uint8_t) */
    uint8_t spi_speed: 4;  /* flash frequency (esp_image_spi_freq_t as uint8_t) */
    uint8_t spi_size: 4;   /* flash chip size (esp_image_flash_size_t as uint8_t) */
    uint32_t entry_addr;
    uint8_t encrypt_flag;    /* encrypt flag */
    uint8_t extra_header[15]; /* ESP32 additional header, unused by second bootloader */
}  esp_image_header_t;

/* Header of binary image segment */
typedef struct {
    uint32_t load_addr;
    uint32_t data_len;
} esp_image_segment_header_t;


/**
 * @brief Read an ESP image header from flash.
 *
 * If encryption is enabled, data will be transparently decrypted.
 *
 * @param src_addr Address in flash to load image header. Must be 4 byte aligned.
 * @param log_errors Log error output if image header appears invalid.
 * @param[out] image_header Pointer to an esp_image_header_t struture to be filled with data. If the function fails, contents are undefined.
 *
 * @return ESP_OK if image header was loaded, ESP_ERR_IMAGE_FLASH_FAIL
 * if a SPI flash error occurs, ESP_ERR_IMAGE_INVALID if the image header
 * appears invalid.
 */
esp_err_t esp_image_load_header(uint32_t src_addr, bool log_errors, esp_image_header_t *image_header);

/**
 * @brief Read the segment header and data offset of a segment in the image.
 *
 * If encryption is enabled, data will be transparently decrypted.
 *
 * @param index Index of the segment to load information for.
 * @param src_addr Base address in flash of the image.
 * @param[in] image_header Pointer to the flash image header, already loaded by @ref esp_image_load_header().
 * @param log_errors Log errors reading the segment header.
 * @param[out] segment_header Pointer to a segment header structure to be filled with data. If the function fails, contents are undefined.
 * @param[out] segment_data_offset Pointer to the data offset of the segment.
 *
 * @return ESP_OK if segment_header & segment_data_offset were loaded successfully, ESP_ERR_IMAGE_FLASH_FAIL if a SPI flash error occurs, ESP_ERR_IMAGE_INVALID if the image header appears invalid, ESP_ERR_INVALID_ARG if the index is invalid.
 */
esp_err_t esp_image_load_segment_header(uint8_t index, uint32_t src_addr, const esp_image_header_t *image_header, bool log_errors, esp_image_segment_header_t *segment_header, uint32_t *segment_data_offset);


/**
 * @brief Non-cryptographically validate app image integrity. On success, length of image is provided to caller.
 *
 * If the image has a secure boot signature appended, the signature is not checked and this length is not included in the
 * output value.
 *
 * Image validation checks:
 * - Magic byte
 * - No single segment longer than 16MB
 * - Total image no longer than 16MB
 * - 8 bit image checksum is valid
 *
 * If flash encryption is enabled, the image will be tranpsarently decrypted.
 *
 * @param src_addr Offset of the start of the image in flash. Must be 4 byte aligned.
 * @param allow_decrypt If true and flash encryption is enabled, the image will be transparently decrypted.
 * @param log_errors Log errors verifying the image.
 * @param[out] length Length of the image, set to a value if the image is valid. Can be null.
 *
 * @return ESP_OK if image is valid, ESP_FAIL or ESP_ERR_IMAGE_INVALID on errors.
 *
 */
esp_err_t esp_image_basic_verify(uint32_t src_addr, bool log_errors, uint32_t *length);


typedef struct {
    uint32_t drom_addr;
    uint32_t drom_load_addr;
    uint32_t drom_size;
    uint32_t irom_addr;
    uint32_t irom_load_addr;
    uint32_t irom_size;
} esp_image_flash_mapping_t;

#endif
