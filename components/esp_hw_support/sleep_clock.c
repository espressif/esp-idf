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

#if SOC_PM_RETENTION_SW_TRIGGER_REGDMA
#include "modem/modem_lpcon_reg.h"
#endif

static __attribute__((unused)) const char *TAG = "sleep_clock";

esp_err_t sleep_clock_system_retention_init(void)
{
#if CONFIG_IDF_TARGET_ESP32C6
    #define N_REGS_PCR()    (((PCR_SRAM_POWER_CONF_REG - DR_REG_PCR_BASE) / 4) + 1)
#elif CONFIG_IDF_TARGET_ESP32H2
    #define N_REGS_PCR()    (((PCR_PWDET_SAR_CLK_CONF_REG - DR_REG_PCR_BASE) / 4) + 1)
#endif
    const static sleep_retention_entries_config_t pcr_regs_retention[] = {
        [0] = { .config = REGDMA_LINK_CONTINUOUS_INIT(REGDMA_PCR_LINK(0), DR_REG_PCR_BASE, DR_REG_PCR_BASE, N_REGS_PCR(), 0, 0), .owner = ENTRY(0) | ENTRY(2) }  /* pcr */
    };

    esp_err_t err = sleep_retention_entries_create(pcr_regs_retention, ARRAY_SIZE(pcr_regs_retention), REGDMA_LINK_PRI_0, SLEEP_RETENTION_MODULE_CLOCK_SYSTEM);
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
#if SOC_PM_RETENTION_SW_TRIGGER_REGDMA
    #define N_REGS_LPCON() (((MODEM_LPCON_MEM_CONF_REG - MODEM_LPCON_TEST_CONF_REG) / 4) + 1)
#endif

    const static sleep_retention_entries_config_t modem_regs_retention[] = {
        [0] = { .config = REGDMA_LINK_CONTINUOUS_INIT(REGDMA_MODEMSYSCON_LINK(0), MODEM_SYSCON_TEST_CONF_REG, MODEM_SYSCON_TEST_CONF_REG, N_REGS_SYSCON(), 0, 0), .owner = ENTRY(0) | ENTRY(1) }, /* MODEM SYSCON */
#if SOC_PM_RETENTION_SW_TRIGGER_REGDMA
        [1] = { .config = REGDMA_LINK_CONTINUOUS_INIT(REGDMA_MODEMLPCON_LINK(0),  MODEM_LPCON_TEST_CONF_REG, MODEM_LPCON_TEST_CONF_REG, N_REGS_LPCON(), 0, 0), .owner = ENTRY(0) | ENTRY(1) } /* MODEM LPCON */
#endif
    };

    esp_err_t err = sleep_retention_entries_create(modem_regs_retention, ARRAY_SIZE(modem_regs_retention), REGDMA_LINK_PRI_1, SLEEP_RETENTION_MODULE_CLOCK_MODEM);
    ESP_RETURN_ON_ERROR(err, TAG, "failed to allocate memory for modem (SYSCON) retention, 2 level priority");
    ESP_LOGI(TAG, "Modem Power, Clock and Reset sleep retention initialization");
    return ESP_OK;
}

void sleep_clock_modem_retention_deinit(void)
{
    sleep_retention_entries_destroy(SLEEP_RETENTION_MODULE_CLOCK_MODEM);
}

bool clock_domain_pd_allowed(void)
{
    const uint32_t modules = sleep_retention_get_modules();
    const uint32_t mask = (const uint32_t) (
            SLEEP_RETENTION_MODULE_CLOCK_SYSTEM
#if CONFIG_MAC_BB_PD || CONFIG_BT_LE_SLEEP_ENABLE || CONFIG_IEEE802154_SLEEP_ENABLE
          | SLEEP_RETENTION_MODULE_CLOCK_MODEM
#endif
          );
    return ((modules & mask) == mask);
}

#if CONFIG_PM_POWER_DOWN_PERIPHERAL_IN_LIGHT_SLEEP || CONFIG_MAC_BB_PD || CONFIG_BT_LE_SLEEP_ENABLE || CONFIG_IEEE802154_SLEEP_ENABLE
ESP_SYSTEM_INIT_FN(sleep_clock_startup_init, BIT(0), 106)
{
#if CONFIG_PM_POWER_DOWN_PERIPHERAL_IN_LIGHT_SLEEP
    sleep_clock_system_retention_init();
#endif

#if CONFIG_MAC_BB_PD || CONFIG_BT_LE_SLEEP_ENABLE || CONFIG_IEEE802154_SLEEP_ENABLE
    sleep_clock_modem_retention_init();
#endif
    return ESP_OK;
}
#endif
