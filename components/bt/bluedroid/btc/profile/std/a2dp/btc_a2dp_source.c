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
 **  Name:          btc_a2dp_source.c
 **
 ******************************************************************************/
#include "bt_target.h"
#include "bt_trace.h"
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include "allocator.h"
#include "alarm.h"
#include "thread.h"
#include "mutex.h"
#include "fixed_queue.h"
#include "a2d_api.h"
#include "a2d_sbc.h"
#include "bta_av_api.h"
#include "bta_av_sbc.h"
#include "bta_av_ci.h"
#include "btc_manage.h"
#include "btc_common.h"
#include "btc_av_co.h"
#include "btc_a2dp.h"
#include "btc_a2dp_control.h"
#include "btc_a2dp_source.h"
#include "btc_av.h"
#include "btc_util.h"
#include "esp_a2dp_api.h"
#include "sbc_encoder.h"

#if BTC_AV_SRC_INCLUDED

/*****************************************************************************
 **  Constants
 *****************************************************************************/

/* BTC source command event definition */
enum {
    BTC_MEDIA_TASK_INIT,
    BTC_MEDIA_TASK_CLEAN_UP,
    BTC_MEDIA_START_AA_TX,
    BTC_MEDIA_STOP_AA_TX,
    BTC_MEDIA_SBC_ENC_INIT,
    BTC_MEDIA_SBC_ENC_UPDATE,
    BTC_MEDIA_FLUSH_AA_TX,
    BTC_MEDIA_AUDIO_FEEDING_INIT,
};

enum {
    BTC_A2DP_SOURCE_STATE_OFF = 0,
    BTC_A2DP_SOURCE_STATE_ON = 1,
    BTC_A2DP_SOURCE_STATE_SHUTTING_DOWN = 2
};

enum {
    BTC_A2DP_SOURCE_DATA_EVT = 1,
};

/* Media task tick in milliseconds, must be set to multiple of
   (1000/TICKS_PER_SEC) */
#define BTC_MEDIA_TIME_TICK_MS                 (30)
#define A2DP_DATA_READ_POLL_MS                 (BTC_MEDIA_TIME_TICK_MS / 2)

#ifndef MAX_PCM_FRAME_NUM_PER_TICK
#define MAX_PCM_FRAME_NUM_PER_TICK             21 // 14 for 20ms
#endif

#define BTC_MEDIA_AA_BUF_SIZE                  (4096+16)

#if (BTA_AV_CO_CP_SCMS_T == TRUE)
#define BTC_MEDIA_AA_SBC_OFFSET (AVDT_MEDIA_OFFSET + BTA_AV_SBC_HDR_SIZE + 1)
#else
#define BTC_MEDIA_AA_SBC_OFFSET (AVDT_MEDIA_OFFSET + BTA_AV_SBC_HDR_SIZE)
#endif

#ifndef BTC_MEDIA_BITRATE_STEP
#define BTC_MEDIA_BITRATE_STEP                 5
#endif

#ifndef BTC_A2DP_NON_EDR_MAX_RATE
#define BTC_A2DP_NON_EDR_MAX_RATE              229
#endif

/* Middle quality quality setting @ 44.1 khz */
#define DEFAULT_SBC_BITRATE                    328

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
#define MAX_OUTPUT_A2DP_SRC_FRAME_QUEUE_SZ     (27) // 18 for 20ms tick

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
} tBTC_AV_MEDIA_FEEDINGS_PCM_STATE;

typedef union {
    tBTC_AV_MEDIA_FEEDINGS_PCM_STATE pcm;
} tBTC_AV_MEDIA_FEEDINGS_STATE;

typedef struct {
    UINT8 TxTranscoding;
    BOOLEAN tx_flush; /* discards any outgoing data when true */
    BOOLEAN is_tx_timer;
    UINT16 TxAaMtuSize;
    UINT32 timestamp;
    fixed_queue_t *TxAaQ;
    tBTC_AV_FEEDING_MODE feeding_mode;
    tBTC_AV_MEDIA_FEEDINGS_STATE media_feeding_state;
    tBTC_AV_MEDIA_FEEDINGS media_feeding;
    SBC_ENC_PARAMS encoder;
    osi_alarm_t *media_alarm;
} tBTC_A2DP_SOURCE_CB;

static void btc_a2dp_source_thread_init(UNUSED_ATTR void *context);
static void btc_a2dp_source_thread_cleanup(UNUSED_ATTR void *context);
static void btc_a2dp_source_flush_q(fixed_queue_t *p_q);

static void btc_a2dp_source_feeding_state_reset(void);
static void btc_a2dp_source_send_aa_frame(void);
static void btc_a2dp_source_aa_start_tx(void);
static void btc_a2dp_source_aa_stop_tx(void);
static void btc_a2dp_source_enc_init(BT_HDR *p_msg);
static void btc_a2dp_source_enc_update(BT_HDR *p_msg);
static void btc_a2dp_source_audio_feeding_init(BT_HDR *p_msg);
static void btc_a2dp_source_aa_tx_flush(void);
static void btc_a2dp_source_prep_2_send(UINT8 nb_frame);
static void btc_a2dp_source_handle_timer(UNUSED_ATTR void *context);
static void btc_a2dp_source_encoder_init(void);

static tBTC_A2DP_SOURCE_CB btc_aa_src_cb;
static int btc_a2dp_source_state = BTC_A2DP_SOURCE_STATE_OFF;
static xTaskHandle  btc_aa_src_task_hdl = NULL;
static QueueHandle_t btc_aa_src_data_queue = NULL;
static QueueHandle_t btc_aa_src_ctrl_queue = NULL;
static QueueSetHandle_t btc_aa_src_queue_set;

static esp_a2d_source_data_cb_t btc_aa_src_data_cb = NULL;
static UINT64 last_frame_us = 0;

void btc_a2dp_src_reg_data_cb(esp_a2d_source_data_cb_t callback)
{
    // todo: critical section protection
    btc_aa_src_data_cb = callback;
}

static inline uint32_t btc_aa_src_data_read(uint8_t *data, int32_t len)
{
    // todo: critical section protection
    if (btc_aa_src_data_cb) {
        return btc_aa_src_data_cb(data, len);
    } else {
        return 0;
    }
}

/*****************************************************************************
 **  Misc helper functions
 *****************************************************************************/
static inline void btc_aa_cb_to_app(esp_a2d_cb_event_t event, esp_a2d_cb_param_t *param)
{
    esp_a2d_cb_t btc_aa_cb = (esp_a2d_cb_t)btc_profile_cb_get(BTC_PID_A2DP);
    if (btc_aa_cb) {
        btc_aa_cb(event, param);
    }
}

/*****************************************************************************
 **  BTC ADAPTATION
 *****************************************************************************/

bool btc_a2dp_source_is_streaming(void)
{
    return btc_aa_src_cb.is_tx_timer == TRUE;
}

bool btc_a2dp_source_is_task_shutting_down(void)
{
    return btc_a2dp_source_state == BTC_A2DP_SOURCE_STATE_SHUTTING_DOWN;
}

static void btc_a2dp_source_ctrl_post(uint32_t sig, void *par)
{
    BtTaskEvt_t *evt = (BtTaskEvt_t *)osi_malloc(sizeof(BtTaskEvt_t));
    if (evt == NULL) {
        return;
    }

    evt->sig = sig;
    evt->par = par;

    if (xQueueSend(btc_aa_src_ctrl_queue, &evt, portMAX_DELAY) != pdTRUE) {
        APPL_TRACE_WARNING("btc_aa_src_ctrl_queue failed, sig 0x%x\n", sig);
    }
}

