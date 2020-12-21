/*
 * SPDX-FileCopyrightText: 2017-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef ESP_APP_TRACE_H_
#define ESP_APP_TRACE_H_

#include <stdarg.h>
#include "esp_err.h"
#include "esp_app_trace_util.h" // ESP_APPTRACE_TMO_INFINITE

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Application trace data destinations bits.
 */
typedef enum {
    ESP_APPTRACE_DEST_JTAG = 1,                         ///< JTAG destination
    ESP_APPTRACE_DEST_TRAX = ESP_APPTRACE_DEST_JTAG, 	///< xxx_TRAX name is obsolete, use more common xxx_JTAG
    ESP_APPTRACE_DEST_UART0,	                        ///< UART0 destination
    ESP_APPTRACE_DEST_MAX = ESP_APPTRACE_DEST_UART0,
    ESP_APPTRACE_DEST_NUM
} esp_apptrace_dest_t;

/**
 * @brief  Initializes application tracing module.
 *
 * @note   Should be called before any esp_apptrace_xxx call.
 *
 * @return ESP_OK on success, otherwise see esp_err_t
 */
esp_err_t esp_apptrace_init(void);

/**
 * @brief Configures down buffer.
 *        @note Needs to be called before initiating any data transfer using esp_apptrace_buffer_get and esp_apptrace_write.
 *              This function does not protect internal data by lock.
 *
 * @param buf Address of buffer to use for down channel (host to target) data.
 * @param size Size of the buffer.
 */
void esp_apptrace_down_buffer_config(uint8_t *buf, uint32_t size);

/**
 * @brief Allocates buffer for trace data.
 *        After data in buffer are ready to be sent off esp_apptrace_buffer_put must be called to indicate it.
 *
 * @param dest Indicates HW interface to send data.
 * @param size Size of data to write to trace buffer.
 * @param tmo  Timeout for operation (in us). Use ESP_APPTRACE_TMO_INFINITE to wait indefinitely.
 *
 * @return non-NULL on success, otherwise NULL.
 */
uint8_t *esp_apptrace_buffer_get(esp_apptrace_dest_t dest, uint32_t size, uint32_t tmo);

/**
 * @brief Indicates that the data in buffer are ready to be sent off.
 *        This function is a counterpart of and must be preceeded by esp_apptrace_buffer_get.
 *
 * @param dest Indicates HW interface to send data. Should be identical to the same parameter in call to esp_apptrace_buffer_get.
 * @param ptr  Address of trace buffer to release. Should be the value returned by call to esp_apptrace_buffer_get.
 * @param tmo  Timeout for operation (in us). Use ESP_APPTRACE_TMO_INFINITE to wait indefinitely.
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
 * @param tmo  Timeout for operation (in us). Use ESP_APPTRACE_TMO_INFINITE to wait indefinitely.
 *
 * @return ESP_OK on success, otherwise see esp_err_t
 */
esp_err_t esp_apptrace_write(esp_apptrace_dest_t dest, const void *data, uint32_t size, uint32_t tmo);

/**
 * @brief vprintf-like function to sent log messages to host via specified HW interface.
 *
 * @param dest Indicates HW interface to send data.
 * @param tmo  Timeout for operation (in us). Use ESP_APPTRACE_TMO_INFINITE to wait indefinitely.
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
 * @param tmo  Timeout for operation (in us). Use ESP_APPTRACE_TMO_INFINITE to wait indefinitely.
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
 * @param tmo    Timeout for operation (in us). Use ESP_APPTRACE_TMO_INFINITE to wait indefinitely.
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
 * @param tmo  Timeout for operation (in us). Use ESP_APPTRACE_TMO_INFINITE to wait indefinitely.
 *
 * @return ESP_OK on success, otherwise see esp_err_t
 */
esp_err_t esp_apptrace_read(esp_apptrace_dest_t dest, void *data, uint32_t *size, uint32_t tmo);

/**
 * @brief Retrieves incoming data buffer if any.
 *        After data in buffer are processed esp_apptrace_down_buffer_put must be called to indicate it.
 *
 * @param dest Indicates HW interface to receive data.
 * @param size Address to store size of available data in down buffer. Must be initialized with requested value.
 * @param tmo  Timeout for operation (in us). Use ESP_APPTRACE_TMO_INFINITE to wait indefinitely.
 *
 * @return non-NULL on success, otherwise NULL.
 */
uint8_t *esp_apptrace_down_buffer_get(esp_apptrace_dest_t dest, uint32_t *size, uint32_t tmo);

