/*
 * SPDX-FileCopyrightText: 2025-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "common/bt_target.h"
#include "common/bt_trace.h"
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include "osi/allocator.h"
#include "osi/fixed_queue.h"
#include "stack/a2d_api.h"
#include "bta/bta_av_api.h"
#include "bta/bta_av_ci.h"
#include "btc/btc_manage.h"
#include "btc/btc_common.h"
#include "btc_av_co.h"
#include "btc_a2dp.h"
#include "btc_a2dp_control.h"
#include "btc_a2dp_source.h"
#include "btc_av.h"
#include "esp_a2dp_api.h"
#include <assert.h>

#if (BTC_AV_SRC_INCLUDED == TRUE) && (BTC_AV_EXT_CODEC == TRUE)

#define MAX_OUTPUT_A2DP_SRC_FRAME_QUEUE_SZ     (27)

enum {
    BTC_A2DP_SOURCE_STATE_OFF = 0,
    BTC_A2DP_SOURCE_STATE_ON = 1,
};

typedef struct {
    BOOLEAN                 stream_started;
    BOOLEAN                 tx_flush;
    fixed_queue_t          *audio_tx_q;
} a2dp_source_local_param_t;

static int btc_a2dp_source_state = BTC_A2DP_SOURCE_STATE_OFF;
#if A2D_DYNAMIC_MEMORY == FALSE
static a2dp_source_local_param_t a2dp_source_local_param;
#else
static a2dp_source_local_param_t *a2dp_source_local_param_ptr;
#define a2dp_source_local_param (*a2dp_source_local_param_ptr)
#endif ///A2D_DYNAMIC_MEMORY == FALSE

static inline void btc_aa_cb_to_app(esp_a2d_cb_event_t event, esp_a2d_cb_param_t *param)
{
    esp_a2d_cb_t btc_aa_cb = (esp_a2d_cb_t)btc_profile_cb_get(BTC_PID_A2DP);
    if (btc_aa_cb) {
        btc_aa_cb(event, param);
    }
}

static void btc_a2dp_source_tx_flush(void)
{
    while (!fixed_queue_is_empty(a2dp_source_local_param.audio_tx_q)) {
        osi_free(fixed_queue_dequeue(a2dp_source_local_param.audio_tx_q, 0));
    }
}

static void btc_a2dp_source_tx_stop(void)
{
    if (a2dp_source_local_param.stream_started == TRUE) {
        a2dp_source_local_param.stream_started = FALSE;
        /* ack to command */
        btc_a2dp_control_command_ack(ESP_A2D_MEDIA_CTRL_ACK_SUCCESS);
    }

    /* flush tx queue  */
    btc_a2dp_source_tx_flush();
    a2dp_source_local_param.tx_flush = FALSE;
}

BOOLEAN btc_a2dp_source_enqueue_audio_frame(BT_HDR *p_buf)
{
    if (btc_a2dp_source_state != BTC_A2DP_SOURCE_STATE_ON) {
        APPL_TRACE_WARNING("%s source not start up", __func__);
        return FALSE;
    }

    if (a2dp_source_local_param.tx_flush) {
        APPL_TRACE_WARNING("%s try to send data when tx flush enable", __func__);
        return FALSE;
    }

    if (fixed_queue_length(a2dp_source_local_param.audio_tx_q) > MAX_OUTPUT_A2DP_SRC_FRAME_QUEUE_SZ) {
        APPL_TRACE_WARNING("%s audio tx queue overflow: %d", __func__, fixed_queue_length(a2dp_source_local_param.audio_tx_q));
        return FALSE;
    }

    fixed_queue_enqueue(a2dp_source_local_param.audio_tx_q, p_buf, FIXED_QUEUE_MAX_TIMEOUT);
    bta_av_ci_src_data_ready(BTA_AV_CHNL_AUDIO);
    return TRUE;
}

void btc_source_report_delay_value(UINT16 delay_value)
{
    esp_a2d_cb_param_t param;

    if (btc_a2dp_source_state != BTC_A2DP_SOURCE_STATE_ON){
        return;
    }

    param.a2d_report_delay_value_stat.delay_value = delay_value;

    btc_aa_cb_to_app(ESP_A2D_REPORT_SNK_DELAY_VALUE_EVT, &param);
}

BOOLEAN btc_a2dp_source_start_audio_req(void)
{
    a2dp_source_local_param.stream_started = TRUE;
    return TRUE;
}

