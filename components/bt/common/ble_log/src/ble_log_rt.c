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
#include "ble_log_task_registry.h"
#include "ble_log_util.h"

#include "esp_log.h"
#include "esp_timer.h"
#include "esp_task.h" /* ESP_TASK_TIMER_PRIO for the USB dispatch task */
#if CONFIG_BLE_LOG_LL_ENABLED
#include "esp_bt.h"
#endif
#include "driver/gpio.h"

/* MACRO */
#define TAG                                      "BLE-Log"
#define BLE_LOG_RT_DEFER_TIMEOUT_US              (1000)
/* Bounded wait for the USB dispatcher stop handshake (ack plus suspended
 * state), matching the runtime reference-count wait budget. */
#define BLE_LOG_RT_STOP_TIMEOUT_MS               (1000)

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
#if CONFIG_BLE_LOG_PRPH_USB
/* USB builds drain the queue from a dedicated task: the 1 ms defer
 * cadence caps throughput at pool-depth x transport-size per millisecond
 * (~5 MiB/s), far below the USB bulk exit. Same dispatch function, same
 * single-dispatcher invariant; the task blocks on the queue so idle cost
 * is zero. SPI/UART builds keep the deferred esp_timer batch below. */
BLE_LOG_STATIC BLE_LOG_DRAM_ATTR TaskHandle_t rt_task_handle = NULL;
/* Teardown completion ack: the task publishes it after it has finished the
 * transport it owned and drained what the closed gate left behind. The ref
 * count above cannot cover a dequeued transport. */
BLE_LOG_STATIC BLE_LOG_DRAM_ATTR uint32_t rt_task_stopped = 0;
#else
BLE_LOG_STATIC BLE_LOG_DRAM_ATTR esp_timer_handle_t rt_defer_timer = NULL;
#endif
BLE_LOG_STATIC BLE_LOG_DRAM_ATTR esp_timer_handle_t rt_ts_timer = NULL;
/* Toggle IO phase; stays false when the toggle IO is compiled out. */
BLE_LOG_STATIC BLE_LOG_DRAM_ATTR bool rt_ts_io_level = false;
#if CONFIG_BLE_LOG_TS_SYNC_TOGGLE_IO_ENABLED
/* Runtime IO control is independent from the always-on periodic snapshot. */
BLE_LOG_STATIC BLE_LOG_DRAM_ATTR bool rt_ts_io_toggle_enabled = false;
#endif

/* PRIVATE FUNCTION DECLARATION */
#if !CONFIG_BLE_LOG_PRPH_USB
BLE_LOG_STATIC void ble_log_rt_defer_cb(void *arg);
BLE_LOG_STATIC void ble_log_rt_dispatch(QueueHandle_t queue,
                                        UBaseType_t pending);
#else
BLE_LOG_STATIC void ble_log_rt_task(void *pvParameters);
#endif
BLE_LOG_STATIC void ble_log_rt_ts_trigger(void *arg);
BLE_LOG_STATIC void ble_log_rt_report_loss(void);

/* PRIVATE FUNCTION */
/* One loss-warning line per window (plus one CDC FIFO line on USB
 * builds), non-zero windows only. The first line reports frames the
 * runtime could not write into a BLE Log buffer (pool exhausted or
 * claimed from a non-yieldable context); the CDC line reports sealed
 * transports the USB FIFO could not accept. Runs on the
 * shared ESP Timer task: a single short line, ~100 bytes at 115200 baud
 * (~9 ms). If the measured worst-case cost ever exceeds the timer-task
 * budget, move formatting to a low-priority diagnostic task instead of
 * shortening the producer wait (spec constraint). */
