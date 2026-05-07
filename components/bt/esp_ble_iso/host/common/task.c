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

#include <../host/conn_internal.h>

#include "common/host.h"
#include "common/iso.h"
#include "common/app/gap.h"
#include "common/app/gatt.h"

static QueueHandle_t iso_queue_handle;

static TaskHandle_t iso_task_handle;

extern void bt_le_timer_handle_event(void *arg);

static void iso_task(void *p)
{
    struct iso_queue_item item = {0};

    while (1) {
        xQueueReceive(iso_queue_handle, &item, portMAX_DELAY);

        switch (item.type) {
        case ISO_QUEUE_ITEM_TYPE_TIMER_EVENT:
            bt_le_timer_handle_event(item.data);
            break;
        case ISO_QUEUE_ITEM_TYPE_GAP_EVENT:
            bt_le_gap_handle_event(item.data, item.data_len);
            break;
        case ISO_QUEUE_ITEM_TYPE_GATT_EVENT:
            bt_le_gatt_handle_event(item.data, item.data_len);
            break;
        case ISO_QUEUE_ITEM_TYPE_ISO_HCI_EVENT:
            bt_le_iso_handle_hci_event(item.data, item.data_len);
            break;
        case ISO_QUEUE_ITEM_TYPE_ISO_TX_COMP:
            bt_le_iso_handle_tx_comp(item.data, item.data_len);
            break;
        case ISO_QUEUE_ITEM_TYPE_ISO_RX_DATA:
            bt_le_iso_handle_rx_data(item.data, item.data_len);
            break;
        default:
            if (item.data) {
                free(item.data);
            }
            assert(0);
            break;
        }
    }
}

int bt_le_iso_task_post(enum iso_queue_item_type type,
                        void *data, size_t data_len)
{
    struct iso_queue_item item = {0};
    int ret;

    item.type = type;
    item.data = data;
    item.data_len = data_len;

    ret = xQueueSend(iso_queue_handle, &item, portMAX_DELAY);
    if (ret != pdTRUE) {
        LOG_ERR("IsoQPostFail[%d]", ret);
        return -1;
    }

    return 0;
}

int bt_le_iso_task_init(void)
{
    int ret;

    LOG_DBG("IsoTaskInit");

    iso_queue_handle = xQueueCreate(ISO_QUEUE_ITEM_COUNT, ISO_QUEUE_ITEM_SIZE);
    if (iso_queue_handle == NULL) {
        LOG_ERR("IsoQCreateFail");
        return -EIO;
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
        vQueueDelete(iso_queue_handle);
        iso_queue_handle = NULL;
        return -EIO;
    }

    return 0;
}

void bt_le_iso_task_deinit(void)
{
    LOG_DBG("IsoTaskDeinit");

    if (iso_task_handle) {
        vTaskDelete(iso_task_handle);
        iso_task_handle = NULL;
    }

    if (iso_queue_handle) {
        vQueueDelete(iso_queue_handle);
        iso_queue_handle = NULL;
    }
}
