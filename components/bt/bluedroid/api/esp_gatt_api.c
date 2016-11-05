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

#include "esp_gatt_api.h"
#include "btm_ble_api.h"
//#include "btm_ble_int.h"



/*******************************************************************************
**
** @function        esp_ble_config_adv_data
**
** @brief           This function is called to override the BTA default ADV parameters.
**
** @param[in]       adv_data: Pointer to User defined ADV data structure. This
**                  memory space can not be freed until p_adv_data_cback
**                  is received.
** @param[in|out]   adv_data_cback: set adv data complete callback.
**
** @return          None
**
*******************************************************************************/
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
		ble_set_scan_rsp(adv_data, NULL);
	}
			
}


/*******************************************************************************
**
** @function        esp_ble_start_advertising
**
** @brief           This function is called to start advertising.
**
** @param[in]       esp_ble_adv_params_all_t: ointer to User defined adv_params data structure.
**
** @return          None
**
*******************************************************************************/
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


/*******************************************************************************
**
** @function        esp_ble_set_scan_params
**
** @brief           This function is called to set scan parameters
**
** @param[in]       esp_ble_scan_params: Pointer to User defined scan_params data structure. This
**                  memory space can not be freed until scan_param_setup_cback
** @param[in]       client_if: Client interface ID
** @param[in|out]   scan_param_setup_status_cback - Set scan param status callback
**
** @return          None
**
*******************************************************************************/
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


/*******************************************************************************
**
** @function        esp_ble_start_scanning
**
** @brief           This procedure keep the device scanning the peer device whith advertising on the air 
**
** @param[in]       duration: Keeping the scaning time, the unit is second.
** @param[in|out]	results_cb: The scanning function callback when the peer device has been scan 
**					which advertising on the air
**
** @return          None
**
*******************************************************************************/
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


/*******************************************************************************
**
** @function        esp_ble_stop_advertising
**
** @brief           This function is called to stop advertising.
**
** @param       	None
**
** @return          None
**
*******************************************************************************/
void esp_ble_stop_advertising (void)
{
	bool stop_adv = false;
	
	BTA_DmBleBroadcast(stop_adv);
}


