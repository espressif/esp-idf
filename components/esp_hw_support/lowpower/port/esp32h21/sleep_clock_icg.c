/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <assert.h>
#include <stdint.h>

#include "esp_attr.h"
#include "esp_err.h"
#include "esp_private/esp_sleep_internal.h"
#include "esp_private/esp_pmu.h"
#include "esp_private/sleep_clock_icg.h"
#include "esp_private/sleep_retention.h"
#include "hal/pmu_ll.h"
#include "pmu_param.h"
#include "sdkconfig.h"
#include "soc/soc_caps.h"

#if CONFIG_PM_SLP_IRAM_OPT
#define SLEEP_CLOCK_ICG_FN_ATTR FORCE_IRAM_ATTR
#else
#define SLEEP_CLOCK_ICG_FN_ATTR
#endif

static int16_t s_sleep_clock_icg_refs[ESP_SLEEP_CLOCK_MAX];

#if CONFIG_ESP_CONSOLE_UART
#if CONFIG_ESP_CONSOLE_UART_NUM == 0
#define RETENTION_CONSOLE_UART     PMU_CLK_ICG( UART0 )
#elif CONFIG_ESP_CONSOLE_UART_NUM == 1
#define RETENTION_CONSOLE_UART     PMU_CLK_ICG( UART1 )
#else
#define RETENTION_CONSOLE_UART     0
#endif
#else
#define RETENTION_CONSOLE_UART     0
#endif

#define RETENTION_SYS_CLK    PMU_CLK_ICGS( HPBUS )

#define RETENTION_COMMON     PMU_CLK_ICGS( REGDMA, HPCORE ) | RETENTION_SYS_CLK

static const uint32_t s_retention_module_retention_clocks[SLEEP_RETENTION_MODULE_MAX + 1] = {
    [SLEEP_RETENTION_MODULE_NULL]           = 0,
    [SLEEP_RETENTION_MODULE_CLOCK_SYSTEM]   = RETENTION_COMMON,
    [SLEEP_RETENTION_MODULE_SYS_PERIPH]     = RETENTION_COMMON | PMU_CLK_ICGS( IOMUX, SYSTIMER, SEC, MSPI ) | RETENTION_CONSOLE_UART,
    [SLEEP_RETENTION_MODULE_TG0_WDT]        = RETENTION_COMMON | PMU_CLK_ICG ( TG0 ),
    [SLEEP_RETENTION_MODULE_TG1_WDT]        = RETENTION_COMMON | PMU_CLK_ICG ( TG1 ),
    [SLEEP_RETENTION_MODULE_TG0_TIMER0]     = RETENTION_COMMON | PMU_CLK_ICG ( TG0 ),
    [SLEEP_RETENTION_MODULE_TG1_TIMER0]     = RETENTION_COMMON | PMU_CLK_ICG ( TG1 ),
    [SLEEP_RETENTION_MODULE_GDMA_CH0]       = RETENTION_COMMON | PMU_CLK_ICG ( GDMA ),
    [SLEEP_RETENTION_MODULE_GDMA_CH1]       = RETENTION_COMMON | PMU_CLK_ICG ( GDMA ),
    [SLEEP_RETENTION_MODULE_GDMA_CH2]       = RETENTION_COMMON | PMU_CLK_ICG ( GDMA ),
    [SLEEP_RETENTION_MODULE_ADC]            = RETENTION_COMMON | PMU_CLK_ICG ( SARADC ),
    [SLEEP_RETENTION_MODULE_I2C0]           = RETENTION_COMMON | PMU_CLK_ICG ( I2C0 ),
    [SLEEP_RETENTION_MODULE_I2C1]           = RETENTION_COMMON | PMU_CLK_ICG ( I2C0 ),
    [SLEEP_RETENTION_MODULE_RMT0]           = RETENTION_COMMON | PMU_CLK_ICG ( RMT ),
    [SLEEP_RETENTION_MODULE_UART0]          = RETENTION_COMMON | PMU_CLK_ICG ( UART0 ),
    [SLEEP_RETENTION_MODULE_UART1]          = RETENTION_COMMON | PMU_CLK_ICG ( UART1 ),
    [SLEEP_RETENTION_MODULE_I2S0]           = RETENTION_COMMON | PMU_CLK_ICGS( I2S_RX, I2S_TX ),
    [SLEEP_RETENTION_MODULE_ETM0]           = RETENTION_COMMON | PMU_CLK_ICG ( SOC_ETM ),
    [SLEEP_RETENTION_MODULE_TEMP_SENSOR]    = RETENTION_COMMON | PMU_CLK_ICG ( TSENS ),
    [SLEEP_RETENTION_MODULE_TWAI0]          = RETENTION_COMMON | PMU_CLK_ICG ( TWAI0 ),
    [SLEEP_RETENTION_MODULE_PARLIO0]        = RETENTION_COMMON | PMU_CLK_ICGS( PARL_TX, PARL_RX ),
    [SLEEP_RETENTION_MODULE_GPSPI2]         = RETENTION_COMMON | PMU_CLK_ICG ( SPI2 ),
    [SLEEP_RETENTION_MODULE_LEDC]           = RETENTION_COMMON | PMU_CLK_ICG ( LEDC0 ),
    [SLEEP_RETENTION_MODULE_MCPWM0]         = RETENTION_COMMON | PMU_CLK_ICG ( PWM ),
    [SLEEP_RETENTION_MODULE_SDM0]           = RETENTION_COMMON,
    [SLEEP_RETENTION_MODULE_CLOCK_MODEM]    = RETENTION_COMMON,
    [SLEEP_RETENTION_MODULE_BLE_MAC]        = RETENTION_COMMON,
    [SLEEP_RETENTION_MODULE_BT_BB]          = RETENTION_COMMON,
    [SLEEP_RETENTION_MODULE_802154_MAC]     = RETENTION_COMMON,
};

