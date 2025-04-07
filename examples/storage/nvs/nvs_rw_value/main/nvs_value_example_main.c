/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
/* Non-Volatile Storage (NVS) Read and Write a Value - Example

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
#include "esp_system.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "nvs.h"

static const char *TAG = "nvs_example";

typedef struct {
    nvs_type_t type;
    const char *str;
} type_str_pair_t;

static const type_str_pair_t type_str_pair[] = {
    { NVS_TYPE_I8, "i8" },
    { NVS_TYPE_U8, "u8" },
    { NVS_TYPE_U16, "u16" },
    { NVS_TYPE_I16, "i16" },
    { NVS_TYPE_U32, "u32" },
    { NVS_TYPE_I32, "i32" },
    { NVS_TYPE_U64, "u64" },
    { NVS_TYPE_I64, "i64" },
    { NVS_TYPE_STR, "str" },
    { NVS_TYPE_BLOB, "blob" },
    { NVS_TYPE_ANY, "any" },
};

static const size_t TYPE_STR_PAIR_SIZE = sizeof(type_str_pair) / sizeof(type_str_pair[0]);

static const char *type_to_str(nvs_type_t type)
{
    for (int i = 0; i < TYPE_STR_PAIR_SIZE; i++) {
        const type_str_pair_t *p = &type_str_pair[i];
        if (p->type == type) {
            return  p->str;
        }
    }

    return "Unknown";
}

void app_main(void)
{
    // Initialize NVS
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        // NVS partition was truncated and needs to be erased
        // Retry nvs_flash_init
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK(err);

    // Open NVS handle
    ESP_LOGI(TAG, "\nOpening Non-Volatile Storage (NVS) handle...");
    nvs_handle_t my_handle;
    err = nvs_open("storage", NVS_READWRITE, &my_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error (%s) opening NVS handle!", esp_err_to_name(err));
        return;
    }

    // Store and read an integer value
    int32_t counter = 42;
    ESP_LOGI(TAG, "\nWriting counter to NVS...");
    err = nvs_set_i32(my_handle, "counter", counter);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to write counter!");
    }

    // Read back the value
    int32_t read_counter = 0;
    ESP_LOGI(TAG, "\nReading counter from NVS...");
    err = nvs_get_i32(my_handle, "counter", &read_counter);
    switch (err) {
        case ESP_OK:
            ESP_LOGI(TAG, "Read counter = %" PRIu32, read_counter);
            break;
        case ESP_ERR_NVS_NOT_FOUND:
            ESP_LOGW(TAG, "The value is not initialized yet!");
            break;
        default:
            ESP_LOGE(TAG, "Error (%s) reading!", esp_err_to_name(err));
    }

    // Store and read a string
    ESP_LOGI(TAG, "\nWriting string to NVS...");
    err = nvs_set_str(my_handle, "message", "Hello from NVS!");
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to write string!");
    }

    // Read back the string
    size_t required_size = 0;
    ESP_LOGI(TAG, "\nReading string from NVS...");
    err = nvs_get_str(my_handle, "message", NULL, &required_size);
    if (err == ESP_OK) {
        char* message = malloc(required_size);
        err = nvs_get_str(my_handle, "message", message, &required_size);
        if (err == ESP_OK) {
            ESP_LOGI(TAG, "Read string: %s", message);
        }
        free(message);
    }

    // Find keys in NVS
    ESP_LOGI(TAG, "\nFinding keys in NVS...");
    nvs_iterator_t it = NULL;
    esp_err_t res = nvs_entry_find("nvs", "storage", NVS_TYPE_ANY, &it);
    while(res == ESP_OK) {
        nvs_entry_info_t info;
        nvs_entry_info(it, &info);
        const char *type_str =  type_to_str(info.type);
        ESP_LOGI(TAG, "Key: '%s', Type: %s", info.key, type_str);
        res = nvs_entry_next(&it);
    }
    nvs_release_iterator(it);

    // Delete a key from NVS
    ESP_LOGI(TAG, "\nDeleting key from NVS...");
    err = nvs_erase_key(my_handle, "counter");
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to erase key!");
    }

    // Commit changes
    // After setting any values, nvs_commit() must be called to ensure changes are written
    // to flash storage. Implementations may write to storage at other times,
    // but this is not guaranteed.
    ESP_LOGI(TAG, "\nCommitting updates in NVS...");
    err = nvs_commit(my_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to commit NVS changes!");
    }

    // Close
    nvs_close(my_handle);
    ESP_LOGI(TAG, "NVS handle closed.");

    ESP_LOGI(TAG, "Returned to app_main");

}
