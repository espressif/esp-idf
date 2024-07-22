/*
 * SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "esp_err.h"
#include "esp_wifi_he_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
  * @brief     Set up an individual TWT agreement (NegotiationType=0) or change TWT parameters of the existing TWT agreement
  *            - TWT Wake Interval = TWT Wake Interval Mantissa * (2 ^ TWT Wake Interval Exponent), unit: us
  *            - e.g. TWT Wake Interval Mantissa = 512,  TWT Wake Interval Exponent = 12, then TWT Wake Interval is 2097.152 ms
  *                   Nominal Minimum Wake Duration = 255, then TWT Wake Duration is 65.28 ms
  *
  * @attention  Support at most 8 TWT agreements, otherwise ESP_ERR_WIFI_TWT_FULL will be returned.
  *             Support sleep time up to (1 << 35) us.
  *
  * @param[in,out]   setup_config pointer to itwt setup config structure.
  *
  * @return
  *    - ESP_OK: succeed
  *    - ESP_ERR_WIFI_NOT_INIT: WiFi is not initialized by esp_wifi_init
  *    - ESP_ERR_WIFI_NOT_STARTED: WiFi is not started by esp_wifi_start
  *    - ESP_ERR_WIFI_CONN: WiFi internal error, station or soft-AP control block wrong
  *    - ESP_ERR_WIFI_NOT_CONNECT: The station is in disconnect status
  *    - ESP_ERR_WIFI_TWT_FULL: no available flow id
  *    - ESP_ERR_INVALID_ARG: invalid argument
  */
esp_err_t esp_wifi_sta_itwt_setup(wifi_itwt_setup_config_t *setup_config);

/**
  * @brief     Tear down individual TWT agreements
  *
  * @param[in]    flow_id  The value range is [0, 7].
  *                        FLOW_ID_ALL indicates tear down all individual TWT agreements.
  *
  * @return
  *    - ESP_OK: succeed
  *    - ESP_ERR_WIFI_NOT_INIT: WiFi is not initialized by esp_wifi_init
  *    - ESP_ERR_WIFI_NOT_STARTED: WiFi is not started by esp_wifi_start
  *    - ESP_ERR_WIFI_CONN: WiFi internal error, station or soft-AP control block wrong
  *    - ESP_ERR_WIFI_NOT_CONNECT: The station is in disconnect status
  *    - ESP_ERR_INVALID_ARG: invalid argument
  */
esp_err_t esp_wifi_sta_itwt_teardown(int flow_id);

/**
  * @brief     Send a TWT Information frame to AP for suspending/resuming established iTWT agreements.
  *
  * @param[in]    flow_id The value range is [0, 7].
  *                       FLOW_ID_ALL indicates suspend all individual TWT agreements
  * @param[in]    suspend_time_ms If the value is 0, indicates the specified flow_id or all established agreements will be suspended until resume by users.
  *                               If the value is greater than 0, indicates the specified flow_id or all established agreements will be suspended until suspend_time_ms timeout, unit: ms.
  *
  * @return
  *    - ESP_OK: succeed
  *    - ESP_ERR_WIFI_NOT_INIT: WiFi is not initialized by esp_wifi_init
  *    - ESP_ERR_WIFI_NOT_STARTED: WiFi is not started by esp_wifi_start
  *    - ESP_ERR_WIFI_CONN: WiFi internal error, station or soft-AP control block wrong
  *    - ESP_ERR_WIFI_NOT_ASSOC: WiFi is not associated
  *    - ESP_ERR_INVALID_ARG: invalid argument
  */
esp_err_t esp_wifi_sta_itwt_suspend(int flow_id, int suspend_time_ms);

/**
  * @brief     Get flow id status
  *
  * @param[in]    flow_id_bitmap Flow id status bitmap with 8 bit. Each bit represents that whether the corresponding flow id is setup.
  *                              1: setup, 0: not setup.
  *
  * @return
  *    - ESP_OK: succeed
  *    - ESP_ERR_WIFI_NOT_INIT: WiFi is not initialized by esp_wifi_init
  *    - ESP_ERR_WIFI_NOT_STARTED: WiFi is not started by esp_wifi_start
  *    - ESP_ERR_WIFI_CONN: WiFi internal error, station or soft-AP control block wrong
  *    - ESP_ERR_WIFI_NOT_CONNECT: The station is in disconnect status
  *    - ESP_ERR_INVALID_ARG: invalid argument
  */
esp_err_t esp_wifi_sta_itwt_get_flow_id_status(int *flow_id_bitmap);

/**
  * @brief     Send probe to update TSF time
  *
  * @attention  In bad network, timeout_ms is variable with the network
  *
  * @param[in]    timeout_ms The estimated time includes sending probe request and receiving probe response, unit: ms.
  *
  * @return
  *    - ESP_OK: succeed
  *    - ESP_ERR_WIFI_NOT_INIT: WiFi is not initialized by esp_wifi_init
  *    - ESP_ERR_WIFI_NOT_STARTED: WiFi is not started by esp_wifi_start
  *    - ESP_ERR_WIFI_CONN: WiFi internal error, station or soft-AP control block wrong
  *    - ESP_ERR_WIFI_NOT_ASSOC: WiFi is not associated
  */
esp_err_t esp_wifi_sta_itwt_send_probe_req(int timeout_ms);

