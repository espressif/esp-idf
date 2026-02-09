/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 *
 * OpenThread Command Line Example
 *
 * This example code is in the Public Domain (or CC0 licensed, at your option.)
 *
 * Unless required by applicable law or agreed to in writing, this
 * software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
 * CONDITIONS OF ANY KIND, either express or implied.
*/

#include "ot_examples_common.h"
#include "esp_check.h"
#include "esp_err.h"
#include "esp_openthread.h"
#include "esp_openthread_lock.h"

void ot_network_auto_start(void)
{
    otOperationalDatasetTlvs dataset;
    esp_openthread_lock_acquire(portMAX_DELAY);
    otError error = otDatasetGetActiveTlvs(esp_openthread_get_instance(), &dataset);
    ESP_ERROR_CHECK(esp_openthread_auto_start((error == OT_ERROR_NONE) ? &dataset : NULL));
    esp_openthread_lock_release();
}
