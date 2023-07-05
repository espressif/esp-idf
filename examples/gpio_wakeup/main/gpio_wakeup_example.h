/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#pragma once
#include "esp_pm.h"
#include "esp_check.h"
#include "driver/gpio.h"

// 最终应配置成menuconfig中可配置的
/* intr type */
#define TO_WAKEUP_INTR_TYPE	GPIO_INTR_LOW_LEVEL
#define TO_SLEEP_INTR_TYPE	GPIO_INTR_HIGH_LEVEL
/* pm lock type */
#define PM_LOCK_TYPE		ESP_PM_NO_LIGHT_SLEEP
/* Use boot button as gpio input */
#define BUTTON_NUM                    4
#define GPIO_WAKEUP_SLEEP_NUM   	  4
/* PM */
#define PM_CONFIG_MAX_FREQ_MHZ		  CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ
#define PM_CONFIG_MIN_FREQ_MHZ		  CONFIG_XTAL_FREQ


/* io low == system active*/
#define GPIO_WAKEUP_LEVEL       0
/* io high == system sleep*/
#define GPIO_SLEEP_LEVEL        1

typedef enum{
	LIGHT_SLEEP_STATE = 0,
	ACTIVE_STATE = 1
}software_state_t;

//软件状态：时刻与硬件状态保持一致
static software_state_t soft_state;
//pm lock handle
static esp_pm_lock_handle_t light_sleep_handle; 

// 设置软件状态
#define SET_SOFTWARE_STATE(state)	soft_state = state


#ifdef __cplusplus
extern "C" {
#endif

esp_err_t power_config(void);

esp_err_t example_register_gpio_wakeup_sleep(void);

esp_err_t example_register_timer_sleep(void);

#if CONFIG_IDF_TARGET_ESP32S2 || CONFIG_IDF_TARGET_ESP32S3
//void example_register_touch_wakeup(void);
#endif

#ifdef __cplusplus
}
#endif
