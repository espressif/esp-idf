/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "hal/dma2d_hal.h"
#include "hal/dma2d_ll.h"

void dma2d_hal_init(dma2d_hal_context_t *hal, int group_id)
{
    hal->dev = DMA2D_LL_GET_HW(group_id);
}

void dma2d_hal_tx_reset_channel(dma2d_hal_context_t *hal, uint32_t channel)
{
    dma2d_ll_tx_disable_cmd(hal->dev, channel, true);
    while (!dma2d_ll_tx_is_reset_avail(hal->dev, channel));
    dma2d_ll_tx_reset_channel(hal->dev, channel);
    dma2d_ll_tx_disable_cmd(hal->dev, channel, false);
}

void dma2d_hal_rx_reset_channel(dma2d_hal_context_t *hal, uint32_t channel)
{
    dma2d_ll_rx_disable_cmd(hal->dev, channel, true);
    while (!dma2d_ll_rx_is_reset_avail(hal->dev, channel));
    dma2d_ll_rx_reset_channel(hal->dev, channel);
    dma2d_ll_rx_disable_cmd(hal->dev, channel, false);
}
