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

#define ESP_PARTITION_TABLE_ADDR 0x8000
#define ESP_PARTITION_MAGIC 0x50AA

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
