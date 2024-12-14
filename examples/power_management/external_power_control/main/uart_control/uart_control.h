/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#pragma once
#include "esp_pm.h"
#include "esp_timer.h"
#include "esp_check.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "external_power_control_example.h"

/**
 * @brief  Important information to be used for uart control
 */
typedef struct {
    int uart_num;                   /*!< uart0 or uart1 */
    int tx_io_num;                  /*!< io pin used by tx */
    int rx_io_num;                  /*!< io pin used by rx */
    int rts_io_num;                 /*!< io pin used by rts */
    int cts_io_num;                 /*!< io pin used by cts */
    int wakeup_threshold;           /*!< uart wakeup threshold */
    TaskHandle_t task_handle;        /*!< a task to handle uart events */
    QueueHandle_t event_queue;      /*!< event queue */
    esp_timer_handle_t sleep_timer; /*!< timer for release power management lock */
    int sleep_timeout;           /*!< wait for some time, if no data is transferred from uart, then ralease lock */
    bool lock_state;                /*!< record the status of the current operating lock, acquired lock or released lock */
    esp_pm_lock_handle_t pm_lock;   /*!< power management lock */
} uart_ctrl_context_t;

#ifdef __cplusplus
extern "C" {
#endif

esp_err_t example_register_uart_control(uart_ctrl_context_t *uart_context);

#ifdef __cplusplus
}
#endif