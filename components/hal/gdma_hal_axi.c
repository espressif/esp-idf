/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "soc/soc_caps.h"
#include "hal/assert.h"
#include "hal/gdma_hal_axi.h"
#include "hal/axi_dma_ll.h"

static gdma_hal_priv_data_t gdma_axi_hal_priv_data = {
    .m2m_free_periph_mask = AXI_DMA_LL_M2M_FREE_PERIPH_ID_MASK,
};

void gdma_axi_hal_start_with_desc(gdma_hal_context_t *hal, int chan_id, gdma_channel_direction_t dir, intptr_t desc_base_addr)
{
    if (dir == GDMA_CHANNEL_DIRECTION_RX) {
        axi_dma_ll_rx_set_desc_addr(hal->axi_dma_dev, chan_id, desc_base_addr);
        axi_dma_ll_rx_start(hal->axi_dma_dev, chan_id);
    } else {
        axi_dma_ll_tx_set_desc_addr(hal->axi_dma_dev, chan_id, desc_base_addr);
        axi_dma_ll_tx_start(hal->axi_dma_dev, chan_id);
    }
}

void gdma_axi_hal_stop(gdma_hal_context_t *hal, int chan_id, gdma_channel_direction_t dir)
{
    if (dir == GDMA_CHANNEL_DIRECTION_RX) {
        axi_dma_ll_rx_stop(hal->axi_dma_dev, chan_id);
    } else {
        axi_dma_ll_tx_stop(hal->axi_dma_dev, chan_id);
    }
}

void gdma_axi_hal_append(gdma_hal_context_t *hal, int chan_id, gdma_channel_direction_t dir)
{
    if (dir == GDMA_CHANNEL_DIRECTION_RX) {
        axi_dma_ll_rx_restart(hal->axi_dma_dev, chan_id);
    } else {
        axi_dma_ll_tx_restart(hal->axi_dma_dev, chan_id);
    }
}

void gdma_axi_hal_reset(gdma_hal_context_t *hal, int chan_id, gdma_channel_direction_t dir)
{
    if (dir == GDMA_CHANNEL_DIRECTION_RX) {
        axi_dma_ll_rx_reset_channel(hal->axi_dma_dev, chan_id);
    } else {
        axi_dma_ll_tx_reset_channel(hal->axi_dma_dev, chan_id);
    }
}

void gdma_axi_hal_set_priority(gdma_hal_context_t *hal, int chan_id, gdma_channel_direction_t dir, uint32_t priority)
{
    if (dir == GDMA_CHANNEL_DIRECTION_RX) {
        axi_dma_ll_rx_set_priority(hal->axi_dma_dev, chan_id, priority);
    } else {
        axi_dma_ll_tx_set_priority(hal->axi_dma_dev, chan_id, priority);
    }
}

void gdma_axi_hal_connect_peri(gdma_hal_context_t *hal, int chan_id, gdma_channel_direction_t dir, gdma_trigger_peripheral_t periph, int periph_sub_id)
{
    if (dir == GDMA_CHANNEL_DIRECTION_RX) {
        axi_dma_ll_rx_reset_channel(hal->axi_dma_dev, chan_id); // reset channel
        axi_dma_ll_rx_connect_to_periph(hal->axi_dma_dev, chan_id, periph, periph_sub_id);
    } else {
        axi_dma_ll_tx_reset_channel(hal->axi_dma_dev, chan_id); // reset channel
        axi_dma_ll_tx_connect_to_periph(hal->axi_dma_dev, chan_id, periph, periph_sub_id);
    }
}

void gdma_axi_hal_disconnect_peri(gdma_hal_context_t *hal, int chan_id, gdma_channel_direction_t dir)
{
    if (dir == GDMA_CHANNEL_DIRECTION_RX) {
        axi_dma_ll_rx_disconnect_from_periph(hal->axi_dma_dev, chan_id);
    } else {
        axi_dma_ll_tx_disconnect_from_periph(hal->axi_dma_dev, chan_id);
    }
}

void gdma_axi_hal_enable_burst(gdma_hal_context_t *hal, int chan_id, gdma_channel_direction_t dir, bool en_data_burst, bool en_desc_burst)
{
    if (dir == GDMA_CHANNEL_DIRECTION_RX) {
        axi_dma_ll_rx_enable_data_burst(hal->axi_dma_dev, chan_id, en_data_burst);
        axi_dma_ll_rx_enable_descriptor_burst(hal->axi_dma_dev, chan_id, en_desc_burst);
    } else {
        axi_dma_ll_tx_enable_data_burst(hal->axi_dma_dev, chan_id, en_data_burst);
        axi_dma_ll_tx_enable_descriptor_burst(hal->axi_dma_dev, chan_id, en_desc_burst);
    }
}

