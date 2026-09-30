/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* --------------------------------------------------------------- */
/* BLE Log USB CDC smoke / throughput / raw-pipe test app          */
/* --------------------------------------------------------------- */

/* INCLUDE */
#include <stdlib.h>

#include "ble_log.h"
#include "ble_log_lbm_v2.h"
#include "sdkconfig.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#if CONFIG_BLE_LOG_USB_TEST_RAW_PIPE_MODE
#include "tinyusb.h"
#include "tinyusb_default_config.h"
#include "tusb.h"
#endif

/* MACRO */
#define TAG "ble_log_usb_test"

/* Smoke mode: one 8-byte payload every 100 ms (functional check). */
#define SMOKE_PAYLOAD_LEN 8
#define SMOKE_INTERVAL_MS 100

/* Perf mode: back-to-back writes with a sequence-carrying payload. The
 * host-side parser counts frames and bytes and checks the per-source SN
 * continuity and payload checksum; report throughput in MB/s. */
#define PERF_PAYLOAD_LEN CONFIG_BLE_LOG_USB_TEST_PERF_PAYLOAD_LEN
#define PERF_DURATION_S  CONFIG_BLE_LOG_USB_TEST_PERF_DURATION_S

#if !CONFIG_BLE_LOG_USB_TEST_PERF_MODE && !CONFIG_BLE_LOG_USB_TEST_RAW_PIPE_MODE && \
    !CONFIG_BLE_LOG_USB_TEST_LIFECYCLE_MODE
/* VARIABLE */
static uint32_t s_seq;

/* PRIVATE FUNCTION */
/* Smoke payload: 4-byte little-endian sequence + fixed marker. */
static void smoke_payload_fill(uint8_t *buf, size_t len)
{
    buf[0] = (uint8_t)(s_seq);
    buf[1] = (uint8_t)(s_seq >> 8);
    buf[2] = (uint8_t)(s_seq >> 16);
    buf[3] = (uint8_t)(s_seq >> 24);
    buf[4] = 0xA5;
    buf[5] = 0x5A;
    for (size_t i = 6; i < len; i++) {
        buf[i] = (uint8_t)i;
    }
}

/* INTERFACE */
void app_main(void)
{
    if (!ble_log_init()) {
        ESP_LOGE(TAG, "ble_log_init failed");
        return;
    }
    if (!ble_log_enable(true)) {
        ESP_LOGE(TAG, "ble_log_enable failed");
        return;
    }

    ESP_LOGI(TAG, "USB CDC log running; open the serial port on the host");
    while (1) {
        uint8_t payload[SMOKE_PAYLOAD_LEN];
        smoke_payload_fill(payload, sizeof(payload));
        (void)ble_log_write_hex(BLE_LOG_SRC_CUSTOM, payload, sizeof(payload));
        s_seq++;
        vTaskDelay(pdMS_TO_TICKS(SMOKE_INTERVAL_MS));
    }
}
#elif CONFIG_BLE_LOG_USB_TEST_PERF_MODE
/* VARIABLE */
static uint32_t s_seq;
static uint32_t s_written;
static uint32_t s_rejected;

