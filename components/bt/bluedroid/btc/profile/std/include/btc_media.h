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
 *  Filename:      btc_media.h
 *
 *  Description:   This is the audio module for the BTC system.
 *
 *******************************************************************************/

#ifndef __BTC_MEDIA_H__
#define __BTC_MEDIA_H__

#include <stdbool.h>
#include "bta_api.h"
#include "gki.h"
#include "btc_av_api.h"

#if (BTA_AV_INCLUDED == TRUE)

/*******************************************************************************
 **  Constants
 *******************************************************************************/
#define BTC_SUCCESS                         (0)
/**
 * AV (Audio Video source) Errors
 */
#define BTC_ERROR_SRV_AV_NOT_ENABLED        700     /* AV is not enabled */
#define BTC_ERROR_SRV_AV_FEEDING_NOT_SUPPORTED 701  /* Requested Feeding not supported */
#define BTC_ERROR_SRV_AV_BUSY               702     /* Another operation ongoing */
#define BTC_ERROR_SRV_AV_NOT_OPENED         703     /* No AV link opened */
#define BTC_ERROR_SRV_AV_NOT_STARTED        704     /* AV is not started */
#define BTC_ERROR_SRV_AV_CP_NOT_SUPPORTED   705     /* Content protection is not supported by all headsets */

/* Transcoding definition for TxTranscoding and RxTranscoding */
#define BTC_MEDIA_TRSCD_OFF             0
#define BTC_MEDIA_TRSCD_PCM_2_SBC       1  /* Tx */


/*******************************************************************************
 **  Data types
 *******************************************************************************/

typedef int tBTC_STATUS;

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

#if (BTA_AV_INCLUDED == TRUE)
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

typedef struct {
    BT_HDR hdr;
    UINT8 codec_info[AVDT_CODEC_SIZE];
} tBTC_MEDIA_SINK_CFG_UPDATE;
#endif

/*******************************************************************************
 **  Public functions
 *******************************************************************************/

/*******************************************************************************
 **
 ** Function         btc_av_task
 **
 ** Description
 **
 ** Returns          void
 **
 *******************************************************************************/
extern void btc_media_task(void);

/*******************************************************************************
 **
 ** Function         btc_media_task_enc_init_req
 **
 ** Description      Request to initialize the media task encoder
 **
 ** Returns          TRUE is success
 **
 *******************************************************************************/
extern BOOLEAN btc_media_task_enc_init_req(tBTC_MEDIA_INIT_AUDIO *p_msg);

/*******************************************************************************
 **
 ** Function         btc_media_task_enc_update_req
 **
 ** Description      Request to update the media task encoder
 **
 ** Returns          TRUE is success
 **
 *******************************************************************************/
#if (BTA_AV_INCLUDED == TRUE)
extern BOOLEAN btc_media_task_enc_update_req(tBTC_MEDIA_UPDATE_AUDIO *p_msg);
#endif

/*******************************************************************************
 **
 ** Function         btc_media_task_start_aa_req
 **
 ** Description      Request to start audio encoding task
 **
 ** Returns          TRUE is success
 **
 *******************************************************************************/
extern BOOLEAN btc_media_task_start_aa_req(void);

/*******************************************************************************
 **
 ** Function         btc_media_task_stop_aa_req
 **
 ** Description      Request to stop audio encoding task
 **
 ** Returns          TRUE is success
 **
 *******************************************************************************/
extern BOOLEAN btc_media_task_stop_aa_req(void);

/*******************************************************************************
 **
 ** Function         btc_media_task_aa_rx_flush_req
 **
 ** Description      Request to flush audio decoding pipe
 **
 ** Returns          TRUE is success
 **
 *******************************************************************************/
extern BOOLEAN btc_media_task_aa_rx_flush_req(void);
/*******************************************************************************
 **
 ** Function         btc_media_task_aa_tx_flush_req
 **
 ** Description      Request to flush audio encoding pipe
 **
 ** Returns          TRUE is success
 **
 *******************************************************************************/
extern BOOLEAN btc_media_task_aa_tx_flush_req(void);

/*******************************************************************************
 **
 ** Function         btc_media_aa_readbuf
 **
 ** Description      Read an audio GKI buffer from the BTC media TX queue
 **
 ** Returns          pointer on a GKI aa buffer ready to send
 **
 *******************************************************************************/
extern BT_HDR *btc_media_aa_readbuf(void);

/*******************************************************************************
 **
 ** Function         btc_media_sink_enque_buf
 **
 ** Description      This function is called by the av_co to fill A2DP Sink Queue
 **
 **
 ** Returns          size of the queue
 *******************************************************************************/
UINT8 btc_media_sink_enque_buf(BT_HDR *p_buf);



/*******************************************************************************
 **
 ** Function         btc_media_aa_writebuf
 **
 ** Description      Enqueue a Advance Audio media GKI buffer to be processed by btc media task.
 **
 ** Returns          TRUE is success
 **
 *******************************************************************************/
extern void btc_media_aa_writebuf(BT_HDR *pBuf, UINT32 timestamp, UINT16 seq_num);

/*******************************************************************************
 **
 ** Function         btc_media_av_writebuf
 **
 ** Description      Enqueue a video media GKI buffer to be processed by btc media task.
 **
 ** Returns          TRUE is success
 **
 *******************************************************************************/
extern BOOLEAN btc_media_av_writebuf(UINT8 *p_media, UINT32 media_len,
                                     UINT32 timestamp, UINT16 seq_num);

#if (BTA_AV_INCLUDED == TRUE)
/*******************************************************************************
 **
 ** Function         btc_media_task_audio_feeding_init_req
 **
 ** Description      Request to initialize audio feeding
 **
 ** Returns          TRUE is success
 **
 *******************************************************************************/

extern BOOLEAN btc_media_task_audio_feeding_init_req(tBTC_MEDIA_INIT_AUDIO_FEEDING *p_msg);
#endif

/*******************************************************************************
 **
 ** Function         dump_codec_info
 **
 ** Description      Decode and display codec_info (for debug)
 **
 ** Returns          void
 **
 *******************************************************************************/
extern void dump_codec_info(unsigned char *p_codec);

/**
 * Local adaptation helper functions between btc and media task
 */

bool btc_a2dp_start_media_task(void);
void btc_a2dp_stop_media_task(void);

void btc_a2dp_on_init(void);
void btc_a2dp_setup_codec(void);
void btc_a2dp_on_idle(void);
BOOLEAN btc_a2dp_on_started(tBTA_AV_START *p_av, BOOLEAN pending_start);
void btc_a2dp_on_stop_req(void);
void btc_a2dp_on_stopped(tBTA_AV_SUSPEND *p_av);
void btc_a2dp_on_suspend(void);
void btc_a2dp_on_suspended(tBTA_AV_SUSPEND *p_av);
void btc_a2dp_set_rx_flush(BOOLEAN enable);
void btc_media_check_iop_exceptions(UINT8 *peer_bda);
void btc_reset_decoder(UINT8 *p_av);

int btc_a2dp_get_track_frequency(UINT8 frequency);
int btc_a2dp_get_track_channel_count(UINT8 channeltype);
void btc_a2dp_set_peer_sep(UINT8 sep);
#endif  ///BTA_AV_INCLUDED == TRUE
#endif
