/*
 * SPDX-FileCopyrightText: 2021-2023 Espressif Systems (Shanghai) CO LTD
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

/* pm lock type */
#if (CONFIG_EXAMPLE_PM_LOCK_TYPE == 0)
#define DEFAULT_PM_LOCK_TYPE	(ESP_PM_CPU_FREQ_MAX)
#elif (CONFIG_EXAMPLE_PM_LOCK_TYPE == 1)
#define DEFAULT_PM_LOCK_TYPE	(ESP_PM_APB_FREQ_MAX)
#else
#define DEFAULT_PM_LOCK_TYPE	(ESP_PM_NO_LIGHT_SLEEP)
#endif

/* easy to print lock type */
#define PM_LOCK_TYPE_TO_STRING	(DEFAULT_PM_LOCK_TYPE == 0 ? "CPU_FREQ_MAX" : (DEFAULT_PM_LOCK_TYPE == 1 ? "APB_FREQ_MAX" : "NO_LIGHT_SLEEP"))


/* system hold lock state */
typedef enum{
	NO_LOCK_STATE = 0,
	HOLD_LOCK_STATE = 1
} hold_pm_lock_state_t;

/* important properties of auto light sleep example */
typedef struct {
	// wakeup io pin
	gpio_num_t gpio;
	// system hold lock state
	hold_pm_lock_state_t hold_lock_state;
	// pm lock handle
	esp_pm_lock_handle_t pm_lock;
	// task handle
	TaskHandle_t event_task;
	// event queue
	QueueHandle_t evt_queue
} gpio_ws_t;


#ifdef __cplusplus
extern "C" {
#endif

esp_err_t power_config( gpio_ws_t* );
esp_err_t example_register_gpio_wakeup_sleep( gpio_ws_t* );

#ifdef __cplusplus
}
#endif
