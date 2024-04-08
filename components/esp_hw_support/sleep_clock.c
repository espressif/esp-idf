/*
 * SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_private/sleep_clock.h"

bool clock_domain_pd_allowed(void)
{
    const uint32_t inited_modules = sleep_retention_get_inited_modules();
    const uint32_t created_modules = sleep_retention_get_created_modules();
    const uint32_t sys_clk_dep_modules = (const uint32_t) (BIT(SLEEP_RETENTION_MODULE_SYS_PERIPH));

    /* The clock and reset of MODEM (WiFi, BLE and 15.4) modules are managed
     * through MODEM_SYSCON, when one or more MODEMs are initialized, it is
     * necessary to check the state of CLOCK_MODEM to determine MODEM domain on
     * or off. The clock and reset of digital peripherals are managed through
     * PCR, with TOP domain similar to MODEM domain. */
    uint32_t modem_clk_dep_modules = 0;
#if SOC_WIFI_SUPPORTED
    modem_clk_dep_modules |= BIT(SLEEP_RETENTION_MODULE_WIFI_MAC) | BIT(SLEEP_RETENTION_MODULE_WIFI_BB);
#endif
#if SOC_BT_SUPPORTED
    modem_clk_dep_modules |= BIT(SLEEP_RETENTION_MODULE_BLE_MAC) | BIT(SLEEP_RETENTION_MODULE_BT_BB);
#endif
#if SOC_IEEE802154_SUPPORTED
    modem_clk_dep_modules |= BIT(SLEEP_RETENTION_MODULE_802154_MAC) | BIT(SLEEP_RETENTION_MODULE_BT_BB);
#endif

    uint32_t mask = 0;
    if (inited_modules & sys_clk_dep_modules) {
        mask |= BIT(SLEEP_RETENTION_MODULE_CLOCK_SYSTEM);
    }
    if (inited_modules & modem_clk_dep_modules) {
#if SOC_WIFI_SUPPORTED || SOC_BT_SUPPORTED || SOC_IEEE802154_SUPPORTED
        mask |= BIT(SLEEP_RETENTION_MODULE_CLOCK_MODEM);
#endif
    }
    return ((inited_modules & mask) == (created_modules & mask));
}

ESP_SYSTEM_INIT_FN(sleep_clock_startup_init, SECONDARY, BIT(0), 106)
{
    sleep_retention_module_init_param_t init_param = {
        .cbs       = { .create = { .handle = sleep_clock_system_retention_init, .arg = NULL } },
        .attribute = SLEEP_RETENTION_MODULE_ATTR_PASSIVE
    };
    sleep_retention_module_init(SLEEP_RETENTION_MODULE_CLOCK_SYSTEM, &init_param);

#if CONFIG_MAC_BB_PD || CONFIG_BT_LE_SLEEP_ENABLE || CONFIG_IEEE802154_SLEEP_ENABLE
    init_param = (sleep_retention_module_init_param_t) {
        .cbs       = { .create = { .handle = sleep_clock_modem_retention_init, .arg = NULL } },
        .attribute = SLEEP_RETENTION_MODULE_ATTR_PASSIVE
    };
    sleep_retention_module_init(SLEEP_RETENTION_MODULE_CLOCK_MODEM, &init_param);
#endif
    return ESP_OK;
}
