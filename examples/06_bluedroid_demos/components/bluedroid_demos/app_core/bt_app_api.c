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


void esp_ble_config_adv_data (esp_ble_adv_data_cfg_t *adv_data,
												     esp_ble_set_adv_data_cmpl_cb_t *adv_data_cback)
{
	if(adv_data != NULL)
	{
		ble_config_adv_data(adv_data, adv_data_cback);
	}else{
		LOG_ERROR("The adv_data is NULL\n");
	}

	if(++adv_data != NULL)
	{
		ble_set_scan_rsp(adv_data,NULL);
	}
			
}


void esp_ble_start_advertising (esp_ble_adv_params_all_t *ble_adv_params)
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

	if ((ble_adv_params->adv_filter_policy < ADV_ALLOW_SCAN_ANY_CON_ANY) && 
		(ble_adv_params->adv_filter_policy > ADV_ALLOW_SCAN_WLST_CON_WLST) )
	{
		LOG_ERROR("Invalid advertisting type parameters.\n");
		return;
	}

	LOG_ERROR("API_Ble_AppStartAdvertising\n");

	///
	BTA_DmSetBleAdvParamsAll(ble_adv_params->adv_int_min,
							 ble_adv_params->adv_int_max,
							 ble_adv_params->adv_type,
							 ble_adv_params->addr_type_own,
							 ble_adv_params->channel_map,
							 ble_adv_params->adv_filter_policy,
							 ble_adv_params->p_dir_bda);

	
}


void esp_ble_set_scan_params (esp_ble_scan_params *scan_params, esp_gattc_if_t client_if,
                            		            esp_scan_param_setup_cb_t scan_param_setup_cback)
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


void esp_ble_start_scanning (UINT8 duration, esp_dm_search_cb_t *results_cb)
{
	if((duration != 0) && (results_cb != NULL))
	{
		///Start scan the device
		BTA_DmBleObserve(true, duration, results_cb);	
	}else{
		LOG_ERROR("The scan duration or p_results_cb invalid\n");
	}
}

void esp_ble_stop_advertising (void)
{
	bool stop_adv = true;
	
	BTA_DmBleBroadcast(stop_adv);
}

void esp_ble_update_conec_params (BD_ADDR bd_addr, uint16_t min_int, 
												uint16_t max_int, uint16_t latency, uint16_t timeout)
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

void esp_ble_set_pkt_data_len (BD_ADDR remote_device, uint16_t tx_data_length)
{
	if (tx_data_length > BTM_BLE_DATA_SIZE_MAX){
       tx_data_length =  BTM_BLE_DATA_SIZE_MAX;
	}else if (tx_data_length < BTM_BLE_DATA_SIZE_MIN){
       tx_data_length =  BTM_BLE_DATA_SIZE_MIN;
	}

	BTA_DmBleSetDataLength(remote_device, tx_data_length);
}


void esp_ble_set_rand_addr (BD_ADDR rand_addr)
{
	if (rand_addr != NULL){
		BTA_DmSetRandAddress(rand_addr);
	}else{
		LOG_ERROR("Invalid randrom address.\n");
	}
}

void  esp_ble_config_local_privacy (BOOLEAN privacy_enable)
{
	 BTA_DmBleConfigLocalPrivacy(privacy_enable);
}




void esp_ble_gattc_app_register(esp_bt_uuid_t *app_uuid, esp_gattc_cb_t *client_cb)
{
	if (app_uuid != NULL)
	{
		BTA_GATTC_AppRegister(app_uuid, client_cb);
	}else{
		LOG_ERROR("The app uuid invalid.\n");
	}
}

void esp_ble_gattc_app_unregister(esp_gattc_if_t client_if)
{
	BTA_GATTC_AppDeregister(client_if);
}

