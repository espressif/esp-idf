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

#include "esp_bt_defs.h"


 /*******************************************************************************
 **
 ** Function		 esp_ble_config_adv_data
 **
 ** Description 	 This function is called to override the BTA default ADV parameters.
 **
 ** 				 adv_data: Pointer to User defined ADV data structure. This
 ** 							memory space can not be freed until p_adv_data_cback
 ** 							is received.
 ** 				 p_adv_data_cback: set adv data complete callback.
 **
 ** Returns 		 None
 **
 *******************************************************************************/
 void esp_ble_config_adv_data(esp_ble_adv_data_cfg_t *adv_data,
												esp_ble_set_adv_data_cmpl_cb_t *p_adv_data_cback)
{
	tBTA_BLE_AD_MASK data_mask = 0;
	if(adv_data->adv_name != NULL)
	{
		 data_mask |= BTM_BLE_AD_BIT_DEV_NAME;
		 BTA_DmSetDeviceName(adv_data->adv_name);
	}
	if(adv_data->ble_adv_data.int_range.low != 0 ||
	   adv_data->ble_adv_data.int_range.hi != 0)
		data_mask |= BTM_BLE_AD_BIT_INT_RANGE;
	
	if(adv_data->ble_adv_data.p_manu != NULL)
	{
		data_mask |= BTM_BLE_AD_BIT_MANU;
	}

	if(adv_data->ble_adv_data.p_services != NULL)
	{
		data_mask |= BTM_BLE_AD_BIT_SERVICE;
	}

	if(adv_data->ble_adv_data.p_service_32b != NULL)
	{
		data_mask |= BTM_BLE_AD_BIT_SERVICE_32;
	}

	if(adv_data->ble_adv_data.p_services_128b != NULL)
	{
		data_mask |= BTM_BLE_AD_BIT_SERVICE_128;
	}

	if(adv_data->ble_adv_data.p_sol_services != NULL)
	{
		data_mask |= BTM_BLE_AD_BIT_SERVICE_SOL;
	}

	if(adv_data->ble_adv_data.p_sol_service_32b != NULL)
	{
		data_mask |= BTM_BLE_AD_BIT_SERVICE_32SOL;
	}

	if(adv_data->ble_adv_data.p_sol_service_128b != NULL)
	{
		data_mask |= BTM_BLE_AD_BIT_SERVICE_128SOL;
	}
	
	if(adv_data->ble_adv_data.p_service_data != NULL)
	{
		data_mask |= BTM_BLE_AD_BIT_SERVICE_DATA;
	}
	
	if(adv_data->ble_adv_data.appearance != 0)
	{
		data_mask |= BTM_BLE_AD_BIT_APPEARANCE;
	}

	if(adv_data->ble_adv_data.p_proprietary != NULL)
	{
		data_mask |= BTM_BLE_AD_BIT_PROPRIETARY;
	}

	if(adv_data->ble_adv_data.tx_power != 0)
	{
		data_mask |= BTM_BLE_AD_BIT_TX_PWR;
	}

	BTA_DmBleSetAdvConfig(data_mask, &(adv_data->ble_adv_data), p_adv_data_cback);
}


/*******************************************************************************
**
** Function         esp_ble_set_scan_rsp
**
** Description      This function is called to override the app scan response.
**
** Parameters       Pointer to User defined ADV data structure
**
** Returns          None
**
*******************************************************************************/
void esp_ble_set_scan_rsp(esp_ble_adv_data_cfg_t *scan_rsp_data,
												 esp_ble_set_adv_data_cmpl_cb_t *p_scan_rsp_data_cback)
{
	tBTA_BLE_AD_MASK data_mask = 0;
	if(scan_rsp_data->adv_name != NULL)
	{
		 data_mask |= BTM_BLE_AD_BIT_DEV_NAME;
		 BTA_DmSetDeviceName(scan_rsp_data->adv_name);
	}
	if(scan_rsp_data->ble_adv_data.int_range.low != 0 || 
	   scan_rsp_data->ble_adv_data.int_range.hi != 0)
		data_mask |= BTM_BLE_AD_BIT_INT_RANGE;
	
	if(scan_rsp_data->ble_adv_data.p_manu != NULL)
	{
		data_mask |= BTM_BLE_AD_BIT_MANU;
	}

	if(scan_rsp_data->ble_adv_data.p_services != NULL)
	{
		data_mask |= BTM_BLE_AD_BIT_SERVICE;
	}

	if(scan_rsp_data->ble_adv_data.p_service_32b != NULL)
	{
		data_mask |= BTM_BLE_AD_BIT_SERVICE_32;
	}

	if(scan_rsp_data->ble_adv_data.p_services_128b != NULL)
	{
		data_mask |= BTM_BLE_AD_BIT_SERVICE_128;
	}

	if(scan_rsp_data->ble_adv_data.p_sol_services != NULL)
	{
		data_mask |= BTM_BLE_AD_BIT_SERVICE_SOL;
	}

	if(scan_rsp_data->ble_adv_data.p_sol_service_32b != NULL)
	{
		data_mask |= BTM_BLE_AD_BIT_SERVICE_32SOL;
	}

	if(scan_rsp_data->ble_adv_data.p_sol_service_128b != NULL)
	{
		data_mask |= BTM_BLE_AD_BIT_SERVICE_128SOL;
	}
	
	if(scan_rsp_data->ble_adv_data.p_service_data != NULL)
	{
		data_mask |= BTM_BLE_AD_BIT_SERVICE_DATA;
	}
	
	if(scan_rsp_data->ble_adv_data.appearance != 0)
	{
		data_mask |= BTM_BLE_AD_BIT_APPEARANCE;
	}

	if(scan_rsp_data->ble_adv_data.p_proprietary != NULL)
	{
		data_mask |= BTM_BLE_AD_BIT_PROPRIETARY;
	}

	if(scan_rsp_data->ble_adv_data.tx_power != 0)
	{
		data_mask |= BTM_BLE_AD_BIT_TX_PWR;
	}

	BTA_DmBleSetScanRsp(data_mask, &(scan_rsp_data->ble_adv_data), p_scan_rsp_data_cback);
}

#if 0 

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
#endif

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



