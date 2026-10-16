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

#include "btc/btc_task.h"
#include "common/bt_target.h"

#if (C2H_FLOW_CONTROL_INCLUDED == TRUE)

#include "osi/alarm.h"
#include "osi/thread.h"
#include "stack/bt_types.h"
#include "stack/btu.h"
#include "stack/c2h_credit.h"

#define TEST_WORKER_STACK     3072
#define TEST_WORKER_PRIO      2
#define TEST_MAX_ENTS         C2H_CREDIT_TAB_CAP
#define TEST_RETRY_DELAY_MS   100
#define TEST_DEFER_DELAY_MS   10

extern osi_thread_t *btu_thread;
extern void set_leak_threshold(int threshold);

static SemaphoreHandle_t s_block;

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
    TEST_ASSERT_EQUAL(pdTRUE, xSemaphoreTake(s_block, pdMS_TO_TICKS(2000)));
}

static void c2h_test_begin_q(size_t queue_len)
{
    const size_t workqueue_len[] = {queue_len};

    TEST_ASSERT_EQUAL(0, osi_thread_event_init());
    TEST_ASSERT_EQUAL(0, osi_alarm_create_mux());
    osi_alarm_init();

    /* Pin to core 0: OSI_THREAD_CORE_AFFINITY is not a valid FreeRTOS core id. */
    btu_thread = osi_thread_create("c2h_test", TEST_WORKER_STACK, TEST_WORKER_PRIO,
                                   OSI_THREAD_CORE_0, 1, workqueue_len, false);
    TEST_ASSERT_NOT_NULL(btu_thread);
    TEST_ASSERT_TRUE(c2h_credit_init());
    /* Harness has no HCI command_queue; keep btsnd stub in fail mode by default. */
}

static void c2h_test_begin(void)
{
    c2h_test_begin_q(4);
}

static void c2h_test_end(void)
{
    /* Ensure close() never enqueues into a NULL HCI command_queue. */
    hci_c2h_credit_test_set_btsnd_fail(true);
    c2h_credit_close();
    if (btu_thread != NULL) {
        osi_thread_free(btu_thread);
        btu_thread = NULL;
    }
    c2h_credit_deinit();
    osi_alarm_deinit();
    TEST_ASSERT_EQUAL(0, osi_alarm_delete_mux());
    osi_thread_event_deinit();

    /* Idle task reclaims deleted worker TCBs asynchronously. */
    vTaskDelay(pdMS_TO_TICKS(20));
}

/* Alarm callbacks are dispatched on the BTC thread. */
static void c2h_test_begin_btc(void)
{
    const size_t workqueue_len[] = {4};

    TEST_ASSERT_EQUAL(BT_STATUS_SUCCESS, btc_init());
    TEST_ASSERT_EQUAL(0, osi_alarm_create_mux());
    osi_alarm_init();

    btu_thread = osi_thread_create("c2h_test", TEST_WORKER_STACK, TEST_WORKER_PRIO,
                                   OSI_THREAD_CORE_0, 1, workqueue_len, false);
    TEST_ASSERT_NOT_NULL(btu_thread);
    TEST_ASSERT_TRUE(c2h_credit_init());
}

