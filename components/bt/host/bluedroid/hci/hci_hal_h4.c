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
#if (BLE_ADV_REPORT_FLOW_CONTROL == TRUE)
#include "osi/mutex.h"
#endif

#if (C2H_FLOW_CONTROL_INCLUDED == TRUE)
#include "l2c_int.h"
#endif ///C2H_FLOW_CONTROL_INCLUDED == TRUE
#include "stack/hcimsgs.h"

#define HCI_RECV_HEAP_MONITOR_SIZE (5120)  // 5k buffer
#define HCI_HAL_SERIAL_BUFFER_SIZE 1026
#define HCI_BLE_EVENT 0x3e
#define PACKET_TYPE_TO_INBOUND_INDEX(type) ((type) - 2)
#define PACKET_TYPE_TO_INDEX(type) ((type) - 1)
extern bool BTU_check_queue_is_congest(void);

typedef enum {
    ADV_PKT   = 0,
    OTHER_PKT = 1
} hci_recv_pkt_type_t;

typedef void (* hci_recv_fail_cb_t)(hci_recv_pkt_type_t type, uint16_t pkt_len, uint32_t free_heap_size);

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
#if (BLE_ADV_REPORT_FLOW_CONTROL == TRUE)
static osi_mutex_t adv_flow_lock;
static esp_timer_handle_t monitor_timer = NULL;
#endif
static hci_recv_fail_cb_t hci_recv_fail_cb = NULL;

static void host_send_pkt_available_cb(void);
static int host_recv_pkt_cb(uint8_t *data, uint16_t len);

static void hci_hal_h4_rx_handler(void *arg);
static void event_uart_has_bytes(fixed_queue_t *queue);

#if (BLE_ADV_REPORT_FLOW_CONTROL == TRUE)
static void monitor_timer_cb(void* arg);
#endif

esp_err_t esp_bt_register_hci_recv_fail_callback(hci_recv_fail_cb_t callback)
{
    if (callback) {
        hci_recv_fail_cb = callback;
        return ESP_OK;
    }

    return ESP_FAIL;
}

static void hci_hal_env_init(
    size_t buffer_size,
    size_t max_buffer_count)
{
#if (BLE_ADV_REPORT_FLOW_CONTROL == TRUE)
    const esp_timer_create_args_t monitor_timer_args = {
        .callback = &monitor_timer_cb,
        .name = "monitor_timer"
    };
#endif
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

#if (BLE_ADV_REPORT_FLOW_CONTROL == TRUE)
    osi_mutex_new(&adv_flow_lock);
    esp_timer_create(&monitor_timer_args, &monitor_timer);
    assert(monitor_timer);
    esp_timer_start_periodic(monitor_timer, 5000000);
#endif

    return;
}

static void hci_hal_env_deinit(void)
{
    fixed_queue_free(hci_hal_env.rx_q, osi_free_func);
    hci_hal_env.rx_q = NULL;

#if (BLE_ADV_REPORT_FLOW_CONTROL == TRUE)
    osi_mutex_free(&adv_flow_lock);
    if (monitor_timer) {
        esp_timer_stop(monitor_timer);
        esp_timer_delete(monitor_timer);
        monitor_timer = NULL;
    }
#endif
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

static void hal_close()
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
    uint8_t type, num_handle;
    uint16_t handle;
    uint16_t handles[MAX_L2CAP_LINKS + 4];
    uint16_t num_packets[MAX_L2CAP_LINKS + 4];
    uint8_t *stream = packet->data + packet->offset;
    tL2C_LCB  *p_lcb = NULL;

    STREAM_TO_UINT8(type, stream);
    if (type == DATA_TYPE_ACL/* || type == DATA_TYPE_SCO*/) {
        STREAM_TO_UINT16(handle, stream);
        handle = handle & HCI_DATA_HANDLE_MASK;
        p_lcb = l2cu_find_lcb_by_handle(handle);
        if (p_lcb) {
            p_lcb->completed_packets++;
        }
        if (esp_vhci_host_check_send_available()){
            num_handle = l2cu_find_completed_packets(handles, num_packets);
            if (num_handle > 0){
                btsnd_hcic_host_num_xmitted_pkts (num_handle, handles, num_packets);
            }
        } else {
            //Send HCI_Host_Number_of_Completed_Packets next time.
        }

    }
}
#endif ///C2H_FLOW_CONTROL_INCLUDED == TRUE

