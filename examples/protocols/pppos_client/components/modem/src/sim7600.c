// Copyright 2020 Espressif Systems (Shanghai) PTE LTD
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
#include <stdlib.h>
#include <string.h>
#include "esp_log.h"
#include "esp_modem_dce_service.h"
#include "bg96.h"

/**
 * @brief This module supports SIM7600 module, which has a very similar interface
 * to the BG96, so it just references most of the handlers from BG96 and implements
 * only those that differ.
 */
static const char *DCE_TAG = "sim7600";

/**
 * @brief Macro defined for error checking
 *
 */
#define DCE_CHECK(a, str, goto_tag, ...)                                              \
    do                                                                                \
    {                                                                                 \
        if (!(a))                                                                     \
        {                                                                             \
            ESP_LOGE(DCE_TAG, "%s(%d): " str, __FUNCTION__, __LINE__, ##__VA_ARGS__); \
            goto goto_tag;                                                            \
        }                                                                             \
    } while (0)

/**
 * @brief Handle response from AT+CBC
 */
static esp_err_t sim7600_handle_cbc(modem_dce_t *dce, const char *line)
{
    esp_err_t err = ESP_FAIL;
    esp_modem_dce_t *esp_modem_dce = __containerof(dce, esp_modem_dce_t, parent);
    if (strstr(line, MODEM_RESULT_CODE_SUCCESS)) {
        err = esp_modem_process_command_done(dce, MODEM_STATE_SUCCESS);
    } else if (strstr(line, MODEM_RESULT_CODE_ERROR)) {
        err = esp_modem_process_command_done(dce, MODEM_STATE_FAIL);
    } else if (!strncmp(line, "+CBC", strlen("+CBC"))) {
        /* store value of bcs, bcl, voltage */
        int32_t **cbc = esp_modem_dce->priv_resource;
        int32_t volts = 0, fraction = 0;
        /* +CBC: <voltage in Volts> V*/
        sscanf(line, "+CBC: %d.%dV", &volts, &fraction);
        /* Since the "read_battery_status()" API (besides voltage) returns also values for BCS, BCL (charge status),
         * which are not applicable to this modem, we return -1 to indicate invalid value
         */
        *cbc[0] = -1; // BCS
        *cbc[1] = -1; // BCL
        *cbc[2] = volts*1000 + fraction;
        err = ESP_OK;
    }
    return err;
}

/**
 * @brief Handle response from AT+CPOF
 */
static esp_err_t sim7600_handle_cpof(modem_dce_t *dce, const char *line)
{
    esp_err_t err = ESP_OK;
    if (strstr(line, MODEM_RESULT_CODE_SUCCESS)) {
        err = esp_modem_process_command_done(dce, MODEM_STATE_SUCCESS);
    } else if (strstr(line, MODEM_RESULT_CODE_NO_CARRIER)) {
        err = ESP_OK;
    } else if (strstr(line, MODEM_RESULT_CODE_ERROR)) {
        err = esp_modem_process_command_done(dce, MODEM_STATE_FAIL);
    }
    return err;
}

/**
 * @brief Get battery status
 *
 * @param dce Modem DCE object
 * @param bcs Battery charge status
 * @param bcl Battery connection level
 * @param voltage Battery voltage
 * @return esp_err_t
 *      - ESP_OK on success
 *      - ESP_FAIL on error
 */
static esp_err_t sim7600_get_battery_status(modem_dce_t *dce, uint32_t *bcs, uint32_t *bcl, uint32_t *voltage)
{
    modem_dte_t *dte = dce->dte;
    esp_modem_dce_t *esp_modem_dce = __containerof(dce, esp_modem_dce_t, parent);
    uint32_t *resource[3] = {bcs, bcl, voltage};
    esp_modem_dce->priv_resource = resource;
    dce->handle_line = sim7600_handle_cbc;
    DCE_CHECK(dte->send_cmd(dte, "AT+CBC\r", MODEM_COMMAND_TIMEOUT_DEFAULT) == ESP_OK, "send command failed", err);
    DCE_CHECK(dce->state == MODEM_STATE_SUCCESS, "inquire battery status failed", err);
    ESP_LOGD(DCE_TAG, "inquire battery status ok");
    return ESP_OK;
err:
    return ESP_FAIL;
}

/**
 * @brief Set the SIM7600 device to power down mode
 *
 * @param dce common modem dce object (modem_dce_t)
 * @return esp_err_t
 *      - ESP_OK on success
 *      - ESP_FAIL on error
 */
static esp_err_t sim7600_power_down(modem_dce_t *dce)
{
    modem_dte_t *dte = dce->dte;
    dce->handle_line = sim7600_handle_cpof;
    DCE_CHECK(dte->send_cmd(dte, "AT+CPOF\r", MODEM_COMMAND_TIMEOUT_POWEROFF) == ESP_OK, "send command failed", err);
    DCE_CHECK(dce->state == MODEM_STATE_SUCCESS, "power down failed", err);
    ESP_LOGD(DCE_TAG, "power down ok");
    return ESP_OK;
err:
    return ESP_FAIL;
}

/**
 * @brief Create and initialize SIM7600 object
 *
 */
modem_dce_t *sim7600_init(modem_dte_t *dte)
{
    modem_dce_t *dce = bg96_init(dte);
    dte->dce->get_battery_status = sim7600_get_battery_status;
    dte->dce->power_down = sim7600_power_down;
    return dce;
}
