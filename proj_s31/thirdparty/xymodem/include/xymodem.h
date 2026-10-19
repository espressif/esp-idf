/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#include <stddef.h>
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ESP_ERR_XYMODEM_BASE           0x1E000
#define ESP_ERR_XYMODEM_CANCEL_BY_PEER (ESP_ERR_XYMODEM_BASE + 1)  ///< Cancelled by the peer (two consecutive CAN bytes).
#define ESP_ERR_XYMODEM_RETRY_EXCEEDED (ESP_ERR_XYMODEM_BASE + 2)  ///< Retry limit reached.
#define ESP_ERR_XYMODEM_BAD_PACKET     (ESP_ERR_XYMODEM_BASE + 3)  ///< Received data is not a valid packet.
#define ESP_ERR_XYMODEM_NO_RESPONSE    (ESP_ERR_XYMODEM_BASE + 4)  ///< Peer did not respond within the timeout.
#define ESP_ERR_XYMODEM_OUT_OF_SYNC    (ESP_ERR_XYMODEM_BASE + 5)  ///< Packet sequence number is neither expected nor previous.

/**
 * @brief The abstraction of the low-level communication channel used by X/YMODEM protocol layer.
 */
typedef struct {
    /**
     * @brief Receive data through some interface, such as a serial port.
     *
     * @param[in] ctx User-defined context.
     * @param[out] buf Buffer to store received data.
     * @param len Requested length of data to receive.
     * @param[out] recv_len Actual length of data received.
     * @param timeout_ms Timeout in milliseconds. 0 means non-blocking.
     *
     * @return
     *   - ESP_OK if the requested amount of data is received in full, or if the timeout expires.
     *   - others if an error occurs during the transfer. Timeout is NOT considered as an error.
     *
     * @note This function attempts to receive the full requested amount within the timeout period.
     *       If the timeout is reached, return whatever data has been received. Returning early with
     *       a short read before the timeout is not allowed.
     */
    esp_err_t (*recv)(void *ctx, void *buf, size_t len, size_t *recv_len, uint32_t timeout_ms);
    /**
     * @brief Send data through some interface, such as a serial port.
     *
     * @param[in] ctx User-defined context.
     * @param[in] buf Buffer to send.
     * @param len Length of data to send.
     *
     * @return
     *   - ESP_OK if all data was successfully sent or queued for transmission.
     *   - others if an error occurs during the transfer.
     */
    esp_err_t (*send)(void *ctx, const void *buf, size_t len);
    /**
     * @brief User-defined context passed to `recv()` and `send()`.
     */
    void *ctx;
} xymodem_transport_t;

/**
 * @brief Read data from a source, such as a file or a simple array.
 *        The protocol layer obtains the data to be sent through this function.
 *
 * @param[in] ctx User-defined context.
 * @param[out] buf Buffer to store read data.
 * @param len Requested length of data to read.
 * @param[out] read_len Actual length of data read.
 *
 * @return ESP_OK if the data was successfully read. Returning an error will abort the X/YMODEM transfer.
 *
 * @note This function attempts to read exactly len bytes, but may return fewer bytes if the end of the source is reached.
 *       A short read (read_len < len) is only allowed when no further data is available. read_len must not exceed len
 */
typedef esp_err_t (*xymodem_read_fn_t)(void *ctx, void *buf, size_t len, size_t *read_len);

/**
 * @brief Write data to a destination, such as a file or a simple array.
 *        The data received by the protocol layer is passed to the upper-layer application through this function.
 *
 * @param[in] ctx User-defined context.
 * @param[in] buf Buffer containing the data to write.
 * @param len Length of data to write.
 *
 * @return ESP_OK if all data was successfully written. Returning an error will abort the X/YMODEM transfer.
 *
 * @note This function can also process the data on the fly, without necessarily storing it anywhere.
 */
typedef esp_err_t (*xymodem_write_fn_t)(void *ctx, const void *buf, size_t len);

/**
 * @brief The abstraction representing the source of data to be sent via XMODEM.
 */
typedef struct {
    /**
     * @brief Read the next chunk of payload to transmit.
     */
    xymodem_read_fn_t read;
    /**
     * @brief User-defined context passed to `read()`.
     */
    void *ctx;
} xmodem_data_source_t;

