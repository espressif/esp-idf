/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_private/sleep_clock.h"
#include "soc/hp_sys_clkrst_reg.h"
#include "soc/hp_alive_sys_reg.h"
#include "soc/pmu_reg.h"
#include "soc/lp_peri_clkrst_reg.h"
#include "soc/ds_reg.h"
#include "soc/ecdsa_reg.h"
#include "pmu_bit_defs.h"
#include "modem/modem_syscon_reg.h"
#include "modem/modem_lpcon_reg.h"

ESP_LOG_ATTR_TAG(TAG, "sleep_clock");

static esp_err_t sleep_clock_pll_source_retention_init(void *arg)
{
    /* Modem/system clock retention restore always powers BBPLL and runs calibration here. */
    const static sleep_retention_entries_config_t pll_source_regs_retention[] = {
        /* Force BBPLL on (same effects as clk_ll_bbpll_enable) */
        [0] = { .config = REGDMA_LINK_WRITE_INIT (REGDMA_PLL_LINK(0),   PMU_IMM_HP_CK_POWER_1_REG,          PMU_TIE_HIGH_XPD_BBPLL | PMU_TIE_HIGH_XPD_BBPLL_I2C,    PMU_TIE_HIGH_XPD_BBPLL_M | PMU_TIE_HIGH_XPD_BBPLL_I2C_M,    1, 0), .owner = ENTRY(0) | ENTRY(1)},
        [1] = { .config = REGDMA_LINK_WRITE_INIT (REGDMA_PLL_LINK(1),   PMU_IMM_HP_CK_POWER_1_REG,          PMU_TIE_HIGH_GLOBAL_BBPLL_ICG,                          PMU_TIE_HIGH_GLOBAL_BBPLL_ICG_M,                            1, 0), .owner = ENTRY(0) | ENTRY(1)},
        [2] = { .config = REGDMA_LINK_WRITE_INIT (REGDMA_PLL_LINK(2),   HP_ALIVE_SYS_HP_CLK_CTRL_REG,       HP_ALIVE_SYS_HP_SPLL_480M_CLK_EN,                       HP_ALIVE_SYS_HP_SPLL_480M_CLK_EN_M,                         1, 0), .owner = ENTRY(0) | ENTRY(1)},
        [3] = { .config = REGDMA_LINK_WRITE_INIT (REGDMA_PLL_LINK(3),   PMU_HP_ACTIVE_HP_CK_POWER_REG,      PMU_HP_ACTIVE_XPD_BBPLL_I2C | PMU_HP_ACTIVE_XPD_BBPLL,  PMU_HP_ACTIVE_XPD_BBPLL_I2C_M | PMU_HP_ACTIVE_XPD_BBPLL_M,  1, 0), .owner = ENTRY(0) | ENTRY(1)},
        /* Enable i2c master clock */
        [4] = { .config = REGDMA_LINK_WRITE_INIT (REGDMA_PLL_LINK(4),   LP_PERICLKRST_I2CMST_CTRL_REG,      LP_PERICLKRST_LP_I2CMST_CLK_EN,                         LP_PERICLKRST_LP_I2CMST_CLK_EN_M,                           1, 0), .owner = ENTRY(0) | ENTRY(1)},
        /* Start SYSPLL self-calibration */
        [5] = { .config = REGDMA_LINK_WRITE_INIT (REGDMA_PLL_LINK(5),   HP_SYS_CLKRST_ANA_PLL_CTRL0_REG,    0,                                                      HP_SYS_CLKRST_REG_BBPLL_CAL_STOP_M,                         1, 0), .owner = ENTRY(0) | ENTRY(1)},
        /* Wait calibration done */
        [6] = { .config = REGDMA_LINK_WAIT_INIT  (REGDMA_PLL_LINK(6),   HP_SYS_CLKRST_ANA_PLL_CTRL0_REG,    HP_SYS_CLKRST_REG_BBPLL_CAL_END,                        HP_SYS_CLKRST_REG_BBPLL_CAL_END_M,                          1, 0), .owner = ENTRY(0) | ENTRY(1)},
        /* Stop SYSPLL self-calibration */
        [7] = { .config = REGDMA_LINK_WRITE_INIT (REGDMA_PLL_LINK(7),   HP_SYS_CLKRST_ANA_PLL_CTRL0_REG,    HP_SYS_CLKRST_REG_BBPLL_CAL_STOP,                       HP_SYS_CLKRST_REG_BBPLL_CAL_STOP_M,                         1, 0), .owner = ENTRY(0) | ENTRY(1)},
    };

    esp_err_t err = sleep_retention_entries_create(pll_source_regs_retention, ARRAY_SIZE(pll_source_regs_retention), REGDMA_LINK_PRI_PLL_SOURCE, SLEEP_RETENTION_MODULE_PLL_SOURCE);
    ESP_RETURN_ON_ERROR(err, TAG, "failed to allocate memory for PLL source retention");
    ESP_LOGI(TAG, "PLL source sleep retention initialization");
    return ESP_OK;
}

