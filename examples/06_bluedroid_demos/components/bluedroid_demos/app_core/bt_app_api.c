/**
 ****************************************************************************************
 *
 * @file bt_app_api.c
 *
 * @brief Application entry point
 *
 * Copyright (C) Espressif 2016
 * Created by Yulong at 2016/10/17
 *
 *
 ****************************************************************************************
 */

#include "bt_app_api.h"
#include "btm_ble_api.h"
//#include "btm_ble_int.h"


void API_Ble_AppConfigAdvData(tESP_BLE_ADV_DATA *adv_data,
											tAPI_SET_ADV_DATA_CMPL_CBACK *p_adv_data_cback)
{
	if(adv_data != NULL)
	{
		ESP_AppBleConfigadvData(adv_data, p_adv_data_cback);
	}else{
		LOG_ERROR("The adv_data is NULL\n");
	}

	if(++adv_data != NULL)
	{
		ESP_AppBleSetScanRsp(adv_data,NULL);
	}
			
}


void API_Ble_AppStartAdvertising(tESP_API_BLE_ADV_PARAMS_ALL *ble_adv_params)
{
	if (!API_BLE_ISVALID_PARAM(ble_adv_params->adv_int_min, BTM_BLE_ADV_INT_MIN, BTM_BLE_ADV_INT_MAX) ||
        !API_BLE_ISVALID_PARAM(ble_adv_params->adv_int_max, BTM_BLE_ADV_INT_MIN, BTM_BLE_ADV_INT_MAX))
    {
    	LOG_ERROR("Invalid advertisting interval parameters.\n");
        return ;
    }

	if ((ble_adv_params->adv_type < API_NON_DISCOVERABLE) && 
		(ble_adv_params->adv_type > API_BROADCASTER_MODE) )
	{
		LOG_ERROR("Invalid advertisting type parameters.\n");
		return;
	}

	if ((ble_adv_params->adv_filter_policy < API_ADV_NON_CONN) && 
		(ble_adv_params->adv_filter_policy > API_ADV_DIRECT) )
	{
		LOG_ERROR("Invalid advertisting type parameters.\n");
		return;
	}

	///
	BTA_DmSetBleAdvParamsAll(ble_adv_params->adv_int_min,
							 ble_adv_params->adv_int_max,
							 ble_adv_params->adv_type,
							 ble_adv_params->addr_type_own,
							 ble_adv_params->channel_map,
							 ble_adv_params->adv_filter_policy,
							 ble_adv_params->p_dir_bda);

	
}


void API_Ble_SetScanParams (tESP_BLE_SCAN_PARAMS *scan_params, tGATT_IF client_if,
                            		            tBLE_SCAN_PARAM_SETUP_CBACK scan_param_setup_cback)
{
	if (API_BLE_ISVALID_PARAM(scan_params->scan_intv, BTM_BLE_SCAN_INT_MIN, BTM_BLE_SCAN_INT_MAX) &&
        API_BLE_ISVALID_PARAM(scan_params->scan_win, BTM_BLE_SCAN_WIN_MIN, BTM_BLE_SCAN_WIN_MAX) &&
       (scan_params->scan_type == BTM_BLE_SCAN_MODE_ACTI || scan_params->scan_type == BTM_BLE_SCAN_MODE_PASS))
	{
		BTA_DmSetBleScanFilterParams(client_if, 
									scan_params->scan_intv,
									scan_params->scan_win,
									scan_params->scan_type,
									scan_params->scan_fil_policy,
									scan_params->addr_type_own,
									scan_param_setup_cback);
	}
}


void API_Ble_StartScanning (UINT8 duration, tBTA_DM_SEARCH_CBACK *p_results_cb)
{
	if((duration != 0) && (p_results_cb != NULL))
	{
		///Start scan the device
		BTA_DmBleObserve(true, duration, p_results_cb);	
	}else{
		LOG_ERROR("The scan duration or p_results_cb invalid\n");
	}
}

void API_Ble_AppStopAdvertising(void)
{
	bool stop_adv = true;
	
	BTA_DmBleBroadcast(stop_adv);
}

void API_Ble_AppUpdateConnectionParams(BD_ADDR bd_addr, UINT16 min_int, 
												UINT16 max_int, UINT16 latency, UINT16 timeout)
{
	if (min_int > max_int){
		min_int = max_int;
	}

	if (min_int < BTM_BLE_CONN_INT_MIN || max_int > BTM_BLE_CONN_INT_MAX){
		LOG_ERROR("Invalid interval value.\n");
	}

	 BTA_DmBleUpdateConnectionParams(bd_addr, min_int, max_int,
                                     latency, timeout);
	
}

