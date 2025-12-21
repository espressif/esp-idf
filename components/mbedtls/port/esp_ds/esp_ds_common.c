/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "sdkconfig.h"
#include "esp_ds.h"
#include "esp_log.h"
#include "esp_ds_common.h"
#include "esp_memory_utils.h"
#include "esp_ds/esp_ds_rsa.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "psa/crypto.h"

#ifdef SOC_DIG_SIGN_SUPPORTED
#include "rom/digital_signature.h"
#else
#error   "Selected target does not support DS peripheral"
#endif

#define FACTOR_KEYLEN_IN_BYTES 4

static const char *TAG = "ESP_DS_COMMON";

static hmac_key_id_t s_esp_ds_hmac_key_id = -1;
static esp_ds_data_t *s_ds_data = NULL;
static SemaphoreHandle_t s_ds_lock = NULL;
static int s_timeout_ms = 0;

esp_ds_data_t *esp_ds_get_data_ctx(void)
{
    return s_ds_data;
}

hmac_key_id_t esp_ds_get_hmac_key_id(void)
{
    return s_esp_ds_hmac_key_id;
}

size_t esp_ds_get_keylen(void *ctx)
{
    if (s_ds_data == NULL) {
        ESP_LOGE(TAG, "s_ds_data is NULL, cannot get key length");
        return 0;
    }
    /* calculating the rsa_length in bytes */
    return ((s_ds_data->rsa_length + 1) * FACTOR_KEYLEN_IN_BYTES);
}

size_t esp_ds_get_keylen_alt(mbedtls_pk_context *ctx)
{
    if (s_ds_data == NULL) {
        ESP_LOGE(TAG, "s_ds_data is NULL, cannot get key length");
        return 0;
    }
    /* calculating the rsa_length in bytes */
    return ((s_ds_data->rsa_length + 1) * FACTOR_KEYLEN_IN_BYTES);
}

/* Lock for the DS session, other TLS connections trying to use the DS peripheral will be blocked
 * till this DS session is completed (i.e. TLS handshake for this connection is completed) */
static void __attribute__((constructor)) esp_ds_conn_lock(void)
{
    if ((s_ds_lock = xSemaphoreCreateMutex()) == NULL) {
        ESP_EARLY_LOGE(TAG, "mutex for the DS session lock could not be created");
    }
}

void esp_ds_release_ds_lock(void)
{
    if (s_ds_lock == NULL) {
        ESP_LOGE(TAG, "s_ds_lock is NULL, cannot release lock");
        return;
    }
    if (xSemaphoreGetMutexHolder(s_ds_lock) == xTaskGetCurrentTaskHandle()) {
        /* Give back the semaphore (DS lock) */
        xSemaphoreGive(s_ds_lock);
    }
}

void esp_ds_set_session_timeout(int timeout)
{
    /* add additional offset of 1000 ms to have enough time for deleting the TLS connection and free the previous ds context after exceeding timeout value (this offset also helps when timeout is set to 0) */
    if (timeout > s_timeout_ms) {
        s_timeout_ms = timeout + 1000;
    }
}

