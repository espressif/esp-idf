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
#include "esp_log.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "esp_partition.h"

#define MOCK_DATA_NAMESPACE "_mock_data"
#define MOCK_DATA_BACKUP_NAMESPACE "_mock_backup"

/* NVS on-flash geometry. These are by-design constants of the NVS format (one
 * 4096-byte flash sector per page, 32-byte entries, 126 usable entries per
 * page). They are not exposed through the public API, so they are mirrored here
 * to allow precise control over page-level fragmentation below. */
#define NVS_ENTRY_SIZE              32
#define NVS_ENTRIES_PER_PAGE        126
#define NVS_PAGE_CHUNK_MAX_SIZE     (NVS_ENTRY_SIZE * (NVS_ENTRIES_PER_PAGE - 1)) // 4000 B

/* Namespace directory entries that live on the first NVS page before any
 * pre-population strings are written: one for the blob namespace and one for the
 * pre-population namespace. They are accounted for when filling page 0. */
#define FIRST_PAGE_NS_ENTRIES       2

#define PREP_NAMESPACE              "_prep"
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

// One measured combination of partition and blob size. The partition names must
// match NVS partitions declared in partitions.csv.
typedef struct {
    const char *partition_name;     // NVS partition to run the measurement on
    size_t blob_size;               // blob payload size in bytes
} measurement_combo_t;

// Partition size / blob size combinations to measure. Adjust freely.
static const measurement_combo_t measurement_combos[] = {
    { "nvs_16k", 128  },
    { "nvs_16k", 350  },
    { "nvs_16k", 416  },
    { "nvs_16k", 417  },
    { "nvs_32k", 768  },
    { "nvs_32k", 920  },
    { "nvs_64k", 1612 },
};

// A per-page free-space target, expressed as an absolute number of free
// (available) entries that must remain on a page after pre-population (out of
// NVS_ENTRIES_PER_PAGE per page). The first page of the partition keeps
// 'first_page_free' free entries and every remaining page keeps 'rest_free'.
typedef struct {
    unsigned first_page_free;    // free (available) entries left on the first NVS page
    unsigned rest_free;          // free (available) entries left on every remaining NVS page
} page_free_range_t;

// Per-page free-entry targets to measure (free entries out of 126 per page).
static const page_free_range_t page_free_ranges[] = {
    { 14, 11 },
    { 14, 7  },
    { 14, 4  },
    { 13, 4  },
};

// Results-table layout (see run_overhead_measurement()). Columns 2 and 3 ("First"
// and "Remaining") are grouped under a shared "Free Entries per Page" header.
#define TABLE_COL_COUNT 9
#define TABLE_GROUP_C0  2   // "First"
#define TABLE_GROUP_C1  3   // "Remaining"
static const int table_col_width[TABLE_COL_COUNT] = { 9, 14, 9, 13, 17, 16, 14, 16, 8 };

// Theoretical (ideal) number of entries consumed by a single blob stored in a
// non-fragmented partition:
//   1 BLOB_INDEX entry + 'chunks' chunk-header entries + ceil(size/32) data entries.
static size_t entries_per_blob_ideal(size_t blob_size)
{
    size_t chunks = (blob_size + NVS_PAGE_CHUNK_MAX_SIZE - 1) / NVS_PAGE_CHUNK_MAX_SIZE;
    if (chunks == 0) {
        chunks = 1;     // an empty blob still needs one (empty) data chunk
    }
    size_t data_entries = (blob_size + NVS_ENTRY_SIZE - 1) / NVS_ENTRY_SIZE;
    return 1 + chunks + data_entries;
}

// strlen (excluding the NUL terminator) of a string value occupying exactly
// 'span' NVS entries. A string uses 1 header entry + ceil((strlen + 1) / 32) data
// entries; choosing strlen = (span - 1) * 32 - 1 makes the data occupy exactly
// (span - 1) entries with no rounding slack.
static size_t string_len_for_span(size_t span)
{
    return (span - 1) * NVS_ENTRY_SIZE - 1;
}

// Create (and immediately close) a namespace so its single directory entry is
// written to flash on the currently active page.
static void create_namespace_entry(const char *partition_name, const char *namespace_name)
{
    nvs_handle_t handle;
    esp_err_t err = nvs_open_from_partition(partition_name, namespace_name, NVS_READWRITE, &handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error (%s) creating namespace '%s' on '%s'!", esp_err_to_name(err), namespace_name, partition_name);
        return;
    }
    nvs_close(handle);
}

