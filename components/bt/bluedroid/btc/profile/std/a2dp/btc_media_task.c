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

/******************************************************************************
 **
 **  Name:          btc_media_task.c
 **
 **  Description:   This is the multimedia module for the BTC system.  It
 **                 contains task implementations AV, HS and HF profiles
 **                 audio & video processing
 **
 ******************************************************************************/

#include "bt_target.h"
#include "bt_trace.h"
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include "fixed_queue.h"
#include "bta_api.h"
#include "btu.h"
#include "bta_sys.h"
#include "bta_sys_int.h"
#include "bta_av_api.h"
#include "a2d_api.h"
#include "a2d_sbc.h"
#include "a2d_int.h"
#include "bta_av_sbc.h"
#include "bta_av_ci.h"
#include "l2c_api.h"
#include "btc_av_co.h"
#include "btc_media.h"
#include "alarm.h"
#include "bt_trace.h"
#include "thread.h"
#include "bt_defs.h"
#include "btc_av.h"
#include "btc_sm.h"
#include "btc_util.h"
#include "allocator.h"
#include "bt_utils.h"
#include "esp_a2dp_api.h"
#include "mutex.h"

// #if (BTA_AV_SINK_INCLUDED == TRUE)
#include "oi_codec_sbc.h"
#include "oi_status.h"
// #endif

#if BTC_AV_INCLUDED

// #if (BTA_AV_SINK_INCLUDED == TRUE)
OI_CODEC_SBC_DECODER_CONTEXT context;
OI_UINT32 contextData[CODEC_DATA_WORDS(2, SBC_CODEC_FAST_FILTER_BUFFERS)];
OI_INT16 pcmData[15 * SBC_MAX_SAMPLES_PER_FRAME * SBC_MAX_CHANNELS];
// #endif

/*****************************************************************************
 **  Constants
 *****************************************************************************/

/* BTC media cmd event definition : BTC_MEDIA_TASK_CMD */
enum {
    BTC_MEDIA_FLUSH_AA_RX = 1,
    BTC_MEDIA_AUDIO_SINK_CFG_UPDATE,
    BTC_MEDIA_AUDIO_SINK_CLEAR_TRACK
};

enum {
    MEDIA_TASK_STATE_OFF = 0,
    MEDIA_TASK_STATE_ON = 1,
    MEDIA_TASK_STATE_SHUTTING_DOWN = 2
};

enum {
    SIG_MEDIA_TASK_INIT = 0xf0,
    SIG_MEDIA_TASK_CLEAN_UP = 0xf1,
    SIG_MEDIA_TASK_AVK_DATA_READY = 0xf2,
    SIG_MEDIA_TASK_CMD_READY = 0xf3
};

/*
 * CONGESTION COMPENSATION CTRL ::
 *
 * Thus setting controls how many buffers we will hold in media task
 * during temp link congestion. Together with the stack buffer queues
 * it controls much temporary a2dp link congestion we can
 * compensate for. It however also depends on the default run level of sinks
 * jitterbuffers. Depending on type of sink this would vary.
 * Ideally the (SRC) max tx buffer capacity should equal the sinks
 * jitterbuffer runlevel including any intermediate buffers on the way
 * towards the sinks codec.
 */

/* fixme -- define this in pcm time instead of buffer count */

/* The typical runlevel of the tx queue size is ~1 buffer
   but due to link flow control or thread preemption in lower
   layers we might need to temporarily buffer up data */

/* 5 frames is equivalent to 6.89*5*2.9 ~= 100 ms @ 44.1 khz, 20 ms mediatick */
#define MAX_OUTPUT_A2DP_FRAME_QUEUE_SZ         (5)

#define MEDIA_DATA_Q_LEN                       (1)
#define MEDIA_CTRL_Q_LEN                       (5)
#define COMBINED_MEDIA_Q_LEN                   (MEDIA_DATA_Q_LEN + MEDIA_CTRL_Q_LEN)

typedef struct {
    UINT16 num_frames_to_be_processed;
    UINT16 len;
    UINT16 offset;
    UINT16 layer_specific;
} tBT_SBC_HDR;

typedef struct {
    fixed_queue_t *RxSbcQ;
    void *av_sm_hdl;
    UINT8 peer_sep;
    UINT8 busy_level;
    BOOLEAN rx_flush; /* discards any incoming data when true */
    BOOLEAN data_channel_open;
    UINT8   channel_count;
    UINT32  sample_rate;
} tBTC_MEDIA_CB;

// #if (BTA_AV_SINK_INCLUDED == TRUE)
extern OI_STATUS OI_CODEC_SBC_DecodeFrame(OI_CODEC_SBC_DECODER_CONTEXT *context,
        const OI_BYTE **frameData,
        unsigned long *frameBytes,
        OI_INT16 *pcmData,
        unsigned long *pcmBytes);
extern OI_STATUS OI_CODEC_SBC_DecoderReset(OI_CODEC_SBC_DECODER_CONTEXT *context,
        unsigned long *decoderData,
        unsigned long decoderDataBytes,
        OI_UINT8 maxChannels,
        OI_UINT8 pcmStride,
        OI_BOOL enhanced);
