/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>
#include <assert.h>

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include "esp_timer.h"

#include <../host/conn_internal.h>

#include "common/host.h"
#include "common/iso.h"
#include "common/gatt.h"
#include "common/app/gap.h"
#include "common/app/gatt.h"

LOG_MODULE_REGISTER(ISO_TASK, CONFIG_BT_ISO_LOG_LEVEL);

/* Nothing to poll for - iso_ctrl_queue wakes the task for deinit. The dispatch
 * monitor is the exception: its periodic dump is driven from this loop, so it
 * needs a wakeup even while no event arrives. */
#if CONFIG_BT_ISO_DISPATCH_MONITOR
#define ISO_TASK_WAIT           (ISO_STATS_DUMP_PERIOD_US / 1000 / portTICK_PERIOD_MS)
#else /* CONFIG_BT_ISO_DISPATCH_MONITOR */
#define ISO_TASK_WAIT           portMAX_DELAY
#endif /* CONFIG_BT_ISO_DISPATCH_MONITOR */

/* Generous: expiry means a dispatch handler is wedged, which is a bug
 * elsewhere. Deinit reports it upward rather than freeing under a live task. */
#define ISO_TASK_STOP_TIMEOUT   (2000 / portTICK_PERIOD_MS)

/* Three priority tiers share one task via a queue set. The task drains
 * critical before normal before floodable, so a flood of GAP reports cannot
 * delay the latency-critical ISO data path. See common/task.h for the mapping.
 */
static BT_ISO_CTRL_BSS_ATTR QueueHandle_t iso_critical_queue;
static BT_ISO_CTRL_BSS_ATTR QueueHandle_t iso_normal_queue;
static BT_ISO_CTRL_BSS_ATTR QueueHandle_t iso_floodable_queue;
/* Not a tier: deinit-only wakeup, see ISO_CTRL_QUEUE_LEN. */
static BT_ISO_CTRL_BSS_ATTR QueueHandle_t iso_ctrl_queue;
static BT_ISO_CTRL_BSS_ATTR QueueSetHandle_t iso_queue_set;

static BT_ISO_CTRL_BSS_ATTR TaskHandle_t iso_task_handle;

/* Gate + handshake for deinit. iso_task_stopping also rejects new posts, so a
 * producer cannot strand a payload on a queue nobody will drain. */
static BT_ISO_CTRL_BSS_ATTR volatile bool iso_task_stopping;
static BT_ISO_CTRL_BSS_ATTR SemaphoreHandle_t iso_task_stopped;

extern void bt_le_timer_handle_event(void *arg, size_t gen);

#if CONFIG_BT_OTS || CONFIG_BT_OTS_CLIENT
/* Defined in esp_ble_audio (ots/adapter/l2cap.c) - the only L2CAP consumer is
 * OTS, so the shim lives there. Declared instead of included to keep esp_ble_iso
 * free of audio headers; both live in the bt component, so the link resolves. */
extern void bt_le_l2cap_handle_event(void *data, size_t data_len);
extern void bt_le_l2cap_event_free(void *data);
#endif

#if CONFIG_BT_ISO_DISPATCH_MONITOR
/* Per-type dispatch timing, indexed by iso_queue_item_type. Written only by
 * iso_task (single writer); read by bt_le_iso_dispatch_stats_dump. */
static BT_ISO_CTRL_BSS_ATTR struct iso_dispatch_stats {
    int64_t  max_us;
    uint32_t count;
    uint32_t slow_count;
} iso_stats[ISO_QUEUE_ITEM_TYPE_MAX];

static void iso_dispatch_record(uint8_t type, int64_t elapsed_us)
{
    struct iso_dispatch_stats *st;

    if (type >= ISO_QUEUE_ITEM_TYPE_MAX) {
        return;
    }

    st = &iso_stats[type];
    st->count++;
    if (elapsed_us > st->max_us) {
        st->max_us = elapsed_us;
    }
    if (elapsed_us > ISO_DISPATCH_THRESHOLD_US) {
        st->slow_count++;
        LOG_WRN("IsoCbSlow[%u][%lld]", type, (long long)elapsed_us);
    }
}

void bt_le_iso_dispatch_stats_dump(void)
{
    int i;

    LOG_INF("IsoCbStats thr=%dus", ISO_DISPATCH_THRESHOLD_US);

    for (i = 0; i < ISO_QUEUE_ITEM_TYPE_MAX; i++) {
        if (iso_stats[i].count == 0) {
            continue;
        }

        LOG_INF("  type[%d] cnt=%u max=%lld slow=%u", i, iso_stats[i].count,
                (long long)iso_stats[i].max_us, iso_stats[i].slow_count);
    }
}
#endif /* CONFIG_BT_ISO_DISPATCH_MONITOR */

