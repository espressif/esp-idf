/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "esp_chip_info.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/task.h"
#include "unity.h"

#include "ble_log.h"
#include "ble_log_lbm_v2.h"
#include "ble_log_prph_test.h"
#include "ble_log_rt.h"
#include "ble_log_task_registry.h"
#include "test_ble_log_main.h"
#if CONFIG_BLE_HOST_COMPRESSED_LOG_ENABLE
#include "log_compression/utils.h"
#endif

#if !CONFIG_BLE_LOG_PRPH_TEST
#error "BLE Log test app requires CONFIG_BLE_LOG_PRPH_TEST"
#endif

/* Internal Snapshots use the production periodic cadence; let one full
 * window elapse when a test needs to observe the next periodic sample. */
#define TEST_HOOK_SETTLE_MS          (BLE_LOG_TS_TRIGGER_TIMEOUT_MS + 100)
#define TEST_READ_TIMEOUT_MS         (50)
#define TEST_MAX_ROUNDS              (3)
#define TEST_WRITES_PER_ROUND        (64)
#define TEST_PAYLOAD_LEN             (64)
#define TEST_READ_BUF_SIZE           (4096)
#define TEST_READER_STACK_SIZE       (3072)
#define TEST_READER_PRIO             (2)
#define TEST_LIFECYCLE_STACK_SIZE    (2048)
#define TEST_LIFECYCLE_PRIO          (3)

typedef struct {
    size_t version_info_count;
    ble_log_version_info_t version_info;
} version_capture_t;

typedef struct {
    version_capture_t capture;
    bool malformed;
    volatile bool stop;
    SemaphoreHandle_t done;
} reader_ctx_t;

static uint8_t s_read_buf[TEST_READ_BUF_SIZE];
static bool s_claim_hook_armed;
static bool s_init_snapshot_hook_armed;
static uint32_t s_stale_claim_handle;
static volatile bool s_locked_hook_armed;
static volatile bool s_enable_hook_armed;
static SemaphoreHandle_t s_enable_hook_entered;
static SemaphoreHandle_t s_enable_hook_continue;
static volatile bool s_disable_hook_armed;
static SemaphoreHandle_t s_disable_hook_entered;
static SemaphoreHandle_t s_disable_hook_continue;

/* Stale-hint candidate flush (pool lost-wakeup interleaving). */
static volatile bool s_flush_hint_hook_armed;
static volatile bool s_flush_stale_hook_armed;
static volatile bool s_flush_hint_drained;
static volatile int s_flush_hint_id = -1;
static SemaphoreHandle_t s_stale_writer_start;
static SemaphoreHandle_t s_stale_writer_started;

void ble_log_test_claim_pre_publish_hook(void);
void ble_log_test_claim_locked_hook(void);
void ble_log_test_init_snapshot_before_acquire_hook(void);
void ble_log_test_enable_before_lifecycle_lock_hook(void);
void ble_log_test_disable_before_wake_hook(void);
void ble_log_test_flush_hint_seen_hook(uint8_t id);
void ble_log_test_flush_stale_locked_hook(void);
#if CONFIG_BLE_HOST_COMPRESSED_LOG_ENABLE
extern int ble_log_compressed_hex_print(uint8_t source, uint32_t log_index,
                                        size_t args_cnt, ...);
#endif

void ble_log_test_claim_pre_publish_hook(void)
{
    if (s_claim_hook_armed) {
        /* A valid length exercises the state check on the still-OPEN
         * transport: a deleted check would frame stale claim metadata
         * (duplicate SN) on the wire. */
        ble_log_commit(s_stale_claim_handle, 1);
    }
}

void ble_log_test_claim_locked_hook(void)
{
    if (s_locked_hook_armed) {
        s_locked_hook_armed = false;
        /* Runs while the claiming writer itself holds the OPEN transport
         * lock: the flush must skip the busy transport and leave the
         * pending-seal marker for the next claim. */
        ble_log_lbm_flush_open_trans();
    }
}

void ble_log_test_init_snapshot_before_acquire_hook(void)
{
    if (s_init_snapshot_hook_armed) {
        vTaskDelay(pdMS_TO_TICKS(TEST_HOOK_SETTLE_MS));
    }
}

void ble_log_test_enable_before_lifecycle_lock_hook(void)
{
    if (s_enable_hook_armed) {
        xSemaphoreGive(s_enable_hook_entered);
        xSemaphoreTake(s_enable_hook_continue, portMAX_DELAY);
    }
}

void ble_log_test_disable_before_wake_hook(void)
{
    if (s_disable_hook_armed) {
        xSemaphoreGive(s_disable_hook_entered);
        xSemaphoreTake(s_disable_hook_continue, portMAX_DELAY);
    }
}

void ble_log_test_flush_hint_seen_hook(uint8_t id)
{
    if (!s_flush_hint_hook_armed) {
        return;
    }
    s_flush_hint_hook_armed = false;
    s_flush_hint_id = id;
    /* Play the other core between the flusher's hint read and its candidate
     * lock: seal, send and recycle the one OPEN transport. The recycle
     * notification runs while no writer is registered yet, so only the
     * flusher's own candidate release can re-advertise it. drain dispatches
     * to the peripheral queue; the peripheral releases on read, and that
     * queue holds only the hinted transport (the other shared ones are
     * pinned by open claims), so one read recycles exactly it. */
    ble_log_lbm_flush_open_trans();
    s_flush_hint_drained = ble_log_rt_drain();
    if (s_flush_hint_drained) {
        s_flush_hint_drained =
            ble_log_prph_test_read(s_read_buf, sizeof(s_read_buf), 0, 0, NULL) > 0;
    }
}

void ble_log_test_flush_stale_locked_hook(void)
{
    if (!s_flush_stale_hook_armed) {
        return;
    }
    s_flush_stale_hook_armed = false;
    /* The flusher holds the recycled transport's candidate lock. Start the
     * writer: with every other shared transport pinned by a claim, its two
     * scans bounce off this lock and it parks before this hook returns. */
    xSemaphoreGive(s_stale_writer_start);
    (void)xSemaphoreTake(s_stale_writer_started, portMAX_DELAY);
    vTaskDelay(pdMS_TO_TICKS(20));
}

/* A commit field is hex characters, zero-padded after a shorter value;
 * anything else (garbage, non-hex, zeros after data) is invalid. */
static bool commit_is_valid(const uint8_t *commit, size_t len)
{
    bool padding = false;
    for (size_t i = 0; i < len; i++) {
        if (commit[i] == 0) {
            padding = true;
            continue;
        }
        if (padding ||
                !((commit[i] >= '0' && commit[i] <= '9') ||
                  (commit[i] >= 'a' && commit[i] <= 'f') ||
                  (commit[i] >= 'A' && commit[i] <= 'F'))) {
            return false;
        }
    }
    return true;
}

static bool commit_is_zero(const uint8_t *commit, size_t len)
{
    for (size_t i = 0; i < len; i++) {
        if (commit[i] != 0) {
            return false;
        }
    }
    return true;
}

static void capture_version_info_frame(const test_ble_log_frame_t *frame, void *ctx)
{
    version_capture_t *capture = ctx;
    if (frame->payload_len < sizeof(uint32_t)) {
        return;
    }
    const uint8_t *record = frame->payload + sizeof(uint32_t);
    size_t record_len = frame->payload_len - sizeof(uint32_t);

    if (frame->src == BLE_LOG_SRC_INTERNAL &&
            record_len == sizeof(ble_log_internal_snapshot_t) &&
            record[0] == BLE_LOG_INT_SRC_SNAPSHOT) {
        ble_log_internal_snapshot_t snapshot;
        memcpy(&snapshot, record, sizeof(snapshot));
        memcpy(&capture->version_info, &snapshot.version_info,
               sizeof(capture->version_info));
        capture->version_info_count++;
    }
}

typedef struct {
    bool found;
    uint16_t reason_flags;
} first_snapshot_capture_t;

static void capture_first_snapshot(const test_ble_log_frame_t *frame, void *ctx)
{
    first_snapshot_capture_t *capture = ctx;
    if (capture->found || frame->src != BLE_LOG_SRC_INTERNAL ||
            frame->payload_len != sizeof(uint32_t) +
                                  sizeof(ble_log_internal_snapshot_t)) {
        return;
    }

    ble_log_internal_snapshot_t snapshot;
    memcpy(&snapshot, frame->payload + sizeof(uint32_t), sizeof(snapshot));
    if (snapshot.int_src_code == BLE_LOG_INT_SRC_SNAPSHOT) {
        capture->found = true;
        capture->reason_flags = snapshot.reason_flags;
    }
}

/* Consumes pending test transports concurrently with the writer: transports
 * are recycled only once read, and the LBM has a small pool of them. */
static void test_reader_task(void *arg)
{
    reader_ctx_t *ctx = arg;
    while (!ctx->stop) {
        size_t len = ble_log_prph_test_read(s_read_buf, sizeof(s_read_buf),
                                            pdMS_TO_TICKS(TEST_READ_TIMEOUT_MS), 0,
                                            NULL);
        if (len > 0 &&
                !test_ble_log_walk_frames(s_read_buf, len, capture_version_info_frame,
                                          &ctx->capture)) {
            ctx->malformed = true;
        }
    }
    xSemaphoreGive(ctx->done);
    vTaskDelete(NULL);
}

typedef struct {
    size_t count;
    test_ble_log_frame_t frame;
} golden_capture_t;

static void capture_golden_frame(const test_ble_log_frame_t *frame, void *ctx)
{
    golden_capture_t *capture = ctx;
    capture->frame = *frame;
    capture->count++;
}

TEST_CASE("BLE Log v8 framing matches golden bytes", "[ble_log][wire]")
{
    static const uint8_t golden_frame[] = {
        0x05, 0x00, 0x07, 0xde, 0xc0, 0x00,
        0x78, 0x56, 0x34, 0x12, 0xab,
        0xf1, 0x12, 0xd4, 0x88,
    };
    static const uint8_t golden_payload[] = {
        0x78, 0x56, 0x34, 0x12, 0xab,
    };

    TEST_ASSERT_EQUAL_UINT8(8, BLE_LOG_VERSION);
    TEST_ASSERT_EQUAL_UINT8(1, BLE_LOG_SRC_CORE_FIRST);
    TEST_ASSERT_EQUAL_UINT8(7, BLE_LOG_SRC_CORE_COUNT);
    TEST_ASSERT_EQUAL_UINT8(7, BLE_LOG_SRC_ENCODE);
    TEST_ASSERT_EQUAL_UINT8(7, BLE_LOG_INT_SRC_VERSION_INFO);
    TEST_ASSERT_EQUAL_UINT8(8, BLE_LOG_INT_SRC_SNAPSHOT);
    TEST_ASSERT_EQUAL_size_t(8, sizeof(ble_log_source_stat_t));
    TEST_ASSERT_EQUAL_size_t(134, sizeof(ble_log_internal_snapshot_t));
    TEST_ASSERT_EQUAL_size_t(
        3, offsetof(ble_log_internal_snapshot_t, version_info));
    TEST_ASSERT_EQUAL_size_t(
        62, offsetof(ble_log_internal_snapshot_t, ts.lc_ts));
    TEST_ASSERT_EQUAL_size_t(
        66, offsetof(ble_log_internal_snapshot_t, ts.esp_ts));
    TEST_ASSERT_EQUAL_size_t(
        70, offsetof(ble_log_internal_snapshot_t, ts.os_ts));
    TEST_ASSERT_EQUAL_size_t(
        78, offsetof(ble_log_internal_snapshot_t, stats));
    TEST_ASSERT_EQUAL_UINT32(148, BLE_LOG_INTERNAL_FRAME_LEN);
#if CONFIG_BLE_LOG_LL_ENABLED
    TEST_ASSERT_EQUAL_UINT8(4, BLE_LOG_LL_FLAG_HCI);
    TEST_ASSERT_EQUAL_UINT8(7, BLE_LOG_LL_FLAG_HCI_UPSTREAM);
#endif
    TEST_ASSERT_EQUAL_HEX32(
        0x00c0de07, BLE_LOG_MAKE_FRAME_META(0x07, 0x00c0de));
    TEST_ASSERT_EQUAL_HEX32(
        0x00000007, BLE_LOG_MAKE_FRAME_META(0x07, 0x01000000));

    golden_capture_t capture = {0};
    TEST_ASSERT_TRUE(test_ble_log_walk_frames(golden_frame,
                                              sizeof(golden_frame),
                                              capture_golden_frame,
                                              &capture));
    TEST_ASSERT_EQUAL_size_t(1, capture.count);
    TEST_ASSERT_EQUAL_UINT8(BLE_LOG_SRC_ENCODE, capture.frame.src);
    TEST_ASSERT_EQUAL_HEX32(0x00c0de, capture.frame.sn);
    TEST_ASSERT_EQUAL_size_t(sizeof(golden_payload), capture.frame.payload_len);
    TEST_ASSERT_EQUAL_MEMORY(golden_payload, capture.frame.payload,
                             sizeof(golden_payload));
}

