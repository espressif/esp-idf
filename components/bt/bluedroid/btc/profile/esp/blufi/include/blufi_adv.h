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

#ifndef __BLUFI_ADV_H__
#define __BLUFI_ADV_H__

#include "bta_api.h"
#include "btm_ble_api.h"
#include "esp_bt_defs.h"

typedef enum {
    BLE_ADV_DATA_IDX    =   0,
    BLE_SCAN_RSP_DATA_IDX = 1,
    ADV_SCAN_IDX_MAX,
} ADV_SCAN_IDX_t;

typedef struct {
    char    *adv_name;              //set the device name to be sent on the advertising
    tBTA_BLE_ADV_DATA ble_adv_data;
} tBLUFI_BLE_ADV_DATA;

extern void BlufiBleConfigadvData(tBLUFI_BLE_ADV_DATA *adv_data,
                                  tBTA_SET_ADV_DATA_CMPL_CBACK *p_adv_data_cback);

extern void BlufiBleSetScanRsp(tBLUFI_BLE_ADV_DATA *scan_rsp_data,
                               tBTA_SET_ADV_DATA_CMPL_CBACK *p_scan_rsp_data_cback);

#endif /* __BLUFI_ADV_H__ */
