/*
 * SPDX-FileCopyrightText: 2023-2025 Espressif Systems (Shanghai) CO LTD
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
#include "soc/lp_aon_struct.h"
#include "hal/pau_types.h"
#include "hal/assert.h"
#include "hal/misc.h"

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
    return dev->regdma_conf.flow_err;
}

static inline void pau_ll_select_regdma_entry_link(pau_dev_t *dev, int link)
{
    dev->regdma_conf.link_sel = link;
}

static inline void pau_ll_set_regdma_entry_link_backup_direction(pau_dev_t *dev, bool to_mem)
{
    dev->regdma_conf.to_mem = to_mem ? 1 : 0;
}

static inline void pau_ll_set_regdma_entry_link_backup_start_enable(pau_dev_t *dev, bool enable)
{
    dev->regdma_conf.start = enable;
}

static inline void pau_ll_set_regdma_select_wifimac_link(pau_dev_t *dev)
{
    dev->regdma_conf.sel_mac = 1;
}

static inline void pau_ll_set_regdma_deselect_wifimac_link(pau_dev_t *dev)
{
    dev->regdma_conf.sel_mac = 0;
}

static inline void pau_ll_set_regdma_wifimac_link_backup_direction(pau_dev_t *dev, bool to_mem)
{
    dev->regdma_conf.to_mem_mac = to_mem ? 1 : 0;
}

static inline void pau_ll_set_regdma_wifimac_link_backup_start_enable(pau_dev_t *dev)
{
    dev->regdma_conf.start_mac = 1;
}

static inline void pau_ll_set_regdma_wifimac_link_backup_start_disable(pau_dev_t *dev)
{
    dev->regdma_conf.start_mac = 0;
}

static inline uint32_t pau_ll_get_regdma_current_link_addr(pau_dev_t *dev)
{
    return dev->regdma_current_link_addr.val;
}

static inline uint32_t pau_ll_get_regdma_backup_addr(pau_dev_t *dev)
{
    return dev->regdma_peri_addr.val;
}

static inline uint32_t pau_ll_get_regdma_memory_addr(pau_dev_t *dev)
{
    return dev->regdma_mem_addr.val;
}

static inline uint32_t pau_ll_get_regdma_intr_raw_signal(pau_dev_t *dev)
{
    return dev->int_raw.val;
}

static inline uint32_t pau_ll_get_regdma_intr_status(pau_dev_t *dev)
{
    return dev->int_st.val;
}

static inline void pau_ll_set_regdma_backup_done_intr_enable(pau_dev_t *dev, bool enable)
{
    dev->int_ena.done_int_ena = enable;
}

static inline void pau_ll_set_regdma_backup_error_intr_enable(pau_dev_t *dev)
{
    dev->int_ena.error_int_ena = 1;
}

static inline void pau_ll_set_regdma_backup_error_intr_disable(pau_dev_t *dev)
{
    dev->int_ena.error_int_ena = 0;
}

static inline void pau_ll_clear_regdma_backup_done_intr_state(pau_dev_t *dev)
{
    dev->int_clr.val = 0x1;
}

static inline void pau_ll_clear_regdma_backup_error_intr_state(pau_dev_t *dev)
{
    dev->int_clr.val = 0x2;
}


/**
 * @brief Set the maximum number of linked lists supported by REGDMA
 * @param count: the maximum number of regdma link
 */
static inline void pau_ll_set_regdma_link_count(int count)
{
    HAL_FORCE_MODIFY_U32_REG_FIELD(LP_AON.backup_dma_cfg0, branch_link_length_aon, count);
}

/**
 * @brief Set the maximum number of times a single linked list can run for REGDMA. If a linked list continuously reads in a loop
 *        for some reason and the execution count exceeds this configured number, a timeout will be triggered.
 * @param count: the maximum number of loop
 */
static inline void pau_ll_set_regdma_link_loop_threshold(int count)
{
    HAL_FORCE_MODIFY_U32_REG_FIELD(LP_AON.backup_dma_cfg1, link_work_tout_thres_aon, count);
}

/**
 * @brief Set the timeout duration for accessing registers. If REGDMA encounters bus-related issues while accessing
 *        registers and gets stuck on the bus, a timeout will be triggered.
 * @param count: the maximum number of time
 */
static inline void pau_ll_set_regdma_link_reg_access_tout_threshold(int count)
{
    HAL_FORCE_MODIFY_U32_REG_FIELD(LP_AON.backup_dma_cfg1, link_backup_tout_thres_aon, count);
}

/**
 * @brief Set the regdma_link_addr
 * @param addr: the addr of regdma_link
 */
static inline void pau_ll_set_regdma_link_addr(uint32_t addr)
{
    HAL_FORCE_MODIFY_U32_REG_FIELD(LP_AON.backup_dma_cfg2, link_addr_aon, addr);
}

static inline void pau_ll_set_regdma_link_wait_retry_count(int count)
{
    HAL_FORCE_MODIFY_U32_REG_FIELD(LP_AON.backup_dma_cfg1, link_wait_tout_thres_aon, count);
}

static inline void pau_ll_set_regdma_link_wait_read_interval(int interval)
{
    HAL_FORCE_MODIFY_U32_REG_FIELD(LP_AON.backup_dma_cfg0, read_interval_aon, interval);
}

#ifdef __cplusplus
}
#endif