// Live-entry target for a page that must keep 'free' entries available, clamped so
// that both the "keep" and the "filler" string stay writable.
static size_t page_target_from_free(unsigned free)
{
    long target = (long)NVS_ENTRIES_PER_PAGE - (long)free;
    if (target < (long)(FIRST_PAGE_NS_ENTRIES + 2)) {
        target = FIRST_PAGE_NS_ENTRIES + 2;                                  // keep the page-0 'keep' string writable
    }
    if (target > (long)(NVS_ENTRIES_PER_PAGE - 2)) {
        target = NVS_ENTRIES_PER_PAGE - 2;                                   // leave room for a >= 2-entry filler
    }
    return (size_t)target;
}

// Pre-populate every usable NVS page with string values, leaving 'first_page_free'
// free (available) entries on the first page and 'rest_free' on every remaining
// page (the namespace entries on page 0 count towards page 0's occupancy). The
// entries not meant to stay free are first filled with removable strings and then
// erased, so each page ends up FULL from the page allocator's point of view while
// still exposing the requested number of reclaimable free entries. This scatters
// the free space into per-page gaps - the fragmentation pattern a single blob
// write then has to cope with.
static void prepopulate_partition(const char *partition_name, unsigned first_page_free, unsigned rest_free)
{
    nvs_handle_t handle;
    esp_err_t err = nvs_open_from_partition(partition_name, PREP_NAMESPACE, NVS_READWRITE, &handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error (%s) opening pre-population handle on '%s'!", esp_err_to_name(err), partition_name);
        return;
    }

    // Live-entry targets derived from the requested free-entry counts.
    size_t first_target = page_target_from_free(first_page_free);
    size_t rest_target  = page_target_from_free(rest_free);

    // Size the reusable string buffer for the largest string span that can occur:
    // the largest "keep" string (highest live-entry target) or the largest "filler"
    // string (largest free gap), whichever is bigger.
    size_t max_target = (first_target > rest_target) ? first_target : rest_target;
    size_t min_target = (first_target < rest_target) ? first_target : rest_target;
    size_t max_span = max_target;
    size_t max_gap  = NVS_ENTRIES_PER_PAGE - min_target;
    if (max_gap > max_span) {
        max_span = max_gap;
    }

    char *buffer = malloc(string_len_for_span(max_span) + 1);
    if (buffer == NULL) {
        ESP_LOGE(TAG, "Failed to allocate pre-population buffer!");
        nvs_close(handle);
        return;
    }
    memset(buffer, 'A', string_len_for_span(max_span));

    unsigned page = 0;
    while (true) {
        char key[16];

        // Live-entry target for this page: first_target on page 0, rest_target on all others.
        size_t target = (page == 0) ? first_target : rest_target;
        size_t free_gap = NVS_ENTRIES_PER_PAGE - target;

        // On page 0 the two namespace entries already occupy part of the target share.
        size_t keep_span = (page == 0) ? (target - FIRST_PAGE_NS_ENTRIES) : target;
        size_t keep_len = string_len_for_span(keep_span);
        buffer[keep_len] = '\0';
        snprintf(key, sizeof(key), "k%05u", page);
        err = nvs_set_str(handle, key, buffer);
        buffer[keep_len] = 'A';
        if (err == ESP_ERR_NVS_NOT_ENOUGH_SPACE) {
            break;                                                           // partition full (one page kept in reserve)
        }
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "Error (%s) writing pre-population string!", esp_err_to_name(err));
            break;
        }

        // Fill the rest of this page with a removable string, marking the page FULL.
        size_t fill_len = string_len_for_span(free_gap);
        buffer[fill_len] = '\0';
        snprintf(key, sizeof(key), "x%05u", page);
        err = nvs_set_str(handle, key, buffer);
        buffer[fill_len] = 'A';
        if (err != ESP_OK) {
            break;
        }
        nvs_commit(handle);
        page++;
    }

    // Erase every filler, turning the completely full pages into pages that keep
    // 'target' live entries and expose 'free_gap' reclaimable entries each.
    for (unsigned i = 0; i < page; i++) {
        char key[16];
        snprintf(key, sizeof(key), "x%05u", i);
        nvs_erase_key(handle, key);
    }
    nvs_commit(handle);

    free(buffer);
    nvs_close(handle);
}

