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

static __attribute__((unused)) const char *TAG = "esp_tee_att";

esp_err_t esp_tee_att_generate_token(const uint32_t nonce, const uint32_t client_id, const char *psa_cert_ref,
                                     uint8_t *token_buf, const size_t token_buf_size, uint32_t *token_len)
{
    return (esp_err_t)esp_tee_service_call_with_noniram_intr_disabled(7, SS_ESP_TEE_ATT_GENERATE_TOKEN, nonce, client_id,
                                                                      psa_cert_ref, token_buf, token_buf_size, token_len);
}
