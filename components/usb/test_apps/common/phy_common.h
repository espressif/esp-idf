/*
 * SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once
#include "hal/usb_phy_types.h"

/**
 * @brief Install USB PHY separately from the usb_host_install()
 *
 * @param[in] phy_target USB PHY target
 */
void test_setup_usb_phy(usb_phy_target_t phy_target);

/**
 * @brief Uninstall PHY
 */
void test_delete_usb_phy(void);