// Print one horizontal separator line with a '+' at every column boundary.
static void print_table_separator(void)
{
    putchar('+');
    for (int c = 0; c < TABLE_COL_COUNT; c++) {
        for (int i = 0; i < table_col_width[c] + 2; i++) {
            putchar('-');
        }
        putchar('+');
    }
    putchar('\n');
}

// Print the top border of the header: like print_table_separator(), but the two
// grouped columns are merged into a single segment (no boundary between them).
static void print_table_group_top(void)
{
    for (int c = 0; c < TABLE_COL_COUNT; c++) {
        putchar((c == TABLE_GROUP_C1) ? '-' : '+');
        for (int i = 0; i < table_col_width[c] + 2; i++) {
            putchar('-');
        }
    }
    putchar('+');
    putchar('\n');
}

// Print the divider between the group label and its sub-labels: the grouped
// columns are split with a dashed rule, every other column stays blank.
static void print_table_group_divider(void)
{
    for (int c = 0; c < TABLE_COL_COUNT; c++) {
        bool group_edge = (c == TABLE_GROUP_C0 || c == TABLE_GROUP_C1 || c == TABLE_GROUP_C1 + 1);
        char fill = (c == TABLE_GROUP_C0 || c == TABLE_GROUP_C1) ? '-' : ' ';
        putchar(group_edge ? '+' : '|');
        for (int i = 0; i < table_col_width[c] + 2; i++) {
            putchar(fill);
        }
    }
    putchar('|');
    putchar('\n');
}

// Print one table row from TABLE_COL_COUNT right-aligned cell strings.
static void print_table_row(const char *cells[TABLE_COL_COUNT])
{
    putchar('|');
    for (int c = 0; c < TABLE_COL_COUNT; c++) {
        printf(" %*s |", table_col_width[c], cells[c]);
    }
    putchar('\n');
}

// Print a string centered within a 'width'-character field.
static void print_centered(const char *s, int width)
{
    int len = (int)strlen(s);
    if (len > width) {
        len = width;
    }
    int left = (width - len) / 2;
    int right = width - len - left;
    printf("%*s%.*s%*s", left, "", len, s, right, "");
}

// Print the two-line header. The first line carries the column names (with "Free
// Entries per Page" centered above the two grouped columns) and the second line
// carries the unit of measurement of each column (and, for the grouped columns,
// their "First" / "Remaining" sub-labels which keep their unit inline).
static void print_table_header(void)
{
    // Row 1: column names (units moved to row 2 below).
    static const char *names[TABLE_COL_COUNT] = {
        "Blob Size", "Partition Size", "Free Entries per Page", "",
        "Available Entries", "Expected Entries", "Actual Entries",
        "Overhead Entries", "Overhead"
    };
    // Row 2: units of measurement; the grouped columns keep their sub-labels here.
    static const char *units[TABLE_COL_COUNT] = {
        "[B]", "[k]", "First [-]", "Remaining [-]",
        "[-]", "[-]", "[-]", "[-]", "[%]"
    };

    print_table_group_top();

    // Row 1: column names, with the group name centered above the two grouped columns.
    putchar('|');
    for (int c = 0; c < TABLE_COL_COUNT; c++) {
        if (c == TABLE_GROUP_C0) {
            putchar(' ');
            print_centered(names[TABLE_GROUP_C0], table_col_width[TABLE_GROUP_C0] + table_col_width[TABLE_GROUP_C1] + 3);
            printf(" |");
            c = TABLE_GROUP_C1;     // the second grouped column is covered by the merged name
        } else {
            printf(" %*s |", table_col_width[c], names[c]);
        }
    }
    putchar('\n');

    print_table_group_divider();

    // Row 2: units of measurement (grouped columns split into their sub-labels).
    print_table_row(units);

    print_table_separator();
}

