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
#include "common/app/gap.h"
#include "common/app/gatt.h"

LOG_MODULE_REGISTER(ISO_TASK, CONFIG_BT_ISO_LOG_LEVEL);

/* Three priority tiers share one task via a queue set. The task drains
 * critical before normal before floodable, so a flood of GAP reports cannot
 * delay the latency-critical ISO data path. See common/task.h for the mapping.
 */
static QueueHandle_t iso_critical_queue;
static QueueHandle_t iso_normal_queue;
static QueueHandle_t iso_floodable_queue;
static QueueSetHandle_t iso_queue_set;

static TaskHandle_t iso_task_handle;

extern void bt_le_timer_handle_event(void *arg);

#if CONFIG_BT_ISO_DISPATCH_MONITOR
/* Per-type dispatch timing, indexed by iso_queue_item_type. Written only by
 * iso_task (single writer); read by bt_le_iso_dispatch_stats_dump. */
static struct iso_dispatch_stats {
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

static void iso_dispatch_item(const struct iso_queue_item *item)
{
#if CONFIG_BT_ISO_DISPATCH_MONITOR
    int64_t elapsed_us;
    int64_t start_us;

    start_us = esp_timer_get_time();
#endif /* CONFIG_BT_ISO_DISPATCH_MONITOR */

    switch (item->type) {
    case ISO_QUEUE_ITEM_TYPE_TIMER_EVENT:
        bt_le_timer_handle_event(item->data);
        break;
    case ISO_QUEUE_ITEM_TYPE_GAP_EVENT:
    case ISO_QUEUE_ITEM_TYPE_EXT_ADV_REPORT:
    case ISO_QUEUE_ITEM_TYPE_PER_ADV_REPORT:
        bt_le_gap_handle_event(item->data, item->data_len);
        break;
    case ISO_QUEUE_ITEM_TYPE_GATT_EVENT:
        bt_le_gatt_handle_event(item->data, item->data_len);
        break;
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
        if (item->data) {
            free(item->data);
        }
        assert(0);
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

    while (1) {
        /* Block until any tier has data. The returned member handle is ignored:
         * we always service by strict priority below (critical > normal >
         * floodable), processing one item per wakeup and re-checking critical
         * first on the next loop. A pdFALSE receive is tolerated as a benign
         * side effect of servicing queues outside xQueueSelectFromSet. */
        (void)xQueueSelectFromSet(iso_queue_set, portMAX_DELAY);

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
}

int bt_le_iso_task_post(enum iso_queue_item_type type,
                        void *data, size_t data_len)
{
    struct iso_queue_item item = {0};
    QueueHandle_t queue;
    TickType_t wait;
    int ret;

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
        /* Timer / GATT / HCI / GAP lifecycle: reliable, block until space. */
        queue = iso_normal_queue;
        wait = portMAX_DELAY;
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

    if (iso_queue_set) {
        vQueueDelete(iso_queue_set);
        iso_queue_set = NULL;
    }
}

int bt_le_iso_task_init(void)
{
    int ret;

    LOG_DBG("IsoTaskInit");

    iso_critical_queue  = xQueueCreate(ISO_CRITICAL_QUEUE_LEN, ISO_QUEUE_ITEM_SIZE);
    iso_normal_queue    = xQueueCreate(ISO_NORMAL_QUEUE_LEN, ISO_QUEUE_ITEM_SIZE);
    iso_floodable_queue = xQueueCreate(ISO_FLOODABLE_QUEUE_LEN, ISO_QUEUE_ITEM_SIZE);
    iso_queue_set       = xQueueCreateSet(ISO_QUEUE_SET_LEN);
    if (iso_critical_queue == NULL || iso_normal_queue == NULL ||
            iso_floodable_queue == NULL || iso_queue_set == NULL) {
        LOG_ERR("IsoQCreateFail");
        goto fail;
    }

    if (xQueueAddToSet(iso_critical_queue, iso_queue_set) != pdPASS ||
            xQueueAddToSet(iso_normal_queue, iso_queue_set) != pdPASS ||
            xQueueAddToSet(iso_floodable_queue, iso_queue_set) != pdPASS) {
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
    return -EIO;
}

void bt_le_iso_task_deinit(void)
{
    LOG_DBG("IsoTaskDeinit");

    if (iso_task_handle) {
        vTaskDelete(iso_task_handle);
        iso_task_handle = NULL;
    }

#if CONFIG_BT_ISO_DISPATCH_MONITOR
    /* Task is gone: no concurrent writer, safe to read the stats. */
    bt_le_iso_dispatch_stats_dump();
#endif /* CONFIG_BT_ISO_DISPATCH_MONITOR */

    iso_queues_destroy();
}
