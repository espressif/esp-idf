/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "common/bt_target.h"
#include "common/bt_trace.h"
#include <string.h>
#include <stdint.h>
#include "common/bt_defs.h"
#include "osi/allocator.h"
#include "osi/mutex.h"
#include "osi/thread.h"
#include "osi/fixed_queue.h"
#include "stack/a2d_api.h"
#include "bta/bta_av_api.h"
#include "bta/bta_av_ci.h"
#include "btc_av_co.h"
#include "btc_a2dp.h"
#include "btc_a2dp_control.h"
#include "btc_a2dp_sink.h"
#include "btc/btc_manage.h"
#include "btc_av.h"
#include "btc/btc_util.h"
#include "esp_a2dp_api.h"
#include "osi/future.h"
#include <assert.h>

#if (BTC_AV_SINK_INCLUDED == TRUE) && (BTC_AV_EXT_CODEC == TRUE)

#define MAX_OUTPUT_A2DP_SNK_FRAME_QUEUE_SZ     (25)
#define BTC_A2DP_SNK_DATA_QUEUE_IDX            (1)

enum {
    BTC_A2DP_SINK_STATE_OFF = 0,
    BTC_A2DP_SINK_STATE_ON = 1,
};

typedef struct {
    BOOLEAN             rx_flush;           /* discards any incoming data when true */
    struct osi_event   *data_ready_event;
    fixed_queue_t      *audio_rx_q;
} tBTC_A2DP_SINK_CB;

typedef struct {
    uint16_t    expected_seq_num;
    bool        seq_num_recount;
} a2dp_sink_media_pkt_seq_num_t;

typedef struct {
    tBTC_A2DP_SINK_CB               btc_aa_snk_cb;
    osi_thread_t                   *btc_aa_snk_task_hdl;
    a2dp_sink_media_pkt_seq_num_t   media_pkt_seq_num;
} a2dp_sink_local_param_t;

static int btc_a2dp_sink_state = BTC_A2DP_SINK_STATE_OFF;
static esp_a2d_sink_audio_data_cb_t bt_aa_snk_audio_data_cb = NULL;

#if A2D_DYNAMIC_MEMORY == FALSE
static a2dp_sink_local_param_t a2dp_sink_local_param;
#else
static a2dp_sink_local_param_t *a2dp_sink_local_param_ptr;
#define a2dp_sink_local_param (*a2dp_sink_local_param_ptr)
#endif ///A2D_DYNAMIC_MEMORY == FALSE

static void btc_a2dp_sink_data_ready(UNUSED_ATTR void *context);
static void btc_a2dp_sink_handle_inc_media(BT_HDR *p_msg);

void btc_a2dp_sink_reg_audio_data_cb(esp_a2d_sink_audio_data_cb_t callback)
{
    bt_aa_snk_audio_data_cb = callback;
}

static inline void btc_a2d_audio_data_cb_to_app(uint16_t conn_hdl, uint8_t *buf, uint8_t *data, uint16_t len, uint16_t number_frame, uint32_t timestamp)
{
    if (bt_aa_snk_audio_data_cb) {
        /* AVDT media packet offset is larger than sizeof(esp_a2d_audio_buff_t), it safe to do this */
        esp_a2d_audio_buff_t *audio_buff = (esp_a2d_audio_buff_t *)buf;
        audio_buff->buff_size = len;
        audio_buff->number_frame = number_frame;
        audio_buff->data_len = len;
        audio_buff->data = data;
        audio_buff->timestamp = timestamp;
        bt_aa_snk_audio_data_cb(conn_hdl, audio_buff);
    }
    else {
        osi_free(buf);
    }
}

static void btc_a2dp_sink_rx_flush(void)
{
    while (!fixed_queue_is_empty(a2dp_sink_local_param.btc_aa_snk_cb.audio_rx_q)) {
        osi_free(fixed_queue_dequeue(a2dp_sink_local_param.btc_aa_snk_cb.audio_rx_q, 0));
    }
}