void esp_ble_gattc_conn(esp_gattc_if_t client_if, BD_ADDR remote_bda, BOOLEAN is_direct)
{
	tBTA_GATT_TRANSPORT transport = BTA_GATT_TRANSPORT_LE;
	if(remote_bda == NULL ){
		LOG_ERROR("Invaild address data \n");
		return;
	}

	BTA_GATTC_Open(client_if, remote_bda, is_direct, transport);
	
	

	
}

void esp_ble_gattc_cancel_conn (esp_gattc_if_t client_if, BD_ADDR remote_bda, BOOLEAN is_direct)
{
	if(remote_bda == NULL ){
		LOG_ERROR("Invaild address data \n");
		return;
	}

	BTA_GATTC_CancelOpen(client_if, remote_bda, is_direct);
}

void esp_ble_gattc_close (uint16_t conn_id)
{
	BTA_GATTC_Close(conn_id);
}


tGATT_STATUS esp_ble_gattc_config_mtu (uint16_t conn_id, uint16_t mtu)
{
	if ((mtu < GATT_DEF_BLE_MTU_SIZE) || (mtu > GATT_MAX_MTU_SIZE)){
		LOG_ERROR("Invalid MTU parameters\n");
		return GATT_ILLEGAL_PARAMETER;
	}

	BTA_GATTC_ConfigureMTU (conn_id, mtu);
	
}

void esp_ble_gattc_svc_search_req (uint16_t conn_id, esp_bt_uuid_t *srvc_uuid)
{
	BTA_GATTC_ServiceSearchRequest(conn_id, srvc_uuid);
}


tBTA_GATT_STATUS  esp_ble_gattc_get_first_char (uint16_t conn_id, esp_gatt_srvc_id_t *srvc_id,
                                          						   esp_bt_uuid_t *char_uuid_cond,
                                          						   esp_gattc_char_id_t *char_result,
                                          						   esp_gatt_char_prop_t *property)
{
	tBTA_GATT_STATUS status = 0;
	
	if (!srvc_id || !char_result){
    	return BTA_GATT_ILLEGAL_PARAMETER;
	}

	status = BTA_GATTC_GetFirstChar (conn_id, srvc_id, char_uuid_cond,
                                     char_result, property);

	return status;
	
	
}

tBTA_GATT_STATUS  esp_ble_gattc_get_first_char_descr (uint16_t conn_id, esp_gattc_char_id_t *char_id,
                                                esp_bt_uuid_t *descr_uuid_cond,
                                                esp_gattc_char_descr_id_t *descr_result)
{
	tBTA_GATT_STATUS    status;

    if (!char_id || !descr_result){
      return BTA_GATT_ILLEGAL_PARAMETER;
    }

	status = BTA_GATTC_GetFirstCharDescr (conn_id, char_id, descr_uuid_cond, descr_result);

	return status;
	
}


tBTA_GATT_STATUS  esp_ble_gattc_get_next_char (uint16_t conn_id,
                                         esp_gattc_char_id_t *start_char_id,
                                         esp_bt_uuid_t       *char_uuid_cond,
                                         esp_gattc_char_id_t *char_result,
                                         esp_gatt_char_prop_t    *property)
{
	
	tBTA_GATT_STATUS	status;
	
	   if (!start_char_id || !char_result){
	   	return BTA_GATT_ILLEGAL_PARAMETER;
	   }

	status = BTA_GATTC_GetNextChar(conn_id, start_char_id, char_uuid_cond,
                                   char_result, property);

	return status;
	   
}

tBTA_GATT_STATUS   esp_ble_gattc_get_next_char_descr (uint16_t conn_id,
                                             esp_gattc_char_descr_id_t *start_descr_id,
                                             esp_bt_uuid_t             *descr_uuid_cond,
                                             esp_gattc_char_descr_id_t *descr_result)
{
	 tBTA_GATT_STATUS status;

    if (!start_descr_id || !descr_result){
    	return BTA_GATT_ILLEGAL_PARAMETER;
    }

	status = BTA_GATTC_GetNextCharDescr (conn_id, start_descr_id, descr_uuid_cond, descr_result);

	return status;
	
}

