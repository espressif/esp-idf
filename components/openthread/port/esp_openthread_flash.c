// Copyright 2021 Espressif Systems (Shanghai) CO LTD
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
// limitations under the Licens

#include "esp_partition.h"
#include "esp_spi_flash.h"
#include "openthread/instance.h"
#include "openthread/platform/flash.h"
#include "openthread/platform/settings.h"

#define ESP_OT_FLASH_PAGE_NUM 2
#define ESP_OT_FLASH_PAGE_SIZE 4096

static const esp_partition_t *s_ot_partition = NULL;

void otPlatFlashInit(otInstance *instance)
{
    s_ot_partition = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_ANY, CONFIG_OPENTHREAD_PARTITION_NAME);

    assert(s_ot_partition != NULL);
    assert(s_ot_partition->size >= otPlatFlashGetSwapSize(instance));
}

uint32_t otPlatFlashGetSwapSize(otInstance *instance)
{
    return ESP_OT_FLASH_PAGE_SIZE;
}

void otPlatFlashErase(otInstance *instance, uint8_t index)
{
    uint32_t address = ESP_OT_FLASH_PAGE_SIZE * (index != 0);
    uint32_t size = ESP_OT_FLASH_PAGE_SIZE;
    esp_err_t err = ESP_OK;

    err = esp_partition_erase_range(s_ot_partition, address, size);

    assert(err == ESP_OK);
}

void otPlatFlashRead(otInstance *instance, uint8_t index, uint32_t offset, void *data, uint32_t size)
{
    esp_err_t err = ESP_OK;

    offset += ESP_OT_FLASH_PAGE_SIZE * (index != 0);

    err = esp_partition_read(s_ot_partition, offset, data, size);

    assert(err == ESP_OK);
}

void otPlatFlashWrite(otInstance *instance, uint8_t index, uint32_t offset, const void *data, uint32_t size)
{
    esp_err_t err = ESP_OK;

    offset += ESP_OT_FLASH_PAGE_SIZE * (index != 0);

    err = esp_partition_write(s_ot_partition, offset, data, size);

    assert(err == ESP_OK);
}
