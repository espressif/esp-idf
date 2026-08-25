/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <inttypes.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ble_log.h"
#include "ble_log_lbm.h"
#include "ble_log_prph_test.h"
#include "ble_log_rt.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/task.h"
#include "test_ble_log_main.h"
#include "unity.h"

#define RT_SAMPLE_COUNT             (32)
#define RT_BURST_SIZE               (4)
#define RT_TASK_POOL_TRANS_COUNT    ((BLE_LOG_LBM_ATOMIC_TASK_CNT + 1) * BLE_LOG_TRANS_BUF_CNT)
#define RT_READ_TIMEOUT_MS          (100)
#define RT_QUIET_TIMEOUT_MS         (10)
#define RT_QUIET_DRAIN_DEADLINE_MS  (2000)
#define RT_EXPECTED_DEFER_US        (1000)
#define RT_MARKER_MAGIC             UINT32_C(0x52545046)
#define RT_USER_PAYLOAD_LEN         (BLE_LOG_TRANS_SIZE - sizeof(uint32_t) - BLE_LOG_FRAME_OVERHEAD)
#define RT_STARVATION_FEEDBACK      (16384)
#define RT_HEARTBEAT_DELAY_US       (2000)
#define RT_HEARTBEAT_MAX_ELAPSED_US (10000)
#define RT_CONSUMER_DELAY_MS        (30)
#define RT_RECEIVE_MAX_LATENCY_US   (10000)
#define RT_BURST_SPAN_MAX_US        (500)
#define RT_PEAK_WRITES              (8)
#define RT_DEINIT_ROUNDS            (200)
#define RT_JOIN_TIMEOUT_MS          (5000)

typedef struct {
    uint32_t magic;
    uint32_t seq;
} rt_marker_t;

typedef struct {
    bool found;
    uint32_t seq;
    uint32_t ts_count;
} rt_marker_observer_t;

typedef struct {
    SemaphoreHandle_t done;
    uint32_t remaining;
    bool stop;
    bool write_failed;
    int64_t fired_us;
} rt_starvation_ctx_t;

typedef struct {
    uint32_t stop;
    uint32_t exited;
    uint32_t attempts;
} rt_deinit_writer_ctx_t;

typedef struct {
    uint32_t buf_util_frames;
    uint32_t max_inflight_peak;
    bool over_limit;
} rt_peak_observer_t;

_Static_assert(RT_USER_PAYLOAD_LEN >= sizeof(rt_marker_t),
               "BLE Log transport is too small for the runtime test marker");

static uint8_t s_payload[RT_USER_PAYLOAD_LEN];
static uint8_t s_capture[BLE_LOG_TRANS_SIZE];
static uint32_t s_single_latency_us[RT_SAMPLE_COUNT];
static uint32_t s_burst_first_latency_us[RT_SAMPLE_COUNT];
static uint32_t s_burst_last_latency_us[RT_SAMPLE_COUNT];
static rt_starvation_ctx_t s_starvation;
/* File-scope so a writer that outlives the test never references a dead
 * stack frame (same pattern as s_starvation). */
static rt_deinit_writer_ctx_t s_deinit_race;

static void prepare_payload(uint32_t seq)
{
    rt_marker_t marker = {
        .magic = RT_MARKER_MAGIC,
        .seq = seq,
    };

    memset(s_payload, (uint8_t)seq, sizeof(s_payload));
    memcpy(s_payload, &marker, sizeof(marker));
}

static void observe_runtime_marker(const test_ble_log_frame_t *frame, void *ctx)
{
    rt_marker_observer_t *observer = ctx;
    if (frame->src == BLE_LOG_SRC_INTERNAL &&
            frame->payload_len > sizeof(uint32_t) &&
            frame->payload[sizeof(uint32_t)] == BLE_LOG_INT_SRC_TS) {
        observer->ts_count++;
    }

    if (frame->src != BLE_LOG_SRC_CUSTOM ||
            frame->payload_len < sizeof(uint32_t) + sizeof(rt_marker_t)) {
        return;
    }

    rt_marker_t marker;
    memcpy(&marker, frame->payload + sizeof(uint32_t), sizeof(marker));
    if (marker.magic == RT_MARKER_MAGIC) {
        observer->found = true;
        observer->seq = marker.seq;
    }
}

static TickType_t runtime_timeout_ticks(uint32_t timeout_ms)
{
    uint64_t ticks = ((uint64_t)timeout_ms * configTICK_RATE_HZ + 999) / 1000;
    if (ticks == 0) {
        ticks = 1;
    }
    return ticks > portMAX_DELAY ? portMAX_DELAY : (TickType_t)ticks;
}

