// Copyright 2015-2019 Espressif Systems (Shanghai) PTE LTD
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
#ifndef ESP_CORE_DUMP_COMMON_H_
#define ESP_CORE_DUMP_COMMON_H_

#include "freertos/FreeRTOS.h"
#include "soc/cpu.h"
#include "esp_debug_helpers.h"
#include "esp_app_format.h"
#include "esp_core_dump_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Enumeration of the existing memory regions.
 * One can use these definitions to retrieve the start address and/or the size
 * of a specific region using the functions below.
 */
typedef enum {
    COREDUMP_MEMORY_DRAM,
    COREDUMP_MEMORY_IRAM,
    COREDUMP_MEMORY_RTC,
    COREDUMP_MEMORY_RTC_FAST,
    COREDUMP_MEMORY_MAX,
    COREDUMP_MEMORY_START = COREDUMP_MEMORY_DRAM
} coredump_region_t;

/**
 * @brief Get the (FreeRTOS) task handle for the current task.
 *
 * @return Task handle of the current task.
 */
core_dump_task_handle_t esp_core_dump_get_current_task_handle(void);

/**
 * @brief Get next task handle of a given handle.
 *
 * @param handle Task handle to get the next handle from.
 *
 * @return Next task handle.
 */
core_dump_task_handle_t esp_core_dump_get_next_task(core_dump_task_handle_t handle);

/**
 * @brief Get a task snapshot from a given handle.
 *
 * @param handle Task handle to get the snapshot from.
 * @param task Returned task header.
 * @param interrupted_stack Backup of the task stack if the handle passed is the task
 *                          that crashed and if it crashed within an ISR context.
 *
 * @return false is the task is broken, true else.
 */
bool esp_core_dump_get_task_snapshot(core_dump_task_handle_t handle,
                                     core_dump_task_header_t *task,
                                     core_dump_mem_seg_header_t *interrupted_stack);

/**
 * @brief Reset tasks snapshot iterator.
 */
void esp_core_dump_reset_tasks_snapshots_iter(void);


/**
 * @brief Check if the TCB passed as a parameter is sane.
 *
 * @param address Address of the TCB to check.
 *
 * @return true if the TCB is sane, false else.
 */
bool esp_core_dump_tcb_addr_is_sane(uint32_t addr);

/**
 * @brief Get the number of RAM segments.
 *
 * @return Number of RAM segments.
 */
uint32_t esp_core_dump_get_user_ram_segments(void);


/**
 * @brief Get start address and size of a memory region.
 *
 * @param region Memory region to get information about.
 * @param start  Pointer that will be filled with the region start address.
 *               Must **not** be NULL.
 *
 * @return Size, in bytes, of the memory region.
 */
int esp_core_dump_get_user_ram_info(coredump_region_t region, uint32_t *start);


/**
 * @brief Check if the current task is in an ISR.
 *
 * @return true if task in an ISR, false else.
 */
bool esp_core_dump_in_isr_context(void);


/**
 * @brief Get the size all the memory regions (DRAM, RTC, RTC_FAST, IRAM)
 *
 * @return Size, in bytes, of all the memory regions.
 */
uint32_t esp_core_dump_get_user_ram_size(void);


/**
 * @brief Get TCB length, in bytes.
 *
 * @return Length of TCB, in bytes.
 */
static inline uint32_t esp_core_dump_get_tcb_len(void)
{
    return (sizeof(StaticTask_t) % sizeof(uint32_t)) ?
           ((sizeof(StaticTask_t) / sizeof(uint32_t) + 1) * sizeof(uint32_t)) :
           sizeof(StaticTask_t);
}

/**
 * @brief Get the length, in bytes, of a given memory location. Padding is
 * taken into account in this calculation.
 *
 * @param start Start address of the momery location.
 * @param end End address of the memory location.
 *
 * @return Size of the memory location, multiple of sizeof(uint32_t).
 */
static inline uint32_t esp_core_dump_get_memory_len(uint32_t start, uint32_t end)
{
    const uint32_t len = end - start;
    // Take stack padding into account
    return (len + sizeof(uint32_t) - 1) & ~(sizeof(uint32_t) - 1);
}


#ifdef __cplusplus
}
#endif

#endif
