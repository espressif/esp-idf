/*
 * SPDX-FileCopyrightText: 2025-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* ----------------- */
/* BLE Log - Runtime */
/* ----------------- */

/* INCLUDE */
#include "ble_log.h"
#include "ble_log_rt.h"
#include "ble_log_lbm.h"

#include "esp_log.h"
#include "esp_timer.h"
#include "esp_chip_info.h"

/* MACRO */
#define TAG                                      "ble_log_rt"
#define BLE_LOG_RT_DEFER_TIMEOUT_US              (1000)

#if CONFIG_BT_CONTROLLER_ENABLED
#if CONFIG_IDF_TARGET_ESP32 || CONFIG_IDF_TARGET_ESP32C3 || CONFIG_IDF_TARGET_ESP32S3
extern const char *btdm_controller_get_compile_version(void);
#define BLE_LOG_CONTROLLER_GET_COMMIT() btdm_controller_get_compile_version()
#elif !CONFIG_BT_DUAL_MODE_ARCH || CONFIG_BT_CTRL_BLE_ENABLE
/* BR/EDR-only dual-mode builds do not link the BLE controller lib */
extern char *ble_controller_get_compile_version(void);
#define BLE_LOG_CONTROLLER_GET_COMMIT() ble_controller_get_compile_version()
#endif
#if CONFIG_BT_DUAL_MODE_ARCH
/* BTDM common lib (dual-mode arch only) */
extern const char *r_btdm_get_compile_version(void);
#define BLE_LOG_BTDM_COMMON_GET_COMMIT() r_btdm_get_compile_version()
#endif
#endif

#if CONFIG_BLE_MESH && CONFIG_BLE_MESH_V11_SUPPORT
/* "Bluetooth Mesh v1.1 commit: <hash>" */
extern const char bt_mesh_v11_commit_str[];
#endif

#if CONFIG_BT_AUDIO && CONFIG_SOC_BLE_AUDIO_SUPPORTED
extern const char *lib_audio_commit_get(void);
#endif

_Static_assert(sizeof(ble_log_version_info_t) == 58,
               "Unexpected BLE Log version info frame size");

/* VARIABLE */
BLE_LOG_STATIC BLE_LOG_DRAM_ATTR uint32_t rt_inited = 0;
BLE_LOG_STATIC BLE_LOG_DRAM_ATTR volatile uint32_t rt_ref_count = 0;
BLE_LOG_STATIC BLE_LOG_DRAM_ATTR QueueHandle_t rt_queue_handle = NULL;
BLE_LOG_STATIC BLE_LOG_DRAM_ATTR esp_timer_handle_t rt_defer_timer = NULL;
BLE_LOG_STATIC uint32_t rt_last_hook_os_ts = 0;
#if CONFIG_BLE_LOG_TS_ENABLED
BLE_LOG_STATIC BLE_LOG_DRAM_ATTR uint32_t rt_ts_enabled = 0;
BLE_LOG_STATIC esp_timer_handle_t rt_ts_timer = NULL;
#endif /* CONFIG_BLE_LOG_TS_ENABLED */

/* PRIVATE FUNCTION DECLARATION */
BLE_LOG_STATIC void ble_log_rt_defer_cb(void *arg);
BLE_LOG_STATIC bool ble_log_rt_dispatch(QueueHandle_t queue, UBaseType_t pending);
BLE_LOG_STATIC void ble_log_rt_run_hook(void);
#if CONFIG_BLE_LOG_TS_ENABLED
BLE_LOG_STATIC void ble_log_rt_ts_trigger(void *arg);
#endif /* CONFIG_BLE_LOG_TS_ENABLED */

/* PRIVATE FUNCTION */
/* Copies a NUL-terminated commit string into a fixed-width zero-padded field */
BLE_LOG_STATIC void ble_log_commit_copy(uint8_t *dst, const char *src, size_t len)
{
    BLE_LOG_MEMCPY(dst, src, strnlen(src, len));
}