TEST_CASE("BLE Log periodic tick reports build and chip versions", "[ble_log]")
{
    static const uint8_t payload[TEST_PAYLOAD_LEN] = {0};

    reader_ctx_t ctx = {0};
    ctx.done = xSemaphoreCreateBinary();
    TEST_ASSERT_NOT_NULL(ctx.done);

    TaskHandle_t reader;
    TEST_ASSERT_EQUAL(pdTRUE, xTaskCreate(test_reader_task, "ble_log_rd",
                                          TEST_READER_STACK_SIZE, &ctx,
                                          TEST_READER_PRIO, &reader));
    TEST_ASSERT_TRUE(ble_log_enable(true));
    TEST_ASSERT_TRUE(ble_log_sync_enable(true));

    /* Each periodic tick submits one consolidated Internal Snapshot containing
     * the version, statistics, utilization, and optional TS sample. */
    for (int round = 0; round < TEST_MAX_ROUNDS && ctx.capture.version_info_count == 0; round++) {
        vTaskDelay(pdMS_TO_TICKS(TEST_HOOK_SETTLE_MS));
        for (int i = 0; i < TEST_WRITES_PER_ROUND; i++) {
            ble_log_write_hex(BLE_LOG_SRC_CUSTOM, payload, sizeof(payload));
        }
        vTaskDelay(pdMS_TO_TICKS(200));
    }

    ctx.stop = true;
    TEST_ASSERT_TRUE(xSemaphoreTake(ctx.done, pdMS_TO_TICKS(1000)));

    TEST_ASSERT_FALSE(ctx.malformed);
    TEST_ASSERT_GREATER_OR_EQUAL_size_t(1, ctx.capture.version_info_count);

    const ble_log_version_info_t *vi = &ctx.capture.version_info;
    TEST_ASSERT_EQUAL_UINT8(BLE_LOG_INT_SRC_VERSION_INFO, vi->int_src_code);
    TEST_ASSERT_EQUAL_UINT8(BLE_LOG_VERSION, vi->version);
    TEST_ASSERT_TRUE(commit_is_valid(vi->idf_commit, BLE_LOG_IDF_COMMIT_LEN));
    /* Built from an IDF git checkout; the build commit must be injected */
    TEST_ASSERT_FALSE(commit_is_zero(vi->idf_commit, BLE_LOG_IDF_COMMIT_LEN));
    TEST_ASSERT_TRUE(commit_is_valid(vi->controller_commit, BLE_LOG_LIB_COMMIT_LEN));
    TEST_ASSERT_TRUE(commit_is_valid(vi->btdm_common_commit, BLE_LOG_LIB_COMMIT_LEN));
    TEST_ASSERT_TRUE(commit_is_valid(vi->mesh_commit, BLE_LOG_LIB_COMMIT_LEN));
    TEST_ASSERT_TRUE(commit_is_valid(vi->audio_commit, BLE_LOG_LIB_COMMIT_LEN));
    /* A commit field is non-zero only when the matching lib is linked in */
#if CONFIG_BT_CONTROLLER_ENABLED && (!CONFIG_BT_DUAL_MODE_ARCH || CONFIG_BT_CTRL_BLE_ENABLE)
    TEST_ASSERT_FALSE(commit_is_zero(vi->controller_commit, BLE_LOG_LIB_COMMIT_LEN));
#else
    TEST_ASSERT_TRUE(commit_is_zero(vi->controller_commit, BLE_LOG_LIB_COMMIT_LEN));
#endif
#if CONFIG_BT_CONTROLLER_ENABLED && CONFIG_BT_DUAL_MODE_ARCH
    TEST_ASSERT_FALSE(commit_is_zero(vi->btdm_common_commit, BLE_LOG_LIB_COMMIT_LEN));
#else
    TEST_ASSERT_TRUE(commit_is_zero(vi->btdm_common_commit, BLE_LOG_LIB_COMMIT_LEN));
#endif
#if CONFIG_BLE_MESH && CONFIG_BLE_MESH_V11_SUPPORT
    TEST_ASSERT_FALSE(commit_is_zero(vi->mesh_commit, BLE_LOG_LIB_COMMIT_LEN));
#else
    TEST_ASSERT_TRUE(commit_is_zero(vi->mesh_commit, BLE_LOG_LIB_COMMIT_LEN));
#endif
#if CONFIG_BT_AUDIO && CONFIG_SOC_BLE_AUDIO_SUPPORTED
    TEST_ASSERT_FALSE(commit_is_zero(vi->audio_commit, BLE_LOG_LIB_COMMIT_LEN));
#else
    TEST_ASSERT_TRUE(commit_is_zero(vi->audio_commit, BLE_LOG_LIB_COMMIT_LEN));
#endif

    esp_chip_info_t chip_info;
    esp_chip_info(&chip_info);
    TEST_ASSERT_EQUAL_UINT16((uint16_t)chip_info.model, vi->chip_model);
    TEST_ASSERT_EQUAL_UINT16(chip_info.revision, vi->chip_revision);

    (void)ble_log_sync_enable(false);
    vSemaphoreDelete(ctx.done);
}

TEST_CASE("BLE Log INIT snapshot starts each receiver epoch", "[ble_log][wire]")
{
    ble_log_deinit();
    s_init_snapshot_hook_armed = true;
    bool initialized = ble_log_init();
    s_init_snapshot_hook_armed = false;
    TEST_ASSERT_TRUE(initialized);

    size_t len = ble_log_prph_test_read(
        s_read_buf, sizeof(s_read_buf), pdMS_TO_TICKS(1000), 0, NULL);
    TEST_ASSERT_GREATER_THAN_size_t(0, len);

    first_snapshot_capture_t capture = {0};
    TEST_ASSERT_TRUE(test_ble_log_walk_frames(
        s_read_buf, len, capture_first_snapshot, &capture));
    TEST_ASSERT_TRUE(capture.found);
    TEST_ASSERT_EQUAL_HEX16(BLE_LOG_SNAPSHOT_REASON_INIT,
                            capture.reason_flags);
}

TEST_CASE("BLE Log sync IO APIs retain runtime lifecycle checks", "[ble_log]")
{
    ble_log_deinit();
    TEST_ASSERT_FALSE(ble_log_ts_sync_io_toggle_enable(true));
    TEST_ASSERT_FALSE(ble_log_sync_enable(true));

    TEST_ASSERT_TRUE(ble_log_init());
    TEST_ASSERT_TRUE(ble_log_ts_sync_io_toggle_enable(false));
    TEST_ASSERT_TRUE(ble_log_sync_enable(false));
    TEST_ASSERT_TRUE(ble_log_rt_drain());
    while (ble_log_prph_test_read(s_read_buf, sizeof(s_read_buf),
                                  0, 0, NULL) > 0) {
    }
}

typedef struct {
    bool task_frame;
    bool critical_frame;
    bool hci_downstream_frame;
    bool hci_upstream_frame;
    int ll_hci_frame_cnt;
    int ll_hci_upstream_frame_cnt;
    bool claimed_frame;
    bool stale_protected_frame;
    int encode_frame_cnt;
} frame_meta_capture_t;

static void capture_frame_meta(const test_ble_log_frame_t *frame, void *ctx)
{
    frame_meta_capture_t *capture = ctx;
    if (frame->src == BLE_LOG_SRC_ENCODE) {
        capture->encode_frame_cnt++;
    }
    if (frame->payload_len < sizeof(uint32_t) + 1) {
        return;
    }

    uint8_t marker = frame->payload[sizeof(uint32_t)];
    if (frame->src == BLE_LOG_SRC_CUSTOM && marker == 0x11) {
        capture->task_frame = true;
    } else if (frame->src == BLE_LOG_SRC_CUSTOM && marker == 0x22) {
        capture->critical_frame = true;
    } else if (frame->src == BLE_LOG_SRC_HCI && marker == 0x01) {
        capture->hci_downstream_frame = true;
    } else if (frame->src == BLE_LOG_SRC_HCI && marker == 0x82) {
        capture->hci_upstream_frame = true;
    } else if (frame->src == BLE_LOG_SRC_ENCODE && marker == 0x33) {
        capture->claimed_frame = true;
    } else if (frame->src == BLE_LOG_SRC_ENCODE && marker == 0x44) {
        capture->stale_protected_frame = true;
    } else if (marker == 0x51 || marker == 0x52) {
        static const uint8_t controller_timestamp[] = {0x78, 0x56, 0x34, 0x12};
        TEST_ASSERT_EQUAL_size_t(sizeof(controller_timestamp) + 1, frame->payload_len);
        TEST_ASSERT_EQUAL_MEMORY(controller_timestamp, frame->payload,
                                 sizeof(controller_timestamp));
        if (marker == 0x51) {
            TEST_ASSERT_EQUAL_UINT8(BLE_LOG_SRC_LL_HCI, frame->src);
            capture->ll_hci_frame_cnt++;
        } else {
            TEST_ASSERT_EQUAL_UINT8(BLE_LOG_SRC_HCI, frame->src);
            capture->ll_hci_upstream_frame_cnt++;
        }
    }
}

TEST_CASE("BLE Log writes from critical sections and commits claimed payload",
          "[ble_log][lbm]")
{
    const uint8_t task_marker = 0x11;
    const uint8_t critical_marker = 0x22;
    TEST_ASSERT_TRUE(ble_log_enable(true));
    ble_log_lbm_flush_open_trans();
    for (int round = 0; round < 2; round++) {
        TEST_ASSERT_TRUE(ble_log_rt_drain());
        while (ble_log_prph_test_read(s_read_buf, sizeof(s_read_buf),
                                      0, 0, NULL) > 0) {
        }
    }
    TEST_ASSERT_TRUE(ble_log_write_hex(BLE_LOG_SRC_CUSTOM,
                                       &task_marker, sizeof(task_marker)));

    portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;
    portENTER_CRITICAL(&mux);
    bool critical_written = ble_log_write_hex(BLE_LOG_SRC_CUSTOM,
                                               &critical_marker,
                                               sizeof(critical_marker));
    portEXIT_CRITICAL(&mux);
    TEST_ASSERT_TRUE(critical_written);

    uint8_t hci_type = 0x81;
    ble_log_write_hci(BLE_LOG_HCI_DOWNSTREAM, &hci_type, sizeof(hci_type));
    TEST_ASSERT_EQUAL_HEX8(0x81, hci_type);
    hci_type = 0x02;
    ble_log_write_hci(BLE_LOG_HCI_UPSTREAM, &hci_type, sizeof(hci_type));
    TEST_ASSERT_EQUAL_HEX8(0x02, hci_type);
    /* The HCI macro requires one writable type byte. Exercise invalid public
     * input at the validating API instead. */
    TEST_ASSERT_FALSE(ble_log_write_hex(BLE_LOG_SRC_HCI, NULL, 1));

#if CONFIG_BLE_LOG_LL_ENABLED
    /* Controller records keep their existing payload, including timestamp.
     * Exercise both contiguous and appended payloads at the real LL entry. */
    static const uint8_t ll_hci_payload[] = {0x78, 0x56, 0x34, 0x12, 0x51};
    const uint8_t ll_hci_upstream = 0x52;
    ble_log_write_hex_ll(sizeof(ll_hci_payload), ll_hci_payload, 0, NULL,
                         BIT(BLE_LOG_LL_FLAG_HCI));
    ble_log_write_hex_ll(sizeof(ll_hci_payload) - 1, ll_hci_payload,
                         sizeof(ll_hci_upstream), &ll_hci_upstream,
                         BIT(BLE_LOG_LL_FLAG_HCI_UPSTREAM));
#endif

    uint32_t handle;
    uint8_t *claimed = ble_log_claim(BLE_LOG_SRC_ENCODE, 8, &handle, true);
    TEST_ASSERT_NOT_NULL(claimed);
    claimed[0] = 0x33;
    ble_log_commit(handle, 1);

    /* Invoke the stale commit immediately before the next claim publishes
     * CLAIMED. It must see the prior OPEN state and leave the new claim
     * intact. */
    s_stale_claim_handle = handle;
    s_claim_hook_armed = true;
    uint32_t fresh_handle;
    claimed = ble_log_claim(BLE_LOG_SRC_ENCODE, 8, &fresh_handle, true);
    s_claim_hook_armed = false;
    TEST_ASSERT_NOT_NULL(claimed);
    TEST_ASSERT_NOT_EQUAL(handle, fresh_handle);
    /* The fresh claim is published: the same stale handle must now be
     * rejected by the generation check instead. A zero length mirrors a
     * stale abort, which would otherwise consume the fresh claim. */
    ble_log_commit(s_stale_claim_handle, 0);
    claimed[0] = 0x44;
    ble_log_commit(fresh_handle, 1);

    ble_log_lbm_flush_open_trans();
    TEST_ASSERT_TRUE(ble_log_rt_drain());

    frame_meta_capture_t capture = {0};
    for (int i = 0; i < BLE_LOG_TRANS_TOTAL_CNT; i++) {
        size_t len = ble_log_prph_test_read(s_read_buf, sizeof(s_read_buf),
                                            pdMS_TO_TICKS(TEST_READ_TIMEOUT_MS),
                                            0, NULL);
        if (!len) {
            break;
        }
        TEST_ASSERT_TRUE(test_ble_log_walk_frames(s_read_buf, len,
                                                  capture_frame_meta,
                                                  &capture));
    }

    TEST_ASSERT_TRUE(capture.task_frame);
    TEST_ASSERT_TRUE(capture.critical_frame);
    TEST_ASSERT_TRUE(capture.hci_downstream_frame);
    TEST_ASSERT_TRUE(capture.hci_upstream_frame);
#if CONFIG_BLE_LOG_LL_ENABLED && CONFIG_BLE_LOG_HCI_LOG_ENABLED && \
    !CONFIG_BT_BLUEDROID_ENABLED && !CONFIG_BT_NIMBLE_LEGACY_VHCI_ENABLE
    TEST_ASSERT_EQUAL(1, capture.ll_hci_frame_cnt);
    TEST_ASSERT_EQUAL(1, capture.ll_hci_upstream_frame_cnt);
#else
    /* Disabled HCI or an existing Host capture must suppress duplicates. */
    TEST_ASSERT_EQUAL(0, capture.ll_hci_frame_cnt);
    TEST_ASSERT_EQUAL(0, capture.ll_hci_upstream_frame_cnt);
#endif
    TEST_ASSERT_TRUE(capture.claimed_frame);
    TEST_ASSERT_TRUE(capture.stale_protected_frame);
    /* Exactly the two committed claims: a stale commit that slipped past
     * the state or generation check would add or consume an ENCODE frame. */
    TEST_ASSERT_EQUAL(2, capture.encode_frame_cnt);
}

#if CONFIG_BLE_HOST_COMPRESSED_LOG_ENABLE
#define TEST_CP_TASK_UNKNOWN      UINT8_C(0xff)
#define TEST_CP_ZERO_ARGS         UINT8_C(0x40)
#define TEST_CP_WRITER_RECORDS    8
#define TEST_CP_INDEX_MAIN_A      UINT16_C(0x600)
#define TEST_CP_INDEX_MAIN_B      UINT16_C(0x601)
#define TEST_CP_INDEX_BASE_A      UINT16_C(0x700)
#define TEST_CP_INDEX_BASE_B      UINT16_C(0x710)
#define TEST_CP_INDEX_FILL_A      UINT16_C(0x720)
#define TEST_CP_INDEX_FILL_B      UINT16_C(0x721)
#define TEST_CP_INDEX_FILLER      UINT16_C(0x722)
#define TEST_CP_NAME_MAX          17