bool host_recv_adv_packet(uint8_t *data)
{
    if(data[0] == DATA_TYPE_EVENT && data[1] == HCI_BLE_EVENT) {
        if(data[3] ==  HCI_BLE_ADV_PKT_RPT_EVT || (data[3] == HCI_BLE_DIRECT_ADV_EVT)
#if (BLE_ADV_REPORT_FLOW_CONTROL == TRUE)
        || (data[3] == HCI_BLE_ADV_DISCARD_REPORT_EVT)
#endif
        ) {
            return true;
        }
    }
    return false;
}

#if (BLE_ADV_REPORT_FLOW_CONTROL == TRUE)
static void hci_update_adv_report_flow_control(bool need_add)
{
    uint16_t temp_adv_free_num;

    osi_mutex_lock(&adv_flow_lock, OSI_MUTEX_MAX_TIMEOUT);
    if (need_add) {
        // update adv free number
        hci_hal_env.adv_free_num ++;
    }

    temp_adv_free_num = hci_hal_env.adv_free_num;
    osi_mutex_unlock(&adv_flow_lock);

    if ((temp_adv_free_num > 0) && (esp_vhci_host_check_send_available())){
        // send hci cmd
        if (btsnd_hcic_ble_update_adv_report_flow_control(temp_adv_free_num)) {
            // update adv free number
            osi_mutex_lock(&adv_flow_lock, OSI_MUTEX_MAX_TIMEOUT);
            hci_hal_env.adv_free_num -= temp_adv_free_num;
            osi_mutex_unlock(&adv_flow_lock);
        } else {
            HCI_TRACE_ERROR("Send Adv Flow HCI command fail");
        }
    } else {
        //do nothing
    }
}

static void monitor_timer_cb(void* arg)
{
    hci_update_adv_report_flow_control(false);
}
#endif

static void hci_hal_h4_hdl_rx_packet(BT_HDR *packet)
{
    uint8_t type, hdr_size;
    uint16_t length;
    uint8_t *stream = packet->data + packet->offset;

    if (!packet) {
        return;
    }

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
    if (host_recv_adv_packet(packet->data)) {
        hci_update_adv_report_flow_control(true);
    }
#endif

#if SCAN_QUEUE_CONGEST_CHECK
    if(BTU_check_queue_is_congest() && host_recv_adv_packet(packet->data)) {
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

    // Determine whether it is a broadcase packet, if so, check the free heap size.
    if (host_recv_adv_packet(data)) {
        if (esp_get_free_heap_size() < HCI_RECV_HEAP_MONITOR_SIZE) {
            goto hci_recv_fail;
        }
    }

    pkt_size = BT_HDR_SIZE + len;
    pkt = (BT_HDR *) osi_calloc(pkt_size);
    if (!pkt) {
        HCI_TRACE_ERROR("%s couldn't aquire memory for inbound data buffer.\n", __func__);
        goto hci_recv_fail;
    }

    pkt->offset = 0;
    pkt->len = len;
    pkt->layer_specific = 0;
    memcpy(pkt->data, data, len);
    fixed_queue_enqueue(hci_hal_env.rx_q, pkt, FIXED_QUEUE_MAX_TIMEOUT);
    hci_hal_h4_task_post(0);


    BTTRC_DUMP_BUFFER("Recv Pkt", pkt->data, len);

    return 0;

hci_recv_fail:

    if (host_recv_adv_packet(data)) {
#if (BLE_ADV_REPORT_FLOW_CONTROL == TRUE)
        hci_update_adv_report_flow_control(true);
#endif
        if (hci_recv_fail_cb) {
            hci_recv_fail_cb(ADV_PKT, len, esp_get_free_heap_size());
        }
    } else {
        if (hci_recv_fail_cb) {
            hci_recv_fail_cb(OTHER_PKT, len, esp_get_free_heap_size());
        }
    }

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