void gdma_axi_hal_set_strategy(gdma_hal_context_t *hal, int chan_id, gdma_channel_direction_t dir, bool en_owner_check, bool en_desc_write_back)
{
    if (dir == GDMA_CHANNEL_DIRECTION_RX) {
        axi_dma_ll_rx_enable_owner_check(hal->axi_dma_dev, chan_id, en_owner_check);
    } else {
        axi_dma_ll_tx_enable_owner_check(hal->axi_dma_dev, chan_id, en_owner_check);
        axi_dma_ll_tx_enable_auto_write_back(hal->axi_dma_dev, chan_id, en_desc_write_back);
    }
}

void gdma_axi_hal_enable_intr(gdma_hal_context_t *hal, int chan_id, gdma_channel_direction_t dir, uint32_t intr_event_mask, bool en_or_dis)
{
    if (dir == GDMA_CHANNEL_DIRECTION_RX) {
        axi_dma_ll_rx_enable_interrupt(hal->axi_dma_dev, chan_id, intr_event_mask, en_or_dis);
    } else {
        axi_dma_ll_tx_enable_interrupt(hal->axi_dma_dev, chan_id, intr_event_mask, en_or_dis);
    }
}

void gdma_axi_hal_clear_intr(gdma_hal_context_t *hal, int chan_id, gdma_channel_direction_t dir, uint32_t intr_event_mask)
{
    if (dir == GDMA_CHANNEL_DIRECTION_RX) {
        axi_dma_ll_rx_clear_interrupt_status(hal->axi_dma_dev, chan_id, intr_event_mask);
    } else {
        axi_dma_ll_tx_clear_interrupt_status(hal->axi_dma_dev, chan_id, intr_event_mask);
    }
}

uint32_t gdma_axi_hal_read_intr_status(gdma_hal_context_t *hal, int chan_id, gdma_channel_direction_t dir)
{
    if (dir == GDMA_CHANNEL_DIRECTION_RX) {
        return axi_dma_ll_rx_get_interrupt_status(hal->axi_dma_dev, chan_id);
    } else {
        return axi_dma_ll_tx_get_interrupt_status(hal->axi_dma_dev, chan_id);
    }
}

uint32_t gdma_axi_hal_get_intr_status_reg(gdma_hal_context_t *hal, int chan_id, gdma_channel_direction_t dir)
{
    if (dir == GDMA_CHANNEL_DIRECTION_RX) {
        return (uint32_t)axi_dma_ll_rx_get_interrupt_status_reg(hal->axi_dma_dev, chan_id);
    } else {
        return (uint32_t)axi_dma_ll_tx_get_interrupt_status_reg(hal->axi_dma_dev, chan_id);
    }
}

uint32_t gdma_axi_hal_get_eof_desc_addr(gdma_hal_context_t *hal, int chan_id, gdma_channel_direction_t dir, bool is_success)
{
    if (dir == GDMA_CHANNEL_DIRECTION_RX) {
        if (is_success) {
            return axi_dma_ll_rx_get_success_eof_desc_addr(hal->axi_dma_dev, chan_id);
        }
        return axi_dma_ll_rx_get_error_eof_desc_addr(hal->axi_dma_dev, chan_id);
    } else {
        // The TX direction only has success EOF, parameter 'is_success' is ignored
        return axi_dma_ll_tx_get_eof_desc_addr(hal->axi_dma_dev, chan_id);
    }
}

void gdma_axi_hal_init(gdma_hal_context_t *hal, const gdma_hal_config_t *config)
{
    hal->axi_dma_dev = AXI_DMA_LL_GET_HW(config->group_id - GDMA_LL_AXI_GROUP_START_ID);
    hal->start_with_desc = gdma_axi_hal_start_with_desc;
    hal->stop = gdma_axi_hal_stop;
    hal->append = gdma_axi_hal_append;
    hal->reset = gdma_axi_hal_reset;
    hal->set_priority = gdma_axi_hal_set_priority;
    hal->connect_peri = gdma_axi_hal_connect_peri;
    hal->disconnect_peri = gdma_axi_hal_disconnect_peri;
    hal->enable_burst = gdma_axi_hal_enable_burst;
    hal->set_strategy = gdma_axi_hal_set_strategy;
    hal->enable_intr = gdma_axi_hal_enable_intr;
    hal->clear_intr = gdma_axi_hal_clear_intr;
    hal->read_intr_status = gdma_axi_hal_read_intr_status;
    hal->get_intr_status_reg = gdma_axi_hal_get_intr_status_reg;
    hal->get_eof_desc_addr = gdma_axi_hal_get_eof_desc_addr;
    hal->priv_data = &gdma_axi_hal_priv_data;
}
