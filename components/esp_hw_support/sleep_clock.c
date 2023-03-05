/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
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

static __attribute__((unused)) const char *TAG = "sleep_clock";

esp_err_t sleep_clock_system_retention_init(void)
{
    #define N_REGS_PCR()    (((PCR_SRAM_POWER_CONF_REG - DR_REG_PCR_BASE) / 4) + 1)

    const static sleep_retention_entries_config_t pcr_regs_retention[] = {
        [0] = { .config = REGDMA_LINK_CONTINUOUS_INIT(REGDMA_PCR_LINK(0), DR_REG_PCR_BASE, DR_REG_PCR_BASE, N_REGS_PCR(), 0, 0), .owner = ENTRY(0) | ENTRY(2) }  /* pcr */
    };

    esp_err_t err = sleep_retention_entries_create(pcr_regs_retention, ARRAY_SIZE(pcr_regs_retention), REGDMA_LINK_PRI_1, SLEEP_RETENTION_MODULE_CLOCK_SYSTEM);
    ESP_RETURN_ON_ERROR(err, TAG, "failed to allocate memory for system (PCR) retention");
    ESP_LOGI(TAG, "System Power, Clock and Reset sleep retention initialization");
    return ESP_OK;
}

void sleep_clock_system_retention_deinit(void)
{
    sleep_retention_entries_destroy(SLEEP_RETENTION_MODULE_CLOCK_SYSTEM);
}