bool btc_a2dp_sink_startup(void)
{
    if (btc_a2dp_sink_state != BTC_A2DP_SINK_STATE_OFF) {
        APPL_TRACE_ERROR("a2dp sink already start up");
        return false;
    }

#if A2D_DYNAMIC_MEMORY == TRUE
    if ((a2dp_sink_local_param_ptr = (a2dp_sink_local_param_t *)osi_malloc(sizeof(a2dp_sink_local_param_t))) == NULL) {
        APPL_TRACE_ERROR("%s malloc failed!", __func__);
        return false;
    }
    memset((void *)a2dp_sink_local_param_ptr, 0, sizeof(a2dp_sink_local_param_t));
#endif

    a2dp_sink_local_param.btc_aa_snk_task_hdl = btc_get_current_thread();

    struct osi_event *data_event = osi_event_create(btc_a2dp_sink_data_ready, NULL);
    assert (data_event != NULL);
    osi_event_bind(data_event, a2dp_sink_local_param.btc_aa_snk_task_hdl, BTC_A2DP_SNK_DATA_QUEUE_IDX);
    a2dp_sink_local_param.btc_aa_snk_cb.data_ready_event = data_event;
    a2dp_sink_local_param.btc_aa_snk_cb.audio_rx_q = fixed_queue_new(QUEUE_SIZE_MAX);
    btc_a2dp_sink_state = BTC_A2DP_SINK_STATE_ON;

    btc_a2dp_control_init();

    return true;
}

void btc_a2dp_sink_shutdown(void)
{
    if (btc_a2dp_sink_state != BTC_A2DP_SINK_STATE_ON) {
        APPL_TRACE_ERROR("a2dp sink already shutdown");
        return;
    }

    btc_a2dp_control_set_datachnl_stat(FALSE);

    btc_a2dp_sink_state = BTC_A2DP_SINK_STATE_OFF;

    btc_a2dp_control_cleanup();

    fixed_queue_free(a2dp_sink_local_param.btc_aa_snk_cb.audio_rx_q, osi_free_func);

    a2dp_sink_local_param.btc_aa_snk_cb.audio_rx_q = NULL;

    osi_event_delete(a2dp_sink_local_param.btc_aa_snk_cb.data_ready_event);
    a2dp_sink_local_param.btc_aa_snk_cb.data_ready_event = NULL;

    a2dp_sink_local_param.btc_aa_snk_task_hdl = NULL;

#if A2D_DYNAMIC_MEMORY == TRUE
    osi_free(a2dp_sink_local_param_ptr);
    a2dp_sink_local_param_ptr = NULL;
#endif
}

void btc_a2dp_sink_on_idle(void)
{
    a2dp_sink_local_param.btc_aa_snk_cb.rx_flush = TRUE;
    btc_a2dp_sink_rx_flush();
}

void btc_a2dp_sink_on_stopped(tBTA_AV_SUSPEND *p_av)
{
    UNUSED(p_av);

    a2dp_sink_local_param.btc_aa_snk_cb.rx_flush = TRUE;
    btc_a2dp_control_set_datachnl_stat(FALSE);
    btc_a2dp_sink_rx_flush();
}

void btc_a2dp_sink_on_suspended(tBTA_AV_SUSPEND *p_av)
{
    UNUSED(p_av);

    a2dp_sink_local_param.btc_aa_snk_cb.rx_flush = TRUE;
    btc_a2dp_sink_rx_flush();
}

void btc_a2dp_sink_set_rx_flush(BOOLEAN enable)
{
    if (enable == FALSE) {
        a2dp_sink_local_param.media_pkt_seq_num.expected_seq_num = 0x1;
        a2dp_sink_local_param.media_pkt_seq_num.seq_num_recount = true;
    }
    a2dp_sink_local_param.btc_aa_snk_cb.rx_flush = enable;
    btc_a2dp_sink_rx_flush();
}

static void btc_a2dp_sink_data_ready(UNUSED_ATTR void *context)
{
    BT_HDR *p_msg;
    int nb_of_msgs_to_process = 0;

    if (btc_a2dp_sink_state != BTC_A2DP_SINK_STATE_ON){
        return;
    }

    if (!fixed_queue_is_empty(a2dp_sink_local_param.btc_aa_snk_cb.audio_rx_q)) {
        if (a2dp_sink_local_param.btc_aa_snk_cb.rx_flush == TRUE) {
            btc_a2dp_sink_rx_flush();
            return;
        }
        nb_of_msgs_to_process = fixed_queue_length(a2dp_sink_local_param.btc_aa_snk_cb.audio_rx_q);
        APPL_TRACE_DEBUG("%s nb msgs:%d", __func__, nb_of_msgs_to_process);
        while (nb_of_msgs_to_process > 0) {
            p_msg = (BT_HDR *)fixed_queue_dequeue(a2dp_sink_local_param.btc_aa_snk_cb.audio_rx_q, 0);
            if ( p_msg == NULL ) {
                APPL_TRACE_ERROR("%s insufficient data in queue", __func__);
                break;
            }
            btc_a2dp_sink_handle_inc_media(p_msg);
            nb_of_msgs_to_process--;
        }

        if (!fixed_queue_is_empty(a2dp_sink_local_param.btc_aa_snk_cb.audio_rx_q)) {
            osi_thread_post_event(a2dp_sink_local_param.btc_aa_snk_cb.data_ready_event, OSI_THREAD_MAX_TIMEOUT);
        }
    }
}

