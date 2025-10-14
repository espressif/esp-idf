/*
 * SPDX-FileCopyrightText: 2022-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "sdkconfig.h"
#include <sys/param.h>
#include <string.h>
#include "inttypes.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
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

TEST_CASE("Can find paddr caps by any paddr offset", "[mmu]")
{
    const esp_partition_t *part = s_get_partition();
    ESP_LOGI(TAG, "found partition '%s' at offset 0x%"PRIx32" with size 0x%"PRIx32, part->label, part->address, part->size);

    void *ptr0 = NULL;
    TEST_ESP_OK(esp_mmu_map(part->address, TEST_BLOCK_SIZE, MMU_TARGET_FLASH0, MMU_MEM_CAP_READ, 0, &ptr0));

    mmu_mem_caps_t caps = 0;
    TEST_ESP_OK(esp_mmu_paddr_find_caps(part->address, &caps));
    ESP_LOGI(TAG, "caps: 0x%x", caps);
    TEST_ASSERT(caps == MMU_MEM_CAP_READ);

    TEST_ESP_OK(esp_mmu_paddr_find_caps(part->address + 0x100, &caps));
    ESP_LOGI(TAG, "caps: 0x%x", caps);
    TEST_ASSERT(caps == MMU_MEM_CAP_READ);

    TEST_ESP_OK(esp_mmu_unmap(ptr0));
}

typedef struct {
    SemaphoreHandle_t done;
} test_task_arg_t;

void map_task(void *varg)
{
    esp_err_t ret = ESP_FAIL;
    test_task_arg_t* args = (test_task_arg_t*) varg;

    const esp_partition_t *part = s_get_partition();
    srand(199);
    int cnt = 0;
    while (true) {
        if (cnt >= 10000) {
            break;
        }
        size_t i = rand() % part->size;
        void *ptr0 = NULL;
        size_t phys_addr = part->address + i;
        size_t region_offset = phys_addr & (CONFIG_MMU_PAGE_SIZE - 1);
        size_t mmap_addr = phys_addr & ~(CONFIG_MMU_PAGE_SIZE - 1);
        ret = esp_mmu_map(mmap_addr, 1 + region_offset, MMU_TARGET_FLASH0, MMU_MEM_CAP_READ | MMU_MEM_CAP_8BIT, ESP_MMU_MMAP_FLAG_PADDR_SHARED, &ptr0);
        if (ret == ESP_OK) {
            esp_mmu_unmap(ptr0);
        }
        cnt++;
        vTaskDelay(pdMS_TO_TICKS(1));
    }

    xSemaphoreGive(args->done);
    vTaskDelete(NULL);
}

TEST_CASE("Test mmap concurrency", "[mmu][manual][ignore]")
{
    test_task_arg_t args0 = {
        .done = xSemaphoreCreateBinary(),
    };

    test_task_arg_t args1 = {
        .done = xSemaphoreCreateBinary(),
    };

    xTaskCreate(map_task, "map0_task", 8096, &args0, 1, NULL);
    xTaskCreate(map_task, "map1_task", 8096, &args1, 1, NULL);

    xSemaphoreTake(args0.done, portMAX_DELAY);
    xSemaphoreTake(args1.done, portMAX_DELAY);
    vTaskDelay(100);
    vSemaphoreDelete(args0.done);
    vSemaphoreDelete(args1.done);
}

#if CONFIG_SPIRAM
#if !CONFIG_IDF_TARGET_ESP32  //ESP32 doesn't support using `esp_mmu_map` to map to PSRAM
TEST_CASE("Can find paddr when mapping to psram", "[mmu]")
{
    esp_paddr_t paddr = 0;
    mmu_target_t target = MMU_TARGET_FLASH0;

    void *vaddr = NULL;
    esp_err_t err = ESP_FAIL;

    vaddr = heap_caps_malloc(10, MALLOC_CAP_SPIRAM);
    err = esp_mmu_vaddr_to_paddr(vaddr, &paddr, &target);
    if (err == ESP_OK) {
        ESP_LOGI("MMU", "Virtual Address: %p, Physical Address: 0x%lx, Target: %d", vaddr, paddr, target);
    } else {
        ESP_LOGE("MMU", "Failed to convert virtual address to physical address: %s", esp_err_to_name(err));
    }

    TEST_ASSERT(target == MMU_TARGET_PSRAM0);
}
#endif  //#if !CONFIG_IDF_TARGET_ESP32
#endif  //#if CONFIG_SPIRAM
