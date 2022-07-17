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
#include "hci/hci_hal.h"
#include "hci/hci_internals.h"
#include "hci/hci_layer.h"
#include "osi/thread.h"
#include "osi/pkt_queue.h"
#include "esp_bt.h"
#include "stack/hcimsgs.h"

#if SOC_ESP_NIMBLE_CONTROLLER
#include "nimble/ble_hci_trans.h"
#endif

#if (C2H_FLOW_CONTROL_INCLUDED == TRUE)
#include "l2c_int.h"
#endif ///C2H_FLOW_CONTROL_INCLUDED == TRUE
#include "stack/hcimsgs.h"

#define HCI_BLE_EVENT 0x3e
#define PACKET_TYPE_TO_INBOUND_INDEX(type) ((type) - 2)
#define PACKET_TYPE_TO_INDEX(type) ((type) - 1)
#define HCI_UPSTREAM_DATA_QUEUE_IDX   (1)

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
    fixed_queue_t *rx_q;
    struct pkt_queue *adv_rpt_q;
    uint16_t adv_free_num;
    hci_hal_callbacks_t *callbacks;
    osi_thread_t *hci_h4_thread;
    struct osi_event *upstream_data_ready;
} hci_hal_env_t;


static hci_hal_env_t hci_hal_env;
static const hci_hal_t interface;
static const esp_vhci_host_callback_t vhci_host_cb;

static void host_send_pkt_available_cb(void);
static int host_recv_pkt_cb(uint8_t *data, uint16_t len);
static void hci_hal_h4_hdl_rx_packet(BT_HDR *packet);
static void hci_hal_h4_hdl_rx_adv_rpt(pkt_linked_item_t *linked_pkt);
static void hci_upstream_data_handler(void *arg);
static bool hci_upstream_data_post(uint32_t timeout);

static bool hci_hal_env_init(const hci_hal_callbacks_t *upper_callbacks, osi_thread_t *task_thread)
{
    assert(upper_callbacks != NULL);
    assert(task_thread != NULL);

    hci_hal_env.hci_h4_thread = task_thread;
    hci_hal_env.callbacks = (hci_hal_callbacks_t *)upper_callbacks;
    hci_hal_env.adv_free_num = 0;

    hci_hal_env.rx_q = fixed_queue_new(QUEUE_SIZE_MAX);
    assert(hci_hal_env.rx_q != NULL);

    hci_hal_env.adv_rpt_q = pkt_queue_create();
    assert(hci_hal_env.adv_rpt_q != NULL);

    struct osi_event *event = osi_event_create(hci_upstream_data_handler, NULL);
    assert(event != NULL);
    hci_hal_env.upstream_data_ready = event;
    osi_event_bind(hci_hal_env.upstream_data_ready, hci_hal_env.hci_h4_thread, HCI_UPSTREAM_DATA_QUEUE_IDX);

    return true;
}

static void hci_hal_env_deinit(void)
{
    fixed_queue_free(hci_hal_env.rx_q, osi_free_func);
    hci_hal_env.rx_q = NULL;

    pkt_queue_destroy(hci_hal_env.adv_rpt_q, NULL);
    hci_hal_env.adv_rpt_q = NULL;

    osi_event_delete(hci_hal_env.upstream_data_ready);
    hci_hal_env.upstream_data_ready = NULL;

    hci_hal_env.hci_h4_thread = NULL;

    memset(&hci_hal_env, 0, sizeof(hci_hal_env_t));
}

static bool hal_open(const hci_hal_callbacks_t *upper_callbacks, void *task_thread)
{
    hci_hal_env_init(upper_callbacks, (osi_thread_t *)task_thread);

    //register vhci host cb
    if (esp_vhci_host_register_callback(&vhci_host_cb) != ESP_OK) {
        return false;
    }

    return true;
}

