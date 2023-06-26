/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

// The LL layer for ESP32-C6 LP_AON register operations

#pragma once

#include <stdlib.h>
#include "soc/soc.h"
#include "soc/lp_aon_struct.h"
#include "hal/misc.h"
#include "esp32c6/rom/rtc.h"


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
 * @param mask wakeup LP_IO bitmap, bit 0~7 corresponds to LP_IO 0~7
 * @param mode 0: Wake the chip when all selected GPIOs go low
 *             1: Wake the chip when any of the selected GPIOs go high
 */
static inline  void lp_aon_ll_ext1_set_wakeup_pins(uint32_t mask, int mode)
{
    uint32_t wakeup_sel_mask = HAL_FORCE_READ_U32_REG_FIELD(LP_AON.ext_wakeup_cntl, ext_wakeup_sel);
    wakeup_sel_mask |= mask;
    HAL_FORCE_MODIFY_U32_REG_FIELD(LP_AON.ext_wakeup_cntl, ext_wakeup_sel, wakeup_sel_mask);

    uint32_t wakeup_level_mask = HAL_FORCE_READ_U32_REG_FIELD(LP_AON.ext_wakeup_cntl, ext_wakeup_lv);
    if (mode) {
        wakeup_level_mask |= mask;
    } else {
        wakeup_level_mask &= ~mask;
    }
    HAL_FORCE_MODIFY_U32_REG_FIELD(LP_AON.ext_wakeup_cntl, ext_wakeup_lv, wakeup_level_mask);
}

/**
 * @brief Clear all ext1 wakup-source setting
 */
static inline  void lp_aon_ll_ext1_clear_wakeup_pins(void)
{
    HAL_FORCE_MODIFY_U32_REG_FIELD(LP_AON.ext_wakeup_cntl, ext_wakeup_sel, 0);
}

/**
 * @brief Get ext1 wakeup source setting
 * @return  The lower 8 bits of the returned value are the bitmap of
 *          the wakeup source status, bit 0~7 corresponds to LP_IO 0~7
 */
static inline  uint32_t lp_aon_ll_ext1_get_wakeup_pins(void)
{
    return HAL_FORCE_READ_U32_REG_FIELD(LP_AON.ext_wakeup_cntl, ext_wakeup_sel);
}


/**
 *  @brief ROM obtains the wake-up type through LP_AON_STORE9_REG[0].
 *         Set the flag to inform
 * @param true: deepsleep      false: lightsleep
 */
static inline  void lp_aon_ll_inform_wakeup_type(bool dslp)
{
    if (dslp) {
        REG_SET_BIT(SLEEP_MODE_REG, BIT(0));    /* Tell rom to run deep sleep wake stub */

    } else {
        REG_CLR_BIT(SLEEP_MODE_REG, BIT(0));    /* Tell rom to run light sleep wake stub */
    }
}

#ifdef __cplusplus
}
#endif
