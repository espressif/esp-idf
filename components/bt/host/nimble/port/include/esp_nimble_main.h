/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_err.h"

esp_err_t esp_nimble_init();

esp_err_t esp_nimble_enable(void *host_task);

esp_err_t esp_nimble_disable();

esp_err_t esp_nimble_deinit();
