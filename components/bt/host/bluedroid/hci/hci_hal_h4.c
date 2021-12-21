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
#include "common/bt_defs.h"
#include "common/bt_trace.h"
#include "stack/bt_types.h"
#include "osi/fixed_queue.h"
#include "hci/hci_hal.h"
#include "hci/hci_internals.h"
#include "hci/hci_layer.h"
#include "osi/thread.h"
#include "esp_bt.h"
#include "stack/hcimsgs.h"

#if (C2H_FLOW_CONTROL_INCLUDED == TRUE)
#include "l2c_int.h"
#endif ///C2H_FLOW_CONTROL_INCLUDED == TRUE
#include "stack/hcimsgs.h"

#define HCI_HAL_SERIAL_BUFFER_SIZE 1026
#define HCI_BLE_EVENT 0x3e
#define PACKET_TYPE_TO_INBOUND_INDEX(type) ((type) - 2)
#define PACKET_TYPE_TO_INDEX(type) ((type) - 1)
extern bool BTU_check_queue_is_congest(void);


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
    size_t buffer_size;
    fixed_queue_t *rx_q;
    uint16_t adv_free_num;
} hci_hal_env_t;


static hci_hal_env_t hci_hal_env;
static const hci_hal_t interface;
static const hci_hal_callbacks_t *callbacks;
static const esp_vhci_host_callback_t vhci_host_cb;
static osi_thread_t *hci_h4_thread;

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

    hci_hal_env.buffer_size = buffer_size;
    hci_hal_env.adv_free_num = 0;

    hci_hal_env.rx_q = fixed_queue_new(max_buffer_count);
    if (hci_hal_env.rx_q) {
        fixed_queue_register_dequeue(hci_hal_env.rx_q, event_uart_has_bytes);
    } else {
        HCI_TRACE_ERROR("%s unable to create rx queue.\n", __func__);
    }

    return;
}

static void hci_hal_env_deinit(void)
{
    fixed_queue_free(hci_hal_env.rx_q, osi_free_func);
    hci_hal_env.rx_q = NULL;
}

static bool hal_open(const hci_hal_callbacks_t *upper_callbacks, void *task_thread)
{
    assert(upper_callbacks != NULL);
    assert(task_thread != NULL);

    callbacks = upper_callbacks;
#if (BLE_ADV_REPORT_FLOW_CONTROL == TRUE)
    hci_hal_env_init(HCI_HAL_SERIAL_BUFFER_SIZE, BLE_ADV_REPORT_FLOW_CONTROL_NUM + L2CAP_HOST_FC_ACL_BUFS + QUEUE_SIZE_MAX); // adv flow control num + ACL flow control num + hci cmd numeber
#else
    hci_hal_env_init(HCI_HAL_SERIAL_BUFFER_SIZE, QUEUE_SIZE_MAX);
#endif

    hci_h4_thread = (osi_thread_t *)task_thread;

    //register vhci host cb
    if (esp_vhci_host_register_callback(&vhci_host_cb) != ESP_OK) {
        return false;
    }

    return true;
}

static void hal_close(void)
{
    hci_hal_env_deinit();

    hci_h4_thread = NULL;
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
        HCI_TRACE_ERROR("%s invalid data type: %d", __func__, type);
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
    fixed_queue_process(hci_hal_env.rx_q);
}

bool hci_hal_h4_task_post(uint32_t timeout)
{
    return osi_thread_post(hci_h4_thread, hci_hal_h4_rx_handler, NULL, 1, timeout);
}

#if (C2H_FLOW_CONTROL_INCLUDED == TRUE)
static void hci_packet_complete(BT_HDR *packet){
    uint8_t type;
    uint16_t handle;
    uint16_t num_packets = 1;
    uint8_t *stream = packet->data + packet->offset;

    STREAM_TO_UINT8(type, stream);
    if (type == DATA_TYPE_ACL/* || type == DATA_TYPE_SCO*/) {
        STREAM_TO_UINT16(handle, stream);
        handle = handle & HCI_DATA_HANDLE_MASK;
        btsnd_hcic_host_num_xmitted_pkts(1, &handle, &num_packets);
    }
}
#endif ///C2H_FLOW_CONTROL_INCLUDED == TRUE

bool host_recv_adv_packet(BT_HDR *packet)
{
    assert(packet);
    if(packet->data[0] == DATA_TYPE_EVENT && packet->data[1] == HCI_BLE_EVENT) {
        if(packet->data[3] ==  HCI_BLE_ADV_PKT_RPT_EVT 
#if (BLE_ADV_REPORT_FLOW_CONTROL == TRUE)
        || packet->data[3] ==  HCI_BLE_ADV_DISCARD_REPORT_EVT
#endif
        ) {
            return true;
        }
    }
    return false;
}

