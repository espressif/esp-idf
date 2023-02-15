/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "sdkconfig.h"
#include <sys/param.h>
#include <string.h>
#include "inttypes.h"
#include "esp_log.h"
#include "esp_attr.h"
#include "unity.h"
#include "esp_heap_caps.h"
#include "esp_partition.h"

#include "esp_mmu_map.h"
#include "esp_rom_sys.h"

#define TEST_BLOCK_SIZE    CONFIG_MMU_PAGE_SIZE

const static char *TAG = "MMU_TEST";

static const esp_partition_t *s_get_partition(void)
{
    //Find the "storage1" partition defined in `partitions.csv`
    const esp_partition_t *result = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_ANY, "storage1");
    if (!result) {
        ESP_LOGE(TAG, "Can't find the partition, please define it correctly in `partitions.csv`");
        abort();
    }
    return result;
}

TEST_CASE("Can dump mapped block stats", "[mmu]")
{
    const esp_partition_t *part = s_get_partition();
    ESP_LOGI(TAG, "found partition '%s' at offset 0x%"PRIx32" with size 0x%"PRIx32, part->label, part->address, part->size);

    void *ptr0 = NULL;
    TEST_ESP_OK(esp_mmu_map(part->address, TEST_BLOCK_SIZE, MMU_TARGET_FLASH0, MMU_MEM_CAP_READ, 0, &ptr0));
    void *ptr1 = NULL;
    TEST_ESP_OK(esp_mmu_map(part->address + TEST_BLOCK_SIZE, TEST_BLOCK_SIZE, MMU_TARGET_FLASH0, MMU_MEM_CAP_EXEC, 0, &ptr1));
    void *ptr2 = NULL;
    TEST_ESP_OK(esp_mmu_map(part->address + 2 * TEST_BLOCK_SIZE, TEST_BLOCK_SIZE, MMU_TARGET_FLASH0, MMU_MEM_CAP_READ, 0, &ptr2));

    esp_mmu_map_dump_mapped_blocks(stdout);

    TEST_ESP_OK(esp_mmu_unmap(ptr0));
    TEST_ESP_OK(esp_mmu_unmap(ptr1));
    TEST_ESP_OK(esp_mmu_unmap(ptr2));
}
