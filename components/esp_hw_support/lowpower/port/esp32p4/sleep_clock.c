/*
 * SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_private/sleep_clock.h"
#include "soc/hp_sys_clkrst_reg.h"

static const char *TAG = "sleep_clock";

esp_err_t sleep_clock_system_retention_init(void *arg)
{
    #define N_REGS_PCR()    (((HP_SYS_CLKRST_HPCORE_WDT_RESET_SOURCE0_REG - DR_REG_HP_SYS_CLKRST_BASE) / 4) + 1)

    const static sleep_retention_entries_config_t pcr_regs_retention[] = {
        [0] = { .config = REGDMA_LINK_CONTINUOUS_INIT(REGDMA_PCR_LINK(0x0),  DR_REG_HP_SYS_CLKRST_BASE,         DR_REG_HP_SYS_CLKRST_BASE,                      N_REGS_PCR(),                                       0, 0), .owner = ENTRY(0) },  /* pcr */
        [1] = { .config = REGDMA_LINK_WRITE_INIT     (REGDMA_PCR_LINK(0x01), HP_SYS_CLKRST_ROOT_CLK_CTRL0_REG,  HP_SYS_CLKRST_REG_SOC_CLK_DIV_UPDATE,           HP_SYS_CLKRST_REG_SOC_CLK_DIV_UPDATE_M,             1, 0), .owner = ENTRY(0) },
        [2] = { .config = REGDMA_LINK_WAIT_INIT      (REGDMA_PCR_LINK(0x02), HP_SYS_CLKRST_ROOT_CLK_CTRL0_REG,  0x0,                                            HP_SYS_CLKRST_REG_SOC_CLK_DIV_UPDATE_M,             1, 0), .owner = ENTRY(0) },
        [3] = { .config = REGDMA_LINK_WRITE_INIT     (REGDMA_PCR_LINK(0x03), HP_SYS_CLKRST_PERI_CLK_CTRL02_REG, HP_SYS_CLKRST_REG_SDIO_LS_CLK_EDGE_CFG_UPDATE,  HP_SYS_CLKRST_REG_SDIO_LS_CLK_EDGE_CFG_UPDATE_M,    1, 0), .owner = ENTRY(0) },
        [4] = { .config = REGDMA_LINK_WAIT_INIT      (REGDMA_PCR_LINK(0x04), HP_SYS_CLKRST_PERI_CLK_CTRL02_REG, 0x0,                                            HP_SYS_CLKRST_REG_SDIO_LS_CLK_EDGE_CFG_UPDATE_M,    1, 0), .owner = ENTRY(0) },
    };

    esp_err_t err = sleep_retention_entries_create(pcr_regs_retention, ARRAY_SIZE(pcr_regs_retention), REGDMA_LINK_PRI_SYS_CLK, SLEEP_RETENTION_MODULE_CLOCK_SYSTEM);
    ESP_RETURN_ON_ERROR(err, TAG, "failed to allocate memory for system (PCR) retention");
    ESP_LOGI(TAG, "System Power, Clock and Reset sleep retention initialization");
    return ESP_OK;

    #undef N_REGS_PCR
}

bool clock_domain_pd_allowed(void)
{
    const uint32_t inited_modules = sleep_retention_get_inited_modules();
    const uint32_t created_modules = sleep_retention_get_created_modules();
    const uint32_t sys_clk_dep_modules = (const uint32_t) (BIT(SLEEP_RETENTION_MODULE_SYS_PERIPH));

    uint32_t mask = 0;
    if (inited_modules & sys_clk_dep_modules) {
        mask |= BIT(SLEEP_RETENTION_MODULE_CLOCK_SYSTEM);
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
    return ESP_OK;
}
