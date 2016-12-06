/******************************************************************************
 *
 *  Copyright (C) 2009-2012 Broadcom Corporation
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at:
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 ******************************************************************************/

/******************************************************************************
 **
 **  Name:          btif_media_task.c
 **
 **  Description:   This is the multimedia module for the BTIF system.  It
 **                 contains task implementations AV, HS and HF profiles
 **                 audio & video processing
 **
 ******************************************************************************/

#define LOG_TAG "bt_btif_media"

#include "bt_trace.h"
#include <string.h>
#include <stdio.h>
#include <stdint.h>

#include "bt_target.h"
#include "fixed_queue.h"
#include "gki.h"
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

#include "btif_av_co.h"
#include "btif_media.h"

#include "alarm.h"
#include "bt_trace.h"
#include "thread.h"

#include "bt_defs.h"
#include "btif_av.h"
#include "btif_sm.h"
#include "btif_util.h"
#if (BTA_AV_SINK_INCLUDED == TRUE)
#include "oi_codec_sbc.h"
#include "oi_status.h"
#endif
#include "stdio.h"

#include "btif_media.h"
#include "allocator.h"
#include "bt_utils.h"
#include "esp_a2dp_api.h"

#if (BTA_AV_SINK_INCLUDED == TRUE)
OI_CODEC_SBC_DECODER_CONTEXT context;
OI_UINT32 contextData[CODEC_DATA_WORDS(2, SBC_CODEC_FAST_FILTER_BUFFERS)];
OI_INT16 pcmData[15 * SBC_MAX_SAMPLES_PER_FRAME * SBC_MAX_CHANNELS];
#endif


/*****************************************************************************
 **  Constants
 *****************************************************************************/

#ifndef AUDIO_CHANNEL_OUT_MONO
#define AUDIO_CHANNEL_OUT_MONO 0x01
#endif

#ifndef AUDIO_CHANNEL_OUT_STEREO
#define AUDIO_CHANNEL_OUT_STEREO 0x03
#endif

/* BTIF media cmd event definition : BTIF_MEDIA_TASK_CMD */
enum {
    BTIF_MEDIA_START_AA_TX = 1,
    BTIF_MEDIA_STOP_AA_TX,
    BTIF_MEDIA_AA_RX_RDY,
    BTIF_MEDIA_UIPC_RX_RDY,
    BTIF_MEDIA_SBC_ENC_INIT,
    BTIF_MEDIA_SBC_ENC_UPDATE,
    BTIF_MEDIA_SBC_DEC_INIT,
    BTIF_MEDIA_VIDEO_DEC_INIT,
    BTIF_MEDIA_FLUSH_AA_TX,
    BTIF_MEDIA_FLUSH_AA_RX,
    BTIF_MEDIA_AUDIO_FEEDING_INIT,
    BTIF_MEDIA_AUDIO_RECEIVING_INIT,
    BTIF_MEDIA_AUDIO_SINK_CFG_UPDATE,
    BTIF_MEDIA_AUDIO_SINK_CLEAR_TRACK
};

enum {
    MEDIA_TASK_STATE_OFF = 0,
    MEDIA_TASK_STATE_ON = 1,
    MEDIA_TASK_STATE_SHUTTING_DOWN = 2
};

enum {
    SIG_MEDIA_TASK_INIT = 0xf0,
    SIG_MEDIA_TASK_CLEAN_UP = 0xf1,
    SIG_MEDIA_TASK_AVK_ALARM_TO = 0xf2,
    SIG_MEDIA_TASK_AA_ALARM_TO = 0xf3,
    SIG_MEDIA_TASK_CMD_READY = 0xf4
};

/* Macro to multiply the media task tick */
#ifndef BTIF_MEDIA_NUM_TICK
#define BTIF_MEDIA_NUM_TICK      1
#endif

/* Media task tick in milliseconds, must be set to multiple of
   (1000/TICKS_PER_SEC) (10) */

#define BTIF_MEDIA_TIME_TICK                     (20 * BTIF_MEDIA_NUM_TICK)
#define A2DP_DATA_READ_POLL_MS    (BTIF_MEDIA_TIME_TICK / 2)
#define BTIF_SINK_MEDIA_TIME_TICK                (20 * BTIF_MEDIA_NUM_TICK)


/* buffer pool */
#define BTIF_MEDIA_AA_POOL_ID GKI_POOL_ID_3
#define BTIF_MEDIA_AA_BUF_SIZE GKI_BUF3_SIZE

