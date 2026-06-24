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
 * @file esp_sys_event_system_init.h
 * @brief System initialization events using esp_sys_event framework
 *
 * This header provides macros for registering system initialization functions
 * that replace the legacy ESP_SYSTEM_INIT_FN infrastructure.
 *
 * There are two system init event lists:
 * - system_init_core: CORE stage, runs on core 0 (secondary cores are not
 *   started yet at this point, so there is no per-CPU variant)
 * - system_init_secondary: SECONDARY stage, runs on each core; core-scoped
 *   registration macros use wrappers that skip on cores where they should not
 *   run, preserving one priority-ordered list for the whole secondary stage.
 *
 * Migration from ESP_SYSTEM_INIT_FN:
 * - ESP_SYSTEM_INIT_FN(fn, CORE, BIT(0), prio) -> ESP_SYSTEM_INIT_CORE(fn, prio)
 * - ESP_SYSTEM_INIT_FN(fn, SECONDARY, BIT(0), prio) -> ESP_SYSTEM_INIT_SECONDARY(fn, prio)
 * - ESP_SYSTEM_INIT_FN(fn, SECONDARY, ALL_CORES, prio) -> ESP_SYSTEM_INIT_SECONDARY_PERCPU(fn, prio)
 */

/**
 * @brief Register a CORE stage init function (runs once on core 0)
 *
 * This is the most common init function type. The function runs once during
 * the CORE initialization stage on CPU 0.
 *
 * @param fn Function name
 * @param prio Priority (lower runs first)
 */
#define ESP_SYSTEM_INIT_CORE(fn, prio) \
    _ESP_SYS_EVENT_REGISTER(SYSTEM_INIT_CORE, prio, fn)

#define _ESP_SYSTEM_INIT_SECONDARY_RAW(fn, prio) \
    _ESP_SYS_EVENT_REGISTER(SYSTEM_INIT_SECONDARY, prio, fn)

#define _ESP_SYSTEM_INIT_SECONDARY_ON_CORE(fn, prio, core_id) \
    static esp_err_t fn(void *user_arg, void *ctx); \
    _ESP_SYSTEM_INIT_SECONDARY_RAW(__esp_system_init_secondary_##fn, prio) \
    { \
        if (esp_cpu_get_core_id() != (core_id)) { \
            return ESP_OK; \
        } \
        return fn(user_arg, ctx); \
    } \
    static esp_err_t fn(void *user_arg, void *ctx)

/**
 * @brief Register a SECONDARY stage init function (runs once on core 0)
 *
 * This is the most common init function type for secondary stage. The function
 * runs once during the SECONDARY initialization stage on CPU 0.
 *
 * @param fn Function name
 * @param prio Priority (lower runs first)
 */
#define ESP_SYSTEM_INIT_SECONDARY(fn, prio) \
    _ESP_SYSTEM_INIT_SECONDARY_ON_CORE(fn, prio, 0)

/**
 * @brief Register a SECONDARY stage init function that runs on each core
 *
 * Use this for init functions that must run on every core during the
 * SECONDARY initialization stage.
 *
 * @param fn Function name
 * @param prio Priority (lower runs first)
 */
#define ESP_SYSTEM_INIT_SECONDARY_PERCPU(fn, prio) \
    _ESP_SYSTEM_INIT_SECONDARY_RAW(fn, prio)

#ifdef __cplusplus
}
#endif
