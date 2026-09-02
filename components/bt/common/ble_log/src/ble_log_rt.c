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
#include "ble_log_lbm_v2.h"
#include "ble_log_util.h"

#include "esp_log.h"
#include "esp_timer.h"
#if CONFIG_BLE_LOG_LL_ENABLED
#include "esp_bt.h"
#endif
#include "driver/gpio.h"

/* MACRO */
#define TAG                                      "ble_log_rt"
#define BLE_LOG_RT_DEFER_TIMEOUT_US              (1000)

/* Link-layer clock sample; 0 when the controller exports no accessor. */
#if CONFIG_BLE_LOG_LL_ENABLED
#if CONFIG_BT_DUAL_MODE_ARCH
/* Temporary fallback until the dual-mode controller libraries export the
 * link-layer timer accessor. A strong library definition overrides it. */
uint32_t r_sched_timer_getCurrentTimeU32(void) __attribute__((weak));
uint32_t r_sched_timer_getCurrentTimeU32(void)
{
    return 0;
}
#define BLE_LOG_GET_LC_TS r_sched_timer_getCurrentTimeU32()
/* ESP BLE Controller Gen 2 */
#elif defined(CONFIG_IDF_TARGET_ESP32H2) || defined(CONFIG_IDF_TARGET_ESP32C6) || defined(CONFIG_IDF_TARGET_ESP32C5) ||\
    defined(CONFIG_IDF_TARGET_ESP32C61) || defined(CONFIG_IDF_TARGET_ESP32H21)
extern uint32_t r_ble_lll_timer_current_tick_get(void);
#define BLE_LOG_GET_LC_TS r_ble_lll_timer_current_tick_get()
/* ESP BLE Controller Gen 1 */
#elif defined(CONFIG_IDF_TARGET_ESP32C2)
extern uint32_t r_os_cputime_get32(void);
#define BLE_LOG_GET_LC_TS r_os_cputime_get32()
/* Legacy BLE Controller */
#elif defined(CONFIG_IDF_TARGET_ESP32C3) || defined(CONFIG_IDF_TARGET_ESP32S3)
extern uint32_t lld_read_clock_us(void);
#define BLE_LOG_GET_LC_TS lld_read_clock_us()
#else /* Other targets */
#define BLE_LOG_GET_LC_TS 0
#endif /* BLE targets */
#else /* !CONFIG_BLE_LOG_LL_ENABLED */
#define BLE_LOG_GET_LC_TS 0
#endif /* CONFIG_BLE_LOG_LL_ENABLED */

BLE_LOG_STATIC uint32_t ble_log_rt_lc_ts_get(void)
{
#if CONFIG_BLE_LOG_LL_ENABLED
    /* Legacy accessors dereference controller state. INIT is emitted before
     * controller initialization completes, and standalone users may keep the
     * controller idle for the entire BLE Log epoch. */
    if (esp_bt_controller_get_status() == ESP_BT_CONTROLLER_STATUS_IDLE) {
        return 0;
    }
#endif
    return BLE_LOG_GET_LC_TS;
}

/* VARIABLE */
BLE_LOG_STATIC BLE_LOG_DRAM_ATTR uint32_t rt_inited = 0;
BLE_LOG_STATIC BLE_LOG_DRAM_ATTR volatile uint32_t rt_ref_count = 0;
BLE_LOG_STATIC BLE_LOG_DRAM_ATTR QueueHandle_t rt_queue_handle = NULL;
BLE_LOG_STATIC BLE_LOG_DRAM_ATTR esp_timer_handle_t rt_defer_timer = NULL;
BLE_LOG_STATIC BLE_LOG_DRAM_ATTR esp_timer_handle_t rt_ts_timer = NULL;
/* Toggle IO phase; stays false when the toggle IO is compiled out. */
BLE_LOG_STATIC BLE_LOG_DRAM_ATTR bool rt_ts_io_level = false;
#if CONFIG_BLE_LOG_TS_SYNC_TOGGLE_IO_ENABLED
/* Runtime IO control is independent from the always-on periodic snapshot. */
BLE_LOG_STATIC BLE_LOG_DRAM_ATTR bool rt_ts_io_toggle_enabled = false;
#endif

/* PRIVATE FUNCTION DECLARATION */
BLE_LOG_STATIC void ble_log_rt_defer_cb(void *arg);
BLE_LOG_STATIC void ble_log_rt_dispatch(QueueHandle_t queue,
                                        UBaseType_t pending);
BLE_LOG_STATIC void ble_log_rt_ts_trigger(void *arg);

