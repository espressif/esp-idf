/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
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
#include "esp_transport_socks_proxy.h"

#define TARGET_ADDR CONFIG_EXAMPLE_TARGET_ADDR
#define TARGET_PORT CONFIG_EXAMPLE_TARGET_PORT

#ifdef CONFIG_EXAMPLE_ENABLE_PROXY
#define PROXY_ADDR CONFIG_EXAMPLE_PROXY_ADDR
#define PROXY_PORT CONFIG_EXAMPLE_PROXY_PORT
#endif

static const char *TAG = "tcp_transport_client";
static const char *payload = "Message from ESP32\n";

static void tcp_transport_client_task(void *pvParameters)
{
    char rx_buffer[128];
    char host_ip[] = TARGET_ADDR;
    esp_transport_handle_t transport = esp_transport_tcp_init();

    #ifdef CONFIG_EXAMPLE_ENABLE_PROXY
    /*
     * The socks transport is a composed transport, so we save the previously created
     * handler to use it as a parent transport, so our transport is now socks over tcp.
     * We could have used the ssl transport as parent and we can use a socks transport as a
     * parent to websocket transport.
     *
     */
    esp_transport_handle_t parent = transport;
    esp_transport_socks_proxy_config_t proxy_config = {.port = PROXY_PORT, .address = PROXY_ADDR, .version = SOCKS4};
    transport = esp_transport_socks_proxy_init(parent, &proxy_config);
    #endif

  while (1) {
        if (transport == NULL) {
            ESP_LOGE(TAG, "Error occurred during esp_transport_proxy_init()");
            break;
        }
        int err = esp_transport_connect(transport, TARGET_ADDR, TARGET_PORT, -1);
        if (err != 0) {
            ESP_LOGE(TAG, "Client unable to connect: errno %d", errno);
            break;
        }
        ESP_LOGI(TAG, "Successfully connected");

        while (1) {
            int bytes_written = esp_transport_write(transport, payload, strlen(payload), 0);
            if (bytes_written < 0) {
                ESP_LOGE(TAG, "Error occurred during sending: esp_transport_write() returned %d, errno %d", bytes_written, errno);
                break;
            }
            int len = esp_transport_read(transport, rx_buffer, sizeof(rx_buffer) - 1, 0);
            // Error occurred during receiving
            if (len < 0) {
                ESP_LOGE(TAG, "recv failed: esp_transport_read() returned %d, errno %d", len, errno);
                break;
            }
            // Data received
            rx_buffer[len] = 0; // Null-terminate whatever we received and treat like a string
            ESP_LOGI(TAG, "Received %d bytes from %s:", len, host_ip);
            ESP_LOGI(TAG, "Received data : %s", rx_buffer);

            vTaskDelay(2000 / portTICK_PERIOD_MS);
        }

        ESP_LOGE(TAG, "Shutting down transport and restarting...");
        esp_transport_close(transport);
    }
    esp_transport_destroy(transport);

    #ifdef CONFIG_EXAMPLE_ENABLE_PROXY
    esp_transport_destroy(parent);
    #endif

  vTaskDelete(NULL);
}

void app_main(void)
{
    esp_log_level_set("transport", ESP_LOG_VERBOSE);
    esp_log_level_set("transport_base", ESP_LOG_VERBOSE);
    esp_log_level_set("transport_proxy", ESP_LOG_VERBOSE);

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