typedef struct {
    /* id-to-name bindings learned from INTERNAL task-binding records */
    char names[CONFIG_BLE_LOG_TASK_ID_MAX][TEST_CP_NAME_MAX];
    uint8_t announce_cnt[CONFIG_BLE_LOG_TASK_ID_MAX];
    /* per-writer record accounting */
    uint8_t main_records;
    uint8_t a_records;
    uint8_t b_records;
    uint8_t filler_records;
    uint8_t fill_a_records;
    uint8_t fill_b_records;
    bool a_id_set;
    bool a_id_mismatch;
    uint8_t a_id;
    bool b_id_set;
    bool b_id_mismatch;
    uint8_t b_id;
    bool main_id_set;
    bool main_id_mismatch;
    uint8_t main_id;
    bool filler_unknown;
    bool fill_a_id_set;
    uint8_t fill_a_id;
    bool fill_b_unknown;
} compression_capture_t;

typedef struct {
    SemaphoreHandle_t token;
    SemaphoreHandle_t peer_token;
    SemaphoreHandle_t done;
    uint16_t index_base;
} compression_writer_ctx_t;

typedef struct {
    SemaphoreHandle_t done;
    uint16_t log_index;
} compression_once_ctx_t;

static void compression_writer_task(void *arg)
{
    compression_writer_ctx_t *ctx = arg;
    for (int i = 0; i < TEST_CP_WRITER_RECORDS; i++) {
        xSemaphoreTake(ctx->token, portMAX_DELAY);
        ble_log_compressed_hex_print(BLE_COMPRESSED_LOG_OUT_SOURCE_HOST,
                                     ctx->index_base + i, 0);
        xSemaphoreGive(ctx->peer_token);
    }
    xSemaphoreGive(ctx->done);
    vTaskDelete(NULL);
}

static void compression_once_task(void *arg)
{
    compression_once_ctx_t *ctx = arg;
    ble_log_compressed_hex_print(BLE_COMPRESSED_LOG_OUT_SOURCE_HOST,
                                 ctx->log_index, 0);
    xSemaphoreGive(ctx->done);
    vTaskDelete(NULL);
}

/* Walks one ENCODE frame: [ts 4B][source 1B][task_id 1B][header 1B]...
 * and one INTERNAL task-binding frame: [ts 4B][int_src 1B][task_id 1B]
 * [name 17B] (the periodic registry broadcast). */
static void capture_compressed_frame(const test_ble_log_frame_t *frame,
                                     void *ctx)
{
    compression_capture_t *capture = ctx;
    /* Bindings ride the INTERNAL stream: one frame per periodic window,
     * packing one fixed-layout record per registered entry. */
    if (frame->src == BLE_LOG_SRC_INTERNAL &&
            frame->payload_len >= sizeof(uint32_t) +
                                  sizeof(ble_log_task_binding_t) &&
            (frame->payload_len - sizeof(uint32_t)) %
                sizeof(ble_log_task_binding_t) == 0) {
        const uint8_t *record = frame->payload + sizeof(uint32_t);
        size_t records = (frame->payload_len - sizeof(uint32_t)) /
                         sizeof(ble_log_task_binding_t);
        for (size_t r = 0; r < records; r++, record += sizeof(ble_log_task_binding_t)) {
            if (record[0] != BLE_LOG_INT_SRC_TASK_BINDING) {
                continue;
            }
            uint8_t task_id = record[1];
            const uint8_t *name = record + 2;
            if (task_id < CONFIG_BLE_LOG_TASK_ID_MAX) {
                capture->announce_cnt[task_id]++;
                if (capture->announce_cnt[task_id] == 1) {
                    memcpy(capture->names[task_id], name,
                           sizeof(capture->names[task_id]));
                } else if (strcmp(capture->names[task_id],
                                  (const char *)name) != 0) {
                    /* same id re-announced with a different name */
                    capture->names[task_id][0] = '\0';
                }
            }
        }
        return;
    }
    if (frame->src != BLE_LOG_SRC_ENCODE) {
        return;
    }
    const uint8_t *record = frame->payload + sizeof(uint32_t);
    size_t record_len = frame->payload_len - sizeof(uint32_t);
    if (record_len < 3 ||
            record[0] != BLE_COMPRESSED_LOG_OUT_SOURCE_HOST) {
        return;
    }
    uint8_t task_id = record[1];
    uint8_t header = record[2];

    if (header != TEST_CP_ZERO_ARGS || record_len < 5) {
        return;
    }
    uint16_t log_index;
    memcpy(&log_index, record + 3, sizeof(log_index));

    if (log_index == TEST_CP_INDEX_MAIN_A || log_index == TEST_CP_INDEX_MAIN_B) {
        capture->main_records++;
        if (!capture->main_id_set) {
            capture->main_id_set = true;
            capture->main_id = task_id;
        } else if (capture->main_id != task_id) {
            capture->main_id_mismatch = true;
        }
    } else if (log_index >= TEST_CP_INDEX_BASE_A &&
               log_index < TEST_CP_INDEX_BASE_A + TEST_CP_WRITER_RECORDS) {
        capture->a_records++;
        if (!capture->a_id_set) {
            capture->a_id_set = true;
            capture->a_id = task_id;
        } else if (capture->a_id != task_id) {
            capture->a_id_mismatch = true;
        }
    } else if (log_index >= TEST_CP_INDEX_BASE_B &&
               log_index < TEST_CP_INDEX_BASE_B + TEST_CP_WRITER_RECORDS) {
        capture->b_records++;
        if (!capture->b_id_set) {
            capture->b_id_set = true;
            capture->b_id = task_id;
        } else if (capture->b_id != task_id) {
            capture->b_id_mismatch = true;
        }
    } else if (log_index >= TEST_CP_INDEX_FILLER &&
               log_index < TEST_CP_INDEX_FILLER + CONFIG_BLE_LOG_TASK_ID_MAX - 1) {
        capture->filler_records++;
        if (task_id == TEST_CP_TASK_UNKNOWN) {
            capture->filler_unknown = true;
        }
    } else if (log_index == TEST_CP_INDEX_FILL_A) {
        capture->fill_a_records++;
        if (task_id != TEST_CP_TASK_UNKNOWN) {
            capture->fill_a_id_set = true;
            capture->fill_a_id = task_id;
        }
    } else if (log_index == TEST_CP_INDEX_FILL_B) {
        capture->fill_b_records++;
        if (task_id == TEST_CP_TASK_UNKNOWN) {
            capture->fill_b_unknown = true;
        }
    }
}

static void compression_drain_and_capture(compression_capture_t *capture)
{
    ble_log_lbm_flush_open_trans();
    TEST_ASSERT_TRUE(ble_log_rt_drain());
    memset(capture, 0, sizeof(*capture));
    for (int i = 0; i < BLE_LOG_TRANS_TOTAL_CNT; i++) {
        size_t len = ble_log_prph_test_read(
            s_read_buf, sizeof(s_read_buf), pdMS_TO_TICKS(TEST_READ_TIMEOUT_MS),
            0, NULL);
        if (!len) {
            break;
        }
        TEST_ASSERT_TRUE(test_ble_log_walk_frames(
            s_read_buf, len, capture_compressed_frame, capture));
    }
}

static void compression_reset_streams(void)
{
    /* Wipe the task-id registry so neither compression case depends on the
     * other's registrations or on test-case order. */
    ble_log_test_task_registry_reset();
    TEST_ASSERT_TRUE(ble_log_enable(true));
    ble_log_lbm_flush_open_trans();
    for (int round = 0; round < 2; round++) {
        TEST_ASSERT_TRUE(ble_log_rt_drain());
        while (ble_log_prph_test_read(s_read_buf, sizeof(s_read_buf),
                                      0, 0, NULL) > 0) {
        }
    }
}

static uint8_t compression_announced_id(const compression_capture_t *capture,
                                         const char *name)
{
    for (int i = 0; i < CONFIG_BLE_LOG_TASK_ID_MAX; i++) {
        if (capture->announce_cnt[i] > 0 &&
                strcmp(capture->names[i], name) == 0) {
            return (uint8_t)i;
        }
    }
    return TEST_CP_TASK_UNKNOWN;
}

/* Protocol v8: task attribution is a property of the record. Two writers
 * interleave records on one compression source; every record carries its
 * writer's id and no record is dropped for contention (there is no source
 * lock to contend). Bindings are broadcast on the periodic window only
 * (the LBM task-binding publish), so it is driven once before the capture
 * below. */
TEST_CASE("BLE Log attributes compressed records to their writer task",
          "[ble_log][compression]")
{
    compression_reset_streams();

    ble_log_compressed_hex_print(BLE_COMPRESSED_LOG_OUT_SOURCE_HOST,
                                 TEST_CP_INDEX_MAIN_A, 0);

    compression_writer_ctx_t writer_a = {
        .token = xSemaphoreCreateBinary(),
        .peer_token = NULL,
        .done = xSemaphoreCreateBinary(),
        .index_base = TEST_CP_INDEX_BASE_A,
    };
    compression_writer_ctx_t writer_b = {
        .token = xSemaphoreCreateBinary(),
        .peer_token = NULL,
        .done = xSemaphoreCreateBinary(),
        .index_base = TEST_CP_INDEX_BASE_B,
    };
    writer_a.peer_token = writer_b.token;
    writer_b.peer_token = writer_a.token;
    TEST_ASSERT_NOT_NULL(writer_a.token);
    TEST_ASSERT_NOT_NULL(writer_b.token);
    TEST_ASSERT_NOT_NULL(writer_a.done);
    TEST_ASSERT_NOT_NULL(writer_b.done);

    TEST_ASSERT_EQUAL(pdTRUE,
        xTaskCreate(compression_writer_task, "cp_wr_a",
                    TEST_LIFECYCLE_STACK_SIZE, &writer_a,
                    TEST_LIFECYCLE_PRIO, NULL));
    TEST_ASSERT_EQUAL(pdTRUE,
        xTaskCreate(compression_writer_task, "cp_wr_b",
                    TEST_LIFECYCLE_STACK_SIZE, &writer_b,
                    TEST_LIFECYCLE_PRIO, NULL));

    xSemaphoreGive(writer_a.token);
    TEST_ASSERT_TRUE(xSemaphoreTake(writer_a.done, pdMS_TO_TICKS(2000)));
    TEST_ASSERT_TRUE(xSemaphoreTake(writer_b.done, pdMS_TO_TICKS(2000)));

    /* A second record from the main task must not register a new entry:
     * its records keep the registered id. Periodic publish may repeat
     * the binding in this window, always with the same name. */
    ble_log_compressed_hex_print(BLE_COMPRESSED_LOG_OUT_SOURCE_HOST,
                                 TEST_CP_INDEX_MAIN_B, 0);

    /* Bindings ride the periodic window only: drive the LBM broadcast
     * once so this capture carries every registered writer's binding. */
    ble_log_task_bindings_publish();

    compression_capture_t capture;
    compression_drain_and_capture(&capture);

    const char *main_name = pcTaskGetName(NULL);
    TEST_ASSERT_NOT_NULL(main_name);

    /* All records present: contention between interleaved writers no
     * longer drops records. */
    TEST_ASSERT_EQUAL_UINT8(TEST_CP_WRITER_RECORDS, capture.a_records);
    TEST_ASSERT_EQUAL_UINT8(TEST_CP_WRITER_RECORDS, capture.b_records);
    TEST_ASSERT_EQUAL_UINT8(2, capture.main_records);

    /* Every record of a writer carries that writer's announced id. */
    TEST_ASSERT_TRUE(capture.a_id_set);
    TEST_ASSERT_FALSE(capture.a_id_mismatch);
    TEST_ASSERT_TRUE(capture.b_id_set);
    TEST_ASSERT_FALSE(capture.b_id_mismatch);
    TEST_ASSERT_TRUE(capture.main_id_set);
    TEST_ASSERT_FALSE(capture.main_id_mismatch);
    TEST_ASSERT_NOT_EQUAL(TEST_CP_TASK_UNKNOWN,
                          compression_announced_id(&capture, "cp_wr_a"));
    TEST_ASSERT_NOT_EQUAL(TEST_CP_TASK_UNKNOWN,
                          compression_announced_id(&capture, "cp_wr_b"));
    TEST_ASSERT_NOT_EQUAL(TEST_CP_TASK_UNKNOWN,
                          compression_announced_id(&capture, main_name));
    TEST_ASSERT_EQUAL_UINT8(compression_announced_id(&capture, "cp_wr_a"),
                            capture.a_id);
    TEST_ASSERT_EQUAL_UINT8(compression_announced_id(&capture, "cp_wr_b"),
                            capture.b_id);
    TEST_ASSERT_EQUAL_UINT8(compression_announced_id(&capture, main_name),
                            capture.main_id);

    /* Every writer is announced at least once by the driven publish (a
     * production tick may add another, with the same name), and the
     * capture rejects a same-id rename. */
    TEST_ASSERT_TRUE(
        capture.announce_cnt[compression_announced_id(&capture, "cp_wr_a")] >= 1);
    TEST_ASSERT_TRUE(
        capture.announce_cnt[compression_announced_id(&capture, "cp_wr_b")] >= 1);
    TEST_ASSERT_TRUE(
        capture.announce_cnt[compression_announced_id(&capture, main_name)] >= 1);

    vSemaphoreDelete(writer_a.token);
    vSemaphoreDelete(writer_b.token);
    vSemaphoreDelete(writer_a.done);
    vSemaphoreDelete(writer_b.done);
}

/* Periodic binding publish: bindings are broadcast on the snapshot window
 * (the LBM periodic publish); a task's first record already carries its
 * id and is bound by name at the next window. Drive the publish
 * synchronously so the window is deterministic: capture the main task's
 * record first to learn its id, then the driven publish binds that id to
 * the name and announces no other id. */
TEST_CASE("BLE Log republishes task bindings on the periodic window",
          "[ble_log][compression]")
{
    compression_reset_streams();

    const char *main_name = pcTaskGetName(NULL);
    TEST_ASSERT_NOT_NULL(main_name);

    /* First window: the record carries the registered id, but no binding
     * is guaranteed yet (a production tick may or may not have landed in
     * this window, so no announce assertion is made here). */
    ble_log_compressed_hex_print(BLE_COMPRESSED_LOG_OUT_SOURCE_HOST,
                                 TEST_CP_INDEX_MAIN_A, 0);
    compression_capture_t first;
    compression_drain_and_capture(&first);
    TEST_ASSERT_TRUE(first.main_id_set);
    uint8_t main_id = first.main_id;
    TEST_ASSERT_NOT_EQUAL(TEST_CP_TASK_UNKNOWN, main_id);

    /* One synchronous publish; a production tick may add another with the
     * same id, so only presence and id/name stability are asserted. */
    ble_log_task_bindings_publish();
    compression_capture_t second;
    compression_drain_and_capture(&second);

    TEST_ASSERT_TRUE(second.announce_cnt[main_id] >= 1);
    TEST_ASSERT_EQUAL_UINT8(main_id,
                            compression_announced_id(&second, main_name));
    /* Publishing never fabricates bindings: no other id is announced. */
    for (int i = 0; i < CONFIG_BLE_LOG_TASK_ID_MAX; i++) {
        if (i != main_id) {
            TEST_ASSERT_EQUAL_UINT8(0, second.announce_cnt[i]);
        }
    }
}

