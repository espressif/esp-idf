// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <stdint.h>

#include <string.h>
#include <stdbool.h>
#include <stdio.h>


#include "bt_target.h"
#include "bt_trace.h"
#include "bt_types.h"
#include "gatt_api.h"
#include "bta_api.h"
#include "bta_gatt_api.h"
#include "bta_gatts_int.h"

#include "blufi_prf.h"
#include "blufi_adv.h"

#define BT_BD_ADDR_STR         "%02x:%02x:%02x:%02x:%02x:%02x"
#define BT_BD_ADDR_HEX(addr)   addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]

UINT16 esp32_uuid = SVC_BLUFI_UUID;
UINT8 esp32_manu[17] = {0xff,0x20,0x14,0x07,0x22,0x00,0x02,0x5B,0x00,0x33,0x49,0x31,0x30,0x4a,0x30,0x30,0x31};
tBTA_BLE_MANU	p_esp32_manu = {sizeof(esp32_manu),esp32_manu};			/* manufacturer data */

tBTA_BLE_SERVICE esp32_service = {
							0x01,		//only one service in the ijiazu button profile
							false,
							&esp32_uuid
							};        /* 16 bits services */


tBLUFI_BLE_ADV_DATA esp32_adv_data[ADV_SCAN_IDX_MAX] = 
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



static tBLUFI_CB_ENV blufi_cb_env;



/*****************************************************************************
**  Constants
*****************************************************************************/
static void blufi_profile_cb(tBTA_GATTS_EVT event,  tBTA_GATTS *p_data);


