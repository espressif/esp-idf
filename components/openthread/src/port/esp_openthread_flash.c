/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_openthread_flash.h"
#include "esp_partition.h"
#include "openthread/instance.h"
#include "openthread/platform/flash.h"
#include "openthread/platform/settings.h"

#define ESP_OT_FLASH_PAGE_NUM 2
#define ESP_OT_FLASH_PAGE_SIZE 4096

static const esp_partition_t *s_ot_partition = NULL;

void esp_openthread_flash_set_partition(const esp_partition_t *partition)
{
    s_ot_partition = partition;
}

void otPlatFlashInit(otInstance *instance)
{
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
