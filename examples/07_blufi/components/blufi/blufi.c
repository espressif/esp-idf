/***************************************************************
*
* This file is for gatt server device. It instantiates BATTERY
* sevice. It can be scanned and connected by central device,
* and the client will get the BAS value. It calls the API bta 
* layer provides.
*
****************************************************************/

#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>

#include "bta_api.h"
#include "bta_gatt_api.h"
#include "controller.h"

#include "gatt_int.h"
#include "bt_trace.h"
#include "btm_api.h"
#include "bt_types.h"
#include "dis_api.h"

#include "blufi_prf.h"
#include "hid_le_prf.h"
#include "prf_defs.h"
#include "hcimsgs.h"

#include "blufi.h"
#include "blufi_adv.h"

static void BlufiDataCallBack(UINT8 app_id, UINT8 event, UINT8 len, UINT8 *p_data);

struct dm_evt {
	tBTA_DM_SEC_EVT event;
	tBTA_DM_SEC* p_data;
};

#define HEADER_SSID	"ssid"
#define HEADER_PASSWD	"passwd"
#define HEADER_CONFIRM	"confirm"
extern void wifi_set_blue_config(char *ssid, char *passwd);
static char tmp_ssid[33];
static char tmp_passwd[33];

static void BlufiDataCallBack(UINT8 app_id, UINT8 event, UINT8 len, UINT8 *p_data)
{
	char *p = NULL;
	LOG_DEBUG("the data is:%s\n", p_data);
#if	0
	switch(event)
	{
		case RECEIVE_NET_PASSWD_EVT:
		LOG_ERROR("Received the network passwork");
		break;
		case RECEIVE_NET_SSD_EVT:
		 LOG_ERROR("Received the network SSID");
		break;
		default:
		break;
	}
#endif
	p = strstr(p_data, HEADER_SSID);
	if (p) {
		LOG_ERROR("SSID: %s\n", p+strlen(HEADER_SSID)+1);
		strcpy(tmp_ssid, p+strlen(HEADER_SSID)+1);
	}
	p = strstr(p_data, HEADER_PASSWD);
	if (p) {
		LOG_ERROR("PASSWORD: %s\n", p+strlen(HEADER_PASSWD)+1);
		strcpy(tmp_passwd, p+strlen(HEADER_PASSWD)+1);
	}
	p = strstr(p_data, HEADER_CONFIRM);
	if (p) {
		LOG_ERROR("CONFIRM\n");
		wifi_set_blue_config(tmp_ssid, tmp_passwd);
	}
	
}

static bt_status_t blufi_dm_upstreams_evt(void *arg)
{
	struct dm_evt *evt = (struct dm_evt *)arg;
	
    tBTA_DM_SEC *p_data = (tBTA_DM_SEC*)evt->p_data;
    switch (evt->event) {
	    case BTA_DM_ENABLE_EVT: {
	        /*set connectable,discoverable, pairable and paired only modes of local device*/
	        tBTA_DM_DISC disc_mode = BTA_DM_BLE_GENERAL_DISCOVERABLE;
	        tBTA_DM_CONN conn_mode = BTA_DM_BLE_CONNECTABLE;
	        BTA_DmSetVisibility(disc_mode, conn_mode, (uint8_t)BTA_DM_NON_PAIRABLE, (uint8_t)BTA_DM_CONN_ALL );
	
#if (defined(BLE_INCLUDED) && (BLE_INCLUDED == TRUE))
	        /* Enable local privacy */
	        //BTA_DmBleConfigLocalPrivacy(BLE_LOCAL_PRIVACY_ENABLED);
	        do {
	            const controller_t *controller = controller_get_interface();
	            char bdstr[18];
	            bdaddr_to_string(controller->get_address(), bdstr, sizeof(bdstr));
	            LOG_DEBUG("BDA is: %s\n", bdstr);
	        } while (0);
#endif
			blufi_profile_init(BlufiDataCallBack);
			break;
		default:
			break;
	    }
	}

    GKI_freebuf(evt);

	return BT_STATUS_SUCCESS;
}


void blufi_bte_dm_evt(tBTA_DM_SEC_EVT event, tBTA_DM_SEC* p_data)
{
	struct dm_evt *evt;

    LOG_DEBUG("%s: %d\n", __func__, (uint16_t)event);

	evt = (struct dm_evt *)GKI_getbuf(sizeof(struct dm_evt));
	if (evt == NULL)
		return;

	evt->event = event;
	evt->p_data = p_data;

    blufi_transfer_context(blufi_dm_upstreams_evt, evt);
}

bt_status_t blufi_enable(void *arg)
{
    BTM_SetTraceLevel(BT_TRACE_LEVEL_ERROR);

    BTA_EnableBluetooth(blufi_bte_dm_evt);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
}
