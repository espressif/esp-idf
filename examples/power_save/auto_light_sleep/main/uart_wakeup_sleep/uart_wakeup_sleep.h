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
#include "esp_timer.h"
#include "auto_light_sleep_example.h"

/* important properties of uart wakeup */
typedef struct {
	// uart0 or uart1
	int uart_num;
	// tx io num
	int tx_io_num;
	// rx io num
	int rx_io_num;
	// rts io num
	int rts_io_num;
	// cts io num
	int cts_io_num;
	// uart wakeup threshold
	int wakeup_threshold;
	// task handle
	TaskHandle_t event_task;
	// event queue
	QueueHandle_t evt_queue;
	// timer handle
	esp_timer_handle_t to_sleep_timer;
	// system hold lock state
	hold_pm_lock_state_t hold_lock_state;
	// pm lock handle
	esp_pm_lock_handle_t pm_lock;
} uart_wakeup_object_t;


#ifdef __cplusplus
extern "C" {
#endif

esp_err_t example_register_uart_wakeup_sleep( uart_wakeup_object_t* );

#ifdef __cplusplus
}
#endif
