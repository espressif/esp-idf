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
} tBT_APP_EVT;

typedef union {
    esp_a2d_cb_param_t a2d;
} tBT_APP_EVT_DATA;

static void bt_app_evt(tBT_APP_EVT event, tBT_APP_EVT_DATA *p_data);

static void bt_app_a2d_cb(uint32_t event, void *param)
{
    switch (event) {
    case ESP_A2D_CONNECTION_STATE_EVT:
    case ESP_A2D_AUDIO_STATE_EVT:
    case ESP_A2D_AUDIO_CFG_EVT:
    {
        bt_app_evt(event, (tBT_APP_EVT_DATA *)param);
        break;
    }
    default:
        LOG_ERROR("===a2dp invalid cb event: %d\n", event);
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
    LOG_ERROR("set class of device: major 0x%x, minor 0x%x, service 0x%x\n",
              cod.major, cod.minor, cod.service);
}

static void bt_app_handle_evt(UINT16 event, char *p_param)
{
    esp_a2d_cb_param_t *a2d = NULL;
    switch (event) {
    case BT_APP_EVT_STACK_ON: {
        char *dev_name = "SDP_SERVER_CLIENT";
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
        LOG_ERROR("===a2dp conn_state_cb %d ===\n", a2d->conn_stat.state);
        break;
    }
    case ESP_A2D_AUDIO_STATE_EVT: {
        a2d = (esp_a2d_cb_param_t *)(p_param);
        LOG_ERROR("===a2dp audio_state_cb %d ===\n", a2d->audio_stat.state);
        break;
    }
    case ESP_A2D_AUDIO_CFG_EVT: {
        a2d = (esp_a2d_cb_param_t *)(p_param);
        LOG_ERROR("===a2dp audio_cfg_cb type %d ===\n", a2d->audio_cfg.mcc.type);
        if (a2d->audio_cfg.mcc.type == ESP_A2D_MCT_SBC) {
            // temporarily hardcoded the PCM configuaration
            EspAudioPlayerStreamCfg(StreamSampleRate_44k, 2, StreamBitLen_16BIT);
            EspAudio_SetupStream("stream.pcm", InputSrcType_Stream);
            EspAudio_SetVolume(99);
        }
        break;
    }
    default:
        LOG_ERROR("===application invalid event: %d\n", event);
        break;
    }
}

static void bt_app_evt(tBT_APP_EVT event, tBT_APP_EVT_DATA *p_data)
{
    LOG_ERROR("bt_app_evt: %d\n", (uint16_t)event);
    bt_app_transfer_context(bt_app_handle_evt, (uint16_t)event,
                            (void *)p_data, sizeof(tBT_APP_EVT_DATA), NULL);
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
    
    bt_app_evt(BT_APP_EVT_STACK_ON, NULL);
}

