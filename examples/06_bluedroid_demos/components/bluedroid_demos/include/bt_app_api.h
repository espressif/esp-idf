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
#include "bta_gatt_api.h"

typedef tBTA_SET_ADV_DATA_CMPL_CBACK tAPI_SET_ADV_DATA_CMPL_CBACK ;
typedef	tBTA_STATUS tAPI_STATUS;

extern void API_Ble_AppConfigAdvData (tESP_BLE_ADV_DATA *adv_data,
												     tAPI_SET_ADV_DATA_CMPL_CBACK *p_adv_data_cback);

extern void API_Ble_SetScanRsp (tBTA_BLE_AD_MASK data_mask, tBTA_BLE_ADV_DATA *p_adv_cfg,
                                            tBTA_SET_ADV_DATA_CMPL_CBACK *p_adv_data_cback);


extern void API_Ble_SetScanParams (tESP_BLE_SCAN_PARAMS *scan_params, tGATT_IF client_if,
                            		            tBLE_SCAN_PARAM_SETUP_CBACK scan_param_setup_cback);

extern void API_Ble_StartScanning (UINT8 duration, tBTA_DM_SEARCH_CBACK *p_results_cb);


extern void API_Ble_AppStartAdvertising (tESP_API_BLE_ADV_PARAMS_ALL *ble_adv_params);

extern void API_Ble_AppStopAdvertising (void);

extern void API_Ble_AppUpdateConnectionParams (BD_ADDR bd_addr, UINT16 min_int, 
												UINT16 max_int, UINT16 latency, UINT16 timeout);

extern void API_Ble_SetPacketDataLength (BD_ADDR remote_device, UINT16 tx_data_length);

extern void API_Ble_SetRandAddress (BD_ADDR rand_addr);

extern void API_Ble_ConfigLocalPrivacy (BOOLEAN privacy_enable);											

extern void API_Ble_GattcAppRegister (tBT_UUID *p_app_uuid, tBTA_GATTC_CBACK *p_client_cb);

extern void API_Ble_GattcAppDeregister (tBTA_GATTC_IF client_if);

extern void API_Ble_GattcOpen (tBTA_GATTC_IF client_if, BD_ADDR remote_bda,
                    BOOLEAN is_direct, tBTA_GATT_TRANSPORT transport);

extern void API_GattcCancelOpen (tBTA_GATTC_IF client_if, BD_ADDR remote_bda, BOOLEAN is_direct);

extern void API_Ble_GattcClose (UINT16 conn_id);

extern void API_Ble_GattcConfigureMTU (UINT16 conn_id, UINT16 mtu);

extern void API_Ble_GattcServiceSearchRequest (UINT16 conn_id, tBT_UUID *p_srvc_uuid);

extern tBTA_GATT_STATUS  API_Ble_GattcGetFirstChar (UINT16 conn_id, tBTA_GATT_SRVC_ID *p_srvc_id,
                                          						   tBT_UUID *p_char_uuid_cond,
                                          						   tBTA_GATTC_CHAR_ID *p_char_result,
                                          						   tBTA_GATT_CHAR_PROP *p_property);

extern tBTA_GATT_STATUS  API_Ble_GattcGetNextChar (UINT16 conn_id,
                                         tBTA_GATTC_CHAR_ID *p_start_char_id,
                                         tBT_UUID           *p_char_uuid_cond,
                                         tBTA_GATTC_CHAR_ID *p_char_result,
                                         tBTA_GATT_CHAR_PROP    *p_property);

extern tBTA_GATT_STATUS  API_Ble_GattcGetNextCharDescr (UINT16 conn_id,
                                             tBTA_GATTC_CHAR_DESCR_ID *p_start_descr_id,
                                             tBT_UUID           *p_descr_uuid_cond,
                                             tBTA_GATTC_CHAR_DESCR_ID *p_descr_result);

extern tBTA_GATT_STATUS  BTA_GATTC_GetFirstIncludedService (UINT16 conn_id, tBTA_GATT_SRVC_ID *p_srvc_id,
                                                    tBT_UUID *p_uuid_cond, tBTA_GATTC_INCL_SVC_ID *p_result);

extern void API_Ble_GattcReadCharacteristic (UINT16 conn_id, tBTA_GATTC_CHAR_ID *p_char_id,
                                  tBTA_GATT_AUTH_REQ auth_req);

extern void API_Ble_GattcReadCharDescr (UINT16 conn_id,
                             							 tBTA_GATTC_CHAR_DESCR_ID  *p_descr_id,
                              							 tBTA_GATT_AUTH_REQ auth_req);

extern void API_Ble_GattcReadMultiple (UINT16 conn_id, tBTA_GATTC_MULTI *p_read_multi,
                            tBTA_GATT_AUTH_REQ auth_req);

extern void API_Ble_GattcWriteCharValue ( UINT16 conn_id,
                               							 tBTA_GATTC_CHAR_ID *p_char_id,
                                						 tBTA_GATTC_WRITE_TYPE  write_type,
                                						 UINT16 len,
                                						 UINT8 *p_value,
                                						 tBTA_GATT_AUTH_REQ auth_req);

extern void API_Ble_GattcWriteCharDescr (UINT16 conn_id,
                               tBTA_GATTC_CHAR_DESCR_ID *p_char_descr_id,
                               tBTA_GATTC_WRITE_TYPE  write_type,
                               tBTA_GATT_UNFMT      *p_data,
                               tBTA_GATT_AUTH_REQ auth_req);

extern void API_Ble_GattcPrepareWrite (UINT16 conn_id, tBTA_GATTC_CHAR_ID *p_char_id,
                              						UINT16 offset, UINT16 len, UINT8 *p_value,
                              						tBTA_GATT_AUTH_REQ auth_req);

extern void  API_Ble_GattcExecuteWrite (UINT16 conn_id, BOOLEAN is_execute);

extern void API_Ble_GattcSendIndConfirm (UINT16 conn_id, tBTA_GATTC_CHAR_ID *p_char_id);

extern tBTA_GATT_STATUS API_Ble_GattcRegisterForNotifications (tBTA_GATTC_IF client_if,
                                                     BD_ADDR bda,
                                                     tBTA_GATTC_CHAR_ID *p_char_id);

extern tBTA_GATT_STATUS API_Ble_GattcDeregisterForNotifications (tBTA_GATTC_IF client_if,
                                                       BD_ADDR bda,
                                                       tBTA_GATTC_CHAR_ID *p_char_id);



void API_Ble_PrfEnable (void); 