static bool runtime_deadline_ticks(int64_t deadline_us, TickType_t *ticks)
{
    int64_t now_us = esp_timer_get_time();
    if (now_us >= deadline_us) {
        return false;
    }
    uint32_t remain_ms = (uint32_t)((deadline_us - now_us + 999) / 1000);
    if (remain_ms == 0) {
        remain_ms = 1;
    }
    *ticks = runtime_timeout_ticks(remain_ms);
    return true;
}

static bool write_runtime_marker(uint32_t seq, int64_t *enqueued_at_us)
{
    prepare_payload(seq);
    /* Include LBM packing and queueing in the measured runtime latency. */
    int64_t before_us = esp_timer_get_time();
    if (!ble_log_write_hex(BLE_LOG_SRC_CUSTOM, s_payload, sizeof(s_payload))) {
        return false;
    }
    if (enqueued_at_us) {
        *enqueued_at_us = before_us;
    }
    return true;
}

static bool read_runtime_marker(uint32_t *seq, uint32_t *ts_count,
                                int64_t *received_at_us)
{
    const int64_t deadline_us = esp_timer_get_time() +
                                (int64_t)RT_READ_TIMEOUT_MS * 1000;
    uint32_t observed_ts = 0;

    while (true) {
        TickType_t remaining;
        if (!runtime_deadline_ticks(deadline_us, &remaining)) {
            return false;
        }
        int64_t transport_received_at_us;
        size_t len = ble_log_prph_test_read(s_capture, sizeof(s_capture), remaining, 0,
                                            &transport_received_at_us);
        if (!len) {
            continue;
        }

        rt_marker_observer_t observer = {0};
        TEST_ASSERT_TRUE_MESSAGE(test_ble_log_walk_frames(s_capture, len,
                                                         observe_runtime_marker,
                                                         &observer),
                                 "Runtime dispatch produced an invalid transport");
        observed_ts += observer.ts_count;
        if (observer.found) {
            *seq = observer.seq;
            if (ts_count) {
                *ts_count = observed_ts;
            }
            if (received_at_us) {
                *received_at_us = transport_received_at_us;
            }
            return true;
        }
    }
}

static bool runtime_stream_is_quiet(void)
{
    const int64_t drain_deadline_us = esp_timer_get_time() +
                                      (int64_t)RT_QUIET_DRAIN_DEADLINE_MS * 1000;

    while (esp_timer_get_time() < drain_deadline_us) {
        const int64_t gap_deadline_us = esp_timer_get_time() +
                                        (int64_t)RT_QUIET_TIMEOUT_MS * 1000;
        size_t len = 0;
        while (esp_timer_get_time() < gap_deadline_us) {
            TickType_t remaining;
            if (!runtime_deadline_ticks(gap_deadline_us, &remaining)) {
                break;
            }
            len = ble_log_prph_test_read(s_capture, sizeof(s_capture), remaining, 0, NULL);
            if (len) {
                break;
            }
        }
        if (!len) {
            return true;
        }

        rt_marker_observer_t observer = {0};
        if (!test_ble_log_walk_frames(s_capture, len, observe_runtime_marker, &observer) ||
                observer.found) {
            return false;
        }
    }
    return false;
}

static void refill_runtime_queue(void *arg)
{
    rt_starvation_ctx_t *ctx = arg;
    if (ctx->stop || !ctx->remaining) {
        return;
    }

    ctx->remaining--;
    if (!ble_log_write_hex(BLE_LOG_SRC_CUSTOM, s_payload, sizeof(s_payload))) {
        ctx->write_failed = true;
    }
}

static void heartbeat_cb(void *arg)
{
    rt_starvation_ctx_t *ctx = arg;
    ctx->fired_us = esp_timer_get_time();
    ctx->stop = true;
    xSemaphoreGive(ctx->done);
}

#if CONFIG_ESP_TIMER_SUPPORTS_ISR_DISPATCH_METHOD
static void BLE_LOG_IRAM_ATTR isr_write_cb(void *arg)
{
    (void)arg;
    (void)ble_log_write_hex(BLE_LOG_SRC_CUSTOM, s_payload, sizeof(s_payload));
}
#endif

static void noop_callback(void *arg)
{
    (void)arg;
}

static void deinit_writer_task(void *arg)
{
    rt_deinit_writer_ctx_t *ctx = arg;
    while (!__atomic_load_n(&ctx->stop, __ATOMIC_ACQUIRE)) {
        ctx->attempts++;
        (void)ble_log_write_hex(BLE_LOG_SRC_CUSTOM, s_payload, sizeof(s_payload));
        taskYIELD();
    }
    __atomic_store_n(&ctx->exited, true, __ATOMIC_RELEASE);
    vTaskDelete(NULL);
}

