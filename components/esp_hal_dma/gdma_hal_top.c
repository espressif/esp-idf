/*
 * SPDX-FileCopyrightText: 2020-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdlib.h>
#include "hal/assert.h"
#include "hal/gdma_hal.h"

void gdma_hal_deinit(gdma_hal_context_t *hal)
{
    hal->generic_dev = NULL;
}

void gdma_hal_start_with_desc(gdma_hal_context_t *hal, int chan_id, gdma_channel_direction_t dir, intptr_t desc_base_addr)
{
    hal->start_with_desc(hal, chan_id, dir, desc_base_addr);
}

void gdma_hal_stop(gdma_hal_context_t *hal, int chan_id, gdma_channel_direction_t dir)
{
    hal->stop(hal, chan_id, dir);
}

void gdma_hal_append(gdma_hal_context_t *hal, int chan_id, gdma_channel_direction_t dir)
{
    hal->append(hal, chan_id, dir);
}

void gdma_hal_reset(gdma_hal_context_t *hal, int chan_id, gdma_channel_direction_t dir)
{
    hal->reset(hal, chan_id, dir);
}

void gdma_hal_set_priority(gdma_hal_context_t *hal, int chan_id, gdma_channel_direction_t dir, uint32_t priority)
{
    hal->set_priority(hal, chan_id, dir, priority);
}

void gdma_hal_connect_peri(gdma_hal_context_t *hal, int chan_id, gdma_channel_direction_t dir, int periph_id)
{
    hal->connect_peri(hal, chan_id, dir, periph_id);
}

void gdma_hal_connect_mem(gdma_hal_context_t *hal, int chan_id, gdma_channel_direction_t dir, int dummy_id)
{
    hal->connect_mem(hal, chan_id, dir, dummy_id);
}

void gdma_hal_disconnect_all(gdma_hal_context_t *hal, int chan_id, gdma_channel_direction_t dir)
{
    hal->disconnect_all(hal, chan_id, dir);
}

void gdma_hal_enable_burst(gdma_hal_context_t *hal, int chan_id, gdma_channel_direction_t dir, bool en_data_burst, bool en_desc_burst)
{
    hal->enable_burst(hal, chan_id, dir, en_data_burst, en_desc_burst);
}

void gdma_hal_set_burst_size(gdma_hal_context_t *hal, int chan_id, gdma_channel_direction_t dir, uint32_t burst_sz)
{
    if (hal->set_burst_size) {
        hal->set_burst_size(hal, chan_id, dir, burst_sz);
    }
}

void gdma_hal_set_strategy(gdma_hal_context_t *hal, int chan_id, gdma_channel_direction_t dir, bool en_owner_check, bool en_desc_write_back, bool eof_till_popped)
{
    hal->set_strategy(hal, chan_id, dir, en_owner_check, en_desc_write_back, eof_till_popped);
}

void gdma_hal_enable_intr(gdma_hal_context_t *hal, int chan_id, gdma_channel_direction_t dir, uint32_t intr_event_mask, bool en_or_dis)
{
    hal->enable_intr(hal, chan_id, dir, intr_event_mask, en_or_dis);
}

void gdma_hal_clear_intr(gdma_hal_context_t *hal, int chan_id, gdma_channel_direction_t dir, uint32_t intr_event_mask)
{
    hal->clear_intr(hal, chan_id, dir, intr_event_mask);
}

uint32_t gdma_hal_read_intr_status(gdma_hal_context_t *hal, int chan_id, gdma_channel_direction_t dir, bool raw)
{
    return hal->read_intr_status(hal, chan_id, dir, raw);
}

uint32_t gdma_hal_get_intr_status_reg(gdma_hal_context_t *hal, int chan_id, gdma_channel_direction_t dir)
{
    return hal->get_intr_status_reg(hal, chan_id, dir);
}

uint32_t gdma_hal_get_eof_desc_addr(gdma_hal_context_t *hal, int chan_id, gdma_channel_direction_t dir, bool is_success)
{
    return hal->get_eof_desc_addr(hal, chan_id, dir, is_success);
}

#if SOC_GDMA_SUPPORT_CRC
void gdma_hal_clear_crc(gdma_hal_context_t *hal, int chan_id, gdma_channel_direction_t dir)
{
    hal->clear_crc(hal, chan_id, dir);
}

void gdma_hal_set_crc_poly(gdma_hal_context_t *hal, int chan_id, gdma_channel_direction_t dir, const gdma_hal_crc_config_t *config)
{
    hal->set_crc_poly(hal, chan_id, dir, config);
}

uint32_t gdma_hal_get_crc_result(gdma_hal_context_t *hal, int chan_id, gdma_channel_direction_t dir)
{
    return hal->get_crc_result(hal, chan_id, dir);
}
#endif // SOC_GDMA_SUPPORT_CRC

#if SOC_GDMA_SUPPORT_ETM
void gdma_hal_enable_etm_task(gdma_hal_context_t *hal, int chan_id, gdma_channel_direction_t dir, bool en_or_dis)
{
    hal->enable_etm_task(hal, chan_id, dir, en_or_dis);
}
#endif // SOC_GDMA_SUPPORT_ETM

#if SOC_GDMA_SUPPORT_WEIGHTED_ARBITRATION
void gdma_hal_set_weight(gdma_hal_context_t *hal, int chan_id, gdma_channel_direction_t dir, uint32_t weight)
{
    hal->set_weight(hal, chan_id, dir, weight);
}
#endif // SOC_GDMA_SUPPORT_WEIGHTED_ARBITRATION
