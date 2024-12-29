/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

// The LL layer for ESP32-C5 LP_AON register operations

#pragma once

#include <stdlib.h>
#include "soc/soc.h"
#include "soc/lp_aon_struct.h"
#include "hal/misc.h"
#include "esp32c5/rom/rtc.h"


#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Get ext1 wakeup source status
 * @return  The lower 8 bits of the returned value are the bitmap of
 *          the wakeup source status, bit 0~7 corresponds to LP_IO 0~7
 */
static inline  uint32_t lp_aon_ll_ext1_get_wakeup_status(void)
{
    return HAL_FORCE_READ_U32_REG_FIELD(LP_AON.ext_wakeup_cntl, ext_wakeup_status);
}

/**
 * @brief Clear the ext1 wakeup source status
 */
static inline void lp_aon_ll_ext1_clear_wakeup_status(void)
{
    HAL_FORCE_MODIFY_U32_REG_FIELD(LP_AON.ext_wakeup_cntl, ext_wakeup_status_clr, 1);
}

/**
 * @brief Set the wake-up LP_IO of the ext1 wake-up source
 * @param io_mask     wakeup LP_IO bitmap, bit 0~7 corresponds to LP_IO 0~7
 * @param level_mask  LP_IO wakeup level bitmap, bit 0~7 corresponds to LP_IO 0~7 wakeup level
 *                    each bit's corresponding position is set to 0, the wakeup level will be low
 *                    on the contrary, each bit's corresponding position is set to 1, the wakeup
 *                    level will be high
 */
static inline void lp_aon_ll_ext1_set_wakeup_pins(uint32_t io_mask, uint32_t level_mask)
{
    HAL_FORCE_MODIFY_U32_REG_FIELD(LP_AON.ext_wakeup_cntl, ext_wakeup_sel, io_mask);
    HAL_FORCE_MODIFY_U32_REG_FIELD(LP_AON.ext_wakeup_cntl, ext_wakeup_lv, level_mask);
}

/**
 * @brief Clear all ext1 wakup-source setting
 */
static inline void lp_aon_ll_ext1_clear_wakeup_pins(void)
{
    HAL_FORCE_MODIFY_U32_REG_FIELD(LP_AON.ext_wakeup_cntl, ext_wakeup_sel, 0);
}

/**
 * @brief Get ext1 wakeup source setting
 * @return  The lower 8 bits of the returned value are the bitmap of
 *          the wakeup source status, bit 0~7 corresponds to LP_IO 0~7
 */
static inline uint32_t lp_aon_ll_ext1_get_wakeup_pins(void)
{
    return HAL_FORCE_READ_U32_REG_FIELD(LP_AON.ext_wakeup_cntl, ext_wakeup_sel);
}


/**
 *  @brief ROM obtains the wake-up type through LP_AON_STORE9_REG[0].
 *         Set the flag to inform
 * @param true: deepsleep      false: lightsleep
 */
static inline void lp_aon_ll_inform_wakeup_type(bool dslp)
{
    if (dslp) {
        REG_SET_BIT(RTC_SLEEP_MODE_REG, BIT(0));    /* Tell rom to run deep sleep wake stub */

    } else {
        REG_CLR_BIT(RTC_SLEEP_MODE_REG, BIT(0));    /* Tell rom to run light sleep wake stub */
    }
}

/**
 * @brief Set the maximum number of linked lists supported by REGDMA
 * @param count: the maximum number of regdma link
 */
static inline void lp_aon_ll_set_regdma_link_count(int count)
{
    HAL_FORCE_MODIFY_U32_REG_FIELD(LP_AON.backup_dma_cfg0, branch_link_length_aon, count);
}

/**
 * @brief Set the maximum number of times a single linked list can run for REGDMA. If a linked list continuously reads in a loop
 *        for some reason and the execution count exceeds this configured number, a timeout will be triggered.
 * @param count: the maximum number of loop
 */
static inline void lp_aon_ll_set_regdma_link_loop_threshold(int count)
{
    HAL_FORCE_MODIFY_U32_REG_FIELD(LP_AON.backup_dma_cfg1, link_work_tout_thres_aon, count);
}

/**
 * @brief Set the timeout duration for accessing registers. If REGDMA encounters bus-related issues while accessing
 *        registers and gets stuck on the bus, a timeout will be triggered.
 * @param count: the maximum number of time
 */
static inline void lp_aon_ll_set_regdma_link_reg_access_tout_threshold(int count)
{
    HAL_FORCE_MODIFY_U32_REG_FIELD(LP_AON.backup_dma_cfg1, link_backup_tout_thres_aon, count);
}

/**
 * @brief Set the regdma_link_addr
 * @param addr: the addr of regdma_link
 */
static inline void lp_aon_ll_set_regdma_link_addr(uint32_t addr)
{
    HAL_FORCE_MODIFY_U32_REG_FIELD(LP_AON.backup_dma_cfg2, link_addr_aon, addr);
}

static inline void lp_aon_ll_set_regdma_link_wait_retry_count(int count)
{
    HAL_FORCE_MODIFY_U32_REG_FIELD(LP_AON.backup_dma_cfg1, link_wait_tout_thres_aon, count);
}

static inline void lp_aon_ll_set_regdma_link_wait_read_interval(int interval)
{
    HAL_FORCE_MODIFY_U32_REG_FIELD(LP_AON.backup_dma_cfg0, read_interval_aon, interval);
}

#ifdef __cplusplus
}
#endif
