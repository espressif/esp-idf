/*
 * SPDX-FileCopyrightText: 2023-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

// The LL layer for ESP32-C6 LP_SYS register operations

#pragma once

#include <stdlib.h>
#include "esp_attr.h"
#include "soc/soc.h"
#include "soc/lp_system_struct.h"
#include "hal/misc.h"
#include "hal/config.h"
#include "esp32p4/rom/rtc.h"


#ifdef __cplusplus
extern "C" {
#endif

#define MEM_AUX_SHUTDOWN         BIT(0)
#define MEM_AUX_LIGHTSLEEP       BIT(1)
#define MEM_AUX_DEEPSLEEP        BIT(2)

/**
 * @brief ROM obtains the wake-up type through LP_SYS_STORE9_REG[0].
 *         Set the flag to inform
 * @param true: deepsleep      false: lightsleep
 */
FORCE_INLINE_ATTR void lp_sys_ll_inform_wakeup_type(bool dslp)
{
    if (dslp) {
        REG_SET_BIT(RTC_SLEEP_MODE_REG, BIT(0));    /* Tell rom to run deep sleep wake stub */

    } else {
        REG_CLR_BIT(RTC_SLEEP_MODE_REG, BIT(0));    /* Tell rom to run light sleep wake stub */
    }
}

FORCE_INLINE_ATTR void lp_sys_ll_set_pau_aon_bypass(bool bypass)
{
    LP_SYS.backup_dma_cfg1.aon_bypass = bypass ? 1 : 0;
}

FORCE_INLINE_ATTR bool lp_sys_ll_get_pau_aon_bypass(void)
{
    return LP_SYS.backup_dma_cfg1.aon_bypass;
}

FORCE_INLINE_ATTR void lp_sys_ll_set_pau_link_tout_thres(uint32_t tout)
{
    LP_SYS.backup_dma_cfg0.link_tout_thres_aon = tout;
}

FORCE_INLINE_ATTR void lp_sys_ll_set_pau_link_backup_tout_thres(uint32_t tout)
{
    LP_SYS.backup_dma_cfg0.link_backup_tout_thres_aon = tout;
}

FORCE_INLINE_ATTR void lp_sys_ll_set_pau_reg_read_interval(uint32_t val)
{
    LP_SYS.backup_dma_cfg0.read_interval_aon = val;
}

FORCE_INLINE_ATTR void lp_sys_ll_set_pau_link_addr(uint32_t addr)
{
    LP_SYS.backup_dma_cfg2.link_addr_aon = addr;
}

#if HAL_CONFIG(CHIP_SUPPORT_MIN_REV) >= 300
FORCE_INLINE_ATTR void lp_sys_ll_set_hp_mem_lowpower_mode(uint32_t mode)
{
    LP_SYS.hp_mem_aux_ctrl.hp_mem_lowpower_mode = mode;
}

FORCE_INLINE_ATTR void lp_sys_ll_set_lp_mem_lowpower_mode(uint32_t mode)
{
    LP_SYS.lp_mem_aux_ctrl.lp_mem_lowpower_mode = mode;
}
#endif
#ifdef __cplusplus
}
#endif
