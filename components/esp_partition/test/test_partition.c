/*
 * SPDX-FileCopyrightText: 2016-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdio.h>
#include <stdlib.h>
#include "unity.h"
#include "test_utils.h"
#include "esp_partition.h"


TEST_CASE("Can read partition table", "[partition]")
{

    const esp_partition_t *p = esp_partition_find_first(ESP_PARTITION_TYPE_APP, ESP_PARTITION_SUBTYPE_ANY, NULL);
    TEST_ASSERT_NOT_NULL(p);
    TEST_ASSERT_EQUAL(0x20000, p->address);
    TEST_ASSERT_EQUAL(ESP_PARTITION_SUBTYPE_APP_FACTORY, p->subtype);

    esp_partition_iterator_t it = esp_partition_find(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_ANY, NULL);
    TEST_ASSERT_NOT_NULL(it);
    int count = 0;
    const esp_partition_t* prev = NULL;
    for (; it != NULL; it = esp_partition_next(it)) {
        const esp_partition_t *p = esp_partition_get(it);
        TEST_ASSERT_NOT_NULL(p);
        if (prev) {
            TEST_ASSERT_TRUE_MESSAGE(prev->address < p->address, "incorrect partition order");
        }
        prev = p;
        ++count;
    }
    esp_partition_iterator_release(it);
    TEST_ASSERT_EQUAL(5, count);

    it = esp_partition_find(ESP_PARTITION_TYPE_ANY, ESP_PARTITION_SUBTYPE_ANY, NULL);
    TEST_ASSERT_NOT_NULL(it);
    count = 0;
    for (; it != NULL; it = esp_partition_next(it)) {
        ++count;
    }
    esp_partition_iterator_release(it);
    TEST_ASSERT_EQUAL(8, count);
}

TEST_CASE("Can write, read, mmap partition", "[partition][ignore]")
{
    const esp_partition_t *p = get_test_data_partition();
    printf("Using partition %s at 0x%x, size 0x%x\n", p->label, p->address, p->size);
    TEST_ASSERT_NOT_NULL(p);
    const size_t max_size = 2 * p->erase_size;
    uint8_t *data = (uint8_t *) malloc(max_size);
    TEST_ASSERT_NOT_NULL(data);

    TEST_ASSERT_EQUAL(ESP_OK, esp_partition_erase_range(p, 0, p->size));

    srand(0);
    size_t block_size;
    for (size_t offset = 0; offset < p->size; offset += block_size) {
        block_size = ((rand() + 4) % max_size) & (~0x3);
        size_t left = p->size - offset;
        if (block_size > left) {
            block_size = left;
        }
        for (size_t i = 0; i < block_size / 4; ++i) {
            ((uint32_t *) (data))[i] = rand();
        }
        TEST_ASSERT_EQUAL(ESP_OK, esp_partition_write(p, offset, data, block_size));
    }

    srand(0);
    for (size_t offset = 0; offset < p->size; offset += block_size) {
        block_size = ((rand() + 4) % max_size) & (~0x3);
        size_t left = p->size - offset;
        if (block_size > left) {
            block_size = left;
        }
        TEST_ASSERT_EQUAL(ESP_OK, esp_partition_read(p, offset, data, block_size));
        for (size_t i = 0; i < block_size / 4; ++i) {
            TEST_ASSERT_EQUAL(rand(), ((uint32_t *) data)[i]);
        }
    }

    free(data);

    const uint32_t *mmap_data;
    esp_partition_mmap_handle_t mmap_handle;
    size_t begin = 3000;
    size_t size = 64000; //chosen so size is smaller than 64K but the mmap straddles 2 MMU blocks
    TEST_ASSERT_EQUAL(ESP_OK, esp_partition_mmap(p, begin, size, ESP_PARTITION_MMAP_DATA,
                      (const void **)&mmap_data, &mmap_handle));
    srand(0);
    for (size_t offset = 0; offset < p->size; offset += block_size) {
        block_size = ((rand() + 4) % max_size) & (~0x3);
        size_t left = p->size - offset;
        if (block_size > left) {
            block_size = left;
        }
        for (size_t i = 0; i < block_size / 4; ++i) {
            size_t pos = offset + i * 4;
            uint32_t expected = rand();
            if (pos < begin || pos >= (begin + size)) {
                continue;
            }
            TEST_ASSERT_EQUAL(expected, mmap_data[(pos - begin) / 4]);
        }
    }

    esp_partition_munmap(mmap_handle);
}