/**
  * @brief     Set time offset with TBTT of target wake time field in itwt setup request frame.
  *
  * @param[in]    offset_us Offset with TBTT of target wake time field in itwt setup request frame, range is [0, 102400], unit microseconds.
  *
  * @return
  *    - ESP_OK: succeed
  *    - ESP_ERR_WIFI_NOT_INIT: WiFi is not initialized by esp_wifi_init
  *    - ESP_ERR_WIFI_NOT_STARTED: WiFi is not started by esp_wifi_start
  *    - ESP_ERR_WIFI_CONN: WiFi internal error, station or soft-AP control block wrong
  *    - ESP_ERR_WIFI_NOT_ASSOC: WiFi is not associated
  *    - ESP_ERR_INVALID_ARG: invalid argument
  */
esp_err_t esp_wifi_sta_itwt_set_target_wake_time_offset(int offset_us);

/**
  * @brief     Enable the reception statistics.
  *
  * @param[in]    rx_stats indicate whether enable the reception statistics for HT, HE SU, HE ER SU and legacy
  * @param[in]    rx_mu_stats indicate whether enable the reception statistics for DL MU-MIMO and DL OFDMA
  *
  * @return
  *    - ESP_OK: succeed
  *    - ESP_ERR_NO_MEM: out of memory
  */
esp_err_t esp_wifi_enable_rx_statistics(bool rx_stats, bool rx_mu_stats);

/**
  * @brief     Enable the transmission statistics.
  *
  * @param[in]    aci access category of the transmission
  * @param[in]    tx_stats indicate whether enable the transmission statistics
  *
  * @return
  *    - ESP_OK: succeed
  *    - ESP_ERR_NO_MEM: out of memory
  */
esp_err_t esp_wifi_enable_tx_statistics(esp_wifi_aci_t aci, bool tx_stats);

/**
  * @brief     Set up an broadcast TWT agreement (NegotiationType=3) or change TWT parameters of the existing TWT agreement
  *            - TWT Wake Interval = TWT Wake Interval Mantissa * (2 ^ TWT Wake Interval Exponent), unit: us
  *            - e.g. TWT Wake Interval Mantissa = 512,  TWT Wake Interval Exponent = 12, then TWT Wake Interval is 2097.152 ms
  *                   Nominal Minimum Wake Duration = 255, then TWT Wake Duration is 65.28 ms
  *
  * @attention  Support at most 32 TWT agreements, otherwise ESP_ERR_WIFI_TWT_FULL will be returned.
  *             Support sleep time up to (1 << 35) us.
  *
  * @param[in,out]   config pointer to btwt setup config structure.
  *
  * @return
  *    - ESP_OK: succeed
  *    - ESP_ERR_WIFI_NOT_INIT: WiFi is not initialized by esp_wifi_init
  *    - ESP_ERR_WIFI_NOT_STARTED: WiFi is not started by esp_wifi_start
  *    - ESP_ERR_WIFI_CONN: WiFi internal error, station or soft-AP control block wrong
  *    - ESP_ERR_WIFI_NOT_CONNECT: The station is in disconnect status
  *    - ESP_ERR_WIFI_TWT_FULL: no available flow id
  *    - ESP_ERR_INVALID_ARG: invalid argument
  */
esp_err_t esp_wifi_sta_btwt_setup(wifi_btwt_setup_config_t *config);

/**
  * @brief     Tear down broadcast TWT agreements
  *
  * @param[in]    btwt_id  The value range is [0, 31].
  *                        BTWT_ID_ALL indicates tear down all broadcast TWT agreements.
  *
  * @return
  *    - ESP_OK: succeed
  *    - ESP_ERR_WIFI_NOT_INIT: WiFi is not initialized by esp_wifi_init
  *    - ESP_ERR_WIFI_NOT_STARTED: WiFi is not started by esp_wifi_start
  *    - ESP_ERR_WIFI_CONN: WiFi internal error, station or soft-AP control block wrong
  *    - ESP_ERR_WIFI_NOT_CONNECT: The station is in disconnect status
  *    - ESP_ERR_INVALID_ARG: invalid argument
  */
esp_err_t esp_wifi_sta_btwt_teardown(uint8_t btwt_id);

/**
  * @brief     Get number of broadcast TWTs supported by the connected AP
  *
  * @param[out] btwt_number  store number of btwts supported by the connected AP
  *
  * @return
  *    - ESP_OK: succeed
  *    - ESP_ERR_WIFI_NOT_INIT: WiFi is not initialized by esp_wifi_init
  *    - ESP_ERR_WIFI_NOT_STARTED: WiFi is not started by esp_wifi_start
  *    - ESP_ERR_INVALID_ARG: invalid argument
  */
esp_err_t esp_wifi_sta_get_btwt_num(uint8_t *btwt_number);

/**
  * @brief     Get broadcast TWT information
  *
  * @param[in]    btwt_number As input param, it stores max btwt number AP supported.
  * @param[in]    btwt_info array to hold the btwt information supported by AP, and the array size must be at least as large as the BTWT number.
  *
  * @return
  *    - ESP_OK: succeed
  *    - ESP_FAIL: fail
  *    - ESP_ERR_WIFI_CONN: WiFi internal error, station or soft-AP control block wrong
  */
esp_err_t esp_wifi_sta_btwt_get_info(uint8_t btwt_number, esp_wifi_btwt_info_t *btwt_info);

/**
  * @brief     Set WiFi TWT config
  *
  * @param[in]    config pointer to the WiFi TWT configure structure.
  *
  * @return
  *    - ESP_OK: succeed
  */
esp_err_t esp_wifi_sta_twt_config(wifi_twt_config_t *config);

#ifdef __cplusplus
}
#endif