static void btc_a2dp_source_ctrl_handler(BtTaskEvt_t *e)
{
    if (e == NULL) {
        return;
    }
    switch (e->sig) {
    case BTC_MEDIA_TASK_INIT:
        btc_a2dp_source_thread_init(NULL);
        break;
    case BTC_MEDIA_TASK_CLEAN_UP:
        btc_a2dp_source_thread_cleanup(NULL);
        break;
    case BTC_MEDIA_START_AA_TX:
        btc_a2dp_source_aa_start_tx();
        break;
    case BTC_MEDIA_STOP_AA_TX:
        btc_a2dp_source_aa_stop_tx();
        break;
    case BTC_MEDIA_SBC_ENC_INIT:
        btc_a2dp_source_enc_init(e->par);
        break;
    case BTC_MEDIA_SBC_ENC_UPDATE:
        btc_a2dp_source_enc_update(e->par);
        break;
    case BTC_MEDIA_AUDIO_FEEDING_INIT:
        btc_a2dp_source_audio_feeding_init(e->par);
        break;
    case BTC_MEDIA_FLUSH_AA_TX:
        btc_a2dp_source_aa_tx_flush();
        break;
    default:
        APPL_TRACE_WARNING("media task unhandled evt: 0x%x\n", e->sig);
    }
    if (e->par != NULL) {
        osi_free(e->par);
    }
}

static void btc_a2dp_source_task_handler(void *arg)
{
    QueueSetMemberHandle_t xActivatedMember;
    BtTaskEvt_t *e = NULL;
    for (;;) {
        xActivatedMember = xQueueSelectFromSet(btc_aa_src_queue_set, portMAX_DELAY);
        if (xActivatedMember == btc_aa_src_data_queue) {
            int32_t data_evt;
            xQueueReceive(xActivatedMember, &data_evt, 0);
            if (data_evt == BTC_A2DP_SOURCE_DATA_EVT) {
                btc_a2dp_source_handle_timer(NULL);
            }
        } else if (xActivatedMember == btc_aa_src_ctrl_queue) {
            xQueueReceive(xActivatedMember, &e, 0);
            btc_a2dp_source_ctrl_handler(e);
            osi_free(e);
        }
    }
}

bool btc_a2dp_source_startup(void)
{
    if (btc_a2dp_source_state != BTC_A2DP_SOURCE_STATE_OFF) {
        APPL_TRACE_ERROR("warning : media task already running");
        return false;
    }

    APPL_TRACE_EVENT("## A2DP SOURCE START MEDIA THREAD ##");

    btc_aa_src_queue_set = xQueueCreateSet(BTC_A2DP_SOURCE_TASK_QUEUE_SET_LEN);
    configASSERT(btc_aa_src_queue_set);
    btc_aa_src_data_queue = xQueueCreate(BTC_A2DP_SOURCE_DATA_QUEUE_LEN, sizeof(void *));
    configASSERT(btc_aa_src_data_queue);
    xQueueAddToSet(btc_aa_src_data_queue, btc_aa_src_queue_set);

    btc_aa_src_ctrl_queue = xQueueCreate(BTC_A2DP_SOURCE_CTRL_QUEUE_LEN, sizeof(void *));
    configASSERT(btc_aa_src_ctrl_queue);
    xQueueAddToSet(btc_aa_src_ctrl_queue, btc_aa_src_queue_set);

    if (!btc_aa_src_data_queue || !btc_aa_src_ctrl_queue || !btc_aa_src_queue_set ) {
        goto error_exit;
    }

    xTaskCreatePinnedToCore(btc_a2dp_source_task_handler, BTC_A2DP_SOURCE_TASK_NAME, BTC_A2DP_SOURCE_TASK_STACK_SIZE, NULL, BTC_A2DP_SOURCE_TASK_PRIO, &btc_aa_src_task_hdl, BTC_A2DP_SOURCE_TASK_PINNED_TO_CORE);
    if (btc_aa_src_task_hdl == NULL) {
        goto error_exit;
    }

    btc_a2dp_source_ctrl_post(BTC_MEDIA_TASK_INIT, NULL);

    APPL_TRACE_EVENT("## A2DP SOURCE MEDIA THREAD STARTED ##\n");

    return true;

error_exit:;
    APPL_TRACE_ERROR("%s unable to start up media thread\n", __func__);

    if (btc_aa_src_task_hdl != NULL) {
        vTaskDelete(btc_aa_src_task_hdl);
        btc_aa_src_task_hdl = NULL;
    }

    if (btc_aa_src_data_queue) {
        vQueueDelete(btc_aa_src_data_queue);
        btc_aa_src_data_queue = NULL;
    }
    if (btc_aa_src_ctrl_queue) {
        vQueueDelete(btc_aa_src_ctrl_queue);
        btc_aa_src_ctrl_queue = NULL;
    }

    return false;
}

void btc_a2dp_source_shutdown(void)
{
    APPL_TRACE_EVENT("## A2DP SOURCE STOP MEDIA THREAD ##\n");

    // Exit thread
    btc_a2dp_source_ctrl_post(BTC_MEDIA_TASK_CLEAN_UP, NULL);

    vTaskDelete(btc_aa_src_task_hdl);
    btc_aa_src_task_hdl = NULL;

    vQueueDelete(btc_aa_src_data_queue);
    btc_aa_src_data_queue = NULL;

    vQueueDelete(btc_aa_src_ctrl_queue);
    btc_aa_src_ctrl_queue = NULL;
}

/*****************************************************************************
**
** Function        btc_a2dp_source_on_idle
**
*******************************************************************************/
void btc_a2dp_source_on_idle(void)
{
    /* Make sure media task is stopped */
    btc_a2dp_source_stop_audio_req();
}

/*****************************************************************************
**
** Function        btc_a2dp_source_on_stopped
**
*******************************************************************************/
void btc_a2dp_source_on_stopped(tBTA_AV_SUSPEND *p_av)
{
    /* allow using this api for other than suspend */
    if (p_av != NULL) {
        if (p_av->status != BTA_AV_SUCCESS) {
            APPL_TRACE_EVENT("AV STOP FAILED (%d)", p_av->status);
            if (p_av->initiator) {
                btc_a2dp_control_command_ack(ESP_A2D_MEDIA_CTRL_ACK_FAILURE);
            }
            return;
        }
    }

    /* ensure tx frames are immediately suspended */
    btc_aa_src_cb.tx_flush = 1;

    /* request to stop media task  */
    btc_a2dp_source_tx_flush_req();
    btc_a2dp_source_stop_audio_req();

    /* once stream is fully stopped we will ack back */
}

/*****************************************************************************
**
** Function        btc_a2dp_source_on_suspended
**
**
*******************************************************************************/

void btc_a2dp_source_on_suspended(tBTA_AV_SUSPEND *p_av)
{
    /* check for status failures */
    if (p_av->status != BTA_AV_SUCCESS) {
        if (p_av->initiator == TRUE) {
            btc_a2dp_control_command_ack(ESP_A2D_MEDIA_CTRL_ACK_FAILURE);
        }
    }

    /* once stream is fully stopped we will ack back */

    /* ensure tx frames are immediately flushed */
    btc_aa_src_cb.tx_flush = 1;

    /* stop timer tick */
    btc_a2dp_source_stop_audio_req();
}

static void btc_a2dp_source_data_post(int32_t data_type)
{
    if (xQueueSend(btc_aa_src_data_queue, &data_type, 0) != pdTRUE) {
        APPL_TRACE_DEBUG("Media data Q filled\n");
    }
}

static UINT64 time_now_us()
{
#if _POSIX_TIMERS
    struct timespec ts_now;
    clock_gettime(CLOCK_BOOTTIME, &ts_now);
    return ((UINT64)ts_now.tv_sec * 1000000L) + ((UINT64)ts_now.tv_nsec / 1000);
#else
    struct timeval ts_now;
    gettimeofday(&ts_now, NULL);
    return ((UINT64)ts_now.tv_sec * 1000000L) + ((UINT64)ts_now.tv_usec);
#endif
}

static void log_tstamps_us(char *comment)
{
    static UINT64 prev_us = 0;
    const UINT64 now_us = time_now_us();
    APPL_TRACE_DEBUG("[%s] ts %08llu, diff : %08llu, queue sz %d", comment, now_us, now_us - prev_us,
                     fixed_queue_length(btc_aa_src_cb.TxAaQ));
    prev_us = now_us;
}

/* when true media task discards any tx frames */
void btc_a2dp_source_set_tx_flush(BOOLEAN enable)
{
    APPL_TRACE_EVENT("## DROP TX %d ##", enable);
    btc_aa_src_cb.tx_flush = enable;
}

