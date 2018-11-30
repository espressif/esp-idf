// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
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
 * @brief Initialise heap tracing in standalone mode.
 * @note Standalone mode is the only mode currently supported.
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
 * @brief Return number of records in the heap trace buffer
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
 *
 *
 */
void heap_trace_dump(void);

#ifdef __cplusplus
}
#endif