void API_Ble_SetPacketDataLength(BD_ADDR remote_device, UINT16 tx_data_length)
{
	if (tx_data_length > BTM_BLE_DATA_SIZE_MAX){
       tx_data_length =  BTM_BLE_DATA_SIZE_MAX;
	}else if (tx_data_length < BTM_BLE_DATA_SIZE_MIN){
       tx_data_length =  BTM_BLE_DATA_SIZE_MIN;
	}

	BTA_DmBleSetDataLength(remote_device, tx_data_length);
}


void API_Ble_SetRandAddress(BD_ADDR rand_addr)
{
	if (rand_addr != NULL){
		BTA_DmSetRandAddress(rand_addr);
	}else{
		LOG_ERROR("Invalid randrom address.\n");
	}
}

void API_Ble_ConfigLocalPrivacy(BOOLEAN privacy_enable)
{
	 BTA_DmBleConfigLocalPrivacy(privacy_enable);
}




void API_Ble_GattcAppRegister(tBT_UUID *p_app_uuid, tBTA_GATTC_CBACK *p_client_cb)
{
	if (p_app_uuid != NULL)
	{
		BTA_GATTC_AppRegister(p_app_uuid, *p_client_cb);
	}else{
		LOG_ERROR("The app uuid invalid.\n");
	}
}

void API_Ble_GattcAppDeregister(tBTA_GATTC_IF client_if)
{
	BTA_GATTC_AppDeregister(client_if);
}

void API_Ble_GattcOpen(tBTA_GATTC_IF client_if, BD_ADDR remote_bda, BOOLEAN is_direct)
{
	tBTA_GATT_TRANSPORT transport = BTA_GATT_TRANSPORT_LE;
	if(remote_bda == NULL ){
		LOG_ERROR("Invaild address data \n");
		return;
	}

	BTA_GATTC_Open(client_if, remote_bda, is_direct, transport);
	
	

	
}

void API_GattcCancelOpen(tBTA_GATTC_IF client_if, BD_ADDR remote_bda, BOOLEAN is_direct)
{
	if(remote_bda == NULL ){
		LOG_ERROR("Invaild address data \n");
		return;
	}

	BTA_GATTC_CancelOpen(client_if, remote_bda, is_direct);
}

void API_Ble_GattcClose(UINT16 conn_id)
{
	BTA_GATTC_Close(conn_id);
}


tGATT_STATUS API_Ble_GattcConfigureMTU (UINT16 conn_id, UINT16 mtu)
{
	if ((mtu < GATT_DEF_BLE_MTU_SIZE) || (mtu > GATT_MAX_MTU_SIZE)){
		LOG_ERROR("Invalid MTU parameters\n");
		return GATT_ILLEGAL_PARAMETER;
	}

	BTA_GATTC_ConfigureMTU (conn_id, mtu);
	
}

void API_Ble_GattcServiceSearchRequest (UINT16 conn_id, tBT_UUID *p_srvc_uuid)
{
	BTA_GATTC_ServiceSearchRequest(conn_id, p_srvc_uuid);
}


tBTA_GATT_STATUS  API_Ble_GattcGetFirstChar (UINT16 conn_id, tBTA_GATT_SRVC_ID *p_srvc_id,
                                          	tBT_UUID *p_char_uuid_cond,
                                          	tBTA_GATTC_CHAR_ID *p_char_result,
                                          	tBTA_GATT_CHAR_PROP *p_property)
{
	tBTA_GATT_STATUS status = 0;
	
	if (!p_srvc_id || !p_char_result){
    	return BTA_GATT_ILLEGAL_PARAMETER;
	}

	status = BTA_GATTC_GetFirstChar (conn_id, p_srvc_id, p_char_uuid_cond,
                                     p_char_result, p_property);

	return status;
	
	
}

tBTA_GATT_STATUS  API_Ble_GattcGetFirstCharDescr (UINT16 conn_id, tBTA_GATTC_CHAR_ID *p_char_id,
                                                tBT_UUID *p_descr_uuid_cond,
                                                tBTA_GATTC_CHAR_DESCR_ID *p_descr_result)
{
	tBTA_GATT_STATUS    status;

    if (!p_char_id || !p_descr_result){
      return BTA_GATT_ILLEGAL_PARAMETER;
    }

	status = BTA_GATTC_GetFirstCharDescr (conn_id, p_char_id, p_descr_uuid_cond, p_descr_result);

	return status;
	
}


