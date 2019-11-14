// Copyright 2015-2018 Espressif Systems (Shanghai) PTE LTD
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

#include "esp_err.h"
#include "esp_log.h"
#include "esp_event.h"
#include "esp_event_legacy.h"

#include "sdkconfig.h"

static const char* TAG = "event";

static system_event_cb_t s_event_handler_cb;
static void *s_event_ctx;
static bool s_initialized;

ESP_EVENT_DEFINE_BASE(SYSTEM_EVENT);

static void esp_event_post_to_user(void* arg, esp_event_base_t base, int32_t id, void* data)
{
    if (s_event_handler_cb) {
        system_event_t* event = (system_event_t*) data;
        (*s_event_handler_cb)(s_event_ctx, event);
    }
}

system_event_cb_t esp_event_loop_set_cb(system_event_cb_t cb, void *ctx)
{
    system_event_cb_t old_cb = s_event_handler_cb;
    s_event_handler_cb = cb;
    s_event_ctx = ctx;
    return old_cb;
}

esp_err_t esp_event_send_legacy(system_event_t *event)
{
    if (!s_initialized) {
        ESP_LOGE(TAG, "system event loop not initialized via esp_event_loop_init");
        return ESP_ERR_INVALID_STATE;
    }

    return esp_event_post(SYSTEM_EVENT, event->event_id, event, sizeof(*event), 0);
}

esp_err_t esp_event_loop_init(system_event_cb_t cb, void *ctx)
{
    if (s_initialized) {
        ESP_LOGE(TAG, "system event loop already initialized");
        return ESP_ERR_INVALID_STATE;
    }

    esp_err_t err = esp_event_loop_create_default();
    if (err != ESP_OK && err != ESP_ERR_INVALID_STATE) {
        return err;
    }

    err = esp_event_handler_register(SYSTEM_EVENT, ESP_EVENT_ANY_ID, esp_event_post_to_user, NULL);
    if (err != ESP_OK) {
        return err;
    }

    s_initialized = true;
    s_event_handler_cb = cb;
    s_event_ctx = ctx;
    return ESP_OK;
}

esp_err_t esp_event_loop_deinit(void) 
{
    if (!s_initialized) {
        ESP_LOGE(TAG, "system event loop not initialized");
        return ESP_ERR_INVALID_STATE;
    }

    esp_err_t err = esp_event_handler_unregister(SYSTEM_EVENT, ESP_EVENT_ANY_ID, esp_event_post_to_user);
    if (err != ESP_OK) {
        return err;
    }

    err = esp_event_loop_delete_default();
    if (err != ESP_OK && err != ESP_ERR_INVALID_STATE) {
        return err;
    }

    s_initialized = false;
    s_event_handler_cb = NULL;
    s_event_ctx = NULL;
    return ESP_OK;
}

