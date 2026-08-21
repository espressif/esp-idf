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
#include "ble_log_prph_test.h"
#include "ble_log_rt.h"
#include "test_ble_log_main.h"

#if !CONFIG_BLE_LOG_PRPH_TEST
#error "BLE Log test app requires CONFIG_BLE_LOG_PRPH_TEST"
#endif

/* The runtime task hook is throttled to one pass per
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
    /* Every frame payload starts with a 4-byte timestamp prefix */
    const uint8_t *record = frame->payload + sizeof(uint32_t);
    size_t record_len = frame->payload_len - sizeof(uint32_t);

    if (frame->src == BLE_LOG_SRC_INTERNAL &&
            record_len == sizeof(ble_log_version_info_t) &&
            record[0] == BLE_LOG_INT_SRC_VERSION_INFO) {
        memcpy(&capture->version_info, record, sizeof(capture->version_info));
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
                                            pdMS_TO_TICKS(TEST_READ_TIMEOUT_MS), 0);
        if (len > 0 &&
                !test_ble_log_walk_frames(s_read_buf, len, capture_version_info_frame,
                                          &ctx->capture)) {
            ctx->malformed = true;
        }
    }
    xSemaphoreGive(ctx->done);
    vTaskDelete(NULL);
}

TEST_CASE("BLE Log runtime hook reports build and chip versions", "[ble_log]")
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

    /* Transports are auto-submitted once full, which wakes the runtime task;
     * after the throttle window elapses, a hook pass writes the version frame
     * into the LBM and a later transport carries it out. ble_log_flush()
     * cannot be used here: it disables the module while waiting for the
     * transports to drain, so the hook frame written during the flush window
     * would be dropped. */
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

    vSemaphoreDelete(ctx.done);
}
