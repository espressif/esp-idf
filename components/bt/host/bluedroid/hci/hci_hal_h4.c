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
#if (BLE_ADV_REPORT_FLOW_CONTROL == TRUE)
#include "osi/mutex.h"
#include "osi/alarm.h"
#endif
#include "esp_bt.h"
#include "stack/hcimsgs.h"

#if (C2H_FLOW_CONTROL_INCLUDED == TRUE)
#include "l2c_int.h"
#endif ///C2H_FLOW_CONTROL_INCLUDED == TRUE
#include "stack/hcimsgs.h"
#include "hci_log/bt_hci_log.h"

#if CONFIG_BT_BLE_LOG_SPI_OUT_HCI_ENABLED
#include "ble_log/ble_log_spi_out.h"
#endif // CONFIG_BT_BLE_LOG_SPI_OUT_HCI_ENABLED

#if CONFIG_BT_BLE_LOG_SPI_OUT_HCI_ENABLED
#include "ble_log/ble_log_spi_out.h"
#endif // CONFIG_BT_BLE_LOG_SPI_OUT_HCI_ENABLED

#define HCI_BLE_EVENT 0x3e
#define PACKET_TYPE_TO_INBOUND_INDEX(type) ((type) - 2)
#define PACKET_TYPE_TO_INDEX(type) ((type) - 1)
#define HCI_UPSTREAM_DATA_QUEUE_IDX   (1)
#if (BLE_ADV_REPORT_FLOW_CONTROL == TRUE)
#define HCI_BLE_ADV_MIN_CREDITS_TO_RELEASE     (10)
#define HCI_ADV_FLOW_MONITOR_PERIOD_MS         (500)
#else
#define HCI_HAL_BLE_ADV_RPT_QUEUE_LEN_MAX      (200)
#endif

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
#if (BLE_ADV_REPORT_FLOW_CONTROL == TRUE)
    osi_mutex_t adv_flow_lock;
    osi_alarm_t *adv_flow_monitor;
    int adv_credits;
    int adv_credits_to_release;
    pkt_linked_item_t *adv_fc_cmd_buf;
    bool cmd_buf_in_use;
#endif
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

#if (BLE_ADV_REPORT_FLOW_CONTROL == TRUE)
static void hci_adv_flow_monitor(void *context);
static void hci_adv_flow_cmd_free_cb(pkt_linked_item_t *linked_pkt);
#endif

