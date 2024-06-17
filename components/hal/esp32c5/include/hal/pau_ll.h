/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

// The LL layer for ESP32-C5 PAU(Power Assist Unit) register operations

#pragma once

#include <stdlib.h>
#include <stdbool.h>
#include "soc/soc.h"
#include "soc/pau_reg.h"
#include "soc/pau_struct.h"
#include "soc/pcr_struct.h"
#include "hal/pau_types.h"
#include "hal/assert.h"

#ifdef __cplusplus
extern "C" {
#endif

static inline void pau_ll_enable_bus_clock(bool enable)
{
    if (enable) {
        PCR.regdma_conf.regdma_clk_en = 1;
        PCR.regdma_conf.regdma_rst_en = 0;
    } else {
        PCR.regdma_conf.regdma_clk_en = 0;
        PCR.regdma_conf.regdma_rst_en = 1;
    }
}

static inline uint32_t pau_ll_get_regdma_backup_flow_error(pau_dev_t *dev)
{
    HAL_ASSERT(false && "pau not supported yet");
    return 0;
}

static inline void pau_ll_select_regdma_entry_link(pau_dev_t *dev, int link)
{
    HAL_ASSERT(false && "pau not supported yet");
}

static inline void pau_ll_set_regdma_entry_link_backup_direction(pau_dev_t *dev, bool to_mem)
{
    HAL_ASSERT(false && "pau not supported yet");
}

static inline void pau_ll_set_regdma_entry_link_backup_start_enable(pau_dev_t *dev)
{
    HAL_ASSERT(false && "pau not supported yet");
}

static inline void pau_ll_set_regdma_entry_link_backup_start_disable(pau_dev_t *dev)
{
    HAL_ASSERT(false && "pau not supported yet");
}

static inline void pau_ll_set_regdma_select_wifimac_link(pau_dev_t *dev)
{
    HAL_ASSERT(false && "pau not supported yet");
}

static inline void pau_ll_set_regdma_deselect_wifimac_link(pau_dev_t *dev)
{
    HAL_ASSERT(false && "pau not supported yet");
}

static inline void pau_ll_set_regdma_wifimac_link_backup_direction(pau_dev_t *dev, bool to_mem)
{
    HAL_ASSERT(false && "pau not supported yet");
}

static inline void pau_ll_set_regdma_wifimac_link_backup_start_enable(pau_dev_t *dev)
{
    HAL_ASSERT(false && "pau not supported yet");
}

static inline void pau_ll_set_regdma_wifimac_link_backup_start_disable(pau_dev_t *dev)
{
    HAL_ASSERT(false && "pau not supported yet");
}

static inline void pau_ll_set_regdma_link0_addr(pau_dev_t *dev, void *link_addr)
{
    HAL_ASSERT(false && "pau not supported yet");
}

static inline void pau_ll_set_regdma_link1_addr(pau_dev_t *dev, void *link_addr)
{
    HAL_ASSERT(false && "pau not supported yet");
}

static inline void pau_ll_set_regdma_link2_addr(pau_dev_t *dev, void *link_addr)
{
    HAL_ASSERT(false && "pau not supported yet");
}

static inline void pau_ll_set_regdma_link3_addr(pau_dev_t *dev, void *link_addr)
{
    HAL_ASSERT(false && "pau not supported yet");
}

static inline void pau_ll_set_regdma_wifimac_link_addr(pau_dev_t *dev, void *link_addr)
{
    HAL_ASSERT(false && "pau not supported yet");
}

static inline uint32_t pau_ll_get_regdma_current_link_addr(pau_dev_t *dev)
{
    HAL_ASSERT(false && "pau not supported yet");
    return 0;
}

static inline uint32_t pau_ll_get_regdma_backup_addr(pau_dev_t *dev)
{
    HAL_ASSERT(false && "pau not supported yet");
    return 0;
}

static inline uint32_t pau_ll_get_regdma_memory_addr(pau_dev_t *dev)
{
    HAL_ASSERT(false && "pau not supported yet");
    return 0;
}

static inline uint32_t pau_ll_get_regdma_intr_raw_signal(pau_dev_t *dev)
{
    HAL_ASSERT(false && "pau not supported yet");
    return 0;
}

static inline uint32_t pau_ll_get_regdma_intr_status(pau_dev_t *dev)
{
    HAL_ASSERT(false && "pau not supported yet");
    return 0;
}

static inline void pau_ll_set_regdma_backup_done_intr_enable(pau_dev_t *dev)
{
    HAL_ASSERT(false && "pau not supported yet");
}

static inline void pau_ll_set_regdma_backup_done_intr_disable(pau_dev_t *dev)
{
    HAL_ASSERT(false && "pau not supported yet");
}

static inline void pau_ll_set_regdma_backup_error_intr_enable(pau_dev_t *dev)
{
    HAL_ASSERT(false && "pau not supported yet");
}

static inline void pau_ll_set_regdma_backup_error_intr_disable(pau_dev_t *dev)
{
    HAL_ASSERT(false && "pau not supported yet");
}

static inline void pau_ll_clear_regdma_backup_done_intr_state(pau_dev_t *dev)
{
    HAL_ASSERT(false && "pau not supported yet");
}

static inline void pau_ll_clear_regdma_backup_error_intr_state(pau_dev_t *dev)
{
    HAL_ASSERT(false && "pau not supported yet");
}

#ifdef __cplusplus
}
#endif
