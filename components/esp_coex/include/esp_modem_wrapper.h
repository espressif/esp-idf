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

#ifdef CONFIG_IDF_TARGET_ESP32
void * spin_lock_create_wrapper(void);

uint32_t wifi_int_disable_wrapper(void *wifi_int_mux);

void wifi_int_restore_wrapper(void *wifi_int_mux, uint32_t tmp);
#endif

void task_yield_from_isr_wrapper(void);

#if !defined(CONFIG_IDF_TARGET_ESP32) && !defined(CONFIG_IDF_TARGET_ESP32S2) && !defined(CONFIG_IDF_TARGET_ESP32S3)
void * semphr_create_wrapper(uint32_t max, uint32_t init);

void semphr_delete_wrapper(void *semphr);

int32_t semphr_take_from_isr_wrapper(void *semphr, void *hptw);

int32_t semphr_give_from_isr_wrapper(void *semphr, void *hptw);

int32_t semphr_take_wrapper(void *semphr, uint32_t block_time_tick);

int32_t semphr_give_wrapper(void *semphr);
#endif

void timer_disarm_wrapper(void *timer);

void timer_done_wrapper(void *ptimer);

void timer_setfn_wrapper(void *ptimer, void *pfunction, void *parg);

void timer_arm_us_wrapper(void *ptimer, uint32_t us, bool repeat);

void * malloc_internal_wrapper(size_t size);

#if CONFIG_IDF_TARGET_ESP32C2 || CONFIG_IDF_TARGET_ESP32C6
bool env_is_chip_wrapper(void);

uint32_t esp_clk_slowclk_cal_get_wrapper(void);
#endif

#ifdef __cplusplus
}
#endif

#endif