// #endif

static void btc_media_flush_q(fixed_queue_t *p_q);
static void btc_media_task_aa_rx_flush(void);
static const char *dump_media_event(UINT16 event);
static void btc_media_thread_handle_cmd(fixed_queue_t *queue);

/* Handle incoming media packets A2DP SINK streaming*/
static void btc_media_task_handle_inc_media(tBT_SBC_HDR *p_msg);
static void btc_media_task_aa_handle_decoder_reset(BT_HDR *p_msg);
static void btc_media_task_aa_handle_clear_track(void);
BOOLEAN btc_media_task_clear_track(void);
static void btc_media_task_handler(void *arg);

static void btc_media_task_avk_data_ready(UNUSED_ATTR void *context);
static void btc_media_thread_init(UNUSED_ATTR void *context);
static void btc_media_thread_cleanup(UNUSED_ATTR void *context);

static tBTC_MEDIA_CB btc_media_cb;
static int media_task_running = MEDIA_TASK_STATE_OFF;
static fixed_queue_t *btc_media_cmd_msg_queue = NULL;
static xTaskHandle  xBtcMediaTaskHandle = NULL;
static QueueHandle_t xBtcMediaDataQueue = NULL;
static QueueHandle_t xBtcMediaCtrlQueue = NULL;
static QueueSetHandle_t xBtcMediaQueueSet;
static esp_a2d_data_cb_t bt_av_sink_data_callback = NULL;

void btc_a2dp_sink_reg_data_cb(esp_a2d_data_cb_t callback)
{
    // todo: critical section protection
    bt_av_sink_data_callback = callback;
}

static inline void btc_a2d_data_cb_to_app(const uint8_t *data, uint32_t len)
{
    // todo: critical section protection
    if (bt_av_sink_data_callback) {
        bt_av_sink_data_callback(data, len);
    }
}

/*****************************************************************************
 **  Misc helper functions
 *****************************************************************************/
UNUSED_ATTR static const char *dump_media_event(UINT16 event)
{
    switch (event) {
        CASE_RETURN_STR(BTC_MEDIA_FLUSH_AA_RX)
        CASE_RETURN_STR(BTC_MEDIA_AUDIO_SINK_CFG_UPDATE)
        CASE_RETURN_STR(BTC_MEDIA_AUDIO_SINK_CLEAR_TRACK)

    default:
        return "UNKNOWN MEDIA EVENT";
    }
}

/*****************************************************************************
 **  BTC ADAPTATION
 *****************************************************************************/

static void btc_media_ctrl_post(uint32_t sig)
{
    BtTaskEvt_t *evt = (BtTaskEvt_t *)osi_malloc(sizeof(BtTaskEvt_t));
    if (evt == NULL) {
        return;
    }

    evt->sig = sig;
    evt->par = 0;

    if (xQueueSend(xBtcMediaCtrlQueue, &evt, 10 / portTICK_RATE_MS) != pdTRUE) {
        APPL_TRACE_WARNING("xBtcMediaCtrlQueue failed, sig 0x%x\n", sig);
    }
}

static void btc_media_data_post(void)
{
    void *evt;
    if (xQueueSend(xBtcMediaDataQueue, &evt, 0) != pdTRUE) {
        APPL_TRACE_DEBUG("Media data Q filled\n");
    }
}

static void btc_media_ctrl_handler(BtTaskEvt_t *e)
{
    if (e == NULL) {
        return;
    }
    switch (e->sig) {
    case SIG_MEDIA_TASK_CMD_READY:
        fixed_queue_process(btc_media_cmd_msg_queue);
        break;
    case SIG_MEDIA_TASK_INIT:
        btc_media_thread_init(NULL);
        break;
    case SIG_MEDIA_TASK_CLEAN_UP:
        btc_media_thread_cleanup(NULL);
        break;
    default:
        APPL_TRACE_WARNING("media task unhandled evt: 0x%x\n", e->sig);
    }
}


static void btc_media_task_handler(void *arg)
{
    QueueSetMemberHandle_t xActivatedMember;
    BtTaskEvt_t *e = NULL;
    for (;;) {
        xActivatedMember = xQueueSelectFromSet(xBtcMediaQueueSet, portMAX_DELAY);
        if (xActivatedMember == xBtcMediaDataQueue) {
            xQueueReceive(xActivatedMember, &e, 0);
            btc_media_task_avk_data_ready(NULL);
        } else if (xActivatedMember == xBtcMediaCtrlQueue) {
            xQueueReceive(xActivatedMember, &e, 0);
            btc_media_ctrl_handler(e);
            osi_free(e);
        }
    }
}