esp_err_t sleep_clock_system_retention_init(void *arg)
{
    #define N_REGS_PCR()    (((HP_SYS_CLKRST_CLK_EN0_REG - DR_REG_HP_SYS_CLKRST_BASE) / 4) + 1)

    const static sleep_retention_entries_config_t pcr_regs_retention[] = {
        /* Clock configuration retention */
        [0] = { .config = REGDMA_LINK_ADDR_MAP_INIT(REGDMA_PCR_LINK(0), DR_REG_HP_SYS_CLKRST_BASE, DR_REG_HP_SYS_CLKRST_BASE, N_REGS_PCR() - 1, 0, 0,
                                                   0xffbfffff, 0xffffffff, 0xffffffff, 0x7fffffff), .owner = ENTRY(0) | ENTRY(1)},
        [1] = { .config = REGDMA_LINK_WRITE_INIT     (REGDMA_PCR_LINK(1), HP_SYS_CLKRST_ROOT_CLK_CTRL0_REG,         HP_SYS_CLKRST_REG_SOC_CLK_UPDATE,              HP_SYS_CLKRST_REG_SOC_CLK_UPDATE_M,              1, 0), .owner = ENTRY(0) | ENTRY(1)},
        [2] = { .config = REGDMA_LINK_WAIT_INIT      (REGDMA_PCR_LINK(2), HP_SYS_CLKRST_ROOT_CLK_CTRL0_REG,         0x0,                                           HP_SYS_CLKRST_REG_SOC_CLK_UPDATE_M,              1, 0), .owner = ENTRY(0) | ENTRY(1)},
        [3] = { .config = REGDMA_LINK_WRITE_INIT     (REGDMA_PCR_LINK(3), HP_SYS_CLKRST_SDIO_HOST_FUNC_CTRL0_REG,   HP_SYS_CLKRST_REG_SDIO_LS_CLK_EDGE_CFG_UPDATE, HP_SYS_CLKRST_REG_SDIO_LS_CLK_EDGE_CFG_UPDATE_M, 1, 0), .owner = ENTRY(0) | ENTRY(1)},
        [4] = { .config = REGDMA_LINK_WAIT_INIT      (REGDMA_PCR_LINK(4), HP_SYS_CLKRST_SDIO_HOST_FUNC_CTRL0_REG,   0x0,                                           HP_SYS_CLKRST_REG_SDIO_LS_CLK_EDGE_CFG_UPDATE_M, 1, 0), .owner = ENTRY(0) | ENTRY(1)},
        /* TOP PD wake: DS/ECDSA CLK_EN defaults to 1 and start mem clean; wait idle before restoring their clocks */
        [5] = { .config = REGDMA_LINK_WAIT_INIT      (REGDMA_PCR_LINK(5), DS_QUERY_BUSY_REG,                        0,                                             DS_QUERY_BUSY_M,                                 1, 0), .owner = ENTRY(0) | ENTRY(1)},
        [6] = { .config = REGDMA_LINK_WAIT_INIT     (REGDMA_PCR_LINK(6), ECDSA_STATE_REG,                         0,                                             ECDSA_BUSY_M,                                    1, 0), .owner = ENTRY(0) | ENTRY(1)},
        [7] = { .config = REGDMA_LINK_CONTINUOUS_INIT(REGDMA_PCR_LINK(7), HP_SYS_CLKRST_CRYPTO_CTRL0_REG,          HP_SYS_CLKRST_CRYPTO_CTRL0_REG,                1,                                               0, 0), .owner = ENTRY(0) | ENTRY(1)},
    };

    esp_err_t err = sleep_retention_entries_create(pcr_regs_retention, ARRAY_SIZE(pcr_regs_retention), REGDMA_LINK_PRI_SYS_CLK, SLEEP_RETENTION_MODULE_CLOCK_SYSTEM);
    ESP_RETURN_ON_ERROR(err, TAG, "failed to allocate memory for system (PCR) retention");
    ESP_LOGI(TAG, "System Power, Clock and Reset sleep retention initialization");
    return ESP_OK;

    #undef N_REGS_PCR
}