tBTA_GATT_STATUS  esp_ble_gattc_get_first_inclu_srvc (uint16_t conn_id, esp_gatt_srvc_id_t *srvc_id,
                                                    esp_bt_uuid_t *uuid_cond, esp_gattc_incl_srvc_id_t *result)
{
	
	tBTA_GATT_STATUS status;
	
	if (!srvc_id || !result){
		return BTA_GATT_ILLEGAL_PARAMETER;
	}

	status = BTA_GATTC_GetFirstIncludedService(conn_id, srvc_id, uuid_cond, result);

	return status;
}

tBTA_GATT_STATUS  esp_ble_gattc_get_next_inclu_srvc (uint16_t conn_id,
                                                   esp_gattc_incl_srvc_id_t *start_id,
                                                   esp_bt_uuid_t            *uuid_cond,
                                                   esp_gattc_incl_srvc_id_t *result)
{
	tBTA_GATT_STATUS status;

    if (!start_id || !result){
    	return BTA_GATT_ILLEGAL_PARAMETER;
    }

	status = BTA_GATTC_GetNextIncludedService(conn_id, start_id, uuid_cond, result);

	return status;
}


void esp_ble_gattc_read_char (uint16_t conn_id, esp_gattc_char_id_t *char_id,
                                  esp_gatt_auth_req_t auth_req)
{
	 BTA_GATTC_ReadCharacteristic(conn_id, char_id, auth_req);
}


void esp_ble_gattc_read_char_descr (uint16_t conn_id,
                             							 esp_gattc_char_descr_id_t  *descr_id,
                              							 esp_gatt_auth_req_t auth_req)
{
	BTA_GATTC_ReadCharDescr (conn_id, descr_id, auth_req);
}

void esp_ble_gattc_read_multi (uint16_t conn_id, esp_gattc_multi_t *read_multi,
                            esp_gatt_auth_req_t auth_req)
{
	 BTA_GATTC_ReadMultiple(conn_id, read_multi, auth_req);
}

void esp_ble_gattc_write_char_val ( uint16_t conn_id,
                               							 esp_gattc_char_id_t *char_id,
                                						 esp_gattc_write_type_t  write_type,
                                						 uint16_t len,
                                						 uint8_t *value,
                                						 esp_gatt_auth_req_t auth_req)
{
	BTA_GATTC_WriteCharValue (conn_id, char_id, write_type, len, value, auth_req);
}


void esp_ble_gattc_write_char_descr (uint16_t conn_id,
                               esp_gattc_char_descr_id_t *char_descr_id,
                               esp_gattc_write_type_t  write_type,
                               esp_gatt_unfmt_t      *data,
                               esp_gatt_auth_req_t auth_req)
{
	 BTA_GATTC_WriteCharDescr (conn_id, char_descr_id, write_type, data, auth_req);
}


void esp_ble_gattc_prepa_write (uint16_t conn_id, esp_gattc_char_id_t *char_id,
                              						uint16_t offset, uint16_t len, uint8_t *value,
                              						esp_gatt_auth_req_t auth_req)
{
	BTA_GATTC_PrepareWrite  (conn_id, char_id, offset, len, value, auth_req);
}

void  esp_ble_gattc_execu_write (uint16_t conn_id, BOOLEAN is_execute)
{
	BTA_GATTC_ExecuteWrite (conn_id, is_execute);
}

void esp_ble_gattc_send_ind_cfm (uint16_t conn_id, esp_gattc_char_id_t *char_id)
{
	BTA_GATTC_SendIndConfirm (conn_id, char_id);
}

