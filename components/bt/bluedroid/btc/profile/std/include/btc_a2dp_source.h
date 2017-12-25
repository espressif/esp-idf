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
 *  Filename:      btc_a2dp_source.h
 *
 *******************************************************************************/

#ifndef __BTC_A2DP_SOURCE_H__
#define __BTC_A2DP_SOURCE_H__

#include <stdbool.h>
#include "bt_target.h"
#include "bta_api.h"
#include "btc_av_api.h"
#include "esp_a2dp_api.h"

#if BTC_AV_SRC_INCLUDED
/*******************************************************************************
 **  Data types
 *******************************************************************************/

/* tBTC_MEDIA_INIT_AUDIO msg structure */
typedef struct {
    BT_HDR hdr;
    UINT16 SamplingFreq; /* 16k, 32k, 44.1k or 48k*/
    UINT8 ChannelMode; /* mono, dual, stereo or joint stereo*/
    UINT8 NumOfSubBands; /* 4 or 8 */
    UINT8 NumOfBlocks; /* 4, 8, 12 or 16*/
    UINT8 AllocationMethod; /* loudness or SNR*/
    UINT16 MtuSize; /* peer mtu size */
} tBTC_MEDIA_INIT_AUDIO;

/* tBTC_MEDIA_UPDATE_AUDIO msg structure */
typedef struct {
    BT_HDR hdr;
    UINT16 MinMtuSize; /* Minimum peer mtu size */
    UINT8 MaxBitPool; /* Maximum peer bitpool */
    UINT8 MinBitPool; /* Minimum peer bitpool */
} tBTC_MEDIA_UPDATE_AUDIO;

/* tBTC_MEDIA_INIT_AUDIO_FEEDING msg structure */
typedef struct {
    BT_HDR hdr;
    tBTC_AV_FEEDING_MODE feeding_mode;
    tBTC_AV_MEDIA_FEEDINGS feeding;
} tBTC_MEDIA_INIT_AUDIO_FEEDING;

/*******************************************************************************
 **  Public functions
 *******************************************************************************/

/*******************************************************************************
 **
 ** Function         btc_a2dp_source_startup
 **
 ** Description      Initialize and startup the A2DP source module. This function
 **                  should be called by the BTC AV state machine prior to using
 **                  the module
 **
 ** Returns          TRUE is success
 **
 *******************************************************************************/
bool btc_a2dp_source_startup(void);

/*******************************************************************************
 **
 ** Function         btc_a2dp_source_shutdown
 **
 ** Description      Shutdown and cleanup the A2DP source module.
 **
 *******************************************************************************/
void btc_a2dp_source_shutdown(void);

/*******************************************************************************
 **
 ** Function         btc_a2dp_source_enc_init_req
 **
 ** Description      Request to initialize the media task encoder
 **
 ** Returns          TRUE is success
 **
 *******************************************************************************/
BOOLEAN btc_a2dp_source_enc_init_req(tBTC_MEDIA_INIT_AUDIO *p_msg);

/*******************************************************************************
 **
 ** Function         btc_a2dp_source_enc_udpate_req
 **
 ** Description      Request to update the media task encoder
 **
 ** Returns          TRUE is success
 **
 *******************************************************************************/
BOOLEAN btc_a2dp_source_enc_update_req(tBTC_MEDIA_UPDATE_AUDIO *p_msg);


/*******************************************************************************
 **
 ** Function         btc_a2dp_source_start_audio_req
 **
 ** Description      Request to start audio encoding task
 **
 ** Returns          TRUE is success
 **
 *******************************************************************************/
BOOLEAN btc_a2dp_source_start_audio_req(void);

/*******************************************************************************
 **
 ** Function         btc_a2dp_source_stop_audio_req
 **
 ** Description      Request to stop audio encoding task
 **
 ** Returns          TRUE is success
 **
 *******************************************************************************/
BOOLEAN btc_a2dp_source_stop_audio_req(void);

/*******************************************************************************
 **
 ** Function         btc_a2dp_source_tx_flush_req
 **
 ** Description      Request to flush audio encoding pipe
 **
 ** Returns          TRUE is success
 **
 *******************************************************************************/
BOOLEAN btc_a2dp_source_tx_flush_req(void);

/*******************************************************************************
 **
 ** Function         btc_a2dp_source_audio_readbuf
 **
 ** Description      Read an audio buffer from the BTC media TX queue
 **
 ** Returns          pointer on a aa buffer ready to send
 **
 *******************************************************************************/
BT_HDR *btc_a2dp_source_audio_readbuf(void);

/*******************************************************************************
 **
 ** Function         btc_a2dp_source_audio_feeding_init_req
 **
 ** Description      Request to initialize audio feeding
 **
 ** Returns          TRUE if success
 **
 *******************************************************************************/

BOOLEAN btc_a2dp_source_audio_feeding_init_req(tBTC_MEDIA_INIT_AUDIO_FEEDING *p_msg);

/*******************************************************************************
 **
 ** Function         btc_a2dp_source_is_streaming
 **
 ** Description      Check whether A2DP source is in streaming state
 **
 *******************************************************************************/
bool btc_a2dp_source_is_streaming(void);

/*******************************************************************************
 **
 ** Function         btc_a2dp_source_is_task_shutting_down
 **
 ** Description      Check whether A2DP source media task is shutting down
 **
 *******************************************************************************/
bool btc_a2dp_source_is_task_shutting_down(void);


/*******************************************************************************
 **
 ** Function         btc_a2dp_source_on_idle
 **
 ** Description      Request 'idle' request from BTC AV state machine during
 **                  initialization
 **
 *******************************************************************************/
void btc_a2dp_source_on_idle(void);

/*******************************************************************************
 **
 ** Function         btc_a2dp_source_on_stopped
 **
 ** Description      Process 'stop' request from the BTC AV state machine to stop
 **                  A2DP streaming
 **
 *******************************************************************************/
void btc_a2dp_source_on_stopped(tBTA_AV_SUSPEND *p_av);

/*******************************************************************************
 **
 ** Function         btc_a2dp_source_on_suspended
 **
 ** Description      Process 'suspend' request from the BTC AV state machine to stop
 **                  A2DP streaming
 **
 *******************************************************************************/
void btc_a2dp_source_on_suspended(tBTA_AV_SUSPEND *p_av);

/*******************************************************************************
 **
 ** Function         btc_a2dp_source_setup_codec
 **
 ** Description      initialize the encoder parameters
 **
 *******************************************************************************/
void btc_a2dp_source_setup_codec(void);

/*******************************************************************************
 **
 ** Function         btc_a2dp_source_set_tx_flush
 **
 ** Description      enable/disable discarding of transmitted frames
 **
 *******************************************************************************/
void btc_a2dp_source_set_tx_flush(BOOLEAN enable);

/*******************************************************************************
 **
 ** Function         btc_a2dp_source_encoder_update
 **
 ** Description      update changed SBC encoder parameters
 **
 *******************************************************************************/
void btc_a2dp_source_encoder_update(void);

#endif /* #if BTC_AV_SRC_INCLUDED */

#endif /* __BTC_A2DP_SOURCE_H__ */