tBTA_GATT_STATUS  API_Ble_GattcGetNextChar (UINT16 conn_id,
                                         tBTA_GATTC_CHAR_ID *p_start_char_id,
                                         tBT_UUID           *p_char_uuid_cond,
                                         tBTA_GATTC_CHAR_ID *p_char_result,
                                         tBTA_GATT_CHAR_PROP    *p_property)
{
	
	tBTA_GATT_STATUS	status;
	
	   if (!p_start_char_id || !p_char_result){
	   	return BTA_GATT_ILLEGAL_PARAMETER;
	   }

	status = BTA_GATTC_GetNextChar(conn_id, p_start_char_id, p_char_uuid_cond,
                                   p_char_result, p_property);

	return status;
	   
}

tBTA_GATT_STATUS  API_Ble_GattcGetNextCharDescr (UINT16 conn_id,
                                             tBTA_GATTC_CHAR_DESCR_ID *p_start_descr_id,
                                             tBT_UUID           *p_descr_uuid_cond,
                                             tBTA_GATTC_CHAR_DESCR_ID *p_descr_result)
{
	 tBTA_GATT_STATUS status;

    if (!p_start_descr_id || !p_descr_result){
    	return BTA_GATT_ILLEGAL_PARAMETER;
    }

	status = BTA_GATTC_GetNextCharDescr (conn_id, p_start_descr_id, p_descr_uuid_cond, p_descr_result);

	return status;
	
}

tBTA_GATT_STATUS  API_Ble_GattcGetFirstIncludedService(UINT16 conn_id, tBTA_GATT_SRVC_ID *p_srvc_id,
                                                    tBT_UUID *p_uuid_cond, tBTA_GATTC_INCL_SVC_ID *p_result)
{
	
	tBTA_GATT_STATUS status;
	
	if (!p_srvc_id || !p_result){
		return BTA_GATT_ILLEGAL_PARAMETER;
	}

	status = BTA_GATTC_GetFirstIncludedService(conn_id, p_srvc_id, p_uuid_cond, p_result);

	return status;
}

tBTA_GATT_STATUS  API_Ble_GattcGetNextIncludedService(UINT16 conn_id,
                                                   tBTA_GATTC_INCL_SVC_ID *p_start_id,
                                                   tBT_UUID               *p_uuid_cond,
                                                   tBTA_GATTC_INCL_SVC_ID *p_result)
{
	tBTA_GATT_STATUS status;

    if (!p_start_id || !p_result){
    	return BTA_GATT_ILLEGAL_PARAMETER;
    }

	status = BTA_GATTC_GetNextIncludedService(conn_id, p_start_id, p_uuid_cond, p_result);

	return status;
}


void API_Ble_GattcReadCharacteristic(UINT16 conn_id, tBTA_GATTC_CHAR_ID *p_char_id,
                                  tBTA_GATT_AUTH_REQ auth_req)
{
	 BTA_GATTC_ReadCharacteristic(conn_id, p_char_id, auth_req);
}


void API_Ble_GattcReadCharDescr (UINT16 conn_id,
                             					tBTA_GATTC_CHAR_DESCR_ID  *p_descr_id,
                              					tBTA_GATT_AUTH_REQ auth_req)
{
	BTA_GATTC_ReadCharDescr (conn_id, p_descr_id, auth_req);
}

void API_Ble_GattcReadMultiple(UINT16 conn_id, tBTA_GATTC_MULTI *p_read_multi,
                            				tBTA_GATT_AUTH_REQ auth_req)
{
	 BTA_GATTC_ReadMultiple(conn_id, p_read_multi, auth_req);
}

void API_Ble_GattcWriteCharValue ( UINT16 conn_id,
                               							 tBTA_GATTC_CHAR_ID *p_char_id,
                                						 tBTA_GATTC_WRITE_TYPE  write_type,
                                						 UINT16 len,
                                						 UINT8 *p_value,
                                						 tBTA_GATT_AUTH_REQ auth_req)
{
	BTA_GATTC_WriteCharValue (conn_id, p_char_id, write_type, len, p_value, auth_req);
}

void API_Ble_GattcWriteCharDescr (UINT16 conn_id,
                              					 tBTA_GATTC_CHAR_DESCR_ID *p_char_descr_id,
                              					 tBTA_GATTC_WRITE_TYPE  write_type,
                              					 tBTA_GATT_UNFMT      *p_data,
                              					 tBTA_GATT_AUTH_REQ auth_req)
{
	 BTA_GATTC_WriteCharDescr (conn_id, p_char_descr_id, write_type, p_data, auth_req);
}


void API_Ble_GattcPrepareWrite  (UINT16 conn_id, tBTA_GATTC_CHAR_ID *p_char_id,
                              						UINT16 offset, UINT16 len, UINT8 *p_value,
                              						tBTA_GATT_AUTH_REQ auth_req)
{
	BTA_GATTC_PrepareWrite  (conn_id, p_char_id, offset, len, p_value, auth_req);
}