bool btc_a2dp_start_media_task(void)
{
    if (media_task_running != MEDIA_TASK_STATE_OFF) {
        APPL_TRACE_ERROR("warning : media task already running");
        return false;
    }

    APPL_TRACE_EVENT("## A2DP START MEDIA THREAD ##");

    xBtcMediaQueueSet = xQueueCreateSet(COMBINED_MEDIA_Q_LEN);
    configASSERT(xBtcMediaQueueSet);
    xBtcMediaDataQueue = xQueueCreate(MEDIA_DATA_Q_LEN, sizeof(void *));
    configASSERT(xBtcMediaDataQueue);
    xQueueAddToSet(xBtcMediaDataQueue, xBtcMediaQueueSet);

    xBtcMediaCtrlQueue = xQueueCreate(MEDIA_CTRL_Q_LEN, sizeof(void *));
    configASSERT(xBtcMediaCtrlQueue);
    xQueueAddToSet(xBtcMediaCtrlQueue, xBtcMediaQueueSet);

    if (!xBtcMediaDataQueue || !xBtcMediaCtrlQueue || !xBtcMediaQueueSet ) {
        goto error_exit;
    }

    xTaskCreatePinnedToCore(btc_media_task_handler, "BtcMediaT\n", 2048, NULL, configMAX_PRIORITIES - 3, &xBtcMediaTaskHandle, 0);
    if (xBtcMediaTaskHandle == NULL) {
        goto error_exit;
    }

    btc_media_cmd_msg_queue = fixed_queue_new(SIZE_MAX);
    if (btc_media_cmd_msg_queue == NULL) {
        goto error_exit;
    }
    fixed_queue_register_dequeue(btc_media_cmd_msg_queue, btc_media_thread_handle_cmd);
    btc_media_ctrl_post(SIG_MEDIA_TASK_INIT);

    APPL_TRACE_EVENT("## A2DP MEDIA THREAD STARTED ##\n");

    return true;

error_exit:;
    APPL_TRACE_ERROR("%s unable to start up media thread\n", __func__);

    if (xBtcMediaTaskHandle != NULL) {
        vTaskDelete(xBtcMediaTaskHandle);
        xBtcMediaTaskHandle = NULL;
    }

    if (xBtcMediaDataQueue) {
        vQueueDelete(xBtcMediaDataQueue);
        xBtcMediaDataQueue = NULL;
    }
    if (xBtcMediaCtrlQueue) {
        vQueueDelete(xBtcMediaCtrlQueue);
        xBtcMediaCtrlQueue = NULL;
    }

    fixed_queue_free(btc_media_cmd_msg_queue, NULL);
    btc_media_cmd_msg_queue = NULL;
    return false;
}

void btc_a2dp_stop_media_task(void)
{
    APPL_TRACE_EVENT("## A2DP STOP MEDIA THREAD ##\n");

    // Exit thread
    btc_media_ctrl_post(SIG_MEDIA_TASK_CLEAN_UP);
    // TODO: wait until CLEAN up is done, then do task delete
    vTaskDelete(xBtcMediaTaskHandle);
    xBtcMediaTaskHandle = NULL;

    vQueueDelete(xBtcMediaDataQueue);
    xBtcMediaDataQueue = NULL;

    vQueueDelete(xBtcMediaCtrlQueue);
    xBtcMediaCtrlQueue = NULL;

    fixed_queue_free(btc_media_cmd_msg_queue, NULL);
    btc_media_cmd_msg_queue = NULL;
}

/*****************************************************************************
**
** Function        btc_a2dp_on_init
**
** Description
**
** Returns
**
*******************************************************************************/
void btc_a2dp_on_init(void)
{
    //tput_mon(1, 0, 1);
}

/*****************************************************************************
**
** Function        btc_a2dp_setup_codec
**
** Description
**
** Returns
**
*******************************************************************************/

void btc_a2dp_setup_codec(void)
{
    tBTC_AV_MEDIA_FEEDINGS media_feeding;
    tBTC_STATUS status;

    APPL_TRACE_EVENT("## A2DP SETUP CODEC ##\n");

    osi_mutex_global_lock();

    /* for now hardcode 44.1 khz 16 bit stereo PCM format */
    media_feeding.cfg.pcm.sampling_freq = 44100;
    media_feeding.cfg.pcm.bit_per_sample = 16;
    media_feeding.cfg.pcm.num_channel = 2;
    media_feeding.format = BTC_AV_CODEC_PCM;

    bta_av_co_audio_set_codec(&media_feeding, &status);


    osi_mutex_global_unlock();
}

/*****************************************************************************
**
** Function        btc_a2dp_on_idle
**
** Description
**
** Returns
**
*******************************************************************************/

void btc_a2dp_on_idle(void)
{
    APPL_TRACE_EVENT("## ON A2DP IDLE ##\n");

    bta_av_co_init();
    if (btc_media_cb.peer_sep == AVDT_TSEP_SRC) {
        btc_media_cb.rx_flush = TRUE;
        btc_media_task_aa_rx_flush_req();
        btc_media_task_clear_track();
        APPL_TRACE_DEBUG("Stopped BT track");
    }
}

/*******************************************************************************
 **
 ** Function         btc_media_task_clear_track
 **
 ** Description
 **
 ** Returns          TRUE is success
 **
 *******************************************************************************/