esp_err_t esp_ds_init_data_ctx(esp_ds_data_ctx_t *ds_data)
{
    if (ds_data == NULL || ds_data->esp_ds_data == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    /* mutex is given back when the DS context is freed after the TLS handshake is completed or in case of failure (at cleanup) */
    if ((xSemaphoreTake(s_ds_lock, s_timeout_ms / portTICK_PERIOD_MS) != pdTRUE)) {
        ESP_LOGE(TAG, "ds_lock could not be obtained in specified time");
        return ESP_FAIL;
    }
    s_ds_data = ds_data->esp_ds_data;
    ESP_LOGD(TAG, "Using DS with key block %u, RSA length %u", ds_data->efuse_key_id, ds_data->rsa_length_bits);
    s_esp_ds_hmac_key_id = (hmac_key_id_t) ds_data->efuse_key_id;

    const unsigned rsa_length_int = (ds_data->rsa_length_bits / 32) - 1;
    if (esp_ptr_byte_accessible(s_ds_data)) {
        /* calculate the rsa_length in terms of esp_digital_signature_length_t which is required for the internal DS API */
        s_ds_data->rsa_length = rsa_length_int;
    } else if (s_ds_data->rsa_length != rsa_length_int) {
        /*
         * Configuration data is most likely from DROM segment and it
         * is not properly formatted for all parameters consideration.
         * Moreover, we can not modify as it is read-only and hence
         * the error.
         */
        ESP_LOGE(TAG, "RSA length mismatch %u, %u", s_ds_data->rsa_length, rsa_length_int);
        return ESP_ERR_INVALID_ARG;
    }

    return ESP_OK;
}

esp_err_t esp_ds_deinit_data_ctx(void)
{
    esp_ds_release_ds_lock();
    s_ds_data = NULL;
    s_esp_ds_hmac_key_id = -1;

    return ESP_OK;
}

int esp_ds_mgf_mask(unsigned char *dst, size_t dlen, unsigned char *src,
                    size_t slen, mbedtls_md_type_t md_alg)
{
    unsigned char counter[4];
    unsigned char *p;
    unsigned int hlen;
    size_t i, use_len;
    unsigned char mask[MBEDTLS_MD_MAX_SIZE];
    int ret = 0;
    const mbedtls_md_info_t *md_info;
    mbedtls_md_context_t md_ctx;

    mbedtls_md_init(&md_ctx);
    md_info = mbedtls_md_info_from_type(md_alg);
    if (md_info == NULL) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    if ((ret = mbedtls_md_setup(&md_ctx, md_info, 0)) != 0) {
        goto exit;
    }

    hlen = mbedtls_md_get_size(md_info);

    memset(mask, 0, sizeof(mask));
    memset(counter, 0, 4);

    /* Generate and apply dbMask */
    p = dst;

    while (dlen > 0) {
        use_len = hlen;
        if (dlen < hlen) {
            use_len = dlen;
        }

        if ((ret = mbedtls_md_starts(&md_ctx)) != 0) {
            goto exit;
        }
        if ((ret = mbedtls_md_update(&md_ctx, src, slen)) != 0) {
            goto exit;
        }
        if ((ret = mbedtls_md_update(&md_ctx, counter, 4)) != 0) {
            goto exit;
        }
        if ((ret = mbedtls_md_finish(&md_ctx, mask)) != 0) {
            goto exit;
        }

        for (i = 0; i < use_len; ++i) {
            *p++ ^= mask[i];
        }

        counter[3]++;

        dlen -= use_len;
    }

exit:
    mbedtls_platform_zeroize(mask, sizeof(mask));
    mbedtls_md_free(&md_ctx);

    return ret;
}

int esp_ds_hash_mprime(const unsigned char *hash, size_t hlen,
                       const unsigned char *salt, size_t slen,
                       unsigned char *out, mbedtls_md_type_t md_alg)
{
    const unsigned char zeros[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };

    mbedtls_md_context_t md_ctx;
    int ret = PSA_ERROR_INVALID_ARGUMENT;

    const mbedtls_md_info_t *md_info = mbedtls_md_info_from_type(md_alg);
    if (md_info == NULL) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    mbedtls_md_init(&md_ctx);
    if ((ret = mbedtls_md_setup(&md_ctx, md_info, 0)) != 0) {
        goto exit;
    }
    if ((ret = mbedtls_md_starts(&md_ctx)) != 0) {
        goto exit;
    }
    if ((ret = mbedtls_md_update(&md_ctx, zeros, sizeof(zeros))) != 0) {
        goto exit;
    }
    if ((ret = mbedtls_md_update(&md_ctx, hash, hlen)) != 0) {
        goto exit;
    }
    if ((ret = mbedtls_md_update(&md_ctx, salt, slen)) != 0) {
        goto exit;
    }
    if ((ret = mbedtls_md_finish(&md_ctx, out)) != 0) {
        goto exit;
    }

exit:
    mbedtls_md_free(&md_ctx);

    return ret;
}