void  API_Ble_GattcExecuteWrite (UINT16 conn_id, BOOLEAN is_execute)
{
	BTA_GATTC_ExecuteWrite (conn_id, is_execute);
}

void API_Ble_GattcSendIndConfirm (UINT16 conn_id, tBTA_GATTC_CHAR_ID *p_char_id)
{
	BTA_GATTC_SendIndConfirm (conn_id, p_char_id);
}

tBTA_GATT_STATUS API_Ble_GattcRegisterForNotifications (tBTA_GATTC_IF client_if,
                                                     BD_ADDR bda,
                                                     tBTA_GATTC_CHAR_ID *p_char_id)
{
	tBTA_GATT_STATUS    status = BTA_GATT_ILLEGAL_PARAMETER;
	status = BTA_GATTC_RegisterForNotifications (client_if, bda, p_char_id);

	return status;
}

tBTA_GATT_STATUS API_Ble_GattcDeregisterForNotifications (tBTA_GATTC_IF client_if,
                                                       							BD_ADDR bda,
                                                       							tBTA_GATTC_CHAR_ID *p_char_id)
{
	tBTA_GATT_STATUS    status = BTA_GATT_ILLEGAL_PARAMETER;
	status = BTA_GATTC_DeregisterForNotifications (client_if, bda, p_char_id);

	return status;
}

void API_Ble_GattsDisable(void)
{
	 BTA_GATTS_Disable();
}

void API_Ble_GattsAppRegister(tBT_UUID *p_app_uuid, tBTA_GATTS_CBACK *p_cback)
{
	BTA_GATTS_AppRegister(p_app_uuid, p_cback);
}


void API_Ble_GattsAppDeregister(tBTA_GATTS_IF server_if)
{
	BTA_GATTS_AppDeregister(server_if);
}

void API_Ble_GattsCreateService(tBTA_GATTS_IF server_if, tBT_UUID *p_service_uuid, UINT8 inst,
                             UINT16 num_handle, BOOLEAN is_primary)
{
	BTA_GATTS_CreateService(server_if, p_service_uuid, inst, num_handle, is_primary);
}


void API_Ble_GattsAddIncludeService(UINT16 service_id, UINT16 included_service_id)
{
	BTA_GATTS_AddIncludeService(service_id, included_service_id);
}


void API_Ble_GattsAddCharacteristic (UINT16 service_id,  tBT_UUID  *p_char_uuid,
                                  					tBTA_GATT_PERM perm, tBTA_GATT_CHAR_PROP property)
{
	BTA_GATTS_AddCharacteristic (service_id, p_char_uuid, perm, property);
}

void API_Ble_GattsAddCharDescriptor (UINT16 service_id,
													   tBTA_GATT_PERM perm,
													   tBT_UUID  * p_descr_uuid)
{
	BTA_GATTS_AddCharDescriptor (service_id, perm, p_descr_uuid);
}

void API_Ble_GattsDeleteService(UINT16 service_id)
{
	 BTA_GATTS_DeleteService(service_id);
}

void API_Ble_GattsStartService(UINT16 service_id, tBTA_GATT_TRANSPORT sup_transport)
{
	BTA_GATTS_StartService(service_id, sup_transport);
}


void API_Ble_GattsStopService(UINT16 service_id)
{
	BTA_GATTS_StopService(service_id);
}


void API_Ble_GattsHandleValueIndication (UINT16 conn_id, UINT16 attr_id, UINT16 data_len,
                                      								UINT8 *p_data, BOOLEAN need_confirm)
{
	BTA_GATTS_HandleValueIndication (conn_id, attr_id, data_len, p_data, need_confirm);
}

void API_Ble_GattsSendRsp (UINT16 conn_id, UINT32 trans_id,
                        			tBTA_GATT_STATUS status, tBTA_GATTS_RSP *p_msg)
{
	BTA_GATTS_SendRsp (conn_id, trans_id, status, p_msg);
}

void API_Ble_GattsOpen(tBTA_GATTS_IF server_if, BD_ADDR remote_bda, BOOLEAN is_direct,
				   tBTA_GATT_TRANSPORT transport)
{
	BTA_GATTS_Open(server_if, remote_bda, is_direct, transport);
}

void API_Ble_GattsCancelOpen(tBTA_GATTS_IF server_if, BD_ADDR remote_bda, BOOLEAN is_direct)
{
	 BTA_GATTS_CancelOpen(server_if, remote_bda, is_direct);
}

void API_Ble_GattsClose(UINT16 conn_id)
{
	BTA_GATTS_Close(conn_id);
}







