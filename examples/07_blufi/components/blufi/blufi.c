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

static void SimpleDataCallBack(UINT8 app_id, UINT8 event, UINT8 len, UINT8 *p_data);

struct dm_evt {
	tBTA_DM_SEC_EVT event;
	tBTA_DM_SEC* p_data;
};

typedef struct {
    uint8_t uu[16];
} bt_uuid_t;


#define BT_BD_ADDR_STR         "%02x:%02x:%02x:%02x:%02x:%02x"
#define BT_BD_ADDR_HEX(addr)   addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]
tBTA_GATTS_IF server_if;

static unsigned char BASE_UUID[16] = {
    0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80,
    0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    };

UINT16 esp32_uuid = 0xffff;
tBTA_BLE_SERVICE esp32_service = {
	0x01,		//only one service in the esp32 button profile
	false,
	&esp32_uuid
	};        /* 16 bits services */


UINT8 beacon_manu[25] = {
	0x4c, 0x00,0x02, 0x15, 0xfd, 0xa5, 0x06, 0x93, 0xa4, 0xe2, 
    0x4f, 0xb1, 0xaf, 0xcf, 0xc6, 0xeb, 0x07, 0x64, 0x78, 0x25,
    0x27, 0x32, 0xe6, 0x08, 0xc5
    };

UINT8 esp32_manu[17] = {0xff,0x20,0x14,0x07,0x22,0x00,0x02,0x5B,0x00,0x33,0x49,0x31,0x30,0x4a,0x30,0x30,0x31};
tBTA_BLE_MANU	p_esp32_manu = {sizeof(esp32_manu),esp32_manu};			/* manufacturer data */


BD_ADDR rand_esp32_addr = {0x00,0x02,0x5B,0x00,0x32,0x55};

tESP_BLE_ADV_DATA esp32_adv_data[ADV_SCAN_IDX_MAX] = 
{
	[BLE_ADV_DATA_IDX] = {
		.adv_name = "Espressif_008",
		{
		{0,0},
		NULL,			//no manufature data to be setting in the esp32 adervetisiing datas
		&esp32_service,
		NULL,					//the  128 bits service uuid set to null(not used)
		NULL,					//the 32 bits Service UUID set to null(not used)
		NULL,					//16 bits services Solicitation UUIDs set to null(not used)
		NULL,					//List of 32 bit Service Solicitation UUIDs set to null(not used)
		NULL,					//List of 128 bit Service Solicitation UUIDs set to null(not used)
		NULL,					//proprietary data set to null(not used)
		NULL,					//service data set not null(no service data to be sent)
		0x0200,         		//device type : generic display
		BTA_DM_GENERAL_DISC,	// General discoverable. 
		0xFE					//the tx power value,defult value is 0
		},
	},

	[BLE_SCAN_RSP_DATA_IDX] = {
		.adv_name = NULL,	
		{
		{0,0},
		&p_esp32_manu,
		NULL,
		NULL,					//the  128 bits service uuid set to null(not used)
		NULL,					//the 32 bits Service UUID set to null(not used)
		NULL,					//16 bits services Solicitation UUIDs set to null(not used)
		NULL,					//List of 32 bit Service Solicitation UUIDs set to null(not used)
		NULL,					//List of 128 bit Service Solicitation UUIDs set to null(not used)
		NULL,					//proprietary data set to null(not used)
		NULL,					//service data set not null(no service data to be sent)
		0x0000,         		//device type : generic display
		0x00,					// General discoverable. 
		0x00},					//the tx power value,defult value is 0
	}
};

int uuidType(unsigned char* p_uuid)
{
    int i = 0;
    int match = 0;
    int all_zero = 1;
                
    for(i = 0; i != 16; ++i)
    {
        if (i == 12 || i == 13)
            continue;
                              
        if (p_uuid[i] == BASE_UUID[i])
            ++match;
                                        
        if (p_uuid[i] != 0)
            all_zero = 0;
    }
    if (all_zero)
        return 0;
    if (match == 12)
        return LEN_UUID_32;
    if (match == 14)
        return LEN_UUID_16;
    return LEN_UUID_128;
}

