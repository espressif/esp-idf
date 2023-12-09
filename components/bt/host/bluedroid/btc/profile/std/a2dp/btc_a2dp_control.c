/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*****************************************************************************
 *
 *  Filename:      btc_a2dp_control.c
 *
 *****************************************************************************/
#include "common/bt_target.h"
#include <string.h>
#include "common/bt_trace.h"
#include "bta/bta_api.h"
#include "bta/bta_av_api.h"
#include "btc/btc_manage.h"
#include "btc_av.h"
#include "btc_a2dp.h"
#include "btc_a2dp_control.h"
#include "btc_a2dp_sink.h"
#include "btc_a2dp_source.h"
#include "esp_a2dp_api.h"

#if BTC_AV_INCLUDED

typedef struct {
    BOOLEAN data_channel_open; /* used only by A2DP sink */
    UINT8 a2dp_cmd_pending; /* we can have max one command pending */
} tBTC_AA_CTRL_CB;

static tBTC_AA_CTRL_CB btc_aa_ctrl_cb;

static inline void btc_a2d_cb_to_app(esp_a2d_cb_event_t event, esp_a2d_cb_param_t *param)
{
    esp_a2d_cb_t btc_a2d_cb = (esp_a2d_cb_t)btc_profile_cb_get(BTC_PID_A2DP);
    if (btc_a2d_cb) {
        btc_a2d_cb(event, param);
    }
}

static inline void a2dp_cmd_acknowledge(int cmd, int status)
{
    esp_a2d_cb_param_t param;

    param.media_ctrl_stat.cmd = cmd;
    param.media_ctrl_stat.status = status;

    btc_a2d_cb_to_app(ESP_A2D_MEDIA_CTRL_ACK_EVT, &param);
}

void btc_a2dp_control_command_ack(int status)
{
    /* sanity check */
    if (btc_aa_ctrl_cb.a2dp_cmd_pending == ESP_A2D_MEDIA_CTRL_NONE) {
        APPL_TRACE_ERROR("warning : no command pending, ignore ack");
        return;
    }

    /* clear pending */
    int cmd = btc_aa_ctrl_cb.a2dp_cmd_pending;
    btc_aa_ctrl_cb.a2dp_cmd_pending = ESP_A2D_MEDIA_CTRL_NONE;

    a2dp_cmd_acknowledge(cmd, status);
}

static void btc_a2dp_datapath_open(void)
{
#if BTC_AV_SRC_INCLUDED
    if (btc_av_get_peer_sep() == AVDT_TSEP_SNK && btc_av_get_service_id() == BTA_A2DP_SOURCE_SERVICE_ID) {
        /* Start the media task to encode SBC */
        btc_a2dp_source_start_audio_req();

        /* make sure we update any changed sbc encoder params */
        btc_a2dp_source_encoder_update();
    }
#endif
#if (BTC_AV_SINK_INCLUDED == TRUE)
    btc_aa_ctrl_cb.data_channel_open = TRUE;
#endif
}

BOOLEAN btc_a2dp_control_get_datachnl_stat(void)
{
    return btc_aa_ctrl_cb.data_channel_open;
}

void btc_a2dp_control_set_datachnl_stat(BOOLEAN open)
{
    btc_aa_ctrl_cb.data_channel_open = open;
}