/**
 * @brief Indicates that the data in down buffer are processed.
 *        This function is a counterpart of and must be preceeded by esp_apptrace_down_buffer_get.
 *
 * @param dest Indicates HW interface to receive data. Should be identical to the same parameter in call to esp_apptrace_down_buffer_get.
 * @param ptr  Address of trace buffer to release. Should be the value returned by call to esp_apptrace_down_buffer_get.
 * @param tmo  Timeout for operation (in us). Use ESP_APPTRACE_TMO_INFINITE to wait indefinitely.
 *
 * @return ESP_OK on success, otherwise see esp_err_t
 */
esp_err_t esp_apptrace_down_buffer_put(esp_apptrace_dest_t dest, uint8_t *ptr, uint32_t tmo);

/**
 * @brief Checks whether host is connected.
 *
 * @param dest Indicates HW interface to use.
 *
 * @return true if host is connected, otherwise false
 */
bool esp_apptrace_host_is_connected(esp_apptrace_dest_t dest);

/**
 * @brief Opens file on host.
 *		  This function has the same semantic as 'fopen' except for the first argument.
 *
 * @param dest Indicates HW interface to use.
 * @param path Path to file.
 * @param mode Mode string. See fopen for details.
 *
 * @return non zero file handle on success, otherwise 0
 */
void *esp_apptrace_fopen(esp_apptrace_dest_t dest, const char *path, const char *mode);

/**
 * @brief Closes file on host.
 *		  This function has the same semantic as 'fclose' except for the first argument.
 *
 * @param dest   Indicates HW interface to use.
 * @param stream File handle returned by esp_apptrace_fopen.
 *
 * @return Zero on success, otherwise non-zero. See fclose for details.
 */
int esp_apptrace_fclose(esp_apptrace_dest_t dest, void *stream);

/**
 * @brief Writes to file on host.
 *		  This function has the same semantic as 'fwrite' except for the first argument.
 *
 * @param dest   Indicates HW interface to use.
 * @param ptr 	 Address of data to write.
 * @param size 	 Size of an item.
 * @param nmemb  Number of items to write.
 * @param stream File handle returned by esp_apptrace_fopen.
 *
 * @return Number of written items. See fwrite for details.
 */
size_t esp_apptrace_fwrite(esp_apptrace_dest_t dest, const void *ptr, size_t size, size_t nmemb, void *stream);

/**
 * @brief Read file on host.
 *		  This function has the same semantic as 'fread' except for the first argument.
 *
 * @param dest   Indicates HW interface to use.
 * @param ptr 	 Address to store read data.
 * @param size 	 Size of an item.
 * @param nmemb  Number of items to read.
 * @param stream File handle returned by esp_apptrace_fopen.
 *
 * @return Number of read items. See fread for details.
 */
size_t esp_apptrace_fread(esp_apptrace_dest_t dest, void *ptr, size_t size, size_t nmemb, void *stream);

/**
 * @brief Set position indicator in file on host.
 *		  This function has the same semantic as 'fseek' except for the first argument.
 *
 * @param dest   Indicates HW interface to use.
 * @param stream File handle returned by esp_apptrace_fopen.
 * @param offset Offset. See fseek for details.
 * @param whence Position in file. See fseek for details.
 *
 * @return Zero on success, otherwise non-zero. See fseek for details.
 */
int esp_apptrace_fseek(esp_apptrace_dest_t dest, void *stream, long offset, int whence);

/**
 * @brief Get current position indicator for file on host.
 *		  This function has the same semantic as 'ftell' except for the first argument.
 *
 * @param dest   Indicates HW interface to use.
 * @param stream File handle returned by esp_apptrace_fopen.
 *
 * @return Current position in file. See ftell for details.
 */
int esp_apptrace_ftell(esp_apptrace_dest_t dest, void *stream);

/**
 * @brief Indicates to the host that all file operations are completed.
 *		  This function should be called after all file operations are finished and
 *		  indicate to the host that it can perform cleanup operations (close open files etc.).
 *
 * @param dest   Indicates HW interface to use.
 *
 * @return ESP_OK on success, otherwise see esp_err_t
 */
int esp_apptrace_fstop(esp_apptrace_dest_t dest);

/**
 * @brief Triggers gcov info dump.
 *		  This function waits for the host to connect to target before dumping data.
 */
void esp_gcov_dump(void);

#ifdef __cplusplus
}
#endif

#endif
