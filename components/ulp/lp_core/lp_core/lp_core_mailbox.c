/*
 * SPDX-FileCopyrightText: 2025-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include "esp_err.h"
#include "hal/misc.h"
#include "ulp_lp_core_mailbox.h"
#include "ulp_lp_core_mailbox_impl_shared.h"
#include "ulp_lp_core_utils.h"
#include "ulp_lp_core_interrupts.h"
#include "ulp_lp_core_print.h"

#ifndef BIT
#define BIT(i)  (1U << (i))
#endif

/**
 * @brief Number of messages allocated for LP -> HP communication
 */
#define LP_MAILBOX_TX_MSG_COUNT  (LP_MAILBOX_MSG_COUNT / 2)

/**
 * @brief Starting index of the LP -> HP messages
 */
#define LP_MAILBOX_TX_MSG_IDX    0U

/**
 * @brief Starting index of the HP -> LP messages
 */
#define LP_MAILBOX_RX_MSG_IDX    (LP_MAILBOX_TX_MSG_COUNT)

/**
 * @brief Number of messages allocated for HP -> LP communication
 */
#define LP_MAILBOX_RX_MSG_COUNT  (LP_MAILBOX_MSG_COUNT - LP_MAILBOX_RX_MSG_IDX)

/**
 * @brief Mask for the HP -> LP messages
 */
#define LP_MAILBOX_RX_MSG_MASK   (((1U << LP_MAILBOX_RX_MSG_COUNT) - 1U) << LP_MAILBOX_RX_MSG_IDX)

#define LP_MAILBOX_ACK        ((lp_message_t) 0xe5U)

struct lp_mailbox_t {
    lp_core_mailbox_ctx_t      mb_ctx;
    lp_core_mailbox_callback_t rcv_callback;
    uint32_t                   rcv_remaining;
    int                        tx_idx;
};

static struct lp_mailbox_t s_mailbox;

/* In the current implementation, it should always be pointing to `s_mailbox`, but keep it for now in case
 * we need to have something more modular */
static lp_mailbox_t s_isr_arg;

/******************************************************************************
 *                              STATIC HELPERS                                *
 ******************************************************************************/

static inline int lp_core_next_msg_idx(int idx)
{
    return (idx + 1) % LP_MAILBOX_TX_MSG_COUNT;
}

static inline bool mb_msg_status(lp_mailbox_t mailbox, int msg_idx)
{
    return lp_core_mailbox_impl_intr_raw(mailbox->mb_ctx) & BIT(msg_idx);
}

static inline bool mb_msg_search_read(lp_mailbox_t mailbox, int* msg_idx, lp_message_t* msg)
{
    for (int i = LP_MAILBOX_RX_MSG_IDX; i < (LP_MAILBOX_RX_MSG_IDX + LP_MAILBOX_RX_MSG_COUNT); i += 2) {
        if (mb_msg_status(mailbox, i)) {
            *msg_idx = i;
            *msg = lp_core_mailbox_impl_get_message(mailbox->mb_ctx, i);
            return true;
        }
    }
    return false;
}

static inline bool mb_async_mode(lp_mailbox_t mailbox)
{
    return mailbox->rcv_callback != NULL;
}

static inline bool lp_core_mailbox_check_timeout(uint32_t start_cycle, int32_t timeout)
{
    return timeout != -1 && (ulp_lp_core_get_cpu_cycles() - start_cycle) >= (uint32_t) timeout;
}

