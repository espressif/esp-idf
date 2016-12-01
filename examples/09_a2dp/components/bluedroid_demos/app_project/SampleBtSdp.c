#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "bt_app_common.h"
#include "btif_stack_manager.h"
#include "esp_gap_bt_api.h"
#include "bta_api.h"
// #include "bt_av.h"
#include "esp_a2dp_api.h"

/* utl_set_device_class() */
#include "utl.h"

#include "alarm.h"
typedef enum {
    BT_APP_EVT_STACK_ON,
    BT_APP_EVT_STACK_OFF,
    BT_APP_EVT_AV_OPEN_TO,
    BT_APP_EVT
} tBT_APP_EVT;

typedef union {
    uint32_t dummy;
} tBT_APP_EVT_DATA;

// extern const btav_interface_t *btif_av_get_sink_interface(void);
static void bt_stack_evt(tBT_APP_EVT event, tBT_APP_EVT_DATA *p_data);
static void bt_stack_state_changed(bt_state_t state);

// static bt_bdaddr_t peer_bd_addr = {{0x00, 0x1b, 0xdc, 0x08, 0x0f, 0xe7}};

static bt_callbacks_t bt_callbacks = {
    bt_stack_state_changed
};

osi_alarm_t *app_alarm = NULL;

static void esp_a2d_cb(uint32_t event, void *param)
{
    esp_a2d_cb_param_t *p = (esp_a2d_cb_param_t *)param;
    switch (event) {
    case ESP_A2D_CONNECTION_STATE_EVT:
	LOG_ERROR("===a2dp conn_state_cb %d ===\n", p->conn_stat.state);
	break;
    case ESP_A2D_AUDIO_STATE_EVT:
	LOG_ERROR("===a2dp audio_state_cb %d ===\n", p->audio_stat.state);
	break;
    case ESP_A2D_AUDIO_CFG_EVT:
        LOG_ERROR("===a2dp audio_cfg_cb type %d ===\n", p->audio_cfg.mcc.type);
        break;
    default:
        LOG_ERROR("===a2dp invalid cb event: %d\n", event);
    }
}

static void btav_open_to(void *context)
{
    (void)(context);
    bt_stack_evt(BT_APP_EVT_AV_OPEN_TO, NULL);
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

static void bt_app_stack_evt(UINT16 event, char *p_param)
{
    switch (event) {
    case BT_APP_EVT_STACK_ON: {
        char *dev_name = "SDP_SERVER_CLIENT";
        BTM_SetTraceLevel(BT_TRACE_LEVEL_WARNING);
        btav_set_device_class();
        BTA_DmSetDeviceName(dev_name);
        esp_bt_gap_set_scan_mode(BT_SCAN_MODE_CONNECTABLE_DISCOVERABLE);
        esp_a2d_sink_init(esp_a2d_cb);

        // app_alarm = osi_alarm_new("app_alarm", bt_sdp_add_record_to, NULL, 1000, false);
        app_alarm = osi_alarm_new("app_alarm", btav_open_to, NULL, 1000, false);
        osi_alarm_set(app_alarm, 1000);
    }
    break;
    case BT_APP_EVT_AV_OPEN_TO: {
        LOG_ERROR("**BT_APP_EVT_AV_OPEN_TO\n");
        // btif_av_get_sink_interface()->connect(&peer_bd_addr);
        osi_alarm_free(app_alarm);
        app_alarm = NULL;
    }
    break;
    default:
        break;
    }
}

static void bt_stack_evt(tBT_APP_EVT event, tBT_APP_EVT_DATA *p_data)
{
    LOG_ERROR("bt_stack_evt: %d\n", (uint16_t)event);
    bt_app_transfer_context(bt_app_stack_evt, (uint16_t)event,
                            (void *)p_data, sizeof(tBT_APP_EVT_DATA), NULL);
}

static void bt_stack_state_changed(bt_state_t state)
{
    if (state == BT_STATE_ON) {
        bt_stack_evt(BT_APP_EVT_STACK_ON, NULL);
    }
}

void app_main_entry(void)
{
    bt_status_t stat;
    stat = BTIF_InitStack(&bt_callbacks);
    if (stat == BT_STATUS_SUCCESS) {
        BTIF_EnableStack();
    }
}