esp_err_t sleep_clock_modem_retention_init(void)
{
    #define N_REGS_SYSCON() (((MODEM_SYSCON_MEM_CONF_REG - MODEM_SYSCON_TEST_CONF_REG) / 4) + 1)

    #define MODEM_WIFI_RETENTION_CLOCK  (MODEM_SYSCON_CLK_WIFI_APB_EN | MODEM_SYSCON_CLK_FE_APB_EN)

    #define WIFI_MAC_MODEM_STATE_CLK_EN (MODEM_SYSCON_CLK_WIFIMAC_EN | MODEM_SYSCON_CLK_WIFI_APB_EN)
    #define WIFI_BB_MODEM_STATE_CLK_EN  (MODEM_SYSCON_CLK_WIFIBB_22M_EN  | \
                                         MODEM_SYSCON_CLK_WIFIBB_40M_EN  | \
                                         MODEM_SYSCON_CLK_WIFIBB_44M_EN  | \
                                         MODEM_SYSCON_CLK_WIFIBB_80M_EN  | \
                                         MODEM_SYSCON_CLK_WIFIBB_40X_EN  | \
                                         MODEM_SYSCON_CLK_WIFIBB_80X_EN  | \
                                         MODEM_SYSCON_CLK_WIFIBB_40X1_EN | \
                                         MODEM_SYSCON_CLK_WIFIBB_80X1_EN | \
                                         MODEM_SYSCON_CLK_WIFIBB_160X1_EN)
    #define FE_MODEM_STATE_CLK_EN       (MODEM_SYSCON_CLK_FE_80M_EN      | \
                                         MODEM_SYSCON_CLK_FE_160M_EN     | \
                                         MODEM_SYSCON_CLK_FE_CAL_160M_EN | \
                                         MODEM_SYSCON_CLK_FE_APB_EN)
    #define WIFI_MODEM_STATE_CLOCK_EN   (WIFI_MAC_MODEM_STATE_CLK_EN | WIFI_BB_MODEM_STATE_CLK_EN | FE_MODEM_STATE_CLK_EN)


    const static sleep_retention_entries_config_t modem_regs_retention[] = {
        [0] = { .config = REGDMA_LINK_WRITE_INIT     (REGDMA_MODEMSYSCON_LINK(0x00), MODEM_SYSCON_CLK_CONF1_REG, 0x0,                        0x200,           0, 1), .owner = ENTRY(0) }, /* WiFi MAC clock disable */
        [1] = { .config = REGDMA_LINK_CONTINUOUS_INIT(REGDMA_MODEMSYSCON_LINK(0x01), MODEM_SYSCON_TEST_CONF_REG, MODEM_SYSCON_TEST_CONF_REG, N_REGS_SYSCON(), 0, 0), .owner = ENTRY(0) | ENTRY(1) }, /* MODEM SYSCON */
        [2] = { .config = REGDMA_LINK_WRITE_INIT     (REGDMA_MODEMSYSCON_LINK(0x02), MODEM_SYSCON_CLK_CONF1_REG, MODEM_SYSCON_CLK_WIFIMAC_EN,0x200,           1, 0), .owner = ENTRY(0) }, /* WiFi MAC clock enable */
        [3] = { .config = REGDMA_LINK_WRITE_INIT     (REGDMA_MODEMSYSCON_LINK(0x03), MODEM_SYSCON_CLK_CONF1_REG, MODEM_WIFI_RETENTION_CLOCK, 0x10400,         0, 0), .owner = ENTRY(0) }, /* WiFi (MAC, BB and FE) retention clock enable */
        [4] = { .config = REGDMA_LINK_WRITE_INIT     (REGDMA_MODEMSYSCON_LINK(0x04), MODEM_SYSCON_CLK_CONF1_REG, WIFI_MODEM_STATE_CLOCK_EN,  0x1e7ff,         1, 0), .owner = ENTRY(1) }
    };

    const static sleep_retention_entries_config_t modem_retention_clock[] = {
        [0] = { .config = REGDMA_LINK_WRITE_INIT     (REGDMA_MODEMSYSCON_LINK(0xff), MODEM_SYSCON_CLK_CONF1_REG, 0x0,                        0x10400,         0, 0), .owner = ENTRY(0) }  /* WiFi (MAC, BB and FE) retention clock disable */
    };

    esp_err_t err = sleep_retention_entries_create(modem_regs_retention, ARRAY_SIZE(modem_regs_retention), REGDMA_LINK_PRI_2, SLEEP_RETENTION_MODULE_CLOCK_MODEM);
    ESP_RETURN_ON_ERROR(err, TAG, "failed to allocate memory for modem (SYSCON) retention, 2 level priority");

    err = sleep_retention_entries_create(modem_retention_clock, ARRAY_SIZE(modem_retention_clock), REGDMA_LINK_PRI_7, SLEEP_RETENTION_MODULE_CLOCK_MODEM);
    ESP_RETURN_ON_ERROR(err, TAG, "failed to allocate memory for modem (SYSCON) retention, lowest level priority");

    ESP_LOGI(TAG, "Modem Power, Clock and Reset sleep retention initialization");
    return ESP_OK;
}

void sleep_clock_modem_retention_deinit(void)
{
    sleep_retention_entries_destroy(SLEEP_RETENTION_MODULE_CLOCK_MODEM);
}

bool IRAM_ATTR clock_domain_pd_allowed(void)
{
    const uint32_t modules = sleep_retention_get_modules();
    const uint32_t mask = (const uint32_t) (
            SLEEP_RETENTION_MODULE_CLOCK_SYSTEM
#if CONFIG_MAC_BB_PD
          | SLEEP_RETENTION_MODULE_CLOCK_MODEM
#endif
          );
    return ((modules & mask) == mask);
}

#if CONFIG_PM_POWER_DOWN_PERIPHERAL_IN_LIGHT_SLEEP || CONFIG_MAC_BB_PD
ESP_SYSTEM_INIT_FN(sleep_clock_startup_init, BIT(0), 106)
{
#if CONFIG_PM_POWER_DOWN_PERIPHERAL_IN_LIGHT_SLEEP
    sleep_clock_system_retention_init();
#endif

#if CONFIG_MAC_BB_PD
    sleep_clock_modem_retention_init();
#endif
    return ESP_OK;
}
#endif
