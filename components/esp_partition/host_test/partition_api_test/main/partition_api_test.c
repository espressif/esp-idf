/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Linux host partition API test
 */

#include <string.h>
#include "esp_err.h"
#include "esp_partition.h"
#include "esp_private/partition_linux.h"
#include "unity.h"
#include "unity_fixture.h"


TEST_GROUP(partition_api);

TEST_SETUP(partition_api)
{
}

TEST_TEAR_DOWN(partition_api)
{
}

TEST(partition_api, test_partition_find_basic)
{
    esp_partition_iterator_t iter = esp_partition_find(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_ANY, "storage");
    TEST_ASSERT_NOT_NULL(iter);

    const esp_partition_t *part = esp_partition_get(iter);
    TEST_ASSERT_NOT_NULL(part);

    esp_partition_iterator_release(iter);
}

TEST(partition_api, test_partition_find_app)
{
    esp_partition_iterator_t iter = esp_partition_find(ESP_PARTITION_TYPE_APP, ESP_PARTITION_SUBTYPE_ANY, NULL);
    TEST_ASSERT_NOT_NULL(iter);
    size_t counter = 0;

    while (iter != NULL) {
        const esp_partition_t *part_data = esp_partition_get(iter);
        counter++;
        TEST_ASSERT_NOT_NULL(part_data);
        iter = esp_partition_next(iter);
    }
    esp_partition_iterator_release(iter);
}

TEST(partition_api, test_partition_find_data)
{
    esp_partition_iterator_t iter = esp_partition_find(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_ANY, NULL);
    TEST_ASSERT_NOT_NULL(iter);
    size_t counter = 0;

    while (iter != NULL) {
        const esp_partition_t *part_data = esp_partition_get(iter);
        counter++;
        TEST_ASSERT_NOT_NULL(part_data);
        iter = esp_partition_next(iter);
    }
    esp_partition_iterator_release(iter);
}

TEST(partition_api, test_partition_find_first)
{
    const esp_partition_t *partition_app = esp_partition_find_first(ESP_PARTITION_TYPE_APP, ESP_PARTITION_SUBTYPE_ANY, NULL);
    TEST_ASSERT_NOT_NULL(partition_app);

    const esp_partition_t *partition_data = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_ANY, "storage");
    TEST_ASSERT_NOT_NULL(partition_data);
}

TEST(partition_api, test_partition_ops)
{
    const esp_partition_t *partition_data = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_ANY, "storage");
    TEST_ASSERT_NOT_NULL(partition_data);

    uint8_t buff[] = "ABCDEFGHIJKLMNOP";
    size_t bufsize = sizeof(buff);
    size_t off = 0x100;

    //8. esp_partition_write/raw
    esp_err_t err = esp_partition_write(partition_data, off, (const void *)buff, bufsize);
    TEST_ESP_OK(err);

    //9. esp_partition_read/raw
    uint8_t buffout[32] = {0};
    err = esp_partition_read(partition_data, off, (void *)buffout, bufsize);
    TEST_ESP_OK(err);

    //10. esp_partition_erase_range
    uint8_t buferase[bufsize];
    memset(buferase, 0xFF, bufsize);
    memset(buffout, 0, sizeof(buffout));
    size_t sector_off = 0; //erase works per whole sector - offset must be aligned to 4kB boundaries

    err = esp_partition_erase_range(partition_data, sector_off, partition_data->erase_size);
    assert(esp_partition_read(partition_data, off, (void *)buffout, bufsize) == ESP_OK);
    TEST_ESP_OK(err);
    TEST_ASSERT_EQUAL(0, memcmp(buffout, buferase, bufsize));

    //11. esp_partition_verify (partition_data)
    const esp_partition_t *verified_partition = esp_partition_verify(partition_data);
    TEST_ASSERT_NOT_NULL(verified_partition);
}

TEST_GROUP_RUNNER(partition_api)
{
    RUN_TEST_CASE(partition_api, test_partition_find_basic);
    RUN_TEST_CASE(partition_api, test_partition_find_app);
    RUN_TEST_CASE(partition_api, test_partition_find_data);
    RUN_TEST_CASE(partition_api, test_partition_find_first);
    RUN_TEST_CASE(partition_api, test_partition_ops);
}

static void run_all_tests(void)
{
    RUN_TEST_GROUP(partition_api);
}

int main(int argc, char **argv)
{
    UNITY_MAIN_FUNC(run_all_tests);
    return 0;
}
