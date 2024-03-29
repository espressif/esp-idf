/*
 * SPDX-FileCopyrightText: 2021-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Linux host partition API test
 */

#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include "esp_err.h"
#include "esp_partition.h"
#include "esp_private/partition_linux.h"
#include "unity.h"
#include "unity_fixture.h"
#include "esp_log.h"
#include "spi_flash_mmap.h"

const char *TAG = "partition_api_test";

/* generate timestamp-based filename in /tmp dir */
static void partition_test_get_unique_filename(char *filename, size_t len)
{
    struct timeval  tv;
    gettimeofday(&tv, NULL);
    long long int nanotimestamp = tv.tv_sec * 1000000000 + tv.tv_usec;
    snprintf(filename, len, "/tmp/espparttest%lld", nanotimestamp);
}

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

    // esp_partition_write/raw
    esp_err_t err = esp_partition_write(partition_data, off, (const void *)buff, bufsize);
    TEST_ESP_OK(err);

    // esp_partition_read/raw
    uint8_t buffout[32] = {0};
    err = esp_partition_read(partition_data, off, (void *)buffout, bufsize);
    TEST_ESP_OK(err);

    // esp_partition_erase_range
    uint8_t buferase[bufsize];
    memset(buferase, 0xFF, bufsize);
    memset(buffout, 0, sizeof(buffout));
    size_t sector_off = 0; //erase works per whole sector - offset must be aligned to 4kB boundaries

    err = esp_partition_erase_range(partition_data, sector_off, partition_data->erase_size);
    assert(esp_partition_read(partition_data, off, (void *)buffout, bufsize) == ESP_OK);
    TEST_ESP_OK(err);
    TEST_ASSERT_EQUAL(0, memcmp(buffout, buferase, bufsize));

    // esp_partition_verify (partition_data)
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
    offset = partition_data->size + 1;
    size = 1;

    err = esp_partition_mmap(partition_data, offset, size, memory, (const void **) &out_ptr, &out_handle);
    TEST_ASSERT_EQUAL(err, ESP_ERR_INVALID_ARG);

    // mapped length beyond partition size
    offset = 1;
    size = partition_data->size;

    err = esp_partition_mmap(partition_data, offset, size, memory, (const void **) &out_ptr, &out_handle);
    TEST_ASSERT_EQUAL(err, ESP_ERR_INVALID_SIZE);
}

TEST(partition_api, test_partition_mmap_diff_size)
{
    // Scenario: default temporary flash file, explicitly specified size and file with partition table
    // Check the size of "storage" partition. Should be 6M = 6*1024*1024

    // unmap file to have correct initial conditions, regardless of result
    esp_partition_file_munmap();

    // get and initialize the control structure for file mmap
    esp_partition_file_mmap_ctrl_t *p_file_mmap_ctrl = esp_partition_get_file_mmap_ctrl_input();
    TEST_ASSERT_NOT_NULL(p_file_mmap_ctrl);

    memset(p_file_mmap_ctrl, 0, sizeof(*p_file_mmap_ctrl));
    p_file_mmap_ctrl->flash_file_size = 0x800000;   // 8MB
    strlcpy(p_file_mmap_ctrl->partition_file_name, BUILD_DIR "/partition_table/partition-table_8M.bin", sizeof(p_file_mmap_ctrl->partition_file_name));

    // esp_partition_find_first calls the esp_partition_file_mmap in the background
    const esp_partition_t *partition_data = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_ANY, "storage");
    TEST_ASSERT_NOT_NULL(partition_data);

    // Check partition size
    size_t exp_size = 0x600000;  // 6MB
    size_t act_size = partition_data->size;
    TEST_ASSERT_EQUAL(exp_size, act_size);

    // cleanup after test
    esp_partition_file_munmap();
}

