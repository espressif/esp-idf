/*
 * SPDX-FileCopyrightText: 2021-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#ifndef __BT_APP_HF_H__
#define __BT_APP_HF_H__

#include <stdint.h>
#include "sdkconfig.h"
#include "esp_hf_ag_api.h"
#include "esp_bt_defs.h"

extern esp_bd_addr_t hf_peer_addr; // Declaration of peer device bdaddr

#define BT_HF_TAG               "BT_APP_HF"

/**
 * @brief     callback function for HF client
 */
void bt_app_hf_cb(esp_hf_cb_event_t event, esp_hf_cb_param_t *param);

#if CONFIG_BT_HFP_AUDIO_DATA_PATH_HCI
/**
 * @brief Stop PCM/LC3 TX before esp_hf_ag_audio_disconnect() to avoid use-after-free.
 */
void bt_app_hf_prepare_audio_disconnect(void);
#endif

#endif /* __BT_APP_HF_H__*/
