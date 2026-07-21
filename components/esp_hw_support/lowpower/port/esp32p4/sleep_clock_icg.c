/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <assert.h>
#include <stdint.h>

#include "esp_attr.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_private/esp_sleep_internal.h"
#include "esp_private/esp_pmu.h"
#include "esp_private/sleep_clock_icg.h"
#include "sdkconfig.h"
#include "soc/soc_caps.h"

ESP_LOG_ATTR_TAG(TAG, "sleep_clock_icg");

#if CONFIG_PM_SLP_IRAM_OPT
#define SLEEP_CLOCK_ICG_FN_ATTR FORCE_IRAM_ATTR
#else
#define SLEEP_CLOCK_ICG_FN_ATTR
#endif

static int16_t s_sleep_clock_icg_refs[ESP_SLEEP_CLOCK_MAX];

SLEEP_CLOCK_ICG_FN_ATTR void sleep_clock_icg_get_icg_flags(uint32_t *clk_icg0_flags, uint32_t *clk_icg1_flags)
{
    if (s_sleep_clock_icg_refs[ESP_SLEEP_CLOCK_IOMUX] > 0) {
        *clk_icg0_flags |= PMU_SLEEP_CLK_ICG_BIT(PMU_ICG_FUNC_ENA_IOMUX);
    }
    if (s_sleep_clock_icg_refs[ESP_SLEEP_CLOCK_LEDC0] > 0) {
        *clk_icg0_flags |= PMU_SLEEP_CLK_ICG_BIT(PMU_ICG_FUNC_ENA_LEDC0);
    }
    if (s_sleep_clock_icg_refs[ESP_SLEEP_CLOCK_UART0] > 0) {
        *clk_icg0_flags |= PMU_SLEEP_CLK_ICG_BIT(PMU_ICG_FUNC_ENA_UART0);
    }
    if (s_sleep_clock_icg_refs[ESP_SLEEP_CLOCK_UART1] > 0) {
        *clk_icg0_flags |= PMU_SLEEP_CLK_ICG_BIT(PMU_ICG_FUNC_ENA_UART1);
    }
#if SOC_UART_HP_NUM > 2
    if (s_sleep_clock_icg_refs[ESP_SLEEP_CLOCK_UART2] > 0) {
        *clk_icg0_flags |= PMU_SLEEP_CLK_ICG_BIT(PMU_ICG_FUNC_ENA_UART2);
    }
#endif
#if SOC_UART_HP_NUM > 3
    if (s_sleep_clock_icg_refs[ESP_SLEEP_CLOCK_UART3] > 0) {
        *clk_icg0_flags |= PMU_SLEEP_CLK_ICG_BIT(PMU_ICG_FUNC_ENA_UART3);
    }
#endif
#if SOC_UART_HP_NUM > 4
    if (s_sleep_clock_icg_refs[ESP_SLEEP_CLOCK_UART4] > 0) {
        *clk_icg0_flags |= PMU_SLEEP_CLK_ICG_BIT(PMU_ICG_FUNC_ENA_UART4);
    }
#endif

    (void)clk_icg1_flags;
}

esp_err_t esp_sleep_clock_config(esp_sleep_clock_t clock, esp_sleep_clock_option_t option)
{
#if SOC_PM_SLEEP_CLK_ICG_USE_REGDMA && !CONFIG_PM_SLEEP_CLK_ICG_ENABLE
    static bool warned;
    if (!warned) {
        warned = true;
        ESP_LOGW(TAG, "PM_SLEEP_CLK_ICG_ENABLE is disabled. "
                 "Clock ungate requests may not be honored during sleep.");
    }
#endif

    if (clock >= ESP_SLEEP_CLOCK_MAX || option >= ESP_SLEEP_CLOCK_OPTION_MAX) {
        return ESP_ERR_INVALID_ARG;
    }

    int __attribute__((unused)) refs;
    esp_sleep_enter_critical_safe();
    refs = (option == ESP_SLEEP_CLOCK_OPTION_UNGATE) ? s_sleep_clock_icg_refs[clock]++ \
         : (option == ESP_SLEEP_CLOCK_OPTION_GATE)   ? --s_sleep_clock_icg_refs[clock] \
         : s_sleep_clock_icg_refs[clock];
    esp_sleep_exit_critical_safe();
    assert(refs >= 0);

    return ESP_OK;
}
