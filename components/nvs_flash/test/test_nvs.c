#include <stdio.h>
#include <ctype.h>
#include <errno.h>
#include <stdlib.h>
#include <time.h>
#include "unity.h"
#include "nvs.h"
#include "nvs_flash.h"
#include "esp_partition.h"
#include "esp_log.h"
#include <string.h>

static const char* TAG = "test_nvs";

TEST_CASE("various nvs tests", "[nvs]")
{
    nvs_handle handle_1;
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES) {
        ESP_LOGW(TAG, "nvs_flash_init failed (0x%x), erasing partition and retrying", err);
        const esp_partition_t* nvs_partition = esp_partition_find_first(
                ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_NVS, NULL);
        assert(nvs_partition && "partition table must have an NVS partition");
        ESP_ERROR_CHECK( esp_partition_erase_range(nvs_partition, 0, nvs_partition->size) );
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK( err );

    TEST_ESP_ERR(nvs_open("test_namespace1", NVS_READONLY, &handle_1), ESP_ERR_NVS_NOT_FOUND);

    TEST_ESP_ERR(nvs_set_i32(handle_1, "foo", 0x12345678), ESP_ERR_NVS_INVALID_HANDLE);
    nvs_close(handle_1);

    TEST_ESP_OK(nvs_open("test_namespace2", NVS_READWRITE, &handle_1));
    TEST_ESP_OK(nvs_erase_all(handle_1));
    TEST_ESP_OK(nvs_set_i32(handle_1, "foo", 0x12345678));
    TEST_ESP_OK(nvs_set_i32(handle_1, "foo", 0x23456789));

    nvs_handle handle_2;
    TEST_ESP_OK(nvs_open("test_namespace3", NVS_READWRITE, &handle_2));
    TEST_ESP_OK(nvs_erase_all(handle_2));
    TEST_ESP_OK(nvs_set_i32(handle_2, "foo", 0x3456789a));
    const char* str = "value 0123456789abcdef0123456789abcdef";
    TEST_ESP_OK(nvs_set_str(handle_2, "key", str));

    int32_t v1;
    TEST_ESP_OK(nvs_get_i32(handle_1, "foo", &v1));
    TEST_ASSERT_EQUAL_INT32(0x23456789, v1);

    int32_t v2;
    TEST_ESP_OK(nvs_get_i32(handle_2, "foo", &v2));
    TEST_ASSERT_EQUAL_INT32(0x3456789a, v2);

    char buf[strlen(str) + 1];
    size_t buf_len = sizeof(buf);

    TEST_ESP_OK(nvs_get_str(handle_2, "key", buf, &buf_len));

    TEST_ASSERT_EQUAL_INT32(0, strcmp(buf, str));

    nvs_close(handle_1);

    // check that deinit does not leak memory if some handles are still open
    nvs_flash_deinit();

    nvs_close(handle_2);
}

