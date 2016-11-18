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



/****************************************************************************
*
* This file is for gatt client. It can scan ble device, connect one device, 
*
****************************************************************************/

#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include "controller.h"
               
#include "bt_trace.h"
#include "bt_types.h"
#include "btm_api.h"
#include "bta_api.h"
#include "bta_gatt_api.h"
#include "esp_gap_ble_api.h"
#include "esp_gattc_api.h"
#include "esp_bt_main.h"


#define BT_BD_ADDR_STR         "%02x:%02x:%02x:%02x:%02x:%02x"
#define BT_BD_ADDR_HEX(addr)   addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]

tBTA_GATTC_IF client_if;
BD_ADDR obj_addr;
static unsigned char BASE_UUID[16] = {
    0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80,
    0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    };

static esp_ble_scan_params_t ble_scan_params =
{
	.scan_type 			= BLE_SCAN_TYPE_ACTIVE,
	.own_addr_type		= ESP_PUBLIC_ADDR,
	.scan_filter_policy		= BLE_SCAN_FILTER_ALLOW_ALL,
	.scan_interval			= 0x50,
	.scan_window			= 0x30
};


static void esp_scan_result_cb(uint32_t event, void *scan_param);

static void esp_gattc_result_cb(uint32_t event, void *gattc_param);

                  
int arch_uuidType(unsigned char* p_uuid)
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

