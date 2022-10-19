/*
 * SPDX-FileCopyrightText: 2020-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_private/periph_ctrl.h"
#include "esp_crypto_lock.h"
#include "hal/ds_hal.h"
#include "hal/ds_ll.h"
#include "hal/hmac_hal.h"
#include "esp32h4/rom/digital_signature.h"
#include "esp_timer.h"
#include "esp_ds.h"

struct esp_ds_context {
    const esp_ds_data_t *data;
};

/**
 * The vtask delay \c esp_ds_sign() is using while waiting for completion of the signing operation.
 */
#define ESP_DS_SIGN_TASK_DELAY_MS 10

#define RSA_LEN_MAX 127

/*
 * esp_digital_signature_length_t is used in esp_ds_data_t in contrast to ets_ds_data_t, where unsigned is used.
 * Check esp_digital_signature_length_t's width here because it's converted to unsigned using raw casts.
 */
_Static_assert(sizeof(esp_digital_signature_length_t) == sizeof(unsigned),
        "The size of esp_digital_signature_length_t and unsigned has to be the same");

/*
 * esp_ds_data_t is used in the encryption function but casted to ets_ds_data_t.
 * Check esp_ds_data_t's width here because it's converted using raw casts.
 */
_Static_assert(sizeof(esp_ds_data_t) == sizeof(ets_ds_data_t),
        "The size of esp_ds_data_t and ets_ds_data_t has to be the same");

static void ds_acquire_enable(void)
{
    esp_crypto_ds_lock_acquire();

    // We also enable SHA and HMAC here. SHA is used by HMAC, HMAC is used by DS.
    periph_module_enable(PERIPH_HMAC_MODULE);
    periph_module_enable(PERIPH_SHA_MODULE);
    periph_module_enable(PERIPH_DS_MODULE);

    hmac_hal_start();
}

static void ds_disable_release(void)
{
    ds_hal_finish();

    periph_module_disable(PERIPH_DS_MODULE);
    periph_module_disable(PERIPH_SHA_MODULE);
    periph_module_disable(PERIPH_HMAC_MODULE);

    esp_crypto_ds_lock_release();
}

esp_err_t esp_ds_sign(const void *message,
        const esp_ds_data_t *data,
        hmac_key_id_t key_id,
        void *signature)
{
    // Need to check signature here, otherwise the signature is only checked when the signing has finished and fails
    // but the signing isn't uninitialized and the mutex is still locked.
    if (!signature) {
        return ESP_ERR_INVALID_ARG;
    }

    esp_ds_context_t *context;
    esp_err_t result = esp_ds_start_sign(message, data, key_id, &context);
    if (result != ESP_OK) {
        return result;
    }

    while (esp_ds_is_busy())
        vTaskDelay(ESP_DS_SIGN_TASK_DELAY_MS / portTICK_PERIOD_MS);

    return esp_ds_finish_sign(signature, context);
}

esp_err_t esp_ds_start_sign(const void *message,
        const esp_ds_data_t *data,
        hmac_key_id_t key_id,
        esp_ds_context_t **esp_ds_ctx)
{
    if (!message || !data || !esp_ds_ctx) {
        return ESP_ERR_INVALID_ARG;
    }

    if (key_id >= HMAC_KEY_MAX) {
        return ESP_ERR_INVALID_ARG;
    }

    if (!(data->rsa_length == ESP_DS_RSA_1024
            || data->rsa_length == ESP_DS_RSA_2048
            || data->rsa_length == ESP_DS_RSA_3072)) {
        return ESP_ERR_INVALID_ARG;
    }

    ds_acquire_enable();

    // initiate hmac
    uint32_t conf_error = hmac_hal_configure(HMAC_OUTPUT_DS, key_id);
    if (conf_error) {
        ds_disable_release();
        return ESP_ERR_HW_CRYPTO_DS_HMAC_FAIL;
    }

    ds_hal_start();

    // check encryption key from HMAC
    int64_t start_time = esp_timer_get_time();
    while (ds_ll_busy() != 0) {
        if ((esp_timer_get_time() - start_time) > SOC_DS_KEY_CHECK_MAX_WAIT_US) {
            ds_disable_release();
            return ESP_ERR_HW_CRYPTO_DS_INVALID_KEY;
        }
    }

    esp_ds_context_t *context = malloc(sizeof(esp_ds_context_t));
    if (!context) {
        ds_disable_release();
        return ESP_ERR_NO_MEM;
    }

    size_t rsa_len = (data->rsa_length + 1) * 4;
    ds_hal_write_private_key_params(data->c);
    ds_hal_configure_iv(data->iv);
    ds_hal_write_message(message, rsa_len);

    // initiate signing
    ds_hal_start_sign();

    context->data = data;
    *esp_ds_ctx = context;

    return ESP_OK;
}

bool esp_ds_is_busy(void)
{
    return ds_hal_busy();
}

esp_err_t esp_ds_finish_sign(void *signature, esp_ds_context_t *esp_ds_ctx)
{
    if (!signature || !esp_ds_ctx) {
        return ESP_ERR_INVALID_ARG;
    }

    const esp_ds_data_t *data = esp_ds_ctx->data;
    unsigned rsa_len = (data->rsa_length + 1) * 4;

    while (ds_hal_busy()) { }

    ds_signature_check_t sig_check_result = ds_hal_read_result((uint8_t*) signature, (size_t) rsa_len);

    esp_err_t return_value = ESP_OK;

    if (sig_check_result == DS_SIGNATURE_MD_FAIL || sig_check_result == DS_SIGNATURE_PADDING_AND_MD_FAIL) {
        return_value = ESP_ERR_HW_CRYPTO_DS_INVALID_DIGEST;
    }

    if (sig_check_result == DS_SIGNATURE_PADDING_FAIL) {
        return_value = ESP_ERR_HW_CRYPTO_DS_INVALID_PADDING;
    }

    free(esp_ds_ctx);

    hmac_hal_clean();

    ds_disable_release();

    return return_value;
}

esp_err_t esp_ds_encrypt_params(esp_ds_data_t *data,
        const void *iv,
        const esp_ds_p_data_t *p_data,
        const void *key)
{
    if (!p_data) {
        return ESP_ERR_INVALID_ARG;
    }

    esp_err_t result = ESP_OK;

    esp_crypto_ds_lock_acquire();
    periph_module_enable(PERIPH_AES_MODULE);
    periph_module_enable(PERIPH_DS_MODULE);
    periph_module_enable(PERIPH_SHA_MODULE);
    periph_module_enable(PERIPH_HMAC_MODULE);
    periph_module_enable(PERIPH_RSA_MODULE);

    ets_ds_data_t *ds_data = (ets_ds_data_t*) data;
    const ets_ds_p_data_t *ds_plain_data = (const ets_ds_p_data_t*) p_data;

    ets_ds_result_t ets_result = ets_ds_encrypt_params(ds_data, iv, ds_plain_data, key, ETS_DS_KEY_HMAC);

    if (ets_result == ETS_DS_INVALID_PARAM) {
        result = ESP_ERR_INVALID_ARG;
    }

    periph_module_disable(PERIPH_RSA_MODULE);
    periph_module_disable(PERIPH_HMAC_MODULE);
    periph_module_disable(PERIPH_SHA_MODULE);
    periph_module_disable(PERIPH_DS_MODULE);
    periph_module_disable(PERIPH_AES_MODULE);
    esp_crypto_ds_lock_release();

    return result;
}
