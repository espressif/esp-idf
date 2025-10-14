/*
 * SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
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
#include "soc/assist_debug_reg.h"
#include "esp_attr.h"
#include "hal/misc.h"

#ifdef __cplusplus
extern "C" {
#endif

FORCE_INLINE_ATTR void cpu_utility_ll_reset_cpu(uint32_t cpu_no)
{
    if (cpu_no == 0) {
        LP_AON_CLKRST.hpcpu_reset_ctrl0.hpcore0_sw_reset = 1;
    } else {
        LP_AON_CLKRST.hpcpu_reset_ctrl0.hpcore1_sw_reset = 1;
    }
}

#if SOC_CPU_CORES_NUM > 1   // We only allow stalling/unstalling of other cores
FORCE_INLINE_ATTR void cpu_utility_ll_stall_cpu(uint32_t cpu_no)
{
    if (cpu_no == 0) {
        HAL_FORCE_MODIFY_U32_REG_FIELD(PMU.cpu_sw_stall, hpcore0_stall_code, 0x86);
        while(!REG_GET_BIT(HP_SYSTEM_CPU_CORESTALLED_ST_REG, HP_SYSTEM_REG_CORE0_CORESTALLED_ST));
    } else {
        HAL_FORCE_MODIFY_U32_REG_FIELD(PMU.cpu_sw_stall, hpcore1_stall_code, 0x86);
        while(!REG_GET_BIT(HP_SYSTEM_CPU_CORESTALLED_ST_REG, HP_SYSTEM_REG_CORE1_CORESTALLED_ST));
    }
}

FORCE_INLINE_ATTR void cpu_utility_ll_unstall_cpu(uint32_t cpu_no)
{
    if (cpu_no == 0) {
        HAL_FORCE_MODIFY_U32_REG_FIELD(PMU.cpu_sw_stall, hpcore0_stall_code, 0xFF);
        while(REG_GET_BIT(HP_SYSTEM_CPU_CORESTALLED_ST_REG, HP_SYSTEM_REG_CORE0_CORESTALLED_ST));
    } else {
        HAL_FORCE_MODIFY_U32_REG_FIELD(PMU.cpu_sw_stall, hpcore1_stall_code, 0xFF);
        while(REG_GET_BIT(HP_SYSTEM_CPU_CORESTALLED_ST_REG, HP_SYSTEM_REG_CORE1_CORESTALLED_ST));
    }
}

FORCE_INLINE_ATTR void cpu_utility_ll_enable_debug(uint32_t cpu_no)
{
    if (cpu_no == 0) {
        REG_SET_BIT(ASSIST_DEBUG_CORE_0_RCD_EN_REG, ASSIST_DEBUG_CORE_0_RCD_PDEBUGEN);
    } else {
        REG_SET_BIT(ASSIST_DEBUG_CORE_1_RCD_EN_REG, ASSIST_DEBUG_CORE_1_RCD_PDEBUGEN);
    }
}

FORCE_INLINE_ATTR void cpu_utility_ll_enable_record(uint32_t cpu_no)
{
    if (cpu_no == 0) {
        REG_SET_BIT(ASSIST_DEBUG_CORE_0_RCD_EN_REG, ASSIST_DEBUG_CORE_0_RCD_RECORDEN);
    } else {
        REG_SET_BIT(ASSIST_DEBUG_CORE_1_RCD_EN_REG, ASSIST_DEBUG_CORE_1_RCD_RECORDEN);
    }
}

FORCE_INLINE_ATTR void cpu_utility_ll_enable_clock_and_reset_app_cpu(void)
{
    if (!REG_GET_BIT(HP_SYS_CLKRST_SOC_CLK_CTRL0_REG, HP_SYS_CLKRST_REG_CORE1_CPU_CLK_EN)) {
        REG_SET_BIT(HP_SYS_CLKRST_SOC_CLK_CTRL0_REG, HP_SYS_CLKRST_REG_CORE1_CPU_CLK_EN);
    }
    if (REG_GET_BIT(HP_SYS_CLKRST_HP_RST_EN0_REG, HP_SYS_CLKRST_REG_RST_EN_CORE1_GLOBAL)) {
        REG_CLR_BIT(HP_SYS_CLKRST_HP_RST_EN0_REG, HP_SYS_CLKRST_REG_RST_EN_CORE1_GLOBAL);
    }
}

FORCE_INLINE_ATTR void cpu_utility_ll_enable_clock_and_reset_app_cpu_int_matrix(void)
{
}

#endif // SOC_CPU_CORES_NUM > 1

FORCE_INLINE_ATTR uint32_t cpu_utility_ll_wait_mode(void)
{
    return REG_GET_BIT(HP_SYSTEM_CPU_WAITI_CONF_REG, HP_SYSTEM_CPU_WAIT_MODE_FORCE_ON);
}

#ifdef __cplusplus
}
#endif