TEST(partition_api, test_partition_mmap_reopen)
{
    // Scenario: default temporary flash file, write some data
    // Remember name of temporary file, reset remove flag, unmmap file.
    // Set file name from previous step, mmap file, read and compare data

    // unmap file to have correct initial conditions, regardless of result
    esp_partition_file_munmap();

    // get and initialize the control structure for file mmap
    esp_partition_file_mmap_ctrl_t *p_file_mmap_ctrl_input = esp_partition_get_file_mmap_ctrl_input();
    TEST_ASSERT_NOT_NULL(p_file_mmap_ctrl_input);

    memset(p_file_mmap_ctrl_input, 0, sizeof(*p_file_mmap_ctrl_input));

    // esp_partition_find_first calls the esp_partition_file_mmap in the background
    const esp_partition_t *partition_data = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_ANY, "storage");
    TEST_ASSERT_NOT_NULL(partition_data);

    const char *test_string = "Is HAL6000 an IBM6000 ?";
    size_t test_string_len = strlen(test_string) + 1;

    // write test string
    esp_err_t err = esp_partition_write(partition_data, 0, test_string, test_string_len);
    TEST_ESP_OK(err);

    // remember memory mapped file name
    esp_partition_file_mmap_ctrl_t *p_file_mmap_ctrl_act = esp_partition_get_file_mmap_ctrl_act();
    TEST_ASSERT_NOT_NULL(p_file_mmap_ctrl_act);

    char generated_file_name[PATH_MAX];
    strlcpy(generated_file_name, p_file_mmap_ctrl_act->flash_file_name, sizeof(generated_file_name));

    // ensure remove flag is not set
    p_file_mmap_ctrl_input->remove_dump = false;

    // unmap
    err = esp_partition_file_munmap();
    TEST_ESP_OK(err);

    // initialize control struct with memory mapped file name
    memset(p_file_mmap_ctrl_input, 0, sizeof(*p_file_mmap_ctrl_input));
    strlcpy(p_file_mmap_ctrl_input->flash_file_name, generated_file_name, sizeof(p_file_mmap_ctrl_input->flash_file_name));

    // get partition
    partition_data = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_ANY, "storage");
    TEST_ASSERT_NOT_NULL(partition_data);

    // read verify string
    char *verify_string = malloc(test_string_len);
    err = esp_partition_read(partition_data, 0, verify_string, test_string_len);
    TEST_ESP_OK(err);

    // compare strings
    bool strings_equal = (strncmp(test_string, verify_string, test_string_len) == 0);
    TEST_ASSERT_EQUAL(strings_equal, true);

    free(verify_string);

    // cleanup after test
    esp_partition_file_munmap();
}

/* Positive TC to prove temporary file removal after file unmap.
 * Error is reported during subsequent attempt to map already removed file.
 * This error proves that file was removed after unmap as requested.
 */
TEST(partition_api, test_partition_mmap_remove)
{
    // Scenario: default temporary flash file, write some data
    // Remember name of temporary file, set remove flag, unmmap file.
    // Set file name from previous step, try to get partition "storage", should fail with NULL returned

    // unmap file to have correct initial conditions, regardless of result
    esp_partition_file_munmap();

    // get and initialize the control structure for file mmap
    esp_partition_file_mmap_ctrl_t *p_file_mmap_ctrl_input = esp_partition_get_file_mmap_ctrl_input();
    TEST_ASSERT_NOT_NULL(p_file_mmap_ctrl_input);

    memset(p_file_mmap_ctrl_input, 0, sizeof(*p_file_mmap_ctrl_input));

    // esp_partition_find_first calls the esp_partition_file_mmap in the background
    const esp_partition_t *partition_data = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_ANY, "storage");
    TEST_ASSERT_NOT_NULL(partition_data);

    const char *test_string = "This text should dismiss after esp_partition_file_munmap";
    size_t test_string_len = strlen(test_string) + 1;

    // write test string
    esp_err_t err = esp_partition_write(partition_data, 0, test_string, test_string_len);
    TEST_ESP_OK(err);

    // remember memory mapped file name
    esp_partition_file_mmap_ctrl_t *p_file_mmap_ctrl_act = esp_partition_get_file_mmap_ctrl_act();
    TEST_ASSERT_NOT_NULL(p_file_mmap_ctrl_act);

    char generated_file_name[PATH_MAX];
    strlcpy(generated_file_name, p_file_mmap_ctrl_act->flash_file_name, sizeof(generated_file_name));

    // ensure remove flag is set
    p_file_mmap_ctrl_input->remove_dump = true;

    // unmap
    err = esp_partition_file_munmap();
    TEST_ESP_OK(err);

    // initialize control struct with memory mapped file name
    memset(p_file_mmap_ctrl_input, 0, sizeof(*p_file_mmap_ctrl_input));
    strlcpy(p_file_mmap_ctrl_input->flash_file_name, generated_file_name, sizeof(p_file_mmap_ctrl_input->flash_file_name));

    // get partition, should fail with NULL returned
    partition_data = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_ANY, "storage");
    TEST_ASSERT_EQUAL(NULL, partition_data);

    // cleanup after test
    esp_partition_file_munmap();
    memset(p_file_mmap_ctrl_input, 0, sizeof(*p_file_mmap_ctrl_input));
}

