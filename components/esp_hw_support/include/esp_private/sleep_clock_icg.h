/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>

#include "esp_err.h"
#include "sdkconfig.h"
#include "soc/soc_caps.h"
#if SOC_PM_SUPPORT_PMU_CLK_ICG
#include "soc/pmu_icg_mapping.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if SOC_PM_SUPPORT_PMU_CLK_ICG

/**
 * @brief Convert sleep clock ICG reference counts to PMU ICG flags for HP sleep mode
 */
pmu_sleep_clk_icg_flags_t esp_sleep_clock_get_clk_icg_flags(void);

#endif /* SOC_PM_SUPPORT_PMU_CLK_ICG */

#ifdef __cplusplus
}
#endif