/* The registry is sized by CONFIG_BLE_LOG_TASK_ID_MAX (4 in this
 * app) and wiped between cases, so this case fills it itself: one-shot
 * filler tasks consume every slot but the last, cp_fill_a takes the last
 * free slot, and cp_fill_b beyond the registry degrades to the unknown id
 * (0xFF) and its record is still emitted. Bindings are broadcast on the
 * periodic window only, so the publish is driven once before the
 * capture. */
TEST_CASE("BLE Log degrades to the unknown task id when the registry is full",
          "[ble_log][compression]")
{
    compression_reset_streams();

    /* Consume every slot but the last with distinct one-shot tasks. The
     * name is copied into the task's TCB, so one buffer is reused. */
    compression_once_ctx_t filler[CONFIG_BLE_LOG_TASK_ID_MAX - 1];
    char filler_name[8];
    for (int i = 0; i < CONFIG_BLE_LOG_TASK_ID_MAX - 1; i++) {
        filler[i].done = xSemaphoreCreateBinary();
        TEST_ASSERT_NOT_NULL(filler[i].done);
        filler[i].log_index = TEST_CP_INDEX_FILLER + i;
        snprintf(filler_name, sizeof(filler_name), "cp_f%d", i);
        TEST_ASSERT_EQUAL(pdTRUE,
            xTaskCreate(compression_once_task, filler_name,
                        TEST_LIFECYCLE_STACK_SIZE, &filler[i],
                        TEST_LIFECYCLE_PRIO, NULL));
        TEST_ASSERT_TRUE(xSemaphoreTake(filler[i].done, pdMS_TO_TICKS(2000)));
    }

    compression_once_ctx_t fill_a = {
        .done = xSemaphoreCreateBinary(),
        .log_index = TEST_CP_INDEX_FILL_A,
    };
    compression_once_ctx_t fill_b = {
        .done = xSemaphoreCreateBinary(),
        .log_index = TEST_CP_INDEX_FILL_B,
    };
    TEST_ASSERT_NOT_NULL(fill_a.done);
    TEST_ASSERT_NOT_NULL(fill_b.done);

    TEST_ASSERT_EQUAL(pdTRUE,
        xTaskCreate(compression_once_task, "cp_fill_a",
                    TEST_LIFECYCLE_STACK_SIZE, &fill_a,
                    TEST_LIFECYCLE_PRIO, NULL));
    TEST_ASSERT_TRUE(xSemaphoreTake(fill_a.done, pdMS_TO_TICKS(2000)));

    TEST_ASSERT_EQUAL(pdTRUE,
        xTaskCreate(compression_once_task, "cp_fill_b",
                    TEST_LIFECYCLE_STACK_SIZE, &fill_b,
                    TEST_LIFECYCLE_PRIO, NULL));
    TEST_ASSERT_TRUE(xSemaphoreTake(fill_b.done, pdMS_TO_TICKS(2000)));

    /* Bindings ride the periodic window only: drive the LBM broadcast
     * once so the capture carries every registered name. */
    ble_log_task_bindings_publish();

    compression_capture_t capture;
    compression_drain_and_capture(&capture);

    /* The fillers consumed the first slots: every record emitted with a
     * valid id. */
    TEST_ASSERT_EQUAL_UINT8(CONFIG_BLE_LOG_TASK_ID_MAX - 1,
                            capture.filler_records);
    TEST_ASSERT_FALSE(capture.filler_unknown);

    /* The last free slot still works: valid id, bound by the driven
     * periodic publish. */
    TEST_ASSERT_EQUAL_UINT8(1, capture.fill_a_records);
    TEST_ASSERT_TRUE(capture.fill_a_id_set);
    TEST_ASSERT_NOT_EQUAL(TEST_CP_TASK_UNKNOWN,
                          compression_announced_id(&capture, "cp_fill_a"));
    TEST_ASSERT_EQUAL_UINT8(compression_announced_id(&capture, "cp_fill_a"),
                            capture.fill_a_id);

    /* Beyond the registry: unknown id, no binding, log still emitted. */
    TEST_ASSERT_EQUAL_UINT8(1, capture.fill_b_records);
    TEST_ASSERT_TRUE(capture.fill_b_unknown);
    TEST_ASSERT_EQUAL_UINT8(TEST_CP_TASK_UNKNOWN,
                            compression_announced_id(&capture, "cp_fill_b"));

    for (int i = 0; i < CONFIG_BLE_LOG_TASK_ID_MAX - 1; i++) {
        vSemaphoreDelete(filler[i].done);
    }
    vSemaphoreDelete(fill_a.done);
    vSemaphoreDelete(fill_b.done);
}
#endif /* CONFIG_BLE_HOST_COMPRESSED_LOG_ENABLE */

typedef struct {
    ble_log_src_t src;
    uint8_t marker;
    bool found;
    uint32_t sn;
} sequence_capture_t;

typedef struct {
    sequence_capture_t *captures;
    size_t count;
} sequence_capture_set_t;

static void capture_sequence_frame(const test_ble_log_frame_t *frame, void *ctx)
{
    sequence_capture_set_t *set = ctx;
    bool is_ll = frame->src == BLE_LOG_SRC_LL_TASK ||
                 frame->src == BLE_LOG_SRC_LL_ISR;
    size_t marker_offset = is_ll ? 0 : sizeof(uint32_t);
    /* LL payloads already carry the controller's LC timestamp; unlike the
     * other sources, they must not gain an ESP timestamp prefix. */
    if (frame->payload_len != marker_offset + 1) {
        return;
    }

    uint8_t marker = frame->payload[marker_offset];
    for (size_t i = 0; i < set->count; i++) {
        sequence_capture_t *capture = &set->captures[i];
        if (frame->src == capture->src && marker == capture->marker) {
            capture->found = true;
            capture->sn = frame->sn;
        }
    }
}

static void read_sequence_frames(sequence_capture_t *captures, size_t count)
{
    sequence_capture_set_t set = {
        .captures = captures,
        .count = count,
    };
    for (int i = 0; i < BLE_LOG_TRANS_TOTAL_CNT; i++) {
        size_t len = ble_log_prph_test_read(
            s_read_buf, sizeof(s_read_buf), pdMS_TO_TICKS(TEST_READ_TIMEOUT_MS),
            0, NULL);
        if (!len) {
            break;
        }
        TEST_ASSERT_TRUE(test_ble_log_walk_frames(
            s_read_buf, len, capture_sequence_frame, &set));
    }
}

static void auto_recycle_noop(void *ctx)
{
    (void)ctx;
}

TEST_CASE("BLE Log preserves LL payload and rejects oversized records",
          "[ble_log][lbm][wire]")
{
    enum {
        CUSTOM_BEFORE,
        CUSTOM_OVERSIZED,
        CUSTOM_AFTER,
#if CONFIG_BLE_LOG_LL_ENABLED
        LL_BEFORE,
        LL_PRIMARY_OVERSIZED,
        LL_APPEND_OVERSIZED,
        LL_AFTER,
#endif
        SEQUENCE_CAPTURE_COUNT,
    };
    static uint8_t custom_oversized_payload[
        BLE_LOG_MAX_PAYLOAD_LEN - sizeof(uint32_t) + 1];
#if CONFIG_BLE_LOG_LL_ENABLED
    static uint8_t ll_oversized_payload[BLE_LOG_MAX_PAYLOAD_LEN + 1];
    static uint8_t ll_oversized_append[BLE_LOG_MAX_PAYLOAD_LEN];
#endif
    const uint8_t custom_before = 0x61;
    const uint8_t custom_oversized = 0x62;
    const uint8_t custom_after = 0x63;
#if CONFIG_BLE_LOG_LL_ENABLED
    const uint8_t ll_before = 0x64;
    const uint8_t ll_primary_oversized = 0x65;
    const uint8_t ll_append_oversized = 0x66;
    const uint8_t ll_after = 0x67;
#endif
    sequence_capture_t captures[SEQUENCE_CAPTURE_COUNT] = {
        [CUSTOM_BEFORE] = {.src = BLE_LOG_SRC_CUSTOM, .marker = custom_before},
        [CUSTOM_OVERSIZED] = {
            .src = BLE_LOG_SRC_CUSTOM,
            .marker = custom_oversized,
        },
        [CUSTOM_AFTER] = {.src = BLE_LOG_SRC_CUSTOM, .marker = custom_after},
#if CONFIG_BLE_LOG_LL_ENABLED
        [LL_BEFORE] = {.src = BLE_LOG_SRC_LL_TASK, .marker = ll_before},
        [LL_PRIMARY_OVERSIZED] = {
            .src = BLE_LOG_SRC_LL_TASK,
            .marker = ll_primary_oversized,
        },
        [LL_APPEND_OVERSIZED] = {
            .src = BLE_LOG_SRC_LL_TASK,
            .marker = ll_append_oversized,
        },
        [LL_AFTER] = {.src = BLE_LOG_SRC_LL_TASK, .marker = ll_after},
#endif
    };

    TEST_ASSERT_TRUE(ble_log_enable(true));
    ble_log_lbm_flush_open_trans();
    for (int round = 0; round < 2; round++) {
        TEST_ASSERT_TRUE(ble_log_rt_drain());
        while (ble_log_prph_test_read(s_read_buf, sizeof(s_read_buf),
                                      0, 0, NULL) > 0) {
        }
    }

    TEST_ASSERT_TRUE(ble_log_write_hex(BLE_LOG_SRC_CUSTOM,
                                       &custom_before, sizeof(custom_before)));
    memset(custom_oversized_payload, 0, sizeof(custom_oversized_payload));
    custom_oversized_payload[0] = custom_oversized;
    TEST_ASSERT_FALSE(ble_log_write_hex(BLE_LOG_SRC_CUSTOM,
                                        custom_oversized_payload,
                                        sizeof(custom_oversized_payload)));
    TEST_ASSERT_TRUE(ble_log_write_hex(BLE_LOG_SRC_CUSTOM,
                                       &custom_after, sizeof(custom_after)));

#if CONFIG_BLE_LOG_LL_ENABLED
    ble_log_write_hex_ll(sizeof(ll_before), &ll_before, 0, NULL,
                         BIT(BLE_LOG_LL_FLAG_TASK));
    memset(ll_oversized_payload, 0, sizeof(ll_oversized_payload));
    ll_oversized_payload[0] = ll_primary_oversized;
    ble_log_write_hex_ll(sizeof(ll_oversized_payload), ll_oversized_payload,
                         0, NULL, BIT(BLE_LOG_LL_FLAG_TASK));
    memset(ll_oversized_append, 0, sizeof(ll_oversized_append));
    ble_log_write_hex_ll(sizeof(ll_append_oversized), &ll_append_oversized,
                         sizeof(ll_oversized_append), ll_oversized_append,
                         BIT(BLE_LOG_LL_FLAG_TASK));
    ble_log_write_hex_ll(sizeof(ll_after), &ll_after, 0, NULL,
                         BIT(BLE_LOG_LL_FLAG_TASK));
#endif

    ble_log_lbm_flush_open_trans();
    TEST_ASSERT_TRUE(ble_log_rt_drain());
    read_sequence_frames(captures, SEQUENCE_CAPTURE_COUNT);

    TEST_ASSERT_TRUE(captures[CUSTOM_BEFORE].found);
    TEST_ASSERT_FALSE(captures[CUSTOM_OVERSIZED].found);
    TEST_ASSERT_TRUE(captures[CUSTOM_AFTER].found);
    TEST_ASSERT_EQUAL_HEX32(
        (captures[CUSTOM_BEFORE].sn + 2) & 0x00ffffffU,
        captures[CUSTOM_AFTER].sn);
#if CONFIG_BLE_LOG_LL_ENABLED
    TEST_ASSERT_TRUE(captures[LL_BEFORE].found);
    TEST_ASSERT_FALSE(captures[LL_PRIMARY_OVERSIZED].found);
    TEST_ASSERT_FALSE(captures[LL_APPEND_OVERSIZED].found);
    TEST_ASSERT_TRUE(captures[LL_AFTER].found);
    TEST_ASSERT_EQUAL_HEX32(
        (captures[LL_BEFORE].sn + 3) & 0x00ffffffU,
        captures[LL_AFTER].sn);
#endif
}

TEST_CASE("BLE Log flush preserves source-local sequence continuity",
          "[ble_log][wire]")
{
    const uint8_t before_marker = 0x71;
    const uint8_t after_marker = 0x72;
    TEST_ASSERT_TRUE(ble_log_enable(true));
    ble_log_lbm_flush_open_trans();
    for (int round = 0; round < 2; round++) {
        TEST_ASSERT_TRUE(ble_log_rt_drain());
        while (ble_log_prph_test_read(s_read_buf, sizeof(s_read_buf),
                                      0, 0, NULL) > 0) {
        }
    }

    TEST_ASSERT_TRUE(ble_log_write_hex(BLE_LOG_SRC_CUSTOM,
                                       &before_marker, sizeof(before_marker)));
    ble_log_lbm_flush_open_trans();
    TEST_ASSERT_TRUE(ble_log_rt_drain());
    sequence_capture_t before = {
        .src = BLE_LOG_SRC_CUSTOM,
        .marker = before_marker,
    };
    read_sequence_frames(&before, 1);
    TEST_ASSERT_TRUE(before.found);

    /* The test peripheral normally recycles only when read. Auto-recycle lets
     * synchronous flush observe completion without a second reader task. */
    ble_log_prph_test_set_auto_recycle_hook(auto_recycle_noop, NULL);
    ble_log_flush();
    ble_log_prph_test_set_auto_recycle_hook(NULL, NULL);

    TEST_ASSERT_TRUE(ble_log_write_hex(BLE_LOG_SRC_CUSTOM,
                                       &after_marker, sizeof(after_marker)));
    ble_log_lbm_flush_open_trans();
    TEST_ASSERT_TRUE(ble_log_rt_drain());
    sequence_capture_t after = {
        .src = BLE_LOG_SRC_CUSTOM,
        .marker = after_marker,
    };
    read_sequence_frames(&after, 1);
    TEST_ASSERT_TRUE(after.found);
    TEST_ASSERT_EQUAL_HEX32((before.sn + 1) & 0x00ffffffU, after.sn);
}

