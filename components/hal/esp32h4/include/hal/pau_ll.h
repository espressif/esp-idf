/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

// The LL layer for ESP32-H4 PAU(Power Assist Unit) register operations

#pragma once

#include <stdlib.h>
#include <stdbool.h>
#include "soc/soc.h"
#include "soc/pau_reg.h"
#include "soc/pau_struct.h"
#include "soc/pcr_struct.h"
#include "soc/lp_aon_struct.h"
#include "soc/lp_aon_reg.h"
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
    return LP_AON.backup_dma_cfg0.aon_regdma_error;
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

static inline void pau_ll_set_regdma_link0_addr(pau_dev_t *dev, void *link_addr)
{
    LP_AON.backup_dma_cfg2.aon_link_addr_aon = (uint32_t)link_addr;
}

static inline void pau_ll_set_regdma_timeout_link_backup_wait(pau_dev_t *dev, uint32_t thres)
{
    REG_SET_FIELD(LP_AON_BACKUP_DMA_CFG1_REG, LP_AON_LINK_BACKUP_TOUT_THRES_AON, thres);
}

static inline void pau_ll_set_regdma_timeout_read_interval(pau_dev_t *dev, uint32_t thres)
{
    REG_SET_FIELD(LP_AON_BACKUP_DMA_CFG0_REG, LP_AON_READ_INTERVAL_AON, thres);
}

static inline void pau_ll_set_regdma_timeout_burst_limit(pau_dev_t *dev, uint32_t thres)
{
    REG_SET_FIELD(LP_AON_BACKUP_DMA_CFG0_REG, LP_AON_BURST_LIMIT_AON, thres);
}
static inline void pau_ll_set_regdma_timeout_max_link_work(pau_dev_t *dev, uint32_t thres)
{
    REG_SET_FIELD(LP_AON_BACKUP_DMA_CFG1_REG, LP_AON_LINK_WORK_TOUT_THRES_AON, thres);
}

static inline void pau_ll_set_regdma_timeout_read_mode_try_time(pau_dev_t *dev, uint32_t thres)
{
    REG_SET_FIELD(LP_AON_BACKUP_DMA_CFG1_REG, LP_AON_LINK_WAIT_TOUT_THRES_AON, thres);
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

static inline void pau_ll_set_regdma_backup_error_intr_enable(pau_dev_t *dev, bool enable)
{
    dev->int_ena.error_int_ena = enable;
}

static inline void pau_ll_clear_regdma_backup_done_intr_state(pau_dev_t *dev)
{
    dev->int_clr.val = 0x1;
}

static inline void pau_ll_clear_regdma_backup_error_intr_state(pau_dev_t *dev)
{
    dev->int_clr.val = 0x2;
}

static inline void pau_ll_arbiter_auto_retry_enable(pau_dev_t *dev, bool ena)
{
    dev->regdma_conf.sw_retry_en = ena;
}

static inline void pau_ll_arbiter_fix_priority_enable(pau_dev_t *dev, bool ena)
{
    dev->regdma_conf.fix_pri_en = ena;
}

/**
 * arbiter result coding:
 * 1: mac_req
 * 2: pmu_req
 * 3: sw_req
 * 4: etm0_req
 * 5: etm1_req
 * 6: etm2_req
 * 7: etm3_req
*/
static inline uint32_t pau_ll_arbiter_get_start_result(pau_dev_t *dev)
{
    return dev->regdma_grant_result.grant_start_result;
}

static inline uint32_t pau_ll_arbiter_get_done_result(pau_dev_t *dev)
{
    return dev->regdma_grant_result.grant_done_result;
}

static inline void pau_ll_arbiter_clr_result_flag(pau_dev_t *dev)
{
    dev->regdma_grant_result.grant_result_clr = 1;
}

static inline bool pau_ll_is_busy(pau_dev_t *dev)
{
    return dev->regdma_conf.paudma_busy;
}

/**
 * @brief Set the maximum number of linked lists supported by REGDMA
 * @param count: the maximum number of regdma link
 */
static inline void pau_ll_set_regdma_link_count(int count)
{
    HAL_FORCE_MODIFY_U32_REG_FIELD(LP_AON.backup_dma_cfg0, aon_branch_link_length_aon, count);
}

/**
 * @brief Set the maximum number of times a single linked list can run for REGDMA. If a linked list continuously reads in a loop
 *        for some reason and the execution count exceeds this configured number, a timeout will be triggered.
 * @param count: the maximum number of loop
 */
static inline void pau_ll_set_regdma_link_loop_threshold(int count)
{
    HAL_FORCE_MODIFY_U32_REG_FIELD(LP_AON.backup_dma_cfg1, aon_link_work_tout_thres_aon, count);
}

/**
 * @brief Set the timeout duration for accessing registers. If REGDMA encounters bus-related issues while accessing
 *        registers and gets stuck on the bus, a timeout will be triggered.
 * @param count: the maximum number of time
 */
static inline void pau_ll_set_regdma_link_reg_access_tout_threshold(int count)
{
    HAL_FORCE_MODIFY_U32_REG_FIELD(LP_AON.backup_dma_cfg1, aon_link_backup_tout_thres_aon, count);
}

/**
 * @brief Set the regdma_link_addr
 * @param addr: the addr of regdma_link
 */
static inline void pau_ll_set_regdma_link_addr(uint32_t addr)
{
    LP_AON.backup_dma_cfg2.aon_link_addr_aon = addr;
}

static inline void pau_ll_set_regdma_link_wait_retry_count(int count)
{
    HAL_FORCE_MODIFY_U32_REG_FIELD(LP_AON.backup_dma_cfg1, aon_link_wait_tout_thres_aon, count);
}

static inline void pau_ll_set_regdma_link_wait_read_interval(int interval)
{
    HAL_FORCE_MODIFY_U32_REG_FIELD(LP_AON.backup_dma_cfg0, aon_read_interval_aon, interval);
}

#ifdef __cplusplus
}
#endif
