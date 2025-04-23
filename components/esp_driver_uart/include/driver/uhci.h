/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "driver/uhci_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief UHCI controller specific configurations
 */
typedef struct {
    uart_port_t uart_port;                                /*!< UART port that connect to UHCI controller */
    size_t tx_trans_queue_depth;                          /*!< Depth of internal transfer queue, increase this value can support more transfers pending in the background */
    size_t max_transmit_size;                             /*!< Maximum transfer size in one transaction, in bytes. This decides the number of DMA nodes will be used for each transaction */
    size_t max_receive_internal_mem;                      /*!< Internal DMA usage memory. Each DMA node can point to a maximum of x bytes (depends on chip). This value determines the number of DMA nodes used for each transaction. When your transfer size is large enough, it is recommended to set this value greater than x to facilitate efficient ping-pong operations, such as 2 * x. */
    size_t dma_burst_size;                                /*!< DMA burst size, in bytes. Set to 0 to disable data burst. Otherwise, use a power of 2. */
    size_t max_packet_receive;                            /*!< Max receive size, auto stop receiving after reach this value, only valid when `length_eof` set true */

    struct {
        uint16_t rx_brk_eof: 1;                           /*!< UHCI will end payload receive process when NULL frame is received by UART. */
        uint16_t idle_eof: 1;                             /*!< UHCI will end payload receive process when UART has been in idle state. */
        uint16_t length_eof: 1;                           /*!< UHCI will end payload receive process when the receiving byte count has reached the specific value. */
    } rx_eof_flags;                                       /*!< UHCI eof flags */
} uhci_controller_config_t;

/**
 * @brief Structure for defining callback functions for UHCI events.
 */
typedef struct {
    uhci_rx_event_callback_t on_rx_trans_event;           /*!< Callback function for handling the completion of a reception. */
    uhci_tx_done_callback_t on_tx_trans_done;             /*!< Callback function for handling the completion of a transmission. */
} uhci_event_callbacks_t;

/**
 * @brief Create and initialize a new UHCI controller.
 *
 * This function initializes a new UHCI controller instance based on the provided configuration.
 * It allocates and configures resources required for the UHCI controller, such as DMA and
 * communication settings. The created controller handle is returned through the output parameter.
 *
 * @param[in] config       Pointer to a `uhci_controller_config_t` structure containing the
 *                         configuration parameters for the UHCI controller.
 * @param[out] ret_uhci_ctrl   Pointer to a variable where the handle to the newly created UHCI controller
 *                         will be stored. This handle is used in subsequent operations involving the
 *                         controller.
 *
 * @return
 * - `ESP_OK`: Controller successfully created and initialized.
 * - `ESP_ERR_INVALID_ARG`: One or more arguments are invalid (e.g., null pointers or invalid config).
 * - `ESP_ERR_NO_MEM`: Memory allocation for the controller failed.
 * - Other error codes: Indicate failure in the underlying hardware or driver initialization.
 */
esp_err_t uhci_new_controller(const uhci_controller_config_t *config, uhci_controller_handle_t *ret_uhci_ctrl);

/**
 * @brief Receive data from the UHCI controller.
 *
 * This function retrieves data from the UHCI controller into the provided buffer. It is typically
 * used for receiving data that was transmitted via UART and processed by the UHCI DMA controller.
 *
 * @param[in] uhci_ctrl       Handle to the UHCI controller, which was previously created using
 *                            `uhci_new_controller()`.
 * @param[out] read_buffer    Pointer to the buffer where the received data will be stored.
 *                            The buffer must be pre-allocated by the caller.
 * @param[in] buffer_size     The size of read buffer.
 *
 * @note @note The function is non-blocking, it just mounts the user buffer to the DMA.
 * The return from the function doesn't mean a finished receive. You need to register corresponding
 * callback function to get notification.
 *
 * @return
 * - `ESP_OK`: Data successfully received and written to the buffer.
 * - `ESP_ERR_INVALID_ARG`: Invalid arguments (e.g., null buffer or invalid controller handle).
 */
