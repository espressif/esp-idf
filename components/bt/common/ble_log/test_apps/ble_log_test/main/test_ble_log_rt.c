/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "esp_chip_info.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/task.h"
#include "unity.h"

#include "ble_log.h"
#include "ble_log_lbm_v2.h"
#include "ble_log_prph_test.h"
#include "ble_log_rt.h"
#include "test_ble_log_main.h"
#if CONFIG_BLE_HOST_COMPRESSED_LOG_ENABLE
#include "log_compression/utils.h"
#endif

#if !CONFIG_BLE_LOG_PRPH_TEST
#error "BLE Log test app requires CONFIG_BLE_LOG_PRPH_TEST"
#endif

/* The runtime dispatch hook is throttled to one pass per
 * BLE_LOG_TS_TRIGGER_TIMEOUT_MS; let the window elapse between write bursts
 * so a hook pass is guaranteed to run after the settle delay. */
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
static uint32_t s_stale_claim_handle;
static volatile bool s_locked_hook_armed;
static volatile bool s_enable_hook_armed;
static SemaphoreHandle_t s_enable_hook_entered;
static SemaphoreHandle_t s_enable_hook_continue;
static volatile bool s_disable_hook_armed;
static SemaphoreHandle_t s_disable_hook_entered;
static SemaphoreHandle_t s_disable_hook_continue;
#if CONFIG_BLE_HOST_COMPRESSED_LOG_ENABLE
static volatile bool s_compression_hook_armed;
static SemaphoreHandle_t s_compression_hook_entered;
static SemaphoreHandle_t s_compression_hook_continue;
#endif

void ble_log_test_claim_pre_publish_hook(void);
void ble_log_test_claim_locked_hook(void);
void ble_log_test_enable_before_lifecycle_lock_hook(void);
void ble_log_test_disable_before_wake_hook(void);
#if CONFIG_BLE_HOST_COMPRESSED_LOG_ENABLE
void ble_log_test_compression_after_lock_hook(uint8_t source);
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
        ble_log_lbm_flush_open_transports();
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

#if CONFIG_BLE_HOST_COMPRESSED_LOG_ENABLE
void ble_log_test_compression_after_lock_hook(uint8_t source)
{
    if (s_compression_hook_armed &&
            source == BLE_COMPRESSED_LOG_OUT_SOURCE_HOST) {
        xSemaphoreGive(s_compression_hook_entered);
        xSemaphoreTake(s_compression_hook_continue, portMAX_DELAY);
    }
}
#endif

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

TEST_CASE("BLE Log v7 framing matches golden bytes", "[ble_log][wire]")
{
    static const uint8_t golden_frame[] = {
        0x05, 0x00, 0x87, 0xde, 0xc0, 0x00,
        0x78, 0x56, 0x34, 0x12, 0xab,
        0xf1, 0x12, 0x54, 0x88,
    };
    static const uint8_t golden_payload[] = {
        0x78, 0x56, 0x34, 0x12, 0xab,
    };

    TEST_ASSERT_EQUAL_UINT8(7, BLE_LOG_VERSION);
    TEST_ASSERT_EQUAL_HEX8(0x80, BLE_LOG_SRC_FLAG_NON_YIELD);
    TEST_ASSERT_EQUAL_UINT8(1, BLE_LOG_SRC_CORE_FIRST);
    TEST_ASSERT_EQUAL_UINT8(7, BLE_LOG_SRC_CORE_COUNT);
    TEST_ASSERT_EQUAL_UINT8(7, BLE_LOG_SRC_ENCODE);
    TEST_ASSERT_EQUAL_UINT8(7, BLE_LOG_INT_SRC_VERSION_INFO);
    TEST_ASSERT_EQUAL_UINT8(8, BLE_LOG_INT_SRC_SNAPSHOT);
    TEST_ASSERT_EQUAL_size_t(8, sizeof(ble_log_source_stat_t));
    TEST_ASSERT_EQUAL_size_t(134, sizeof(ble_log_internal_snapshot_t));
    TEST_ASSERT_EQUAL_size_t(
        4, offsetof(ble_log_internal_snapshot_t, version_info));
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
        0x00c0de87, BLE_LOG_MAKE_FRAME_META(0x87, 0x00c0de));
    TEST_ASSERT_EQUAL_HEX32(
        0x00000007, BLE_LOG_MAKE_FRAME_META(0x07, 0x01000000));

    golden_capture_t capture = {0};
    TEST_ASSERT_TRUE(test_ble_log_walk_frames(golden_frame,
                                              sizeof(golden_frame),
                                              capture_golden_frame,
                                              &capture));
    TEST_ASSERT_EQUAL_size_t(1, capture.count);
    TEST_ASSERT_EQUAL_UINT8(BLE_LOG_SRC_ENCODE, capture.frame.src);
    TEST_ASSERT_EQUAL_HEX8(0x87, capture.frame.source_meta);
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

