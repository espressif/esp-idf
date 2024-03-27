/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stddef.h>
#include <string.h>

#include "sdkconfig.h"
#include "soc/soc_caps.h"
#include "esp_private/sleep_event.h"

#include "esp_sleep.h"
#include "esp_log.h"
#include "esp_check.h"
#include "freertos/FreeRTOS.h"

static __attribute__((unused)) const char *TAG = "sleep_event";

#if CONFIG_ESP_SLEEP_EVENT_CALLBACKS
esp_sleep_event_cbs_config_t g_sleep_event_cbs_config;
static portMUX_TYPE s_sleep_event_mutex = portMUX_INITIALIZER_UNLOCKED;

esp_err_t esp_sleep_register_event_callback(esp_sleep_event_cb_index_t event_id, const esp_sleep_event_cb_config_t *event_cb_conf) {
    if (event_cb_conf == NULL || event_id >= SLEEP_EVENT_CB_INDEX_NUM) {
        return ESP_ERR_INVALID_ARG;
    }
    esp_sleep_event_cb_config_t *new_config = (esp_sleep_event_cb_config_t *)heap_caps_malloc(sizeof(esp_sleep_event_cb_config_t), MALLOC_CAP_INTERNAL);
    if (new_config == NULL) {
        return ESP_ERR_NO_MEM; /* Memory allocation failed */
    }

    portENTER_CRITICAL(&s_sleep_event_mutex);
    esp_sleep_event_cb_config_t **current_ptr = &(g_sleep_event_cbs_config.sleep_event_cb_config[event_id]);
    while (*current_ptr != NULL) {
        if (((*current_ptr)->cb) == (event_cb_conf->cb)) {
            free(new_config);
            portEXIT_CRITICAL(&s_sleep_event_mutex);
            return ESP_FAIL;
        }
        current_ptr = &((*current_ptr)->next);
    }

    *new_config = *event_cb_conf;
    while (*current_ptr != NULL && (*current_ptr)->prior <= new_config->prior) {
        current_ptr = &((*current_ptr)->next);
    }
    new_config->next = *current_ptr;
    *current_ptr = new_config;
    portEXIT_CRITICAL(&s_sleep_event_mutex);
    return ESP_OK;
}

esp_err_t esp_sleep_unregister_event_callback(esp_sleep_event_cb_index_t event_id, esp_sleep_event_cb_t cb) {
    if (cb == NULL || event_id >= SLEEP_EVENT_CB_INDEX_NUM) {
        return ESP_ERR_INVALID_ARG;
    }
    portENTER_CRITICAL(&s_sleep_event_mutex);
    esp_sleep_event_cb_config_t **current_ptr = &(g_sleep_event_cbs_config.sleep_event_cb_config[event_id]);
    while (*current_ptr != NULL) {
        if (((*current_ptr)->cb) == cb) {
            esp_sleep_event_cb_config_t *temp = *current_ptr;
            *current_ptr = (*current_ptr)->next;
            free(temp);
            break;
        }
        current_ptr = &((*current_ptr)->next);
    }
    portEXIT_CRITICAL(&s_sleep_event_mutex);
    return ESP_OK;
}
#endif

void IRAM_ATTR esp_sleep_execute_event_callbacks(esp_sleep_event_cb_index_t event_id, void *ext_arg)
{
#if CONFIG_ESP_SLEEP_EVENT_CALLBACKS
    if (event_id >= SLEEP_EVENT_CB_INDEX_NUM) {
        ESP_EARLY_LOGW(TAG, "event_id out of range");
        return;
    }
    esp_sleep_event_cb_config_t *current = g_sleep_event_cbs_config.sleep_event_cb_config[event_id];
    while (current != NULL) {
        if (current->cb != NULL) {
            if (ESP_OK != (*current->cb)(current->user_arg, ext_arg)) {
                ESP_EARLY_LOGW(TAG, "esp_sleep_execute_event_callbacks has an err, current->cb = %p", current->cb);
            }
        }
        current = current->next;
    }
#endif
}