BOOLEAN btc_media_task_clear_track(void)
{
    BT_HDR *p_buf;

    if (NULL == (p_buf = osi_malloc(sizeof(BT_HDR)))) {
        return FALSE;
    }

    p_buf->event = BTC_MEDIA_AUDIO_SINK_CLEAR_TRACK;

    fixed_queue_enqueue(btc_media_cmd_msg_queue, p_buf);
    btc_media_ctrl_post(SIG_MEDIA_TASK_CMD_READY);
    return TRUE;
}

/*****************************************************************************
**
** Function        btc_reset_decoder
**
** Description
**
** Returns
**
*******************************************************************************/

void btc_reset_decoder(UINT8 *p_av)
{
    APPL_TRACE_EVENT("btc_reset_decoder");
    APPL_TRACE_DEBUG("btc_reset_decoder p_codec_info[%x:%x:%x:%x:%x:%x]\n",
                     p_av[1], p_av[2], p_av[3],
                     p_av[4], p_av[5], p_av[6]);

    tBTC_MEDIA_SINK_CFG_UPDATE *p_buf;
    if (NULL == (p_buf = osi_malloc(sizeof(tBTC_MEDIA_SINK_CFG_UPDATE)))) {
        APPL_TRACE_ERROR("btc_reset_decoder No Buffer ");
        return;
    }

    memcpy(p_buf->codec_info, p_av, AVDT_CODEC_SIZE);
    p_buf->hdr.event = BTC_MEDIA_AUDIO_SINK_CFG_UPDATE;

    fixed_queue_enqueue(btc_media_cmd_msg_queue, p_buf);
    btc_media_ctrl_post(SIG_MEDIA_TASK_CMD_READY);
}

/*****************************************************************************
**
** Function        btc_a2dp_on_stopped
**
** Description
**
** Returns
**
*******************************************************************************/

void btc_a2dp_on_stopped(tBTA_AV_SUSPEND *p_av)
{
    APPL_TRACE_EVENT("## ON A2DP STOPPED ##\n");
    if (btc_media_cb.peer_sep == AVDT_TSEP_SRC) { /*  Handling for A2DP SINK cases*/
        btc_media_cb.rx_flush = TRUE;
        btc_media_task_aa_rx_flush_req();
        btc_media_cb.data_channel_open = FALSE;
        return;
    }
}

/*****************************************************************************
**
** Function        btc_a2dp_on_suspended
**
** Description
**
** Returns
**
*******************************************************************************/

void btc_a2dp_on_suspended(tBTA_AV_SUSPEND *p_av)
{
    APPL_TRACE_EVENT("## ON A2DP SUSPENDED ##\n");
    if (btc_media_cb.peer_sep == AVDT_TSEP_SRC) {
        btc_media_cb.rx_flush = TRUE;
        btc_media_task_aa_rx_flush_req();
        return;
    }
}

/* when true media task discards any rx frames */
void btc_a2dp_set_rx_flush(BOOLEAN enable)
{
    APPL_TRACE_EVENT("## DROP RX %d ##\n", enable);
    btc_media_cb.rx_flush = enable;
}

static void btc_media_task_avk_data_ready(UNUSED_ATTR void *context)
{
    tBT_SBC_HDR *p_msg;

    if (fixed_queue_is_empty(btc_media_cb.RxSbcQ)) {
        APPL_TRACE_DEBUG("  QUE  EMPTY ");
    } else {
        if (btc_media_cb.rx_flush == TRUE) {
            btc_media_flush_q(btc_media_cb.RxSbcQ);
            return;
        }

        while ((p_msg = (tBT_SBC_HDR *)fixed_queue_try_peek_first(btc_media_cb.RxSbcQ)) != NULL ) {
            btc_media_task_handle_inc_media(p_msg);
            p_msg = (tBT_SBC_HDR *)fixed_queue_try_dequeue(btc_media_cb.RxSbcQ);
            if ( p_msg == NULL ) {
                APPL_TRACE_ERROR("Insufficient data in que ");
                break;
            }
            osi_free(p_msg);
        }
        APPL_TRACE_DEBUG(" Process Frames - ");
    }
}

static void btc_media_thread_init(UNUSED_ATTR void *context)
{
    APPL_TRACE_EVENT("%s\n", __func__);
    memset(&btc_media_cb, 0, sizeof(btc_media_cb));
    btc_media_cb.av_sm_hdl = btc_av_get_sm_handle();
    raise_priority_a2dp(TASK_HIGH_MEDIA);
    media_task_running = MEDIA_TASK_STATE_ON;

    btc_media_cb.RxSbcQ = fixed_queue_new(SIZE_MAX);
}

static void btc_media_thread_cleanup(UNUSED_ATTR void *context)
{
    /* make sure no channels are restarted while shutting down */
    media_task_running = MEDIA_TASK_STATE_SHUTTING_DOWN;

    btc_media_cb.data_channel_open = FALSE;
    /* Clear media task flag */
    media_task_running = MEDIA_TASK_STATE_OFF;

    fixed_queue_free(btc_media_cb.RxSbcQ, osi_free_func);
}