static void observe_buf_util(const test_ble_log_frame_t *frame, void *ctx)
{
    rt_peak_observer_t *observer = ctx;
    if (frame->src != BLE_LOG_SRC_INTERNAL ||
            frame->payload_len < sizeof(uint32_t) + sizeof(ble_log_buf_util_t) ||
            frame->payload[sizeof(uint32_t)] != BLE_LOG_INT_SRC_BUF_UTIL) {
        return;
    }

    ble_log_buf_util_t util;
    memcpy(&util, frame->payload + sizeof(uint32_t), sizeof(util));
    observer->buf_util_frames++;
    if (util.inflight_peak > observer->max_inflight_peak) {
        observer->max_inflight_peak = util.inflight_peak;
    }
    if (util.inflight_peak > util.trans_cnt) {
        observer->over_limit = true;
    }
}

static int compare_u32(const void *lhs, const void *rhs)
{
    uint32_t a = *(const uint32_t *)lhs;
    uint32_t b = *(const uint32_t *)rhs;
    return (a > b) - (a < b);
}

static uint32_t percentile(const uint32_t *sorted, size_t count, uint32_t percent)
{
    size_t rank = (count * percent + 99) / 100;
    return sorted[rank - 1];
}

static void print_latency_stats(const char *mode, uint32_t batch, uint32_t *samples)
{
    uint64_t total = 0;
    for (size_t i = 0; i < RT_SAMPLE_COUNT; i++) {
        total += samples[i];
    }
    qsort(samples, RT_SAMPLE_COUNT, sizeof(samples[0]), compare_u32);

    printf("BLE_LOG_RT_PERF mode=%s samples=%u batch=%u payload=%uB "
           "min=%" PRIu32 "us avg=%" PRIu64 "us p50=%" PRIu32
           "us p95=%" PRIu32 "us max=%" PRIu32 "us\n",
           mode, (unsigned)RT_SAMPLE_COUNT, (unsigned)batch,
           (unsigned)sizeof(s_payload), samples[0], total / RT_SAMPLE_COUNT,
           percentile(samples, RT_SAMPLE_COUNT, 50),
           percentile(samples, RT_SAMPLE_COUNT, 95),
           samples[RT_SAMPLE_COUNT - 1]);
}

static void warm_up_runtime(void)
{
    uint32_t received_seq;
    prepare_payload(0);
    TEST_ASSERT_TRUE(ble_log_write_hex(BLE_LOG_SRC_CUSTOM,
                                       s_payload, sizeof(s_payload)));
    TEST_ASSERT_TRUE_MESSAGE(read_runtime_marker(&received_seq, NULL, NULL),
                             "Timed out waiting for runtime warm-up dispatch");
    TEST_ASSERT_EQUAL_UINT32(0, received_seq);
    TEST_ASSERT_TRUE(runtime_stream_is_quiet());
}

TEST_CASE("BLE Log runtime millisecond waits remain nonzero",
          "[ble_log][runtime][ignore]")
{
    TEST_ASSERT_GREATER_THAN_UINT32(0, runtime_timeout_ticks(1));
    TEST_ASSERT_GREATER_THAN_UINT32(
        0, runtime_timeout_ticks(RT_QUIET_TIMEOUT_MS));
    TEST_ASSERT_GREATER_THAN_UINT32(
        0, runtime_timeout_ticks(RT_READ_TIMEOUT_MS));

    const int64_t start_us = esp_timer_get_time();
    const int64_t deadline_us = start_us + (int64_t)RT_READ_TIMEOUT_MS * 1000;
    TickType_t ticks;
    while (runtime_deadline_ticks(deadline_us, &ticks)) {
        vTaskDelay(ticks);
    }
    TEST_ASSERT_TRUE_MESSAGE(
        esp_timer_get_time() - start_us >= (int64_t)RT_READ_TIMEOUT_MS * 1000,
        "Millisecond timeout returned before the requested deadline");
}

