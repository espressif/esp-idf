/*
 * SPDX-FileCopyrightText: 2018-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __ESP_MODEM_WRAPPER_INTERNAL_H__
#define __ESP_MODEM_WRAPPER_INTERNAL_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <string.h>
#include <stdint.h>
#include <stdbool.h>

bool esp_coex_common_env_is_chip_wrapper(void);

void * esp_coex_common_spin_lock_create_wrapper(void);

uint32_t esp_coex_common_int_disable_wrapper(void *wifi_int_mux);

void esp_coex_common_int_restore_wrapper(void *wifi_int_mux, uint32_t tmp);

void esp_coex_common_task_yield_from_isr_wrapper(void);

void * esp_coex_common_semphr_create_wrapper(uint32_t max, uint32_t init);

void esp_coex_common_semphr_delete_wrapper(void *semphr);

int32_t esp_coex_common_semphr_take_wrapper(void *semphr, uint32_t block_time_tick);

int32_t esp_coex_common_semphr_give_wrapper(void *semphr);

void esp_coex_common_timer_disarm_wrapper(void *timer);

void esp_coex_common_timer_done_wrapper(void *ptimer);

void esp_coex_common_timer_setfn_wrapper(void *ptimer, void *pfunction, void *parg);

void esp_coex_common_timer_arm_us_wrapper(void *ptimer, uint32_t us, bool repeat);

void * esp_coex_common_malloc_internal_wrapper(size_t size);

#ifndef CONFIG_IDF_TARGET_ESP32
uint32_t esp_coex_common_clk_slowclk_cal_get_wrapper(void);
#endif

#ifdef __cplusplus
}
#endif

#endif
