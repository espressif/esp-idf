/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
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

/* 头文件中尽量少放内容，只在一个源文件中使用的宏，就可以直接定义在源文件中 */

//--------------最终应配置成menuconfig中可配置的----------------
/* gpio pin for wake up and sleep */
#define EXAMPLE_GPIO_WAKEUP_SLEEP_NUM   	  (CONFIG_EXAMPLE_GPIO_WAKEUP_SLEEP_NUM)
/* low level wake up */
#if CONFIG_EXAMPLE_GPIO_WAKEUP_HIGH_LEVEL
#define EXAMPLE_GPIO_WAKEUP_LEVEL       	  (1)
#else
#define EXAMPLE_GPIO_WAKEUP_LEVEL       	  (0)
#endif
/* whether to use internal pull up and pull down */
#define EXAMPLE_USE_INTERNAL_PU_PD			  (true)
/* pm lock type */
#define EXAMPLE_PM_LOCK_TYPE				  ESP_PM_NO_LIGHT_SLEEP
/* pm config */
#define EXAMPLE_PM_CONFIG_MAX_FREQ_MHZ		  CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ
#define EXAMPLE_PM_CONFIG_MIN_FREQ_MHZ		  CONFIG_XTAL_FREQ
//--------------最终应配置成menuconfig中可配置的------------------

/* easy to print lock type */
#define PM_LOCK_TYPE_TO_STRING		  		  (EXAMPLE_PM_LOCK_TYPE == 0 ? "CPU_FREQ_MAX" : (EXAMPLE_PM_LOCK_TYPE == 1 ? "APB_FREQ_MAX" : "NO_LIGHT_SLEEP"))

/* acquire lock and release lock interrupt trigger type */
#if EXAMPLE_GPIO_WAKEUP_LEVEL
#define TO_ACTIVE_INTR_TYPE			  		  GPIO_INTR_HIGH_LEVEL
#define TO_SLEEP_INTR_TYPE					  GPIO_INTR_LOW_LEVEL
#else
#define TO_ACTIVE_INTR_TYPE			  		  GPIO_INTR_LOW_LEVEL
#define TO_SLEEP_INTR_TYPE					  GPIO_INTR_HIGH_LEVEL
#endif

/* Types of events that task can handle */
typedef enum {
	PM_LOCK_EVENT_ACQUIRE = 0,
	PM_LOCK_EVENT_RELEASE = 1
} task_event_t;

/* system hold lock state */
typedef enum{
	NO_LOCK_STATE = 0,
	HOLD_LOCK_STATE = 1
} hold_pm_lock_state_t;

/* important properties of gpio wake-up example */
typedef struct {
	// system hold lock state
	hold_pm_lock_state_t hold_lock_state;
	// pm lock handle
	esp_pm_lock_handle_t pm_lock;
	//task handle
	TaskHandle_t event_task;
	// event queue
	QueueHandle_t evt_queue
} gpio_ws_t;


#ifdef __cplusplus
extern "C" {
#endif

esp_err_t power_config(gpio_ws_t* arg);
esp_err_t example_register_gpio_wakeup_sleep(gpio_ws_t* arg);

#ifdef __cplusplus
}
#endif
