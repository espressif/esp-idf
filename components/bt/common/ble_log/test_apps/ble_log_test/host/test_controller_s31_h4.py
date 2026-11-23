# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
"""Test BTDM lifecycle integration for S31/H4."""

import itertools

from lifecycle_test_utils import BT
from lifecycle_test_utils import extract
from lifecycle_test_utils import run_c
from test_log_stop import HELPERS
from test_log_stop import PRELUDE
from test_log_stop import macros
from test_log_stop import production

BTDM_PRELUDE = r"""#define CONFIG_BT_LE_CONTROLLER_LOG_ENABLED 1
#define CONFIG_BT_LE_CONTROLLER_LOG_MODE_BLE_LOG_V2 1
#define ESP_BT_CONTROLLER_STATUS_ENABLED 2
#define ESP_BT_CONTROLLER_STATUS_INITED 1
static bool log_is_inited = true;
static int shutdowns, unregisters;
static int s_btdm_controller_status = ESP_BT_CONTROLLER_STATUS_ENABLED;
static int deleted;
static void r_btdm_task_disable(void) {
#if CONFIG_BT_LE_CONTROLLER_LOG_ENABLED && CONFIG_BT_LE_CONTROLLER_LOG_MODE_BLE_LOG_V2
    assert(!lbm_enabled && !lbm_ref_count && !g_pool.waiting_task_count);
#endif
    ++deleted;
}
static void btdm_coex_disable(void) {}
static void btdm_lp_reset(bool enable) { assert(!enable); }
static void r_btdm_task_shutdown(void) {
#if CONFIG_BT_LE_CONTROLLER_LOG_ENABLED && CONFIG_BT_LE_CONTROLLER_LOG_MODE_BLE_LOG_V2
    assert(!lbm_enabled && !lbm_ref_count && !g_pool.waiting_task_count);
#endif
    ++shutdowns;
}
static void btdm_lp_shutdown(void) { assert(shutdowns); }
void bt_shutdown(void);
static esp_err_t esp_unregister_shutdown_handler(void (*handler)(void)) {
    assert(handler == bt_shutdown); ++unregisters; return ESP_OK;
}
"""
TESTS = r"""
static void reset_controller(void) {
    reset();
    deleted = shutdowns = unregisters = 0;
    log_is_inited = true;
    s_btdm_controller_status = ESP_BT_CONTROLLER_STATUS_ENABLED;
}
int main(void) {
    reset_controller();
    lbm_inited = lbm_enabled = 0;
    ble_log_pause();
    assert(!lbm_ref_count && !ticks && !wakes);
    /* A completed direct BLE Log deinit leaves the controller's flag set. */
    assert(log_is_inited);
    assert(esp_bt_controller_disable() == ESP_OK);
    assert(deleted == 1 && unregisters == 1 && !lbm_enabled);
    assert(s_btdm_controller_status == ESP_BT_CONTROLLER_STATUS_INITED);

    /* Model a compiled-out BLE_LOG_ASSERT: never delete an unfinished writer. */
    reset_controller(); g_pool.waiting_task_count = 1; ignore_pause_assert = true;
    on_delay = finish_after_assert_disabled;
    assert(esp_bt_controller_disable() == ESP_OK);
    assert(ticks == pdMS_TO_TICKS(1500) && pause_asserts == 1 && deleted == 1 && !lbm_enabled);

    reset_controller(); lbm_ref_count = 1;
    expect_pause_assert = true;
    if (setjmp(pause_assert_env) == 0) {
        (void)esp_bt_controller_disable();
        assert(!"disable returned instead of asserting");
    }
    assert(ticks == pdMS_TO_TICKS(1000) && pause_asserts == 1 && !deleted && !lbm_enabled);
    assert(s_btdm_controller_status == ESP_BT_CONTROLLER_STATUS_ENABLED);
    lbm_ref_count = 0;
    assert(esp_bt_controller_disable() == ESP_OK);
    assert(deleted == 1 && unregisters == 1 && !lbm_enabled);
    /* A task still switching out must not re-enter logging after disable. */
    check_entry = true;
    assert(!ble_log_writer_acquire() && !lbm_ref_count && !lock_depth);
    check_entry = false;
    ble_log_flush();
    assert(!lbm_enabled && !lbm_ref_count);

    reset_controller(); g_pool.waiting_task_count = 1; on_delay = finish_writer;
    assert(esp_bt_controller_disable() == ESP_OK);
    assert(deleted && !lbm_enabled && ticks > 0);

    reset_controller(); lbm_enabled = 0;
    assert(esp_bt_controller_disable() == ESP_OK);
    assert(deleted && !lbm_enabled);

    /* The public API still rejects disable before enable without touching logs. */
    reset_controller(); s_btdm_controller_status = ESP_BT_CONTROLLER_STATUS_INITED;
    assert(esp_bt_controller_disable() == ESP_ERR_INVALID_STATE);
    assert(lbm_enabled && !deleted && !unregisters && !ticks && !wakes);

    /* Internal enable-failure cleanup must drain writers even with a closed gate. */
    for (unsigned enabled = 0; enabled < 2; ++enabled) {
        reset_controller(); lbm_enabled = enabled; g_pool.waiting_task_count = 1;
        s_btdm_controller_status = ESP_BT_CONTROLLER_STATUS_INITED;
        on_delay = finish_writer;
        bt_controller_disable();
        assert(!lbm_enabled && !g_pool.waiting_task_count && !lbm_ref_count);
        assert(deleted == 1 && unregisters == 1 && ticks > 0);
    }

    /* An uninitialized log module is a no-op during internal cleanup. */
    reset_controller(); log_is_inited = false; lbm_inited = lbm_enabled = 0;
    s_btdm_controller_status = ESP_BT_CONTROLLER_STATUS_INITED;
    bt_controller_disable();
    assert(deleted == 1 && unregisters == 1 && !ticks && !wakes);
    log_is_inited = true;

    /* Both dynamic handler orders must flush queued frames and reject new writers. */
    for (int log_first = 0; log_first < 2; ++log_first) {
        reset_controller(); queued_frames = 3; g_pool.waiting_task_count = 1;
        on_delay = finish_shutdown_waiter;
        if (log_first) ble_log_shutdown_handler(); else bt_shutdown();
        assert(!lbm_enabled && !lbm_ref_count && !g_pool.waiting_task_count && ticks > 0);
        assert(queued_frames == (log_first ? 0U : 3U));
        check_entry = true;
        assert(!ble_log_writer_acquire() && !lbm_ref_count && !lock_depth);
        check_entry = false;
        if (log_first) bt_shutdown(); else ble_log_shutdown_handler();
        assert(shutdowns == 1 && !queued_frames && flushed_frames == 3);
        assert(!lbm_enabled && !lbm_ref_count);
        assert(s_btdm_controller_status == ESP_BT_CONTROLLER_STATUS_INITED);
    }
    reset_controller(); flush_success = false; queued_frames = 3;
    ble_log_shutdown_handler();
    assert(!lbm_enabled && queued_frames == 3 && !flushed_frames);
    bt_shutdown(); assert(shutdowns == 1);

    reset_controller(); lbm_ref_count = 1;
    expect_pause_assert = true;
    if (setjmp(pause_assert_env) == 0) {
        bt_shutdown();
        assert(!"shutdown returned instead of asserting");
    }
    assert(ticks == pdMS_TO_TICKS(1000) && pause_asserts == 1 && !lbm_enabled && !shutdowns);
    assert(s_btdm_controller_status == ESP_BT_CONTROLLER_STATUS_ENABLED);
    reset_controller(); s_btdm_controller_status = ESP_BT_CONTROLLER_STATUS_INITED;
    bt_shutdown(); assert(!shutdowns && lbm_enabled && !ticks);

    puts("PASS: BTDM disable/shutdown lifecycle");
}
"""
WITHOUT_LOG_TESTS = r"""
int main(void) {
    assert(log_is_inited);
    lbm_ref_count = 1; g_pool.waiting_task_count = 1;
    bt_shutdown();
    assert(shutdowns == 1 && lbm_enabled && !ticks && !wakes);
    assert(s_btdm_controller_status == ESP_BT_CONTROLLER_STATUS_INITED);
    bt_shutdown(); assert(shutdowns == 1);
    s_btdm_controller_status = ESP_BT_CONTROLLER_STATUS_ENABLED;
    assert(esp_bt_controller_disable() == ESP_OK);
    assert(deleted == 1 && unregisters == 1 && lbm_enabled && !ticks && !wakes);
    assert(lbm_ref_count == 1 && g_pool.waiting_task_count == 1);
    puts("PASS: disable/shutdown without controller BLE Log v2");
}
"""

