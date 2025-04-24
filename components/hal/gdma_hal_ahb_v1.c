/*
 * SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "soc/soc_caps.h"
#include "hal/assert.h"
#include "hal/gdma_hal_ahb.h"
#include "hal/gdma_ll.h"

static gdma_hal_priv_data_t gdma_ahb_hal_priv_data = {
    .m2m_free_periph_mask = GDMA_LL_M2M_FREE_PERIPH_ID_MASK,
};

void gdma_ahb_hal_start_with_desc(gdma_hal_context_t *hal, int chan_id, gdma_channel_direction_t dir, intptr_t desc_base_addr)
{
    if (dir == GDMA_CHANNEL_DIRECTION_RX) {
        gdma_ll_rx_set_desc_addr(hal->dev, chan_id, desc_base_addr);
        gdma_ll_rx_start(hal->dev, chan_id);
    } else {
        gdma_ll_tx_set_desc_addr(hal->dev, chan_id, desc_base_addr);
        gdma_ll_tx_start(hal->dev, chan_id);
    }
}

void gdma_ahb_hal_stop(gdma_hal_context_t *hal, int chan_id, gdma_channel_direction_t dir)
{
    if (dir == GDMA_CHANNEL_DIRECTION_RX) {
        gdma_ll_rx_stop(hal->dev, chan_id);
    } else {
        gdma_ll_tx_stop(hal->dev, chan_id);
    }
}

void gdma_ahb_hal_append(gdma_hal_context_t *hal, int chan_id, gdma_channel_direction_t dir)
{
    if (dir == GDMA_CHANNEL_DIRECTION_RX) {
        gdma_ll_rx_restart(hal->dev, chan_id);
    } else {
        gdma_ll_tx_restart(hal->dev, chan_id);
    }
}

void gdma_ahb_hal_reset(gdma_hal_context_t *hal, int chan_id, gdma_channel_direction_t dir)
{
    if (dir == GDMA_CHANNEL_DIRECTION_RX) {
        gdma_ll_rx_reset_channel(hal->dev, chan_id);
    } else {
        gdma_ll_tx_reset_channel(hal->dev, chan_id);
    }
}

void gdma_ahb_hal_set_priority(gdma_hal_context_t *hal, int chan_id, gdma_channel_direction_t dir, uint32_t priority)
{
    if (dir == GDMA_CHANNEL_DIRECTION_RX) {
        gdma_ll_rx_set_priority(hal->dev, chan_id, priority);
    } else {
        gdma_ll_tx_set_priority(hal->dev, chan_id, priority);
    }
}

void gdma_ahb_hal_connect_peri(gdma_hal_context_t *hal, int chan_id, gdma_channel_direction_t dir, gdma_trigger_peripheral_t periph, int periph_sub_id)
{
    if (dir == GDMA_CHANNEL_DIRECTION_RX) {
        gdma_ll_rx_reset_channel(hal->dev, chan_id); // reset channel
        gdma_ll_rx_connect_to_periph(hal->dev, chan_id, periph, periph_sub_id);
    } else {
        gdma_ll_tx_reset_channel(hal->dev, chan_id); // reset channel
        gdma_ll_tx_connect_to_periph(hal->dev, chan_id, periph, periph_sub_id);
    }
}

void gdma_ahb_hal_disconnect_peri(gdma_hal_context_t *hal, int chan_id, gdma_channel_direction_t dir)
{
    if (dir == GDMA_CHANNEL_DIRECTION_RX) {
        gdma_ll_rx_disconnect_from_periph(hal->dev, chan_id);
    } else {
        gdma_ll_tx_disconnect_from_periph(hal->dev, chan_id);
    }
}

void gdma_ahb_hal_enable_burst(gdma_hal_context_t *hal, int chan_id, gdma_channel_direction_t dir, bool en_data_burst, bool en_desc_burst)
{
    if (dir == GDMA_CHANNEL_DIRECTION_RX) {
        gdma_ll_rx_enable_data_burst(hal->dev, chan_id, en_data_burst);
        gdma_ll_rx_enable_descriptor_burst(hal->dev, chan_id, en_desc_burst);
    } else {
        gdma_ll_tx_enable_data_burst(hal->dev, chan_id, en_data_burst);
        gdma_ll_tx_enable_descriptor_burst(hal->dev, chan_id, en_desc_burst);
    }
}

#if GDMA_LL_AHB_BURST_SIZE_ADJUSTABLE
void gdma_ahb_hal_set_burst_size(gdma_hal_context_t *hal, int chan_id, gdma_channel_direction_t dir, uint32_t burst_sz)
{
    if (dir == GDMA_CHANNEL_DIRECTION_RX) {
        gdma_ll_rx_set_burst_size(hal->dev, chan_id, burst_sz);
    } else {
        gdma_ll_tx_set_burst_size(hal->dev, chan_id, burst_sz);
    }
}
#endif // GDMA_LL_AHB_BURST_SIZE_ADJUSTABLE

void gdma_ahb_hal_set_strategy(gdma_hal_context_t *hal, int chan_id, gdma_channel_direction_t dir, bool en_owner_check, bool en_desc_write_back, bool eof_till_popped)
{
    if (dir == GDMA_CHANNEL_DIRECTION_RX) {
        gdma_ll_rx_enable_owner_check(hal->dev, chan_id, en_owner_check);
        // RX direction always has the descriptor write-back feature enabled
        // RX direction don't need config eof_mode
    } else {
        gdma_ll_tx_enable_owner_check(hal->dev, chan_id, en_owner_check);
        gdma_ll_tx_enable_auto_write_back(hal->dev, chan_id, en_desc_write_back);
        gdma_ll_tx_set_eof_mode(hal->dev, chan_id, eof_till_popped);
    }
}

void gdma_ahb_hal_enable_intr(gdma_hal_context_t *hal, int chan_id, gdma_channel_direction_t dir, uint32_t intr_event_mask, bool en_or_dis)
{
    if (dir == GDMA_CHANNEL_DIRECTION_RX) {
        gdma_ll_rx_enable_interrupt(hal->dev, chan_id, intr_event_mask, en_or_dis);
    } else {
        gdma_ll_tx_enable_interrupt(hal->dev, chan_id, intr_event_mask, en_or_dis);
    }
}

void gdma_ahb_hal_clear_intr(gdma_hal_context_t *hal, int chan_id, gdma_channel_direction_t dir, uint32_t intr_event_mask)
{
    if (dir == GDMA_CHANNEL_DIRECTION_RX) {
        gdma_ll_rx_clear_interrupt_status(hal->dev, chan_id, intr_event_mask);
    } else {
        gdma_ll_tx_clear_interrupt_status(hal->dev, chan_id, intr_event_mask);
    }
}

uint32_t gdma_ahb_hal_read_intr_status(gdma_hal_context_t *hal, int chan_id, gdma_channel_direction_t dir, bool raw)
{
    if (dir == GDMA_CHANNEL_DIRECTION_RX) {
        return gdma_ll_rx_get_interrupt_status(hal->dev, chan_id, raw);
    } else {
        return gdma_ll_tx_get_interrupt_status(hal->dev, chan_id, raw);
    }
}

uint32_t gdma_ahb_hal_get_intr_status_reg(gdma_hal_context_t *hal, int chan_id, gdma_channel_direction_t dir)
{
    if (dir == GDMA_CHANNEL_DIRECTION_RX) {
        return (uint32_t)gdma_ll_rx_get_interrupt_status_reg(hal->dev, chan_id);
    } else {
        return (uint32_t)gdma_ll_tx_get_interrupt_status_reg(hal->dev, chan_id);
    }
}

uint32_t gdma_ahb_hal_get_eof_desc_addr(gdma_hal_context_t *hal, int chan_id, gdma_channel_direction_t dir, bool is_success)
{
    if (dir == GDMA_CHANNEL_DIRECTION_RX) {
        if (is_success) {
            return gdma_ll_rx_get_success_eof_desc_addr(hal->dev, chan_id);
        }
        return gdma_ll_rx_get_error_eof_desc_addr(hal->dev, chan_id);
    } else {
        // The TX direction only has success EOF, parameter 'is_success' is ignored
        return gdma_ll_tx_get_eof_desc_addr(hal->dev, chan_id);
    }
}

#if SOC_GDMA_SUPPORT_ETM
void gdma_ahb_hal_enable_etm_task(gdma_hal_context_t *hal, int chan_id, gdma_channel_direction_t dir, bool en_or_dis)
{
    if (dir == GDMA_CHANNEL_DIRECTION_RX) {
        gdma_ll_rx_enable_etm_task(hal->dev, chan_id, en_or_dis);
    } else {
        gdma_ll_tx_enable_etm_task(hal->dev, chan_id, en_or_dis);
    }
}
#endif // SOC_GDMA_SUPPORT_ETM

void gdma_ahb_hal_init(gdma_hal_context_t *hal, const gdma_hal_config_t *config)
{
    hal->dev = GDMA_LL_GET_HW(config->group_id - GDMA_LL_AHB_GROUP_START_ID);
    hal->start_with_desc = gdma_ahb_hal_start_with_desc;
    hal->stop = gdma_ahb_hal_stop;
    hal->append = gdma_ahb_hal_append;
    hal->reset = gdma_ahb_hal_reset;
    hal->set_priority = gdma_ahb_hal_set_priority;
    hal->connect_peri = gdma_ahb_hal_connect_peri;
    hal->disconnect_peri = gdma_ahb_hal_disconnect_peri;
    hal->enable_burst = gdma_ahb_hal_enable_burst;
    hal->set_strategy = gdma_ahb_hal_set_strategy;
    hal->enable_intr = gdma_ahb_hal_enable_intr;
    hal->clear_intr = gdma_ahb_hal_clear_intr;
    hal->read_intr_status = gdma_ahb_hal_read_intr_status;
    hal->get_intr_status_reg = gdma_ahb_hal_get_intr_status_reg;
    hal->get_eof_desc_addr = gdma_ahb_hal_get_eof_desc_addr;
#if SOC_GDMA_SUPPORT_ETM
    hal->enable_etm_task = gdma_ahb_hal_enable_etm_task;
#endif
#if GDMA_LL_AHB_BURST_SIZE_ADJUSTABLE
    hal->set_burst_size = gdma_ahb_hal_set_burst_size;
#endif // GDMA_LL_AHB_BURST_SIZE_ADJUSTABLE
    hal->priv_data = &gdma_ahb_hal_priv_data;
}