/* ------- pending-seal and deinit drain ------- */

#define TEST_MARKER_CHUNK_MAX          (4)

typedef struct {
    uint8_t markers[TEST_MARKER_CHUNK_MAX];
    int count;
} marker_chunk_capture_t;

static void capture_custom_marker(const test_ble_log_frame_t *frame, void *ctx)
{
    marker_chunk_capture_t *capture = ctx;
    if (frame->src == BLE_LOG_SRC_CUSTOM &&
            frame->payload_len == sizeof(uint32_t) + 1 &&
            capture->count < TEST_MARKER_CHUNK_MAX) {
        capture->markers[capture->count++] = frame->payload[sizeof(uint32_t)];
    }
}

/* Reads every pending transport chunk, keeping only the chunks that carry
 * CUSTOM marker frames; interleaved snapshot transports are consumed and
 * ignored. */
static int read_marker_chunks(marker_chunk_capture_t *chunks, int max_chunks)
{
    int marker_chunks = 0;
    for (int i = 0; i < BLE_LOG_TRANS_TOTAL_CNT; i++) {
        size_t len = ble_log_prph_test_read(
            s_read_buf, sizeof(s_read_buf), pdMS_TO_TICKS(TEST_READ_TIMEOUT_MS),
            0, NULL);
        if (!len) {
            break;
        }
        marker_chunk_capture_t chunk = {0};
        TEST_ASSERT_TRUE(test_ble_log_walk_frames(s_read_buf, len,
                                                  capture_custom_marker,
                                                  &chunk));
        if (chunk.count && marker_chunks < max_chunks) {
            chunks[marker_chunks++] = chunk;
        }
    }
    return marker_chunks;
}

static void auto_recycle_count(void *ctx)
{
    (*(int *)ctx)++;
}

TEST_CASE("BLE Log pending-seal marker defers the busy transport flush",
          "[ble_log][lbm]")
{
    const uint8_t first_marker = 0xd1;
    const uint8_t cursor_marker = 0xd2;
    const uint8_t hooked_marker = 0xd3;
    const uint8_t post_marker = 0xd4;

    TEST_ASSERT_TRUE(ble_log_enable(true));
    ble_log_lbm_flush_open_trans();
    for (int round = 0; round < 2; round++) {
        TEST_ASSERT_TRUE(ble_log_rt_drain());
        while (ble_log_prph_test_read(s_read_buf, sizeof(s_read_buf),
                                      0, 0, NULL) > 0) {
        }
    }

    /* Park one frame in an OPEN transport; the claim cursor stays on it. */
    TEST_ASSERT_TRUE(ble_log_write_hex(BLE_LOG_SRC_CUSTOM,
                                       &first_marker, sizeof(first_marker)));

    /* A prior test may leave open_cursor at any recycled transport. Prime
     * it through either the direct path or the OPEN bitmap scan before
     * arming the direct-path hook. */
    TEST_ASSERT_TRUE(ble_log_write_hex(BLE_LOG_SRC_CUSTOM,
                                       &cursor_marker, sizeof(cursor_marker)));

    /* The armed hook runs a flush while the claiming writer itself holds
     * the transport lock: the flush must skip the busy transport and leave
     * the pending-seal marker for the NEXT claim. */
    s_locked_hook_armed = true;
    bool hooked_write = ble_log_write_hex(BLE_LOG_SRC_CUSTOM,
                                          &hooked_marker,
                                          sizeof(hooked_marker));
    bool hook_ran = !s_locked_hook_armed;
    s_locked_hook_armed = false;
    TEST_ASSERT_TRUE(hooked_write);
    TEST_ASSERT_TRUE(hook_ran);

    /* The next writer must seal the flagged transport first and place its
     * own frame in another one. */
    TEST_ASSERT_TRUE(ble_log_write_hex(BLE_LOG_SRC_CUSTOM,
                                       &post_marker, sizeof(post_marker)));

    ble_log_lbm_flush_open_trans();
    TEST_ASSERT_TRUE(ble_log_rt_drain());

    marker_chunk_capture_t chunks[2] = {0};
    TEST_ASSERT_EQUAL(2, read_marker_chunks(chunks, 2));
    TEST_ASSERT_EQUAL(3, chunks[0].count);
    TEST_ASSERT_EQUAL_HEX8(first_marker, chunks[0].markers[0]);
    TEST_ASSERT_EQUAL_HEX8(cursor_marker, chunks[0].markers[1]);
    TEST_ASSERT_EQUAL_HEX8(hooked_marker, chunks[0].markers[2]);
    TEST_ASSERT_EQUAL(1, chunks[1].count);
    TEST_ASSERT_EQUAL_HEX8(post_marker, chunks[1].markers[0]);
}

TEST_CASE("BLE Log deinit drain delivers parked open transports",
          "[ble_log][lbm]")
{
    const uint8_t markers[TEST_MARKER_CHUNK_MAX] = {0xe1, 0xe2, 0xe3, 0xe4};

    TEST_ASSERT_TRUE(ble_log_enable(true));
    ble_log_lbm_flush_open_trans();
    for (int round = 0; round < 2; round++) {
        TEST_ASSERT_TRUE(ble_log_rt_drain());
        while (ble_log_prph_test_read(s_read_buf, sizeof(s_read_buf),
                                      0, 0, NULL) > 0) {
        }
    }

    /* Sub-capacity burst: frames park in an OPEN transport that no
     * capacity seal ever sends. */
    for (int i = 0; i < TEST_MARKER_CHUNK_MAX; i++) {
        TEST_ASSERT_TRUE(ble_log_write_hex(BLE_LOG_SRC_CUSTOM,
                                           &markers[i], 1));
    }

    /* Exercise the exact deinit-drain contract: close the producer gate and
     * wait for writers before sealing every OPEN transport. */
    ble_log_lbm_begin_deinit();
    ble_log_lbm_drain_open_trans();

    marker_chunk_capture_t chunks[1] = {0};
    int chunk_count = read_marker_chunks(chunks, 1);

    /* Finish the partially-entered teardown and restore the module before
     * assertions so the following Unity case starts from a valid lifetime. */
    ble_log_deinit();
    TEST_ASSERT_TRUE(ble_log_init());

    TEST_ASSERT_EQUAL(1, chunk_count);
    TEST_ASSERT_EQUAL(TEST_MARKER_CHUNK_MAX, chunks[0].count);
    for (int i = 0; i < TEST_MARKER_CHUNK_MAX; i++) {
        TEST_ASSERT_EQUAL_HEX8(markers[i], chunks[0].markers[i]);
    }
}

TEST_CASE("BLE Log deinit hands residual transports to the peripheral",
          "[ble_log][lbm]")
{
    const uint8_t marker = 0xf1;
    int recycled = 0;

    TEST_ASSERT_TRUE(ble_log_enable(true));
    ble_log_lbm_flush_open_trans();
    for (int round = 0; round < 2; round++) {
        TEST_ASSERT_TRUE(ble_log_rt_drain());
        while (ble_log_prph_test_read(s_read_buf, sizeof(s_read_buf),
                                      0, 0, NULL) > 0) {
        }
    }

    TEST_ASSERT_TRUE(ble_log_write_hex(BLE_LOG_SRC_CUSTOM,
                                       &marker, sizeof(marker)));

    /* Auto-recycle turns every dispatched transport into a countable
     * event, so the real deinit path is observable without a reader
     * task racing the teardown. */
    ble_log_prph_test_set_auto_recycle_hook(auto_recycle_count, &recycled);
    ble_log_deinit();
    TEST_ASSERT_GREATER_OR_EQUAL(1, recycled);
    TEST_ASSERT_TRUE(ble_log_init());
}

typedef struct {
    SemaphoreHandle_t started;
    SemaphoreHandle_t done;
    bool result;
} blocked_writer_ctx_t;

static void blocked_write_task(void *arg)
{
    blocked_writer_ctx_t *ctx = arg;
    static const uint8_t marker = 0x57;
    xSemaphoreGive(ctx->started);
    ctx->result = ble_log_write_hex(BLE_LOG_SRC_CUSTOM, &marker,
                                    sizeof(marker));
    xSemaphoreGive(ctx->done);
    vTaskDelete(NULL);
}

static void blocked_claim_task(void *arg)
{
    blocked_writer_ctx_t *ctx = arg;
    uint32_t handle;
    xSemaphoreGive(ctx->started);
    uint8_t *payload = ble_log_claim(BLE_LOG_SRC_ENCODE, 1, &handle, true);
    ctx->result = payload != NULL;
    if (payload) {
        payload[0] = 0x58;
    }
    ble_log_commit(handle, 1);
    xSemaphoreGive(ctx->done);
    vTaskDelete(NULL);
}

typedef struct {
    bool write_frame;
    bool claim_frame;
} blocked_capture_t;

static void capture_blocked_markers(const test_ble_log_frame_t *frame, void *ctx)
{
    blocked_capture_t *capture = ctx;
    if (frame->payload_len != sizeof(uint32_t) + 1) {
        return;
    }
    uint8_t marker = frame->payload[sizeof(uint32_t)];
    if (frame->src == BLE_LOG_SRC_CUSTOM && marker == 0x57) {
        capture->write_frame = true;
    } else if (frame->src == BLE_LOG_SRC_ENCODE && marker == 0x58) {
        capture->claim_frame = true;
    }
}

TEST_CASE("BLE Log task writers wait for a shared transport", "[ble_log][lbm]")
{
    static const uint8_t full_payload[
        BLE_LOG_MAX_PAYLOAD_LEN - sizeof(uint32_t)] = {0};

    TEST_ASSERT_TRUE(ble_log_enable(true));
    ble_log_lbm_flush_open_trans();
    for (int round = 0; round < 2; round++) {
        TEST_ASSERT_TRUE(ble_log_rt_drain());
        while (ble_log_prph_test_read(s_read_buf, sizeof(s_read_buf),
                                      0, 0, NULL) > 0) {
        }
    }

    /* Keep every task-usable transport SENDING in the test peripheral. */
    for (int i = 0; i < BLE_LOG_POOL_SHARED_CNT; i++) {
        TEST_ASSERT_TRUE(ble_log_write_hex(BLE_LOG_SRC_CUSTOM, full_payload,
                                           sizeof(full_payload)));
    }

    blocked_writer_ctx_t writers[2] = {0};
    for (int i = 0; i < 2; i++) {
        writers[i].started = xSemaphoreCreateBinary();
        writers[i].done = xSemaphoreCreateBinary();
        TEST_ASSERT_NOT_NULL(writers[i].started);
        TEST_ASSERT_NOT_NULL(writers[i].done);
    }
    TEST_ASSERT_EQUAL(pdTRUE,
                      xTaskCreate(blocked_write_task, "ble_log_wait1",
                                  TEST_LIFECYCLE_STACK_SIZE, &writers[0],
                                  TEST_LIFECYCLE_PRIO, NULL));
    TEST_ASSERT_EQUAL(pdTRUE,
                      xTaskCreate(blocked_claim_task, "ble_log_wait2",
                                  TEST_LIFECYCLE_STACK_SIZE, &writers[1],
                                  TEST_LIFECYCLE_PRIO, NULL));
    for (int i = 0; i < 2; i++) {
        TEST_ASSERT_TRUE(xSemaphoreTake(writers[i].started,
                                        pdMS_TO_TICKS(1000)));
    }
    vTaskDelay(1);
    /* No shared transport is free: both yieldable writers must be parked. */
    for (int i = 0; i < 2; i++) {
        TEST_ASSERT_EQUAL(pdFALSE, xSemaphoreTake(writers[i].done, 0));
    }

    /* Recycling transports wakes the parked writers; neither write is lost. */
    TEST_ASSERT_TRUE(ble_log_rt_drain());
    for (int i = 0; i < 2; i++) {
        size_t len = ble_log_prph_test_read(s_read_buf, sizeof(s_read_buf),
                                            pdMS_TO_TICKS(TEST_READ_TIMEOUT_MS),
                                            0, NULL);
        TEST_ASSERT_GREATER_THAN_size_t(0, len);
    }
    for (int i = 0; i < 2; i++) {
        TEST_ASSERT_TRUE(xSemaphoreTake(writers[i].done, pdMS_TO_TICKS(1000)));
        TEST_ASSERT_TRUE(writers[i].result);
    }

    ble_log_lbm_flush_open_trans();
    TEST_ASSERT_TRUE(ble_log_rt_drain());
    blocked_capture_t capture = {0};
    for (int i = 0; i < BLE_LOG_TRANS_TOTAL_CNT; i++) {
        size_t len = ble_log_prph_test_read(s_read_buf, sizeof(s_read_buf),
                                            pdMS_TO_TICKS(TEST_READ_TIMEOUT_MS),
                                            0, NULL);
        if (!len) {
            break;
        }
        TEST_ASSERT_TRUE(test_ble_log_walk_frames(s_read_buf, len,
                                                  capture_blocked_markers,
                                                  &capture));
    }
    TEST_ASSERT_TRUE(capture.write_frame);
    TEST_ASSERT_TRUE(capture.claim_frame);

    for (int i = 0; i < 2; i++) {
        vSemaphoreDelete(writers[i].started);
        vSemaphoreDelete(writers[i].done);
    }
}

typedef struct {
    SemaphoreHandle_t done;
    bool write_result;
} stale_writer_ctx_t;

static void stale_candidate_writer_task(void *arg)
{
    stale_writer_ctx_t *ctx = arg;
    (void)xSemaphoreTake(s_stale_writer_start, portMAX_DELAY);
    xSemaphoreGive(s_stale_writer_started);
    const uint8_t marker = 0x61;
    ctx->write_result = ble_log_write_hex(BLE_LOG_SRC_CUSTOM, &marker, 1);
    xSemaphoreGive(ctx->done);
    vTaskDelete(NULL);
}

