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

/*******************************************************************************
 *
 *  Filename:      btc_a2dp_sink.h
 *
 *******************************************************************************/

#ifndef __BTC_A2DP_SINK_H__
#define __BTC_A2DP_SINK_H__

#include <stdbool.h>
#include "bt_target.h"
#include "bta_api.h"
#include "btc_av_api.h"
#include "esp_a2dp_api.h"

#if BTC_AV_SINK_INCLUDED
/*******************************************************************************
 **  Data types
 *******************************************************************************/
typedef struct {
    BT_HDR hdr;
    UINT8 codec_info[AVDT_CODEC_SIZE];
} tBTC_MEDIA_SINK_CFG_UPDATE;

/*******************************************************************************
 **  Public functions
 *******************************************************************************/

/*******************************************************************************
 **
 ** Function         btc_a2dp_sink_startup
 **
 ** Description      Initialize and startup the A2DP sink module. This function
 **                  should be called by the BTC AV state machine prior to using
 **                  the module.
 **
 ** Returns          true if success
 **
 *******************************************************************************/
bool btc_a2dp_sink_startup(void);

/*******************************************************************************
 **
 ** Function         btc_a2dp_sink_shutdown
 **
 ** Description      Shutdown and cleanup the A2DP sink module
 **
 *******************************************************************************/
void btc_a2dp_sink_shutdown(void);

/*******************************************************************************
 **
 ** Function         btc_a2dp_sink_rx_flush_req
 **
 ** Description      Request to flush audio decoding pipe
 **
 ** Returns          TRUE if success
 **
 *******************************************************************************/
BOOLEAN btc_a2dp_sink_rx_flush_req(void);

/*******************************************************************************
 **
 ** Function         btc_a2dp_sink_enque_buf
 **
 ** Description      Enqueue a Advance Audio media buffer to be processed by btc media task.
 **
 ** Returns          size of the queue
 **
 *******************************************************************************/
UINT8 btc_a2dp_sink_enque_buf(BT_HDR *p_buf);


/*******************************************************************************
 **
 ** Function         btc_a2dp_sink_on_idle
 **
 ** Description      Process 'idle' request from the BTC AV state machine during
 **                  initialization
 **
 *******************************************************************************/
void btc_a2dp_sink_on_idle(void);

/*******************************************************************************
 **
 ** Function         btc_a2dp_sink_on_stopped
 **
 ** Description      Process 'stop' request from the BTC AV state machine to stop
 **                  A2DP streaming
 **
 *******************************************************************************/
void btc_a2dp_sink_on_stopped(tBTA_AV_SUSPEND *p_av);

/*******************************************************************************
 **
 ** Function         btc_a2dp_sink_on_suspended
 **
 ** Description      Process 'suspend' request from the BTC AV state machine to
 **                  suspend A2DP streaming
 **
 *******************************************************************************/
void btc_a2dp_sink_on_suspended(tBTA_AV_SUSPEND *p_av);

/*******************************************************************************
 **
 ** Function         btc_a2dp_sink_set_rx_flush
 **
 ** Description      enable/disabel discarding of received A2DP frames
 **
 *******************************************************************************/
void btc_a2dp_sink_set_rx_flush(BOOLEAN enable);

/*******************************************************************************
 **
 ** Function         btc_a2dp_sink_reset_decoder
 **
 ** Description      Reset decoder parameters according to configuration from remote
 **                  device
 **
 *******************************************************************************/
void btc_a2dp_sink_reset_decoder(UINT8 *p_av);

#endif /* #if BTC_AV_SINK_INCLUDED */

#endif /* __BTC_A2DP_SINK_H__ */