tBTA_GATT_STATUS esp_ble_gattc_register_ntf (esp_gattc_if_t client_if,
                                                     BD_ADDR bda,
                                                     esp_gattc_char_id_t *char_id)
{
	tBTA_GATT_STATUS    status = BTA_GATT_ILLEGAL_PARAMETER;
	status = BTA_GATTC_RegisterForNotifications (client_if, bda, char_id);

	return status;
}

tBTA_GATT_STATUS esp_ble_gattc_unregister_ntf (esp_gattc_if_t client_if,
                                                       BD_ADDR bda,
                                                       esp_gattc_char_id_t *char_id)
{
	tBTA_GATT_STATUS    status = BTA_GATT_ILLEGAL_PARAMETER;
	status = BTA_GATTC_DeregisterForNotifications (client_if, bda, char_id);

	return status;
}

void esp_ble_gatts_disale(void)
{
	 BTA_GATTS_Disable();
}

void esp_ble_gatts_app_register(esp_bt_uuid_t *app_uuid, esp_gatts_cb_t *cback)
{
	BTA_GATTS_AppRegister(app_uuid, cback);
}


void esp_ble_gatts_app_unregister(esp_gatts_if_t server_if)
{
	BTA_GATTS_AppDeregister(server_if);
}

void esp_ble_gatts_create_srvc(esp_gatts_if_t server_if, esp_bt_uuid_t *service_uuid, uint8_t inst,
                             uint16_t num_handle, BOOLEAN is_primary)
{
	BTA_GATTS_CreateService(server_if, service_uuid, inst, num_handle, is_primary);
}


void esp_ble_gatts_add_inclu_srvc (uint16_t service_id, uint16_t included_service_id)
{
	BTA_GATTS_AddIncludeService(service_id, included_service_id);
}


void esp_ble_gatts_add_char (uint16_t service_id,  esp_bt_uuid_t  *char_uuid,
                                  esp_gatt_perm_t perm, esp_gatt_char_prop_t property)
{
	BTA_GATTS_AddCharacteristic (service_id, char_uuid, perm, property);
}

void esp_ble_gatts_add_char_descr (uint16_t service_id,
                                  						esp_gatt_perm_t perm,
                                  						esp_bt_uuid_t  * descr_uuid)
{
	BTA_GATTS_AddCharDescriptor (service_id, perm, descr_uuid);
}

void esp_ble_gatts_dele_srvc (uint16_t service_id)
{
	 BTA_GATTS_DeleteService(service_id);
}

void esp_ble_gatts_start_srvc(uint16_t service_id)
{
	tBTA_GATT_TRANSPORT transport = BT_TRANSPORT_LE;
	BTA_GATTS_StartService(service_id, transport);
}


void esp_ble_gatts_stop_srvc(uint16_t service_id)
{
	BTA_GATTS_StopService(service_id);
}


void esp_ble_gatts_hdl_val_indica (uint16_t conn_id, uint16_t attr_id, uint16_t data_len,
                                      								uint8_t *data, BOOLEAN need_confirm)
{
	BTA_GATTS_HandleValueIndication (conn_id, attr_id, data_len, data, need_confirm);
}

void esp_ble_gatts_send_rsp (uint16_t conn_id, uint32_t trans_id,
                        			esp_gatt_status_t status, esp_gatts_rsp_t *msg)
{
	BTA_GATTS_SendRsp (conn_id, trans_id, status, msg);
}

void esp_ble_gatts_conn (esp_gatts_if_t server_if, BD_ADDR remote_bda, BOOLEAN is_direct)
{
	tBTA_GATT_TRANSPORT transport = BT_TRANSPORT_LE;
	BTA_GATTS_Open(server_if, remote_bda, is_direct, transport);
}

void esp_ble_gatts_cancel_conn (esp_gatts_if_t server_if, BD_ADDR remote_bda, BOOLEAN is_direct)
{
	 BTA_GATTS_CancelOpen(server_if, remote_bda, is_direct);
}

void esp_ble_gatts_close(uint16_t conn_id)
{
	BTA_GATTS_Close(conn_id);
}







