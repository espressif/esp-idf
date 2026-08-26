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

#include "esp_ble_audio_cap_api.h"
#include "esp_ble_audio_tmap_api.h"
#include "esp_ble_audio_bap_lc3_preset_defs.h"

#include "ble_audio_example_init.h"
#include "ble_audio_example_utils.h"

#define TAG "TMAP_BMS"

#define LOCAL_DEVICE_NAME       "TMAP Broadcast Source"

#define ADV_HANDLE              0
#define ADV_SID                 0
#define ADV_TX_POWER            127
#define ADV_INTERVAL_MS         200
#define PER_ADV_INTERVAL_MS     100

int app_host_init(void);

int set_device_name(void);

int ext_adv_start(const uint8_t *ext_data, uint8_t ext_len,
                  const uint8_t *per_data, uint8_t per_len);

int ext_adv_stop(void);

int cap_initiator_setup(void);

int cap_initiator_update(void);

int cap_initiator_stop(void);

int cap_initiator_init(void);
