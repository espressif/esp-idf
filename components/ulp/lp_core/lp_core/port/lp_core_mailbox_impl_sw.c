/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file Implementation of the mailbox port layer, using software
 */

#include <string.h>
#include "ulp_lp_core_critical_section_shared.h"
#include "ulp_lp_core_mailbox_impl_shared.h"
#include "ulp_lp_core_utils.h"
#include "ulp_lp_core_interrupts.h"
#include "ulp_lp_core_print.h"

typedef struct {
    volatile lp_message_t msg[LP_MAILBOX_MSG_COUNT];
    /* Message mask to the HP core */
    volatile uint32_t to_hp_msg_raw;
    /* Message mask to the LP core */
    volatile uint32_t to_lp_msg_raw;
    /* Lock for all the "raw" values, array is guaranteed to be split between LP/HP */
    ulp_lp_core_spinlock_t  lock;
} lp_core_mailbox_impl_sw_t;

lp_core_mailbox_impl_sw_t g_lp_core_mailbox_impl_sw_ctx;

/* Implementation agnostic interrupt handler */
static void (*s_intr_handler)(void);

void LP_CORE_ISR_ATTR ulp_lp_core_sw_intr_handler(void)
{
    if (s_intr_handler) {
        s_intr_handler();
    }
}

lp_core_mailbox_ctx_t lp_core_mailbox_impl_get_context(void)
{
    /* This structure must be initialized by the LP core */
    memset(&g_lp_core_mailbox_impl_sw_ctx, 0, sizeof(g_lp_core_mailbox_impl_sw_ctx));
    ulp_lp_core_spinlock_init(&g_lp_core_mailbox_impl_sw_ctx.lock);
    ulp_lp_core_sw_intr_from_hp_enable(false);
    return (lp_core_mailbox_ctx_t) &g_lp_core_mailbox_impl_sw_ctx;
}

uint32_t lp_core_mailbox_impl_intr_raw(lp_core_mailbox_ctx_t ctx)
{
    lp_core_mailbox_impl_sw_t* sw_ctx = (lp_core_mailbox_impl_sw_t*) ctx;
    return sw_ctx->to_lp_msg_raw;
}

uint32_t lp_core_mailbox_impl_intr_status(lp_core_mailbox_ctx_t ctx)
{
    lp_core_mailbox_impl_sw_t* sw_ctx = (lp_core_mailbox_impl_sw_t*) ctx;
    return sw_ctx->to_lp_msg_raw;
}

void lp_core_mailbox_impl_set_intr_handler(lp_core_mailbox_ctx_t ctx, void (*handler)(void))
{
    (void) ctx;
    s_intr_handler = handler;
}

void lp_core_mailbox_impl_intr_enable(lp_core_mailbox_ctx_t ctx, uint32_t msg_mask)
{
    /* This function is called when asynchronous received is used.
     * In the case of the software implementation, we don't really care about the mask, it should
     * be able to receive any RX message. */
    ulp_lp_core_sw_intr_from_hp_enable(true);
}

void lp_core_mailbox_impl_intr_disable(lp_core_mailbox_ctx_t ctx, uint32_t msg_mask)
{
    /* Similarly to the `enable` function, this is used for receiving messages.
     * Since we cannot only disable software interrupts, set the flags we defined above. */
    (void) ctx;
    (void) msg_mask;
    ulp_lp_core_sw_intr_from_hp_enable(false);
}

void lp_core_mailbox_impl_intr_clear(lp_core_mailbox_ctx_t ctx, uint32_t msg_mask)
{
    lp_core_mailbox_impl_sw_t* sw_ctx = (lp_core_mailbox_impl_sw_t*) ctx;
    /* Clear the flag */
    ulp_lp_core_enter_critical(&sw_ctx->lock);
    sw_ctx->to_lp_msg_raw &= ~msg_mask;
    if (sw_ctx->to_lp_msg_raw == 0) {
        /* All interrupts were handled */
        ulp_lp_core_sw_intr_from_hp_clear();
    }
    ulp_lp_core_exit_critical(&sw_ctx->lock);
}

void lp_core_mailbox_impl_set_message(lp_core_mailbox_ctx_t ctx, int index, lp_message_t value)
{
    lp_core_mailbox_impl_sw_t* sw_ctx = (lp_core_mailbox_impl_sw_t*) ctx;
    sw_ctx->msg[index] = value;
    ulp_lp_core_enter_critical(&sw_ctx->lock);
    sw_ctx->to_hp_msg_raw |= 1 << index;
    ulp_lp_core_exit_critical(&sw_ctx->lock);
    /* Trigger a software interrupt */
    ulp_lp_core_sw_intr_to_hp_trigger();
}

lp_message_t lp_core_mailbox_impl_get_message(lp_core_mailbox_ctx_t ctx, int index)
{
    return ((lp_core_mailbox_impl_sw_t*) ctx)->msg[index];
}
