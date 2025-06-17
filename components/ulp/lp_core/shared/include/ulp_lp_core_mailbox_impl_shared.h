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
#include "soc/soc_caps.h"

#if SOC_LP_MAILBOX_SUPPORTED
#include "hal/lp_mailbox_ll.h"
/**
 * @brief Total number of message we have in the hardware
 */
#define LP_MAILBOX_MSG_COUNT     (LP_MAILBOX_LL_MSG_COUNT)
#else // !SOC_LP_MAILBOX_SUPPORTED
#define LP_MAILBOX_MSG_COUNT     (16)
#endif // SOC_LP_MAILBOX_SUPPORTED

typedef intptr_t lp_message_t;

/**
 * @brief Abstract context for the mailbox implementation
 */
typedef void*   lp_core_mailbox_ctx_t;

/**
 * @brief Get the mailbox context for the current implementation
 */
lp_core_mailbox_ctx_t lp_core_mailbox_impl_get_context(void);

/**
 * @brief Set the interrupt handler to call when the a mailbox-related interrupt occurs.
 */
void lp_core_mailbox_impl_set_intr_handler(lp_core_mailbox_ctx_t ctx, void (*handler)(void));

/**
 * @brief Raw status of the messages
 */
uint32_t lp_core_mailbox_impl_intr_raw(lp_core_mailbox_ctx_t ctx);

/**
 * @brief Status of the messages, returns a bitmap where bit `i` is `1` if message `i` was just received.
 */
uint32_t lp_core_mailbox_impl_intr_status(lp_core_mailbox_ctx_t ctx);

/**
 * @brief Enable the interrupts for the messages pointed by the mask
 */
void lp_core_mailbox_impl_intr_enable(lp_core_mailbox_ctx_t ctx, uint32_t msg_mask);

/**
 * @brief Disable the interrupts for the messages pointed by the mask
 */
void lp_core_mailbox_impl_intr_disable(lp_core_mailbox_ctx_t ctx, uint32_t msg_mask);

/**
 * @brief Clear the interrupts for the messages pointed by the mask
 */
void lp_core_mailbox_impl_intr_clear(lp_core_mailbox_ctx_t ctx, uint32_t msg_mask);

/**
 * @brief Set the message and notify the receiver of this new entry
 */
void lp_core_mailbox_impl_set_message(lp_core_mailbox_ctx_t ctx, int index, lp_message_t value);

/**
 * @brief Get the message content, regardless of it status
 */
lp_message_t lp_core_mailbox_impl_get_message(lp_core_mailbox_ctx_t ctx, int index);

#ifdef __cplusplus
}
#endif
