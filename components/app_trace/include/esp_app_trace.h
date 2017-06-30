// Copyright 2017 Espressif Systems (Shanghai) PTE LTD
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
#ifndef ESP_APP_TRACE_H_
#define ESP_APP_TRACE_H_

#include <stdarg.h>
#include "esp_err.h"
#include "freertos/portmacro.h"

/** Infinite waiting timeout */
#define ESP_APPTRACE_TMO_INFINITE               ((uint32_t)-1)

/**
 * Application trace data destinations bits.
 */
typedef enum {
    ESP_APPTRACE_DEST_TRAX = 0x1,	///< JTAG destination
    ESP_APPTRACE_DEST_UART0 = 0x2,	///< UART destination
} esp_apptrace_dest_t;

/**
 * @brief  Initializes application tracing module.
 *
 * @note   Should be called before any esp_apptrace_xxx call.
 *
 * @return ESP_OK on success, otherwise see esp_err_t
 */
esp_err_t esp_apptrace_init();

/**
 * @brief Allocates buffer for trace data.
 *        After data in buffer are ready to be sent off esp_apptrace_buffer_put must be called to indicate it.
 *
 * @param dest Indicates HW interface to send data.
 * @param size Size of data to write to trace buffer.
 * @param tmo  Timeout for operation (in us). Use ESP_APPTRACE_TMO_INFINITE to wait indefinetly.
 *
 * @return non-NULL on success, otherwise NULL.
 */
uint8_t *esp_apptrace_buffer_get(esp_apptrace_dest_t dest, size_t size, uint32_t tmo);

/**
 * @brief Indicates that the data in buffer are ready to be sent off.
 *        This function is a counterpart of must be preceeded by esp_apptrace_buffer_get.
 *
 * @param dest Indicates HW interface to send data. Should be identical to the same parameter in call to esp_apptrace_buffer_get.
 * @param ptr  Address of trace buffer to release. Should be the value returned by call to esp_apptrace_buffer_get.
 * @param tmo  Timeout for operation (in us). Use ESP_APPTRACE_TMO_INFINITE to wait indefinetly.
 *
 * @return ESP_OK on success, otherwise see esp_err_t
 */
esp_err_t esp_apptrace_buffer_put(esp_apptrace_dest_t dest, uint8_t *ptr, uint32_t tmo);

/**
 * @brief  Writes data to trace buffer.
 *
 * @param dest Indicates HW interface to send data.
 * @param data Address of data to write to trace buffer.
 * @param size Size of data to write to trace buffer.
 * @param tmo  Timeout for operation (in us). Use ESP_APPTRACE_TMO_INFINITE to wait indefinetly.
 *
 * @return ESP_OK on success, otherwise see esp_err_t
 */
esp_err_t esp_apptrace_write(esp_apptrace_dest_t dest, const void *data, size_t size, uint32_t tmo);

/**
 * @brief vprintf-like function to sent log messages to host via specified HW interface.
 *
 * @param dest Indicates HW interface to send data.
 * @param tmo  Timeout for operation (in us). Use ESP_APPTRACE_TMO_INFINITE to wait indefinetly.
 * @param fmt  Address of format string.
 * @param ap   List of arguments.
 *
 * @return Number of bytes written.
 */
int esp_apptrace_vprintf_to(esp_apptrace_dest_t dest, uint32_t tmo, const char *fmt, va_list ap);

/**
 * @brief vprintf-like function to sent log messages to host.
 *
 * @param fmt  Address of format string.
 * @param ap   List of arguments.
 *
 * @return Number of bytes written.
 */
int esp_apptrace_vprintf(const char *fmt, va_list ap);

/**
 * @brief Flushes remaining data in trace buffer to host.
 *
 * @param dest Indicates HW interface to flush data on.
 * @param tmo  Timeout for operation (in us). Use ESP_APPTRACE_TMO_INFINITE to wait indefinetly.
 *
 * @return ESP_OK on success, otherwise see esp_err_t
 */
esp_err_t esp_apptrace_flush(esp_apptrace_dest_t dest, uint32_t tmo);

/**
 * @brief Flushes remaining data in trace buffer to host without locking internal data.
 *        This is special version of esp_apptrace_flush which should be called from panic handler.
 *
 * @param dest   Indicates HW interface to flush data on.
 * @param min_sz Threshold for flushing data. If current filling level is above this value, data will be flushed. TRAX destinations only.
 * @param tmo    Timeout for operation (in us). Use ESP_APPTRACE_TMO_INFINITE to wait indefinetly.
 *
 * @return ESP_OK on success, otherwise see esp_err_t
 */
esp_err_t esp_apptrace_flush_nolock(esp_apptrace_dest_t dest, uint32_t min_sz, uint32_t tmo);

/**
 * @brief Reads host data from trace buffer.
 *
 * @param dest Indicates HW interface to read the data on.
 * @param data Address of buffer to put data from trace buffer.
 * @param size Pointer to store size of read data. Before call to this function pointed memory must hold requested size of data
 * @param tmo  Timeout for operation (in us). Use ESP_APPTRACE_TMO_INFINITE to wait indefinetly.
 *
 * @return ESP_OK on success, otherwise see esp_err_t
 */
esp_err_t esp_apptrace_read(esp_apptrace_dest_t dest, void *data, size_t *size, uint32_t tmo);


#endif