/* Negative TC to ensure mmap setup is consistent prior call to mmap.
 * Configuration specifies both flash file name to be mapped and its size.
 * This is invalid combination as size is determined by the file itself.
 */
TEST(partition_api, test_partition_mmap_name_size)
{
    // Negative Scenario: conflicting settings - flash_file_name together with one or both of flash_file_size, partition_file_name
    // esp_partition_file_mmap should return ESP_ERR_INVALID_ARG

    // unmap file to have correct initial conditions, regardless of result
    esp_partition_file_munmap();

    // get and initialize the control structure for file mmap
    esp_partition_file_mmap_ctrl_t *p_file_mmap_ctrl_input = esp_partition_get_file_mmap_ctrl_input();
    TEST_ASSERT_NOT_NULL(p_file_mmap_ctrl_input);

    memset(p_file_mmap_ctrl_input, 0, sizeof(*p_file_mmap_ctrl_input));
    const char *flash_file_name = "/tmp/xyz";
    strlcpy(p_file_mmap_ctrl_input->flash_file_name, flash_file_name, sizeof(p_file_mmap_ctrl_input->flash_file_name));
    p_file_mmap_ctrl_input->flash_file_size = 1;  // anything different from 0

    const uint8_t *p_mem_block = NULL;
    esp_err_t err = esp_partition_file_mmap(&p_mem_block);

    // expected result is invalid argument
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, err);

    // cleanup after test
    esp_partition_file_munmap();
    memset(p_file_mmap_ctrl_input, 0, sizeof(*p_file_mmap_ctrl_input));
}

/* Negative TC to ensure mmap setup checks presence of partition file name (partition table binary file)
 * if flash size parameter was specified.
 * This test case specifies just flash file size but omits partition table binary file name.
 */
TEST(partition_api, test_partition_mmap_size_no_partition)
{
    // Negative Scenario: conflicting settings - flash_file_name empty, flash_file_size set and partition_file_name not set
    // esp_partition_file_mmap should return ESP_ERR_INVALID_ARG

    // unmap file to have correct initial conditions, regardless of result
    esp_partition_file_munmap();

    // get and initialize the control structure for file mmap
    esp_partition_file_mmap_ctrl_t *p_file_mmap_ctrl_input = esp_partition_get_file_mmap_ctrl_input();
    TEST_ASSERT_NOT_NULL(p_file_mmap_ctrl_input);

    memset(p_file_mmap_ctrl_input, 0, sizeof(*p_file_mmap_ctrl_input));
    p_file_mmap_ctrl_input->flash_file_size = 1;  // anything different from 0

    const uint8_t *p_mem_block = NULL;
    esp_err_t err = esp_partition_file_mmap(&p_mem_block);

    // expected result is invalid argument
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, err);

    // cleanup after test
    esp_partition_file_munmap();
    memset(p_file_mmap_ctrl_input, 0, sizeof(*p_file_mmap_ctrl_input));
}

/* Negative TC to ensure mmap setup checks presence of flash size parameter if partition file name (partition table binary file) was specified.
 * This test case specifies just partition table binary file name but omits flash file size.
 */
TEST(partition_api, test_partition_mmap_no_size_partition)
{
    // Negative Scenario: conflicting settings - flash_file_name empty, flash_file_size not set and partition_file_name set
    // esp_partition_file_mmap should return ESP_ERR_INVALID_ARG

    // unmap file to have correct initial conditions, regardless of result
    esp_partition_file_munmap();

    // get and initialize the control structure for file mmap
    esp_partition_file_mmap_ctrl_t *p_file_mmap_ctrl_input = esp_partition_get_file_mmap_ctrl_input();
    TEST_ASSERT_NOT_NULL(p_file_mmap_ctrl_input);

    memset(p_file_mmap_ctrl_input, 0, sizeof(*p_file_mmap_ctrl_input));
    const char *partition_file_name = "/tmp/xyz.bin";
    strlcpy(p_file_mmap_ctrl_input->partition_file_name, partition_file_name, sizeof(p_file_mmap_ctrl_input->partition_file_name));

    const uint8_t *p_mem_block = NULL;
    esp_err_t err = esp_partition_file_mmap(&p_mem_block);

    // expected result is invalid argument
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, err);

    // cleanup after test
    esp_partition_file_munmap();
    memset(p_file_mmap_ctrl_input, 0, sizeof(*p_file_mmap_ctrl_input));
}

