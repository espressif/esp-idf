// Copyright 2015-2018 Espressif Systems (Shanghai) PTE LTD
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
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "esp_modem_dce.h"

/**
 * @brief Indicate that processing current command has done
 *
 * @param dce Modem DCE object
 * @param state Modem state after processing
 * @return esp_err_t
 *      - ESP_OK on success
 *      - ESP_FAIL on error
 */
static inline esp_err_t esp_modem_process_command_done(modem_dce_t *dce, modem_state_t state)
{
    dce->state = state;
    return dce->dte->process_cmd_done(dce->dte);
}

/**
 * @brief Strip the tailed "\r\n"
 *
 * @param str string to strip
 * @param len length of string
 */
static inline void strip_cr_lf_tail(char *str, uint32_t len)
{
    if (str[len - 2] == '\r') {
        str[len - 2] = '\0';
    } else if (str[len - 1] == '\r') {
        str[len - 1] = '\0';
    }
}

/**
 * @brief Default handler for response
 * Some responses for command are simple, commonly will return OK when succeed of ERROR when failed
 *
 * @param dce Modem DCE object
 * @param line line string
 * @return esp_err_t
 *      - ESP_OK on success
 *      - ESP_FAIL on error
 */
esp_err_t esp_modem_dce_handle_response_default(modem_dce_t *dce, const char *line);

/**
 * @brief Handle response from +++ (Set Working Mode)
 *
 * @param dce Modem DCE object
 * @param line line string
 * @return esp_err_t
 *      - ESP_OK on success
 *      - ESP_FAIL on error
 */
esp_err_t esp_modem_dce_handle_exit_data_mode(modem_dce_t *dce, const char *line);

/**
 * @brief Handle response from ATD*99# (Set Working Mode)
 *
 * @param dce Modem DCE object
 * @param line line string
 * @return esp_err_t
 *      - ESP_OK on success
 *      - ESP_FAIL on error
 */
esp_err_t esp_modem_dce_handle_atd_ppp(modem_dce_t *dce, const char *line);

/**
 * @brief Syncronization
 *
 * @param dce Modem DCE object
 * @return esp_err_t
 *      - ESP_OK on success
 *      - ESP_FAIL on error
 */
esp_err_t esp_modem_dce_sync(modem_dce_t *dce);

/**
 * @brief Enable or not echo mode of DCE (V.25TER specification)
 *
 * @param dce Modem DCE object
 * @param on true to enable echo mode, false to disable echo mode
 * @return esp_err_t
 *      - ESP_OK on success
 *      - ESP_FAIL on error
 */
esp_err_t esp_modem_dce_echo(modem_dce_t *dce, bool on);

/**
 * @brief Store current parameter setting in the user profile
 *
 * @param dce Modem DCE object
 * @return esp_err_t
 *      - ESP_OK on success
 *      - ESP_FAIL on error
 */
esp_err_t esp_modem_dce_store_profile(modem_dce_t *dce);

/**
 * @brief Set flow control mode of DCE in data mode (V.25TER specification)
 *
 * @param dce Modem DCE object
 * @param flow_ctrl flow control mode
 * @return esp_err_t
 *      - ESP_OK on success
 *      - ESP_FAIL on error
 */
esp_err_t esp_modem_dce_set_flow_ctrl(modem_dce_t *dce, modem_flow_ctrl_t flow_ctrl);

/**
 * @brief Define PDP context (GPRS)
 *
 * @param dce Modem DCE object
 * @param cid PDP context identifier
 * @param type Protocol type
 * @param apn Access point name
 * @return esp_err_t
 *      - ESP_OK on success
 *      - ESP_FAIL on error
 */
esp_err_t esp_modem_dce_define_pdp_context(modem_dce_t *dce, uint32_t cid, const char *type, const char *apn);

/**
 * @brief Get signal quality (3GPP TS 27.007 specification)
 *
 * @param dce Modem DCE object
 * @param rssi received signal strength indication
 * @param ber bit error ratio
 * @return esp_err_t
 *      - ESP_OK on success
 *      - ESP_FAIL on error
 */
esp_err_t esp_modem_dce_get_signal_quality(modem_dce_t *dce, uint32_t *rssi, uint32_t *ber);

/**
 * @brief Get battery status (3GPP TS 27.007 specification)
 *
 * @param dce Modem DCE object
 * @param bcs Battery charge status
 * @param bcl Battery connection level
 * @param voltage Battery voltage
 * @return esp_err_t
 *      - ESP_OK on success
 *      - ESP_FAIL on error
 */
esp_err_t esp_modem_dce_get_battery_status(modem_dce_t *dce, uint32_t *bcs, uint32_t *bcl, uint32_t *voltage);

/**
 * @brief Get DCE module name (3GPP TS 27.007 specification)
 *
 * @param dce Modem DCE object
 * @return esp_err_t
 *      - ESP_OK on success
 *      - ESP_FAIL on error
 */
esp_err_t esp_modem_dce_get_module_name(modem_dce_t *dce);

/**
 * @brief Get DCE module IMEI number (3GPP TS 27.007 specification)
 *
 * @param dce Modem DCE object
 * @return esp_err_t
 *      - ESP_OK on success
 *      - ESP_FAIL on error
 */
esp_err_t esp_modem_dce_get_imei_number(modem_dce_t *dce);

/**
 * @brief Get DCE module IMSI number (3GPP TS 27.007 specification)
 *
 * @param dce Modem DCE object
 * @return esp_err_t
 *      - ESP_OK on success
 *      - ESP_FAIL on error
 */
esp_err_t esp_modem_dce_get_imsi_number(modem_dce_t *dce);

/**
 * @brief Get Operator's name (3GPP TS 27.007 specification)
 *
 * @param dce Modem DCE object
 * @return esp_err_t
 *      - ESP_OK on success
 *      - ESP_FAIL on error
 */
esp_err_t esp_modem_dce_get_operator_name(modem_dce_t *dce);

/**
 * @brief Hang up (V.25TER specification)
 *
 * @param dce Modem DCE object
 * @return esp_err_t
 *      - ESP_OK on success
 *      - ESP_FAIL on error
 */
esp_err_t esp_modem_dce_hang_up(modem_dce_t *dce);

#ifdef __cplusplus
}
#endif
