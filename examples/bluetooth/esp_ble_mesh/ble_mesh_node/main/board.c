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

#include "driver/gpio.h"
#include "esp_log.h"
#include "board.h"
#include "esp_ble_mesh_provisioning_api.h"

#define TAG "BOARD"

#define INTR_FLAG_DEFAULT 0

static xQueueHandle s_evt_queue;

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
    for (int i = 0; i < 3; i++) {
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

static void board_led_init(void)
{
    for (int i = 0; i < 3; i++) {
        gpio_pad_select_gpio(led_state[i].pin);
        gpio_set_direction(led_state[i].pin, GPIO_MODE_OUTPUT);
        gpio_set_level(led_state[i].pin, LED_OFF);
        led_state[i].previous = LED_OFF;
    }
}

static void IRAM_ATTR switch_isr_handler(void *arg)
{
    uint32_t gpio_num = (uint32_t) arg;
    xQueueSendFromISR(s_evt_queue, &gpio_num, NULL);
}

static void switch_key_init(uint32_t key)
{
    gpio_config_t io_conf;
    io_conf.intr_type = GPIO_INTR_NEGEDGE;
    io_conf.pin_bit_mask = 1 << key;
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pull_up_en = 1;
    io_conf.pull_down_en = 0;
    gpio_config(&io_conf);

    gpio_set_intr_type(key, GPIO_INTR_NEGEDGE);
    gpio_install_isr_service(INTR_FLAG_DEFAULT);
    gpio_isr_handler_add(key, switch_isr_handler, (void *)key);
}

static void switch_task_entry(void *arg)
{
    while (1) {
        uint32_t io_num;
        if (xQueueReceive(s_evt_queue, &io_num, portMAX_DELAY) == pdTRUE) {
            uint8_t onoff = led_state[0].previous;
            ESP_LOGI(TAG, "GPIO[%d] intr, val: %d", io_num, gpio_get_level(io_num));
            board_led_operation(LED_R, !onoff);
            led_state[0].previous = !onoff;
            //TODO: publish state change message
        }
    }
}

static void switch_init(gpio_num_t gpio_num)
{
    s_evt_queue = xQueueCreate(3, sizeof(uint32_t));
    if (!s_evt_queue) {
        return;
    }

    BaseType_t ret = xTaskCreate(switch_task_entry, "switch", 4096, NULL, 4, NULL);
    if (ret == pdFAIL) {
        goto fail;
    }

    switch_key_init(gpio_num);
    return;

fail:
    vQueueDelete(s_evt_queue);
}

void board_init(void)
{
    board_led_init();
    switch_init(GPIO_NUM_18);
}
