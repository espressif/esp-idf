/*
 * SPDX-FileCopyrightText: 2025-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
/* Non-Volatile Storage (NVS) Statistics Example

   For other examples please check:
   https://github.com/espressif/esp-idf/tree/master/examples

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_check.h"
#include "esp_system.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "nvs.h"

#define MOCK_DATA_NAMESPACE "_mock_data"
#define MOCK_DATA_BACKUP_NAMESPACE "_mock_backup"

/* NVS on-flash geometry. These are by-design constants of the NVS format (one
 * 4096-byte flash sector per page, 32-byte entries, 126 usable entries per
 * page). They are not exposed through the public API, so they are mirrored here
 * to allow precise control over page-level fragmentation below. */
#define NVS_ENTRY_SIZE              32
#define NVS_ENTRIES_PER_PAGE        126
#define NVS_PAGE_CHUNK_MAX_SIZE     (NVS_ENTRY_SIZE * (NVS_ENTRIES_PER_PAGE - 1)) // 4000 B

/* String lengths (strlen, excluding the NUL terminator) used to fragment a
 * partition. A string of length L occupies 1 header entry + ceil((L+1)/32) data
 * entries and must fit contiguously within a single page.
 *  - FRAG_STR_LEN fills a fresh page to 124 entries, leaving exactly 2 free.
 *  - FRAG_STR_FIRST_LEN is one entry shorter to account for the namespace entry
 *    that is written on the first page, so that page also keeps 2 free entries. */
#define FRAG_STR_LEN                (123 * NVS_ENTRY_SIZE - 1) // 3935 -> 124 entries
#define FRAG_STR_FIRST_LEN          (122 * NVS_ENTRY_SIZE - 1) // 3903 -> 123 entries

#define FRAG_NAMESPACE              "_frag"
#define BLOB_NAMESPACE              "_blobs"

static const char *TAG = "nvs_statistics_example";

// Maximum key character length is 15 (NVS_KEY_NAME_MAX_SIZE-1)
static const char* mock_data_keys[] = {
    "wifi_ssid",
    "wifi_pass",
    "dev_name",
    "temp_unit",
    "target_temp",
    "eco_mode",
    "fw_version",
    "led_bright",
    "auto_update",
    "last_sync",
    "user_lang",
    "long_token",
    "very_long_token"
};

// String values are limited in length by NVS implementation to 4000 bytes (including null character)
// Each string value occupies 1 overhead NVS entry + 1 NVS entry per each 32 characters including null character
#define CHARS_PER_STRING_VALUE_ENTRY 32
static const char* mock_data_values[] = {
    "HomeNetwork",
    "MySecretPass",
    "LivingRoomThermostat",
    "Celsius",
    "22",
    "false",
    "1.2.3",
    "80",
    "true",
    "2025-01-01T08:00:00Z",
    "en",
    "2f8c1e7b5a4d9c6e3b0f1a8e5d7c2b6f4e1a9c7b",
    "7e2b1c9f5a4d8e3b0f1a6c7e2d9b5a4c8e1f7b2d6c3a9e5b0f1a8c7e2d9b5a4c8e1f7b2d6c3a9e5b"
};

static const size_t mock_data_count = sizeof(mock_data_keys) / sizeof(mock_data_keys[0]);

static void print_nvs_stats(nvs_stats_t *nvs_stats)
{
    ESP_LOGI(TAG, "NVS statistics:");
    ESP_LOGI(TAG, "Used NVS entries: %u", nvs_stats->used_entries);
    ESP_LOGI(TAG, "Free NVS entries: %u", nvs_stats->free_entries);
    ESP_LOGI(TAG, "Available NVS entries: %u", nvs_stats->available_entries);
    ESP_LOGI(TAG, "Total NVS entries: %u", nvs_stats->total_entries);
    ESP_LOGI(TAG, "Namespace count: %u", nvs_stats->namespace_count);
}

