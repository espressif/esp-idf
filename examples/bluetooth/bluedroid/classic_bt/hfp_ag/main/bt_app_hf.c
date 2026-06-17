/*
 * SPDX-FileCopyrightText: 2021-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include "esp_log.h"
#include "esp_bt_main.h"
#include "esp_bt_device.h"
#include "esp_gap_bt_api.h"
#include "esp_hf_ag_api.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "freertos/ringbuf.h"
#include "time.h"
#include "sys/time.h"
#include "sdkconfig.h"
#include "bt_app_core.h"
#include "bt_app_hf.h"
#include "esp_hf_defs.h"
#if CONFIG_BT_HFP_USE_EXTERNAL_CODEC
#include "encoder/impl/esp_sbc_enc.h"
#include "decoder/impl/esp_sbc_dec.h"
#if CONFIG_BT_HFP_LC3_ENABLE
#include "encoder/impl/esp_lc3_enc.h"
#include "decoder/impl/esp_lc3_dec.h"
#endif
#endif /* CONFIG_BT_HFP_USE_EXTERNAL_CODEC */

const char *c_hf_evt_str[] = {
    "CONNECTION_STATE_EVT",              /*!< SERVICE LEVEL CONNECTION STATE CONTROL */
    "AUDIO_STATE_EVT",                   /*!< AUDIO CONNECTION STATE CONTROL */
    "VR_STATE_CHANGE_EVT",               /*!< VOICE RECOGNITION CHANGE */
    "VOLUME_CONTROL_EVT",                /*!< AUDIO VOLUME CONTROL */
    "UNKNOW_AT_CMD",                     /*!< UNKNOWN AT COMMAND RECEIVED */
    "IND_UPDATE",                        /*!< INDICATION UPDATE */
    "CIND_RESPONSE_EVT",                 /*!< CALL & DEVICE INDICATION */
    "COPS_RESPONSE_EVT",                 /*!< CURRENT OPERATOR EVENT */
    "CLCC_RESPONSE_EVT",                 /*!< LIST OF CURRENT CALL EVENT */
    "CNUM_RESPONSE_EVT",                 /*!< SUBSCRIBER INFORTMATION OF CALL EVENT */
    "DTMF_RESPONSE_EVT",                 /*!< DTMF TRANSFER EVT */
    "NREC_RESPONSE_EVT",                 /*!< NREC RESPONSE EVT */
    "ANSWER_INCOMING_EVT",               /*!< ANSWER INCOMING EVT */
    "REJECT_INCOMING_EVT",               /*!< AREJECT INCOMING EVT */
    "DIAL_EVT",                          /*!< DIAL INCOMING EVT */
    "WBS_EVT",                           /*!< CURRENT CODEC EVT */
    "BCS_EVT",                           /*!< CODEC NEGO EVT */
    "PKT_STAT_EVT",                      /*!< REQUEST PACKET STATUS EVT */
    "PROF_STATE_EVT",                    /*!< Indicate HF init or deinit complete */
    "BAC_RESPONSE_EVT",                  /*!< Peer codec capabilities from AT+BAC */
};

//esp_hf_connection_state_t
const char *c_connection_state_str[] = {
    "DISCONNECTED",
    "CONNECTING",
    "CONNECTED",
    "SLC_CONNECTED",
    "DISCONNECTING",
};

// esp_hf_audio_state_t
const char *c_audio_state_str[] = {
    [ESP_HF_AUDIO_STATE_DISCONNECTED]      = "disconnected",
    [ESP_HF_AUDIO_STATE_CONNECTING]        = "connecting",
    [ESP_HF_AUDIO_STATE_CONNECTED]         = "connected",
    [ESP_HF_AUDIO_STATE_CONNECTED_MSBC]    = "connected_msbc",
    [ESP_HF_AUDIO_STATE_CONNECTED_LC3]     = "connected_lc3",
};

/// esp_hf_vr_state_t
const char *c_vr_state_str[] = {
    "Disabled",
    "Enabled",
};

// esp_hf_nrec_t
const char *c_nrec_status_str[] = {
    "NREC DISABLE",
    "NREC ABLE",
};

// esp_hf_control_target_t
const char *c_volume_control_target_str[] = {
    "SPEAKER",
    "MICROPHONE",
};

// esp_hf_subscriber_service_type_t
char *c_operator_name_str[] = {
    "China Mobile",
    "China Unicom",
    "China Telecom",
};

// esp_hf_subscriber_service_type_t
char *c_subscriber_service_type_str[] = {
    "UNKNOWN",
    "VOICE",
    "FAX",
};

// esp_hf_codec_mode_t
const char *c_codec_mode_str[] = {
    [ESP_HF_CODEC_NONE]  = "None",
    [ESP_HF_CODEC_CVSD]  = "CVSD",
    [ESP_HF_CODEC_MSBC]  = "MSBC",
    [ESP_HF_CODEC_LC3]   = "LC3-SWB",
};

#if CONFIG_BT_HFP_AUDIO_DATA_PATH_HCI
#define TABLE_SIZE         100
#define TABLE_SIZE_BYTE    200
// Produce a sine audio
static const int16_t sine_int16[TABLE_SIZE] = {
     0,    2057,    4107,    6140,    8149,   10126,   12062,   13952,   15786,   17557,
 19260,   20886,   22431,   23886,   25247,   26509,   27666,   28714,   29648,   30466,
 31163,   31738,   32187,   32509,   32702,   32767,   32702,   32509,   32187,   31738,
 31163,   30466,   29648,   28714,   27666,   26509,   25247,   23886,   22431,   20886,
 19260,   17557,   15786,   13952,   12062,   10126,    8149,    6140,    4107,    2057,
     0,   -2057,   -4107,   -6140,   -8149,  -10126,  -12062,  -13952,  -15786,  -17557,
-19260,  -20886,  -22431,  -23886,  -25247,  -26509,  -27666,  -28714,  -29648,  -30466,
-31163,  -31738,  -32187,  -32509,  -32702,  -32767,  -32702,  -32509,  -32187,  -31738,
-31163,  -30466,  -29648,  -28714,  -27666,  -26509,  -25247,  -23886,  -22431,  -20886,
-19260,  -17557,  -15786,  -13952,  -12062,  -10126,   -8149,   -6140,   -4107,   -2057,
};

#define ESP_HFP_RINGBUF_SIZE 3600

// 7500 microseconds(=12 slots) is aligned to 1 msbc frame duration, and is multiple of common Tesco for eSCO link with EV3 or 2-EV3 packet type
#define PCM_BLOCK_DURATION_US        (7500)

