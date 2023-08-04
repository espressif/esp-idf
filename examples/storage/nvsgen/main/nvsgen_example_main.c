/* Non-Volatile Storage (NVS) Image Generation on Build Example
 *
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include <stdio.h>
#include <inttypes.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "nvs.h"

static const char *TAG = "example";

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

    // Open the pre-filled NVS partition called "nvs"
    ESP_LOGI(TAG, "Opening Non-Volatile Storage (NVS) handle");
    nvs_handle_t my_handle;
    err = nvs_open_from_partition("nvs", "storage", NVS_READONLY, &my_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error (%s) opening NVS handle!\n", esp_err_to_name(err));
        return;
    }
    ESP_LOGI(TAG, "The NVS handle successfully opened");

    // Read values
    ESP_LOGI(TAG, "Reading values from NVS");

    uint8_t u8_val = 0;
    err = nvs_get_u8(my_handle, "u8_key", &u8_val);
    ESP_ERROR_CHECK(err);
    printf("%"PRIu8"\n", u8_val);
    assert(u8_val == 255);

    int8_t i8_val = 0;
    err = nvs_get_i8(my_handle, "i8_key", &i8_val);
    ESP_ERROR_CHECK(err);
    printf("%"PRIi8"\n", i8_val);
    assert(i8_val == -128);

    uint16_t u16_val = 0;
    err = nvs_get_u16(my_handle, "u16_key", &u16_val);
    ESP_ERROR_CHECK(err);
    printf("%"PRIu16"\n", u16_val);
    assert(u16_val == 65535);

    uint32_t u32_val = 0;
    err = nvs_get_u32(my_handle, "u32_key", &u32_val);
    ESP_ERROR_CHECK(err);
    printf("%"PRIu32"\n", u32_val);
    assert(u32_val == 4294967295);

    int32_t i32_val = 0;
    err = nvs_get_i32(my_handle, "i32_key", &i32_val);
    ESP_ERROR_CHECK(err);
    printf("%"PRIi32"\n", i32_val);
    assert(i32_val == -2147483648);

    size_t str_len = 0;
    err = nvs_get_str(my_handle, "str_key", NULL, &str_len);
    ESP_ERROR_CHECK(err);
    assert(str_len == 222);

    char* str_val = (char*) malloc(str_len);
    err = nvs_get_str(my_handle, "str_key", str_val, &str_len);
    ESP_ERROR_CHECK(err);
    printf("%s\n", str_val);
    assert(str_val[0] == 'L');
    free(str_val);

    // Close
    nvs_close(my_handle);
    ESP_LOGI(TAG, "Reading values from NVS done - all OK");
}