/*****************************************************************************
**
** Function        btc_a2dp_source_setup_codec
**
** Description
**
** Returns
**
*******************************************************************************/

void btc_a2dp_source_setup_codec(void)
{
    tBTC_AV_MEDIA_FEEDINGS media_feeding;
    tBTC_AV_STATUS status;

    APPL_TRACE_EVENT("## A2DP SETUP CODEC ##\n");

    osi_mutex_global_lock();

    /* for now hardcode 44.1 khz 16 bit stereo PCM format */
    media_feeding.cfg.pcm.sampling_freq = 44100;
    media_feeding.cfg.pcm.bit_per_sample = 16;
    media_feeding.cfg.pcm.num_channel = 2;
    media_feeding.format = BTC_AV_CODEC_PCM;

    if (bta_av_co_audio_set_codec(&media_feeding, &status)) {
        tBTC_MEDIA_INIT_AUDIO_FEEDING mfeed;

        /* Init the encoding task */
        btc_a2dp_source_encoder_init();

        /* Build the media task configuration */
        mfeed.feeding = media_feeding;
        mfeed.feeding_mode = BTC_AV_FEEDING_ASYNCHRONOUS;
        /* Send message to Media task to configure transcoding */
        btc_a2dp_source_audio_feeding_init_req(&mfeed);
    }

    osi_mutex_global_unlock();
}


/*******************************************************************************
 **
 ** Function         btc_a2dp_source_audio_readbuf
 **
 ** Description      This function is called by the av_co to get the next buffer to send
 **
 **
 ** Returns          void
 *******************************************************************************/
BT_HDR *btc_a2dp_source_audio_readbuf(void)
{
    return fixed_queue_try_dequeue(btc_aa_src_cb.TxAaQ);
}

/*******************************************************************************
 **
 ** Function         btc_a2dp_source_start_audio_req
 **
 ** Description
 **
 ** Returns          TRUE is success
 **
 *******************************************************************************/
BOOLEAN btc_a2dp_source_start_audio_req(void)
{
    btc_a2dp_source_ctrl_post(BTC_MEDIA_START_AA_TX, NULL);
    return TRUE;
}

/*******************************************************************************
 **
 ** Function         btc_a2dp_source_stop_audio_req
 **
 ** Description
 **
 ** Returns          TRUE is success
 **
 *******************************************************************************/
BOOLEAN btc_a2dp_source_stop_audio_req(void)
{
    /*
     * Explicitly check whether the btc_aa_src_ctrl_queue is not NULL to
     * avoid a race condition during shutdown of the Bluetooth stack.
     * This race condition is triggered when A2DP audio is streaming on
     * shutdown:
     * "btc_a2dp_on_stopped() -> btc_a2dp_source_stop_audio_req()" is called
     * to stop the particular audio stream, and this happens right after
     * the "cleanup() -> btc_a2dp_stop_media_task()" processing during
     * the shutdown of the Bluetooth stack.
     */
    if (btc_aa_src_ctrl_queue != NULL) {
        btc_a2dp_source_ctrl_post(BTC_MEDIA_STOP_AA_TX, NULL);
    }
    return TRUE;
}

/*******************************************************************************
 **
 ** Function         btc_a2dp_source_enc_init_req
 **
 ** Description
 **
 ** Returns          TRUE is success
 **
 *******************************************************************************/
BOOLEAN btc_a2dp_source_enc_init_req(tBTC_MEDIA_INIT_AUDIO *p_msg)
{
    tBTC_MEDIA_INIT_AUDIO *p_buf;
    if (NULL == (p_buf = osi_malloc(sizeof(tBTC_MEDIA_INIT_AUDIO)))) {
        return FALSE;
    }

    memcpy(p_buf, p_msg, sizeof(tBTC_MEDIA_INIT_AUDIO));

    btc_a2dp_source_ctrl_post(BTC_MEDIA_SBC_ENC_INIT, p_buf);

    return TRUE;
}

/*******************************************************************************
 **
 ** Function         btc_a2dp_source_enc_update_req
 **
 ** Description
 **
 ** Returns          TRUE is success
 **
 *******************************************************************************/
BOOLEAN btc_a2dp_source_enc_update_req(tBTC_MEDIA_UPDATE_AUDIO *p_msg)
{
    tBTC_MEDIA_UPDATE_AUDIO *p_buf;
    if (NULL == (p_buf = osi_malloc(sizeof(tBTC_MEDIA_UPDATE_AUDIO)))) {
        return FALSE;
    }

    memcpy(p_buf, p_msg, sizeof(tBTC_MEDIA_UPDATE_AUDIO));
    btc_a2dp_source_ctrl_post(BTC_MEDIA_SBC_ENC_UPDATE, p_buf);
    return TRUE;
}

/*******************************************************************************
 **
 ** Function         btc_a2dp_source_audio_feeding_init_req
 **
 ** Description
 **
 ** Returns          TRUE is success
 **
 *******************************************************************************/
BOOLEAN btc_a2dp_source_audio_feeding_init_req(tBTC_MEDIA_INIT_AUDIO_FEEDING *p_msg)
{
    tBTC_MEDIA_INIT_AUDIO_FEEDING *p_buf;
    if (NULL == (p_buf = osi_malloc(sizeof(tBTC_MEDIA_INIT_AUDIO_FEEDING)))) {
        return FALSE;
    }

    memcpy(p_buf, p_msg, sizeof(tBTC_MEDIA_INIT_AUDIO_FEEDING));
    btc_a2dp_source_ctrl_post(BTC_MEDIA_AUDIO_FEEDING_INIT, p_buf);
    return TRUE;
}

/*******************************************************************************
 **
 ** Function         btc_a2dp_source_tx_flush_req
 **
 ** Description
 **
 ** Returns          TRUE is success
 **
 *******************************************************************************/
BOOLEAN btc_a2dp_source_tx_flush_req(void)
{
    /*
     * Explicitly check whether the btc_aa_src_ctrl_queue is not NULL to
     * avoid a race condition during shutdown of the Bluetooth stack.
     * This race condition is triggered when A2DP audio is streaming on
     * shutdown:
     * "btc_a2dp_on_stopped() -> btc_a2dp_source_tx_flush_req()" is called
     * to stop the particular audio stream, and this happens right after
     * the "cleanup() -> btc_a2dp_stop_media_task()" processing during
     * the shutdown of the Bluetooth stack.
     */
    if (btc_aa_src_ctrl_queue != NULL) {
        btc_a2dp_source_ctrl_post(BTC_MEDIA_FLUSH_AA_TX, NULL);
    }

    return TRUE;
}

/*****************************************************************************
 **  BTC ADAPTATION
 *****************************************************************************/
static UINT16 btc_a2dp_source_get_sbc_rate(void)
{
    UINT16 rate = DEFAULT_SBC_BITRATE;

    /* restrict bitrate if a2dp link is non-edr */
    if (!btc_av_is_peer_edr()) {
        rate = BTC_A2DP_NON_EDR_MAX_RATE;
        APPL_TRACE_DEBUG("non-edr a2dp sink detected, restrict rate to %d", rate);
    }
    return rate;
}

static void btc_a2dp_source_encoder_init(void)
{
    UINT16 minmtu;
    tBTC_MEDIA_INIT_AUDIO msg;
    tA2D_SBC_CIE sbc_config;

    /* lookup table for converting channel mode */
    UINT16 codec_mode_tbl[5] = { SBC_JOINT_STEREO, SBC_STEREO, SBC_DUAL, 0, SBC_MONO };

    /* lookup table for converting number of blocks */
    UINT16 codec_block_tbl[5] = { 16, 12, 8, 0, 4 };

    /* lookup table to convert freq */
    UINT16 freq_block_tbl[5] = { SBC_sf48000, SBC_sf44100, SBC_sf32000, 0, SBC_sf16000 };

    APPL_TRACE_DEBUG("%s", __FUNCTION__);

    /* Retrieve the current SBC configuration (default if currently not used) */
    bta_av_co_audio_get_sbc_config(&sbc_config, &minmtu);
    msg.NumOfSubBands = (sbc_config.num_subbands == A2D_SBC_IE_SUBBAND_4) ? 4 : 8;
    msg.NumOfBlocks = codec_block_tbl[sbc_config.block_len >> 5];
    msg.AllocationMethod = (sbc_config.alloc_mthd == A2D_SBC_IE_ALLOC_MD_L) ? SBC_LOUDNESS : SBC_SNR;
    msg.ChannelMode = codec_mode_tbl[sbc_config.ch_mode >> 1];
    msg.SamplingFreq = freq_block_tbl[sbc_config.samp_freq >> 5];
    msg.MtuSize = minmtu;

    APPL_TRACE_EVENT("msg.ChannelMode %x", msg.ChannelMode);

    /* Init the media task to encode SBC properly */
    btc_a2dp_source_enc_init_req(&msg);
}