BLE_LOG_STATIC void ble_log_rt_run_hook(void)
{
    uint32_t now = pdTICKS_TO_MS(xTaskGetTickCount());
    if ((uint32_t)(now - rt_last_hook_os_ts) < BLE_LOG_TS_TRIGGER_TIMEOUT_MS) {
        return;
    }
    rt_last_hook_os_ts = now;

    /* Write version info: BLE Log version, idf commit (build-time),
     * linked-in BLE lib commits, chip model/revision (efuse, runtime-only).
     * Libs absent from the build leave their fields zero. */
    ble_log_version_info_t version_info = {
        .int_src_code = BLE_LOG_INT_SRC_VERSION_INFO,
        .version = BLE_LOG_VERSION,
    };
#ifdef BLE_LOG_IDF_COMMIT
    BLE_LOG_MEMCPY(version_info.idf_commit, BLE_LOG_IDF_COMMIT, BLE_LOG_IDF_COMMIT_LEN);
#endif
#if CONFIG_BT_CONTROLLER_ENABLED && defined(BLE_LOG_CONTROLLER_GET_COMMIT)
    ble_log_commit_copy(version_info.controller_commit, BLE_LOG_CONTROLLER_GET_COMMIT(),
                        BLE_LOG_LIB_COMMIT_LEN);
#endif
#if CONFIG_BT_CONTROLLER_ENABLED && defined(BLE_LOG_BTDM_COMMON_GET_COMMIT)
    ble_log_commit_copy(version_info.btdm_common_commit, BLE_LOG_BTDM_COMMON_GET_COMMIT(),
                        BLE_LOG_LIB_COMMIT_LEN);
#endif
#if CONFIG_BLE_MESH && CONFIG_BLE_MESH_V11_SUPPORT
    /* The hash is the substring after the last space of the lib string */
    const char *mesh_commit = strrchr(bt_mesh_v11_commit_str, ' ');
    if (mesh_commit) {
        ble_log_commit_copy(version_info.mesh_commit, mesh_commit + 1,
                            BLE_LOG_LIB_COMMIT_LEN);
    }
#endif
#if CONFIG_BT_AUDIO && CONFIG_SOC_BLE_AUDIO_SUPPORTED
    ble_log_commit_copy(version_info.audio_commit, lib_audio_commit_get(),
                        BLE_LOG_LIB_COMMIT_LEN);
#endif
    esp_chip_info_t chip_info;
    esp_chip_info(&chip_info);
    version_info.chip_model = (uint16_t)chip_info.model;
    version_info.chip_revision = chip_info.revision;
    ble_log_write_hex(BLE_LOG_SRC_INTERNAL, (const uint8_t *)&version_info,
                      sizeof(version_info));

    ble_log_write_enh_stat();
    ble_log_write_buf_util();
}

BLE_LOG_STATIC bool ble_log_rt_dispatch(QueueHandle_t queue, UBaseType_t pending)
{
    ble_log_prph_trans_t *trans = NULL;
    bool processed = false;
    while (pending-- && xQueueReceive(queue, &trans, 0) == pdTRUE) {
        ble_log_prph_send_trans(trans);
        processed = true;
    }
    return processed;
}

BLE_LOG_STATIC void ble_log_rt_defer_cb(void *arg)
{
    (void)arg;

    if (!BLE_LOG_ATOMIC_LOAD_ACQUIRE(rt_inited)) {
        return;
    }

    QueueHandle_t queue = rt_queue_handle;
    if (!queue) {
        return;
    }

    UBaseType_t pending = uxQueueMessagesWaiting(queue);
    if (ble_log_rt_dispatch(queue, pending)) {
        ble_log_rt_run_hook();
    }

    pending = uxQueueMessagesWaiting(queue);
    if (pending &&
            ble_log_ref_count_try_acquire(&rt_ref_count, &rt_inited)) {
        (void)esp_timer_start_once(rt_defer_timer, BLE_LOG_RT_DEFER_TIMEOUT_US);
        BLE_LOG_REF_COUNT_RELEASE(&rt_ref_count);
    }
}

#if CONFIG_BLE_LOG_TS_ENABLED
BLE_LOG_STATIC void ble_log_rt_ts_trigger(void *arg)
{
    (void)arg;
    if (!BLE_LOG_ATOMIC_LOAD_ACQUIRE(rt_inited) ||
        !BLE_LOG_ATOMIC_LOAD_ACQUIRE(rt_ts_enabled)) {
        return;
    }

    ble_log_ts_info_t *ts_info = NULL;
    ble_log_ts_info_update(&ts_info);
    if (ts_info) {
        ble_log_write_hex(BLE_LOG_SRC_INTERNAL, (const uint8_t *)ts_info, sizeof(ble_log_ts_info_t));
    }
}
#endif /* CONFIG_BLE_LOG_TS_ENABLED */

/* INTERFACE */
bool ble_log_rt_init(void)
{
    if (BLE_LOG_ATOMIC_LOAD_ACQUIRE(rt_inited)) {
        return true;
    }

    rt_queue_handle = xQueueCreate(BLE_LOG_TRANS_TOTAL_CNT, sizeof(ble_log_prph_trans_t *));
    if (!rt_queue_handle) {
        goto exit;
    }

    esp_timer_create_args_t defer_timer_args = {
        .callback = ble_log_rt_defer_cb,
        .dispatch_method = ESP_TIMER_TASK,
        .name = "ble_log_rt",
        /* One-shot dispatch delay must remain a light-sleep wake source. */
        .skip_unhandled_events = false,
    };
    if (esp_timer_create(&defer_timer_args, &rt_defer_timer) != ESP_OK) {
        goto exit;
    }

#if CONFIG_BLE_LOG_TS_ENABLED
    BLE_LOG_ATOMIC_STORE_RELAXED(rt_ts_enabled, false);
    esp_timer_create_args_t ts_timer_args = {
        .callback = ble_log_rt_ts_trigger,
        .arg = NULL,
        .dispatch_method = ESP_TIMER_TASK,
        .name = "ble_log_ts_timer",
        /* Do not wake light sleep or replay every missed periodic callback. */
        .skip_unhandled_events = true,
    };
    if (esp_timer_create(&ts_timer_args, &rt_ts_timer) != ESP_OK ||
        esp_timer_start_periodic(rt_ts_timer, BLE_LOG_TS_TRIGGER_TIMEOUT_US) != ESP_OK) {
        goto exit;
    }
#endif /* CONFIG_BLE_LOG_TS_ENABLED */

    rt_last_hook_os_ts = 0;
    BLE_LOG_ATOMIC_STORE_RELEASE(rt_inited, true);
    return true;

exit:
    ble_log_rt_deinit();
    return false;
}