TEST_CASE("BLE Log flush re-advertises a stale candidate for parked writers",
          "[ble_log][lbm]")
{
    /* Deterministic lost-wakeup interleaving: the periodic flusher reads an
     * OPEN hint, the hinted transport is sealed, sent and recycled before
     * the flusher takes its candidate lock, and a task writer registers
     * and parks while that lock is held. Only the flusher's release can
     * then re-advertise the FREE transport; a bare unlock loses the
     * writer. */
    TEST_ASSERT_TRUE(ble_log_enable(true));
    ble_log_lbm_flush_open_trans();
    TEST_ASSERT_TRUE(ble_log_rt_drain());
    while (ble_log_prph_test_read(s_read_buf, sizeof(s_read_buf), 0, 0, NULL) > 0) {
    }

    /* Pin every shared transport but one with an open claim: claimed
     * transports are in no bitmap and never enter the runtime queue. */
    uint32_t pinned[BLE_LOG_POOL_TRANS_CNT] = {0};
    int pinned_cnt = 0;
    for (; pinned_cnt < BLE_LOG_POOL_SHARED_CNT - 1; pinned_cnt++) {
        uint8_t *payload = ble_log_claim(BLE_LOG_SRC_ENCODE, 1,
                                         &pinned[pinned_cnt], false);
        TEST_ASSERT_NOT_NULL(payload);
    }

    /* The last shared transport stays OPEN, so the armed flush targets it. */
    const uint8_t marker = 0x61;
    TEST_ASSERT_TRUE(ble_log_write_hex(BLE_LOG_SRC_CUSTOM, &marker, 1));

    stale_writer_ctx_t ctx = {.done = xSemaphoreCreateBinary()};
    s_stale_writer_start = xSemaphoreCreateBinary();
    s_stale_writer_started = xSemaphoreCreateBinary();
    TEST_ASSERT_NOT_NULL(ctx.done);
    TEST_ASSERT_NOT_NULL(s_stale_writer_start);
    TEST_ASSERT_NOT_NULL(s_stale_writer_started);
    TEST_ASSERT_EQUAL(pdTRUE,
                      xTaskCreate(stale_candidate_writer_task, "ble_log_stale",
                                  TEST_LIFECYCLE_STACK_SIZE, &ctx,
                                  TEST_LIFECYCLE_PRIO, NULL));

    s_flush_hint_id = -1;
    s_flush_hint_drained = false;
    s_flush_hint_hook_armed = true;
    s_flush_stale_hook_armed = true;
    ble_log_lbm_flush_open_trans();
    TEST_ASSERT_TRUE(s_flush_hint_drained);
    TEST_ASSERT_TRUE(s_flush_hint_id >= 0);

    bool completed = xSemaphoreTake(ctx.done, pdMS_TO_TICKS(1000)) == pdTRUE;
    if (!completed) {
        /* Lost wake: un-park the writer, release the pinned claims and
         * restore the gate before failing, so later cases stay clean. */
        TEST_ASSERT_TRUE(ble_log_enable(false));
        TEST_ASSERT_EQUAL(pdTRUE, xSemaphoreTake(ctx.done, pdMS_TO_TICKS(1000)));
        TEST_ASSERT_FALSE(ctx.write_result);
        for (int i = 0; i < pinned_cnt; i++) {
            ble_log_commit(pinned[i], 0);
        }
        TEST_ASSERT_TRUE(ble_log_enable(true));
        TEST_FAIL_MESSAGE("stale-candidate release lost the parked writer's wake");
    }
    TEST_ASSERT_TRUE(ctx.write_result);

    for (int i = 0; i < pinned_cnt; i++) {
        ble_log_commit(pinned[i], 0);
    }

    /* The recovered writer's frame reaches the peripheral. */
    ble_log_lbm_flush_open_trans();
    TEST_ASSERT_TRUE(ble_log_rt_drain());
    TEST_ASSERT_GREATER_THAN_size_t(
        0, ble_log_prph_test_read(s_read_buf, sizeof(s_read_buf),
                                  pdMS_TO_TICKS(TEST_READ_TIMEOUT_MS), 0, NULL));

    vSemaphoreDelete(ctx.done);
    vSemaphoreDelete(s_stale_writer_start);
    vSemaphoreDelete(s_stale_writer_started);
}

typedef struct {
    SemaphoreHandle_t done;
    uint8_t marker;
    bool result;
    bool finished;
} conc_writer_ctx_t;

#define CONC_WRITER_CNT   (4)
#define CONC_FRAMES_EACH  (96)

static void conc_writer_task(void *arg)
{
    conc_writer_ctx_t *ctx = arg;
    for (uint16_t i = 0; i < CONC_FRAMES_EACH; i++) {
        uint8_t payload[3] = {ctx->marker, (uint8_t)(i >> 8), (uint8_t)i};
        if (!ble_log_write_hex(BLE_LOG_SRC_CUSTOM, payload, sizeof(payload))) {
            ctx->result = false;
            break;
        }
    }
    xSemaphoreGive(ctx->done);
    vTaskDelete(NULL);
}

typedef struct {
    size_t frames;
    size_t per_marker[CONC_WRITER_CNT];
    uint32_t sn[CONC_WRITER_CNT * CONC_FRAMES_EACH];
    size_t sn_count;
} conc_capture_t;

static void capture_conc_frame(const test_ble_log_frame_t *frame, void *ctx_)
{
    conc_capture_t *cap = ctx_;
    if (frame->src != BLE_LOG_SRC_CUSTOM || frame->payload_len != 4 + 3) {
        return;
    }
    cap->frames++;
    uint8_t marker = frame->payload[4];
    for (int w = 0; w < CONC_WRITER_CNT; w++) {
        if (marker == 0xa0 + w) {
            cap->per_marker[w]++;
        }
    }
    if (cap->sn_count < CONC_WRITER_CNT * CONC_FRAMES_EACH) {
        cap->sn[cap->sn_count++] = frame->sn;
    }
}

typedef struct {
    SemaphoreHandle_t done;
    uint32_t hammer_cnt;
    bool taken;
} hammer_ctx_t;

static void hammer_claim_task(void *arg)
{
    hammer_ctx_t *ctx = arg;
    uint32_t committed = 0;
    const uint8_t marker = 0xb0;
    for (int i = 0; i < 4000; i++) {
        uint32_t handle;
        uint8_t *payload = ble_log_claim(BLE_LOG_SRC_ENCODE, 1,
                                         &handle, false);
        if (payload) {
            payload[0] = marker;
            ble_log_commit(handle, 1);
            committed++;
        }
    }
    ctx->hammer_cnt = committed;
    xSemaphoreGive(ctx->done);
    vTaskDelete(NULL);
}

typedef struct {
    uint32_t sn[CONC_WRITER_CNT * CONC_FRAMES_EACH + 16 * 1024];
    size_t sn_count;
} hammer_capture_t;

static void capture_hammer_frame(const test_ble_log_frame_t *frame, void *ctx_)
{
    hammer_capture_t *cap = ctx_;
    if (frame->src == BLE_LOG_SRC_ENCODE &&
            cap->sn_count < CONC_WRITER_CNT * CONC_FRAMES_EACH + 16 * 1024) {
        cap->sn[cap->sn_count++] = frame->sn;
    }
}

static size_t count_unique_sn(const uint32_t *sn, size_t n)
{
    size_t unique = 0;
    for (size_t i = 0; i < n; i++) {
        bool first = true;
        for (size_t j = 0; j < i; j++) {
            if (sn[j] == sn[i]) {
                first = false;
                break;
            }
        }
        unique += first;
    }
    return unique;
}

typedef struct {
    SemaphoreHandle_t done;
    bool write_result;
    bool claim_result;
} timer_writer_ctx_t;

static void timer_writer_cb(void *arg)
{
    timer_writer_ctx_t *ctx = arg;
    const uint8_t marker = 0x59;
    ctx->write_result = ble_log_write_hex(BLE_LOG_SRC_CUSTOM, &marker, 1);
    uint32_t handle;
    uint8_t *payload = ble_log_claim(BLE_LOG_SRC_ENCODE, 1, &handle, true);
    ctx->claim_result = payload != NULL;
    if (payload) {
        payload[0] = marker;
        ble_log_commit(handle, 1);
    }
#if CONFIG_BLE_LOG_LL_ENABLED
    ble_log_write_hex_ll(1, &marker, 0, NULL, BIT(BLE_LOG_LL_FLAG_TASK));
#endif
#if CONFIG_BLE_HOST_COMPRESSED_LOG_ENABLE
    ble_log_compressed_hex_print(BLE_COMPRESSED_LOG_OUT_SOURCE_HOST, 0x730, 0);
#endif
    xSemaphoreGive(ctx->done);
}

TEST_CASE("BLE Log ESP Timer writers never wait for shared transports",
          "[ble_log][lbm]")
{
    static const uint8_t full_payload[
        BLE_LOG_MAX_PAYLOAD_LEN - sizeof(uint32_t)] = {0};
    TEST_ASSERT_TRUE(ble_log_enable(true));
    ble_log_lbm_flush_open_trans();
    TEST_ASSERT_TRUE(ble_log_rt_drain());
    while (ble_log_prph_test_read(s_read_buf, sizeof(s_read_buf), 0, 0, NULL) > 0) {
    }
    /* The test peripheral holds these transports until explicitly read.
     * Reserve capacity remains available but must not be used by this callback. */
    for (int i = 0; i < BLE_LOG_POOL_SHARED_CNT; i++) {
        TEST_ASSERT_TRUE(ble_log_write_hex(BLE_LOG_SRC_CUSTOM, full_payload,
                                           sizeof(full_payload)));
    }

    timer_writer_ctx_t ctx = {.done = xSemaphoreCreateBinary()};
    TEST_ASSERT_NOT_NULL(ctx.done);
    esp_timer_handle_t timer;
    const esp_timer_create_args_t args = {
        .callback = timer_writer_cb,
        .arg = &ctx,
        .dispatch_method = ESP_TIMER_TASK,
        .name = "ble_log_writer",
    };
    TEST_ESP_OK(esp_timer_create(&args, &timer));

    bool returned = true;
    bool results_match = true;
    /* First fire with shared capacity exhausted, then again after recycle:
     * both callbacks must return, but only the second may write/claim. */
    for (int attempt = 0; attempt < 2; attempt++) {
        TEST_ESP_OK(esp_timer_start_once(timer, 1));
        bool completed = xSemaphoreTake(ctx.done, pdMS_TO_TICKS(1000)) == pdTRUE;
        returned &= completed;
        if (!completed) {
            /* A regressed writer may be parked. Wake it before waiting for
             * callback completion; never delete a task inside a pool API. */
            TEST_ASSERT_TRUE(ble_log_enable(false));
        }
        TEST_ESP_OK(esp_timer_stop_blocking(timer, portMAX_DELAY));
        (void)xSemaphoreTake(ctx.done, 0);
        results_match &= ctx.write_result == (attempt != 0) &&
                         ctx.claim_result == (attempt != 0);

        ble_log_lbm_flush_open_trans();
        TEST_ASSERT_TRUE(ble_log_rt_drain());
        while (ble_log_prph_test_read(s_read_buf, sizeof(s_read_buf), 0, 0, NULL) > 0) {
        }
        TEST_ASSERT_TRUE(ble_log_enable(true));
    }
    TEST_ESP_OK(esp_timer_delete(timer));
    vSemaphoreDelete(ctx.done);
    TEST_ASSERT_TRUE_MESSAGE(returned, "ESP Timer writer waited for a shared transport");
    TEST_ASSERT_TRUE(results_match);
}

#define SNAPSHOT_CAPTURE_MAX 8

typedef struct {
    uint32_t sn[SNAPSHOT_CAPTURE_MAX];
    int count;
} snapshot_capture_t;

static void capture_snapshot_loss(const test_ble_log_frame_t *frame, void *ctx)
{
    snapshot_capture_t *capture = ctx;
    if (frame->src != BLE_LOG_SRC_INTERNAL ||
            frame->payload_len != sizeof(uint32_t) +
                                  sizeof(ble_log_internal_snapshot_t)) {
        return;
    }

    ble_log_internal_snapshot_t snapshot;
    memcpy(&snapshot, frame->payload + sizeof(uint32_t), sizeof(snapshot));
    if (snapshot.int_src_code == BLE_LOG_INT_SRC_SNAPSHOT &&
            (snapshot.reason_flags & BLE_LOG_SNAPSHOT_REASON_PERIODIC) &&
            capture->count < SNAPSHOT_CAPTURE_MAX) {
        capture->sn[capture->count++] = frame->sn;
    }
}

TEST_CASE("BLE Log periodic snapshot ignores producer gate and fails fast when busy",
          "[ble_log][lbm]")
{
    ble_log_ts_info_t ts_info;
    ble_log_rt_ts_sample(&ts_info, false);

    TEST_ASSERT_TRUE(ble_log_enable(true));
    ble_log_lbm_flush_open_trans();
    for (int round = 0; round < 2; round++) {
        TEST_ASSERT_TRUE(ble_log_rt_drain());
        while (ble_log_prph_test_read(s_read_buf, sizeof(s_read_buf),
                                      0, 0, NULL) > 0) {
        }
    }

    TEST_ASSERT_TRUE(ble_log_enable(false));
    bool submitted = false;
    for (int attempt = 0; attempt < TEST_MAX_ROUNDS && !submitted; attempt++) {
        TEST_ASSERT_TRUE(ble_log_rt_drain());
        while (ble_log_prph_test_read(s_read_buf, sizeof(s_read_buf),
                                      0, 0, NULL) > 0) {
        }
        submitted = ble_log_internal_snapshot(
            BLE_LOG_SNAPSHOT_REASON_PERIODIC, &ts_info, false);
    }
    TEST_ASSERT_TRUE_MESSAGE(submitted,
                             "Periodic timer kept the Internal transport busy");
    TEST_ASSERT_FALSE(ble_log_internal_snapshot(
        BLE_LOG_SNAPSHOT_REASON_PERIODIC, &ts_info, false));
    TEST_ASSERT_TRUE(ble_log_enable(true));
    TEST_ASSERT_TRUE(ble_log_rt_drain());

    snapshot_capture_t capture = {0};
    size_t first_len = ble_log_prph_test_read(
        s_read_buf, sizeof(s_read_buf), pdMS_TO_TICKS(TEST_READ_TIMEOUT_MS),
        0, NULL);
    TEST_ASSERT_GREATER_THAN_size_t(0, first_len);
    TEST_ASSERT_TRUE(test_ble_log_walk_frames(
        s_read_buf, first_len, capture_snapshot_loss, &capture));

    TEST_ASSERT_TRUE(ble_log_internal_snapshot(
        BLE_LOG_SNAPSHOT_REASON_PERIODIC, &ts_info, false));
    TEST_ASSERT_TRUE(ble_log_rt_drain());

    for (int i = 0; i < BLE_LOG_TRANS_TOTAL_CNT; i++) {
        size_t len = ble_log_prph_test_read(
            s_read_buf, sizeof(s_read_buf), pdMS_TO_TICKS(TEST_READ_TIMEOUT_MS),
            0, NULL);
        if (!len) {
            break;
        }
        TEST_ASSERT_TRUE(test_ble_log_walk_frames(
            s_read_buf, len, capture_snapshot_loss, &capture));
    }
    /* The busy periodic attempt burned one snapshot SN: the gap in the
     * snapshot sequence is the loss signal. */
    TEST_ASSERT_GREATER_OR_EQUAL_INT(2, capture.count);
    bool gap_seen = false;
    for (int i = 1; i < capture.count; i++) {
        if (((capture.sn[i] - capture.sn[i - 1]) & 0x00ffffffU) >= 2) {
            gap_seen = true;
        }
    }
    TEST_ASSERT_TRUE(gap_seen);
}

