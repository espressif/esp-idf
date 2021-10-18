/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdlib.h>
#include <string.h>
#include "esp_log.h"
#include "esp_modem_dce_service.h"
#include "sim800.h"

#define MODEM_RESULT_CODE_POWERDOWN "POWER DOWN"

static const char *DCE_TAG = "sim800";

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
 * @brief Handle response from AT+CPOWD=1
 */
static esp_err_t sim800_handle_power_down(modem_dce_t *dce, const char *line)
{
    esp_err_t err = ESP_FAIL;
    if (strstr(line, MODEM_RESULT_CODE_POWERDOWN)) {
        err = esp_modem_process_command_done(dce, MODEM_STATE_SUCCESS);
    }
    return err;
}

/**
 * @brief Power down
 *
 * @param sim800_dce sim800 object
 * @return esp_err_t
 *      - ESP_OK on success
 *      - ESP_FAIL on error
 */
static esp_err_t sim800_power_down(modem_dce_t *dce)
{
    modem_dte_t *dte = dce->dte;
    dce->handle_line = sim800_handle_power_down;
    DCE_CHECK(dte->send_cmd(dte, "AT+CPOWD=1\r", MODEM_COMMAND_TIMEOUT_POWEROFF) == ESP_OK, "send command failed", err);
    DCE_CHECK(dce->state == MODEM_STATE_SUCCESS, "power down failed", err);
    ESP_LOGD(DCE_TAG, "power down ok");
    return ESP_OK;
err:
    return ESP_FAIL;
}

/**
 * @brief Set Working Mode
 *
 * @param dce Modem DCE object
 * @param mode woking mode
 * @return esp_err_t
 *      - ESP_OK on success
 *      - ESP_FAIL on error
 */
static esp_err_t sim800_set_working_mode(modem_dce_t *dce, modem_mode_t mode)
{
    modem_dte_t *dte = dce->dte;
    switch (mode) {
    case MODEM_COMMAND_MODE:
        dce->handle_line = esp_modem_dce_handle_exit_data_mode;
        vTaskDelay(pdMS_TO_TICKS(1000)); // spec: 1s delay for the modem to recognize the escape sequence
        if (dte->send_cmd(dte, "+++", MODEM_COMMAND_TIMEOUT_MODE_CHANGE) != ESP_OK) {
            // "+++" Could fail if we are already in the command mode.
            // in that case we ignore the timeout and re-sync the modem
            ESP_LOGI(DCE_TAG, "Sending \"+++\" command failed");
            dce->handle_line = esp_modem_dce_handle_response_default;
            DCE_CHECK(dte->send_cmd(dte, "AT\r", MODEM_COMMAND_TIMEOUT_DEFAULT) == ESP_OK, "send command failed", err);
            DCE_CHECK(dce->state == MODEM_STATE_SUCCESS, "sync failed", err);
        } else {
            DCE_CHECK(dce->state == MODEM_STATE_SUCCESS, "enter command mode failed", err);
            vTaskDelay(pdMS_TO_TICKS(1000)); // spec: 1s delay after `+++` command
        }
        ESP_LOGD(DCE_TAG, "enter command mode ok");
        dce->mode = MODEM_COMMAND_MODE;
        break;
    case MODEM_PPP_MODE:
        dce->handle_line = esp_modem_dce_handle_atd_ppp;
        DCE_CHECK(dte->send_cmd(dte, "ATD*99#\r", MODEM_COMMAND_TIMEOUT_MODE_CHANGE) == ESP_OK, "send command failed", err);
            if (dce->state != MODEM_STATE_SUCCESS) {
                // Initiate PPP mode could fail, if we've already "dialed" the data call before.
                // in that case we retry with "ATO" to just resume the data mode
                ESP_LOGD(DCE_TAG, "enter ppp mode failed, retry with ATO");
                dce->handle_line = esp_modem_dce_handle_atd_ppp;
                DCE_CHECK(dte->send_cmd(dte, "ATO\r", MODEM_COMMAND_TIMEOUT_MODE_CHANGE) == ESP_OK, "send command failed", err);
                DCE_CHECK(dce->state == MODEM_STATE_SUCCESS, "enter ppp mode failed", err);
            }
        ESP_LOGD(DCE_TAG, "enter ppp mode ok");
        dce->mode = MODEM_PPP_MODE;
        break;
    default:
        ESP_LOGW(DCE_TAG, "unsupported working mode: %d", mode);
        goto err;
        break;
    }
    return ESP_OK;
err:
    return ESP_FAIL;
}

