/*
 * SPDX-FileCopyrightText: 2023 NXP
 * SPDX-FileContributor: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>

#include "esp_log.h"

#include "sdkconfig.h"

#include "esp_ble_audio_lc3_defs.h"
#include "esp_ble_audio_aics_api.h"
#include "esp_ble_audio_cap_api.h"
#include "esp_ble_audio_pacs_api.h"
#include "esp_ble_audio_vcp_api.h"
#include "esp_ble_audio_tbs_api.h"
#include "esp_ble_audio_tmap_api.h"
#include "esp_ble_audio_csip_api.h"
#include "esp_ble_audio_mcs_defs.h"
#include "esp_ble_audio_mcc_api.h"
#include "esp_ble_audio_media_proxy_api.h"
#include "esp_ble_audio_vocs_api.h"

#include "ble_audio_example_init.h"
#include "ble_audio_example_utils.h"

#define TAG "TMAP_PER"

#define CONN_HANDLE_INIT        0xFFFF

#define LOCAL_DEVICE_NAME       "TMAP Peripheral"

#define ADV_HANDLE              0
#define ADV_SID                 0
#define ADV_TX_POWER            127
#define ADV_INTERVAL_MS         200

#define SINK_CONTEXT        (ESP_BLE_AUDIO_CONTEXT_TYPE_UNSPECIFIED | \
                             ESP_BLE_AUDIO_CONTEXT_TYPE_CONVERSATIONAL | \
                             ESP_BLE_AUDIO_CONTEXT_TYPE_MEDIA | \
                             ESP_BLE_AUDIO_CONTEXT_TYPE_GAME | \
                             ESP_BLE_AUDIO_CONTEXT_TYPE_INSTRUCTIONAL)

#define SOURCE_CONTEXT      (ESP_BLE_AUDIO_CONTEXT_TYPE_UNSPECIFIED | \
                             ESP_BLE_AUDIO_CONTEXT_TYPE_CONVERSATIONAL | \
                             ESP_BLE_AUDIO_CONTEXT_TYPE_MEDIA | \
                             ESP_BLE_AUDIO_CONTEXT_TYPE_GAME | \
                             ESP_BLE_AUDIO_CONTEXT_TYPE_INSTRUCTIONAL)

int app_host_init(void);

int set_device_name(void);

int ext_adv_start(const uint8_t *ext_data, uint16_t ext_len);

uint16_t default_conn_handle_get(void);

int vcp_vol_renderer_init(void);

int csip_set_member_init(void);

int csip_generate_rsi(uint8_t rsi[ESP_BLE_AUDIO_CSIP_RSI_SIZE]);

void *csip_svc_inst_get(void);

int bap_unicast_sr_init(void);

int ccp_call_ctrl_init(void);

int ccp_discover_tbs(void);

int ccp_originate_call(void);

int ccp_terminate_call(void);

int mcp_ctlr_init(void);

int mcp_discover_mcs(void);

int mcp_send_cmd(uint8_t mcp_opcode);

bool tmap_peer_is_cg(void);

bool tmap_peer_is_ums(void);

int tmap_ct_umr_init(void);

int tmap_discover_tmas(void);