static void iso_item_release(const struct iso_queue_item *item)
{
    switch (item->type) {
    case ISO_QUEUE_ITEM_TYPE_TIMER_EVENT:
        /* data is the k_work, data_len its generation counter - not a block. */
        break;
    case ISO_QUEUE_ITEM_TYPE_GATT_EVENT:
        bt_le_gatt_event_free(item->data);
        break;
    case ISO_QUEUE_ITEM_TYPE_GAP_EVENT:
    case ISO_QUEUE_ITEM_TYPE_EXT_ADV_REPORT:
    case ISO_QUEUE_ITEM_TYPE_PER_ADV_REPORT:
        bt_le_gap_event_free(item->data);
        break;
#if CONFIG_BT_OTS || CONFIG_BT_OTS_CLIENT
    case ISO_QUEUE_ITEM_TYPE_L2CAP_EVENT:
        bt_le_l2cap_event_free(item->data);
        break;
#endif /* CONFIG_BT_OTS || CONFIG_BT_OTS_CLIENT */
    default:
        if (item->data) {
            free(item->data);
        }
        break;
    }
}

/* Runs on iso_task after the loop exits, so no producer can be mid-dispatch and
 * the queues are provably empty when iso_queues_destroy() deletes them. */
static void iso_queues_drain(void)
{
    struct iso_queue_item item = {0};

    while (xQueueReceive(iso_critical_queue, &item, 0) == pdTRUE) {
        iso_item_release(&item);
    }

    while (xQueueReceive(iso_normal_queue, &item, 0) == pdTRUE) {
        iso_item_release(&item);
    }

    while (xQueueReceive(iso_floodable_queue, &item, 0) == pdTRUE) {
        iso_item_release(&item);
    }
}

static void iso_dispatch_item(const struct iso_queue_item *item)
{
#if CONFIG_BT_ISO_DISPATCH_MONITOR
    int64_t elapsed_us;
    int64_t start_us;

    start_us = esp_timer_get_time();
#endif /* CONFIG_BT_ISO_DISPATCH_MONITOR */

    switch (item->type) {
    case ISO_QUEUE_ITEM_TYPE_TIMER_EVENT:
        bt_le_timer_handle_event(item->data, item->data_len);
        break;
    case ISO_QUEUE_ITEM_TYPE_GAP_EVENT:
    case ISO_QUEUE_ITEM_TYPE_EXT_ADV_REPORT:
    case ISO_QUEUE_ITEM_TYPE_PER_ADV_REPORT:
        bt_le_gap_handle_event(item->data, item->data_len);
        break;
    case ISO_QUEUE_ITEM_TYPE_GATT_EVENT:
        bt_le_gatt_handle_event(item->data, item->data_len);
        break;
#if CONFIG_BT_OTS || CONFIG_BT_OTS_CLIENT
    case ISO_QUEUE_ITEM_TYPE_L2CAP_EVENT:
        bt_le_l2cap_handle_event(item->data, item->data_len);
        break;
#endif /* CONFIG_BT_OTS || CONFIG_BT_OTS_CLIENT */
    case ISO_QUEUE_ITEM_TYPE_ISO_HCI_EVENT:
    case ISO_QUEUE_ITEM_TYPE_BIGINFO_ADV_REPORT:
        bt_le_iso_handle_hci_event(item->data, item->data_len);
        break;
    case ISO_QUEUE_ITEM_TYPE_ISO_TX_COMP:
        bt_le_iso_handle_tx_comp(item->data, item->data_len);
        break;
    case ISO_QUEUE_ITEM_TYPE_ISO_RX_DATA:
        bt_le_iso_handle_rx_data(item->data, item->data_len);
        break;
    default:
        iso_item_release(item);
        BT_LE_ASSERT(0);
        break;
    }

#if CONFIG_BT_ISO_DISPATCH_MONITOR
    elapsed_us = esp_timer_get_time() - start_us;
    iso_dispatch_record(item->type, elapsed_us);
#endif /* CONFIG_BT_ISO_DISPATCH_MONITOR */
}

