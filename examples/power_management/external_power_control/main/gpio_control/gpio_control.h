/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#pragma once
#include "esp_pm.h"
#include "esp_check.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "external_power_control_example.h"

/**
 * @brief  GPIO control mode
 */
typedef enum {
    GPIO_LEVEL_CONTROL = 0, /*!< level control mode */
    GPIO_PULSE_CONTROL      /*!< pulse control mode */
} gpio_control_mode_t;

/**
 * @brief  Important information to be used for gpio control
 */
typedef struct {
    gpio_control_mode_t control_mode;   /*!< gpio control mode */
    int control_level;                  /*!< level of trigger control */
    gpio_num_t gpio;                    /*!< pin for control */
    TaskHandle_t task_handle;           /*!< a task to handle gpio events */
    QueueHandle_t event_queue;          /*!< event queue */
    bool lock_state;                    /*!< record the status of the current operating lock, acquired lock or released lock */
    esp_pm_lock_handle_t pm_lock;       /*!< power management lock */
} gpio_ctrl_context_t;

#ifdef __cplusplus
extern "C" {
#endif

esp_err_t example_register_gpio_control(gpio_ctrl_context_t *gpio_context);

#ifdef __cplusplus
}
#endif