#if (BLE_ADV_REPORT_FLOW_CONTROL == TRUE)
static void hci_update_adv_report_flow_control(BT_HDR *packet)
{
    // this is adv packet
    if(host_recv_adv_packet(packet)) {
        // update adv free number
        hci_hal_env.adv_free_num ++;
        if (esp_vhci_host_check_send_available()){
            // send hci cmd
            btsnd_hcic_ble_update_adv_report_flow_control(hci_hal_env.adv_free_num);
            hci_hal_env.adv_free_num = 0;
        } else {
            //do nothing
        }
    }

}
#endif

static void hci_hal_h4_hdl_rx_packet(BT_HDR *packet)
{
    uint8_t type, hdr_size;
    uint16_t length;
    uint8_t *stream = NULL;

    if (!packet) {
        return;
    }
    stream = packet->data + packet->offset;

#if (C2H_FLOW_CONTROL_INCLUDED == TRUE)
    hci_packet_complete(packet);
#endif ///C2H_FLOW_CONTROL_INCLUDED == TRUE

    STREAM_TO_UINT8(type, stream);
    packet->offset++;
    packet->len--;
    if (type == HCI_BLE_EVENT) {
#if (!CONFIG_BT_STACK_NO_LOG)
        uint8_t len = 0;
        STREAM_TO_UINT8(len, stream);
#endif
        HCI_TRACE_ERROR("Workround stream corrupted during LE SCAN: pkt_len=%d ble_event_len=%d\n",
                  packet->len, len);
        osi_free(packet);
        return;
    }
    if (type < DATA_TYPE_ACL || type > DATA_TYPE_EVENT) {
        HCI_TRACE_ERROR("%s Unknown HCI message type. Dropping this byte 0x%x,"
                  " min %x, max %x\n", __func__, type,
                  DATA_TYPE_ACL, DATA_TYPE_EVENT);
        osi_free(packet);
        return;
    }
    hdr_size = preamble_sizes[type - 1];
    if (packet->len < hdr_size) {
        HCI_TRACE_ERROR("Wrong packet length type=%d pkt_len=%d hdr_len=%d",
                  type, packet->len, hdr_size);
        osi_free(packet);
        return;
    }
    if (type == DATA_TYPE_ACL) {
        stream += hdr_size - 2;
        STREAM_TO_UINT16(length, stream);
    } else {
        stream += hdr_size - 1;
        STREAM_TO_UINT8(length, stream);
    }

    if ((length + hdr_size) != packet->len) {
        HCI_TRACE_ERROR("Wrong packet length type=%d hdr_len=%d pd_len=%d "
                  "pkt_len=%d", type, hdr_size, length, packet->len);
        osi_free(packet);
        return;
    }

#if (BLE_ADV_REPORT_FLOW_CONTROL == TRUE)
    hci_update_adv_report_flow_control(packet);
#endif

#if SCAN_QUEUE_CONGEST_CHECK
    if(BTU_check_queue_is_congest() && host_recv_adv_packet(packet)) {
        HCI_TRACE_DEBUG("BtuQueue is congested");
        osi_free(packet);
        return;
    }
#endif
    packet->event = outbound_event_types[PACKET_TYPE_TO_INDEX(type)];
    callbacks->packet_ready(packet);
}

static void event_uart_has_bytes(fixed_queue_t *queue)
{
    BT_HDR *packet;
    while (!fixed_queue_is_empty(queue)) {
        packet = fixed_queue_dequeue(queue, FIXED_QUEUE_MAX_TIMEOUT);
        hci_hal_h4_hdl_rx_packet(packet);
    }
}

static void host_send_pkt_available_cb(void)
{
    //Controller rx cache buffer is ready for receiving new host packet
    //Just Call Host main thread task to process pending packets.
    hci_host_task_post(OSI_THREAD_MAX_TIMEOUT);
}

static int host_recv_pkt_cb(uint8_t *data, uint16_t len)
{
    //Target has packet to host, malloc new buffer for packet
    BT_HDR *pkt;
    size_t pkt_size;

    if (hci_hal_env.rx_q == NULL) {
        return 0;
    }

    pkt_size = BT_HDR_SIZE + len;
    pkt = (BT_HDR *) osi_calloc(pkt_size);

    if (!pkt) {
        HCI_TRACE_ERROR("%s couldn't aquire memory for inbound data buffer.\n", __func__);
        return -1;
    }
    pkt->offset = 0;
    pkt->len = len;
    pkt->layer_specific = 0;
    memcpy(pkt->data, data, len);
    fixed_queue_enqueue(hci_hal_env.rx_q, pkt, FIXED_QUEUE_MAX_TIMEOUT);
    hci_hal_h4_task_post(0);


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

const hci_hal_t *hci_hal_h4_get_interface(void)
{
    return &interface;
}