/*******************************************************************************
 **
 ** Function         btc_media_flush_q
 **
 ** Description
 **
 ** Returns          void
 **
 *******************************************************************************/
static void btc_media_flush_q(fixed_queue_t *p_q)
{
    while (! fixed_queue_is_empty(p_q)) {
	    osi_free(fixed_queue_try_dequeue(p_q));
    }
}

static void btc_media_thread_handle_cmd(fixed_queue_t *queue)
{
    BT_HDR *p_msg;
    while (!fixed_queue_is_empty(queue)) {
        p_msg = (BT_HDR *)fixed_queue_dequeue(queue);
        APPL_TRACE_VERBOSE("btc_media_thread_handle_cmd : %d %s\n", p_msg->event,
                           dump_media_event(p_msg->event));

        switch (p_msg->event) {
        case BTC_MEDIA_AUDIO_SINK_CFG_UPDATE:
            btc_media_task_aa_handle_decoder_reset(p_msg);
            break;
        case BTC_MEDIA_AUDIO_SINK_CLEAR_TRACK:
            btc_media_task_aa_handle_clear_track();
            break;
        case BTC_MEDIA_FLUSH_AA_RX:
            btc_media_task_aa_rx_flush();
            break;
        default:
            APPL_TRACE_ERROR("ERROR in %s unknown event %d\n", __func__, p_msg->event);
        }
        osi_free(p_msg);
        APPL_TRACE_VERBOSE("%s: %s DONE\n", __func__, dump_media_event(p_msg->event));
    }
}

/*******************************************************************************
 **
 ** Function         btc_media_task_handle_inc_media
 **
 ** Description
 **
 ** Returns          void
 **
 *******************************************************************************/
static void btc_media_task_handle_inc_media(tBT_SBC_HDR *p_msg)
{
    UINT8 *sbc_start_frame = ((UINT8 *)(p_msg + 1) + p_msg->offset + 1);
    int count;
    UINT32 pcmBytes, availPcmBytes;
    OI_INT16 *pcmDataPointer = pcmData; /*Will be overwritten on next packet receipt*/
    OI_STATUS status;
    int num_sbc_frames = p_msg->num_frames_to_be_processed;
    UINT32 sbc_frame_len = p_msg->len - 1;
    availPcmBytes = 2 * sizeof(pcmData);

    if ((btc_media_cb.peer_sep == AVDT_TSEP_SNK) || (btc_media_cb.rx_flush)) {
        APPL_TRACE_DEBUG(" State Changed happened in this tick ");
        return;
    }

    // ignore data if no one is listening
    if (!btc_media_cb.data_channel_open) {
        return;
    }

    APPL_TRACE_DEBUG("Number of sbc frames %d, frame_len %d\n", num_sbc_frames, sbc_frame_len);

    for (count = 0; count < num_sbc_frames && sbc_frame_len != 0; count ++) {
        pcmBytes = availPcmBytes;
        status = OI_CODEC_SBC_DecodeFrame(&context, (const OI_BYTE **)&sbc_start_frame,
                                          (OI_UINT32 *)&sbc_frame_len,
                                          (OI_INT16 *)pcmDataPointer,
                                          (OI_UINT32 *)&pcmBytes);
        if (!OI_SUCCESS(status)) {
            APPL_TRACE_ERROR("Decoding failure: %d\n", status);
            break;
        }
        availPcmBytes -= pcmBytes;
        pcmDataPointer += pcmBytes / 2;
        p_msg->offset += (p_msg->len - 1) - sbc_frame_len;
        p_msg->len = sbc_frame_len + 1;
    }

    btc_a2d_data_cb_to_app((uint8_t *)pcmData, (2 * sizeof(pcmData) - availPcmBytes));
}

/*******************************************************************************
 **
 ** Function         btc_media_task_aa_rx_flush_req
 **
 ** Description
 **
 ** Returns          TRUE is success
 **
 *******************************************************************************/
BOOLEAN btc_media_task_aa_rx_flush_req(void)
{
    BT_HDR *p_buf;

    if (fixed_queue_is_empty(btc_media_cb.RxSbcQ) == TRUE) { /*  Que is already empty */
        return TRUE;
    }

    if (NULL == (p_buf = osi_malloc(sizeof(BT_HDR)))) {
        return FALSE;
    }

    p_buf->event = BTC_MEDIA_FLUSH_AA_RX;

    fixed_queue_enqueue(btc_media_cmd_msg_queue, p_buf);
    btc_media_ctrl_post(SIG_MEDIA_TASK_CMD_READY);
    return TRUE;
}

/*******************************************************************************
 **
 ** Function         btc_media_task_aa_rx_flush
 **
 ** Description
 **
 ** Returns          void
 **
 *******************************************************************************/
static void btc_media_task_aa_rx_flush(void)
{
    /* Flush all enqueued SBC  buffers (encoded) */
    APPL_TRACE_DEBUG("btc_media_task_aa_rx_flush");

    btc_media_flush_q(btc_media_cb.RxSbcQ);
}

