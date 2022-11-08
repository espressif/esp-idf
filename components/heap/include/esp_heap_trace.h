/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "sdkconfig.h"
#include <stdint.h>
#include <esp_err.h>

#ifdef __cplusplus
extern "C" {
#endif

#if !defined(CONFIG_HEAP_TRACING) && !defined(HEAP_TRACE_SRCFILE)
#warning "esp_heap_trace.h is included but heap tracing is disabled in menuconfig, functions are no-ops"
#endif

#ifndef CONFIG_HEAP_TRACING_STACK_DEPTH
#define CONFIG_HEAP_TRACING_STACK_DEPTH 0
#endif

typedef enum {
    HEAP_TRACE_ALL,
    HEAP_TRACE_LEAKS,
} heap_trace_mode_t;

/**
 * @brief Trace record data type. Stores information about an allocated region of memory.
 */
typedef struct {
    uint32_t ccount; ///< CCOUNT of the CPU when the allocation was made. LSB (bit value 1) is the CPU number (0 or 1).
    void *address;   ///< Address which was allocated
    size_t size;     ///< Size of the allocation
    void *alloced_by[CONFIG_HEAP_TRACING_STACK_DEPTH]; ///< Call stack of the caller which allocated the memory.
    void *freed_by[CONFIG_HEAP_TRACING_STACK_DEPTH];   ///< Call stack of the caller which freed the memory (all zero if not freed.)
} heap_trace_record_t;

/**
 * @brief Stores information about the result of a heap trace.
 */
typedef struct {
    heap_trace_mode_t mode;          ///< The heap trace mode we just completed / are running
    size_t total_allocations;        ///< The total number of allocations made during tracing
    size_t total_frees;              ///< The total number of frees made during tracing
    size_t internal_count;           ///< The number of records in the internal buffer
    size_t internal_capacity;        ///< The capacity of the internal buffer
    size_t internal_high_water_mark; ///< The maximum value that 'count' got to
    size_t internal_overflowed;      ///< True if the internal buffer overflowed
    size_t spi_count;                ///< The number of records in the spi buffer
    size_t spi_capacity;             ///< The capacity of the spi buffer
    size_t spi_high_water_mark;      ///< The maximum value that 'count' got to
    size_t spi_overflowed;           ///< True if the spi buffer overflowed
} heap_trace_summary_t;

/**
 * @brief Initialise heap tracing in standalone mode.
 *
 * This function must be called before any other heap tracing functions.
 *
 * To disable heap tracing and allow the buffer to be freed, stop tracing and then call heap_trace_init_standalone(NULL, 0);
 *
 * @param record_buffer Provide a buffer to use for heap trace data. Must remain valid any time heap tracing is enabled, meaning
 * it must be allocated from internal memory not in PSRAM.
 * @param num_records Size of the heap trace buffer, as number of record structures.
 * @return
 *  - ESP_ERR_NOT_SUPPORTED Project was compiled without heap tracing enabled in menuconfig.
 *  - ESP_ERR_INVALID_STATE Heap tracing is currently in progress.
 *  - ESP_OK Heap tracing initialised successfully.
 */
esp_err_t heap_trace_init_standalone(heap_trace_record_t *record_buffer, size_t num_records);

/**
 * @brief Initialise heap tracing in standalone mode with support for recording to SPI RAM
 *
 * This function must be called before any other heap tracing functions.
 *
 * To disable heap tracing and allow the buffer to be freed, stop tracing and then call heap_trace_init_standalone(NULL, 0);
 *
 * @param internal_record_buffer Provide a buffer to use for temporarily storing heap trace data. 
 * Must remain valid any time heap tracing is enabled, meaning
 * it must be allocated from internal memory not in PSRAM.
 * @param internal_num_records Size of the internal heap trace buffer, as number of record structures.
  * @param spi_record_buffer Provide a buffer to use for storing heap trace data. It should be allocated in SPI RAM.
 * @param spi_num_records Size of the SPI RAM heap trace buffer, as number of record structures.
 * @return
 *  - ESP_ERR_NOT_SUPPORTED Project was compiled without heap tracing enabled in menuconfig.
 *  - ESP_ERR_INVALID_STATE Heap tracing is currently in progress.
 *  - ESP_OK Heap tracing initialised successfully.
 */
esp_err_t heap_trace_init_standalone_with_spi_ram(
    heap_trace_record_t *internal_record_buffer, size_t internal_num_records,
    heap_trace_record_t *spi_record_buffer, size_t spi_num_records);

/**
 * @brief Initialise heap tracing in host-based mode.
 *
 * This function must be called before any other heap tracing functions.
 *
 * @return
 *  - ESP_ERR_INVALID_STATE Heap tracing is currently in progress.
 *  - ESP_OK Heap tracing initialised successfully.
 */
esp_err_t heap_trace_init_tohost(void);

/**
 * @brief Start heap tracing. All heap allocations & frees will be traced, until heap_trace_stop() is called.
 *
 * @note heap_trace_init_standalone() must be called to provide a valid buffer, before this function is called.
 *
 * @note Calling this function while heap tracing is running will reset the heap trace state and continue tracing.
 *
 * @param mode Mode for tracing.
 * - HEAP_TRACE_ALL means all heap allocations and frees are traced.
 * - HEAP_TRACE_LEAKS means only suspected memory leaks are traced. (When memory is freed, the record is removed from the trace buffer.)
 * @return
 * - ESP_ERR_NOT_SUPPORTED Project was compiled without heap tracing enabled in menuconfig.
 * - ESP_ERR_INVALID_STATE A non-zero-length buffer has not been set via heap_trace_init_standalone().
 * - ESP_OK Tracing is started.
 */
esp_err_t heap_trace_start(heap_trace_mode_t mode);

/**
 * @brief Stop heap tracing.
 *
 * @return
 * - ESP_ERR_NOT_SUPPORTED Project was compiled without heap tracing enabled in menuconfig.
 * - ESP_ERR_INVALID_STATE Heap tracing was not in progress.
 * - ESP_OK Heap tracing stopped..
 */
esp_err_t heap_trace_stop(void);

/**
 * @brief Resume heap tracing which was previously stopped.
 *
 * Unlike heap_trace_start(), this function does not clear the
 * buffer of any pre-existing trace records.
 *
 * The heap trace mode is the same as when heap_trace_start() was
 * last called (or HEAP_TRACE_ALL if heap_trace_start() was never called).
 *
 * @return
 * - ESP_ERR_NOT_SUPPORTED Project was compiled without heap tracing enabled in menuconfig.
 * - ESP_ERR_INVALID_STATE Heap tracing was already started.
 * - ESP_OK Heap tracing resumed.
 */
esp_err_t heap_trace_resume(void);

/**
 * @brief Return number of records in the heap trace buffer.
 *
 * It is safe to call this function while heap tracing is running.
 */
size_t heap_trace_get_count(void);

/**
 * @brief Return a raw record from the heap trace buffer
 *
 * @note It is safe to call this function while heap tracing is running, however in HEAP_TRACE_LEAK mode record indexing may
 * skip entries unless heap tracing is stopped first.
 *
 * @param index Index (zero-based) of the record to return.
 * @param[out] record Record where the heap trace record will be copied.
 * @return
 * - ESP_ERR_NOT_SUPPORTED Project was compiled without heap tracing enabled in menuconfig.
 * - ESP_ERR_INVALID_STATE Heap tracing was not initialised.
 * - ESP_ERR_INVALID_ARG Index is out of bounds for current heap trace record count.
 * - ESP_OK Record returned successfully.
 */
esp_err_t heap_trace_get(size_t index, heap_trace_record_t *record);

/**
 * @brief Dump heap trace record data to stdout
 *
 * @note It is safe to call this function while heap tracing is running, however in HEAP_TRACE_LEAK mode the dump may skip
 * entries unless heap tracing is stopped first.
 */
void heap_trace_dump(void);

/**
 * @brief Same as heap_trace_dump() but will only log allocations in Internal-RAM 
 *
 * @note It is safe to call this function while heap tracing is running, however in HEAP_TRACE_LEAK mode the dump may skip
 * entries unless heap tracing is stopped first.
 */
void heap_trace_dump_internal_ram_only(void);

/**
 * @brief Same as heap_trace_dump() but will only log allocations in SPI-RAM
 *
 * @note It is safe to call this function while heap tracing is running, however in HEAP_TRACE_LEAK mode the dump may skip
 * entries unless heap tracing is stopped first.
 */
void heap_trace_dump_spi_ram_only(void);

/**
 * @brief Get summary information about the result of a heap trace
 * 
 *  @note It is safe to call this function while heap tracing is running.
 */
esp_err_t heap_trace_summary(heap_trace_summary_t* summary);

#ifdef __cplusplus
}
#endif
