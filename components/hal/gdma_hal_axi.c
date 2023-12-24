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
        // RX direction always has the descriptor write-back feature enabled
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

uint32_t gdma_axi_hal_read_intr_status(gdma_hal_context_t *hal, int chan_id, gdma_channel_direction_t dir, bool raw)
{
    if (dir == GDMA_CHANNEL_DIRECTION_RX) {
        return axi_dma_ll_rx_get_interrupt_status(hal->axi_dma_dev, chan_id, raw);
    } else {
        return axi_dma_ll_tx_get_interrupt_status(hal->axi_dma_dev, chan_id, raw);
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

#if SOC_GDMA_SUPPORT_CRC
void gdma_axi_hal_clear_crc(gdma_hal_context_t *hal, int chan_id, gdma_channel_direction_t dir)
{
    if (dir == GDMA_CHANNEL_DIRECTION_RX) {
        axi_dma_ll_rx_crc_clear(hal->axi_dma_dev, chan_id);
    } else {
        axi_dma_ll_tx_crc_clear(hal->axi_dma_dev, chan_id);
    }
}

void gdma_axi_hal_set_crc_poly(gdma_hal_context_t *hal, int chan_id, gdma_channel_direction_t dir, const gdma_hal_crc_config_t *config)
{
    uint32_t init_value = config->init_value;
    uint32_t crc_bit_width = config->crc_bit_width;
    uint32_t poly_hex = config->poly_hex;
    // bit matrix for parallel CRC
    uint32_t lfsr_matrix[crc_bit_width];
    uint32_t data_matrix[GDMA_LL_PARALLEL_CRC_DATA_WIDTH];
    uint32_t lfsr_mask = 0;
    uint32_t data_mask = 0;
    // build the parallel CRC matrix first, later we will extract the control mask from it
    gdma_hal_build_parallel_crc_matrix(crc_bit_width, poly_hex, GDMA_LL_PARALLEL_CRC_DATA_WIDTH,
                                       lfsr_matrix, data_matrix);
    if (dir == GDMA_CHANNEL_DIRECTION_RX) {
        axi_dma_ll_rx_crc_set_init_value(hal->axi_dma_dev, chan_id, init_value);
        axi_dma_ll_rx_crc_set_width(hal->axi_dma_dev, chan_id, crc_bit_width);
        for (uint32_t i = 0; i < crc_bit_width; i++) {
            // extract the control mask from the matrix, for each CRC bit
            data_mask = gdma_hal_get_data_mask_from_matrix(data_matrix, GDMA_LL_PARALLEL_CRC_DATA_WIDTH, i);
            lfsr_mask = gdma_hal_get_lfsr_mask_from_matrix(lfsr_matrix, crc_bit_width, i);
            axi_dma_ll_rx_crc_set_lfsr_data_mask(hal->axi_dma_dev, chan_id, i, lfsr_mask, data_mask, config->reverse_data_mask);
            axi_dma_ll_rx_crc_latch_config(hal->axi_dma_dev, chan_id);
        }
    } else {
        axi_dma_ll_tx_crc_set_init_value(hal->axi_dma_dev, chan_id, init_value);
        axi_dma_ll_tx_crc_set_width(hal->axi_dma_dev, chan_id, crc_bit_width);
        for (uint32_t i = 0; i < crc_bit_width; i++) {
            // extract the control mask from the matrix, for each CRC bit
            data_mask = gdma_hal_get_data_mask_from_matrix(data_matrix, GDMA_LL_PARALLEL_CRC_DATA_WIDTH, i);
            lfsr_mask = gdma_hal_get_lfsr_mask_from_matrix(lfsr_matrix, crc_bit_width, i);
            axi_dma_ll_tx_crc_set_lfsr_data_mask(hal->axi_dma_dev, chan_id, i, lfsr_mask, data_mask, config->reverse_data_mask);
            axi_dma_ll_tx_crc_latch_config(hal->axi_dma_dev, chan_id);
        }
    }
}

uint32_t gdma_axi_hal_get_crc_result(gdma_hal_context_t *hal, int chan_id, gdma_channel_direction_t dir)
{
    if (dir == GDMA_CHANNEL_DIRECTION_RX) {
        return axi_dma_ll_rx_crc_get_result(hal->axi_dma_dev, chan_id);
    } else {
        return axi_dma_ll_tx_crc_get_result(hal->axi_dma_dev, chan_id);
    }
}
#endif // SOC_GDMA_SUPPORT_CRC

#if SOC_GDMA_SUPPORT_ETM
void gdma_axi_hal_enable_etm_task(gdma_hal_context_t *hal, int chan_id, gdma_channel_direction_t dir, bool en_or_dis)
{
    if (dir == GDMA_CHANNEL_DIRECTION_RX) {
        axi_dma_ll_rx_enable_etm_task(hal->axi_dma_dev, chan_id, en_or_dis);
    } else {
        axi_dma_ll_tx_enable_etm_task(hal->axi_dma_dev, chan_id, en_or_dis);
    }
}
#endif // SOC_GDMA_SUPPORT_ETM

void gdma_axi_hal_init(gdma_hal_context_t *hal, const gdma_hal_config_t *config)
{
    hal->axi_dma_dev = AXI_DMA_LL_GET_HW(config->group_id - GDMA_LL_AXI_GROUP_START_ID);
    hal->priv_data = &gdma_axi_hal_priv_data;
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
#if SOC_GDMA_SUPPORT_CRC
    hal->clear_crc = gdma_axi_hal_clear_crc;
    hal->set_crc_poly = gdma_axi_hal_set_crc_poly;
    hal->get_crc_result = gdma_axi_hal_get_crc_result;
#endif // SOC_GDMA_SUPPORT_CRC
#if SOC_GDMA_SUPPORT_ETM
    hal->enable_etm_task = gdma_axi_hal_enable_etm_task;
#endif // SOC_GDMA_SUPPORT_ETM
}
