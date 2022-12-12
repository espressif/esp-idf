/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"
#include "esp_check.h"

#include "onewire_bus.h"
#include "ds18b20.h"

static const char *TAG = "example";

#define EXAMPLE_ONEWIRE_GPIO_PIN GPIO_NUM_5
#define EXAMPLE_ONEWIRE_MAX_DEVICES 5

void app_main(void)
{
    onewire_rmt_config_t config = {
        .gpio_pin = EXAMPLE_ONEWIRE_GPIO_PIN,
        .max_rx_bytes = 10, // 10 tx bytes(1byte ROM command + 8byte ROM number + 1byte device command)
    };

    // install new 1-wire bus
    onewire_bus_handle_t handle;
    ESP_ERROR_CHECK(onewire_new_bus_rmt(&config, &handle));
    ESP_LOGI(TAG, "1-wire bus installed");

    // create 1-wire rom search context
    onewire_rom_search_context_handler_t context_handler;
    ESP_ERROR_CHECK(onewire_rom_search_context_create(handle, &context_handler));

    uint8_t device_num = 0;
    uint8_t device_rom_id[EXAMPLE_ONEWIRE_MAX_DEVICES][8];

    // search for devices on the bus
    do {
        esp_err_t search_result = onewire_rom_search(context_handler);

        if (search_result == ESP_ERR_INVALID_CRC) {
            continue; // continue on crc error
        } else if (search_result == ESP_FAIL || search_result == ESP_ERR_NOT_FOUND) {
            break; // break on finish or no device
        }

        ESP_ERROR_CHECK(onewire_rom_get_number(context_handler, device_rom_id[device_num]));
        ESP_LOGI(TAG, "found device with rom id " ONEWIRE_ROM_ID_STR, ONEWIRE_ROM_ID(device_rom_id[device_num]));
        device_num ++;
    } while (device_num < EXAMPLE_ONEWIRE_MAX_DEVICES);

    // delete 1-wire rom search context
    ESP_ERROR_CHECK(onewire_rom_search_context_delete(context_handler));
    ESP_LOGI(TAG, "%d device%s found on 1-wire bus", device_num, device_num > 1 ? "s" : "");

    // convert and read temperature
    while (device_num > 0) {
        esp_err_t err;
        vTaskDelay(pdMS_TO_TICKS(200));

        // set all sensors' temperature conversion resolution
        err = ds18b20_set_resolution(handle, NULL, DS18B20_RESOLUTION_12B);
        if (err != ESP_OK) {
            continue;
        }

        // trigger all sensors to start temperature conversion
        err = ds18b20_trigger_temperature_conversion(handle, NULL); // skip rom to send command to all devices on the bus
        if (err != ESP_OK) {
            continue;
        }

        vTaskDelay(pdMS_TO_TICKS(800)); // 12-bit resolution needs 750ms to convert

        // get temperature from sensors
        for (uint8_t i = 0; i < device_num; i ++) {
            float temperature;
            err = ds18b20_get_temperature(handle, device_rom_id[i], &temperature); // read scratchpad and get temperature
            if (err != ESP_OK) {
                continue;
            }
            ESP_LOGI(TAG, "temperature of device " ONEWIRE_ROM_ID_STR ": %.2fC", ONEWIRE_ROM_ID(device_rom_id[i]), temperature);
        }
    }

    ESP_ERROR_CHECK(onewire_del_bus(handle));
    ESP_LOGI(TAG, "1-wire bus deleted");
}