#if CONFIG_MAC_BB_PD || CONFIG_BT_CTRL_SLEEP_ENABLE || CONFIG_IEEE802154_SLEEP_ENABLE
esp_err_t sleep_clock_modem_retention_init(void *arg)
{
    #define N_REGS_SYSCON() (((MODEM_SYSCON_MEM_RF2_CONF_REG - MODEM_SYSCON_TEST_CONF_REG) / 4) + 1)
    #define N_REGS_LPCON() (((MODEM_LPCON_MODEM_INTR_STATUS_REG - MODEM_LPCON_TEST_CONF_REG) / 4) + 1)
    const static sleep_retention_entries_config_t modem_regs_retention[] = {
        /* !!! pll_source_regs_retention link above does not above guarantee that the PMU has released the clock gate for the PLL.
         * And due to hardware limitations, REGDMA is also unable to obtain the PMU's PLL gate status when the chip is in PD_TOP mode.
         * So WORKAROUND with issue a dummy write to MODEM_SYSCON_DATE_REG as the first REGDMA node to avoid an APB access timeout
         * during the subsequent REGDMA operations. */
        [0] = { .config = REGDMA_LINK_WRITE_INIT(REGDMA_MODEMSYSCON_LINK(0), MODEM_SYSCON_DATE_REG, MODEM_SYSCON_DATE, MODEM_SYSCON_DATE_M, 1, 0), .owner = ENTRY(0) | ENTRY(1) },
        [1] = { .config = REGDMA_LINK_CONTINUOUS_INIT(REGDMA_MODEMSYSCON_LINK(1), MODEM_SYSCON_TEST_CONF_REG, MODEM_SYSCON_TEST_CONF_REG, N_REGS_SYSCON(), 0, 0), .owner = ENTRY(0) | ENTRY(1) }, /* MODEM SYSCON */
        [2] = { .config = REGDMA_LINK_CONTINUOUS_INIT(REGDMA_MODEMLPCON_LINK(0), MODEM_LPCON_TEST_CONF_REG, MODEM_LPCON_TEST_CONF_REG, N_REGS_LPCON(), 0, 0), .owner = ENTRY(0) | ENTRY(1) }, /* MODEM SYSCON */
        [3] = { .config = REGDMA_LINK_WRITE_INIT(REGDMA_CLOCK_ICG_LINK(0), HP_SYS_CLKRST_REF_160M_CTRL0_REG, HP_SYS_CLKRST_REG_REF_160M_CLK_EN, HP_SYS_CLKRST_REG_REF_160M_CLK_EN_M, 1, 0), .owner = ENTRY(1)},
        [4] = { .config = REGDMA_LINK_WRITE_INIT(REGDMA_CLOCK_ICG_LINK(1), HP_SYS_CLKRST_MODEM_CONF_REG, 0x3d, 0x3d, 1, 0), .owner = ENTRY(1)},
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
    const sleep_retention_module_bitmap_t retained_modules = sleep_retention_get_retained_modules();
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
        mask.bitmap[SLEEP_RETENTION_MODULE_PLL_SOURCE >> 5] |= BIT(SLEEP_RETENTION_MODULE_PLL_SOURCE % 32);
        mask.bitmap[SLEEP_RETENTION_MODULE_CLOCK_SYSTEM >> 5] |= BIT(SLEEP_RETENTION_MODULE_CLOCK_SYSTEM % 32);
    }

#if SOC_WIFI_SUPPORTED || SOC_BT_SUPPORTED || SOC_IEEE802154_SUPPORTED
    const sleep_retention_module_bitmap_t modem_modules = sleep_retention_module_bitmap_and(inited_modules, modem_clk_dep_modules);
    if (!sleep_retention_module_bitmap_eq(modem_modules, null_module)) {
        mask.bitmap[SLEEP_RETENTION_MODULE_PLL_SOURCE >> 5] |= BIT(SLEEP_RETENTION_MODULE_PLL_SOURCE % 32);
        mask.bitmap[SLEEP_RETENTION_MODULE_CLOCK_MODEM >> 5] |= BIT(SLEEP_RETENTION_MODULE_CLOCK_MODEM % 32);
    }
#endif

    const sleep_retention_module_bitmap_t clock_domain_inited_modules = sleep_retention_module_bitmap_and(inited_modules, mask);
    const sleep_retention_module_bitmap_t clock_domain_created_modules = sleep_retention_module_bitmap_and(created_modules, mask);
    const sleep_retention_module_bitmap_t clock_domain_retained_modules = sleep_retention_module_bitmap_and(retained_modules, mask);
    bool ic = sleep_retention_module_bitmap_eq(clock_domain_inited_modules, clock_domain_created_modules);
    bool cr = sleep_retention_module_bitmap_eq(clock_domain_created_modules, clock_domain_retained_modules);
    return ic && cr;
}

