#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_system.h"
// #include "EspAudio.h"
// #include "EspAudioCom.h"

#include "bt_app_common.h"
#include "esp_bt_stack_manager.h"
#include "esp_gap_bt_api.h"
// #include "bta_api.h"
#include "esp_a2dp_api.h"

typedef enum {
    BT_APP_EVT_STACK_ON = 0xa0,
    BT_APP_EVT_MAX
} bt_app_evt_t;

typedef union {
    esp_a2d_cb_param_t a2d;
} bt_app_evt_arg;

static void bt_app_handle_evt(uint16_t event, void *p_param);

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

static void bt_app_a2d_data_cb(const uint8_t *data, uint32_t len)
{
    // EspAudioPlayerStreamWrite((uint8_t *)data, len, 10);
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
        BT_APP_TRACE_EVENT("===a2dp audio_state_cb %d ===\n", a2d->audio_stat.state);
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
