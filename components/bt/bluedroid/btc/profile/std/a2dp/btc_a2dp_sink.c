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
 **  Name:          btc_a2dp_sink.c
 **
 ******************************************************************************/
#include "bt_target.h"
#include "bt_trace.h"
#include <string.h>
#include <stdint.h>
#include "bt_defs.h"
#include "allocator.h"
#include "mutex.h"
#include "thread.h"
#include "fixed_queue.h"
#include "a2d_api.h"
#include "a2d_sbc.h"
#include "bta_av_api.h"
#include "bta_av_ci.h"
#include "btc_av_co.h"
#include "btc_a2dp.h"
#include "btc_a2dp_control.h"
#include "btc_a2dp_sink.h"
#include "btc_manage.h"
#include "btc_av.h"
#include "btc_util.h"
#include "esp_a2dp_api.h"
#include "oi_codec_sbc.h"
#include "oi_status.h"

#if (BTC_AV_SINK_INCLUDED == TRUE)

/*****************************************************************************
 **  Constants
 *****************************************************************************/

/* BTC media cmd event definition : BTC_MEDIA_TASK_CMD */
enum {
    BTC_MEDIA_TASK_SINK_INIT,
    BTC_MEDIA_TASK_SINK_CLEAN_UP,
    BTC_MEDIA_FLUSH_AA_RX,
    BTC_MEDIA_AUDIO_SINK_CFG_UPDATE,
    BTC_MEDIA_AUDIO_SINK_CLEAR_TRACK,
};

enum {
    BTC_A2DP_SINK_STATE_OFF = 0,
    BTC_A2DP_SINK_STATE_ON = 1,
    BTC_A2DP_SINK_STATE_SHUTTING_DOWN = 2
};

