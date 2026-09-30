/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "unity.h"

#include "ble_log_usb_tx.h"

TEST_CASE("USB TX admits a transport that fits a connected CDC FIFO",
          "[ble_log][usb_tx]")
{
    TEST_ASSERT_EQUAL(BLE_LOG_USB_TX_WRITE,
                      ble_log_usb_tx_action(true, 2048, 640));
    TEST_ASSERT_EQUAL(BLE_LOG_USB_TX_WRITE,
                      ble_log_usb_tx_action(true, 640, 640));
    TEST_ASSERT_FALSE(ble_log_usb_tx_should_warn(BLE_LOG_USB_TX_WRITE));
}

TEST_CASE("USB TX drops with a warning when the CDC FIFO cannot hold a frame",
          "[ble_log][usb_tx]")
{
    TEST_ASSERT_EQUAL(BLE_LOG_USB_TX_DROP_FIFO_FULL,
                      ble_log_usb_tx_action(true, 639, 640));
    TEST_ASSERT_EQUAL(BLE_LOG_USB_TX_DROP_FIFO_FULL,
                      ble_log_usb_tx_action(true, 0, 1));
    TEST_ASSERT_TRUE(ble_log_usb_tx_should_warn(BLE_LOG_USB_TX_DROP_FIFO_FULL));
}

TEST_CASE("USB TX drops silently when CDC is not connected",
          "[ble_log][usb_tx]")
{
    TEST_ASSERT_EQUAL(BLE_LOG_USB_TX_DROP_NOT_CONNECTED,
                      ble_log_usb_tx_action(false, 2048, 640));
    TEST_ASSERT_FALSE(
        ble_log_usb_tx_should_warn(BLE_LOG_USB_TX_DROP_NOT_CONNECTED));
}

TEST_CASE("USB TX treats an empty transport as a recycle with no bus traffic",
          "[ble_log][usb_tx]")
{
    TEST_ASSERT_EQUAL(BLE_LOG_USB_TX_WRITE,
                      ble_log_usb_tx_action(false, 0, 0));
    TEST_ASSERT_EQUAL(BLE_LOG_USB_TX_WRITE,
                      ble_log_usb_tx_action(true, 0, 0));
}

TEST_CASE("USB identity is distinct from TinyUSB CDC default and blbm bridge",
          "[ble_log][usb_tx]")
{
    TEST_ASSERT_EQUAL_HEX16(0x303A, BLE_LOG_USB_VID);
    TEST_ASSERT_EQUAL_HEX16(0x10B1, BLE_LOG_USB_PID);
    TEST_ASSERT_TRUE(BLE_LOG_USB_PID < 0x4000 || BLE_LOG_USB_PID > 0x4007);
    TEST_ASSERT_EQUAL_STRING("ESP-BLE-Log-Port", BLE_LOG_USB_PRODUCT);
}
