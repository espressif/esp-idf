/*
 * SPDX-FileCopyrightText: 2017-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef ESP_APP_TRACE_H_
#define ESP_APP_TRACE_H_

#include <stdarg.h>
#include "esp_err.h"
#include "esp_app_trace_config.h"
#include "esp_app_trace_util.h" // ESP_APPTRACE_TMO_INFINITE

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief  Initializes application tracing module for the selected destination and configuration.
 *
 * @note   Should be called before any esp_apptrace_xxx call.
 *
 * @return ESP_OK on success, otherwise see esp_err_t
 */
esp_err_t esp_apptrace_init(const esp_apptrace_config_t *config);

/**
 * @brief Configures down buffer.
 *
 * @note Needs to be called before attempting to receive any data using esp_apptrace_down_buffer_get and
 *       esp_apptrace_read. This function does not protect internal data by lock.
 *
 * @param buf Address of buffer to use for down channel (host to target) data.
 * @param size Size of the buffer.
 *
 * @return ESP_OK on success, otherwise see esp_err_t
 */
esp_err_t esp_apptrace_down_buffer_config(uint8_t *buf, uint32_t size);

/**
 * @brief Allocates buffer for trace data.
 *        Once the data in the buffer is ready to be sent, esp_apptrace_buffer_put must be called to indicate it.
 *
 * @param size Size of data to write to trace buffer.
 * @param tmo  Timeout for operation (in us). Use ESP_APPTRACE_TMO_INFINITE to wait indefinitely.
 *
 * @return non-NULL on success, otherwise NULL.
 */
uint8_t *esp_apptrace_buffer_get(uint32_t size, uint32_t tmo);

/**
 * @brief Indicates that the data in the buffer is ready to be sent.
 *        This function is a counterpart of and must be preceded by esp_apptrace_buffer_get.
 *
 * @param ptr  Address of trace buffer to release. Should be the value returned by call to esp_apptrace_buffer_get.
 * @param tmo  Timeout for operation (in us). Use ESP_APPTRACE_TMO_INFINITE to wait indefinitely.
 *
 * @return ESP_OK on success, otherwise see esp_err_t
 */
esp_err_t esp_apptrace_buffer_put(uint8_t *ptr, uint32_t tmo);

/**
 * @brief  Writes data to trace buffer.
 *
 * @param data Address of data to write to trace buffer.
 * @param size Size of data to write to trace buffer.
 * @param tmo  Timeout for operation (in us). Use ESP_APPTRACE_TMO_INFINITE to wait indefinitely.
 *
 * @return ESP_OK on success, otherwise see esp_err_t
 */
esp_err_t esp_apptrace_write(const void *data, uint32_t size, uint32_t tmo);

/**
 * @brief vprintf-like function to send log messages to host via specified HW interface.
 *
 * @param tmo  Timeout for operation (in us). Use ESP_APPTRACE_TMO_INFINITE to wait indefinitely.
 * @param fmt  Address of format string.
 * @param ap   List of arguments.
 *
 * @return Number of bytes written.
 */
int esp_apptrace_vprintf_to(uint32_t tmo, const char *fmt, va_list ap);

/**
 * @brief vprintf-like function to send log messages to host.
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
 * @param tmo  Timeout for operation (in us). Use ESP_APPTRACE_TMO_INFINITE to wait indefinitely.
 *
 * @return ESP_OK on success, otherwise see esp_err_t
 */
esp_err_t esp_apptrace_flush(uint32_t tmo);

/**
 * @brief Flushes remaining data in trace buffer to host without locking internal data.
 *        This is a special version of esp_apptrace_flush which should be called from panic handler.
 *
 * @param min_sz Threshold for flushing data. If current filling level is above this value, data will be flushed. JTAG destinations only.
 * @param tmo    Timeout for operation (in us). Use ESP_APPTRACE_TMO_INFINITE to wait indefinitely.
 *
 * @return ESP_OK on success, otherwise see esp_err_t
 */
esp_err_t esp_apptrace_flush_nolock(uint32_t min_sz, uint32_t tmo);

/**
 * @brief Reads host data from trace buffer.
 *
 * @param data Address of buffer to put data from trace buffer.
 * @param size Pointer to store size of read data. Before call to this function pointed memory must hold requested size of data
 * @param tmo  Timeout for operation (in us). Use ESP_APPTRACE_TMO_INFINITE to wait indefinitely.
 *
 * @return ESP_OK on success, otherwise see esp_err_t
 */
esp_err_t esp_apptrace_read(void *data, uint32_t *size, uint32_t tmo);

/**
 * @brief Retrieves incoming data buffer if any.
 *        Once data in the buffer is processed, esp_apptrace_down_buffer_put must be called to indicate it.
 *
 * @param size Address to store size of available data in down buffer. Must be initialized with requested value.
 * @param tmo  Timeout for operation (in us). Use ESP_APPTRACE_TMO_INFINITE to wait indefinitely.
 *
 * @return non-NULL on success, otherwise NULL.
 */
uint8_t *esp_apptrace_down_buffer_get(uint32_t *size, uint32_t tmo);

/**
 * @brief Indicates that the data in the down buffer is processed.
 *        This function is a counterpart of and must be preceded by esp_apptrace_down_buffer_get.
 *
 * @param ptr  Address of trace buffer to release. Should be the value returned by call to esp_apptrace_down_buffer_get.
 * @param tmo  Timeout for operation (in us). Use ESP_APPTRACE_TMO_INFINITE to wait indefinitely.
 *
 * @return ESP_OK on success, otherwise see esp_err_t
 */
esp_err_t esp_apptrace_down_buffer_put(uint8_t *ptr, uint32_t tmo);

