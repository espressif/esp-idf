// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

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

#include "prf_defs.h"

#include "bta_api.h"
#include "bta_gatt_api.h"
#include "controller.h"

#include "gatt_int.h"
#include "bt_trace.h"
#include "btm_api.h"
#include "bt_types.h"
#include "dis_api.h"
#include "bt_app_common.h"

//#include "prf_defs.h"

#include "wx_airsync_prf.h"

#include "button_pro.h"
#include "app_button.h"
#include "hid_le_prf.h"

#include "esp_gatt_api.h"

//

#include "hcimsgs.h"
#include "esp_bt_defs.h"


#define BT_BD_ADDR_STR         "%02x:%02x:%02x:%02x:%02x:%02x"
#define BT_BD_ADDR_HEX(addr)   addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]
tBTA_GATTS_IF server_if;

static unsigned char DIS_UUID[16] = {
    0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80,
    0x00, 0x10, 0x00, 0x00, 0x0a, 0x18, 0x00, 0x00
    };
static unsigned char BASE_UUID[16] = {
    0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80,
    0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    };

UINT16 ijiazu_uuid = 0xffff;
UINT16 wechat_uuid = 0xfee7;
tBTA_BLE_SERVICE ijiazu_service = {
							0x01,		//only one service in the ijiazu button profile
							false,
							&ijiazu_uuid
							};        /* 16 bits services */

tBTA_BLE_SERVICE wechat_service = {
							0x01,		//only one service in the ijiazu button profile
							false,
							&wechat_uuid
							};        /* 16 bits services */



UINT8 beacon_manu[25] = {0x4c, 0x00,0x02, 0x15, 0xfd, 0xa5, 0x06, 0x93, 0xa4, 0xe2, 
                  0x4f, 0xb1, 0xaf, 0xcf, 0xc6, 0xeb, 0x07, 0x64, 0x78, 0x25,
                  0x27, 0x32, 0xe6, 0x08, 0xc5};

//UINT8 ijiazu_manu[17] = {0xff,0x20,0x14,0x07,0x22,0x00,0x02,0x5B,0x00,0x33,0x49,0x31,0x30,0x4a,0x30,0x30,0x31};
UINT8 ijiazu_manu[17] = {0xff,0x20,0x14,0x07,0x22,0x00,0x02,0x5B,0x00,0x33,0x49,0x31,0x30,0x4a,0x30,0x30,0x31};
UINT8 wechat_manu[] = {0x00,0x00,0x18,0xfe,0x34,0x6a,0x86,0x2e};
tBTA_BLE_MANU	p_ijiazu_manu = {sizeof(ijiazu_manu),ijiazu_manu};			/* manufacturer data */
tBTA_BLE_MANU   p_wechat_manu = {sizeof(wechat_manu),wechat_manu};


tBLE_BD_ADDR			p_peer_bda = {
	.type	= API_PUBLIC_ADDR,
	.bda	= {0}
};

esp_ble_adv_params_all_t adv_params = 
{
	.adv_int_min 		= BTM_BLE_ADV_INT_MIN + 0x100,
	.adv_int_max 		= BTM_BLE_ADV_INT_MIN + 0x100,
	.adv_type	 		= API_NON_DISCOVERABLE,
	.addr_type_own 		= API_PUBLIC_ADDR,
	.channel_map		= ESP_BLE_ADV_CHNL_MAP,
	.adv_filter_policy 	= ADV_ALLOW_SCAN_ANY_CON_ANY,
	.p_dir_bda			= &p_peer_bda
};





BD_ADDR rand_ijiazu_addr = {0x00,0x02,0x5B,0x00,0x32,0x55};