int btc_a2dp_get_track_frequency(UINT8 frequency)
{
    int freq = 48000;
    switch (frequency) {
    case A2D_SBC_IE_SAMP_FREQ_16:
        freq = 16000;
        break;
    case A2D_SBC_IE_SAMP_FREQ_32:
        freq = 32000;
        break;
    case A2D_SBC_IE_SAMP_FREQ_44:
        freq = 44100;
        break;
    case A2D_SBC_IE_SAMP_FREQ_48:
        freq = 48000;
        break;
    }
    return freq;
}

int btc_a2dp_get_track_channel_count(UINT8 channeltype)
{
    int count = 1;
    switch (channeltype) {
    case A2D_SBC_IE_CH_MD_MONO:
        count = 1;
        break;
    case A2D_SBC_IE_CH_MD_DUAL:
    case A2D_SBC_IE_CH_MD_STEREO:
    case A2D_SBC_IE_CH_MD_JOINT:
        count = 2;
        break;
    }
    return count;
}

void btc_a2dp_set_peer_sep(UINT8 sep)
{
    btc_media_cb.peer_sep = sep;
}

static void btc_media_task_aa_handle_clear_track (void)
{
    APPL_TRACE_DEBUG("btc_media_task_aa_handle_clear_track");
}

/*******************************************************************************
 **
 ** Function         btc_media_task_aa_handle_decoder_reset
 **
 ** Description
 **
 ** Returns          void
 **
 *******************************************************************************/
static void btc_media_task_aa_handle_decoder_reset(BT_HDR *p_msg)
{
    tBTC_MEDIA_SINK_CFG_UPDATE *p_buf = (tBTC_MEDIA_SINK_CFG_UPDATE *) p_msg;
    tA2D_STATUS a2d_status;
    tA2D_SBC_CIE sbc_cie;
    OI_STATUS       status;
    UINT32          freq_multiple = 48 * 20; /* frequency multiple for 20ms of data , initialize with 48K*/
    UINT32          num_blocks = 16;
    UINT32          num_subbands = 8;

    APPL_TRACE_EVENT("btc_media_task_aa_handle_decoder_reset p_codec_info[%x:%x:%x:%x:%x:%x]\n",
                     p_buf->codec_info[1], p_buf->codec_info[2], p_buf->codec_info[3],
                     p_buf->codec_info[4], p_buf->codec_info[5], p_buf->codec_info[6]);

    a2d_status = A2D_ParsSbcInfo(&sbc_cie, p_buf->codec_info, FALSE);
    if (a2d_status != A2D_SUCCESS) {
        APPL_TRACE_ERROR("ERROR dump_codec_info A2D_ParsSbcInfo fail:%d\n", a2d_status);
        return;
    }

    btc_media_cb.sample_rate = btc_a2dp_get_track_frequency(sbc_cie.samp_freq);
    btc_media_cb.channel_count = btc_a2dp_get_track_channel_count(sbc_cie.ch_mode);

    btc_media_cb.rx_flush = FALSE;
    APPL_TRACE_EVENT("Reset to sink role");
    status = OI_CODEC_SBC_DecoderReset(&context, contextData, sizeof(contextData), 2, 2, FALSE);
    if (!OI_SUCCESS(status)) {
        APPL_TRACE_ERROR("OI_CODEC_SBC_DecoderReset failed with error code %d\n", status);
    }

    btc_media_cb.data_channel_open = TRUE;

    switch (sbc_cie.samp_freq) {
    case A2D_SBC_IE_SAMP_FREQ_16:
        APPL_TRACE_DEBUG("\tsamp_freq:%d (16000)\n", sbc_cie.samp_freq);
        freq_multiple = 16 * 20;
        break;
    case A2D_SBC_IE_SAMP_FREQ_32:
        APPL_TRACE_DEBUG("\tsamp_freq:%d (32000)\n", sbc_cie.samp_freq);
        freq_multiple = 32 * 20;
        break;
    case A2D_SBC_IE_SAMP_FREQ_44:
        APPL_TRACE_DEBUG("\tsamp_freq:%d (44100)\n", sbc_cie.samp_freq);
        freq_multiple = 441 * 2;
        break;
    case A2D_SBC_IE_SAMP_FREQ_48:
        APPL_TRACE_DEBUG("\tsamp_freq:%d (48000)\n", sbc_cie.samp_freq);
        freq_multiple = 48 * 20;
        break;
    default:
        APPL_TRACE_DEBUG(" Unknown Frequency ");
        break;
    }

    switch (sbc_cie.ch_mode) {
    case A2D_SBC_IE_CH_MD_MONO:
        APPL_TRACE_DEBUG("\tch_mode:%d (Mono)\n", sbc_cie.ch_mode);
        break;
    case A2D_SBC_IE_CH_MD_DUAL:
        APPL_TRACE_DEBUG("\tch_mode:%d (DUAL)\n", sbc_cie.ch_mode);
        break;
    case A2D_SBC_IE_CH_MD_STEREO:
        APPL_TRACE_DEBUG("\tch_mode:%d (STEREO)\n", sbc_cie.ch_mode);
        break;
    case A2D_SBC_IE_CH_MD_JOINT:
        APPL_TRACE_DEBUG("\tch_mode:%d (JOINT)\n", sbc_cie.ch_mode);
        break;
    default:
        APPL_TRACE_DEBUG(" Unknown Mode ");
        break;
    }

    switch (sbc_cie.block_len) {
    case A2D_SBC_IE_BLOCKS_4:
        APPL_TRACE_DEBUG("\tblock_len:%d (4)\n", sbc_cie.block_len);
        num_blocks = 4;
        break;
    case A2D_SBC_IE_BLOCKS_8:
        APPL_TRACE_DEBUG("\tblock_len:%d (8)\n", sbc_cie.block_len);
        num_blocks = 8;
        break;
    case A2D_SBC_IE_BLOCKS_12:
        APPL_TRACE_DEBUG("\tblock_len:%d (12)\n", sbc_cie.block_len);
        num_blocks = 12;
        break;
    case A2D_SBC_IE_BLOCKS_16:
        APPL_TRACE_DEBUG("\tblock_len:%d (16)\n", sbc_cie.block_len);
        num_blocks = 16;
        break;
    default:
        APPL_TRACE_DEBUG(" Unknown BlockLen ");
        break;
    }

    switch (sbc_cie.num_subbands) {
    case A2D_SBC_IE_SUBBAND_4:
        APPL_TRACE_DEBUG("\tnum_subbands:%d (4)\n", sbc_cie.num_subbands);
        num_subbands = 4;
        break;
    case A2D_SBC_IE_SUBBAND_8:
        APPL_TRACE_DEBUG("\tnum_subbands:%d (8)\n", sbc_cie.num_subbands);
        num_subbands = 8;
        break;
    default:
        APPL_TRACE_DEBUG(" Unknown SubBands ");
        break;
    }

    switch (sbc_cie.alloc_mthd) {
    case A2D_SBC_IE_ALLOC_MD_S:
        APPL_TRACE_DEBUG("\talloc_mthd:%d (SNR)\n", sbc_cie.alloc_mthd);
        break;
    case A2D_SBC_IE_ALLOC_MD_L:
        APPL_TRACE_DEBUG("\talloc_mthd:%d (Loudness)\n", sbc_cie.alloc_mthd);
        break;
    default:
        APPL_TRACE_DEBUG(" Unknown Allocation Method");
        break;
    }

    APPL_TRACE_EVENT("\tBit pool Min:%d Max:%d\n", sbc_cie.min_bitpool, sbc_cie.max_bitpool);

    int frames_to_process = ((freq_multiple) / (num_blocks * num_subbands)) + 1;
    APPL_TRACE_EVENT(" Frames to be processed in 20 ms %d\n", frames_to_process);
}