/*  Negative TC to ensure missing flash file to be mapped is reported with correct error code.
 */
TEST(partition_api, test_partition_mmap_ffile_nf)
{
    // Negative Scenario: specified flash_file_name file not found
    // esp_partition_file_mmap should return ESP_ERR_NOT_FOUND

    // unmap file to have correct initial conditions, regardless of result
    esp_partition_file_munmap();

    // get and initialize the control structure for file mmap
    esp_partition_file_mmap_ctrl_t *p_file_mmap_ctrl_input = esp_partition_get_file_mmap_ctrl_input();
    TEST_ASSERT_NOT_NULL(p_file_mmap_ctrl_input);

    memset(p_file_mmap_ctrl_input, 0, sizeof(*p_file_mmap_ctrl_input));

    // timestamp-based unique filename, the file is very unlikely to exist => no extra check
    char flash_file_name[40] = {0};
    partition_test_get_unique_filename(flash_file_name, sizeof(flash_file_name));

    strlcpy(p_file_mmap_ctrl_input->flash_file_name, flash_file_name, sizeof(p_file_mmap_ctrl_input->flash_file_name));

    const uint8_t *p_mem_block = NULL;
    esp_err_t err = esp_partition_file_mmap(&p_mem_block);

    // expected result is file not found
    TEST_ASSERT_EQUAL(ESP_ERR_NOT_FOUND, err);

    // cleanup after test
    esp_partition_file_munmap();
    memset(p_file_mmap_ctrl_input, 0, sizeof(*p_file_mmap_ctrl_input));
}

/* Negative TC to ensure missing binary partition file to be loaded is reported with correct error code.
 */
TEST(partition_api, test_partition_mmap_pfile_nf)
{
    // Negative Scenario: specified partition_file_name file not found
    // esp_partition_file_mmap should return ESP_ERR_NOT_FOUND

    // unmap file to have correct initial conditions, regardless of result
    esp_partition_file_munmap();

    // get and initialize the control structure for file mmap
    esp_partition_file_mmap_ctrl_t *p_file_mmap_ctrl_input = esp_partition_get_file_mmap_ctrl_input();
    TEST_ASSERT_NOT_NULL(p_file_mmap_ctrl_input);

    memset(p_file_mmap_ctrl_input, 0, sizeof(*p_file_mmap_ctrl_input));

    // timestamp-based unique filename, the file is very unlikely to exist => no extra check
    char partition_file_name[40] = {0};
    partition_test_get_unique_filename(partition_file_name, sizeof(partition_file_name));

    strlcpy(p_file_mmap_ctrl_input->partition_file_name, partition_file_name, sizeof(p_file_mmap_ctrl_input->partition_file_name));
    p_file_mmap_ctrl_input->flash_file_size = 0x10000;    // any non zero value to pass validation

    const uint8_t *p_mem_block = NULL;
    esp_err_t err = esp_partition_file_mmap(&p_mem_block);

    // expected result is file not found
    TEST_ASSERT_EQUAL(ESP_ERR_NOT_FOUND, err);

    // cleanup after test
    esp_partition_file_munmap();
    memset(p_file_mmap_ctrl_input, 0, sizeof(*p_file_mmap_ctrl_input));
}

/* Negative TC to check that requested size of emulated flash is at least so big to be able to load binary partition table.
 * Too small emulated flash size is introduced and respective error code is evaluated after mmap call.
 */
TEST(partition_api, test_partition_mmap_size_too_small)
{
    // Negative Scenario: specified flash file size too small to hold at least partition table at default offset
    // esp_partition_file_mmap should return ESP_ERR_INVALID_SIZE

    // unmap file to have correct initial conditions, regardless of result
    esp_partition_file_munmap();

    // get and initialize the control structure for file mmap
    esp_partition_file_mmap_ctrl_t *p_file_mmap_ctrl_input = esp_partition_get_file_mmap_ctrl_input();
    TEST_ASSERT_NOT_NULL(p_file_mmap_ctrl_input);

    memset(p_file_mmap_ctrl_input, 0, sizeof(*p_file_mmap_ctrl_input));

    // set valid partition table name and very small flash size
    strlcpy(p_file_mmap_ctrl_input->partition_file_name, BUILD_DIR "/partition_table/partition-table.bin", sizeof(p_file_mmap_ctrl_input->partition_file_name));
    p_file_mmap_ctrl_input->flash_file_size = 1;

    const uint8_t *p_mem_block = NULL;
    esp_err_t err = esp_partition_file_mmap(&p_mem_block);

    // expected result is invalid argument
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_SIZE, err);

    // cleanup after test
    esp_partition_file_munmap();
    memset(p_file_mmap_ctrl_input, 0, sizeof(*p_file_mmap_ctrl_input));
}