static void ulp_lp_core_mailbox_intr_handler(void)
{
    lp_message_t received[LP_MAILBOX_RX_MSG_COUNT / 2];
    int received_count = 0;

    /* `s_isr_arg` cannot be NULL but let's be safe and test it */
    if (s_isr_arg == NULL || s_isr_arg->mb_ctx == NULL) {
        return;
    }

    /* Acknowledge all the received message as fast as possible */
    const uint32_t status = lp_core_mailbox_impl_intr_status(s_isr_arg->mb_ctx);
    /* Mask of all the processed messages */
    uint32_t clr_mask = 0;

    for (int i = 0; i < LP_MAILBOX_RX_MSG_COUNT; i += 2) {
        const uint32_t mask = BIT(LP_MAILBOX_RX_MSG_IDX + i);
        if (status & mask) {
            clr_mask |= mask;
            received[received_count] = lp_core_mailbox_impl_get_message(s_isr_arg->mb_ctx, LP_MAILBOX_RX_MSG_IDX + i);
            /* Acknowledge reception */
            const int ack_msg_idx = LP_MAILBOX_RX_MSG_IDX + lp_core_next_msg_idx(i);
            lp_core_mailbox_impl_set_message(s_isr_arg->mb_ctx, ack_msg_idx, LP_MAILBOX_ACK);
            /* Clear ACK self-interrupt (writing ACK sets LP's own interrupt bit) */
            lp_core_mailbox_impl_intr_clear(s_isr_arg->mb_ctx, BIT(ack_msg_idx));
            received_count++;
            /* We must not acknowledge more messages than what the caller needs */
            s_isr_arg->rcv_remaining--;
            if (s_isr_arg->rcv_remaining == 0) {
                break;
            }
        }
    }
    /* Clear the interrupt status */
    lp_core_mailbox_impl_intr_clear(s_isr_arg->mb_ctx, clr_mask);

    /* Call the callback for all the message received */
    for (int i = 0; i < received_count; i++) {
        s_isr_arg->rcv_callback(received[i]);
    }

    /* Disable interrupts if no more messages are expected */
    if (s_isr_arg->rcv_remaining == 0) {
        lp_core_mailbox_impl_intr_disable(s_isr_arg->mb_ctx, LP_MAILBOX_RX_MSG_MASK);
        s_isr_arg->rcv_callback = NULL;
    }
}

/******************************************************************************
 *                              PUBLIC API                                    *
 ******************************************************************************/

