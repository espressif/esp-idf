/******************************************************************************
 *
 *  Copyright (C) 2014 Google, Inc.
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at:
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 ******************************************************************************/
#include <string.h>
#include "bt_defs.h"
#include "bt_trace.h"
#include "bt_types.h"
#include "buffer_allocator.h"
#include "fixed_queue.h"
#include "hci_hal.h"
#include "hci_internals.h"
#include "hci_layer.h"
#include "thread.h"
#include "bt.h"

#define HCI_HAL_SERIAL_BUFFER_SIZE 1026
#define HCI_BLE_EVENT 0x3e
#define PACKET_TYPE_TO_INBOUND_INDEX(type) ((type) - 2)
#define PACKET_TYPE_TO_INDEX(type) ((type) - 1)


static const uint8_t preamble_sizes[] = {
    HCI_COMMAND_PREAMBLE_SIZE,
    HCI_ACL_PREAMBLE_SIZE,
    HCI_SCO_PREAMBLE_SIZE,
    HCI_EVENT_PREAMBLE_SIZE
};

static const uint16_t outbound_event_types[] = {
    MSG_HC_TO_STACK_HCI_ERR,
    MSG_HC_TO_STACK_HCI_ACL,
    MSG_HC_TO_STACK_HCI_SCO,
    MSG_HC_TO_STACK_HCI_EVT
};

typedef struct {
    const allocator_t *allocator;
    size_t buffer_size;
    fixed_queue_t *rx_q;
} hci_hal_env_t;


static hci_hal_env_t hci_hal_env;
static const hci_hal_t interface;
static const hci_hal_callbacks_t *callbacks;
static const esp_vhci_host_callback_t vhci_host_cb;

static xTaskHandle xHciH4TaskHandle;
static xQueueHandle xHciH4Queue;

static void host_send_pkt_available_cb(void);
static int host_recv_pkt_cb(uint8_t *data, uint16_t len);

static void hci_hal_h4_rx_handler(void *arg);
static void event_uart_has_bytes(fixed_queue_t *queue);


static void hci_hal_env_init(
    size_t buffer_size,
    size_t max_buffer_count)
{
    assert(buffer_size > 0);
    assert(max_buffer_count > 0);

    hci_hal_env.allocator = buffer_allocator_get_interface();
    hci_hal_env.buffer_size = buffer_size;

    hci_hal_env.rx_q = fixed_queue_new(max_buffer_count);
    if (hci_hal_env.rx_q) {
        fixed_queue_register_dequeue(hci_hal_env.rx_q, event_uart_has_bytes);
    } else {
        LOG_ERROR("%s unable to create rx queue.\n", __func__);
    }

    return;
}

static void hci_hal_env_deinit(void)
{
    fixed_queue_free(hci_hal_env.rx_q, hci_hal_env.allocator->free);
}

static bool hal_open(const hci_hal_callbacks_t *upper_callbacks)
{
    assert(upper_callbacks != NULL);
    callbacks = upper_callbacks;

    hci_hal_env_init(HCI_HAL_SERIAL_BUFFER_SIZE, SIZE_MAX);

    xHciH4Queue = xQueueCreate(HCI_H4_QUEUE_NUM, sizeof(BtTaskEvt_t));
    xTaskCreatePinnedToCore(hci_hal_h4_rx_handler, HCI_H4_TASK_NAME, HCI_H4_TASK_STACK_SIZE, NULL, HCI_H4_TASK_PRIO, &xHciH4TaskHandle, 0);

    //register vhci host cb
    esp_vhci_host_register_callback(&vhci_host_cb);


    return true;
}

static void hal_close()
{
    hci_hal_env_deinit();

    /* delete task and queue */
    vTaskDelete(xHciH4TaskHandle);
    vQueueDelete(xHciH4Queue);
}

/**
 * Function: transmit_data -TX data to low-layer
 * It is ported from Bluedroid source code, so it is not
 * needed to use write() to send data.
 * TODO: Just use firmware API to send data.
 */
static uint16_t transmit_data(serial_data_type_t type,
                              uint8_t *data, uint16_t length)
{
    uint8_t previous_byte;

    assert(data != NULL);
    assert(length > 0);

    if (type < DATA_TYPE_COMMAND || type > DATA_TYPE_SCO) {
        LOG_ERROR("%s invalid data type: %d", __func__, type);
        return 0;
    }

    // Write the signal byte right before the data
    --data;
    previous_byte = *data;
    *(data) = type;
    ++length;

    BTTRC_DUMP_BUFFER("Transmit Pkt", data, length);

    // TX Data to target
    esp_vhci_host_send_packet(data, length);

    // Be nice and restore the old value of that byte
    *(data) = previous_byte;

    return length - 1;
}

