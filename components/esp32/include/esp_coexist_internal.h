// Copyright 2018-2018 Espressif Systems (Shanghai) PTE LTD
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

#ifndef __ESP_COEXIST_INTERNAL_H__
#define __ESP_COEXIST_INTERNAL_H__

#include <stdbool.h>
#include "esp_coexist_adapter.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    COEX_PREFER_WIFI = 0,
    COEX_PREFER_BT,
    COEX_PREFER_BALANCE,
    COEX_PREFER_NUM,
} coex_prefer_t;

typedef void (* coex_func_cb_t)(uint32_t event, int sched_cnt);

typedef enum {
    COEX_SCHM_ST_TYPE_WIFI = 0,
    COEX_SCHM_ST_TYPE_BLE,
    COEX_SCHM_ST_TYPE_BT,
} coex_schm_st_type_t;

#define COEX_SCHM_BLE_ST_IDLE               0x0
#define COEX_SCHM_BLE_ST_ADV                0x01
#define COEX_SCHM_BLE_ST_SCAN               0x02
#define COEX_SCHM_BLE_ST_CONNECTED          0x04
#define COEX_SCHM_BLE_ST_MESH_CONFIG        0x08
#define COEX_SCHM_BLE_ST_MESH_TRAFFIC       0x10
#define COEX_SCHM_BLE_ST_MESH_STANDBY       0x20

#define COEX_SCHM_BT_ST_IDLE                0x0
#define COEX_SCHM_BT_ST_ISCAN               0x01
#define COEX_SCHM_BT_ST_INQ                 0x02
#define COEX_SCHM_BT_ST_ACL_CONNECTED       0x04
#define COEX_SCHM_BT_ST_SNIFF               0x08
#define COEX_SCHM_BT_ST_A2DP_STREAMING      0x10
#define COEX_SCHM_BT_ST_A2DP_PAUSED         0x20

/**
 * @brief Pre-Init software coexist
 *        extern function for internal use.
 *
 * @return Init ok or failed.
 */
esp_err_t coex_pre_init(void);

/**
 * @brief Init software coexist
 *        extern function for internal use.
 *
 * @return Init ok or failed.
 */
esp_err_t coex_init(void);

/**
 * @brief De-init software coexist
 *        extern function for internal use.
 */
void coex_deinit(void);

/**
 * @brief Pause software coexist
 *        extern function for internal use.
 */
void coex_pause(void);

/**
 * @brief Resume software coexist
 *        extern function for internal use.
 */
void coex_resume(void);

/** * @brief Get software coexist version string
 *        extern function for internal use.
 * @return : version string
 */
const char *coex_version_get(void);

/**
 * @brief Coexist performance preference set from libbt.a
 *        extern function for internal use.
 *
 *  @param prefer : the prefer enumeration value
 *  @return : ESP_OK - success, other - failed
 */
esp_err_t coex_preference_set(coex_prefer_t prefer);

/**
 * @brief Set coexist wifi_percent for internal
 *  Default is 50%. The range is 10% <= wifi_percent <= 90%.
 *
 *  @param prefer : percent without %. Eg: 70 means 70%
 *  @return : ESP_OK - success, other - failed
 */
esp_err_t coex_wifi_percent_set(int wifi_percent);

/**
 * @brief Get software coexist status.
 * @return : software coexist status
 */
uint32_t coex_status_get(void);

/**
 * @brief Set coex schm status
 * @param type : WIFI/BLE/BT
 * @param status : WIFI/BLE/BT STATUS
 * @return : ESP_OK - success, other - failed
 */
esp_err_t coex_schm_status_set(coex_schm_st_type_t type, uint32_t status);

/**
 * @brief Get coex schm status
 * @param type : WIFI/BLE/BT
 * @return : status
 */
uint32_t coex_schm_status_get(coex_schm_st_type_t type);

/**
 * @brief Set software coexist condition.
 * @return : software coexist condition
 */
void coex_condition_set(uint32_t type, bool dissatisfy);

/**
 * @brief WiFi requests coexistence.
 *
 *  @param event : WiFi event
 *  @param latency : WiFi will request coexistence after latency
 *  @param duration : duration for WiFi to request coexistence
 *  @return : 0 - success, other - failed
 */
int coex_wifi_request(uint32_t event, uint32_t latency, uint32_t duration);

/**
 * @brief WiFi release coexistence.
 *
 *  @param event : WiFi event
 *  @return : 0 - success, other - failed
 */
int coex_wifi_release(uint32_t event);

/**
 * @brief Blue tooth requests coexistence.
 *
 *  @param event : blue tooth event
 *  @param latency : blue tooth will request coexistence after latency
 *  @param duration : duration for blue tooth to request coexistence
 *  @return : 0 - success, other - failed
 */
int coex_bt_request(uint32_t event, uint32_t latency, uint32_t duration);

/**
 * @brief Blue tooth release coexistence.
 *
 *  @param event : blue tooth event
 *  @return : 0 - success, other - failed
 */
int coex_bt_release(uint32_t event);

/**
 * @brief Register callback function for blue tooth.
 *
 *  @param cb : callback function
 *  @return : 0 - success, other - failed
 */
int coex_register_bt_cb(coex_func_cb_t cb);

/**
 * @brief Lock before reset base band.
 *
 *  @return : lock value
 */
uint32_t coex_bb_reset_lock(void);

/**
 * @brief Unlock after reset base band.
 *
 *  @param restore : lock value
 */
void coex_bb_reset_unlock(uint32_t restore);

/**
 * @brief Register coexistence adapter functions.
 *
 *  @param funcs : coexistence adapter functions
 *  @return : ESP_OK - success, other - failed
 */
esp_err_t esp_coex_adapter_register(coex_adapter_funcs_t *funcs);

/**
  * @brief     Check the MD5 values of the coexistence adapter header files in IDF and WiFi library
  *
  * @attention 1. It is used for internal CI version check
  *
  * @return
  *     - ESP_OK : succeed
  *     - ESP_WIFI_INVALID_ARG : MD5 check fail
  */
esp_err_t esp_coex_adapter_funcs_md5_check(const char *md5);

#ifdef __cplusplus
}
#endif

#endif /* __ESP_COEXIST_INTERNAL_H__ */
