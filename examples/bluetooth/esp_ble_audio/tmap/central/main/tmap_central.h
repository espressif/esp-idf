/*
 * SPDX-FileCopyrightText: 2023 NXP
 * SPDX-FileCopyrightText: 2024 Nordic Semiconductor ASA
 * SPDX-FileContributor: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include <assert.h>
#include <errno.h>

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
#include "esp_ble_audio_tbs_api.h"

#include "ble_audio_example_init.h"
#include "ble_audio_example_utils.h"

#define TAG "TMAP_CEN"

#define CONN_HANDLE_INIT        0xFFFF

#define LOCAL_DEVICE_NAME       "TMAP Central"

#define SCAN_INTERVAL           160     /* 100ms */
#define SCAN_WINDOW             160     /* 100ms */

/* ACL init parameters shared between bluedroid and nimble host wrappers.
 * Raw HCI units (scan: 0.625ms; conn interval: 1.25ms; timeout: 10ms). */
#define INIT_SCAN_INTERVAL      16      /* 10ms */
#define INIT_SCAN_WINDOW        16      /* 10ms */
#define CONN_INTERVAL           24      /* 30ms */
#define CONN_LATENCY            0
#define CONN_TIMEOUT            500     /* 5s */
#define CONN_MIN_CE_LEN         0xFFFF
#define CONN_MAX_CE_LEN         0xFFFF

int app_host_init(void);

int set_device_name(void);

int ext_scan_start(void);
int ext_scan_stop(void);

int conn_create(uint8_t addr_type, const uint8_t addr[6]);

int pairing_start(uint16_t conn_handle);

int exchange_mtu(uint16_t conn_handle);

void security_failed_recover(uint16_t conn_handle, uint8_t status);

uint16_t default_conn_handle_get(void);

int mcp_server_init(void);

int ccp_server_init(void);

int vcp_vol_ctlr_init(void);

int vcp_vol_ctlr_discover(void);

int cap_initiator_init(void);

int cap_initiator_setup(void);

int unicast_group_delete(void);
