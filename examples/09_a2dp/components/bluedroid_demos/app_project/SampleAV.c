#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"

#include "esp_system.h"
// #include "EspAudio.h"
// #include "EspAudioCom.h"

#include "bt_app_common.h"
#include "esp_bt_main.h"
#include "esp_gap_bt_api.h"
#include "esp_a2dp_api.h"
#include "esp_avrc_api.h"

typedef enum {
    BT_APP_EVT_STACK_ON = 0xa0,
    BT_APP_EVT_MAX
} bt_app_evt_t;


typedef union {
    esp_a2d_cb_param_t a2d;
    esp_avrc_ct_cb_event_t rc;
} bt_app_evt_arg;

static esp_a2d_audio_state_t m_audio_state = ESP_A2D_AUDIO_STATE_STOPPED;

static void bt_app_handle_evt(uint16_t event, void *p_param);

static void bt_app_handle_rc_evt(uint16_t event, void *p_param);

static void bt_app_a2d_cb(esp_a2d_cb_event_t event, esp_a2d_cb_param_t *param)
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

static void bt_app_a2d_data_cb(const uint8_t *data, uint32_t len)
{
    // EspAudioPlayerStreamWrite((uint8_t *)data, len, 10);
}

static void bt_app_rc_ct_cb(esp_avrc_ct_cb_event_t event, esp_avrc_ct_cb_param_t *param)
{
    switch (event) {
    case ESP_AVRC_CT_CONNECTION_STATE_EVT:
    case ESP_AVRC_CT_PASSTHROUGH_RSP_EVT: {
        bt_app_transfer_context(bt_app_handle_rc_evt, event, param, sizeof(bt_app_evt_arg), NULL);
        break;
    }
    default:
        BT_APP_TRACE_ERROR("===a2dp invalid cb event: %d\n", event);
        break;
    }
}

static void bt_app_handle_evt(uint16_t event, void *p_param)
{
    BT_APP_TRACE_DEBUG("bt_app_handle_evt 0x%x\n", event);
    esp_a2d_cb_param_t *a2d = NULL;
    switch (event) {
    case BT_APP_EVT_STACK_ON: {
        char *dev_name = "ESP_SPEAKER";

	esp_bt_gap_set_device_name(dev_name);

        esp_a2d_register_callback(&bt_app_a2d_cb);
        esp_a2d_register_data_callback(bt_app_a2d_data_cb);

        esp_a2d_sink_init();

        esp_avrc_ct_init();
        esp_avrc_ct_register_callback(bt_app_rc_ct_cb);
        
        esp_bt_gap_set_scan_mode(ESP_BT_SCAN_MODE_CONNECTABLE_DISCOVERABLE);
        break;
    }
    case ESP_A2D_CONNECTION_STATE_EVT: {
        a2d = (esp_a2d_cb_param_t *)(p_param);
        BT_APP_TRACE_EVENT("===a2dp conn_state_cb %d ===\n", a2d->conn_stat.state);
        break;
    }
    case ESP_A2D_AUDIO_STATE_EVT: {
        a2d = (esp_a2d_cb_param_t *)(p_param);
        BT_APP_TRACE_EVENT("===a2dp audio_state_cb %d===\n", a2d->audio_stat.state);
        m_audio_state = a2d->audio_stat.state;
        break;
    }
    case ESP_A2D_AUDIO_CFG_EVT: {
        a2d = (esp_a2d_cb_param_t *)(p_param);
        BT_APP_TRACE_EVENT("===a2dp audio_cfg_cb type %d ===\n", a2d->audio_cfg.mcc.type);
        if (a2d->audio_cfg.mcc.type == ESP_A2D_MCT_SBC) {
            // temporarily hardcoded the PCM configuaration
            BT_APP_TRACE_EVENT("configure audio player\n");
            // EspAudioPlayerStreamCfg(StreamSampleRate_44k, 2, StreamBitLen_16BIT);
            // EspAudio_SetupStream("stream.pcm", InputSrcType_Stream);
            // EspAudio_SetVolume(99);
        }
        break;
    }
    default:
        BT_APP_TRACE_ERROR("===application invalid event: %d\n", event);
        break;
    }
    
}

void bt_app_handle_rc_evt(uint16_t event, void *p_param)
{
    BT_APP_TRACE_DEBUG("bt_app_handle_avrc_evt 0x%x\n", event);
    esp_avrc_ct_cb_param_t *rc = (esp_avrc_ct_cb_param_t *)(p_param);
    switch (event) {
    case ESP_AVRC_CT_CONNECTION_STATE_EVT: {
        uint8_t *bda = rc->conn_stat.remote_bda;
        BT_APP_TRACE_EVENT("===avrc conn_state evt: state %d, feature 0x%x, [%02x:%02x:%02x:%02x:%02x:%02x]===\n",
                           rc->conn_stat.connected, rc->conn_stat.feat_mask, bda[0], bda[1], bda[2], bda[3], bda[4], bda[5]);
        break;
    }
    case ESP_AVRC_CT_PASSTHROUGH_RSP_EVT: {
        BT_APP_TRACE_EVENT("===avrc passthrough rsp: key_code 0x%x, key_state %d===\n", rc->psth_rsp.key_code, rc->psth_rsp.key_state);
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
    // init = esp_bt_init_stack();
    init = esp_bluedroid_init();
    if (init != ESP_OK) {
        return;
    }

    // enable = esp_bt_enable_stack();
    enable = esp_bluedroid_enable();
    if (enable != ESP_OK) {
        return;
    }

    bt_app_transfer_context(bt_app_handle_evt, BT_APP_EVT_STACK_ON, NULL, 0, NULL);
}
