/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

/*
 * Unit tests for C2H credit (stack/btu/c2h_credit.c).
 *
 * BTU accounts into the host tab; HCI owns btsnd (pending + mail handoff).
 * Accounting tests stay paused so flush does not attempt HCI TX.
 * Alarm tests arm HCI flush/retry one-shots and must close before expiry.
 * btsnd is stubbed (default fail): harness has no HCI command_queue.
 */

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "unity.h"
#include "unity_test_runner.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/task.h"
#include "esp_timer.h"

#include "btc/btc_task.h"
#include "common/bt_target.h"

#if (C2H_FLOW_CONTROL_INCLUDED == TRUE)

#include "hci/hci_layer.h"
#include "osi/alarm.h"
#include "osi/thread.h"
#include "stack/bt_types.h"
#include "stack/btu.h"
#include "stack/c2h_credit.h"

extern void test_case_cleanup_set(void (*cleanup)(void));

#define TEST_WORKER_STACK     3072
#define TEST_WORKER_PRIO      2
#define TEST_MAX_ENTS         C2H_CREDIT_TAB_CAP
#define TEST_RETRY_DELAY_MS   C2H_CREDIT_RETRY_DELAY_MS
#define TEST_DEFER_DELAY_MS   C2H_CREDIT_HCI_DEFER_MS
#define TEST_DEFER_MIN_MS     C2H_CREDIT_DEFER_MIN_MS
#define TEST_DEFER_MAX_MS     C2H_CREDIT_DEFER_MAX_MS
#define TEST_DEFER_COLD_MS    C2H_CREDIT_DEFER_COLD_MS
/* Shorter than the minimum cold window, so a boot-time wait stays inside it. */
#define TEST_INSIDE_WINDOW_MS (4)
_Static_assert(TEST_INSIDE_WINDOW_MS < C2H_CREDIT_DEFER_MIN_MS,
               "coalesce wait must stay inside the cold window");

extern osi_thread_t *btu_thread;
extern void set_leak_threshold(int threshold);

static SemaphoreHandle_t s_block;
static osi_thread_t *s_hci_thread;
static volatile bool s_worker_timed_out;
static volatile TaskHandle_t s_flush_task;
static volatile TaskHandle_t s_send_task;

static void c2h_test_end(void);
static void c2h_test_end_btc(void);
static bool wait_pending(uint32_t expect, uint32_t wait_ms);

static void wait_boot_ms(uint32_t delay_ms)
{
    uint32_t start = osi_time_get_os_boottime_ms();

    while ((uint32_t)(osi_time_get_os_boottime_ms() - start) < delay_ms) {
    }
}

/* Just-armed one-shot, read from esp_timer milliseconds. A full tick of slack
 * accepts every positive remaining when the delay itself is one tick. */
static void assert_armed_near(uint32_t remaining, uint32_t armed_ms)
{
    uint32_t slack = 1;

    TEST_ASSERT_TRUE(remaining > 0);
    TEST_ASSERT_TRUE(remaining <= armed_ms);
    if (armed_ms > portTICK_PERIOD_MS) {
        slack = portTICK_PERIOD_MS;
    }
    TEST_ASSERT_TRUE((armed_ms - remaining) <= slack);
}

static UINT16 snapshot_packets(UINT16 handle)
{
    UINT16 num_pkts[TEST_MAX_ENTS];
    UINT16 handles[TEST_MAX_ENTS];
    UINT8 n;
    UINT8 i;

    n = btu_c2h_credit_pkts_rcvd(num_pkts, handles);
    for (i = 0; i < n; i++) {
        if (handles[i] == handle) {
            return num_pkts[i];
        }
    }
    return 0;
}

static void dummy_block(void *context)
{
    (void)context;
    /* Unity longjmps to the test task. A timeout here only sets a flag. */
    if (s_block == NULL || xSemaphoreTake(s_block, pdMS_TO_TICKS(2000)) != pdTRUE) {
        s_worker_timed_out = true;
    }
}