TEST_CASE("calculate used and free space", "[nvs]")
{
    TEST_ESP_ERR(nvs_get_stats(NULL, NULL), ESP_ERR_INVALID_ARG);
    nvs_stats_t stat1;
    nvs_stats_t stat2;
    TEST_ESP_ERR(nvs_get_stats(NULL, &stat1), ESP_ERR_NVS_NOT_INITIALIZED);
    TEST_ASSERT_TRUE(stat1.free_entries == 0);
    TEST_ASSERT_TRUE(stat1.namespace_count == 0);
    TEST_ASSERT_TRUE(stat1.total_entries == 0);
    TEST_ASSERT_TRUE(stat1.used_entries == 0);

    nvs_handle handle = 0;
    size_t h_count_entries;
    TEST_ESP_ERR(nvs_get_used_entry_count(handle, &h_count_entries), ESP_ERR_NVS_INVALID_HANDLE);
    TEST_ASSERT_TRUE(h_count_entries == 0);

    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES) {
        ESP_LOGW(TAG, "nvs_flash_init failed (0x%x), erasing partition and retrying", err);
        const esp_partition_t* nvs_partition = esp_partition_find_first(
                ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_NVS, NULL);
        assert(nvs_partition && "partition table must have an NVS partition");
        ESP_ERROR_CHECK( esp_partition_erase_range(nvs_partition, 0, nvs_partition->size) );
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK( err );

    // erase if have any namespace
    TEST_ESP_OK(nvs_get_stats(NULL, &stat1));
    if(stat1.namespace_count != 0) {
        TEST_ESP_OK(nvs_flash_erase());
        TEST_ESP_OK(nvs_flash_deinit());
        TEST_ESP_OK(nvs_flash_init());
    }

    // after erase. empty partition
    TEST_ESP_OK(nvs_get_stats(NULL, &stat1));
    TEST_ASSERT_TRUE(stat1.free_entries != 0);
    TEST_ASSERT_TRUE(stat1.namespace_count == 0);
    TEST_ASSERT_TRUE(stat1.total_entries != 0);
    TEST_ASSERT_TRUE(stat1.used_entries == 0);

    // create namespace test_k1
    nvs_handle handle_1;
    TEST_ESP_OK(nvs_open("test_k1", NVS_READWRITE, &handle_1));
    TEST_ESP_OK(nvs_get_stats(NULL, &stat2));
    TEST_ASSERT_TRUE(stat2.free_entries + 1 == stat1.free_entries);
    TEST_ASSERT_TRUE(stat2.namespace_count == 1);
    TEST_ASSERT_TRUE(stat2.total_entries == stat1.total_entries);
    TEST_ASSERT_TRUE(stat2.used_entries == 1);

    // create pair key-value com
    TEST_ESP_OK(nvs_set_i32(handle_1, "com", 0x12345678));
    TEST_ESP_OK(nvs_get_stats(NULL, &stat1));
    TEST_ASSERT_TRUE(stat1.free_entries + 1 == stat2.free_entries);
    TEST_ASSERT_TRUE(stat1.namespace_count == 1);
    TEST_ASSERT_TRUE(stat1.total_entries == stat2.total_entries);
    TEST_ASSERT_TRUE(stat1.used_entries == 2);

    // change value in com
    TEST_ESP_OK(nvs_set_i32(handle_1, "com", 0x01234567));
    TEST_ESP_OK(nvs_get_stats(NULL, &stat2));
    TEST_ASSERT_TRUE(stat2.free_entries == stat1.free_entries);
    TEST_ASSERT_TRUE(stat2.namespace_count == 1);
    TEST_ASSERT_TRUE(stat2.total_entries != 0);
    TEST_ASSERT_TRUE(stat2.used_entries == 2);

    // create pair key-value ru
    TEST_ESP_OK(nvs_set_i32(handle_1, "ru", 0x00FF00FF));
    TEST_ESP_OK(nvs_get_stats(NULL, &stat1));
    TEST_ASSERT_TRUE(stat1.free_entries + 1 == stat2.free_entries);
    TEST_ASSERT_TRUE(stat1.namespace_count == 1);
    TEST_ASSERT_TRUE(stat1.total_entries != 0);
    TEST_ASSERT_TRUE(stat1.used_entries == 3);

    // amount valid pair in namespace 1
    size_t h1_count_entries;
    TEST_ESP_OK(nvs_get_used_entry_count(handle_1, &h1_count_entries));
    TEST_ASSERT_TRUE(h1_count_entries == 2);

    nvs_handle handle_2;
    // create namespace test_k2
    TEST_ESP_OK(nvs_open("test_k2", NVS_READWRITE, &handle_2));
    TEST_ESP_OK(nvs_get_stats(NULL, &stat2));
    TEST_ASSERT_TRUE(stat2.free_entries + 1 == stat1.free_entries);
    TEST_ASSERT_TRUE(stat2.namespace_count == 2);
    TEST_ASSERT_TRUE(stat2.total_entries == stat1.total_entries);
    TEST_ASSERT_TRUE(stat2.used_entries == 4);

    // create pair key-value
    TEST_ESP_OK(nvs_set_i32(handle_2, "su1", 0x00000001));
    TEST_ESP_OK(nvs_set_i32(handle_2, "su2", 0x00000002));
    TEST_ESP_OK(nvs_set_i32(handle_2, "sus", 0x00000003));
    TEST_ESP_OK(nvs_get_stats(NULL, &stat1));
    TEST_ASSERT_TRUE(stat1.free_entries + 3 == stat2.free_entries);
    TEST_ASSERT_TRUE(stat1.namespace_count == 2);
    TEST_ASSERT_TRUE(stat1.total_entries == stat2.total_entries);
    TEST_ASSERT_TRUE(stat1.used_entries == 7);

    TEST_ASSERT_TRUE(stat1.total_entries == (stat1.used_entries + stat1.free_entries));

    // amount valid pair in namespace 2
    size_t h2_count_entries;
    TEST_ESP_OK(nvs_get_used_entry_count(handle_2, &h2_count_entries));
    TEST_ASSERT_TRUE(h2_count_entries == 3);

    TEST_ASSERT_TRUE(stat1.used_entries == (h1_count_entries + h2_count_entries + stat1.namespace_count));

    nvs_close(handle_1);
    nvs_close(handle_2);

    size_t temp = h2_count_entries;
    TEST_ESP_ERR(nvs_get_used_entry_count(handle_1, &h2_count_entries), ESP_ERR_NVS_INVALID_HANDLE);
    TEST_ASSERT_TRUE(h2_count_entries == 0);
    h2_count_entries = temp;
    TEST_ESP_ERR(nvs_get_used_entry_count(handle_1, NULL), ESP_ERR_INVALID_ARG);

    nvs_handle handle_3;
    // create namespace test_k3
    TEST_ESP_OK(nvs_open("test_k3", NVS_READWRITE, &handle_3));
    TEST_ESP_OK(nvs_get_stats(NULL, &stat2));
    TEST_ASSERT_TRUE(stat2.free_entries + 1 == stat1.free_entries);
    TEST_ASSERT_TRUE(stat2.namespace_count == 3);
    TEST_ASSERT_TRUE(stat2.total_entries == stat1.total_entries);
    TEST_ASSERT_TRUE(stat2.used_entries == 8);

    // create pair blobs
    uint32_t blob[12];
    TEST_ESP_OK(nvs_set_blob(handle_3, "bl1", &blob, sizeof(blob)));
    TEST_ESP_OK(nvs_get_stats(NULL, &stat1));
    TEST_ASSERT_TRUE(stat1.free_entries + 4 == stat2.free_entries);
    TEST_ASSERT_TRUE(stat1.namespace_count == 3);
    TEST_ASSERT_TRUE(stat1.total_entries == stat2.total_entries);
    TEST_ASSERT_TRUE(stat1.used_entries == 12);

    // amount valid pair in namespace 2
    size_t h3_count_entries;
    TEST_ESP_OK(nvs_get_used_entry_count(handle_3, &h3_count_entries));
    TEST_ASSERT_TRUE(h3_count_entries == 4);

    TEST_ASSERT_TRUE(stat1.used_entries == (h1_count_entries + h2_count_entries + h3_count_entries + stat1.namespace_count));

    nvs_close(handle_3);

    TEST_ESP_OK(nvs_flash_erase());
    TEST_ESP_OK(nvs_flash_deinit());
}
