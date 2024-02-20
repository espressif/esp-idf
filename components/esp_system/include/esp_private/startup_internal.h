/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "esp_attr.h"
#include "esp_err.h"
#include "esp_bit_defs.h"
#include "esp_cpu.h"

#include "soc/soc_caps.h"

#include "sdkconfig.h"

#ifdef __cplusplus
extern "C" {
#endif

// Port layer defines the entry point. It then transfer control to a `sys_startup_fn_t`, stored in this
// array, one per core.
typedef void (*sys_startup_fn_t)(void);

/* This array of per-CPU system layer startup functions is initialized in the non-port part of esp_system */
#if !CONFIG_ESP_SYSTEM_SINGLE_CORE_MODE
extern sys_startup_fn_t const g_startup_fn[SOC_CPU_CORES_NUM];
#else
extern sys_startup_fn_t const g_startup_fn[1];
#endif

// Utility to execute `sys_startup_fn_t` for the current core.
#define SYS_STARTUP_FN()  ((*g_startup_fn[(esp_cpu_get_core_id())])())

#if !CONFIG_ESP_SYSTEM_SINGLE_CORE_MODE
void startup_resume_other_cores(void);
#endif

/**
 * Internal structure describing ESP_SYSTEM_INIT_FN startup functions
 */
typedef struct {
    esp_err_t (*fn)(void);   /*!< Pointer to the startup function */
    uint16_t cores;          /*!< Bit mask of cores where the function has to be called */
    uint16_t stage;          /*!< Init stage number (0 or 1) */
} esp_system_init_fn_t;

#define ESP_SYSTEM_INIT_STAGE_CORE          0
#define ESP_SYSTEM_INIT_STAGE_SECONDARY     1

/**
 * @brief Define a system initialization function which will be executed on the specified cores
 *
 * @param f  function name (identifier)
 * @param stage_  init stage name (CORE or SECONDARY)
 * @param c  bit mask of cores to execute the function on (ex. if BIT0 is set, the function
 *           will be executed on CPU 0, if BIT1 is set - on CPU 1, and so on)
 * @param priority  integer, priority of the initialization function. Higher values mean that
 *                  the function will be executed later in the process.
 * @param (varargs)  optional, additional attributes for the function declaration (such as IRAM_ATTR)
 *
 * The function defined using this macro must return ESP_OK on success. Any other value will be
 * logged and the startup process will abort.
 *
 * Initialization functions should be placed in a compilation unit where at least one other
 * symbol is referenced in another compilation unit. This means that the reference should not itself
 * get optimized out by the compiler or discarded by the linker if the related feature is used.
 * It is, on the other hand, a good practice to make sure the initialization function does get
 * discarded if the related feature is not used.
 */
#define ESP_SYSTEM_INIT_FN(f, stage_, c, priority, ...) \
    static esp_err_t __VA_ARGS__ __esp_system_init_fn_##f(void); \
    static __attribute__((used)) _SECTION_ATTR_IMPL(".esp_system_init_fn", priority) \
        esp_system_init_fn_t esp_system_init_fn_##f = { \
            .fn = ( __esp_system_init_fn_##f), \
            .cores = (c), \
            .stage = ESP_SYSTEM_INIT_STAGE_##stage_ \
        }; \
    static esp_err_t __esp_system_init_fn_##f(void)

#ifdef CONFIG_ESP_SYSTEM_SINGLE_CORE_MODE
#define ESP_SYSTEM_INIT_ALL_CORES BIT(0)
#else
#define ESP_SYSTEM_INIT_ALL_CORES (BIT(SOC_CPU_CORES_NUM) - 1)
#endif

extern uint64_t g_startup_time;   // Startup time that serves as the point of origin for system time. Should be set by the entry
// function in the port layer. May be 0 as well if this is not backed by a persistent counter, in which case
// startup time = system time = 0 at the point the entry function sets this variable.

#ifdef __cplusplus
}
#endif
