/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include <string.h>
#include "esp_timer.h"
#include "esp_log.h"
#include "esp_private/esp_gpio_reserve.h"
#include "soc/gpio_sig_map.h"
#include "driver/gpio.h"
#include "esp_rom_gpio.h"
#include "esp_phy_init.h"
#include "esp_private/phy.h"
#include "esp_phy.h"
#include "esp_attr.h"

static const char* TAG = "phy_comm";

static volatile uint16_t s_phy_modem_flag = 0;

extern void phy_param_track_tot(bool en_wifi, bool en_ble_154);
static esp_timer_handle_t phy_track_pll_timer;
#if CONFIG_ESP_WIFI_ENABLED
static volatile int64_t s_wifi_prev_timestamp;
#endif
#if CONFIG_IEEE802154_ENABLED || CONFIG_BT_ENABLED
static volatile int64_t s_bt_154_prev_timestamp;
#endif
#define PHY_TRACK_PLL_PERIOD_IN_US 1000000
static void phy_track_pll_internal(void);

static esp_phy_ant_gpio_config_t s_phy_ant_gpio_config = { 0 };
static esp_phy_ant_config_t s_phy_ant_config = { 0 };

#if CONFIG_IEEE802154_ENABLED || CONFIG_BT_ENABLED || CONFIG_ESP_WIFI_ENABLED
bool phy_enabled_modem_contains(esp_phy_modem_t modem)
{
    return (s_phy_modem_flag & modem) != 0;
}
#endif

void phy_track_pll(void)
{
    // Light sleep scenario: enabling and disabling PHY frequently, the timer will not get triggered.
    // Using a variable to record the previously tracked time when PLL was last called.
    // If the duration is larger than PHY_TRACK_PLL_PERIOD_IN_US, then track PLL.
    bool need_track_pll = false;
#if CONFIG_ESP_WIFI_ENABLED
    if (phy_enabled_modem_contains(PHY_MODEM_WIFI)) {
        need_track_pll = need_track_pll || ((esp_timer_get_time() - s_wifi_prev_timestamp) > PHY_TRACK_PLL_PERIOD_IN_US);
    }
#endif
#if CONFIG_IEEE802154_ENABLED || CONFIG_BT_ENABLED
    if (phy_enabled_modem_contains(PHY_MODEM_BT | PHY_MODEM_IEEE802154)) {
        need_track_pll = need_track_pll || ((esp_timer_get_time() - s_bt_154_prev_timestamp) > PHY_TRACK_PLL_PERIOD_IN_US);
    }
#endif
    if (need_track_pll) {
        phy_track_pll_internal();
    }
}

static void phy_track_pll_internal(void)
{
    bool wifi_track_pll = false;
    bool ble_154_track_pll = false;
#if CONFIG_ESP_WIFI_ENABLED
    if (phy_enabled_modem_contains(PHY_MODEM_WIFI)) {
        wifi_track_pll = true;
        s_wifi_prev_timestamp = esp_timer_get_time();
    }
#endif

#if CONFIG_IEEE802154_ENABLED || CONFIG_BT_ENABLED
    if (phy_enabled_modem_contains(PHY_MODEM_BT | PHY_MODEM_IEEE802154)) {
        ble_154_track_pll = true;
        s_bt_154_prev_timestamp = esp_timer_get_time();
    }
#endif
    if (wifi_track_pll || ble_154_track_pll) {
#if CONFIG_ESP_PHY_PLL_TRACK_DEBUG
#if CONFIG_IEEE802154_ENABLED || CONFIG_BT_ENABLED
        ESP_LOGI("PLL_TRACK", "BT or IEEE802154 tracks PLL: %s", ble_154_track_pll ? "True" : "False");
#endif
#if CONFIG_ESP_WIFI_ENABLED
        ESP_LOGI("PLL_TRACK", "Wi-Fi tracks PLL: %s", wifi_track_pll ? "True" : "False");
#endif
#endif
        phy_param_track_tot(wifi_track_pll, ble_154_track_pll);
    }
}

static void phy_track_pll_timer_callback(void* arg)
{
    _lock_t phy_lock = phy_get_lock();
    _lock_acquire(&phy_lock);
    phy_track_pll_internal();
    _lock_release(&phy_lock);
}

