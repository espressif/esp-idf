/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef HOST_COMMON_TASK_H_
#define HOST_COMMON_TASK_H_

#include <stdint.h>

#include "sdkconfig.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"

#ifdef __cplusplus
extern "C" {
#endif

#if CONFIG_BT_BLUEDROID_ENABLED
#if CONFIG_BT_BLUEDROID_PINNED_TO_CORE
#define ISO_TASK_CORE           CONFIG_BT_BLUEDROID_PINNED_TO_CORE
#else /* CONFIG_BT_BLUEDROID_PINNED_TO_CORE */
#define ISO_TASK_CORE           (0)
#endif /* CONFIG_BT_BLUEDROID_PINNED_TO_CORE */
#else /* CONFIG_BT_BLUEDROID_ENABLED */
#if CONFIG_BT_NIMBLE_PINNED_TO_CORE
#define ISO_TASK_CORE           CONFIG_BT_NIMBLE_PINNED_TO_CORE
#else /* CONFIG_BT_NIMBLE_PINNED_TO_CORE */
#define ISO_TASK_CORE           (0)
#endif /* CONFIG_BT_NIMBLE_PINNED_TO_CORE */
#endif /* CONFIG_BT_BLUEDROID_ENABLED */

#define ISO_TASK_STACK_SIZE     4096
#define ISO_TASK_NAME           "iso_task"
/* Ref:
 * - Bluedroid BTC task: configMAX_PRIORITIES - 6
 * - Bluedroid BTU task: configMAX_PRIORITIES - 5
 * - NimBLE Host task:   configMAX_PRIORITIES - 4
 */
#if CONFIG_BT_BLUEDROID_ENABLED
#define ISO_TASK_PRIO           (configMAX_PRIORITIES - 5)
#else
#define ISO_TASK_PRIO           (configMAX_PRIORITIES - 4)
#endif

/* Ordered by priority tier, highest first. The numeric value is not used for
 * prioritization (that comes from which queue bt_le_iso_task_post routes to);
 * the order is for readability of the three tiers. */
enum iso_queue_item_type {
    /* critical: latency-critical ISO data, drained first, posted non-blocking
     * from the controller; an overflow drops the newest. */
    ISO_QUEUE_ITEM_TYPE_ISO_RX_DATA,
    ISO_QUEUE_ITEM_TYPE_ISO_TX_COMP,

    /* normal: reliable events (blocking send, never dropped). */
    ISO_QUEUE_ITEM_TYPE_TIMER_EVENT,
    ISO_QUEUE_ITEM_TYPE_GAP_EVENT,
    ISO_QUEUE_ITEM_TYPE_GATT_EVENT,
    ISO_QUEUE_ITEM_TYPE_ISO_HCI_EVENT,

    /* floodable: high-volume best-effort reports, posted non-blocking, newest
     * dropped on overflow. Named 1:1 after the HCI report events. They share
     * the floodable queue but EXT/PER use bt_le_gap_handle_event while BIGInfo
     * uses bt_le_iso_handle_hci_event:
     *   EXT_ADV_REPORT     - LE Extended Advertising Report
     *   PER_ADV_REPORT     - LE Periodic Advertising Report
     *   BIGINFO_ADV_REPORT - LE BIGInfo Advertising Report (per spec it follows
     *                        every PA report carrying a BIGInfo field, i.e. at
     *                        the PA interval rate) */
    ISO_QUEUE_ITEM_TYPE_EXT_ADV_REPORT,
    ISO_QUEUE_ITEM_TYPE_PER_ADV_REPORT,
    ISO_QUEUE_ITEM_TYPE_BIGINFO_ADV_REPORT,

    ISO_QUEUE_ITEM_TYPE_MAX,
};

struct iso_queue_item {
    uint8_t  type;
    uint8_t *data;
    size_t   data_len;
};

/* Per-priority queue depths. The single FIFO is split into three queues so a
 * flood of GAP reports cannot delay the latency-critical ISO data path:
 *   critical  - ISO_RX_DATA / ISO_TX_COMP (drained first, non-blocking send)
 *   normal    - timer / gatt / hci / GAP lifecycle (reliable, blocking send)
 *   floodable - GAP scan / PA data reports (droppable, non-blocking send)
 */
#define ISO_CRITICAL_QUEUE_LEN      32
#define ISO_NORMAL_QUEUE_LEN        64
#define ISO_FLOODABLE_QUEUE_LEN     32
/* The set must be able to hold one token per item across all three queues. */
#define ISO_QUEUE_SET_LEN           (ISO_CRITICAL_QUEUE_LEN + \
                                     ISO_NORMAL_QUEUE_LEN + \
                                     ISO_FLOODABLE_QUEUE_LEN)
#define ISO_QUEUE_ITEM_SIZE         sizeof(struct iso_queue_item)

#if CONFIG_BT_ISO_DISPATCH_MONITOR
#define ISO_DISPATCH_THRESHOLD_US   CONFIG_BT_ISO_DISPATCH_THRESHOLD_US
#define ISO_STATS_DUMP_PERIOD_US    (CONFIG_BT_ISO_DISPATCH_DUMP_PERIOD_S * 1000 * 1000)

void bt_le_iso_dispatch_stats_dump(void);
#endif /* CONFIG_BT_ISO_DISPATCH_MONITOR */

int bt_le_iso_task_post(enum iso_queue_item_type type,
                        void *data, size_t data_len);

int bt_le_iso_task_init(void);

void bt_le_iso_task_deinit(void);

#ifdef __cplusplus
}
#endif

#endif /* HOST_COMMON_TASK_H_ */
