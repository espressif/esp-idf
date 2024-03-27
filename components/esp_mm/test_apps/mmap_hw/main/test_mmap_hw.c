/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "sdkconfig.h"
#include <sys/param.h>
#include <string.h>
#include <sys/queue.h>
#include "inttypes.h"
#include "esp_log.h"
#include "esp_attr.h"
#include "unity.h"
#include "esp_heap_caps.h"
#include "esp_partition.h"
#include "esp_flash.h"

#include "esp_mmu_map.h"
#include "esp_rom_sys.h"

/**
 * This file contains simple hw tests for vaddr memory regions
 *
 * Traversing all vaddr memory regions to see if they have correct capabilities
 */

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

/**
 * Do following two tests:
 * - test all readable vaddr can map to flash
 * - test all executable vaddr can map to flash
 *
 * manually. Do a reset before a test start, as each of the tests
 * will map as much as possible, and don't do unmap.
 */

#define TEST_BLOCK_SIZE    CONFIG_MMU_PAGE_SIZE

typedef struct test_block_info_ {
    uint32_t vaddr;
    LIST_ENTRY(test_block_info_) entries;
} test_block_info_t;

static LIST_HEAD(test_block_list_head_, test_block_info_) test_block_head;

static void s_fill_random_data(uint8_t *buffer, size_t size, int random_seed)
{
    srand(random_seed);
    for (int i = 0 ; i < size; i++) {
        buffer[i] = rand() % 0xff;
    }
}

static bool s_test_mmap_data_by_random(uint8_t *mblock_ptr, size_t size, int random_seed, uint8_t *flash_ref_buf)
{
    srand(random_seed);
    uint8_t *test_ptr = mblock_ptr;

    for (int i = 0; i < size; i++) {
        uint8_t test_data = rand() % 0xff;
        if (test_data != test_ptr[i]) {
            printf("i: %d\n", i);
            printf("test_data: %d\n", test_data);
            printf("test_ptr[%d]: %d\n", i, test_ptr[i]);
            printf("flash_ref_buf[%d]: %d\n", i, flash_ref_buf[i]);
            ESP_EARLY_LOGE(TAG, "FAIL!!!!!!");
            return false;
        }
    }
    return true;
}

