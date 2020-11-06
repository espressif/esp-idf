// Copyright 2019 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "ccomp_timer.h"

#include "ccomp_timer_impl.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

#include "esp_log.h"
#include "esp_intr_alloc.h"

static const char TAG[] = "ccomp_timer";

esp_err_t ccomp_timer_start(void)
{
    esp_err_t err = ESP_OK;

    ccomp_timer_impl_lock();
    if (ccomp_timer_impl_is_init()) {
        if (ccomp_timer_impl_is_active()) {
            err = ESP_ERR_INVALID_STATE;
        }
    }
    else {
        err = ccomp_timer_impl_init();
    }
    ccomp_timer_impl_unlock();

    if (err != ESP_OK) {
        goto fail;
    }

    err = ccomp_timer_impl_reset();

    if (err != ESP_OK) {
        goto fail;
    }

    err = ccomp_timer_impl_start();

    if (err == ESP_OK) {
        return ESP_OK;
    }

fail:
    ESP_LOGE(TAG, "Unable to start performance timer");
    return err;
}

int64_t IRAM_ATTR ccomp_timer_stop(void)
{
    esp_err_t err = ESP_OK;
    ccomp_timer_impl_lock();
    if (!ccomp_timer_impl_is_active()) {
        err = ESP_ERR_INVALID_STATE;
    }
    ccomp_timer_impl_unlock();

    if (err != ESP_OK) {
        goto fail;
    }

    err = ccomp_timer_impl_stop();
    if (err != ESP_OK) {
        goto fail;
    }

    int64_t t = ccomp_timer_get_time();

    err = ccomp_timer_impl_deinit();

    if (err == ESP_OK && t != -1) {
        return t;
    }

fail:
    ESP_LOGE(TAG, "Unable to stop performance timer");
    return -1;
}

int64_t IRAM_ATTR ccomp_timer_get_time(void)
{
    return ccomp_timer_impl_get_time();
}