/*******************************************************************************
 **
 ** Function         btc_media_sink_enque_buf
 **
 ** Description      This function is called by the av_co to fill A2DP Sink Queue
 **
 **
 ** Returns          size of the queue
 *******************************************************************************/
UINT8 btc_media_sink_enque_buf(BT_HDR *p_pkt)
{
    tBT_SBC_HDR *p_msg;

    if (btc_media_cb.rx_flush == TRUE) { /* Flush enabled, do not enque*/
        return fixed_queue_length(btc_media_cb.RxSbcQ);
    }

    if (fixed_queue_length(btc_media_cb.RxSbcQ) >= MAX_OUTPUT_A2DP_FRAME_QUEUE_SZ) {
        APPL_TRACE_WARNING("Pkt dropped\n");
        return fixed_queue_length(btc_media_cb.RxSbcQ);
    }

    APPL_TRACE_DEBUG("btc_media_sink_enque_buf + ");

    /* allocate and Queue this buffer */
    if ((p_msg = (tBT_SBC_HDR *) osi_malloc(sizeof(tBT_SBC_HDR) +
                                            p_pkt->offset + p_pkt->len)) != NULL) {
        memcpy(p_msg, p_pkt, (sizeof(BT_HDR) + p_pkt->offset + p_pkt->len));
        p_msg->num_frames_to_be_processed = (*((UINT8 *)(p_msg + 1) + p_msg->offset)) & 0x0f;
        APPL_TRACE_VERBOSE("btc_media_sink_enque_buf %d + \n", p_msg->num_frames_to_be_processed);
        fixed_queue_enqueue(btc_media_cb.RxSbcQ, p_msg);
        btc_media_data_post();
    } else {
        /* let caller deal with a failed allocation */
        APPL_TRACE_WARNING("btc_media_sink_enque_buf No Buffer left - ");
    }
    return fixed_queue_length(btc_media_cb.RxSbcQ);
}

/*******************************************************************************
 **
 ** Function         btc_media_aa_readbuf
 **
 ** Description      This function is called by the av_co to get the next buffer to send
 **
 **
 ** Returns          void
 *******************************************************************************/
BT_HDR *btc_media_aa_readbuf(void)
{
    return NULL;
}

/*******************************************************************************
 **
 ** Function         dump_codec_info
 **
 ** Description      Decode and display codec_info (for debug)
 **
 ** Returns          void
 **
 *******************************************************************************/