static void c2h_test_end_btc(void)
{
    hci_c2h_credit_test_set_btsnd_fail(true);
    c2h_credit_close();
    if (btu_thread != NULL) {
        osi_thread_free(btu_thread);
        btu_thread = NULL;
    }
    c2h_credit_deinit();
    osi_alarm_deinit();
    TEST_ASSERT_EQUAL(0, osi_alarm_delete_mux());
    btc_deinit();
    vTaskDelay(pdMS_TO_TICKS(20));
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

TEST_CASE("c2h_credit empty flush does not wake BTU", "[c2h_credit]")
{
    int depth;
    UINT16 num_pkts[TEST_MAX_ENTS];
    UINT16 handles[TEST_MAX_ENTS];

    c2h_test_begin();

    s_block = xSemaphoreCreateCounting(8, 0);
    TEST_ASSERT_NOT_NULL(s_block);
    TEST_ASSERT_TRUE(osi_thread_post(btu_thread, dummy_block, NULL, 0, 0));
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
    TEST_ASSERT_TRUE(osi_thread_post(btu_thread, dummy_block, NULL, 0, 0));
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

    /* close() must stop the timer before osi_alarm_free; otherwise the
     * callback would run after teardown. */
    vTaskDelay(pdMS_TO_TICKS(TEST_RETRY_DELAY_MS + 50));
    TEST_ASSERT_FALSE(hci_c2h_credit_flush_alarm_is_active());

    if (btu_thread != NULL) {
        osi_thread_free(btu_thread);
        btu_thread = NULL;
    }
    c2h_credit_deinit();
    osi_alarm_deinit();
    TEST_ASSERT_EQUAL(0, osi_alarm_delete_mux());
    osi_thread_event_deinit();
    vTaskDelay(pdMS_TO_TICKS(20));
}

TEST_CASE("c2h_credit flush handoff works while BTU queue is busy", "[c2h_credit]")
{
    c2h_test_begin_q(2);

    s_block = xSemaphoreCreateCounting(8, 0);
    TEST_ASSERT_NOT_NULL(s_block);

    TEST_ASSERT_TRUE(osi_thread_post(btu_thread, dummy_block, NULL, 0, 0));
    vTaskDelay(pdMS_TO_TICKS(20));
    TEST_ASSERT_TRUE(osi_thread_post(btu_thread, dummy_block, NULL, 0, 0));
    TEST_ASSERT_TRUE(osi_thread_post(btu_thread, dummy_block, NULL, 0, 0));
    TEST_ASSERT_FALSE(osi_thread_post(btu_thread, dummy_block, NULL, 0, 0));

    /* Send path is on HCI — a full BTU queue must not block credit handoff. */
    TEST_ASSERT_FALSE(hci_c2h_credit_flush_alarm_is_active());
    btu_c2h_credit_post(0x0053, 2);
    TEST_ASSERT_TRUE(hci_c2h_credit_flush_alarm_is_active());

    /*
     * Cancel the 10 ms defer before draining BTU. This harness does not start
     * BTC; an expired alarm would assert in btc_task_post(thread=NULL).
     */
    hci_c2h_credit_test_set_btsnd_fail(true);
    c2h_credit_close();
    TEST_ASSERT_FALSE(hci_c2h_credit_flush_alarm_is_active());

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

TEST_CASE("c2h_credit flush below threshold defers 10 ms", "[c2h_credit]")
{
    UINT32 remaining;

    c2h_test_begin();
    TEST_ASSERT_FALSE(hci_c2h_credit_flush_alarm_is_active());

    btu_c2h_credit_add(0x0070, 1);
    btu_c2h_credit_flush();
    TEST_ASSERT_TRUE(hci_c2h_credit_flush_alarm_is_active());
    remaining = hci_c2h_credit_flush_alarm_remaining_ms();
    TEST_ASSERT_UINT32_WITHIN(8, TEST_DEFER_DELAY_MS, remaining);
    /* Below FLUSH_MIN: stay on host tab (no mail_lock); close before alarm. */
    TEST_ASSERT_EQUAL_UINT16(1, snapshot_packets(0x0070));

    c2h_test_end();
}

TEST_CASE("c2h_credit schedule shortens deadline from retry to defer", "[c2h_credit]")
{
    UINT16 num_pkts[] = {1};
    UINT16 handles[] = {0x0074};
    UINT32 remaining;

    c2h_test_begin();
    TEST_ASSERT_FALSE(hci_c2h_credit_flush_alarm_is_active());

    /* Rollback handoffs and arms the 100 ms retry. */
    btu_c2h_credit_rollback(1, handles, num_pkts);
    TEST_ASSERT_TRUE(hci_c2h_credit_flush_alarm_is_active());
    remaining = hci_c2h_credit_flush_alarm_remaining_ms();
    TEST_ASSERT_UINT32_WITHIN(20, TEST_RETRY_DELAY_MS, remaining);

    /* A later 10 ms defer must pull the deadline earlier. */
    btu_c2h_credit_add(0x0075, 1);
    btu_c2h_credit_flush();
    TEST_ASSERT_TRUE(hci_c2h_credit_flush_alarm_is_active());
    remaining = hci_c2h_credit_flush_alarm_remaining_ms();
    TEST_ASSERT_UINT32_WITHIN(8, TEST_DEFER_DELAY_MS, remaining);

    c2h_test_end();
}

TEST_CASE("c2h_credit flush while deferred coalesces on host tab", "[c2h_credit]")
{
    UINT32 remaining_after_first;
    UINT32 remaining_after_second;

    c2h_test_begin();
    TEST_ASSERT_FALSE(hci_c2h_credit_flush_alarm_is_active());

    btu_c2h_credit_add(0x0073, 1);
    btu_c2h_credit_flush();
    TEST_ASSERT_TRUE(hci_c2h_credit_flush_alarm_is_active());
    remaining_after_first = hci_c2h_credit_flush_alarm_remaining_ms();
    TEST_ASSERT_UINT32_WITHIN(8, TEST_DEFER_DELAY_MS, remaining_after_first);

    vTaskDelay(pdMS_TO_TICKS(4));
    remaining_after_first = hci_c2h_credit_flush_alarm_remaining_ms();
    TEST_ASSERT_TRUE(remaining_after_first < TEST_DEFER_DELAY_MS);

    btu_c2h_credit_add(0x0073, 1);
    btu_c2h_credit_flush();
    TEST_ASSERT_TRUE(hci_c2h_credit_flush_alarm_is_active());
    remaining_after_second = hci_c2h_credit_flush_alarm_remaining_ms();
    TEST_ASSERT_TRUE(remaining_after_second <= remaining_after_first);
    TEST_ASSERT_TRUE(remaining_after_second < TEST_DEFER_DELAY_MS);
    /* Still below FLUSH_MIN: both credits remain on host (no handoff yet). */
    TEST_ASSERT_EQUAL_UINT16(2, snapshot_packets(0x0073));

    c2h_test_end();
}

TEST_CASE("c2h_credit BTU flush defers; HCI batch flush sends at threshold", "[c2h_credit]")
{
    UINT32 remaining = 0;

    c2h_test_begin();

    btu_c2h_credit_add(0x0071, 4);
    btu_c2h_credit_flush();
    TEST_ASSERT_TRUE(hci_c2h_credit_flush_alarm_is_active());
    remaining = hci_c2h_credit_flush_alarm_remaining_ms();
    TEST_ASSERT_UINT32_WITHIN(8, TEST_DEFER_DELAY_MS, remaining);

    /* RX batch flush merges mail and sends (>= FLUSH_MIN). */
    hci_c2h_credit_flush();
    if (hci_c2h_credit_flush_alarm_is_active()) {
        remaining = hci_c2h_credit_flush_alarm_remaining_ms();
        TEST_ASSERT_TRUE(remaining > 20);
    }

    c2h_test_end();
}

TEST_CASE("c2h_credit flush_and_purge sends without the 10 ms window", "[c2h_credit]")
{
    UINT32 remaining;

    c2h_test_begin();

    btu_c2h_credit_add(0x0072, 1);
    btu_c2h_credit_flush();
    TEST_ASSERT_TRUE(hci_c2h_credit_flush_alarm_is_active());
    remaining = hci_c2h_credit_flush_alarm_remaining_ms();
    TEST_ASSERT_UINT32_WITHIN(8, TEST_DEFER_DELAY_MS, remaining);

    btu_c2h_credit_flush_and_purge(0x0072);

    if (hci_c2h_credit_flush_alarm_is_active()) {
        remaining = hci_c2h_credit_flush_alarm_remaining_ms();
        TEST_ASSERT_TRUE(remaining > 20);
    }

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
    TEST_ASSERT_UINT32_WITHIN(8, TEST_DEFER_DELAY_MS, remaining);

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
    TEST_ASSERT_UINT32_WITHIN(8, TEST_DEFER_DELAY_MS, remaining_after_first);

    vTaskDelay(pdMS_TO_TICKS(4));
    remaining_after_first = hci_c2h_credit_flush_alarm_remaining_ms();
    TEST_ASSERT_TRUE(remaining_after_first < TEST_DEFER_DELAY_MS);

    hci_c2h_credit_account(0x0081, 1);
    hci_c2h_credit_flush();
    TEST_ASSERT_TRUE(hci_c2h_credit_flush_alarm_is_active());
    remaining_after_second = hci_c2h_credit_flush_alarm_remaining_ms();
    TEST_ASSERT_TRUE(remaining_after_second <= remaining_after_first);
    TEST_ASSERT_TRUE(remaining_after_second < TEST_DEFER_DELAY_MS);

    c2h_test_end();
}

TEST_CASE("c2h_credit hci flush at threshold is not deferred", "[c2h_credit]")
{
    UINT32 remaining = 0;

    c2h_test_begin();

    hci_c2h_credit_account(0x0082, 4);
    hci_c2h_credit_flush();

    if (hci_c2h_credit_flush_alarm_is_active()) {
        remaining = hci_c2h_credit_flush_alarm_remaining_ms();
        TEST_ASSERT_TRUE(remaining > 20);
    }

    c2h_test_end();
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

    if (btu_thread != NULL) {
        osi_thread_free(btu_thread);
        btu_thread = NULL;
    }
    c2h_credit_deinit();
    osi_alarm_deinit();
    TEST_ASSERT_EQUAL(0, osi_alarm_delete_mux());
    osi_thread_event_deinit();
    vTaskDelay(pdMS_TO_TICKS(20));
}

TEST_CASE("c2h_credit init fails without a BTU thread", "[c2h_credit]")
{
    TEST_ASSERT_EQUAL(0, osi_thread_event_init());
    TEST_ASSERT_EQUAL(0, osi_alarm_create_mux());
    osi_alarm_init();

    /* No hciT and no BTU worker: flush_ready cannot bind, init must fail. */
    btu_thread = NULL;
    TEST_ASSERT_FALSE(c2h_credit_init());
    c2h_credit_close();
    c2h_credit_deinit();

    osi_alarm_deinit();
    TEST_ASSERT_EQUAL(0, osi_alarm_delete_mux());
    osi_thread_event_deinit();
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

    hci_c2h_credit_test_set_btsnd_fail(false);
    if (btu_thread != NULL) {
        osi_thread_free(btu_thread);
        btu_thread = NULL;
    }
    c2h_credit_deinit();
    osi_alarm_deinit();
    TEST_ASSERT_EQUAL(0, osi_alarm_delete_mux());
    osi_thread_event_deinit();
    vTaskDelay(pdMS_TO_TICKS(20));
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
    if (btu_thread != NULL) {
        osi_thread_free(btu_thread);
        btu_thread = NULL;
    }
    btu_c2h_credit_deinit();
    osi_alarm_deinit();
    TEST_ASSERT_EQUAL(0, osi_alarm_delete_mux());
    osi_thread_event_deinit();
    vTaskDelay(pdMS_TO_TICKS(20));
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
