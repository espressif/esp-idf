/*
 * SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
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

#if ESP_TEE_BUILD
#include "esp_fault.h"
#include "esp_tee_memory_utils.h"
#include "esp_attestation.h"
#endif

#include "esp_tee.h"
#include "secure_service_num.h"

#include "sdkconfig.h"

static __attribute__((unused)) const char *TAG = "esp_tee_att";

#if ESP_TEE_BUILD

esp_err_t _ss_esp_tee_att_generate_token(const uint32_t nonce, const uint32_t client_id, const char *psa_cert_ref,
                                         uint8_t *token_buf, const size_t token_buf_size, uint32_t *token_len)
{
    bool valid_addr = (esp_tee_ptr_in_ree((void *)psa_cert_ref) &&
                       esp_tee_ptr_in_ree((void *)token_buf) &&
                       esp_tee_ptr_in_ree((void *)token_len));
    valid_addr &= (esp_tee_ptr_in_ree((void *)((char *)psa_cert_ref + 20)) &&
                   esp_tee_ptr_in_ree((void *)((char *)token_buf + token_buf_size)));

    if (!valid_addr) {
        return ESP_ERR_INVALID_ARG;
    }
    ESP_FAULT_ASSERT(valid_addr);

    return esp_att_generate_token(nonce, client_id, psa_cert_ref, token_buf, token_buf_size, token_len);
}

#else

esp_err_t esp_tee_att_generate_token(const uint32_t nonce, const uint32_t client_id, const char *psa_cert_ref,
                                     uint8_t *token_buf, const size_t token_buf_size, uint32_t *token_len)
{
    return (esp_err_t)esp_tee_service_call_with_noniram_intr_disabled(7, SS_ESP_TEE_ATT_GENERATE_TOKEN, nonce, client_id,
                                                                      psa_cert_ref, token_buf, token_buf_size, token_len);
}
#endif
