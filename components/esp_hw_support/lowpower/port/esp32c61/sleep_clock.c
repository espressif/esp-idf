/*
 * SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_private/sleep_clock.h"
#include "soc/pcr_reg.h"
#include "soc/pmu_reg.h"
#include "modem/modem_syscon_reg.h"

static const char *TAG = "sleep_clock";

esp_err_t sleep_clock_system_retention_init(void *arg)
{
    const static sleep_retention_entries_config_t pcr_regs_retention[] = {
        [0] = { .config = REGDMA_LINK_WAIT_INIT    (REGDMA_PCR_LINK(0), PMU_CLK_STATE0_REG,     PMU_STABLE_XPD_BBPLL_STATE, PMU_STABLE_XPD_BBPLL_STATE_M,   1, 0),  .owner = ENTRY(0)},             /* Wait PMU_WAIT_XTL_STABLE done */
        [1] = { .config = REGDMA_LINK_WRITE_INIT   (REGDMA_PCR_LINK(1), PCR_AHB_FREQ_CONF_REG,  0,                          PCR_AHB_DIV_NUM,                1, 0),  .owner = ENTRY(0) | ENTRY(1) }, /* Set AHB bus frequency to XTAL frequency */
        [2] = { .config = REGDMA_LINK_WRITE_INIT   (REGDMA_PCR_LINK(2), PCR_BUS_CLK_UPDATE_REG, 1,                          PCR_BUS_CLOCK_UPDATE,           1, 0),  .owner = ENTRY(0) | ENTRY(1) },
#if CONFIG_PM_POWER_DOWN_PERIPHERAL_IN_LIGHT_SLEEP
        [3] = { .config = REGDMA_LINK_ADDR_MAP_INIT(REGDMA_PCR_LINK(3), DR_REG_PCR_BASE,        DR_REG_PCR_BASE,            63,     0, 0, 0xfd73ffff, 0xfdffffff, 0xe001, 0x0), .owner = ENTRY(0) | ENTRY(1) }
#endif
    };

    esp_err_t err = sleep_retention_entries_create(pcr_regs_retention, ARRAY_SIZE(pcr_regs_retention), REGDMA_LINK_PRI_SYS_CLK, SLEEP_RETENTION_MODULE_CLOCK_SYSTEM);
    ESP_RETURN_ON_ERROR(err, TAG, "failed to allocate memory for system (PCR) retention");

    const static sleep_retention_entries_config_t modem_ahb_config[] = {
        [0] = { .config = REGDMA_LINK_WRITE_INIT (REGDMA_PCR_LINK(4), PCR_AHB_FREQ_CONF_REG,  3, PCR_AHB_DIV_NUM,      1, 0), .owner = ENTRY(1) }, /* Set AHB bus frequency to 40 MHz under PMU MODEM state */
        [1] = { .config = REGDMA_LINK_WRITE_INIT (REGDMA_PCR_LINK(5), PCR_BUS_CLK_UPDATE_REG, 1, PCR_BUS_CLOCK_UPDATE, 1, 0), .owner = ENTRY(1) },
    };
    err = sleep_retention_entries_create(modem_ahb_config, ARRAY_SIZE(modem_ahb_config), REGDMA_LINK_PRI_4, SLEEP_RETENTION_MODULE_CLOCK_SYSTEM);
    ESP_RETURN_ON_ERROR(err, TAG, "failed to allocate memory for system (PCR) retention, 4 level priority");

    ESP_LOGI(TAG, "System Power, Clock and Reset sleep retention initialization");
    return ESP_OK;
}

#if CONFIG_MAC_BB_PD || CONFIG_BT_LE_SLEEP_ENABLE || CONFIG_IEEE802154_SLEEP_ENABLE
esp_err_t sleep_clock_modem_retention_init(void *arg)
{
    #define N_REGS_SYSCON() (((MODEM_SYSCON_MEM_RF2_CONF_REG - MODEM_SYSCON_TEST_CONF_REG) / 4) + 1)

    const static sleep_retention_entries_config_t modem_regs_retention[] = {
        [0] = { .config = REGDMA_LINK_CONTINUOUS_INIT(REGDMA_MODEMSYSCON_LINK(0), MODEM_SYSCON_TEST_CONF_REG, MODEM_SYSCON_TEST_CONF_REG, N_REGS_SYSCON(), 0, 0), .owner = ENTRY(0) | ENTRY(1) }, /* MODEM SYSCON */
    };

    esp_err_t err = sleep_retention_entries_create(modem_regs_retention, ARRAY_SIZE(modem_regs_retention), REGDMA_LINK_PRI_MODEM_CLK, SLEEP_RETENTION_MODULE_CLOCK_MODEM);
    ESP_RETURN_ON_ERROR(err, TAG, "failed to allocate memory for modem (SYSCON) retention, 1 level priority");
    ESP_LOGI(TAG, "Modem Power, Clock and Reset sleep retention initialization");
    return ESP_OK;

    #undef N_REGS_SYSCON
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
        .depends   = BIT(SLEEP_RETENTION_MODULE_CLOCK_SYSTEM),
        .attribute = SLEEP_RETENTION_MODULE_ATTR_PASSIVE
    };
    sleep_retention_module_init(SLEEP_RETENTION_MODULE_CLOCK_MODEM, &init_param);
#endif
    return ESP_OK;
}