/**
 * @brief The abstraction representing where data received by XMODEM is delivered.
 */
typedef struct {
    /**
     * @brief Deliver received payload bytes to the application.
     *
     * @note The XMODEM protocol cannot distinguish the payload from trailing padding.
     *       The padding is also delivered through this function.
     */
    xymodem_write_fn_t write;
    /**
     * @brief User-defined context passed to `write()`.
     */
    void *ctx;
} xmodem_data_sink_t;

/**
 * @brief Verification types as defined by the X/YMODEM protocol.
 */
typedef enum {
    XYMODEM_CHECK_AUTO = 0,  ///< Use CRC16 by default, and fall back to SUM if the peer does not support it.
    XYMODEM_CHECK_SUM8,      ///< 8-bit checksum.
    XYMODEM_CHECK_CRC16,     ///< CRC-16.
} xymodem_check_type_t;

/**
 * @brief Block size as defined by the X/YMODEM protocol.
 */
typedef enum {
    XYMODEM_BLOCK_SIZE_MIXED = 0,    ///< Use 1024-byte blocks by default, and switch to 128-byte blocks if the remaining data is no more than 128 bytes.
    XYMODEM_BLOCK_SIZE_128 = 128,    ///< Use 128-byte blocks only.
    XYMODEM_BLOCK_SIZE_1024 = 1024,  ///< Use 1024-byte blocks only.
} xymodem_block_size_t;

/**
 * @brief Sender configuration.
 */
typedef struct {
    xymodem_block_size_t block_size; ///< Packet payload size to use when sending.
    uint32_t response_timeout_ms;    ///< Wait for handshake or ACK/NAK. Must be > 0. Set several times longer than the receiver packet timeout.
    uint32_t max_retry_cnt;          ///< Maximum number of extra retries after the first attempt.
    uint8_t cancel_can_count;        ///< Number of CAN bytes to send when aborting. Must be >= 2.
    uint32_t can_interval_ms;        ///< Max interval between two CAN bytes when recognizing peer cancel. Must be > 0.
    uint32_t purge_idle_timeout_ms;  ///< RX idle time required to finish a purge. Set based on the physical link rate.
} xymodem_send_config_t;

/**
 * @brief Receiver configuration.
 */
typedef struct {
    xymodem_check_type_t check_type; ///< Checksum or CRC mode to request during handshake.
    uint32_t packet_timeout_ms;      ///< Wait for a packet before sending NAK. Must be > 0.
    uint32_t max_retry_cnt;          ///< Maximum number of extra retries after the first attempt.
    uint8_t cancel_can_count;        ///< Number of CAN bytes to send when aborting. Must be >= 2.
    uint32_t can_interval_ms;        ///< Max interval between two CAN bytes when recognizing peer cancel. Must be > 0.
    uint32_t purge_idle_timeout_ms;  ///< RX idle time required to finish a purge. Set based on the physical link rate.
} xymodem_recv_config_t;

/**
 * @brief Default XMODEM sender configuration.
 *
 * @note These defaults are intended for a good-quality link to a modern peer
 *       (CRC-16, 1K blocks). For noisy links, slower devices, or legacy
 *       implementations, adjust the fields as needed.
 */
#define XYMODEM_SEND_CONFIG_DEFAULT() { \
    .block_size = XYMODEM_BLOCK_SIZE_MIXED, \
    .response_timeout_ms = 30000, \
    .max_retry_cnt = 10, \
    .cancel_can_count = 2, \
    .can_interval_ms = 100, \
    .purge_idle_timeout_ms = 10, \
}

/**
 * @brief Default XMODEM receiver configuration.
 *
 * @note These defaults are intended for a good-quality link to a modern peer
 *       (CRC-16, 1K blocks). For noisy links, slower devices, or legacy
 *       implementations, adjust the fields as needed.
 */
#define XYMODEM_RECV_CONFIG_DEFAULT() { \
    .check_type = XYMODEM_CHECK_AUTO, \
    .packet_timeout_ms = 3000, \
    .max_retry_cnt = 10, \
    .cancel_can_count = 2, \
    .can_interval_ms = 100, \
    .purge_idle_timeout_ms = 10, \
}

/**************************************
 * Generic APIs
 **************************************/