static void c2h_release_workers(void)
{
    int i;

    if (s_block == NULL) {
        return;
    }

    for (i = 0; i < 8; i++) {
        xSemaphoreGive(s_block);
    }
    /* A failed case may still be sitting in dummy_block. Let it leave. */
    if (Unity.CurrentTestFailed != 0) {
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}

static void c2h_free_threads(void)
{
    hci_test_set_host_thread(NULL);
    if (s_hci_thread != NULL) {
        osi_thread_free(s_hci_thread);
        s_hci_thread = NULL;
    }
    if (btu_thread != NULL) {
        osi_thread_free(btu_thread);
        btu_thread = NULL;
    }
}

static osi_thread_t *c2h_worker(const char *name, size_t queue_len)
{
    const size_t workqueue_len[] = {queue_len};
    osi_thread_t *thread = osi_thread_create(name, TEST_WORKER_STACK, TEST_WORKER_PRIO,
                                             OSI_THREAD_CORE_0, 1, workqueue_len, false);

    TEST_ASSERT_NOT_NULL(thread);
    return thread;
}

static void c2h_finish(bool with_btc)
{
    int mux;

    test_case_cleanup_set(NULL);
    hci_c2h_credit_test_set_flush_ready_hook(NULL);
    hci_c2h_credit_test_set_send_alarm_hook(NULL);
    /* Ensure close() never enqueues into a NULL HCI command_queue. */
    hci_c2h_credit_test_set_btsnd_fail(true);
    c2h_release_workers();
    c2h_credit_close();
    c2h_free_threads();
    c2h_credit_deinit();
    if (s_block != NULL) {
        vSemaphoreDelete(s_block);
        s_block = NULL;
    }
    osi_alarm_deinit();
    mux = osi_alarm_delete_mux();
    if (with_btc) {
        btc_deinit();
    } else {
        osi_thread_event_deinit();
    }

    /* Idle task reclaims deleted worker TCBs asynchronously. */
    vTaskDelay(pdMS_TO_TICKS(20));
    if (Unity.CurrentTestFailed == 0) {
        TEST_ASSERT_FALSE(s_worker_timed_out);
        TEST_ASSERT_EQUAL(0, mux);
    }
}

static void c2h_test_end(void)
{
    c2h_finish(false);
}

static void c2h_test_end_btc(void)
{
    c2h_finish(true);
}

static void c2h_test_begin_q(size_t queue_len)
{
    test_case_cleanup_set(c2h_test_end);
    s_worker_timed_out = false;
    s_flush_task = NULL;
    s_send_task = NULL;
    s_hci_thread = NULL;

    TEST_ASSERT_EQUAL(0, osi_thread_event_init());
    TEST_ASSERT_EQUAL(0, osi_alarm_create_mux());
    osi_alarm_init();

    /* Pin to core 0: OSI_THREAD_CORE_AFFINITY is not a valid FreeRTOS core id. */
    btu_thread = c2h_worker("c2h_test", queue_len);
    TEST_ASSERT_TRUE(c2h_credit_init());
    /* Harness has no HCI command_queue; keep btsnd stub in fail mode by default. */
}

static void c2h_test_begin(void)
{
    c2h_test_begin_q(4);
}

/* Alarm callbacks are dispatched on the BTC thread. */
static void c2h_test_begin_btc(void)
{
    test_case_cleanup_set(c2h_test_end_btc);
    s_worker_timed_out = false;
    s_flush_task = NULL;
    s_send_task = NULL;
    s_hci_thread = NULL;

    TEST_ASSERT_EQUAL(BT_STATUS_SUCCESS, btc_init());
    TEST_ASSERT_EQUAL(0, osi_alarm_create_mux());
    osi_alarm_init();

    btu_thread = c2h_worker("c2h_test", 4);
    TEST_ASSERT_TRUE(c2h_credit_init());
}

/* Install an HCI thread before init so flush_ready and send_alarm bind to it. */
static void c2h_test_begin_hci(void)
{
    test_case_cleanup_set(c2h_test_end);
    s_worker_timed_out = false;
    s_flush_task = NULL;
    s_send_task = NULL;

    TEST_ASSERT_EQUAL(0, osi_thread_event_init());
    TEST_ASSERT_EQUAL(0, osi_alarm_create_mux());
    osi_alarm_init();

    btu_thread = c2h_worker("c2h_test", 4);
    s_hci_thread = c2h_worker("c2h_hci", 4);
    hci_test_set_host_thread(s_hci_thread);
    TEST_ASSERT_TRUE(c2h_credit_init());
}

TEST_CASE("c2h_credit coalesces packets on the same handle", "[c2h_credit]")
{
    UINT16 num_pkts[TEST_MAX_ENTS];
    UINT16 handles[TEST_MAX_ENTS];
    UINT8 n;

    c2h_test_begin();
    btu_c2h_credit_pause();

    btu_c2h_credit_post(0x0042, 1);
    btu_c2h_credit_post(0x0042, 3);

    n = btu_c2h_credit_pkts_rcvd(num_pkts, handles);
    TEST_ASSERT_EQUAL_UINT8(1, n);
    TEST_ASSERT_EQUAL_HEX16(0x0042, handles[0]);
    TEST_ASSERT_EQUAL_UINT16(4, num_pkts[0]);
    TEST_ASSERT_EQUAL_UINT8(0, btu_c2h_credit_pkts_rcvd(num_pkts, handles));

    c2h_test_end();
}

TEST_CASE("c2h_credit add_n accounts a table under one call", "[c2h_credit]")
{
    UINT16 num_pkts[TEST_MAX_ENTS];
    UINT16 handles[TEST_MAX_ENTS];
    UINT16 in_handles[] = { 0x0001, 0x0002, 0x0001 };
    UINT16 in_pkts[] = { 2, 5, 1 };
    UINT8 n;

    c2h_test_begin();
    btu_c2h_credit_pause();

    btu_c2h_credit_add_n(3, in_handles, in_pkts);
    n = btu_c2h_credit_pkts_rcvd(num_pkts, handles);
    TEST_ASSERT_EQUAL_UINT8(2, n);
    TEST_ASSERT_EQUAL_HEX16(0x0001, handles[0]);
    TEST_ASSERT_EQUAL_UINT16(3, num_pkts[0]);
    TEST_ASSERT_EQUAL_HEX16(0x0002, handles[1]);
    TEST_ASSERT_EQUAL_UINT16(5, num_pkts[1]);

    c2h_test_end();
}

TEST_CASE("c2h_credit keeps a slot per handle", "[c2h_credit]")
{
    UINT16 num_pkts[TEST_MAX_ENTS];
    UINT16 handles[TEST_MAX_ENTS];
    UINT8 n;

    c2h_test_begin();
    btu_c2h_credit_pause();

    btu_c2h_credit_post(0x0001, 2);
    btu_c2h_credit_post(0x0002, 5);
    btu_c2h_credit_post(0x0001, 1);

    n = btu_c2h_credit_pkts_rcvd(num_pkts, handles);
    TEST_ASSERT_EQUAL_UINT8(2, n);
    /* Dense tab keeps first-seen order. */
    TEST_ASSERT_EQUAL_HEX16(0x0001, handles[0]);
    TEST_ASSERT_EQUAL_UINT16(3, num_pkts[0]);
    TEST_ASSERT_EQUAL_HEX16(0x0002, handles[1]);
    TEST_ASSERT_EQUAL_UINT16(5, num_pkts[1]);

    c2h_test_end();
}

TEST_CASE("c2h_credit strips HCI handle flags and saturates", "[c2h_credit]")
{
    c2h_test_begin();
    btu_c2h_credit_pause();

    /* High PB/BC bits must not create a distinct slot. */
    btu_c2h_credit_post(0x2007, 1);
    btu_c2h_credit_post(0x0007, 2);
    TEST_ASSERT_EQUAL_UINT16(3, snapshot_packets(0x0007));

    btu_c2h_credit_post(0x0008, 0xFFFF);
    btu_c2h_credit_post(0x0008, 4);
    TEST_ASSERT_EQUAL_UINT16(0xFFFF, snapshot_packets(0x0008));

    btu_c2h_credit_post(0x0009, 0);
    TEST_ASSERT_EQUAL_UINT16(0, snapshot_packets(0x0009));

    c2h_test_end();
}

TEST_CASE("c2h_credit ignores posts while disabled", "[c2h_credit]")
{
    UINT16 num_pkts[TEST_MAX_ENTS];
    UINT16 handles[TEST_MAX_ENTS];

    c2h_test_begin();
    btu_c2h_credit_pause();

    btu_c2h_credit_set_enabled(FALSE);
    btu_c2h_credit_post(0x0010, 4);
    TEST_ASSERT_EQUAL_UINT8(0, btu_c2h_credit_pkts_rcvd(num_pkts, handles));

    btu_c2h_credit_set_enabled(TRUE);
    btu_c2h_credit_post(0x0010, 2);
    TEST_ASSERT_EQUAL_UINT16(2, snapshot_packets(0x0010));

    c2h_test_end();
}

TEST_CASE("c2h_credit reset discards pending credits", "[c2h_credit]")
{
    UINT16 num_pkts[TEST_MAX_ENTS];
    UINT16 handles[TEST_MAX_ENTS];

    c2h_test_begin();
    btu_c2h_credit_pause();

    btu_c2h_credit_post(0x0011, 7);
    btu_c2h_credit_reset();
    TEST_ASSERT_EQUAL_UINT8(0, btu_c2h_credit_pkts_rcvd(num_pkts, handles));

    /* Reset also clears pause so a later post would wake; pause again. */
    btu_c2h_credit_pause();
    btu_c2h_credit_post(0x0011, 1);
    TEST_ASSERT_EQUAL_UINT16(1, snapshot_packets(0x0011));

    c2h_test_end();
}

TEST_CASE("c2h_credit reset cancels active alarm and rate state", "[c2h_credit]")
{
    c2h_test_begin();

    btu_c2h_credit_add(0x0012, 1);
    btu_c2h_credit_flush();
    TEST_ASSERT_TRUE(hci_c2h_credit_flush_alarm_is_active());
    TEST_ASSERT_TRUE(hci_c2h_credit_flush_alarm_is_outstanding());

    btu_c2h_credit_reset();
    TEST_ASSERT_FALSE(hci_c2h_credit_flush_alarm_is_active());
    TEST_ASSERT_FALSE(hci_c2h_credit_flush_alarm_is_outstanding());
    TEST_ASSERT_EQUAL_UINT32(0, btu_c2h_credit_interval_ewma_ms());

    c2h_test_end();
}

TEST_CASE("c2h_credit empty flush does not wake BTU", "[c2h_credit]")
{
    int depth;
    UINT16 num_pkts[TEST_MAX_ENTS];
    UINT16 handles[TEST_MAX_ENTS];

    c2h_test_begin();

    s_block = xSemaphoreCreateCounting(8, 0);
    TEST_ASSERT_NOT_NULL(s_block);
    TEST_ASSERT_TRUE(osi_thread_post(btu_thread, dummy_block, NULL, NULL, 0, 0));
    vTaskDelay(pdMS_TO_TICKS(20));

    depth = osi_thread_queue_wait_size(btu_thread, 0);
    btu_c2h_credit_flush();
    TEST_ASSERT_EQUAL(depth, osi_thread_queue_wait_size(btu_thread, 0));
    TEST_ASSERT_EQUAL_UINT8(0, btu_c2h_credit_pkts_rcvd(num_pkts, handles));

    xSemaphoreGive(s_block);
    vTaskDelay(pdMS_TO_TICKS(20));
    vSemaphoreDelete(s_block);
    s_block = NULL;
    c2h_test_end();
}

TEST_CASE("c2h_credit rollback restores a drained snapshot", "[c2h_credit]")
{
    UINT16 num_pkts[TEST_MAX_ENTS];
    UINT16 handles[TEST_MAX_ENTS];
    UINT8 n;

    c2h_test_begin();
    btu_c2h_credit_pause();

    btu_c2h_credit_post(0x0021, 3);
    btu_c2h_credit_post(0x0022, 9);
    n = btu_c2h_credit_pkts_rcvd(num_pkts, handles);
    TEST_ASSERT_EQUAL_UINT8(2, n);

    btu_c2h_credit_rollback(n, handles, num_pkts);
    n = btu_c2h_credit_pkts_rcvd(num_pkts, handles);
    TEST_ASSERT_EQUAL_UINT8(2, n);
    TEST_ASSERT_EQUAL_HEX16(0x0021, handles[0]);
    TEST_ASSERT_EQUAL_UINT16(3, num_pkts[0]);
    TEST_ASSERT_EQUAL_HEX16(0x0022, handles[1]);
    TEST_ASSERT_EQUAL_UINT16(9, num_pkts[1]);

    c2h_test_end();
}

TEST_CASE("c2h_credit pause suppresses wake; resume with empty pending is safe", "[c2h_credit]")
{
    UINT16 num_pkts[TEST_MAX_ENTS];
    UINT16 handles[TEST_MAX_ENTS];

    c2h_test_begin();
    btu_c2h_credit_pause();
    btu_c2h_credit_post(0x0030, 6);
    btu_c2h_credit_flush();
    TEST_ASSERT_EQUAL_UINT16(6, snapshot_packets(0x0030));

    /* Queue is empty: resume only clears PAUSED (no flush). */
    btu_c2h_credit_resume();
    vTaskDelay(pdMS_TO_TICKS(20));
    TEST_ASSERT_EQUAL_UINT8(0, btu_c2h_credit_pkts_rcvd(num_pkts, handles));

    c2h_test_end();
}

TEST_CASE("c2h_credit add coalesces without posting ready event", "[c2h_credit]")
{
    int depth;

    c2h_test_begin();

    s_block = xSemaphoreCreateCounting(8, 0);
    TEST_ASSERT_NOT_NULL(s_block);
    TEST_ASSERT_TRUE(osi_thread_post(btu_thread, dummy_block, NULL, NULL, 0, 0));
    vTaskDelay(pdMS_TO_TICKS(20));

    depth = osi_thread_queue_wait_size(btu_thread, 0);
    btu_c2h_credit_add(0x0060, 1);
    btu_c2h_credit_add(0x0060, 3);
    TEST_ASSERT_EQUAL(depth, osi_thread_queue_wait_size(btu_thread, 0));

    /* Drain under pause so the flush handler never touches HCI. */
    btu_c2h_credit_pause();
    TEST_ASSERT_EQUAL_UINT16(4, snapshot_packets(0x0060));

    xSemaphoreGive(s_block);
    vTaskDelay(pdMS_TO_TICKS(20));
    vSemaphoreDelete(s_block);
    s_block = NULL;
    c2h_test_end();
}

TEST_CASE("c2h_credit resume clears pause without auto flush", "[c2h_credit]")
{
    c2h_test_begin();
    btu_c2h_credit_pause();
    btu_c2h_credit_add(0x0062, 5);

    btu_c2h_credit_resume();
    /* Resume must not itself hand off / send. */
    TEST_ASSERT_EQUAL_UINT16(5, snapshot_packets(0x0062));

    btu_c2h_credit_add(0x0062, 5);
    btu_c2h_credit_flush();
    /* Handoff leaves the host tab empty (send runs on HCI / inline). */
    TEST_ASSERT_EQUAL_UINT16(0, snapshot_packets(0x0062));

    c2h_test_end();
}

TEST_CASE("c2h_credit accounts up to C2H_CREDIT_TAB_CAP unique handles", "[c2h_credit]")
{
    UINT16 num_pkts[C2H_CREDIT_TAB_CAP];
    UINT16 handles[C2H_CREDIT_TAB_CAP];
    UINT8 n;
    UINT16 i;
    UINT32 sum = 0;

    c2h_test_begin();
    btu_c2h_credit_pause();

    for (i = 1; i <= C2H_CREDIT_TAB_CAP; i++) {
        btu_c2h_credit_post(i, i);
    }

    n = btu_c2h_credit_pkts_rcvd(num_pkts, handles);
    TEST_ASSERT_EQUAL_UINT8(C2H_CREDIT_TAB_CAP, n);
    for (i = 0; i < n; i++) {
        sum += num_pkts[i];
    }
    TEST_ASSERT_EQUAL_UINT32((UINT32)C2H_CREDIT_TAB_CAP * (C2H_CREDIT_TAB_CAP + 1) / 2, sum);
    TEST_ASSERT_EQUAL_UINT8(0, btu_c2h_credit_pkts_rcvd(num_pkts, handles));

    c2h_test_end();
}

TEST_CASE("c2h_credit overflow keeps credits beyond TAB_CAP while paused", "[c2h_credit]")
{
    UINT16 num_pkts[C2H_CREDIT_TAB_CAP];
    UINT16 handles[C2H_CREDIT_TAB_CAP];
    UINT8 n;
    UINT16 i;
    UINT32 unique = 0;
    UINT32 pkts = 0;
    const UINT16 extra = 3;
    const UINT16 total = (UINT16)(C2H_CREDIT_TAB_CAP + extra);

    c2h_test_begin();
    btu_c2h_credit_pause();

    for (i = 1; i <= total; i++) {
        btu_c2h_credit_post(i, 1);
    }

    do {
        n = btu_c2h_credit_pkts_rcvd(num_pkts, handles);
        unique += n;
        for (i = 0; i < n; i++) {
            pkts += num_pkts[i];
        }
    } while (n > 0);

    TEST_ASSERT_EQUAL_UINT32(total, unique);
    TEST_ASSERT_EQUAL_UINT32(total, pkts);

    c2h_test_end();
}

TEST_CASE("c2h_credit retry alarm coalesces repeated rollback arms", "[c2h_credit]")
{
    UINT16 num_pkts[] = {2, 4};
    UINT16 handles[] = {0x0041, 0x0042};
    UINT32 remaining_after_first;
    UINT32 remaining_after_second;

    c2h_test_begin();
    TEST_ASSERT_FALSE(hci_c2h_credit_flush_alarm_is_active());

    /* Not paused: rollback handoffs to HCI mail and arms the 100 ms retry. */
    btu_c2h_credit_rollback(2, handles, num_pkts);
    TEST_ASSERT_TRUE(hci_c2h_credit_flush_alarm_is_active());
    remaining_after_first = hci_c2h_credit_flush_alarm_remaining_ms();
    TEST_ASSERT_UINT32_WITHIN(20, TEST_RETRY_DELAY_MS, remaining_after_first);

    vTaskDelay(pdMS_TO_TICKS(40));
    remaining_after_first = hci_c2h_credit_flush_alarm_remaining_ms();
    TEST_ASSERT_TRUE(remaining_after_first < 80);

    btu_c2h_credit_rollback(2, handles, num_pkts);
    TEST_ASSERT_TRUE(hci_c2h_credit_flush_alarm_is_active());
    remaining_after_second = hci_c2h_credit_flush_alarm_remaining_ms();
    TEST_ASSERT_TRUE(remaining_after_second <= remaining_after_first);
    TEST_ASSERT_TRUE(remaining_after_second < 80);

    btu_c2h_credit_rollback(1, handles, num_pkts);

    /* Close before the retry fires (no HCI in this harness). */
    c2h_test_end();
}

TEST_CASE("c2h_credit retry alarm is not armed while paused", "[c2h_credit]")
{
    UINT16 num_pkts[] = {3};
    UINT16 handles[] = {0x0051};

    c2h_test_begin();
    btu_c2h_credit_pause();
    btu_c2h_credit_rollback(1, handles, num_pkts);
    TEST_ASSERT_FALSE(hci_c2h_credit_flush_alarm_is_active());
    TEST_ASSERT_EQUAL_UINT32(0, hci_c2h_credit_flush_alarm_remaining_ms());

    /* If the one-shot had been armed, the callback would post into BTC (null). */
    vTaskDelay(pdMS_TO_TICKS(TEST_RETRY_DELAY_MS + 50));
    TEST_ASSERT_FALSE(hci_c2h_credit_flush_alarm_is_active());
    TEST_ASSERT_EQUAL_UINT16(3, snapshot_packets(0x0051));

    c2h_test_end();
}

TEST_CASE("c2h_credit retry alarm is cancelled on close", "[c2h_credit]")
{
    UINT16 num_pkts[] = {1};
    UINT16 handles[] = {0x0052};

    c2h_test_begin();
    btu_c2h_credit_rollback(1, handles, num_pkts);
    TEST_ASSERT_TRUE(hci_c2h_credit_flush_alarm_is_active());

    c2h_credit_close();
    TEST_ASSERT_FALSE(hci_c2h_credit_flush_alarm_is_active());
    TEST_ASSERT_FALSE(hci_c2h_credit_flush_alarm_is_outstanding());

    /* close() must stop the timer before osi_alarm_free; otherwise the
     * callback would run after teardown. */
    vTaskDelay(pdMS_TO_TICKS(TEST_RETRY_DELAY_MS + 50));
    TEST_ASSERT_FALSE(hci_c2h_credit_flush_alarm_is_active());

    c2h_test_end();
}

TEST_CASE("c2h_credit flush handoff works while BTU queue is busy", "[c2h_credit]")
{
    c2h_test_begin_q(2);

    s_block = xSemaphoreCreateCounting(8, 0);
    TEST_ASSERT_NOT_NULL(s_block);

    TEST_ASSERT_TRUE(osi_thread_post(btu_thread, dummy_block, NULL, NULL, 0, 0));
    vTaskDelay(pdMS_TO_TICKS(20));
    TEST_ASSERT_TRUE(osi_thread_post(btu_thread, dummy_block, NULL, NULL, 0, 0));
    TEST_ASSERT_TRUE(osi_thread_post(btu_thread, dummy_block, NULL, NULL, 0, 0));
    TEST_ASSERT_FALSE(osi_thread_post(btu_thread, dummy_block, NULL, NULL, 0, 0));

    /* Send path is on HCI — a full BTU queue must not block credit handoff. */
    TEST_ASSERT_FALSE(hci_c2h_credit_flush_alarm_is_active());
    btu_c2h_credit_post(0x0053, 2);
    TEST_ASSERT_TRUE(hci_c2h_credit_flush_alarm_is_active());

    /* Cancel the defer before draining BTU; this harness has no BTC task. */
    hci_c2h_credit_test_set_btsnd_fail(true);
    c2h_credit_close();
    TEST_ASSERT_FALSE(hci_c2h_credit_flush_alarm_is_active());
    TEST_ASSERT_FALSE(hci_c2h_credit_flush_alarm_is_outstanding());

    for (int i = 0; i < 3; i++) {
        xSemaphoreGive(s_block);
    }
    vTaskDelay(pdMS_TO_TICKS(20));

    vSemaphoreDelete(s_block);
    s_block = NULL;
    c2h_test_end();
}

TEST_CASE("c2h_credit retry alarm fires and is idle after callback", "[c2h_credit]")
{
    UINT16 num_pkts[] = {5};
    UINT16 handles[] = {0x0054};

    set_leak_threshold(-4000);
    c2h_test_begin_btc();

    btu_c2h_credit_pause();
    btu_c2h_credit_rollback(1, handles, num_pkts);
    TEST_ASSERT_FALSE(hci_c2h_credit_flush_alarm_is_active());
    TEST_ASSERT_EQUAL_UINT16(5, snapshot_packets(0x0054));

    /* Re-queue and arm retry without pause so the alarm can fire. */
    btu_c2h_credit_resume();
    btu_c2h_credit_rollback(1, handles, num_pkts);
    TEST_ASSERT_TRUE(hci_c2h_credit_flush_alarm_is_active());

    /* Simulate successful Host_Number_Of_Completed_Packets (no HCI queue). */
    hci_c2h_credit_test_set_btsnd_fail(false);
    vTaskDelay(pdMS_TO_TICKS(TEST_RETRY_DELAY_MS + 50));
    TEST_ASSERT_FALSE(hci_c2h_credit_flush_alarm_is_active());

    hci_c2h_credit_test_set_btsnd_fail(true);
    c2h_test_end_btc();
}

TEST_CASE("c2h_credit alarm does not rearm while BTC callback is pending", "[c2h_credit]")
{
    int queued;

    set_leak_threshold(-4000);
    c2h_test_begin_btc();

    s_block = xSemaphoreCreateCounting(1, 0);
    TEST_ASSERT_NOT_NULL(s_block);
    TEST_ASSERT_TRUE(osi_thread_post(btc_get_current_thread(), dummy_block, NULL, NULL, 0, 0));
    vTaskDelay(pdMS_TO_TICKS(20));

    hci_c2h_credit_account(0x0060, 1);
    hci_c2h_credit_flush();
    TEST_ASSERT_TRUE(hci_c2h_credit_flush_alarm_is_active());
    TEST_ASSERT_TRUE(hci_c2h_credit_flush_alarm_is_outstanding());

    vTaskDelay(pdMS_TO_TICKS(TEST_DEFER_DELAY_MS + 20));
    TEST_ASSERT_FALSE(hci_c2h_credit_flush_alarm_is_active());
    TEST_ASSERT_TRUE(hci_c2h_credit_flush_alarm_is_outstanding());
    queued = get_btc_work_queue_size();
    TEST_ASSERT_EQUAL(1, queued);

    for (int i = 0; i < 1000; i++) {
        hci_c2h_credit_flush();
    }
    TEST_ASSERT_FALSE(hci_c2h_credit_flush_alarm_is_active());
    TEST_ASSERT_TRUE(hci_c2h_credit_flush_alarm_is_outstanding());
    TEST_ASSERT_EQUAL(queued, get_btc_work_queue_size());

    hci_c2h_credit_test_set_btsnd_fail(false);
    xSemaphoreGive(s_block);
    vTaskDelay(pdMS_TO_TICKS(50));
    TEST_ASSERT_FALSE(hci_c2h_credit_flush_alarm_is_outstanding());
    TEST_ASSERT_EQUAL_UINT32(0, hci_c2h_credit_pending_pkts());

    vSemaphoreDelete(s_block);
    s_block = NULL;
    hci_c2h_credit_test_set_btsnd_fail(true);
    c2h_test_end_btc();
}

TEST_CASE("c2h_credit close clears outstanding after the timer has expired", "[c2h_credit]")
{
    set_leak_threshold(-4000);
    c2h_test_begin_btc();

    s_block = xSemaphoreCreateCounting(1, 0);
    TEST_ASSERT_NOT_NULL(s_block);
    TEST_ASSERT_TRUE(osi_thread_post(btc_get_current_thread(), dummy_block, NULL, NULL, 0, 0));
    vTaskDelay(pdMS_TO_TICKS(20));

    hci_c2h_credit_account(0x0061, 1);
    hci_c2h_credit_flush();
    TEST_ASSERT_TRUE(hci_c2h_credit_flush_alarm_is_active());
    TEST_ASSERT_TRUE(hci_c2h_credit_flush_alarm_is_outstanding());

    /* BTC is blocked, so expiry cannot run the callback that clears the latch. */
    vTaskDelay(pdMS_TO_TICKS(TEST_DEFER_DELAY_MS + 20));
    TEST_ASSERT_FALSE(hci_c2h_credit_flush_alarm_is_active());
    TEST_ASSERT_TRUE(hci_c2h_credit_flush_alarm_is_outstanding());

    hci_c2h_credit_test_set_btsnd_fail(true);
    hci_c2h_credit_close();
    TEST_ASSERT_FALSE(hci_c2h_credit_flush_alarm_is_active());
    TEST_ASSERT_FALSE(hci_c2h_credit_flush_alarm_is_outstanding());
    TEST_ASSERT_EQUAL_UINT32(0, hci_c2h_credit_pending_pkts());

    xSemaphoreGive(s_block);
    vTaskDelay(pdMS_TO_TICKS(50));
    TEST_ASSERT_FALSE(hci_c2h_credit_flush_alarm_is_outstanding());

    vSemaphoreDelete(s_block);
    s_block = NULL;
    c2h_test_end_btc();
}

TEST_CASE("c2h_credit adaptive defer calculation is bounded", "[c2h_credit]")
{
    TEST_ASSERT_EQUAL_UINT32(
        TEST_DEFER_COLD_MS, btu_c2h_credit_test_defer_delay_ms(0, 1));
    TEST_ASSERT_EQUAL_UINT32(
        30, btu_c2h_credit_test_defer_delay_ms(10, 1));
    TEST_ASSERT_EQUAL_UINT32(
        TEST_DEFER_MIN_MS, btu_c2h_credit_test_defer_delay_ms(1, 1));
    TEST_ASSERT_EQUAL_UINT32(
        TEST_DEFER_MAX_MS, btu_c2h_credit_test_defer_delay_ms(1000, 1));
    TEST_ASSERT_EQUAL_UINT32(
        0, btu_c2h_credit_test_defer_delay_ms(1000, 4));
}

TEST_CASE("c2h_credit rate estimator resets after idle", "[c2h_credit]")
{
    c2h_test_begin();
    btu_c2h_credit_pause();

    btu_c2h_credit_add(0x0061, 1);
    TEST_ASSERT_EQUAL_UINT32(0, btu_c2h_credit_interval_ewma_ms());
    vTaskDelay(pdMS_TO_TICKS(5));
    btu_c2h_credit_add(0x0061, 1);
    TEST_ASSERT_GREATER_THAN_UINT32(0, btu_c2h_credit_interval_ewma_ms());

    vTaskDelay(pdMS_TO_TICKS(1050));
    btu_c2h_credit_add(0x0061, 1);
    TEST_ASSERT_EQUAL_UINT32(0, btu_c2h_credit_interval_ewma_ms());

    c2h_test_end();
}

TEST_CASE("c2h_credit rate estimator follows traffic steps", "[c2h_credit]")
{
    UINT32 slow_interval;
    UINT32 fast_interval;
    UINT32 recovered_interval;

    c2h_test_begin();
    btu_c2h_credit_pause();

    btu_c2h_credit_add(0x0062, 1);
    wait_boot_ms(2 * portTICK_PERIOD_MS);
    btu_c2h_credit_add(0x0062, 1);
    slow_interval = btu_c2h_credit_interval_ewma_ms();
    TEST_ASSERT_GREATER_THAN_UINT32(portTICK_PERIOD_MS, slow_interval);

    for (int i = 0; i < 12; i++) {
        wait_boot_ms(portTICK_PERIOD_MS);
        btu_c2h_credit_add(0x0062, 1);
    }
    fast_interval = btu_c2h_credit_interval_ewma_ms();
    TEST_ASSERT_TRUE(fast_interval < slow_interval);

    for (int i = 0; i < 12; i++) {
        wait_boot_ms(2 * portTICK_PERIOD_MS);
        btu_c2h_credit_add(0x0062, 1);
    }
    recovered_interval = btu_c2h_credit_interval_ewma_ms();
    TEST_ASSERT_TRUE(recovered_interval > fast_interval);

    c2h_test_end();
}

TEST_CASE("c2h_credit rate estimator rises through small steps", "[c2h_credit]")
{
    int i;

    c2h_test_begin();
    btu_c2h_credit_pause();

    /* (7*e + s) >> 3 cannot leave 1 until the sample is at least 9. */
    btu_c2h_credit_test_sample_ms(1);
    TEST_ASSERT_EQUAL_UINT32(1, btu_c2h_credit_interval_ewma_ms());
    for (i = 0; i < 8; i++) {
        btu_c2h_credit_test_sample_ms(4);
    }
    TEST_ASSERT_GREATER_THAN_UINT32(1, btu_c2h_credit_interval_ewma_ms());

    btu_c2h_credit_reset();
    btu_c2h_credit_pause();
    btu_c2h_credit_test_sample_ms(1);
    for (i = 0; i < 16; i++) {
        btu_c2h_credit_test_sample_ms(8);
    }
    TEST_ASSERT_GREATER_THAN_UINT32(4, btu_c2h_credit_interval_ewma_ms());

    btu_c2h_credit_reset();
    btu_c2h_credit_pause();
    btu_c2h_credit_test_sample_ms(20);
    for (i = 0; i < 8; i++) {
        btu_c2h_credit_test_sample_ms(13);
    }
    TEST_ASSERT_LESS_THAN_UINT32(20, btu_c2h_credit_interval_ewma_ms());

    c2h_test_end();
}

TEST_CASE("c2h_credit first host flush uses unknown-rate defer", "[c2h_credit]")
{
    UINT32 remaining;

    c2h_test_begin();
    TEST_ASSERT_FALSE(hci_c2h_credit_flush_alarm_is_active());

    btu_c2h_credit_add(0x0070, 1);
    btu_c2h_credit_flush();
    TEST_ASSERT_TRUE(hci_c2h_credit_flush_alarm_is_active());
    remaining = hci_c2h_credit_flush_alarm_remaining_ms();
    assert_armed_near(remaining, TEST_DEFER_COLD_MS);
    /* Below FLUSH_MIN: stay on host tab (no mail_lock); close before alarm. */
    TEST_ASSERT_EQUAL_UINT16(1, snapshot_packets(0x0070));

    c2h_test_end();
}

TEST_CASE("c2h_credit schedule shortens deadline from retry to defer", "[c2h_credit]")
{
    UINT16 num_pkts[] = {1};
    UINT16 handles[] = {0x0074};
    UINT32 defer;
    UINT32 remaining;

    c2h_test_begin();
    TEST_ASSERT_FALSE(hci_c2h_credit_flush_alarm_is_active());

    /* Rollback handoffs and arms the 100 ms retry. */
    btu_c2h_credit_rollback(1, handles, num_pkts);
    TEST_ASSERT_TRUE(hci_c2h_credit_flush_alarm_is_active());
    remaining = hci_c2h_credit_flush_alarm_remaining_ms();
    assert_armed_near(remaining, TEST_RETRY_DELAY_MS);

    /* A known 20 ms sample with one credit defers 60 ms, well under the retry. */
    btu_c2h_credit_add(0x0075, 1);
    btu_c2h_credit_test_sample_ms(20);
    defer = btu_c2h_credit_test_defer_delay_ms(btu_c2h_credit_interval_ewma_ms(), 1);
    TEST_ASSERT_TRUE(defer + portTICK_PERIOD_MS < TEST_RETRY_DELAY_MS);
    btu_c2h_credit_flush();
    TEST_ASSERT_TRUE(hci_c2h_credit_flush_alarm_is_active());
    remaining = hci_c2h_credit_flush_alarm_remaining_ms();
    assert_armed_near(remaining, defer);

    c2h_test_end();
}

TEST_CASE("c2h_credit flush while deferred coalesces on host tab", "[c2h_credit]")
{
    UINT32 defer;
    UINT32 remaining;

    c2h_test_begin();
    TEST_ASSERT_FALSE(hci_c2h_credit_flush_alarm_is_active());

    /* 100 ms sample, one credit: 300 ms, above the 10 ms cold window. */
    btu_c2h_credit_add(0x0073, 1);
    btu_c2h_credit_test_sample_ms(100);
    defer = btu_c2h_credit_test_defer_delay_ms(btu_c2h_credit_interval_ewma_ms(), 1);
    TEST_ASSERT_TRUE(defer > TEST_DEFER_COLD_MS);
    btu_c2h_credit_flush();
    TEST_ASSERT_TRUE(hci_c2h_credit_flush_alarm_is_active());
    remaining = hci_c2h_credit_flush_alarm_remaining_ms();
    assert_armed_near(remaining, defer);

    btu_c2h_credit_add(0x0073, 1);
    defer = btu_c2h_credit_test_defer_delay_ms(btu_c2h_credit_interval_ewma_ms(), 2);
    TEST_ASSERT_TRUE(defer + portTICK_PERIOD_MS < remaining);
    btu_c2h_credit_flush();
    TEST_ASSERT_TRUE(hci_c2h_credit_flush_alarm_is_active());
    remaining = hci_c2h_credit_flush_alarm_remaining_ms();
    assert_armed_near(remaining, defer);
    /* Still below FLUSH_MIN: both credits remain on host (no handoff yet). */
    TEST_ASSERT_EQUAL_UINT16(2, snapshot_packets(0x0073));

    c2h_test_end();
}

TEST_CASE("c2h_credit BTU flush sends immediately at threshold", "[c2h_credit]")
{
    set_leak_threshold(-4000);
    c2h_test_begin_hci();
    hci_c2h_credit_test_set_btsnd_fail(false);

    btu_c2h_credit_add(0x0071, 4);
    btu_c2h_credit_flush();
    TEST_ASSERT_EQUAL_UINT16(0, snapshot_packets(0x0071));
    TEST_ASSERT_TRUE(wait_pending(0, 500));
    TEST_ASSERT_FALSE(hci_c2h_credit_flush_alarm_is_active());

    hci_c2h_credit_test_set_btsnd_fail(true);
    c2h_test_end();
}

TEST_CASE("c2h_credit flush_and_purge bypasses the defer window", "[c2h_credit]")
{
    UINT32 remaining;

    set_leak_threshold(-4000);
    c2h_test_begin_hci();

    btu_c2h_credit_add(0x0072, 1);
    btu_c2h_credit_flush();
    TEST_ASSERT_TRUE(hci_c2h_credit_flush_alarm_is_active());
    remaining = hci_c2h_credit_flush_alarm_remaining_ms();
    assert_armed_near(remaining, TEST_DEFER_COLD_MS);

    hci_c2h_credit_test_set_btsnd_fail(false);
    btu_c2h_credit_flush_and_purge(0x0072);

    /* Kick only posts flush_ready. The credit stays pending if that post never runs. */
    TEST_ASSERT_TRUE(wait_pending(0, 500));
    TEST_ASSERT_FALSE(hci_c2h_credit_flush_alarm_is_active());
    TEST_ASSERT_EQUAL_UINT16(0, snapshot_packets(0x0072));

    hci_c2h_credit_test_set_btsnd_fail(true);
    c2h_test_end();
}

TEST_CASE("c2h_credit purge on the HCI thread keeps the other handle", "[c2h_credit]")
{
    UINT32 remaining;

    set_leak_threshold(-4000);
    c2h_test_begin_hci();
    hci_c2h_credit_test_set_btsnd_fail(true);

    btu_c2h_credit_add(0x00C1, 1);
    btu_c2h_credit_flush();
    TEST_ASSERT_TRUE(hci_c2h_credit_flush_alarm_is_active());
    remaining = hci_c2h_credit_flush_alarm_remaining_ms();
    assert_armed_near(remaining, TEST_DEFER_COLD_MS);

    btu_c2h_credit_flush_and_purge(0x00C2);

    /* Reported, or still covered by the send alarm. btsnd fails, so a handed-off
     * credit remains visible in the HCI tabs instead of a controller counter. */
    TEST_ASSERT_TRUE(hci_c2h_credit_pending_pkts() != 0 ||
                     hci_c2h_credit_flush_alarm_is_active());

    hci_c2h_credit_test_set_btsnd_fail(false);
    c2h_test_end();
}

TEST_CASE("c2h_credit hci flush below threshold defers 10 ms", "[c2h_credit]")
{
    UINT32 remaining;

    c2h_test_begin();
    TEST_ASSERT_FALSE(hci_c2h_credit_flush_alarm_is_active());

    hci_c2h_credit_account(0x0080, 1);
    hci_c2h_credit_flush();
    TEST_ASSERT_TRUE(hci_c2h_credit_flush_alarm_is_active());
    remaining = hci_c2h_credit_flush_alarm_remaining_ms();
    assert_armed_near(remaining, TEST_DEFER_DELAY_MS);

    c2h_test_end();
}

TEST_CASE("c2h_credit hci flush while deferred is edge-triggered", "[c2h_credit]")
{
    UINT32 remaining_after_first;
    UINT32 remaining_after_second;

    c2h_test_begin();
    TEST_ASSERT_FALSE(hci_c2h_credit_flush_alarm_is_active());

    hci_c2h_credit_account(0x0081, 1);
    hci_c2h_credit_flush();
    TEST_ASSERT_TRUE(hci_c2h_credit_flush_alarm_is_active());
    remaining_after_first = hci_c2h_credit_flush_alarm_remaining_ms();
    assert_armed_near(remaining_after_first, TEST_DEFER_DELAY_MS);

    /* Busy-wait on esp_timer. vTaskDelay(4 ms) is zero ticks at 100 Hz. */
    wait_boot_ms(TEST_INSIDE_WINDOW_MS);
    remaining_after_first = hci_c2h_credit_flush_alarm_remaining_ms();
    TEST_ASSERT_TRUE(remaining_after_first + TEST_INSIDE_WINDOW_MS <= TEST_DEFER_DELAY_MS + 1);

    hci_c2h_credit_account(0x0081, 1);
    hci_c2h_credit_flush();
    TEST_ASSERT_TRUE(hci_c2h_credit_flush_alarm_is_active());
    remaining_after_second = hci_c2h_credit_flush_alarm_remaining_ms();
    TEST_ASSERT_TRUE(remaining_after_second <= remaining_after_first);
    TEST_ASSERT_TRUE(remaining_after_second + 1 < TEST_DEFER_DELAY_MS);

    c2h_test_end();
}

TEST_CASE("c2h_credit hci flush at threshold is not deferred", "[c2h_credit]")
{
    c2h_test_begin();
    hci_c2h_credit_test_set_btsnd_fail(false);

    hci_c2h_credit_account(0x0082, 4);
    hci_c2h_credit_flush();
    TEST_ASSERT_EQUAL_UINT32(0, hci_c2h_credit_pending_pkts());
    TEST_ASSERT_FALSE(hci_c2h_credit_flush_alarm_is_active());

    hci_c2h_credit_test_set_btsnd_fail(true);
    c2h_test_end();
}

TEST_CASE("c2h_credit hci flush keeps an armed host defer", "[c2h_credit]")
{
    UINT32 remaining;
    int64_t deadline;

    set_leak_threshold(-4000);
    c2h_test_begin_btc();
    hci_c2h_credit_test_set_btsnd_fail(false);

    /* pkts_rcvd extracts the host tab, so the owed credit is observed on the
     * HCI pending count after the defer hands it off. */
    btu_c2h_credit_add(0x00B0, 1);
    btu_c2h_credit_flush();
    TEST_ASSERT_TRUE(hci_c2h_credit_flush_alarm_is_active());
    remaining = hci_c2h_credit_flush_alarm_remaining_ms();
    assert_armed_near(remaining, TEST_DEFER_COLD_MS);

    hci_c2h_credit_account(0x00B1, 4);
    hci_c2h_credit_flush();
    TEST_ASSERT_EQUAL_UINT32(0, hci_c2h_credit_pending_pkts());
    TEST_ASSERT_TRUE(hci_c2h_credit_flush_alarm_is_active());
    remaining = hci_c2h_credit_flush_alarm_remaining_ms();
    TEST_ASSERT_TRUE(remaining > 0);

    hci_c2h_credit_test_set_btsnd_fail(true);
    deadline = esp_timer_get_time() + (int64_t)(TEST_DEFER_COLD_MS + 200) * 1000;
    while (hci_c2h_credit_pending_pkts() == 0 && esp_timer_get_time() < deadline) {
        vTaskDelay(pdMS_TO_TICKS(10));
    }
    TEST_ASSERT_EQUAL_UINT32(1, hci_c2h_credit_pending_pkts());
    TEST_ASSERT_EQUAL_UINT16(0, snapshot_packets(0x00B0));

    c2h_test_end_btc();
}

TEST_CASE("c2h_credit hci flush alarm is cancelled on close", "[c2h_credit]")
{
    c2h_test_begin();
    hci_c2h_credit_account(0x0083, 1);
    hci_c2h_credit_flush();
    TEST_ASSERT_TRUE(hci_c2h_credit_flush_alarm_is_active());

    c2h_credit_close();
    TEST_ASSERT_FALSE(hci_c2h_credit_flush_alarm_is_active());

    vTaskDelay(pdMS_TO_TICKS(TEST_DEFER_DELAY_MS + 50));
    TEST_ASSERT_FALSE(hci_c2h_credit_flush_alarm_is_active());

    c2h_test_end();
}

TEST_CASE("c2h_credit init fails without a BTU thread", "[c2h_credit]")
{
    test_case_cleanup_set(c2h_test_end);
    s_hci_thread = NULL;
    s_worker_timed_out = false;

    TEST_ASSERT_EQUAL(0, osi_thread_event_init());
    TEST_ASSERT_EQUAL(0, osi_alarm_create_mux());
    osi_alarm_init();

    /* No hciT and no BTU worker: flush_ready cannot bind, init must fail. */
    btu_thread = NULL;
    TEST_ASSERT_FALSE(c2h_credit_init());
    c2h_test_end();
}

TEST_CASE("c2h_credit host close handoffs before HCI close (shutdown order)", "[c2h_credit]")
{
    c2h_test_begin();
    hci_c2h_credit_test_set_btsnd_fail(true);

    /* Host tab has credits that must reach HCI mail before HCI teardown. */
    btu_c2h_credit_add(0x0090, 3);
    TEST_ASSERT_EQUAL_UINT32(0, hci_c2h_credit_pending_pkts());

    btu_c2h_credit_close();
    TEST_ASSERT_EQUAL_UINT16(0, snapshot_packets(0x0090));
    TEST_ASSERT_EQUAL_UINT32(3, hci_c2h_credit_pending_pkts());

    /* HCI close must attempt flush then cancel alarm while still alive. */
    hci_c2h_credit_close();
    TEST_ASSERT_FALSE(hci_c2h_credit_flush_alarm_is_active());
    TEST_ASSERT_FALSE(hci_c2h_credit_flush_alarm_is_outstanding());
    TEST_ASSERT_EQUAL_UINT32(0, hci_c2h_credit_pending_pkts());

    hci_c2h_credit_test_set_btsnd_fail(false);
    c2h_test_end();
}

TEST_CASE("c2h_credit btsnd failure restores pending and arms retry", "[c2h_credit]")
{
    UINT32 remaining;

    c2h_test_begin();
    hci_c2h_credit_test_set_btsnd_fail(true);

    hci_c2h_credit_account(0x0091, 4);
    TEST_ASSERT_EQUAL_UINT32(4, hci_c2h_credit_pending_pkts());

    hci_c2h_credit_flush();
    /* Force path: restore keeps credits and arms 100 ms retry. */
    TEST_ASSERT_EQUAL_UINT32(4, hci_c2h_credit_pending_pkts());
    TEST_ASSERT_TRUE(hci_c2h_credit_flush_alarm_is_active());
    remaining = hci_c2h_credit_flush_alarm_remaining_ms();
    TEST_ASSERT_UINT32_WITHIN(20, TEST_RETRY_DELAY_MS, remaining);

    hci_c2h_credit_test_set_btsnd_fail(false);
    c2h_test_end();
}

TEST_CASE("c2h_credit flush_and_purge keeps credits after HCI closing", "[c2h_credit]")
{
    c2h_test_begin();
    hci_c2h_credit_test_set_btsnd_fail(true);

    btu_c2h_credit_add(0x0092, 5);

    /* Simulate hci_shut_down having closed HCI credit first. */
    hci_c2h_credit_close();
    btu_c2h_credit_flush_and_purge(0x0092);

    /* Closing: cannot mail; credits must remain on host (not dropped). */
    TEST_ASSERT_EQUAL_UINT16(5, snapshot_packets(0x0092));
    TEST_ASSERT_EQUAL_UINT32(0, hci_c2h_credit_pending_pkts());

    hci_c2h_credit_test_set_btsnd_fail(false);
    c2h_test_end();
}

TEST_CASE("c2h_credit host close after HCI close is safe", "[c2h_credit]")
{
    c2h_test_begin();
    hci_c2h_credit_test_set_btsnd_fail(true);

    btu_c2h_credit_add(0x0093, 2);
    hci_c2h_credit_close();
    hci_c2h_credit_deinit();

    /* BTU_ShutDown after hci_shut_down: handoff is a no-op without s_hci. */
    btu_c2h_credit_close();
    TEST_ASSERT_EQUAL_UINT16(2, snapshot_packets(0x0093));

    hci_c2h_credit_test_set_btsnd_fail(false);
    c2h_test_end();
}

static void note_flush_task(void)
{
    s_flush_task = xTaskGetCurrentTaskHandle();
}

static void note_send_task(void)
{
    s_send_task = xTaskGetCurrentTaskHandle();
}

static void assert_on_hci_thread(TaskHandle_t task)
{
    TEST_ASSERT_NOT_NULL(task);
    TEST_ASSERT_NOT_EQUAL(xTaskGetCurrentTaskHandle(), task);
    TEST_ASSERT_EQUAL_STRING(osi_thread_name(s_hci_thread), pcTaskGetName(task));
}

static bool wait_pending(uint32_t expect, uint32_t wait_ms)
{
    int64_t deadline = esp_timer_get_time() + (int64_t)wait_ms * 1000;

    while (esp_timer_get_time() < deadline) {
        if (hci_c2h_credit_pending_pkts() == expect) {
            return true;
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
    return hci_c2h_credit_pending_pkts() == expect;
}

static bool wait_alarm_active(bool active, uint32_t wait_ms)
{
    int64_t deadline = esp_timer_get_time() + (int64_t)wait_ms * 1000;

    while (esp_timer_get_time() < deadline) {
        if (hci_c2h_credit_flush_alarm_is_active() == active) {
            return true;
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
    return hci_c2h_credit_flush_alarm_is_active() == active;
}

static bool wait_task(volatile TaskHandle_t *slot, uint32_t wait_ms)
{
    int64_t deadline = esp_timer_get_time() + (int64_t)wait_ms * 1000;

    while (*slot == NULL && esp_timer_get_time() < deadline) {
        vTaskDelay(pdMS_TO_TICKS(10));
    }
    return *slot != NULL;
}

TEST_CASE("c2h_credit posts flush_ready on the HCI thread", "[c2h_credit]")
{
    TaskHandle_t flush_task;

    set_leak_threshold(-4000);
    c2h_test_begin_hci();
    hci_c2h_credit_test_set_flush_ready_hook(note_flush_task);
    hci_c2h_credit_test_set_btsnd_fail(false);

    s_block = xSemaphoreCreateCounting(1, 0);
    TEST_ASSERT_NOT_NULL(s_block);
    TEST_ASSERT_TRUE(osi_thread_post(s_hci_thread, dummy_block, NULL, NULL, 0, 0));
    vTaskDelay(pdMS_TO_TICKS(20));

    btu_c2h_credit_post(0x00A0, 4);
    TEST_ASSERT_EQUAL_UINT32(4, hci_c2h_credit_pending_pkts());
    TEST_ASSERT_FALSE(hci_c2h_credit_flush_alarm_is_active());
    flush_task = s_flush_task;
    TEST_ASSERT_NULL(flush_task);

    /* Event is already queued behind the blocker. A second kick must not arm a retry. */
    btu_c2h_credit_post(0x00A0, 4);
    TEST_ASSERT_EQUAL_UINT32(8, hci_c2h_credit_pending_pkts());
    TEST_ASSERT_FALSE(hci_c2h_credit_flush_alarm_is_active());
    flush_task = s_flush_task;
    TEST_ASSERT_NULL(flush_task);

    xSemaphoreGive(s_block);
    TEST_ASSERT_TRUE(wait_pending(0, 500));
    TEST_ASSERT_TRUE(wait_task(&s_flush_task, 100));
    flush_task = s_flush_task;
    assert_on_hci_thread(flush_task);
    TEST_ASSERT_FALSE(hci_c2h_credit_flush_alarm_is_active());

    c2h_test_end();
}

TEST_CASE("c2h_credit send alarm runs on the HCI thread", "[c2h_credit]")
{
    TaskHandle_t flush_task;
    TaskHandle_t send_task;

    set_leak_threshold(-4000);
    c2h_test_begin_hci();
    hci_c2h_credit_test_set_flush_ready_hook(note_flush_task);
    hci_c2h_credit_test_set_send_alarm_hook(note_send_task);
    hci_c2h_credit_test_set_btsnd_fail(true);

    btu_c2h_credit_post(0x00A1, 4);
    TEST_ASSERT_TRUE(wait_task(&s_flush_task, 500));
    flush_task = s_flush_task;
    assert_on_hci_thread(flush_task);
    TEST_ASSERT_TRUE(wait_alarm_active(true, 200));

    hci_c2h_credit_test_set_btsnd_fail(false);
    TEST_ASSERT_TRUE(wait_task(&s_send_task, TEST_RETRY_DELAY_MS + 400));
    send_task = s_send_task;
    assert_on_hci_thread(send_task);
    TEST_ASSERT_TRUE(wait_pending(0, 500));
    TEST_ASSERT_FALSE(hci_c2h_credit_flush_alarm_is_active());

    c2h_test_end();
}

TEST_CASE("c2h_credit drain_ready reject keeps the send retry", "[c2h_credit]")
{
    UINT32 remaining;

    set_leak_threshold(-4000);
    c2h_test_begin_hci();
    hci_c2h_credit_test_set_send_alarm_hook(note_send_task);

    /* One worker is inside dummy_block; four more fill the queue of length 4. */
    s_block = xSemaphoreCreateCounting(8, 0);
    TEST_ASSERT_NOT_NULL(s_block);
    TEST_ASSERT_TRUE(osi_thread_post(btu_thread, dummy_block, NULL, NULL, 0, 0));
    vTaskDelay(pdMS_TO_TICKS(20));
    TEST_ASSERT_TRUE(osi_thread_post(btu_thread, dummy_block, NULL, NULL, 0, 0));
    TEST_ASSERT_TRUE(osi_thread_post(btu_thread, dummy_block, NULL, NULL, 0, 0));
    TEST_ASSERT_TRUE(osi_thread_post(btu_thread, dummy_block, NULL, NULL, 0, 0));
    TEST_ASSERT_TRUE(osi_thread_post(btu_thread, dummy_block, NULL, NULL, 0, 0));
    TEST_ASSERT_FALSE(osi_thread_post(btu_thread, dummy_block, NULL, NULL, 0, 0));

    btu_c2h_credit_add(0x00C0, 1);
    btu_c2h_credit_flush();
    TEST_ASSERT_TRUE(wait_task(&s_send_task, TEST_DEFER_COLD_MS + 400));
    TEST_ASSERT_TRUE(hci_c2h_credit_flush_alarm_is_active());
    remaining = hci_c2h_credit_flush_alarm_remaining_ms();
    assert_armed_near(remaining, TEST_RETRY_DELAY_MS);
    TEST_ASSERT_EQUAL_UINT16(1, snapshot_packets(0x00C0));

    c2h_test_end();
}

TEST_CASE("c2h_credit ACL FC buffers leave headroom above high water", "[c2h_credit]")
{
    /* Controller host ACL window == queue capacity; high-water must leave
     * room for fragmenter-held START partials still owed to the controller. */
    TEST_ASSERT_TRUE(L2CAP_HOST_FC_ACL_BUFS >= 100);
    TEST_ASSERT_EQUAL_UINT16(L2CAP_HOST_FC_ACL_BUFS, BTU_ACL_QUEUE_CAPACITY);
    TEST_ASSERT_TRUE(BTU_ACL_QUEUE_HIGH_WATER < BTU_ACL_QUEUE_CAPACITY);
    TEST_ASSERT_TRUE((BTU_ACL_QUEUE_CAPACITY - BTU_ACL_QUEUE_HIGH_WATER) >= 25);
}

#endif /* C2H_FLOW_CONTROL_INCLUDED == TRUE */