BT_HDR *btc_a2dp_source_audio_readbuf(void)
{
    if (btc_a2dp_source_state != BTC_A2DP_SOURCE_STATE_ON || a2dp_source_local_param.tx_flush){
        return NULL;
    }
    return fixed_queue_dequeue(a2dp_source_local_param.audio_tx_q, 0);
}

void btc_a2dp_source_set_tx_flush(BOOLEAN enable)
{
    a2dp_source_local_param.tx_flush = enable;
    if (enable) {
        btc_a2dp_source_tx_flush();
    }
}

void btc_a2dp_source_on_suspended(tBTA_AV_SUSPEND *p_av)
{
    /* check for status failures */
    if (p_av->status != BTA_AV_SUCCESS) {
        if (p_av->initiator == TRUE) {
            btc_a2dp_control_command_ack(ESP_A2D_MEDIA_CTRL_ACK_FAILURE);
        }
    }

    /* stop tx, ack to cmd, flush tx queue */
    btc_a2dp_source_tx_stop();
}

void btc_a2dp_source_on_stopped(tBTA_AV_SUSPEND *p_av)
{
    /* allow using this api for other than suspend */
    if (p_av != NULL) {
        if (p_av->status != BTA_AV_SUCCESS) {
            if (p_av->initiator) {
                btc_a2dp_control_command_ack(ESP_A2D_MEDIA_CTRL_ACK_FAILURE);
            }
            return;
        }
    }

    /* stop tx, ack to cmd, flush tx queue */
    btc_a2dp_source_tx_stop();
}

void btc_a2dp_source_on_idle(void)
{
    /* stop tx, ack to cmd, flush tx queue */
    btc_a2dp_source_tx_stop();
}

bool btc_a2dp_source_is_task_shutting_down(void)
{
    /* always return false, remove this api when internal codec is remove */
    return false;
}

bool btc_a2dp_source_startup(void)
{
    if (btc_a2dp_source_state != BTC_A2DP_SOURCE_STATE_OFF) {
        APPL_TRACE_ERROR("%s A2DP source already start up", __func__);
        return false;
    }

#if A2D_DYNAMIC_MEMORY == TRUE
    if ((a2dp_source_local_param_ptr = (a2dp_source_local_param_t *)osi_malloc(sizeof(a2dp_source_local_param_t))) == NULL) {
        APPL_TRACE_ERROR("%s malloc failed!", __func__);
        return false;
    }
    memset((void *)a2dp_source_local_param_ptr, 0, sizeof(a2dp_source_local_param_t));
#endif

    btc_a2dp_source_state = BTC_A2DP_SOURCE_STATE_ON;

    a2dp_source_local_param.audio_tx_q = fixed_queue_new(QUEUE_SIZE_MAX);
    if(a2dp_source_local_param.audio_tx_q == NULL) {
        goto error_exit;
    }

    btc_a2dp_control_init();

    return true;

error_exit:;
    APPL_TRACE_ERROR("%s A2DP source start up failed", __func__);

#if A2D_DYNAMIC_MEMORY == TRUE
    osi_free(a2dp_source_local_param_ptr);
    a2dp_source_local_param_ptr = NULL;
#endif

    return false;
}

void btc_a2dp_source_shutdown(void)
{
    if (btc_a2dp_source_state != BTC_A2DP_SOURCE_STATE_ON) {
        APPL_TRACE_ERROR("%s A2DP source already shutdown", __func__);
        return;
    }
    btc_a2dp_source_state = BTC_A2DP_SOURCE_STATE_OFF;

    btc_a2dp_control_cleanup();

    fixed_queue_free(a2dp_source_local_param.audio_tx_q, osi_free_func);

    a2dp_source_local_param.audio_tx_q = NULL;
    a2dp_source_local_param.tx_flush = FALSE;
    a2dp_source_local_param.stream_started = FALSE;

#if A2D_DYNAMIC_MEMORY == TRUE
    osi_free(a2dp_source_local_param_ptr);
    a2dp_source_local_param_ptr = NULL;
#endif
}

/*******************************************************************************
 **
 ** Function         btc_a2dp_source_set_pref_mcc
 **
 ** Description
 **
 ** Returns          TRUE if the preferred config is supported, FALSE otherwise
 **
 *******************************************************************************/
BOOLEAN btc_a2dp_source_set_pref_mcc(tBTA_AV_HNDL hndl, tBTC_AV_CODEC_INFO *pref_mcc)
{
    return bta_av_co_audio_set_pref_mcc(hndl, pref_mcc);
}

#endif /* (BTC_AV_SRC_INCLUDED == TRUE) && (BTC_AV_EXT_CODEC == TRUE) */
