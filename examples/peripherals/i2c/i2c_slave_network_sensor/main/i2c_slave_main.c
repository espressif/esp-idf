/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include <stdio.h>
#include <string.h>
#include "esp_log.h"
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include "protocol_examples_common.h"
#include "esp_http_client.h"
#include "cJSON.h"
#include "driver/i2c_slave.h"

static const char *TAG = "example";

#define I2C_SLAVE_SCL_IO CONFIG_I2C_SLAVE_SCL               /*!< gpio number for i2c slave clock */
#define I2C_SLAVE_SDA_IO CONFIG_I2C_SLAVE_SDA               /*!< gpio number for i2c slave data */
#define I2C_SLAVE_NUM    0
#define ESP_SLAVE_ADDR   CONFIG_I2C_SLAVE_ADDRESS           /*!< ESP slave address, you can set any 7bit value */

// Command Lists
#define STARS_COMMAND          (0x10)
#define FORKS_COMMAND          (0x20)
#define OPENISSUES_COMMAND     (0x30)
#define DESCRIPTIONS_COMMAND   (0x40)

#define GITHUB_API_URL "https://api.github.com/repos/espressif/esp-idf"

typedef struct {
    char *json_buffer;
    int json_size;
    uint8_t tmp_buffer_stars[sizeof(int)];
    uint8_t tmp_buffer_forks[sizeof(int)];
    uint8_t tmp_buffer_open_issues[sizeof(int)];
    uint8_t tmp_buffer_descriptions[100];
    QueueHandle_t event_queue;
    uint8_t command_data;
    i2c_slave_dev_handle_t handle;
} i2c_slave_github_context_t;

typedef enum {
    I2C_SLAVE_EVT_RX,
    I2C_SLAVE_EVT_TX
} i2c_slave_event_t;

static esp_err_t _http_event_handler(esp_http_client_event_t *evt)
{
    i2c_slave_github_context_t *context = (i2c_slave_github_context_t *)evt->user_data;
    int star_count = 0, forks_count = 0, open_issues_count = 0;

    switch (evt->event_id) {
    case HTTP_EVENT_ON_DATA:
        if (evt->data_len > 0) {
            if (context->json_buffer == NULL) {
                context->json_buffer = malloc(evt->data_len + 1);
            } else {
                context->json_buffer = realloc(context->json_buffer, context->json_size + evt->data_len + 1);
            }
            if (context->json_buffer == NULL) {
                ESP_LOGE("HTTP_CLIENT", "Failed to allocate memory for data json_buffer");
                return ESP_FAIL;
            }
            memcpy(context->json_buffer + context->json_size, evt->data, evt->data_len);
            context->json_size += evt->data_len;
            context->json_buffer[context->json_size] = '\0';  // Null-terminate the string
        }
        break;
    case HTTP_EVENT_ON_FINISH:
        if (context->json_buffer != NULL) {
            // Process received data
            cJSON *root = cJSON_Parse(context->json_buffer);
            cJSON *stars = cJSON_GetObjectItem(root, "stargazers_count");

            if (stars != NULL) {
                star_count = stars->valueint;
                printf("Star count: %d\n", star_count);
                memcpy(context->tmp_buffer_stars, &star_count, sizeof(int));
            }
            cJSON *forks = cJSON_GetObjectItem(root, "forks_count");
            if (forks != NULL) {
                forks_count = forks->valueint;
                printf("Forks count: %d\n", forks_count);
                memcpy(context->tmp_buffer_forks, &forks_count, sizeof(int));
            }
            cJSON *open_issues = cJSON_GetObjectItem(root, "open_issues_count");
            if (open_issues != NULL) {
                open_issues_count = open_issues->valueint;
                printf("issue count: %d\n", open_issues_count);
                memcpy(context->tmp_buffer_open_issues, &open_issues_count, sizeof(int));
            }
            cJSON *descriptions = cJSON_GetObjectItem(root, "description");
            if (descriptions != NULL) {
                printf("the description is: %s\n", descriptions->valuestring);
                memcpy(context->tmp_buffer_descriptions, descriptions->valuestring, strlen(descriptions->valuestring));
            }
            cJSON_Delete(root);
            free(context->json_buffer);
            context->json_buffer = NULL;
            context->json_size = 0;
        }
        break;
    default:
        break;
    }

    return ESP_OK;
}

void http_get_task(void *pvParameters)
{
    i2c_slave_github_context_t *context = (i2c_slave_github_context_t *)pvParameters;

    esp_http_client_config_t config = {
        .url = GITHUB_API_URL,
        .event_handler = _http_event_handler,
        .method = HTTP_METHOD_GET,
        .buffer_size = 2048,
        .user_data = context,
    };

    while (1) {
        esp_http_client_handle_t client = esp_http_client_init(&config);
        esp_err_t err = esp_http_client_perform(client);
        if (err == ESP_OK) {
            ESP_LOGI("HTTP_CLIENT", "HTTP GET Status = %d, content_length = %lld",
                     esp_http_client_get_status_code(client),
                     esp_http_client_get_content_length(client));
        } else {
            ESP_LOGE("HTTP_CLIENT", "HTTP GET request failed: %s", esp_err_to_name(err));
        }
        esp_http_client_cleanup(client);
        vTaskDelay(30 * 60 * 1000 / portTICK_PERIOD_MS);
    }
    vTaskDelete(NULL);
}

