/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "esp_sys_event.h"
#include "esp_private/panic_internal.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @file esp_sys_event_panic.h
 * @brief Panic event using esp_sys_event framework
 *
 * This header provides macros for registering panic handlers that are
 * called when the system panics. Panic handlers receive information about
 * the panic via panic_info_t and can perform actions like logging, core
 * dump preparation, or custom diagnostics.
 *
 * Note: Panic handlers run in a very restricted context. They must:
 * - Be placed in IRAM if they access any non-IRAM data
 * - Not allocate memory
 * - Not use FreeRTOS APIs
 * - Complete quickly to avoid triggering watchdog timeout
 */

// Context passed to panic handlers
typedef struct {
    const panic_info_t *info;   /*!< Pointer to panic info */
} esp_panic_ctx_t;

/**
 * @brief Register a static (link-time) panic handler
 *
 * Handlers registered with this macro are called during panic processing
 * in priority order (lower priority values run first). The watchdog is
 * fed before and after each handler to prevent timeout during handler
 * execution.
 *
 * @param fn Handler function name
 * @param prio Priority (lower runs first)
 */
#define ESP_PANIC_HANDLER_REGISTER(fn, prio) \
    _ESP_SYS_EVENT_REGISTER(PANIC, prio, fn)

/**
 * @brief Register a static (link-time) early panic breakpoint handler
 *
 * Handlers registered with this macro are called before
 * panic handler breaks with debugger attached.
 *
 * @param fn Handler function name
 * @param prio Priority (lower runs first)
 */
#define ESP_PANIC_EARLY_BREAK_HANDLER_REGISTER(fn, prio) \
    _ESP_SYS_EVENT_REGISTER(PANIC_EARLY_BREAK, prio, fn)

#ifdef __cplusplus
}
#endif