ESP_SYSTEM_INIT_FN(sleep_clock_startup_init, SECONDARY, BIT(0), 106)
{
    sleep_retention_module_init_param_t init_param = {
        .cbs       = { .create = { .handle = sleep_clock_pll_source_retention_init, .arg = NULL } },
        .attribute = SLEEP_RETENTION_MODULE_ATTR_PASSIVE | SLEEP_RETENTION_MODULE_ATTR_ATTACH
    };
    sleep_retention_module_init(SLEEP_RETENTION_MODULE_PLL_SOURCE, &init_param);

    init_param = (sleep_retention_module_init_param_t) {
        .cbs       = { .create = { .handle = sleep_clock_system_retention_init, .arg = NULL } },
        .depends   = RETENTION_MODULE_BITMAP_INIT(PLL_SOURCE),
        .attribute = SLEEP_RETENTION_MODULE_ATTR_PASSIVE | SLEEP_RETENTION_MODULE_ATTR_ATTACH
    };
    sleep_retention_module_init(SLEEP_RETENTION_MODULE_CLOCK_SYSTEM, &init_param);

#if CONFIG_MAC_BB_PD || CONFIG_BT_CTRL_SLEEP_ENABLE || CONFIG_IEEE802154_SLEEP_ENABLE
    init_param = (sleep_retention_module_init_param_t) {
        .cbs       = { .create = { .handle = sleep_clock_modem_retention_init, .arg = NULL } },
        .depends   = RETENTION_MODULE_BITMAP_INIT(PLL_SOURCE),
        .attribute = SLEEP_RETENTION_MODULE_ATTR_PASSIVE | SLEEP_RETENTION_MODULE_ATTR_ATTACH
    };
    sleep_retention_module_init(SLEEP_RETENTION_MODULE_CLOCK_MODEM, &init_param);
#endif
    return ESP_OK;
}