/* offset */
#if (BTA_AV_CO_CP_SCMS_T == TRUE)
#define BTIF_MEDIA_AA_SBC_OFFSET (AVDT_MEDIA_OFFSET + BTA_AV_SBC_HDR_SIZE + 1)
#else
#define BTIF_MEDIA_AA_SBC_OFFSET (AVDT_MEDIA_OFFSET + BTA_AV_SBC_HDR_SIZE)
#endif

/* Define the bitrate step when trying to match bitpool value */
#ifndef BTIF_MEDIA_BITRATE_STEP
#define BTIF_MEDIA_BITRATE_STEP 5
#endif

/* Middle quality quality setting @ 44.1 khz */
#define DEFAULT_SBC_BITRATE 328

#ifndef BTIF_A2DP_NON_EDR_MAX_RATE
#define BTIF_A2DP_NON_EDR_MAX_RATE 229
#endif

#define USEC_PER_SEC 1000000L
#define TPUT_STATS_INTERVAL_US (3000*1000)

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

/* 18 frames is equivalent to 6.89*18*2.9 ~= 360 ms @ 44.1 khz, 20 ms mediatick */
#define MAX_OUTPUT_A2DP_FRAME_QUEUE_SZ 18

#ifndef MAX_PCM_FRAME_NUM_PER_TICK
#define MAX_PCM_FRAME_NUM_PER_TICK     14
#endif

/* In case of A2DP SINK, we will delay start by 5 AVDTP Packets*/
#define MAX_A2DP_DELAYED_START_FRAME_COUNT 5
#define PACKET_PLAYED_PER_TICK_48 8
#define PACKET_PLAYED_PER_TICK_44 7
#define PACKET_PLAYED_PER_TICK_32 5
#define PACKET_PLAYED_PER_TICK_16 3

typedef struct {
    UINT16 num_frames_to_be_processed;
    UINT16 len;
    UINT16 offset;
    UINT16 layer_specific;
} tBT_SBC_HDR;

typedef struct {
    UINT32 aa_frame_counter;
    INT32  aa_feed_counter;
    INT32  aa_feed_residue;
    UINT32 counter;
    UINT32 bytes_per_tick;  /* pcm bytes read each media task tick */
} tBTIF_AV_MEDIA_FEEDINGS_PCM_STATE;

typedef union {
    tBTIF_AV_MEDIA_FEEDINGS_PCM_STATE pcm;
} tBTIF_AV_MEDIA_FEEDINGS_STATE;

typedef struct {
#if (BTA_AV_INCLUDED == TRUE)
    BUFFER_Q RxSbcQ;
    BOOLEAN is_tx_timer;
    BOOLEAN is_rx_timer;
    UINT8 busy_level;
    void *av_sm_hdl;
    UINT8 a2dp_cmd_pending; /* we can have max one command pending */
    BOOLEAN rx_flush; /* discards any incoming data when true */
    UINT8 peer_sep;
    BOOLEAN data_channel_open;
    UINT8   frames_to_process;

    UINT32  sample_rate;
    UINT8   channel_count;
    osi_alarm_t *decode_alarm;
#endif

} tBTIF_MEDIA_CB;

typedef struct {
    long long rx;
    long long rx_tot;
    long long tx;
    long long tx_tot;
    long long ts_prev_us;
} t_stat;

#if (BTA_AV_SINK_INCLUDED == TRUE)
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
#endif
static void btif_media_flush_q(BUFFER_Q *p_q);
static void btif_media_task_aa_handle_stop_decoding(void );
static void btif_media_task_aa_rx_flush(void);

static const char *dump_media_event(UINT16 event);
static void btif_media_thread_handle_cmd(fixed_queue_t *queue);

/* Handle incoming media packets A2DP SINK streaming*/
#if (BTA_AV_SINK_INCLUDED == TRUE)
static void btif_media_task_handle_inc_media(tBT_SBC_HDR *p_msg);
#endif

#if (BTA_AV_INCLUDED == TRUE)
#if (BTA_AV_SINK_INCLUDED == TRUE)
static void btif_media_task_aa_handle_decoder_reset(BT_HDR *p_msg);
static void btif_media_task_aa_handle_clear_track(void);
#endif
static void btif_media_task_aa_handle_start_decoding(void);
#endif
BOOLEAN btif_media_task_clear_track(void);
static void btif_media_task_handler(void *arg);

static void btif_media_task_avk_handle_timer(UNUSED_ATTR void *context);
static void btif_media_thread_init(UNUSED_ATTR void *context);
static void btif_media_thread_cleanup(UNUSED_ATTR void *context);
extern BOOLEAN btif_hf_is_call_idle();

