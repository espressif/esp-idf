# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
"""Shared disable-contract model; callers select only their own controller sources."""

from lifecycle_test_utils import BT
from lifecycle_test_utils import extract
from lifecycle_test_utils import run_c

PRELUDE = r"""
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
typedef int esp_err_t;
#define ESP_OK 0
#define ESP_FAIL -1
#define ESP_ERR_INVALID_STATE 2
#define ESP_BT_CONTROLLER_STATUS_ENABLED 2
#define ESP_BT_CONTROLLER_STATUS_INITED 1
#define PHY_MODEM_BT 0
#define ESP_LOGW(...) ((void)0)
static int ble_controller_status, disable_result, pauses, deletes, cleaned;
static bool log_is_inited, gate, s_ble_active;
static void ble_log_pause(void) {
    ++pauses; gate = false;
}
static int r_ble_controller_disable(void) {
    if (CONFIG_BT_LE_CONTROLLER_LOG_MODE_BLE_LOG_V2 && log_is_inited) assert(!gate && pauses == 1);
    ++deletes; return disable_result;
}
static void ble_controller_disable(void) { (void)r_ble_controller_disable(); }
static void ble_stack_disable(void) { ++cleaned; }
static void esp_btbb_disable(void) {}
static void esp_phy_disable(int mode) { (void)mode; }
static void reset_controller(void) {
    ble_controller_status = ESP_BT_CONTROLLER_STATUS_ENABLED;
    log_is_inited = gate = s_ble_active = true;
    disable_result = pauses = deletes = cleaned = 0;
}
"""
TESTS = r"""
int main(void) {
    for (int enabled = 0; enabled < 2; ++enabled) {
        reset_controller(); gate = enabled;
        assert(esp_bt_controller_disable() == ESP_OK);
        assert(deletes == 1 && cleaned == 1 && !s_ble_active);
        assert(gate == (CONFIG_BT_LE_CONTROLLER_LOG_MODE_BLE_LOG_V2 ? false : enabled));
        assert(ble_controller_status == ESP_BT_CONTROLLER_STATUS_INITED);
        assert(pauses == CONFIG_BT_LE_CONTROLLER_LOG_MODE_BLE_LOG_V2);
    }
#ifndef TEST_C2
    for (int enabled = 0; enabled < 2; ++enabled) {
        reset_controller(); gate = enabled; disable_result = ESP_FAIL;
        assert(esp_bt_controller_disable() == ESP_FAIL);
        assert(deletes == 1 && !cleaned && s_ble_active);
        assert(gate == (CONFIG_BT_LE_CONTROLLER_LOG_MODE_BLE_LOG_V2 ? false : enabled));
        assert(ble_controller_status == ESP_BT_CONTROLLER_STATUS_ENABLED);
    }
#endif
    reset_controller(); log_is_inited = false;
    assert(esp_bt_controller_disable() == ESP_OK && !pauses);
    reset_controller(); ble_controller_status = ESP_BT_CONTROLLER_STATUS_INITED;
    assert(esp_bt_controller_disable() == ESP_FAIL && !pauses && !deletes);
}
"""


def run(chips: tuple[str, ...]) -> None:
    for chip in chips:
        controller = extract(BT / f'controller/{chip}/bt.c', [('esp_bt_controller_disable', 'esp_err_t')])
        for log_v2 in (0, 1):
            run_c(
                f'#define CONFIG_BT_LE_CONTROLLER_LOG_MODE_BLE_LOG_V2 {log_v2}\n'
                + ('#define TEST_C2 1\n' if chip == 'esp32c2' else '')
                + PRELUDE
                + controller
                + TESTS
            )
            print(f'PASS: {chip} disable, BLE_LOG_V2={log_v2}')