/**
 * @brief Send data using the XMODEM protocol.
 *
 * This function performs a complete XMODEM transmission using the provided
 * transport and data source, and blocks until the transmission completes,
 * fails, or is cancelled.
 *
 * @param transport Transport interface used for sending and receiving protocol data.
 * @param config XMODEM sender configuration.
 * @param source Data source providing the payload to be transmitted.
 *
 * @return
 *   - ESP_OK: Transmission completed successfully.
 *   - ESP_ERR_INVALID_ARG: One or more arguments or configuration values are invalid.
 *   - ESP_ERR_NO_MEM: Not enough memory to complete the transmission.
 *   - ESP_ERR_XYMODEM_NO_RESPONSE: The peer did not respond within the configured timeout.
 *   - ESP_ERR_XYMODEM_CANCEL_BY_PEER: The transmission was cancelled by the peer.
 *   - ESP_ERR_XYMODEM_RETRY_EXCEEDED: The maximum number of retries was exceeded.
 *   - Other errors returned by the transport or data source.
 */
esp_err_t xmodem_send(const xymodem_transport_t *transport, const xymodem_send_config_t *config, const xmodem_data_source_t *source);

/**
 * @brief Receive data using the XMODEM protocol.
 *
 * This function performs a complete XMODEM reception using the provided
 * transport and data sink, and blocks until the transmission completes,
 * fails, or is cancelled.
 *
 * @param transport Transport interface used for sending and receiving protocol data.
 * @param config XMODEM receiver configuration.
 * @param sink Data sink that receives the transmitted payload.
 *
 * @return
 *   - ESP_OK: Reception completed successfully.
 *   - ESP_ERR_INVALID_ARG: One or more arguments or configuration values are invalid.
 *   - ESP_ERR_NO_MEM: Not enough memory to complete the reception.
 *   - ESP_ERR_XYMODEM_NO_RESPONSE: The peer did not respond within the configured timeout.
 *   - ESP_ERR_XYMODEM_CANCEL_BY_PEER: The transmission was cancelled by the peer.
 *   - ESP_ERR_XYMODEM_RETRY_EXCEEDED: The maximum number of retries was exceeded.
 *   - ESP_ERR_XYMODEM_OUT_OF_SYNC: The received packet sequence is out of sync.
 *   - Other errors returned by the transport or data sink.
 */
esp_err_t xmodem_recv(const xymodem_transport_t *transport, const xymodem_recv_config_t *config, const xmodem_data_sink_t *sink);

/**************************************
 * Convenience APIs
 * implemented on top of the generic APIs
 **************************************/

/**
 * @brief Directly send the contents of a buffer using XMODEM.
 *
 * @param[in] transport The transport layer to use.
 * @param[in] config The send configuration.
 * @param[in] buf The buffer to send.
 * @param len The length of the buffer.
 *
 * @return
 *   - ESP_OK if the data was successfully sent.
 *   - others if an error occurs (same as `xmodem_send()`).
 */
esp_err_t xmodem_send_buffer(const xymodem_transport_t *transport, const xymodem_send_config_t *config, const void *buf, size_t len);

/**
 * @brief Directly receive data into a buffer using XMODEM.
 *
 * @param[in] transport The transport layer to use.
 * @param[in] config The receive configuration.
 * @param[out] buf The buffer to receive the data.
 * @param len The length of the buffer.
 * @param[out] recv_len Number of bytes stored in `buf`. May be NULL.
 *
 * @note XMODEM cannot distinguish payload data from trailing padding, so the last packet may be larger
 *       than the remaining buffer space. Bytes from that packet that fit are stored; the rest of the
 *       packet is discarded. If another packet arrives after `buf` is already full, the transfer is aborted.
 *
 * @return
 *   - ESP_OK if the transfer completed. `recv_len` (when not NULL) is the number of bytes stored in `buf`, which is at most `len`.
 *   - ESP_ERR_NO_MEM: `buf` was filled and the peer sent another packet.
 *   - others if an error occurs (same as `xmodem_recv()`). `recv_len` still reports how many bytes were stored before the failure.
 */
esp_err_t xmodem_recv_buffer(const xymodem_transport_t *transport, const xymodem_recv_config_t *config, void *buf, size_t len, size_t *recv_len);

#ifdef __cplusplus
}
#endif
