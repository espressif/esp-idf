/*
 * SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_private/sleep_clock.h"
#include "soc/pcr_reg.h"
#include "modem/modem_syscon_reg.h"

static __attribute__((unused)) const char *TAG = "sleep_clock";

esp_err_t sleep_clock_system_retention_init(void *arg)
{
    #define N_REGS_PCR()    (((PCR_SRAM_POWER_CONF_REG - DR_REG_PCR_BASE) / 4) + 1)

    const static sleep_retention_entries_config_t pcr_regs_retention[] = {
        [0] = { .config = REGDMA_LINK_CONTINUOUS_INIT(REGDMA_PCR_LINK(0), DR_REG_PCR_BASE, DR_REG_PCR_BASE, N_REGS_PCR(), 0, 0), .owner = ENTRY(0) | ENTRY(2) }  /* pcr */
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

    #define MODEM_WIFI_RETENTION_CLOCK      (MODEM_SYSCON_CLK_WIFI_APB_FO | MODEM_SYSCON_CLK_FE_APB_FO)
    #define MODEM_WIFI_RETENTION_CLOCK_MASK (MODEM_SYSCON_CLK_WIFI_APB_FO_M | MODEM_SYSCON_CLK_FE_APB_FO_M)

    const static sleep_retention_entries_config_t modem_regs_retention[] = {
        [0] = { .config = REGDMA_LINK_CONTINUOUS_INIT(REGDMA_MODEMSYSCON_LINK(0), MODEM_SYSCON_TEST_CONF_REG, MODEM_SYSCON_TEST_CONF_REG, N_REGS_SYSCON(), 0, 0), .owner = ENTRY(0) | ENTRY(1) }, /* MODEM SYSCON */
        [1] = { .config = REGDMA_LINK_WRITE_INIT     (REGDMA_MODEMSYSCON_LINK(1), MODEM_SYSCON_CLK_CONF1_FORCE_ON_REG, MODEM_WIFI_RETENTION_CLOCK, MODEM_WIFI_RETENTION_CLOCK_MASK, 0, 0), .owner = ENTRY(0) }, /* WiFi (MAC, BB and FE) retention clock enable */
#if SOC_PM_RETENTION_SW_TRIGGER_REGDMA
        [2] = { .config = REGDMA_LINK_CONTINUOUS_INIT(REGDMA_MODEMLPCON_LINK(0),  MODEM_LPCON_TEST_CONF_REG, MODEM_LPCON_TEST_CONF_REG, N_REGS_LPCON(), 0, 0), .owner = ENTRY(0) | ENTRY(1) } /* MODEM LPCON */
#endif
    };

    const static sleep_retention_entries_config_t modem_retention_clock[] = {
        [0] = { .config = REGDMA_LINK_WRITE_INIT     (REGDMA_MODEMSYSCON_LINK(0xf0), MODEM_SYSCON_CLK_CONF1_FORCE_ON_REG, 0x0, MODEM_WIFI_RETENTION_CLOCK_MASK, 0, 0), .owner = ENTRY(0) }  /* WiFi (MAC, BB and FE) retention clock disable */
    };

    esp_err_t err = sleep_retention_entries_create(modem_regs_retention, ARRAY_SIZE(modem_regs_retention), REGDMA_LINK_PRI_MODEM_CLK, SLEEP_RETENTION_MODULE_CLOCK_MODEM);
    ESP_RETURN_ON_ERROR(err, TAG, "failed to allocate memory for modem (SYSCON) retention, 1 level priority");

    err = sleep_retention_entries_create(modem_retention_clock, ARRAY_SIZE(modem_retention_clock), REGDMA_LINK_PRI_7, SLEEP_RETENTION_MODULE_CLOCK_MODEM);
    ESP_RETURN_ON_ERROR(err, TAG, "failed to allocate memory for modem (SYSCON) retention, lowest level priority");

    ESP_LOGI(TAG, "Modem Power, Clock and Reset sleep retention initialization");
    return ESP_OK;
}
#endif