#if CONFIG_BLE_LOG_LL_ENABLED
typedef struct {
    SemaphoreHandle_t started;
    SemaphoreHandle_t done;
} waiter_ctx_t;

typedef struct {
    SemaphoreHandle_t done;
    bool result;
} enable_ctx_t;

typedef struct {
    SemaphoreHandle_t started;
    SemaphoreHandle_t done;
} deinit_ctx_t;

static void blocked_ll_writer_task(void *arg)
{
    waiter_ctx_t *ctx = arg;
    static const uint8_t marker = 0x55;
    xSemaphoreGive(ctx->started);
    ble_log_write_hex_ll(sizeof(marker), &marker, 0, NULL,
                         BIT(BLE_LOG_LL_FLAG_TASK));
    xSemaphoreGive(ctx->done);
    vTaskDelete(NULL);
}

static void racing_enable_task(void *arg)
{
    enable_ctx_t *ctx = arg;
    ctx->result = ble_log_enable(true);
    xSemaphoreGive(ctx->done);
    vTaskDelete(NULL);
}

static void racing_disable_task(void *arg)
{
    enable_ctx_t *ctx = arg;
    ctx->result = ble_log_enable(false);
    xSemaphoreGive(ctx->done);
    vTaskDelete(NULL);
}

static void lifecycle_deinit_task(void *arg)
{
    deinit_ctx_t *ctx = arg;
    xSemaphoreGive(ctx->started);
    ble_log_deinit();
    xSemaphoreGive(ctx->done);
    vTaskDelete(NULL);
}

TEST_CASE("BLE Log deinit closes a parked LL writer before racing enable",
          "[ble_log][lbm]")
{
    static const uint8_t full_payload[
        BLE_LOG_MAX_PAYLOAD_LEN - sizeof(uint32_t)] = {0};

    TEST_ASSERT_TRUE(ble_log_enable(true));
    ble_log_lbm_flush_open_trans();
    TEST_ASSERT_TRUE(ble_log_rt_drain());
    while (ble_log_prph_test_read(s_read_buf, sizeof(s_read_buf), 0, 0, NULL) > 0) {
    }
    TEST_ASSERT_TRUE(ble_log_rt_drain());
    while (ble_log_prph_test_read(s_read_buf, sizeof(s_read_buf), 0, 0, NULL) > 0) {
    }

    /* Keep every task-usable transport SENDING in the test peripheral. */
    for (int i = 0; i < BLE_LOG_POOL_SHARED_CNT; i++) {
        TEST_ASSERT_TRUE(ble_log_write_hex(BLE_LOG_SRC_CUSTOM, full_payload,
                                           sizeof(full_payload)));
    }
    static const uint8_t reserve_marker = 0x56;

    portMUX_TYPE reserve_mux = portMUX_INITIALIZER_UNLOCKED;
    portENTER_CRITICAL(&reserve_mux);
    bool reserve_written = ble_log_write_hex(BLE_LOG_SRC_CUSTOM,
                                              &reserve_marker,
                                              sizeof(reserve_marker));
    portEXIT_CRITICAL(&reserve_mux);
    TEST_ASSERT_TRUE(reserve_written);

    waiter_ctx_t waiter = {
        .started = xSemaphoreCreateBinary(),
        .done = xSemaphoreCreateBinary(),
    };
    TEST_ASSERT_NOT_NULL(waiter.started);
    TEST_ASSERT_NOT_NULL(waiter.done);
    TEST_ASSERT_EQUAL(pdTRUE,
                      xTaskCreate(blocked_ll_writer_task, "ble_log_wait",
                                  TEST_LIFECYCLE_STACK_SIZE, &waiter,
                                  TEST_LIFECYCLE_PRIO, NULL));
    TEST_ASSERT_TRUE(xSemaphoreTake(waiter.started, pdMS_TO_TICKS(1000)));
    vTaskDelay(1);
    TEST_ASSERT_EQUAL(pdFALSE, xSemaphoreTake(waiter.done, 0));

    s_enable_hook_entered = xSemaphoreCreateBinary();
    s_enable_hook_continue = xSemaphoreCreateBinary();
    TEST_ASSERT_NOT_NULL(s_enable_hook_entered);
    TEST_ASSERT_NOT_NULL(s_enable_hook_continue);
    s_enable_hook_armed = true;

    enable_ctx_t enabler = {
        .done = xSemaphoreCreateBinary(),
    };
    TEST_ASSERT_NOT_NULL(enabler.done);
    TEST_ASSERT_EQUAL(pdTRUE,
                      xTaskCreate(racing_enable_task, "ble_log_enable",
                                  TEST_LIFECYCLE_STACK_SIZE, &enabler,
                                  TEST_LIFECYCLE_PRIO, NULL));
    TEST_ASSERT_TRUE(xSemaphoreTake(s_enable_hook_entered,
                                    pdMS_TO_TICKS(1000)));

    /* The enable task is paused immediately before the lifecycle lock. Close
     * and destroy the module, then let enable perform its locked recheck. */
    ble_log_deinit();
    TEST_ASSERT_TRUE(xSemaphoreTake(waiter.done, pdMS_TO_TICKS(1000)));
    xSemaphoreGive(s_enable_hook_continue);
    TEST_ASSERT_TRUE(xSemaphoreTake(enabler.done, pdMS_TO_TICKS(1000)));
    s_enable_hook_armed = false;
    TEST_ASSERT_FALSE(enabler.result);

    vSemaphoreDelete(waiter.started);
    vSemaphoreDelete(waiter.done);
    vSemaphoreDelete(enabler.done);
    vSemaphoreDelete(s_enable_hook_entered);
    vSemaphoreDelete(s_enable_hook_continue);
    s_enable_hook_entered = NULL;
    s_enable_hook_continue = NULL;

    TEST_ASSERT_TRUE(ble_log_init());
}

TEST_CASE("BLE Log disable keeps waiter semaphore alive during deinit",
          "[ble_log][lbm]")
{
    TEST_ASSERT_TRUE(ble_log_enable(true));
    s_disable_hook_entered = xSemaphoreCreateBinary();
    s_disable_hook_continue = xSemaphoreCreateBinary();
    TEST_ASSERT_NOT_NULL(s_disable_hook_entered);
    TEST_ASSERT_NOT_NULL(s_disable_hook_continue);
    s_disable_hook_armed = true;

    enable_ctx_t disabler = {
        .done = xSemaphoreCreateBinary(),
    };
    deinit_ctx_t deinit = {
        .started = xSemaphoreCreateBinary(),
        .done = xSemaphoreCreateBinary(),
    };
    TEST_ASSERT_NOT_NULL(disabler.done);
    TEST_ASSERT_NOT_NULL(deinit.started);
    TEST_ASSERT_NOT_NULL(deinit.done);

    TEST_ASSERT_EQUAL(pdTRUE,
                      xTaskCreate(racing_disable_task, "ble_log_disable",
                                  TEST_LIFECYCLE_STACK_SIZE, &disabler,
                                  TEST_LIFECYCLE_PRIO, NULL));
    TEST_ASSERT_TRUE(xSemaphoreTake(s_disable_hook_entered,
                                    pdMS_TO_TICKS(1000)));
    TEST_ASSERT_EQUAL(pdTRUE,
                      xTaskCreate(lifecycle_deinit_task, "ble_log_deinit",
                                  TEST_LIFECYCLE_STACK_SIZE, &deinit,
                                  TEST_LIFECYCLE_PRIO, NULL));
    TEST_ASSERT_TRUE(xSemaphoreTake(deinit.started, pdMS_TO_TICKS(1000)));
    TEST_ASSERT_EQUAL(pdFALSE,
                      xSemaphoreTake(deinit.done, pdMS_TO_TICKS(20)));

    xSemaphoreGive(s_disable_hook_continue);
    TEST_ASSERT_TRUE(xSemaphoreTake(disabler.done, pdMS_TO_TICKS(1000)));
    TEST_ASSERT_TRUE(xSemaphoreTake(deinit.done, pdMS_TO_TICKS(1000)));
    s_disable_hook_armed = false;
    TEST_ASSERT_TRUE(disabler.result);

    vSemaphoreDelete(disabler.done);
    vSemaphoreDelete(deinit.started);
    vSemaphoreDelete(deinit.done);
    vSemaphoreDelete(s_disable_hook_entered);
    vSemaphoreDelete(s_disable_hook_continue);
    s_disable_hook_entered = NULL;
    s_disable_hook_continue = NULL;

    TEST_ASSERT_TRUE(ble_log_init());
}
#endif /* CONFIG_BLE_LOG_LL_ENABLED */

/* ------------------------------------------------------------------ */
/* Review blocker repro: pool waiter self-wake spin.                   */
/* A cancelled claim publishes FREE state before the free-bitmap hint; */
/* a waiter scanning that transport from the stale open-cursor hint    */
/* must PARK. Unconditional notification on the unlocked FREE state    */
/* lets the waiter mint and consume its own wake tokens in a tight     */
/* loop: the writer never parks, and the publisher suspended inside    */
/* the recycle window can never run to publish the bitmap.            */
/* ------------------------------------------------------------------ */
static volatile bool s_spin_hook_armed;
static SemaphoreHandle_t s_spin_writer_start;
static SemaphoreHandle_t s_spin_writer_entered;
static SemaphoreHandle_t s_spin_canceller_hold;

typedef struct {
    SemaphoreHandle_t done;
    volatile bool result;
} spin_writer_ctx_t;

static void spin_writer_task(void *arg)
{
    spin_writer_ctx_t *ctx = arg;
    (void)xSemaphoreTake(s_spin_writer_start, portMAX_DELAY);
    xSemaphoreGive(s_spin_writer_entered);
    const uint8_t marker = 0xC1;
    ctx->result = ble_log_write_hex(BLE_LOG_SRC_CUSTOM, &marker, 1);
    xSemaphoreGive(ctx->done);
    vTaskDelete(NULL);
}

typedef struct {
    SemaphoreHandle_t done;
    uint32_t handle;
} spin_cancel_ctx_t;

static void spin_canceller_task(void *arg)
{
    spin_cancel_ctx_t *ctx = arg;
    s_spin_hook_armed = true;
    ble_log_commit(ctx->handle, 0);
    xSemaphoreGive(ctx->done);
    vTaskDelete(NULL);
}

void ble_log_test_recycle_pre_bitmap_hook(ble_log_prph_trans_t *trans);

void ble_log_test_recycle_pre_bitmap_hook(ble_log_prph_trans_t *trans)
{
    (void)trans;
    if (!s_spin_hook_armed) {
        return;
    }
    s_spin_hook_armed = false;
    /* Deterministic preemption between the FREE publication and the
     * free-bitmap hint: release the waiting writer, then stay suspended
     * exactly like a publisher preempted in this window. */
    xSemaphoreGive(s_spin_writer_start);
    (void)xSemaphoreTake(s_spin_canceller_hold, portMAX_DELAY);
}

typedef struct {
    volatile bool stop;
    volatile uint32_t loops;
} canary_ctx_t;

static void spin_canary_task(void *arg)
{
    canary_ctx_t *ctx = arg;
    while (!ctx->stop) {
        ctx->loops++;
        vTaskDelay(1);
    }
    vTaskDelete(NULL);
}

