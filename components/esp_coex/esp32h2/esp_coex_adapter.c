/*
 * SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <pthread.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "freertos/portmacro.h"
#include "esp_heap_caps.h"
#include "esp_timer.h"
#include "soc/rtc.h"
#include "esp_private/esp_clk.h"
#include "private/esp_coexist_adapter.h"
#include "esp32h2/rom/ets_sys.h"
#include "private/esp_coexist_debug.h"

#define TAG "esp_coex_adapter"

#define OSI_FUNCS_TIME_BLOCKING  0xffffffff

bool IRAM_ATTR esp_coex_common_env_is_chip_wrapper(void)
{
#ifdef CONFIG_IDF_ENV_FPGA
    return false;
#else
    return true;
#endif
}

void *esp_coex_common_spin_lock_create_wrapper(void)
{
    portMUX_TYPE tmp = portMUX_INITIALIZER_UNLOCKED;
    void *mux = malloc(sizeof(portMUX_TYPE));

    if (mux) {
        memcpy(mux, &tmp, sizeof(portMUX_TYPE));
        return mux;
    }
    return NULL;
}

uint32_t IRAM_ATTR esp_coex_common_int_disable_wrapper(void *wifi_int_mux)
{
    if (xPortInIsrContext()) {
        portENTER_CRITICAL_ISR(wifi_int_mux);
    } else {
        portENTER_CRITICAL(wifi_int_mux);
    }

    return 0;
}

void IRAM_ATTR esp_coex_common_int_restore_wrapper(void *wifi_int_mux, uint32_t tmp)
{
    if (xPortInIsrContext()) {
        portEXIT_CRITICAL_ISR(wifi_int_mux);
    } else {
        portEXIT_CRITICAL(wifi_int_mux);
    }
}

void IRAM_ATTR esp_coex_common_task_yield_from_isr_wrapper(void)
{
    portYIELD_FROM_ISR();
}

void *esp_coex_common_semphr_create_wrapper(uint32_t max, uint32_t init)
{
    return (void *)xSemaphoreCreateCounting(max, init);
}

void esp_coex_common_semphr_delete_wrapper(void *semphr)
{
    vSemaphoreDelete(semphr);
}

int32_t esp_coex_common_semphr_take_wrapper(void *semphr, uint32_t block_time_tick)
{
    if (block_time_tick == OSI_FUNCS_TIME_BLOCKING) {
        return (int32_t)xSemaphoreTake(semphr, portMAX_DELAY);
    } else {
        return (int32_t)xSemaphoreTake(semphr, block_time_tick);
    }
}

int32_t esp_coex_common_semphr_give_wrapper(void *semphr)
{
    return (int32_t)xSemaphoreGive(semphr);
}

void IRAM_ATTR esp_coex_common_timer_disarm_wrapper(void *timer)
{
    ets_timer_disarm(timer);
}

void esp_coex_common_timer_done_wrapper(void *ptimer)
{
    ets_timer_done(ptimer);
}

void esp_coex_common_timer_setfn_wrapper(void *ptimer, void *pfunction, void *parg)
{
    ets_timer_setfn(ptimer, pfunction, parg);
}

void IRAM_ATTR esp_coex_common_timer_arm_us_wrapper(void *ptimer, uint32_t us, bool repeat)
{
    ets_timer_arm_us(ptimer, us, repeat);
}

void *IRAM_ATTR esp_coex_common_malloc_internal_wrapper(size_t size)
{
    return heap_caps_malloc(size, MALLOC_CAP_8BIT | MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL);
}

/* static wrapper */

static int32_t IRAM_ATTR esp_coex_semphr_take_from_isr_wrapper(void *semphr, void *hptw)
{
    return (int32_t)xSemaphoreTakeFromISR(semphr, hptw);
}

static int32_t IRAM_ATTR esp_coex_semphr_give_from_isr_wrapper(void *semphr, void *hptw)
{
    return (int32_t)xSemaphoreGiveFromISR(semphr, hptw);
}

static int esp_coexist_debug_matrix_init_wrapper(int evt, int sig, bool rev)
{
#if CONFIG_ESP_COEX_GPIO_DEBUG
    return esp_coexist_debug_matrix_init(evt, sig, rev);
#else
    return ESP_ERR_NOT_SUPPORTED;
#endif
}

static IRAM_ATTR int esp_coex_common_xtal_freq_get_wrapper(void)
{
    return rtc_clk_xtal_freq_get();
}

coex_adapter_funcs_t g_coex_adapter_funcs = {
    ._version = COEX_ADAPTER_VERSION,
    ._task_yield_from_isr = esp_coex_common_task_yield_from_isr_wrapper,
    ._semphr_create = esp_coex_common_semphr_create_wrapper,
    ._semphr_delete = esp_coex_common_semphr_delete_wrapper,
    ._semphr_take_from_isr = esp_coex_semphr_take_from_isr_wrapper,
    ._semphr_give_from_isr = esp_coex_semphr_give_from_isr_wrapper,
    ._semphr_take = esp_coex_common_semphr_take_wrapper,
    ._semphr_give = esp_coex_common_semphr_give_wrapper,
    ._is_in_isr = xPortInIsrContext,
    ._malloc_internal =  esp_coex_common_malloc_internal_wrapper,
    ._free = free,
    ._esp_timer_get_time = esp_timer_get_time,
    ._env_is_chip = esp_coex_common_env_is_chip_wrapper,
    ._timer_disarm = esp_coex_common_timer_disarm_wrapper,
    ._timer_done = esp_coex_common_timer_done_wrapper,
    ._timer_setfn = esp_coex_common_timer_setfn_wrapper,
    ._timer_arm_us = esp_coex_common_timer_arm_us_wrapper,
    ._debug_matrix_init = esp_coexist_debug_matrix_init_wrapper,
    ._xtal_freq_get = esp_coex_common_xtal_freq_get_wrapper,
    ._magic = COEX_ADAPTER_MAGIC,
};
