/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdbool.h>
#include "freertos/FreeRTOS.h"

/**
 * @brief Initialize the internal USB PHY and USB Controller for USB Host testing
 */
void test_usb_init_phy(void);

/**
 * @brief Deinitialize the internal USB PHY and USB Controller after USB Host testing
 */
void test_usb_deinit_phy(void);

/**
 * @brief For the USB PHY into the connected or disconnected state
 *
 * @param connected For into connected state if true, disconnected if false
 * @param delay_ticks Delay in ticks before forcing state
 */
void test_usb_set_phy_state(bool connected, TickType_t delay_ticks);
