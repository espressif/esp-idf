/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include "freertos/FreeRTOS.h" // for TickType_t
#include "esp_err.h"

typedef intptr_t lp_message_t;

/**
 * @typedef lp_core_mailbox_callback_t
 * @brief Represents a callback function type triggered when a message is received in asynchronous mode.
 *
 * Note: There is no guarantee regarding the order in which messages are processed between synchronous
 * and asynchronous modes.
 *
 * @param msg The received message, represented as an `lp_message_t` object.
 */
typedef void (*lp_core_mailbox_callback_t)(lp_message_t msg);

/**
 * @brief Mailbox structure.
 */
typedef struct lp_mailbox_t* lp_mailbox_t;

/**
 * @brief Configuration structure for the mailbox
 */
typedef struct {
    uint32_t dummy;     /*!< Placeholder for future field, ignored for now */
} lp_mailbox_config_t;

/**
 * @brief Initialize a mailbox.
 *
 * @note Depending on the target, the implementation may use either the hardware mailbox controller
 * or a software implementation. In the latter case, the PMU software interrupt will be used for
 * sending and receiving notifications between the HP and LP cores.
 *
 * @note For the software implementation, this function must be called first by the LP core before
 * being called by the HP core.
 *
 * @param mailbox Pointer to the mailbox instance to initialize.
 * @param config  Configuration structure, can be NULL to use default values.
 *
 * @return ESP_OK on success,
 *         ESP_ERR_INVALID_STATE if `mailbox` parameter is NULL,
 *          if the mailbox has already been initialized on the HP core,
 *          or if the LP core has not yet initialized its mailbox (software implementation)
 *         ESP_ERR_NO_MEM if no more memory is available in the system.
 */
esp_err_t lp_core_mailbox_init(lp_mailbox_t *mailbox, lp_mailbox_config_t *config);

/**
 * @brief Deinitialize a mailbox.
 *
 * @param mailbox Pointer to the mailbox instance to deinitialize.
 *
 */
void lp_core_mailbox_deinit(lp_mailbox_t mailbox);

/**
 * @brief Send a message through the mailbox.
 *
 * @note This function will put a message in the mailbox and wait for a receiver to get it.
 *
 * @param mailbox   Pointer to the mailbox instance.
 * @param msg       Message to be sent.
 * @param ticks_to_wait Maximum ticks to wait before issuing a timeout.
 *
 * @return ESP_OK on success.
 *         ESP_INVALID_ARGUMENT if `mailbox` is NULL.
 *         ESP_TIMEOUT if timeout was reached and no receiver took the message.
 */
esp_err_t lp_core_mailbox_send(lp_mailbox_t mailbox, lp_message_t msg, TickType_t ticks_to_wait);

/**
 * @brief Send a message through the mailbox asynchronously.
 *
 * @note Unlike the synchronous send function, this function does not wait for a receiver to acknowledge
 *       the message.
 *
 * @param mailbox   Pointer to the mailbox instance.
 * @param msg       Message to be sent.
 *
 * @return ESP_OK on success.
 *         ESP_INVALID_ARGUMENT if `mailbox` is NULL.
 */
esp_err_t lp_core_mailbox_send_async(lp_mailbox_t mailbox, lp_message_t msg);

/**
 * @brief Receive a message from the mailbox.
 *
 * @note This function waits for a message to be sent by a sender.
 *
 * @param mailbox   Pointer to the mailbox instance.
 * @param msg       Pointer to store the received message.
 * @param ticks_to_wait Maximum ticks to wait before issuing a timeout.
 *
 * @return ESP_OK on success.
 *         ESP_INVALID_ARGUMENT if `mailbox` is NULL.
 *         ESP_TIMEOUT if the timeout expires without receiving a message.
 */
esp_err_t lp_core_mailbox_receive(lp_mailbox_t mailbox, lp_message_t* msg, TickType_t ticks_to_wait);

/**
 * @brief Receive a message from the mailbox.
 *
 * @param mailbox   Pointer to the mailbox instance.
 * @param count     Number of messages to receive asynchronously. To terminate it at anytime,
 *                  use lp_core_mailbox_receive_async_cancel() to terminate it.
 * @param cb        Callback invoked as soon as a message is received. Please note that this function
 *                  will be invoked from an ISR context.
 *
 * @return ESP_OK on success.
 *         ESP_INVALID_ARGUMENT if `mailbox` or `cb` is NULL.
 */
esp_err_t lp_core_mailbox_receive_async(lp_mailbox_t mailbox, uint32_t count, lp_core_mailbox_callback_t cb);

/**
 * @brief Cancel an ongoing asynchronous mailbox receive.
 *
 * This stops any pending lp_core_mailbox_receive_async().
 *
 * @param mailbox Pointer to the mailbox instance.
 * @param remaining Optional pointer to store the number of messages left to receive.
 *
 * @return ESP_OK on success, ESP_ERR_INVALID_STATE if no async receive was active.
 */
esp_err_t lp_core_mailbox_receive_async_cancel(lp_mailbox_t mailbox, uint32_t* remaining);

#ifdef __cplusplus
}
#endif
