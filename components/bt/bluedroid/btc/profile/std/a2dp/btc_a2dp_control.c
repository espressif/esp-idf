// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

/*****************************************************************************
 *
 *  Filename:      btc_a2dp_control.c
 *
 *****************************************************************************/
#include "bt_target.h"
#include <string.h>
#include "bt_trace.h"
#include "bta_api.h"
#include "bta_av_api.h"
#include "btc_manage.h"
#include "btc_av.h"
#include "btc_a2dp.h"
#include "btc_a2dp_control.h"
#include "btc_a2dp_sink.h"
#include "btc_a2dp_source.h"
#include "esp_a2dp_api.h"

#if BTC_AV_INCLUDED

typedef struct {
    BOOLEAN data_channel_open;
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
    if (btc_av_get_peer_sep() == AVDT_TSEP_SNK) {
        /* Start the media task to encode SBC */
        btc_a2dp_source_start_audio_req();

        /* make sure we update any changed sbc encoder params */
        btc_a2dp_source_encoder_update();
    }
#endif
    btc_aa_ctrl_cb.data_channel_open = TRUE;
}

BOOLEAN btc_a2dp_control_get_datachnl_stat(void)
{
    return btc_aa_ctrl_cb.data_channel_open;
}

void btc_a2dp_control_set_datachnl_stat(BOOLEAN open)
{
    btc_aa_ctrl_cb.data_channel_open = open;
}

static void btc_a2dp_dispatch_datapath_evt(uint32_t dp_evt)
{
    btc_msg_t msg;
    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_A2DP;
    msg.act = BTC_AV_DATAPATH_CTRL_EVT;

    btc_av_args_t arg;
    memset(&arg, 0, sizeof(btc_av_args_t));
    arg.dp_evt = dp_evt;

    /* Switch to BTC context */
    APPL_TRACE_DEBUG("%s sig %u act %u, dp_evt %u\n", __func__, msg.sig, msg.act, arg.dp_evt);
    btc_transfer_context(&msg, &arg, sizeof(btc_av_args_t), NULL);
}

void btc_a2dp_control_media_ctrl(esp_a2d_media_ctrl_t ctrl)
{
    APPL_TRACE_DEBUG("BTC MEDIA (A2DP-DATA) EVENT %u", ctrl);

    if (btc_aa_ctrl_cb.a2dp_cmd_pending != ESP_A2D_MEDIA_CTRL_NONE) {
        APPL_TRACE_DEBUG("un-acked a2dp cmd: %u", btc_aa_ctrl_cb.a2dp_cmd_pending);
        a2dp_cmd_acknowledge(ctrl, ESP_A2D_MEDIA_CTRL_ACK_BUSY);
        return;
    }

    btc_aa_ctrl_cb.a2dp_cmd_pending = ctrl;

    switch (ctrl) {
    case ESP_A2D_MEDIA_CTRL_CHECK_SRC_RDY:
#if BTC_AV_SRC_INCLUDED
        if (btc_a2dp_source_is_task_shutting_down()) {
            btc_a2dp_control_command_ack(ESP_A2D_MEDIA_CTRL_ACK_FAILURE);
        } else if ((btc_av_stream_ready() == TRUE) ||
                   (btc_av_stream_started_ready() == TRUE)) {
            /* check whether av is ready to setup a2dp datapath */
            btc_a2dp_control_command_ack(ESP_A2D_MEDIA_CTRL_ACK_SUCCESS);
        } else {
            btc_a2dp_control_command_ack(ESP_A2D_MEDIA_CTRL_ACK_FAILURE);
        }
#else /* BTC_AV_SRC_INCLUDED */
        btc_a2dp_control_command_ack(ESP_A2D_MEDIA_CTRL_ACK_FAILURE);
#endif /* #if BTC_AV_SRC_INCLUDED */
        break;
    case ESP_A2D_MEDIA_CTRL_START:
        if (btc_av_stream_ready() == TRUE ) {
            /* post start event and wait for audio path to open */
            btc_dispatch_sm_event(BTC_AV_START_STREAM_REQ_EVT, NULL, 0);

            btc_a2dp_dispatch_datapath_evt(BTC_AV_DATAPATH_OPEN_EVT);
#if (BTC_AV_SINK_INCLUDED == TRUE)
            if (btc_av_get_peer_sep() == AVDT_TSEP_SRC) {
                btc_a2dp_control_command_ack(ESP_A2D_MEDIA_CTRL_ACK_SUCCESS);
            }
#endif
        } else if (btc_av_stream_started_ready()) {
            btc_a2dp_dispatch_datapath_evt(BTC_AV_DATAPATH_OPEN_EVT);
            btc_a2dp_control_command_ack(ESP_A2D_MEDIA_CTRL_ACK_SUCCESS);
        } else {
            btc_a2dp_control_command_ack(ESP_A2D_MEDIA_CTRL_ACK_FAILURE);
        }
        break;
    case ESP_A2D_MEDIA_CTRL_STOP:
#if BTC_AV_SRC_INCLUDED
        if (btc_av_get_peer_sep() == AVDT_TSEP_SNK && !btc_a2dp_source_is_streaming()) {
            /* we are already stopped, just ack back*/
            btc_a2dp_control_command_ack(ESP_A2D_MEDIA_CTRL_ACK_SUCCESS);
            break;
        }
#endif /* BTC_AV_SRC_INCLUDED */
        btc_dispatch_sm_event(BTC_AV_STOP_STREAM_REQ_EVT, NULL, 0);
#if (BTC_AV_SINK_INCLUDED == TRUE)
        if (btc_av_get_peer_sep() == AVDT_TSEP_SRC) {
            btc_a2dp_control_command_ack(ESP_A2D_MEDIA_CTRL_ACK_SUCCESS);
        }
#endif
        break;

    case ESP_A2D_MEDIA_CTRL_SUSPEND:
        /* local suspend */
        if (btc_av_stream_started_ready()) {
            btc_dispatch_sm_event(BTC_AV_SUSPEND_STREAM_REQ_EVT, NULL, 0);
        } else {
            /* we are not in started state; just ack back ok. This can happen if we are
               remotely suspended; clear REMOTE SUSPEND Flag */
            btc_av_clear_remote_suspend_flag();
            btc_a2dp_control_command_ack(ESP_A2D_MEDIA_CTRL_ACK_SUCCESS);
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
