#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"

#include "esp_system.h"
#include "EspAudio.h"
#include "EspAudioCom.h"

#include "bt_app_common.h"
#include "esp_bt_stack_manager.h"
#include "esp_gap_bt_api.h"
#include "esp_a2dp_api.h"

#include "bt_rc.h"

typedef enum {
    BT_APP_EVT_STACK_ON = 0xa0,
    BT_APP_EVT_MAX
} bt_app_evt_t;

typedef struct {
    bool state;
    bt_bdaddr_t bd_addr;
} esp_avrc_conn_state_t;

typedef struct {
    int id;
    int key_state;
} esp_avrc_passthrough_rsp_t;

typedef struct {
    int id;
    int key_state;
} esp_avrc_key_state_t;

typedef union {
    esp_a2d_cb_param_t a2d;
    esp_avrc_conn_state_t avrc_state;
    esp_avrc_passthrough_rsp_t avrc_passthrough_rsp;
    esp_avrc_key_state_t avrc_key;
} bt_app_evt_arg;

/// AVRC callback events
typedef enum {
    ESP_AVRC_CONNECTION_STATE_EVT = 5,           /*!< connection state changed event */
    ESP_AVRC_PASSTHROUGH_RSP_EVT,                /*!< AVRC PASSTHROUGH commands */
    ESP_AVRC_KEY_STATE_TO
} esp_avrc_cb_event_t;

static esp_a2d_audio_state_t m_audio_state = ESP_A2D_AUDIO_STATE_STOPPED;
static TimerHandle_t m_key_tmr = 0;
static int m_key_state = 1; // 0 for pressed, 1 for released
static xTaskHandle  xKeyTaskHandle = 0;

static void bt_app_handle_evt(uint16_t event, void *p_param);

static void key_press_task_handler(void *arg)
{
    int key_id = 0x48; // rewind
    for(;;) {
        if (m_audio_state != ESP_A2D_AUDIO_STATE_STARTED) {
            BT_APP_TRACE_EVENT("-----key_tmr_hdlr, return, audio state: %d\n", m_audio_state);
            vTaskDelay(5000 / portTICK_PERIOD_MS);
            continue;
        }

        bt_app_evt_arg param;
        memset(&param, 0, sizeof(bt_app_evt_arg));
        if (m_key_state == 1) {
            param.avrc_key.key_state = 1;
            m_key_state = 0;
            vTaskDelay(5000 / portTICK_PERIOD_MS);
        } else {
            param.avrc_key.key_state = 0;
            m_key_state = 1;
            vTaskDelay(30 / portTICK_PERIOD_MS);
        }
        param.avrc_key.id = key_id; // 0x41 volume up, 0x4b FORWARD

        BT_APP_TRACE_EVENT("-----key_task_hdlr: %d, key_id %d---\n", m_key_state, param.avrc_key.id);
        bt_app_transfer_context(bt_app_handle_evt, ESP_AVRC_KEY_STATE_TO, &param, sizeof(bt_app_evt_arg), NULL);
    }
}

static void key_tmr_handler(TimerHandle_t xTimer)
{
    if (m_audio_state != ESP_A2D_AUDIO_STATE_STARTED) {
        BT_APP_TRACE_EVENT("-----key_tmr_hdlr, return, audio state: %d\n", m_audio_state);
        return;
    }
            
    bt_app_evt_arg param;
    memset(&param, 0, sizeof(bt_app_evt_arg));
    if (m_key_state == 1) {
        param.avrc_key.key_state = 1;
        m_key_state = 0;
    } else {
        param.avrc_key.key_state = 0;
        m_key_state = 1;
    }
    param.avrc_key.id = 0x41; // volume up
    BT_APP_TRACE_EVENT("-----key_tmr_hdlr: %d, key_id %d---\n", m_key_state, param.avrc_key.id);
    bt_app_transfer_context(bt_app_handle_evt, ESP_AVRC_KEY_STATE_TO, &param, sizeof(bt_app_evt_arg), NULL);
}

static void bt_app_a2d_cb(uint32_t event, void *param)
{
    switch (event) {
    case ESP_A2D_CONNECTION_STATE_EVT:
    case ESP_A2D_AUDIO_STATE_EVT:
    case ESP_A2D_AUDIO_CFG_EVT:
    {
	bt_app_transfer_context(bt_app_handle_evt, event, param, sizeof(bt_app_evt_arg), NULL);
        break;
    }
    default:
        BT_APP_TRACE_ERROR("===a2dp invalid cb event: %d\n", event);
        break;
    }
}

static void btrc_passthrough_rsp_cb(int id, int key_state)
{
    bt_app_evt_arg param;
    memset(&param, 0, sizeof(bt_app_evt_arg));
    param.avrc_passthrough_rsp.id = id;
    param.avrc_passthrough_rsp.key_state = key_state;
    bt_app_transfer_context(bt_app_handle_evt, ESP_AVRC_PASSTHROUGH_RSP_EVT, &param, sizeof(bt_app_evt_arg), NULL);
}

