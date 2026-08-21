# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
#
# SPDX-License-Identifier: Apache-2.0

# BLE Log Functional Test

| Supported Targets |
| ----------------- |

This test app verifies the BLE Log runtime behaviour on target, using the
in-memory test peripheral (`CONFIG_BLE_LOG_PRPH_TEST=y`) to capture the
transport stream written by the runtime task hook.

Currently covered:

- `BLE_LOG_INT_SRC_VERSION_INFO` frame: BLE Log version, ESP-IDF build commit,
  controller lib commit, btdm_common lib commit, BLE Mesh and BLE Audio lib
  commits, chip model and chip revision