/* INTERFACE */
void app_main(void)
{
    if (!ble_log_init()) {
        ESP_LOGE(TAG, "ble_log_init failed");
        return;
    }
    if (!ble_log_enable(true)) {
        ESP_LOGE(TAG, "ble_log_enable failed");
        return;
    }

    ESP_LOGI(TAG,
             "perf: payload %d bytes, duration %d s; open the port and read",
             PERF_PAYLOAD_LEN, PERF_DURATION_S);

    /* The write loop is CPU-bound with no delay: BLE Log backpressure
     * (pool wait) and the USB path absorb what they can, and the host
     * measures the delivered throughput. The UART console keeps showing
     * the loss warnings for context. */
    const TickType_t deadline = xTaskGetTickCount() +
                                pdMS_TO_TICKS(PERF_DURATION_S * 1000);
    while (xTaskGetTickCount() < deadline) {
        uint8_t payload[PERF_PAYLOAD_LEN];
        s_seq++;
        payload[0] = (uint8_t)(s_seq);
        payload[1] = (uint8_t)(s_seq >> 8);
        payload[2] = (uint8_t)(s_seq >> 16);
        payload[3] = (uint8_t)(s_seq >> 24);
        payload[4] = 0xA5;
        payload[5] = 0x5A;
        for (int i = 6; i < PERF_PAYLOAD_LEN; i++) {
            payload[i] = (uint8_t)i;
        }
        if (ble_log_write_hex(BLE_LOG_SRC_CUSTOM, payload, sizeof(payload))) {
            s_written++;
        } else {
            s_rejected++;
        }
    }

    ESP_LOGI(TAG, "perf: %lu frames accepted, %lu rejected in %d s",
             (unsigned long)s_written, (unsigned long)s_rejected,
             PERF_DURATION_S);
    ESP_LOGI(TAG, "perf: host-side delivered throughput is the metric; "
             "compare against the transport loss warnings");

    while (1) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
#elif CONFIG_BLE_LOG_USB_TEST_LIFECYCLE_MODE
/* Dispatcher teardown interleaving check. The USB dispatch task owns a
 * transport between the queue receive and the recycle, so deinit must not
 * tear the pool or the peripheral down while a dispatch call is in flight.
 *
 * The interleaving is forced, not raced: the pre-send test hook parks the
 * dispatch task inside the dispatch call while it owns exactly one
 * transport and the runtime queue is empty, deinit then runs from a second
 * task, and the check verifies that deinit has not completed while that
 * call is still parked and that the parked call finishes (was not aborted
 * mid-flight) after it is released. An empty queue matters: with transports
 * still queued, the flush drain inside deinit would serialize with the
 * dispatcher all by itself and hide the teardown bug. */

#define LIFECYCLE_HOLD_MS       100
#define LIFECYCLE_WAIT_MS       3000
/* The periodic trigger (1000 ms after enable) seals its own snapshot and
 * binding transports. A boundary inside this check would put transports back
 * into the queue behind the parked dispatcher and mask the teardown. Keep the
 * check well inside one period, and fail loudly instead of passing if it ever
 * runs longer than that. */
#define LIFECYCLE_PERIOD_GUARD_MS 900

/* VARIABLE */
static volatile bool s_hook_armed;
static volatile bool s_dispatch_parked;
static volatile bool s_dispatch_resumed;
static volatile bool s_release_dispatch;
static volatile bool s_deinit_done;
static volatile bool s_post_ack_armed;
static volatile bool s_post_ack_parked;
static volatile bool s_release_post_ack;

/* Test hook body: runs on the dispatch task, after the dequeue and before
 * the send. Park until the check releases this call. */
void ble_log_test_usb_dispatch_pre_send_hook(void)
{
    if (!s_hook_armed) {
        return;
    }
    s_dispatch_parked = true;
    while (!s_release_dispatch) {
        vTaskDelay(1);
    }
    /* Reaching this point proves this call survived the teardown attempt and
     * got past the park; the send and the recycle after it are not observed
     * here. */
    s_dispatch_resumed = true;
}

/* Test hook body: runs on the dispatch task after it published the stop ack
 * and before it suspends itself. Parking here holds the task in the window
 * where deinit has the ack but the handle is not safe to delete yet. */
void ble_log_test_usb_dispatch_post_ack_hook(void)
{
    if (!s_post_ack_armed) {
        return;
    }
    s_post_ack_parked = true;
    while (!s_release_post_ack) {
        vTaskDelay(1);
    }
}

/* PRIVATE FUNCTION */
static void lifecycle_deinit_task(void *arg)
{
    (void)arg;
    ble_log_deinit();
    s_deinit_done = true;
    vTaskDelete(NULL);
}

static bool lifecycle_wait_for(volatile bool *flag, uint32_t timeout_ms)
{
    const TickType_t deadline = xTaskGetTickCount() + pdMS_TO_TICKS(timeout_ms);
    while (!*flag) {
        if (xTaskGetTickCount() >= deadline) {
            return false;
        }
        vTaskDelay(1);
    }
    return true;
}

/* INTERFACE */
void app_main(void)
{
    if (!ble_log_init() || !ble_log_enable(true)) {
        ESP_LOGE(TAG, "lifecycle: ble_log init/enable failed");
        abort();
    }

    /* Settle the dispatcher first: the flush drain returns once the queue is
     * empty, and the delay lets any transport it already took finish. */
    ble_log_flush();
    vTaskDelay(pdMS_TO_TICKS(20));

    /* One maximum-length record fills a transport exactly: the pool seals a
     * transport whose free space no longer holds a frame. ble_log_write_hex
     * prepends a 4-byte timestamp, so the largest accepted record is
     * BLE_LOG_MAX_PAYLOAD_LEN minus that timestamp. That leaves no partially
     * filled transport behind, so the periodic flush has nothing to seal and
     * the only transport to dispatch is this one. */
    const TickType_t start_tick = xTaskGetTickCount();
    static uint8_t payload[BLE_LOG_MAX_PAYLOAD_LEN - sizeof(uint32_t)];
    s_hook_armed = true;
    if (!ble_log_write_hex(BLE_LOG_SRC_CUSTOM, payload, sizeof(payload))) {
        ESP_LOGE(TAG, "TEST FAIL: the sealing write was rejected");
        abort();
    }

    /* Deinit immediately: its drain lets the dispatcher dequeue the transport
     * and park on it, so the transport the dispatcher owns is the last one and
     * the runtime queue is empty by the time deinit decides about it. Waiting
     * for the park before starting deinit would let that same drain serialize
     * with the dispatcher and hide the teardown defect. */
    if (xTaskCreate(lifecycle_deinit_task, "lifecycle_deinit", 4096, NULL,
                    5, NULL) != pdTRUE) {
        ESP_LOGE(TAG, "lifecycle: deinit task create failed");
        abort();
    }
    if (!lifecycle_wait_for(&s_dispatch_parked, LIFECYCLE_WAIT_MS)) {
        ESP_LOGE(TAG, "TEST FAIL: no dispatch was parked");
        abort();
    }
    if ((xTaskGetTickCount() - start_tick) >=
            pdMS_TO_TICKS(LIFECYCLE_PERIOD_GUARD_MS)) {
        ESP_LOGE(TAG, "TEST FAIL: check reached the periodic boundary; re-run");
        abort();
    }

    vTaskDelay(pdMS_TO_TICKS(LIFECYCLE_HOLD_MS));
    if (s_deinit_done) {
        ESP_LOGE(TAG, "TEST FAIL: deinit completed while the dispatch task "
                 "still owned a transport");
        abort();
    }

    s_release_dispatch = true;
    if (!lifecycle_wait_for(&s_deinit_done, LIFECYCLE_WAIT_MS)) {
        ESP_LOGE(TAG, "TEST FAIL: deinit never completed after the release");
        abort();
    }
    if (!s_dispatch_resumed) {
        ESP_LOGE(TAG, "TEST FAIL: the parked dispatch call did not resume "
                 "after the release");
        abort();
    }

    ESP_LOGI(TAG, "TEST PASS: deinit waited for the dispatch task to finish "
             "the transport it owned");

    /* Second check: the stop ack and the suspended state are two different
     * instants. Park the dispatcher between them and require deinit to keep
     * waiting - deleting there would hand a still-running task to the
     * kernel's terminated-task cleanup and lose its TCB and stack. */
    s_deinit_done = false;
    s_hook_armed = false;
    if (!ble_log_init()) {
        ESP_LOGE(TAG, "TEST FAIL: re-init for the stop-window check failed");
        abort();
    }
    s_post_ack_armed = true;
    if (xTaskCreate(lifecycle_deinit_task, "lifecycle_deinit", 4096, NULL,
                    5, NULL) != pdTRUE) {
        ESP_LOGE(TAG, "lifecycle: deinit task create failed");
        abort();
    }
    if (!lifecycle_wait_for(&s_post_ack_parked, LIFECYCLE_WAIT_MS)) {
        ESP_LOGE(TAG, "TEST FAIL: no dispatch parked after its stop ack");
        abort();
    }
    vTaskDelay(pdMS_TO_TICKS(LIFECYCLE_HOLD_MS));
    if (s_deinit_done) {
        ESP_LOGE(TAG, "TEST FAIL: deinit completed between the stop ack and "
                 "the suspended state");
        abort();
    }
    s_release_post_ack = true;
    if (!lifecycle_wait_for(&s_deinit_done, LIFECYCLE_WAIT_MS)) {
        ESP_LOGE(TAG, "TEST FAIL: deinit never completed after the suspend");
        abort();
    }
    ESP_LOGI(TAG, "TEST PASS: deinit waited for the dispatcher to reach the "
             "suspended state");
    while (1) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
#else /* RAW_PIPE_MODE */
/* Raw pipe: same TinyUSB CDC device as the BLE Log peripheral, but a
 * dedicated producer task writes the CDC TX FIFO directly with no BLE
 * Log runtime, no LBM pool, and no ESP timer dispatch. It isolates the
 * USB path so the host can compare raw-pipe MB/s against BLE Log MB/s
 * and attribute the gap. The pattern is a fixed 640-byte block, the
 * BLE Log transport size, so packet-splitting costs match. */

#define RAW_PIPE_BLOCK 640
#define RAW_PIPE_DURATION_S 30

/* VARIABLE */
static uint32_t s_blocks;

/* INTERFACE */
void app_main(void)
{
    /* TinyUSB defaults: default descriptors (esp_tinyusb CDC default
     * PID 0x4001, identity does not matter for a throughput probe),
     * default task stack/priority, matching the BLE Log peripheral's
     * install path. */
    tinyusb_config_t tusb_cfg = TINYUSB_DEFAULT_CONFIG();
    if (tinyusb_driver_install(&tusb_cfg) != ESP_OK) {
        ESP_LOGE(TAG, "tinyusb_driver_install failed");
        return;
    }

    ESP_LOGI(TAG, "raw-pipe: %d-byte blocks for %d s; open the port",
             RAW_PIPE_BLOCK, RAW_PIPE_DURATION_S);

    /* GHWCFG2 probe removed after one-shot verification (2026-09-29):
     * GSNPSID=0x4f54400a, GHWCFG2.otgarch=2 (internal DMA),
     * GAHBCFG=0x27 (DMAEN=1, INCR4). Measured with this app; it is not a
     * configuration requirement.
     * Note the S3 register map differs from the DWC2 default layout
     * (GAHBCFG at 0x08, GHWCFG2 at 0x48; see soc/usb_dwc_struct.h). */

    static uint8_t block[RAW_PIPE_BLOCK];
    for (int i = 0; i < RAW_PIPE_BLOCK; i++) {
        block[i] = (uint8_t)i;
    }

    const TickType_t deadline = xTaskGetTickCount() +
                                pdMS_TO_TICKS(RAW_PIPE_DURATION_S * 1000);
    while (xTaskGetTickCount() < deadline) {
        /* Wait for the DTR connection like the BLE Log path does. */
        if (!tud_cdc_n_connected(0)) {
            vTaskDelay(pdMS_TO_TICKS(10));
            continue;
        }
        const size_t avail = tud_cdc_n_write_available(0);
        if (avail < RAW_PIPE_BLOCK) {
            /* FIFO cannot take a whole block; yield and retry. This is
             * a throughput probe, not a loss accounting path. */
            taskYIELD();
            continue;
        }
        if (tud_cdc_n_write(0, block, RAW_PIPE_BLOCK) == RAW_PIPE_BLOCK) {
            (void)tud_cdc_n_write_flush(0);
            s_blocks++;
        }
    }

    ESP_LOGI(TAG, "raw-pipe: %lu blocks (%lu bytes) in %d s",
             (unsigned long)s_blocks,
             (unsigned long)s_blocks * RAW_PIPE_BLOCK,
             RAW_PIPE_DURATION_S);
    while (1) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
#endif
