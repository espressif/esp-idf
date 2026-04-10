/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

// The LL layer for ESP32-H4 LP_AON register operations

#pragma once

#include <stdlib.h>
#include "soc/soc.h"
#include "soc/lp_aon_struct.h"
#include "hal/misc.h"
#include "esp32h4/rom/rtc.h"


#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Get ext1 wakeup source status
 * @return  The lower 6 bits of the returned value are the bitmap of
 *          the wakeup source status, bit 0~5 corresponds to LP_IO 0~5
 */
static inline  uint32_t lp_aon_ll_ext1_get_wakeup_status(void)
{
    return HAL_FORCE_READ_U32_REG_FIELD(LP_AON.ext_wakeup_cntl1, aon_ext_wakeup_status);
}

/**
 * @brief Clear the ext1 wakeup source status
 */
static inline void lp_aon_ll_ext1_clear_wakeup_status(void)
{
    LP_AON.ext_wakeup_cntl1.aon_ext_wakeup_status_clr = 1;
}

/**
 * @brief Set the wake-up LP_IO of the ext1 wake-up source
 * @param io_mask     wakeup LP_IO bitmap, bit 0~5 corresponds to LP_IO 0~5
 * @param level_mask  LP_IO wakeup level bitmap, bit 0~5 corresponds to LP_IO 0~5 wakeup level
 *                    each bit's corresponding position is set to 0, the wakeup level will be low
 *                    on the contrary, each bit's corresponding position is set to 1, the wakeup
 *                    level will be high
 */
static inline void lp_aon_ll_ext1_set_wakeup_pins(uint32_t io_mask, uint32_t level_mask)
{
    uint32_t wakeup_sel_mask = HAL_FORCE_READ_U32_REG_FIELD(LP_AON.ext_wakeup_cntl, aon_ext_wakeup_sel);
    wakeup_sel_mask |= io_mask;
    HAL_FORCE_MODIFY_U32_REG_FIELD(LP_AON.ext_wakeup_cntl, aon_ext_wakeup_sel, wakeup_sel_mask);

    uint32_t wakeup_level_mask = HAL_FORCE_READ_U32_REG_FIELD(LP_AON.ext_wakeup_cntl, aon_ext_wakeup_lv);
    wakeup_level_mask |= io_mask & level_mask;
    wakeup_level_mask &= ~(io_mask & ~level_mask);

    HAL_FORCE_MODIFY_U32_REG_FIELD(LP_AON.ext_wakeup_cntl, aon_ext_wakeup_lv, wakeup_level_mask);
}

/**
 * @brief Clear all ext1 wakup-source setting
 */
static inline  void lp_aon_ll_ext1_clear_wakeup_pins(void)
{
    HAL_FORCE_MODIFY_U32_REG_FIELD(LP_AON.ext_wakeup_cntl, aon_ext_wakeup_sel, 0);
}

/**
 * @brief Get ext1 wakeup source setting
 * @return  The lower 8 bits of the returned value are the bitmap of
 *          the wakeup source status, bit 0~5 corresponds to LP_IO 0~5
 */
static inline  uint32_t lp_aon_ll_ext1_get_wakeup_pins(void)
{
    return HAL_FORCE_READ_U32_REG_FIELD(LP_AON.ext_wakeup_cntl, aon_ext_wakeup_sel);
}


/**
 *  @brief ROM obtains the wake-up type through LP_AON_STORE9_REG[0].
 *         Set the flag to inform
 * @param true: deepsleep      false: lightsleep
 */
static inline  void lp_aon_ll_inform_wakeup_type(bool dslp)
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
    HAL_FORCE_MODIFY_U32_REG_FIELD(LP_AON.backup_dma_cfg0, aon_branch_link_length_aon, count);
}

static inline void lp_aon_ll_set_regdma_link_addr(uint32_t addr)
{
    HAL_FORCE_MODIFY_U32_REG_FIELD(LP_AON.backup_dma_cfg2, aon_link_addr_aon, addr);
}

static inline void lp_aon_ll_set_ldo_sw(uint32_t value)
{
    CLEAR_PERI_REG_MASK(LP_AON_DATE_REG, LP_AON_DREG_LDO_HW);
    REG_SET_FIELD(LP_AON_DATE_REG, LP_AON_DREG_LDO_SW, value);
}

#ifdef __cplusplus
}
#endif