#define WBS_PCM_SAMPLING_RATE_KHZ    (16)
#define SWB_PCM_SAMPLING_RATE_KHZ    (32)
#define PCM_SAMPLING_RATE_KHZ        (8)

#define BYTES_PER_SAMPLE             (2)

// input can refer to Enhanced Setup Synchronous Connection Command in core spec4.2 Vol2, Part E
#define WBS_PCM_INPUT_DATA_SIZE  (WBS_PCM_SAMPLING_RATE_KHZ * PCM_BLOCK_DURATION_US / 1000 * BYTES_PER_SAMPLE) //240
#define SWB_PCM_INPUT_DATA_SIZE  (SWB_PCM_SAMPLING_RATE_KHZ * PCM_BLOCK_DURATION_US / 1000 * BYTES_PER_SAMPLE) //480
#define PCM_INPUT_DATA_SIZE      (PCM_SAMPLING_RATE_KHZ * PCM_BLOCK_DURATION_US / 1000 * BYTES_PER_SAMPLE)     //120

#define PCM_GENERATOR_TICK_US        (4000)

#define TASK_STOP_TIMEOUT_MS         (1000)

static void bt_app_send_data_shut_down(void);

static long s_data_num = 0;
static RingbufHandle_t s_m_rb = NULL;
static uint64_t s_time_new, s_time_old;
static esp_timer_handle_t s_periodic_timer;
static uint64_t s_last_enter_time, s_now_enter_time;
static uint64_t s_us_duration;
static SemaphoreHandle_t s_send_data_Semaphore = NULL;
static SemaphoreHandle_t s_send_data_done_sem = NULL;
static TaskHandle_t s_bt_app_send_data_task_handler = NULL;
static volatile bool s_audio_tx_active = false;
static bool s_periodic_timer_running = false;
static esp_hf_audio_state_t s_audio_code;
static esp_hf_sync_conn_hdl_t s_sync_conn_hdl = ESP_INVALID_CONN_HANDLE;

#if CONFIG_BT_HFP_USE_EXTERNAL_CODEC
static void bt_app_hf_incoming_cb(const uint8_t *buf, uint32_t sz);

#define EXT_CODEC_TASK_STACK      (8 * 1024)
#define EXT_CODEC_TASK_PRIO       (configMAX_PRIORITIES - 5)
#define EXT_CODEC_RX_QUEUE_LEN    8
#define EXT_CODEC_TX_QUEUE_LEN    4
#define EXT_CODEC_ENC_TASK_CORE   1
#define EXT_CODEC_DEC_TASK_CORE   0

#define EXT_CODEC_ENC_MAX_SIZE    \
    ((ESP_HF_LC3_ENCODED_FRAME_SIZE > ESP_HF_MSBC_ENCODED_FRAME_SIZE) ? \
     ESP_HF_LC3_ENCODED_FRAME_SIZE : ESP_HF_MSBC_ENCODED_FRAME_SIZE)
#define EXT_CODEC_PCM_MAX_SIZE    SWB_PCM_INPUT_DATA_SIZE

typedef enum {
    BT_APP_EXT_CODEC_NONE = 0,
    BT_APP_EXT_CODEC_MSBC,
#if CONFIG_BT_HFP_LC3_ENABLE
    BT_APP_EXT_CODEC_LC3,
#endif
} bt_app_ext_codec_type_t;

typedef struct {
    uint8_t data[EXT_CODEC_ENC_MAX_SIZE];
    uint16_t len;
    bool is_bad;
} bt_app_ext_rx_item_t;

typedef struct {
    uint8_t pcm[EXT_CODEC_PCM_MAX_SIZE];
    uint16_t pcm_len;
} bt_app_ext_tx_item_t;

static bt_app_ext_codec_type_t s_ext_codec_type = BT_APP_EXT_CODEC_NONE;
static uint16_t s_ext_pcm_frame_bytes = 0;
static uint16_t s_ext_enc_frame_bytes = 0;
static void *s_ext_enc_handle = NULL;
static void *s_ext_dec_handle = NULL;
static QueueHandle_t s_ext_rx_queue = NULL;
static QueueHandle_t s_ext_tx_queue = NULL;
static TaskHandle_t s_ext_enc_task = NULL;
static TaskHandle_t s_ext_dec_task = NULL;
static SemaphoreHandle_t s_ext_codec_done_sem = NULL;
static volatile bool s_ext_codec_active = false;

/* Shared push TX path: one 7.5 ms PCM frame per timer tick, no ringbuffer. */
static uint8_t s_ext_tx_pcm_frame[EXT_CODEC_PCM_MAX_SIZE];

static void bt_app_hf_ext_codec_deinit(void);

static void bt_app_hf_ext_enc_send_pcm(const uint8_t *pcm, uint32_t pcm_len)
{
    if (!s_audio_tx_active || !s_ext_codec_active || s_ext_enc_handle == NULL ||
        s_sync_conn_hdl == ESP_INVALID_CONN_HANDLE || pcm_len != s_ext_pcm_frame_bytes) {
        return;
    }

    esp_audio_enc_in_frame_t in_frame = {
        .buffer = (uint8_t *)pcm,
        .len = (int)pcm_len,
    };
    uint8_t enc_buf[EXT_CODEC_ENC_MAX_SIZE];
    esp_audio_enc_out_frame_t out_frame = {
        .buffer = enc_buf,
        .len = sizeof(enc_buf),
    };
    esp_audio_err_t enc_ret = ESP_AUDIO_ERR_FAIL;

    if (s_ext_codec_type == BT_APP_EXT_CODEC_MSBC) {
        enc_ret = esp_sbc_enc_process(s_ext_enc_handle, &in_frame, &out_frame);
#if CONFIG_BT_HFP_LC3_ENABLE
    } else if (s_ext_codec_type == BT_APP_EXT_CODEC_LC3) {
        enc_ret = esp_lc3_enc_process(s_ext_enc_handle, &in_frame, &out_frame);
#endif
    }
    if (enc_ret != ESP_AUDIO_ERR_OK || out_frame.encoded_bytes == 0 ||
        out_frame.encoded_bytes > s_ext_enc_frame_bytes) {
        return;
    }

    esp_hf_audio_buff_t *audio_buf = esp_hf_ag_audio_buff_alloc(s_ext_enc_frame_bytes);
    if (audio_buf == NULL) {
        return;
    }
    memcpy(audio_buf->data, enc_buf, out_frame.encoded_bytes);
    audio_buf->data_len = out_frame.encoded_bytes;
    if (esp_hf_ag_audio_data_send(s_sync_conn_hdl, audio_buf) != ESP_OK) {
        esp_hf_ag_audio_buff_free(audio_buf);
    }
}

