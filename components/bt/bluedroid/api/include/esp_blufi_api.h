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

#include "esp_bt_defs.h"
#include "esp_gatt_defs.h"
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ESP_BLUFI_RECV_DATA_LEN_MAX (64+1)

#define ESP_BLUFI_EVENT_INIT_FINISH         0
#define ESP_BLUFI_EVENT_DEINIT_FINISH       1
#define ESP_BLUFI_EVENT_RECV_DATA           2

/// BLUFI config status
typedef enum {
    ESP_BLUFI_CONFIG_OK = 0,
    ESP_BLUFI_CONFIG_FAILED,
} esp_blufi_config_state_t;

/// BLUFI init status
typedef enum {
    ESP_BLUFI_INIT_OK = 0,
    ESP_BLUFI_INIT_FAILED = 0,
} esp_blufi_init_state_t;

/// BLUFI deinit status
typedef enum {
    ESP_BLUFI_DEINIT_OK = 0,
    ESP_BLUFI_DEINIT_FAILED = 0,
} esp_blufi_deinit_state_t;

/**
 * @brief BLUFI callback parameters union 
 */
typedef union {
    /**
	 * @brief ESP_BLUFI_EVENT_INIT_FINISH
	 */
    struct blufi_init_finish_evt_param {
        esp_blufi_init_state_t state;				/*!< Initial status */
    } init_finish;									/*!< Blufi callback param of ESP_BLUFI_EVENT_INIT_FINISH */

    /**
	 * @brief ESP_BLUFI_EVENT_DEINIT_FINISH
	 */
    struct blufi_deinit_finish_evt_param {
        esp_blufi_deinit_state_t state;				/*!< De-initial status */
    } deinit_finish;								/*!< Blufi callback param of ESP_BLUFI_EVENT_DEINIT_FINISH */

    /**
     * @brief ESP_BLUFI_EVENT_RECV_DATA
	 */
    struct blufi_recv_evt_param {
        uint8_t data[ESP_BLUFI_RECV_DATA_LEN_MAX];	/*!< Blufi receive data */
        uint8_t data_len;							/*!< Blufi receive data length */
    } recv_data;									/*!< Blufi callback param of ESP_BLUFI_EVENT_RECV_DATA */
} esp_blufi_cb_param_t;

/**
 *
 * @brief           This function is called to receive blufi callback event
 *
 * @param[in]       callback: callback function
 *
 * @return          ESP_OK - success, other - failed
 *
 */
esp_err_t esp_blufi_register_callback(esp_profile_cb_t callback);

/**
 *
 * @brief           This function is called to send config state to phone
 *
 * @param[in]       state: blufi config OK or not
 *
 * @return          ESP_OK - success, other - failed
 *
 */
esp_err_t esp_blufi_send_config_state(esp_blufi_config_state_t state);

/**
 *
 * @brief           This function is called to initialize blufi_profile
 *
 * @return          ESP_OK - success, other - failed
 *
 */
esp_err_t esp_blufi_profile_init(void);

/**
 *
 * @brief           This function is called to de-initialize blufi_profile
 *
 * @return          ESP_OK - success, other - failed
 *
 */
esp_err_t esp_blufi_profile_deinit(void);

#ifdef __cplusplus
}
#endif

#endif /* _ESP_BLUFI_API_ */