void btc_a2dp_source_encoder_update(void)
{
    UINT16 minmtu;
    tA2D_SBC_CIE sbc_config;
    tBTC_MEDIA_UPDATE_AUDIO msg;
    UINT8 pref_min;
    UINT8 pref_max;

    APPL_TRACE_DEBUG("%s", __FUNCTION__);

    /* Retrieve the current SBC configuration (default if currently not used) */
    bta_av_co_audio_get_sbc_config(&sbc_config, &minmtu);

    APPL_TRACE_DEBUG("%s: Common min_bitpool:%d(0x%x) max_bitpool:%d(0x%x)", __FUNCTION__,
                     sbc_config.min_bitpool, sbc_config.min_bitpool,
                     sbc_config.max_bitpool, sbc_config.max_bitpool);

    if (sbc_config.min_bitpool > sbc_config.max_bitpool) {
        APPL_TRACE_ERROR("%s: ERROR min_bitpool > max_bitpool", __FUNCTION__);
    }

    /* check if remote sink has a preferred bitpool range */
    if (bta_av_co_get_remote_bitpool_pref(&pref_min, &pref_max) == TRUE) {
        /* adjust our preferred bitpool with the remote preference if within
           our capable range */

        if (pref_min < sbc_config.min_bitpool) {
            pref_min = sbc_config.min_bitpool;
        }

        if (pref_max > sbc_config.max_bitpool) {
            pref_max = sbc_config.max_bitpool;
        }

        msg.MinBitPool = pref_min;
        msg.MaxBitPool = pref_max;

        if ((pref_min != sbc_config.min_bitpool) || (pref_max != sbc_config.max_bitpool)) {
            APPL_TRACE_EVENT("## adjusted our bitpool range to peer pref [%d:%d] ##",
                             pref_min, pref_max);
        }
    } else {
        msg.MinBitPool = sbc_config.min_bitpool;
        msg.MaxBitPool = sbc_config.max_bitpool;
    }

    msg.MinMtuSize = minmtu;

    /* Update the media task to encode SBC properly */
    btc_a2dp_source_enc_update_req(&msg);
}

/*******************************************************************************
 **
 ** Function       btc_a2dp_source_enc_init
 **
 ** Description    Initialize encoding task
 **
 ** Returns        void
 **
 *******************************************************************************/
static void btc_a2dp_source_enc_init(BT_HDR *p_msg)
{
    tBTC_MEDIA_INIT_AUDIO *pInitAudio = (tBTC_MEDIA_INIT_AUDIO *) p_msg;

    APPL_TRACE_DEBUG("btc_a2dp_source_enc_init");

    btc_aa_src_cb.timestamp = 0;

    /* SBC encoder config (enforced even if not used) */
    btc_aa_src_cb.encoder.s16ChannelMode = pInitAudio->ChannelMode;
    btc_aa_src_cb.encoder.s16NumOfSubBands = pInitAudio->NumOfSubBands;
    btc_aa_src_cb.encoder.s16NumOfBlocks = pInitAudio->NumOfBlocks;
    btc_aa_src_cb.encoder.s16AllocationMethod = pInitAudio->AllocationMethod;
    btc_aa_src_cb.encoder.s16SamplingFreq = pInitAudio->SamplingFreq;

    btc_aa_src_cb.encoder.u16BitRate = btc_a2dp_source_get_sbc_rate();

    /* Default transcoding is PCM to SBC, modified by feeding configuration */
    btc_aa_src_cb.TxTranscoding = BTC_MEDIA_TRSCD_PCM_2_SBC;
    btc_aa_src_cb.TxAaMtuSize = ((BTC_MEDIA_AA_BUF_SIZE - BTC_MEDIA_AA_SBC_OFFSET - sizeof(BT_HDR))
                                 < pInitAudio->MtuSize) ? (BTC_MEDIA_AA_BUF_SIZE - BTC_MEDIA_AA_SBC_OFFSET
                                         - sizeof(BT_HDR)) : pInitAudio->MtuSize;

    APPL_TRACE_EVENT("btc_a2dp_source_enc_init mtu %d, peer mtu %d",
                     btc_aa_src_cb.TxAaMtuSize, pInitAudio->MtuSize);
    APPL_TRACE_EVENT("      ch mode %d, subnd %d, nb blk %d, alloc %d, rate %d, freq %d",
                     btc_aa_src_cb.encoder.s16ChannelMode, btc_aa_src_cb.encoder.s16NumOfSubBands,
                     btc_aa_src_cb.encoder.s16NumOfBlocks,
                     btc_aa_src_cb.encoder.s16AllocationMethod, btc_aa_src_cb.encoder.u16BitRate,
                     btc_aa_src_cb.encoder.s16SamplingFreq);

    /* Reset entirely the SBC encoder */
    SBC_Encoder_Init(&(btc_aa_src_cb.encoder));
    APPL_TRACE_DEBUG("btc_a2dp_source_enc_init bit pool %d", btc_aa_src_cb.encoder.s16BitPool);
}


/*******************************************************************************
 **
 ** Function       btc_a2dp_source_enc_update
 **
 ** Description    Update encoding task
 **
 ** Returns        void
 **
 *******************************************************************************/