typedef struct {
    bool task_frame;
    bool non_yield_frame;
    bool hci_downstream_frame;
    bool hci_upstream_frame;
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
        capture->task_frame = !BLE_LOG_SRC_IS_NON_YIELD(frame->source_meta);
    } else if (frame->src == BLE_LOG_SRC_CUSTOM && marker == 0x22) {
        capture->non_yield_frame =
            BLE_LOG_SRC_IS_NON_YIELD(frame->source_meta);
    } else if (frame->src == BLE_LOG_SRC_HCI && marker == 0x01) {
        capture->hci_downstream_frame = true;
    } else if (frame->src == BLE_LOG_SRC_HCI && marker == 0x82) {
        capture->hci_upstream_frame = true;
    } else if (frame->src == BLE_LOG_SRC_ENCODE && marker == 0x33) {
        capture->claimed_frame = true;
    } else if (frame->src == BLE_LOG_SRC_ENCODE && marker == 0x44) {
        capture->stale_protected_frame = true;
    }
}

TEST_CASE("BLE Log marks non-yield context and commits claimed payload",
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

    uint32_t handle;
    uint8_t *claimed = ble_log_claim(BLE_LOG_SRC_ENCODE, 8, &handle);
    TEST_ASSERT_NOT_NULL(claimed);
    claimed[0] = 0x33;
    ble_log_commit(handle, 1);

    /* Invoke the stale commit immediately before the next claim publishes
     * CLAIMED. It must see the prior OPEN state and leave the new claim
     * intact. */
    s_stale_claim_handle = handle;
    s_claim_hook_armed = true;
    uint32_t fresh_handle;
    claimed = ble_log_claim(BLE_LOG_SRC_ENCODE, 8, &fresh_handle);
    s_claim_hook_armed = false;
    TEST_ASSERT_NOT_NULL(claimed);
    TEST_ASSERT_NOT_EQUAL(handle, fresh_handle);
    /* The fresh claim is published: the same stale handle must now be
     * rejected by the generation check instead. A zero length mirrors a
     * stale abort, which would otherwise consume the fresh claim. */
    ble_log_commit(s_stale_claim_handle, 0);
    claimed[0] = 0x44;
    ble_log_commit(fresh_handle, 1);

    ble_log_lbm_flush_open_transports();
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
    TEST_ASSERT_TRUE(capture.non_yield_frame);
    TEST_ASSERT_TRUE(capture.hci_downstream_frame);
    TEST_ASSERT_TRUE(capture.hci_upstream_frame);
    TEST_ASSERT_TRUE(capture.claimed_frame);
    TEST_ASSERT_TRUE(capture.stale_protected_frame);
    /* Exactly the two committed claims: a stale commit that slipped past
     * the state or generation check would add or consume an ENCODE frame. */
    TEST_ASSERT_EQUAL(2, capture.encode_frame_cnt);
}

#if CONFIG_BLE_HOST_COMPRESSED_LOG_ENABLE
#define TEST_CP_INDEX_FIRST    UINT16_C(0x601)
#define TEST_CP_INDEX_DROPPED  UINT16_C(0x602)
#define TEST_CP_INDEX_SECOND   UINT16_C(0x603)
#define TEST_CP_TASK_SWITCH    UINT8_C(0xc2)
#define TEST_CP_ZERO_ARGS      UINT8_C(0x40)

typedef struct {
    bool first_found;
    uint32_t first_sn;
    bool dropped_found;
    bool second_found;
    bool second_switched;
    uint32_t second_sn;
    bool third_found;
    bool third_switched;
    uint32_t third_sn;
} compression_capture_t;

typedef struct {
    SemaphoreHandle_t committed;
    SemaphoreHandle_t exit;
    SemaphoreHandle_t exited;
} compression_writer_ctx_t;

