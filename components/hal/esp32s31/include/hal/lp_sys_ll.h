/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

// The LL layer for ESP32-S31 LP_SYS register operations

#pragma once

#include <stdlib.h>
#include "esp_attr.h"
#include "soc/soc.h"
#include "soc/lp_system_struct.h"
#include "hal/misc.h"
#include "esp32s31/rom/rtc.h"


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

FORCE_INLINE_ATTR void lp_sys_ll_set_pau_link_wait_tout_thres(uint32_t tout)
{
    LP_SYS.backup_dma_cfg1.link_wait_tout_thres_aon = tout;
}

FORCE_INLINE_ATTR void lp_sys_ll_set_pau_link_work_tout_thres(uint32_t tout)
{
    LP_SYS.backup_dma_cfg1.link_work_tout_thres_aon = tout;
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

static inline void lp_sys_set_regdma_link_count(int count)
{
    HAL_FORCE_MODIFY_U32_REG_FIELD(LP_SYS.backup_dma_cfg1, branch_link_length_aon, count);
}

#ifdef __cplusplus
}
#endif
