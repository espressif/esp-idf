/*
 * SPDX-FileCopyrightText: 2018-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef ESP_SYSVIEW_TRACE_H_
#define ESP_SYSVIEW_TRACE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdarg.h>
#include "esp_err.h"
#include "SEGGER_RTT.h" // SEGGER_RTT_ESP_Flush
#include "esp_app_trace_util.h" // ESP_APPTRACE_TMO_INFINITE

/**
 * @brief Flushes remaining data in SystemView trace buffer to host.
 *
 * @param tmo  Timeout for operation (in us). Use ESP_APPTRACE_TMO_INFINITE to wait indefinetly.
 *
 * @return ESP_OK.
 */
static inline esp_err_t esp_sysview_flush(uint32_t tmo)
{
    SEGGER_RTT_ESP_Flush(0, tmo);
    return ESP_OK;
}

/**
 * @brief vprintf-like function to sent log messages to the host.
 *
 * @param format  Address of format string.
 * @param args    List of arguments.
 *
 * @return Number of bytes written.
 */
int esp_sysview_vprintf(const char * format, va_list args);

/**
 * @brief  Starts SystemView heap tracing.
 *
 * @param tmo Timeout (in us) to wait for the host to be connected. Use -1 to wait forever.
 *
 * @return ESP_OK on success, ESP_ERR_TIMEOUT if operation has been timed out.
 */
esp_err_t esp_sysview_heap_trace_start(uint32_t tmo);

/**
 * @brief  Stops SystemView heap tracing.
 *
 * @return ESP_OK.
 */
esp_err_t esp_sysview_heap_trace_stop(void);

/**
 * @brief Sends heap allocation event to the host.
 *
 * @param addr      Address of allocated block.
 * @param size      Size of allocated block.
 * @param callers   Pointer to array with callstack addresses.
 *                  Array size must be CONFIG_HEAP_TRACING_STACK_DEPTH.
 */
void esp_sysview_heap_trace_alloc(void *addr, uint32_t size, const void *callers);

/**
 * @brief Sends heap de-allocation event to the host.
 *
 * @param addr      Address of de-allocated block.
 * @param callers   Pointer to array with callstack addresses.
 *                  Array size must be CONFIG_HEAP_TRACING_STACK_DEPTH.
 */
void esp_sysview_heap_trace_free(void *addr, const void *callers);

#ifdef __cplusplus
}
#endif

#endif //ESP_SYSVIEW_TRACE_H_