void dump_codec_info(unsigned char *p_codec)
{
    tA2D_STATUS a2d_status;
    tA2D_SBC_CIE sbc_cie;

    a2d_status = A2D_ParsSbcInfo(&sbc_cie, p_codec, FALSE);
    if (a2d_status != A2D_SUCCESS) {
        APPL_TRACE_ERROR("ERROR dump_codec_info A2D_ParsSbcInfo fail:%d\n", a2d_status);
        return;
    }

    APPL_TRACE_DEBUG("dump_codec_info");

    if (sbc_cie.samp_freq == A2D_SBC_IE_SAMP_FREQ_16) {
        APPL_TRACE_DEBUG("\tsamp_freq:%d (16000)\n", sbc_cie.samp_freq);
    } else  if (sbc_cie.samp_freq == A2D_SBC_IE_SAMP_FREQ_32) {
        APPL_TRACE_DEBUG("\tsamp_freq:%d (32000)\n", sbc_cie.samp_freq);
    } else  if (sbc_cie.samp_freq == A2D_SBC_IE_SAMP_FREQ_44) {
        APPL_TRACE_DEBUG("\tsamp_freq:%d (44.100)\n", sbc_cie.samp_freq);
    } else  if (sbc_cie.samp_freq == A2D_SBC_IE_SAMP_FREQ_48) {
        APPL_TRACE_DEBUG("\tsamp_freq:%d (48000)\n", sbc_cie.samp_freq);
    } else {
        APPL_TRACE_DEBUG("\tBAD samp_freq:%d\n", sbc_cie.samp_freq);
    }

    if (sbc_cie.ch_mode == A2D_SBC_IE_CH_MD_MONO) {
        APPL_TRACE_DEBUG("\tch_mode:%d (Mono)\n", sbc_cie.ch_mode);
    } else  if (sbc_cie.ch_mode == A2D_SBC_IE_CH_MD_DUAL) {
        APPL_TRACE_DEBUG("\tch_mode:%d (Dual)\n", sbc_cie.ch_mode);
    } else  if (sbc_cie.ch_mode == A2D_SBC_IE_CH_MD_STEREO) {
        APPL_TRACE_DEBUG("\tch_mode:%d (Stereo)\n", sbc_cie.ch_mode);
    } else  if (sbc_cie.ch_mode == A2D_SBC_IE_CH_MD_JOINT) {
        APPL_TRACE_DEBUG("\tch_mode:%d (Joint)\n", sbc_cie.ch_mode);
    } else {
        APPL_TRACE_DEBUG("\tBAD ch_mode:%d\n", sbc_cie.ch_mode);
    }

    if (sbc_cie.block_len == A2D_SBC_IE_BLOCKS_4) {
        APPL_TRACE_DEBUG("\tblock_len:%d (4)\n", sbc_cie.block_len);
    } else  if (sbc_cie.block_len == A2D_SBC_IE_BLOCKS_8) {
        APPL_TRACE_DEBUG("\tblock_len:%d (8)\n", sbc_cie.block_len);
    } else  if (sbc_cie.block_len == A2D_SBC_IE_BLOCKS_12) {
        APPL_TRACE_DEBUG("\tblock_len:%d (12)\n", sbc_cie.block_len);
    } else  if (sbc_cie.block_len == A2D_SBC_IE_BLOCKS_16) {
        APPL_TRACE_DEBUG("\tblock_len:%d (16)\n", sbc_cie.block_len);
    } else {
        APPL_TRACE_DEBUG("\tBAD block_len:%d\n", sbc_cie.block_len);
    }

    if (sbc_cie.num_subbands == A2D_SBC_IE_SUBBAND_4) {
        APPL_TRACE_DEBUG("\tnum_subbands:%d (4)\n", sbc_cie.num_subbands);
    } else  if (sbc_cie.num_subbands == A2D_SBC_IE_SUBBAND_8) {
        APPL_TRACE_DEBUG("\tnum_subbands:%d (8)\n", sbc_cie.num_subbands);
    } else {
        APPL_TRACE_DEBUG("\tBAD num_subbands:%d\n", sbc_cie.num_subbands);
    }

    if (sbc_cie.alloc_mthd == A2D_SBC_IE_ALLOC_MD_S) {
        APPL_TRACE_DEBUG("\talloc_mthd:%d (SNR)\n", sbc_cie.alloc_mthd);
    } else  if (sbc_cie.alloc_mthd == A2D_SBC_IE_ALLOC_MD_L) {
        APPL_TRACE_DEBUG("\talloc_mthd:%d (Loundess)\n", sbc_cie.alloc_mthd);
    } else {
        APPL_TRACE_DEBUG("\tBAD alloc_mthd:%d\n", sbc_cie.alloc_mthd);
    }

    APPL_TRACE_DEBUG("\tBit pool Min:%d Max:%d\n", sbc_cie.min_bitpool, sbc_cie.max_bitpool);

}

#endif /* #if BTC_AV_INCLUDED */