static void hal_close(void)
{
    hci_hal_env_deinit();
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
static void hci_upstream_data_handler(void *arg)
{
    fixed_queue_t *rx_q = hci_hal_env.rx_q;
    struct pkt_queue *adv_rpt_q = hci_hal_env.adv_rpt_q;
    size_t pkts_to_process;

    do {
        pkts_to_process = fixed_queue_length(rx_q);
        for (size_t i = 0; i < pkts_to_process; i++) {
            BT_HDR *packet = fixed_queue_dequeue(rx_q, 0);
            if (packet != NULL) {
                hci_hal_h4_hdl_rx_packet(packet);
            }
        }
    } while (0);

    do {
        pkts_to_process = pkt_queue_length(adv_rpt_q);
        for (size_t i = 0; i < pkts_to_process; i++) {
            pkt_linked_item_t *linked_pkt = pkt_queue_dequeue(adv_rpt_q);
            if (linked_pkt != NULL) {
                hci_hal_h4_hdl_rx_adv_rpt(linked_pkt);
            }
        }
    } while (0);

    if (!fixed_queue_is_empty(rx_q) || pkt_queue_length(adv_rpt_q) > 0) {
        hci_upstream_data_post(OSI_THREAD_MAX_TIMEOUT);
    }
}

static bool hci_upstream_data_post(uint32_t timeout)
{
    return osi_thread_post_event(hci_hal_env.upstream_data_ready, timeout);
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

bool host_recv_adv_packet(uint8_t *packet)
{
    assert(packet);
    if(packet[0] == DATA_TYPE_EVENT && packet[1] == HCI_BLE_EVENT) {
        if(packet[3] ==  HCI_BLE_ADV_PKT_RPT_EVT
#if (BLE_ADV_REPORT_FLOW_CONTROL == TRUE)
        || packet[3] ==  HCI_BLE_ADV_DISCARD_REPORT_EVT
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
    uint8_t *data = packet->data + packet->offset;
    if(host_recv_adv_packet(data)) {
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

    packet->event = outbound_event_types[PACKET_TYPE_TO_INDEX(type)];
    hci_hal_env.callbacks->packet_ready(packet);
}

static void hci_hal_h4_hdl_rx_adv_rpt(pkt_linked_item_t *linked_pkt)
{
    uint8_t type;
    uint8_t hdr_size;
    uint16_t length;
    uint8_t *stream = NULL;

    if (!linked_pkt) {
        return;
    }

    BT_HDR* packet = (BT_HDR *)linked_pkt->data;
    stream = packet->data + packet->offset;

    assert(host_recv_adv_packet(stream) == true);

    STREAM_TO_UINT8(type, stream);
    packet->offset++;
    packet->len--;
    hdr_size = preamble_sizes[type - 1];

    if (packet->len < hdr_size) {
        HCI_TRACE_ERROR("Wrong packet length type=%d pkt_len=%d hdr_len=%d",
                  type, packet->len, hdr_size);
        osi_free(linked_pkt);
        return;
    }

    stream += hdr_size - 1;
    STREAM_TO_UINT8(length, stream);
    if ((length + hdr_size) != packet->len) {
        HCI_TRACE_ERROR("Wrong packet length type=%d hdr_len=%d pd_len=%d "
                  "pkt_len=%d", type, hdr_size, length, packet->len);
        osi_free(linked_pkt);
        return;
    }

#if (BLE_ADV_REPORT_FLOW_CONTROL == TRUE)
    hci_update_adv_report_flow_control(packet);
#endif

#if SCAN_QUEUE_CONGEST_CHECK
    if(BTU_check_queue_is_congest()) {
        HCI_TRACE_DEBUG("BtuQueue is congested");
        osi_free(linked_pkt);
        return;
    }
#endif

    packet->event = outbound_event_types[PACKET_TYPE_TO_INDEX(type)];
    hci_hal_env.callbacks->adv_rpt_ready(linked_pkt);
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
    BT_HDR *pkt = NULL;
    pkt_linked_item_t *linked_pkt = NULL;
    size_t pkt_size;

    if (hci_hal_env.rx_q == NULL) {
        return 0;
    }

    bool is_adv_rpt = host_recv_adv_packet(data);

    if (!is_adv_rpt) {
        pkt_size = BT_HDR_SIZE + len;
        pkt = (BT_HDR *) osi_calloc(pkt_size);
        if (!pkt) {
            HCI_TRACE_ERROR("%s couldn't aquire memory for inbound data buffer.\n", __func__);
            assert(0);
        }

        pkt->offset = 0;
        pkt->len = len;
        pkt->layer_specific = 0;
        memcpy(pkt->data, data, len);
        fixed_queue_enqueue(hci_hal_env.rx_q, pkt, FIXED_QUEUE_MAX_TIMEOUT);
    } else {
        pkt_size = BT_PKT_LINKED_HDR_SIZE + BT_HDR_SIZE + len;
        linked_pkt = (pkt_linked_item_t *) osi_calloc(pkt_size);
        if (!linked_pkt) {
            // TODO: handle the adv report flow control exception
            return 0;
        }
        pkt = (BT_HDR *)linked_pkt->data;
        pkt->offset = 0;
        pkt->len = len;
        pkt->layer_specific = 0;
        memcpy(pkt->data, data, len);
        pkt_queue_enqueue(hci_hal_env.adv_rpt_q, linked_pkt);
    }

    hci_upstream_data_post(OSI_THREAD_MAX_TIMEOUT);

    BTTRC_DUMP_BUFFER("Recv Pkt", pkt->data, len);

    return 0;
}
#if SOC_ESP_NIMBLE_CONTROLLER

int
ble_hs_hci_rx_evt(uint8_t *hci_ev, void *arg)
{
    if(esp_bluedroid_get_status() == ESP_BLUEDROID_STATUS_UNINITIALIZED) {
	return 0;
    }
    uint8_t len = hci_ev[1] + 3;
    uint8_t *data = (uint8_t *)malloc(len);
    data[0] = 0x04;
    memcpy(&data[1], hci_ev, len - 1);
    ble_hci_trans_buf_free(hci_ev);
    host_recv_pkt_cb(data, len);
    free(data);
    return 0;
}

static void *trans_om;
void hci_trans_free_mbuf(void)
{
    os_mbuf_free_chain(trans_om);
}
int
ble_hs_rx_data(struct os_mbuf *om, void *arg)
{
    uint8_t len = om->om_len + 1;
    uint8_t *data = (uint8_t *)malloc(len);
    data[0] = 0x02;
    memcpy(&data[1], om->om_data, len - 1);
    host_recv_pkt_cb(data, len);
    trans_om = om;
    free(data);
    hci_trans_free_mbuf();
    return 0;
}

#endif
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
