/*
 * SPDX-FileCopyrightText: 2020-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdlib.h>
#include <string.h>
#include <assert.h>

#if !ESP_TEE_BUILD
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_timer.h"
#else
#include "esp_rom_sys.h"
#include "esp_cpu.h"
#endif

#include "soc/soc_caps.h"
#include "esp_ds.h"
#include "esp_crypto_lock.h"
#include "esp_crypto_periph_clk.h"
#include "esp_hmac.h"
#include "esp_memory_utils.h"
#if CONFIG_IDF_TARGET_ESP32S2
#include "esp32s2/rom/aes.h"
#include "esp32s2/rom/sha.h"
#include "esp32s2/rom/hmac.h"
#include "soc/soc_memory_layout.h"
#else /* CONFIG_IDF_TARGET_ESP32S2 */
#include "esp_private/periph_ctrl.h"
#include "hal/aes_ll.h"
#include "hal/ds_hal.h"
#include "hal/ds_ll.h"
#include "hal/hmac_hal.h"
#include "hal/hmac_ll.h"
#include "hal/sha_ll.h"
#endif /* !CONFIG_IDF_TARGET_ESP32S2 */

#ifdef SOC_KEY_MANAGER_DS_KEY_DEPLOY
#include "hal/key_mgr_hal.h"
#include "hal/key_mgr_ll.h"
#endif

/**
 * The vtask delay \c esp_ds_sign() is using while waiting for completion of the signing operation.
 */
#define ESP_DS_SIGN_TASK_DELAY_MS 10

#define RSA_LEN_MAX ((SOC_RSA_MAX_BIT_LEN/8) - 1)

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

#ifdef CONFIG_IDF_TARGET_ESP32S2

static void ds_acquire_enable(void)
{
    /* Lock AES, SHA and RSA peripheral */
    esp_crypto_dma_lock_acquire();
    esp_crypto_mpi_lock_acquire();
    ets_hmac_enable();
    ets_ds_enable();
}

static void ds_disable_release(void)
{
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
    if (!signature) {
        return ESP_ERR_INVALID_ARG;
    }

    esp_ds_context_t *context;
    esp_err_t result = esp_ds_start_sign(message, data, key_id, &context);
    if (result != ESP_OK) {
        return result;
    }

    while (esp_ds_is_busy()) {
        vTaskDelay(ESP_DS_SIGN_TASK_DELAY_MS / portTICK_PERIOD_MS);
    }

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
            || data->rsa_length == ESP_DS_RSA_3072
#if SOC_RSA_MAX_BIT_LEN == 4096
            || data->rsa_length == ESP_DS_RSA_4096
#endif
         )) {
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

    ets_ds_data_t *ds_data = (ets_ds_data_t *) data;

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

    context->data = (const ets_ds_data_t *)ds_data;
    *esp_ds_ctx = context;

    return ESP_OK;
}

bool esp_ds_is_busy(void)
{
    return ets_ds_is_busy();
}

esp_err_t esp_ds_finish_sign(void *signature, esp_ds_context_t *esp_ds_ctx)
{
    if (!signature || !esp_ds_ctx) {
        return ESP_ERR_INVALID_ARG;
    }

    const ets_ds_data_t *ds_data = esp_ds_ctx->data;

    ets_ds_result_t result = ets_ds_finish_sign(signature, ds_data);

    esp_err_t return_value = ESP_OK;

    // we checked all the parameters
    assert(result != ETS_DS_INVALID_PARAM);

    if (result == ETS_DS_INVALID_DIGEST) {
        return_value = ESP_ERR_HW_CRYPTO_DS_INVALID_DIGEST;
    }
    if (result == ETS_DS_INVALID_PADDING) {
        return_value = ESP_ERR_HW_CRYPTO_DS_INVALID_PADDING;
    }

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
    if (!p_data) {
        return ESP_ERR_INVALID_ARG;
    }
    assert(esp_ptr_internal(p_data) && esp_ptr_word_aligned(p_data));

    esp_err_t result = ESP_OK;

    esp_crypto_dma_lock_acquire();
    ets_aes_enable();
    ets_sha_enable();

    ets_ds_data_t *ds_data = (ets_ds_data_t *) data;
    const ets_ds_p_data_t *ds_plain_data = (const ets_ds_p_data_t *) p_data;

    ets_ds_result_t ets_result = ets_ds_encrypt_params(ds_data, iv, ds_plain_data, key, ETS_DS_KEY_HMAC);

    if (ets_result == ETS_DS_INVALID_PARAM) {
        result = ESP_ERR_INVALID_ARG;
    }

    ets_sha_disable();
    ets_aes_disable();
    esp_crypto_dma_lock_release();

    return result;
}

#else /* !CONFIG_IDF_TARGET_ESP32S2 (targets other than esp32s2) */

static inline int64_t get_time_us(void)
{
#if !ESP_TEE_BUILD
    return esp_timer_get_time();
#else
    return (int64_t)esp_cpu_get_cycle_count() / (int64_t)esp_rom_get_cpu_ticks_per_us();
#endif
}

static void ds_acquire_enable(void)
{
    esp_crypto_ds_lock_acquire();

    // We also enable SHA and HMAC here. SHA is used by HMAC, HMAC is used by DS.
    esp_crypto_hmac_enable_periph_clk(true);
    esp_crypto_sha_enable_periph_clk(true);
    esp_crypto_mpi_enable_periph_clk(true);
    esp_crypto_ds_enable_periph_clk(true);
}

