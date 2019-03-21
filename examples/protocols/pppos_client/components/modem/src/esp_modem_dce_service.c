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
#include <string.h>
#include "esp_log.h"
#include "esp_modem_dce_service.h"

/**
 * @brief Macro defined for error checking
 *
 */
static const char *DCE_TAG = "dce_service";
#define DCE_CHECK(a, str, goto_tag, ...)                                              \
    do                                                                                \
    {                                                                                 \
        if (!(a))                                                                     \
        {                                                                             \
            ESP_LOGE(DCE_TAG, "%s(%d): " str, __FUNCTION__, __LINE__, ##__VA_ARGS__); \
            goto goto_tag;                                                            \
        }                                                                             \
    } while (0)

esp_err_t esp_modem_dce_handle_response_default(modem_dce_t *dce, const char *line)
{
    esp_err_t err = ESP_FAIL;
    if (strstr(line, MODEM_RESULT_CODE_SUCCESS)) {
        err = esp_modem_process_command_done(dce, MODEM_STATE_SUCCESS);
    } else if (strstr(line, MODEM_RESULT_CODE_ERROR)) {
        err = esp_modem_process_command_done(dce, MODEM_STATE_FAIL);
    }
    return err;
}

esp_err_t esp_modem_dce_sync(modem_dce_t *dce)
{
    modem_dte_t *dte = dce->dte;
    dce->handle_line = esp_modem_dce_handle_response_default;
    DCE_CHECK(dte->send_cmd(dte, "AT\r", MODEM_COMMAND_TIMEOUT_DEFAULT) == ESP_OK, "send command failed", err);
    DCE_CHECK(dce->state == MODEM_STATE_SUCCESS, "sync failed", err);
    ESP_LOGD(DCE_TAG, "sync ok");
    return ESP_OK;
err:
    return ESP_FAIL;
}

esp_err_t esp_modem_dce_echo(modem_dce_t *dce, bool on)
{
    modem_dte_t *dte = dce->dte;
    dce->handle_line = esp_modem_dce_handle_response_default;
    if (on) {
        DCE_CHECK(dte->send_cmd(dte, "ATE1\r", MODEM_COMMAND_TIMEOUT_DEFAULT) == ESP_OK, "send command failed", err);
        DCE_CHECK(dce->state == MODEM_STATE_SUCCESS, "enable echo failed", err);
        ESP_LOGD(DCE_TAG, "enable echo ok");
    } else {
        DCE_CHECK(dte->send_cmd(dte, "ATE0\r", MODEM_COMMAND_TIMEOUT_DEFAULT) == ESP_OK, "send command failed", err);
        DCE_CHECK(dce->state == MODEM_STATE_SUCCESS, "disable echo failed", err);
        ESP_LOGD(DCE_TAG, "disable echo ok");
    }
    return ESP_OK;
err:
    return ESP_FAIL;
}

esp_err_t esp_modem_dce_store_profile(modem_dce_t *dce)
{
    modem_dte_t *dte = dce->dte;
    dce->handle_line = esp_modem_dce_handle_response_default;
    DCE_CHECK(dte->send_cmd(dte, "AT&W\r", MODEM_COMMAND_TIMEOUT_DEFAULT) == ESP_OK, "send command failed", err);
    DCE_CHECK(dce->state == MODEM_STATE_SUCCESS, "save settings failed", err);
    ESP_LOGD(DCE_TAG, "save settings ok");
    return ESP_OK;
err:
    return ESP_FAIL;
}

esp_err_t esp_modem_dce_set_flow_ctrl(modem_dce_t *dce, modem_flow_ctrl_t flow_ctrl)
{
    modem_dte_t *dte = dce->dte;
    char command[16];
    int len = snprintf(command, sizeof(command), "AT+IFC=%d,%d\r", dte->flow_ctrl, flow_ctrl);
    DCE_CHECK(len < sizeof(command), "command too long: %s", err, command);
    dce->handle_line = esp_modem_dce_handle_response_default;
    DCE_CHECK(dte->send_cmd(dte, command, MODEM_COMMAND_TIMEOUT_DEFAULT) == ESP_OK, "send command failed", err);
    DCE_CHECK(dce->state == MODEM_STATE_SUCCESS, "set flow control failed", err);
    ESP_LOGD(DCE_TAG, "set flow control ok");
    return ESP_OK;
err:
    return ESP_FAIL;
}

esp_err_t esp_modem_dce_define_pdp_context(modem_dce_t *dce, uint32_t cid, const char *type, const char *apn)
{
    modem_dte_t *dte = dce->dte;
    char command[64];
    int len = snprintf(command, sizeof(command), "AT+CGDCONT=%d,\"%s\",\"%s\"\r", cid, type, apn);
    DCE_CHECK(len < sizeof(command), "command too long: %s", err, command);
    dce->handle_line = esp_modem_dce_handle_response_default;
    DCE_CHECK(dte->send_cmd(dte, command, MODEM_COMMAND_TIMEOUT_DEFAULT) == ESP_OK, "send command failed", err);
    DCE_CHECK(dce->state == MODEM_STATE_SUCCESS, "define pdp context failed", err);
    ESP_LOGD(DCE_TAG, "define pdp context ok");
    return ESP_OK;
err:
    return ESP_FAIL;
}

esp_err_t esp_modem_dce_hang_up(modem_dce_t *dce)
{
    modem_dte_t *dte = dce->dte;
    dce->handle_line = esp_modem_dce_handle_response_default;
    DCE_CHECK(dte->send_cmd(dte, "ATH\r", MODEM_COMMAND_TIMEOUT_HANG_UP) == ESP_OK, "send command failed", err);
    DCE_CHECK(dce->state == MODEM_STATE_SUCCESS, "hang up failed", err);
    ESP_LOGD(DCE_TAG, "hang up ok");
    return ESP_OK;
err:
    return ESP_FAIL;
}
