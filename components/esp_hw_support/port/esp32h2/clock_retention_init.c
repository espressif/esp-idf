/*
 * SPDX-FileCopyrightText: 2022-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_private/sleep_clock.h"
#include "soc/i2c_ana_mst_reg.h"
#include "soc/regi2c_defs.h"
#include "soc/pcr_reg.h"
#include "soc/regi2c_defs.h"
#include "modem/modem_syscon_reg.h"
#include "modem/modem_lpcon_reg.h"


static __attribute__((unused)) const char *TAG = "sleep_clock";

esp_err_t sleep_clock_system_retention_init(void *arg)
{
    #define N_REGS_PCR()    (((PCR_PWDET_SAR_CLK_CONF_REG - DR_REG_PCR_BASE) / 4) + 1)

    const static sleep_retention_entries_config_t pcr_regs_retention[] = {
        /* Enable i2c master clock */
        [0] = { .config = REGDMA_LINK_WRITE_INIT     (REGDMA_PCR_LINK(0),   MODEM_LPCON_CLK_CONF_REG,   MODEM_LPCON_CLK_I2C_MST_EN,     MODEM_LPCON_CLK_I2C_MST_EN_M,   1, 0), .owner = ENTRY(0) },
        /* Start BBPLL self-calibration */
        [1] = { .config = REGDMA_LINK_WRITE_INIT     (REGDMA_PCR_LINK(1),   I2C_MST_ANA_CONF0_REG,      0,                              I2C_MST_BBPLL_STOP_FORCE_HIGH,  1, 0), .owner = ENTRY(0) },
        [2] = { .config = REGDMA_LINK_WRITE_INIT     (REGDMA_PCR_LINK(2),   I2C_MST_ANA_CONF0_REG,      I2C_MST_BBPLL_STOP_FORCE_LOW,   I2C_MST_BBPLL_STOP_FORCE_LOW,   1, 0), .owner = ENTRY(0) },
        /* Wait calibration done */
        [3] = { .config = REGDMA_LINK_WAIT_INIT      (REGDMA_PCR_LINK(3),   I2C_MST_ANA_CONF0_REG,      I2C_MST_BBPLL_CAL_DONE,         I2C_MST_BBPLL_CAL_DONE,         1, 0), .owner = ENTRY(0) },
        /* Stop BBPLL self-calibration */
        [4] = { .config = REGDMA_LINK_WRITE_INIT     (REGDMA_PCR_LINK(4),   I2C_MST_ANA_CONF0_REG,      0,                              I2C_MST_BBPLL_STOP_FORCE_LOW,   1, 0), .owner = ENTRY(0) },
        [5] = { .config = REGDMA_LINK_WRITE_INIT     (REGDMA_PCR_LINK(5),   I2C_MST_ANA_CONF0_REG,      I2C_MST_BBPLL_STOP_FORCE_HIGH,  I2C_MST_BBPLL_STOP_FORCE_HIGH,  1, 0), .owner = ENTRY(0) },
        /* Clock configuration retention */
        [6] = { .config = REGDMA_LINK_CONTINUOUS_INIT(REGDMA_PCR_LINK(6),   DR_REG_PCR_BASE,            DR_REG_PCR_BASE,                N_REGS_PCR(),                   0, 0), .owner = ENTRY(0) | ENTRY(2) },
        [7] = { .config = REGDMA_LINK_CONTINUOUS_INIT(REGDMA_PCR_LINK(7),   PCR_RESET_EVENT_BYPASS_REG, PCR_RESET_EVENT_BYPASS_REG,     1,                              0, 0), .owner = ENTRY(0) | ENTRY(2) },
        [8] = { .config = REGDMA_LINK_WRITE_INIT     (REGDMA_PCR_LINK(8),   PCR_BUS_CLK_UPDATE_REG,     PCR_BUS_CLOCK_UPDATE,           PCR_BUS_CLOCK_UPDATE_M,         1, 0), .owner = ENTRY(0) | ENTRY(2) },
        [9] = { .config = REGDMA_LINK_WAIT_INIT      (REGDMA_PCR_LINK(9),   PCR_BUS_CLK_UPDATE_REG,     0x0,                            PCR_BUS_CLOCK_UPDATE_M,         1, 0), .owner = ENTRY(0) | ENTRY(2) },
    };

    esp_err_t err = sleep_retention_entries_create(pcr_regs_retention, ARRAY_SIZE(pcr_regs_retention), REGDMA_LINK_PRI_SYS_CLK, SLEEP_RETENTION_MODULE_CLOCK_SYSTEM);
    ESP_RETURN_ON_ERROR(err, TAG, "failed to allocate memory for system (PCR) retention");
    ESP_LOGI(TAG, "System Power, Clock and Reset sleep retention initialization");
    return ESP_OK;
}

#if CONFIG_MAC_BB_PD || CONFIG_BT_LE_SLEEP_ENABLE || CONFIG_IEEE802154_SLEEP_ENABLE
esp_err_t sleep_clock_modem_retention_init(void *arg)
{
    #define N_REGS_SYSCON() (((MODEM_SYSCON_MEM_CONF_REG - MODEM_SYSCON_TEST_CONF_REG) / 4) + 1)
    #define N_REGS_LPCON() (((MODEM_LPCON_MEM_CONF_REG - MODEM_LPCON_TEST_CONF_REG) / 4) + 1)

    const static sleep_retention_entries_config_t modem_regs_retention[] = {
        [0] = { .config = REGDMA_LINK_CONTINUOUS_INIT(REGDMA_MODEMSYSCON_LINK(0), MODEM_SYSCON_TEST_CONF_REG, MODEM_SYSCON_TEST_CONF_REG, N_REGS_SYSCON(), 0, 0), .owner = ENTRY(0) | ENTRY(1) }, /* MODEM SYSCON */
        [1] = { .config = REGDMA_LINK_CONTINUOUS_INIT(REGDMA_MODEMLPCON_LINK(0),  MODEM_LPCON_TEST_CONF_REG, MODEM_LPCON_TEST_CONF_REG, N_REGS_LPCON(), 0, 0), .owner = ENTRY(0) | ENTRY(1) } /* MODEM LPCON */
    };

    esp_err_t err = sleep_retention_entries_create(modem_regs_retention, ARRAY_SIZE(modem_regs_retention), REGDMA_LINK_PRI_MODEM_CLK, SLEEP_RETENTION_MODULE_CLOCK_MODEM);
    ESP_RETURN_ON_ERROR(err, TAG, "failed to allocate memory for modem (SYSCON) retention, 1 level priority");
    ESP_LOGI(TAG, "Modem Power, Clock and Reset sleep retention initialization");
    return ESP_OK;
}
#endif
