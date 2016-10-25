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
#ifndef __ESP_BIN_TYPES_H__
#define __ESP_BIN_TYPES_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

#define ESP_PARTITION_TABLE_ADDR 0x4000
#define ESP_PARTITION_MAGIC 0x50AA

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

/* Main header of binary image */
typedef struct {
    uint8_t magic;
    uint8_t blocks;
    uint8_t spi_mode;      /* flash read mode (esp_image_spi_mode_t as uint8_t) */
    uint8_t spi_speed: 4;  /* flash frequency (esp_image_spi_freq_t as uint8_t) */
    uint8_t spi_size: 4;   /* flash chip size (esp_image_flash_size_t as uint8_t) */
    uint32_t entry_addr;
    uint8_t encrypt_flag;    /* encrypt flag */
    uint8_t secure_boot_flag; /* secure boot flag */
    uint8_t extra_header[14]; /* ESP32 additional header, unused by second bootloader */
}  esp_image_header_t;

/* Header of binary image segment */
typedef struct {
    uint32_t load_addr;
    uint32_t data_len;
} esp_image_section_header_t;


/* OTA selection structure (two copies in the OTA data partition.)
   Size of 32 bytes is friendly to flash encryption */
typedef struct {
    uint32_t ota_seq;
    uint8_t  seq_label[24];
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
    uint8_t  reserved[4];
} esp_partition_info_t;


#ifdef __cplusplus
}
#endif

#endif //__ESP_BIN_TYPES_H__