if __name__ == '__main__':
    wrapper = extract(BT / 'porting_btdm/controller/btdm_common/src/btdm_log.c', [('btdm_log_pause', 'void')])
    for chip in ('esp32s31', 'esp32h4'):
        controller = extract(
            BT / f'controller/{chip}/bt.c',
            [('bt_shutdown', 'void'), ('bt_controller_disable', 'void'), ('esp_bt_controller_disable', 'esp_err_t')],
        )
        for tick_hz, (log_enabled, log_v2) in itertools.product((100, 1000), ((1, 1), (0, 0), (1, 0))):
            bt_prelude = BTDM_PRELUDE.replace(
                '#define CONFIG_BT_LE_CONTROLLER_LOG_ENABLED 1',
                f'#define CONFIG_BT_LE_CONTROLLER_LOG_ENABLED {log_enabled}',
            ).replace(
                '#define CONFIG_BT_LE_CONTROLLER_LOG_MODE_BLE_LOG_V2 1',
                f'#define CONFIG_BT_LE_CONTROLLER_LOG_MODE_BLE_LOG_V2 {log_v2}',
            )
            run_c(
                f'#define configTICK_RATE_HZ {tick_hz}\n'
                + PRELUDE
                + bt_prelude
                + macros
                + production
                + wrapper
                + controller
                + HELPERS
                + (TESTS if log_enabled and log_v2 else WITHOUT_LOG_TESTS)
            )
            print(f'PASS: {chip}, tick_hz={tick_hz}, controller_log={log_enabled}, BLE_LOG_V2={log_v2}')