static void btc_a2dp_sink_handle_inc_media(BT_HDR *p_msg)
{
    UINT8 *sbc_start_frame = ((UINT8 *)(p_msg + 1) + p_msg->offset + 1);
    int num_sbc_frames = (*((UINT8 *)(p_msg + 1) + p_msg->offset)) & 0x0f;
    UINT32 sbc_frame_len = p_msg->len - 1;

    if (a2dp_sink_local_param.btc_aa_snk_cb.rx_flush) {
        osi_free(p_msg);
        return;
    }

    /* ignore data if no one is listening */
    if (!btc_a2dp_control_get_datachnl_stat()) {
        osi_free(p_msg);
        return;
    }

    if (p_msg->layer_specific != a2dp_sink_local_param.media_pkt_seq_num.expected_seq_num) {
        /* Because the sequence number of some devices is not recounted */
        if (!a2dp_sink_local_param.media_pkt_seq_num.seq_num_recount ||
                a2dp_sink_local_param.media_pkt_seq_num.expected_seq_num != 0x1) {
            APPL_TRACE_WARNING("Sequence numbers error, recv:0x%x, expect:0x%x, recount:0x%x",
                                p_msg->layer_specific, a2dp_sink_local_param.media_pkt_seq_num.expected_seq_num,
                                a2dp_sink_local_param.media_pkt_seq_num.seq_num_recount);
        }
    }
    a2dp_sink_local_param.media_pkt_seq_num.expected_seq_num  = p_msg->layer_specific + 1;
    a2dp_sink_local_param.media_pkt_seq_num.seq_num_recount = false;

    APPL_TRACE_DEBUG("Number of sbc frames %d, frame_len %d\n", num_sbc_frames, sbc_frame_len);

    UINT32 timestamp = *((UINT32 *) (p_msg + 1));
    UINT16 conn_hdl = btc_a2d_conn_handle_get();
    btc_a2d_audio_data_cb_to_app(conn_hdl, (uint8_t *)p_msg, sbc_start_frame, sbc_frame_len, num_sbc_frames, timestamp);
    /* dont free p_msg here */
}

UINT8 btc_a2dp_sink_enque_buf(BT_HDR *p_pkt)
{
    if (btc_a2dp_sink_state != BTC_A2DP_SINK_STATE_ON){
        osi_free(p_pkt);
        return 0;
    }

    if (a2dp_sink_local_param.btc_aa_snk_cb.rx_flush == TRUE) {
        /* Flush enabled, do not enqueue */
        osi_free(p_pkt);
        return fixed_queue_length(a2dp_sink_local_param.btc_aa_snk_cb.audio_rx_q);
    }

    if (fixed_queue_length(a2dp_sink_local_param.btc_aa_snk_cb.audio_rx_q) >= MAX_OUTPUT_A2DP_SNK_FRAME_QUEUE_SZ) {
        osi_free(p_pkt);
        APPL_TRACE_WARNING("Pkt dropped\n");
        return fixed_queue_length(a2dp_sink_local_param.btc_aa_snk_cb.audio_rx_q);
    }

    APPL_TRACE_DEBUG("btc_a2dp_sink_enque_buf + ");

    // p_msg->num_frames_to_be_processed = (*((UINT8 *)(p_msg + 1) + p_msg->offset)) & 0x0f;
    fixed_queue_enqueue(a2dp_sink_local_param.btc_aa_snk_cb.audio_rx_q, p_pkt, FIXED_QUEUE_MAX_TIMEOUT);
    osi_thread_post_event(a2dp_sink_local_param.btc_aa_snk_cb.data_ready_event, OSI_THREAD_MAX_TIMEOUT);

    return fixed_queue_length(a2dp_sink_local_param.btc_aa_snk_cb.audio_rx_q);
}

#endif