TEST_CASE("BLE Log runtime quiet check rejects an extra marker",
          "[ble_log][runtime][ignore]")
{
    const uint32_t first_seq = UINT32_C(0x35000);
    uint32_t received_seq;

    TEST_ASSERT_TRUE(ble_log_enable(true));
    (void)runtime_stream_is_quiet();
    prepare_payload(first_seq);
    TEST_ASSERT_TRUE(ble_log_write_hex(BLE_LOG_SRC_CUSTOM,
                                       s_payload, sizeof(s_payload)));
    prepare_payload(first_seq + 1);
    TEST_ASSERT_TRUE(ble_log_write_hex(BLE_LOG_SRC_CUSTOM,
                                       s_payload, sizeof(s_payload)));

    TEST_ASSERT_TRUE(read_runtime_marker(&received_seq, NULL, NULL));
    TEST_ASSERT_EQUAL_UINT32(first_seq, received_seq);
    bool quiet = runtime_stream_is_quiet();

    TEST_ASSERT_FALSE_MESSAGE(quiet,
                              "Quiet check silently discarded an extra runtime marker");
}

TEST_CASE("BLE Log runtime latency excludes consumer delay",
          "[ble_log][runtime][ignore]")
{
    const uint32_t seq = UINT32_C(0x36000);
    uint32_t received_seq;
    int64_t received_at_us;

    TEST_ASSERT_TRUE(ble_log_enable(true));
    warm_up_runtime();
    int64_t start_us;
    TEST_ASSERT_TRUE(write_runtime_marker(seq, &start_us));
    const int64_t delay_deadline_us = esp_timer_get_time() +
                                      (int64_t)RT_CONSUMER_DELAY_MS * 1000;
    TickType_t delay_ticks;
    while (runtime_deadline_ticks(delay_deadline_us, &delay_ticks)) {
        vTaskDelay(delay_ticks);
    }
    TEST_ASSERT_TRUE(read_runtime_marker(&received_seq, NULL, &received_at_us));
    uint32_t measured_latency_us = (uint32_t)(received_at_us - start_us);
    TEST_ASSERT_TRUE(runtime_stream_is_quiet());

    TEST_ASSERT_EQUAL_UINT32(seq, received_seq);
    TEST_ASSERT_TRUE(received_at_us >= start_us);
    TEST_ASSERT_LESS_THAN_UINT32_MESSAGE(
        RT_RECEIVE_MAX_LATENCY_US, measured_latency_us,
        "Runtime latency included time spent waiting for the consumer");
}

TEST_CASE("BLE Log ISR-only submission arms runtime dispatch",
          "[ble_log][runtime][ignore]")
{
#if !CONFIG_ESP_TIMER_SUPPORTS_ISR_DISPATCH_METHOD
    TEST_IGNORE_MESSAGE("Requires ESP Timer ISR dispatch support");
#else
    const uint32_t seq = UINT32_C(0x28000);
    uint32_t received_seq = 0;

    TEST_ASSERT_TRUE(ble_log_enable(true));
    warm_up_runtime();
    prepare_payload(seq);

    esp_timer_handle_t isr_timer = NULL;
    const esp_timer_create_args_t timer_args = {
        .callback = isr_write_cb,
        .dispatch_method = ESP_TIMER_ISR,
        .name = "ble_log_isr_write",
    };
    TEST_ASSERT_EQUAL(ESP_OK, esp_timer_create(&timer_args, &isr_timer));

    esp_err_t start_err = esp_timer_start_once(isr_timer, 1);
    bool received = start_err == ESP_OK &&
                    read_runtime_marker(&received_seq, NULL, NULL);
    TEST_ASSERT_EQUAL(ESP_OK,
                      esp_timer_stop_blocking(isr_timer, portMAX_DELAY));
    TEST_ASSERT_EQUAL(ESP_OK, esp_timer_delete(isr_timer));

    TEST_ASSERT_EQUAL(ESP_OK, start_err);
    TEST_ASSERT_TRUE_MESSAGE(received,
                             "ISR submission did not arm runtime dispatch");
    TEST_ASSERT_EQUAL_UINT32(seq, received_seq);
    TEST_ASSERT_TRUE(runtime_stream_is_quiet());
#endif
}

