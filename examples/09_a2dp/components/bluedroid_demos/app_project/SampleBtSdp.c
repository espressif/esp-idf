#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>


#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "bt_app_common.h"
#include "btif_stack_manager.h"
#include "btif_sdp.h"
#include "esp_gap_api.h"

#include "bta_api.h"

#include "alarm.h"
typedef enum {
    BT_APP_EVT_STACK_ON,
    BT_APP_EVT_STACK_OFF,
    BT_APP_EVT_ADD_SDP_RECORD_TO,
    BT_APP_EVT_SDP_SEARCH_START_TO,
    BT_APP_EVT
} tBT_APP_EVT;

typedef union {
    uint32_t dummy;
} tBT_APP_EVT_DATA;

static void bt_stack_evt(tBT_APP_EVT event, tBT_APP_EVT_DATA *p_data);
static void bt_stack_state_changed(bt_state_t state);
static int bt_sdp_add_record(void);
static void bt_sdp_search_complete(bt_status_t status, bt_bdaddr_t *bd_addr, uint8_t* uuid, int num_records, bluetooth_sdp_record *records);

// static bt_bdaddr_t peer_bd_addr = {{0x00, 0x1b, 0xdc, 0x08, 0x0f, 0xe7}};
static bt_bdaddr_t peer_bd_addr = {{0xfc, 0x3f, 0x7c, 0xf1, 0x2c, 0x78}};

/* root browse
static const uint8_t target_uuid[16] = { 0x00, 0x00, 0x10, 0x02, 0x00, 0x00, 0x10, 0x00,
                                         0x80, 0x00, 0x00, 0x80, 0x5F, 0x9B, 0x34, 0xFB };
*/

/* UUID_MAP_MAS */
static const uint8_t  target_uuid[] = {0x00, 0x00, 0x11, 0x32, 0x00, 0x00, 0x10, 0x00,
                                        0x80, 0x00, 0x00, 0x80, 0x5F, 0x9B, 0x34, 0xFB};

/* UUID AUDIO Source */
/*
static const uint8_t  target_uuid[] = {0x00, 0x00, 0x11, 0x0A, 0x00, 0x00, 0x10, 0x00,
                                        0x80, 0x00, 0x00, 0x80, 0x5F, 0x9B, 0x34, 0xFB};
*/

static bt_callbacks_t bt_callbacks = {
    bt_stack_state_changed
};

static btsdp_callbacks_t btsdp_callbacks = {
    bt_sdp_search_complete
};

osi_alarm_t *app_alarm = NULL;

static void bt_sdp_add_record_to(void *context)
{
    (void)(context);
    bt_stack_evt(BT_APP_EVT_ADD_SDP_RECORD_TO, NULL);
}

static void bt_sdp_search_start_to(void *context)
{
    (void)(context);
    bt_stack_evt(BT_APP_EVT_SDP_SEARCH_START_TO, NULL);
}

static void bt_app_stack_evt(UINT16 event, char *p_param)
{
    switch (event) {
    case BT_APP_EVT_STACK_ON: {
	char *dev_name = "SDP_SERVER_CLIENT";
	BTM_SetTraceLevel(BT_TRACE_LEVEL_DEBUG);
	BTA_DmSetDeviceName(dev_name);
    
        esp_bt_gap_set_scan_mode(BT_SCAN_MODE_CONNECTABLE_DISCOVERABLE);
        BTIF_SdpInit(&btsdp_callbacks);

        app_alarm = osi_alarm_new("app_alarm", bt_sdp_add_record_to, NULL, 1000, false);
        osi_alarm_set(app_alarm, 1000);
    }
        break;
    case BT_APP_EVT_ADD_SDP_RECORD_TO: {
        bt_sdp_add_record();
        osi_alarm_free(app_alarm);
        app_alarm = NULL;
        app_alarm = osi_alarm_new("app_alarm", bt_sdp_search_start_to, NULL, 20000, false);
        osi_alarm_set(app_alarm, 20000);
    }
        break;
    case BT_APP_EVT_SDP_SEARCH_START_TO: {
        osi_alarm_free(app_alarm);
        app_alarm = NULL;
        BTIF_SdpSearch(&peer_bd_addr, target_uuid);
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

static int bt_sdp_add_record(void)
{
    int handle;
    bluetooth_sdp_sap_record sap_svr;
    memset (&sap_svr, 0, sizeof(bluetooth_sdp_sap_record));
    
    sap_svr.hdr.type = SDP_TYPE_SAP_SERVER;
    sap_svr.hdr.rfcomm_channel_number = 2;
    sap_svr.hdr.service_name = "SIM ACCESS";
    sap_svr.hdr.service_name_length = 10;
    sap_svr.hdr.profile_version = 0x0100;

    BTIF_SdpCreateRecord((bluetooth_sdp_record *)(&sap_svr), &handle);
    return handle;
}

static void bt_sdp_search_complete(bt_status_t status, bt_bdaddr_t *bd_addr, uint8_t* uuid, int num_records, bluetooth_sdp_record *records)
{
    uint8_t *addr = bd_addr->address;
    bluetooth_sdp_hdr_overlay *p = &records->mas.hdr;
    LOG_ERROR("sdp search cmpl: st %d, bd_addr: %02x:%02x:%02x:%02x:%02x:%02x, records %d\n",
              status, addr[0], addr[1], addr[2], addr[3], addr[4], addr[5], num_records);
    if (p->service_name_length > 0) {
        LOG_ERROR("service name: %s\n", p->service_name);
    }
    LOG_ERROR("rfc_chl_num %d, l2cap_psm %d, version %02x\n",
	      p->rfcomm_channel_number, p->l2cap_psm, p->profile_version);
#if 0
    uint8_t *addr = bd_addr->address;
    bluetooth_sdp_hdr_overlay *p = &records->hdr;
    LOG_ERROR("sdp search cmpl: st %d, bd_addr: %02x:%02x:%02x:%02x:%02x:%02x, records %d, len:%d\n",
              status, addr[0], addr[1], addr[2], addr[3], addr[4], addr[5], num_records, p->user1_ptr_len);
    if (p->service_name_length > 0) {
        LOG_ERROR("service name: %s\n", p->service_name);
    }
#endif
}

void app_main_entry(void)
{
    bt_status_t stat;
    stat = BTIF_InitStack(&bt_callbacks);
    if (stat == BT_STATUS_SUCCESS) {
        BTIF_EnableStack();
    }
}