static void capture_compressed_frame(const test_ble_log_frame_t *frame,
                                     void *ctx)
{
    compression_capture_t *capture = ctx;
    if (frame->src != BLE_LOG_SRC_ENCODE ||
            frame->payload_len < sizeof(uint32_t) + 4) {
        return;
    }

    const uint8_t *record = frame->payload + sizeof(uint32_t);
    size_t offset = 0;
    if (record[offset++] != BLE_COMPRESSED_LOG_OUT_SOURCE_HOST) {
        return;
    }
    size_t record_len = frame->payload_len - sizeof(uint32_t);
    bool switched = record[offset] == TEST_CP_TASK_SWITCH;
    offset += switched;
    if (record_len - offset < 3 || record[offset++] != TEST_CP_ZERO_ARGS) {
        return;
    }
    uint16_t log_index;
    memcpy(&log_index, record + offset, sizeof(log_index));

    if (log_index == TEST_CP_INDEX_FIRST) {
        capture->first_found = true;
        capture->first_sn = frame->sn;
    } else if (log_index == TEST_CP_INDEX_DROPPED) {
        capture->dropped_found = true;
    } else if (log_index == TEST_CP_INDEX_SECOND) {
        capture->second_found = true;
        capture->second_switched = switched;
        capture->second_sn = frame->sn;
    } else if (log_index == TEST_CP_INDEX_SECOND + 1) {
        capture->third_found = true;
        capture->third_switched = switched;
        capture->third_sn = frame->sn;
    }
}

static void compression_writer_task(void *arg)
{
    compression_writer_ctx_t *ctx = arg;
    ble_log_compressed_hex_print(BLE_COMPRESSED_LOG_OUT_SOURCE_HOST,
                                 TEST_CP_INDEX_SECOND, 0);
    xSemaphoreGive(ctx->committed);
    xSemaphoreTake(ctx->exit, portMAX_DELAY);
    xSemaphoreGive(ctx->exited);
    vTaskDelete(NULL);
}