/**
 * @brief Checks whether host is connected.
 *
 * @return true if host is connected, otherwise false
 */
bool esp_apptrace_host_is_connected(void);

/**
 * @brief Gets the destination of the application trace.
 *
 * @return The destination of the application trace.
 */
esp_apptrace_dest_t esp_apptrace_get_destination(void);

/**
 * @brief Sets the header size of the application trace packet.
 *
 * @param header_size The header size to set.
 *
 * @return ESP_OK on success, otherwise see esp_err_t
 */
esp_err_t esp_apptrace_set_header_size(esp_apptrace_header_size_t header_size);

/**
 * @brief Opens file on host.
 *        This function has the same semantic as 'fopen' except for the first argument.
 *
 * @param path Path to file.
 * @param mode Mode string. See fopen for details.
 *
 * @return non zero file handle on success, otherwise 0
 */
void *esp_apptrace_fopen(const char *path, const char *mode);

/**
 * @brief Closes file on host.
 *        This function has the same semantic as 'fclose' except for the first argument.
 *
 * @param stream File handle returned by esp_apptrace_fopen.
 *
 * @return Zero on success, otherwise non-zero. See fclose for details.
 */
int esp_apptrace_fclose(void *stream);

/**
 * @brief Writes to file on host.
 *        This function has the same semantic as 'fwrite' except for the first argument.
 *
 * @param ptr    Address of data to write.
 * @param size   Size of an item.
 * @param nmemb  Number of items to write.
 * @param stream File handle returned by esp_apptrace_fopen.
 *
 * @return Number of written items. See fwrite for details.
 */
size_t esp_apptrace_fwrite(const void *ptr, size_t size, size_t nmemb, void *stream);

/**
 * @brief Read file on host.
 *        This function has the same semantic as 'fread' except for the first argument.
 *
 * @param ptr    Address to store read data.
 * @param size   Size of an item.
 * @param nmemb  Number of items to read.
 * @param stream File handle returned by esp_apptrace_fopen.
 *
 * @return Number of read items. See fread for details.
 */
size_t esp_apptrace_fread(void *ptr, size_t size, size_t nmemb, void *stream);

/**
 * @brief Set position indicator in file on host.
 *        This function has the same semantic as 'fseek' except for the first argument.
 *
 * @param stream File handle returned by esp_apptrace_fopen.
 * @param offset Offset. See fseek for details.
 * @param whence Position in file. See fseek for details.
 *
 * @return Zero on success, otherwise non-zero. See fseek for details.
 */
int esp_apptrace_fseek(void *stream, long offset, int whence);

/**
 * @brief Get current position indicator for file on host.
 *        This function has the same semantic as 'ftell' except for the first argument.
 *
 * @param stream File handle returned by esp_apptrace_fopen.
 *
 * @return Current position in file. See ftell for details.
 */
int esp_apptrace_ftell(void *stream);

/**
 * @brief Indicates to the host that all file operations are complete.
 *        This function should be called after all file operations are finished and
 *        indicate to the host that it can perform cleanup operations (close open files etc.).
 *
 * @return ESP_OK on success, otherwise see esp_err_t
 */
int esp_apptrace_fstop(void);

/**
 * @brief Test end-of-file indicator on a stream.
 *        This function has the same semantic as 'feof' except for the first argument.
 *
 * @param stream File handle returned by esp_apptrace_fopen.
 *
 * @return Non-Zero if end-of-file indicator is set for stream. See feof for details.
 */
int esp_apptrace_feof(void *stream);

#if !CONFIG_APPTRACE_DEST_UART // JTAG or NONE
#define APPTRACE_JTAG_CONFIG_DEFAULT() {                 \
    .dest = ESP_APPTRACE_DEST_JTAG,                      \
    .dest_cfg.jtag = {0},                                \
    .flush_tmo    = CONFIG_APPTRACE_ONPANIC_HOST_FLUSH_TMO, \
    .flush_thresh = CONFIG_APPTRACE_POSTMORTEM_FLUSH_THRESH, \
}
#endif

#if !CONFIG_APPTRACE_DEST_JTAG // UART or NONE
#define APPTRACE_UART_CONFIG_DEFAULT() {                    \
    .dest = ESP_APPTRACE_DEST_UART,                         \
    .dest_cfg.uart = {                                      \
        .uart_num     = CONFIG_APPTRACE_DEST_UART_NUM,      \
        .tx_pin_num   = CONFIG_APPTRACE_UART_TX_GPIO,       \
        .rx_pin_num   = CONFIG_APPTRACE_UART_RX_GPIO,       \
        .baud_rate    = CONFIG_APPTRACE_UART_BAUDRATE,      \
        .tx_buff_size = CONFIG_APPTRACE_UART_TX_BUFF_SIZE,  \
        .tx_msg_size  = CONFIG_APPTRACE_UART_TX_MSG_SIZE,   \
        .task_prio    = CONFIG_APPTRACE_UART_TASK_PRIO,     \
    },                                                      \
    .flush_tmo    = CONFIG_APPTRACE_ONPANIC_HOST_FLUSH_TMO, \
    .flush_thresh = CONFIG_APPTRACE_POSTMORTEM_FLUSH_THRESH, \
}
#endif

// Default picks JTAG if available, otherwise UART
#if !CONFIG_APPTRACE_DEST_UART
#define APPTRACE_CONFIG_DEFAULT() APPTRACE_JTAG_CONFIG_DEFAULT()
#else
#define APPTRACE_CONFIG_DEFAULT() APPTRACE_UART_CONFIG_DEFAULT()
#endif

#ifdef __cplusplus
}
#endif

#endif
