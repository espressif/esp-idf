/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdbool.h>

#include "bt_common.h"

struct bluedroid_config {
    bool (*get_ssp_enabled)(void);
};

bt_status_t bluedriod_config_init(esp_bluedroid_config_t *cfg);

void bluedriod_config_deinit(void);

const struct bluedroid_config *bluedriod_config_get(void);
