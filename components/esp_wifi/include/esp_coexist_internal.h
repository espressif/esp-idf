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
 * @brief Enable software coexist
 *        extern function for internal use.
 *
 * @return Enable ok or failed.
 */
esp_err_t coex_enable(void);

/**
 * @brief Disable software coexist
 *        extern function for internal use.
 */
void coex_disable(void);

/**
 * @brief Get software coexist version string
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
 * @brief Get software coexist status.
 * @return : software coexist status
 */
uint32_t coex_status_get(void);

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
 * @brief Set WiFi channel to coexistence module.
 *
 *  @param primary : WiFi primary channel
 *  @param secondary : WiFi secondary channel
 *  @return : 0 - success, other - failed
 */
int coex_wifi_channel_set(uint8_t primary, uint8_t secondary);

/**
 * @brief Get coexistence event duration.
 *
 *  @param event : Coexistence event
 *  @param duration: Coexistence event duration 
 *  @return : 0 - success, other - failed
 */
int coex_event_duration_get(uint32_t event, uint32_t *duration);

/**
 * @brief Clear coexistence status.
 *
 *  @param type : Coexistence status type
 *  @param status: Coexistence status
 */
void coex_schm_status_bit_clear(uint32_t type, uint32_t status);

/**
 * @brief Set coexistence status.
 *
 *  @param type : Coexistence status type
 *  @param status: Coexistence status
 */
void coex_schm_status_bit_set(uint32_t type, uint32_t status);

/**
 * @brief Set coexistence scheme interval.
 *
 *  @param interval : Coexistence scheme interval 
 *  @return : 0 - success, other - failed
 */
int coex_schm_interval_set(uint32_t interval);

/**
 * @brief Get coexistence scheme interval.
 *
 *  @return : Coexistence scheme interval
 */
uint32_t coex_schm_interval_get(void);

/**
 * @brief Get current coexistence scheme period.
 *
 *  @return : Coexistence scheme period 
 */
uint8_t coex_schm_curr_period_get(void);

/**
 * @brief Get current coexistence scheme phase.
 *
 *  @return : Coexistence scheme phase 
 */
void * coex_schm_curr_phase_get(void);

/**
 * @brief Set current coexistence scheme phase index.
 *
 *  @param interval : Coexistence scheme phase index 
 *  @return : 0 - success, other - failed
 */
int coex_schm_curr_phase_idx_set(int idx);

/**
 * @brief Get current coexistence scheme phase index.
 *
 *  @return : Coexistence scheme phase index
 */
int coex_schm_curr_phase_idx_get(void);

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
