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
#define TO_ACTIVE_INTR_TYPE			  GPIO_INTR_LOW_LEVEL
#define TO_SLEEP_INTR_TYPE			  GPIO_INTR_HIGH_LEVEL
/* pm lock type */
#define PM_LOCK_TYPE				  ESP_PM_NO_LIGHT_SLEEP
#define PM_LOCK_TYPE_TO_STRING		  (PM_LOCK_TYPE == 0 ? "CPU_FREQ_MAX" : (PM_LOCK_TYPE == 1 ? "APB_FREQ_MAX" : "NO_LIGHT_SLEEP"))
/* Use boot button as gpio input */
#define BUTTON_NUM                    4
//EXAMPLE_GPIO_WAKEUP
#define GPIO_WAKEUP_SLEEP_NUM   	  4
/* PM */
#define PM_CONFIG_MAX_FREQ_MHZ		  CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ
#define PM_CONFIG_MIN_FREQ_MHZ		  CONFIG_XTAL_FREQ
/* io low == system active*/
#define GPIO_WAKEUP_LEVEL       	  (0)
/* io high == system sleep*/
//#define GPIO_SLEEP_LEVEL        	  1

typedef enum{
	LIGHT_SLEEP_STATE = 0,
	ACTIVE_STATE = 1
}software_state_t;

// 设置软件状态
#define MARK_SOFTWARE_STATE(state)	(sys_state = state)


#ifdef __cplusplus
extern "C" {
#endif

esp_err_t power_config(void);

esp_err_t example_register_gpio_wakeup_sleep(void);

#if CONFIG_IDF_TARGET_ESP32S2 || CONFIG_IDF_TARGET_ESP32S3
//void example_register_touch_wakeup(void);
#endif

#ifdef __cplusplus
}
#endif
