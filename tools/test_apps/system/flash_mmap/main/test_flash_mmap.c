/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <string.h>
#include <sys/queue.h>
#include "sdkconfig.h"
#include "unity.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_check.h"
#include "esp_attr.h"
#include "esp_flash.h"
#include "esp_partition.h"
#include "spi_flash_mmap.h"

const static char *TAG = "MMAP_TEST";

#define TEST_BLOCK_SIZE    CONFIG_MMU_PAGE_SIZE

typedef struct test_block_info_ {
    uint32_t vaddr;
    spi_flash_mmap_handle_t handle;
    LIST_ENTRY(test_block_info_) entries;
} test_block_info_t;

static LIST_HEAD(test_block_list_head_, test_block_info_) test_block_head;
static DRAM_ATTR uint8_t sector_buf[TEST_BLOCK_SIZE];

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


static void s_fill_random_data(uint8_t *buffer, size_t size, int random_seed)
{
    srand(random_seed);
    for (int i = 0 ; i < size; i++) {
        buffer[i] = rand() % 0xff;
    }
}

static bool s_test_mmap_data_by_random(uint8_t *mblock_ptr, size_t size, int random_seed)
{
    srand(random_seed);
    uint8_t *test_ptr = mblock_ptr;

    for (int i = 0; i < size; i++) {
        uint8_t test_data = rand() % 0xff;
        if(test_data != test_ptr[i]) {
            printf("i: %d\n", i);
            printf("test_data: %d\n", test_data);
            printf("test_ptr[%d]: %d\n", i, test_ptr[i]);
            printf("sector_buf[%d]: %d\n", i, sector_buf[i]);
            ESP_EARLY_LOGE(TAG, "FAIL!!!!!!");
            return false;
        }
    }
    return true;
}

static void s_print_free_pages(void)
{
    uint32_t free_i_pages = spi_flash_mmap_get_free_pages(SPI_FLASH_MMAP_INST);
    uint32_t free_d_pages = spi_flash_mmap_get_free_pages(SPI_FLASH_MMAP_DATA);
    printf("free_i_pages: 0d%"PRId32"\n", free_i_pages);
    printf("free_d_pages: 0d%"PRId32"\n", free_d_pages);
}

void app_main(void)
{
    //Get the partition used for SPI1 erase operation
    const esp_partition_t *part = s_get_partition();
    ESP_LOGI(TAG, "found partition '%s' at offset 0x%"PRIx32" with size 0x%"PRIx32, part->label, part->address, part->size);

    //Erase whole region
    TEST_ESP_OK(esp_flash_erase_region(part->flash_chip, part->address, part->size));

    ESP_LOGI(TAG, "TEST_BLOCK_SIZE: 0x%x", TEST_BLOCK_SIZE);
    s_print_free_pages();

    uint32_t offset = 0;
    int test_seed = 299;
    while (1) {
        s_fill_random_data(sector_buf, sizeof(sector_buf), test_seed);
        ESP_LOGI(TAG, "rand seed: %d, write flash addr: %p...", test_seed, (void *)(part->address + offset));
        TEST_ESP_OK(esp_flash_write(part->flash_chip, sector_buf, (part->address + offset), sizeof(sector_buf)));

        test_seed++;
        offset += TEST_BLOCK_SIZE;

        if (offset == part->size) {
            break;
        }
    }

    esp_err_t ret = ESP_FAIL;
    int count = 0;
    LIST_INIT(&test_block_head);

    offset = 0;
    test_seed = 299;

    while (1) {
        test_block_info_t *block_info = heap_caps_calloc(1, sizeof(test_block_info_t), MALLOC_CAP_INTERNAL|MALLOC_CAP_8BIT);
        TEST_ASSERT(block_info && "no mem");

        spi_flash_mmap_handle_t handle;
        const void *ptr = NULL;
        ret = spi_flash_mmap(part->address + offset, TEST_BLOCK_SIZE, SPI_FLASH_MMAP_DATA, &ptr, &handle);
        if (ret == ESP_OK) {
            ESP_LOGI(TAG, "ptr is %p", ptr);

            s_fill_random_data(sector_buf, sizeof(sector_buf), test_seed);
            bool success = s_test_mmap_data_by_random((uint8_t *)ptr, sizeof(sector_buf), test_seed);
            TEST_ASSERT(success);
        } else if (ret == ESP_ERR_NOT_FOUND) {
            free(block_info);
            break;
        } else {
            ESP_LOGE(TAG, "ret: 0x%x", ret);
            TEST_ASSERT(false);
        }

        block_info->vaddr = (uint32_t)ptr;
        block_info->handle = handle;
        LIST_INSERT_HEAD(&test_block_head, block_info, entries);
        count++;
        test_seed++;
        offset += TEST_BLOCK_SIZE;

        if (offset == part->size) {
            break;
        }
    }

    ESP_LOGI(TAG, "no more free block / free flash size, finish test, test block size: 0x%x, count: 0d%d", TEST_BLOCK_SIZE, count);
    s_print_free_pages();

    test_block_info_t *block_to_free = LIST_FIRST(&test_block_head);
    test_block_info_t *temp = NULL;
    while (block_to_free) {
        temp = block_to_free;
        spi_flash_munmap(block_to_free->handle);
        block_to_free = LIST_NEXT(block_to_free, entries);
        free(temp);
    }
    s_print_free_pages();

    printf("flash mmap test success\n");
}
