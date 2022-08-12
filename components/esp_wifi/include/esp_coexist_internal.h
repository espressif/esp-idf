/*
 * SPDX-FileCopyrightText: 2018-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __ESP_COEXIST_INTERNAL_H__
#define __ESP_COEXIST_INTERNAL_H__

#include <stdbool.h>
#include "esp_coexist.h"
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
typedef esp_err_t (* coex_set_lpclk_source_callback_t)(void);

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
 * @brief Register application callback function to Wi-Fi update low power clock module.
 *
 * @param callback : Wi-Fi update low power clock callback function
 */
void coex_wifi_register_update_lpclk_callback(coex_set_lpclk_source_callback_t callback);

/**
 * @brief Update low power clock interval
 */
void coex_update_lpclk_interval(void);

/**
 * @brief Get coexistence event duration.
 *
 *  @param event : Coexistence event
 *  @param duration: Coexistence event duration
 *  @return : 0 - success, other - failed
 */
int coex_event_duration_get(uint32_t event, uint32_t *duration);

#if SOC_COEX_HW_PTI
/**
 * @brief Get coexistence event priority.
 *
 *  @param event : Coexistence event
 *  @param pti: Coexistence event priority
 *  @return : 0 - success, other - failed
 */
int coex_pti_get(uint32_t event, uint8_t *pti);
#endif

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

#if CONFIG_EXTERNAL_COEX_ENABLE
/**
  * @brief     Set external coexistence pti level and enable it.
  *
  * @param     level1    external coex low pti
  * @param     level2    external coex mid pti
  * @param     level3    external coex high pti
  *
  * @return
  *    - ESP_OK: succeed
  */
esp_err_t esp_coex_external_set(esp_coex_pti_level_t level1,
         esp_coex_pti_level_t level2, esp_coex_pti_level_t level3);

/**
  * @brief     Disable external coexist
  *
  * @return
  *    - ESP_OK: succeed
  */
void esp_coex_external_stop(void);
#endif    /*External Coex*/

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
