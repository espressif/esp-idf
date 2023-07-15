/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "sdkconfig.h"
#include "esp_assert.h"

/*
Source used to store ESP chip revision and ESP-IDF minimum supported revision in the future.
Currently only used to hold static assert to check that the configured minimum and maximum supported chip revisions of
ESP-IDF are valid.
*/

ESP_STATIC_ASSERT(CONFIG_ESP_REV_MIN_FULL <= CONFIG_ESP_REV_MAX_FULL);
