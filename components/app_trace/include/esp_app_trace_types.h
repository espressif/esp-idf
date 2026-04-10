/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef ESP_APP_TRACE_TYPES_H_
#define ESP_APP_TRACE_TYPES_H_

#include <stdint.h>
#include "spinlock.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Structure which holds data necessary for measuring time intervals.
 *
 *  After initialization via esp_apptrace_tmo_init() user needs to call esp_apptrace_tmo_check()
 *  periodically to check timeout for expiration.
 */
typedef struct {
    int64_t   start;   ///< time interval start (in us)
    int64_t   tmo;     ///< timeout value (in us)
    int64_t   elapsed; ///< elapsed time (in us)
} esp_apptrace_tmo_t;

/** Tracing module synchronization lock */
typedef struct {
    spinlock_t mux;
    unsigned int int_state;
} esp_apptrace_lock_t;

/** Ring buffer control structure.
 *
 * @note For purposes of application tracing module if there is no enough space for user data and write pointer can be wrapped
 *       current ring buffer size can be temporarily shrunk in order to provide buffer with requested size.
 */
typedef struct {
    uint8_t *data;      ///< pointer to data storage
    volatile uint32_t size;      ///< size of data storage
    volatile uint32_t cur_size;  ///< current size of data storage
    volatile uint32_t rd;        ///< read pointer
    volatile uint32_t wr;        ///< write pointer
} esp_apptrace_rb_t;

/**
 * Application trace data destinations
 */
typedef enum {
    ESP_APPTRACE_DEST_JTAG,
    ESP_APPTRACE_DEST_UART,
} esp_apptrace_dest_t;

/**
 * Application trace configuration for UART destination
 */
typedef struct {
    int uart_num;           ///< Port number
    int tx_pin_num;         ///< TX pin number
    int rx_pin_num;         ///< RX pin number
    int baud_rate;          ///< Baud rate
    uint32_t tx_buff_size;  ///< TX ring buffer size
    uint32_t tx_msg_size;   ///< Maximum size of the single message to transfer.
} esp_apptrace_uart_config_t;

/**
 * Application trace trace header size in bytes. It is 2 bytes for SEGGER SystemView
 */
typedef enum {
    ESP_APPTRACE_HEADER_SIZE_16 = 2,
    ESP_APPTRACE_HEADER_SIZE_32 = 4,
} esp_apptrace_header_size_t;

/**
 * Application trace configuration
 */
typedef struct {
    esp_apptrace_dest_t dest; ///< Destination type (JTAG or UART)

    union {
        esp_apptrace_uart_config_t uart; ///< UART configuration (when dest is ESP_APPTRACE_DEST_UART)
        struct {                         ///< Reserved for JTAG (when dest is ESP_APPTRACE_DEST_JTAG)
            uint8_t _unused;
        } jtag;
    } dest_cfg; ///< Destination-specific configuration

    uint32_t flush_tmo;    ///< Flush timeout in milliseconds
    uint32_t flush_thresh; ///< Flush threshold in bytes
} esp_apptrace_config_t;

#ifdef __cplusplus
}
#endif

#endif /* ESP_APP_TRACE_TYPES_H_ */