static void bt_app_hf_ext_dec_frame(const bt_app_ext_rx_item_t *rx_item)
{
    if (!s_ext_codec_active || s_ext_dec_handle == NULL || rx_item == NULL) {
        return;
    }

    esp_audio_dec_in_raw_t in_frame = {
        .buffer = (uint8_t *)rx_item->data,
        .len = rx_item->len,
        .frame_recover = rx_item->is_bad ? ESP_AUDIO_DEC_RECOVERY_PLC : ESP_AUDIO_DEC_RECOVERY_NONE,
    };
    int16_t pcm_buf[EXT_CODEC_PCM_MAX_SIZE / sizeof(int16_t)];
    esp_audio_dec_out_frame_t out_frame = {
        .buffer = (uint8_t *)pcm_buf,
        .len = sizeof(pcm_buf),
        .decoded_size = 0,
    };
    esp_audio_dec_info_t dec_info = {0};
    esp_audio_err_t dec_ret = ESP_AUDIO_ERR_FAIL;

    if (s_ext_codec_type == BT_APP_EXT_CODEC_MSBC) {
        dec_ret = esp_sbc_dec_decode(s_ext_dec_handle, &in_frame, &out_frame, &dec_info);
#if CONFIG_BT_HFP_LC3_ENABLE
    } else if (s_ext_codec_type == BT_APP_EXT_CODEC_LC3) {
        dec_ret = esp_lc3_dec_decode(s_ext_dec_handle, &in_frame, &out_frame, &dec_info);
#endif
    }

    if (dec_ret == ESP_AUDIO_ERR_OK) {
        bt_app_hf_incoming_cb((const uint8_t *)pcm_buf, out_frame.decoded_size);
    } else if (dec_ret != ESP_AUDIO_ERR_BUFF_NOT_ENOUGH) {
        ESP_LOGD(BT_HF_TAG, "ext codec decode skipped, ret: %d", dec_ret);
    }
}

static void bt_app_hf_ext_enc_task(void *arg)
{
    bt_app_ext_tx_item_t tx_item;

    (void)arg;
    while (s_ext_codec_active || uxQueueMessagesWaiting(s_ext_tx_queue) > 0) {
        if (xQueueReceive(s_ext_tx_queue, &tx_item, pdMS_TO_TICKS(10)) == pdTRUE) {
            /* Keep only the latest pending PCM frame if we fell behind. */
            bt_app_ext_tx_item_t newer;
            while (xQueueReceive(s_ext_tx_queue, &newer, 0) == pdTRUE) {
                tx_item = newer;
            }
            bt_app_hf_ext_enc_send_pcm(tx_item.pcm, tx_item.pcm_len);
        } else if (!s_ext_codec_active) {
            break;
        }
    }
    if (s_ext_codec_done_sem) {
        xSemaphoreGive(s_ext_codec_done_sem);
    }
    vTaskDelete(NULL);
}

static void bt_app_hf_ext_dec_task(void *arg)
{
    bt_app_ext_rx_item_t rx_item;

    (void)arg;
    while (s_ext_codec_active || uxQueueMessagesWaiting(s_ext_rx_queue) > 0) {
        if (xQueueReceive(s_ext_rx_queue, &rx_item, pdMS_TO_TICKS(10)) == pdTRUE) {
            bt_app_ext_rx_item_t newer;
            while (xQueueReceive(s_ext_rx_queue, &newer, 0) == pdTRUE) {
                rx_item = newer;
            }
            bt_app_hf_ext_dec_frame(&rx_item);
        } else if (!s_ext_codec_active) {
            break;
        }
    }
    if (s_ext_codec_done_sem) {
        xSemaphoreGive(s_ext_codec_done_sem);
    }
    vTaskDelete(NULL);
}

/* Single-owner deletion: worker deletes itself and signals via sem. */
static void bt_app_hf_ext_wait_task_done(TaskHandle_t *task_handle)
{
    if (*task_handle == NULL) {
        return;
    }
    if (s_ext_codec_done_sem == NULL ||
        xSemaphoreTake(s_ext_codec_done_sem, pdMS_TO_TICKS(TASK_STOP_TIMEOUT_MS)) != pdTRUE) {
        ESP_LOGE(BT_HF_TAG, "ext codec task stop timeout");
    }
    *task_handle = NULL;
}

static bool bt_app_hf_ext_codec_worker_start(void)
{
    if (s_ext_rx_queue == NULL) {
        s_ext_rx_queue = xQueueCreate(EXT_CODEC_RX_QUEUE_LEN, sizeof(bt_app_ext_rx_item_t));
    }
    if (s_ext_tx_queue == NULL) {
        s_ext_tx_queue = xQueueCreate(EXT_CODEC_TX_QUEUE_LEN, sizeof(bt_app_ext_tx_item_t));
    }
    if (s_ext_rx_queue == NULL || s_ext_tx_queue == NULL) {
        return false;
    }
    if (s_ext_codec_done_sem == NULL) {
        s_ext_codec_done_sem = xSemaphoreCreateCounting(2, 0);
    }
    if (s_ext_codec_done_sem == NULL) {
        return false;
    }
    s_ext_codec_active = true;
    if (s_ext_enc_task == NULL) {
        BaseType_t ret = xTaskCreatePinnedToCore(bt_app_hf_ext_enc_task, "HfExtEnc",
                                                 EXT_CODEC_TASK_STACK, NULL, EXT_CODEC_TASK_PRIO,
                                                 &s_ext_enc_task, EXT_CODEC_ENC_TASK_CORE);
        if (ret != pdPASS) {
            s_ext_codec_active = false;
            return false;
        }
    }
    if (s_ext_dec_task == NULL) {
        BaseType_t ret = xTaskCreatePinnedToCore(bt_app_hf_ext_dec_task, "HfExtDec",
                                                 EXT_CODEC_TASK_STACK, NULL, EXT_CODEC_TASK_PRIO,
                                                 &s_ext_dec_task, EXT_CODEC_DEC_TASK_CORE);
        if (ret != pdPASS) {
            s_ext_codec_active = false;
            bt_app_hf_ext_wait_task_done(&s_ext_enc_task);
            return false;
        }
    }
    return true;
}

static void bt_app_hf_ext_codec_worker_stop(void)
{
    s_ext_codec_active = false;
    bt_app_hf_ext_wait_task_done(&s_ext_enc_task);
    bt_app_hf_ext_wait_task_done(&s_ext_dec_task);
    if (s_ext_rx_queue) {
        xQueueReset(s_ext_rx_queue);
    }
    if (s_ext_tx_queue) {
        xQueueReset(s_ext_tx_queue);
    }
}

