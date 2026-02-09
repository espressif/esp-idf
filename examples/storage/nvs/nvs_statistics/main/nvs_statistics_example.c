/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
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
#include <inttypes.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_check.h"
#include "esp_system.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "nvs.h"

#define MOCK_DATA_NAMESPACE "_mock_data"
#define MOCK_DATA_BACKUP_NAMESPACE "_mock_backup"

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

    ESP_LOGI(TAG, "Returning from app_main().");
}
