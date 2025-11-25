/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once
#include "soc/soc.h"
#include "soc/soc_caps.h"
#include "soc/lp_clkrst_struct.h"
#include "soc/pmu_struct.h"
#include "soc/hp_system_reg.h"
#include "soc/hp_sys_clkrst_reg.h"
#include "esp_attr.h"
#include "hal/misc.h"

#ifdef __cplusplus
extern "C" {
#endif

FORCE_INLINE_ATTR void cpu_utility_ll_reset_cpu(uint32_t cpu_no)
{
    // TODO: [ESP32S31] IDF-14655
}

FORCE_INLINE_ATTR void cpu_utility_ll_stall_cpu(uint32_t cpu_no)
{
    // TODO: [ESP32S31] IDF-14655
}

FORCE_INLINE_ATTR void cpu_utility_ll_unstall_cpu(uint32_t cpu_no)
{
    // TODO: [ESP32S31] IDF-14655
}

FORCE_INLINE_ATTR void cpu_utility_ll_enable_debug(uint32_t cpu_no)
{
    // TODO: [ESP32S31] IDF-14655
}

FORCE_INLINE_ATTR void cpu_utility_ll_enable_record(uint32_t cpu_no)
{
    // TODO: [ESP32S31] IDF-14655
}

FORCE_INLINE_ATTR void cpu_utility_ll_enable_clock_and_reset_app_cpu(void)
{
    // TODO: [ESP32S31] IDF-14655
    if (!REG_GET_BIT(HP_SYS_CLKRST_HPCORE1_CTRL0_REG, HP_SYS_CLKRST_REG_CORE1_CPU_CLK_EN)) {
        REG_SET_BIT(HP_SYS_CLKRST_HPCORE1_CTRL0_REG, HP_SYS_CLKRST_REG_CORE1_CPU_CLK_EN);
    }

    if (!REG_GET_BIT(HP_SYS_CLKRST_HPCORE1_CTRL0_REG, HP_SYS_CLKRST_REG_CORE1_CLIC_CLK_EN)) {
        REG_SET_BIT(HP_SYS_CLKRST_HPCORE1_CTRL0_REG, HP_SYS_CLKRST_REG_CORE1_CLIC_CLK_EN);
    }

    if (REG_GET_BIT(HP_SYS_CLKRST_HPCORE1_CTRL0_REG, HP_SYS_CLKRST_REG_CORE1_GLOBAL_RST_EN)) {
        REG_CLR_BIT(HP_SYS_CLKRST_HPCORE1_CTRL0_REG, HP_SYS_CLKRST_REG_CORE1_GLOBAL_RST_EN);
    }
}

FORCE_INLINE_ATTR uint32_t cpu_utility_ll_wait_mode(void)
{
    return 0;// TODO: [ESP32S31] IDF-14655
}

FORCE_INLINE_ATTR void cpu_utility_ll_enable_clock_and_reset_app_cpu_int_matrix(void)
{
    // TODO: [ESP32S31] IDF-14655
}

#ifdef __cplusplus
}
#endif
