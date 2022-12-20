/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
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
  * @attention  Support at most 4 TWT agreements, otherwise ESP_ERR_WIFI_TWT_FULL will be returned.
  *             Support sleep time up to (1 << 35) us.
  *
  * @param[in]     setup_cmd   Indicates the type of TWT command
  * @param[in]     trigger     true: a trigger-enabled TWT, false: a non-trigger-enabled TWT
  * @param[in]     flow_type   0: an announced TWT, 1: an unannounced TWT
  * @param[in]     min_wake_dura  Nominal Minimum Wake Duration, indicates the minimum amount of time, in unit of 256 us, that the TWT requesting STA expects that it needs to be awake. The value range is [1, 255].
  * @param[in]     wake_invl_expn  TWT Wake Interval Exponent. The value range is [0, 31].
  * @param[in]     wake_invl_mant  TWT Wake Interval Mantissa. The value range is [1, 65535].
  * @param[in/out]    flow_id  When set up an individual TWT agreement, the flow id will be assigned by AP after a successful agreement setup.
  *                            flow_id is allowed to be NULL. flow_id could be specified to a value in the range of [0, 7], but it might be changed by AP in the response.
  *                            When change TWT parameters of the existing TWT agreement, flow_id should be an existing one. The value range is [0, 7].
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
esp_err_t esp_wifi_sta_itwt_setup(wifi_twt_setup_cmds_t setup_cmd, bool trigger, int flow_type, int min_wake_dura, int wake_invl_expn, int wake_invl_mant, int *flow_id);

/**
  * @brief     Tear down individual TWT agreements
  *
  * @param[in]    flow_id  The value range is [0, 8]. 8 indicates tear down all individual TWT agreements.
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
  * @param[in]    flow_id The value range is [0, 8]. 8 indicates suspend all individual TWT agreements
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


#ifdef __cplusplus
}
#endif
