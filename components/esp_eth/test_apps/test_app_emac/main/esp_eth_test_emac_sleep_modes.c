/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdlib.h>
#include "esp_eth_mac_esp.h"
#include "esp_pm.h"
#include "esp_private/sleep_retention.h"
#include "unity.h"

#if CONFIG_ETH_USE_ESP32_EMAC

TEST_CASE("internal emac rejects invalid sleep configuration", "[esp_emac][sleep_mode][skip_setup_teardown]")
{
    eth_mac_config_t mac_config = ETH_MAC_DEFAULT_CONFIG();
    const eth_esp32_emac_config_t default_emac_config = ETH_ESP32_EMAC_DEFAULT_CONFIG();
    eth_esp32_emac_config_t emac_config = default_emac_config;
    TEST_ASSERT_NULL(esp_eth_mac_new_esp32(NULL, &mac_config));
    TEST_ASSERT_NULL(esp_eth_mac_new_esp32(&emac_config, NULL));
    emac_config.sleep_mode = (emac_sleep_mode_t)-1;
    TEST_ASSERT_NULL(esp_eth_mac_new_esp32(&emac_config, &mac_config));
    emac_config.sleep_mode = (emac_sleep_mode_t)(EMAC_SLEEP_MODE_ALLOW_PD + 1);
    TEST_ASSERT_NULL(esp_eth_mac_new_esp32(&emac_config, &mac_config));

    // WoL needs an SoC wakeup route; only ESP32-S31 with retention support provides one.
    emac_config.sleep_mode = EMAC_SLEEP_MODE_MAC_WAKE_ON_LAN;
#if CONFIG_PM_POWER_DOWN_PERIPHERAL_IN_LIGHT_SLEEP && SOC_EMAC_SUPPORT_SLEEP_RETENTION && SOC_PM_SUPPORT_EMAC_WAKEUP
    {
        esp_eth_mac_t *wol_mac = esp_eth_mac_new_esp32(&emac_config, &mac_config);
        TEST_ASSERT_NOT_NULL(wol_mac);
        TEST_ESP_OK(wol_mac->del(wol_mac));
    }
    // An ESP-generated RMII clock comes from a PLL that is powered down in Light-sleep.
    emac_config = default_emac_config;
    emac_config.sleep_mode = EMAC_SLEEP_MODE_MAC_WAKE_ON_LAN;
    emac_config.interface = EMAC_DATA_INTERFACE_RMII;
    emac_config.clock_config.rmii.clock_mode = EMAC_CLK_OUT;
    TEST_ASSERT_NULL(esp_eth_mac_new_esp32(&emac_config, &mac_config));
#else
    TEST_ASSERT_NULL(esp_eth_mac_new_esp32(&emac_config, &mac_config));
#endif
#if CONFIG_PM_POWER_DOWN_PERIPHERAL_IN_LIGHT_SLEEP && SOC_EMAC_SUPPORT_SLEEP_RETENTION
    emac_config = default_emac_config;
    emac_config.sleep_mode = EMAC_SLEEP_MODE_ALLOW_PD;
    emac_config.interface = EMAC_DATA_INTERFACE_MII;
    TEST_ASSERT_NULL(esp_eth_mac_new_esp32(&emac_config, &mac_config));
    emac_config = default_emac_config;
    emac_config.sleep_mode = EMAC_SLEEP_MODE_ALLOW_PD;
    emac_config.interface = EMAC_DATA_INTERFACE_RMII;
    emac_config.clock_config.rmii.clock_mode = EMAC_CLK_OUT;
    TEST_ASSERT_NULL(esp_eth_mac_new_esp32(&emac_config, &mac_config));
#else
    emac_config.sleep_mode = EMAC_SLEEP_MODE_ALLOW_PD;
    TEST_ASSERT_NULL(esp_eth_mac_new_esp32(&emac_config, &mac_config));
#endif
}

#if CONFIG_PM_POWER_DOWN_PERIPHERAL_IN_LIGHT_SLEEP && SOC_EMAC_SUPPORT_SLEEP_RETENTION

static bool s_reject_init;

static esp_err_t sleep_test_state_changed(esp_eth_mediator_t *eth, esp_eth_state_t state, void *args)
{
    return s_reject_init && state == ETH_STATE_LLINIT ? ESP_FAIL : ESP_OK;
}

static esp_err_t sleep_test_input(esp_eth_mediator_t *eth, uint8_t *buffer, uint32_t length, void *info)
{
    free(buffer);
    return ESP_OK;
}

