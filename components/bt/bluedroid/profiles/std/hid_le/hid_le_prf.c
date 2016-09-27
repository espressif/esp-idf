/**
 ****************************************************************************************
 *
 * @file hid_le_prf.c
 *
 * @brief Application entry point
 *
 * Copyright (C) Espressif 2016
 * Created by Yulong at 2016/9/7
 *
 *
 ****************************************************************************************
 */
#include <string.h>
#include "hid_le_prf.h"
#include "prf_defs.h"
#include "bta_gatt_api.h"

tHIDD_LE_ENV hidd_le_env;

tBT_UUID char_info_uuid = {LEN_UUID_16, {CHAR_HID_INFO_UUID}};
tBT_UUID char_ctnl_pt_uuid = {LEN_UUID_16, {CHAR_HID_CTNL_PT_UUID}};
tBT_UUID char_report_map_uuid = {LEN_UUID_16, {CHAR_REPORT_MAP_UUID}};
tBT_UUID char_proto_mode_uuid = {LEN_UUID_16, {CHAR_PROTOCOL_MODE_UUID}};
tBT_UUID char_kb_in_report_uuid = {LEN_UUID_16, {CHAR_BOOT_KB_IN_REPORT_UUID}};
tBT_UUID char_kb_out_report_uuid = {LEN_UUID_16,{CHAR_BOOT_KB_OUT_REPORT_UUID}};
tBT_UUID char_mouse_in_report_uuid = {LEN_UUID_16,{CHAR_BOOT_MOUSE_IN_REPORT_UUID}};



/// Full HID device Database Description - Used to add attributes into the database
const  tCHAR_DESC hids_char_db[HIDD_LE_CHAR_MAX] =
{
	// HID Information Characteristic Value
	[HIDD_LE_INFO_CHAR]                        = {
		&char_info_uuid,
		GATT_PERM_READ,
		GATT_CHAR_PROP_BIT_READ
	},

	// HID Control Point Characteristic Value
	[HIDD_LE_CTNL_PT_CHAR]                     = {
		&char_ctnl_pt_uuid,
		GATT_PERM_WRITE,
		GATT_CHAR_PROP_BIT_WRITE_NR
	},

	// Report Map Characteristic Value
	[HIDD_LE_REPORT_MAP_CHAR]                  = {
		&char_report_map_uuid,
		GATT_PERM_READ,
		GATT_CHAR_PROP_BIT_READ
	},

	// Protocol Mode Characteristic Declaration
	[HIDD_LE_PROTO_MODE_CHAR]                  = {
		&char_proto_mode_uuid,
		GATT_PERM_READ,
		GATT_CHAR_PROP_BIT_READ,
	},

	// Boot Keyboard Input Report Characteristic Value
	[HIDD_LE_BOOT_KB_IN_REPORT_CHAR]           = {
		&char_kb_in_report_uuid,
		GATT_PERM_READ,
		(GATT_CHAR_PROP_BIT_READ|GATT_CHAR_PROP_BIT_NOTIFY),

	},

	// Boot Keyboard Output Report Characteristic Value
	[HIDD_LE_BOOT_KB_OUT_REPORT_CHAR]          = {
		&char_kb_out_report_uuid,
		GATT_PERM_READ,
		(GATT_CHAR_PROP_BIT_READ|GATT_CHAR_PROP_BIT_WRITE|GATT_CHAR_PROP_BIT_WRITE_NR)
	},

	// Boot Mouse Input Report Characteristic Value
	[HIDD_LE_BOOT_MOUSE_IN_REPORT_CHAR]        = {
		&char_mouse_in_report_uuid,
		GATT_PERM_READ,
		(GATT_CHAR_PROP_BIT_READ|GATT_CHAR_PROP_BIT_NOTIFY),
	},
};


//tBT_UUID hid_uuid = {LEN_UUID_16, {ATT_SVC_BUTTON}};

static void HID_AddCharacteristic(const tCHAR_DESC *char_desc);

/*****************************************************************************
 **  Constants
 *****************************************************************************/
static void hidd_le_profile_cb(tBTA_GATTS_EVT event,  tBTA_GATTS *p_data);

/*******************************************************************************
 **
 ** Function         hidd_le_profile_cb
 **
 ** Description      the callback function after the hid device profile has been register to the BTA manager module
 **
 ** Returns          NULL 
 **
 *******************************************************************************/
static void HID_AddCharacteristic(const tCHAR_DESC *char_desc)
{	
	UINT16 service_id;
	if(char_desc == NULL)
	{
		LOG_ERROR("Invalid hid characteristic");
		return;
	}
	//check the hid device serivce has been register to the data base or not
	if(!hidd_le_env.enabled) 
	{
		LOG_ERROR("The hid device didn't register yet");
		return;
	}
	//get the service id from the env whitch has been register
	service_id = hidd_le_env.hidd_clcb.cur_srvc_id;
	if(char_desc->char_uuid != 0x00)
	{
		// start added the charact to the data base
		BTA_GATTS_AddCharacteristic(service_id,
				char_desc->char_uuid,
				char_desc->perm,
				char_desc->prop);
	}
}


/*******************************************************************************
 **
 ** Function         hidd_le_profile_cb
 **
 ** Description      the callback function after the hid device profile has been register to the BTA manager module
 **
 ** Returns          NULL 
 **
 *******************************************************************************/
