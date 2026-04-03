/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_private/sleep_clock.h"
#include "soc/hp_sys_clkrst_reg.h"
#include "soc/pmu_reg.h"
#include "soc/lp_peri_clkrst_reg.h"
#include "pmu_bit_defs.h"
#include "modem/modem_syscon_reg.h"
#include "modem/modem_lpcon_reg.h"

ESP_LOG_ATTR_TAG(TAG, "sleep_clock");

esp_err_t sleep_clock_system_retention_init(void *arg)
{
    #define N_REGS_PCR()    (((HP_SYS_CLKRST_CLK_EN0_REG - DR_REG_HP_SYS_CLKRST_BASE) / 4) + 1)

    const static sleep_retention_entries_config_t pcr_regs_retention[] = {
        /* Enable i2c master clock */
        [0] = { .config = REGDMA_LINK_WRITE_INIT (REGDMA_PCR_LINK(0),     LP_PERICLKRST_I2CMST_CTRL_REG,            LP_PERICLKRST_LP_I2CMST_CLK_EN,                LP_PERICLKRST_LP_I2CMST_CLK_EN_M,                1, 0), .owner = ENTRY(0) },
        /* Start SYSPLL self-calibration */
        [1] = { .config = REGDMA_LINK_WRITE_INIT (REGDMA_PCR_LINK(1),     HP_SYS_CLKRST_ANA_PLL_CTRL0_REG,          0,                                             HP_SYS_CLKRST_REG_CPU_PLL_CAL_STOP_M,            1, 0), .owner = ENTRY(0) },
        /* Wait calibration done */
        [2] = { .config = REGDMA_LINK_WAIT_INIT  (REGDMA_PCR_LINK(2),     HP_SYS_CLKRST_ANA_PLL_CTRL0_REG,          HP_SYS_CLKRST_REG_CPU_PLL_CAL_END,             HP_SYS_CLKRST_REG_CPU_PLL_CAL_END_M,             1, 0), .owner = ENTRY(0) },
        /* Stop SYSPLL self-calibration */
        [3] = { .config = REGDMA_LINK_WRITE_INIT (REGDMA_PCR_LINK(3),     HP_SYS_CLKRST_ANA_PLL_CTRL0_REG,          HP_SYS_CLKRST_REG_CPU_PLL_CAL_STOP,            HP_SYS_CLKRST_REG_CPU_PLL_CAL_STOP_M,            1, 0), .owner = ENTRY(0) },
        /* Clock configuration retention */
        [4] = { .config = REGDMA_LINK_CONTINUOUS_INIT(REGDMA_PCR_LINK(4), DR_REG_HP_SYS_CLKRST_BASE,                DR_REG_HP_SYS_CLKRST_BASE,                     N_REGS_PCR(),                                    0, 0), .owner = ENTRY(0) },  /* pcr */
        [5] = { .config = REGDMA_LINK_WRITE_INIT     (REGDMA_PCR_LINK(5), HP_SYS_CLKRST_ROOT_CLK_CTRL0_REG,         HP_SYS_CLKRST_REG_SOC_CLK_UPDATE,              HP_SYS_CLKRST_REG_SOC_CLK_UPDATE_M,              1, 0), .owner = ENTRY(0) },
        [6] = { .config = REGDMA_LINK_WAIT_INIT      (REGDMA_PCR_LINK(6), HP_SYS_CLKRST_ROOT_CLK_CTRL0_REG,         0x0,                                           HP_SYS_CLKRST_REG_SOC_CLK_UPDATE_M,              1, 0), .owner = ENTRY(0) },
        [7] = { .config = REGDMA_LINK_WRITE_INIT     (REGDMA_PCR_LINK(7), HP_SYS_CLKRST_SDIO_HOST_FUNC_CTRL0_REG,   HP_SYS_CLKRST_REG_SDIO_LS_CLK_EDGE_CFG_UPDATE, HP_SYS_CLKRST_REG_SDIO_LS_CLK_EDGE_CFG_UPDATE_M, 1, 0), .owner = ENTRY(0) },
        [8] = { .config = REGDMA_LINK_WAIT_INIT      (REGDMA_PCR_LINK(8), HP_SYS_CLKRST_SDIO_HOST_FUNC_CTRL0_REG,   0x0,                                           HP_SYS_CLKRST_REG_SDIO_LS_CLK_EDGE_CFG_UPDATE_M, 1, 0), .owner = ENTRY(0) },
    };

    esp_err_t err = sleep_retention_entries_create(pcr_regs_retention, ARRAY_SIZE(pcr_regs_retention), REGDMA_LINK_PRI_SYS_CLK, SLEEP_RETENTION_MODULE_CLOCK_SYSTEM);
    ESP_RETURN_ON_ERROR(err, TAG, "failed to allocate memory for system (PCR) retention");
    ESP_LOGI(TAG, "System Power, Clock and Reset sleep retention initialization");
    return ESP_OK;

    #undef N_REGS_PCR
}

