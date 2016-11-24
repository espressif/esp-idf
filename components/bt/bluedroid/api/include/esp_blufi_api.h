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

#ifndef __ESP_BLUFI_API_H__
#define __ESP_BLUFI_API_H__

#include "bt_types.h"
#include "esp_bt_defs.h"
#include "esp_gatt_defs.h"
#include "bta_gatt_api.h"
#include "esp_err.h"

#define ESP_BLUFI_RECV_DATA_LEN_MAX	128

#define ESP_BLUFI_EVENT_INIT_FINISH			0
#define ESP_BLUFI_EVENT_DEINIT_FINISH		1
#define ESP_BLUFI_EVENT_RECV_DATA			2

typedef enum {
	ESP_BLUFI_CONFIG_OK = 0,
	ESP_BLUFI_CONFIG_FAILED,
} esp_blufi_config_state_t;

typedef enum {
	ESP_BLUFI_INIT_OK = 0,
	ESP_BLUFI_INIT_FAILED = 0,
} esp_blufi_init_state_t;

typedef enum {
	ESP_BLUFI_DEINIT_OK = 0,
	ESP_BLUFI_DEINIT_FAILED = 0,
} esp_blufi_deinit_state_t;

typedef union {
	//ESP_BLUFI_EVENT_INIT_FINISH
	struct blufi_init_finish_evt_param {
		esp_blufi_init_state_t state;
	} init_finish;
	//ESP_BLUFI_EVENT_DEINIT_FINISH
	struct blufi_deinit_finish_evt_param {
		esp_blufi_deinit_state_t state;
	} deinit_finish;
	//ESP_BLUFI_EVENT_RECV_DATA
	struct blufi_recv_evt_param {
		uint8_t data[ESP_BLUFI_RECV_DATA_LEN_MAX];
		uint8_t data_len;
	} recv_data;
} esp_blufi_cb_param_t;

/*******************************************************************************
**
** @function        esp_blufi_register_callback
**
** @brief           This function is called to receive blufi callback event
**
** @param[in]       callback: callback function
**
** @return          ESP_OK - success, other - failed
**
*******************************************************************************/
esp_err_t esp_blufi_register_callback(esp_profile_cb_t callback);

/*******************************************************************************
**
** @function        esp_blufi_send_config_state
**
** @brief           This function is called to send config state to phone
**
** @param[in]       state: blufi config ok or not
**
** @return          ESP_OK - success, other - failed
**
*******************************************************************************/
esp_err_t esp_blufi_send_config_state(esp_blufi_config_state_t state);

/*******************************************************************************
**
** @function        esp_blufi_profile_init
**
** @brief           This function is called to init blufi_profile
**
** @return          ESP_OK - success, other - failed
**
*******************************************************************************/
esp_err_t esp_blufi_profile_init(void);

/*******************************************************************************
**
** @function        esp_blufi_profile_deinit
**
** @brief           This function is called to init blufi_profile
**
** @return          ESP_OK - success, other - failed
**
*******************************************************************************/
esp_err_t esp_blufi_profile_deinit(void);



#endif /* _ESP_BLUFI_API_ */
