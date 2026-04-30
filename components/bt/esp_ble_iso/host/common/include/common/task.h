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

#if CONFIG_BT_NIMBLE_PINNED_TO_CORE
#define ISO_TASK_CORE           CONFIG_BT_NIMBLE_PINNED_TO_CORE
#else /* CONFIG_BT_NIMBLE_PINNED_TO_CORE */
#define ISO_TASK_CORE           (0)
#endif /* CONFIG_BT_NIMBLE_PINNED_TO_CORE */

#define ISO_TASK_STACK_SIZE     4096
#define ISO_TASK_NAME           "iso_task"
/* Ref:
 * - NimBLE Host task:   configMAX_PRIORITIES - 4
 */
#define ISO_TASK_PRIO           (configMAX_PRIORITIES - 4)

enum iso_queue_item_type {
    ISO_QUEUE_ITEM_TYPE_TIMER_EVENT,
    ISO_QUEUE_ITEM_TYPE_GAP_EVENT,
    ISO_QUEUE_ITEM_TYPE_GATT_EVENT,
    ISO_QUEUE_ITEM_TYPE_ISO_HCI_EVENT,
    ISO_QUEUE_ITEM_TYPE_ISO_TX_COMP,
    ISO_QUEUE_ITEM_TYPE_ISO_RX_DATA,

    ISO_QUEUE_ITEM_TYPE_MAX,
};

struct iso_queue_item {
    uint8_t  type;
    uint8_t *data;
    size_t   data_len;
};

#define ISO_QUEUE_ITEM_COUNT    100
#define ISO_QUEUE_ITEM_SIZE     sizeof(struct iso_queue_item)

int bt_le_iso_task_post(enum iso_queue_item_type type,
                        void *data, size_t data_len);

int bt_le_iso_task_init(void);

void bt_le_iso_task_deinit(void);

#ifdef __cplusplus
}
#endif

#endif /* HOST_COMMON_TASK_H_ */
