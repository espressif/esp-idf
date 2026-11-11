# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
"""Test BLE Log itself without loading any controller source."""

import re

from lifecycle_test_utils import BT
from lifecycle_test_utils import extract
from lifecycle_test_utils import run_c

lbm = BT / 'common/ble_log/src/ble_log_lbm_v2.c'
util = (BT / 'common/ble_log/src/internal_include/ble_log_util.h').read_text()
macros = '\n'.join(
    line for line in util.splitlines() if line.startswith(('#define BLE_LOG_ATOMIC_', '#define BLE_LOG_REF_COUNT_'))
)
timeout_macro = re.search(r'^#define BLE_LOG_WAIT_TIMEOUT_TICKS .+$', lbm.read_text(), re.M)
assert timeout_macro, 'Missing production writer-drain timeout'
macros += '\n' + timeout_macro[0]
production = extract(
    lbm,
    [
        ('ble_log_lbm_disable', 'void'),
        ('ble_log_writer_acquire', 'bool'),
        ('ble_log_pool_wake_all', 'void'),
        ('ble_log_pool_waiter_adjust', 'void'),
        ('ble_log_pool_acquire', 'ble_log_prph_trans_t *'),
        ('ble_log_lbm_wait_writers', 'bool'),
        ('ble_log_lbm_begin_deinit', 'void'),
        ('ble_log_lbm_is_enabled', 'bool'),
        ('ble_log_enable', 'bool'),
        ('ble_log_pause', 'void'),
        ('ble_log_flush', 'void'),
    ],
)
production += extract(
    BT / 'common/ble_log/src/ble_log.c',
    [('ble_log_shutdown_handler', 'void')],
)
production = extract(BT / 'common/ble_log/src/ble_log_util.c', [('ble_log_ref_count_try_acquire', 'bool')]) + production