/* PRIVATE FUNCTION */
/* Captures the link-layer, ESP and OS clocks at one instant. */
void ble_log_rt_ts_sample(ble_log_ts_info_t *info, bool toggle_io)
{
    info->int_src_code = BLE_LOG_INT_SRC_TS;
    BLE_LOG_ENTER_CRITICAL();
#if CONFIG_BLE_LOG_TS_SYNC_TOGGLE_IO_ENABLED
    /* The critical section keeps the optional IO edge and clock samples
     * adjacent, and serializes runtime IO control with the phase update. */
    if (toggle_io && rt_ts_io_toggle_enabled) {
        rt_ts_io_level = !rt_ts_io_level;
        gpio_set_level(CONFIG_BLE_LOG_SYNC_IO_NUM, rt_ts_io_level);
    }
#endif /* CONFIG_BLE_LOG_TS_SYNC_TOGGLE_IO_ENABLED */
    info->io_level = rt_ts_io_level;
    info->lc_ts = ble_log_rt_lc_ts_get();
    info->esp_ts = esp_timer_get_time();
    info->os_ts = pdTICKS_TO_MS(xTaskGetTickCountFromISR());
    BLE_LOG_EXIT_CRITICAL();
}

/* Dispatch only the queue depth observed at callback entry. A backend may
 * recycle synchronously on queue-full while another core immediately refills
 * the runtime queue; an unbounded loop here could starve every other callback
 * on the shared ESP timer task. */
BLE_LOG_STATIC void ble_log_rt_dispatch(QueueHandle_t queue,
                                        UBaseType_t pending)
{
    ble_log_prph_trans_t *trans = NULL;
    while (pending-- && xQueueReceive(queue, &trans, 0) == pdTRUE) {
        ble_log_prph_send_trans(trans);
    }
}

BLE_LOG_STATIC void ble_log_rt_defer_cb(void *arg)
{
    (void)arg;

    /* Deinit clears the queue only after stop_blocking has waited for this
     * callback, so a live gate implies a live queue. */
    if (!BLE_LOG_ATOMIC_LOAD_ACQUIRE(rt_inited)) {
        return;
    }

    QueueHandle_t queue = rt_queue_handle;
    ble_log_rt_dispatch(queue, uxQueueMessagesWaiting(queue));

    /* A submit racing an active one-shot callback may fail to arm it. If the
     * bounded batch left work behind, schedule another turn after yielding
     * the shared timer task to callbacks that are already due. */
    if (uxQueueMessagesWaiting(queue) &&
            ble_log_ref_count_try_acquire(&rt_ref_count, &rt_inited)) {
        (void)esp_timer_start_once(rt_defer_timer,
                                   BLE_LOG_RT_DEFER_TIMEOUT_US);
        BLE_LOG_REF_COUNT_RELEASE(&rt_ref_count);
    }
}

BLE_LOG_STATIC void ble_log_rt_ts_trigger(void *arg)
{
    (void)arg;
    if (!BLE_LOG_ATOMIC_LOAD_ACQUIRE(rt_inited)) {
        return;
    }

    ble_log_ts_info_t ts_info;
    ble_log_rt_ts_sample(&ts_info, true);

    /* Unified periodic output: best-effort flush of partially-filled OPEN
     * transports ahead of the periodic snapshot, so parked frames do not
     * wait for the next capacity seal. */
    ble_log_lbm_flush_open_trans();

    (void)ble_log_internal_snapshot(
        BLE_LOG_SNAPSHOT_REASON_PERIODIC |
        BLE_LOG_SNAPSHOT_REASON_TS_VALID,
        &ts_info, false);
}

/* INTERFACE */
bool ble_log_rt_init(void)
{
    if (BLE_LOG_ATOMIC_LOAD_ACQUIRE(rt_inited)) {
        return true;
    }

    /* Configure the analyzer toggle IO */
#if CONFIG_BLE_LOG_TS_SYNC_TOGGLE_IO_ENABLED
    gpio_config_t sync_io_conf = {
        .intr_type = GPIO_INTR_DISABLE,
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = BIT64(CONFIG_BLE_LOG_SYNC_IO_NUM),
    };
    /* Program the output latch before enabling the driver to avoid a stale
     * high level or a high glitch after deinit/reinit. */
    if (gpio_set_level(CONFIG_BLE_LOG_SYNC_IO_NUM, 0) != ESP_OK ||
            gpio_config(&sync_io_conf) != ESP_OK) {
        goto exit;
    }
#endif /* CONFIG_BLE_LOG_TS_SYNC_TOGGLE_IO_ENABLED */

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

    /* Create the system-periodic path here; the top-level init starts it only
     * after queuing INIT, then it runs through deinit. Runtime sync control
     * affects only the optional analyzer IO toggle. */
    rt_ts_io_level = false;
#if CONFIG_BLE_LOG_TS_SYNC_TOGGLE_IO_ENABLED
    rt_ts_io_toggle_enabled = false;
#endif
    esp_timer_create_args_t ts_timer_args = {
        .callback = ble_log_rt_ts_trigger,
        .arg = NULL,
        .dispatch_method = ESP_TIMER_TASK,
        .name = "ble_log_ts_timer",
        /* Do not wake light sleep or replay every missed periodic callback. */
        .skip_unhandled_events = true,
    };
    if (esp_timer_create(&ts_timer_args, &rt_ts_timer) != ESP_OK) {
        goto exit;
    }

    BLE_LOG_ATOMIC_STORE_RELEASE(rt_inited, true);
    return true;

exit:
    ble_log_rt_deinit();
    return false;
}

