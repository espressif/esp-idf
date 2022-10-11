/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include <string.h>
#include <sys/param.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include "protocol_examples_common.h"
#include "esp_transport.h"
#include "esp_transport_tcp.h"


#define HOST_IP_ADDR CONFIG_EXAMPLE_IPV4_ADDR

#define PORT CONFIG_EXAMPLE_PORT

static const char *TAG = "tcp_transport_client";
static const char *payload = "Message from ESP32";

static void tcp_transport_client_task(void *pvParameters)
{
    char rx_buffer[128];
    char host_ip[] = HOST_IP_ADDR;
    esp_transport_handle_t tcp = esp_transport_tcp_init();
    while (1) {
        if (tcp == NULL) {
            ESP_LOGE(TAG, "Error occurred during esp_transport_tcp_init()");
            break;
        }
        int err = esp_transport_connect(tcp, HOST_IP_ADDR, PORT, -1);
        if (err != 0) {
            ESP_LOGE(TAG, "Client unable to connect: errno %d", errno);
            break;
        }
        ESP_LOGI(TAG, "Successfully connected");

        while (1) {
            int bytes_written = esp_transport_write(tcp, payload, strlen(payload), 0);
            if (bytes_written < 0) {
                ESP_LOGE(TAG, "Error occurred during sending: esp_transport_write() returned %d, errno %d", bytes_written, errno);
                break;
            }
            int len = esp_transport_read(tcp, rx_buffer, sizeof(rx_buffer) - 1, 0);
            // Error occurred during receiving
            if (len < 0) {
                ESP_LOGE(TAG, "recv failed: esp_transport_read() returned %d, errno %d", len, errno);
                break;
            }
            // Data received
                rx_buffer[len] = 0; // Null-terminate whatever we received and treat like a string
                ESP_LOGI(TAG, "Received %d bytes from %s:", len, host_ip);
                ESP_LOGI(TAG, "%s", rx_buffer);

            vTaskDelay(2000 / portTICK_PERIOD_MS);
        }

            ESP_LOGE(TAG, "Shutting down TCP and restarting...");
            esp_transport_close(tcp);
    }
    esp_transport_destroy(tcp);
    vTaskDelete(NULL);
}

void app_main(void)
{
    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    /* This helper function configures Wi-Fi or Ethernet, as selected in menuconfig.
     * Read "Establishing Wi-Fi or Ethernet Connection" section in
     * examples/protocols/README.md for more information about this function.
     */
    ESP_ERROR_CHECK(example_connect());

    xTaskCreate(tcp_transport_client_task, "tcp_transport_client", 4096, NULL, 5, NULL);
}