TEST_CASE("BLE Log serializes task context per compression source",
          "[ble_log][compression]")
{
    TEST_ASSERT_TRUE(ble_log_enable(true));
    ble_log_lbm_flush_open_transports();
    for (int round = 0; round < 2; round++) {
        TEST_ASSERT_TRUE(ble_log_rt_drain());
        while (ble_log_prph_test_read(s_read_buf, sizeof(s_read_buf),
                                      0, 0, NULL) > 0) {
        }
    }

    ble_log_compressed_hex_print(BLE_COMPRESSED_LOG_OUT_SOURCE_HOST,
                                 TEST_CP_INDEX_FIRST, 0);

    s_compression_hook_entered = xSemaphoreCreateBinary();
    s_compression_hook_continue = xSemaphoreCreateBinary();
    compression_writer_ctx_t writer = {
        .committed = xSemaphoreCreateBinary(),
        .exit = xSemaphoreCreateBinary(),
        .exited = xSemaphoreCreateBinary(),
    };
    TEST_ASSERT_NOT_NULL(s_compression_hook_entered);
    TEST_ASSERT_NOT_NULL(s_compression_hook_continue);
    TEST_ASSERT_NOT_NULL(writer.committed);
    TEST_ASSERT_NOT_NULL(writer.exit);
    TEST_ASSERT_NOT_NULL(writer.exited);

    s_compression_hook_armed = true;
    TEST_ASSERT_EQUAL(
        pdTRUE,
        xTaskCreate(compression_writer_task, "ble_log_cp",
                    TEST_LIFECYCLE_STACK_SIZE, &writer,
                    TEST_LIFECYCLE_PRIO, NULL));
    TEST_ASSERT_TRUE(xSemaphoreTake(s_compression_hook_entered,
                                    pdMS_TO_TICKS(1000)));

    /* This call claims pool space but fails the per-source trylock. It must
     * cancel immediately, count one lost ENCODE SN, and leave task state to
     * the lock owner. */
    ble_log_compressed_hex_print(BLE_COMPRESSED_LOG_OUT_SOURCE_HOST,
                                 TEST_CP_INDEX_DROPPED, 0);
    xSemaphoreGive(s_compression_hook_continue);
    TEST_ASSERT_TRUE(xSemaphoreTake(writer.committed,
                                    pdMS_TO_TICKS(1000)));
    s_compression_hook_armed = false;

    ble_log_compressed_hex_print(BLE_COMPRESSED_LOG_OUT_SOURCE_HOST,
                                 TEST_CP_INDEX_SECOND + 1, 0);
    xSemaphoreGive(writer.exit);
    TEST_ASSERT_TRUE(xSemaphoreTake(writer.exited, pdMS_TO_TICKS(1000)));

    ble_log_lbm_flush_open_transports();
    TEST_ASSERT_TRUE(ble_log_rt_drain());
    compression_capture_t capture = {0};
    for (int i = 0; i < BLE_LOG_TRANS_TOTAL_CNT; i++) {
        size_t len = ble_log_prph_test_read(
            s_read_buf, sizeof(s_read_buf), pdMS_TO_TICKS(TEST_READ_TIMEOUT_MS),
            0, NULL);
        if (!len) {
            break;
        }
        TEST_ASSERT_TRUE(test_ble_log_walk_frames(
            s_read_buf, len, capture_compressed_frame, &capture));
    }

    TEST_ASSERT_TRUE(capture.first_found);
    TEST_ASSERT_FALSE(capture.dropped_found);
    TEST_ASSERT_TRUE(capture.second_found);
    TEST_ASSERT_TRUE(capture.second_switched);
    TEST_ASSERT_TRUE(capture.third_found);
    TEST_ASSERT_TRUE(capture.third_switched);
    /* The blocked writer claims its SN before taking the compression lock;
     * the rejected contender burns the following SN. */
    TEST_ASSERT_EQUAL_HEX32((capture.first_sn + 1) & 0x00ffffffU,
                            capture.second_sn);
    TEST_ASSERT_EQUAL_HEX32((capture.second_sn + 2) & 0x00ffffffU,
                            capture.third_sn);

    vSemaphoreDelete(writer.committed);
    vSemaphoreDelete(writer.exit);
    vSemaphoreDelete(writer.exited);
    vSemaphoreDelete(s_compression_hook_entered);
    vSemaphoreDelete(s_compression_hook_continue);
    s_compression_hook_entered = NULL;
    s_compression_hook_continue = NULL;
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
    ble_log_lbm_flush_open_transports();
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

    ble_log_lbm_flush_open_transports();
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
    ble_log_lbm_flush_open_transports();
    for (int round = 0; round < 2; round++) {
        TEST_ASSERT_TRUE(ble_log_rt_drain());
        while (ble_log_prph_test_read(s_read_buf, sizeof(s_read_buf),
                                      0, 0, NULL) > 0) {
        }
    }

    TEST_ASSERT_TRUE(ble_log_write_hex(BLE_LOG_SRC_CUSTOM,
                                       &before_marker, sizeof(before_marker)));
    ble_log_lbm_flush_open_transports();
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
    ble_log_lbm_flush_open_transports();
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
    ble_log_lbm_flush_open_transports();
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

    ble_log_lbm_flush_open_transports();
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
    ble_log_lbm_flush_open_transports();
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
    ble_log_lbm_drain_open_transports();

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
    ble_log_lbm_flush_open_transports();
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

TEST_CASE("BLE Log periodic snapshot fails fast while its transport is busy",
          "[ble_log][lbm]")
{
    TEST_ASSERT_TRUE(ble_log_enable(true));
    ble_log_lbm_flush_open_transports();
    for (int round = 0; round < 2; round++) {
        TEST_ASSERT_TRUE(ble_log_rt_drain());
        while (ble_log_prph_test_read(s_read_buf, sizeof(s_read_buf),
                                      0, 0, NULL) > 0) {
        }
    }

    TEST_ASSERT_TRUE(ble_log_enable(false));
    TEST_ASSERT_FALSE(ble_log_internal_snapshot(
        BLE_LOG_SNAPSHOT_REASON_PERIODIC, NULL, false));
    TEST_ASSERT_TRUE(ble_log_enable(true));

    TEST_ASSERT_TRUE(ble_log_internal_snapshot(
        BLE_LOG_SNAPSHOT_REASON_PERIODIC, NULL, false));
    TEST_ASSERT_FALSE(ble_log_internal_snapshot(
        BLE_LOG_SNAPSHOT_REASON_PERIODIC, NULL, false));
    TEST_ASSERT_TRUE(ble_log_rt_drain());
    TEST_ASSERT_GREATER_THAN_size_t(
        0, ble_log_prph_test_read(s_read_buf, sizeof(s_read_buf),
                                  pdMS_TO_TICKS(TEST_READ_TIMEOUT_MS), 0, NULL));

    TEST_ASSERT_TRUE(ble_log_internal_snapshot(
        BLE_LOG_SNAPSHOT_REASON_PERIODIC, NULL, false));
    TEST_ASSERT_TRUE(ble_log_rt_drain());

    snapshot_capture_t capture = {0};
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
    ble_log_lbm_flush_open_transports();
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
    TEST_ASSERT_FALSE(ble_log_write_hex(BLE_LOG_SRC_CUSTOM,
                                        &reserve_marker,
                                        sizeof(reserve_marker)));
    uint32_t exhausted_handle;
    TEST_ASSERT_NULL(ble_log_claim(BLE_LOG_SRC_ENCODE, 1,
                                   &exhausted_handle));

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
