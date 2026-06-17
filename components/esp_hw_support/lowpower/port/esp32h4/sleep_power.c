/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_private/sleep_power.h"
#include "soc/pmu_reg.h"
#include "modem/modem_lpcon_reg.h"

ESP_LOG_ATTR_TAG(TAG, "sleep_power");

#if CONFIG_BT_CTRL_SLEEP_ENABLE && SOC_PM_MODEM_LOCK_CLK_WORKAROUND

esp_err_t sleep_power_system_retention_init(void *arg)
{
    const static sleep_retention_entries_config_t power_regs_retention[] = {
        /* During the modem-to-active transition, the MODEM lock keeps the xtalx2, bbpll xpd status unchanged.
        * Therefore, if xtalx2, bbpll is disabled in modem state, we need to set xtalx2, bbpll tie-high to enable it in active state. */
        [0] = { .config = REGDMA_LINK_WRITE_INIT     (REGDMA_POWER_LINK(0),   PMU_IMM_HP_CK_POWER_REG,   PMU_TIE_HIGH_XTALX2    | PMU_TIE_HIGH_GLOBAL_XTALX2_ICG,  PMU_TIE_HIGH_XTALX2_M    | PMU_TIE_HIGH_GLOBAL_XTALX2_ICG_M,  1,  0),  .owner = ENTRY(2) },
        [1] = { .config = REGDMA_LINK_WRITE_INIT     (REGDMA_POWER_LINK(1),   PMU_IMM_HP_CK_POWER_REG,   PMU_TIE_HIGH_XPD_BBPLL | PMU_TIE_HIGH_XPD_BBPLL_I2C,      PMU_TIE_HIGH_XPD_BBPLL_M | PMU_TIE_HIGH_XPD_BBPLL_I2C_M,      1,  0),  .owner = ENTRY(2) },
    };
    esp_err_t err = sleep_retention_entries_create(power_regs_retention, ARRAY_SIZE(power_regs_retention), REGDMA_LINK_PRI_POWER, SLEEP_RETENTION_MODULE_POWER);
    ESP_RETURN_ON_ERROR(err, TAG, "failed to allocate memory for clock power retention");
    ESP_LOGI(TAG, "Clock power sleep retention initialization");
    return ESP_OK;
}

ESP_SYSTEM_INIT_FN(sleep_power_startup_init, SECONDARY, BIT(0), 108)
{
    sleep_retention_module_init_param_t init_param = {
        .cbs       = { .create = { .handle = sleep_power_system_retention_init, .arg = NULL } },
        .attribute = SLEEP_RETENTION_MODULE_ATTR_PASSIVE | SLEEP_RETENTION_MODULE_ATTR_ATTACH
    };

    esp_err_t err = sleep_retention_module_init(SLEEP_RETENTION_MODULE_POWER, &init_param);
    if (err != ESP_OK) {
        ESP_LOGW(TAG, "failed to init power retention module, err=%d", err);
    }

    return ESP_OK;
}

#endif
