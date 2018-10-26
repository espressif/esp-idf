/* board.c - Board-specific hooks */

/*
 * Copyright (c) 2017 Intel Corporation
 * Additional Copyright (c) 2018 Espressif Systems (Shanghai) PTE LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"

#include "driver/uart.h"
#include "esp_log.h"

#include "esp_ble_mesh_provisioning_api.h"
#include "board.h"

#define TAG "BOARD"

#define MESH_UART_NUM UART_NUM_1
#define MESH_UART    (&UART1)

#define UART_BUF_SIZE 128

#define UART1_TX_PIN  GPIO_NUM_16
#define UART1_RX_PIN  GPIO_NUM_17

extern uint16_t remote_addr;

struct _led_state led_state[3] = {
    { LED_OFF, LED_OFF, LED_R, "red"   },
    { LED_OFF, LED_OFF, LED_G, "green" },
    { LED_OFF, LED_OFF, LED_B, "blue"  },
};

void board_output_number(esp_ble_mesh_output_action_t action, uint32_t number)
{
    ESP_LOGI(TAG, "Board output number %d", number);
}

void board_prov_complete(void)
{
    board_led_operation(LED_G, LED_OFF);
}

void board_led_operation(uint8_t pin, uint8_t onoff)
{
    for (int i = 0; i < ARRAY_SIZE(led_state); i++) {
        if (led_state[i].pin != pin) {
            continue;
        }
        if (onoff == led_state[i].previous) {
            ESP_LOGW(TAG, "led %s is already %s",
                     led_state[i].name, (onoff ? "on" : "off"));
            return;
        }
        gpio_set_level(pin, onoff);
        led_state[i].previous = onoff;
        return;
    }
    ESP_LOGE(TAG, "LED is not found!");
}

static void board_uart_task(void *p)
{
    uint8_t *data = calloc(1, UART_BUF_SIZE);
    uint32_t input;

    while (1) {
        int len = uart_read_bytes(MESH_UART_NUM, data, UART_BUF_SIZE, 100 / portTICK_RATE_MS);
        if (len > 0) {
            input = strtoul((const char *)data, NULL, 16);
            remote_addr = input & 0xFFFF;
            ESP_LOGI(TAG, "%s: input 0x%08x, remote_addr 0x%04x", __func__, input, remote_addr);
            memset(data, 0, UART_BUF_SIZE);
        }
    }

    vTaskDelete(NULL);
}

static void board_led_init(void)
{
    for (int i = 0; i < ARRAY_SIZE(led_state); i++) {
        gpio_pad_select_gpio(led_state[i].pin);
        gpio_set_direction(led_state[i].pin, GPIO_MODE_OUTPUT);
        gpio_set_level(led_state[i].pin, LED_OFF);
        led_state[i].previous = LED_OFF;
    }
}

static void board_uart_init(void)
{
    uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };
    uart_param_config(MESH_UART_NUM, &uart_config);
    uart_set_pin(MESH_UART_NUM, UART1_TX_PIN, UART1_RX_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    uart_driver_install(MESH_UART_NUM, UART_BUF_SIZE * 2, 0, 0, NULL, 0);
}

void board_init(void)
{
    board_led_init();
    board_uart_init();
    xTaskCreate(board_uart_task, "board_uart_task", 2048, NULL, 5, NULL);
}
