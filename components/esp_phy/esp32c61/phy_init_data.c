/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "sdkconfig.h"
#include "soc/soc_caps.h"
#include "phy_init_data.h"
#include "esp_private/phy.h"
#include "esp_check.h"


const char __attribute__((section(".rodata"))) phy_init_magic_pre[] = PHY_INIT_MAGIC;

/**
 * @brief Structure containing default recommended PHY initialization parameters.
 */
const esp_phy_init_data_t phy_init_data= { {
        0x1,
        0x0,
        LIMIT(CONFIG_ESP_PHY_MAX_TX_POWER * 4, 0, 0x50),
        LIMIT(CONFIG_ESP_PHY_MAX_TX_POWER * 4, 0, 0x50),
        LIMIT(CONFIG_ESP_PHY_MAX_TX_POWER * 4, 0, 0x50),
        LIMIT(CONFIG_ESP_PHY_MAX_TX_POWER * 4, 0, 0x50),
        LIMIT(CONFIG_ESP_PHY_MAX_TX_POWER * 4, 0, 0x34),
        LIMIT(CONFIG_ESP_PHY_MAX_TX_POWER * 4, 0, 0x34),
        LIMIT(CONFIG_ESP_PHY_MAX_TX_POWER * 4, 0, 0x4c),
        LIMIT(CONFIG_ESP_PHY_MAX_TX_POWER * 4, 0, 0x4c),
        LIMIT(CONFIG_ESP_PHY_MAX_TX_POWER * 4, 0, 0x30),
        LIMIT(CONFIG_ESP_PHY_MAX_TX_POWER * 4, 0, 0x30),
        LIMIT(CONFIG_ESP_PHY_MAX_TX_POWER * 4, 0, 0x28),
        LIMIT(CONFIG_ESP_PHY_MAX_TX_POWER * 4, 0, 0x24),
        LIMIT(CONFIG_ESP_PHY_MAX_TX_POWER * 4, 0, 0x24),
        LIMIT(CONFIG_ESP_PHY_MAX_TX_POWER * 4, 0, 0x24),
        LIMIT(CONFIG_ESP_PHY_MAX_TX_POWER * 4, 0, 0x4c),
        LIMIT(CONFIG_ESP_PHY_MAX_TX_POWER * 4, 0, 0x4c),
        LIMIT(CONFIG_ESP_PHY_MAX_TX_POWER * 4, 0, 0x2c),
        LIMIT(CONFIG_ESP_PHY_MAX_TX_POWER * 4, 0, 0x2c),
        0x0,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0x51
} };

const char __attribute__((section(".rodata"))) phy_init_magic_post[] = PHY_INIT_MAGIC;

#if SOC_PM_MODEM_RETENTION_BY_REGDMA && CONFIG_MAC_BB_PD

#include "esp_private/sleep_retention.h"

static const char* TAG = "phy_sleep";

static esp_err_t sleep_retention_wifi_bb_init(void *arg)
{
    #define N_REGS_WIFI_AGC()       (121)
    #define N_REGS_WIFI_TX()        (14)
    #define N_REGS_WIFI_NRX()       (136)
    #define N_REGS_WIFI_BB()        (53)
    #define N_REGS_WIFI_BRX()       (39)
    #define N_REGS_WIFI_FE_COEX()   (58)
    #define N_REGS_WIFI_FE_DATA()   (41)
    #define N_REGS_WIFI_FE_CTRL()   (87)

    const static sleep_retention_entries_config_t bb_regs_retention[] = {
        [0] = { .config = REGDMA_LINK_CONTINUOUS_INIT(0x0b00, 0x600a7000, 0x600a7000, N_REGS_WIFI_AGC(),     0, 0), .owner = BIT(0) | BIT(1) }, /* AGC */
        [1] = { .config = REGDMA_LINK_CONTINUOUS_INIT(0x0b01, 0x600a7400, 0x600a7400, N_REGS_WIFI_TX(),      0, 0), .owner = BIT(0) | BIT(1) }, /* TX */
        [2] = { .config = REGDMA_LINK_CONTINUOUS_INIT(0x0b02, 0x600a7800, 0x600a7800, N_REGS_WIFI_NRX(),     0, 0), .owner = BIT(0) | BIT(1) }, /* NRX */
        [3] = { .config = REGDMA_LINK_CONTINUOUS_INIT(0x0b03, 0x600a7c00, 0x600a7c00, N_REGS_WIFI_BB(),      0, 0), .owner = BIT(0) | BIT(1) }, /* BB */
        [4] = { .config = REGDMA_LINK_CONTINUOUS_INIT(0x0b05, 0x600a0000, 0x600a0000, N_REGS_WIFI_FE_COEX(), 0, 0), .owner = BIT(0) | BIT(1) }, /* FE COEX */
        [5] = { .config = REGDMA_LINK_CONTINUOUS_INIT(0x0b06, 0x600a8000, 0x600a8000, N_REGS_WIFI_BRX(),     0, 0), .owner = BIT(0) | BIT(1) }, /* BRX */
        [6] = { .config = REGDMA_LINK_CONTINUOUS_INIT(0x0b07, 0x600a0400, 0x600a0400, N_REGS_WIFI_FE_DATA(), 0, 0), .owner = BIT(0) | BIT(1) }, /* FE DATA */
        [7] = { .config = REGDMA_LINK_CONTINUOUS_INIT(0x0b08, 0x600a0800, 0x600a0800, N_REGS_WIFI_FE_CTRL(), 0, 0), .owner = BIT(0) | BIT(1) }, /* FE CTRL */
    };
    esp_err_t err = sleep_retention_entries_create(bb_regs_retention, ARRAY_SIZE(bb_regs_retention), 3, SLEEP_RETENTION_MODULE_WIFI_BB);
    ESP_RETURN_ON_ERROR(err, TAG, "failed to allocate memory for modem (%s) retention", "WiFi BB");
    ESP_LOGD(TAG, "WiFi BB sleep retention initialization");
    return ESP_OK;
}

void esp_phy_sleep_data_init(void)
{
    sleep_retention_module_init_param_t init_param = {
        .cbs     = { .create = { .handle = sleep_retention_wifi_bb_init, .arg = NULL } },
        .depends = RETENTION_MODULE_BITMAP_INIT(CLOCK_MODEM)
    };
    esp_err_t err = sleep_retention_module_init(SLEEP_RETENTION_MODULE_WIFI_BB, &init_param);
    if (err != ESP_OK) {
        ESP_LOGW(TAG, "WiFi BB sleep retention init failed");
        return;
    }
    err = sleep_retention_module_allocate(SLEEP_RETENTION_MODULE_WIFI_BB);
    if (err != ESP_OK) {
        ESP_LOGW(TAG, "failed to allocate sleep retention linked list for wifi bb retention");
    }
}

void esp_phy_sleep_data_deinit(void)
{
    esp_err_t err = sleep_retention_module_free(SLEEP_RETENTION_MODULE_WIFI_BB);
    if (err != ESP_OK) {
        ESP_LOGW(TAG, "failed to free sleep retention linked list for wifi bb retention");
        return;
    }
    err = sleep_retention_module_deinit(SLEEP_RETENTION_MODULE_WIFI_BB);
    if (err != ESP_OK) {
        ESP_LOGW(TAG, "WiFi BB sleep retention deinit failed");
    }
}

#endif /* SOC_PM_MODEM_RETENTION_BY_REGDMA && CONFIG_MAC_BB_PD */