bool ble_log_rt_start_periodic(void)
{
    bool started = false;
    if (!ble_log_ref_count_try_acquire(&rt_ref_count, &rt_inited)) {
        return false;
    }
    if (rt_ts_timer &&
            esp_timer_start_periodic(rt_ts_timer,
                                     BLE_LOG_TS_TRIGGER_TIMEOUT_US) == ESP_OK) {
        started = true;
    }
    BLE_LOG_REF_COUNT_RELEASE(&rt_ref_count);
    return started;
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
    if (rt_ts_timer) {
        esp_timer_stop_blocking(rt_ts_timer, portMAX_DELAY);
        esp_timer_delete(rt_ts_timer);
        rt_ts_timer = NULL;
    }

    if (rt_defer_timer) {
        esp_timer_stop_blocking(rt_defer_timer, portMAX_DELAY);
        esp_timer_delete(rt_defer_timer);
        rt_defer_timer = NULL;
    }

    if (rt_queue_handle) {
        ble_log_prph_trans_t *trans = NULL;
        while (xQueueReceive(rt_queue_handle, &trans, 0) == pdTRUE) {
            ble_log_lbm_recycle_trans(trans);
        }
        vQueueDelete(rt_queue_handle);
        rt_queue_handle = NULL;
    }

    /* Release the toggle IO */
    rt_ts_io_level = false;
#if CONFIG_BLE_LOG_TS_SYNC_TOGGLE_IO_ENABLED
    rt_ts_io_toggle_enabled = false;
    gpio_reset_pin(CONFIG_BLE_LOG_SYNC_IO_NUM);
#endif /* CONFIG_BLE_LOG_TS_SYNC_TOGGLE_IO_ENABLED */
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
    while (uxQueueMessagesWaiting(queue)) {
        ble_log_rt_dispatch(queue, uxQueueMessagesWaiting(queue));
    }
    drained = true;

exit:
    BLE_LOG_REF_COUNT_RELEASE(&rt_ref_count);
    return drained;
}

BLE_LOG_IRAM_ATTR void ble_log_rt_submit_trans(ble_log_prph_trans_t *trans)
{
    if (!ble_log_ref_count_try_acquire(&rt_ref_count, &rt_inited)) {
        /* No reference was taken, so this path must not release; it stays
         * separate from the held-reference failures below. */
        ble_log_lbm_recycle_trans(trans);
        return;
    }
    if (!rt_queue_handle) {
        goto fail;
    }

    bool in_isr = BLE_LOG_IN_ISR();
    BaseType_t queued = in_isr
                        ? xQueueSendFromISR(rt_queue_handle, &trans, NULL)
                        : xQueueSend(rt_queue_handle, &trans, 0);
    if (queued != pdTRUE) {
        goto fail;
    }

    /* An active timer keeps the deadline anchored to the first submission. */
    (void)esp_timer_start_once(rt_defer_timer, BLE_LOG_RT_DEFER_TIMEOUT_US);
    BLE_LOG_REF_COUNT_RELEASE(&rt_ref_count);
    return;

fail:
    BLE_LOG_REF_COUNT_RELEASE(&rt_ref_count);
    ble_log_lbm_recycle_trans(trans);
}

bool ble_log_ts_sync_io_toggle_enable(bool enable)
{
    if (!ble_log_ref_count_try_acquire(&rt_ref_count, &rt_inited)) {
        return false;
    }

#if CONFIG_BLE_LOG_TS_SYNC_TOGGLE_IO_ENABLED
    /* Restart the analyzer phase from low. When disabling an already-low IO,
     * drive it high first so the external analyzer still sees a final
     * falling edge. The periodic snapshot remains active in either state. */
    BLE_LOG_ENTER_CRITICAL();
    rt_ts_io_toggle_enabled = enable;
    if (!enable && !rt_ts_io_level) {
        gpio_set_level(CONFIG_BLE_LOG_SYNC_IO_NUM, 1);
    }
    rt_ts_io_level = false;
    gpio_set_level(CONFIG_BLE_LOG_SYNC_IO_NUM, 0);
    BLE_LOG_EXIT_CRITICAL();
#else
    (void)enable;
#endif /* CONFIG_BLE_LOG_TS_SYNC_TOGGLE_IO_ENABLED */

    BLE_LOG_REF_COUNT_RELEASE(&rt_ref_count);
    return true;
}

bool ble_log_sync_enable(bool enable)
{
    return ble_log_ts_sync_io_toggle_enable(enable);
}