static void iso_task(void *p)
{
#if CONFIG_BT_ISO_DISPATCH_MONITOR
    int64_t last_dump_us = esp_timer_get_time();
#endif /* CONFIG_BT_ISO_DISPATCH_MONITOR */
    struct iso_queue_item item = {0};

    while (!iso_task_stopping) {
        /* The returned handle is ignored: service by strict priority instead
         * (critical > normal > floodable), one item per wakeup. A pdFALSE
         * receive is benign - that is what a deinit wakeup looks like. */
        (void)xQueueSelectFromSet(iso_queue_set, ISO_TASK_WAIT);

        if (xQueueReceive(iso_critical_queue, &item, 0) == pdTRUE) {
            iso_dispatch_item(&item);
        } else if (xQueueReceive(iso_normal_queue, &item, 0) == pdTRUE) {
            iso_dispatch_item(&item);
        } else if (xQueueReceive(iso_floodable_queue, &item, 0) == pdTRUE) {
            iso_dispatch_item(&item);
        }

#if CONFIG_BT_ISO_DISPATCH_MONITOR
        if (esp_timer_get_time() - last_dump_us >= ISO_STATS_DUMP_PERIOD_US) {
            bt_le_iso_dispatch_stats_dump();
            last_dump_us = esp_timer_get_time();
        }
#endif /* CONFIG_BT_ISO_DISPATCH_MONITOR */
    }

    /* Draining here rather than in the deinit caller keeps payload ownership on
     * a single task: no producer is mid-post and no consumer is mid-dispatch. */
    iso_queues_drain();

    xSemaphoreGive(iso_task_stopped);

    vTaskDelete(NULL);
}

int bt_le_iso_task_post(enum iso_queue_item_type type,
                        void *data, size_t data_len)
{
    struct iso_queue_item item = {0};
    QueueHandle_t queue;
    TickType_t wait;
    int ret;

    /* No consumer before init or after deinit began, so accepting would strand
     * the payload (callers free on failure). Distinct from the -1 below: this
     * one persists, a full queue is transient. */
    if (iso_task_handle == NULL || iso_task_stopping) {
        return -ESHUTDOWN;
    }

    item.type = type;
    item.data = data;
    item.data_len = data_len;

    switch (type) {
    case ISO_QUEUE_ITEM_TYPE_ISO_RX_DATA:
    case ISO_QUEUE_ITEM_TYPE_ISO_TX_COMP:
        /* Latency-critical, posted from the controller task: never block.
         * On a full queue the caller drops + frees the payload. */
        queue = iso_critical_queue;
        wait = 0;
        break;
    case ISO_QUEUE_ITEM_TYPE_EXT_ADV_REPORT:
    case ISO_QUEUE_ITEM_TYPE_PER_ADV_REPORT:
    case ISO_QUEUE_ITEM_TYPE_BIGINFO_ADV_REPORT:
        /* High-volume best-effort reports: never block, drop newest on full. */
        queue = iso_floodable_queue;
        wait = 0;
        break;
    default:
        /* Timer / GATT / HCI / GAP lifecycle. A self-post from iso_task must
         * not block (it is the sole consumer -> blocking on its own full queue
         * deadlocks); external producers get a bounded wait, not portMAX_DELAY,
         * so a wedged iso_task can't freeze esp_timer / the host task and stall
         * the ISO data path. */
        queue = iso_normal_queue;
        wait = (xTaskGetCurrentTaskHandle() == iso_task_handle) ? 0 : K_QUEUE_SHORT;
        break;
    }

    ret = xQueueSend(queue, &item, wait);
    if (ret != pdTRUE) {
        /* A non-blocking send failing is an expected drop (handled + logged by
         * the caller); only a blocking send failing is a genuine error. */
        if (wait != 0) {
            LOG_ERR("IsoQPostFail[%d][%u]", ret, type);
        }
        return -1;
    }

    return 0;
}

static void iso_queue_destroy_one(QueueHandle_t *queue)
{
    struct iso_queue_item item = {0};

    if (*queue == NULL) {
        return;
    }

    /* Empty it so it can leave the set; pending payloads are not freed here
     * (matches the pre-split deinit behavior). */
    while (xQueueReceive(*queue, &item, 0) == pdTRUE) {
    }

    if (iso_queue_set) {
        xQueueRemoveFromSet(*queue, iso_queue_set);
    }

    vQueueDelete(*queue);
    *queue = NULL;
}

static void iso_queues_destroy(void)
{
    iso_queue_destroy_one(&iso_critical_queue);
    iso_queue_destroy_one(&iso_normal_queue);
    iso_queue_destroy_one(&iso_floodable_queue);
    iso_queue_destroy_one(&iso_ctrl_queue);

    if (iso_queue_set) {
        vQueueDelete(iso_queue_set);
        iso_queue_set = NULL;
    }
}