#if CONFIG_BLE_LOG_TS_ENABLED
TEST_CASE("BLE Log periodic timestamp skips light sleep wakeups",
          "[ble_log][runtime][timestamp][ignore]")
{
    const uint32_t seq = UINT32_C(0x40000);
    uint32_t received_seq;
    uint32_t ts_count = 0;
    esp_timer_handle_t wake_probe_timer = NULL;
    const esp_timer_create_args_t wake_probe_args = {
        .callback = noop_callback,
        .dispatch_method = ESP_TIMER_TASK,
        .name = "ble_log_wake_probe",
    };

    TEST_ESP_OK(esp_timer_create(&wake_probe_args, &wake_probe_timer));
    int64_t probe_start_us = esp_timer_get_time();
    TEST_ESP_OK(esp_timer_start_once(
        wake_probe_timer, BLE_LOG_TS_TRIGGER_TIMEOUT_US * 3 / 2));
    int64_t next_wake_us = esp_timer_get_next_alarm_for_wake_up();
    TEST_ESP_OK(esp_timer_stop(wake_probe_timer));
    TEST_ESP_OK(esp_timer_delete(wake_probe_timer));

    TEST_ASSERT_TRUE_MESSAGE(next_wake_us != INT64_MAX,
                             "Wake-capable probe timer was not scheduled");
    TEST_ASSERT_GREATER_THAN_INT64_MESSAGE(
        BLE_LOG_TS_TRIGGER_TIMEOUT_US * 5 / 4,
        next_wake_us - probe_start_us,
        "Periodic timestamp timer was selected to wake light sleep");

    TEST_ASSERT_TRUE(ble_log_enable(true));
    TEST_ASSERT_TRUE(runtime_stream_is_quiet());
    TEST_ASSERT_TRUE(ble_log_sync_enable(true));
    for (int i = 0; i < 3; i++) {
        vTaskDelay(runtime_timeout_ticks(CONFIG_BLE_LOG_TS_TRIGGER_TIMEOUT_MS));
    }
    TEST_ASSERT_TRUE(ble_log_sync_enable(false));

    /* A full marker rolls the partial timestamp transport through the normal
     * LBM submission path without making runtime dispatch the TS trigger. */
    TEST_ASSERT_TRUE(write_runtime_marker(seq, NULL));
    TEST_ASSERT_TRUE_MESSAGE(read_runtime_marker(&received_seq, &ts_count, NULL),
                             "Timed out waiting for the periodic timestamp probe");
    TEST_ASSERT_EQUAL_UINT32(seq, received_seq);
    TEST_ASSERT_TRUE(runtime_stream_is_quiet());
    TEST_ASSERT_GREATER_THAN_UINT32_MESSAGE(
        0, ts_count,
        "Periodic ESP timer did not emit a timestamp frame");
}
#endif

TEST_CASE("BLE Log runtime dispatch yields to other timer callbacks",
          "[ble_log][runtime][ignore]")
{
    TEST_ASSERT_TRUE(ble_log_enable(true));
    warm_up_runtime();

    memset(&s_starvation, 0, sizeof(s_starvation));
    s_starvation.done = xSemaphoreCreateBinary();
    s_starvation.remaining = RT_STARVATION_FEEDBACK;
    TEST_ASSERT_NOT_NULL(s_starvation.done);

    esp_timer_handle_t heartbeat_timer = NULL;
    const esp_timer_create_args_t timer_args = {
        .callback = heartbeat_cb,
        .arg = &s_starvation,
        .dispatch_method = ESP_TIMER_TASK,
        .name = "ble_log_heartbeat",
        .skip_unhandled_events = true,
    };
    TEST_ASSERT_EQUAL(ESP_OK, esp_timer_create(&timer_args, &heartbeat_timer));

    prepare_payload(UINT32_C(0x30000));
    ble_log_prph_test_set_auto_recycle_hook(refill_runtime_queue, &s_starvation);
    int64_t start_us = esp_timer_get_time();
    esp_err_t start_err = esp_timer_start_once(heartbeat_timer,
                                               RT_HEARTBEAT_DELAY_US);
    bool wrote = start_err == ESP_OK &&
                 ble_log_write_hex(BLE_LOG_SRC_CUSTOM, s_payload,
                                   sizeof(s_payload));
    bool heartbeat_fired = false;
    if (wrote) {
        heartbeat_fired =
            xSemaphoreTake(s_starvation.done, runtime_timeout_ticks(1000)) == pdTRUE;
    }
    ble_log_prph_test_set_auto_recycle_hook(NULL, NULL);
    esp_timer_stop_blocking(heartbeat_timer, portMAX_DELAY);
    TEST_ASSERT_EQUAL(ESP_OK, esp_timer_delete(heartbeat_timer));
    vSemaphoreDelete(s_starvation.done);
    (void)runtime_stream_is_quiet();
    TEST_ASSERT_EQUAL(ESP_OK, start_err);
    TEST_ASSERT_TRUE_MESSAGE(wrote, "Feedback seed write failed");

    TEST_ASSERT_TRUE_MESSAGE(heartbeat_fired,
                             "Shared ESP timer callback never got CPU time");
    TEST_ASSERT_TRUE_MESSAGE(
        s_starvation.remaining < RT_STARVATION_FEEDBACK,
        "Fairness test did not create feedback load");
    TEST_ASSERT_FALSE_MESSAGE(s_starvation.write_failed,
                              "Feedback write unexpectedly failed");
    TEST_ASSERT_TRUE_MESSAGE(s_starvation.fired_us >= start_us,
                             "Heartbeat fired before feedback load started");
    TEST_ASSERT_TRUE_MESSAGE(
        s_starvation.fired_us - start_us < RT_HEARTBEAT_MAX_ELAPSED_US,
        "Runtime dispatch monopolized the shared ESP timer task");
}