/*16-bits uuid to the structure of holding any type of UUID*/
void btif_to_bta_uuid(tBT_UUID *p_dest, bt_uuid_t *p_src)
{
    char *p_byte = (char*)p_src;
   
    int i = 0;
            
    p_dest->len = uuidType(p_src->uu);
                  
    switch (p_dest->len)
    {
        case LEN_UUID_16:
            p_dest->uu.uuid16 = (p_src->uu[13] << 8) + p_src->uu[12];
            break;
                                    
        case LEN_UUID_32:
            p_dest->uu.uuid32  = (p_src->uu[13] <<  8) + p_src->uu[12];
            p_dest->uu.uuid32 += (p_src->uu[15] << 24) + (p_src->uu[14] << 16);
            break;
                                                   
        case LEN_UUID_128:
            for(i = 0; i != 16; ++i)
                p_dest->uu.uuid128[i] = p_byte[i];
                break;
                                                            
        default:         
        LOG_ERROR("%s: Unknown UUID length %d!", __FUNCTION__, p_dest->len);
        break;
    }
}
/*set advertising config callback*/
static void bta_gatts_set_adv_data_cback(tBTA_STATUS call_status)
{
    LOG_ERROR("set advertising config:status=%d\n", call_status);
	blufi_profile_init(SimpleDataCallBack);
}

/*register callback*/
void bta_gatts_callback(tBTA_GATTS_EVT event, tBTA_GATTS* p_data)
{
    switch (event)
    {   
        case BTA_GATTS_REG_EVT:
        {  
            tBTA_GATT_STATUS  status = p_data->reg_oper.status;
            server_if = p_data->reg_oper.server_if;
            LOG_ERROR("register complete: event=%d, status=%d, server_if=%d\n", 
                event, status, server_if);
            
            LOG_ERROR("set advertising parameters\n");
			//set the advertising data to the btm layer
			ESP_AppBleConfigadvData(&esp32_adv_data[BLE_ADV_DATA_IDX],
								bta_gatts_set_adv_data_cback);
			//set the adversting data to the btm layer
			ESP_AppBleSetScanRsp(&esp32_adv_data[BLE_SCAN_RSP_DATA_IDX],NULL);
    	    BTA_GATTS_Listen(server_if, true, NULL);
        }
        break;
        /*connect callback*/
        case BTA_GATTS_CONNECT_EVT:
        {
            LOG_ERROR("\ndevice is connected "BT_BD_ADDR_STR", server_if=%d,reason=0x%x,connect_id=%d\n", 
                             BT_BD_ADDR_HEX(p_data->conn.remote_bda), p_data->conn.server_if,
                             p_data->conn.reason, p_data->conn.conn_id);
            /*return whether the remote device is currently connected*/
            int is_connected = BTA_DmGetConnectionState(p_data->conn.remote_bda);
            LOG_ERROR("is_connected=%d\n",is_connected);
        }
        break;
        
        default:
        LOG_ERROR("unsettled event: %d\n", event);
    }

}

#define HEADER_SSID	"ssid"
#define HEADER_PASSWD	"passwd"
#define HEADER_CONFIRM	"confirm"
extern void wifi_set_blue_config(char *ssid, char *passwd);
static char tmp_ssid[33];
static char tmp_passwd[33];

static void SimpleDataCallBack(UINT8 app_id, UINT8 event, UINT8 len, UINT8 *p_data)
{
	char *p = NULL;
	LOG_ERROR("the data is:%s\n", p_data);
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
		ets_printf("SSID: %s\n", p+strlen(HEADER_SSID)+1);
		strcpy(tmp_ssid, p+strlen(HEADER_SSID)+1);
	}
	p = strstr(p_data, HEADER_PASSWD);
	if (p) {
		ets_printf("PASSWORD: %s\n", p+strlen(HEADER_PASSWD)+1);
		strcpy(tmp_passwd, p+strlen(HEADER_PASSWD)+1);
	}
	p = strstr(p_data, HEADER_CONFIRM);
	if (p) {
		ets_printf("CONFIRM\n");
		wifi_set_blue_config(tmp_ssid, tmp_passwd);
	}
	
}

static void blufi_ble_server_appRegister(void)
{    
    bt_uuid_t uuid;
    tBT_UUID t_uuid;
    memcpy(&uuid, BASE_UUID, sizeof(bt_uuid_t));
    btif_to_bta_uuid(&t_uuid, &uuid);

    LOG_ERROR("register gatts application\n");
    BTA_GATTS_AppRegister(&t_uuid, bta_gatts_callback);
}

static BtStatus_t blufi_dm_upstreams_evt(void *arg)
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
	            LOG_ERROR("BDA is: %s\n", bdstr);
	        } while (0);
#endif
	    	blufi_ble_server_appRegister();
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

    LOG_ERROR("%s: %d\n", __func__, (uint16_t)event);

	evt = (struct dm_evt *)GKI_getbuf(sizeof(struct dm_evt));
	if (evt == NULL)
		return;

	evt->event = event;
	evt->p_data = p_data;

    blufi_transfer_context(blufi_dm_upstreams_evt, evt);
}

BtStatus_t blufi_enable(void *arg)
{
    BTM_SetTraceLevel(BT_TRACE_LEVEL_DEBUG);

    BTA_EnableBluetooth(blufi_bte_dm_evt);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
}
