// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "blufi_adv.h"
#include "esp_adv_api.h"

 /*******************************************************************************
 **
 ** Function		 BlufiConfigadvData
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
 void BlufiBleConfigadvData(esp_ble_adv_data_cfg_t *adv_data,
												esp_ble_set_adv_data_cmpl_cb_t *p_adv_data_cback)
{
	esp_ble_config_adv_data(adv_data, p_adv_data_cback);
}


/*******************************************************************************
**
** Function         BLUFI_BleSetScanRsp
**
** Description      This function is called to override the app scan response.
**
** Parameters       Pointer to User defined ADV data structure
**
** Returns          None
**
*******************************************************************************/
void BlufiBleSetScanRsp(esp_ble_adv_data_cfg_t *scan_rsp_data,
												esp_ble_set_adv_data_cmpl_cb_t *p_scan_rsp_data_cback)
{
	esp_ble_set_scan_rsp(scan_rsp_data, p_scan_rsp_data_cback);
}