static size_t calculate_expected_entries_for_string_array(const char** string_array, size_t array_size)
{
    size_t expected_newly_used_entries = 0;
    for (int i = 0; i < array_size; i++) {
        // Each string occupies 1 overhead entry + 1 entry per each 32 characters including null character
        size_t str_len = strlen(string_array[i]);
        size_t span = 1 + ((str_len + CHARS_PER_STRING_VALUE_ENTRY - 1) / CHARS_PER_STRING_VALUE_ENTRY); // 1 overhead + integer division rounding up
        expected_newly_used_entries += span;
    }
    return expected_newly_used_entries;
}

static esp_err_t save_mock_data_to_namespace(const char* namespace_name)
{
    ESP_LOGI(TAG, "Opening Non-Volatile Storage (NVS) handle for namespace '%s'...", namespace_name);
    nvs_handle_t my_handle;
    // Opening NVS storage handle uses up 1 entry for the namespace
    esp_err_t ret = nvs_open(namespace_name, NVS_READWRITE, &my_handle);
    ESP_RETURN_ON_ERROR(ret, TAG, "Error (%s) opening NVS handle for namespace '%s'!", esp_err_to_name(ret), namespace_name);

    ESP_LOGI(TAG, "Getting NVS statistics...");
    nvs_stats_t nvs_stats;
    ret = nvs_get_stats(NVS_DEFAULT_PART_NAME, &nvs_stats);
    ESP_GOTO_ON_ERROR(ret, nvs_close_label, TAG, "Error (%s) getting NVS statistics!", esp_err_to_name(ret));

    // Print the pre-write NVS statistics
    print_nvs_stats(&nvs_stats);

    ESP_LOGI(TAG, "Writing mock data key-value pairs to NVS namespace '%s'...", namespace_name);
    for (int i = 0; i < mock_data_count; i++) {
        // Write string values
        ret = nvs_set_str(my_handle, mock_data_keys[i], mock_data_values[i]);
        // Don't break/abort on error, try to continue writing
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "Error (%s) writing '%s':'%s' pair!", esp_err_to_name(ret), mock_data_keys[i], mock_data_values[i]);
        }
    }

    ESP_LOGI(TAG, "Committing data in NVS namespace '%s'...", namespace_name);
    ret = nvs_commit(my_handle);
    ESP_GOTO_ON_ERROR(ret, nvs_close_label, TAG, "Error (%s) committing data for namespace '%s'!", esp_err_to_name(ret), namespace_name);

    ESP_LOGI(TAG, "Getting post-commit NVS statistics...");
    nvs_stats_t new_nvs_stats;
    ret = nvs_get_stats(NVS_DEFAULT_PART_NAME, &new_nvs_stats);
    ESP_GOTO_ON_ERROR(ret, nvs_close_label, TAG, "Error (%s) getting NVS statistics!", esp_err_to_name(ret));

    // Print the post-write NVS statistics
    print_nvs_stats(&new_nvs_stats);

    size_t expected_newly_used_entries = calculate_expected_entries_for_string_array(mock_data_values, mock_data_count);

    if (new_nvs_stats.used_entries - nvs_stats.used_entries != expected_newly_used_entries) {
        ESP_LOGE(TAG, "Newly used entries: %u, expected: %u.",
                 new_nvs_stats.used_entries - nvs_stats.used_entries,
                 expected_newly_used_entries);
    } else {
        ESP_LOGI(TAG, "Newly used entries match expectation.");
        ESP_LOGI(TAG, "Newly used entries: %u, expected: %u.",
                 new_nvs_stats.used_entries - nvs_stats.used_entries,
                 expected_newly_used_entries);
    }

    nvs_close_label:
    // Close the storage handle, freeing allocated resources
    nvs_close(my_handle);
    ESP_LOGI(TAG, "NVS handle for namespace '%s' closed.", namespace_name);

    return ESP_OK;
}