static bool bt_app_hf_ext_codec_open_msbc(void)
{
    esp_sbc_enc_config_t enc_cfg = ESP_SBC_MSBC_ENC_CONFIG_DEFAULT();
    esp_sbc_dec_cfg_t dec_cfg = {
        .sbc_mode = ESP_SBC_MODE_MSBC,
        .ch_num = 1,
        .enable_plc = true,
    };
    int in_size = 0;
    int out_size = 0;

    if (esp_sbc_enc_open(&enc_cfg, sizeof(enc_cfg), &s_ext_enc_handle) != ESP_AUDIO_ERR_OK) {
        return false;
    }
    if (esp_sbc_dec_open(&dec_cfg, sizeof(dec_cfg), &s_ext_dec_handle) != ESP_AUDIO_ERR_OK) {
        esp_sbc_enc_close(s_ext_enc_handle);
        s_ext_enc_handle = NULL;
        return false;
    }
    if (esp_sbc_enc_get_frame_size(s_ext_enc_handle, &in_size, &out_size) != ESP_AUDIO_ERR_OK ||
        in_size <= 0 || out_size <= 0 || out_size > EXT_CODEC_ENC_MAX_SIZE ||
        in_size > EXT_CODEC_PCM_MAX_SIZE) {
        esp_sbc_enc_close(s_ext_enc_handle);
        esp_sbc_dec_close(s_ext_dec_handle);
        s_ext_enc_handle = NULL;
        s_ext_dec_handle = NULL;
        return false;
    }
    s_ext_codec_type = BT_APP_EXT_CODEC_MSBC;
    s_ext_pcm_frame_bytes = (uint16_t)in_size;
    s_ext_enc_frame_bytes = (uint16_t)out_size;
    return true;
}

#if CONFIG_BT_HFP_LC3_ENABLE
static bool bt_app_hf_ext_codec_open_lc3(void)
{
    esp_lc3_enc_config_t enc_cfg = {
        .sample_rate = ESP_HF_LC3_SAMPLING_RATE_HZ,
        .bits_per_sample = 16,
        .channel = 1,
        .frame_dms = ESP_HF_LC3_FRAME_DURATION_US / 100,
        .nbyte = ESP_HF_LC3_ENCODED_FRAME_SIZE,
        .len_prefixed = false,
    };
    esp_lc3_dec_cfg_t dec_cfg = {
        .sample_rate = ESP_HF_LC3_SAMPLING_RATE_HZ,
        .channel = 1,
        .bits_per_sample = 16,
        .frame_dms = ESP_HF_LC3_FRAME_DURATION_US / 100,
        .nbyte = ESP_HF_LC3_ENCODED_FRAME_SIZE,
        .is_cbr = true,
        .len_prefixed = false,
        .enable_plc = true,
    };

    if (esp_lc3_enc_open(&enc_cfg, sizeof(enc_cfg), &s_ext_enc_handle) != ESP_AUDIO_ERR_OK) {
        return false;
    }
    if (esp_lc3_dec_open(&dec_cfg, sizeof(dec_cfg), &s_ext_dec_handle) != ESP_AUDIO_ERR_OK) {
        esp_lc3_enc_close(s_ext_enc_handle);
        s_ext_enc_handle = NULL;
        return false;
    }
    s_ext_codec_type = BT_APP_EXT_CODEC_LC3;
    s_ext_pcm_frame_bytes = SWB_PCM_INPUT_DATA_SIZE;
    s_ext_enc_frame_bytes = ESP_HF_LC3_ENCODED_FRAME_SIZE;
    return true;
}
#endif /* CONFIG_BT_HFP_LC3_ENABLE */

static bool bt_app_hf_ext_codec_init(esp_hf_audio_state_t audio_state)
{
    bool opened = false;

    if (audio_state == ESP_HF_AUDIO_STATE_CONNECTED_MSBC) {
        opened = bt_app_hf_ext_codec_open_msbc();
#if CONFIG_BT_HFP_LC3_ENABLE
    } else if (audio_state == ESP_HF_AUDIO_STATE_CONNECTED_LC3) {
        opened = bt_app_hf_ext_codec_open_lc3();
#endif
    }
    if (!opened) {
        return false;
    }
    if (!bt_app_hf_ext_codec_worker_start()) {
        bt_app_hf_ext_codec_deinit();
        return false;
    }
    ESP_LOGI(BT_HF_TAG, "ext codec ready: type=%s pcm=%u enc=%u",
             (s_ext_codec_type == BT_APP_EXT_CODEC_MSBC) ? "mSBC" :
#if CONFIG_BT_HFP_LC3_ENABLE
             (s_ext_codec_type == BT_APP_EXT_CODEC_LC3) ? "LC3-SWB" :
#endif
             "unknown",
             s_ext_pcm_frame_bytes, s_ext_enc_frame_bytes);
    return true;
}

static void bt_app_hf_ext_codec_deinit(void)
{
    bt_app_hf_ext_codec_worker_stop();
    if (s_ext_enc_handle) {
        if (s_ext_codec_type == BT_APP_EXT_CODEC_MSBC) {
            esp_sbc_enc_close(s_ext_enc_handle);
#if CONFIG_BT_HFP_LC3_ENABLE
        } else if (s_ext_codec_type == BT_APP_EXT_CODEC_LC3) {
            esp_lc3_enc_close(s_ext_enc_handle);
#endif
        }
        s_ext_enc_handle = NULL;
    }
    if (s_ext_dec_handle) {
        if (s_ext_codec_type == BT_APP_EXT_CODEC_MSBC) {
            esp_sbc_dec_close(s_ext_dec_handle);
#if CONFIG_BT_HFP_LC3_ENABLE
        } else if (s_ext_codec_type == BT_APP_EXT_CODEC_LC3) {
            esp_lc3_dec_close(s_ext_dec_handle);
#endif
        }
        s_ext_dec_handle = NULL;
    }
    if (s_ext_rx_queue) {
        vQueueDelete(s_ext_rx_queue);
        s_ext_rx_queue = NULL;
    }
    if (s_ext_tx_queue) {
        vQueueDelete(s_ext_tx_queue);
        s_ext_tx_queue = NULL;
    }
    if (s_ext_codec_done_sem) {
        vSemaphoreDelete(s_ext_codec_done_sem);
        s_ext_codec_done_sem = NULL;
    }
    s_ext_codec_type = BT_APP_EXT_CODEC_NONE;
    s_ext_pcm_frame_bytes = 0;
    s_ext_enc_frame_bytes = 0;
}