esp_ble_adv_data_cfg_t ijiazu_adv_data[ADV_SCAN_IDX_MAX] = 
{
	[BLE_ADV_DATA_IDX] 		= {
										.adv_name = "Espressif_007",
										{
										{0,0},
										NULL,			//no manufature data to be setting in the ijiazu adervetisiing datas
										&ijiazu_service,
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
										&p_ijiazu_manu,
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

esp_ble_adv_data_cfg_t wechat_adv_data[ADV_SCAN_IDX_MAX] = 
{
	[BLE_ADV_DATA_IDX] 		= {
										.adv_name = NULL,
										{
										{0,0},
										NULL,			//no manufature data to be setting in the ijiazu adervetisiing datas
										&wechat_service,
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
										.adv_name = "wechat_demo",	
										{
										{0,0},
										&p_wechat_manu,
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

#if	(BUT_PROFILE_CFG)
static void SimpleDataCallBack(uint8_t app_id, uint8_t event, uint16_t len, uint8_t *p_data);
#endif
                  
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
    /*dis init*/
/*    tDIS_ATTR_MASK dis_attr_mask;
    dis_attr_mask = DIS_ATTR_SYS_ID_BIT | DIS_ATTR_MODEL_NUM_BIT | DIS_ATTR_SERIAL_NUM_BIT | 
        DIS_ATTR_FW_NUM_BIT | DIS_ATTR_HW_NUM_BIT | DIS_ATTR_SW_NUM_BIT | DIS_ATTR_MANU_NAME_BIT |
        DIS_ATTR_IEEE_DATA_BIT | DIS_ATTR_PNP_ID_BIT;
    DIS_SrInit(dis_attr_mask);
*/
	//ble_but_create_svc();
    /*instantiate a battery service*/
    bas_register();  
	/*instantiate the driver for button profile*/
	//app_button_init();
#if (BUT_PROFILE_CFG)
	/*instantiate a button service*/
	//button_init(SimpleDataCallBack);
#endif	///BUT_PROFILE_CFG

#if (HIDD_LE_PROFILE_CFG)
	/*instantiate a hid device service*/
	hidd_le_init();
#endif	///HIDD_LE_PROFILE_CFG

#if (WX_AIRSYNC_CFG)
	AirSync_Init(NULL);
#endif	///WX_AIRSYNC_CFG
	esp_ble_start_advertising(&adv_params);
	//API_Ble_AppStartAdvertising(&adv_params);
    /*start advetising*/
//    BTA_GATTS_Listen(server_if, true, NULL);
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
			esp_ble_config_adv_data(&wechat_adv_data[BLE_ADV_DATA_IDX],
								bta_gatts_set_adv_data_cback);
           	
        }
        break;
        /*connect callback*/
        case BTA_GATTS_CONNECT_EVT:
        {
			///Stop the advertising when the connection is establish
			esp_ble_stop_advertising();
            LOG_ERROR("\ndevice is connected "BT_BD_ADDR_STR", server_if=%d,reason=0x%x,connect_id=%d\n", 
                             BT_BD_ADDR_HEX(p_data->conn.remote_bda), p_data->conn.server_if,
                             p_data->conn.reason, p_data->conn.conn_id);
            /*return whether the remote device is currently connected*/
            int is_connected = BTA_DmGetConnectionState(p_data->conn.remote_bda);
            LOG_ERROR("is_connected=%d\n",is_connected);
        }
        break;

		case BTA_GATTS_DISCONNECT_EVT:
			///start the advertising again when lose the connection
			esp_ble_start_advertising(&adv_params);
        	break;
        default:
        LOG_ERROR("unsettled event: %d\n", event);
    }

}

#if	(BUT_PROFILE_CFG)
static void SimpleDataCallBack(uint8_t app_id, uint8_t event, uint16_t len, uint8_t *p_data)
{
	LOG_ERROR("the event value is:%x\n",event);
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
}
#endif	///BUT_PROFILE_CFG


static void ble_server_appRegister(void)
{    
    bt_uuid_t uuid;
    tBT_UUID t_uuid;
    memcpy(&uuid, BASE_UUID, sizeof(bt_uuid_t));
    //memcpy(&uuid, DIS_UUID, sizeof(bt_uuid_t));
    btif_to_bta_uuid(&t_uuid, &uuid);

    LOG_ERROR("register gatts application\n");
    esp_ble_gatts_app_register(&t_uuid, bta_gatts_callback);

	bt_prf_sys_init();
	ble_but_prf_enable();
}

void gatts_server_test(void)
{
    BTM_SetTraceLevel(BT_TRACE_LEVEL_DEBUG);

    ble_server_appRegister();
}