static tBTIF_MEDIA_CB btif_media_cb;
static int media_task_running = MEDIA_TASK_STATE_OFF;

static fixed_queue_t *btif_media_cmd_msg_queue = NULL;
static xTaskHandle  xBtifMediaTaskHandle = NULL;
static QueueHandle_t xBtifMediaQueue = NULL;

static esp_a2d_data_cb_t bt_av_sink_data_callback = NULL;

esp_err_t esp_a2d_register_data_callback(esp_a2d_data_cb_t cb)
{
    // TODO: need protection against race
    bt_av_sink_data_callback = cb;
}

// TODO: need protection against race
#define BTIF_A2D_DATA_CB_TO_APP(data, len)    do { \
	if (bt_av_sink_data_callback) { \
	    bt_av_sink_data_callback(data, len); \
	} \
    } while (0)

/*****************************************************************************
 **  temporary hacked functions. TODO: port these functions or remove them?
 *****************************************************************************/
BOOLEAN btif_hf_is_call_idle(void)
{
    return FALSE;
}

/*****************************************************************************
 **  Misc helper functions
 *****************************************************************************/

UNUSED_ATTR static const char *dump_media_event(UINT16 event)
{
    switch (event) {
        CASE_RETURN_STR(BTIF_MEDIA_START_AA_TX)
        CASE_RETURN_STR(BTIF_MEDIA_STOP_AA_TX)
        CASE_RETURN_STR(BTIF_MEDIA_AA_RX_RDY)
        CASE_RETURN_STR(BTIF_MEDIA_UIPC_RX_RDY)
        CASE_RETURN_STR(BTIF_MEDIA_SBC_ENC_INIT)
        CASE_RETURN_STR(BTIF_MEDIA_SBC_ENC_UPDATE)
        CASE_RETURN_STR(BTIF_MEDIA_SBC_DEC_INIT)
        CASE_RETURN_STR(BTIF_MEDIA_VIDEO_DEC_INIT)
        CASE_RETURN_STR(BTIF_MEDIA_FLUSH_AA_TX)
        CASE_RETURN_STR(BTIF_MEDIA_FLUSH_AA_RX)
        CASE_RETURN_STR(BTIF_MEDIA_AUDIO_FEEDING_INIT)
        CASE_RETURN_STR(BTIF_MEDIA_AUDIO_RECEIVING_INIT)
        CASE_RETURN_STR(BTIF_MEDIA_AUDIO_SINK_CFG_UPDATE)
        CASE_RETURN_STR(BTIF_MEDIA_AUDIO_SINK_CLEAR_TRACK)

    default:
        return "UNKNOWN MEDIA EVENT";
    }
}

/*****************************************************************************
 **  A2DP CTRL PATH
 *****************************************************************************/
#if 0
// TODO: consider the necessity to add an API based on this function
static void btif_audiopath_detached(void)
{
    APPL_TRACE_EVENT("## AUDIO PATH DETACHED ##");

    /*  send stop request only if we are actively streaming and haven't received
        a stop request. Potentially audioflinger detached abnormally */
    if (btif_media_cb.is_tx_timer) {
        /* post stop event and wait for audio path to stop */
        btif_dispatch_sm_event(BTIF_AV_STOP_STREAM_REQ_EVT, NULL, 0);
    }
}
#endif

/*****************************************************************************
 **  BTIF ADAPTATION
 *****************************************************************************/

static void btif_media_task_post(uint32_t sig)
{
    BtTaskEvt_t *evt = (BtTaskEvt_t *)osi_malloc(sizeof(BtTaskEvt_t));
    if (evt == NULL) {
        return;
    }

    evt->sig = sig;
    evt->par = 0;

    if (xQueueSend(xBtifMediaQueue, &evt, 10 / portTICK_RATE_MS) != pdTRUE) {
        APPL_TRACE_ERROR("xBtifMediaQueue failed\n");
    }
}

static void btif_media_task_handler(void *arg)
{
    BtTaskEvt_t *e;
    for (;;) {
        if (pdTRUE == xQueueReceive(xBtifMediaQueue, &e, (portTickType)portMAX_DELAY)) {
            // LOG_ERROR("med evt %d\n", e->sig);
            switch (e->sig) {
            case SIG_MEDIA_TASK_AVK_ALARM_TO:
                btif_media_task_avk_handle_timer(NULL);
                break;
            case SIG_MEDIA_TASK_CMD_READY:
                fixed_queue_process(btif_media_cmd_msg_queue);
                break;
            case SIG_MEDIA_TASK_INIT:
                btif_media_thread_init(NULL);
                break;
            case SIG_MEDIA_TASK_CLEAN_UP:
                btif_media_thread_cleanup(NULL);
                break;
            default:
                APPL_TRACE_ERROR("media task unhandled evt: 0x%x\n", e->sig);
            }
        }
        osi_free(e);
    }
}