static bool bt_app_hf_ext_queue_tx_pcm(const uint8_t *pcm, uint32_t pcm_len)
{
    if (!s_ext_codec_active || s_ext_tx_queue == NULL || pcm_len == 0 ||
        pcm_len != s_ext_pcm_frame_bytes || pcm_len > EXT_CODEC_PCM_MAX_SIZE) {
        return false;
    }
    bt_app_ext_tx_item_t item = {0};
    memcpy(item.pcm, pcm, pcm_len);
    item.pcm_len = (uint16_t)pcm_len;
    if (xQueueSend(s_ext_tx_queue, &item, 0) == pdTRUE) {
        return true;
    }
    /* Drop oldest and keep newest. */
    bt_app_ext_tx_item_t drop;
    if (xQueueReceive(s_ext_tx_queue, &drop, 0) == pdTRUE) {
        return xQueueSend(s_ext_tx_queue, &item, 0) == pdTRUE;
    }
    return false;
}

static void bt_app_hf_ext_audio_cb(esp_hf_sync_conn_hdl_t sync_conn_hdl, esp_hf_audio_buff_t *audio_buff, bool is_bad_frame)
{
    (void)sync_conn_hdl;
    if (!s_ext_codec_active || s_ext_rx_queue == NULL) {
        if (audio_buff) {
            esp_hf_ag_audio_buff_free(audio_buff);
        }
        return;
    }

    bt_app_ext_rx_item_t item = {0};
    item.is_bad = is_bad_frame;
    if (!is_bad_frame) {
        if (audio_buff == NULL || audio_buff->data_len == 0 ||
            audio_buff->data_len > EXT_CODEC_ENC_MAX_SIZE) {
            if (audio_buff) {
                esp_hf_ag_audio_buff_free(audio_buff);
            }
            return;
        }
        memcpy(item.data, audio_buff->data, audio_buff->data_len);
        item.len = audio_buff->data_len;
    }
    if (audio_buff) {
        esp_hf_ag_audio_buff_free(audio_buff);
    }
    if (xQueueSend(s_ext_rx_queue, &item, 0) != pdTRUE) {
        ESP_LOGD(BT_HF_TAG, "ext codec RX queue full, drop frame");
    }
}

static bool bt_app_hf_ext_codec_active(void)
{
    return s_ext_codec_active;
}
#endif /* CONFIG_BT_HFP_USE_EXTERNAL_CODEC */

static void print_speed(void);

static uint32_t bt_app_hf_outgoing_cb(uint8_t *p_buf, uint32_t sz)
{
    size_t item_size = 0;
    uint8_t *data;
    if (!s_m_rb) {
        return 0;
    }
    vRingbufferGetInfo(s_m_rb, NULL, NULL, NULL, NULL, &item_size);
    if (item_size >= sz) {
        data = xRingbufferReceiveUpTo(s_m_rb, &item_size, 0, sz);
        memcpy(p_buf, data, item_size);
        vRingbufferReturnItem(s_m_rb, data);
        return sz;
    } else {
        // data not enough, do not read\n
        return 0;
    }
    return 0;
}

static void bt_app_hf_incoming_cb(const uint8_t *buf, uint32_t sz)
{
    s_time_new = esp_timer_get_time();
    s_data_num += sz;
    if ((s_time_new - s_time_old) >= 3000000) {
        print_speed();
    }
}

static uint32_t bt_app_hf_create_audio_data(uint8_t *p_buf, uint32_t sz)
{
    static int index = 0;
    uint8_t *data = (uint8_t *)sine_int16;

    for (uint32_t i = 0; i < sz; i++) {
        p_buf[i] = data[index++];
        if (index >= TABLE_SIZE_BYTE) {
            index -= TABLE_SIZE_BYTE;
        }
    }
    return sz;
}

static void print_speed(void)
{
    float tick_s = (s_time_new - s_time_old) / 1000000.0;
    float speed = s_data_num * 8 / tick_s / 1000.0;
    ESP_LOGI(BT_HF_TAG, "speed(%fs ~ %fs): %f kbit/s" , s_time_old / 1000000.0, s_time_new / 1000000.0, speed);
    s_data_num = 0;
    s_time_old = s_time_new;
}

static void bt_app_send_data_timer_cb(void *arg)
{
    if (!s_audio_tx_active || s_send_data_Semaphore == NULL) {
        return;
    }
    /* Binary semaphore: give may fail if send task is still encoding (ext codec). */
    if (xSemaphoreGive(s_send_data_Semaphore) != pdTRUE) {
        ESP_LOGD(BT_HF_TAG, "%s: send task busy, skip tick", __func__);
    }
}

static void bt_app_send_data_task(void *arg)
{
    uint64_t frame_data_num;
    size_t item_size = 0;
    uint8_t *buf = NULL;
    for (;;) {
        if (xSemaphoreTake(s_send_data_Semaphore, (TickType_t)portMAX_DELAY) != pdTRUE) {
            continue;
        }
        if (!s_audio_tx_active) {
            break;
        }

#if CONFIG_BT_HFP_USE_EXTERNAL_CODEC
        if (bt_app_hf_ext_codec_active()) {
            /* mSBC/LC3 external path bypasses the TX ringbuffer. Timer is one
             * 7.5 ms tick per frame: generate PCM and queue encode directly. */
            s_last_enter_time = esp_timer_get_time();
            bt_app_hf_create_audio_data(s_ext_tx_pcm_frame, s_ext_pcm_frame_bytes);
            bt_app_hf_ext_queue_tx_pcm(s_ext_tx_pcm_frame, s_ext_pcm_frame_bytes);
            continue;
        }
#endif
        {
            s_now_enter_time = esp_timer_get_time();
            s_us_duration = s_now_enter_time - s_last_enter_time;
            if (s_audio_code == ESP_HF_AUDIO_STATE_CONNECTED_MSBC) {
            // time of a frame is 7.5ms, sample is 120, data is 2 (byte/sample), so a frame is 240 byte (HF_SBC_ENC_RAW_DATA_SIZE)
                frame_data_num = s_us_duration / PCM_BLOCK_DURATION_US * WBS_PCM_INPUT_DATA_SIZE;
                s_last_enter_time += frame_data_num / WBS_PCM_INPUT_DATA_SIZE * PCM_BLOCK_DURATION_US;
            } else {
                frame_data_num = s_us_duration / PCM_BLOCK_DURATION_US * PCM_INPUT_DATA_SIZE;
                s_last_enter_time += frame_data_num / PCM_INPUT_DATA_SIZE * PCM_BLOCK_DURATION_US;
            }
            if (frame_data_num == 0) {
                continue;
            }
            buf = (uint8_t *)malloc(frame_data_num);
            if (!buf) {
                ESP_LOGE(BT_HF_TAG, "%s, no mem", __FUNCTION__);
                continue;
            }
            bt_app_hf_create_audio_data(buf, frame_data_num);
            BaseType_t done = xRingbufferSend(s_m_rb, buf, frame_data_num, 0);
            if (!done) {
                ESP_LOGE(BT_HF_TAG, "rb send fail");
            }
            free(buf);
            vRingbufferGetInfo(s_m_rb, NULL, NULL, NULL, NULL, &item_size);

            if(s_audio_code == ESP_HF_AUDIO_STATE_CONNECTED_MSBC) {
                if(item_size >= WBS_PCM_INPUT_DATA_SIZE) {
                    esp_hf_ag_outgoing_data_ready();
                }
            } else {
                if(item_size >= PCM_INPUT_DATA_SIZE) {
                    esp_hf_ag_outgoing_data_ready();
                }
            }
        }
    }
    if (s_send_data_done_sem) {
        xSemaphoreGive(s_send_data_done_sem);
    }
    vTaskDelete(NULL);
}