PRELUDE = r"""
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <setjmp.h>
#include <stdio.h>
#define CONFIG_BLE_LOG_PRPH_TEST 1
#define pdMS_TO_TICKS(ms) ((ms) * configTICK_RATE_HZ / 1000)
#define BLE_LOG_POOL_TRANS_SIZE 64
#define BLE_LOG_FRAME_OVERHEAD 8
#define BLE_LOG_POOL_WAIT_TICKS UINT32_MAX
#define BLE_LOG_SRC_MAX 1
#define BLE_LOG_TRANS_STATE_FREE 0
#define BLE_LOG_SNAPSHOT_REASON_FLUSH 2
#define portMAX_DELAY UINT32_MAX
#define taskSCHEDULER_RUNNING 1
#define ESP_OK 0
#define ESP_ERR_INVALID_STATE 2
#define BLE_LOG_ENTER_CRITICAL() (++lock_depth)
#define BLE_LOG_EXIT_CRITICAL() (--lock_depth)
#define ESP_ERROR_CHECK(x) assert((x) == ESP_OK)
#define BLE_LOG_CONSOLE(...) ((void)0)
#define ESP_LOGW(...) ((void)0)
#define UNUSED(x) ((void)(x))
typedef unsigned TickType_t;
typedef int TimeOut_t;
typedef int esp_err_t;
typedef int ble_log_prph_trans_t;
static unsigned lbm_enabled = 1, lbm_inited = 1, lbm_ref_count;
static struct { unsigned waiting_task_count; void *sem; unsigned inflight_peak; } g_pool = {0, (void *)1, 0};
static bool flush_in_progress;
static bool available, check_entry, flush_success = true;
static int lock_depth, ticks, wakes;
static unsigned queued_frames, flushed_frames;
static void (*on_delay)(void), (*on_take)(void);
static void (*ble_log_test_enable_before_lifecycle_lock_hook)(void);
static void (*ble_log_test_disable_before_wake_hook)(void);
static void (*ble_log_test_flush_drain_between_loads_hook)(void);
static void (*ble_log_test_acquire_after_unregister_hook)(void);
static jmp_buf pause_assert_env;
static bool expect_pause_assert, ignore_pause_assert;
static unsigned pause_asserts;
#define BLE_LOG_ASSERT(condition) do { \
    if (!(condition)) { \
        ++pause_asserts; \
        if (!ignore_pause_assert) { \
            assert(expect_pause_assert); longjmp(pause_assert_env, 1); \
        } \
    } \
} while (0)
static unsigned xTaskGetTickCount(void) { return ticks; }
static void vTaskDelay(int n) { ticks += n; if (on_delay) on_delay(); }
static int xSemaphoreGive(void *sem) { assert(sem); ++wakes; return 1; }
static int xSemaphoreTake(void *sem, unsigned t) {
    assert(sem && t && g_pool.waiting_task_count == 1 && lbm_ref_count == 0);
    assert(on_take); on_take(); return 1;
}
static void *xTaskGetCurrentTaskHandle(void) { return (void *)2; }
static void *esp_timer_impl_get_timer_task_handle(void) { return (void *)3; }
static void vTaskSetTimeOutState(TimeOut_t *t) { *t = 0; }
static int xTaskCheckForTimeOut(TimeOut_t *t, unsigned *r) { (void)t; (void)r; return 0; }
static ble_log_prph_trans_t *ble_log_pool_try_claim_available(unsigned n, bool reserve) {
    static ble_log_prph_trans_t trans;
    (void)n; (void)reserve; return available ? &trans : NULL;
}
typedef int ble_log_ts_info_t;
static struct { unsigned state; } transport, *internal_trans = &transport;
static struct { struct { unsigned written_frame_cnt, lost_frame_cnt, written_bytes_cnt; } counters; } stat_mgr_ctx[1];
static bool ble_log_pool_flush_all_trans(void) {
    assert(!lbm_enabled && lbm_ref_count == 1 && !g_pool.waiting_task_count);
    if (flush_success) { flushed_frames += queued_frames; queued_frames = 0; }
    return flush_success;
}
static void ble_log_sync_sample(ble_log_ts_info_t *info) { *info = 0; }
static bool ble_log_internal_snapshot(int reason, const ble_log_ts_info_t *info, bool wait) {
    (void)reason; (void)info; (void)wait; return true;
}
static bool ble_log_rt_drain(void) { return true; }
static void ble_log_task_registry_begin_deinit(void) {
    assert(!lbm_enabled && !lbm_inited && !lock_depth);
}
"""
macros += r"""
/* Model deletion at an entry reference increment: rejected writers must never
 * reach this point, and accepted writers must still hold the admission lock. */
#undef BLE_LOG_REF_COUNT_ACQUIRE_SEQ_CST
#define BLE_LOG_REF_COUNT_ACQUIRE_SEQ_CST(p) do { \
    if (check_entry) assert(lock_depth && lbm_enabled); \
    __atomic_fetch_add((p), 1, __ATOMIC_SEQ_CST); \
} while (0)
"""
macros += r"""
/* Gate closure must serialize with writer admission, including flush. */
#undef BLE_LOG_ATOMIC_STORE_SEQ_CST
#define BLE_LOG_ATOMIC_STORE_SEQ_CST(var, value) do { \
    if (&(var) == &lbm_enabled) assert(lock_depth); \
    __atomic_store_n(&(var), (value), __ATOMIC_SEQ_CST); \
} while (0)
"""
HELPERS = r"""
static void finish_writer(void) {
    assert(!lbm_enabled && wakes);
    lbm_ref_count = 0; g_pool.waiting_task_count = 0; on_delay = NULL;
}
static void finish_slow_writer(void) {
    assert(!lbm_enabled);
    if (ticks >= pdMS_TO_TICKS(500)) finish_writer();
}
static void finish_after_assert_disabled(void) {
    assert(!lbm_enabled);
    if (ticks >= pdMS_TO_TICKS(1500)) finish_writer();
}
static void finish_shutdown_waiter(void) {
    /* A parked writer exits; preserve the flush caller's own reference. */
    assert(!lbm_enabled && wakes && lbm_ref_count <= 1);
    g_pool.waiting_task_count = 0; on_delay = NULL;
}
static void close_while_parked(void) { assert(ble_log_enable(false)); }
static void recycle_while_parked(void) { available = true; }
static void straddle(void) {
    lbm_ref_count = 2; g_pool.waiting_task_count = 0;
    ble_log_test_flush_drain_between_loads_hook = NULL;
}
static void release_straddle(void) { lbm_ref_count = 1; on_delay = NULL; }
static void reset(void) {
    expect_pause_assert = ignore_pause_assert = false; pause_asserts = 0;
    lbm_enabled = lbm_inited = 1; lbm_ref_count = 0; g_pool.waiting_task_count = 0;
    ticks = wakes = 0; on_delay = on_take = NULL;
    queued_frames = flushed_frames = 0;
    available = check_entry = flush_in_progress = false; flush_success = true;
}
"""
TESTS = r"""
int main(void) {
    reset();
    lbm_inited = lbm_enabled = 0;
    ble_log_pause();
    assert(!lbm_ref_count && !ticks && !wakes);
    /* Repeated pause keeps admission closed. */
    reset(); ble_log_pause();
    assert(!lbm_enabled && !lbm_ref_count);
    ble_log_pause();
    assert(!lbm_enabled && !lbm_ref_count);

    /* Closing an already closed gate still has to drain outstanding writers. */
    for (unsigned enabled = 0; enabled < 2; ++enabled) {
        reset(); lbm_enabled = enabled; lbm_ref_count = 1;
        expect_pause_assert = true;
        if (setjmp(pause_assert_env) == 0) {
            ble_log_pause();
            assert(!"pause returned instead of asserting");
        }
        assert(ticks == pdMS_TO_TICKS(1000) && pause_asserts == 1);
        assert(!lbm_enabled && lbm_ref_count == 1 && !lock_depth);
    }

    reset(); g_pool.waiting_task_count = 1; on_delay = finish_slow_writer;
    ble_log_pause();
    assert(ticks == pdMS_TO_TICKS(500) && !pause_asserts && !lbm_enabled && wakes);

    /* Model a compiled-out BLE_LOG_ASSERT: never delete an unfinished writer. */
    reset(); g_pool.waiting_task_count = 1; ignore_pause_assert = true;
    on_delay = finish_after_assert_disabled;
    ble_log_pause();
    assert(ticks == pdMS_TO_TICKS(1500) && pause_asserts == 1 && !lbm_enabled);

    /* Deinit must close both gates and complete the same drain before returning. */
    reset(); lbm_ref_count = 1; g_pool.waiting_task_count = 1;
    on_delay = finish_slow_writer;
    ble_log_lbm_begin_deinit();
    assert(!lbm_enabled && !lbm_inited && !lbm_ref_count && !g_pool.waiting_task_count);
    assert(ticks == pdMS_TO_TICKS(500) && !pause_asserts);

    reset(); lbm_ref_count = 1;
    expect_pause_assert = true;
    if (setjmp(pause_assert_env) == 0) {
        ble_log_lbm_begin_deinit();
        assert(!"deinit returned instead of asserting");
    }
    assert(ticks == pdMS_TO_TICKS(1000) && pause_asserts == 1);
    assert(!lbm_enabled && !lbm_inited && lbm_ref_count == 1 && !lock_depth);

    reset(); g_pool.waiting_task_count = 1; ignore_pause_assert = true;
    on_delay = finish_after_assert_disabled;
    ble_log_lbm_begin_deinit();
    assert(!lbm_enabled && !lbm_inited && !lbm_ref_count && !g_pool.waiting_task_count);
    assert(ticks == pdMS_TO_TICKS(1500) && pause_asserts == 1);

    reset();
    check_entry = true;
    assert(ble_log_writer_acquire() && lbm_ref_count == 1 && !lock_depth);
    BLE_LOG_REF_COUNT_RELEASE(&lbm_ref_count);
    check_entry = false;
    assert(ble_log_enable(false));
    check_entry = true;
    assert(!ble_log_writer_acquire() && !lbm_ref_count && !lock_depth);

    reset(); assert(ble_log_writer_acquire());
    on_take = recycle_while_parked;
    assert(ble_log_pool_acquire(1, false, true) != NULL);
    assert(lbm_enabled && lbm_ref_count == 1 && !g_pool.waiting_task_count);

    reset();
    assert(ble_log_writer_acquire());
    on_take = close_while_parked;
    assert(ble_log_pool_acquire(1, false, true) == NULL);
    assert(lbm_ref_count == 1 && !g_pool.waiting_task_count);
    BLE_LOG_REF_COUNT_RELEASE(&lbm_ref_count);
    assert(ble_log_lbm_wait_writers(0));

    reset(); lbm_enabled = 0; lbm_ref_count = 1; g_pool.waiting_task_count = 1;
    ble_log_test_flush_drain_between_loads_hook = straddle;
    on_delay = release_straddle;
    assert(ble_log_lbm_wait_writers(1) && ticks > 0);

    for (unsigned enabled = 0; enabled < 2; ++enabled) {
        for (unsigned success = 0; success < 2; ++success) {
            reset(); lbm_enabled = enabled; flush_success = success;
            ble_log_flush();
            assert(lbm_enabled == enabled && !lbm_ref_count && !flush_in_progress);
        }
        reset(); lbm_enabled = enabled; lbm_ref_count = 1;
        ble_log_flush();
        assert(lbm_enabled == enabled && lbm_ref_count == 1 && ticks == pdMS_TO_TICKS(1000));
        assert(!flush_in_progress);
    }
    reset(); flush_in_progress = true;
    assert(!ble_log_enable(false));
    ble_log_flush();
    assert(lbm_enabled && !lbm_ref_count && flush_in_progress);
    reset(); queued_frames = 3; g_pool.waiting_task_count = 1;
    on_delay = finish_shutdown_waiter;
    ble_log_shutdown_handler();
    assert(!lbm_enabled && !lbm_ref_count && !g_pool.waiting_task_count);
    assert(!queued_frames && flushed_frames == 3);

    puts("PASS: blocking/recycle, cancellation, counter straddle, pause timeout assertion, "
         "closed admission, deinit, flush");
}
"""

if __name__ == '__main__':
    for tick_hz in (100, 1000):
        run_c(f'#define configTICK_RATE_HZ {tick_hz}\n' + PRELUDE + macros + production + HELPERS + TESTS)
        print(f'PASS: BLE Log common lifecycle, tick_hz={tick_hz}')
