/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "sdkconfig.h"
#include "soc/soc_caps.h"
#include "phy_init_data.h"
#include "esp_private/phy.h"
#include "esp_check.h"

#if SOC_PM_MODEM_RETENTION_BY_REGDMA && CONFIG_ESP_PHY_HW_SWITCH_RF
#include "esp_private/sleep_retention.h"

static const char* TAG = "phy_sleep";

static esp_err_t sleep_retention_phy_fe_init(void *arg)
{
    #define REG_FECOEX_BASE    0x600C0000
    #define REG_FECTRL_BASE    0x600C0800
    #define REG_FEDATA_BASE    0x600C0400
    #define REG_FEDATABLE_BASE 0x600C0C00
    #define N_REGS_FE_COEX()    (11)
    #define N_REGS_FE_CTRL()    (26)
    #define N_REGS_FE_DATA()    (8)
    #define N_REGS_FE_DATA_BLE()    (2)

    const static sleep_retention_entries_config_t phy_fe_regs_retention[] = {
        #define PHY_ENTRY() (BIT(SOC_PM_PAU_REGDMA_LINK_IDX_PHY))
        [0] = { .config = REGDMA_LINK_ADDR_MAP_INIT(REGDMA_MODEM_FE_LINK(0),   REG_FECOEX_BASE, REG_FECOEX_BASE, N_REGS_FE_COEX(),  0, 0, 0x147e89, 0x0, 0x0, 0x0),         .owner = PHY_ENTRY() },
        [1] = { .config = REGDMA_LINK_ADDR_MAP_INIT(REGDMA_MODEM_FE_LINK(1),   REG_FECTRL_BASE, REG_FECTRL_BASE, N_REGS_FE_CTRL(),  0, 0, 0x370e100b, 0x7ff8084, 0x0, 0x0), .owner = PHY_ENTRY() },
        [2] = { .config = REGDMA_LINK_ADDR_MAP_INIT(REGDMA_MODEM_FE_LINK(2),   REG_FEDATA_BASE, REG_FEDATA_BASE, N_REGS_FE_DATA(),  0, 0, 0x6b805, 0x0, 0x0, 0x0),          .owner = PHY_ENTRY() },
        [3] = { .config = REGDMA_LINK_ADDR_MAP_INIT(REGDMA_MODEM_FE_LINK(3),   REG_FEDATABLE_BASE, REG_FEDATABLE_BASE, N_REGS_FE_DATA_BLE(),  0, 0, 0x3, 0x0, 0x0, 0x0),    .owner = PHY_ENTRY() },
    };
    esp_err_t err = sleep_retention_entries_create(phy_fe_regs_retention, ARRAY_SIZE(phy_fe_regs_retention), 3, SLEEP_RETENTION_MODULE_PHY_FE);
    ESP_RETURN_ON_ERROR(err, TAG, "failed to allocate memory for modem (%s) retention", "PHY FE");
    ESP_LOGD(TAG, "PHY FE sleep retention initialization");
    return ESP_OK;
}

esp_err_t esp_phy_fe_sleep_data_init(void)
{
    sleep_retention_module_init_param_t init_param = {
        .cbs     = { .create = { .handle = sleep_retention_phy_fe_init, .arg = NULL } },
        .attribute = SLEEP_RETENTION_MODULE_ATTR_PASSIVE | SLEEP_RETENTION_MODULE_ATTR_ATTACH
    };
    esp_err_t err = sleep_retention_module_init(SLEEP_RETENTION_MODULE_PHY_FE, &init_param);
    if (err != ESP_OK) {
        ESP_LOGW(TAG, "PHY FE sleep retention init failed");
        return err;
    }
    return ESP_OK;
}

void esp_phy_fe_sleep_data_deinit(void)
{
    esp_err_t err = sleep_retention_module_deinit(SLEEP_RETENTION_MODULE_PHY_FE);
    if (err != ESP_OK) {
        ESP_LOGW(TAG, "PHY FE sleep retention deinit failed");
    }
}
#endif // SOC_PM_MODEM_RETENTION_BY_REGDMA && CONFIG_ESP_PHY_HW_SWITCH_RF
