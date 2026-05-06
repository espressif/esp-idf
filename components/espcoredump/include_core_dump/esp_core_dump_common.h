/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef ESP_CORE_DUMP_COMMON_H_
#define ESP_CORE_DUMP_COMMON_H_

#include "freertos/FreeRTOS.h"
#include "freertos/freertos_debug.h"
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
    COREDUMP_MEMORY_IRAM,
#if CONFIG_ESP_COREDUMP_CAPTURE_DRAM
    COREDUMP_MEMORY_DRAM_BSS,
    COREDUMP_MEMORY_DRAM_DATA,
#if CONFIG_IDF_TARGET_ESP32P4
    COREDUMP_MEMORY_DRAM_BSS_HIGH,
    COREDUMP_MEMORY_DRAM_DATA_HIGH,
#endif
#else
    COREDUMP_MEMORY_DRAM,
#endif
#if SOC_RTC_MEM_SUPPORTED
    COREDUMP_MEMORY_RTC,
    COREDUMP_MEMORY_RTC_FAST,
#endif
    COREDUMP_MEMORY_NOINIT,
    COREDUMP_MEMORY_MAX,
    COREDUMP_MEMORY_START = COREDUMP_MEMORY_IRAM
} coredump_region_t;

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
 * @brief Prints write start info string according to destination.
 */
void esp_core_dump_print_write_start(void);

/**
 * @brief Prints write end info string according to destination.
 */
void esp_core_dump_print_write_end(void);

/**
 * @brief Initializes the flash/UART hardware for data storage.
 */
esp_err_t esp_core_dump_write_init(void);

/**
 * @brief Prepares the flash/UART for data storage
 */
esp_err_t esp_core_dump_write_prepare(core_dump_write_data_t *wr_data, uint32_t *data_len);

/**
 * @brief Initiates the beginning of data writing.
 */
esp_err_t esp_core_dump_write_start(core_dump_write_data_t *wr_data);

/**
 * @brief Writes a data chunk to the flash/UART
 */
esp_err_t esp_core_dump_write_data(core_dump_write_data_t *wr_data, void *data, uint32_t data_len);

/**
 * @brief Finalizes the data writing process
 */
esp_err_t esp_core_dump_write_end(core_dump_write_data_t *wr_data);

/**
 * @brief Stores the core dump in ELF format.
 */
esp_err_t esp_core_dump_write_elf(void);

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
 * @brief Get the (FreeRTOS) task handle for the current task.
 *
 * @return Task handle of the current task.
 */
static inline core_dump_task_handle_t esp_core_dump_get_current_task_handle(void)
{
    return (core_dump_task_handle_t) xTaskGetCurrentTaskHandleForCore(xPortGetCoreID());
}

/**
 * @brief Get the length, in bytes, of a given memory location. Padding is
 * taken into account in this calculation.
 *
 * @param start Start address of the memory location.
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

/**
 * @brief Initialize the task iterator to start traversing task lists.
 */
static inline void esp_core_dump_task_iterator_init(TaskIterator_t *iter)
{
    if (iter) {
        iter->uxCurrentListIndex = 0;
        iter->pxNextListItem = NULL;
        iter->pxTaskHandle = NULL;
    }
}

/**
 * @brief Get the next task using the task iterator
 *
 * This function retrieves the next task in the traversal sequence.
 *
 * @param task_iterator Pointer to the task iterator structure.
 *
 * @return The index of the current task list. Returns -1 if all tasks have been traversed.
 */
static inline int esp_core_dump_task_iterator_next(TaskIterator_t *task_iterator)
{
    return xTaskGetNext(task_iterator);
}

#ifdef __cplusplus
}
#endif

#endif