void phy_track_pll_init(void)
{
    const esp_timer_create_args_t phy_track_pll_timer_args = {
            .callback = &phy_track_pll_timer_callback,
            .name = "phy-track-pll-timer"
    };
    ESP_ERROR_CHECK(esp_timer_create(&phy_track_pll_timer_args, &phy_track_pll_timer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(phy_track_pll_timer, PHY_TRACK_PLL_PERIOD_IN_US));
}

void phy_track_pll_deinit(void)
{
    ESP_ERROR_CHECK(esp_timer_stop(phy_track_pll_timer));
    ESP_ERROR_CHECK(esp_timer_delete(phy_track_pll_timer));
}

void phy_set_modem_flag(esp_phy_modem_t modem)
{
    s_phy_modem_flag |= modem;
}

void phy_clr_modem_flag(esp_phy_modem_t modem)
{
    s_phy_modem_flag &= ~modem;
}

esp_phy_modem_t phy_get_modem_flag(void)
{
    return s_phy_modem_flag;
}

static DRAM_ATTR bool s_phy_ant_need_update_flag = false;

IRAM_ATTR bool phy_ant_need_update(void)
{
    return s_phy_ant_need_update_flag;
}

void phy_ant_clr_update_flag(void)
{
    s_phy_ant_need_update_flag = false;
}

static void phy_ant_set_gpio_output(uint32_t io_num)
{
    gpio_config_t io_conf = {};
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = (1ULL << io_num);
    io_conf.pull_down_en = 0;
    io_conf.pull_up_en = 0;
    gpio_config(&io_conf);
}

esp_err_t esp_phy_set_ant_gpio(esp_phy_ant_gpio_config_t *config)
{
    if (config == NULL) {
        ESP_LOGE(TAG, "Invalid configuration");
        return ESP_ERR_INVALID_ARG;
    }

    for (int i = 0; i < 4; i++) {
        if (config->gpio_cfg[i].gpio_select == 1) {
            if(esp_gpio_is_reserved(config->gpio_cfg[i].gpio_num)) {
                ESP_LOGE(TAG, "gpio[%d] number: %d is reserved\n", i, config->gpio_cfg[i].gpio_num);
                return ESP_ERR_INVALID_ARG;
            }
        }
    }

    for (int i = 0; i < 4; i++) {
        if (config->gpio_cfg[i].gpio_select == 1) {
            phy_ant_set_gpio_output(config->gpio_cfg[i].gpio_num);
            esp_rom_gpio_connect_out_signal(config->gpio_cfg[i].gpio_num, ANT_SEL0_IDX + i, 0, 0);
        }
    }

    memcpy(&s_phy_ant_gpio_config, config, sizeof(esp_phy_ant_gpio_config_t));

    return ESP_OK;
}

esp_err_t esp_phy_get_ant_gpio(esp_phy_ant_gpio_config_t *config)
{
    if (config == NULL) {
        ESP_LOGE(TAG, "Invalid configuration");
        return ESP_ERR_INVALID_ARG;
    }

    memcpy(config, &s_phy_ant_gpio_config, sizeof(esp_phy_ant_gpio_config_t));

    return ESP_OK;
}

static bool phy_ant_config_check(esp_phy_ant_config_t *config)
{
    if ((config->rx_ant_mode >= ESP_PHY_ANT_MODE_MAX)
        ||(config->tx_ant_mode >= ESP_PHY_ANT_MODE_MAX)
        ||(config->rx_ant_default >= ESP_PHY_ANT_MAX)) {
        ESP_LOGE(TAG, "Invalid antenna: rx=%d, tx=%d, default=%d",
            config->rx_ant_mode, config->tx_ant_mode, config->rx_ant_default);
        return ESP_ERR_INVALID_ARG;
    }

    if ((config->tx_ant_mode == ESP_PHY_ANT_MODE_AUTO) && (config->rx_ant_mode != ESP_PHY_ANT_MODE_AUTO)) {
        ESP_LOGE(TAG, "If tx ant is AUTO, also need to set rx ant to AUTO");
        return ESP_ERR_INVALID_ARG;
    }

    return ESP_OK;
}

void phy_ant_update(void)
{
    uint8_t rx_ant0 = 0, rx_ant1 = 0, tx_ant0 = 0;
    esp_phy_ant_config_t *config = &s_phy_ant_config;
    uint8_t ant0 = config->enabled_ant0;
    uint8_t ant1 = config->enabled_ant1;
    bool rx_auto = false;
    uint8_t def_ant = 0;
    switch (config->rx_ant_mode) {
        case ESP_PHY_ANT_MODE_ANT0:
            rx_ant0 = ant0;
            rx_ant1 = ant0;
            break;
        case ESP_PHY_ANT_MODE_ANT1:
            rx_ant0 = ant1;
            rx_ant1 = ant1;
            break;
        case ESP_PHY_ANT_MODE_AUTO:
            rx_ant0 = ant0;
            rx_ant1 = ant1;
            rx_auto = true;
            break;
        default:
            rx_ant0 = ant0;
            rx_ant1 = ant0;
    }

    switch (config->tx_ant_mode) {
        case ESP_PHY_ANT_MODE_ANT0:
            tx_ant0 = ant0;
            break;
        case ESP_PHY_ANT_MODE_ANT1:
            tx_ant0 = ant1;
            break;
        default:
            tx_ant0 = ant0;
    }

    switch (config->rx_ant_default) {
        case ESP_PHY_ANT_ANT0:
            def_ant = 0;
            break;
        case ESP_PHY_ANT_ANT1:
            def_ant = 1;
            break;
        default:
            def_ant = 0;
    }

    ant_dft_cfg(def_ant);
    ant_tx_cfg(tx_ant0);
    ant_rx_cfg(rx_auto, rx_ant0, rx_ant1);
}

esp_err_t esp_phy_set_ant(esp_phy_ant_config_t *config)
{
    if (!config || (phy_ant_config_check(config) != ESP_OK)) {
        return ESP_ERR_INVALID_ARG;
    }

    memcpy(&s_phy_ant_config, config, sizeof(esp_phy_ant_config_t));
    if ( phy_get_modem_flag() == 0 ) {
        // Set flag and will be updated when PHY enable
        s_phy_ant_need_update_flag = true;
    } else {
        // Update immediately when PHY is enabled
        phy_ant_update();
    }

    return ESP_OK;
}

esp_err_t esp_phy_get_ant(esp_phy_ant_config_t *config)
{
    if (config == NULL) {
        ESP_LOGE(TAG, "Invalid args");
        return ESP_ERR_INVALID_ARG;
    }
    memcpy(config, &s_phy_ant_config, sizeof(esp_phy_ant_config_t));
    return ESP_OK;
}
