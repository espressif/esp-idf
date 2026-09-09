/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: LicenseRef-Included
 *
 * This example code is in the Public Domain (or CC0 licensed, at your option.)
 *
 * Unless required by applicable law or agreed to in writing, this
 * software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
 * CONDITIONS OF ANY KIND, either express or implied.
 */

#pragma once

#include "esp_err.h"
#include "esp_rcp_update.h"

esp_err_t esp_zigbee_rcp_init(esp_rcp_update_config_t *config);

void esp_zigbee_rcp_deinit(void);

esp_err_t esp_zigbee_rcp_update(void);