static void bt_app_stop_periodic_timer(void)
{
    if (s_periodic_timer && s_periodic_timer_running) {
        esp_err_t err = esp_timer_stop(s_periodic_timer);
        if (err != ESP_OK && err != ESP_ERR_INVALID_STATE) {
            ESP_LOGW(BT_HF_TAG, "esp_timer_stop failed: %s", esp_err_to_name(err));
        }
        s_periodic_timer_running = false;
    }
}

static void bt_app_audio_tx_stop(void)
{
    if (!s_audio_tx_active && s_bt_app_send_data_task_handler == NULL) {
        return;
    }
    s_audio_tx_active = false;
    s_sync_conn_hdl = ESP_INVALID_CONN_HANDLE;

    bt_app_stop_periodic_timer();
    if (s_send_data_Semaphore) {
        xSemaphoreGive(s_send_data_Semaphore);
    }
    if (s_bt_app_send_data_task_handler) {
        if (s_send_data_done_sem == NULL ||
            xSemaphoreTake(s_send_data_done_sem, pdMS_TO_TICKS(TASK_STOP_TIMEOUT_MS)) != pdTRUE) {
            ESP_LOGE(BT_HF_TAG, "send data task stop timeout");
        }
        s_bt_app_send_data_task_handler = NULL;
    }
}

void bt_app_hf_prepare_audio_disconnect(void)
{
    bt_app_audio_tx_stop();
}

void bt_app_send_data(void)
{
    if (s_periodic_timer || s_bt_app_send_data_task_handler) {
        bt_app_send_data_shut_down();
    }
    s_send_data_Semaphore = xSemaphoreCreateBinary();
    if (s_send_data_done_sem == NULL) {
        s_send_data_done_sem = xSemaphoreCreateBinary();
    }
    xTaskCreate(bt_app_send_data_task, "BtAppSendDataTask", 4 * 1024, NULL, configMAX_PRIORITIES - 3, &s_bt_app_send_data_task_handler);
#if CONFIG_BT_HFP_USE_EXTERNAL_CODEC
    if (!bt_app_hf_ext_codec_active())
#endif
    {
        s_m_rb = xRingbufferCreate(ESP_HFP_RINGBUF_SIZE, RINGBUF_TYPE_BYTEBUF);
    }
    const esp_timer_create_args_t c_periodic_timer_args = {
            .callback = &bt_app_send_data_timer_cb,
            .name = "periodic"
    };
    ESP_ERROR_CHECK(esp_timer_create(&c_periodic_timer_args, &s_periodic_timer));
#if CONFIG_BT_HFP_USE_EXTERNAL_CODEC
    /* External mSBC/LC3 frame period is 7.5 ms. */
    uint64_t tick_us = bt_app_hf_ext_codec_active() ?
                       PCM_BLOCK_DURATION_US : PCM_GENERATOR_TICK_US;
#else
    uint64_t tick_us = PCM_GENERATOR_TICK_US;
#endif
    ESP_ERROR_CHECK(esp_timer_start_periodic(s_periodic_timer, tick_us));
    s_periodic_timer_running = true;
    s_last_enter_time = esp_timer_get_time();
    s_audio_tx_active = true;
    return;
}

void bt_app_send_data_shut_down(void)
{
    bt_app_audio_tx_stop();
    bt_app_stop_periodic_timer();
    if (s_periodic_timer) {
        esp_err_t err = esp_timer_delete(s_periodic_timer);
        if (err != ESP_OK) {
            ESP_LOGW(BT_HF_TAG, "esp_timer_delete failed: %s", esp_err_to_name(err));
        }
        s_periodic_timer = NULL;
    }
    if (s_send_data_Semaphore) {
        vSemaphoreDelete(s_send_data_Semaphore);
        s_send_data_Semaphore = NULL;
    }
    if (s_send_data_done_sem) {
        vSemaphoreDelete(s_send_data_done_sem);
        s_send_data_done_sem = NULL;
    }
    if (s_m_rb) {
        vRingbufferDelete(s_m_rb);
        s_m_rb = NULL;
    }
    return;
}
#endif /* #if CONFIG_BT_HFP_AUDIO_DATA_PATH_HCI */

