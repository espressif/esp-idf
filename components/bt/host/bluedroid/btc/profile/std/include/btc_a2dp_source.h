/*
 * SPDX-FileCopyrightText: 2015-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*******************************************************************************
 *
 *  Filename:      btc_a2dp_source.h
 *
 *******************************************************************************/

#ifndef __BTC_A2DP_SOURCE_H__
#define __BTC_A2DP_SOURCE_H__

#include <stdbool.h>
#include "common/bt_target.h"
#include "bta/bta_api.h"
#include "btc_av_api.h"
#include "esp_a2dp_api.h"

#if BTC_AV_SRC_INCLUDED
/*******************************************************************************
 **  Data types
 *******************************************************************************/
#if (BTC_AV_EXT_CODEC == FALSE)

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

#endif

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
 ** Function         btc_a2dp_source_set_tx_flush
 **
 ** Description      enable/disable discarding of transmitted frames
 **
 *******************************************************************************/
void btc_a2dp_source_set_tx_flush(BOOLEAN enable);

/*******************************************************************************
 **
 ** Function         btc_a2dp_source_set_pref_mcc
 **
 ** Description      set preferred codec configuration
 **
 ** Returns          TRUE if the preferred config is supported, FALSE otherwise
 **
 *******************************************************************************/
BOOLEAN btc_a2dp_source_set_pref_mcc(tBTA_AV_HNDL hndl, tBTC_AV_CODEC_INFO *pref_mcc);

#if (BTC_AV_EXT_CODEC == FALSE)

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
 ** Function         btc_a2dp_source_encoder_update
 **
 ** Description      update changed SBC encoder parameters
 **
 *******************************************************************************/
void btc_a2dp_source_encoder_update(void);

#endif

/*****************************************************************************
**
** Function        btc_source_report_delay_value
**
** Description     Report sink delay report value
**
*******************************************************************************/
void btc_source_report_delay_value(UINT16 delay_value);

#if (BTC_AV_EXT_CODEC == TRUE)

/*****************************************************************************
**
** Function        btc_a2dp_source_enqueue_audio_frame
**
** Description     Enqueue source audio frame to tx queue
**
*******************************************************************************/
BOOLEAN btc_a2dp_source_enqueue_audio_frame(BT_HDR *p_buf);

#endif

#endif /* #if BTC_AV_SRC_INCLUDED */

#endif /* __BTC_A2DP_SOURCE_H__ */
