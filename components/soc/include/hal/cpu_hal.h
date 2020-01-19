// Copyright 2020 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#include <stdint.h>
#include <stdbool.h>

#include "esp_err.h"

#include "hal/cpu_types.h"
#include "hal/cpu_ll.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Return the ID of the core currently executing this code.
 *
 * @return core id [0..SOC_CPU_CORES_NUM - 1]
 */
#define cpu_hal_get_core_id()           cpu_ll_get_core_id()

/**
 * Get the current value of the stack pointer.
 *
 * @return the current stack pointer
 */
#define cpu_hal_get_sp()                cpu_ll_get_sp()

/**
 * Get the current value of the internal counter that increments
 * every processor-clock cycle.
 *
 * @return cycle count; returns 0 if not supported
 */
#define cpu_hal_get_cycle_count()       cpu_ll_get_cycle_count()

/**
 * Check if some form of debugger is attached to CPU.
 *
 * @return true debugger is attached
 * @return false no debugger is attached/ no support for debuggers
 */
#define cpu_hal_is_debugger_attached()  cpu_ll_is_debugger_attached()

/**
 * Init HW loop status.
 */
#define cpu_hal_init_hwloop()           cpu_ll_init_hwloop()

/**
 * Trigger a call to debugger.
 */
#define cpu_hal_break()                 cpu_ll_break()

/**
 * Set and enable breakpoint at an instruction address.
 *
 * @note Overwrites previously set breakpoint with same breakpoint ID.
 *
 * @param id breakpoint to set [0..SOC_CPU_BREAKPOINT_NUM - 1]
 * @param addr address to set a breakpoint on
 *
 * @return ESP_ERR_INVALID_ARG invalid breakpoint id or addr
 * @return ESP_ERR_NOT_SUPPORTED processor does not support breakpoints
 * @return ESP_OK success
 */
esp_err_t cpu_hal_set_breakpoint(int id, const void* addr);

/**
 * Clear and disable breakpoint.
 *
 * @param id breakpoint to clear [0..SOC_CPU_BREAKPOINT_NUM - 1]
 *
 * @return ESP_ERR_INVALID_ARG invalid breakpoint id
 * @return ESP_ERR_NOT_SUPPORTED processor does not support breakpoints
 * @return ESP_OK success
 */
esp_err_t cpu_hal_clear_breakpoint(int id);

/**
 * Set and enable a watchpoint, specifying the memory range and trigger operation.
 *
 * @param id watchpoint to set [0..SOC_CPU_WATCHPOINT_NUM - 1]
 * @param addr starting address
 * @param size number of bytes from starting address to watch
 * @param trigger operation on specified memory range that triggers the watchpoint (read, write, read/write)
 *
 * @return ESP_ERR_INVALID_ARG invalid watchpoint id
 * @return ESP_ERR_NOT_SUPPORTED processor does not support watchpoints
 * @return ESP_OK success
 */
esp_err_t cpu_hal_set_watchpoint(int id, const void* addr, size_t size, watchpoint_trigger_t trigger);

/**
 * Clear and disable watchpoint.
 *
 * @param id watchpoint to clear [0..SOC_CPU_WATCHPOINT_NUM - 1]
 *
 * @return ESP_ERR_INVALID_ARG invalid watchpoint id
 * @return ESP_ERR_NOT_SUPPORTED processor does not support watchpoints
 * @return ESP_OK success
 */
esp_err_t cpu_hal_clear_watchpoint(int id);

#ifdef __cplusplus
}
#endif