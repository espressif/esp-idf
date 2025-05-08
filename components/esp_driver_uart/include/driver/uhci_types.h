/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stddef.h>
#include "hal/uart_types.h"
#include "hal/uhci_types.h"
#include "soc/soc_caps.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief UHCI Controller Handle Type
 */
typedef struct uhci_controller_t *uhci_controller_handle_t;

/**
 * @brief UHCI TX Done Event Data
 */
typedef struct {
    uint8_t *buffer;            /**< Pointer to the which data buffer has been finished the transaction */
    size_t sent_size;           /**< Size has been sent out */
} uhci_tx_done_event_data_t;

/**
 * @brief UHCI TX Done Callback Function Type
 * @param uhci_ctrl Handle to the UHCI controller that initiated the transmission.
 * @param edata Pointer to a structure containing event data related to the completed transmission.
 *              This structure provides details such as the number of bytes transmitted and any
 *              status information relevant to the operation.
 * @param user_ctx User-defined context passed during the callback registration.
 *                 It can be used to maintain application-specific state or data.
 *
 * @return Whether a high priority task has been waken up by this callback function
 */
typedef bool (*uhci_tx_done_callback_t)(uhci_controller_handle_t uhci_ctrl, const uhci_tx_done_event_data_t *edata, void *user_ctx);

/**
 * @brief UHCI RX Done Event Data Structure
 */
typedef struct {
    uint8_t *data;                 /*!< Pointer to the received data buffer */
    size_t recv_size;              /*!< Number of bytes received */
    struct {
        uint32_t totally_received: 1;     /*!< When callback is invoked, while this bit is not set, means the current event gives partial of whole data, the transaction has not been finished. If set, means the current event gives whole data, the transaction finished. */
    } flags;                       /*!< I2C master config flags */
} uhci_rx_event_data_t;

/**
 * @brief UHCI RX Done Callback Function Type
 * @param uhci_ctrl Handle to the UHCI controller that initiated the transmission.
 * @param edata Pointer to a structure containing event data related to receive event.
 *              This structure provides details such as the number of bytes received and any
 *              status information relevant to the operation.
 * @param user_ctx User-defined context passed during the callback registration.
 *                 It can be used to maintain application-specific state or data.
 *
 * @return Whether a high priority task has been waken up by this callback function
 */
typedef bool (*uhci_rx_event_callback_t)(uhci_controller_handle_t uhci_ctrl, const uhci_rx_event_data_t *edata, void *user_ctx);

#ifdef __cplusplus
}
#endif
