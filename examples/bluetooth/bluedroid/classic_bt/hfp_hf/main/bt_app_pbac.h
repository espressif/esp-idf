/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "esp_bt.h"
#include "esp_pbac_api.h"

void bt_app_pbac_cb(esp_pbac_event_t event, esp_pbac_param_t *param);