static esp_err_t read_mock_data_from_namespace(const char* namespace_name)
{
    ESP_LOGI(TAG, "Opening Non-Volatile Storage (NVS) handle for namespace '%s'...", namespace_name);
    nvs_handle_t my_handle;
    esp_err_t ret = nvs_open(namespace_name, NVS_READONLY, &my_handle);
    ESP_RETURN_ON_ERROR(ret, TAG, "Error (%s) opening NVS handle for namespace '%s'!", esp_err_to_name(ret), namespace_name);

    ESP_LOGI(TAG, "Reading stored data from NVS namespace '%s'...", namespace_name);
    for (int i = 0; i < mock_data_count; i++) {
        size_t required_size = 0;
        // Obtain required size of the string value including null character
        ret = nvs_get_str(my_handle, mock_data_keys[i], NULL, &required_size);
        if (ret != ESP_OK && ret != ESP_ERR_NVS_NOT_FOUND) {
            ESP_LOGE(TAG, "Error (%s) getting required size for key '%s'!", esp_err_to_name(ret), mock_data_keys[i]);
            continue;
        }
        if (ret == ESP_ERR_NVS_NOT_FOUND) {
            ESP_LOGW(TAG, "Key '%s' not found in namespace '%s'!", mock_data_keys[i], namespace_name);
            continue;
        }

        // Allocate memory for the string value
        char* value = malloc(required_size);
        if (value == NULL) {
            ESP_LOGE(TAG, "Failed to allocate memory for key '%s'!", mock_data_keys[i]);
            continue;
        }

        // Read the string value
        ret = nvs_get_str(my_handle, mock_data_keys[i], value, &required_size);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "Error (%s) reading key '%s'!", esp_err_to_name(ret), mock_data_keys[i]);
            free(value);
            continue;
        }

        ESP_LOGI(TAG, "Read key-value pair from NVS: '%s':'%s'", mock_data_keys[i], value);
        free(value);
    }

    // Close the storage handle, freeing allocated resources
    nvs_close(my_handle);
    ESP_LOGI(TAG, "NVS handle for namespace '%s' closed.", namespace_name);

    return ESP_OK;
}

#if CONFIG_EXAMPLE_RUN_OVERHEAD_MEASUREMENT

// Partitions of various sizes (declared in partitions.csv) swept by the measurement.
static const char* measured_partitions[] = {
    "nvs_16k",
    "nvs_32k",
    "nvs_64k",
};

// Blob sizes (in bytes) measured for each partition.
static const size_t blob_sizes[] = {128, 256, 512, 1024, 2048, 4096};

// Theoretical entries consumed by a single blob in a non-fragmented partition:
// 1 BLOB_INDEX entry + 'chunks' chunk-header entries + ceil(size/32) data entries.
static size_t entries_per_blob_ideal(size_t blob_size)
{
    size_t chunks = (blob_size + NVS_PAGE_CHUNK_MAX_SIZE - 1) / NVS_PAGE_CHUNK_MAX_SIZE;
    if (chunks == 0) {
        chunks = 1;
    }
    size_t data_entries = (blob_size + NVS_ENTRY_SIZE - 1) / NVS_ENTRY_SIZE;
    return 1 + chunks + data_entries;
}