// Measure the real vs. ideal blob entry consumption for one combination and one
// per-page free-entry target, then print the corresponding results-table row.
static void measure_blob_overhead(const measurement_combo_t *combo, page_free_range_t page_free)
{
    const char *part = combo->partition_name;

    ESP_ERROR_CHECK(nvs_flash_erase_partition(part));
    ESP_ERROR_CHECK(nvs_flash_init_partition(part));

    unsigned size_kb = 0;
    const esp_partition_t *p = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_NVS, part);
    if (p != NULL) {
        size_kb = (unsigned)(p->size / 1024);
    }

    // Create the blob namespace up-front so its entry is part of the population.
    create_namespace_entry(part, BLOB_NAMESPACE);

    // Leave the requested number of free entries on the first page and remaining pages.
    prepopulate_partition(part, page_free.first_page_free, page_free.rest_free);

    // Available entries reported right before the blob is written.
    nvs_stats_t stats_before;
    ESP_ERROR_CHECK(nvs_get_stats(part, &stats_before));

    // Attempt to write a single blob and measure its real entry footprint.
    bool stored = false;
    size_t actual_entries = 0;
    nvs_handle_t handle;
    ESP_ERROR_CHECK(nvs_open_from_partition(part, BLOB_NAMESPACE, NVS_READWRITE, &handle));
    uint8_t *blob = malloc(combo->blob_size);
    if (blob != NULL) {
        memset(blob, 0x5A, combo->blob_size);
        esp_err_t err = nvs_set_blob(handle, "the_blob", blob, combo->blob_size);
        if (err == ESP_OK) {
            err = nvs_commit(handle);
        }
        free(blob);
        if (err == ESP_OK) {
            nvs_stats_t stats_after;
            ESP_ERROR_CHECK(nvs_get_stats(part, &stats_after));
            actual_entries = stats_after.used_entries - stats_before.used_entries;
            stored = true;
        }
        // A failing write (e.g. ESP_ERR_NVS_NOT_ENOUGH_SPACE) is an expected outcome
        // for tightly populated partitions and is reported as "FAIL" in the table below.
    } else {
        ESP_LOGE(TAG, "Failed to allocate %u B blob buffer!", (unsigned)combo->blob_size);
    }
    nvs_close(handle);

    size_t expected_entries = entries_per_blob_ideal(combo->blob_size);

    char c_blob[16], c_part[16], c_first[16], c_rest[16], c_avail[16], c_exp[16], c_act[16], c_ovh[16], c_ovhpct[16];
    snprintf(c_blob,  sizeof(c_blob),  "%u", (unsigned)combo->blob_size);
    snprintf(c_part,  sizeof(c_part),  "%u", size_kb);
    snprintf(c_first, sizeof(c_first), "%u", page_free.first_page_free);
    snprintf(c_rest,  sizeof(c_rest),  "%u", page_free.rest_free);
    snprintf(c_avail, sizeof(c_avail), "%u", (unsigned)stats_before.available_entries);
    snprintf(c_exp,   sizeof(c_exp),   "%u", (unsigned)expected_entries);
    if (stored) {
        long overhead_entries = (long)actual_entries - (long)expected_entries;
        double overhead_pct = (expected_entries != 0) ? (100.0 * (double)overhead_entries / (double)expected_entries) : 0.0;
        snprintf(c_act,    sizeof(c_act),    "%u", (unsigned)actual_entries);
        snprintf(c_ovh,    sizeof(c_ovh),    "%ld", overhead_entries);
        snprintf(c_ovhpct, sizeof(c_ovhpct), "%.1f", overhead_pct);
    } else {
        snprintf(c_act,    sizeof(c_act),    "%s", "FAIL");
        snprintf(c_ovh,    sizeof(c_ovh),    "%s", "-");
        snprintf(c_ovhpct, sizeof(c_ovhpct), "%s", "-");
    }

    const char *cells[TABLE_COL_COUNT] = { c_blob, c_part, c_first, c_rest, c_avail, c_exp, c_act, c_ovh, c_ovhpct };
    print_table_row(cells);

    ESP_ERROR_CHECK(nvs_flash_deinit_partition(part));
}

static void run_overhead_measurement(void)
{
    const size_t combo_count = sizeof(measurement_combos) / sizeof(measurement_combos[0]);
    const size_t free_range_count = sizeof(page_free_ranges) / sizeof(page_free_ranges[0]);

    ESP_LOGI(TAG, "Starting NVS blob storage-overhead measurement...");

    printf("\n");
    print_table_header();

    for (size_t i = 0; i < combo_count; i++) {
        for (size_t j = 0; j < free_range_count; j++) {
            measure_blob_overhead(&measurement_combos[i], page_free_ranges[j]);
        }
    }

    print_table_separator();
    printf("\n");

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