enum {
    BTC_A2DP_SINK_DATA_EVT = 0,
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
#define MAX_OUTPUT_A2DP_SNK_FRAME_QUEUE_SZ     (5)

typedef struct {
    UINT16 num_frames_to_be_processed;
    UINT16 len;
    UINT16 offset;
    UINT16 layer_specific;
} tBT_SBC_HDR;

typedef struct {
    BOOLEAN rx_flush; /* discards any incoming data when true */
    UINT8   channel_count;
    fixed_queue_t *RxSbcQ;
    UINT32  sample_rate;
} tBTC_A2DP_SINK_CB;

static void btc_a2dp_sink_thread_init(UNUSED_ATTR void *context);
static void btc_a2dp_sink_thread_cleanup(UNUSED_ATTR void *context);
static void btc_a2dp_sink_flush_q(fixed_queue_t *p_q);
static void btc_a2dp_sink_rx_flush(void);
static int btc_a2dp_sink_get_track_frequency(UINT8 frequency);
static int btc_a2dp_sink_get_track_channel_count(UINT8 channeltype);
/* Handle incoming media packets A2DP SINK streaming*/
static void btc_a2dp_sink_handle_inc_media(tBT_SBC_HDR *p_msg);
static void btc_a2dp_sink_handle_decoder_reset(tBTC_MEDIA_SINK_CFG_UPDATE *p_msg);
static void btc_a2dp_sink_handle_clear_track(void);
static BOOLEAN btc_a2dp_sink_clear_track(void);
static void btc_a2dp_sink_task_handler(void *arg);

static void btc_a2dp_sink_data_ready(UNUSED_ATTR void *context);

static tBTC_A2DP_SINK_CB btc_aa_snk_cb;
static int btc_a2dp_sink_state = BTC_A2DP_SINK_STATE_OFF;
static xTaskHandle  btc_aa_snk_task_hdl = NULL;
static QueueHandle_t btc_aa_snk_data_queue = NULL;
static QueueHandle_t btc_aa_snk_ctrl_queue = NULL;
static QueueSetHandle_t btc_aa_snk_queue_set;

static esp_a2d_sink_data_cb_t bt_aa_snk_data_cb = NULL;

void btc_a2dp_sink_reg_data_cb(esp_a2d_sink_data_cb_t callback)
{
    // todo: critical section protection
    bt_aa_snk_data_cb = callback;
}

static inline void btc_a2d_data_cb_to_app(const uint8_t *data, uint32_t len)
{
    // todo: critical section protection
    if (bt_aa_snk_data_cb) {
        bt_aa_snk_data_cb(data, len);
    }
}

OI_CODEC_SBC_DECODER_CONTEXT context;
OI_UINT32 contextData[CODEC_DATA_WORDS(2, SBC_CODEC_FAST_FILTER_BUFFERS)];
OI_INT16 pcmData[15 * SBC_MAX_SAMPLES_PER_FRAME * SBC_MAX_CHANNELS];

/*****************************************************************************
 **  Misc helper functions
 *****************************************************************************/
static inline void btc_a2d_cb_to_app(esp_a2d_cb_event_t event, esp_a2d_cb_param_t *param)
{
    esp_a2d_cb_t btc_aa_cb = (esp_a2d_cb_t)btc_profile_cb_get(BTC_PID_A2DP);
    if (btc_aa_cb) {
        btc_aa_cb(event, param);
    }
}

/*****************************************************************************
 **  BTC ADAPTATION
 *****************************************************************************/

static void btc_a2dp_sink_ctrl_post(uint32_t sig, void *par)
{
    BtTaskEvt_t *evt = (BtTaskEvt_t *)osi_malloc(sizeof(BtTaskEvt_t));
    if (evt == NULL) {
        return;
    }

    evt->sig = sig;
    evt->par = par;

    if (xQueueSend(btc_aa_snk_ctrl_queue, &evt, portMAX_DELAY) != pdTRUE) {
        APPL_TRACE_WARNING("btc_aa_snk_ctrl_queue failed, sig 0x%x\n", sig);
    }
}

static void btc_a2dp_sink_ctrl_handler(BtTaskEvt_t *e)
{
    if (e == NULL) {
        return;
    }
    switch (e->sig) {
    case BTC_MEDIA_TASK_SINK_INIT:
        btc_a2dp_sink_thread_init(NULL);
        break;
    case BTC_MEDIA_TASK_SINK_CLEAN_UP:
        btc_a2dp_sink_thread_cleanup(NULL);
        break;
    case BTC_MEDIA_AUDIO_SINK_CFG_UPDATE:
        btc_a2dp_sink_handle_decoder_reset(e->par);
        break;
    case BTC_MEDIA_AUDIO_SINK_CLEAR_TRACK:
        btc_a2dp_sink_handle_clear_track();
        break;
    case BTC_MEDIA_FLUSH_AA_RX:
        btc_a2dp_sink_rx_flush();
        break;
    default:
        APPL_TRACE_WARNING("media task unhandled evt: 0x%x\n", e->sig);
    }
    if (e->par != NULL) {
        osi_free(e->par);
    }
}

static void btc_a2dp_sink_task_handler(void *arg)
{
    QueueSetMemberHandle_t xActivatedMember;
    BtTaskEvt_t *e = NULL;
    for (;;) {
        xActivatedMember = xQueueSelectFromSet(btc_aa_snk_queue_set, portMAX_DELAY);
        if (xActivatedMember == btc_aa_snk_data_queue) {
            int32_t data_evt;
            xQueueReceive(xActivatedMember, &data_evt, 0);
            if (data_evt == BTC_A2DP_SINK_DATA_EVT) {
                btc_a2dp_sink_data_ready(NULL);
            }
        } else if (xActivatedMember == btc_aa_snk_ctrl_queue) {
            xQueueReceive(xActivatedMember, &e, 0);
            btc_a2dp_sink_ctrl_handler(e);
            osi_free(e);
        }
    }
}

bool btc_a2dp_sink_startup(void)
{
    if (btc_a2dp_sink_state != BTC_A2DP_SINK_STATE_OFF) {
        APPL_TRACE_ERROR("warning : media task already running");
        return false;
    }

    APPL_TRACE_EVENT("## A2DP SINK START MEDIA THREAD ##");

    btc_aa_snk_queue_set = xQueueCreateSet(BTC_MEDIA_TASK_QUEUE_SET_LEN);
    configASSERT(btc_aa_snk_queue_set);
    btc_aa_snk_data_queue = xQueueCreate(BTC_MEDIA_DATA_QUEUE_LEN, sizeof(int32_t));
    configASSERT(btc_aa_snk_data_queue);
    xQueueAddToSet(btc_aa_snk_data_queue, btc_aa_snk_queue_set);

    btc_aa_snk_ctrl_queue = xQueueCreate(BTC_MEDIA_CTRL_QUEUE_LEN, sizeof(void *));
    configASSERT(btc_aa_snk_ctrl_queue);
    xQueueAddToSet(btc_aa_snk_ctrl_queue, btc_aa_snk_queue_set);

    if (!btc_aa_snk_data_queue || !btc_aa_snk_ctrl_queue || !btc_aa_snk_queue_set ) {
        goto error_exit;
    }

    xTaskCreatePinnedToCore(btc_a2dp_sink_task_handler, BTC_MEDIA_TASK_NAME, BTC_MEDIA_TASK_STACK_SIZE, NULL, BTC_MEDIA_TASK_PRIO, &btc_aa_snk_task_hdl, BTC_MEDIA_TASK_PINNED_TO_CORE);
    if (btc_aa_snk_task_hdl == NULL) {
        goto error_exit;
    }

    btc_a2dp_sink_ctrl_post(BTC_MEDIA_TASK_SINK_INIT, NULL);

    APPL_TRACE_EVENT("## A2DP SINK MEDIA THREAD STARTED ##\n");

    return true;

error_exit:;
    APPL_TRACE_ERROR("%s unable to start up media thread\n", __func__);

    if (btc_aa_snk_task_hdl != NULL) {
        vTaskDelete(btc_aa_snk_task_hdl);
        btc_aa_snk_task_hdl = NULL;
    }

    if (btc_aa_snk_data_queue) {
        vQueueDelete(btc_aa_snk_data_queue);
        btc_aa_snk_data_queue = NULL;
    }
    if (btc_aa_snk_ctrl_queue) {
        vQueueDelete(btc_aa_snk_ctrl_queue);
        btc_aa_snk_ctrl_queue = NULL;
    }

    return false;
}

void btc_a2dp_sink_shutdown(void)
{
    APPL_TRACE_EVENT("## A2DP SINK STOP MEDIA THREAD ##\n");

    // Exit thread
    btc_a2dp_sink_ctrl_post(BTC_MEDIA_TASK_SINK_CLEAN_UP, NULL);

    vTaskDelete(btc_aa_snk_task_hdl);
    btc_aa_snk_task_hdl = NULL;

    vQueueDelete(btc_aa_snk_data_queue);
    btc_aa_snk_data_queue = NULL;

    vQueueDelete(btc_aa_snk_ctrl_queue);
    btc_aa_snk_ctrl_queue = NULL;
}

/*****************************************************************************
**
** Function        btc_a2dp_sink_on_idle
**
*******************************************************************************/

void btc_a2dp_sink_on_idle(void)
{
    btc_aa_snk_cb.rx_flush = TRUE;
    btc_a2dp_sink_rx_flush_req();
    btc_a2dp_sink_clear_track();

    APPL_TRACE_DEBUG("Stopped BT track");
}

/*****************************************************************************
**
** Function        btc_a2dp_sink_on_stopped
**
*******************************************************************************/

void btc_a2dp_sink_on_stopped(tBTA_AV_SUSPEND *p_av)
{
    btc_aa_snk_cb.rx_flush = TRUE;
    btc_a2dp_sink_rx_flush_req();
    btc_a2dp_control_set_datachnl_stat(FALSE);
}

/*****************************************************************************
**
** Function        btc_a2dp_on_suspended
**
*******************************************************************************/

void btc_a2dp_sink_on_suspended(tBTA_AV_SUSPEND *p_av)
{
    btc_aa_snk_cb.rx_flush = TRUE;
    btc_a2dp_sink_rx_flush_req();
    return;
}

static void btc_a2dp_sink_data_post(int32_t data_type)
{
    if (xQueueSend(btc_aa_snk_data_queue, &data_type, 0) != pdTRUE) {
        APPL_TRACE_DEBUG("Media data Q filled\n");
    }
}

/*******************************************************************************
 **
 ** Function         btc_a2dp_sink_clear_track
 **
 ** Description
 **
 ** Returns          TRUE is success
 **
 *******************************************************************************/
static BOOLEAN btc_a2dp_sink_clear_track(void)
{
    btc_a2dp_sink_ctrl_post(BTC_MEDIA_AUDIO_SINK_CLEAR_TRACK, NULL);
    return TRUE;
}

/* when true media task discards any rx frames */
void btc_a2dp_sink_set_rx_flush(BOOLEAN enable)
{
    APPL_TRACE_EVENT("## DROP RX %d ##\n", enable);
    btc_aa_snk_cb.rx_flush = enable;
}

/*****************************************************************************
**
** Function        btc_a2dp_sink_reset_decoder
**
** Description
**
** Returns
**
*******************************************************************************/

void btc_a2dp_sink_reset_decoder(UINT8 *p_av)
{
    APPL_TRACE_EVENT("btc reset decoder");
    APPL_TRACE_DEBUG("btc reset decoder p_codec_info[%x:%x:%x:%x:%x:%x]\n",
                     p_av[1], p_av[2], p_av[3],
                     p_av[4], p_av[5], p_av[6]);

    tBTC_MEDIA_SINK_CFG_UPDATE *p_buf;
    if (NULL == (p_buf = osi_malloc(sizeof(tBTC_MEDIA_SINK_CFG_UPDATE)))) {
        APPL_TRACE_ERROR("btc reset decoder No Buffer ");
        return;
    }

    memcpy(p_buf->codec_info, p_av, AVDT_CODEC_SIZE);
    btc_a2dp_sink_ctrl_post(BTC_MEDIA_AUDIO_SINK_CFG_UPDATE, p_buf);
}

static void btc_a2dp_sink_data_ready(UNUSED_ATTR void *context)
{
    tBT_SBC_HDR *p_msg;

    if (fixed_queue_is_empty(btc_aa_snk_cb.RxSbcQ)) {
        APPL_TRACE_DEBUG("  QUE  EMPTY ");
    } else {
        if (btc_aa_snk_cb.rx_flush == TRUE) {
            btc_a2dp_sink_flush_q(btc_aa_snk_cb.RxSbcQ);
            return;
        }

        while ((p_msg = (tBT_SBC_HDR *)fixed_queue_try_peek_first(btc_aa_snk_cb.RxSbcQ)) != NULL ) {
            btc_a2dp_sink_handle_inc_media(p_msg);
            p_msg = (tBT_SBC_HDR *)fixed_queue_try_dequeue(btc_aa_snk_cb.RxSbcQ);
            if ( p_msg == NULL ) {
                APPL_TRACE_ERROR("Insufficient data in que ");
                break;
            }
            osi_free(p_msg);
        }
        APPL_TRACE_DEBUG(" Process Frames - ");
    }
}

/*******************************************************************************
 **
 ** Function         btc_a2dp_sink_handle_decoder_reset
 **
 ** Description
 **
 ** Returns          void
 **
 *******************************************************************************/
static void btc_a2dp_sink_handle_decoder_reset(tBTC_MEDIA_SINK_CFG_UPDATE *p_msg)
{
    tBTC_MEDIA_SINK_CFG_UPDATE *p_buf = p_msg;
    tA2D_STATUS a2d_status;
    tA2D_SBC_CIE sbc_cie;
    OI_STATUS       status;
    UINT32          freq_multiple = 48 * 20; /* frequency multiple for 20ms of data , initialize with 48K*/
    UINT32          num_blocks = 16;
    UINT32          num_subbands = 8;

    APPL_TRACE_EVENT("%s p_codec_info[%x:%x:%x:%x:%x:%x]\n", __FUNCTION__,
                     p_buf->codec_info[1], p_buf->codec_info[2], p_buf->codec_info[3],
                     p_buf->codec_info[4], p_buf->codec_info[5], p_buf->codec_info[6]);

    a2d_status = A2D_ParsSbcInfo(&sbc_cie, p_buf->codec_info, FALSE);
    if (a2d_status != A2D_SUCCESS) {
        APPL_TRACE_ERROR("ERROR dump_codec_info A2D_ParsSbcInfo fail:%d\n", a2d_status);
        return;
    }

    btc_aa_snk_cb.sample_rate = btc_a2dp_sink_get_track_frequency(sbc_cie.samp_freq);
    btc_aa_snk_cb.channel_count = btc_a2dp_sink_get_track_channel_count(sbc_cie.ch_mode);

    btc_aa_snk_cb.rx_flush = FALSE;
    APPL_TRACE_EVENT("Reset to sink role");
    status = OI_CODEC_SBC_DecoderReset(&context, contextData, sizeof(contextData), 2, 2, FALSE);
    if (!OI_SUCCESS(status)) {
        APPL_TRACE_ERROR("OI_CODEC_SBC_DecoderReset failed with error code %d\n", status);
    }

    btc_a2dp_control_set_datachnl_stat(TRUE);

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
 ** Function         btc_a2dp_sink_handle_inc_media
 **
 ** Description
 **
 ** Returns          void
 **
 *******************************************************************************/
static void btc_a2dp_sink_handle_inc_media(tBT_SBC_HDR *p_msg)
{
    UINT8 *sbc_start_frame = ((UINT8 *)(p_msg + 1) + p_msg->offset + 1);
    int count;
    UINT32 pcmBytes, availPcmBytes;
    OI_INT16 *pcmDataPointer = pcmData; /*Will be overwritten on next packet receipt*/
    OI_STATUS status;
    int num_sbc_frames = p_msg->num_frames_to_be_processed;
    UINT32 sbc_frame_len = p_msg->len - 1;
    availPcmBytes = 2 * sizeof(pcmData);

    if (btc_av_get_peer_sep() == AVDT_TSEP_SNK || (btc_aa_snk_cb.rx_flush)) {
        APPL_TRACE_DEBUG(" State Changed happened in this tick ");
        return;
    }

    // ignore data if no one is listening
    if (!btc_a2dp_control_get_datachnl_stat()) {
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
 ** Function         btc_a2dp_sink_rx_flush_req
 **
 ** Description
 **
 ** Returns          TRUE is success
 **
 *******************************************************************************/
BOOLEAN btc_a2dp_sink_rx_flush_req(void)
{
    if (fixed_queue_is_empty(btc_aa_snk_cb.RxSbcQ) == TRUE) { /*  Que is already empty */
        return TRUE;
    }

    btc_a2dp_sink_ctrl_post(BTC_MEDIA_FLUSH_AA_RX, NULL);
    return TRUE;
}

/*******************************************************************************
 **
 ** Function         btc_a2dp_sink_rx_flush
 **
 ** Description
 **
 ** Returns          void
 **
 *******************************************************************************/
static void btc_a2dp_sink_rx_flush(void)
{
    /* Flush all enqueued SBC  buffers (encoded) */
    APPL_TRACE_DEBUG("btc_a2dp_sink_rx_flush");

    btc_a2dp_sink_flush_q(btc_aa_snk_cb.RxSbcQ);
}

static int btc_a2dp_sink_get_track_frequency(UINT8 frequency)
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

static int btc_a2dp_sink_get_track_channel_count(UINT8 channeltype)
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

/*******************************************************************************
 **
 ** Function         btc_a2dp_sink_enque_buf
 **
 ** Description      This function is called by the av_co to fill A2DP Sink Queue
 **
 **
 ** Returns          size of the queue
 *******************************************************************************/
UINT8 btc_a2dp_sink_enque_buf(BT_HDR *p_pkt)
{
    tBT_SBC_HDR *p_msg;

    if (btc_aa_snk_cb.rx_flush == TRUE) { /* Flush enabled, do not enque*/
        return fixed_queue_length(btc_aa_snk_cb.RxSbcQ);
    }

    if (fixed_queue_length(btc_aa_snk_cb.RxSbcQ) >= MAX_OUTPUT_A2DP_SNK_FRAME_QUEUE_SZ) {
        APPL_TRACE_WARNING("Pkt dropped\n");
        return fixed_queue_length(btc_aa_snk_cb.RxSbcQ);
    }

    APPL_TRACE_DEBUG("btc_a2dp_sink_enque_buf + ");

    /* allocate and Queue this buffer */
    if ((p_msg = (tBT_SBC_HDR *) osi_malloc(sizeof(tBT_SBC_HDR) +
                                            p_pkt->offset + p_pkt->len)) != NULL) {
        memcpy(p_msg, p_pkt, (sizeof(BT_HDR) + p_pkt->offset + p_pkt->len));
        p_msg->num_frames_to_be_processed = (*((UINT8 *)(p_msg + 1) + p_msg->offset)) & 0x0f;
        APPL_TRACE_VERBOSE("btc_a2dp_sink_enque_buf %d + \n", p_msg->num_frames_to_be_processed);
        fixed_queue_enqueue(btc_aa_snk_cb.RxSbcQ, p_msg);
        btc_a2dp_sink_data_post(BTC_A2DP_SINK_DATA_EVT);
    } else {
        /* let caller deal with a failed allocation */
        APPL_TRACE_WARNING("btc_a2dp_sink_enque_buf No Buffer left - ");
    }
    return fixed_queue_length(btc_aa_snk_cb.RxSbcQ);
}

static void btc_a2dp_sink_handle_clear_track (void)
{
    APPL_TRACE_DEBUG("%s", __FUNCTION__);
}

/*******************************************************************************
 **
 ** Function         btc_a2dp_sink_flush_q
 **
 ** Description
 **
 ** Returns          void
 **
 *******************************************************************************/
static void btc_a2dp_sink_flush_q(fixed_queue_t *p_q)
{
    while (! fixed_queue_is_empty(p_q)) {
        osi_free(fixed_queue_try_dequeue(p_q));
    }
}

static void btc_a2dp_sink_thread_init(UNUSED_ATTR void *context)
{
    APPL_TRACE_EVENT("%s\n", __func__);
    memset(&btc_aa_snk_cb, 0, sizeof(btc_aa_snk_cb));

    btc_a2dp_sink_state = BTC_A2DP_SINK_STATE_ON;

    btc_aa_snk_cb.RxSbcQ = fixed_queue_new(SIZE_MAX);

    btc_a2dp_control_init();
}

static void btc_a2dp_sink_thread_cleanup(UNUSED_ATTR void *context)
{
    /* make sure no channels are restarted while shutting down */
    btc_a2dp_sink_state = BTC_A2DP_SINK_STATE_SHUTTING_DOWN;

    btc_a2dp_control_set_datachnl_stat(FALSE);
    /* Clear task flag */
    btc_a2dp_sink_state = BTC_A2DP_SINK_STATE_OFF;

    btc_a2dp_control_cleanup();

    fixed_queue_free(btc_aa_snk_cb.RxSbcQ, osi_free_func);
}

#endif /* BTC_AV_SINK_INCLUDED */