static void btc_a2dp_source_enc_update(BT_HDR *p_msg)
{
    tBTC_MEDIA_UPDATE_AUDIO *pUpdateAudio = (tBTC_MEDIA_UPDATE_AUDIO *) p_msg;
    SBC_ENC_PARAMS *pstrEncParams = &btc_aa_src_cb.encoder;
    UINT16 s16SamplingFreq;
    SINT16 s16BitPool = 0;
    SINT16 s16BitRate;
    SINT16 s16FrameLen;
    UINT8 protect = 0;

    APPL_TRACE_DEBUG("%s : minmtu %d, maxbp %d minbp %d", __FUNCTION__,
                     pUpdateAudio->MinMtuSize, pUpdateAudio->MaxBitPool, pUpdateAudio->MinBitPool);

    /* Only update the bitrate and MTU size while timer is running to make sure it has been initialized */
    //if (btc_aa_src_cb.is_tx_timer)
    {
        btc_aa_src_cb.TxAaMtuSize = ((BTC_MEDIA_AA_BUF_SIZE -
                                      BTC_MEDIA_AA_SBC_OFFSET - sizeof(BT_HDR))
                                     < pUpdateAudio->MinMtuSize) ? (BTC_MEDIA_AA_BUF_SIZE - BTC_MEDIA_AA_SBC_OFFSET
                                             - sizeof(BT_HDR)) : pUpdateAudio->MinMtuSize;

        /* Set the initial target bit rate */
        pstrEncParams->u16BitRate = btc_a2dp_source_get_sbc_rate();

        if (pstrEncParams->s16SamplingFreq == SBC_sf16000) {
            s16SamplingFreq = 16000;
        } else if (pstrEncParams->s16SamplingFreq == SBC_sf32000) {
            s16SamplingFreq = 32000;
        } else if (pstrEncParams->s16SamplingFreq == SBC_sf44100) {
            s16SamplingFreq = 44100;
        } else {
            s16SamplingFreq = 48000;
        }

        do {
            if (pstrEncParams->s16NumOfBlocks == 0 || pstrEncParams->s16NumOfSubBands == 0
                    || pstrEncParams->s16NumOfChannels == 0) {
                APPL_TRACE_ERROR("%s - Avoiding division by zero...", __FUNCTION__);
                APPL_TRACE_ERROR("%s - block=%d, subBands=%d, channels=%d", __FUNCTION__,
                                 pstrEncParams->s16NumOfBlocks, pstrEncParams->s16NumOfSubBands,
                                 pstrEncParams->s16NumOfChannels);
                break;
            }

            if ((pstrEncParams->s16ChannelMode == SBC_JOINT_STEREO) ||
                    (pstrEncParams->s16ChannelMode == SBC_STEREO) ) {
                s16BitPool = (SINT16)( (pstrEncParams->u16BitRate *
                                        pstrEncParams->s16NumOfSubBands * 1000 / s16SamplingFreq)
                                       - ( (32 + (4 * pstrEncParams->s16NumOfSubBands *
                                                  pstrEncParams->s16NumOfChannels)
                                            + ( (pstrEncParams->s16ChannelMode - 2) *
                                                pstrEncParams->s16NumOfSubBands )   )
                                           / pstrEncParams->s16NumOfBlocks) );

                s16FrameLen = 4 + (4 * pstrEncParams->s16NumOfSubBands *
                                   pstrEncParams->s16NumOfChannels) / 8
                              + ( ((pstrEncParams->s16ChannelMode - 2) *
                                   pstrEncParams->s16NumOfSubBands)
                                  + (pstrEncParams->s16NumOfBlocks * s16BitPool) ) / 8;

                s16BitRate = (8 * s16FrameLen * s16SamplingFreq)
                             / (pstrEncParams->s16NumOfSubBands *
                                pstrEncParams->s16NumOfBlocks * 1000);

                if (s16BitRate > pstrEncParams->u16BitRate) {
                    s16BitPool--;
                }

                if (pstrEncParams->s16NumOfSubBands == 8) {
                    s16BitPool = (s16BitPool > 255) ? 255 : s16BitPool;
                } else {
                    s16BitPool = (s16BitPool > 128) ? 128 : s16BitPool;
                }
            } else {
                s16BitPool = (SINT16)( ((pstrEncParams->s16NumOfSubBands *
                                         pstrEncParams->u16BitRate * 1000)
                                        / (s16SamplingFreq * pstrEncParams->s16NumOfChannels))
                                       - ( ( (32 / pstrEncParams->s16NumOfChannels) +
                                             (4 * pstrEncParams->s16NumOfSubBands) )
                                           /   pstrEncParams->s16NumOfBlocks ) );

                pstrEncParams->s16BitPool = (s16BitPool >
                                             (16 * pstrEncParams->s16NumOfSubBands))
                                            ? (16 * pstrEncParams->s16NumOfSubBands) : s16BitPool;
            }

            if (s16BitPool < 0) {
                s16BitPool = 0;
            }

            APPL_TRACE_EVENT("bitpool candidate : %d (%d kbps)",
                             s16BitPool, pstrEncParams->u16BitRate);

            if (s16BitPool > pUpdateAudio->MaxBitPool) {
                APPL_TRACE_DEBUG("%s computed bitpool too large (%d)",  __FUNCTION__, s16BitPool);
                /* Decrease bitrate */
                btc_aa_src_cb.encoder.u16BitRate -= BTC_MEDIA_BITRATE_STEP;
                /* Record that we have decreased the bitrate */
                protect |= 1;
            } else if (s16BitPool < pUpdateAudio->MinBitPool) {
                APPL_TRACE_WARNING("%s computed bitpool too small (%d)", __FUNCTION__, s16BitPool);

                /* Increase bitrate */
                UINT16 previous_u16BitRate = btc_aa_src_cb.encoder.u16BitRate;
                btc_aa_src_cb.encoder.u16BitRate += BTC_MEDIA_BITRATE_STEP;
                /* Record that we have increased the bitrate */
                protect |= 2;
                /* Check over-flow */
                if (btc_aa_src_cb.encoder.u16BitRate < previous_u16BitRate) {
                    protect |= 3;
                }
            } else {
                break;
            }
            /* In case we have already increased and decreased the bitrate, just stop */
            if (protect == 3) {
                APPL_TRACE_ERROR("%s could not find bitpool in range", __FUNCTION__);
                break;
            }
        } while (1);

        /* Finally update the bitpool in the encoder structure */
        pstrEncParams->s16BitPool = s16BitPool;

        APPL_TRACE_DEBUG("%s final bit rate %d, final bit pool %d", __FUNCTION__,
                         btc_aa_src_cb.encoder.u16BitRate, btc_aa_src_cb.encoder.s16BitPool);

        /* make sure we reinitialize encoder with new settings */
        SBC_Encoder_Init(&(btc_aa_src_cb.encoder));
    }
}

/*******************************************************************************
 **
 ** Function         btc_a2dp_source_pcm2sbc_init
 **
 ** Description      Init encoding task for PCM to SBC according to feeding
 **
 ** Returns          void
 **
 *******************************************************************************/
static void btc_a2dp_source_pcm2sbc_init(tBTC_MEDIA_INIT_AUDIO_FEEDING *p_feeding)
{
    BOOLEAN reconfig_needed = FALSE;

    APPL_TRACE_DEBUG("PCM feeding:");
    APPL_TRACE_DEBUG("sampling_freq:%d", p_feeding->feeding.cfg.pcm.sampling_freq);
    APPL_TRACE_DEBUG("num_channel:%d", p_feeding->feeding.cfg.pcm.num_channel);
    APPL_TRACE_DEBUG("bit_per_sample:%d", p_feeding->feeding.cfg.pcm.bit_per_sample);

    /* Check the PCM feeding sampling_freq */
    switch (p_feeding->feeding.cfg.pcm.sampling_freq) {
    case  8000:
    case 12000:
    case 16000:
    case 24000:
    case 32000:
    case 48000:
        /* For these sampling_freq the AV connection must be 48000 */
        if (btc_aa_src_cb.encoder.s16SamplingFreq != SBC_sf48000) {
            /* Reconfiguration needed at 48000 */
            APPL_TRACE_DEBUG("SBC Reconfiguration needed at 48000");
            btc_aa_src_cb.encoder.s16SamplingFreq = SBC_sf48000;
            reconfig_needed = TRUE;
        }
        break;

    case 11025:
    case 22050:
    case 44100:
        /* For these sampling_freq the AV connection must be 44100 */
        if (btc_aa_src_cb.encoder.s16SamplingFreq != SBC_sf44100) {
            /* Reconfiguration needed at 44100 */
            APPL_TRACE_DEBUG("SBC Reconfiguration needed at 44100");
            btc_aa_src_cb.encoder.s16SamplingFreq = SBC_sf44100;
            reconfig_needed = TRUE;
        }
        break;
    default:
        APPL_TRACE_DEBUG("Feeding PCM sampling_freq unsupported");
        break;
    }

    /* Some AV Headsets do not support Mono => always ask for Stereo */
    if (btc_aa_src_cb.encoder.s16ChannelMode == SBC_MONO) {
        APPL_TRACE_DEBUG("SBC Reconfiguration needed in Stereo");
        btc_aa_src_cb.encoder.s16ChannelMode = SBC_JOINT_STEREO;
        reconfig_needed = TRUE;
    }

    if (reconfig_needed != FALSE) {
        APPL_TRACE_DEBUG("%s :: mtu %d", __FUNCTION__, btc_aa_src_cb.TxAaMtuSize);
        APPL_TRACE_DEBUG("ch mode %d, nbsubd %d, nb %d, alloc %d, rate %d, freq %d",
                         btc_aa_src_cb.encoder.s16ChannelMode,
                         btc_aa_src_cb.encoder.s16NumOfSubBands, btc_aa_src_cb.encoder.s16NumOfBlocks,
                         btc_aa_src_cb.encoder.s16AllocationMethod, btc_aa_src_cb.encoder.u16BitRate,
                         btc_aa_src_cb.encoder.s16SamplingFreq);

        SBC_Encoder_Init(&(btc_aa_src_cb.encoder));
    } else {
        APPL_TRACE_DEBUG("%s no SBC reconfig needed", __FUNCTION__);
    }
}

/*******************************************************************************
 **
 ** Function         btc_a2dp_source_audio_feeding_init
 **
 ** Description      Initialize the audio path according to the feeding format
 **
 ** Returns          void
 **
 *******************************************************************************/