/*******************************************************************************
**
** @function        esp_ble_update_conn_params
**
** @brief           Update connection parameters, can only be used when connection is up.
**
** @param[in]      bd_addr   - BD address of the peer
** @param[in]      min_int   -     minimum connection interval, [0x0004~ 0x4000]
** @param[in]      max_int   -     maximum connection interval, [0x0004~ 0x4000]
** @param[in]      latency   -     slave latency [0 ~ 500]
** @param[in]      timeout   -     supervision timeout [0x000a ~ 0xc80]
**
** @return          None
**
*******************************************************************************/
void esp_ble_update_conn_params (BD_ADDR bd_addr, uint16_t min_int, 
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



/*******************************************************************************
**
** @function        esp_ble_set_pkt_data_len
**
** @brief           This function is to set maximum LE data packet size
**
** @return          None
**
**
*******************************************************************************/
void esp_ble_set_pkt_data_len (BD_ADDR remote_device, uint16_t tx_data_length)
{
	if (tx_data_length > BTM_BLE_DATA_SIZE_MAX){
       tx_data_length =  BTM_BLE_DATA_SIZE_MAX;
	}else if (tx_data_length < BTM_BLE_DATA_SIZE_MIN){
       tx_data_length =  BTM_BLE_DATA_SIZE_MIN;
	}

	BTA_DmBleSetDataLength(remote_device, tx_data_length);
}



/*******************************************************************************
**
** @function        esp_ble_set_rand_addr
**
** @brief           This function set the random address for the appliction
**
** @param[in]       rand_addr: the random address whith should be setting
** 
** @return          None
**
**
*******************************************************************************/
void esp_ble_set_rand_addr (BD_ADDR rand_addr)
{
	if (rand_addr != NULL){
		BTA_DmSetRandAddress(rand_addr);
	}else{
		LOG_ERROR("Invalid randrom address.\n");
	}
}


/*******************************************************************************
**
** @function        esp_ble_config_local_privacy
**
** @brief           Enable/disable privacy on the local device
**
** @param[in]       privacy_enable   - enable/disabe privacy on remote device.
**
** @return          None
**
*******************************************************************************/
void  esp_ble_config_local_privacy (BOOLEAN privacy_enable)
{
	 BTA_DmBleConfigLocalPrivacy(privacy_enable);
}


/*******************************************************************************
** @function		esp_gattc_disable
** @brief			This function is called by application to disable GATTC module
**
** @param			NULL      	
**
** @return       	None.
**
*******************************************************************************/
void esp_gattc_disable(void)
{
	// TODO: This API funciton didn't finish, will be added it in future
}


/*******************************************************************************
**
** @function        esp_ble_gattc_app_register
**
** @brief	      	This function is called to register application callbacks
**                  with GATTC module.
**
** @param[in]       app_uuid  - applicaiton UUID
** @param[in]       client_cb - pointer to the application callback function.
**
** @return          None
**
*******************************************************************************/
void esp_ble_gattc_app_register(esp_bt_uuid_t *app_uuid, esp_gattc_cb_t *client_cb)
{
	if (app_uuid != NULL)
	{
		BTA_GATTC_AppRegister(app_uuid, client_cb);
	}else{
		LOG_ERROR("The app uuid invalid.\n");
	}
}


/*******************************************************************************
**
** @function        esp_ble_gattc_app_unregister
**
** @brief      		This function is called to unregister an application
**                  from GATTC module.
**
** @param[in]       client_if - client interface identifier.
**
** @return          None
**
*******************************************************************************/
void esp_ble_gattc_app_unregister(esp_gattc_if_t client_if)
{
	BTA_GATTC_AppDeregister(client_if);
}


/*******************************************************************************
**
** @function        esp_ble_gattc_conn
**
** @brief      		Open a direct connection or add a background auto connection
**                  bd address
**
** @param[in]       client_if: server interface.
** @param[in]       remote_bda: remote device BD address.
** @param[in]       is_direct: direct connection or background auto connection
**                  
** @return          None
**
*******************************************************************************/
void esp_ble_gattc_conn(esp_gattc_if_t client_if, BD_ADDR remote_bda, BOOLEAN is_direct)
{
	tBTA_GATT_TRANSPORT transport = BTA_GATT_TRANSPORT_LE;
	if(remote_bda == NULL ){
		LOG_ERROR("Invaild address data \n");
		return;
	}

	BTA_GATTC_Open(client_if, remote_bda, is_direct, transport);
	
	

	
}


/*******************************************************************************
**
** @function        esp_ble_gattc_cancel_conn
**
** @brief      		Cancel a direct open connection or remove a background auto connection
**                  bd address
**
** @param[in]       client_if: server interface.
** @param[in]       remote_bda: remote device BD address.
** @param[in]       is_direct: direct connection or background auto connection
**
** @return          None
**
*******************************************************************************/
void esp_ble_gattc_cancel_conn (esp_gattc_if_t client_if, BD_ADDR remote_bda, BOOLEAN is_direct)
{
	if(remote_bda == NULL ){
		LOG_ERROR("Invaild address data \n");
		return;
	}

	BTA_GATTC_CancelOpen(client_if, remote_bda, is_direct);
}


/*******************************************************************************
**
** @function        esp_ble_gattc_close
**
** @brief      		Close a connection to a GATT server.
**
** @param[in]       conn_id: connectino ID to be closed.
**
** @return          None
**
*******************************************************************************/
void esp_ble_gattc_close (uint16_t conn_id)
{
	BTA_GATTC_Close(conn_id);
}


/*******************************************************************************
**
** @function        esp_ble_gattc_config_mtu
**
** @brief      		Configure the MTU size in the GATT channel. This can be done
**                  only once per connection.
**
** @param[in]       conn_id: connection ID.
**                  mtu: desired MTU size to use.
**
** @return          Command status code:
**  - @ref ESP_GATT_OK: If request succeeds
**  - @ref ESP_GATT_ILLEGAL_PARAMETER: If the mtu value invalid
**
*******************************************************************************/
esp_gatt_status_t esp_ble_gattc_config_mtu (uint16_t conn_id, uint16_t mtu)
{
	if ((mtu < GATT_DEF_BLE_MTU_SIZE) || (mtu > GATT_MAX_MTU_SIZE)){
		LOG_ERROR("Invalid MTU parameters\n");
		return ESP_GATT_ILLEGAL_PARAMETER;
	}

	BTA_GATTC_ConfigureMTU (conn_id, mtu);
	
}


/*******************************************************************************
**
** @function        esp_ble_gattc_svc_search_req
**
** @brief      		This function is called to request a GATT service discovery
**                  on a GATT server. This function report service search result
**                  by a callback event, and followed by a service search complete
**                  event.
**
** @param[in]       conn_id: connection ID.
** @param[in]       srvc_uuid: a UUID of the service application is interested in.
**                  If Null, discover for all services.
**
** @return          None
**
*******************************************************************************/
void esp_ble_gattc_svc_search_req (uint16_t conn_id, esp_bt_uuid_t *srvc_uuid)
{
	BTA_GATTC_ServiceSearchRequest(conn_id, srvc_uuid);
}


/****************************************************************************************************
**
** @function        esp_ble_gattc_get_first_char
**
** @brief      		This function is called to find the first characteristic of the
**                  service on the given server.
**
** @param[in]       conn_id: connection ID which identify the server.
** @param[in]       srvc_id: the service ID of which the characteristic is belonged to.
** @param[in]       char_uuid_cond: Characteristic UUID, if NULL find the first available
**                  characteristic.
** @param[in]       char_result: output parameter which will store the GATT
**                  characteristic ID.
** @param[in]       property: output parameter to carry the characteristic property.
**
** @return          Command status code:
**  - @ref ESP_GATT_OK: If request succeeds
**  - @ref ESP_GATT_ILLEGAL_PARAMETER: If the srvc_id or char_result parameter is NULL.
**
*****************************************************************************************************/
esp_gatt_status_t  esp_ble_gattc_get_first_char (uint16_t conn_id, esp_gatt_srvc_id_t *srvc_id,
                                          						   esp_bt_uuid_t *char_uuid_cond,
                                          						   esp_gattc_char_id_t *char_result,
                                          						   esp_gatt_char_prop_t *property)
{
	esp_gatt_status_t status = 0;
	
	if (!srvc_id || !char_result){
    	return ESP_GATT_ILLEGAL_PARAMETER;
	}

	status = BTA_GATTC_GetFirstChar (conn_id, srvc_id, char_uuid_cond,
                                     char_result, property);

	return status;
	
	
}


/*******************************************************************************
**
** @function        esp_ble_gattc_get_first_char_descr
**
** @brief      		This function is called to find the first characteristic descriptor of the
**                  characteristic on the given server.
**
** @param[in]       conn_id: connection ID which identify the server.
** @param[in]       char_id: the characteristic ID of which the descriptor is belonged to.
** @param[in] 		descr_uuid_cond: Characteristic Descr UUID, if NULL find the first available
**                  characteristic.
** @param[in]       descr_result: output parameter which will store the GATT
**                 	characteristic descriptor ID.
**
** @return          Command status code:
**  - @ref ESP_GATT_OK: If request succeeds
**  - @ref ESP_GATT_ILLEGAL_PARAMETER: If the char_id or descr_result parameter is NULL.
**
*******************************************************************************/
esp_gatt_status_t  esp_ble_gattc_get_first_char_descr (uint16_t conn_id, esp_gattc_char_id_t *char_id,
                                                esp_bt_uuid_t *descr_uuid_cond,
                                                esp_gattc_char_descr_id_t *descr_result)
{
	esp_gatt_status_t    status;

    if (!char_id || !descr_result){
      return ESP_GATT_ILLEGAL_PARAMETER;
    }

	status = BTA_GATTC_GetFirstCharDescr (conn_id, char_id, descr_uuid_cond, descr_result);

	return status;
	
}


/*******************************************************************************
**
** @function        esp_ble_gattc_get_next_char
**
** @brief      		This function is called to find the next characteristic of the
**                  service on the given server.
**
** @param[in]       conn_id: connection ID which identify the server.
** @param[in]       start_char_id: start the characteristic search from the next record
**                  after the one identified by char_id.
** @param[in]       char_uuid_cond: Characteristic UUID, if NULL find the first available
**                  characteristic.
** @param[in]       char_result: output parameter which will store the GATT
**                  characteristic ID.
** @param[in]       property: output parameter to carry the characteristic property.
**
** @return          Command status code:
**  - @ref ESP_GATT_OK: If request succeeds
**  - @ref ESP_GATT_ILLEGAL_PARAMETER: If the start_char_id or char_result parameter is NULL.
** 
*******************************************************************************/

esp_gatt_status_t  esp_ble_gattc_get_next_char (uint16_t conn_id,
                                         esp_gattc_char_id_t *start_char_id,
                                         esp_bt_uuid_t       *char_uuid_cond,
                                         esp_gattc_char_id_t *char_result,
                                         esp_gatt_char_prop_t    *property)
{
	
	esp_gatt_status_t	status;
	
	   if (!start_char_id || !char_result){
	   	return ESP_GATT_ILLEGAL_PARAMETER;
	   }

	status = BTA_GATTC_GetNextChar(conn_id, start_char_id, char_uuid_cond,
                                   char_result, property);

	return status;
	   
}


/*******************************************************************************
**
** @function        esp_ble_gattc_get_next_char_descr
**
** @brief      		This function is called to find the next characteristic descriptor
**                  of the characterisctic.
**
** @param[in]       conn_id: connection ID which identify the server.
** @param[in]       start_descr_id: start the descriptor search from the next record
**                  after the one identified by p_start_descr_id.
** @param[in]       descr_uuid_cond: Characteristic descriptor UUID, if NULL find
**                               the first available characteristic descriptor.
** @param[in]       descr_result: output parameter which will store the GATT
**                                  characteristic descriptor ID.
**
** @return          Command status code:
**  - @ref ESP_GATT_OK: If request succeeds
**  - @ref ESP_GATT_ILLEGAL_PARAMETER: If the start_descr_id or descr_result parameter is NULL.
**
*******************************************************************************/

esp_gatt_status_t   esp_ble_gattc_get_next_char_descr (uint16_t conn_id,
                                             esp_gattc_char_descr_id_t *start_descr_id,
                                             esp_bt_uuid_t             *descr_uuid_cond,
                                             esp_gattc_char_descr_id_t *descr_result)
{
	 esp_gatt_status_t status;

    if (!start_descr_id || !descr_result){
    	return ESP_GATT_ILLEGAL_PARAMETER;
    }

	status = BTA_GATTC_GetNextCharDescr (conn_id, start_descr_id, descr_uuid_cond, descr_result);

	return status;
	
}


/*******************************************************************************
**
** @function        esp_ble_gattc_get_first_inclu_srvc
**
** @brief      		This function is called to find the first included service of the
**                  service on the given server.
**
** @param[in]       conn_id: connection ID which identify the server.
** @param[in]      	srvc_id: the service ID of which the characteristic is belonged to.
** @param[in]       uuid_cond: Characteristic UUID, if NULL find the first available
**                  characteristic.
** @param[in]       result: output parameter which will store the GATT ID
**                  of the included service found.
**
** @return          Command status code:
**  - @ref ESP_GATT_OK: If request succeeds
**  - @ref ESP_GATT_ILLEGAL_PARAMETER: If the srvc_id or result parameter is NULL.
**
*******************************************************************************/
esp_gatt_status_t  esp_ble_gattc_get_first_inclu_srvc (uint16_t conn_id, esp_gatt_srvc_id_t *srvc_id,
                                                    esp_bt_uuid_t *uuid_cond, esp_gattc_incl_srvc_id_t *result)
{
	
	esp_gatt_status_t status;
	
	if (!srvc_id || !result){
		return ESP_GATT_ILLEGAL_PARAMETER;
	}

	status = BTA_GATTC_GetFirstIncludedService(conn_id, srvc_id, uuid_cond, result);

	return status;
}


/*******************************************************************************
**
** @function        esp_ble_gattc_get_next_inclu_srvc
**
** @brief	      	This function is called to find the next included service of the
**                  service on the given server.
**
** @param[in]       conn_id: connection ID which identify the server.
** @param[in]       start_id: start the search from the next record
**                  after the one identified by p_start_id.
** @param[in]       uuid_cond: Included service UUID, if NULL find the first available
**                  included service.
** @param[in]       result: output parameter which will store the GATT ID
**                  of the included service found.
**
** @return          Command status code:
**  - @ref ESP_GATT_OK: If request succeeds
**  - @ref ESP_GATT_ILLEGAL_PARAMETER: If the p_char_id or p_descr_result parameter is NULL.

*******************************************************************************/
esp_gatt_status_t  esp_ble_gattc_get_next_inclu_srvc (uint16_t conn_id,
                                                   esp_gattc_incl_srvc_id_t *start_id,
                                                   esp_bt_uuid_t            *uuid_cond,
                                                   esp_gattc_incl_srvc_id_t *result)
{
	esp_gatt_status_t status;

    if (!start_id || !result){
    	return ESP_GATT_ILLEGAL_PARAMETER;
    }

	status = BTA_GATTC_GetNextIncludedService(conn_id, start_id, uuid_cond, result);

	return status;
}


/*******************************************************************************
**
** @function        esp_ble_gattc_read_char
**
** @brief      	    This function is called to read a service's characteristics of
**                  the given characteritisc ID.
**
** @param[in]       conn_id - connectino ID.
** @param[in]       char_id - characteritic ID to read.
**
** @return          None
**
*******************************************************************************/
void esp_ble_gattc_read_char (uint16_t conn_id, esp_gattc_char_id_t *char_id,
                                  esp_gatt_auth_req_t auth_req)
{
	 BTA_GATTC_ReadCharacteristic(conn_id, char_id, auth_req);
}


/*******************************************************************************
**
** @function        esp_ble_gattc_read_char_descr
**
** @brief	        This function is called to read a characteristics descriptor.
**
** @param[in]       conn_id - connection ID.
** @param[in]       descr_id - characteritic descriptor ID to read.
**
** @return          None
**
*******************************************************************************/
void esp_ble_gattc_read_char_descr (uint16_t conn_id,
                             							 esp_gattc_char_descr_id_t  *descr_id,
                              							 esp_gatt_auth_req_t auth_req)
{
	BTA_GATTC_ReadCharDescr (conn_id, descr_id, auth_req);
}


/*******************************************************************************
**
** @function        esp_ble_gattc_read_multi
**
** @brief      		This function is called to read multiple characteristic or
**                  characteristic descriptors.
**
** @param[in]       conn_id - connectino ID.
** @param[in]       read_multi - pointer to the read multiple parameter.
**
** @return          None
**
*******************************************************************************/
void esp_ble_gattc_read_multi (uint16_t conn_id, esp_gattc_multi_t *read_multi,
                            esp_gatt_auth_req_t auth_req)
{
	 BTA_GATTC_ReadMultiple(conn_id, read_multi, auth_req);
}


/*******************************************************************************
**
** @function        esp_ble_gattc_write_char_val
**
** @brief      		This function is called to write characteristic value.
**
** @param[in]       conn_id - connection ID.
** @param[in]       char_id - characteristic ID to write.
** @param[in]       write_type - type of write.
** @param[in]       len: length of the data to be written.
** @param[in]       value - the value to be written.
**
** @return          None
**
*******************************************************************************/
void esp_ble_gattc_write_char_val ( uint16_t conn_id,
                               							 esp_gattc_char_id_t *char_id,
                                						 esp_gattc_write_type_t  write_type,
                                						 uint16_t len,
                                						 uint8_t *value,
                                						 esp_gatt_auth_req_t auth_req)
{
	BTA_GATTC_WriteCharValue (conn_id, char_id, write_type, len, value, auth_req);
}


/*******************************************************************************
**
** @function        esp_ble_gattc_write_char_descr
**
** @brief	      	This function is called to write characteristic descriptor value.
**
** @param[in]       conn_id - connection ID
** @param[in]       char_descr_id - characteristic descriptor ID to write.
** @param[in]       write_type - write type.
** @param[in]       data - the value to be written.
**
** @return          None
**
*******************************************************************************/
void esp_ble_gattc_write_char_descr (uint16_t conn_id,
                               esp_gattc_char_descr_id_t *char_descr_id,
                               esp_gattc_write_type_t  write_type,
                               esp_gatt_unfmt_t      *data,
                               esp_gatt_auth_req_t auth_req)
{
	 BTA_GATTC_WriteCharDescr (conn_id, char_descr_id, write_type, data, auth_req);
}


/*******************************************************************************
**
** @function        esp_ble_gattc_prepa_write
**
** @brief      		This function is called to prepare write a characteristic value.
**
** @param[in]       conn_id - connection ID.
** @param[in]       char_id - GATT characteritic ID of the service.
** @param[in]       offset - offset of the write value.
** @param[in]       len: length of the data to be written.
** @param[in]       value - the value to be written.
**
** @return          None
**
*******************************************************************************/
void esp_ble_gattc_prepa_write (uint16_t conn_id, esp_gattc_char_id_t *char_id,
                              						uint16_t offset, uint16_t len, uint8_t *value,
                              						esp_gatt_auth_req_t auth_req)
{
	BTA_GATTC_PrepareWrite  (conn_id, char_id, offset, len, value, auth_req);
}


/*******************************************************************************
**
** @function        esp_ble_gattc_execu_write
**
** @brief           This function is called to execute write a prepare write sequence.
**
** @param[in]       conn_id - connection ID.
** @param[in]       is_execute - execute or cancel.
**
** @return          None
**
*******************************************************************************/
void  esp_ble_gattc_execu_write (uint16_t conn_id, BOOLEAN is_execute)
{
	BTA_GATTC_ExecuteWrite (conn_id, is_execute);
}


/*******************************************************************************
**
** @function        esp_ble_gattc_send_ind_cfm
**
** @brief           This function is called to send handle value confirmation.
**
** @param[in]       conn_id - connection ID.
** @param[in]       char_id - characteristic ID to confirm.
**
** @return          None
**
*******************************************************************************/
void esp_ble_gattc_send_ind_cfm (uint16_t conn_id, esp_gattc_char_id_t *char_id)
{
	BTA_GATTC_SendIndConfirm (conn_id, char_id);
}


/*******************************************************************************
**
** @function        esp_ble_gattc_register_ntf
**
** @brief           This function is called to register for notification of a service.
**
** @param[in]       client_if - client interface.
** @param[in]       bda - target GATT server.
** @param[in]       char_id - pointer to GATT characteristic ID.
**
** @return          OK if registration succeed, otherwise failed.
**
*******************************************************************************/
esp_gatt_status_t esp_ble_gattc_register_ntf (esp_gattc_if_t client_if,
                                                     BD_ADDR bda,
                                                     esp_gattc_char_id_t *char_id)
{
	esp_gatt_status_t    status = ESP_GATT_ILLEGAL_PARAMETER;
	status = BTA_GATTC_RegisterForNotifications (client_if, bda, char_id);

	return status;
}


/*******************************************************************************
**
** @function        esp_ble_gattc_unregister_ntf
**
** @brief           This function is called to de-register for notification of a service.
**
** @param[in]       client_if - client interface.
** @param[in]       bda - target GATT server.
** @param[in]       char_id - pointer to GATT characteristic ID.
**
** @return          OK if deregistration succeed, otherwise failed.
**
*******************************************************************************/
tBTA_GATT_STATUS esp_ble_gattc_unregister_ntf (esp_gattc_if_t client_if,
                                                       BD_ADDR bda,
                                                       esp_gattc_char_id_t *char_id)
{
	esp_gatt_status_t    status = ESP_GATT_ILLEGAL_PARAMETER;
	status = BTA_GATTC_DeregisterForNotifications (client_if, bda, char_id);

	return status;
}


/*******************************************************************************
**
** @function        esp_ble_gatts_disale
**
** @brief           This function is called to disable GATTS module
**
** @param[in]       None.
**
** @return          None
**
*******************************************************************************/
void esp_ble_gatts_disale(void)
{
	 BTA_GATTS_Disable();
}


/*******************************************************************************
**
** @function        esp_ble_gatts_app_register
**
** @brief           This function is called to register application callbacks
**                  with BTA GATTS module.
**
** @param[in]       app_uuid - applicaiton UUID
** @param[in]       cback - pointer to the application callback function.
**
** @return          None
**
*******************************************************************************/
void esp_ble_gatts_app_register(esp_bt_uuid_t *app_uuid, esp_gatts_cb_t *cback)
{
	BTA_GATTS_AppRegister(app_uuid, cback);
}


/*******************************************************************************
**
** @function        esp_ble_gatts_app_unregister
**
** @brief           un-register with GATT Server.
**
** @param[in]       server_if: service interface id.
**
** @return          None
**
*******************************************************************************/
void esp_ble_gatts_app_unregister(esp_gatts_if_t server_if)
{
	BTA_GATTS_AppDeregister(server_if);
}


/*******************************************************************************
**
** @function        esp_ble_gatts_create_srvc
**
** @brief      		Create a service. When service creation is done, a callback
**                  event BTA_GATTS_CREATE_SRVC_EVT is called to report status
**                  and service ID to the profile. The service ID obtained in
**                  the callback function needs to be used when adding included
**                  service and characteristics/descriptors into the service.
**
** @param[in]       server_if: Profile ID this service is belonged to.
** @param[in]       service_uuid: service UUID.
** @param[in]       inst: instance ID number of this service.
** @param[in]       num_handle: numble of handle requessted for this service.
** @param[in]       is_primary: is this service a primary one or not.
**
** @return          None
**
*******************************************************************************/
void esp_ble_gatts_create_srvc(esp_gatts_if_t server_if, esp_bt_uuid_t *service_uuid, uint8_t inst,
                             uint16_t num_handle, BOOLEAN is_primary)
{
	BTA_GATTS_CreateService(server_if, service_uuid, inst, num_handle, is_primary);
}


/*******************************************************************************
**
** @function        esp_ble_gatts_add_inclu_srvc
**
** @brief      		This function is called to add an included service. After included
**                  service is included, a callback event BTA_GATTS_ADD_INCL_SRVC_EVT
**                  is reported the included service ID.
**
** @param[in]       service_id: service ID to which this included service is to
**                  be added.
** @param[in]       included_service_id: the service ID to be included.
**
** @return          None
**
*******************************************************************************/
void esp_ble_gatts_add_inclu_srvc (uint16_t service_id, uint16_t included_service_id)
{
	BTA_GATTS_AddIncludeService(service_id, included_service_id);
}

/*******************************************************************************
**
** @function        esp_ble_gatts_add_char
**
** @brief           This function is called to add a characteristic into a service.
**
** @param[in]       service_id: service ID to which this included service is to
**                  be added.
** @param[in]       char_uuid : Characteristic UUID.
** @param[in]       perm      : Characteristic value declaration attribute permission.
** @param[in]       property  : Characteristic Properties
**
** @return             None
**
*******************************************************************************/
void esp_ble_gatts_add_char (uint16_t service_id,  esp_bt_uuid_t  *char_uuid,
                                  esp_gatt_perm_t perm, esp_gatt_char_prop_t property)
{
	BTA_GATTS_AddCharacteristic (service_id, char_uuid, perm, property);
}


/*******************************************************************************
**
** @function        esp_ble_gatts_add_char_descr
**
** @brief           This function is called to add characteristic descriptor. When
**                  it's done, a callback event BTA_GATTS_ADD_DESCR_EVT is called
**                  to report the status and an ID number for this descriptor.
**
** @param[in]       service_id: service ID to which this charatceristic descriptor is to
**                              be added.
** @param[in]       perm: descriptor access permission.
** @param[in]       descr_uuid: descriptor UUID.
**
** @return          None
**
*******************************************************************************/
void esp_ble_gatts_add_char_descr (uint16_t service_id,
                                  						esp_gatt_perm_t perm,
                                  						esp_bt_uuid_t  * descr_uuid)
{
	BTA_GATTS_AddCharDescriptor (service_id, perm, descr_uuid);
}


/*******************************************************************************
**
** @function        esp_ble_gatts_dele_srvc
**
** @brief           This function is called to delete a service. When this is done,
**                  a callback event BTA_GATTS_DELETE_EVT is report with the status.
**
** @param[in]       service_id: service_id to be deleted.
**
** @return          None
**
*******************************************************************************/
void esp_ble_gatts_dele_srvc (uint16_t service_id)
{
	 BTA_GATTS_DeleteService(service_id);
}


/*******************************************************************************
**
** @function        esp_ble_gatts_start_srvc
**
** @brief           This function is called to start a service.
**
** @param[in]       service_id: the service ID to be started.
** @param[in]       sup_transport: supported trasnport.
**
** @return          None
**
*******************************************************************************/
void esp_ble_gatts_start_srvc(uint16_t service_id)
{
	tBTA_GATT_TRANSPORT transport = BT_TRANSPORT_LE;
	BTA_GATTS_StartService(service_id, transport);
}

/*******************************************************************************
**
** @function        esp_ble_gatts_stop_srvc
**
** @brief           This function is called to stop a service.
**
** @param[in]       service_id - service to be topped.
**
** @return          None
**
*******************************************************************************/
void esp_ble_gatts_stop_srvc(uint16_t service_id)
{
	BTA_GATTS_StopService(service_id);
}


/*******************************************************************************
**
** @function        esp_ble_gatts_hdl_val_indica
**
** @brief           This function is called to read a characteristics descriptor.
**
** @param[in]       bda - remote device bd address to indicate.
** @param[in]	    attr_id - attribute ID to indicate.
** @param[in]       data_len - indicate data length.
** @param[in]       data: data to indicate.
** @param[in]       need_confirm - if this indication expects a confirmation or not.
**
** @return          None
**
*******************************************************************************/
void esp_ble_gatts_hdl_val_indica (uint16_t conn_id, uint16_t attr_id, uint16_t data_len,
                                      								uint8_t *data, BOOLEAN need_confirm)
{
	BTA_GATTS_HandleValueIndication (conn_id, attr_id, data_len, data, need_confirm);
}


/*******************************************************************************
**
** @function        esp_ble_gatts_send_rsp
**
** @brief           This function is called to send a response to a request.
**
** @param[in]       conn_id - connection identifier.
** @param[in]       trans_id - transaction ID.
** @param[in]       status - response status
** @param[in]       msg - response data.
**
** @return          None
**
*******************************************************************************/
void esp_ble_gatts_send_rsp (uint16_t conn_id, uint32_t trans_id,
                        			esp_gatt_status_t status, esp_gatts_rsp_t *msg)
{
	BTA_GATTS_SendRsp (conn_id, trans_id, status, msg);
}


/*******************************************************************************
**
** @function        esp_ble_gatts_conn
**
** @brief           Open a direct open connection or add a background auto connection
**                  bd address
**
** @param[in]       server_if: server interface.
** @param[in]       remote_bda: remote device BD address.
** @param[in]       is_direct: direct connection or background auto connection
**
** @return          None
**
*******************************************************************************/
void esp_ble_gatts_conn (esp_gatts_if_t server_if, BD_ADDR remote_bda, BOOLEAN is_direct)
{
	tBTA_GATT_TRANSPORT transport = BT_TRANSPORT_LE;
	BTA_GATTS_Open(server_if, remote_bda, is_direct, transport);
}


/*******************************************************************************
**
** @function         esp_ble_gatts_cancel_conn
**
** @brief           Cancel a direct open connection or remove a background auto connection
**                  bd address
**
** @param[in]       server_if: server interface.
** @param[in]       remote_bda: remote device BD address.
** @param[in]       is_direct: direct connection or background auto connection
**
** @return          None
**
*******************************************************************************/
void esp_ble_gatts_cancel_conn (esp_gatts_if_t server_if, BD_ADDR remote_bda, BOOLEAN is_direct)
{
	 BTA_GATTS_CancelOpen(server_if, remote_bda, is_direct);
}


/*******************************************************************************
**
** @function        esp_ble_gatts_close
**
** @param[in]       Close a connection  a remote device.
**
** @param[in]       conn_id: connectino ID to be closed.
**
** @return          None
**
*******************************************************************************/
void esp_ble_gatts_close(uint16_t conn_id)
{
	BTA_GATTS_Close(conn_id);
}


/*******************************************************************************
**
** @function        esp_prf_app_register
**
** @brief           This function is called to register application callbacks
**                    with BTA GATTS module.
**
** @param[in]       p_app_uuid - applicaiton UUID
** @param[in]       p_cback - pointer to the application callback function.
**
** @return          None
**
*******************************************************************************/
void esp_prf_app_register(uint8_t prf_id, void *p_cback)
{
    //tBTA_GATTS_API_REG  *p_buf;

    /* register with BTA system manager */
    if (bt_prf_sys_is_register(PRF_ID_SYS) == FALSE)
    {
        bt_prf_sys_register(PRF_ID_SYS, NULL);
    }

//    if ((p_buf = (tBTA_GATTS_API_REG *) GKI_getbuf(sizeof(tBTA_GATTS_API_REG))) != NULL)
//    {
//        p_buf->hdr.event    = BTA_GATTS_API_REG_EVT;

//        if (p_app_uuid != NULL)
//            memcpy(&p_buf->app_uuid, p_app_uuid, sizeof(tBT_UUID));
//        p_buf->p_cback      = p_cback;

//        bta_sys_sendmsg(p_buf);
//    }
    return;
}





