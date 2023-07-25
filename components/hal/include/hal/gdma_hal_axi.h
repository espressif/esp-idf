/*
 * SPDX-FileCopyrightText: 2020-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "hal/gdma_hal.h"

#ifdef __cplusplus
extern "C" {
#endif

void gdma_axi_hal_start_with_desc(gdma_hal_context_t *hal, int chan_id, gdma_channel_direction_t dir, intptr_t desc_base_addr);

void gdma_axi_hal_stop(gdma_hal_context_t *hal, int chan_id, gdma_channel_direction_t dir);

void gdma_axi_hal_append(gdma_hal_context_t *hal, int chan_id, gdma_channel_direction_t dir);

void gdma_axi_hal_reset(gdma_hal_context_t *hal, int chan_id, gdma_channel_direction_t dir);

void gdma_axi_hal_set_priority(gdma_hal_context_t *hal, int chan_id, gdma_channel_direction_t dir, uint32_t priority);

void gdma_axi_hal_connect_peri(gdma_hal_context_t *hal, int chan_id, gdma_channel_direction_t dir, gdma_trigger_peripheral_t periph, int periph_sub_id);

void gdma_axi_hal_disconnect_peri(gdma_hal_context_t *hal, int chan_id, gdma_channel_direction_t dir);

void gdma_axi_hal_enable_burst(gdma_hal_context_t *hal, int chan_id, gdma_channel_direction_t dir, bool en_data_burst, bool en_desc_burst);

void gdma_axi_hal_set_ext_mem_align(gdma_hal_context_t *hal, int chan_id, gdma_channel_direction_t dir, uint8_t align);

void gdma_axi_hal_set_strategy(gdma_hal_context_t *hal, int chan_id, gdma_channel_direction_t dir, bool en_owner_check, bool en_desc_write_back);

void gdma_axi_hal_enable_intr(gdma_hal_context_t *hal, int chan_id, gdma_channel_direction_t dir, uint32_t intr_event_mask, bool en_or_dis);

void gdma_axi_hal_clear_intr(gdma_hal_context_t *hal, int chan_id, gdma_channel_direction_t dir, uint32_t intr_event_mask);

uint32_t gdma_axi_hal_read_intr_status(gdma_hal_context_t *hal, int chan_id, gdma_channel_direction_t dir);

uint32_t gdma_axi_hal_get_intr_status_reg(gdma_hal_context_t *hal, int chan_id, gdma_channel_direction_t dir);

uint32_t gdma_axi_hal_get_eof_desc_addr(gdma_hal_context_t *hal, int chan_id, gdma_channel_direction_t dir, bool is_success);

void gdma_axi_hal_init(gdma_hal_context_t *hal, const gdma_hal_config_t *config);

#ifdef __cplusplus
}
#endif