bool btif_a2dp_start_media_task(void)
{
    if (media_task_running != MEDIA_TASK_STATE_OFF) {
        APPL_TRACE_ERROR("warning : media task already running");
        return false;
    }

    APPL_TRACE_EVENT("## A2DP START MEDIA THREAD ##");

    btif_media_cmd_msg_queue = fixed_queue_new(SIZE_MAX);
    if (btif_media_cmd_msg_queue == NULL) {
        goto error_exit;
    }

    xBtifMediaQueue = xQueueCreate(60, sizeof(void *));
    if (xBtifMediaQueue == 0) {
        goto error_exit;
    }
    xTaskCreate(btif_media_task_handler, "BtifMediaT\n", 2048, NULL, configMAX_PRIORITIES - 1, &xBtifMediaTaskHandle);
    if (xBtifMediaTaskHandle == NULL) {
        goto error_exit;
    }
    fixed_queue_register_dequeue(btif_media_cmd_msg_queue, btif_media_thread_handle_cmd);
    btif_media_task_post(SIG_MEDIA_TASK_INIT);

    APPL_TRACE_EVENT("## A2DP MEDIA THREAD STARTED ##\n");

    return true;

error_exit:;
    APPL_TRACE_ERROR("%s unable to start up media thread\n", __func__);

    if (xBtifMediaTaskHandle != NULL) {
        vTaskDelete(xBtifMediaTaskHandle);
        xBtifMediaTaskHandle = NULL;
    }

    if (xBtifMediaQueue != 0) {
        vQueueDelete(xBtifMediaQueue);
        xBtifMediaQueue = 0;
    }
    fixed_queue_free(btif_media_cmd_msg_queue, NULL);
    btif_media_cmd_msg_queue = NULL;
    return false;
}

void btif_a2dp_stop_media_task(void)
{
    APPL_TRACE_EVENT("## A2DP STOP MEDIA THREAD ##\n");

    // Exit thread
    btif_media_task_post(SIG_MEDIA_TASK_CLEAN_UP);
    // TODO: wait until CLEAN up is done, then do task delete
    vTaskDelete(xBtifMediaTaskHandle);
    xBtifMediaTaskHandle = NULL;
    vQueueDelete(xBtifMediaQueue);
    xBtifMediaQueue = NULL;

    fixed_queue_free(btif_media_cmd_msg_queue, NULL);
    btif_media_cmd_msg_queue = NULL;
}

/*****************************************************************************
**
** Function        btif_a2dp_on_init
**
** Description
**
** Returns
**
*******************************************************************************/

void btif_a2dp_on_init(void)
{
    //tput_mon(1, 0, 1);
}


/*****************************************************************************
**
** Function        btif_a2dp_setup_codec
**
** Description
**
** Returns
**
*******************************************************************************/

void btif_a2dp_setup_codec(void)
{
    tBTIF_AV_MEDIA_FEEDINGS media_feeding;
    tBTIF_STATUS status;

    APPL_TRACE_EVENT("## A2DP SETUP CODEC ##\n");

    GKI_disable();

    /* for now hardcode 44.1 khz 16 bit stereo PCM format */
    media_feeding.cfg.pcm.sampling_freq = 44100;
    media_feeding.cfg.pcm.bit_per_sample = 16;
    media_feeding.cfg.pcm.num_channel = 2;
    media_feeding.format = BTIF_AV_CODEC_PCM;

    bta_av_co_audio_set_codec(&media_feeding, &status);

    GKI_enable();
}


/*****************************************************************************
**
** Function        btif_a2dp_on_idle
**
** Description
**
** Returns
**
*******************************************************************************/

void btif_a2dp_on_idle(void)
{
    APPL_TRACE_EVENT("## ON A2DP IDLE ##\n");

    bta_av_co_init();
#if (BTA_AV_SINK_INCLUDED == TRUE)
    if (btif_media_cb.peer_sep == AVDT_TSEP_SRC) {
        btif_media_cb.rx_flush = TRUE;
        btif_media_task_aa_rx_flush_req();
        btif_media_task_aa_handle_stop_decoding();
        btif_media_task_clear_track();
        APPL_TRACE_DEBUG("Stopped BT track");
    }
#endif
}

/*******************************************************************************
 **
 ** Function         btif_media_task_clear_track
 **
 ** Description
 **
 ** Returns          TRUE is success
 **
 *******************************************************************************/