static void btc_a2dp_source_audio_feeding_init(BT_HDR *p_msg)
{
    tBTC_MEDIA_INIT_AUDIO_FEEDING *p_feeding = (tBTC_MEDIA_INIT_AUDIO_FEEDING *) p_msg;

    APPL_TRACE_DEBUG("%s format:%d", __FUNCTION__, p_feeding->feeding.format);

    /* Save Media Feeding information */
    btc_aa_src_cb.feeding_mode = p_feeding->feeding_mode;
    btc_aa_src_cb.media_feeding = p_feeding->feeding;

    /* Handle different feeding formats */
    switch (p_feeding->feeding.format) {
    case BTC_AV_CODEC_PCM:
        btc_aa_src_cb.TxTranscoding = BTC_MEDIA_TRSCD_PCM_2_SBC;
        btc_a2dp_source_pcm2sbc_init(p_feeding);
        break;

    default :
        APPL_TRACE_ERROR("unknown feeding format %d", p_feeding->feeding.format);
        break;
    }
}

/*******************************************************************************
 **
 ** Function         btc_a2dp_source_aa_tx_flush
 **
 ** Description
 **
 ** Returns          void
 **
 *******************************************************************************/
static void btc_a2dp_source_aa_tx_flush(void)
{
    /* Flush all enqueued music buffers (encoded) */
    APPL_TRACE_DEBUG("%s", __FUNCTION__);

    btc_aa_src_cb.media_feeding_state.pcm.counter = 0;
    btc_aa_src_cb.media_feeding_state.pcm.aa_feed_residue = 0;

    btc_a2dp_source_flush_q(btc_aa_src_cb.TxAaQ);

    btc_aa_src_data_read(NULL, -1);
}

/*******************************************************************************
 **
 ** Function         btc_get_num_aa_frame
 **
 ** Description
 **
 ** Returns          The number of media frames in this time slice
 **
 *******************************************************************************/
static UINT8 btc_get_num_aa_frame(void)
{
    UINT8 result = 0;

    switch (btc_aa_src_cb.TxTranscoding) {
    case BTC_MEDIA_TRSCD_PCM_2_SBC: {
        UINT32 pcm_bytes_per_frame = btc_aa_src_cb.encoder.s16NumOfSubBands *
                                     btc_aa_src_cb.encoder.s16NumOfBlocks *
                                     btc_aa_src_cb.media_feeding.cfg.pcm.num_channel *
                                     btc_aa_src_cb.media_feeding.cfg.pcm.bit_per_sample / 8;

        UINT32 us_this_tick = BTC_MEDIA_TIME_TICK_MS * 1000;
        UINT64 now_us = time_now_us();
        if (last_frame_us != 0) {
#if _POSIX_TIMERS
            us_this_tick = (now_us - last_frame_us);
#else
            // consider the case that the number of day increases and timeofday wraps around
            us_this_tick = (now_us > last_frame_us) ? (now_us - last_frame_us) :
                           (now_us + 86400000000ull - last_frame_us);
#endif
        }
        last_frame_us = now_us;

        btc_aa_src_cb.media_feeding_state.pcm.counter +=
            btc_aa_src_cb.media_feeding_state.pcm.bytes_per_tick *
            us_this_tick / (BTC_MEDIA_TIME_TICK_MS * 1000);

        /* calculate nbr of frames pending for this media tick */
        result = btc_aa_src_cb.media_feeding_state.pcm.counter / pcm_bytes_per_frame;
        if (result > MAX_PCM_FRAME_NUM_PER_TICK) {
            APPL_TRACE_WARNING("%s() - Limiting frames to be sent from %d to %d"
                               , __FUNCTION__, result, MAX_PCM_FRAME_NUM_PER_TICK);
            result = MAX_PCM_FRAME_NUM_PER_TICK;
        }
        btc_aa_src_cb.media_feeding_state.pcm.counter -= result * pcm_bytes_per_frame;

        LOG_VERBOSE("WRITE %d FRAMES", result);
    }
    break;

    default:
        APPL_TRACE_ERROR("ERROR btc_get_num_aa_frame Unsupported transcoding format 0x%x",
                         btc_aa_src_cb.TxTranscoding);
        result = 0;
        break;
    }

    return (UINT8)result;
}

/*******************************************************************************
 **
 ** Function         btc_media_aa_read_feeding
 **
 ** Description
 **
 ** Returns          void
 **
 *******************************************************************************/

BOOLEAN btc_media_aa_read_feeding(void)
{
    UINT16 blocm_x_subband = btc_aa_src_cb.encoder.s16NumOfSubBands * \
                             btc_aa_src_cb.encoder.s16NumOfBlocks;
    UINT32 read_size;
    UINT16 sbc_sampling = 48000;
    UINT32 src_samples;
    UINT16 bytes_needed = blocm_x_subband * btc_aa_src_cb.encoder.s16NumOfChannels * \
                          btc_aa_src_cb.media_feeding.cfg.pcm.bit_per_sample / 8;
    static UINT16 up_sampled_buffer[SBC_MAX_NUM_FRAME * SBC_MAX_NUM_OF_BLOCKS
                                    * SBC_MAX_NUM_OF_CHANNELS * SBC_MAX_NUM_OF_SUBBANDS * 2];
    static UINT16 read_buffer[SBC_MAX_NUM_FRAME * SBC_MAX_NUM_OF_BLOCKS
                              * SBC_MAX_NUM_OF_CHANNELS * SBC_MAX_NUM_OF_SUBBANDS];
    UINT32 src_size_used;
    UINT32 dst_size_used;
    BOOLEAN fract_needed;
    INT32   fract_max;
    INT32   fract_threshold;
    UINT32  nb_byte_read = 0;

    /* Get the SBC sampling rate */
    switch (btc_aa_src_cb.encoder.s16SamplingFreq) {
    case SBC_sf48000:
        sbc_sampling = 48000;
        break;
    case SBC_sf44100:
        sbc_sampling = 44100;
        break;
    case SBC_sf32000:
        sbc_sampling = 32000;
        break;
    case SBC_sf16000:
        sbc_sampling = 16000;
        break;
    }

    if (sbc_sampling == btc_aa_src_cb.media_feeding.cfg.pcm.sampling_freq) {
        read_size = bytes_needed - btc_aa_src_cb.media_feeding_state.pcm.aa_feed_residue;
        nb_byte_read = btc_aa_src_data_read(
                           ((uint8_t *)btc_aa_src_cb.encoder.as16PcmBuffer) +
                           btc_aa_src_cb.media_feeding_state.pcm.aa_feed_residue,
                           read_size);
        if (nb_byte_read == read_size) {
            btc_aa_src_cb.media_feeding_state.pcm.aa_feed_residue = 0;
            return TRUE;
        } else {
            APPL_TRACE_WARNING("### UNDERFLOW :: ONLY READ %d BYTES OUT OF %d ###",
                               nb_byte_read, read_size);
            btc_aa_src_cb.media_feeding_state.pcm.aa_feed_residue += nb_byte_read;
            return FALSE;
        }
    }

    /* Some Feeding PCM frequencies require to split the number of sample */
    /* to read. */
    /* E.g 128/6=21.3333 => read 22 and 21 and 21 => max = 2; threshold = 0*/
    fract_needed = FALSE;   /* Default */
    switch (btc_aa_src_cb.media_feeding.cfg.pcm.sampling_freq) {
    case 32000:
    case 8000:
        fract_needed = TRUE;
        fract_max = 2;          /* 0, 1 and 2 */
        fract_threshold = 0;    /* Add one for the first */
        break;
    case 16000:
        fract_needed = TRUE;
        fract_max = 2;          /* 0, 1 and 2 */
        fract_threshold = 1;    /* Add one for the first two frames*/
        break;
    }

    /* Compute number of sample to read from source */
    src_samples = blocm_x_subband;
    src_samples *= btc_aa_src_cb.media_feeding.cfg.pcm.sampling_freq;
    src_samples /= sbc_sampling;

    /* The previous division may have a remainder not null */
    if (fract_needed) {
        if (btc_aa_src_cb.media_feeding_state.pcm.aa_feed_counter <= fract_threshold) {
            src_samples++; /* for every read before threshold add one sample */
        }

        /* do nothing if counter >= threshold */
        btc_aa_src_cb.media_feeding_state.pcm.aa_feed_counter++; /* one more read */
        if (btc_aa_src_cb.media_feeding_state.pcm.aa_feed_counter > fract_max) {
            btc_aa_src_cb.media_feeding_state.pcm.aa_feed_counter = 0;
        }
    }

    /* Compute number of bytes to read from source */
    read_size = src_samples;
    read_size *= btc_aa_src_cb.media_feeding.cfg.pcm.num_channel;
    read_size *= (btc_aa_src_cb.media_feeding.cfg.pcm.bit_per_sample / 8);

    /* Read Data from data channel */
    nb_byte_read = btc_aa_src_data_read((uint8_t *)read_buffer, read_size);

    //tput_mon(TRUE, nb_byte_read, FALSE);

    if (nb_byte_read < read_size) {
        APPL_TRACE_WARNING("### UNDERRUN :: ONLY READ %d BYTES OUT OF %d ###",
                           nb_byte_read, read_size);

        if (nb_byte_read == 0) {
            return FALSE;
        }

        if (btc_aa_src_cb.feeding_mode == BTC_AV_FEEDING_ASYNCHRONOUS) {
            /* Fill the unfilled part of the read buffer with silence (0) */
            memset(((UINT8 *)read_buffer) + nb_byte_read, 0, read_size - nb_byte_read);
            nb_byte_read = read_size;
        }
    }

    /* Initialize PCM up-sampling engine */
    bta_av_sbc_init_up_sample(btc_aa_src_cb.media_feeding.cfg.pcm.sampling_freq,
                              sbc_sampling, btc_aa_src_cb.media_feeding.cfg.pcm.bit_per_sample,
                              btc_aa_src_cb.media_feeding.cfg.pcm.num_channel);

    /* re-sample read buffer */
    /* The output PCM buffer will be stereo, 16 bit per sample */
    dst_size_used = bta_av_sbc_up_sample((UINT8 *)read_buffer,
                                         (UINT8 *)up_sampled_buffer + btc_aa_src_cb.media_feeding_state.pcm.aa_feed_residue,
                                         nb_byte_read,
                                         sizeof(up_sampled_buffer) - btc_aa_src_cb.media_feeding_state.pcm.aa_feed_residue,
                                         &src_size_used);

    /* update the residue */
    btc_aa_src_cb.media_feeding_state.pcm.aa_feed_residue += dst_size_used;

    /* only copy the pcm sample when we have up-sampled enough PCM */
    if (btc_aa_src_cb.media_feeding_state.pcm.aa_feed_residue >= bytes_needed) {
        /* Copy the output pcm samples in SBC encoding buffer */
        memcpy((UINT8 *)btc_aa_src_cb.encoder.as16PcmBuffer,
               (UINT8 *)up_sampled_buffer,
               bytes_needed);
        /* update the residue */
        btc_aa_src_cb.media_feeding_state.pcm.aa_feed_residue -= bytes_needed;

        if (btc_aa_src_cb.media_feeding_state.pcm.aa_feed_residue != 0) {
            memcpy((UINT8 *)up_sampled_buffer,
                   (UINT8 *)up_sampled_buffer + bytes_needed,
                   btc_aa_src_cb.media_feeding_state.pcm.aa_feed_residue);
        }
        return TRUE;
    }

    return FALSE;
}

