/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once
#include <stdint.h>
#include "sdkconfig.h"

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
 * @brief Get WiFi modem state
 *
 * @return true or false for WiFi modem state is enabled or disabled
 */
bool sleep_modem_wifi_modem_state_enabled(void);

#endif /* SOC_PM_SUPPORT_PMU_MODEM_STATE */

/**
 * @brief Whether to allow the Modem or TOP power domain to be powered off.
 *
 * In light sleep mode, only when the system can provide enough memory
 * for modem (WiFi, Bluetooth, IEEE802.15.4) retention, the Modem or TOP
 * power domain can be powered off.
 */
bool modem_domain_pd_allowed(void);

/**
 * @brief Get the reject trigger signal of Modem system
 *
 * @return the reject trigger signal of Modem system.
 */
uint32_t sleep_modem_reject_triggers(void);

#ifdef __cplusplus
}
#endif
