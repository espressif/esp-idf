#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>


#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "bt_app_common.h"
#include "bt_stack_manager.h"
#include "bt_sdp.h"
#include "bt_gap.h"

/* bta_api.h should not be included as BTA APIs should not be used by APP */
#include "bta_api.h"

typedef enum {
    BT_APP_EVT_STACK_ON,
    BT_APP_EVT_STACK_OFF,
    BT_APP_EVT
} tBT_APP_EVT;

typedef union {
    uint32_t dummy;
} tBT_APP_EVT_DATA;

static void bt_stack_state_changed(bt_state_t state);
static int bt_sdp_add_record(void);

static bt_callbacks_t bt_callbacks = {
    bt_stack_state_changed
};

static btsdp_callbacks_t btsdp_callbacks = {
    NULL
};
    
static void bt_app_stack_evt(UINT16 event, char *p_param)
{
    // tBT_APP_EVT_DATA *p_data = (tBT_APP_EVT_DATA *)p_param;
    switch (event) {
    case BT_APP_EVT_STACK_ON: {
	// todo: BTM & BTA APIs should not be called in application task
	char *dev_name = "flash";
	BTM_SetTraceLevel(BT_TRACE_LEVEL_DEBUG);
	BTA_DmSetDeviceName(dev_name);
    
	API_BT_GapSetScanMode(BT_SCAN_MODE_CONNECTABLE_DISCOVERABLE);

        API_BT_SdpInit(&btsdp_callbacks);
	// todo: SdpInit should have callback function indicating the end of execution
        // to avoid using vTaskDelay.
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        int handle = bt_sdp_add_record();

        vTaskDelay(20000/portTICK_PERIOD_MS);
        API_BT_SdpRemoveRecord(handle);

        vTaskDelay(20000/portTICK_PERIOD_MS);
        bt_sdp_add_record();
        
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
    sap_svr.hdr.service_name = "SAP_SVR";
    sap_svr.hdr.service_name_length = 7;
    sap_svr.hdr.profile_version = 1;

    API_BT_SdpCreateRecord((bluetooth_sdp_record *)(&sap_svr), &handle);
    return handle;
}

void app_main_entry(void)
{
    bt_status_t stat;
    stat = API_BTDM_InitStack(&bt_callbacks);
    if (stat == BT_STATUS_SUCCESS) {
        API_BTDM_EnableStack();
    }
}