#if CONFIG_MAC_BB_PD || CONFIG_BTDM_CTRL_SLEEP_ENABLE || CONFIG_IEEE802154_SLEEP_ENABLE
#include "rom/ets_sys.h"
esp_err_t sleep_clock_modem_retention_init(void *arg)
{
    #define N_REGS_SYSCON() (((MODEM_SYSCON_MEM_RF2_CONF_REG - MODEM_SYSCON_TEST_CONF_REG) / 4) + 1)
    #define N_REGS_LPCON() (((MODEM_LPCON_MODEM_INTR_STATUS_REG - MODEM_LPCON_TEST_CONF_REG) / 4) + 1)
    const static sleep_retention_entries_config_t modem_regs_retention[] = {
        [0] = { .config = REGDMA_LINK_WRITE_INIT(REGDMA_MODEMSYSCON_LINK(0), 0x20109c30, 0xf, 0xffffffff, 1, 0), .owner = ENTRY(0) | ENTRY(1) },
        [1] = { .config = REGDMA_LINK_CONTINUOUS_INIT(REGDMA_MODEMSYSCON_LINK(1), MODEM_SYSCON_TEST_CONF_REG, MODEM_SYSCON_TEST_CONF_REG, 12, 0, 0), .owner = ENTRY(0) | ENTRY(1) }, /* MODEM SYSCON */
        [2] = { .config = REGDMA_LINK_CONTINUOUS_INIT(REGDMA_MODEMLPCON_LINK(0), MODEM_LPCON_TEST_CONF_REG, MODEM_LPCON_TEST_CONF_REG, N_REGS_LPCON(), 0, 0), .owner = ENTRY(0) | ENTRY(1) }, /* MODEM SYSCON */
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
    const sleep_retention_module_bitmap_t inited_modules = sleep_retention_get_inited_modules();
    const sleep_retention_module_bitmap_t created_modules = sleep_retention_get_created_modules();
    const sleep_retention_module_bitmap_t sys_clk_dep_modules = (sleep_retention_module_bitmap_t){ .bitmap[SLEEP_RETENTION_MODULE_SYS_PERIPH >> 5] = BIT(SLEEP_RETENTION_MODULE_SYS_PERIPH % 32) };
/* The clock and reset of MODEM (WiFi, BLE and 15.4) modules are managed
     * through MODEM_SYSCON, when one or more MODEMs are initialized, it is
     * necessary to check the state of CLOCK_MODEM to determine MODEM domain on
     * or off. The clock and reset of digital peripherals are managed through
     * PCR, with TOP domain similar to MODEM domain. */
    __attribute__((unused)) sleep_retention_module_bitmap_t modem_clk_dep_modules = (sleep_retention_module_bitmap_t){ .bitmap = { 0 } };
#if SOC_WIFI_SUPPORTED
    modem_clk_dep_modules.bitmap[SLEEP_RETENTION_MODULE_WIFI_MAC >> 5] |= BIT(SLEEP_RETENTION_MODULE_WIFI_MAC % 32);
    modem_clk_dep_modules.bitmap[SLEEP_RETENTION_MODULE_WIFI_BB >> 5] |= BIT(SLEEP_RETENTION_MODULE_WIFI_BB % 32);
#endif
#if SOC_BT_SUPPORTED
    modem_clk_dep_modules.bitmap[SLEEP_RETENTION_MODULE_BLE_MAC >> 5] |= BIT(SLEEP_RETENTION_MODULE_BLE_MAC % 32);
    modem_clk_dep_modules.bitmap[SLEEP_RETENTION_MODULE_BT_BB >> 5] |= BIT(SLEEP_RETENTION_MODULE_BT_BB % 32);
#endif
#if SOC_IEEE802154_SUPPORTED
    modem_clk_dep_modules.bitmap[SLEEP_RETENTION_MODULE_802154_MAC >> 5] |= BIT(SLEEP_RETENTION_MODULE_802154_MAC % 32);
    modem_clk_dep_modules.bitmap[SLEEP_RETENTION_MODULE_BT_BB >> 5] |= BIT(SLEEP_RETENTION_MODULE_BT_BB % 32);
#endif

    const sleep_retention_module_bitmap_t null_module = (sleep_retention_module_bitmap_t){ .bitmap = { 0 } };

    sleep_retention_module_bitmap_t mask = (sleep_retention_module_bitmap_t){ .bitmap = { 0 } };
    const sleep_retention_module_bitmap_t system_modules = sleep_retention_module_bitmap_and(inited_modules, sys_clk_dep_modules);
    if (!sleep_retention_module_bitmap_eq(system_modules, null_module)) {
        mask.bitmap[SLEEP_RETENTION_MODULE_CLOCK_SYSTEM >> 5] |= BIT(SLEEP_RETENTION_MODULE_CLOCK_SYSTEM % 32);
    }

    #if SOC_WIFI_SUPPORTED || SOC_BT_SUPPORTED || SOC_IEEE802154_SUPPORTED
    const sleep_retention_module_bitmap_t modem_modules = sleep_retention_module_bitmap_and(inited_modules, modem_clk_dep_modules);
    if (!sleep_retention_module_bitmap_eq(modem_modules, null_module)) {
        mask.bitmap[SLEEP_RETENTION_MODULE_CLOCK_MODEM >> 5] |= BIT(SLEEP_RETENTION_MODULE_CLOCK_MODEM % 32);
    }
#endif

    const sleep_retention_module_bitmap_t clock_domain_inited_modules = sleep_retention_module_bitmap_and(inited_modules, mask);
    const sleep_retention_module_bitmap_t clock_domain_created_modules = sleep_retention_module_bitmap_and(created_modules, mask);
    return sleep_retention_module_bitmap_eq(clock_domain_inited_modules, clock_domain_created_modules);
}

ESP_SYSTEM_INIT_FN(sleep_clock_startup_init, SECONDARY, BIT(0), 106)
{
    sleep_retention_module_init_param_t init_param = {
        .cbs       = { .create = { .handle = sleep_clock_system_retention_init, .arg = NULL } },
        .attribute = SLEEP_RETENTION_MODULE_ATTR_PASSIVE
    };
    sleep_retention_module_init(SLEEP_RETENTION_MODULE_CLOCK_SYSTEM, &init_param);

#if CONFIG_MAC_BB_PD || CONFIG_BTDM_CTRL_SLEEP_ENABLE || CONFIG_IEEE802154_SLEEP_ENABLE
    init_param = (sleep_retention_module_init_param_t) {
        .cbs       = { .create = { .handle = sleep_clock_modem_retention_init, .arg = NULL } },
        .attribute = SLEEP_RETENTION_MODULE_ATTR_PASSIVE
    };
    sleep_retention_module_init(SLEEP_RETENTION_MODULE_CLOCK_MODEM, &init_param);
#endif
    return ESP_OK;
}
