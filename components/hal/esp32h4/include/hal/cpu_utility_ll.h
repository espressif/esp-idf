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
#include "soc/hp_system_reg.h"
#include "soc/assist_debug_reg.h"
#include "esp_attr.h"
#include "hal/misc.h"

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

#if SOC_CPU_CORES_NUM > 1   // We only allow stalling/unstalling of other cores
FORCE_INLINE_ATTR void cpu_utility_ll_stall_cpu(uint32_t cpu_no)
{
    if (cpu_no == 0) {
        HAL_FORCE_MODIFY_U32_REG_FIELD(LP_AON.cpucore_cfg, aon_cpu_core0_sw_stall, 0x86);
        while(!REG_GET_BIT(HP_SYSTEM_CORE_DEBUG_RUNSTALL_CONF_REG, HP_SYSTEM_CORE0_RUNSTALLED));
    } else {
        HAL_FORCE_MODIFY_U32_REG_FIELD(LP_AON.cpucore_cfg, aon_cpu_core1_sw_stall, 0x86);
        while(!REG_GET_BIT(HP_SYSTEM_CORE_DEBUG_RUNSTALL_CONF_REG, HP_SYSTEM_CORE1_RUNSTALLED));
    }
}

FORCE_INLINE_ATTR void cpu_utility_ll_unstall_cpu(uint32_t cpu_no)
{
    if (cpu_no == 0) {
        HAL_FORCE_MODIFY_U32_REG_FIELD(LP_AON.cpucore_cfg, aon_cpu_core0_sw_stall, 0xFF);
        while(REG_GET_BIT(HP_SYSTEM_CORE_DEBUG_RUNSTALL_CONF_REG, HP_SYSTEM_CORE0_RUNSTALLED));
    } else {
        HAL_FORCE_MODIFY_U32_REG_FIELD(LP_AON.cpucore_cfg, aon_cpu_core1_sw_stall, 0xFF);
        while(REG_GET_BIT(HP_SYSTEM_CORE_DEBUG_RUNSTALL_CONF_REG, HP_SYSTEM_CORE1_RUNSTALLED));
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
    if (!REG_GET_BIT(PCR_CORE1_CONF_REG, PCR_CORE1_CLK_EN)) {
        REG_SET_BIT(PCR_CORE1_CONF_REG, PCR_CORE1_CLK_EN);
    }
    if (REG_GET_BIT(PCR_CORE1_CONF_REG, PCR_CORE1_RST_EN)) {
        REG_CLR_BIT(PCR_CORE1_CONF_REG, PCR_CORE1_RST_EN);
    }
}

FORCE_INLINE_ATTR void cpu_utility_ll_enable_clock_and_reset_app_cpu_int_matrix(void)
{
    if (!REG_GET_BIT(PCR_INTMTX_CONF_REG, PCR_INTMTX_CORE1_CLK_EN)) {
        REG_SET_BIT(PCR_INTMTX_CONF_REG, PCR_INTMTX_CORE1_CLK_EN);
    }
    if (REG_GET_BIT(PCR_INTMTX_CONF_REG, PCR_INTMTX_CORE1_RST_EN)) {
        REG_CLR_BIT(PCR_INTMTX_CONF_REG, PCR_INTMTX_CORE1_RST_EN);
    }
}

#endif // SOC_CPU_CORES_NUM > 1

FORCE_INLINE_ATTR uint32_t cpu_utility_ll_wait_mode(void)
{
    return REG_GET_BIT(PCR_CPU_WAITI_CONF_REG, PCR_CPU0_WAIT_MODE_FORCE_ON);
}

#ifdef __cplusplus
}
#endif