static void test_sleep_mode_lifecycle(emac_sleep_mode_t mode)
{
    eth_mac_config_t mac_config = ETH_MAC_DEFAULT_CONFIG();
    eth_esp32_emac_config_t emac_config = ETH_ESP32_EMAC_DEFAULT_CONFIG();
    emac_config.sleep_mode = mode;
#ifdef CONFIG_PM_ENABLE
    esp_pm_lock_stats_t before[ESP_PM_LOCK_MAX];
    esp_pm_lock_stats_t after[ESP_PM_LOCK_MAX];
    TEST_ESP_OK(esp_pm_get_lock_stats_all(before));
#endif
    esp_eth_mac_t *mac = esp_eth_mac_new_esp32(&emac_config, &mac_config);
    TEST_ASSERT_NOT_NULL(mac);
    // Retention is allocated and attached with the MAC object. init/deinit do not own it.
    bool retain = (mode == EMAC_SLEEP_MODE_ALLOW_PD || mode == EMAC_SLEEP_MODE_MAC_WAKE_ON_LAN);
    TEST_ASSERT_EQUAL(retain, sleep_retention_is_module_inited(SLEEP_RETENTION_MODULE_EMAC));
    TEST_ASSERT_EQUAL(retain, sleep_retention_is_module_created(SLEEP_RETENTION_MODULE_EMAC));
    TEST_ASSERT_EQUAL(retain, sleep_retention_is_module_attached(SLEEP_RETENTION_MODULE_EMAC));

    // Force init to fail, to see the retention state after the failure.
    esp_eth_mediator_t mediator = {
        .on_state_changed = sleep_test_state_changed,
        .stack_input_info = sleep_test_input,
    };
    TEST_ESP_OK(mac->set_mediator(mac, &mediator));
    s_reject_init = true;
    TEST_ASSERT_EQUAL(ESP_FAIL, mac->init(mac));
    s_reject_init = false;
    TEST_ASSERT_EQUAL(retain, sleep_retention_is_module_created(SLEEP_RETENTION_MODULE_EMAC));
    TEST_ASSERT_EQUAL(retain, sleep_retention_is_module_attached(SLEEP_RETENTION_MODULE_EMAC));
#ifdef CONFIG_PM_ENABLE
    TEST_ESP_OK(esp_pm_get_lock_stats_all(after));
    TEST_ASSERT_EQUAL(before[ESP_PM_APB_FREQ_MAX].acquired, after[ESP_PM_APB_FREQ_MAX].acquired);
#endif
    // Second pass runs after deinit: init must work again and the PM lock must be back at the baseline.
    for (int i = 0; i < 2; i++) {
        TEST_ESP_OK(mac->init(mac));
        TEST_ASSERT_EQUAL(retain, sleep_retention_is_module_created(SLEEP_RETENTION_MODULE_EMAC));
        TEST_ASSERT_EQUAL(retain, sleep_retention_is_module_attached(SLEEP_RETENTION_MODULE_EMAC));
#ifdef CONFIG_PM_ENABLE
        TEST_ESP_OK(esp_pm_get_lock_stats_all(after));
        TEST_ASSERT_EQUAL(before[ESP_PM_APB_FREQ_MAX].acquired, after[ESP_PM_APB_FREQ_MAX].acquired);
#endif

        // Both modes must support link start/stop, including when no restart nodes exist.
        // start/stop are also invoked on link up/down, so the PM lock must be idempotent.
        TEST_ESP_OK(mac->start(mac));
#ifdef CONFIG_PM_ENABLE
        TEST_ESP_OK(esp_pm_get_lock_stats_all(after));
        TEST_ASSERT_EQUAL(before[ESP_PM_APB_FREQ_MAX].acquired + 1, after[ESP_PM_APB_FREQ_MAX].acquired);
#endif
        TEST_ESP_OK(mac->start(mac));
#ifdef CONFIG_PM_ENABLE
        TEST_ESP_OK(esp_pm_get_lock_stats_all(after));
        TEST_ASSERT_EQUAL(before[ESP_PM_APB_FREQ_MAX].acquired + 1, after[ESP_PM_APB_FREQ_MAX].acquired);
#endif
        TEST_ESP_OK(mac->stop(mac));
#ifdef CONFIG_PM_ENABLE
        TEST_ESP_OK(esp_pm_get_lock_stats_all(after));
        TEST_ASSERT_EQUAL(before[ESP_PM_APB_FREQ_MAX].acquired, after[ESP_PM_APB_FREQ_MAX].acquired);
#endif
        TEST_ESP_OK(mac->stop(mac));
#ifdef CONFIG_PM_ENABLE
        TEST_ESP_OK(esp_pm_get_lock_stats_all(after));
        TEST_ASSERT_EQUAL(before[ESP_PM_APB_FREQ_MAX].acquired, after[ESP_PM_APB_FREQ_MAX].acquired);
#endif
        TEST_ESP_OK(mac->deinit(mac));
        TEST_ASSERT_EQUAL(retain, sleep_retention_is_module_created(SLEEP_RETENTION_MODULE_EMAC));
        TEST_ASSERT_EQUAL(retain, sleep_retention_is_module_attached(SLEEP_RETENTION_MODULE_EMAC));
#ifdef CONFIG_PM_ENABLE
        TEST_ESP_OK(esp_pm_get_lock_stats_all(after));
        TEST_ASSERT_EQUAL(before[ESP_PM_APB_FREQ_MAX].acquired, after[ESP_PM_APB_FREQ_MAX].acquired);
#endif
    }
#ifdef CONFIG_PM_ENABLE
    TEST_ESP_OK(mac->init(mac));
    TEST_ESP_OK(mac->start(mac));
    TEST_ESP_OK(esp_pm_get_lock_stats_all(after));
    TEST_ASSERT_EQUAL(before[ESP_PM_APB_FREQ_MAX].acquired + 1, after[ESP_PM_APB_FREQ_MAX].acquired);
    TEST_ESP_OK(mac->deinit(mac));
    TEST_ESP_OK(esp_pm_get_lock_stats_all(after));
    TEST_ASSERT_EQUAL(before[ESP_PM_APB_FREQ_MAX].acquired, after[ESP_PM_APB_FREQ_MAX].acquired);
#endif
    TEST_ESP_OK(mac->del(mac));
    TEST_ASSERT_FALSE(sleep_retention_is_module_inited(SLEEP_RETENTION_MODULE_EMAC));
    TEST_ASSERT_FALSE(sleep_retention_is_module_created(SLEEP_RETENTION_MODULE_EMAC));
    TEST_ASSERT_FALSE(sleep_retention_is_module_attached(SLEEP_RETENTION_MODULE_EMAC));
#ifdef CONFIG_PM_ENABLE
    TEST_ESP_OK(esp_pm_get_lock_stats_all(after));
    TEST_ASSERT_EQUAL(before[ESP_PM_APB_FREQ_MAX].created, after[ESP_PM_APB_FREQ_MAX].created);
#endif
}