void sleep_clock_icg_retention_clock_config(sleep_retention_module_bitmap_t *module_bitmap)
{
    static DRAM_ATTR uint32_t s_retention_icg_flags = 0;
    pmu_sleep_data_t *data = (pmu_sleep_data_t *)PMU_instance()->priv;
    if (data->func[PMU_SLEEP_PRIV_HW_RETENTION_ICG_CLK] == NULL) {
        data->func[PMU_SLEEP_PRIV_HW_RETENTION_ICG_CLK] = &s_retention_icg_flags;
    }

    /* Check if the last word holds bit above MODULE_MAX. */
    assert(!module_bitmap->bitmap[SLEEP_RETENTION_MODULE_BITMAP_SZ - 1] ||
           ((31 - __builtin_clz(module_bitmap->bitmap[SLEEP_RETENTION_MODULE_BITMAP_SZ - 1])) +
            ((SLEEP_RETENTION_MODULE_BITMAP_SZ - 1) << 5)) <= SLEEP_RETENTION_MODULE_MAX);

    uint32_t clocks_mask = 0;
    sleep_retention_module_t module = 0;
    for (int i = 0; i < SLEEP_RETENTION_MODULE_BITMAP_SZ; i++) {
        for (uint32_t remain = module_bitmap->bitmap[i]; remain; remain &= (remain - 1)) {
            module = (__builtin_ctz(remain) + (i << 5));
            clocks_mask |= s_retention_module_retention_clocks[module];
        }
    }

#if !CONFIG_SECURE_ENABLE_TEE
    clocks_mask &= ~PMU_SLEEP_CLK_ICG_BIT(PMU_ICG_FUNC_ENA_SEC);
#endif

    esp_sleep_enter_critical_safe();
    s_retention_icg_flags = clocks_mask;
    esp_sleep_exit_critical_safe();
}

FORCE_IRAM_ATTR void pmu_sleep_retention_clock_icg_config(void *data, pmu_context_t *ctx)
{
    pmu_sleep_data_t *data_ctx = (pmu_sleep_data_t *)data;
    if (!data_ctx || !ctx || !data_ctx->func[PMU_SLEEP_PRIV_HW_RETENTION_ICG_CLK]) {
        return;
    }

    uint32_t *flags = (uint32_t *)data_ctx->func[PMU_SLEEP_PRIV_HW_RETENTION_ICG_CLK];
    for (pmu_hp_mode_t mode = PMU_MODE_HP_ACTIVE; mode < PMU_MODE_HP_MAX; mode++) {
        pmu_ll_hp_set_backup_icg_func(ctx->hal->dev, mode, *flags);
    }
}

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

    (void)clk_icg1_flags;
}

esp_err_t esp_sleep_clock_config(esp_sleep_clock_t clock, esp_sleep_clock_option_t option)
{
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
