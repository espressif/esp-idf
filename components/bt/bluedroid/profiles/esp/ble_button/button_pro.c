/**
 ****************************************************************************************
 *
 * @file button_pro.c
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
#include "button_pro.h"

#include "prf_defs.h"

#if (BUT_PROFILE_CFG)


#define ARRAY_SIZE(x)	(sizeof(x)/sizeof((x)[0]))

tBUTTON_CB_ENV button_cb_env;



/*****************************************************************************
**  Constants
*****************************************************************************/
static void button_profile_cb(tBTA_GATTS_EVT event,  tBTA_GATTS *p_data);


/*******************************************************************************
**
** Function         button_profile_cb
**
** Description      the callback function after the profile has been register to the BTA manager module
**
** Returns          NULL 
**
*******************************************************************************/
static void button_profile_cb(tBTA_GATTS_EVT event, tBTA_GATTS *p_data)
{
	tBTA_GATTS_RSP rsp;
	tBT_UUID uuid = {LEN_UUID_16, {ATT_SVC_BUTTON}};
	tBUT_INST  *p_inst = &button_cb_env.button_inst;
	UINT8 net_event = 0xff;
	UINT8 len = 0;
	UINT8 *p_rec_data = NULL;
	//LOG_ERROR("p_data->status = %x\n",p_data->status);
	//if(p_data->status != BTA_GATT_OK){
	//	LOG_ERROR("button profile register failed\n");
	//	return;
	//}
	LOG_ERROR("button profile cb event = %x\n",event);
	switch(event)	
	{
		case BTA_GATTS_REG_EVT:
			
			LOG_ERROR("p_data->reg_oper.status = %x\n",p_data->reg_oper.status);
			LOG_ERROR("(p_data->reg_oper.uuid.uu.uuid16=%x\n",p_data->reg_oper.uuid.uu.uuid16);
			if(p_data->reg_oper.status != BTA_GATT_OK)
			{
				LOG_ERROR("button profile register failed\n");
			}
			button_cb_env.gatt_if = p_data->reg_oper.server_if;
			button_cb_env.enabled = true;
			//button_cb_env.button_inst.app_id = p_data->reg_oper.uuid;
			//create the button service to the service data base.
			if(p_data->reg_oper.uuid.uu.uuid16 == ATT_SVC_BUTTON)
			{
				Button_CreateService();
			}
			break;
		case BTA_GATTS_READ_EVT:
			//tBTA_GATTS_RSP rsp;
			memset(&rsp,0,sizeof(tBTA_GATTS_API_RSP));
			rsp.attr_value.handle = p_data->req_data.p_data->read_req.handle;
			rsp.attr_value.len = 2;
			BTA_GATTS_SendRsp(p_data->req_data.conn_id,p_data->req_data.trans_id,
					  p_data->req_data.status,&rsp);
			break;
		case BTA_GATTS_WRITE_EVT:
			BTA_GATTS_SendRsp(p_data->req_data.conn_id,p_data->req_data.trans_id,
								p_data->req_data.status,NULL);
			LOG_ERROR("Received button data:");
			for(int i = 0; i < p_data->req_data.p_data->write_req.len; i++){
				LOG_ERROR("%x",p_data->req_data.p_data->write_req.value[i]);
			}
			LOG_ERROR("\n");
			if(p_data->req_data.p_data->write_req.handle == button_cb_env.button_inst.but_wirt_hdl){
			if(p_data->req_data.p_data->write_req.value[0] == 0xFF){
				net_event = RECEIVE_NET_PASSWD_EVT;
				len = p_data->req_data.p_data->write_req.len - 1;
				p_rec_data = &p_data->req_data.p_data->write_req.value[1];
				LOG_ERROR("RECEIVE_NET_PASSWD_EVT\n");
			}else if(p_data->req_data.p_data->write_req.value[0] == 0xEE){
				
				net_event = RECEIVE_NET_SSD_EVT;
				len = p_data->req_data.p_data->write_req.len - 1;
				p_rec_data = &p_data->req_data.p_data->write_req.value[1];
				LOG_ERROR("RECEIVE_NET_SSD_EVT\n");
			}else{
				net_event = 0xff;
				len = p_data->req_data.p_data->write_req.len;
				p_rec_data = p_data->req_data.p_data->write_req.value;
				
		}		
//	button_msg_notify(len,p_rec_data);	
	(*p_inst->p_cback)(button_cb_env.button_inst.app_id,net_event,len,p_rec_data);
			}
			break;
		case BTA_GATTS_CONF_EVT:
			
			break;
		case BTA_GATTS_CREATE_EVT:
			//tBT_UUID uuid_butt_write;
			uuid.uu.uuid16 = ATT_CHAR_BUTTON_WIT;
			//tBTA_GATT_PERM perm = (GATT_PERM_WRITE|GATT_PERM_READ);
			//tBTA_GATT_CHAR_PROP prop = (GATT_CHAR_PROP_BIT_READ|GATT_CHAR_PROP_BIT_WRITE);
			//uuid = {LEN_UUID_16, {ATT_SVC_BUTTON}};
			button_cb_env.clcb.cur_srvc_id=  p_data->create.service_id;
			button_cb_env.is_primery =  p_data->create.is_primary;
			//uuid = {LEN_UUID_16, {ATT_CHAR_BUTTON_WIT}};
			//start the button service after created
			BTA_GATTS_StartService(p_data->create.service_id,BTA_GATT_TRANSPORT_LE);
			//add the frist button characteristic --> write characteristic
			BTA_GATTS_AddCharacteristic(button_cb_env.clcb.cur_srvc_id,&uuid,
										(GATT_PERM_WRITE|GATT_PERM_READ),
										(GATT_CHAR_PROP_BIT_READ|GATT_CHAR_PROP_BIT_WRITE));
			break;
			
		case BTA_GATTS_ADD_CHAR_EVT:
			if(p_data->add_result.char_uuid.uu.uuid16 == ATT_CHAR_BUTTON_WIT)
			{
				uuid.uu.uuid16 = ATT_CHAR_BUTTON_NTF;
				//tBTA_GATT_PERM perm = GATT_PERM_READ;
				tBTA_GATT_CHAR_PROP prop = (GATT_CHAR_PROP_BIT_READ|GATT_CHAR_PROP_BIT_NOTIFY);
				//save the att handle to the env
				button_cb_env.button_inst.but_wirt_hdl = p_data->add_result.attr_id;
				//add the frist button characteristic --> Notify characteristic
				BTA_GATTS_AddCharacteristic(button_cb_env.clcb.cur_srvc_id,&uuid,
										GATT_PERM_READ,(GATT_CHAR_PROP_BIT_READ|GATT_CHAR_PROP_BIT_NOTIFY));
			}else if(p_data->add_result.char_uuid.uu.uuid16 == ATT_CHAR_BUTTON_NTF){ // add the gattc config descriptor to the notify charateristic
				//tBTA_GATT_PERM perm = (GATT_PERM_WRITE|GATT_PERM_WRITE);
				uuid.uu.uuid16 = GATT_UUID_CHAR_CLIENT_CONFIG;
				button_cb_env.button_inst.but_ntf_hdl = p_data->add_result.attr_id;
				BTA_GATTS_AddCharDescriptor (button_cb_env.clcb.cur_srvc_id,
                                  				(GATT_PERM_WRITE|GATT_PERM_WRITE),
                                  				&uuid);
			}
			
			break;
		case BTA_GATTS_ADD_CHAR_DESCR_EVT:
			if(p_data->add_result.char_uuid.uu.uuid16 == GATT_UUID_CHAR_CLIENT_CONFIG)
			{
				button_cb_env.button_inst.but_cfg_hdl = p_data->add_result.attr_id;
			}
			break;
		case BTA_GATTS_CONNECT_EVT:
			//set the connection flag to true
			button_env_clcb_alloc(p_data->conn.conn_id, p_data->conn.remote_bda);
			break;
		case BTA_GATTS_DISCONNECT_EVT:
			//set the connection flag to true
			button_cb_env.clcb.connected = false;
			break;
		case BTA_GATTS_OPEN_EVT:
			break;
		case BTA_GATTS_CLOSE_EVT:
			if(button_cb_env.clcb.connected && (button_cb_env.clcb.conn_id == p_data->conn.conn_id))
			{
				//set the connection channal congested flag to true
				button_cb_env.clcb.congest =  p_data->congest.congested;
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
** Function         Button_CreateService
**
** Description      Create a Service for the button profile
**
** Returns          NULL 
**
*******************************************************************************/
void Button_CreateService(void)
{
	tBTA_GATTS_IF server_if ;
	tBT_UUID uuid = {LEN_UUID_16, {ATT_SVC_BUTTON}};
	UINT16 num_handle = KEY_IDX_NB;
	UINT8 inst = 0x00;
	server_if = button_cb_env.gatt_if;
	button_cb_env.inst_id = inst;
	//if(!button_cb_env.enabled)
	//{
	//	LOG_ERROR("button service added error.");
	//}	
	BTA_GATTS_CreateService(server_if,&uuid,inst,num_handle,true);
	
}

/*******************************************************************************
**
** Function         button_env_clcb_alloc
**
** Description      The function allocates a GATT profile  connection link control block
**
** Returns          NULL if not found. Otherwise pointer to the connection link block.
**
*******************************************************************************/
tBUT_CLCB *button_env_clcb_alloc (UINT16 conn_id, BD_ADDR remote_bda)
{
	tBUT_CLCB *p_clcb = NULL;
	p_clcb = &button_cb_env.clcb; 
	
	if(!p_clcb->in_use)
	{
		p_clcb->in_use = TRUE;
		p_clcb->conn_id = conn_id;
		LOG_ERROR("p_clcb->conn_id = %x\n",conn_id);
		p_clcb->connected = TRUE;
		memcpy(p_clcb->remote_bda,remote_bda,BD_ADDR_LEN);	
	}

	return p_clcb;
}

/*******************************************************************************
**
** Function         button_env_find_conn_id_by_bd_adddr
**
** Description      The function searches all LCB with macthing bd address
**
** Returns          total number of clcb found.
**
*******************************************************************************/
UINT16 button_env_find_conn_id_by_bd_adddr(BD_ADDR remote_bda)
{
	UINT8 i_clcb;
	tBUT_CLCB *p_clcb = NULL;

	for(i_clcb = 0, p_clcb = &button_cb_env.clcb; i_clcb < BUTT_MAX_APPS; i_clcb++, p_clcb++)
	{
		if(p_clcb->in_use && p_clcb->connected &&memcmp(p_clcb->remote_bda,remote_bda,BD_ADDR_LEN))
		{
			return p_clcb->conn_id;
		}
	}

	return GATT_INVALID_CONN_ID;
}

/*******************************************************************************
**
** Function         button_env_clcb_dealloc
**
** Description      The function deallocates a GATT profile  connection link control block
**
** Returns          True the deallocation is successful
**
*******************************************************************************/
/*
BOOLEAN button_env_clcb_dealloc(UINT16 conn_id)
{
	UINT8 i_clcb = 0;
	tBUT_CLCB *p_clcb = NULL;

	for(i_clcb = 0, p_clcb = &button_cb_env.clcb; i_clcb < BUTT_MAX_APPS; i_clcb++, p_clcb++)
	{
		if(p_clcb->in_use && p_clcb->connected && (p_clcb->conn_id == conn_id))
		{
			unsigned j;
			for(j = 0; j < ARRAY_SIZE(p_clcb->button_value.data_string);j++)
			{
				if(p_clcb->button_value.data_string[j])
				{
					GKI_freebuf(p_clcb->button_value.data_string[j]);
				}
			}
			memset(p_clcb, 0, sizeof(tBUT_CLCB));
			return TRUE;
		}
	}

	return FALSE;
}*/


/*******************************************************************************
**
** Function         button_init
**
** Description      Initializa the GATT Service for button profiles.
**
*******************************************************************************/
tGATT_STATUS button_init (tBU_CBACK *call_back)
{
	tBT_UUID app_uuid = {LEN_UUID_16,{ATT_SVC_BUTTON}};
	

	if(button_cb_env.enabled)
	{
		LOG_ERROR("button svc already initaliezd");
		return GATT_ERROR;
	}
	else
	{
		memset(&button_cb_env,0,sizeof(tBUTTON_CB_ENV));
	}
	

	 if(call_back != NULL)
        {
                button_cb_env.button_inst.p_cback = call_back;
        }

	
	/* register the button profile to the BTA_GATTS module*/
	 BTA_GATTS_AppRegister(&app_uuid,button_profile_cb);

	button_cb_env.enabled = TRUE;

	return GATT_SUCCESS;
}

void button_msg_notify(UINT8 len, UINT8 *button_msg)
{
	 BOOLEAN conn_status = button_cb_env.clcb.connected;
	 UINT16 conn_id = button_cb_env.clcb.conn_id;
	 UINT16 attr_id = button_cb_env.button_inst.but_ntf_hdl;
	 //notify rsp==false; indicate rsp==true.
	 BOOLEAN rsp = false;
	 if(!conn_status && button_cb_env.clcb.congest)
	 {
		LOG_ERROR("the conneciton for button profile has been loss");
		return;
	 }
	 
	 BTA_GATTS_HandleValueIndication (conn_id, attr_id, len,
                                      button_msg, rsp);
}

#endif	///BUT_PROFILE_CFG
