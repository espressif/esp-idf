/*
 * SPDX-FileCopyrightText: 2017-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef H_BLE_HTP_CENT_
#define H_BLE_HTP_CENT_

#include "modlog/modlog.h"
#include "esp_central.h"
#ifdef __cplusplus
extern "C" {
#endif

struct ble_hs_adv_fields;
struct ble_gap_conn_desc;
struct ble_hs_cfg;
union ble_store_value;
union ble_store_key;

/* 16 Bit Health Thermometer Service UUID */
#define BLE_SVC_HTP_UUID16                                   0x1809

/* 16 Bit Health Thermometer Service Characteristic UUIDs */
#define BLE_SVC_HTP_CHR_UUID16_TEMP_MEASUREMENT              0x2A1C
#define BLE_SVC_HTP_CHR_UUID16_TEMP_TYPE                     0x2A1D
#define BLE_SVC_HTP_CHR_UUID16_INTERMEDIATE_TEMP             0x2A1E
#define BLE_SVC_HTP_CHR_UUID16_MEASUREMENT_ITVL              0x2A21

/* 16 Bit Device Information Service Characteristic UUIDs */
#define GATT_DIS_DEVICE_INFO_UUID                            0x180A
#define GATT_DIS_CHR_UUID16_SYS_ID                           0x2A23
#define GATT_DIS_CHR_UUID16_MODEL_NO                         0x2A24
#define GATT_DIS_CHR_UUID16_MFC_NAME                         0x2A29

/* 16 BIT CCCD UUID */
#define BLE_SVC_HTP_DSC_CLT_CFG_UUID16                       0x2902

#ifdef __cplusplus
}
#endif

#endif
