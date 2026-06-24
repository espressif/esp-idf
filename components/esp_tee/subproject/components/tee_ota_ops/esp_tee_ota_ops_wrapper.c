/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <inttypes.h>
#include <assert.h>

#include "esp_log.h"
#include "esp_err.h"

#include "esp_tee.h"
#include "secure_service_num.h"

#include "sdkconfig.h"

static __attribute__((unused)) const char *TAG = "esp_tee_ota_ops_wrapper";

esp_err_t esp_tee_ota_begin(void)
{
    return (esp_err_t)esp_tee_service_call_with_noniram_intr_disabled(1, SS_ESP_TEE_OTA_BEGIN);
}

esp_err_t esp_tee_ota_write(uint32_t rel_offset, const void *data, size_t size)
{
    return (esp_err_t)esp_tee_service_call_with_noniram_intr_disabled(4, SS_ESP_TEE_OTA_WRITE, rel_offset, data, size);
}

esp_err_t esp_tee_ota_end(void)
{
    return (esp_err_t)esp_tee_service_call_with_noniram_intr_disabled(1, SS_ESP_TEE_OTA_END);
}
