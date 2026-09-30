/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef __BLE_LOG_USB_TX_H__
#define __BLE_LOG_USB_TX_H__

/* Admit one sealed transport into the TinyUSB CDC TX FIFO, or drop it.
 * The USB peripheral copies then recycles; this helper is the decision
 * only, so it can be tested without the device stack. */

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* Host matching contract. TinyUSB CDC-only defaults to 0x303A:0x4001
 * (class bitmap 0x4000..0x4007), which is also the blbm P4 bridge.
 * 0x10B1 sits next to USB-Serial-JTAG 0x1001, outside both ranges. */
#define BLE_LOG_USB_VID          0x303A
#define BLE_LOG_USB_PID          0x10B1
#define BLE_LOG_USB_PRODUCT      "ESP-BLE-Log-Port"
#define BLE_LOG_USB_MANUFACTURER "Espressif Systems"

typedef enum {
    BLE_LOG_USB_TX_WRITE = 0,
    BLE_LOG_USB_TX_DROP_NOT_CONNECTED,
    BLE_LOG_USB_TX_DROP_FIFO_FULL,
} ble_log_usb_tx_action_t;

/* nbytes == 0 skips the bus (recycle only). A connected FIFO that cannot
 * hold the whole transport is DROP_FIFO_FULL; partial writes would tear
 * a BLE Log frame. */
static inline ble_log_usb_tx_action_t ble_log_usb_tx_action(
    bool connected, size_t available, size_t nbytes)
{
    if (nbytes == 0) {
        return BLE_LOG_USB_TX_WRITE;
    }
    if (!connected) {
        return BLE_LOG_USB_TX_DROP_NOT_CONNECTED;
    }
    if (available < nbytes) {
        return BLE_LOG_USB_TX_DROP_FIFO_FULL;
    }
    return BLE_LOG_USB_TX_WRITE;
}

static inline bool ble_log_usb_tx_should_warn(ble_log_usb_tx_action_t action)
{
    return action == BLE_LOG_USB_TX_DROP_FIFO_FULL;
}

#endif /* __BLE_LOG_USB_TX_H__ */