void btc_a2dp_control_media_ctrl(esp_a2d_media_ctrl_t ctrl)
{
    APPL_TRACE_DEBUG("BTC MEDIA (A2DP-DATA) EVENT %u", ctrl);

    if (btc_aa_ctrl_cb.a2dp_cmd_pending != ESP_A2D_MEDIA_CTRL_NONE) {
        APPL_TRACE_WARNING("un-acked a2dp cmd: %u", btc_aa_ctrl_cb.a2dp_cmd_pending);
        a2dp_cmd_acknowledge(ctrl, ESP_A2D_MEDIA_CTRL_ACK_BUSY);
        return;
    }

    btc_aa_ctrl_cb.a2dp_cmd_pending = ctrl;

    switch (ctrl) {
    case ESP_A2D_MEDIA_CTRL_CHECK_SRC_RDY:
#if BTC_AV_SRC_INCLUDED
        if (btc_av_get_service_id() == BTA_A2DP_SOURCE_SERVICE_ID) {
            if (btc_a2dp_source_is_task_shutting_down()) {
                btc_a2dp_control_command_ack(ESP_A2D_MEDIA_CTRL_ACK_FAILURE);
            } else if ((btc_av_stream_ready() == TRUE) ||
                    (btc_av_stream_started_ready() == TRUE)) {
                /* check whether av is ready to setup a2dp datapath */
                btc_a2dp_control_command_ack(ESP_A2D_MEDIA_CTRL_ACK_SUCCESS);
            } else {
                btc_a2dp_control_command_ack(ESP_A2D_MEDIA_CTRL_ACK_FAILURE);
            }
        } else {
            btc_a2dp_control_command_ack(ESP_A2D_MEDIA_CTRL_ACK_FAILURE);
        }
#else /* BTC_AV_SRC_INCLUDED */
        btc_a2dp_control_command_ack(ESP_A2D_MEDIA_CTRL_ACK_FAILURE);
#endif /* #if BTC_AV_SRC_INCLUDED */
        break;
    case ESP_A2D_MEDIA_CTRL_START:
        if (btc_av_stream_ready() == TRUE ) {
            /* post start event */
            btc_dispatch_sm_event(BTC_AV_START_STREAM_REQ_EVT, NULL, 0);
#if (BTC_AV_SINK_INCLUDED == TRUE)
            if (btc_av_get_peer_sep() == AVDT_TSEP_SRC && btc_av_get_service_id() == BTA_A2DP_SINK_SERVICE_ID) {
                btc_a2dp_control_command_ack(ESP_A2D_MEDIA_CTRL_ACK_SUCCESS);
            }
#endif
        } else if (btc_av_stream_started_ready()) {
            btc_a2dp_control_command_ack(ESP_A2D_MEDIA_CTRL_ACK_SUCCESS);
        } else {
            btc_a2dp_control_command_ack(ESP_A2D_MEDIA_CTRL_ACK_FAILURE);
        }
        break;
    case ESP_A2D_MEDIA_CTRL_SUSPEND:
        /* local suspend */
        if (btc_av_stream_started_ready()) {
            btc_dispatch_sm_event(BTC_AV_SUSPEND_STREAM_REQ_EVT, NULL, 0);
#if (BTC_AV_SINK_INCLUDED == TRUE)
            if (btc_av_get_peer_sep() == AVDT_TSEP_SRC && btc_av_get_service_id() == BTA_A2DP_SINK_SERVICE_ID) {
                btc_a2dp_control_command_ack(ESP_A2D_MEDIA_CTRL_ACK_SUCCESS);
            }
#endif
        } else if (btc_av_is_connected() == TRUE) {
            /* we are not in started state; just ack back ok. This can happen if we are
               remotely suspended; clear REMOTE SUSPEND Flag */
            btc_av_clear_remote_suspend_flag();
            btc_a2dp_control_command_ack(ESP_A2D_MEDIA_CTRL_ACK_SUCCESS);
        } else {
            btc_a2dp_control_command_ack(ESP_A2D_MEDIA_CTRL_ACK_FAILURE);
        }
        break;
    default :
        APPL_TRACE_ERROR("### A2DP-MEDIA EVENT %u NOT HANDLED ###", ctrl);
        btc_a2dp_control_command_ack(ESP_A2D_MEDIA_CTRL_ACK_FAILURE);
        break;
    }
}

void btc_a2dp_control_datapath_ctrl(uint32_t dp_evt)
{
    switch (dp_evt) {
    case BTC_AV_DATAPATH_OPEN_EVT: {
        btc_a2dp_datapath_open();
        break;
    }
    default:
        break;
    }
    return;
}

bool btc_a2dp_control_init(void)
{
    memset(&btc_aa_ctrl_cb, 0, sizeof(tBTC_AA_CTRL_CB));
    return true;
}

void btc_a2dp_control_cleanup(void)
{
    memset(&btc_aa_ctrl_cb, 0, sizeof(tBTC_AA_CTRL_CB));
}

#endif /* #if BTC_AV_INCLUDED */
