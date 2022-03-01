/*
 * SPDX-FileCopyrightText: 2019-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "hal/rmt_hal.h"
#include "hal/rmt_ll.h"
#include "soc/soc_caps.h"

void rmt_hal_init(rmt_hal_context_t *hal)
{
    hal->regs = &RMT;
}

void rmt_hal_tx_channel_reset(rmt_hal_context_t *hal, uint32_t channel)
{
    rmt_ll_tx_reset_channels_clock_div(hal->regs, 1 << channel);
    rmt_ll_tx_reset_pointer(hal->regs, channel);
#if SOC_RMT_SUPPORT_TX_LOOP_COUNT
    rmt_ll_tx_reset_loop_count(hal->regs, channel);
#endif // SOC_RMT_SUPPORT_TX_LOOP_COUNT
    rmt_ll_enable_interrupt(hal->regs, RMT_LL_EVENT_TX_MASK(channel), false);
    rmt_ll_clear_interrupt_status(hal->regs, RMT_LL_EVENT_TX_MASK(channel));
}

void rmt_hal_rx_channel_reset(rmt_hal_context_t *hal, uint32_t channel)
{
    rmt_ll_rx_reset_channels_clock_div(hal->regs, 1 << channel);
    rmt_ll_rx_reset_pointer(hal->regs, channel);
    rmt_ll_enable_interrupt(hal->regs, RMT_LL_EVENT_RX_MASK(channel), false);
    rmt_ll_clear_interrupt_status(hal->regs, RMT_LL_EVENT_RX_MASK(channel));
}