BLE_LOG_STATIC void ble_log_rt_report_loss(void)
{
    uint32_t by_source[BLE_LOG_SRC_MAX];
    ble_log_lbm_take_loss_window(by_source);

    uint32_t total = 0;
    for (int i = 0; i < BLE_LOG_SRC_MAX; i++) {
        total += by_source[i];
    }
#if CONFIG_BLE_LOG_PRPH_USB
    uint32_t cdc_drops = 0;
    uint32_t cdc_dropped_bytes = 0;
    ble_log_prph_take_cdc_fifo_drops(&cdc_drops, &cdc_dropped_bytes);
#else
    const uint32_t cdc_drops = 0;
    const uint32_t cdc_dropped_bytes = 0;
#endif
    if (total == 0 && cdc_drops == 0 && cdc_dropped_bytes == 0) {
        return;
    }

    /* Fixed scratch, assembled field by field; no dynamic allocation on
     * the timer task. Only non-zero sources appear, one src-<code>=<count>
     * entry each (source code as the plain numeric wire ID). */
    if (total != 0) {
        char line[160];
        int pos = snprintf(line, sizeof(line), "Lost %lu frames",
                           (unsigned long)total);
        for (int i = 0;
             i < BLE_LOG_SRC_MAX && pos > 0 && (size_t)pos < sizeof(line) - 1;
             i++) {
            if (by_source[i] == 0) {
                continue;
            }
            int written = snprintf(line + pos, sizeof(line) - pos,
                                   ", src-%u=%lu",
                                   i, (unsigned long)by_source[i]);
            if (written < 0) {
                break;
            }
            pos += written;
        }
        ESP_LOGW(TAG, "%s", line);
    }
    /* Byte-only guard: the two CDC counters are taken independently, so a
     * producer can land a byte increment after the reporter took the count
     * - that window then holds bytes with no transports and must still
     * print, or the increment is silently cleared by the take above. */
    if (cdc_drops != 0 || cdc_dropped_bytes != 0) {
        ESP_LOGW(TAG,
                 "Lost %lu transport(s), %lu byte(s): CDC TX FIFO full "
                 "after USB output (frames already sealed, not written)",
                 (unsigned long)cdc_drops,
                 (unsigned long)cdc_dropped_bytes);
    }
}
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

#if !CONFIG_BLE_LOG_PRPH_USB
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
#endif /* !CONFIG_BLE_LOG_PRPH_USB */

#if CONFIG_BLE_LOG_PRPH_USB
#if CONFIG_BLE_LOG_USB_DISPATCH_TEST_HOOKS
/* Test-only injection point, defined by the USB test app: pauses the
 * dispatcher between dequeue and send so the teardown handshake can be
 * checked deterministically. */
extern void ble_log_test_usb_dispatch_pre_send_hook(void) __attribute__((weak));
/* Test-only injection point, defined by the USB test app: pauses the
 * dispatcher between the stop ack and its own suspend, the window in which
 * deinit must keep waiting. */
extern void ble_log_test_usb_dispatch_post_ack_hook(void) __attribute__((weak));
#endif /* CONFIG_BLE_LOG_USB_DISPATCH_TEST_HOOKS */

/* Dedicated dispatch task for USB builds: event-driven queue drain. The
 * blocking receive is mandatory for light-sleep support (the queue read is
 * the wake source).
 *
 * The task holds no stop state of its own: deinit closes the gate and posts
 * the stop token (see ble_log_rt_stop_task), which the receive below reads as
 * "stop". It then recycles whatever the closed gate left in the queue and
 * publishes rt_task_stopped, so that ack covers both the transport it owned
 * and the ones still queued. It suspends instead of deleting itself: deinit
 * keeps waiting for the ack and the suspended state, and a task handle must
 * not outlive the TCB the idle task frees. */
BLE_LOG_STATIC void ble_log_rt_task(void *pvParameters)
{
    (void)pvParameters;
    ble_log_prph_trans_t *trans = NULL;
    while (xQueueReceive(rt_queue_handle, &trans, portMAX_DELAY) == pdTRUE) {
        if (trans == NULL) {
            /* The stop token deinit posted. Must precede the gate check: the
             * recycle below dereferences the transport. */
            ESP_LOGI(TAG,
                     "Stop token: draining %u queued transport(s), then acking",
                     (unsigned)uxQueueMessagesWaiting(rt_queue_handle));
            break;
        }
        if (!BLE_LOG_ATOMIC_LOAD_ACQUIRE(rt_inited)) {
            /* Deinit is tearing down; recycle and keep draining. */
            ble_log_lbm_recycle_trans(trans);
            continue;
        }
#if CONFIG_BLE_LOG_USB_DISPATCH_TEST_HOOKS
        if (ble_log_test_usb_dispatch_pre_send_hook) {
            ble_log_test_usb_dispatch_pre_send_hook();
        }
#endif /* CONFIG_BLE_LOG_USB_DISPATCH_TEST_HOOKS */
        ble_log_prph_send_trans(trans);
    }

    /* Reached on the stop token with the gate already closed, so nothing here
     * was submitted after the gate: drain and ack. */
    while (xQueueReceive(rt_queue_handle, &trans, 0) == pdTRUE) {
        ble_log_lbm_recycle_trans(trans);
    }
    BLE_LOG_ATOMIC_STORE_RELEASE(rt_task_stopped, 1);
#if CONFIG_BLE_LOG_USB_DISPATCH_TEST_HOOKS
    if (ble_log_test_usb_dispatch_post_ack_hook) {
        ble_log_test_usb_dispatch_post_ack_hook();
    }
#endif /* CONFIG_BLE_LOG_USB_DISPATCH_TEST_HOOKS */
    vTaskSuspend(NULL);
}

