/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_transport_internal.h"

static const char *TAG = "transport";
struct timeval* esp_transport_utils_ms_to_timeval(int timeout_ms, struct timeval *tv)
{
    if (timeout_ms == -1) {
        return NULL;
    }
    tv->tv_sec = timeout_ms / 1000;
    tv->tv_usec = (timeout_ms - (tv->tv_sec * 1000)) * 1000;
    return tv;
}

esp_foundation_transport_t * esp_transport_init_foundation_transport(void)
{
    esp_foundation_transport_t *foundation = calloc(1, sizeof(esp_foundation_transport_t));
    ESP_TRANSPORT_MEM_CHECK(TAG, foundation, return NULL);
    foundation->error_handle = calloc(1, sizeof(struct esp_transport_error_storage));
    ESP_TRANSPORT_MEM_CHECK(TAG, foundation->error_handle,
                        free(foundation);
                        return NULL);
    return foundation;
}

void esp_transport_destroy_foundation_transport(esp_foundation_transport_t *foundation)
{
    if (foundation) {
        free(foundation->error_handle);
    }
    free(foundation);
}
