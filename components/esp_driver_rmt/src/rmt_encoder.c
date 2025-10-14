/*
 * SPDX-FileCopyrightText: 2022-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "driver/rmt_encoder.h"
#include "rmt_private.h"

esp_err_t rmt_del_encoder(rmt_encoder_handle_t encoder)
{
    ESP_RETURN_ON_FALSE(encoder, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    return encoder->del(encoder);
}

esp_err_t rmt_encoder_reset(rmt_encoder_handle_t encoder)
{
    ESP_RETURN_ON_FALSE(encoder, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    return encoder->reset(encoder);
}

void *rmt_alloc_encoder_mem(size_t size)
{
    return heap_caps_calloc(1, size, RMT_MEM_ALLOC_CAPS);
}
