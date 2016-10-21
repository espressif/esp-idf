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