/*******************************************************************************
**
** Function         blufi_profile_cb
**
** Description      the callback function after the profile has been register to the BTA manager module
**
** Returns          NULL 
**
*******************************************************************************/
static void blufi_profile_cb(tBTA_GATTS_EVT event, tBTA_GATTS *p_data)
{
	tBTA_GATTS_RSP rsp;
	tBT_UUID uuid = {LEN_UUID_16, {SVC_BLUFI_UUID}};
	tBLUFI_INST  *p_inst = &blufi_cb_env.blufi_inst;
	UINT8 net_event = 0xff;
	UINT8 len = 0;
	UINT8 *p_rec_data = NULL;
	tBTA_GATT_STATUS  status;

	LOG_ERROR("blufi profile cb event = %x\n",event);
	switch(event) {
		case BTA_GATTS_REG_EVT:
            status = p_data->reg_oper.status;

			LOG_ERROR("p_data->reg_oper.status = %x\n",p_data->reg_oper.status);
			LOG_ERROR("(p_data->reg_oper.uuid.uu.uuid16=%x\n",p_data->reg_oper.uuid.uu.uuid16);
			if(p_data->reg_oper.status != BTA_GATT_OK) {
				LOG_ERROR("blufi profile register failed\n");
				return;
			}

			blufi_cb_env.gatt_if = p_data->reg_oper.server_if;
			blufi_cb_env.enabled = true;
            LOG_ERROR("register complete: event=%d, status=%d, server_if=%d\n", 
                event, status, blufi_cb_env.gatt_if);
            
            LOG_ERROR("set advertising parameters\n");
			//set the advertising data to the btm layer
			BlufiBleConfigadvData(&esp32_adv_data[BLE_ADV_DATA_IDX], NULL);
			//set the adversting data to the btm layer
			BlufiBleSetScanRsp(&esp32_adv_data[BLE_SCAN_RSP_DATA_IDX],NULL);
    	    BTA_GATTS_Listen(blufi_cb_env.gatt_if, true, NULL);

			//create the blufi service to the service data base.
			if(p_data->reg_oper.uuid.uu.uuid16 == SVC_BLUFI_UUID) {
				blufi_create_service();
			}
			break;
		case BTA_GATTS_READ_EVT:
			memset(&rsp, 0, sizeof(tBTA_GATTS_API_RSP));
			rsp.attr_value.handle = p_data->req_data.p_data->read_req.handle;
			rsp.attr_value.len = 2;
			BTA_GATTS_SendRsp(p_data->req_data.conn_id,p_data->req_data.trans_id,
					  p_data->req_data.status,&rsp);
			break;
		case BTA_GATTS_WRITE_EVT:
			BTA_GATTS_SendRsp(p_data->req_data.conn_id,p_data->req_data.trans_id,
								p_data->req_data.status,NULL);

			LOG_ERROR("Received blufi data:");
			for(int i = 0; i < p_data->req_data.p_data->write_req.len; i++){
				LOG_ERROR("%x",p_data->req_data.p_data->write_req.value[i]);
			}
			LOG_ERROR("\n");

			if (p_data->req_data.p_data->write_req.handle == blufi_cb_env.blufi_inst.blufi_hdl) {

			    p_rec_data = &p_data->req_data.p_data->write_req.value[0];
				if (p_inst && p_inst->blufi_cback)
			        p_inst->blufi_cback(blufi_cb_env.blufi_inst.app_id, net_event, len, p_rec_data);
			}
			break;
		case BTA_GATTS_CONF_EVT:
			/* Nothing */
			break;
		case BTA_GATTS_CREATE_EVT:
			uuid.uu.uuid16 = CHAR_BLUFI_UUID;
			blufi_cb_env.clcb.cur_srvc_id = p_data->create.service_id;
			blufi_cb_env.is_primery =  p_data->create.is_primary;
			//start the blufi service after created
			BTA_GATTS_StartService(p_data->create.service_id, BTA_GATT_TRANSPORT_LE);
			//add the frist blufi characteristic --> write characteristic
			BTA_GATTS_AddCharacteristic(blufi_cb_env.clcb.cur_srvc_id, &uuid,
										(GATT_PERM_WRITE | GATT_PERM_READ),
										(GATT_CHAR_PROP_BIT_READ | GATT_CHAR_PROP_BIT_WRITE));
			break;
		case BTA_GATTS_ADD_CHAR_EVT:
			if(p_data->add_result.char_uuid.uu.uuid16 == CHAR_BLUFI_UUID)
			{
				uuid.uu.uuid16 = CHAR_BLUFI_UUID;
				//save the att handle to the env
				blufi_cb_env.blufi_inst.blufi_hdl = p_data->add_result.attr_id;
			}
			break;
		case BTA_GATTS_ADD_CHAR_DESCR_EVT:
			if(p_data->add_result.char_uuid.uu.uuid16 == GATT_UUID_CHAR_CLIENT_CONFIG)
			{
				blufi_cb_env.blufi_inst.blufi_hdl = p_data->add_result.attr_id;
			}
			break;
		case BTA_GATTS_CONNECT_EVT:
			//set the connection flag to true
			blufi_env_clcb_alloc(p_data->conn.conn_id, p_data->conn.remote_bda);
            LOG_ERROR("\ndevice is connected "BT_BD_ADDR_STR", server_if=%d,reason=0x%x,connect_id=%d\n", 
                             BT_BD_ADDR_HEX(p_data->conn.remote_bda), p_data->conn.server_if,
                             p_data->conn.reason, p_data->conn.conn_id);
            /*return whether the remote device is currently connected*/
            int is_connected = BTA_DmGetConnectionState(p_data->conn.remote_bda);
            LOG_ERROR("is_connected=%d\n",is_connected);
			BTA_DmBleBroadcast(0); //stop adv
			break;
		case BTA_GATTS_DISCONNECT_EVT:
			//set the connection flag to true
			blufi_cb_env.clcb.connected = false;
			break;
		case BTA_GATTS_OPEN_EVT:
			break;
		case BTA_GATTS_CLOSE_EVT:
			if(blufi_cb_env.clcb.connected && (blufi_cb_env.clcb.conn_id == p_data->conn.conn_id))
			{
				//set the connection channal congested flag to true
				blufi_cb_env.clcb.congest =  p_data->congest.congested;
			}
			break;
		case BTA_GATTS_LISTEN_EVT:
			break;
		case BTA_GATTS_CONGEST_EVT:
			break;
		default:
			break;
	}
}


/*******************************************************************************
**
** Function         blufi_create_service
**
** Description      Create a Service for the blufi profile
**
** Returns          NULL 
**
*******************************************************************************/
void blufi_create_service(void)
{
	tBTA_GATTS_IF server_if ;
	tBT_UUID uuid = {LEN_UUID_16, {SVC_BLUFI_UUID}};
	UINT16 num_handle = BLUFI_HDL_NUM;
	UINT8 inst = 0x00;
	server_if = blufi_cb_env.gatt_if;
	blufi_cb_env.inst_id = inst;
	if(!blufi_cb_env.enabled)
	{
		LOG_ERROR("blufi service added error.");
		return;
	}	
	BTA_GATTS_CreateService(server_if, &uuid, inst, num_handle, true);
	
}

