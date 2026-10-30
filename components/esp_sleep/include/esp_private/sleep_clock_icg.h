/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>

#include "esp_err.h"
#include "esp_macros.h"
#include "sdkconfig.h"
#include "soc/soc_caps.h"
#include "esp_private/esp_pmu.h"
#if SOC_PM_SUPPORT_PMU_CLK_ICG
#include "soc/pmu_icg_mapping.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if SOC_PM_SUPPORT_PMU_CLK_ICG

#if SOC_PM_SUPPORT_PMU_RETENTION_CLK_ICG

#include "esp_private/sleep_retention.h"

/**
 * @brief Convert one or more PMU_ICG_FUNC_ENA_xxx cells into an ICG bitmask
 */
#define PMU_CLK_ICG(clock)                 PMU_SLEEP_CLK_ICG_BIT(PMU_ICG_FUNC_ENA_##clock)
#define PMU_CLK_ICGS_1(a)                  PMU_CLK_ICG(a)
#define PMU_CLK_ICGS_2(a, b)               PMU_CLK_ICG(a) | PMU_CLK_ICG(b)
#define PMU_CLK_ICGS_3(a, b, c)            PMU_CLK_ICG(a) | PMU_CLK_ICGS_2(b, c)
#define PMU_CLK_ICGS_4(a, b, c, d)         PMU_CLK_ICG(a) | PMU_CLK_ICGS_3(b, c, d)
#define PMU_CLK_ICGS_5(a, b, c, d, ...)    PMU_CLK_ICGS_4(a, b, c, d) | PMU_CLK_ICGS_1(__VA_ARGS__)
#define PMU_CLK_ICGS_6(a, b, c, d, ...)    PMU_CLK_ICGS_4(a, b, c, d) | PMU_CLK_ICGS_2(__VA_ARGS__)
#define PMU_CLK_ICGS_7(a, b, c, d, ...)    PMU_CLK_ICGS_4(a, b, c, d) | PMU_CLK_ICGS_3(__VA_ARGS__)
#define PMU_CLK_ICGS_8(a, b, c, d, ...)    PMU_CLK_ICGS_4(a, b, c, d) | PMU_CLK_ICGS_4(__VA_ARGS__)
#define PMU_CLK_ICGS_9(a, b, c, d, ...)    PMU_CLK_ICGS_4(a, b, c, d) | PMU_CLK_ICGS_5(__VA_ARGS__)
#define PMU_CLK_ICGS_10(a, b, c, d, ...)   PMU_CLK_ICGS_4(a, b, c, d) | PMU_CLK_ICGS_6(__VA_ARGS__)
#define PMU_CLK_ICGS_11(a, b, c, d, ...)   PMU_CLK_ICGS_4(a, b, c, d) | PMU_CLK_ICGS_7(__VA_ARGS__)
#define PMU_CLK_ICGS_12(a, b, c, d, ...)   PMU_CLK_ICGS_4(a, b, c, d) | PMU_CLK_ICGS_8(__VA_ARGS__)
#define PMU_CLK_ICGS_13(a, b, c, d, ...)   PMU_CLK_ICGS_4(a, b, c, d) | PMU_CLK_ICGS_9(__VA_ARGS__)
#define PMU_CLK_ICGS_14(a, b, c, d, ...)   PMU_CLK_ICGS_4(a, b, c, d) | PMU_CLK_ICGS_10(__VA_ARGS__)

#define PMU_CLK_ICGS_N(n)                  PMU_CLK_ICGS_##n
#define PMU_CLK_ICGS_DISPATCH(n, ...)      PMU_CLK_ICGS_N(n)(__VA_ARGS__)
#define PMU_CLK_ICGS(...) \
    PMU_CLK_ICGS_DISPATCH(ESP_VA_NARG(__VA_ARGS__), __VA_ARGS__)

/**
 * @brief Program HP backup ICG registers from PMU sleep data
 *
 * @param data pointer to pmu_sleep_data_t
 * @param ctx  pointer to pmu_context_t
 */
void pmu_sleep_retention_clock_icg_config(void *data, pmu_context_t *ctx);

/**
 * @brief Recompute the backup clock ICG mask from the current retention module bitmap
 *
 * Called whenever the REGDMA retention module bitmap changes (module created/destroyed/
 * attached/detached), so that the backup ICG flags always reflect which peripherals
 * currently participate in retention.
 *
 * @param module_bitmap pointer to current retention module bitmap
 */
void sleep_clock_icg_retention_clock_config(sleep_retention_module_bitmap_t *module_bitmap);

#endif /* SOC_PM_SUPPORT_PMU_RETENTION_CLK_ICG */

/**
 * @brief Convert sleep clock ICG reference counts to PMU ICG flags for HP sleep mode
 *
 * Caller must zero-initialize both outputs; this function only ORs bits into them.
 *
 * @param clk_icg0_flags  ICG flags bits[31:0]
 * @param clk_icg1_flags  ICG flags bits[63:32] (untouched if the target only has 32-bit ICG)
 */
void sleep_clock_icg_get_icg_flags(uint32_t *clk_icg0_flags, uint32_t *clk_icg1_flags);

#endif /* SOC_PM_SUPPORT_PMU_CLK_ICG */

#ifdef __cplusplus
}
#endif