static bool hci_hal_env_init(const hci_hal_callbacks_t *upper_callbacks, osi_thread_t *task_thread)
{
    assert(upper_callbacks != NULL);
    assert(task_thread != NULL);

    hci_hal_env.hci_h4_thread = task_thread;
    hci_hal_env.callbacks = (hci_hal_callbacks_t *)upper_callbacks;

#if (BLE_ADV_REPORT_FLOW_CONTROL == TRUE)
    hci_hal_env.adv_fc_cmd_buf = osi_calloc(HCI_CMD_LINKED_BUF_SIZE(HCIC_PARAM_SIZE_BLE_UPDATE_ADV_FLOW_CONTROL));
    assert(hci_hal_env.adv_fc_cmd_buf != NULL);
    osi_mutex_new(&hci_hal_env.adv_flow_lock);
    osi_mutex_lock(&hci_hal_env.adv_flow_lock, OSI_MUTEX_MAX_TIMEOUT);
    hci_hal_env.adv_credits = BLE_ADV_REPORT_FLOW_CONTROL_NUM;
    hci_hal_env.adv_credits_to_release = 0;
    hci_hal_env.cmd_buf_in_use = false;
    osi_mutex_unlock(&hci_hal_env.adv_flow_lock);
    hci_hal_env.adv_flow_monitor = osi_alarm_new("adv_fc_mon", hci_adv_flow_monitor, NULL, HCI_ADV_FLOW_MONITOR_PERIOD_MS);
    assert (hci_hal_env.adv_flow_monitor != NULL);
#endif

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
    fixed_queue_t *rx_q = hci_hal_env.rx_q;
    struct pkt_queue *adv_rpt_q = hci_hal_env.adv_rpt_q;
    struct osi_event *upstream_data_ready = hci_hal_env.upstream_data_ready;

    hci_hal_env.rx_q = NULL;
    hci_hal_env.adv_rpt_q = NULL;
    hci_hal_env.upstream_data_ready = NULL;

    fixed_queue_free(rx_q, osi_free_func);

    pkt_queue_destroy(adv_rpt_q, NULL);

    osi_event_delete(upstream_data_ready);

#if (BLE_ADV_REPORT_FLOW_CONTROL == TRUE)
    hci_hal_env.cmd_buf_in_use = true;
    osi_alarm_cancel(hci_hal_env.adv_flow_monitor);
    osi_alarm_free(hci_hal_env.adv_flow_monitor);
    hci_hal_env.adv_flow_monitor = NULL;
    osi_mutex_free(&hci_hal_env.adv_flow_lock);
    osi_free(hci_hal_env.adv_fc_cmd_buf);
    hci_hal_env.adv_fc_cmd_buf = NULL;
#endif

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

#if (BT_HCI_LOG_INCLUDED == TRUE)
        bt_hci_log_record_hci_data(data[0], &data[1], length - 1);
#endif
#if (CONFIG_BT_BLE_LOG_SPI_OUT_HCI_ENABLED && !SOC_ESP_NIMBLE_CONTROLLER)
    ble_log_spi_out_write_with_ts(BLE_LOG_SPI_OUT_SOURCE_NIMBLE, data, length);
#endif // (CONFIG_BT_BLE_LOG_SPI_OUT_HCI_ENABLED && !SOC_ESP_NIMBLE_CONTROLLER)
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
        if(packet[3] ==  HCI_BLE_ADV_PKT_RPT_EVT || packet[3] == HCI_BLE_DIRECT_ADV_EVT
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
static void hci_adv_flow_monitor(void *context)
{
    hci_adv_credits_force_release(0);
}

static void hci_adv_credits_consumed(uint16_t num)
{
    osi_mutex_lock(&hci_hal_env.adv_flow_lock, OSI_MUTEX_MAX_TIMEOUT);
    assert(hci_hal_env.adv_credits >= num);
    hci_hal_env.adv_credits -= num;
    osi_mutex_unlock(&hci_hal_env.adv_flow_lock);
}

int hci_adv_credits_prep_to_release(uint16_t num)
{
    if (num == 0) {
        return hci_hal_env.adv_credits_to_release;
    }

    osi_mutex_lock(&hci_hal_env.adv_flow_lock, OSI_MUTEX_MAX_TIMEOUT);
    int credits_to_release = hci_hal_env.adv_credits_to_release + num;
    assert(hci_hal_env.adv_credits_to_release <= BLE_ADV_REPORT_FLOW_CONTROL_NUM);
    hci_hal_env.adv_credits_to_release = credits_to_release;
    osi_mutex_unlock(&hci_hal_env.adv_flow_lock);

    if (credits_to_release == num && num != 0) {
        osi_alarm_cancel(hci_hal_env.adv_flow_monitor);
        osi_alarm_set(hci_hal_env.adv_flow_monitor, HCI_ADV_FLOW_MONITOR_PERIOD_MS);
    }
    return credits_to_release;
}

static int hci_adv_credits_release(void)
{
    osi_mutex_lock(&hci_hal_env.adv_flow_lock, OSI_MUTEX_MAX_TIMEOUT);
    int credits_released = hci_hal_env.adv_credits_to_release;
    hci_hal_env.adv_credits += credits_released;
    hci_hal_env.adv_credits_to_release -= credits_released;
    assert(hci_hal_env.adv_credits <= BLE_ADV_REPORT_FLOW_CONTROL_NUM);
    assert(hci_hal_env.adv_credits_to_release >= 0);
    osi_mutex_unlock(&hci_hal_env.adv_flow_lock);

    if (hci_hal_env.adv_credits_to_release == 0) {
        osi_alarm_cancel(hci_hal_env.adv_flow_monitor);
    }
    return credits_released;
}

static int hci_adv_credits_release_rollback(uint16_t num)
{
    osi_mutex_lock(&hci_hal_env.adv_flow_lock, OSI_MUTEX_MAX_TIMEOUT);
    hci_hal_env.adv_credits -= num;
    hci_hal_env.adv_credits_to_release += num;
    assert(hci_hal_env.adv_credits >=0);
    assert(hci_hal_env.adv_credits_to_release <= BLE_ADV_REPORT_FLOW_CONTROL_NUM);
    osi_mutex_unlock(&hci_hal_env.adv_flow_lock);

    return num;
}

static void hci_adv_flow_cmd_free_cb(pkt_linked_item_t *linked_pkt)
{
    osi_mutex_lock(&hci_hal_env.adv_flow_lock, OSI_MUTEX_MAX_TIMEOUT);
    hci_hal_env.cmd_buf_in_use = false;
    osi_mutex_unlock(&hci_hal_env.adv_flow_lock);
    hci_adv_credits_try_release(0);
}

bool hci_adv_flow_try_send_command(uint16_t credits_released)
{
    bool sent = false;
    bool use_static_buffer = false;

    /* first try using static buffer, then dynamic buffer */
    if (!hci_hal_env.cmd_buf_in_use) {
        osi_mutex_lock(&hci_hal_env.adv_flow_lock, OSI_MUTEX_MAX_TIMEOUT);
        if (!hci_hal_env.cmd_buf_in_use) {
            hci_hal_env.cmd_buf_in_use = true;
            use_static_buffer = true;
        }
        osi_mutex_unlock(&hci_hal_env.adv_flow_lock);
    }

    if (use_static_buffer) {
        hci_cmd_metadata_t *metadata = (hci_cmd_metadata_t *)(hci_hal_env.adv_fc_cmd_buf->data);
        BT_HDR *static_buffer = &metadata->command;
        metadata->command_free_cb = hci_adv_flow_cmd_free_cb;
        sent = btsnd_hcic_ble_update_adv_report_flow_control(credits_released, static_buffer);
    } else {
        sent = btsnd_hcic_ble_update_adv_report_flow_control(credits_released, NULL);
    }

    return sent;
}

int hci_adv_credits_try_release(uint16_t num)
{
    int credits_released = 0;
    if (hci_adv_credits_prep_to_release(num) >= HCI_BLE_ADV_MIN_CREDITS_TO_RELEASE) {
        credits_released = hci_adv_credits_release();
        if (credits_released > 0) {
            if (!hci_adv_flow_try_send_command(credits_released)) {
                hci_adv_credits_release_rollback(credits_released);
            }
        } else {
            assert (credits_released == 0);
        }
    }
    return credits_released;
}

int hci_adv_credits_force_release(uint16_t num)
{
    hci_adv_credits_prep_to_release(num);
    int credits_released = hci_adv_credits_release();
    if (credits_released > 0) {
        if (!hci_adv_flow_try_send_command(credits_released)) {
            hci_adv_credits_release_rollback(credits_released);
        }
    }

    return credits_released;
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
        HCI_TRACE_ERROR("Workaround stream corrupted during LE SCAN: pkt_len=%d ble_event_len=%d\n",
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
        goto _discard_packet;
    }

    stream += hdr_size - 1;
    STREAM_TO_UINT8(length, stream);
    if ((length + hdr_size) != packet->len) {
        HCI_TRACE_ERROR("Wrong packet length type=%d hdr_len=%d pd_len=%d "
                  "pkt_len=%d", type, hdr_size, length, packet->len);
        goto _discard_packet;
    }

#if SCAN_QUEUE_CONGEST_CHECK
    if(BTU_check_queue_is_congest()) {
        HCI_TRACE_DEBUG("BtuQueue is congested");
        goto _discard_packet;
    }
#endif

    packet->event = outbound_event_types[PACKET_TYPE_TO_INDEX(type)];
    hci_hal_env.callbacks->adv_rpt_ready(linked_pkt);

    return;

_discard_packet:
    osi_free(linked_pkt);
#if (BLE_ADV_REPORT_FLOW_CONTROL == TRUE)
    hci_adv_credits_prep_to_release(1);
#endif
}

static void host_send_pkt_available_cb(void)
{
    //Controller rx cache buffer is ready for receiving new host packet
    //Just Call Host main thread task to process pending packets.
    hci_downstream_data_post(OSI_THREAD_MAX_TIMEOUT);
}


void bt_record_hci_data(uint8_t *data, uint16_t len)
{
#if (BT_HCI_LOG_INCLUDED == TRUE)
    if ((data[0] == DATA_TYPE_EVENT) && (data[1] == HCI_BLE_EVENT) && ((data[3] ==  HCI_BLE_ADV_PKT_RPT_EVT) || (data[3] == HCI_BLE_DIRECT_ADV_EVT)
#if (BLE_ADV_REPORT_FLOW_CONTROL == TRUE)
        || (data[3] ==  HCI_BLE_ADV_DISCARD_REPORT_EVT)
#endif // (BLE_ADV_REPORT_FLOW_CONTROL == TRUE)
 #if (BLE_50_FEATURE_SUPPORT == TRUE)
        || (data[3] == HCI_BLE_EXT_ADV_REPORT_EVT) || (data[3] == HCI_BLE_PERIOD_ADV_REPORT_EVT)
#endif // (BLE_50_FEATURE_SUPPORT == TRUE)
    )) {
        bt_hci_log_record_hci_adv(HCI_LOG_DATA_TYPE_ADV, &data[2], len - 2);
    } else {
        uint8_t data_type = ((data[0] == 2) ? HCI_LOG_DATA_TYPE_C2H_ACL : data[0]);
        bt_hci_log_record_hci_data(data_type, &data[1], len - 1);
    }
#endif // (BT_HCI_LOG_INCLUDED == TRUE)
}

static int host_recv_pkt_cb(uint8_t *data, uint16_t len)
{
#if (BT_BLE_LOG_SPI_OUT_HCI_ENABLED && !SOC_ESP_NIMBLE_CONTROLLER)
    ble_log_spi_out_write_with_ts(BLE_LOG_SPI_OUT_SOURCE_HCI_UPSTREAM, data, len);
#endif // (BT_BLE_LOG_SPI_OUT_HCI_ENABLED && !SOC_ESP_NIMBLE_CONTROLLER)
    //Target has packet to host, malloc new buffer for packet
    BT_HDR *pkt = NULL;
    pkt_linked_item_t *linked_pkt = NULL;
    size_t pkt_size;

    if (hci_hal_env.rx_q == NULL) {
        return 0;
    }

    bt_record_hci_data(data, len);

    bool is_adv_rpt = host_recv_adv_packet(data);

    if (!is_adv_rpt) {
        pkt_size = BT_HDR_SIZE + len;
        pkt = (BT_HDR *) osi_calloc(pkt_size);
        if (!pkt) {
            HCI_TRACE_ERROR("%s couldn't acquire memory for inbound data buffer.\n", __func__);
            assert(0);
        }

        pkt->offset = 0;
        pkt->len = len;
        pkt->layer_specific = 0;
        memcpy(pkt->data, data, len);
        fixed_queue_enqueue(hci_hal_env.rx_q, pkt, FIXED_QUEUE_MAX_TIMEOUT);
    } else {
#if !BLE_ADV_REPORT_FLOW_CONTROL
        // drop the packets if pkt_queue length goes beyond upper limit
        if (pkt_queue_length(hci_hal_env.adv_rpt_q) > HCI_HAL_BLE_ADV_RPT_QUEUE_LEN_MAX) {
            return 0;
        }
#endif
        pkt_size = BT_PKT_LINKED_HDR_SIZE + BT_HDR_SIZE + len;
        #if HEAP_MEMORY_DEBUG
        linked_pkt = (pkt_linked_item_t *) osi_calloc(pkt_size);
        #else
        linked_pkt = (pkt_linked_item_t *) osi_calloc_base(pkt_size);
        #endif
        if (!linked_pkt) {
#if (BLE_ADV_REPORT_FLOW_CONTROL == TRUE)
            hci_adv_credits_consumed(1);
            hci_adv_credits_prep_to_release(1);
#endif
            return 0;
        }
        pkt = (BT_HDR *)linked_pkt->data;
        pkt->offset = 0;
        pkt->len = len;
        pkt->layer_specific = 0;
        memcpy(pkt->data, data, len);
        pkt_queue_enqueue(hci_hal_env.adv_rpt_q, linked_pkt);
#if (BLE_ADV_REPORT_FLOW_CONTROL == TRUE)
        hci_adv_credits_consumed(1);
#endif
    }

    hci_upstream_data_post(OSI_THREAD_MAX_TIMEOUT);

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
