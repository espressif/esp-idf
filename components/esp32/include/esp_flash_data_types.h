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

#define ESP_PARTITION_MAGIC 0x50AA
#define ESP_PARTITION_MAGIC_MD5 0xEBEB

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

#define PART_TYPE_END 0xff
#define PART_SUBTYPE_END 0xff

#define PART_FLAG_ENCRYPTED (1<<0)

#ifdef __cplusplus
}
#endif

#endif //__ESP_BIN_TYPES_H__