BOOLEAN btif_media_task_clear_track(void)
{
    BT_HDR *p_buf;

    if (NULL == (p_buf = GKI_getbuf(sizeof(BT_HDR)))) {
        return FALSE;
    }

    p_buf->event = BTIF_MEDIA_AUDIO_SINK_CLEAR_TRACK;

    fixed_queue_enqueue(btif_media_cmd_msg_queue, p_buf);
    btif_media_task_post(SIG_MEDIA_TASK_CMD_READY);
    return TRUE;
}

/*****************************************************************************
**
** Function        btif_reset_decoder
**
** Description
**
** Returns
**
*******************************************************************************/

void btif_reset_decoder(UINT8 *p_av)
{
    APPL_TRACE_EVENT("btif_reset_decoder");
    APPL_TRACE_DEBUG("btif_reset_decoder p_codec_info[%x:%x:%x:%x:%x:%x]\n",
            p_av[1], p_av[2], p_av[3],
            p_av[4], p_av[5], p_av[6]);

    tBTIF_MEDIA_SINK_CFG_UPDATE *p_buf;
    if (NULL == (p_buf = GKI_getbuf(sizeof(tBTIF_MEDIA_SINK_CFG_UPDATE)))) {
        APPL_TRACE_ERROR("btif_reset_decoder No Buffer ");
        return;
    }

    memcpy(p_buf->codec_info, p_av, AVDT_CODEC_SIZE);
    p_buf->hdr.event = BTIF_MEDIA_AUDIO_SINK_CFG_UPDATE;

    fixed_queue_enqueue(btif_media_cmd_msg_queue, p_buf);
    btif_media_task_post(SIG_MEDIA_TASK_CMD_READY);
}

/*****************************************************************************
**
** Function        btif_a2dp_on_stopped
**
** Description
**
** Returns
**
*******************************************************************************/

void btif_a2dp_on_stopped(tBTA_AV_SUSPEND *p_av)
{
    APPL_TRACE_EVENT("## ON A2DP STOPPED ##\n");
    if (btif_media_cb.peer_sep == AVDT_TSEP_SRC) { /*  Handling for A2DP SINK cases*/
        btif_media_cb.rx_flush = TRUE;
        btif_media_task_aa_rx_flush_req();
        btif_media_task_aa_handle_stop_decoding();
        btif_media_cb.data_channel_open = FALSE;
        return;
    }
}


/*****************************************************************************
**
** Function        btif_a2dp_on_suspended
**
** Description
**
** Returns
**
*******************************************************************************/

void btif_a2dp_on_suspended(tBTA_AV_SUSPEND *p_av)
{
    APPL_TRACE_EVENT("## ON A2DP SUSPENDED ##\n");
    if (btif_media_cb.peer_sep == AVDT_TSEP_SRC) {
        btif_media_cb.rx_flush = TRUE;
        btif_media_task_aa_rx_flush_req();
        btif_media_task_aa_handle_stop_decoding();
        return;
    }
}

/* when true media task discards any rx frames */
void btif_a2dp_set_rx_flush(BOOLEAN enable)
{
    APPL_TRACE_EVENT("## DROP RX %d ##\n", enable);
    btif_media_cb.rx_flush = enable;
}

#if (BTA_AV_SINK_INCLUDED == TRUE)
static void btif_media_task_avk_handle_timer(UNUSED_ATTR void *context)
{
    UINT8 count;
    tBT_SBC_HDR *p_msg;
    int num_sbc_frames;
    int num_frames_to_process;

    count = btif_media_cb.RxSbcQ._count;
    if (0 == count) {
        APPL_TRACE_DEBUG("  QUE  EMPTY ");
    } else {
        if (btif_media_cb.rx_flush == TRUE) {
            btif_media_flush_q(&(btif_media_cb.RxSbcQ));
            return;
        }

        num_frames_to_process = btif_media_cb.frames_to_process;
        APPL_TRACE_DEBUG(" Process Frames + ");
        do {
            p_msg = (tBT_SBC_HDR *)GKI_getfirst(&(btif_media_cb.RxSbcQ));
            if (p_msg == NULL) {
                return;
            }
            num_sbc_frames  = p_msg->num_frames_to_be_processed; /* num of frames in Que Packets */
            APPL_TRACE_DEBUG(" Frames left in topmost packet %d\n", num_sbc_frames);
            APPL_TRACE_DEBUG(" Remaining frames to process in tick %d\n", num_frames_to_process);
            APPL_TRACE_DEBUG(" Num of Packets in Que %d\n", btif_media_cb.RxSbcQ._count);

            if ( num_sbc_frames > num_frames_to_process) { /*  Que Packet has more frames*/
                p_msg->num_frames_to_be_processed = num_frames_to_process;
                btif_media_task_handle_inc_media(p_msg);
                p_msg->num_frames_to_be_processed = num_sbc_frames - num_frames_to_process;
                num_frames_to_process = 0;
                break;
            } else {                                    /*  Que packet has less frames */
                btif_media_task_handle_inc_media(p_msg);
                p_msg = (tBT_SBC_HDR *)GKI_dequeue(&(btif_media_cb.RxSbcQ));
                if ( p_msg == NULL ) {
                    APPL_TRACE_ERROR("Insufficient data in que ");
                    break;
                }
                num_frames_to_process = num_frames_to_process - p_msg->num_frames_to_be_processed;
                GKI_freebuf(p_msg);
            }
        } while (num_frames_to_process > 0);

        APPL_TRACE_DEBUG(" Process Frames - ");
    }
}
#else
static void btif_media_task_avk_handle_timer(UNUSED_ATTR void *context) {}
#endif