esp_err_t lp_core_mailbox_init(lp_mailbox_t *mailbox, lp_mailbox_config_t *config)
{
    /* Configuration is unused for now */
    (void) config;

    if (mailbox == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    if (s_mailbox.mb_ctx != NULL) {
        /* Mailbox has already been initialized! */
        return ESP_ERR_INVALID_STATE;
    }
    hal_memset(&s_mailbox, 0, sizeof(s_mailbox));
    s_mailbox.mb_ctx = lp_core_mailbox_impl_get_context();
    if (s_mailbox.mb_ctx == NULL) {
        return ESP_ERR_INVALID_STATE;
    }
    /* Clear the status and the interrupts for the messages */
    lp_core_mailbox_impl_intr_disable(s_mailbox.mb_ctx, ~0);
    lp_core_mailbox_impl_intr_clear(s_mailbox.mb_ctx, ~0);
    lp_core_mailbox_impl_set_intr_handler(s_mailbox.mb_ctx, ulp_lp_core_mailbox_intr_handler);
    s_mailbox.tx_idx = 0;
    *mailbox = &s_mailbox;
    return ESP_OK;
}

void lp_core_mailbox_deinit(lp_mailbox_t mailbox)
{
    if (mailbox != NULL) {
        lp_core_mailbox_impl_intr_disable(s_mailbox.mb_ctx, ~0);
        lp_core_mailbox_impl_intr_clear(s_mailbox.mb_ctx, ~0);
        s_mailbox.mb_ctx = NULL;
    }
}

esp_err_t lp_core_mailbox_send(lp_mailbox_t mailbox, lp_message_t msg, int32_t timeout)
{
    if (mailbox == NULL || timeout < -1) {
        return ESP_ERR_INVALID_ARG;
    }
    if (mb_async_mode(mailbox)) {
        return ESP_ERR_INVALID_STATE;
    }
    /* Get the address of the next free message */
    const int msg_idx = LP_MAILBOX_TX_MSG_IDX + mailbox->tx_idx;
    /* The message right after will be used for acknowledgement since writing to one message
     * sets the "ready" signal for both LP and HP status registers */
    const int next_idx = lp_core_next_msg_idx(mailbox->tx_idx);
    const int msg_ack_idx = LP_MAILBOX_TX_MSG_IDX + next_idx;
    /* The message after will be the next free message */
    mailbox->tx_idx = lp_core_next_msg_idx(next_idx);
    /* Make sure both messages have cleared status */
    lp_core_mailbox_impl_intr_clear(mailbox->mb_ctx, BIT(msg_idx) | BIT(msg_ack_idx));

    /* Send the message and wait for an acknowledgement */
    lp_core_mailbox_impl_set_message(mailbox->mb_ctx, msg_idx, msg);
    /* Wait for a response from the HP core, on the same message */
    const uint32_t start_cycle = ulp_lp_core_get_cpu_cycles();
    while (1) {
        if (mb_msg_status(mailbox, msg_ack_idx)) {
            /* Clear the status */
            lp_core_mailbox_impl_intr_clear(mailbox->mb_ctx, BIT(msg_idx) | BIT(msg_ack_idx));
            break;
        }
        if (lp_core_mailbox_check_timeout(start_cycle, timeout)) {
            return ESP_ERR_TIMEOUT;
        }
    }

    return ESP_OK;
}

esp_err_t lp_core_mailbox_send_async(lp_mailbox_t mailbox, lp_message_t msg)
{
    if (mailbox == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    if (mb_async_mode(mailbox)) {
        return ESP_ERR_INVALID_STATE;
    }
    /* Get the address of the next free message */
    const int msg_idx = LP_MAILBOX_TX_MSG_IDX + mailbox->tx_idx;
    mailbox->tx_idx = (mailbox->tx_idx + 1) % LP_MAILBOX_TX_MSG_COUNT;
    lp_core_mailbox_impl_set_message(mailbox->mb_ctx, msg_idx, msg);
    return ESP_OK;
}

esp_err_t lp_core_mailbox_receive(lp_mailbox_t mailbox, lp_message_t* msg, int32_t timeout)
{
    int msg_idx = 0;

    if (mailbox == NULL || timeout < -1) {
        return ESP_ERR_INVALID_ARG;
    }
    if (mb_async_mode(mailbox)) {
        return ESP_ERR_INVALID_STATE;
    }

    /* Wait for a message from the HP core */
    const uint32_t start_cycle = ulp_lp_core_get_cpu_cycles();
    while (1) {
        /* Get the address of the next free message */
        if (mb_msg_search_read(mailbox, &msg_idx, msg)) {
            lp_core_mailbox_impl_intr_clear(mailbox->mb_ctx, BIT(msg_idx));
            break;
        }
        if (lp_core_mailbox_check_timeout(start_cycle, timeout)) {
            return ESP_ERR_TIMEOUT;
        }
    }
    /* Acknowledge the message by writing any value to the next one (guaranteed no overflow) */
    const int ack_idx = msg_idx + 1;
    lp_core_mailbox_impl_set_message(mailbox->mb_ctx, ack_idx, LP_MAILBOX_ACK);
    /* Clear ACK messages status */
    lp_core_mailbox_impl_intr_clear(mailbox->mb_ctx, BIT(ack_idx));

    return ESP_OK;
}

esp_err_t lp_core_mailbox_receive_async(lp_mailbox_t mailbox, uint32_t count, lp_core_mailbox_callback_t cb)
{
    /* Make sure another asynchronous transaction is not on-going */
    if (mailbox == NULL || count == 0 || cb == NULL || mb_async_mode(mailbox)) {
        return ESP_ERR_INVALID_ARG;
    }
    mailbox->rcv_callback = cb;
    mailbox->rcv_remaining = count;
    /* Enable interrupts for receive */
    s_isr_arg = mailbox;
    lp_core_mailbox_impl_intr_enable(mailbox->mb_ctx, LP_MAILBOX_RX_MSG_MASK);
    return ESP_OK;
}

esp_err_t lp_core_mailbox_receive_async_cancel(lp_mailbox_t mailbox, uint32_t* remaining)
{
    if (mailbox == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    if (!mb_async_mode(mailbox)) {
        return ESP_ERR_INVALID_STATE;
    }
    lp_core_mailbox_impl_intr_disable(mailbox->mb_ctx, LP_MAILBOX_RX_MSG_MASK);
    mailbox->rcv_callback = NULL;
    if (remaining) {
        *remaining = mailbox->rcv_remaining;
    }
    s_isr_arg = NULL;
    ulp_lp_core_intr_enable();
    return ESP_OK;
}