void ble_log_rt_deinit(void)
{
    /* Closing gate: seq_cst on both sides (see also submit/drain) so a
     * submitter either sees rt_inited == false and bails, or its reference
     * is visible to the ref-count wait before the handles are deleted. */
    BLE_LOG_ATOMIC_STORE_SEQ_CST(rt_inited, false);
    while (!ble_log_ref_count_wait(&rt_ref_count, 0)) {
        ESP_LOGE(TAG, "Timed out waiting for BLE Log runtime references");
        BLE_LOG_ASSERT(false);
    }
#if CONFIG_BLE_LOG_TS_ENABLED
    BLE_LOG_ATOMIC_STORE_RELEASE(rt_ts_enabled, false);
    if (rt_ts_timer) {
        esp_timer_stop_blocking(rt_ts_timer, portMAX_DELAY);
        esp_timer_delete(rt_ts_timer);
        rt_ts_timer = NULL;
    }
#endif /* CONFIG_BLE_LOG_TS_ENABLED */

    if (rt_defer_timer) {
        esp_timer_stop_blocking(rt_defer_timer, portMAX_DELAY);
        esp_timer_delete(rt_defer_timer);
        rt_defer_timer = NULL;
    }

    if (rt_queue_handle) {
        ble_log_prph_trans_t *trans = NULL;
        while (xQueueReceive(rt_queue_handle, &trans, 0) == pdTRUE) {
            trans->pos = 0;
            ble_log_lbm_recycle_trans(trans);
        }
        vQueueDelete(rt_queue_handle);
        rt_queue_handle = NULL;
    }
}

bool ble_log_rt_drain(void)
{
    bool drained = false;
    if (!ble_log_ref_count_try_acquire(&rt_ref_count, &rt_inited)) {
        return false;
    }
    if (!rt_defer_timer || !rt_queue_handle) {
        goto exit;
    }

    if (esp_timer_stop_blocking(rt_defer_timer, portMAX_DELAY) != ESP_OK) {
        goto exit;
    }
    QueueHandle_t queue = rt_queue_handle;
    (void)ble_log_rt_dispatch(queue, uxQueueMessagesWaiting(queue));
    drained = true;

exit:
    BLE_LOG_REF_COUNT_RELEASE(&rt_ref_count);
    return drained;
}

BLE_LOG_IRAM_ATTR void ble_log_rt_submit_trans(ble_log_prph_trans_t *trans)
{
    if (!ble_log_ref_count_try_acquire(&rt_ref_count, &rt_inited)) {
        ble_log_lbm_recycle_trans(trans);
        return;
    }
    if (!rt_queue_handle) {
        BLE_LOG_REF_COUNT_RELEASE(&rt_ref_count);
        ble_log_lbm_recycle_trans(trans);
        return;
    }

    bool in_isr = BLE_LOG_IN_ISR();
    BaseType_t queued = in_isr
                        ? xQueueSendFromISR(rt_queue_handle, &trans, NULL)
                        : xQueueSend(rt_queue_handle, &trans, 0);
    if (queued != pdTRUE) {
        BLE_LOG_REF_COUNT_RELEASE(&rt_ref_count);
        ble_log_lbm_recycle_trans(trans);
        return;
    }

    /* An active timer keeps the deadline anchored to the first submission. */
    (void)esp_timer_start_once(rt_defer_timer, BLE_LOG_RT_DEFER_TIMEOUT_US);
    BLE_LOG_REF_COUNT_RELEASE(&rt_ref_count);
}

#if CONFIG_BLE_LOG_TS_ENABLED
bool ble_log_sync_enable(bool enable)
{
    if (!BLE_LOG_ATOMIC_LOAD_ACQUIRE(rt_inited)) {
        return false;
    }
    BLE_LOG_ATOMIC_STORE_RELEASE(rt_ts_enabled, enable);
    ble_log_ts_reset(enable);
    return true;
}
#endif /* CONFIG_BLE_LOG_TS_ENABLED */