static void btif_media_thread_init(UNUSED_ATTR void *context)
{
    memset(&btif_media_cb, 0, sizeof(btif_media_cb));
    LOG_ERROR("med thread init\n");
#if (UIPC_INCLUDED == TRUE)
    UIPC_Init(NULL);
    
#if (BTA_AV_INCLUDED == TRUE)
    UIPC_Open(UIPC_CH_ID_AV_CTRL , btif_a2dp_ctrl_cb);
#endif ( BTA_AV_INCLUDED == TRUE)
#endif /* UIPC_INCLUDED == TRUE */
    btif_media_cb.av_sm_hdl = btif_av_get_sm_handle();
    raise_priority_a2dp(TASK_HIGH_MEDIA);
    media_task_running = MEDIA_TASK_STATE_ON;
}

static void btif_media_thread_cleanup(UNUSED_ATTR void *context)
{
    /* make sure no channels are restarted while shutting down */
    media_task_running = MEDIA_TASK_STATE_SHUTTING_DOWN;

    btif_media_cb.data_channel_open = FALSE;
    /* Clear media task flag */
    media_task_running = MEDIA_TASK_STATE_OFF;
}

/*******************************************************************************
 **
 ** Function         btif_media_flush_q
 **
 ** Description
 **
 ** Returns          void
 **
 *******************************************************************************/
static void btif_media_flush_q(BUFFER_Q *p_q)
{
    while (!GKI_queue_is_empty(p_q)) {
        GKI_freebuf(GKI_dequeue(p_q));
    }
}

static void btif_media_thread_handle_cmd(fixed_queue_t *queue)
{
    BT_HDR *p_msg;
    while (!fixed_queue_is_empty(queue)) {
        p_msg = (BT_HDR *)fixed_queue_dequeue(queue);
        LOG_VERBOSE("btif_media_thread_handle_cmd : %d %s\n", p_msg->event,
                    dump_media_event(p_msg->event));

        switch (p_msg->event) {
#if (BTA_AV_INCLUDED == TRUE)
        case BTIF_MEDIA_AUDIO_SINK_CFG_UPDATE:
#if (BTA_AV_SINK_INCLUDED == TRUE)
            btif_media_task_aa_handle_decoder_reset(p_msg);
#endif
            break;
        case BTIF_MEDIA_AUDIO_SINK_CLEAR_TRACK:
#if (BTA_AV_SINK_INCLUDED == TRUE)
            btif_media_task_aa_handle_clear_track();
#endif
            break;
        case BTIF_MEDIA_FLUSH_AA_RX:
            btif_media_task_aa_rx_flush();
            break;
#endif
        default:
            APPL_TRACE_ERROR("ERROR in %s unknown event %d\n", __func__, p_msg->event);
        }
        GKI_freebuf(p_msg);
        LOG_VERBOSE("%s: %s DONE\n", __func__, dump_media_event(p_msg->event));
    }
}


#if (BTA_AV_SINK_INCLUDED == TRUE)
/*******************************************************************************
 **
 ** Function         btif_media_task_handle_inc_media
 **
 ** Description
 **
 ** Returns          void
 **
 *******************************************************************************/
