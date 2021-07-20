// Copyright 2020 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "esp32s2/rom/aes.h"
#include "esp32s2/rom/sha.h"
#include "esp32s2/rom/hmac.h"
#include "esp32s2/rom/digital_signature.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "soc/soc_memory_layout.h"
#include "esp_crypto_lock.h"
#include "esp_hmac.h"

#include "esp_ds.h"

struct esp_ds_context {
    const ets_ds_data_t *data;
};

/**
 * The vtask delay \c esp_ds_sign() is using while waiting for completion of the signing operation.
 */
#define ESP_DS_SIGN_TASK_DELAY_MS 10

#define RSA_LEN_MAX 127

/*
 * Check that the size of esp_ds_data_t and ets_ds_data_t is the same because both structs are converted using
 * raw casts.
 */
_Static_assert(sizeof(esp_ds_data_t) == sizeof(ets_ds_data_t),
        "The size and structure of esp_ds_data_t and ets_ds_data_t must match exactly, they're used in raw casts");

/*
 * esp_digital_signature_length_t is used in esp_ds_data_t in contrast to ets_ds_data_t, where unsigned is used.
 * Check esp_digital_signature_length_t's width here because it's converted to unsigned using raw casts.
 */
_Static_assert(sizeof(esp_digital_signature_length_t) == sizeof(unsigned),
        "The size of esp_digital_signature_length_t and unsigned has to be the same");

static void ds_acquire_enable(void) {
    /* Lock AES, SHA and RSA peripheral */
    esp_crypto_dma_lock_acquire();
    esp_crypto_mpi_lock_acquire();
    ets_hmac_enable();
    ets_ds_enable();
}

static void ds_disable_release(void) {
    ets_ds_disable();
    ets_hmac_disable();
    esp_crypto_mpi_lock_release();
    esp_crypto_dma_lock_release();
}

esp_err_t esp_ds_sign(const void *message,
        const esp_ds_data_t *data,
        hmac_key_id_t key_id,
        void *signature)
{
    // Need to check signature here, otherwise the signature is only checked when the signing has finished and fails
    // but the signing isn't uninitialized and the mutex is still locked.
    if (!signature) return ESP_ERR_INVALID_ARG;

    esp_ds_context_t *context;
    esp_err_t result = esp_ds_start_sign(message, data, key_id, &context);
    if (result != ESP_OK) return result;

    while (esp_ds_is_busy())
        vTaskDelay(ESP_DS_SIGN_TASK_DELAY_MS / portTICK_PERIOD_MS);

    return esp_ds_finish_sign(signature, context);
}

esp_err_t esp_ds_start_sign(const void *message,
        const esp_ds_data_t *data,
        hmac_key_id_t key_id,
        esp_ds_context_t **esp_ds_ctx)
{
    if (!message || !data || !esp_ds_ctx) return ESP_ERR_INVALID_ARG;
    if (key_id >= HMAC_KEY_MAX) return ESP_ERR_INVALID_ARG;
    if (!(data->rsa_length == ESP_DS_RSA_1024
            || data->rsa_length == ESP_DS_RSA_2048
            || data->rsa_length == ESP_DS_RSA_3072
            || data->rsa_length == ESP_DS_RSA_4096)) {
        return ESP_ERR_INVALID_ARG;
    }

    ds_acquire_enable();

    // initiate hmac
    int r = ets_hmac_calculate_downstream(ETS_EFUSE_BLOCK_KEY0 + (ets_efuse_block_t) key_id,
                                          ETS_EFUSE_KEY_PURPOSE_HMAC_DOWN_DIGITAL_SIGNATURE);
    if (r != ETS_OK) {
        ds_disable_release();
        return ESP_ERR_HW_CRYPTO_DS_HMAC_FAIL;
    }

    esp_ds_context_t *context = malloc(sizeof(esp_ds_context_t));
    if (!context) {
        ds_disable_release();
        return ESP_ERR_NO_MEM;
    }

    ets_ds_data_t *ds_data = (ets_ds_data_t*) data;

    // initiate signing
    ets_ds_result_t result = ets_ds_start_sign(message, ds_data);

    // ETS_DS_INVALID_PARAM only happens if a parameter is NULL or data->rsa_length is wrong
    // We checked all of that already
    assert(result != ETS_DS_INVALID_PARAM);

    if (result == ETS_DS_INVALID_KEY) {
        ds_disable_release();
        free(context);
        return ESP_ERR_HW_CRYPTO_DS_INVALID_KEY;
    }

    context->data = ds_data;
    *esp_ds_ctx = context;

    return ESP_OK;
}

bool esp_ds_is_busy(void)
{
    return ets_ds_is_busy();
}

esp_err_t esp_ds_finish_sign(void *signature, esp_ds_context_t *esp_ds_ctx)
{
    if (!signature || !esp_ds_ctx) return ESP_ERR_INVALID_ARG;

    const ets_ds_data_t *ds_data = esp_ds_ctx->data;

    ets_ds_result_t result = ets_ds_finish_sign(signature, ds_data);

    esp_err_t return_value = ESP_OK;

    // we checked all the parameters
    assert(result != ETS_DS_INVALID_PARAM);

    if (result == ETS_DS_INVALID_DIGEST) return_value = ESP_ERR_HW_CRYPTO_DS_INVALID_DIGEST;
    if (result == ETS_DS_INVALID_PADDING) return_value = ESP_ERR_HW_CRYPTO_DS_INVALID_PADDING;

    free(esp_ds_ctx);

    int res = ets_hmac_invalidate_downstream(ETS_EFUSE_KEY_PURPOSE_HMAC_DOWN_DIGITAL_SIGNATURE);
    assert(res == ETS_OK); // should not fail if called with correct purpose
    (void)res;

    ds_disable_release();

    return return_value;
}

esp_err_t esp_ds_encrypt_params(esp_ds_data_t *data,
        const void *iv,
        const esp_ds_p_data_t *p_data,
        const void *key)
{
    // p_data has to be valid, in internal memory and word aligned
    if (!p_data) return ESP_ERR_INVALID_ARG;
    assert(esp_ptr_internal(p_data) && esp_ptr_word_aligned(p_data));

    esp_err_t result = ESP_OK;

    esp_crypto_dma_lock_acquire();
    ets_aes_enable();
    ets_sha_enable();

    ets_ds_data_t *ds_data = (ets_ds_data_t*) data;
    const ets_ds_p_data_t *ds_plain_data = (const ets_ds_p_data_t*) p_data;

    ets_ds_result_t ets_result = ets_ds_encrypt_params(ds_data, iv, ds_plain_data, key, ETS_DS_KEY_HMAC);

    if (ets_result == ETS_DS_INVALID_PARAM) result = ESP_ERR_INVALID_ARG;

    ets_sha_disable();
    ets_aes_disable();
    esp_crypto_dma_lock_release();

    return result;
}
