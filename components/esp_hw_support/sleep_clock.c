/*
 * SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stddef.h>
#include <string.h>
#include <stdarg.h>

#include "sdkconfig.h"
#include "soc/soc_caps.h"

#include "esp_err.h"
#include "esp_log.h"
#include "esp_attr.h"
#include "esp_check.h"
#include "esp_regdma.h"
#include "esp_private/startup_internal.h"
#include "esp_private/sleep_retention.h"
#include "esp_private/sleep_clock.h"

#include "soc/pcr_reg.h"
#include "modem/modem_syscon_reg.h"

#if SOC_PM_RETENTION_SW_TRIGGER_REGDMA
#include "modem/modem_lpcon_reg.h"
#endif

static __attribute__((unused)) const char *TAG = "sleep_clock";

static esp_err_t sleep_clock_system_retention_init(void *arg)
{
#if CONFIG_IDF_TARGET_ESP32C6
    #define N_REGS_PCR()    (((PCR_SRAM_POWER_CONF_REG - DR_REG_PCR_BASE) / 4) + 1)
#elif CONFIG_IDF_TARGET_ESP32H2
    #define N_REGS_PCR()    (((PCR_PWDET_SAR_CLK_CONF_REG - DR_REG_PCR_BASE) / 4) + 1)
#endif
    const static sleep_retention_entries_config_t pcr_regs_retention[] = {
        [0] = { .config = REGDMA_LINK_CONTINUOUS_INIT(REGDMA_PCR_LINK(0),   DR_REG_PCR_BASE,            DR_REG_PCR_BASE,            N_REGS_PCR(),           0, 0), .owner = ENTRY(0) | ENTRY(2) },
        [1] = { .config = REGDMA_LINK_CONTINUOUS_INIT(REGDMA_PCR_LINK(1),   PCR_RESET_EVENT_BYPASS_REG, PCR_RESET_EVENT_BYPASS_REG, 1,                      0, 0), .owner = ENTRY(0) | ENTRY(2) },
#if CONFIG_IDF_TARGET_ESP32H2
        [2] = { .config = REGDMA_LINK_WRITE_INIT     (REGDMA_PCR_LINK(2),   PCR_BUS_CLK_UPDATE_REG,     PCR_BUS_CLOCK_UPDATE,       PCR_BUS_CLOCK_UPDATE_M, 1, 0), .owner = ENTRY(0) | ENTRY(2) },
        [3] = { .config = REGDMA_LINK_WAIT_INIT      (REGDMA_PCR_LINK(3),   PCR_BUS_CLK_UPDATE_REG,     0x0,                        PCR_BUS_CLOCK_UPDATE_M, 1, 0), .owner = ENTRY(0) | ENTRY(2) },
#endif
    };

    esp_err_t err = sleep_retention_entries_create(pcr_regs_retention, ARRAY_SIZE(pcr_regs_retention), REGDMA_LINK_PRI_SYS_CLK, SLEEP_RETENTION_MODULE_CLOCK_SYSTEM);
    ESP_RETURN_ON_ERROR(err, TAG, "failed to allocate memory for system (PCR) retention");
    ESP_LOGI(TAG, "System Power, Clock and Reset sleep retention initialization");
    return ESP_OK;
}

#if CONFIG_MAC_BB_PD || CONFIG_BT_LE_SLEEP_ENABLE || CONFIG_IEEE802154_SLEEP_ENABLE
static esp_err_t sleep_clock_modem_retention_init(void *arg)
{
    #define N_REGS_SYSCON() (((MODEM_SYSCON_MEM_CONF_REG - MODEM_SYSCON_TEST_CONF_REG) / 4) + 1)
#if SOC_PM_RETENTION_SW_TRIGGER_REGDMA
    #define N_REGS_LPCON() (((MODEM_LPCON_MEM_CONF_REG - MODEM_LPCON_TEST_CONF_REG) / 4) + 1)
#endif

#if SOC_WIFI_SUPPORTED
    #define MODEM_WIFI_RETENTION_CLOCK      (MODEM_SYSCON_CLK_WIFI_APB_FO | MODEM_SYSCON_CLK_FE_APB_FO)
    #define MODEM_WIFI_RETENTION_CLOCK_MASK (MODEM_SYSCON_CLK_WIFI_APB_FO_M | MODEM_SYSCON_CLK_FE_APB_FO_M)
#endif

    const static sleep_retention_entries_config_t modem_regs_retention[] = {
        { .config = REGDMA_LINK_CONTINUOUS_INIT(REGDMA_MODEMSYSCON_LINK(0), MODEM_SYSCON_TEST_CONF_REG, MODEM_SYSCON_TEST_CONF_REG, N_REGS_SYSCON(), 0, 0), .owner = ENTRY(0) | ENTRY(1) }, /* MODEM SYSCON */
#if SOC_WIFI_SUPPORTED
        { .config = REGDMA_LINK_WRITE_INIT     (REGDMA_MODEMSYSCON_LINK(1), MODEM_SYSCON_CLK_CONF1_FORCE_ON_REG, MODEM_WIFI_RETENTION_CLOCK, MODEM_WIFI_RETENTION_CLOCK_MASK, 0, 0), .owner = ENTRY(0) }, /* WiFi (MAC, BB and FE) retention clock enable */
#endif
#if SOC_PM_RETENTION_SW_TRIGGER_REGDMA
        { .config = REGDMA_LINK_CONTINUOUS_INIT(REGDMA_MODEMLPCON_LINK(0),  MODEM_LPCON_TEST_CONF_REG, MODEM_LPCON_TEST_CONF_REG, N_REGS_LPCON(), 0, 0), .owner = ENTRY(0) | ENTRY(1) } /* MODEM LPCON */
#endif
    };
#if SOC_WIFI_SUPPORTED
    const static sleep_retention_entries_config_t modem_retention_clock[] = {
        [0] = { .config = REGDMA_LINK_WRITE_INIT     (REGDMA_MODEMSYSCON_LINK(0xf0), MODEM_SYSCON_CLK_CONF1_FORCE_ON_REG, 0x0, MODEM_WIFI_RETENTION_CLOCK_MASK, 0, 0), .owner = ENTRY(0) }  /* WiFi (MAC, BB and FE) retention clock disable */
    };
#endif
    esp_err_t err = sleep_retention_entries_create(modem_regs_retention, ARRAY_SIZE(modem_regs_retention), REGDMA_LINK_PRI_MODEM_CLK, SLEEP_RETENTION_MODULE_CLOCK_MODEM);
    ESP_RETURN_ON_ERROR(err, TAG, "failed to allocate memory for modem (SYSCON) retention, 2 level priority");
#if SOC_WIFI_SUPPORTED
    err = sleep_retention_entries_create(modem_retention_clock, ARRAY_SIZE(modem_retention_clock), REGDMA_LINK_PRI_7, SLEEP_RETENTION_MODULE_CLOCK_MODEM);
    ESP_RETURN_ON_ERROR(err, TAG, "failed to allocate memory for modem (SYSCON) retention, lowest level priority");
#endif
    ESP_LOGI(TAG, "Modem Power, Clock and Reset sleep retention initialization");
    return ESP_OK;
}
#endif

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

ESP_SYSTEM_INIT_FN(sleep_clock_startup_init, BIT(0), 106)
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