typedef struct {
    size_t read_ops;
    size_t write_ops;
    size_t erase_ops;
    size_t read_bytes;
    size_t write_bytes;
    size_t total_time;
    size_t *sector_erase_count;
    size_t sector_erase_count_size;
} t_stats;

void init_stats(t_stats *p_stats)
{
    memset(p_stats, 0, sizeof(t_stats));
    p_stats->sector_erase_count_size = esp_partition_get_file_mmap_ctrl_act()->flash_file_size / ESP_PARTITION_EMULATED_SECTOR_SIZE;
    p_stats->sector_erase_count = calloc(p_stats->sector_erase_count_size, sizeof(size_t));
}

void dispose_stats(t_stats *p_stats)
{
    if (p_stats->sector_erase_count != NULL) {
        free(p_stats->sector_erase_count);
    }
}

void print_stats(const t_stats *p_stats)
{
    ESP_LOGI(TAG, "read_ops:%06lu write_ops:%06lu erase_ops:%06lu read_bytes:%06lu write_bytes:%06lu total_time:%06lu",
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

    for (size_t i = 0; i < p_stats->sector_erase_count_size; i++) {
        p_stats->sector_erase_count[i] = esp_partition_get_sector_erase_count(i);
    }
}

// evaluates if final stats differ from initial stats by expected difference stats.
// if there is no need to evaluate some stats, set respective expected difference stats members to SIZE_MAX
bool evaluate_stats(const t_stats *p_initial_stats, const t_stats *p_final_stats, const t_stats *p_expected_difference_stats)
{
    if (p_expected_difference_stats->read_ops != SIZE_MAX) {
        TEST_ASSERT_EQUAL(p_initial_stats->read_ops + p_expected_difference_stats->read_ops, p_final_stats->read_ops);
    }
    if (p_expected_difference_stats->write_ops != SIZE_MAX) {
        TEST_ASSERT_EQUAL(p_initial_stats->write_ops + p_expected_difference_stats->write_ops, p_final_stats->write_ops);
    }
    if (p_expected_difference_stats->erase_ops != SIZE_MAX) {
        TEST_ASSERT_EQUAL(p_initial_stats->erase_ops + p_expected_difference_stats->erase_ops, p_final_stats->erase_ops);
    }
    if (p_expected_difference_stats->read_bytes != SIZE_MAX) {
        TEST_ASSERT_EQUAL(p_initial_stats->read_bytes + p_expected_difference_stats->read_bytes, p_final_stats->read_bytes);
    }
    if (p_expected_difference_stats->write_bytes != SIZE_MAX) {
        TEST_ASSERT_EQUAL(p_initial_stats->write_bytes + p_expected_difference_stats->write_bytes, p_final_stats->write_bytes);
    }
    if (p_expected_difference_stats->total_time != SIZE_MAX) {
        TEST_ASSERT_EQUAL(p_initial_stats->total_time + p_expected_difference_stats->total_time, p_final_stats->total_time);
    }

    for (size_t i = 0; i < p_initial_stats->sector_erase_count_size; i++) {
        if (p_expected_difference_stats->sector_erase_count[i] != SIZE_MAX) {
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
    t_stats zero_stats;

    init_stats(&initial_stats);
    init_stats(&final_stats);
    init_stats(&zero_stats);

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
    err = esp_partition_read(partition_data, 0, test_data_ptr, size);
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
    if ((non_aligned_portions % ESP_PARTITION_EMULATED_SECTOR_SIZE) > 0) {
        erase_ops += 1;
    }

    t_stats expected_difference_stats;
    init_stats(&expected_difference_stats);

    expected_difference_stats.read_ops = 1;
    expected_difference_stats.write_ops = 1;
    expected_difference_stats.erase_ops = erase_ops;
    expected_difference_stats.read_bytes = size;
    expected_difference_stats.write_bytes = size;
    expected_difference_stats.total_time = SIZE_MAX;
    for (size_t i = 0; i < expected_difference_stats.sector_erase_count_size; i++) {
        expected_difference_stats.sector_erase_count[i] = SIZE_MAX;
    }

    evaluate_stats(&initial_stats, &final_stats, &expected_difference_stats);

    // clear statistics
    esp_partition_clear_stats();
    read_stats(&final_stats);

    // evaluate zero statistics
    evaluate_stats(&zero_stats, &final_stats, &zero_stats);

    // free symanically allocated space
    dispose_stats(&initial_stats);
    dispose_stats(&final_stats);
    dispose_stats(&zero_stats);
    dispose_stats(&expected_difference_stats);
    free(test_data_ptr);
}

TEST(partition_api, test_partition_power_off_emulation)
{
    const esp_partition_t *partition_data = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_ANY, "storage");
    TEST_ASSERT_NOT_NULL(partition_data);

    //no offset, map whole partition
    size_t offset = 0;
    size_t size = partition_data->size;

    // prepare test data block
    void *test_data_ptr = malloc(size);
    TEST_ASSERT_NOT_NULL(test_data_ptr);
    memset(test_data_ptr, 0xff, size);

    // --- power-off off ---
    // ensure power-off emulation is off
    esp_partition_fail_after(SIZE_MAX, 0);

    // erase partition data
    esp_err_t err = esp_partition_erase_range(partition_data, offset, size);
    TEST_ESP_OK(err);

    // write data - should pass
    err = esp_partition_write(partition_data, offset, test_data_ptr, size);
    TEST_ESP_OK(err);

    // erase partition data
    err = esp_partition_erase_range(partition_data, offset, size);
    TEST_ESP_OK(err);

    // --- power-off on, write ---
    // ensure power-off emulation is on, below the limit for size
    // esp_partition_write consumes one power off failure cycle per 4 bytes written
    esp_partition_fail_after(size / 4, ESP_PARTITION_FAIL_AFTER_MODE_BOTH);

    // write data - should fail
    err = esp_partition_write(partition_data, offset, test_data_ptr, size);
    TEST_ASSERT_EQUAL(ESP_ERR_FLASH_OP_FAIL, err);

    // --- power-off on, erase has just enough power off failure cycles available---
    // ensure power-off emulation is on, at the limit for size
    // esp_partition_erase_range consumes one power-off emulation cycle per one virtual sector erased
    esp_partition_fail_after(size / ESP_PARTITION_EMULATED_SECTOR_SIZE + 1, ESP_PARTITION_FAIL_AFTER_MODE_BOTH);

    // write data - should be ok
    err = esp_partition_erase_range(partition_data, offset, size);
    TEST_ASSERT_EQUAL(ESP_OK, err);

    // --- power-off on, erase has one cycle less than required---
    // ensure power-off emulation is on, below the limit for size
    // esp_partition_erase_range consumes one power-off emulation cycle per one virtual sector erased
    esp_partition_fail_after(size / ESP_PARTITION_EMULATED_SECTOR_SIZE, ESP_PARTITION_FAIL_AFTER_MODE_BOTH);

    // write data - should fail
    err = esp_partition_erase_range(partition_data, offset, size);
    TEST_ASSERT_EQUAL(ESP_ERR_FLASH_OP_FAIL, err);

    // ---cleanup ---
    // disable power-off emulation
    esp_partition_fail_after(SIZE_MAX, 0);
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
    RUN_TEST_CASE(partition_api, test_partition_mmap_diff_size);
    RUN_TEST_CASE(partition_api, test_partition_mmap_reopen);
    RUN_TEST_CASE(partition_api, test_partition_mmap_remove);
    RUN_TEST_CASE(partition_api, test_partition_mmap_name_size);
    RUN_TEST_CASE(partition_api, test_partition_mmap_size_no_partition);
    RUN_TEST_CASE(partition_api, test_partition_mmap_no_size_partition);
    RUN_TEST_CASE(partition_api, test_partition_mmap_ffile_nf);
    RUN_TEST_CASE(partition_api, test_partition_mmap_pfile_nf);
    RUN_TEST_CASE(partition_api, test_partition_mmap_size_too_small);
    RUN_TEST_CASE(partition_api, test_partition_stats);
    RUN_TEST_CASE(partition_api, test_partition_power_off_emulation);
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