TEST_CASE("BLE Log pool waiter parks instead of self-waking on an unpublished FREE",
          "[ble_log][lbm][repro]")
{
    canary_ctx_t canary = {0};
    spin_writer_ctx_t wctx = {.done = xSemaphoreCreateBinary()};
    spin_cancel_ctx_t cctx = {.done = xSemaphoreCreateBinary()};
    s_spin_writer_start = xSemaphoreCreateBinary();
    s_spin_writer_entered = xSemaphoreCreateBinary();
    s_spin_canceller_hold = xSemaphoreCreateBinary();
    TEST_ASSERT_NOT_NULL(wctx.done);
    TEST_ASSERT_NOT_NULL(cctx.done);
    TEST_ASSERT_NOT_NULL(s_spin_writer_start);
    TEST_ASSERT_NOT_NULL(s_spin_writer_entered);
    TEST_ASSERT_NOT_NULL(s_spin_canceller_hold);

    /* Fresh pool: the first shared transport must land on id 0 so the
     * stale open-cursor hint (still 0 from init) points at it. */
    ble_log_deinit();
    TEST_ASSERT_TRUE(ble_log_init());
    TEST_ASSERT_TRUE(ble_log_enable(true));
    ble_log_lbm_flush_open_trans();
    for (int round = 0; round < 2; round++) {
        TEST_ASSERT_TRUE(ble_log_rt_drain());
        while (ble_log_prph_test_read(s_read_buf, sizeof(s_read_buf),
                                      0, 0, NULL) > 0) {
        }
    }

    TEST_ASSERT_NOT_NULL(ble_log_claim(BLE_LOG_SRC_ENCODE, 1,
                                       &cctx.handle, false));
    TEST_ASSERT_EQUAL(0, cctx.handle & 0xff);
    uint32_t pinned[BLE_LOG_POOL_TRANS_CNT] = {0};
    int pinned_cnt = 0;
    for (; pinned_cnt < BLE_LOG_POOL_SHARED_CNT - 1; pinned_cnt++) {
        TEST_ASSERT_NOT_NULL(ble_log_claim(BLE_LOG_SRC_ENCODE, 1,
                                           &pinned[pinned_cnt], false));
    }

    /* The canary shares the writer's core: it only progresses while the
     * writer is parked; a spinning writer monopolizes the core. */
    TEST_ASSERT_EQUAL(pdTRUE,
                      xTaskCreatePinnedToCore(spin_canary_task, "ble_log_canary",
                                              TEST_LIFECYCLE_STACK_SIZE, &canary,
                                              1, NULL, 1));
    vTaskDelay(pdMS_TO_TICKS(20));
    uint32_t canary_base = canary.loops;
    TEST_ASSERT_GREATER_THAN(0, canary_base);

    TEST_ASSERT_EQUAL(pdTRUE,
                      xTaskCreatePinnedToCore(spin_writer_task, "ble_log_spin",
                                              TEST_LIFECYCLE_STACK_SIZE, &wctx,
                                              TEST_LIFECYCLE_PRIO + 1, NULL, 1));
    TEST_ASSERT_EQUAL(pdTRUE,
                      xTaskCreatePinnedToCore(spin_canceller_task, "ble_log_cancel",
                                              TEST_LIFECYCLE_STACK_SIZE, &cctx,
                                              TEST_LIFECYCLE_PRIO - 1, NULL, 0));

    TEST_ASSERT_TRUE(xSemaphoreTake(s_spin_writer_entered, pdMS_TO_TICKS(1000)));
    /* Let the writer reach its steady state (parked or spinning), then
     * sample the canary across a second window: a parked writer leaves
     * the core to the canary; a spinning writer monopolizes it. */
    vTaskDelay(pdMS_TO_TICKS(30));
    uint32_t canary_settled = canary.loops;
    vTaskDelay(pdMS_TO_TICKS(100));
    uint32_t canary_now = canary.loops;
    bool completed_early = xSemaphoreTake(wctx.done, 0) == pdTRUE;
    printf("B1 sample: writer completed_early=%d canary %u -> %u during hold (core1: %s)\n",
           completed_early, (unsigned)canary_settled, (unsigned)canary_now,
           canary_now == canary_settled ? "MONOPOLIZED by spinning writer" : "progressing, writer parked");

    /* Let the suspended publisher finish the recycle and observe the
     * recovery: the writer must complete once the bitmap is published. */
    xSemaphoreGive(s_spin_canceller_hold);
    TEST_ASSERT_TRUE(xSemaphoreTake(cctx.done, pdMS_TO_TICKS(1000)));
    TEST_ASSERT_TRUE(xSemaphoreTake(wctx.done, pdMS_TO_TICKS(1000)));
    TEST_ASSERT_TRUE(wctx.result);

    for (int i = 0; i < pinned_cnt; i++) {
        ble_log_commit(pinned[i], 0);
    }
    ble_log_lbm_flush_open_trans();
    TEST_ASSERT_TRUE(ble_log_rt_drain());
    while (ble_log_prph_test_read(s_read_buf, sizeof(s_read_buf),
                                  0, 0, NULL) > 0) {
    }

    canary.stop = true;
    vTaskDelay(pdMS_TO_TICKS(5));
    vSemaphoreDelete(wctx.done);
    vSemaphoreDelete(cctx.done);
    vSemaphoreDelete(s_spin_writer_start);
    vSemaphoreDelete(s_spin_writer_entered);
    vSemaphoreDelete(s_spin_canceller_hold);
    s_spin_writer_start = NULL;
    s_spin_writer_entered = NULL;
    s_spin_canceller_hold = NULL;

    /* Contract: while its only exit is suspended, the waiter must be
     * PARKED (canary progresses across the hold window), never spinning
     * on self-minted tokens (canary frozen = core monopolized). */
    TEST_ASSERT_GREATER_THAN(canary_settled, canary_now);
}

/* ------------------------------------------------------------------ */
/* Review blocker repro: flush drain straddle.                         */
/* The drain loop reads the writer refcount and the waiter count as    */
/* two separate loads. A writer waking between them is in-flight (ref  */
/* re-acquired, waiter unregistered, gate not yet checked) when the    */
/* drain concludes; flush then snapshots and re-enables producers      */
/* while the pre-existing writer still owns its reference.             */
/* ------------------------------------------------------------------ */
static volatile bool s_straddle_acquire_armed;
static volatile bool s_straddle_flush_armed;
static volatile int s_straddle_flush_evals;
static volatile bool s_straddle_hog_stop;
static volatile bool s_straddle_writer_frozen_at_return;
static volatile bool s_straddle_writer_result;
static volatile int64_t s_straddle_flush_us;
static SemaphoreHandle_t s_straddle_freeze;
static SemaphoreHandle_t s_straddle_inflight;
static SemaphoreHandle_t s_straddle_flush_start;
static SemaphoreHandle_t s_straddle_flush_done;
static SemaphoreHandle_t s_straddle_writer_started;
static SemaphoreHandle_t s_straddle_writer_done;
static SemaphoreHandle_t s_straddle_reader_start;

void ble_log_test_acquire_after_unregister_hook(void);
void ble_log_test_flush_drain_between_loads_hook(void);
static void straddle_hog_task(void *arg);

void ble_log_test_acquire_after_unregister_hook(void)
{
    if (!s_straddle_acquire_armed) {
        return;
    }
    s_straddle_acquire_armed = false;
    /* The writer is in-flight: reference re-acquired, waiter
     * unregistered, producer gate not yet observed. Any preemption of
     * this task leaves exactly this state behind. */
    xSemaphoreGive(s_straddle_inflight);
    (void)xSemaphoreTake(s_straddle_freeze, portMAX_DELAY);
}

void ble_log_test_flush_drain_between_loads_hook(void)
{
    if (!s_straddle_flush_armed) {
        return;
    }
    switch (++s_straddle_flush_evals) {
    case 1:
        /* Evaluation #1, before this iteration's wake_all: start a hog on
         * the writer's core (idle: the writer is parked) so the wake token
         * minted moments later stays unconsumed. */
        (void)xTaskCreatePinnedToCore(straddle_hog_task, "ble_log_strh",
                                      TEST_LIFECYCLE_STACK_SIZE, NULL,
                                      TEST_LIFECYCLE_PRIO + 2, NULL, 0);
        break;
    case 2:
        /* Evaluation #2, between the two condition loads: stop the hog so
         * the parked writer consumes the wake and runs its real post-take
         * handoff steps (re-acquire + unregister) inside this window.
         * Only now may the link recycle transports: the writer is
         * in-flight and unregistered, so recycles mint no wake tokens. */
        s_straddle_flush_armed = false;
        s_straddle_hog_stop = true;
        if (xSemaphoreTake(s_straddle_inflight, pdMS_TO_TICKS(2000)) == pdTRUE) {
            xSemaphoreGive(s_straddle_reader_start);
        }
        break;
    default:
        break;
    }
}

static void straddle_writer_task(void *arg)
{
    (void)arg;
    xSemaphoreGive(s_straddle_writer_started);
    const uint8_t marker = 0xD2;
    s_straddle_writer_result = ble_log_write_hex(BLE_LOG_SRC_CUSTOM, &marker, 1);
    xSemaphoreGive(s_straddle_writer_done);
    vTaskDelete(NULL);
}

static void straddle_flusher_task(void *arg)
{
    (void)arg;
    (void)xSemaphoreTake(s_straddle_flush_start, portMAX_DELAY);
    int64_t t0 = esp_timer_get_time();
    ble_log_flush();
    s_straddle_flush_us = esp_timer_get_time() - t0;
    s_straddle_writer_frozen_at_return = !s_straddle_acquire_armed;
    xSemaphoreGive(s_straddle_flush_done);
    vTaskDelete(NULL);
}

static void straddle_hog_task(void *arg)
{
    (void)arg;
    while (!s_straddle_hog_stop) {
    }
    vTaskDelete(NULL);
}

static volatile bool s_straddle_reader_stop;

static void straddle_reader_task(void *arg)
{
    (void)arg;
    /* Recycle transports the way a real link does, so flush_all_trans
     * completes and the measured flush duration reflects the writer
     * drain rather than the transport wait. Blocked until the drain has
     * handed the writer off: recycling earlier would wake the parked
     * writer before the straddle point. */
    (void)xSemaphoreTake(s_straddle_reader_start, portMAX_DELAY);
    while (!s_straddle_reader_stop) {
        (void)ble_log_prph_test_read(s_read_buf, sizeof(s_read_buf),
                                    pdMS_TO_TICKS(10), 0, NULL);
    }
    vTaskDelete(NULL);
}

TEST_CASE("BLE Log flush drain does not straddle an in-flight parked writer",
          "[ble_log][lbm][repro]")
{
    static const uint8_t full_payload[
        BLE_LOG_MAX_PAYLOAD_LEN - sizeof(uint32_t)] = {0};

    s_straddle_freeze = xSemaphoreCreateBinary();
    s_straddle_inflight = xSemaphoreCreateBinary();
    s_straddle_flush_start = xSemaphoreCreateBinary();
    s_straddle_flush_done = xSemaphoreCreateBinary();
    s_straddle_writer_started = xSemaphoreCreateBinary();
    s_straddle_writer_done = xSemaphoreCreateBinary();
    s_straddle_reader_start = xSemaphoreCreateBinary();
    TEST_ASSERT_NOT_NULL(s_straddle_freeze);
    TEST_ASSERT_NOT_NULL(s_straddle_inflight);
    TEST_ASSERT_NOT_NULL(s_straddle_flush_start);
    TEST_ASSERT_NOT_NULL(s_straddle_flush_done);
    TEST_ASSERT_NOT_NULL(s_straddle_writer_started);
    TEST_ASSERT_NOT_NULL(s_straddle_writer_done);
    TEST_ASSERT_NOT_NULL(s_straddle_reader_start);

    TEST_ASSERT_TRUE(ble_log_enable(true));
    ble_log_lbm_flush_open_trans();
    for (int round = 0; round < 2; round++) {
        TEST_ASSERT_TRUE(ble_log_rt_drain());
        while (ble_log_prph_test_read(s_read_buf, sizeof(s_read_buf),
                                      0, 0, NULL) > 0) {
        }
    }

    /* Keep every task-usable transport SENDING in the test peripheral. */
    for (int i = 0; i < BLE_LOG_POOL_SHARED_CNT; i++) {
        TEST_ASSERT_TRUE(ble_log_write_hex(BLE_LOG_SRC_CUSTOM, full_payload,
                                           sizeof(full_payload)));
    }
    TEST_ASSERT_TRUE(ble_log_rt_drain());

    /* The writer parks on the full shared pool. */
    TEST_ASSERT_EQUAL(pdTRUE,
                      xTaskCreatePinnedToCore(straddle_writer_task, "ble_log_strw",
                                              TEST_LIFECYCLE_STACK_SIZE, NULL,
                                              TEST_LIFECYCLE_PRIO, NULL, 0));
    TEST_ASSERT_TRUE(xSemaphoreTake(s_straddle_writer_started, pdMS_TO_TICKS(1000)));
    vTaskDelay(pdMS_TO_TICKS(10));

    /* The eval#1 hook starts the hog on the writer's core so the first
     * wake stays unconsumed; the eval#2 hook stops it exactly between
     * the two condition loads. */
    s_straddle_flush_evals = 0;
    s_straddle_flush_armed = true;
    s_straddle_acquire_armed = true;
    s_straddle_hog_stop = false;
    s_straddle_reader_stop = false;
    s_straddle_writer_frozen_at_return = false;
    TEST_ASSERT_EQUAL(pdTRUE,
                      xTaskCreate(straddle_reader_task, "ble_log_strr",
                                  TEST_READER_STACK_SIZE, NULL,
                                  TEST_READER_PRIO, NULL));
    TEST_ASSERT_EQUAL(pdTRUE,
                      xTaskCreatePinnedToCore(straddle_flusher_task, "ble_log_strf",
                                              TEST_READER_STACK_SIZE, NULL,
                                              TEST_READER_PRIO + 2, NULL, 1));
    xSemaphoreGive(s_straddle_flush_start);

    TEST_ASSERT_TRUE(xSemaphoreTake(s_straddle_flush_done, pdMS_TO_TICKS(10000)));
    int64_t flush_ms = s_straddle_flush_us / 1000;
    bool writer_done_early = xSemaphoreTake(s_straddle_writer_done, 0) == pdTRUE;
    printf("B2 sample: flush returned in %lld ms, writer in-flight at return=%d, writer already done=%d\n",
           (long long)flush_ms, s_straddle_writer_frozen_at_return,
           writer_done_early);

    /* Transports are already claimable (the reader kept the link busy);
     * release the frozen writer: on the straddle it observes the
     * re-enabled gate and completes its pre-flush write after flush
     * already returned. */
    xSemaphoreGive(s_straddle_freeze);
    TEST_ASSERT_TRUE(xSemaphoreTake(s_straddle_writer_done, pdMS_TO_TICKS(1000)));
    printf("B2 outcome: frozen writer passed the re-enabled gate and wrote=%d\n",
           s_straddle_writer_result);

    s_straddle_reader_stop = true;
    vTaskDelay(pdMS_TO_TICKS(30));
    ble_log_lbm_flush_open_trans();
    TEST_ASSERT_TRUE(ble_log_rt_drain());
    while (ble_log_prph_test_read(s_read_buf, sizeof(s_read_buf),
                                  0, 0, NULL) > 0) {
    }

    vSemaphoreDelete(s_straddle_freeze);
    vSemaphoreDelete(s_straddle_inflight);
    vSemaphoreDelete(s_straddle_flush_start);
    vSemaphoreDelete(s_straddle_flush_done);
    vSemaphoreDelete(s_straddle_writer_started);
    vSemaphoreDelete(s_straddle_writer_done);
    vSemaphoreDelete(s_straddle_reader_start);
    s_straddle_freeze = NULL;
    s_straddle_inflight = NULL;
    s_straddle_flush_start = NULL;
    s_straddle_flush_done = NULL;
    s_straddle_writer_started = NULL;
    s_straddle_writer_done = NULL;
    s_straddle_reader_start = NULL;

    /* Contract: when a writer was in-flight at flush return, flush must
     * have held the drain for the full documented timeout (1s) instead
     * of concluding between the two counter loads. */
    if (s_straddle_writer_frozen_at_return) {
        TEST_ASSERT_GREATER_OR_EQUAL(950, (int)flush_ms);
    }
}