/*******************************************************************************
**
** Function         blufi_env_clcb_alloc
**
** Description      The function allocates a GATT profile  connection link control block
**
** Returns          NULL if not found. Otherwise pointer to the connection link block.
**
*******************************************************************************/
tBLUFI_CLCB *blufi_env_clcb_alloc (UINT16 conn_id, BD_ADDR remote_bda)
{
	tBLUFI_CLCB *p_clcb = NULL;
	p_clcb = &blufi_cb_env.clcb; 
	
	if(!p_clcb->in_use)
	{
		p_clcb->in_use = TRUE;
		p_clcb->conn_id = conn_id;
		p_clcb->connected = TRUE;
		memcpy(p_clcb->remote_bda, remote_bda, BD_ADDR_LEN);	
		LOG_ERROR("p_clcb->conn_id = %x\n", conn_id);
	}

	return p_clcb;
}

/*******************************************************************************
**
** Function         blufi_env_find_conn_id_by_bd_adddr
**
** Description      The function searches all LCB with macthing bd address
**
** Returns          total number of clcb found.
**
*******************************************************************************/
/*
UINT16 blufi_env_find_conn_id_by_bd_adddr(BD_ADDR remote_bda)
{
	UINT8 i_clcb;
	tBLUFI_CLCB *p_clcb = NULL;

	for(i_clcb = 0, p_clcb = &blufi_cb_env.clcb; i_clcb < BLUFIT_MAX_APPS; i_clcb++, p_clcb++)
	{
		if(p_clcb->in_use && p_clcb->connected && memcmp(p_clcb->remote_bda, remote_bda, BD_ADDR_LEN) == 0)
		{
			return p_clcb->conn_id;
		}
	}

	return GATT_INVALID_CONN_ID;
}
*/
/*******************************************************************************
**
** Function         blufi_env_clcb_dealloc
**
** Description      The function deallocates a GATT profile  connection link control block
**
** Returns          True the deallocation is successful
**
*******************************************************************************/
BOOLEAN blufi_env_clcb_dealloc(UINT16 conn_id)
{
//	UINT8 i_clcb = 0;
//	tBLUFI_CLCB *p_clcb = NULL;

	return FALSE;
}


/*******************************************************************************
**
** Function         blufi_init
**
** Description      Initializa the GATT Service for blufi profiles.
**
*******************************************************************************/
tGATT_STATUS blufi_profile_init (tBLUFI_CBACK *call_back)
{
	tBT_UUID app_uuid = {LEN_UUID_16, {SVC_BLUFI_UUID}};
	

	if(blufi_cb_env.enabled)
	{
		LOG_ERROR("blufi svc already initaliezd");
		return GATT_ERROR;
	}
	else
	{
		memset(&blufi_cb_env,0,sizeof(tBLUFI_CB_ENV));
	}
	

	if(call_back != NULL)
    {
        blufi_cb_env.blufi_inst.blufi_cback = call_back;
    }

	
	/* register the blufi profile to the BTA_GATTS module*/
	BTA_GATTS_AppRegister(&app_uuid, blufi_profile_cb);

	return GATT_SUCCESS;
}

void blufi_msg_notify(UINT8 len, UINT8 *blufi_msg)
{
	 BOOLEAN conn_status = blufi_cb_env.clcb.connected;
	 UINT16 conn_id = blufi_cb_env.clcb.conn_id;
	 UINT16 attr_id = blufi_cb_env.blufi_inst.blufi_hdl;
	 //notify rsp==false; indicate rsp==true.
	 BOOLEAN rsp = false;
	 if(!conn_status && blufi_cb_env.clcb.congest)
	 {
		LOG_ERROR("the conneciton for blufi profile has been loss");
		return;
	 }
	 
	 BTA_GATTS_HandleValueIndication (conn_id, attr_id, len,
                                      blufi_msg, rsp);
}
