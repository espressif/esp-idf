/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once
#include "soc/soc.h"
#include "soc/soc_caps.h"
#include "soc/lp_clkrst_struct.h"
#include "soc/pmu_struct.h"
#include "soc/hp_system_reg.h"
#include "esp_attr.h"

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
        PMU.cpu_sw_stall.hpcore0_stall_code = 0x86;
    } else {
        PMU.cpu_sw_stall.hpcore1_stall_code = 0x86;
    }
}

FORCE_INLINE_ATTR void cpu_utility_ll_unstall_cpu(uint32_t cpu_no)
{
    if (cpu_no == 0) {
        PMU.cpu_sw_stall.hpcore0_stall_code = 0xFF;
    } else {
        PMU.cpu_sw_stall.hpcore1_stall_code = 0xFF;
    }
}
#endif // SOC_CPU_CORES_NUM > 1

FORCE_INLINE_ATTR uint32_t cpu_utility_ll_wait_mode(void)
{
    return REG_GET_BIT(HP_SYSTEM_CPU_WAITI_CONF_REG, HP_SYSTEM_CPU_WAIT_MODE_FORCE_ON);
}

#ifdef __cplusplus
}
#endif
