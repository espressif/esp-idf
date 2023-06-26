/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once
#include <stdint.h>
#include <stdbool.h>
#include "sdkconfig.h"
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @file sleep_modem.h
 *
 * This file contains declarations of MAC and baseband power consumption related functions in light sleep mode.
 */

#if CONFIG_MAC_BB_PD

/**
 * @brief A callback function completes MAC and baseband power down operation
 *
 * In light sleep mode, execute Wi-Fi and Bluetooth module MAC and baseband
 * power down and backup register configuration information operations.
 */
void mac_bb_power_down_cb_execute(void);

/**
 * @brief A callback function completes MAC and baseband power up operation
 *
 * In light sleep mode, execute Wi-Fi and Bluetooth module MAC and baseband
 * power up and restore register configuration information operations.
 */
void mac_bb_power_up_cb_execute(void);

#endif // CONFIG_MAC_BB_PD

#if SOC_PM_SUPPORT_PMU_MODEM_STATE

/**
 * @brief The retention action in the modem state of WiFi PHY module
 *
 * @param restore  true for restore the PHY context, false for backup the PHY context
 */
void sleep_modem_wifi_do_phy_retention(bool restore);

/**
 * @brief Get WiFi modem state
 *
 * @return true or false for WiFi modem state is enabled or disabled
 */
bool sleep_modem_wifi_modem_state_enabled(void);

#endif /* SOC_PM_SUPPORT_PMU_MODEM_STATE */

/**
 * @brief Whether the current target allows Modem or the TOP power domain to be powered off during light sleep
 *
 * During light sleep on some targets, it is possible to power OFF the Modem or TOP
 * power domains in order to further lower power power consumption. However, this
 * can only occur on targets that support REGDMA for modem (WiFi, Bluetooth,
 * IEEE802.15.4) retention.
 */
bool modem_domain_pd_allowed(void);

/**
 * @brief Get the reject trigger signal of Modem system
 *
 * @return the reject trigger signal of Modem system.
 */
uint32_t sleep_modem_reject_triggers(void);

/**
 * @brief Configure the parameters of the modem subsytem during the sleep process
 *
 * In light sleep mode, the wake-up early time of the WiFi module and the TBTT
 * interrupt early time (trigger enabling RF) are determined by the maximum and
 * minimum frequency of system (higher system frequency means less time to wake
 * up and enable RF).
 * For the esp32c6 SOC, the modem state is strongly dependent on the light sleep
 * mode, and the modem state will be enabled only when light sleep is enabled
 * and the `CONFIG_ESP_WIFI_ENHANCED_LIGHT_SLEEP` is configured in menuconfig.
 *
 * @param max_freq_mhz       the maximum frequency of system
 * @param min_freq_mhz       the minimum frequency of system
 * @param light_sleep_enable ture or false for enable or disable light sleep mode, respectively
 *
 * @return
 *  - ESP_OK on success
 */
esp_err_t sleep_modem_configure(int max_freq_mhz, int min_freq_mhz, bool light_sleep_enable);

/**
 * @brief Callback function type for peripherals to know light sleep wakeup overhead.
 *
 */
typedef void (* inform_out_light_sleep_overhead_cb_t)(uint32_t);

/**
  * @brief  Register informing peripherals light sleep wakeup overhead time callback
  *
  * This function allows you to register a callback that informs the peripherals of
  * the wakeup overhead time of light sleep.
  * @param cb function to inform time
  * @return
  *   - ESP_OK on success
  *   - ESP_ERR_NO_MEM if no more callback slots are available
  */
esp_err_t esp_pm_register_inform_out_light_sleep_overhead_callback(inform_out_light_sleep_overhead_cb_t cb);

/**
  * @brief  Unregister informing peripherals light sleep wakeup overhead time callback
  *
  * This function allows you to unregister a callback that informs the peripherals of
  * the wakeup overhead time of light sleep.
  * @param cb function to inform time
  * @return
  *   - ESP_OK on success
  *   - ESP_ERR_INVALID_STATE if the given callback hasn't been registered before
  */
esp_err_t esp_pm_unregister_inform_out_light_sleep_overhead_callback(inform_out_light_sleep_overhead_cb_t cb);

/**
  * @brief  A callback that informs the peripherals of the wakeup overhead time of light sleep
  *
  * @param out_light_sleep_time wakeup overhead time of light sleep
  */
void periph_inform_out_light_sleep_overhead(uint32_t out_light_sleep_time);

/**
 * @brief Callback function type for peripherals to know light sleep default parameters
 */
typedef void (* update_light_sleep_default_params_config_cb_t)(int, int);

/**
 * @brief  Register peripherals light sleep default parameters configure callback
 *
 * This function allows you to register a callback that configure the peripherals
 * of default parameters of light sleep
 * @param cb function to update default parameters
 */
void esp_pm_register_light_sleep_default_params_config_callback(update_light_sleep_default_params_config_cb_t cb);

/**
 * @brief  Unregister peripherals light sleep default parameters configure Callback
 *
 * This function allows you to unregister a callback that configure the peripherals
 * of default parameters of light sleep
 */
void esp_pm_unregister_light_sleep_default_params_config_callback(void);

#ifdef __cplusplus
}
#endif
