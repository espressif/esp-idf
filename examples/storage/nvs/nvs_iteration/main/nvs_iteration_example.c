/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
/* Non-Volatile Storage (NVS) Iteration Example

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

static const char *TAG = "nvs_iteration_example";

static const char *u32_mock_data_keys[] = {
    "system_startup",
    "user_login",
    "data_backup",
    "sched_update",
    "db_cleanup",
    "security_scan",
    "system_shutdown"
};
// Unix timestamps
static const uint32_t u32_mock_data_values[] = {
    1704067200,
    1704067260,
    1704067320,
    1704067440,
    1704067500,
    1704067560,
    1704067620
};

static const size_t u32_mock_data_count = sizeof(u32_mock_data_keys) / sizeof(u32_mock_data_keys[0]);

static const char *i8_mock_data_keys[] = {
    "temperature",
    "humidity",
    "light_level",
    "motion_detected",
    "battery_level",
    "signal_strength",
    "error_code"
};

static const int8_t i8_mock_data_values[] = {
    22,
    45,
    -5,
    1,
    95,
    -10,
    -42
};

static const size_t i8_mock_data_count = sizeof(i8_mock_data_keys) / sizeof(i8_mock_data_keys[0]);

static esp_err_t save_mock_data()
{

    ESP_LOGI(TAG, "Opening Non-Volatile Storage (NVS) handle for namespace '%s'...", MOCK_DATA_NAMESPACE);
    nvs_handle_t my_handle;
    esp_err_t ret = nvs_open(MOCK_DATA_NAMESPACE, NVS_READWRITE, &my_handle);
    ESP_RETURN_ON_ERROR(ret, TAG, "Error (%s) opening NVS handle for namespace '%s'!", esp_err_to_name(ret), MOCK_DATA_NAMESPACE);

    size_t successfully_written_entries = 0;

    ESP_LOGI(TAG, "Writing u32 mock data key-value pairs to NVS namespace '%s'...", MOCK_DATA_NAMESPACE);
    for (int i = 0; i < u32_mock_data_count; i++) {
        // Write uint32_t values
        ret = nvs_set_u32(my_handle, u32_mock_data_keys[i], u32_mock_data_values[i]);
        // Don't break/abort on error, try to continue writing
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "Error (%s) writing '%s':'%u' pair!", esp_err_to_name(ret), u32_mock_data_keys[i], u32_mock_data_values[i]);
        } else {
            successfully_written_entries++;
        }
    }
    ESP_LOGI(TAG, "Wrote %u u32 mock data key-value pairs to NVS namespace '%s'.", successfully_written_entries, MOCK_DATA_NAMESPACE);

    successfully_written_entries = 0;

    ESP_LOGI(TAG, "Writing i8 mock data key-value pairs to NVS namespace '%s'...", MOCK_DATA_NAMESPACE);
    for (int i = 0; i < i8_mock_data_count; i++) {
        // Write int8_t values
        ret = nvs_set_i8(my_handle, i8_mock_data_keys[i], i8_mock_data_values[i]);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "Error (%s) writing '%s':'%d' pair!", esp_err_to_name(ret), i8_mock_data_keys[i], i8_mock_data_values[i]);
        } else {
            successfully_written_entries++;
        }
    }
    ESP_LOGI(TAG, "Wrote %u i8 mock data key-value pairs to NVS namespace '%s'.", successfully_written_entries, MOCK_DATA_NAMESPACE);

    ESP_LOGI(TAG, "Committing data in NVS namespace '%s'...", MOCK_DATA_NAMESPACE);
    ret = nvs_commit(my_handle);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Error (%s) committing data for namespace '%s'!", esp_err_to_name(ret), MOCK_DATA_NAMESPACE);
    }

    // Close the storage handle, freeing allocated resources
    nvs_close(my_handle);
    ESP_LOGI(TAG, "NVS handle for namespace '%s' closed.", MOCK_DATA_NAMESPACE);

    return ESP_OK;
}

static esp_err_t iterate_over_mock_data()
{
    ESP_LOGI(TAG, "Iterating over NVS_TYPE_U32 data in namespace '%s'...", MOCK_DATA_NAMESPACE);
    nvs_iterator_t it = NULL;
    size_t entries_iterated = 0;
    // Find only entries of type NVS_TYPE_U32 and iterate over them
    esp_err_t ret = nvs_entry_find(NVS_DEFAULT_PART_NAME, MOCK_DATA_NAMESPACE, NVS_TYPE_U32, &it);
    while (ret == ESP_OK) {
        nvs_entry_info_t info;
        ret = nvs_entry_info(it, &info);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "Error (%s) obtaining entry info in namespace '%s'!", esp_err_to_name(ret), MOCK_DATA_NAMESPACE);
        } else {
            ESP_LOGI(TAG, "Entry info: key '%s' for data type type %s in namespace '%s'", info.key, info.type == NVS_TYPE_U32 ? "NVS_TYPE_U32" : "Error: not the expected NVS_TYPE_U32!", info.namespace_name);
        }
        entries_iterated++;
        // ret gets checked in the while() condition, terminating the loop if it's not ESP_OK
        ret = nvs_entry_next(&it);
        // ESP_ERR_NVS_NOT_FOUND is expected to be returned when there is no next entry to iterate over (valid return value for last entry)
        if (ret != ESP_OK && ret != ESP_ERR_NVS_NOT_FOUND) {
            ESP_LOGE(TAG, "Error (%s) obtaining next entry in namespace '%s'!", esp_err_to_name(ret), MOCK_DATA_NAMESPACE);
            break;
        }
    }
    nvs_release_iterator(it);
    ESP_LOGI(TAG, "Iterated over %u entries.", entries_iterated);

    ESP_LOGI(TAG, "Iterating over NVS_TYPE_I8 data in namespace '%s'...", MOCK_DATA_NAMESPACE);
    it = NULL;
    entries_iterated = 0;
    // Find only entries of type NVS_TYPE_I8 and iterate over them
    ret = nvs_entry_find(NVS_DEFAULT_PART_NAME, MOCK_DATA_NAMESPACE, NVS_TYPE_I8, &it);
    while (ret == ESP_OK) {
        nvs_entry_info_t info;
        ret = nvs_entry_info(it, &info);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "Error (%s) obtaining entry info in namespace '%s'!", esp_err_to_name(ret), MOCK_DATA_NAMESPACE);
        } else {
            ESP_LOGI(TAG, "Entry info: key '%s' for data type type %s in namespace '%s'", info.key, info.type == NVS_TYPE_I8 ? "NVS_TYPE_I8" : "Error: not the expected NVS_TYPE_I8!", info.namespace_name);
        }
        entries_iterated++;
        ret = nvs_entry_next(&it);
        if (ret != ESP_OK && ret != ESP_ERR_NVS_NOT_FOUND) {
            ESP_LOGE(TAG, "Error (%s) obtaining next entry in namespace '%s'!", esp_err_to_name(ret), MOCK_DATA_NAMESPACE);
            break;
        }
    }

    nvs_release_iterator(it);
    ESP_LOGI(TAG, "Iterated over %u entries.", entries_iterated);

    ESP_LOGI(TAG, "Iterating over NVS_TYPE_ANY data in namespace '%s'...", MOCK_DATA_NAMESPACE);
    it = NULL;
    entries_iterated = 0;
    // Iterate over NVS_TYPE_ANY entries only to count them
    ret = nvs_entry_find(NVS_DEFAULT_PART_NAME, MOCK_DATA_NAMESPACE, NVS_TYPE_ANY, &it);
    while (ret == ESP_OK) {
        entries_iterated++;
        ret = nvs_entry_next(&it);
        if (ret != ESP_OK && ret != ESP_ERR_NVS_NOT_FOUND) {
            ESP_LOGE(TAG, "Error (%s) obtaining next entry in namespace '%s'!", esp_err_to_name(ret), MOCK_DATA_NAMESPACE);
            break;
        }
    }
    nvs_release_iterator(it);
    ESP_LOGI(TAG, "Iterated over %u entries.", entries_iterated);

    return ESP_OK;
}

void app_main(void)
{
    ESP_LOGI(TAG, "Erasing the contents of the default NVS partition...");
    ESP_ERROR_CHECK(nvs_flash_erase());

    // Initialize NVS on default "nvs" partition
    esp_err_t ret = nvs_flash_init();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Error (%s) initializing NVS on default 'nvs' partition!", esp_err_to_name(ret));
        return;
    }

    // Write mock data
    ret = save_mock_data();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Error (%s) saving mock data to namespace '%s'!", esp_err_to_name(ret), MOCK_DATA_NAMESPACE);
    }

    // Iterate over mock data
    ret = iterate_over_mock_data();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Error (%s) iterating over namespace '%s'!", esp_err_to_name(ret), MOCK_DATA_NAMESPACE);
    }

    ESP_LOGI(TAG, "Returning from app_main().");
}
