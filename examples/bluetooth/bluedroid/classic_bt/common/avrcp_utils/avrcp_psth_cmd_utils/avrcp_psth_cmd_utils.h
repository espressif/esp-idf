/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#ifndef __AVRCP_PSTH_CMD_UTILS_H__
#define __AVRCP_PSTH_CMD_UTILS_H__

#include "esp_avrc_api.h"

/**
 * @brief  handle function for AVRCP controller passthrough command event
 *
 * @param [in] event  event id
 * @param [in] param  callback parameter
 */
void bt_avrc_psth_cmd_ct_evt_hdl(esp_avrc_ct_cb_event_t event, esp_avrc_ct_cb_param_t *param);

#endif /* __AVRCP_PSTH_CMD_UTILS_H__ */