/**
 * @brief Deinitialize SIM800 object
 *
 * @param dce Modem DCE object
 * @return esp_err_t
 *      - ESP_OK on success
 *      - ESP_FAIL on fail
 */
static esp_err_t sim800_deinit(modem_dce_t *dce)
{
    esp_modem_dce_t *esp_modem_dce = __containerof(dce, esp_modem_dce_t, parent);
    if (dce->dte) {
        dce->dte->dce = NULL;
    }
    free(esp_modem_dce);
    return ESP_OK;
}

modem_dce_t *sim800_init(modem_dte_t *dte)
{
    DCE_CHECK(dte, "DCE should bind with a DTE", err);
    /* malloc memory for esp_modem_dce object */
    esp_modem_dce_t *esp_modem_dce = calloc(1, sizeof(esp_modem_dce_t));
    DCE_CHECK(esp_modem_dce, "calloc esp_modem_dce_t failed", err);
    /* Bind DTE with DCE */
    esp_modem_dce->parent.dte = dte;
    dte->dce = &(esp_modem_dce->parent);
    /* Bind methods */
    esp_modem_dce->parent.handle_line = NULL;
    esp_modem_dce->parent.sync = esp_modem_dce_sync;
    esp_modem_dce->parent.echo_mode = esp_modem_dce_echo;
    esp_modem_dce->parent.store_profile = esp_modem_dce_store_profile;
    esp_modem_dce->parent.set_flow_ctrl = esp_modem_dce_set_flow_ctrl;
    esp_modem_dce->parent.define_pdp_context = esp_modem_dce_define_pdp_context;
    esp_modem_dce->parent.hang_up = esp_modem_dce_hang_up;
    esp_modem_dce->parent.get_signal_quality = esp_modem_dce_get_signal_quality;
    esp_modem_dce->parent.get_battery_status = esp_modem_dce_get_battery_status;
    esp_modem_dce->parent.get_operator_name = esp_modem_dce_get_operator_name;
    esp_modem_dce->parent.set_working_mode = sim800_set_working_mode;
    esp_modem_dce->parent.power_down = sim800_power_down;
    esp_modem_dce->parent.deinit = sim800_deinit;
    /* Sync between DTE and DCE */
    DCE_CHECK(esp_modem_dce_sync(&(esp_modem_dce->parent)) == ESP_OK, "sync failed", err_io);
    /* Close echo */
    DCE_CHECK(esp_modem_dce_echo(&(esp_modem_dce->parent), false) == ESP_OK, "close echo mode failed", err_io);
    /* Get Module name */
    DCE_CHECK(esp_modem_dce_get_module_name(&(esp_modem_dce->parent)) == ESP_OK, "get module name failed", err_io);
    /* Get IMEI number */
    DCE_CHECK(esp_modem_dce_get_imei_number(&(esp_modem_dce->parent)) == ESP_OK, "get imei failed", err_io);
    /* Get IMSI number */
    DCE_CHECK(esp_modem_dce_get_imsi_number(&(esp_modem_dce->parent)) == ESP_OK, "get imsi failed", err_io);
    /* Get operator name */
    DCE_CHECK(esp_modem_dce_get_operator_name(&(esp_modem_dce->parent)) == ESP_OK, "get operator name failed", err_io);
    return &(esp_modem_dce->parent);
err_io:
    free(esp_modem_dce);
    dte->dce = NULL;
err:
    return NULL;
}