static void btrc_conn_state_cb(bool state, bt_bdaddr_t *bd_addr)
{
    bt_app_evt_arg param;
    memset(&param, 0, sizeof(bt_app_evt_arg));
    param.avrc_state.state = state;
    memcpy(&param.avrc_state.bd_addr, bd_addr, sizeof(bt_bdaddr_t));
    bt_app_transfer_context(bt_app_handle_evt, ESP_AVRC_CONNECTION_STATE_EVT, &param, sizeof(bt_app_evt_arg), NULL);
}

static btrc_ctrl_callbacks_t btrc_ctrl_cb = {
    sizeof(btrc_ctrl_callbacks_t),
    btrc_passthrough_rsp_cb,
    btrc_conn_state_cb
};

static void bt_app_a2d_data_cb(const uint8_t *data, uint32_t len)
{
    EspAudioPlayerStreamWrite((uint8_t *)data, len, 10);
}
			    
static void bt_app_handle_evt(uint16_t event, void *p_param)
{
    BT_APP_TRACE_DEBUG("bt_app_handle_evt 0x%x\n", event);
    esp_a2d_cb_param_t *a2d = NULL;
    switch (event) {
    case BT_APP_EVT_STACK_ON: {
        char *dev_name = "ESP_SPEAKER";
	esp_bt_gap_set_device_name(dev_name);

        esp_a2d_register_callback(bt_app_a2d_cb);
        esp_a2d_register_data_callback(bt_app_a2d_data_cb);

        esp_a2d_sink_init();
	
	btrc_ctrl_init(&btrc_ctrl_cb);
        esp_bt_gap_set_scan_mode(BT_SCAN_MODE_CONNECTABLE_DISCOVERABLE);
        break;
    }
    case ESP_A2D_CONNECTION_STATE_EVT: {
        a2d = (esp_a2d_cb_param_t *)(p_param);
        BT_APP_TRACE_EVENT("===a2dp conn_state_cb %d ===\n", a2d->conn_stat.state);
        break;
    }
    case ESP_A2D_AUDIO_STATE_EVT: {
        a2d = (esp_a2d_cb_param_t *)(p_param);
        BT_APP_TRACE_EVENT("===a2dp audio_state_cb %d, %d===\n", a2d->audio_stat.state, (int)m_key_tmr);
        m_audio_state = a2d->audio_stat.state;
        if (m_audio_state == ESP_A2D_AUDIO_STATE_STARTED &&
            m_key_tmr == 0) {
            BT_APP_TRACE_EVENT("mm1\n");
            xTaskCreate(key_press_task_handler, "keyT", 2048, NULL, 10, &xKeyTaskHandle);
            #if 0
            int32_t key_tmr_id = 10;
            m_key_tmr = xTimerCreate("appKeyTmr", 3000 / portTICK_PERIOD_MS, pdTRUE, (void *) key_tmr_id, key_tmr_handler);
            if (xTimerStart(m_key_tmr, 10 / portTICK_PERIOD_MS) != pdTRUE) {
                BT_APP_TRACE_EVENT(" timer start failed\n");
            }
            #endif
        }
        break;
    }
    case ESP_A2D_AUDIO_CFG_EVT: {
        a2d = (esp_a2d_cb_param_t *)(p_param);
        BT_APP_TRACE_EVENT("===a2dp audio_cfg_cb type %d ===\n", a2d->audio_cfg.mcc.type);
        if (a2d->audio_cfg.mcc.type == ESP_A2D_MCT_SBC) {
            // temporarily hardcoded the PCM configuaration
            BT_APP_TRACE_EVENT("configure audio player\n");
            EspAudioPlayerStreamCfg(StreamSampleRate_44k, 2, StreamBitLen_16BIT);
            EspAudio_SetupStream("stream.pcm", InputSrcType_Stream);
            EspAudio_SetVolume(99);
        }
        break;
    }
    case ESP_AVRC_CONNECTION_STATE_EVT: {
        esp_avrc_conn_state_t *conn_state = (esp_avrc_conn_state_t *)(p_param);
        BT_APP_TRACE_EVENT("===avrc conn_state evt %d ===\n", conn_state->state);
        break;
    }
    case ESP_AVRC_PASSTHROUGH_RSP_EVT: {
        esp_avrc_passthrough_rsp_t *passthrough_rsp = (esp_avrc_passthrough_rsp_t *)(p_param);
        BT_APP_TRACE_EVENT("===avrc passthrough evt id 0x%x, key_state %d===\n", passthrough_rsp->id, passthrough_rsp->key_state);
        break;
    }
    case ESP_AVRC_KEY_STATE_TO: {
        esp_avrc_key_state_t *key_s = (esp_avrc_key_state_t *)(p_param);
        BT_APP_TRACE_EVENT("===avrc send key id 0x%x, state %d\n", key_s->id, key_s->key_state);
        btrc_ctrl_send_passthrough_cmd(NULL, key_s->id, key_s->key_state);
        break;
    }
    default:
        BT_APP_TRACE_ERROR("===application invalid event: %d\n", event);
        break;
    }
    
}

void app_main_entry(void)
{
    esp_err_t init, enable;
    init = esp_bt_init_stack();
    if (init != ESP_OK) {
        return;
    }

    enable = esp_bt_enable_stack();
    if (enable != ESP_OK) {
        return;
    }

    bt_app_transfer_context(bt_app_handle_evt, BT_APP_EVT_STACK_ON, NULL, 0, NULL);
}
