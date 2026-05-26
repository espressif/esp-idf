/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "sdkconfig.h"
#include "esp_bt.h"
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

#if CONFIG_BT_CTRL_HCI_MODE_UART_H4 && CONFIG_BT_CTRL_HCI_UART_INIT_BY_CONTROLLER

/**
 * @brief Install the controller owned HCI UART(H4) transport
 *
 * Configures the HCI UART port and pins, creates the UHCI controller and starts the
 * TX/RX process tasks.
 *
 * @return
 *      - ESP_OK: transport installed
 *      - others: see the underlying UART/UHCI errors
 */
esp_err_t btdm_hci_uart_tl_install(void);

/**
 * @brief Release everything btdm_hci_uart_tl_install() took
 *
 * Does nothing if the transport is not installed.
 */
void btdm_hci_uart_tl_uninstall(void);

/**
 * @brief Get the HCI transport callbacks of the installed transport
 *
 * The returned pointer is meant for `esp_bt_controller_config_t.hci_tl_funcs`.
 */
esp_bt_hci_tl_t *btdm_hci_uart_tl_get_funcs(void);

#endif /* CONFIG_BT_CTRL_HCI_MODE_UART_H4 && CONFIG_BT_CTRL_HCI_UART_INIT_BY_CONTROLLER */

#ifdef __cplusplus
}
#endif
