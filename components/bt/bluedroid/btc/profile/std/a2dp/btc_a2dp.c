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
 *  Filename:      btc_a2dp.c
 *
 *****************************************************************************/
#include "bt_target.h"
#include "bt_trace.h"
#include "bta_api.h"
#include "bta_av_api.h"
#include "btc_av.h"
#include "btc_av_co.h"
#include "btc_a2dp.h"
#include "btc_a2dp_control.h"
#include "btc_a2dp_sink.h"
#include "btc_a2dp_source.h"


#if BTC_AV_INCLUDED

/*****************************************************************************
**
** Function        btc_a2dp_on_init
**
*******************************************************************************/
void btc_a2dp_on_init(void)
{
    //tput_mon(1, 0, 1);
}

/*****************************************************************************
**
** Function        btc_a2dp_on_idle
**
*******************************************************************************/

void btc_a2dp_on_idle(void)
{
    APPL_TRACE_EVENT("## ON A2DP IDLE ## peer_sep = %d", btc_av_get_peer_sep());
#if BTC_AV_SRC_INCLUDED
    if (btc_av_get_peer_sep() == AVDT_TSEP_SNK) {
        btc_a2dp_source_on_idle();
    }
#endif // BTC_AV_SRC_INCLUDED

    bta_av_co_init();

#if BTC_AV_SINK_INCLUDED
    if (btc_av_get_peer_sep() == AVDT_TSEP_SRC) {
        btc_a2dp_sink_on_idle();
    }
#endif // BTC_AV_SINK_INCLUDED
}

/*****************************************************************************
**
** Function        btc_a2dp_on_started
**
** Description
**
** Returns
**
*******************************************************************************/

BOOLEAN btc_a2dp_on_started(tBTA_AV_START *p_av, BOOLEAN pending_start)
{
    BOOLEAN ack = FALSE;

    APPL_TRACE_EVENT("## ON A2DP STARTED ##");
#if BTC_AV_SRC_INCLUDED
    if (p_av == NULL) {
        /* ack back a local start request */
        btc_a2dp_control_command_ack(ESP_A2D_MEDIA_CTRL_ACK_SUCCESS);
        return TRUE;
    }

    if (p_av->status == BTA_AV_SUCCESS) {
        if (p_av->suspending == FALSE) {
            if (p_av->initiator) {
                if (pending_start) {
                    btc_a2dp_control_command_ack(ESP_A2D_MEDIA_CTRL_ACK_SUCCESS);
                    ack = TRUE;
                }
            } else {
                /* we were remotely started,  make sure codec
                   is setup before datapath is started */
                btc_a2dp_source_setup_codec();
            }

            /* media task is autostarted upon a2dp audiopath connection */
        }
    } else if (pending_start) {
        btc_a2dp_control_command_ack(ESP_A2D_MEDIA_CTRL_ACK_FAILURE);
        ack = TRUE;
    }
#endif /* BTC_AV_SRC_INCLUDED */
    return ack;
}

/*****************************************************************************
**
** Function        btc_a2dp_on_stopped
**
*******************************************************************************/

void btc_a2dp_on_stopped(tBTA_AV_SUSPEND *p_av)
{
    APPL_TRACE_EVENT("## ON A2DP STOPPED ##");
#if BTC_AV_SINK_INCLUDED
    if (btc_av_get_peer_sep() == AVDT_TSEP_SRC) {
        btc_a2dp_sink_on_stopped(p_av);
        return;
    }
#endif // BTC_AV_SINK_INCLUDED

#if BTC_AV_SRC_INCLUDED
    btc_a2dp_source_on_stopped(p_av);
#endif // BTC_AV_SRC_INCLUDED
}

/*****************************************************************************
**
** Function        btc_a2dp_on_suspended
**
*******************************************************************************/
void btc_a2dp_on_suspended(tBTA_AV_SUSPEND *p_av)
{
    APPL_TRACE_EVENT("## ON A2DP SUSPENDED ##");
#if BTC_AV_SINK_INCLUDED
    if (btc_av_get_peer_sep() == AVDT_TSEP_SRC) {
        btc_a2dp_sink_on_suspended(p_av);
        return;
    }
#endif // BTC_AV_SINK_INCLUDED
#if BTC_AV_SRC_INCLUDED
    btc_a2dp_source_on_suspended(p_av);
#endif // BTC_AV_SRC_INCLUDED
}

#endif /* #if BTC_AV_INCLUDED */