TEST_CASE("BLE Log runtime defer timer keeps its first deadline and wakes light sleep",
          "[ble_log][runtime][ignore]")
{
#if !CONFIG_FREERTOS_UNICORE
    TEST_IGNORE_MESSAGE("Requires single-core scheduler suspension");
#else
    const uint32_t base_seq = UINT32_C(0x38000);

    TEST_ASSERT_TRUE(ble_log_enable(true));
    warm_up_runtime();

    bool wrote = true;
    vTaskSuspendAll();
    int64_t wake_before = esp_timer_get_next_alarm_for_wake_up();
    prepare_payload(base_seq);
    int64_t first_write_entry_us = esp_timer_get_time();
    wrote = ble_log_write_hex(BLE_LOG_SRC_CUSTOM, s_payload, sizeof(s_payload));
    int64_t first_write_return_us = esp_timer_get_time();
    int64_t first_defer_wake_us = esp_timer_get_next_alarm_for_wake_up();
    for (uint32_t i = 1; i < RT_BURST_SIZE; i++) {
        wrote = wrote && write_runtime_marker(base_seq + i, NULL);
    }
    int64_t burst_defer_wake_us = esp_timer_get_next_alarm_for_wake_up();
    (void)xTaskResumeAll();

    TEST_ASSERT_TRUE(wrote);
    for (uint32_t i = 0; i < RT_BURST_SIZE; i++) {
        uint32_t received_seq;
        TEST_ASSERT_TRUE_MESSAGE(read_runtime_marker(&received_seq, NULL, NULL),
                                 "Timed out waiting for defer-wake probe");
        TEST_ASSERT_EQUAL_UINT32(base_seq + i, received_seq);
    }
    TEST_ASSERT_TRUE(runtime_stream_is_quiet());

    TEST_ASSERT_TRUE_MESSAGE(first_defer_wake_us != INT64_MAX,
                             "Defer timer was not scheduled as a light-sleep wake source");
    TEST_ASSERT_TRUE_MESSAGE(first_defer_wake_us < wake_before,
                             "Defer timer was not the newly scheduled wake alarm");
    TEST_ASSERT_GREATER_OR_EQUAL_INT64_MESSAGE(
        first_write_entry_us + RT_EXPECTED_DEFER_US, first_defer_wake_us,
        "Defer timer was armed earlier than the fixed 1 ms delay");
    TEST_ASSERT_LESS_OR_EQUAL_INT64_MESSAGE(
        first_write_return_us + RT_EXPECTED_DEFER_US, first_defer_wake_us,
        "Defer timer was armed later than the fixed 1 ms delay");
    TEST_ASSERT_EQUAL_INT64_MESSAGE(
        first_defer_wake_us, burst_defer_wake_us,
        "Burst submissions moved the first defer deadline");
#endif
}