static void btif_media_task_handle_inc_media(tBT_SBC_HDR *p_msg)
{
    UINT8 *sbc_start_frame = ((UINT8 *)(p_msg + 1) + p_msg->offset + 1);
    int count;
    UINT32 pcmBytes, availPcmBytes;
    OI_INT16 *pcmDataPointer = pcmData; /*Will be overwritten on next packet receipt*/
    OI_STATUS status;
    int num_sbc_frames = p_msg->num_frames_to_be_processed;
    UINT32 sbc_frame_len = p_msg->len - 1;
    availPcmBytes = 2 * sizeof(pcmData);

    if ((btif_media_cb.peer_sep == AVDT_TSEP_SNK) || (btif_media_cb.rx_flush)) {
        APPL_TRACE_DEBUG(" State Changed happened in this tick ");
        return;
    }

    // ignore data if no one is listening
    if (!btif_media_cb.data_channel_open) {
        return;
    }

    APPL_TRACE_DEBUG("Number of sbc frames %d, frame_len %d\n", num_sbc_frames, sbc_frame_len);
    // LOG_ERROR("Number of sbc frames %d, frame_len %d\n", num_sbc_frames, sbc_frame_len);
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
    // LOG_ERROR("pre-send: %d\n", availPcmBytes);

    // UIPC_Send(UIPC_CH_ID_AV_AUDIO, 0, (UINT8 *)pcmData, (2 * sizeof(pcmData) - availPcmBytes));
    BTIF_A2D_DATA_CB_TO_APP((uint8_t *)pcmData, (2 * sizeof(pcmData) - availPcmBytes));

}
#endif

#if (BTA_AV_INCLUDED == TRUE)

/*******************************************************************************
 **
 ** Function         btif_media_task_aa_rx_flush_req
 **
 ** Description
 **
 ** Returns          TRUE is success
 **
 *******************************************************************************/
BOOLEAN btif_media_task_aa_rx_flush_req(void)
{
    BT_HDR *p_buf;

    if (GKI_queue_is_empty(&(btif_media_cb.RxSbcQ)) == TRUE) { /*  Que is already empty */
        return TRUE;
    }

    if (NULL == (p_buf = GKI_getbuf(sizeof(BT_HDR)))) {
        return FALSE;
    }

    p_buf->event = BTIF_MEDIA_FLUSH_AA_RX;

    fixed_queue_enqueue(btif_media_cmd_msg_queue, p_buf);
    btif_media_task_post(SIG_MEDIA_TASK_CMD_READY);
    return TRUE;
}

/*******************************************************************************
 **
 ** Function         btif_media_task_aa_rx_flush
 **
 ** Description
 **
 ** Returns          void
 **
 *******************************************************************************/
static void btif_media_task_aa_rx_flush(void)
{
    /* Flush all enqueued GKI SBC  buffers (encoded) */
    APPL_TRACE_DEBUG("btif_media_task_aa_rx_flush");

    btif_media_flush_q(&(btif_media_cb.RxSbcQ));
}

int btif_a2dp_get_track_frequency(UINT8 frequency)
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

int btif_a2dp_get_track_channel_count(UINT8 channeltype)
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

void btif_a2dp_set_peer_sep(UINT8 sep)
{
    btif_media_cb.peer_sep = sep;
}

static void btif_decode_alarm_cb(UNUSED_ATTR void *context)
{
    btif_media_task_post(SIG_MEDIA_TASK_AVK_ALARM_TO);
}

static void btif_media_task_aa_handle_stop_decoding(void)
{
    osi_alarm_free(btif_media_cb.decode_alarm);
    btif_media_cb.decode_alarm = NULL;
}

static void btif_media_task_aa_handle_start_decoding(void)
{
    if (btif_media_cb.decode_alarm) {
        return;
    }

    btif_media_cb.decode_alarm = osi_alarm_new("dec_timer\n", btif_decode_alarm_cb, NULL, BTIF_SINK_MEDIA_TIME_TICK, true);
    if (!btif_media_cb.decode_alarm) {
        APPL_TRACE_ERROR("%s unable to allocate decode alarm.\n", __func__);
        return;
    }
    osi_alarm_set(btif_media_cb.decode_alarm, BTIF_SINK_MEDIA_TIME_TICK);
}

#if (BTA_AV_SINK_INCLUDED == TRUE)

static void btif_media_task_aa_handle_clear_track (void)
{
    APPL_TRACE_DEBUG("btif_media_task_aa_handle_clear_track");
}

/*******************************************************************************
 **
 ** Function         btif_media_task_aa_handle_decoder_reset
 **
 ** Description
 **
 ** Returns          void
 **
 *******************************************************************************/