/* Stop handshake state: the ack says the dispatch work is done, the suspended
 * state says the task is out of the scheduler's way. Deinit needs both before
 * it deletes the handle (see ble_log_rt_stop_task). */
BLE_LOG_STATIC bool ble_log_rt_stopped(void)
{
    return BLE_LOG_ATOMIC_LOAD_ACQUIRE(rt_task_stopped) &&
           (eTaskGetState(rt_task_handle) == eSuspended);
}

/* Wake the dispatcher and wait until it is stopped.
 *
 * Both conditions are needed before the handle is safe to delete, and they are
 * not the same instant: the ack only says the task finished the transport it
 * owned and the queue; it then still has to reach the suspended state. On SMP
 * deinit can win the kernel lock between the ack and the task's own
 * vTaskSuspend(), which puts the task on the termination list - and the task's
 * vTaskSuspend() then moves its state list item from that list to the
 * suspended list (tasks.c), leaving the TCB unfreed and the kernel's
 * deleted-task count stuck above zero. Nothing resumes a suspended task, so
 * the state is stable once observed.
 *
 * The wake is a NULL token in the runtime queue, not an abort poke: the
 * dispatcher can be blocked inside its dispatch call, where xTaskAbortDelay
 * would also cancel a wait on one of TinyUSB's own mutexes. TinyUSB takes
 * those with a forever timeout and ignores the result, so a cancelled take
 * leaves it giving back a mutex it never acquired. The token can only be
 * taken by the receive at the top of the loop. The whole wait shares the
 * timeout the reference count wait uses. */
BLE_LOG_STATIC bool ble_log_rt_stop_task(void)
{
    /* The reserved queue slot makes this unconditional: at most one entry per
     * transport can be queued at once, and the gate is closed and the
     * reference count drained before this runs. */
    ble_log_prph_trans_t *stop_token = NULL;
    if (xQueueSend(rt_queue_handle, &stop_token, 0) != pdTRUE) {
        return false;
    }

    TickType_t start_tick = xTaskGetTickCount();
    while (!ble_log_rt_stopped()) {
        if ((xTaskGetTickCount() - start_tick) >=
                pdMS_TO_TICKS(BLE_LOG_RT_STOP_TIMEOUT_MS)) {
            return false;
        }
        vTaskDelay(1);
    }
    return true;
}

#else
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
#endif /* CONFIG_BLE_LOG_PRPH_USB */

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
     * wait for the next capacity seal. The task-id binding broadcast
     * rides the same window, ahead of the snapshot, on its own dedicated
     * transport: a receiver that missed a binding converges on the next
     * one, and the snapshot never waits behind it. */
    ble_log_lbm_flush_open_trans();
    ble_log_task_bindings_publish();

    (void)ble_log_internal_snapshot(
        BLE_LOG_SNAPSHOT_REASON_PERIODIC |
        BLE_LOG_SNAPSHOT_REASON_TS_VALID,
        &ts_info, false);

    ble_log_rt_report_loss();
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

    /* One slot per transport, so every transport the peripheral can hold fits
     * in the queue at once and submit never blocks or drops, plus one reserved
     * slot for the NULL stop token deinit posts (the only non-transport entry
     * this queue ever carries). */
    rt_queue_handle = xQueueCreate(BLE_LOG_TRANS_TOTAL_CNT + 1,
                                   sizeof(ble_log_prph_trans_t *));
    if (!rt_queue_handle) {
        goto exit;
    }