static void hidd_le_profile_cb(tBTA_GATTS_EVT event, tBTA_GATTS *p_data)
{
	tBTA_GATTS_RSP rsp;
	tBT_UUID uuid = {LEN_UUID_16, {ATT_SVC_HID}};
	static UINT8 hid_char_idx;
	tHIDD_CLCB *p_clcb = NULL;
	switch(event)
	{
		case BTA_GATTS_REG_EVT:
			//check the register of the hid device profile has been succeess or not
			if(p_data->reg_oper.status != BTA_GATT_OK)
			{
				LOG_ERROR("button profile register failed\n");
			}
			//save the gatt interface in the hid device ENV
			hidd_le_env.gatt_if = p_data->reg_oper.server_if;
			//set the env flag to enable
			hidd_le_env.enabled = true;
			//create the hid device service to the service data base.
			if(p_data->reg_oper.uuid.uu.uuid16 == ATT_SVC_HID)
			{
				hidd_le_CreateService(true);
			}
			break;
		case BTA_GATTS_CREATE_EVT:
			if(p_data->create.uuid.uu.uuid16 == ATT_SVC_HID)
			{
				///store the service id to the env
				hidd_le_env.hidd_clcb.cur_srvc_id = p_data->create.service_id;
				//start the button service after created
				BTA_GATTS_StartService(p_data->create.service_id,BTA_GATT_TRANSPORT_LE);
				hid_char_idx = HIDD_LE_INFO_CHAR;
				//added the info character to the data base.
				HID_AddCharacteristic(&hids_char_db[hid_char_idx]);
				hid_char_idx++;
			}

			break;
		case BTA_GATTS_ADD_INCL_SRVC_EVT:

			break;
		case BTA_GATTS_ADD_CHAR_EVT:
			if(hid_char_idx < HIDD_LE_CHAR_MAX)	//added the characteristic until the index overflow
			{
				HID_AddCharacteristic(&hids_char_db[hid_char_idx]);
				if((p_data->add_result.char_uuid.uu.uuid16 == CHAR_BOOT_KB_IN_REPORT_UUID) ||
						(p_data->add_result.char_uuid.uu.uuid16 == CHAR_BOOT_MOUSE_IN_REPORT_UUID))
				{ 
					// add the gattc config descriptor to the notify charateristic
					//tBTA_GATT_PERM perm = (GATT_PERM_WRITE|GATT_PERM_WRITE);
					uuid.uu.uuid16 = GATT_UUID_CHAR_CLIENT_CONFIG;

					BTA_GATTS_AddCharDescriptor (hidd_le_env.hidd_clcb.cur_srvc_id,
							GATT_PERM_WRITE,
							&uuid);
				}
			}
			hid_char_idx++;
			break;
		case BTA_GATTS_ADD_CHAR_DESCR_EVT:

			break;
		case BTA_GATTS_WRITE_EVT:
			BTA_GATTS_SendRsp(p_data->req_data.conn_id,p_data->req_data.trans_id,
					p_data->req_data.status,NULL);
			break;
		case BTA_GATTS_CONNECT_EVT: 
			p_clcb = &hidd_le_env.hidd_clcb; 

			if(!p_clcb->in_use)
			{
				p_clcb->in_use = TRUE;
				p_clcb->conn_id = p_data->conn.conn_id;;
				LOG_ERROR("hidd->conn_id = %x\n",p_data->conn.conn_id);
				p_clcb->connected = TRUE;
				memcpy(p_clcb->remote_bda, p_data->conn.remote_bda,BD_ADDR_LEN);	
			}
			break;
		case BTA_GATTS_DISCONNECT_EVT:
			p_clcb = &hidd_le_env.hidd_clcb; 
			//set the connection flag to true
			p_clcb->connected = false;
			p_clcb->in_use = TRUE;
			memset(p_clcb->remote_bda,0,BD_ADDR_LEN);
			break;
		case BTA_GATTS_START_EVT:
			break;
		case BTA_GATTS_CONGEST_EVT:
			if(hidd_le_env.hidd_clcb.connected && (hidd_le_env.hidd_clcb.conn_id == p_data->conn.conn_id))
			{
				//set the connection channal congested flag to true
				hidd_le_env.hidd_clcb.congest =  p_data->congest.congested;
			}
			break;
		default:
			break;
	}
}

/*******************************************************************************
 **
 ** Function         hidd_le_CreateService
 **
 ** Description      Create a Service for the hid device profile
 **
 ** Parameters       is_primary: this service is the primary service or not,true is the primary service
 **								false is not the primary service
 **                  p_service_uuid: service UUID.
 **                
 ** Returns          NULL 
 **
 *******************************************************************************/
void hidd_le_CreateService(BOOLEAN is_primary)
{
	tBTA_GATTS_IF server_if ;
	tBT_UUID uuid = {LEN_UUID_16, {ATT_SVC_HID}};
	//the number of the hid device attributes in the hid service.
	UINT16 num_handle = HIDD_LE_IDX_NB;
		UINT8 inst = 0x00;
	server_if = hidd_le_env.gatt_if;
	hidd_le_env.inst_id = inst;
	//start create the hid device service
	BTA_GATTS_CreateService(server_if,&uuid,inst,num_handle,is_primary);
}




/*******************************************************************************
 **
 ** Function         hidd_le_init
 **
 ** Description      Initializa the GATT Service for button profiles.
 ** Returns          NULL 
 *******************************************************************************/
tGATT_STATUS hidd_le_init (void)
{
	tBT_UUID app_uuid = {LEN_UUID_16,{ATT_SVC_HID}};

	if(hidd_le_env.enabled)
	{
		LOG_ERROR("hid device svc already initaliezd");
		return GATT_ERROR;
	}
	else
	{
		memset(&hidd_le_env,0,sizeof(tHIDD_LE_ENV));
	}


	/* register the hid deivce profile to the BTA_GATTS module*/
		BTA_GATTS_AppRegister(&app_uuid,hidd_le_profile_cb);

	hidd_le_env.enabled = TRUE;

	return GATT_SUCCESS;
}




