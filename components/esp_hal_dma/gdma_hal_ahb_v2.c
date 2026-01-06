/*
 * SPDX-FileCopyrightText: 2022-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "soc/soc_caps.h"
#include "hal/assert.h"
#include "hal/gdma_hal_ahb.h"
#include "hal/ahb_dma_ll.h"
#include "hal/gdma_ll.h"

static gdma_hal_priv_data_t gdma_ahb_hal_priv_data = {
    .m2m_free_periph_mask = AHB_DMA_LL_M2M_FREE_PERIPH_ID_MASK,
};

void gdma_ahb_hal_start_with_desc(gdma_hal_context_t *hal, int chan_id, gdma_channel_direction_t dir, intptr_t desc_base_addr)
{
    if (dir == GDMA_CHANNEL_DIRECTION_RX) {
        ahb_dma_ll_rx_set_desc_addr(hal->ahb_dma_dev, chan_id, desc_base_addr);
        ahb_dma_ll_rx_start(hal->ahb_dma_dev, chan_id);
    } else {
        ahb_dma_ll_tx_set_desc_addr(hal->ahb_dma_dev, chan_id, desc_base_addr);
        ahb_dma_ll_tx_start(hal->ahb_dma_dev, chan_id);
    }
}

void gdma_ahb_hal_stop(gdma_hal_context_t *hal, int chan_id, gdma_channel_direction_t dir)
{
    if (dir == GDMA_CHANNEL_DIRECTION_RX) {
        ahb_dma_ll_rx_stop(hal->ahb_dma_dev, chan_id);
    } else {
        ahb_dma_ll_tx_stop(hal->ahb_dma_dev, chan_id);
    }
}

void gdma_ahb_hal_append(gdma_hal_context_t *hal, int chan_id, gdma_channel_direction_t dir)
{
    if (dir == GDMA_CHANNEL_DIRECTION_RX) {
        ahb_dma_ll_rx_restart(hal->ahb_dma_dev, chan_id);
    } else {
        ahb_dma_ll_tx_restart(hal->ahb_dma_dev, chan_id);
    }
}

void gdma_ahb_hal_reset(gdma_hal_context_t *hal, int chan_id, gdma_channel_direction_t dir)
{
    if (dir == GDMA_CHANNEL_DIRECTION_RX) {
        ahb_dma_ll_rx_reset_channel(hal->ahb_dma_dev, chan_id);
    } else {
        ahb_dma_ll_tx_reset_channel(hal->ahb_dma_dev, chan_id);
    }
}

void gdma_ahb_hal_set_priority(gdma_hal_context_t *hal, int chan_id, gdma_channel_direction_t dir, uint32_t priority)
{
    if (dir == GDMA_CHANNEL_DIRECTION_RX) {
        ahb_dma_ll_rx_set_priority(hal->ahb_dma_dev, chan_id, priority);
    } else {
        ahb_dma_ll_tx_set_priority(hal->ahb_dma_dev, chan_id, priority);
    }
}

void gdma_ahb_hal_connect_peri(gdma_hal_context_t *hal, int chan_id, gdma_channel_direction_t dir, int periph_id)
{
    if (dir == GDMA_CHANNEL_DIRECTION_RX) {
        ahb_dma_ll_rx_connect_to_periph(hal->ahb_dma_dev, chan_id, periph_id);
    } else {
        ahb_dma_ll_tx_connect_to_periph(hal->ahb_dma_dev, chan_id, periph_id);
    }
}

void gdma_ahb_hal_connect_mem(gdma_hal_context_t *hal, int chan_id, gdma_channel_direction_t dir, int dummy_id)
{
    if (dir == GDMA_CHANNEL_DIRECTION_RX) {
        ahb_dma_ll_rx_connect_to_mem(hal->ahb_dma_dev, chan_id, dummy_id);
    } else {
        ahb_dma_ll_tx_connect_to_mem(hal->ahb_dma_dev, chan_id, dummy_id);
    }
}

void gdma_ahb_hal_disconnect_all(gdma_hal_context_t *hal, int chan_id, gdma_channel_direction_t dir)
{
    if (dir == GDMA_CHANNEL_DIRECTION_RX) {
        ahb_dma_ll_rx_disconnect_all(hal->ahb_dma_dev, chan_id);
    } else {
        ahb_dma_ll_tx_disconnect_all(hal->ahb_dma_dev, chan_id);
    }
}

void gdma_ahb_hal_enable_burst(gdma_hal_context_t *hal, int chan_id, gdma_channel_direction_t dir, bool en_data_burst, bool en_desc_burst)
{
    if (dir == GDMA_CHANNEL_DIRECTION_RX) {
        ahb_dma_ll_rx_enable_data_burst(hal->ahb_dma_dev, chan_id, en_data_burst);
        ahb_dma_ll_rx_enable_descriptor_burst(hal->ahb_dma_dev, chan_id, en_desc_burst);
    } else {
        ahb_dma_ll_tx_enable_data_burst(hal->ahb_dma_dev, chan_id, en_data_burst);
        ahb_dma_ll_tx_enable_descriptor_burst(hal->ahb_dma_dev, chan_id, en_desc_burst);
    }
}

void gdma_ahb_hal_set_strategy(gdma_hal_context_t *hal, int chan_id, gdma_channel_direction_t dir, bool en_owner_check, bool en_desc_write_back, bool eof_till_popped)
{
    if (dir == GDMA_CHANNEL_DIRECTION_RX) {
        ahb_dma_ll_rx_enable_owner_check(hal->ahb_dma_dev, chan_id, en_owner_check);
        // RX direction always has the descriptor write-back feature enabled
        // RX direction don't need config eof_mode
    } else {
        ahb_dma_ll_tx_enable_owner_check(hal->ahb_dma_dev, chan_id, en_owner_check);
        ahb_dma_ll_tx_enable_auto_write_back(hal->ahb_dma_dev, chan_id, en_desc_write_back);
        ahb_dma_ll_tx_set_eof_mode(hal->ahb_dma_dev, chan_id, eof_till_popped);
    }
}

#if GDMA_LL_GET(AHB_BURST_SIZE_ADJUSTABLE)
void gdma_ahb_hal_set_burst_size(gdma_hal_context_t *hal, int chan_id, gdma_channel_direction_t dir, uint32_t burst_sz)
{
    if (dir == GDMA_CHANNEL_DIRECTION_RX) {
        ahb_dma_ll_rx_set_burst_size(hal->ahb_dma_dev, chan_id, burst_sz);
    } else {
        ahb_dma_ll_tx_set_burst_size(hal->ahb_dma_dev, chan_id, burst_sz);
    }
}
#endif // GDMA_LL_GET(AHB_BURST_SIZE_ADJUSTABLE)

void gdma_ahb_hal_enable_intr(gdma_hal_context_t *hal, int chan_id, gdma_channel_direction_t dir, uint32_t intr_event_mask, bool en_or_dis)
{
    if (dir == GDMA_CHANNEL_DIRECTION_RX) {
        ahb_dma_ll_rx_enable_interrupt(hal->ahb_dma_dev, chan_id, intr_event_mask, en_or_dis);
    } else {
        ahb_dma_ll_tx_enable_interrupt(hal->ahb_dma_dev, chan_id, intr_event_mask, en_or_dis);
    }
}

void gdma_ahb_hal_clear_intr(gdma_hal_context_t *hal, int chan_id, gdma_channel_direction_t dir, uint32_t intr_event_mask)
{
    if (dir == GDMA_CHANNEL_DIRECTION_RX) {
        ahb_dma_ll_rx_clear_interrupt_status(hal->ahb_dma_dev, chan_id, intr_event_mask);
    } else {
        ahb_dma_ll_tx_clear_interrupt_status(hal->ahb_dma_dev, chan_id, intr_event_mask);
    }
}

uint32_t gdma_ahb_hal_read_intr_status(gdma_hal_context_t *hal, int chan_id, gdma_channel_direction_t dir, bool raw)
{
    if (dir == GDMA_CHANNEL_DIRECTION_RX) {
        return ahb_dma_ll_rx_get_interrupt_status(hal->ahb_dma_dev, chan_id, raw);
    } else {
        return ahb_dma_ll_tx_get_interrupt_status(hal->ahb_dma_dev, chan_id, raw);
    }
}

uint32_t gdma_ahb_hal_get_intr_status_reg(gdma_hal_context_t *hal, int chan_id, gdma_channel_direction_t dir)
{
    if (dir == GDMA_CHANNEL_DIRECTION_RX) {
        return (uint32_t)ahb_dma_ll_rx_get_interrupt_status_reg(hal->ahb_dma_dev, chan_id);
    } else {
        return (uint32_t)ahb_dma_ll_tx_get_interrupt_status_reg(hal->ahb_dma_dev, chan_id);
    }
}

uint32_t gdma_ahb_hal_get_eof_desc_addr(gdma_hal_context_t *hal, int chan_id, gdma_channel_direction_t dir, bool is_success)
{
    if (dir == GDMA_CHANNEL_DIRECTION_RX) {
        if (is_success) {
            return ahb_dma_ll_rx_get_success_eof_desc_addr(hal->ahb_dma_dev, chan_id);
        }
        return ahb_dma_ll_rx_get_error_eof_desc_addr(hal->ahb_dma_dev, chan_id);
    } else {
        // The TX direction only has success EOF, parameter 'is_success' is ignored
        return ahb_dma_ll_tx_get_eof_desc_addr(hal->ahb_dma_dev, chan_id);
    }
}

#if SOC_GDMA_SUPPORT_CRC
void gdma_ahb_hal_clear_crc(gdma_hal_context_t *hal, int chan_id, gdma_channel_direction_t dir)
{
    if (dir == GDMA_CHANNEL_DIRECTION_RX) {
        ahb_dma_ll_rx_crc_clear(hal->ahb_dma_dev, chan_id);
    } else {
        ahb_dma_ll_tx_crc_clear(hal->ahb_dma_dev, chan_id);
    }
}

void gdma_ahb_hal_set_crc_poly(gdma_hal_context_t *hal, int chan_id, gdma_channel_direction_t dir, const gdma_hal_crc_config_t *config)
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
        ahb_dma_ll_rx_crc_set_init_value(hal->ahb_dma_dev, chan_id, init_value);
        ahb_dma_ll_rx_crc_set_width(hal->ahb_dma_dev, chan_id, crc_bit_width);
        for (uint32_t i = 0; i < crc_bit_width; i++) {
            // extract the control mask from the matrix, for each CRC bit
            data_mask = gdma_hal_get_data_mask_from_matrix(data_matrix, GDMA_LL_PARALLEL_CRC_DATA_WIDTH, i);
            lfsr_mask = gdma_hal_get_lfsr_mask_from_matrix(lfsr_matrix, crc_bit_width, i);
            ahb_dma_ll_rx_crc_set_lfsr_data_mask(hal->ahb_dma_dev, chan_id, i, lfsr_mask, data_mask, config->reverse_data_mask);
            ahb_dma_ll_rx_crc_latch_config(hal->ahb_dma_dev, chan_id);
        }
    } else {
        ahb_dma_ll_tx_crc_set_init_value(hal->ahb_dma_dev, chan_id, init_value);
        ahb_dma_ll_tx_crc_set_width(hal->ahb_dma_dev, chan_id, crc_bit_width);
        for (uint32_t i = 0; i < crc_bit_width; i++) {
            // extract the control mask from the matrix, for each CRC bit
            data_mask = gdma_hal_get_data_mask_from_matrix(data_matrix, GDMA_LL_PARALLEL_CRC_DATA_WIDTH, i);
            lfsr_mask = gdma_hal_get_lfsr_mask_from_matrix(lfsr_matrix, crc_bit_width, i);
            ahb_dma_ll_tx_crc_set_lfsr_data_mask(hal->ahb_dma_dev, chan_id, i, lfsr_mask, data_mask, config->reverse_data_mask);
            ahb_dma_ll_tx_crc_latch_config(hal->ahb_dma_dev, chan_id);
        }
    }
}

uint32_t gdma_ahb_hal_get_crc_result(gdma_hal_context_t *hal, int chan_id, gdma_channel_direction_t dir)
{
    if (dir == GDMA_CHANNEL_DIRECTION_RX) {
        return ahb_dma_ll_rx_crc_get_result(hal->ahb_dma_dev, chan_id);
    } else {
        return ahb_dma_ll_tx_crc_get_result(hal->ahb_dma_dev, chan_id);
    }
}
#endif // SOC_GDMA_SUPPORT_CRC

#if SOC_GDMA_SUPPORT_ETM
void gdma_ahb_hal_enable_etm_task(gdma_hal_context_t *hal, int chan_id, gdma_channel_direction_t dir, bool en_or_dis)
{
    if (dir == GDMA_CHANNEL_DIRECTION_RX) {
        ahb_dma_ll_rx_enable_etm_task(hal->ahb_dma_dev, chan_id, en_or_dis);
    } else {
        ahb_dma_ll_tx_enable_etm_task(hal->ahb_dma_dev, chan_id, en_or_dis);
    }
}
#endif // SOC_GDMA_SUPPORT_ETM

#if SOC_GDMA_SUPPORT_WEIGHTED_ARBITRATION
void gdma_ahb_hal_set_weight(gdma_hal_context_t *hal, int chan_id, gdma_channel_direction_t dir, uint32_t weight)
{
    if (dir == GDMA_CHANNEL_DIRECTION_RX) {
        ahb_dma_ll_rx_set_weight(hal->ahb_dma_dev, chan_id, weight);
    } else {
        ahb_dma_ll_tx_set_weight(hal->ahb_dma_dev, chan_id, weight);
    }
}
#endif // SOC_GDMA_SUPPORT_WEIGHTED_ARBITRATION

void gdma_ahb_hal_init(gdma_hal_context_t *hal, const gdma_hal_config_t *config)
{
    hal->ahb_dma_dev = AHB_DMA_LL_GET_HW(config->group_id - GDMA_LL_AHB_GROUP_START_ID);
    hal->priv_data = &gdma_ahb_hal_priv_data;
    hal->start_with_desc = gdma_ahb_hal_start_with_desc;
    hal->stop = gdma_ahb_hal_stop;
    hal->append = gdma_ahb_hal_append;
    hal->reset = gdma_ahb_hal_reset;
    hal->set_priority = gdma_ahb_hal_set_priority;
    hal->connect_peri = gdma_ahb_hal_connect_peri;
    hal->connect_mem = gdma_ahb_hal_connect_mem;
    hal->disconnect_all = gdma_ahb_hal_disconnect_all;
    hal->enable_burst = gdma_ahb_hal_enable_burst;
    hal->set_strategy = gdma_ahb_hal_set_strategy;
    hal->enable_intr = gdma_ahb_hal_enable_intr;
    hal->clear_intr = gdma_ahb_hal_clear_intr;
    hal->read_intr_status = gdma_ahb_hal_read_intr_status;
    hal->get_intr_status_reg = gdma_ahb_hal_get_intr_status_reg;
    hal->get_eof_desc_addr = gdma_ahb_hal_get_eof_desc_addr;
#if SOC_GDMA_SUPPORT_CRC
    hal->clear_crc = gdma_ahb_hal_clear_crc;
    hal->set_crc_poly = gdma_ahb_hal_set_crc_poly;
    hal->get_crc_result = gdma_ahb_hal_get_crc_result;
#endif // SOC_GDMA_SUPPORT_CRC
#if SOC_GDMA_SUPPORT_ETM
    hal->enable_etm_task = gdma_ahb_hal_enable_etm_task;
#endif // SOC_GDMA_SUPPORT_ETM
#if GDMA_LL_GET(AHB_BURST_SIZE_ADJUSTABLE)
    hal->set_burst_size = gdma_ahb_hal_set_burst_size;
#endif // GDMA_LL_GET(AHB_BURST_SIZE_ADJUSTABLE)
#if SOC_GDMA_SUPPORT_WEIGHTED_ARBITRATION
    hal->set_weight = gdma_ahb_hal_set_weight;
    if (config->flags.enable_weighted_arbitration) {
        ahb_dma_ll_enable_weighted_arb(hal->ahb_dma_dev, true);
        // always enable weighted arbitration optimize
        for (int i = 0; i < GDMA_LL_GET(AHB_PAIRS_PER_GROUP); i++) {
            ahb_dma_ll_tx_enable_weighted_arb_opt(hal->ahb_dma_dev, i, true);
            ahb_dma_ll_rx_enable_weighted_arb_opt(hal->ahb_dma_dev, i, true);
        }
        // set timeout to 2000 AHB bus cycles, to ensure that all channels within each time period can almost consume all the tokens
        ahb_dma_ll_set_weighted_arb_timeout(hal->ahb_dma_dev, 2000);
    } else {
        ahb_dma_ll_enable_weighted_arb(hal->ahb_dma_dev, false);
    }
#endif // SOC_GDMA_SUPPORT_WEIGHTED_ARBITRATION
    ahb_dma_ll_set_default_memory_range(hal->ahb_dma_dev);
}
