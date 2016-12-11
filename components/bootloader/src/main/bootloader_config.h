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
#ifndef __BOOT_CONFIG_H__
#define __BOOT_CONFIG_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

#include "esp_flash_data_types.h"

#define BOOT_VERSION "V0.1"
#define SPI_SEC_SIZE 0x1000
#define IROM_LOW    0x400D0000
#define IROM_HIGH   0x40400000
#define DROM_LOW    0x3F400000
#define DROM_HIGH   0x3F800000
#define RTC_IRAM_LOW  0x400C0000
#define RTC_IRAM_HIGH 0x400C2000
#define RTC_DATA_LOW  0x50000000
#define RTC_DATA_HIGH 0x50002000

#define SPI_ERROR_LOG "spi flash error"

typedef struct {
    esp_partition_pos_t ota_info;
    esp_partition_pos_t factory;
    esp_partition_pos_t test;
    esp_partition_pos_t ota[16];
    uint32_t app_count;
    uint32_t selected_subtype;
} bootloader_state_t;

bool flash_encrypt(bootloader_state_t *bs);

#ifdef __cplusplus
}
#endif

#endif /* __BOOT_CONFIG_H__ */
