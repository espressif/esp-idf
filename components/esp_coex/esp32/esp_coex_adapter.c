/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
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
#include "private/esp_coexist_adapter.h"
#include "esp32/rom/ets_sys.h"

#define TAG "esp_coex_adapter"

#define OSI_FUNCS_TIME_BLOCKING  0xffffffff

typedef struct {
    QueueHandle_t handle; /**< FreeRTOS queue handler */
    void *storage;        /**< storage for FreeRTOS queue */
} modem_static_queue_t;

bool IRAM_ATTR esp_coex_common_env_is_chip_wrapper(void)
{
#ifdef CONFIG_IDF_ENV_FPGA
    return false;
#else
    return true;
#endif
}

void * esp_coex_common_spin_lock_create_wrapper(void)
{
    portMUX_TYPE tmp = portMUX_INITIALIZER_UNLOCKED;
    void *mux = heap_caps_malloc(sizeof(portMUX_TYPE), MALLOC_CAP_8BIT|MALLOC_CAP_INTERNAL);

    if (mux) {
        memcpy(mux,&tmp,sizeof(portMUX_TYPE));
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

void * esp_coex_common_semphr_create_wrapper(uint32_t max, uint32_t init)
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

void * IRAM_ATTR esp_coex_common_malloc_internal_wrapper(size_t size)
{
    return heap_caps_malloc(size, MALLOC_CAP_8BIT|MALLOC_CAP_DMA|MALLOC_CAP_INTERNAL);
}

/* static wrapper */

static int IRAM_ATTR esp_coex_is_in_isr_wrapper(void)
{
    return !xPortCanYield();
}

static void *esp_coex_internal_semphr_create_wrapper(uint32_t max, uint32_t init)
{
    modem_static_queue_t *semphr = heap_caps_calloc(1, sizeof(modem_static_queue_t), MALLOC_CAP_8BIT|MALLOC_CAP_INTERNAL);
    if (!semphr) {
        return NULL;
    }

#ifdef CONFIG_SPIRAM_USE_MALLOC
    semphr->storage = heap_caps_calloc(1, sizeof(StaticSemaphore_t), MALLOC_CAP_8BIT|MALLOC_CAP_INTERNAL);
    if (!semphr->storage) {
        goto _error;
    }

    semphr->handle = xSemaphoreCreateCountingStatic(max, init, semphr->storage);
    if (!semphr->handle) {
        goto _error;
    }
    return (void *)semphr;

_error:
    if (semphr) {
        if (semphr->storage) {
            free(semphr->storage);
        }

        free(semphr);
    }
    return NULL;
#else
    semphr->handle = xSemaphoreCreateCounting(max, init);
    return (void *)semphr;
#endif
}

static void esp_coex_internal_semphr_delete_wrapper(void *semphr)
{
    modem_static_queue_t *semphr_item = (modem_static_queue_t *)semphr;
    if (semphr_item) {
        if (semphr_item->handle) {
            vSemaphoreDelete(semphr_item->handle);
        }
#ifdef CONFIG_SPIRAM_USE_MALLOC
        if (semphr_item->storage) {
            free(semphr_item->storage);
        }
#endif
        free(semphr_item);
    }
}

static int32_t IRAM_ATTR esp_coex_internal_semphr_take_from_isr_wrapper(void *semphr, void *hptw)
{
    return (int32_t)xSemaphoreTakeFromISR(((modem_static_queue_t *)semphr)->handle, hptw);
}

static int32_t IRAM_ATTR esp_coex_internal_semphr_give_from_isr_wrapper(void *semphr, void *hptw)
{
    return (int32_t)xSemaphoreGiveFromISR(((modem_static_queue_t *)semphr)->handle, hptw);
}

static int32_t esp_coex_internal_semphr_take_wrapper(void *semphr, uint32_t block_time_tick)
{
    if (block_time_tick == OSI_FUNCS_TIME_BLOCKING) {
        return (int32_t)xSemaphoreTake(((modem_static_queue_t *)semphr)->handle, portMAX_DELAY);
    } else {
        return (int32_t)xSemaphoreTake(((modem_static_queue_t *)semphr)->handle, block_time_tick);
    }
}

static int32_t esp_coex_internal_semphr_give_wrapper(void *semphr)
{
    return (int32_t)xSemaphoreGive(((modem_static_queue_t *)semphr)->handle);
}

coex_adapter_funcs_t g_coex_adapter_funcs = {
    ._version = COEX_ADAPTER_VERSION,
    ._spin_lock_create = esp_coex_common_spin_lock_create_wrapper,
    ._spin_lock_delete = free,
    ._int_disable = esp_coex_common_int_disable_wrapper,
    ._int_enable = esp_coex_common_int_restore_wrapper,
    ._task_yield_from_isr = esp_coex_common_task_yield_from_isr_wrapper,
    ._semphr_create = esp_coex_internal_semphr_create_wrapper,
    ._semphr_delete = esp_coex_internal_semphr_delete_wrapper,
    ._semphr_take_from_isr = esp_coex_internal_semphr_take_from_isr_wrapper,
    ._semphr_give_from_isr = esp_coex_internal_semphr_give_from_isr_wrapper,
    ._semphr_take = esp_coex_internal_semphr_take_wrapper,
    ._semphr_give = esp_coex_internal_semphr_give_wrapper,
    ._is_in_isr = esp_coex_is_in_isr_wrapper,
    ._malloc_internal =  esp_coex_common_malloc_internal_wrapper,
    ._free = free,
    ._esp_timer_get_time = esp_timer_get_time,
    ._timer_disarm = esp_coex_common_timer_disarm_wrapper,
    ._timer_done = esp_coex_common_timer_done_wrapper,
    ._timer_setfn = esp_coex_common_timer_setfn_wrapper,
    ._timer_arm_us = esp_coex_common_timer_arm_us_wrapper,
    ._magic = COEX_ADAPTER_MAGIC,
};