esp_err_t uhci_receive(uhci_controller_handle_t uhci_ctrl, uint8_t *read_buffer, size_t buffer_size);

/**
 * @brief Transmit data using the UHCI controller.
 *
 * This function sends data from the provided buffer through the UHCI controller. It uses the DMA
 * capabilities of UHCI to efficiently handle data transmission via UART.
 *
 * @param[in] uhci_ctrl     Handle to the UHCI controller, which was previously created using
 *                          `uhci_new_controller()`.
 * @param[in] write_buffer  Pointer to the buffer containing the data to be transmitted.
 *                          The buffer must remain valid until the transmission is complete.
 * @param[in] write_size    The number of bytes to transmit from the buffer.
 *
 * @note The function is an non-blocking api, which means this function will return immediately. You can
 * get corresponding event from callbacks.
 *
 * @return
 * - `ESP_OK`: Data successfully queued for transmission.
 * - `ESP_ERR_INVALID_ARG`: Invalid arguments (e.g., null buffer, invalid handle, or zero `write_size`).
 */
esp_err_t uhci_transmit(uhci_controller_handle_t uhci_ctrl, uint8_t *write_buffer, size_t write_size);

/**
 * @brief Uninstall the UHCI (UART Host Controller Interface) driver and release resources.
 *
 * This function deinitializes the UHCI controller and frees any resources allocated during its
 * initialization. It ensures proper cleanup and prevents resource leaks when the UHCI controller
 * is no longer needed.
 *
 * @param[in] uhci_ctrl   Handle to the UHCI controller, which was previously created using
 *                        `uhci_new_controller()`. Passing an invalid or uninitialized handle
 *                        may result in undefined behavior.
 *
 * @return
 * - `ESP_OK`: The UHCI driver was successfully uninstalled, and resources were released.
 * - `ESP_ERR_INVALID_ARG`: The provided `uhci_ctrl` handle is invalid or null.
 */
esp_err_t uhci_del_controller(uhci_controller_handle_t uhci_ctrl);

/**
 * @brief Register event callback functions for a UHCI controller.
 *
 * This function allows the user to register callback functions to handle specific UHCI events, such as
 * transmission or reception completion. The callbacks provide a mechanism to handle asynchronous events
 * generated by the UHCI controller.
 *
 * @param[in] uhci_ctrl   Handle to the UHCI controller, which was previously created using
 *                        `uhci_new_controller()`.
 * @param[in] cbs         Pointer to a `uhci_event_callbacks_t` structure that defines the callback
 *                        functions to be registered. This structure includes pointers to the callback
 *                        functions for handling UHCI events.
 * @param[in] user_data   Pointer to user-defined data that will be passed to the callback functions
 *                        when they are invoked. This can be used to provide context or state information
 *                        specific to the application.
 *
 * @return
 * - `ESP_OK`: Event callbacks were successfully registered.
 * - `ESP_ERR_INVALID_ARG`: Invalid arguments (e.g., null `uhci_ctrl` handle or `cbs` pointer).
 */
esp_err_t uhci_register_event_callbacks(uhci_controller_handle_t uhci_ctrl, const uhci_event_callbacks_t *cbs, void *user_data);

/**
 * @brief Wait for all pending TX transactions done
 *
 * @param[in] uhci_ctrl UHCI controller that created by `uhci_new_controller`
 * @param[in] timeout_ms Timeout in milliseconds, `-1` means to wait forever
 * @return
 *      - ESP_OK: All pending TX transactions is finished and recycled
 *      - ESP_ERR_INVALID_ARG: Wait for all pending TX transactions done failed because of invalid argument
 *      - ESP_ERR_TIMEOUT: Wait for all pending TX transactions done timeout
 *      - ESP_FAIL: Wait for all pending TX transactions done failed because of other error
 */
esp_err_t uhci_wait_all_tx_transaction_done(uhci_controller_handle_t uhci_ctrl, int timeout_ms);

#ifdef __cplusplus
}
#endif
