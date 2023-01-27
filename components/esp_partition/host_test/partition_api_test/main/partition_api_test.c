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

#include "esp_log.h"
const char *TAG = "partition_api_test";


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

TEST(partition_api, test_partition_mmap)
{
    const esp_partition_t *partition_data = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_ANY, "storage");
    TEST_ASSERT_NOT_NULL(partition_data);

    esp_partition_mmap_memory_t memory = ESP_PARTITION_MMAP_DATA;
    void *out_ptr = NULL;
    esp_partition_mmap_handle_t out_handle = 0;

    // no offset, complete length
    size_t offset = 0;
    size_t size = partition_data->size;

    esp_err_t err = esp_partition_mmap(partition_data, offset, size, memory, (const void **) &out_ptr, &out_handle);
    TEST_ESP_OK(err);
    TEST_ASSERT_NOT_NULL(out_ptr);
    esp_partition_munmap(out_handle);

    // offset out of partition size
    offset = partition_data->size+1;
    size = 1;

    err = esp_partition_mmap(partition_data, offset, size, memory, (const void **) &out_ptr, &out_handle);
    TEST_ASSERT_EQUAL(err,ESP_ERR_INVALID_ARG);

    // mapped length beyond partition size
    offset = 1;
    size = partition_data->size;

    err = esp_partition_mmap(partition_data, offset, size, memory, (const void **) &out_ptr, &out_handle);
    TEST_ASSERT_EQUAL(err,ESP_ERR_INVALID_SIZE);
}

#define EMULATED_VIRTUAL_SECTOR_COUNT (ESP_PARTITION_EMULATED_FLASH_SIZE / ESP_PARTITION_EMULATED_SECTOR_SIZE)

typedef struct
{
    size_t read_ops;
    size_t write_ops;
    size_t erase_ops;
    size_t read_bytes;
    size_t write_bytes;
    size_t total_time;
    size_t sector_erase_count[EMULATED_VIRTUAL_SECTOR_COUNT];
} t_stats;

void print_stats(const t_stats *p_stats)
{
    ESP_LOGI(TAG, "read_ops:%06lu write_ops:%06lu erase_ops:%06lu read_bytes:%06lu write_bytes:%06lu total_time:%06lu\n",
        p_stats->read_ops,
        p_stats->write_ops,
        p_stats->erase_ops,
        p_stats->read_bytes,
        p_stats->write_bytes,
        p_stats->total_time);
}

void read_stats(t_stats *p_stats)
{
    p_stats->read_ops = esp_partition_get_read_ops();
    p_stats->write_ops = esp_partition_get_write_ops();
    p_stats->erase_ops = esp_partition_get_erase_ops();
    p_stats->read_bytes = esp_partition_get_read_bytes();
    p_stats->write_bytes = esp_partition_get_write_bytes();
    p_stats->total_time = esp_partition_get_total_time();

    for(size_t i = 0; i < EMULATED_VIRTUAL_SECTOR_COUNT; i++)
        p_stats->sector_erase_count[i] = esp_partition_get_sector_erase_count(i);
}

// evaluates if final stats differ from initial stats by expected difference stats.
// if there is no need to evaluate some stats, set respective expeted difference stats members to SIZE_MAX
bool evaluate_stats(const t_stats *p_initial_stats, const t_stats *p_final_stats, const t_stats *p_expected_difference_stats)
{
    if(p_expected_difference_stats->read_ops != SIZE_MAX)
        TEST_ASSERT_EQUAL(p_initial_stats->read_ops + p_expected_difference_stats->read_ops, p_final_stats->read_ops);
    if(p_expected_difference_stats->write_ops != SIZE_MAX)
        TEST_ASSERT_EQUAL(p_initial_stats->write_ops + p_expected_difference_stats->write_ops, p_final_stats->write_ops);
    if(p_expected_difference_stats->erase_ops != SIZE_MAX)
        TEST_ASSERT_EQUAL(p_initial_stats->erase_ops + p_expected_difference_stats->erase_ops, p_final_stats->erase_ops);
    if(p_expected_difference_stats->read_bytes != SIZE_MAX)
        TEST_ASSERT_EQUAL(p_initial_stats->read_bytes + p_expected_difference_stats->read_bytes, p_final_stats->read_bytes);
    if(p_expected_difference_stats->write_bytes != SIZE_MAX)
        TEST_ASSERT_EQUAL(p_initial_stats->write_bytes + p_expected_difference_stats->write_bytes, p_final_stats->write_bytes);
    if(p_expected_difference_stats->total_time != SIZE_MAX)
        TEST_ASSERT_EQUAL(p_initial_stats->total_time + p_expected_difference_stats->total_time, p_final_stats->total_time);

    for(size_t i = 0; i < EMULATED_VIRTUAL_SECTOR_COUNT; i++)
    {
        if(p_expected_difference_stats->sector_erase_count[i] != SIZE_MAX)
        {
            size_t expected_value = p_initial_stats->sector_erase_count[i] + p_expected_difference_stats->sector_erase_count[i];
            size_t final_value = p_final_stats->sector_erase_count[i];

            TEST_ASSERT_EQUAL(expected_value, final_value);
        }
    }

    return true;
}