/*******************************************************************************
 **
 ** Function         btc_media_aa_prep_sbc_2_send
 **
 ** Description
 **
 ** Returns          void
 **
 *******************************************************************************/
static void btc_media_aa_prep_sbc_2_send(UINT8 nb_frame)
{
    BT_HDR *p_buf;
    UINT16 blocm_x_subband = btc_aa_src_cb.encoder.s16NumOfSubBands *
                             btc_aa_src_cb.encoder.s16NumOfBlocks;

    while (nb_frame) {
        if (NULL == (p_buf = osi_malloc(BTC_MEDIA_AA_BUF_SIZE))) {
            APPL_TRACE_ERROR ("ERROR btc_media_aa_prep_sbc_2_send no buffer TxCnt %d ",
                              fixed_queue_length(btc_aa_src_cb.TxAaQ));
            return;
        }

        /* Init buffer */
        p_buf->offset = BTC_MEDIA_AA_SBC_OFFSET;
        p_buf->len = 0;
        p_buf->layer_specific = 0;

        do {
            /* Write @ of allocated buffer in encoder.pu8Packet */
            btc_aa_src_cb.encoder.pu8Packet = (UINT8 *) (p_buf + 1) + p_buf->offset + p_buf->len;
            /* Fill allocated buffer with 0 */
            memset(btc_aa_src_cb.encoder.as16PcmBuffer, 0, blocm_x_subband
                   * btc_aa_src_cb.encoder.s16NumOfChannels);

            /* Read PCM data and upsample them if needed */
            if (btc_media_aa_read_feeding()) {
                /* SBC encode and descramble frame */
                SBC_Encoder(&(btc_aa_src_cb.encoder));
                A2D_SbcChkFrInit(btc_aa_src_cb.encoder.pu8Packet);
                A2D_SbcDescramble(btc_aa_src_cb.encoder.pu8Packet, btc_aa_src_cb.encoder.u16PacketLength);
                /* Update SBC frame length */
                p_buf->len += btc_aa_src_cb.encoder.u16PacketLength;
                nb_frame--;
                p_buf->layer_specific++;
            } else {
                APPL_TRACE_WARNING("btc_media_aa_prep_sbc_2_send underflow %d, %d",
                                   nb_frame, btc_aa_src_cb.media_feeding_state.pcm.aa_feed_residue);
                btc_aa_src_cb.media_feeding_state.pcm.counter += nb_frame *
                        btc_aa_src_cb.encoder.s16NumOfSubBands *
                        btc_aa_src_cb.encoder.s16NumOfBlocks *
                        btc_aa_src_cb.media_feeding.cfg.pcm.num_channel *
                        btc_aa_src_cb.media_feeding.cfg.pcm.bit_per_sample / 8;
                /* no more pcm to read */
                nb_frame = 0;

                /* break read loop if timer was stopped (media task stopped) */
                if ( btc_aa_src_cb.is_tx_timer == FALSE ) {
                    osi_free(p_buf);
                    return;
                }
            }

        } while (((p_buf->len + btc_aa_src_cb.encoder.u16PacketLength) < btc_aa_src_cb.TxAaMtuSize)
                 && (p_buf->layer_specific < 0x0F) && nb_frame);

        if (p_buf->len) {
            /* timestamp of the media packet header represent the TS of the first SBC frame
               i.e the timestamp before including this frame */
            *((UINT32 *) (p_buf + 1)) = btc_aa_src_cb.timestamp;

            btc_aa_src_cb.timestamp += p_buf->layer_specific * blocm_x_subband;

            if (btc_aa_src_cb.tx_flush) {
                APPL_TRACE_DEBUG("### tx suspended, discarded frame ###");

                if (fixed_queue_length(btc_aa_src_cb.TxAaQ) > 0) {
                    btc_a2dp_source_flush_q(btc_aa_src_cb.TxAaQ);
                }

                osi_free(p_buf);
                return;
            }

            /* Enqueue the encoded SBC frame in AA Tx Queue */
            fixed_queue_enqueue(btc_aa_src_cb.TxAaQ, p_buf);
        } else {
            osi_free(p_buf);
        }
    }
}

/*******************************************************************************
 **
 ** Function         btc_a2dp_source_prep_2_send
 **
 ** Description
 **
 ** Returns          void
 **
 *******************************************************************************/
