/*
 * SPDX-FileCopyrightText: 2016-2025 Espressif Systems (Shanghai) CO LTD
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

TEST_CASE("Test esp_partition_find_err API", "[partition]")
{
    // Test successful partition finding
    esp_partition_iterator_t iter = NULL;
    esp_err_t err = esp_partition_find_err(ESP_PARTITION_TYPE_APP, ESP_PARTITION_SUBTYPE_ANY, NULL, &iter);
    TEST_ASSERT_EQUAL(ESP_OK, err);
    TEST_ASSERT_NOT_NULL(iter);

    const esp_partition_t *part = esp_partition_get(iter);
    TEST_ASSERT_NOT_NULL(part);
    TEST_ASSERT_EQUAL(ESP_PARTITION_TYPE_APP, part->type);

    esp_partition_iterator_release(iter);

    // Test partition not found (returns ESP_ERR_NOT_FOUND)
    err = esp_partition_find_err(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_ANY, "nonexistent_partition", &iter);
    TEST_ASSERT_EQUAL(ESP_ERR_NOT_FOUND, err);
    TEST_ASSERT_NULL(iter);

    // Test invalid argument - wrong type/subtype combination
    err = esp_partition_find_err(ESP_PARTITION_TYPE_ANY, ESP_PARTITION_SUBTYPE_APP_FACTORY, NULL, &iter);
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, err);
    TEST_ASSERT_NULL(iter);

    // Test NULL pointer argument
    err = esp_partition_find_err(ESP_PARTITION_TYPE_APP, ESP_PARTITION_SUBTYPE_ANY, NULL, NULL);
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, err);
    TEST_ASSERT_NULL(iter);

    // Test finding data partitions with error reporting
    iter = NULL;
    err = esp_partition_find_err(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_ANY, NULL, &iter);
    TEST_ASSERT_EQUAL(ESP_OK, err);
    TEST_ASSERT_NOT_NULL(iter);

    int data_count = 0;
    for (; iter != NULL; iter = esp_partition_next(iter)) {
        const esp_partition_t *data_part = esp_partition_get(iter);
        TEST_ASSERT_NOT_NULL(data_part);
        TEST_ASSERT_EQUAL(ESP_PARTITION_TYPE_DATA, data_part->type);
        data_count++;
    }
    esp_partition_iterator_release(iter);
    TEST_ASSERT_TRUE(data_count > 0);
}

TEST_CASE("Test esp_partition_find_first_err API", "[partition]")
{
    // Test successful partition finding
    const esp_partition_t *part = NULL;
    esp_err_t err = esp_partition_find_first_err(ESP_PARTITION_TYPE_APP, ESP_PARTITION_SUBTYPE_ANY, NULL, &part);
    TEST_ASSERT_EQUAL(ESP_OK, err);
    TEST_ASSERT_NOT_NULL(part);
    TEST_ASSERT_EQUAL(ESP_PARTITION_TYPE_APP, part->type);
    TEST_ASSERT_EQUAL(0x20000, part->address);
    TEST_ASSERT_EQUAL(ESP_PARTITION_SUBTYPE_APP_FACTORY, part->subtype);

    // Test finding specific factory partition
    part = NULL;
    err = esp_partition_find_first_err(ESP_PARTITION_TYPE_APP, ESP_PARTITION_SUBTYPE_APP_FACTORY, NULL, &part);
    TEST_ASSERT_EQUAL(ESP_OK, err);
    TEST_ASSERT_NOT_NULL(part);
    TEST_ASSERT_EQUAL(ESP_PARTITION_SUBTYPE_APP_FACTORY, part->subtype);

    // Test partition not found (returns ESP_ERR_NOT_FOUND)
    err = esp_partition_find_first_err(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_ANY, "nonexistent_partition", &part);
    TEST_ASSERT_EQUAL(ESP_ERR_NOT_FOUND, err);
    TEST_ASSERT_NULL(part);

    // Test invalid argument - wrong type/subtype combination
    err = esp_partition_find_first_err(ESP_PARTITION_TYPE_ANY, ESP_PARTITION_SUBTYPE_APP_FACTORY, NULL, &part);
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, err);
    TEST_ASSERT_NULL(part);

    // Test NULL pointer argument
    err = esp_partition_find_first_err(ESP_PARTITION_TYPE_APP, ESP_PARTITION_SUBTYPE_ANY, NULL, NULL);
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, err);

    // Test finding data partitions
    part = NULL;
    err = esp_partition_find_first_err(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_ANY, NULL, &part);
    TEST_ASSERT_EQUAL(ESP_OK, err);
    TEST_ASSERT_NOT_NULL(part);
    TEST_ASSERT_EQUAL(ESP_PARTITION_TYPE_DATA, part->type);

    // Compare with legacy API to ensure consistency
    const esp_partition_t *legacy_part = esp_partition_find_first(ESP_PARTITION_TYPE_APP, ESP_PARTITION_SUBTYPE_ANY, NULL);
    part = NULL;
    err = esp_partition_find_first_err(ESP_PARTITION_TYPE_APP, ESP_PARTITION_SUBTYPE_ANY, NULL, &part);
    TEST_ASSERT_EQUAL(ESP_OK, err);
    TEST_ASSERT_EQUAL(legacy_part, part);  // Should return the same partition
}

TEST_CASE("Test esp_partition_verify_err API", "[partition]")
{
    // Test successful partition verification
    const esp_partition_t *app_part = esp_partition_find_first(ESP_PARTITION_TYPE_APP, ESP_PARTITION_SUBTYPE_ANY, NULL);
    TEST_ASSERT_NOT_NULL(app_part);

    const esp_partition_t *verified_part = NULL;
    esp_err_t err = esp_partition_verify_err(app_part, &verified_part);
    TEST_ASSERT_EQUAL(ESP_OK, err);
    TEST_ASSERT_NOT_NULL(verified_part);
    TEST_ASSERT_EQUAL(app_part, verified_part);  // Should return the same partition pointer

    // Test both parameters NULL
    err = esp_partition_verify_err(NULL, NULL);
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, err);

    // Test with wrong address (should not match)
    esp_partition_t app_copy = *app_part;
    app_copy.address = 0xFFFFFFFF;
    err = esp_partition_verify_err(&app_copy, &verified_part);
    TEST_ASSERT_EQUAL(ESP_ERR_NOT_FOUND, err);
    TEST_ASSERT_NULL(verified_part);

    // Test with wrong type (should not match)
    app_copy = *app_part;
    app_copy.type = ESP_PARTITION_TYPE_DATA;
    err = esp_partition_verify_err(&app_copy, &verified_part);
    TEST_ASSERT_EQUAL(ESP_ERR_NOT_FOUND, err);
    TEST_ASSERT_NULL(verified_part);

    // Test with different readonly flag (should not match)
    app_copy = *app_part;
    app_copy.readonly = !app_part->readonly;
    err = esp_partition_verify_err(&app_copy, &verified_part);
    TEST_ASSERT_EQUAL(ESP_ERR_NOT_FOUND, err);
    TEST_ASSERT_NULL(verified_part);

    // Test output parameter is properly initialized
    verified_part = (const esp_partition_t *)0xDEADBEEF;
    err = esp_partition_verify_err(&app_copy, &verified_part);
    TEST_ASSERT_EQUAL(ESP_ERR_NOT_FOUND, err);
    TEST_ASSERT_NULL(verified_part);  // Should be set to NULL

    // Compare with legacy API for consistency
    const esp_partition_t *legacy_verified = esp_partition_verify(app_part);
    verified_part = NULL;
    err = esp_partition_verify_err(app_part, &verified_part);
    TEST_ASSERT_EQUAL(ESP_OK, err);
    TEST_ASSERT_EQUAL(legacy_verified, verified_part);
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