static void ds_disable_release(void)
{
    esp_crypto_ds_enable_periph_clk(false);
    esp_crypto_mpi_enable_periph_clk(false);
    esp_crypto_sha_enable_periph_clk(false);
    esp_crypto_hmac_enable_periph_clk(false);

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

    esp_ds_context_t *context = NULL;
#if ESP_TEE_BUILD
    esp_ds_context_t ctx;
    context = &ctx;
#endif

    esp_err_t result = esp_ds_start_sign(message, data, key_id, &context);
    if (result != ESP_OK) {
        return result;
    }

    while (esp_ds_is_busy()) {
#if !ESP_TEE_BUILD
        vTaskDelay(ESP_DS_SIGN_TASK_DELAY_MS / portTICK_PERIOD_MS);
#else
        esp_rom_delay_us(1);
#endif
    }

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
            || data->rsa_length == ESP_DS_RSA_3072
#if SOC_RSA_MAX_BIT_LEN == 4096
            || data->rsa_length == ESP_DS_RSA_4096
#endif
         )) {
        return ESP_ERR_INVALID_ARG;
    }

    ds_acquire_enable();

#if SOC_KEY_MANAGER_DS_KEY_DEPLOY
    if (!key_mgr_ll_is_supported()) {
        assert(false && "Key manager is not supported");
    }

    if (key_id == HMAC_KEY_KM) {
        key_mgr_hal_set_key_usage(ESP_KEY_MGR_DS_KEY, ESP_KEY_MGR_USE_OWN_KEY);
        ds_hal_set_key_source(DS_KEY_SOURCE_KEY_MGR);
    } else {
        key_mgr_hal_set_key_usage(ESP_KEY_MGR_DS_KEY, ESP_KEY_MGR_USE_EFUSE_KEY);
        ds_hal_set_key_source(DS_KEY_SOURCE_EFUSE);
#endif
        // initiate hmac
        hmac_hal_start();
        uint32_t conf_error = hmac_hal_configure(HMAC_OUTPUT_DS, key_id);
        if (conf_error) {
            ds_disable_release();
            return ESP_ERR_HW_CRYPTO_DS_HMAC_FAIL;
        }
#if SOC_KEY_MANAGER_DS_KEY_DEPLOY
    }
#endif

    ds_hal_start();

    // check encryption key from HMAC
    int64_t start_time = get_time_us();
    while (ds_ll_busy() != 0) {
        if ((get_time_us() - start_time) > SOC_DS_KEY_CHECK_MAX_WAIT_US) {
            ds_hal_finish();
            ds_disable_release();
            return ESP_ERR_HW_CRYPTO_DS_INVALID_KEY;
        }
    }

#if !ESP_TEE_BUILD
    *esp_ds_ctx = malloc(sizeof(esp_ds_context_t));
#endif
    if (!*esp_ds_ctx) {
        ds_hal_finish();
        ds_disable_release();
        return ESP_ERR_NO_MEM;
    }

    size_t rsa_len = (data->rsa_length + 1) * 4;
    ds_hal_write_private_key_params(data->c);
    ds_hal_configure_iv((uint32_t *)data->iv);
    ds_hal_write_message(message, rsa_len);

    // initiate signing
    ds_hal_start_sign();

    (*esp_ds_ctx)->data = (const ets_ds_data_t *)data;

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

    const esp_ds_data_t *data = (const esp_ds_data_t *)esp_ds_ctx->data;
    unsigned rsa_len = (data->rsa_length + 1) * 4;

    while (ds_hal_busy()) { }

    ds_signature_check_t sig_check_result = ds_hal_read_result((uint8_t *) signature, (size_t) rsa_len);

    esp_err_t return_value = ESP_OK;

    if (sig_check_result == DS_SIGNATURE_MD_FAIL || sig_check_result == DS_SIGNATURE_PADDING_AND_MD_FAIL) {
        return_value = ESP_ERR_HW_CRYPTO_DS_INVALID_DIGEST;
    }

    if (sig_check_result == DS_SIGNATURE_PADDING_FAIL) {
        return_value = ESP_ERR_HW_CRYPTO_DS_INVALID_PADDING;
    }

#if !ESP_TEE_BUILD
    free(esp_ds_ctx);
#endif

    hmac_hal_clean();
    ds_hal_finish();

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

    // The `esp_ds_encrypt_params` operation does not use the Digital Signature peripheral,
    // but just the AES and SHA peripherals, so acquiring locks just for these peripherals
    // would be enough rather than acquiring a lock for the Digital Signature peripheral.
    esp_crypto_sha_aes_lock_acquire();

    esp_crypto_aes_enable_periph_clk(true);

    esp_crypto_sha_enable_periph_clk(true);

    ets_ds_data_t *ds_data = (ets_ds_data_t *) data;
    const ets_ds_p_data_t *ds_plain_data = (const ets_ds_p_data_t *) p_data;

    ets_ds_result_t ets_result = ets_ds_encrypt_params(ds_data, iv, ds_plain_data, key, ETS_DS_KEY_HMAC);

    if (ets_result == ETS_DS_INVALID_PARAM) {
        result = ESP_ERR_INVALID_ARG;
    }

    esp_crypto_sha_enable_periph_clk(false);

    esp_crypto_aes_enable_periph_clk(false);

    esp_crypto_sha_aes_lock_release();

    return result;
}
#endif