#if CONFIG_BLE_LOG_PRPH_USB
    /* Queue must be initialized before creating the task. The priority
     * matches the esp_timer task so dispatch keeps the scheduling position
     * of the deferred-callback design it replaces. */
    BLE_LOG_ATOMIC_STORE_RELEASE(rt_task_stopped, 0);
    if (xTaskCreate(ble_log_rt_task, "ble_log_rt",
                    CONFIG_BLE_LOG_RT_TASK_STACK_SIZE, NULL,
                    ESP_TASK_TIMER_PRIO, &rt_task_handle) != pdTRUE) {
        goto exit;
    }
#else
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
#endif /* CONFIG_BLE_LOG_PRPH_USB */

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
#if CONFIG_BLE_LOG_PRPH_USB
    /* Dispatcher teardown first: it must be quiescent before the tail report
     * below samples the CDC FIFO-drop counters it updates, and before the
     * pool and the peripheral go away.
     *
     * Quiescence comes from the gate plus an ack, not from vTaskDelete: the
     * task sleeps in xQueueReceive, so closing the gate is not enough on its
     * own - deinit closes it and then posts the stop token, and the task
     * recycles what the closed gate left queued before it publishes
     * rt_task_stopped.
     * The ack therefore covers the transport it owned (sent or recycled) and
     * the queued ones, which is what "no dispatch call in flight" means.
     * Deleting the task on its own cannot give that: it interrupts a send or
     * recycle without waiting for the call to return. */
    if (rt_task_handle) {
        if (!ble_log_rt_stop_task()) {
            ESP_LOGE(TAG, "Timed out stopping the BLE Log dispatch task");
            BLE_LOG_ASSERT(false);
        }
        /* Acked and suspended: no dispatch call is in flight, and the task
         * cannot race this delete with its own vTaskSuspend. */
        vTaskDelete(rt_task_handle);
        rt_task_handle = NULL;
    }
#else
    if (rt_defer_timer) {
        esp_timer_stop_blocking(rt_defer_timer, portMAX_DELAY);
        esp_timer_delete(rt_defer_timer);
        rt_defer_timer = NULL;
    }
#endif /* CONFIG_BLE_LOG_PRPH_USB */

    if (rt_ts_timer) {
        esp_timer_stop_blocking(rt_ts_timer, portMAX_DELAY);
        /* Tail window: the callback has exited, so the shadow state is
         * exclusively ours. Report losses since the last periodic window
         * before the runtime queue and peripherals go away. The shadow was
         * zeroed at init, so an early deinit reports exactly what was lost
         * since then. */
        ble_log_rt_report_loss();
        esp_timer_delete(rt_ts_timer);
        rt_ts_timer = NULL;
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
#if CONFIG_BLE_LOG_PRPH_USB
    if (!rt_task_handle || !rt_queue_handle) {
        goto exit;
    }

    /* The dispatch task consumes the queue as long as work arrives; this
     * waits until it has taken every submitted transport. Dequeued is not
     * recycled, though: a transport the dispatcher already took is out of the
     * queue but still in its hands. The teardown path therefore additionally
     * waits for the dispatcher ack in ble_log_rt_deinit(); here the queue
     * check only has to bound how long a flush waits. */
    QueueHandle_t queue = rt_queue_handle;
    while (uxQueueMessagesWaiting(queue)) {
        /* One tick, not pdMS_TO_TICKS(1): at the default 100 Hz tick rate the
         * latter converts to 0 and vTaskDelay(0) only yields, which spins a
         * higher-priority caller instead of letting the dispatcher run. */
        vTaskDelay(1);
    }
    drained = true;
#else
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
#endif /* CONFIG_BLE_LOG_PRPH_USB */

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

#if CONFIG_BLE_LOG_PRPH_USB
    /* The dispatch task blocks on the queue; the send above is the wake. */
#else
    /* An active timer keeps the deadline anchored to the first submission. */
    (void)esp_timer_start_once(rt_defer_timer, BLE_LOG_RT_DEFER_TIMEOUT_US);
#endif
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