int bt_le_iso_task_init(void)
{
    int ret;

    LOG_DBG("IsoTaskInit");

    /* Reset here, not at definition, so a deinit/re-init cycle starts clean. */
    iso_task_stopping = false;

    iso_task_stopped = xSemaphoreCreateBinary();
    if (iso_task_stopped == NULL) {
        LOG_ERR("IsoTaskSemCreateFail");
        return -EIO;
    }

    iso_critical_queue  = xQueueCreate(ISO_CRITICAL_QUEUE_LEN, ISO_QUEUE_ITEM_SIZE);
    iso_normal_queue    = xQueueCreate(ISO_NORMAL_QUEUE_LEN, ISO_QUEUE_ITEM_SIZE);
    iso_floodable_queue = xQueueCreate(ISO_FLOODABLE_QUEUE_LEN, ISO_QUEUE_ITEM_SIZE);
    iso_ctrl_queue      = xQueueCreate(ISO_CTRL_QUEUE_LEN, ISO_QUEUE_ITEM_SIZE);
    iso_queue_set       = xQueueCreateSet(ISO_QUEUE_SET_LEN);
    if (iso_critical_queue == NULL || iso_normal_queue == NULL ||
            iso_floodable_queue == NULL || iso_ctrl_queue == NULL ||
            iso_queue_set == NULL) {
        LOG_ERR("IsoQCreateFail");
        goto fail;
    }

    if (xQueueAddToSet(iso_critical_queue, iso_queue_set) != pdPASS ||
            xQueueAddToSet(iso_normal_queue, iso_queue_set) != pdPASS ||
            xQueueAddToSet(iso_floodable_queue, iso_queue_set) != pdPASS ||
            xQueueAddToSet(iso_ctrl_queue, iso_queue_set) != pdPASS) {
        LOG_ERR("IsoQSetAddFail");
        goto fail;
    }

    ret = xTaskCreatePinnedToCore(iso_task,
                                  ISO_TASK_NAME,
                                  ISO_TASK_STACK_SIZE,
                                  NULL,
                                  ISO_TASK_PRIO,
                                  &iso_task_handle,
                                  ISO_TASK_CORE);
    if (ret != pdTRUE) {
        LOG_ERR("IsoTaskCreateFail[%d]", ret);
        goto fail;
    }

    return 0;

fail:
    iso_queues_destroy();
    vSemaphoreDelete(iso_task_stopped);
    iso_task_stopped = NULL;
    return -EIO;
}

int bt_le_iso_task_deinit(void)
{
    struct iso_queue_item item = {0};

    LOG_DBG("IsoTaskDeinit");

    if (iso_task_handle == NULL) {
        return 0;
    }

    /* This blocks on the task's own exit, so calling it from iso_task would
     * wait for itself forever. */
    if (xTaskGetCurrentTaskHandle() == iso_task_handle) {
        LOG_ERR("IsoTaskDeinitFromSelf");
        return -EDEADLK;
    }

    /* Stops new posts as well, so the queues can only shrink from here. */
    iso_task_stopping = true;

    /* Setting the flag cannot wake a task blocked on the set. Posted after it so
     * whichever select consumes this token re-checks the flag as true; deinit is
     * the sole producer of a one-deep queue, so the send cannot fail. */
    (void)xQueueSend(iso_ctrl_queue, &item, 0);

    if (xSemaphoreTake(iso_task_stopped, ISO_TASK_STOP_TIMEOUT) != pdTRUE) {
        /* A dispatch handler is wedged. Deleting the queues now would pull them
         * out from under a live task, so leave everything in place and let the
         * caller abort the teardown instead. */
        LOG_ERR("IsoTaskStopTimeout");
        iso_task_stopping = false;
        /* Take the wakeup back: nothing reads this queue, so leaving it there
         * would make the next attempt's send fail and never wake the task. */
        (void)xQueueReceive(iso_ctrl_queue, &item, 0);
        return -ETIMEDOUT;
    }

    iso_task_handle = NULL;

#if CONFIG_BT_ISO_DISPATCH_MONITOR
    /* Task is gone: no concurrent writer, safe to read the stats. */
    bt_le_iso_dispatch_stats_dump();
#endif /* CONFIG_BT_ISO_DISPATCH_MONITOR */

    /* Drained by the task before it exited, so these are empty. */
    iso_queues_destroy();

    vSemaphoreDelete(iso_task_stopped);
    iso_task_stopped = NULL;

    return 0;
}