static void btif_media_task_aa_handle_decoder_reset(BT_HDR *p_msg)
{
    tBTIF_MEDIA_SINK_CFG_UPDATE *p_buf = (tBTIF_MEDIA_SINK_CFG_UPDATE *) p_msg;
    tA2D_STATUS a2d_status;
    tA2D_SBC_CIE sbc_cie;
    OI_STATUS       status;
    UINT32          freq_multiple = 48 * 20; /* frequency multiple for 20ms of data , initialize with 48K*/
    UINT32          num_blocks = 16;
    UINT32          num_subbands = 8;

    APPL_TRACE_EVENT("btif_media_task_aa_handle_decoder_reset p_codec_info[%x:%x:%x:%x:%x:%x]\n",
              p_buf->codec_info[1], p_buf->codec_info[2], p_buf->codec_info[3],
              p_buf->codec_info[4], p_buf->codec_info[5], p_buf->codec_info[6]);

    a2d_status = A2D_ParsSbcInfo(&sbc_cie, p_buf->codec_info, FALSE);
    if (a2d_status != A2D_SUCCESS) {
        APPL_TRACE_ERROR("ERROR dump_codec_info A2D_ParsSbcInfo fail:%d\n", a2d_status);
        return;
    }

    btif_media_cb.sample_rate = btif_a2dp_get_track_frequency(sbc_cie.samp_freq);
    btif_media_cb.channel_count = btif_a2dp_get_track_channel_count(sbc_cie.ch_mode);

    btif_media_cb.rx_flush = FALSE;
    APPL_TRACE_EVENT("Reset to sink role");
    status = OI_CODEC_SBC_DecoderReset(&context, contextData, sizeof(contextData), 2, 2, FALSE);
    if (!OI_SUCCESS(status)) {
        APPL_TRACE_ERROR("OI_CODEC_SBC_DecoderReset failed with error code %d\n", status);
    }

    btif_media_cb.data_channel_open = TRUE;

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

    btif_media_cb.frames_to_process = ((freq_multiple) / (num_blocks * num_subbands)) + 1;
    APPL_TRACE_EVENT(" Frames to be processed in 20 ms %d\n", btif_media_cb.frames_to_process);
}
#endif

/*******************************************************************************
 **
 ** Function         btif_media_sink_enque_buf
 **
 ** Description      This function is called by the av_co to fill A2DP Sink Queue
 **
 **
 ** Returns          size of the queue
 *******************************************************************************/
UINT8 btif_media_sink_enque_buf(BT_HDR *p_pkt)
{
    tBT_SBC_HDR *p_msg;

    if (btif_media_cb.rx_flush == TRUE) { /* Flush enabled, do not enque*/
        return GKI_queue_length(&btif_media_cb.RxSbcQ);
    }
    if (GKI_queue_length(&btif_media_cb.RxSbcQ) == MAX_OUTPUT_A2DP_FRAME_QUEUE_SZ) {
        GKI_freebuf(GKI_dequeue(&(btif_media_cb.RxSbcQ)));
    }

    BTIF_TRACE_VERBOSE("btif_media_sink_enque_buf + ");

    /* allocate and Queue this buffer */
    if ((p_msg = (tBT_SBC_HDR *) GKI_getbuf(sizeof(tBT_SBC_HDR) +
                                            p_pkt->offset + p_pkt->len)) != NULL) {
        memcpy(p_msg, p_pkt, (sizeof(BT_HDR) + p_pkt->offset + p_pkt->len));
        p_msg->num_frames_to_be_processed = (*((UINT8 *)(p_msg + 1) + p_msg->offset)) & 0x0f;
        BTIF_TRACE_VERBOSE("btif_media_sink_enque_buf %d + \n", p_msg->num_frames_to_be_processed);
        // LOG_ERROR("sink enq %d\n", p_msg->num_frames_to_be_processed);
        GKI_enqueue(&(btif_media_cb.RxSbcQ), p_msg);
        if (GKI_queue_length(&btif_media_cb.RxSbcQ) == MAX_A2DP_DELAYED_START_FRAME_COUNT) {
            BTIF_TRACE_DEBUG(" Initiate Decoding ");
            btif_media_task_aa_handle_start_decoding();
        }
    } else {
        /* let caller deal with a failed allocation */
        BTIF_TRACE_VERBOSE("btif_media_sink_enque_buf No Buffer left - ");
    }
    return GKI_queue_length(&btif_media_cb.RxSbcQ);
}


/*******************************************************************************
 **
 ** Function         btif_media_aa_readbuf
 **
 ** Description      This function is called by the av_co to get the next buffer to send
 **
 **
 ** Returns          void
 *******************************************************************************/
BT_HDR *btif_media_aa_readbuf(void)
{
    return NULL;
}

#endif /* BTA_AV_INCLUDED == TRUE */
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