// Internal functions
static void hci_hal_h4_rx_handler(void *arg)
{
    BtTaskEvt_t e;

    for (;;) {
        if (pdTRUE == xQueueReceive(xHciH4Queue, &e, (portTickType)portMAX_DELAY)) {
            if (e.sig == 0xff) {
                fixed_queue_process(hci_hal_env.rx_q);
            }
        }
    }
}

void hci_hal_h4_task_post(void)
{
    BtTaskEvt_t evt;

    evt.sig = 0xff;
    evt.par = 0;

    if (xQueueSend(xHciH4Queue, &evt, 10 / portTICK_PERIOD_MS) != pdTRUE) {
        LOG_ERROR("xHciH4Queue failed\n");
    }
}

static void hci_hal_h4_hdl_rx_packet(BT_HDR *packet)
{
    uint8_t type, hdr_size;
    uint16_t length;
    uint8_t *stream = packet->data + packet->offset;

    if (!packet) {
        return;
    }
    STREAM_TO_UINT8(type, stream);
    packet->offset++;
    packet->len--;
    if (type == HCI_BLE_EVENT) {
        uint8_t len;
        STREAM_TO_UINT8(len, stream);
        LOG_ERROR("Workround stream corrupted during LE SCAN: pkt_len=%d ble_event_len=%d\n",
                  packet->len, len);
        hci_hal_env.allocator->free(packet);
        return;
    }
    if (type < DATA_TYPE_ACL || type > DATA_TYPE_EVENT) {
        LOG_ERROR("%s Unknown HCI message type. Dropping this byte 0x%x,"
                  " min %x, max %x\n", __func__, type,
                  DATA_TYPE_ACL, DATA_TYPE_EVENT);
        hci_hal_env.allocator->free(packet);
        return;
    }
    hdr_size = preamble_sizes[type - 1];
    if (packet->len < hdr_size) {
        LOG_ERROR("Wrong packet length type=%d pkt_len=%d hdr_len=%d",
                  type, packet->len, hdr_size);
        hci_hal_env.allocator->free(packet);
        return;
    }
    if (type == DATA_TYPE_ACL) {
        packet->offset--;
        stream += hdr_size - 2;
        STREAM_TO_UINT16(length, stream);
        stream = packet->data + 1;
        memcpy(packet->data, stream, packet->len);
    } else {
        stream += hdr_size - 1;
        STREAM_TO_UINT8(length, stream);
    }

    if ((length + hdr_size) != packet->len) {
        LOG_ERROR("Wrong packet length type=%d hdr_len=%d pd_len=%d "
                  "pkt_len=%d", type, hdr_size, length, packet->len);
        hci_hal_env.allocator->free(packet);
        return;
    }

    packet->event = outbound_event_types[PACKET_TYPE_TO_INDEX(type)];
    callbacks->packet_ready(packet);
}

static void event_uart_has_bytes(fixed_queue_t *queue)
{
    BT_HDR *packet;
    while (!fixed_queue_is_empty(queue)) {
        packet = fixed_queue_dequeue(queue);
        hci_hal_h4_hdl_rx_packet(packet);
    }
}

static void host_send_pkt_available_cb(void)
{
    //Controller rx cache buffer is ready for receiving new host packet
    //Just Call Host main thread task to process pending packets.
    hci_host_task_post();
}

static int host_recv_pkt_cb(uint8_t *data, uint16_t len)
{
    //Target has packet to host, malloc new buffer for packet
    BT_HDR *pkt;
    size_t pkt_size;

    pkt_size = BT_HDR_SIZE + len;
    pkt = (BT_HDR *)hci_hal_env.allocator->alloc(pkt_size);
    if (!pkt) {
        LOG_ERROR("%s couldn't aquire memory for inbound data buffer.\n", __func__);
        return -1;
    }
    pkt->offset = 0;
    pkt->len = len;
    pkt->layer_specific = 0;
    memcpy(pkt->data, data, len);
    fixed_queue_enqueue(hci_hal_env.rx_q, pkt);
    hci_hal_h4_task_post();

    BTTRC_DUMP_BUFFER("Recv Pkt", pkt->data, len);

    return 0;
}

static const esp_vhci_host_callback_t vhci_host_cb = {
    .notify_host_send_available = host_send_pkt_available_cb,
    .notify_host_recv = host_recv_pkt_cb,
};

static const hci_hal_t interface = {
    hal_open,
    hal_close,
    transmit_data,
};

const hci_hal_t *hci_hal_h4_get_interface()
{
    return &interface;
}