TEST_CASE("BLE Log runtime dispatch latency", "[ble_log][runtime][perf][ignore]")
{
    TEST_ASSERT_TRUE(ble_log_enable(true));
    warm_up_runtime();

    for (uint32_t sample = 0; sample < RT_SAMPLE_COUNT; sample++) {
        uint32_t seq = UINT32_C(0x10000) + sample;
        uint32_t received_seq;
        int64_t received_at_us;
        int64_t start_us;

        TEST_ASSERT_TRUE(write_runtime_marker(seq, &start_us));
        TEST_ASSERT_TRUE_MESSAGE(read_runtime_marker(&received_seq, NULL,
                                                     &received_at_us),
                                 "Timed out waiting for a single runtime dispatch");
        TEST_ASSERT_EQUAL_UINT32(seq, received_seq);
        TEST_ASSERT_TRUE(received_at_us >= start_us);
        s_single_latency_us[sample] = (uint32_t)(received_at_us - start_us);
    }

    TEST_ASSERT_TRUE_MESSAGE(runtime_stream_is_quiet(),
                             "Unexpected runtime marker after single samples");

    for (uint32_t sample = 0; sample < RT_SAMPLE_COUNT; sample++) {
        uint32_t base_seq = UINT32_C(0x20000) + sample * RT_BURST_SIZE;
        int64_t start_us[RT_BURST_SIZE];

        for (uint32_t i = 0; i < RT_BURST_SIZE; i++) {
            TEST_ASSERT_TRUE(write_runtime_marker(base_seq + i, &start_us[i]));
        }

        for (uint32_t i = 0; i < RT_BURST_SIZE; i++) {
            uint32_t received_seq;
            int64_t received_at_us;
            TEST_ASSERT_TRUE_MESSAGE(read_runtime_marker(&received_seq, NULL,
                                                         &received_at_us),
                                     "Timed out waiting for a burst runtime dispatch");
            TEST_ASSERT_TRUE(received_at_us >= start_us[i]);
            if (i == 0) {
                s_burst_first_latency_us[sample] =
                    (uint32_t)(received_at_us - start_us[i]);
            }
            if (i == RT_BURST_SIZE - 1) {
                s_burst_last_latency_us[sample] =
                    (uint32_t)(received_at_us - start_us[i]);
            }
            TEST_ASSERT_EQUAL_UINT32(base_seq + i, received_seq);
        }
        TEST_ASSERT_TRUE_MESSAGE(runtime_stream_is_quiet(),
                                 "Unexpected runtime marker after burst sample");
    }

    print_latency_stats("single", 1, s_single_latency_us);
    print_latency_stats("burst_first", RT_BURST_SIZE, s_burst_first_latency_us);
    print_latency_stats("burst_last", RT_BURST_SIZE, s_burst_last_latency_us);
}

TEST_CASE("BLE Log runtime drains the full task pool in one batch",
          "[ble_log][runtime][ignore]")
{
#if !CONFIG_FREERTOS_UNICORE
    TEST_IGNORE_MESSAGE("Requires dispatch exclusion during enqueue (single core)");
#else
    const uint32_t base_seq = UINT32_C(0x50000);
    int64_t first_received_us = 0;
    int64_t last_received_us = 0;

    TEST_ASSERT_TRUE(ble_log_enable(true));

    /* Flush from this ordinary test task so every task-pool transport is free
     * before constructing the callback-entry snapshot. */
    ble_log_prph_test_set_auto_recycle_hook(noop_callback, NULL);
    ble_log_flush();
    ble_log_prph_test_set_auto_recycle_hook(NULL, NULL);
    TEST_ASSERT_TRUE(runtime_stream_is_quiet());

    /* Fill every transport reachable from ordinary task context before the
     * first callback. The callback must drain the complete entry snapshot
     * without an artificial item cap. */
    bool wrote = true;
    vTaskSuspendAll();
    for (uint32_t i = 0; i < RT_TASK_POOL_TRANS_COUNT; i++) {
        prepare_payload(base_seq + i);
        wrote = wrote &&
                ble_log_write_hex(BLE_LOG_SRC_CUSTOM, s_payload, sizeof(s_payload));
    }
    (void)xTaskResumeAll();
    TEST_ASSERT_TRUE_MESSAGE(wrote, "Full task-pool enqueue failed");

    for (uint32_t i = 0; i < RT_TASK_POOL_TRANS_COUNT; i++) {
        uint32_t received_seq;
        int64_t received_at_us;
        TEST_ASSERT_TRUE_MESSAGE(read_runtime_marker(&received_seq, NULL,
                                                     &received_at_us),
                                 "Timed out waiting for a batched burst dispatch");
        TEST_ASSERT_EQUAL_UINT32(base_seq + i, received_seq);
        if (i == 0) {
            first_received_us = received_at_us;
        }
        last_received_us = received_at_us;
    }
    TEST_ASSERT_TRUE(runtime_stream_is_quiet());

    TEST_ASSERT_LESS_THAN_INT64_MESSAGE(
        RT_BURST_SPAN_MAX_US, last_received_us - first_received_us,
        "Burst was split across multiple dispatch callbacks "
        "(per-item re-defer instead of one batch)");
#endif
}

