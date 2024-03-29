/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
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
#include "esp32p4/rom/rtc.h"


#ifdef __cplusplus
extern "C" {
#endif

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

#ifdef __cplusplus
}
#endif
