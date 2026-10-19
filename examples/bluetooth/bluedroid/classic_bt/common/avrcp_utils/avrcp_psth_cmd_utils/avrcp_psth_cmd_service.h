/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#ifndef __AVRCP_PSTH_CMD_SERVICE_H__
#define __AVRCP_PSTH_CMD_SERVICE_H__

#include "esp_err.h"
#include "esp_avrc_api.h"

/**
 * @brief  send a passthrough command
 *
 * @param[in] key_code  Passthrough command code (ESP_AVRC_PT_CMD_*)
 * @param[in] key_state Passthrough command state (ESP_AVRC_PT_CMD_STATE_PRESSED or ESP_AVRC_PT_CMD_STATE_RELEASED)
 *
 * @return
 * - ESP_OK: success
 * - ESP_ERR_INVALID_ARG: if key_state is invalid
 * - ESP_ERR_INVALID_STATE: if Bluetooth is not enabled
 * - ESP_FAIL: otherwise
 */
esp_err_t avrc_psth_cmd_srv_send_cmd(esp_avrc_pt_cmd_t key_code, esp_avrc_pt_cmd_state_t key_state);

#endif /* __AVRCP_PSTH_CMD_SERVICE_H__ */
