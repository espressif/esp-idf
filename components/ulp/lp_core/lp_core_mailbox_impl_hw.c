/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file Implementation of the mailbox port layer, using the hardware controller
 */

#include "ulp_lp_core_mailbox_impl_shared.h"
#include "hal/lp_mailbox_ll.h"

/* Mailbox hardware controller, used as a context */
lp_core_mailbox_ctx_t lp_core_mailbox_impl_get_context(void)
{
    return (lp_core_mailbox_ctx_t) &LP_MAILBOX;
}

uint32_t lp_core_mailbox_impl_intr_raw(lp_core_mailbox_ctx_t ctx)
{
    return lp_mailbox_ll_get_hp_intr_raw((lp_mb_dev_t*) ctx);
}

uint32_t lp_core_mailbox_impl_intr_status(lp_core_mailbox_ctx_t ctx)
{
    return lp_mailbox_ll_hp_intr_status((lp_mb_dev_t*) ctx);
}

void lp_core_mailbox_impl_set_intr_handler(lp_core_mailbox_ctx_t ctx, void (*handler)(void))
{
    /* Unused on the HP core */
    (void) ctx;
    (void) handler;
}

void lp_core_mailbox_impl_intr_enable(lp_core_mailbox_ctx_t ctx, uint32_t msg_mask)
{
    lp_mailbox_ll_hp_intr_enable_mask((lp_mb_dev_t*) ctx, msg_mask);
}

void lp_core_mailbox_impl_intr_disable(lp_core_mailbox_ctx_t ctx, uint32_t msg_mask)
{
    lp_mailbox_ll_hp_intr_disable_mask((lp_mb_dev_t*) ctx, msg_mask);
}

void lp_core_mailbox_impl_intr_clear(lp_core_mailbox_ctx_t ctx, uint32_t msg_mask)
{
    lp_mailbox_ll_hp_intr_clear((lp_mb_dev_t*) ctx, msg_mask);
}

void lp_core_mailbox_impl_set_message(lp_core_mailbox_ctx_t ctx, int index, lp_message_t value)
{
    lp_mailbox_ll_set_message((lp_mb_dev_t*) ctx, index, value);
}

lp_message_t lp_core_mailbox_impl_get_message(lp_core_mailbox_ctx_t ctx, int index)
{
    return lp_mailbox_ll_get_message((lp_mb_dev_t*) ctx, index);
}
