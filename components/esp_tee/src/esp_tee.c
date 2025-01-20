/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdarg.h>

#include "esp_attr.h"
#include "esp_private/cache_utils.h"

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/task.h"

#include "esp_tee.h"
#include "secure_service_num.h"

/* See esp_tee_u2m_switch.S */
extern uint32_t _u2m_switch(int argc, va_list ap);

static DRAM_ATTR SemaphoreHandle_t s_tee_mutex;
static DRAM_ATTR StaticSemaphore_t s_tee_mutex_buf;

static IRAM_ATTR void init_mutex(void)
{
    static bool is_first_call = true;
    if (is_first_call) {
        s_tee_mutex = xSemaphoreCreateMutexStatic(&s_tee_mutex_buf);
        is_first_call = false;
    }
}

/**
 * TEE interface API used by untrusted side application
 * to call secure service in trusted side
 */
uint32_t IRAM_ATTR esp_tee_service_call(int argc, ...)
{
    init_mutex();

    uint32_t val = UINT32_MAX;
    va_list ap;
    va_start(ap, argc);

    if (xTaskGetSchedulerState() == taskSCHEDULER_RUNNING) {
        if (xSemaphoreTake(s_tee_mutex, portMAX_DELAY) == pdTRUE) {
            val = _u2m_switch(argc, ap);
            xSemaphoreGive(s_tee_mutex);
        }
    } else {
        val = _u2m_switch(argc, ap);
    }

    va_end(ap);
    return val;
}

uint32_t IRAM_ATTR esp_tee_service_call_with_noniram_intr_disabled(int argc, ...)
{
    uint32_t val = UINT32_MAX;
    va_list ap;
    va_start(ap, argc);

    /* NOTE: Disabling the scheduler and non-IRAM residing interrupts */
    spi_flash_op_lock();
    esp_intr_noniram_disable();

    val = _u2m_switch(argc, ap);

    esp_intr_noniram_enable();
    spi_flash_op_unlock();

    va_end(ap);
    return val;
}