// Pre-populate a partition with large strings so that every page is filled up to
// its last 2 entries. Returns the number of strings written.
static size_t fragment_partition(const char* partition_name)
{
    nvs_handle_t handle;
    esp_err_t err = nvs_open_from_partition(partition_name, FRAG_NAMESPACE, NVS_READWRITE, &handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error (%s) opening fragmentation handle on '%s'!", esp_err_to_name(err), partition_name);
        return 0;
    }

    char* buffer = malloc(FRAG_STR_LEN + 1);
    if (buffer == NULL) {
        ESP_LOGE(TAG, "Failed to allocate fragmentation buffer!");
        nvs_close(handle);
        return 0;
    }
    memset(buffer, 'A', FRAG_STR_LEN);
    buffer[FRAG_STR_LEN] = '\0';

    size_t count = 0;
    char key[16];

    // First string is one entry shorter to compensate for the namespace entry
    // written on page 0, so that page also retains exactly 2 free entries.
    buffer[FRAG_STR_FIRST_LEN] = '\0';
    snprintf(key, sizeof(key), "f%05u", (unsigned)count);
    err = nvs_set_str(handle, key, buffer);
    buffer[FRAG_STR_FIRST_LEN] = 'A';
    if (err == ESP_OK && nvs_commit(handle) == ESP_OK) {
        count++;
    }

    // Remaining full-page strings until the partition cannot hold another one.
    while (true) {
        snprintf(key, sizeof(key), "f%05u", (unsigned)count);
        err = nvs_set_str(handle, key, buffer);
        if (err == ESP_ERR_NVS_NOT_ENOUGH_SPACE) {
            break;
        }
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "Error (%s) writing fragmentation string!", esp_err_to_name(err));
            break;
        }
        if (nvs_commit(handle) != ESP_OK) {
            break;
        }
        count++;
    }

    free(buffer);
    nvs_close(handle);
    return count;
}

// Fill a partition with same-sized blobs until it runs out of space.
// Returns the number of blobs successfully stored.
static size_t fill_with_blobs(const char* partition_name, size_t blob_size)
{
    nvs_handle_t handle;
    esp_err_t err = nvs_open_from_partition(partition_name, BLOB_NAMESPACE, NVS_READWRITE, &handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error (%s) opening blob handle on '%s'!", esp_err_to_name(err), partition_name);
        return 0;
    }

    uint8_t* blob = malloc(blob_size);
    if (blob == NULL) {
        ESP_LOGE(TAG, "Failed to allocate %u B blob buffer!", (unsigned)blob_size);
        nvs_close(handle);
        return 0;
    }
    memset(blob, 0x5A, blob_size);

    size_t count = 0;
    char key[16];
    while (true) {
        snprintf(key, sizeof(key), "b%05u", (unsigned)count);
        err = nvs_set_blob(handle, key, blob, blob_size);
        if (err == ESP_ERR_NVS_NOT_ENOUGH_SPACE) {
            break;
        }
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "Error (%s) writing blob!", esp_err_to_name(err));
            break;
        }
        err = nvs_commit(handle);
        if (err == ESP_ERR_NVS_NOT_ENOUGH_SPACE) {
            break;
        }
        count++;
    }

    free(blob);
    nvs_close(handle);
    return count;
}

// Run one measurement cell: erase + init a partition, optionally fragment it,
// fill it with blobs of the given size and report heap/entry/overhead statistics.
static void measure_blob_overhead(const char* partition_name, size_t blob_size, bool fragment)
{
    ESP_ERROR_CHECK(nvs_flash_erase_partition(partition_name));

    uint32_t heap_before_init = esp_get_free_heap_size();
    ESP_ERROR_CHECK(nvs_flash_init_partition(partition_name));
    uint32_t heap_after_init = esp_get_free_heap_size();

    nvs_stats_t stats;
    ESP_ERROR_CHECK(nvs_get_stats(partition_name, &stats));
    size_t total_entries = stats.total_entries;

    size_t frag_strings = 0;
    if (fragment) {
        frag_strings = fragment_partition(partition_name);
    }

    size_t stored = fill_with_blobs(partition_name, blob_size);
    uint32_t heap_after_fill = esp_get_free_heap_size();

    ESP_ERROR_CHECK(nvs_get_stats(partition_name, &stats));

    size_t per_blob = entries_per_blob_ideal(blob_size);
    size_t expected = (per_blob != 0) ? (total_entries / per_blob) : 0;
    size_t payload = stored * blob_size;
    size_t capacity = total_entries * NVS_ENTRY_SIZE;
    int overhead_pct = (capacity != 0) ? (int)(100 - (100ULL * payload) / capacity) : 0;

    printf("\n");
    printf("NVS BLOB TEST - %u B (partition '%s', %s):\n",
           (unsigned)blob_size, partition_name, fragment ? "fragmented" : "pristine");
    printf("======================\n");
    printf("heap before NVS init: %" PRIu32 " B\n", heap_before_init);
    printf("heap after NVS init:  %" PRIu32 " B (diff %" PRId32 " B)\n",
           heap_after_init, (int32_t)(heap_before_init - heap_after_init));
    if (fragment) {
        printf("fragmentation strings written: %u\n", (unsigned)frag_strings);
    }
    printf("\n");
    printf("available heap after fill: %" PRIu32 " B (diff %" PRId32 " B)\n",
           heap_after_fill, (int32_t)(heap_after_init - heap_after_fill));
    printf("expected blobs count: %u\n", (unsigned)expected);
    printf("stored blobs count:   %u\n", (unsigned)stored);
    printf("used_entries:  %u (%u B)\n", stats.used_entries, (unsigned)(stats.used_entries * NVS_ENTRY_SIZE));
    printf("free_entries:  %u (%u B)\n", stats.free_entries, (unsigned)(stats.free_entries * NVS_ENTRY_SIZE));
    printf("total_entries: %u (%u B)\n", stats.total_entries, (unsigned)(stats.total_entries * NVS_ENTRY_SIZE));
    printf("STORAGE OVERHEAD: %d%%\n", overhead_pct);

    ESP_ERROR_CHECK(nvs_flash_deinit_partition(partition_name));
}

