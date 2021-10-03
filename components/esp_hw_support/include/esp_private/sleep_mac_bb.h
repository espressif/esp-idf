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
 * @file sleep_mac_bb.h
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

#ifdef __cplusplus
}
#endif