void bt_app_hf_cb(esp_hf_cb_event_t event, esp_hf_cb_param_t *param)
{
    if (event <= ESP_HF_BAC_RESPONSE_EVT) {
        ESP_LOGI(BT_HF_TAG, "APP HFP event: %s", c_hf_evt_str[event]);
    } else {
        ESP_LOGE(BT_HF_TAG, "APP HFP invalid event %d", event);
    }

    switch (event) {
        case ESP_HF_CONNECTION_STATE_EVT:
        {
            ESP_LOGI(BT_HF_TAG, "--connection state %s, peer feats 0x%"PRIx32", chld_feats 0x%"PRIx32,
                    c_connection_state_str[param->conn_stat.state],
                    param->conn_stat.peer_feat,
                    param->conn_stat.chld_feat);
            memcpy(hf_peer_addr, param->conn_stat.remote_bda, ESP_BD_ADDR_LEN);
            break;
        }

        case ESP_HF_AUDIO_STATE_EVT:
        {
            const char *audio_state = (param->audio_stat.state <= ESP_HF_AUDIO_STATE_CONNECTED_LC3 &&
                                       c_audio_state_str[param->audio_stat.state]) ?
                                      c_audio_state_str[param->audio_stat.state] : "unknown";
            ESP_LOGI(BT_HF_TAG, "--Audio State %s", audio_state);
#if CONFIG_BT_HFP_AUDIO_DATA_PATH_HCI
            if (param->audio_stat.state == ESP_HF_AUDIO_STATE_CONNECTED ||
                param->audio_stat.state == ESP_HF_AUDIO_STATE_CONNECTED_MSBC ||
                param->audio_stat.state == ESP_HF_AUDIO_STATE_CONNECTED_LC3)
            {
                s_sync_conn_hdl = param->audio_stat.sync_conn_handle;
                s_audio_code = param->audio_stat.state;
                s_time_old = esp_timer_get_time();
#if CONFIG_BT_HFP_USE_EXTERNAL_CODEC
                if (param->audio_stat.state == ESP_HF_AUDIO_STATE_CONNECTED_MSBC
#if CONFIG_BT_HFP_LC3_ENABLE
                    || param->audio_stat.state == ESP_HF_AUDIO_STATE_CONNECTED_LC3
#endif
                    ) {
                    if (!bt_app_hf_ext_codec_init(param->audio_stat.state)) {
                        ESP_LOGE(BT_HF_TAG, "external codec init failed");
                        break;
                    }
                    esp_hf_ag_register_audio_data_callback(bt_app_hf_ext_audio_cb);
                } else {
                    /* CVSD with EXTERNAL_CODEC: this demo TX path stays on legacy PCM APIs
                     * and is not the main supported case. Prefer mSBC/LC3 for external codec. */
                    ESP_LOGW(BT_HF_TAG, "CVSD + EXTERNAL_CODEC: using legacy PCM callbacks");
                    esp_hf_ag_register_data_callback(bt_app_hf_incoming_cb, bt_app_hf_outgoing_cb);
                }
#else
                esp_hf_ag_register_data_callback(bt_app_hf_incoming_cb, bt_app_hf_outgoing_cb);
                /* Disable connectable and discoverable mode to save the over-the-air bandwidth and ensure audio quality  */
                esp_bt_gap_set_scan_mode(ESP_BT_NON_CONNECTABLE, ESP_BT_NON_DISCOVERABLE);
                /* Begin send esco data task */
#if CONFIG_BT_HFP_LC3_ENABLE
                if (param->audio_stat.state == ESP_HF_AUDIO_STATE_CONNECTED_LC3) {
                    ESP_LOGE(BT_HF_TAG, "LC3-SWB requires CONFIG_BT_HFP_USE_EXTERNAL_CODEC");
                    break;
                }
#endif
                esp_hf_ag_register_data_callback(bt_app_hf_incoming_cb, bt_app_hf_outgoing_cb);
#endif /* CONFIG_BT_HFP_USE_EXTERNAL_CODEC */
                bt_app_send_data();
            } else if (param->audio_stat.state == ESP_HF_AUDIO_STATE_DISCONNECTED) {
                ESP_LOGI(BT_HF_TAG, "--ESP AG Audio Connection Disconnected.");
                bt_app_send_data_shut_down();
#if CONFIG_BT_HFP_USE_EXTERNAL_CODEC
                bt_app_hf_ext_codec_deinit();
#endif
                s_sync_conn_hdl = ESP_INVALID_CONN_HANDLE;
                bt_app_send_data_shut_down();
                /* Resume connectable and discoverable mode */
                esp_bt_gap_set_scan_mode(ESP_BT_CONNECTABLE, ESP_BT_GENERAL_DISCOVERABLE);
            }
#endif /* #if CONFIG_BT_HFP_AUDIO_DATA_PATH_HCI */
            break;
        }

        case ESP_HF_BVRA_RESPONSE_EVT:
        {
            ESP_LOGI(BT_HF_TAG, "--Voice Recognition is %s", c_vr_state_str[param->vra_rep.value]);
            break;
        }

        case ESP_HF_VOLUME_CONTROL_EVT:
        {
            ESP_LOGI(BT_HF_TAG, "--Volume Target: %s, Volume %d", c_volume_control_target_str[param->volume_control.type], param->volume_control.volume);
            break;
        }

        case ESP_HF_UNAT_RESPONSE_EVT:
        {
            ESP_LOGI(BT_HF_TAG, "--UNKOW AT CMD: %s", param->unat_rep.unat);
            esp_hf_ag_unknown_at_send(param->unat_rep.remote_addr, NULL);
            break;
        }

        case ESP_HF_IND_UPDATE_EVT:
        {
            ESP_LOGI(BT_HF_TAG, "--UPDATE INDICATOR!");
            esp_hf_call_status_t call_state = 1;
            esp_hf_call_setup_status_t call_setup_state = 2;
            esp_hf_network_state_t ntk_state = 1;
            int signal = 2;
            int battery = 3;
            esp_hf_ag_ciev_report(param->ind_upd.remote_addr, ESP_HF_IND_TYPE_CALL, call_state);
            esp_hf_ag_ciev_report(param->ind_upd.remote_addr, ESP_HF_IND_TYPE_CALLSETUP, call_setup_state);
            esp_hf_ag_ciev_report(param->ind_upd.remote_addr, ESP_HF_IND_TYPE_SERVICE, ntk_state);
            esp_hf_ag_ciev_report(param->ind_upd.remote_addr, ESP_HF_IND_TYPE_SIGNAL, signal);
            esp_hf_ag_ciev_report(param->ind_upd.remote_addr, ESP_HF_IND_TYPE_BATTCHG, battery);
            break;
        }

        case ESP_HF_CIND_RESPONSE_EVT:
        {
            ESP_LOGI(BT_HF_TAG, "--CIND Start.");
            esp_hf_call_status_t call_status = 0;
            esp_hf_call_setup_status_t call_setup_status = 0;
            esp_hf_network_state_t ntk_state = 1;
            int signal = 4;
            esp_hf_roaming_status_t roam = 0;
            int batt_lev = 3;
            esp_hf_call_held_status_t call_held_status = 0;
            esp_hf_ag_cind_response(param->cind_rep.remote_addr,call_status,call_setup_status,ntk_state,signal,roam,batt_lev,call_held_status);
            break;
        }

        case ESP_HF_COPS_RESPONSE_EVT:
        {
            const int svc_type = 1;
            esp_hf_ag_cops_response(param->cops_rep.remote_addr, c_operator_name_str[svc_type]);
            break;
        }

        case ESP_HF_CLCC_RESPONSE_EVT:
        {
            int index = 1;
            //mandatory
            esp_hf_current_call_direction_t dir = 1;
            esp_hf_current_call_status_t current_call_status = 0;
            esp_hf_current_call_mode_t mode = 0;
            esp_hf_current_call_mpty_type_t mpty = 0;
            //option
            char *number = {"123456"};
            esp_hf_call_addr_type_t type = ESP_HF_CALL_ADDR_TYPE_UNKNOWN;

            ESP_LOGI(BT_HF_TAG, "--Calling Line Identification.");
            esp_hf_ag_clcc_response(param->clcc_rep.remote_addr, index, dir, current_call_status, mode, mpty, number, type);

            //AG shall always send ok response to HF
            //index = 0 means response ok
            index = 0;
            esp_hf_ag_clcc_response(param->clcc_rep.remote_addr, index, dir, current_call_status, mode, mpty, number, type);
            break;
        }

        case ESP_HF_CNUM_RESPONSE_EVT:
        {
            char *number = {"123456"};
            int number_type = 129;
            esp_hf_subscriber_service_type_t service_type = ESP_HF_SUBSCRIBER_SERVICE_TYPE_VOICE;
            if (service_type == ESP_HF_SUBSCRIBER_SERVICE_TYPE_VOICE || service_type == ESP_HF_SUBSCRIBER_SERVICE_TYPE_FAX) {
                ESP_LOGI(BT_HF_TAG, "--Current Number is %s, Number Type is %d, Service Type is %s.", number, number_type, c_subscriber_service_type_str[service_type - 3]);
            } else {
                ESP_LOGI(BT_HF_TAG, "--Current Number is %s, Number Type is %d, Service Type is %s.", number, number_type, c_subscriber_service_type_str[0]);
            }
            esp_hf_ag_cnum_response(hf_peer_addr, number, number_type, service_type);
            break;
        }

        case ESP_HF_VTS_RESPONSE_EVT:
        {
            ESP_LOGI(BT_HF_TAG, "--DTMF code is: %s.", param->vts_rep.code);
            break;
        }

        case ESP_HF_NREC_RESPONSE_EVT:
        {
            ESP_LOGI(BT_HF_TAG, "--NREC status is: %s.", c_nrec_status_str[param->nrec.state]);
            break;
        }

        case ESP_HF_ATA_RESPONSE_EVT:
        {
            ESP_LOGI(BT_HF_TAG, "--Asnwer Incoming Call.");
            char *number = {"123456"};
            esp_hf_ag_answer_call(param->ata_rep.remote_addr,1,0,1,0,number,0);
            break;
        }

        case ESP_HF_CHUP_RESPONSE_EVT:
        {
            ESP_LOGI(BT_HF_TAG, "--Reject Incoming Call.");
            char *number = {"123456"};
            esp_hf_ag_reject_call(param->chup_rep.remote_addr,0,0,0,0,number,0);
            break;
        }

        case ESP_HF_DIAL_EVT:
        {
            if (param->out_call.num_or_loc) {
                if (param->out_call.type == ESP_HF_DIAL_NUM) {
                    // dia_num
                    ESP_LOGI(BT_HF_TAG, "--Dial number \"%s\".", param->out_call.num_or_loc);
                    esp_hf_ag_cmee_send(param->out_call.remote_addr, ESP_HF_AT_RESPONSE_CODE_OK, ESP_HF_CME_AG_FAILURE);
                    esp_hf_ag_out_call(param->out_call.remote_addr,1,0,1,0,param->out_call.num_or_loc,0);
                } else if (param->out_call.type == ESP_HF_DIAL_MEM) {
                    // dia_mem
                    ESP_LOGI(BT_HF_TAG, "--Dial memory \"%s\".", param->out_call.num_or_loc);
                    // AG found phone number by memory position
                    bool num_found = true;
                    if (num_found) {
                        char *number = "123456";
                        esp_hf_ag_cmee_send(param->out_call.remote_addr, ESP_HF_AT_RESPONSE_CODE_OK, ESP_HF_CME_AG_FAILURE);
                        esp_hf_ag_out_call(param->out_call.remote_addr,1,0,1,0,number,0);
                    } else {
                        esp_hf_ag_cmee_send(param->out_call.remote_addr, ESP_HF_AT_RESPONSE_CODE_CME, ESP_HF_CME_MEMORY_FAILURE);
                    }
                }
            } else {
                //dia_last
                //refer to dia_mem
                ESP_LOGI(BT_HF_TAG, "--Dial last number.");
            }
            break;
        }
#if (CONFIG_BT_HFP_WBS_ENABLE)
        case ESP_HF_BAC_RESPONSE_EVT:
        {
            uint16_t caps = param->bac_rep.peer_codecs;
            ESP_LOGI(BT_HF_TAG, "--Peer codec caps: 0x%04x (CVSD=%d mSBC=%d LC3=%d)",
                     caps,
                     !!(caps & ESP_HF_CODEC_CAP_CVSD),
                     !!(caps & ESP_HF_CODEC_CAP_MSBC),
                     !!(caps & ESP_HF_CODEC_CAP_LC3));
            /* Codec preference is set in btc_hf_ag on BAC; avoid duplicate esp_hf_ag_set_codec()
             * here while SLC is not up yet (AT+BAC arrives before SLC_CONNECTED). */
            break;
        }
        case ESP_HF_WBS_RESPONSE_EVT:
        {
            if (param->wbs_rep.status == ESP_BT_STATUS_SUCCESS) {
                ESP_LOGI(BT_HF_TAG, "--Codec preference set: %s", c_codec_mode_str[param->wbs_rep.codec]);
            } else {
                ESP_LOGI(BT_HF_TAG, "--Codec preference rejected: %s", c_codec_mode_str[param->wbs_rep.codec]);
            }
            break;
        }
#endif
        case ESP_HF_BCS_RESPONSE_EVT:
        {
            ESP_LOGI(BT_HF_TAG, "--Consequence of codec negotiation: %s",c_codec_mode_str[param->bcs_rep.mode]);
            break;
        }
        case ESP_HF_PKT_STAT_NUMS_GET_EVT:
        {
            ESP_LOGI(BT_HF_TAG, "ESP_HF_PKT_STAT_NUMS_GET_EVT: %d.", event);
            break;
        }
        case ESP_HF_PROF_STATE_EVT:
        {
            if (ESP_HF_INIT_SUCCESS == param->prof_stat.state) {
                ESP_LOGI(BT_HF_TAG, "AG PROF STATE: Init Complete");
            } else if (ESP_HF_DEINIT_SUCCESS == param->prof_stat.state) {
                ESP_LOGI(BT_HF_TAG, "AG PROF STATE: Deinit Complete");
            } else {
                ESP_LOGE(BT_HF_TAG, "AG PROF STATE error: %d", param->prof_stat.state);
            }
            break;
        }

        default:
            ESP_LOGI(BT_HF_TAG, "Unsupported HF_AG EVT: %d.", event);
            break;

    }
}