static void btif_to_bta_uuid(tBT_UUID *p_dest, bt_uuid_t *p_src)
{
    char *p_byte = (char*)p_src;
   
    int i = 0;
            
    p_dest->len = arch_uuidType(p_src->uu);
                  
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

static void esp_scan_result_cb(uint32_t event, void *param)
{
	uint8_t *adv_name = NULL;
	uint8_t adv_name_len = 0;
	switch(event)
	{
		case ESP_GAP_BLE_SCAN_PARAM_SET_COMPLETE_EVT:
		{
			//the unit of the duration is second
			uint32_t duration = 10;
			esp_ble_gap_start_scanning(duration);
			break;
		}
		case ESP_GAP_BLE_SCAN_RESULT_EVT:
		{
			esp_ble_gap_cb_param_t *scan_result = (esp_ble_gap_cb_param_t *)param;
			switch(scan_result->scan_rst.search_evt)
			{
 				case ESP_GAP_SEARCH_INQ_RES_EVT:
					for (int i = 0; i < 6; i++)
					{
						LOG_ERROR("%x:", scan_result->scan_rst.bda[i]);
					}
					LOG_ERROR("\n");
					adv_name = esp_ble_resolve_adv_data(scan_result->scan_rst.ble_adv, 
											ESP_BLE_AD_TYPE_NAME_CMPL, &adv_name_len);
					LOG_ERROR("adv_name_len=%x\n",adv_name_len);
					for(int j = 0; j < adv_name_len; j++)
					{
						LOG_ERROR("%c",adv_name[j]);
					}
					LOG_ERROR("\n");
					
					//if(strcmp(scan_result->scan_rst.bda, ))
					break;
				case ESP_GAP_SEARCH_INQ_CMPL_EVT:
					break;
				default:
					break;
			//		LOG_ERROR("ESP_GAP_SEARCH_DISC_BLE_RES_EVT\n");
			//	break;
			}
			break;
		}
		//case :
		// 	break;
		default:
			break;
	}
}


static void esp_gattc_result_cb(uint32_t event, void *gattc_param)
{
	
}



/*
uint16_t get_uuid16(tBT_UUID* p_uuid)
{
    if(p_uuid->len == LEN_UUID_16)
    {
        return p_uuid->uu.uuid16;
    }
    else if(p_uuid->len == LEN_UUID_128)
    {
        UINT16 u16;
        UINT8 *p = &p_uuid->uu.uuid128[LEN_UUID_128 - 4];
        STREAM_TO_UINT16(u16, p);
        return u16;
    }
    else
    {
        return (UINT16)p_uuid->uu.uuid32;
    }
}

//fill a GATT ID structure
void bta_le_fill_16bits_gatt_id(UINT8 inst_id, UINT16 uuid, tBTA_GATT_ID* p_output)
{
    p_output->inst_id = inst_id;
    p_output->uuid.len = LEN_UUID_16;
    p_output->uuid.uu.uuid16 = uuid;
}

//fill a service ID structure with a 16 bits service UUID
void bta_le_fill_16bits_srvc_id(bool is_pri, UINT8 inst_id, UINT16 srvc_uuid, tBTA_GATT_SRVC_ID* p_output)
{
    memset((void *)p_output, 0, sizeof(tBTA_GATT_SRVC_ID));
    p_output->is_primary = is_pri;
    bta_le_fill_16bits_gatt_id(inst_id, srvc_uuid, &p_output->id);
}

//fill a char ID structure with a 16 bits char UUID
void bta_le_fill_16bits_char_id(UINT8 inst_id, UINT16 char_uuid, tBTA_GATT_ID* p_output)
{
    memset((void *)p_output, 0, sizeof(tBTA_GATT_ID));
    bta_le_fill_16bits_gatt_id(inst_id, char_uuid, p_output);
}
*/
/*get remote name*/
static bool check_remote_name(tBTA_DM_INQ_RES* result, uint8_t* rmt_name, uint8_t* rmt_name_len)
{
    uint8_t *p_rmt_name = NULL;
    uint8_t remote_name_len = 0;
    
    if (result->p_eir) {
        p_rmt_name = BTM_CheckEirData(result->p_eir,
            BTM_EIR_COMPLETE_LOCAL_NAME_TYPE,
            &remote_name_len);
        if (!p_rmt_name)
            p_rmt_name = BTM_CheckEirData(result->p_eir,
                BTM_EIR_SHORTENED_LOCAL_NAME_TYPE,
                &remote_name_len);
        if (p_rmt_name) {
            if (remote_name_len > BD_NAME_LEN)
                remote_name_len = BD_NAME_LEN;
            if (rmt_name && rmt_name_len) {
                memcpy(rmt_name, p_rmt_name, remote_name_len);
                *(rmt_name + remote_name_len) = 0;
                *rmt_name_len = remote_name_len;
                }
            return true;
        }
    }
    return false;
}

/************************************************************************************
* * Function        bta_scan_recult_callback
* *
* * Description     scan result.it will be called when device scaned a peer device
* *
* * Return          NULL
**************************************************************************************/

#if 0
static void bta_scan_result_callback(tBTA_DM_SEARCH_EVT event, tBTA_DM_SEARCH* p_data)
{
    uint8_t len;
    BD_ADDR bd_addr;
    char dev_name[32];
    tBTA_GATT_TRANSPORT transport = BTA_GATT_TRANSPORT_LE;
    //char obj_name[] = "Find Me";
    char obj_name[] = "SimpleBLEClient";
    uint8_t dev_name_len;

    switch (event)
    {
        case BTA_DM_INQ_RES_EVT:
        {
            LOG_ERROR("scan result: event=%d, "BT_BD_ADDR_STR", device_type=%d\n",
                event, BT_BD_ADDR_HEX(p_data->inq_res.bd_addr), p_data->inq_res.device_type); 
            
            bdcpy(bd_addr, p_data->inq_res.bd_addr);
            if (p_data->inq_res.p_eir)
            {
                if (BTM_CheckEirData(p_data->inq_res.p_eir, BTM_EIR_COMPLETE_LOCAL_NAME_TYPE, &len))
                {
                    p_data->inq_res.remt_name_not_required = TRUE;
                }
            }

            if(check_remote_name(&(p_data->inq_res), dev_name, &dev_name_len))
            {
                LOG_ERROR("scan device name len=%d, name = %s\n", dev_name_len, dev_name);
            }

            if(strcmp(dev_name, obj_name) == 0)
            {
                bdcpy(obj_addr, bd_addr);
                LOG_ERROR("find the device, obj_addr="BT_BD_ADDR_STR"\n", BT_BD_ADDR_HEX(obj_addr));
//                BTA_GATTC_Open(client_if, obj_addr, true, transport);
            }
        }
        break;

        case BTA_DM_INQ_CMPL_EVT:
        {
            LOG_ERROR("%s-BLE observe complete. Num Resp %d\n", __FUNCTION__, p_data->inq_cmpl.num_resps);

            LOG_ERROR("connect the device "BT_BD_ADDR_STR", client_if=%d\n", 
                BT_BD_ADDR_HEX(obj_addr), client_if); 

		/* scan complete, start connect*/
            BTA_GATTC_Open(client_if, obj_addr, true, transport); 
        }
        break;

        default:
        LOG_ERROR("%s : unknown event 0x%x", __FUNCTION__, event);
    }
}

#endif	

/************************************************************************************
* * Function        bta_scan_param_setup_cback
* *
* * Description     set scan param callback.it will be called after setting scan parameter
* *
* * Return          NULL
**************************************************************************************/
/*
static void bta_scan_param_setup_cback(tGATT_IF c_client_if, tBTM_STATUS status)
{
    client_if = c_client_if;
    LOG_ERROR("\nset scan params complete: status=%d, client_if=%d\n", status, client_if);
    
    BTA_DmBleObserve(true, 8, bta_scan_result_callback);
}*/

/************************************************************************************
* * Function        bta_gattc_callback
* *
* * Description     app register callback
* *
* * Return          NULL
**************************************************************************************/
#if 0
static void bta_gattc_callback(tBTA_GATTC_EVT event, tBTA_GATTC* p_data)
{
    switch (event)
    {   
        case BTA_GATTC_REG_EVT:
        {  
            tBTA_GATT_STATUS  status = p_data->reg_oper.status;
            client_if = p_data->reg_oper.client_if;
            LOG_ERROR("%s:register complete: event=%d, status=%d, client_if=%d\n", __FUNCTION__, event, status, client_if);
            UINT8 scan_interval = 0x50;
            UINT8 scan_window = 0x30;
            tBLE_SCAN_MODE scan_mode = BTM_BLE_SCAN_MODE_ACTI;
            
            bac_register();
            /*register complete,set scan parameter*/
            BTA_DmSetBleScanParams(client_if, scan_interval, scan_window, scan_mode,
                bta_scan_param_setup_cback);
  
        }
        break;
        
        /*connect callback*/
        case BTA_GATTC_OPEN_EVT:
        {
            
            LOG_ERROR("\n%s:device is connected "BT_BD_ADDR_STR", client_if=%d, status=%d, connect_id=%d\n", 
                            __FUNCTION__, BT_BD_ADDR_HEX(p_data->open.remote_bda), p_data->open.client_if,
                             p_data->open.status, p_data->open.conn_id);
            /*return whether the remote device is currently connected*/
            int is_connected = BTA_DmGetConnectionState(p_data->open.remote_bda);
            LOG_ERROR("is_connected=%d\n",is_connected);
            /*get the energy info of the controller*/

            /*read battery level*/
            int conn_id = p_data->open.conn_id;

        }
        break;
		
        default:
        LOG_ERROR("%s:unknown event: %d\n", __FUNCTION__, event);
    }

}

#endif	

/************************************************************************************
* * Function        ble_client_appRegister
* *
* * Description     app register function
* *
* * Return          NULL
**************************************************************************************/
void ble_client_appRegister(void)
{
    
    bt_uuid_t uuid;
    tBT_UUID t_uuid;
    memcpy(&uuid, BASE_UUID, sizeof(bt_uuid_t));
    btif_to_bta_uuid(&t_uuid, &uuid);
    esp_err_t status;
    LOG_ERROR("register application\n");
    //BTA_GATTC_AppRegister(&t_uuid, bta_gattc_callback);

    //register the scan callback function to the gap moudule
    if((status = esp_ble_gap_register_callback(esp_scan_result_cb)) == ESP_OK){
		esp_ble_gap_set_scan_params(&ble_scan_params);
	}else{
		LOG_ERROR("gap register error, error code = %x\n",status);	
	}	

   //register the callback function to the gattc module
   if ((status = esp_ble_gattc_register_callback(esp_gattc_result_cb)) != ESP_OK){
	LOG_ERROR("gattc register error, error code = %x\n",status);		
   }	



   

}

void gattc_client_test(void)
{
    BTM_SetTraceLevel(BT_TRACE_LEVEL_DEBUG);
    esp_init_bluetooth();
    esp_enable_bluetooth();
    ble_client_appRegister();
}
