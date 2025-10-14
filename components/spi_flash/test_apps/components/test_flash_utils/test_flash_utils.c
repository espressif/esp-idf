/*
 * SPDX-FileCopyrightText: 2023-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include <stdio.h>
#include "test_flash_utils.h"
#include "hal/cache_ll.h"
#include "esp_intr_alloc.h"
#include "esp_flash.h"
#include "esp_rom_sys.h"
#include "esp_partition.h"
#include "esp_log.h"
#include "esp_check.h"

#define ALIGN_UP_TO_64KB(x) (((x) + 0xFFFF) & ~0xFFFF)

const esp_partition_t *get_test_flash_partition(void)
{
    /* This finds "flash_test" partition defined in custom partitions.csv */
    const esp_partition_t *result = esp_partition_find_first(ESP_PARTITION_TYPE_DATA,
            ESP_PARTITION_SUBTYPE_ANY, "flash_test");
    assert(result != NULL); /* means partition table set wrong */
    return result;
}

#if SOC_SPI_MEM_SUPPORT_AUTO_SUSPEND
#include "hal/spimem_flash_ll.h"

static const char TAG[] = "test-flash";

IRAM_ATTR static bool flash_auto_suspend_done(flash_test_handle_t *handle, const flash_test_event_data_t *event_data, void *arg)
{
    handle->suspend_times++;
    return false;
}

IRAM_ATTR static void flash_suspend_isr_handler(void *arg)
{
    flash_test_handle_t *flash_test = (flash_test_handle_t *)arg;
    uint32_t val;
    spimem_flash_ll_get_intr_mask(flash_test->dev, &val);
    spimem_flash_ll_clear_intr_mask(flash_test->dev, val);

    if (val & SPIMEM_FLASH_LL_SUSPEND_END_INTR) {
        if (flash_test->auto_suspend_done) {
            flash_test->auto_suspend_done(flash_test, NULL, flash_test);
        }
    }
}

esp_err_t spi_flash_suspend_test_init(flash_test_handle_t **test_handle)
{
    esp_err_t ret = ESP_OK;
    flash_test_handle_t *handle = heap_caps_calloc(1, sizeof(flash_test_handle_t), MALLOC_CAP_DEFAULT);
    ESP_RETURN_ON_FALSE(handle, ESP_ERR_NO_MEM, TAG, "no memory for flash test handle");

    handle->dev = spimem_flash_ll_get_hw(SPI1_HOST);

    handle->auto_suspend_done = flash_auto_suspend_done;

    spimem_flash_ll_enable_intr_mask(handle->dev, SPIMEM_FLASH_LL_SUSPEND_END_INTR);

    ret = esp_intr_alloc_intrstatus(SPIMEM_FLASH_LL_INTERRUPT_SOURCE, ESP_INTR_FLAG_IRAM | ESP_INTR_FLAG_LOWMED, (uint32_t)spimem_flash_ll_get_interrupt_status_reg(handle->dev), SPIMEM_FLASH_LL_SUSPEND_END_INTR, flash_suspend_isr_handler, handle, &handle->intr_handle);
    ESP_GOTO_ON_ERROR(ret, err, TAG, "interrupt allocate for spi flash failed");

    const esp_partition_t *last_partition = NULL;
    esp_partition_iterator_t it = esp_partition_find(ESP_PARTITION_TYPE_ANY, ESP_PARTITION_SUBTYPE_ANY, NULL);

    // find the last valid partition
    while (it != NULL) {
        const esp_partition_t *current_partition = esp_partition_get(it);
        if (last_partition == NULL || current_partition->address > last_partition->address) {
            last_partition = current_partition;
        }
        it = esp_partition_next(it);
    }

    esp_partition_iterator_release(it);

    uint32_t flash_size;
    esp_flash_get_size(last_partition->flash_chip, &flash_size);

    handle->operation_address = ALIGN_UP_TO_64KB(last_partition->address + last_partition->size);
    handle->operation_size = flash_size - (last_partition->address + last_partition->size);

    if (handle->operation_size < 64 * 1024) {
        ESP_GOTO_ON_ERROR(ret, err, TAG, "reset memory is so small");
    }

    *test_handle = handle;

    return ESP_OK;
err:
    if (handle) {
        free(handle);
    }
    return ret;
}

const esp_partition_t * spi_flash_suspend_test_find_last_partition(void) {

    const esp_partition_t *last_partition = NULL;
    esp_partition_iterator_t it = esp_partition_find(ESP_PARTITION_TYPE_ANY, ESP_PARTITION_SUBTYPE_ANY, NULL);

    if (it == NULL) {
        ESP_LOGE(TAG, "No partitions found!");
        return NULL;
    }

    // find the last valid partition
    while (it != NULL) {
        const esp_partition_t *current_partition = esp_partition_get(it);
        if (last_partition == NULL || current_partition->address > last_partition->address) {
            last_partition = current_partition;
        }
        it = esp_partition_next(it);
    }

    esp_partition_iterator_release(it);

    return last_partition;
}

esp_err_t spi_flash_suspend_test_deinit(flash_test_handle_t *handle)
{
    ESP_RETURN_ON_FALSE(handle, ESP_ERR_INVALID_ARG, TAG, "no memory for flash test handle");
    if (handle->intr_handle) {
        ESP_RETURN_ON_ERROR(esp_intr_free(handle->intr_handle), TAG, "delete interrupt service failed");
    }
    free(handle);

    return ESP_OK;
}

void spi_flash_suspend_test_invalidate_cache(void)
{
    cache_ll_invalidate_all(CACHE_LL_LEVEL_ALL, CACHE_TYPE_ALL, CACHE_LL_ID_ALL);
}

#endif // SOC_SPI_MEM_SUPPORT_AUTO_SUSPEND