static bool i2c_slave_request_cb(i2c_slave_dev_handle_t i2c_slave, const i2c_slave_request_event_data_t *evt_data, void *arg)
{
    i2c_slave_github_context_t *context = (i2c_slave_github_context_t *)arg;
    i2c_slave_event_t evt = I2C_SLAVE_EVT_TX;
    BaseType_t xTaskWoken = 0;
    xQueueSendFromISR(context->event_queue, &evt, &xTaskWoken);
    return xTaskWoken;
}

static bool i2c_slave_receive_cb(i2c_slave_dev_handle_t i2c_slave, const i2c_slave_rx_done_event_data_t *evt_data, void *arg)
{
    i2c_slave_github_context_t *context = (i2c_slave_github_context_t *)arg;
    i2c_slave_event_t evt = I2C_SLAVE_EVT_RX;
    BaseType_t xTaskWoken = 0;
    // Command only contains one byte, so just save one bytes here.
    context->command_data = *evt_data->buffer;
    xQueueSendFromISR(context->event_queue, &evt, &xTaskWoken);
    return xTaskWoken;
}

static void i2c_slave_task(void *arg)
{
    i2c_slave_github_context_t *context = (i2c_slave_github_context_t *)arg;
    i2c_slave_dev_handle_t handle = (i2c_slave_dev_handle_t)context->handle;

    uint8_t zero_buffer[32] = {}; // Use this buffer to clear the fifo.
    uint32_t write_len, total_written;
    uint32_t buffer_size = 0;

    while (true) {
        i2c_slave_event_t evt;
        if (xQueueReceive(context->event_queue, &evt, 10) == pdTRUE) {
            if (evt == I2C_SLAVE_EVT_TX) {
                uint8_t *data_buffer;
                switch (context->command_data) {
                case STARS_COMMAND:
                    data_buffer = context->tmp_buffer_stars;
                    buffer_size = sizeof(context->tmp_buffer_stars);
                    break;
                case FORKS_COMMAND:
                    data_buffer = context->tmp_buffer_forks;
                    buffer_size = sizeof(context->tmp_buffer_forks);
                    break;
                case OPENISSUES_COMMAND:
                    data_buffer = context->tmp_buffer_open_issues;
                    buffer_size = sizeof(context->tmp_buffer_open_issues);
                    break;
                case DESCRIPTIONS_COMMAND:
                    data_buffer = context->tmp_buffer_descriptions;
                    buffer_size = sizeof(context->tmp_buffer_descriptions);
                    break;
                default:
                    ESP_LOGE(TAG, "Invalid command");
                    data_buffer = zero_buffer;
                    buffer_size = sizeof(zero_buffer);
                    break;
                }

                total_written = 0;
                while (total_written < buffer_size) {
                    ESP_ERROR_CHECK(i2c_slave_write(handle, data_buffer + total_written, buffer_size - total_written, &write_len, 1000));
                    if (write_len == 0) {
                        ESP_LOGE(TAG, "Write error or timeout");
                        break;
                    }
                    total_written += write_len;
                }
            }
        }
    }
    vTaskDelete(NULL);
}

void app_main(void)
{
    static i2c_slave_github_context_t context = {0};

    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    ESP_ERROR_CHECK(example_connect());

    context.event_queue = xQueueCreate(16, sizeof(i2c_slave_event_t));
    if (!context.event_queue) {
        ESP_LOGE(TAG, "Creating queue failed");
        return;
    }

    i2c_slave_config_t i2c_slv_config = {
        .i2c_port = I2C_SLAVE_NUM,
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .scl_io_num = I2C_SLAVE_SCL_IO,
        .sda_io_num = I2C_SLAVE_SDA_IO,
        .slave_addr = ESP_SLAVE_ADDR,
        .send_buf_depth = 100,
        .receive_buf_depth = 100,
    };

    ESP_ERROR_CHECK(i2c_new_slave_device(&i2c_slv_config, &context.handle));
    i2c_slave_event_callbacks_t cbs = {
        .on_receive = i2c_slave_receive_cb,
        .on_request = i2c_slave_request_cb,
    };
    ESP_ERROR_CHECK(i2c_slave_register_event_callbacks(context.handle, &cbs, &context));

    xTaskCreate(http_get_task, "http_get_task", 4096, &context, 20, NULL);
    xTaskCreate(i2c_slave_task, "i2c_slave_task", 1024 * 4, &context, 10, NULL);
}
