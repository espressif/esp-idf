#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_system.h"
#include "EspAudio.h"
#include "EspAudioCom.h"

#include "bt_app_common.h"
#include "btif_stack_manager.h"
#include "esp_gap_bt_api.h"
#include "bta_api.h"
#include "esp_a2dp_api.h"

/* utl_set_device_class() */
#include "utl.h"

typedef enum {
    BT_APP_EVT_STACK_ON = 0xa0,
    BT_APP_EVT_MAX
} bt_app_evt_t;

typedef union {
    esp_a2d_cb_param_t a2d;
} bt_app_evt_arg;

static void bt_app_handle_evt(UINT16 event, void *p_param);

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

static void bt_app_a2d_data_cb(uint8_t *data, uint32_t len)
{
    // uint32_t t_now = system_get_time();
    // printf("t: %u, l %d\n", t_now, len);
    EspAudioPlayerStreamWrite(data, len);
}
			    
static void btav_set_device_class(void)
{
    tBTA_UTL_COD cod;
    memset(&cod, 0, sizeof(tBTA_UTL_COD));
    cod.major = BTM_COD_MAJOR_AUDIO;
    cod.minor = BTM_COD_MINOR_LOUDSPEAKER;
    cod.service = BTM_COD_SERVICE_CAPTURING | BTM_COD_SERVICE_AUDIO;
    utl_set_device_class(&cod, BTA_UTL_SET_COD_ALL);
    BT_APP_TRACE_ERROR("set class of device: major 0x%x, minor 0x%x, service 0x%x\n",
              cod.major, cod.minor, cod.service);
}

static void bt_app_handle_evt(UINT16 event, void *p_param)
{
    BT_APP_TRACE_EVENT("bt_app_handle_evt 0x%x\n", event);
    esp_a2d_cb_param_t *a2d = NULL;
    switch (event) {
    case BT_APP_EVT_STACK_ON: {
        char *dev_name = "ESP_SPEAKER";
        BTM_SetTraceLevel(BT_TRACE_LEVEL_WARNING);
        btav_set_device_class();
        BTA_DmSetDeviceName(dev_name);

        esp_a2d_register_callback(bt_app_a2d_cb);
        esp_a2d_register_data_callback(bt_app_a2d_data_cb);

        esp_a2d_sink_init();
        esp_bt_gap_set_scan_mode(BT_SCAN_MODE_CONNECTABLE_DISCOVERABLE);
        break;
    }
    case ESP_A2D_CONNECTION_STATE_EVT: {
        a2d = (esp_a2d_cb_param_t *)(p_param);
        BT_APP_TRACE_ERROR("===a2dp conn_state_cb %d ===\n", a2d->conn_stat.state);
        break;
    }
    case ESP_A2D_AUDIO_STATE_EVT: {
        a2d = (esp_a2d_cb_param_t *)(p_param);
        BT_APP_TRACE_ERROR("===a2dp audio_state_cb %d ===\n", a2d->audio_stat.state);
        break;
    }
    case ESP_A2D_AUDIO_CFG_EVT: {
        a2d = (esp_a2d_cb_param_t *)(p_param);
        BT_APP_TRACE_ERROR("===a2dp audio_cfg_cb type %d ===\n", a2d->audio_cfg.mcc.type);
        if (a2d->audio_cfg.mcc.type == ESP_A2D_MCT_SBC) {
            // temporarily hardcoded the PCM configuaration
            BT_APP_TRACE_ERROR("configure audio player\n");
            EspAudioPlayerStreamCfg(StreamSampleRate_44k, 2, StreamBitLen_16BIT);
            EspAudio_SetupStream("stream.pcm", InputSrcType_Stream);
            EspAudio_SetVolume(99);
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
    bt_status_t init, enable;
    init = BTIF_InitStack();
    if (init != BT_STATUS_SUCCESS) {
        return;
    }

    enable = BTIF_EnableStack();
    if (enable != BT_STATUS_SUCCESS) {
        return;
    }

    bt_app_transfer_context(bt_app_handle_evt, BT_APP_EVT_STACK_ON, NULL, 0, NULL);
}