static void btc_a2dp_source_prep_2_send(UINT8 nb_frame)
{
    // Check for TX queue overflow
    if (nb_frame > MAX_OUTPUT_A2DP_SRC_FRAME_QUEUE_SZ) {
        nb_frame = MAX_OUTPUT_A2DP_SRC_FRAME_QUEUE_SZ;
    }

    if (fixed_queue_length(btc_aa_src_cb.TxAaQ) > (MAX_OUTPUT_A2DP_SRC_FRAME_QUEUE_SZ - nb_frame)) {
        APPL_TRACE_WARNING("TX Q overflow: %d/%d",
                           fixed_queue_length(btc_aa_src_cb.TxAaQ), MAX_OUTPUT_A2DP_SRC_FRAME_QUEUE_SZ - nb_frame);
    }

    while (fixed_queue_length(btc_aa_src_cb.TxAaQ) > (MAX_OUTPUT_A2DP_SRC_FRAME_QUEUE_SZ - nb_frame)) {
        osi_free(fixed_queue_try_dequeue(btc_aa_src_cb.TxAaQ));
    }

    // Transcode frame

    switch (btc_aa_src_cb.TxTranscoding) {
    case BTC_MEDIA_TRSCD_PCM_2_SBC:
        btc_media_aa_prep_sbc_2_send(nb_frame);
        break;

    default:
        APPL_TRACE_ERROR("%s unsupported transcoding format 0x%x", __func__, btc_aa_src_cb.TxTranscoding);
        break;
    }
}

/*******************************************************************************
 **
 ** Function         btc_a2dp_source_send_aa_frame
 **
 ** Description
 **
 ** Returns          void
 **
 *******************************************************************************/
static void btc_a2dp_source_send_aa_frame(void)
{
    UINT8 nb_frame_2_send;

    /* get the number of frame to send */
    nb_frame_2_send = btc_get_num_aa_frame();

    if (nb_frame_2_send != 0) {
        /* format and Q buffer to send */
        btc_a2dp_source_prep_2_send(nb_frame_2_send);
    }

    /* send it */
    LOG_VERBOSE("%s: send %d frames", __FUNCTION__, nb_frame_2_send);
    bta_av_ci_src_data_ready(BTA_AV_CHNL_AUDIO);
}

static void btc_a2dp_source_handle_timer(UNUSED_ATTR void *context)
{
    log_tstamps_us("media task tx timer");

#if (BTA_AV_INCLUDED == TRUE)
    if (btc_aa_src_cb.is_tx_timer == TRUE) {
        btc_a2dp_source_send_aa_frame();
    } else {
        APPL_TRACE_WARNING("Media task Scheduled after Suspend");
    }
#endif
}

static void btc_a2dp_source_alarm_cb(UNUSED_ATTR void *context)
{
    btc_a2dp_source_data_post(BTC_A2DP_SOURCE_DATA_EVT);
}

/*******************************************************************************
 **
 ** Function         btc_a2dp_source_feeding_state_reset
 **
 ** Description      Reset the media feeding state
 **
 ** Returns          void
 **
 *******************************************************************************/
static void btc_a2dp_source_feeding_state_reset(void)
{
    /* By default, just clear the entire state */
    memset(&btc_aa_src_cb.media_feeding_state, 0, sizeof(btc_aa_src_cb.media_feeding_state));

    if (btc_aa_src_cb.TxTranscoding == BTC_MEDIA_TRSCD_PCM_2_SBC) {
        btc_aa_src_cb.media_feeding_state.pcm.bytes_per_tick =
            (btc_aa_src_cb.media_feeding.cfg.pcm.sampling_freq *
             btc_aa_src_cb.media_feeding.cfg.pcm.bit_per_sample / 8 *
             btc_aa_src_cb.media_feeding.cfg.pcm.num_channel *
             BTC_MEDIA_TIME_TICK_MS) / 1000;

        APPL_TRACE_WARNING("pcm bytes per tick %d",
                           (int)btc_aa_src_cb.media_feeding_state.pcm.bytes_per_tick);
    }
}

/*******************************************************************************
 **
 ** Function         btc_a2dp_source_aa_start_tx
 **
 ** Description      Start media task encoding
 **
 ** Returns          void
 **
 *******************************************************************************/
static void btc_a2dp_source_aa_start_tx(void)
{
    APPL_TRACE_DEBUG("btc_a2dp_source_aa_start_tx is timer %d, feeding mode %d",
                     btc_aa_src_cb.is_tx_timer, btc_aa_src_cb.feeding_mode);

    btc_aa_src_cb.is_tx_timer = TRUE;
    last_frame_us = 0;

    /* Reset the media feeding state */
    btc_a2dp_source_feeding_state_reset();

    APPL_TRACE_EVENT("starting timer %dms", BTC_MEDIA_TIME_TICK_MS);

    assert(btc_aa_src_cb.media_alarm == NULL);

    btc_aa_src_cb.media_alarm = osi_alarm_new("aaTx", btc_a2dp_source_alarm_cb, NULL, BTC_MEDIA_TIME_TICK_MS);

    if (!btc_aa_src_cb.media_alarm) {
        LOG_ERROR("%s unable to allocate media alarm.", __func__);
        return;
    }

    osi_alarm_set_periodic(btc_aa_src_cb.media_alarm, BTC_MEDIA_TIME_TICK_MS);
}

/*******************************************************************************
 **
 ** Function         btc_a2dp_source_aa_stop_tx
 **
 ** Description      Stop media task encoding
 **
 ** Returns          void
 **
 *******************************************************************************/
static void btc_a2dp_source_aa_stop_tx(void)
{
    APPL_TRACE_DEBUG("%s is_tx_timer: %d", __func__, btc_aa_src_cb.is_tx_timer);

    const bool send_ack = (btc_aa_src_cb.is_tx_timer != FALSE);

    /* Stop the timer first */
    if (btc_aa_src_cb.media_alarm) {
        osi_alarm_cancel(btc_aa_src_cb.media_alarm);
        osi_alarm_free(btc_aa_src_cb.media_alarm);
    }
    btc_aa_src_cb.media_alarm = NULL;
    btc_aa_src_cb.is_tx_timer = FALSE;

    /* Try to send acknowldegment once the media stream is
       stopped. This will make sure that the A2DP HAL layer is
       un-blocked on wait for acknowledgment for the sent command.
       This resolves a corner cases AVDTP SUSPEND collision
       when the DUT and the remote device issue SUSPEND simultaneously
       and due to the processing of the SUSPEND request from the remote,
       the media path is torn down. If the A2DP HAL happens to wait
       for ACK for the initiated SUSPEND, it would never receive it casuing
       a block/wait. Due to this acknowledgement, the A2DP HAL is guranteed
       to get the ACK for any pending command in such cases. */

    if (send_ack) {
        btc_a2dp_control_command_ack(ESP_A2D_MEDIA_CTRL_ACK_SUCCESS);
    }

    /* audio engine stopped, reset tx suspended flag */
    btc_aa_src_cb.tx_flush = 0;
    last_frame_us = 0;

    /* Reset the feeding state */
    btc_a2dp_source_feeding_state_reset();
}

/*******************************************************************************
 **
 ** Function         btc_a2dp_source_flush_q
 **
 ** Description
 **
 ** Returns          void
 **
 *******************************************************************************/
static void btc_a2dp_source_flush_q(fixed_queue_t *p_q)
{
    while (! fixed_queue_is_empty(p_q)) {
        osi_free(fixed_queue_try_dequeue(p_q));
    }
}

static void btc_a2dp_source_thread_init(UNUSED_ATTR void *context)
{
    APPL_TRACE_EVENT("%s\n", __func__);
    memset(&btc_aa_src_cb, 0, sizeof(btc_aa_src_cb));

    btc_a2dp_source_state = BTC_A2DP_SOURCE_STATE_ON;

    btc_aa_src_cb.TxAaQ = fixed_queue_new(SIZE_MAX);

    btc_a2dp_control_init();
}

static void btc_a2dp_source_thread_cleanup(UNUSED_ATTR void *context)
{
    /* make sure no channels are restarted while shutting down */
    btc_a2dp_source_state = BTC_A2DP_SOURCE_STATE_SHUTTING_DOWN;

    btc_a2dp_control_set_datachnl_stat(FALSE);
    /* Clear media task flag */
    btc_a2dp_source_state = BTC_A2DP_SOURCE_STATE_OFF;

    btc_a2dp_control_cleanup();

    fixed_queue_free(btc_aa_src_cb.TxAaQ, osi_free_func);
}

#endif /* BTC_AV_INCLUDED */