TEST_CASE("test all readable vaddr can map to flash", "[mmu]")
{
    uint8_t *sector_buf = heap_caps_calloc(1, TEST_BLOCK_SIZE, MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    TEST_ASSERT(sector_buf);

    //Get the partition used for SPI1 erase operation
    const esp_partition_t *part = s_get_partition();
    ESP_LOGI(TAG, "found partition '%s' at offset 0x%"PRIx32" with size 0x%"PRIx32, part->label, part->address, part->size);
    //Erase whole region
    TEST_ESP_OK(esp_flash_erase_region(part->flash_chip, part->address, part->size));

    ESP_LOGI(TAG, "TEST_BLOCK_SIZE: 0x%x", TEST_BLOCK_SIZE);

    int test_seed = 299;
    s_fill_random_data(sector_buf, sizeof(sector_buf), test_seed);
    ESP_LOGV(TAG, "rand seed: %d, write flash addr: %p...", test_seed, (void *)part->address);
    TEST_ESP_OK(esp_flash_write(part->flash_chip, sector_buf, part->address, sizeof(sector_buf)));

    esp_err_t ret = ESP_FAIL;
    int count = 0;
    LIST_INIT(&test_block_head);
    while (1) {
        test_block_info_t *block_info = heap_caps_calloc(1, sizeof(test_block_info_t), MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
        TEST_ASSERT(block_info && "no mem");

        void *ptr = NULL;
        //No need to use flag, we enabled ESP_MMAP_TEST_ALLOW_MAP_TO_MAPPED_PADDR in this test_app
        ret = esp_mmu_map(part->address, TEST_BLOCK_SIZE, MMU_TARGET_FLASH0, MMU_MEM_CAP_READ, 0, &ptr);
        if (ret == ESP_OK) {
            ESP_LOGI(TAG, "ptr is %p", ptr);
            bool success = s_test_mmap_data_by_random((uint8_t *)ptr, sizeof(sector_buf), test_seed, sector_buf);
            TEST_ASSERT(success);
        } else if (ret == ESP_ERR_NOT_FOUND) {
            free(block_info);
            break;
        } else {
            ESP_LOGI(TAG, "ret: 0x%x", ret);
            TEST_ASSERT(false);
        }

        block_info->vaddr = (uint32_t)ptr;
        LIST_INSERT_HEAD(&test_block_head, block_info, entries);
        count++;
    }

    ESP_LOGI(TAG, "no more free block, finish test, test block size: 0x%x, count: 0d%d", TEST_BLOCK_SIZE, count);

    test_block_info_t *block_to_free = LIST_FIRST(&test_block_head);
    test_block_info_t *temp = NULL;
    while (block_to_free) {
        temp = block_to_free;
        TEST_ESP_OK(esp_mmu_unmap((void *)block_to_free->vaddr));
        block_to_free = LIST_NEXT(block_to_free, entries);
        free(temp);
    }

    free(sector_buf);
}

TEST_CASE("test all executable vaddr can map to flash", "[mmu]")
{
    //Get the partition used for SPI1 erase operation
    const esp_partition_t *part = s_get_partition();
    ESP_LOGI(TAG, "found partition '%s' at offset 0x%"PRIx32" with size 0x%"PRIx32, part->label, part->address, part->size);
    //Erase whole region
    TEST_ESP_OK(esp_flash_erase_region(part->flash_chip, part->address, part->size));

    esp_err_t ret = ESP_FAIL;
    int count = 0;
    LIST_INIT(&test_block_head);
    while (1) {
        test_block_info_t *block_info = heap_caps_calloc(1, sizeof(test_block_info_t), MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
        TEST_ASSERT(block_info && "no mem");

        void *ptr = NULL;
        //No need to use flag, we enabled ESP_MMAP_TEST_ALLOW_MAP_TO_MAPPED_PADDR in this test_app
        ret = esp_mmu_map(part->address, TEST_BLOCK_SIZE, MMU_TARGET_FLASH0, MMU_MEM_CAP_EXEC, 0, &ptr);
        if (ret == ESP_OK) {
            ESP_LOGI(TAG, "ptr is %p", ptr);
            for (int i = 0; i < TEST_BLOCK_SIZE; i += 0x100) {
                uint32_t vaddr = (uint32_t)ptr + i;
                uint32_t paddr = 0;
                mmu_target_t mem_target = 0;
                TEST_ESP_OK(esp_mmu_vaddr_to_paddr((void *)vaddr, &paddr, &mem_target));
                TEST_ASSERT(paddr == part->address + i);
                ESP_LOGV(TAG, "paddr: %p, on %s", (void *)paddr, (mem_target) == MMU_TARGET_FLASH0 ? "Flash" : "PSRAM");
            }
        } else if (ret == ESP_ERR_NOT_FOUND) {
            free(block_info);
            break;
        } else {
            TEST_ASSERT(false);
        }

        block_info->vaddr = (uint32_t)ptr;
        LIST_INSERT_HEAD(&test_block_head, block_info, entries);
        count++;
    }

    ESP_LOGI(TAG, "no more free block, finish test, test block size: 0x%x, count: 0d%d", TEST_BLOCK_SIZE, count);

    test_block_info_t *block_to_free = LIST_FIRST(&test_block_head);
    test_block_info_t *temp = NULL;
    while (block_to_free) {
        temp = block_to_free;
        TEST_ESP_OK(esp_mmu_unmap((void *)block_to_free->vaddr));
        block_to_free = LIST_NEXT(block_to_free, entries);
        free(temp);
    }
}