static void run_overhead_measurement(void)
{
    const size_t partition_count = sizeof(measured_partitions) / sizeof(measured_partitions[0]);
    const size_t blob_size_count = sizeof(blob_sizes) / sizeof(blob_sizes[0]);

    ESP_LOGI(TAG, "Starting NVS blob storage-overhead measurement...");
    for (size_t p = 0; p < partition_count; p++) {
        for (size_t b = 0; b < blob_size_count; b++) {
            measure_blob_overhead(measured_partitions[p], blob_sizes[b], false);
#if CONFIG_EXAMPLE_INDUCE_FRAGMENTATION
            measure_blob_overhead(measured_partitions[p], blob_sizes[b], true);
#endif
        }
    }
    ESP_LOGI(TAG, "NVS blob storage-overhead measurement done.");
}

#endif // CONFIG_EXAMPLE_RUN_OVERHEAD_MEASUREMENT

void app_main(void)
{
    // Erase the contents of the default NVS partition for clean run of this example
    ESP_LOGI(TAG, "Erasing the contents of the default NVS partition...");
    ESP_ERROR_CHECK(nvs_flash_erase());

    // Initialize NVS on default "nvs" partition
    esp_err_t ret = nvs_flash_init();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Error (%s) initializing NVS on default 'nvs' partition!", esp_err_to_name(ret));
        return;
    }

    // Write and read mock data
    ret = save_mock_data_to_namespace(MOCK_DATA_NAMESPACE);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Error (%s) saving mock data data to namespace '%s'!", esp_err_to_name(ret), MOCK_DATA_NAMESPACE);
    }

    ret = read_mock_data_from_namespace(MOCK_DATA_NAMESPACE);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Error (%s) reading back stored data from namespace '%s'!", esp_err_to_name(ret), MOCK_DATA_NAMESPACE);
    }

    // Write and read mock data "backup"
    ret = save_mock_data_to_namespace(MOCK_DATA_BACKUP_NAMESPACE);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Error (%s) saving mock data to namespace '%s'!", esp_err_to_name(ret), MOCK_DATA_BACKUP_NAMESPACE);
    }

    ret = read_mock_data_from_namespace(MOCK_DATA_BACKUP_NAMESPACE);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Error (%s) reading back stored data from namespace '%s'!", esp_err_to_name(ret), MOCK_DATA_BACKUP_NAMESPACE);
    }

#if CONFIG_EXAMPLE_RUN_OVERHEAD_MEASUREMENT
    run_overhead_measurement();
#endif

    ESP_LOGI(TAG, "Returning from app_main().");
}
