/**
 ****************************************************************************************
 *
 * @file bt_app.c
 *
 * @brief Application entry point
 *
 * Copyright (C) Espressif 2016
 * Created by Yulong at 2016/9/9
 *
 *
 ****************************************************************************************
 */

#include "bt_app_defs.h"


 /*******************************************************************************
 **
 ** Function		 ESP_AppConfigadvData
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
 void ESP_AppBleConfigadvData(tESP_BLE_ADV_DATA *adv_data,
												tBTA_SET_ADV_DATA_CMPL_CBACK *p_adv_data_cback)
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
** Function         ESP_BleSetScanRsp
**
** Description      This function is called to override the app scan response.
**
** Parameters       Pointer to User defined ADV data structure
**
** Returns          None
**
*******************************************************************************/
void ESP_AppBleSetScanRsp(tESP_BLE_ADV_DATA *scan_rsp_data,
												tBTA_SET_ADV_DATA_CMPL_CBACK *p_scan_rsp_data_cback)
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


