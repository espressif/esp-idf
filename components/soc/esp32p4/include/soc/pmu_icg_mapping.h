/*
 * SPDX-FileCopyrightText: 2025-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0 OR MIT
 */

#pragma once

#include <stdint.h>

#define PMU_ICG_APB_ENA_CORE0_CPU          0
#define PMU_ICG_APB_ENA_CORE1_CPU          1
#define PMU_ICG_APB_ENA_CORE0_CLIC         2
#define PMU_ICG_APB_ENA_CORE1_CLIC         3
#define PMU_ICG_APB_ENA_MISC_CPU           4
#define PMU_ICG_APB_ENA_MISC_SYS           5
#define PMU_ICG_APB_ENA_ICM_SYS            6
#define PMU_ICG_APB_ENA_ICM_CPU            7
#define PMU_ICG_APB_ENA_ICM_MEM            8
#define PMU_ICG_APB_ENA_ICM_APB            9
#define PMU_ICG_APB_ENA_TCM_CPU            10
#define PMU_ICG_APB_ENA_L2MEM_MEM          11
#define PMU_ICG_APB_ENA_L2MEM_SYS          12
#define PMU_ICG_APB_ENA_L1CACHE_CPU        13
#define PMU_ICG_APB_ENA_L1CACHE_D_CPU      14
#define PMU_ICG_APB_ENA_L1CACHE_I0_CPU     15
#define PMU_ICG_APB_ENA_L1CACHE_I1_CPU     16
#define PMU_ICG_APB_ENA_L1CACHE_MEM        17
#define PMU_ICG_APB_ENA_L1CACHE_D_MEM      18
#define PMU_ICG_APB_ENA_L1CACHE_I0_MEM     19
#define PMU_ICG_APB_ENA_L1CACHE_I1_MEM     20
#define PMU_ICG_APB_ENA_L2CACHE_MEM        21
#define PMU_ICG_APB_ENA_L2CACHE_SYS        22
#define PMU_ICG_APB_ENA_REGDMA             23
#define PMU_ICG_APB_ENA_HP_CLKRST          24
#define PMU_ICG_APB_ENA_SYSREG_APB         25
#define PMU_ICG_APB_ENA_INTRMTX_APB        26

#define PMU_ICG_FUNC_ENA_CORE0_CPU          0
#define PMU_ICG_FUNC_ENA_CORE1_CPU          1
#define PMU_ICG_FUNC_ENA_CORE0_CLIC         2
#define PMU_ICG_FUNC_ENA_CORE1_CLIC         3
#define PMU_ICG_FUNC_ENA_MISC_CPU           4
#define PMU_ICG_FUNC_ENA_MISC_SYS           5
#define PMU_ICG_FUNC_ENA_ICM_SYS            6
#define PMU_ICG_FUNC_ENA_ICM_CPU            7
#define PMU_ICG_FUNC_ENA_ICM_MEM            8
#define PMU_ICG_FUNC_ENA_ICM_APB            9
#define PMU_ICG_FUNC_ENA_TCM_CPU            10
#define PMU_ICG_FUNC_ENA_L2MEM_MEM          11
#define PMU_ICG_FUNC_ENA_L2MEM_SYS          12
#define PMU_ICG_FUNC_ENA_L1CACHE_CPU        13
#define PMU_ICG_FUNC_ENA_L1CACHE_D_CPU      14
#define PMU_ICG_FUNC_ENA_L1CACHE_I0_CPU     15
#define PMU_ICG_FUNC_ENA_L1CACHE_I1_CPU     16
#define PMU_ICG_FUNC_ENA_L1CACHE_MEM        17
#define PMU_ICG_FUNC_ENA_L1CACHE_D_MEM      18
#define PMU_ICG_FUNC_ENA_L1CACHE_I0_MEM     19
#define PMU_ICG_FUNC_ENA_L1CACHE_I1_MEM     20
#define PMU_ICG_FUNC_ENA_L2CACHE_MEM        21
#define PMU_ICG_FUNC_ENA_L2CACHE_SYS        22
#define PMU_ICG_FUNC_ENA_REGDMA             23
#define PMU_ICG_FUNC_ENA_HP_CLKRST          24
#define PMU_ICG_FUNC_ENA_SYSREG_APB         25
#define PMU_ICG_FUNC_ENA_INTRMTX_APB        26

#ifdef __cplusplus
extern "C" {
#endif

typedef uint32_t pmu_sleep_clk_icg_flags_t;
#define PMU_SLEEP_CLK_ICG_BIT(b)       (((pmu_sleep_clk_icg_flags_t)1) << (b))

#ifdef __cplusplus
}
#endif
