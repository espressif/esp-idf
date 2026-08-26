/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>

#include "ble_audio_example_utils.h"

#define TAG "BAP_USR"

#define LOCAL_DEVICE_NAME       "BAP Unicast Server"
#define LOCAL_DEVICE_NAME_LEN   (sizeof(LOCAL_DEVICE_NAME) - 1)

#define ADV_HANDLE              0
#define ADV_SID                 0
#define ADV_TX_POWER            127
#define ADV_INTERVAL_MS         200

int app_host_init(void);

int set_device_name(void);

int ext_adv_start(const uint8_t *ext_data, uint16_t ext_len);
