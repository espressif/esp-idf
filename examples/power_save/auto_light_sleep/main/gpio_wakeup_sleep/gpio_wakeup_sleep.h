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
#include "auto_light_sleep_example.h"

/* wakeup mode */
typedef enum{
	GPIO_LEVEL_WAKEUP = 0,
	GPIO_PULSE_WAKEUP
} gpio_wakeup_mode_t;

/* important properties of gpio wakeup */
typedef struct {
	// gpio wakeup mode
	gpio_wakeup_mode_t wakeup_mode;
	// gpio wakeup level
	int wakeup_level;
	// wakeup io pin
	gpio_num_t gpio;
	// task handle
	TaskHandle_t event_task;
	// event queue
	QueueHandle_t evt_queue;
	// system hold lock state
	hold_pm_lock_state_t hold_lock_state;
	// pm lock handle
	esp_pm_lock_handle_t pm_lock;
} gpio_wakeup_object_t;


#ifdef __cplusplus
extern "C" {
#endif

esp_err_t example_register_gpio_wakeup_sleep( gpio_wakeup_object_t* );

#ifdef __cplusplus
}
#endif
