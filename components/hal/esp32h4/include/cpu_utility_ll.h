/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once
#include "soc/soc.h"
#include "soc/lp_aon_reg.h"
#include "soc/lp_aon_struct.h"
#include "soc/pcr_reg.h"
#include "esp_attr.h"

//TODO: [ESP32H4] IDF-12484, need check

#ifdef __cplusplus
extern "C" {
#endif

FORCE_INLINE_ATTR void cpu_utility_ll_reset_cpu(uint32_t cpu_no)
{
    if (cpu_no == 0) {
        LP_AON.cpucore_cfg.aon_cpu_core0_sw_reset = 1;
    } else {
        LP_AON.cpucore_cfg.aon_cpu_core1_sw_reset = 1;
    }
}

FORCE_INLINE_ATTR uint32_t cpu_utility_ll_wait_mode(void)
{
    return REG_GET_BIT(PCR_CPU_WAITI_CONF_REG, PCR_CPU0_WAIT_MODE_FORCE_ON);
}

FORCE_INLINE_ATTR void cpu_utility_ll_enable_debug(uint32_t cpu_no)
{
    // TODO
}

FORCE_INLINE_ATTR void cpu_utility_ll_enable_record(uint32_t cpu_no)
{
    // TODO
}

FORCE_INLINE_ATTR void cpu_utility_ll_enable_clock_and_reset_app_cpu(void)
{
    if (!REG_GET_BIT(PCR_CORE1_CONF_REG, PCR_CORE1_CLK_EN)) {
        REG_SET_BIT(PCR_CORE1_CONF_REG, PCR_CORE1_CLK_EN);
    }
    if (REG_GET_BIT(PCR_CORE1_CONF_REG, PCR_CORE1_RST_EN)) {
        REG_CLR_BIT(PCR_CORE1_CONF_REG, PCR_CORE1_RST_EN);
    }
}

#ifdef __cplusplus
}
#endif
