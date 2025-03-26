/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
/* Non-Volatile Storage (NVS) Read and Write a Blob - Example

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
#include "driver/gpio.h"

#define STORAGE_NAMESPACE "storage"

static const char *TAG = "nvs_blob_example";

/* Test data structure to demonstrate different data types in blob */
typedef struct {
    uint8_t id;
    char name[32];
    float values[2];
    uint32_t flags;
    int16_t counts[2];
    bool active;
} test_data_t;

/* Save test data as a blob in NVS */
esp_err_t save_test_data(void)
{
    nvs_handle_t my_handle;
    esp_err_t err;

    // Create sample test data
    test_data_t test_data = {
        .id = 123,
        .name = "Test Sample",
        .values = {3.14f, 2.718f},
        .flags = 0xABCD1234,
        .counts = {-100, 100},
        .active = true
    };

    // Open NVS handle
    err = nvs_open(STORAGE_NAMESPACE, NVS_READWRITE, &my_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error (%s) opening NVS handle!", esp_err_to_name(err));
        return err;
    }

    // Write blob
    ESP_LOGI(TAG, "Saving test data blob...");
    err = nvs_set_blob(my_handle, "test_data", &test_data, sizeof(test_data_t));
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to write test data blob!");
        nvs_close(my_handle);
        return err;
    }

    // Commit
    err = nvs_commit(my_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to commit data");
    }

    nvs_close(my_handle);
    return err;
}

/* Example of storing and appending array data as a blob */
esp_err_t save_array_data(void)
{
    nvs_handle_t my_handle;
    esp_err_t err;

    // Open
    err = nvs_open(STORAGE_NAMESPACE, NVS_READWRITE, &my_handle);
    if (err != ESP_OK) return err;

    // First, get the size of existing data (if any)
    size_t required_size = 0;
    err = nvs_get_blob(my_handle, "array_data", NULL, &required_size);
    if (err != ESP_OK && err != ESP_ERR_NVS_NOT_FOUND) {
        ESP_LOGE(TAG, "Error (%s) reading array size!", esp_err_to_name(err));
        nvs_close(my_handle);
        return err;
    }

    // Allocate memory and read existing data
    uint32_t* array_data = malloc(required_size + sizeof(uint32_t));
    if (required_size > 0) {
        err = nvs_get_blob(my_handle, "array_data", array_data, &required_size);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "Error (%s) reading array data!", esp_err_to_name(err));
            free(array_data);
            nvs_close(my_handle);
            return err;
        }
    }

    // Append new value
    required_size += sizeof(uint32_t);
    array_data[required_size / sizeof(uint32_t) - 1] = xTaskGetTickCount() * portTICK_PERIOD_MS;

    // Save updated array
    err = nvs_set_blob(my_handle, "array_data", array_data, required_size);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error (%s) saving array data!", esp_err_to_name(err));
    }

    free(array_data);

    // Commit
    err = nvs_commit(my_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error (%s) committing data!", esp_err_to_name(err));
    }

    nvs_close(my_handle);
    return err;
}

/* Read and display all saved blobs */
esp_err_t read_stored_blobs(void)
{
    nvs_handle_t my_handle;
    esp_err_t err;

    err = nvs_open(STORAGE_NAMESPACE, NVS_READONLY, &my_handle);
    if (err != ESP_OK) return err;

    // 1. Read test data blob
    ESP_LOGI(TAG, "Reading test data blob:");
    test_data_t test_data;
    size_t test_data_size = sizeof(test_data_t);
    err = nvs_get_blob(my_handle, "test_data", &test_data, &test_data_size);
    if (err == ESP_OK) {
        ESP_LOGI(TAG, "ID: %d", test_data.id);
        ESP_LOGI(TAG, "Name: %s", test_data.name);
        ESP_LOGI(TAG, "Values: %.3f, %.3f, %.3f, %.3f",
                 test_data.values[0], test_data.values[1],
                 test_data.values[2], test_data.values[3]);
        ESP_LOGI(TAG, "Flags: 0x%08" PRIX32, test_data.flags);
        ESP_LOGI(TAG, "Counts: %d, %d", test_data.counts[0], test_data.counts[1]);
        ESP_LOGI(TAG, "Active: %s", test_data.active ? "true" : "false");
    } else if (err == ESP_ERR_NVS_NOT_FOUND) {
        ESP_LOGW(TAG, "Test data not found!");
    }

    // 2. Read array data blob
    ESP_LOGI(TAG, "\nReading array data blob:");
    size_t required_size = 0;
    err = nvs_get_blob(my_handle, "array_data", NULL, &required_size);
    if (err == ESP_OK) {
        uint32_t* array_data = malloc(required_size);
        err = nvs_get_blob(my_handle, "array_data", array_data, &required_size);
        if (err == ESP_OK) {
            for (int i = 0; i < required_size / sizeof(uint32_t); i++) {
                ESP_LOGI(TAG, "Array[%d] = %" PRIu32, i, array_data[i]);
            }
        }
        free(array_data);
    } else if (err == ESP_ERR_NVS_NOT_FOUND) {
        ESP_LOGW(TAG, "Array data not found!");
    }

    nvs_close(my_handle);
    return ESP_OK;
}

void app_main(void)
{
    // Initialize NVS
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK(err);

    // Save new test data
    err = save_test_data();
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error (%s) saving test data!", esp_err_to_name(err));
    }

    // Save new array data
    err = save_array_data();
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error (%s) saving array data!", esp_err_to_name(err));
    }

    // Read updated data
    ESP_LOGI(TAG, "\nReading updated blob data:");
    err = read_stored_blobs();
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error (%s) reading updated data!", esp_err_to_name(err));
    }

    ESP_LOGI(TAG, "\nBlob operations completed.");
}
