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

#ifndef __BTC_GATT_UTIL_H__
#define __BTC_GATT_UTIL_H__

#include "bt_types.h"
#include "bta_gatt_api.h"
#include "esp_bt_defs.h"
#include "esp_gatt_defs.h"
#include "esp_gattc_api.h"

void btc128_to_bta_uuid(tBT_UUID *p_dest, uint8_t *p_src);
void btc_to_bta_uuid(tBT_UUID *p_dest, esp_bt_uuid_t *p_src);
void btc_to_bta_gatt_id(tBTA_GATT_ID *p_dest, esp_gatt_id_t *p_src);
void btc_to_bta_srvc_id(tBTA_GATT_SRVC_ID *p_dest, esp_gatt_srvc_id_t *p_src);
void btc_to_bta_response(tBTA_GATTS_RSP *rsp_struct, esp_gatt_rsp_t *p_rsp);

void bta_to_btc_uuid(esp_bt_uuid_t *p_dest, tBT_UUID *p_src);
void bta_to_btc_gatt_id(esp_gatt_id_t *p_dest, tBTA_GATT_ID *p_src);
void bta_to_btc_srvc_id(esp_gatt_srvc_id_t *p_dest, tBTA_GATT_SRVC_ID *p_src);

uint16_t set_read_value(esp_ble_gattc_cb_param_t *p_dest, tBTA_GATTC_READ *p_src);

#endif /* __BTC_GATT_UTIL_H__*/