TEST_CASE("internal emac no-sleep lifecycle without retention", "[esp_emac][sleep_mode][skip_setup_teardown]")
{
    test_sleep_mode_lifecycle(EMAC_SLEEP_MODE_NO_SLEEP);
}

TEST_CASE("internal emac allow-pd retention lifecycle", "[esp_emac][sleep_mode][skip_setup_teardown]")
{
    eth_esp32_emac_config_t emac_config = ETH_ESP32_EMAC_DEFAULT_CONFIG();
    // ALLOW_PD is legal only for RMII with an external reference clock.
    if (emac_config.interface == EMAC_DATA_INTERFACE_RMII &&
            emac_config.clock_config.rmii.clock_mode == EMAC_CLK_EXT_IN) {
        test_sleep_mode_lifecycle(EMAC_SLEEP_MODE_ALLOW_PD);
    } else {
        eth_mac_config_t mac_config = ETH_MAC_DEFAULT_CONFIG();
        emac_config.sleep_mode = EMAC_SLEEP_MODE_ALLOW_PD;
        TEST_ASSERT_NULL(esp_eth_mac_new_esp32(&emac_config, &mac_config));
    }
}

TEST_CASE("internal emac wake-on-lan retention lifecycle", "[esp_emac][sleep_mode][skip_setup_teardown]")
{
#if SOC_PM_SUPPORT_EMAC_WAKEUP
    test_sleep_mode_lifecycle(EMAC_SLEEP_MODE_MAC_WAKE_ON_LAN);
#else
    TEST_IGNORE_MESSAGE("EMAC wakeup not supported for this target");
#endif // SOC_PM_SUPPORT_EMAC_WAKEUP
}

#endif // CONFIG_PM_POWER_DOWN_PERIPHERAL_IN_LIGHT_SLEEP && SOC_EMAC_SUPPORT_SLEEP_RETENTION
#endif // CONFIG_ETH_USE_ESP32_EMAC
