/*
 * SPDX-FileCopyrightText: 2018-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __ESP_COEXIST_INTERNAL_H__
#define __ESP_COEXIST_INTERNAL_H__

#include <stdbool.h>
#include "esp_coexist.h"
#include "private/esp_coexist_adapter.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    COEX_PREFER_WIFI = 0,
    COEX_PREFER_BT,
    COEX_PREFER_BALANCE,
    COEX_PREFER_NUM,
} coex_prefer_t;

typedef enum {
    COEX_SCHM_CALLBACK_TYPE_WIFI = 0,
    COEX_SCHM_CALLBACK_TYPE_BT,
    COEX_SCHM_CALLBACK_TYPE_I154,
} coex_schm_callback_type_t;

typedef enum {
    COEX_SCHM_ST_TYPE_WIFI = 0,
    COEX_SCHM_ST_TYPE_BLE,
    COEX_SCHM_ST_TYPE_BT,
} coex_schm_st_type_t;

#define COEX_STATUS_GET_WIFI_BITMAP     (1 << COEX_SCHM_ST_TYPE_WIFI)
#define COEX_STATUS_GET_BLE_BITMAP      (1 << COEX_SCHM_ST_TYPE_BLE)
#define COEX_STATUS_GET_BT_BITMAP       (1 << COEX_SCHM_ST_TYPE_BT)

typedef void (* coex_func_cb_t)(uint32_t event, int sched_cnt);
typedef esp_err_t (* coex_set_lpclk_source_callback_t)(void);
typedef void (* coex_wifi_channel_change_cb_t)(uint8_t primary, uint8_t secondary);

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
 * @brief Get software coexist version value
 *        extern function for internal use.
 * @param ptr_version : points to version structure
 * @return : ESP_OK - success, other - failed
 */
esp_err_t coex_version_get_value(coex_version_t* ptr_version);

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
 *
 * @param bitmap : bitmap of the module getting status.
 * @return : software coexist status
 */
uint32_t coex_status_get(uint8_t bitmap);

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
 * @brief Get WiFi channel from coexistence module.
 *
 *  @param primary : pointer to value of WiFi primary channel
 *  @param secondary : pointer to value of WiFi secondary channel
 *  @return : 0 - success, other - failed
 */
int coex_wifi_channel_get(uint8_t *primary, uint8_t *secondary);

/**
 * @brief Register application callback function to Wi-Fi update low power clock module.
 *
 * @param callback : Wi-Fi update low power clock callback function
 */
void coex_wifi_register_update_lpclk_callback(coex_set_lpclk_source_callback_t callback);

/**
 * @brief Bluetooth requests coexistence
 *
 *  @param event : Bluetooth event
 *  @param latency : Bluetooth will request coexistence after latency
 *  @param duration : duration for Bluetooth to request coexistence
 *  @return : 0 - success, other - failed
 */
int coex_bt_request(uint32_t event, uint32_t latency, uint32_t duration);

/**
 * @brief Bluetooth release coexistence.
 *
 *  @param event : Bluetooth event
 *  @return : 0 - success, other - failed
 */
int coex_bt_release(uint32_t event);

#if CONFIG_IDF_TARGET_ESP32
/**
 * @brief Bluetooth registers callback function to coexistence module
 *        This function is only used on ESP32.
 *
 *  @param callback: callback function registered to coexistence module
 *  @return : 0 - success, other - failed
 */
int coex_register_bt_cb(coex_func_cb_t callback);

/**
 * @brief To acquire the spin-lock used in resetting Bluetooth baseband.
 *        This function is only used to workaround ESP32 hardware issue.
 *
 *  @return : value of the spinlock to be restored
 */
uint32_t coex_bb_reset_lock(void);

/**
 * @brief To release the spin-lock used in resetting Bluetooth baseband.
 *        This function is only used to workaround ESP32 hardware issue.
 *
 *  @param restore: value of the spinlock returned from previous call of coex_bb_rest_lock
 */
void coex_bb_reset_unlock(uint32_t restore);
#endif /* CONFIG_IDF_TARGET_ESP32 */

/**
 * @brief Bluetooth registers callback function to receive notification when Wi-Fi channel changes
 *
 *  @param callback: callback function registered to coexistence module
 *  @return : 0 - success, other - failed
 */
int coex_register_wifi_channel_change_callback(coex_wifi_channel_change_cb_t callback);

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
 *  @param idx : Coexistence scheme phase index
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
 * @brief Register WiFi callback for coexistence starts.
 *
 *  @param cb : WiFi callback
 *  @return : 0 - success, other - failed
 */
int coex_register_start_cb(int (* cb)(void));

/**
 * @brief Restart current coexistence scheme.
 *
 *  @return : 0 - success, other - failed
 */
int coex_schm_process_restart(void);

/**
 * @brief Register callback for coexistence scheme.
 *
 *  @param type : callback type
 *  @param callback : callback
 *  @return : 0 - success, other - failed
 */
int coex_schm_register_callback(coex_schm_callback_type_t type, void *callback);

/**
 * @brief Register coexistence adapter functions.
 *
 *  @param funcs : coexistence adapter functions
 *  @return : ESP_OK - success, other - failed
 */
esp_err_t esp_coex_adapter_register(coex_adapter_funcs_t *funcs);

#if CONFIG_EXTERNAL_COEX_ENABLE
/**
  * @brief     Set external coexistence advanced information, like working mode.
  *
  * @param     out_pti1    This parameter no longer works, will be deprecated and later removed in future releases.
  * @param     out_pti2    This parameter no longer works, will be deprecated and later removed in future releases.
  *
  * @return
  *    - ESP_OK: succeed
  */
esp_err_t esp_coex_external_params(esp_external_coex_advance_t coex_info, uint32_t out_pti1, uint32_t out_pti2);

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

/**
 * @brief Set external coexistence wire type.
 *
 * @param wire_type Set external coexistence wire type.
 *
 */
void esp_coex_external_set_wire_type(external_coex_wire_t wire_type);

#if SOC_EXTERNAL_COEX_LEADER_TX_LINE
/**
  * @brief     Enable external coexist tx line
  *
  * @param     en    Enable external coex tx line
  *
  * @return
  *    - ESP_OK: succeed
  */
void esp_coex_external_set_txline(bool en);
#endif    /*SOC_EXTERNAL_COEX_LEADER_TX_LINE*/
#endif    /*External Coex*/

#if CONFIG_ESP_COEX_POWER_MANAGEMENT
/**
  * @brief     Set coexist scheme flexible period
  *
  * @param     period    flexible period
  *
  * @return
  *    - ESP_OK: succeed
  */
int coex_schm_flexible_period_set(uint8_t period);

/**
  * @brief     Get coexist scheme flexible period
  *
  * @return    Coexist scheme flexible period
  */
uint8_t coex_schm_flexible_period_get(void);
#endif

/**
 * @brief     Get coexistence scheme phase by phase index.
 *
 * @param     phase_idx    Coexistence phase index
 *
 * @return    Coexistence scheme phase
 */
void * coex_schm_get_phase_by_idx(int phase_idx);

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
