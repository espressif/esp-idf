/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "esp_cpu.h"
#include "esp_private/esp_sys_event.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Register a handler to run on core 0 immediately before the scheduler starts
 */
#define ESP_PRE_SCHEDULER_HANDLER_REGISTER(fn, priority) \
    static esp_err_t fn(void *user_arg, void *ctx); \
    _ESP_SYS_EVENT_REGISTER(PRE_SCHEDULER, priority, __esp_pre_scheduler_##fn) \
    { \
        if (esp_cpu_get_core_id() != 0) { \
            return ESP_OK; \
        } \
        return fn(user_arg, ctx); \
    } \
    static esp_err_t fn(void *user_arg, void *ctx)

/**
 * @brief Register a handler to run on every core immediately before its scheduler starts
 */
#define ESP_PRE_SCHEDULER_HANDLER_REGISTER_PER_CPU(fn, priority) \
    _ESP_SYS_EVENT_REGISTER(PRE_SCHEDULER, priority, fn)

/**
 * @brief Register a handler to run in the main task immediately before app_main
 *
 * Handlers run in ascending priority order after all CPUs have completed
 * FreeRTOS initialization. The scheduler and heap allocator are available.
 */
#define ESP_PRE_APP_MAIN_HANDLER_REGISTER(fn, priority) \
    _ESP_SYS_EVENT_REGISTER(PRE_APP_MAIN, priority, fn)

#ifdef __cplusplus
}
#endif
