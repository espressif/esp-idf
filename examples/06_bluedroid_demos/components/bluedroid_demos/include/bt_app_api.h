/**
 ****************************************************************************************
 *
 * @file bt_app_api.h
 *
 * @brief Application entry point
 *
 * Copyright (C) Espressif 2016
 * Created by Yulong at 2016/10/13
 *
 *
 ****************************************************************************************
 */

#include "bt_types.h"
#include "bt_app_defs.h"

typedef tBTA_SET_ADV_DATA_CMPL_CBACK tAPI_SET_ADV_DATA_CMPL_CBACK  ;
typedef	tBTA_STATUS tAPI_STATUS;

extern void API_Ble_AppConfigAdvData(tESP_BLE_ADV_DATA *adv_data,
												tAPI_SET_ADV_DATA_CMPL_CBACK *p_adv_data_cback);

extern void API_Ble_SetScanRsp (tBTA_BLE_AD_MASK data_mask, tBTA_BLE_ADV_DATA *p_adv_cfg,
                                 tBTA_SET_ADV_DATA_CMPL_CBACK *p_adv_data_cback);

extern void API_Ble_SetBleScanParams(tGATT_IF client_if, UINT32 scan_interval,
                            		UINT32 scan_window, tBLE_SCAN_MODE scan_mode,
                            		tBLE_SCAN_PARAM_SETUP_CBACK scan_param_setup_cback);


extern void API_Ble_AppStartAdvertising(tESP_API_BLE_ADV_PARAMS_ALL *ble_adv_params);

extern void API_Ble_AppStopAdvertising(void);

extern void API_Ble_AppUpdateConnectionParams(BD_ADDR bd_addr, UINT16 min_int, 
												UINT16 max_int, UINT16 latency, UINT16 timeout);

extern void API_Ble_SetPacketDataLength(BD_ADDR remote_device, UINT16 tx_data_length);

extern void API_Ble_SetRandAddress(BD_ADDR rand_addr);

extern void API_Ble_ConfigLocalPrivacy(BOOLEAN privacy_enable);											


void API_Ble_PrfEnable(); 
