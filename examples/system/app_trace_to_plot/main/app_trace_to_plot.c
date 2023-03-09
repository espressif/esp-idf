/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
/* Application Trace to Plot Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_app_trace.h"
#include "esp_log.h"
#include <math.h>
#include <limits.h>

#define CYCLE_PERIOD 51
#define STX "esp32"
#define STX_LENGTH 5
#define ETX 0x03
#define MAX_PACKAGE_SIZE 25

static const char *TAG = "example";

typedef struct {
    uint32_t id;
    uint32_t timestamp;
} __attribute__((packed)) package_header_t;

typedef struct {
    package_header_t header;
    int8_t value;
} __attribute__((packed)) sensor_data_pkt_t;

typedef struct {
    package_header_t header;
    int16_t value;
} __attribute__((packed)) sensor2_data_pkt_t;

typedef struct {
    package_header_t header;
    int32_t value;
} __attribute__((packed)) sensor3_data_pkt_t;

typedef struct {
    package_header_t header;
    int64_t value;
} __attribute__((packed)) sensor4_data_pkt_t;

int16_t get_engine_temperature(int cnt)
{
    /* A healhty engine should work between 90 to 105 degree Celcius */
    const int32_t temp_arr[] = {1, 1, 2, 3, 4, 6, 8, 10, 13, 16, 19, 24, 29, 34, 40, 47, 53,
        60, 68, 75, 82, 88, 94, 99, 103, 105, 107, 107, 106, 104, 101, 96, 91, 85, 78, 71, 64,
        57, 50, 43, 37, 31, 26, 21, 17, 14, 11, 9, 7, 5, 4, 3};
    return temp_arr[cnt];
}

int8_t get_outside_temperature(int cnt)
{
    /* Recorded hightest temperature was around 57 degree and lowest was -89 degree Celcius */
    return (rand() % 146) - 89;
}

int32_t get_altitude(void)
{
    /* Very high altidude is around 4000 meter */
    return rand() % 4000;
}

int32_t get_pressure(int cnt)
{
    /* Highest pressure recorded around 1.084b and lowest was 0.870b on earth */
    return (int)(((sin(cnt) / 10) + 1) * 100);
}

void app_main(void)
{
    ESP_LOGI(TAG, "Waiting for OpenOCD connection");
    while (!esp_apptrace_host_is_connected(ESP_APPTRACE_DEST_JTAG)) {
        vTaskDelay(1);
    }

    ESP_LOGI(TAG, "Sending data to the host...");
    uint32_t cnt = 0;
    uint8_t buf[MAX_PACKAGE_SIZE] = {0};
    sensor_data_pkt_t sensor;
    sensor2_data_pkt_t sensor2;
    sensor3_data_pkt_t sensor3;
    sensor4_data_pkt_t sensor4;

    memcpy(buf, (uint8_t *)&STX, STX_LENGTH);

    while (esp_apptrace_host_is_connected(ESP_APPTRACE_DEST_JTAG)) {
        sensor.header.id = 1;
        sensor.header.timestamp = esp_log_timestamp();
        sensor.value = get_outside_temperature(cnt);

        buf[STX_LENGTH] = sizeof(sensor);
        memcpy(buf + STX_LENGTH + 1, (uint8_t *)&sensor, sizeof(sensor));
        buf[STX_LENGTH + 1 + sizeof(sensor)] = ETX;
        esp_err_t res = esp_apptrace_write(ESP_APPTRACE_DEST_JTAG, buf, sizeof(sensor) + STX_LENGTH + 1/*sensor pkt len*/ + 1/*ETX len*/, ESP_APPTRACE_TMO_INFINITE);
        if (res != ESP_OK) {
            ESP_LOGE(TAG, "Failed to write data to host [0x%x] (%s)", res, esp_err_to_name(res));
            break;
        }
        /* Delay added for represent the data better on the plot */
        vTaskDelay(10 / portTICK_PERIOD_MS);

        sensor2.header.id = 2;
        sensor2.header.timestamp = esp_log_timestamp();
        sensor2.value = get_engine_temperature(cnt);

        buf[STX_LENGTH] = sizeof(sensor2);
        memcpy(buf + STX_LENGTH + 1, (uint8_t *)&sensor2, sizeof(sensor2));
        buf[STX_LENGTH + 1 + sizeof(sensor2)] = ETX;
        res = esp_apptrace_write(ESP_APPTRACE_DEST_JTAG, buf, sizeof(sensor2) + STX_LENGTH + 1/*sensor pkt len*/ + 1/*ETX len*/, ESP_APPTRACE_TMO_INFINITE);
        if (res != ESP_OK) {
            ESP_LOGE(TAG, "Failed to write data to host [0x%x] (%s)", res, esp_err_to_name(res));
            break;
        }
        vTaskDelay(10 / portTICK_PERIOD_MS);

        if (cnt % 2 == 0) {
            sensor3.header.id = 3;
            sensor3.header.timestamp = esp_log_timestamp();
            sensor3.value = get_altitude();

            buf[STX_LENGTH] = sizeof(sensor3);
            memcpy(buf + STX_LENGTH + 1, (uint8_t *)&sensor3, sizeof(sensor3));
            buf[STX_LENGTH + 1 + sizeof(sensor3)] = ETX;
            res = esp_apptrace_write(ESP_APPTRACE_DEST_JTAG, buf, sizeof(sensor3) + STX_LENGTH + 1/*sensor pkt len*/ + 1/*ETX len*/, ESP_APPTRACE_TMO_INFINITE);
            if (res != ESP_OK) {
                ESP_LOGE(TAG, "Failed to write data to host [0x%x] (%s)", res, esp_err_to_name(res));
                break;
            }
            vTaskDelay(10 / portTICK_PERIOD_MS);
        }

        sensor4.header.id = 4;
        sensor4.header.timestamp = esp_log_timestamp();
        sensor4.value = get_pressure(cnt);

        buf[STX_LENGTH] = sizeof(sensor4);
        memcpy(buf + STX_LENGTH + 1, (uint8_t *)&sensor4, sizeof(sensor4));
        buf[STX_LENGTH + 1 + sizeof(sensor4)] = ETX;
        res = esp_apptrace_write(ESP_APPTRACE_DEST_JTAG, buf, sizeof(sensor4) + STX_LENGTH + 1/*sensor pkt len*/ + 1/*ETX len*/, ESP_APPTRACE_TMO_INFINITE);
        if (res != ESP_OK) {
            ESP_LOGE(TAG, "Failed to write data to host [0x%x] (%s)", res, esp_err_to_name(res));
            break;
        }
        vTaskDelay(10 / portTICK_PERIOD_MS);

        esp_apptrace_flush(ESP_APPTRACE_DEST_JTAG, 1000);
        cnt = (cnt + 1) % CYCLE_PERIOD;
    }
    ESP_LOGE(TAG, "Apptrace connection lost");
    ESP_LOGI(TAG, "Data sent and getting back to the UART...");
    ESP_LOGI(TAG, "Done!");
}
