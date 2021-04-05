// Copyright 2015-2018 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#include "esp_attr.h"

#include "soc/soc_caps.h"
#include "hal/cpu_hal.h"

#include "sdkconfig.h"

#ifdef __cplusplus
extern "C" {
#endif

extern bool g_spiram_ok; // [refactor-todo] better way to communicate this from port layer to common startup code

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
#define SYS_STARTUP_FN()  ((*g_startup_fn[(cpu_hal_get_core_id())])())

#if !CONFIG_ESP_SYSTEM_SINGLE_CORE_MODE
void startup_resume_other_cores(void);
#endif

typedef struct {
  void (*fn)(void);
  uint32_t cores;
} esp_system_init_fn_t;

/*
 * Declare an component initialization function that will execute on the specified cores (ex. if BIT0 == 1, will execute
 * on CORE0, CORE1 if BIT1 and so on).
 *
 * @note Initialization functions should be placed in a compilation unit where at least one other
 * symbol is referenced 'meaningfully' in another compilation unit, otherwise this gets discarded during linking. (By
 * 'meaningfully' we mean the reference should not itself get optimized out by the compiler/discarded by the linker).
 */
#define ESP_SYSTEM_INIT_FN(f, c, ...) \
static void  __attribute__((used)) __VA_ARGS__ __esp_system_init_fn_##f(void); \
static __attribute__((used)) esp_system_init_fn_t _SECTION_ATTR_IMPL(".esp_system_init_fn", f) \
                    esp_system_init_fn_##f = { .fn = ( __esp_system_init_fn_##f), .cores = (c) }; \
static __attribute__((used)) __VA_ARGS__ void __esp_system_init_fn_##f(void) // [refactor-todo] this can be made public API if we allow components to declare init functions,
                                                                             // instead of calling them explicitly

extern uint64_t g_startup_time;   // Startup time that serves as the point of origin for system time. Should be set by the entry
                                  // function in the port layer. May be 0 as well if this is not backed by a persistent counter, in which case
                                  // startup time = system time = 0 at the point the entry function sets this variable.

#ifdef __cplusplus
}
#endif