TEST_CASE("BLE Log LBM inflight peak stays bounded under bursts",
          "[ble_log][runtime][ignore]")
{
    rt_peak_observer_t observer = {0};

    TEST_ASSERT_TRUE(ble_log_enable(true));
    TEST_ASSERT_TRUE(runtime_stream_is_quiet());

    /* Queue several transports without consuming them so LBM transports are
     * submitted while earlier ones are still in flight. */
    for (uint32_t i = 0; i < RT_PEAK_WRITES; i++) {
        uint32_t seq = UINT32_C(0x80000) + i;
        TEST_ASSERT_TRUE(write_runtime_marker(seq, NULL));
    }

    /* Snapshot the recorded peaks. A full marker forces the partial BUF_UTIL
     * transport to roll over through the normal LBM submission path. */
    ble_log_write_buf_util();
    TEST_ASSERT_TRUE(write_runtime_marker(UINT32_C(0x81000), NULL));
    TEST_ASSERT_TRUE(ble_log_rt_drain());

    const int64_t deadline_us = esp_timer_get_time() +
                                (int64_t)RT_READ_TIMEOUT_MS * 1000;
    while (true) {
        TickType_t remaining;
        if (!runtime_deadline_ticks(deadline_us, &remaining)) {
            break;
        }
        size_t len = ble_log_prph_test_read(s_capture, sizeof(s_capture),
                                            remaining, 0, NULL);
        if (!len) {
            break;
        }
        TEST_ASSERT_TRUE(test_ble_log_walk_frames(s_capture, len,
                                                  observe_buf_util, &observer));
    }
    TEST_ASSERT_TRUE(runtime_stream_is_quiet());

    TEST_ASSERT_GREATER_THAN_UINT32_MESSAGE(
        0, observer.buf_util_frames,
        "No BUF_UTIL snapshots observed after flush");
    TEST_ASSERT_FALSE_MESSAGE(observer.over_limit,
                              "inflight_peak exceeded the LBM transport count");
    TEST_ASSERT_GREATER_THAN_UINT32_MESSAGE(
        1, observer.max_inflight_peak,
        "Burst did not record concurrent inflight transports");
    /* ponytail: a single sequential writer cannot make this fail on the old
     * plain-volatile code; it checks presence and bounds of the recorded
     * peaks. Failing on the data race itself needs concurrent submitters or
     * TSan, which this on-target suite does not provide. */
}

TEST_CASE("BLE Log runtime survives deinit racing submissions",
          "[ble_log][runtime][ignore]")
{
    rt_deinit_writer_ctx_t *ctx = &s_deinit_race;
    bool reinit_ok = true;
    memset(ctx, 0, sizeof(*ctx));
    prepare_payload(UINT32_C(0x70000));

#if CONFIG_FREERTOS_UNICORE
    BaseType_t task_created = xTaskCreate(
        deinit_writer_task, "ble_log_deinit_wr", 4096, ctx,
        uxTaskPriorityGet(NULL), NULL);
#else
    /* Pin the writer away from this core so submissions run concurrently
     * with deinit instead of alternating at yield points. */
    BaseType_t task_created = xTaskCreatePinnedToCore(
        deinit_writer_task, "ble_log_deinit_wr", 4096, ctx,
        uxTaskPriorityGet(NULL), NULL, (xPortGetCoreID() == 0) ? 1 : 0);
#endif
    TEST_ASSERT_EQUAL_MESSAGE(pdPASS, task_created, "Writer task create failed");

    for (int i = 0; i < RT_DEINIT_ROUNDS; i++) {
        ble_log_deinit();
        reinit_ok = reinit_ok && ble_log_init();
        taskYIELD();
    }

    /* Stop the writer and join with a bound before touching ctx or asserting:
     * a unity longjmp past a live writer would leave it on a dead stack. */
    __atomic_store_n(&ctx->stop, true, __ATOMIC_RELEASE);
    const int64_t join_deadline_us = esp_timer_get_time() +
                                     (int64_t)RT_JOIN_TIMEOUT_MS * 1000;
    while (!__atomic_load_n(&ctx->exited, __ATOMIC_ACQUIRE)) {
        TickType_t join_ticks;
        if (!runtime_deadline_ticks(join_deadline_us, &join_ticks)) {
            break;
        }
        vTaskDelay(join_ticks);
    }

    /* Recover module state before any assertion can abort the test: a
     * failed re-init leaves the module deinit-ed and tearDown does not
     * restore it, which would cascade into every later test. */
    ble_log_deinit();
    bool recovered = ble_log_init();
    reinit_ok = reinit_ok && recovered;

    TEST_ASSERT_TRUE_MESSAGE(
        __atomic_load_n(&ctx->exited, __ATOMIC_ACQUIRE),
        "Writer task did not exit after stop");
    TEST_ASSERT_TRUE_MESSAGE(reinit_ok, "BLE Log re-init failed during the race");
    TEST_ASSERT_GREATER_THAN_UINT32_MESSAGE(
        RT_DEINIT_ROUNDS, ctx->attempts,
        "Writer task did not run during the deinit race");
    warm_up_runtime();
}
