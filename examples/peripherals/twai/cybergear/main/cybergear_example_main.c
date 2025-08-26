/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include <stdio.h>
#include <string.h>
#include "esp_system.h"
#include "esp_console.h"
#include "esp_log.h"
#include "cybergear.h"
#include "cybergear_console.h"

static const char* TAG = "Cybergear";

#define TWAI_TX_GPIO                CONFIG_EXAMPLE_TWAI_TX_GPIO_NUM
#define TWAI_RX_GPIO                CONFIG_EXAMPLE_TWAI_RX_GPIO_NUM
#define ESP_NODE_ID                 CONFIG_EXAMPLE_TWAI_ESP_NODE_ID
#define MOTOR_ID                    CONFIG_EXAMPLE_TWAI_MOTOR_ID
#define MOTOR_BITRATE               CONFIG_EXAMPLE_TWAI_MOTOR_BITRATE
#define TWAI_QUEUE_DEPTH            1

void app_main(void)
{
    // Initialize Console
    esp_console_repl_t *repl = NULL;
    esp_console_repl_config_t repl_config = ESP_CONSOLE_REPL_CONFIG_DEFAULT();

    repl_config.prompt = "cybergear>";

    // install console REPL environment
#if CONFIG_ESP_CONSOLE_UART
    esp_console_dev_uart_config_t uart_config = ESP_CONSOLE_DEV_UART_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_console_new_repl_uart(&uart_config, &repl_config, &repl));
#elif CONFIG_ESP_CONSOLE_USB_CDC
    esp_console_dev_usb_cdc_config_t cdc_config = ESP_CONSOLE_DEV_CDC_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_console_new_repl_usb_cdc(&cdc_config, &repl_config, &repl));
#elif CONFIG_ESP_CONSOLE_USB_SERIAL_JTAG
    esp_console_dev_usb_serial_jtag_config_t usbjtag_config = ESP_CONSOLE_DEV_USB_SERIAL_JTAG_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_console_new_repl_usb_serial_jtag(&usbjtag_config, &repl_config, &repl));
#endif

    // Initialize CyberGear
    twai_onchip_node_config_t node_config = {
        .io_cfg.tx = TWAI_TX_GPIO,
        .io_cfg.rx = TWAI_RX_GPIO,
        .io_cfg.quanta_clk_out = GPIO_NUM_NC,
        .io_cfg.bus_off_indicator = GPIO_NUM_NC,
        .bit_timing.bitrate = MOTOR_BITRATE,
        .tx_queue_depth = TWAI_QUEUE_DEPTH,
    };
    twai_node_handle_t twai_node;
    ESP_ERROR_CHECK(twai_new_node_onchip(&node_config, &twai_node));
    cybergear_config_t cybergear_config = {
        .motor_id = MOTOR_ID,
        .esp_node_id = ESP_NODE_ID,
    };
    cybergear_handle_t motor;
    ESP_ERROR_CHECK(cybergear_new_twai_motor(twai_node, &cybergear_config, &motor));
    ESP_LOGI(TAG, "CyberGear node started, TX: GPIO%d, RX: GPIO%d", TWAI_TX_GPIO, TWAI_RX_GPIO);

    register_cybergear_commands(motor);

    ESP_LOGI(TAG, "CyberGear console started");

    // Start console REPL
    ESP_ERROR_CHECK(esp_console_start_repl(repl));
}
