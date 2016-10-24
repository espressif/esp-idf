/**
 ****************************************************************************************
 *
 * @file blufi_pro.c
 *
 * @brief Application entry point
 *
 * Copyright (C) Espressif 2016
 * Created by Yulong at 2016/8/18
 *
 *
 ****************************************************************************************
 */

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

	LOG_ERROR("blufi profile cb event = %x\n",event);
	switch(event) {
		case BTA_GATTS_REG_EVT:
			
			LOG_ERROR("p_data->reg_oper.status = %x\n",p_data->reg_oper.status);
			LOG_ERROR("(p_data->reg_oper.uuid.uu.uuid16=%x\n",p_data->reg_oper.uuid.uu.uuid16);
			if(p_data->reg_oper.status != BTA_GATT_OK) {
				LOG_ERROR("blufi profile register failed\n");
				return;
			}

			blufi_cb_env.gatt_if = p_data->reg_oper.server_if;
			blufi_cb_env.enabled = true;

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
				//tBTA_GATT_PERM perm = GATT_PERM_READ;
				//save the att handle to the env
				blufi_cb_env.blufi_inst.blufi_hdl = p_data->add_result.attr_id;
				//add the frist blufi characteristic --> Notify characteristic
				//BTA_GATTS_AddCharacteristic(blufi_cb_env.clcb.cur_srvc_id,&uuid,
				//						GATT_PERM_READ,(GATT_CHAR_PROP_BIT_READ|GATT_CHAR_PROP_BIT_NOTIFY));
			}
#if 0
			else if(p_data->add_result.char_uuid.uu.uuid16 == CHAR_BLUFI_UUID){ // add the gattc config descriptor to the notify charateristic
				//tBTA_GATT_PERM perm = (GATT_PERM_WRITE|GATT_PERM_WRITE);
				uuid.uu.uuid16 = GATT_UUID_CHAR_CLIENT_CONFIG;
				blufi_cb_env.blufi_inst.blufi_hdl = p_data->add_result.attr_id;
				BTA_GATTS_AddCharDescriptor (blufi_cb_env.clcb.cur_srvc_id,
                                  				(GATT_PERM_WRITE|GATT_PERM_WRITE),
                                  				&uuid);
			}
			
#endif
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