TEST(partition_api, test_partition_stats)
{
    // get storage partition
    const esp_partition_t *partition_data = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_ANY, "storage");
    TEST_ASSERT_NOT_NULL(partition_data);

    t_stats initial_stats;
    t_stats final_stats;
    t_stats zero_stats = {0};

    // get actual statistics
    read_stats(&initial_stats);

    // prepare buffer for r/w
    size_t size = partition_data->size;
    size_t part_offset = partition_data->address; // this is offset of partition data from flash beginning
    void *test_data_ptr = malloc(size);
    TEST_ASSERT_NOT_NULL(test_data_ptr);

    // do some writes
    memset(test_data_ptr, 0xff, size);
    esp_err_t err = esp_partition_write(partition_data, 0, test_data_ptr, size);
    TEST_ESP_OK(err);

    // do some reads
    err = esp_partition_read(partition_data , 0, test_data_ptr, size);
    TEST_ESP_OK(err);

    // do erase
    err = esp_partition_erase_range(partition_data, 0, size);
    TEST_ESP_OK(err);

    // get actual statistics
    read_stats(&final_stats);

    // evaluate expected results
    // erase operations are per virtual sectors touched
    // erase ops size / sector + (part_offset % sector + size % sector) / sector + 1 if ((part_offset % sector + size % sector) % sector > 0)
    size_t non_aligned_portions = (part_offset % ESP_PARTITION_EMULATED_SECTOR_SIZE) + (size % ESP_PARTITION_EMULATED_SECTOR_SIZE);
    size_t erase_ops = size / ESP_PARTITION_EMULATED_SECTOR_SIZE;
    erase_ops += non_aligned_portions / ESP_PARTITION_EMULATED_SECTOR_SIZE;
    if((non_aligned_portions % ESP_PARTITION_EMULATED_SECTOR_SIZE) > 0)
        erase_ops += 1;

    t_stats expected_difference_stats = {
        .read_ops = 1,
        .write_ops = 1,
        .erase_ops = erase_ops,
        .read_bytes = size,
        .write_bytes = size,
        .total_time = SIZE_MAX
    };
    for (size_t i = 0; i < EMULATED_VIRTUAL_SECTOR_COUNT; i++)
        expected_difference_stats.sector_erase_count[i] = SIZE_MAX;

    evaluate_stats(&initial_stats, &final_stats, &expected_difference_stats);

    // clear statistics
    esp_partition_clear_stats();
    read_stats(&final_stats);

    // evaluate zero statistics
    evaluate_stats(&zero_stats, &final_stats, &zero_stats);

    free(test_data_ptr);
}

TEST(partition_api, test_partition_wear_emulation)
{
    const esp_partition_t *partition_data = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_ANY, "storage");
    TEST_ASSERT_NOT_NULL(partition_data);

    // no offset, map whole partition
    size_t offset = 0;
    size_t size = partition_data->size;

    // prepare test data block
    void *test_data_ptr = malloc(size);
    TEST_ASSERT_NOT_NULL(test_data_ptr);
    memset(test_data_ptr, 0xff, size);

    // --- wear off ---
    // ensure wear emulation is off
    esp_partition_fail_after(SIZE_MAX);

    // erase partition data
    esp_err_t err = esp_partition_erase_range(partition_data, offset, size);
    TEST_ESP_OK(err);

    // write data - should pass
    err = esp_partition_write(partition_data, offset, test_data_ptr, size);
    TEST_ESP_OK(err);

    // erase partition data
    err = esp_partition_erase_range(partition_data, offset, size);
    TEST_ESP_OK(err);

    // --- wear on, write ---
    // ensure wear emulation is on, below the limit for size
    // esp_partition_write consumes one wear cycle per 4 bytes written
    esp_partition_fail_after(size / 4 - 1);

    // write data - should fail
    err = esp_partition_write(partition_data, offset, test_data_ptr, size);
    TEST_ASSERT_EQUAL(ESP_FAIL, err);

    // --- wear on, erase has just enough wear cycles available---
    // ensure wear emulation is on, at the limit for size
    // esp_partition_erase_range consumes one wear cycle per one virtual sector erased
    esp_partition_fail_after(size / ESP_PARTITION_EMULATED_SECTOR_SIZE);

    // write data - should be ok
    err = esp_partition_erase_range(partition_data, offset, size);
    TEST_ASSERT_EQUAL(ESP_OK, err);

    // --- wear on, erase has one cycle less than required---
    // ensure wear emulation is on, below the limit for size
    // esp_partition_erase_range consumes one wear cycle per one virtual sector erased
    esp_partition_fail_after(size / ESP_PARTITION_EMULATED_SECTOR_SIZE - 1);

    // write data - should fail
    err = esp_partition_erase_range(partition_data, offset, size);
    TEST_ASSERT_EQUAL(ESP_FAIL, err);

    // ---cleanup ---
    // disable wear emulation
    esp_partition_fail_after(SIZE_MAX);
    free(test_data_ptr);
}


TEST_GROUP_RUNNER(partition_api)
{
    RUN_TEST_CASE(partition_api, test_partition_find_basic);
    RUN_TEST_CASE(partition_api, test_partition_find_app);
    RUN_TEST_CASE(partition_api, test_partition_find_data);
    RUN_TEST_CASE(partition_api, test_partition_find_first);
    RUN_TEST_CASE(partition_api, test_partition_ops);
    RUN_TEST_CASE(partition_api, test_partition_mmap);
    RUN_TEST_CASE(partition_api, test_partition_stats);
    RUN_TEST_CASE(partition_api, test_partition_wear_emulation);
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
