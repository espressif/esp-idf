/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */


#include <string.h>
#include "sdkconfig.h"
#include "esp_ds.h"
#include "rsa_dec_alt.h"
#include "mbedtls/private/rsa.h"
#include "psa/crypto.h"
#include "esp_ds_common.h"
#include "esp_log.h"

static const char *TAG = "ESP_RSA_DEC_ALT";

#define MIN_V15_PADDING_LEN 11

static int esp_ds_rsaes_pkcs1_v15_unpadding(unsigned char *input,
    size_t ilen,
    unsigned char *output,
    size_t output_max_len,
    size_t *olen)
{
    if (ilen < MIN_V15_PADDING_LEN) {
        return MBEDTLS_ERR_CIPHER_INVALID_PADDING;
    }

    unsigned char bad = 0;
    size_t msg_len = 0;
    size_t msg_max_len = 0;
    unsigned char pad_done = 0;
    size_t pad_count = 0;

    msg_max_len = (output_max_len > ilen - MIN_V15_PADDING_LEN) ? ilen - MIN_V15_PADDING_LEN : output_max_len;

    /* Check the first byte (0x00) */
    bad |= input[0];

    /* Check the padding type */
    bad |= input[1] ^ 2; // MBEDTLS_RSA_CRYPT;

    /* Scan for separator (0x00) and count padding bytes in constant time */
    for (size_t i = 2; i < ilen; i++) {
        unsigned char found = (input[i] == 0x00);
        pad_done = pad_done | found;
        pad_count += (pad_done == 0) ? 1 : 0;
    }

    /* Check if we found a separator and padding is long enough */
    bad |= (pad_done == 0);  /* No separator found */
    bad |= (pad_count < 8);  /* Padding too short (need at least 8 non-zero bytes) */

    /* Calculate message length */
    msg_len = ilen - pad_count - 3;

    /* Check if separator is not at the very end */
    bad |= (msg_len > output_max_len);
    if (bad) {
        msg_len = msg_max_len;
    }

    /* Verify padding bytes are non-zero in constant time */
#if defined(__clang__) && defined(__xtensa__)
    #pragma clang loop vectorize(disable)
#endif
    for (size_t i = 2; i < ilen; i++) {
        unsigned char in_padding = (i < pad_count + 2);
        unsigned char is_zero = (input[i] == 0x00);
        bad |= in_padding & is_zero;
    }

    if (bad) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    *olen = msg_len;
    if (*olen > 0) {
        memcpy(output, input + ilen - msg_len, msg_len);
    }
    return 0;
}

static int esp_ds_compute_hash(mbedtls_md_type_t md_alg,
    const unsigned char *input, size_t ilen,
    unsigned char *output)
{
    const mbedtls_md_info_t *md_info = mbedtls_md_info_from_type(md_alg);
    if (md_info == NULL) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }
    return mbedtls_md(md_info, input, ilen, output);
}

static int esp_ds_rsaes_pkcs1_v21_unpadding(unsigned char *input,
    size_t ilen,
    unsigned char *output,
    size_t output_max_len,
    size_t *olen)
{
    int ret;
    unsigned int hlen = mbedtls_md_get_size_from_type(MBEDTLS_MD_SHA256);
    unsigned char bad = 0;
    size_t pad_len = 0;
    size_t msg_len = 0;

    /* Validate input length */
    bad |= (ilen < 2 * hlen + 2);

    /* Apply MGF masks */
    ret = esp_ds_mgf_mask(input + 1, hlen, input + hlen + 1, ilen - hlen - 1, MBEDTLS_MD_SHA256);
    if (ret != 0) {
        ESP_LOGE(TAG, "Error in MGF mask, returned %d", ret);
        return ret;
    }

    ret = esp_ds_mgf_mask(input + hlen + 1, ilen - hlen - 1, input + 1, hlen, MBEDTLS_MD_SHA256);
    if (ret != 0) {
        ESP_LOGE(TAG, "Error in MGF mask, returned %d", ret);
        return ret;
    }

    /* Check first byte (should be 0x00) */
    bad |= input[0];

    /* Skip the first byte and maskSeed */
    unsigned char *db = input + 1 + hlen;
    size_t db_len = ilen - hlen - 1;

    /* Compute hash, label is NULL and label_len is 0 */
    unsigned char lhash[MBEDTLS_MD_MAX_SIZE];
    ret = esp_ds_compute_hash(MBEDTLS_MD_SHA256, NULL, 0, lhash);
    if (ret != 0) {
        ESP_LOGE(TAG, "Error in compute_hash, returned %d", ret);
        return ret;
    }

    /* Verify hash portion of db against lhash */
    for (size_t i = 0; i < hlen && i < db_len; i++) {
        bad |= db[i] ^ lhash[i];
    }

    /* Skip past lhash in DB */
    unsigned char *p = db + hlen;
    size_t remaining = db_len - hlen;

    /* Get zero-padding len, following mbedTLS pattern
     * Always read till end of buffer (minus one, for the 01 byte) */
    unsigned char in_padding = 1;
    for (size_t i = 0; i < remaining - 1; i++) {
        unsigned char is_zero = (p[i] == 0);
        in_padding = in_padding & is_zero;
        pad_len += in_padding;
    }

    p += pad_len;
    bad |= (*p != 0x01);
    p++;

    /* Calculate message length */
    msg_len = remaining - pad_len - 1;

    /* Check output buffer size */
    bad |= (output_max_len < msg_len);

    if (bad) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    /* Copy message in constant time */
    *olen = msg_len;
    if (*olen > 0) {
        memcpy(output, p, msg_len);
    }

    return 0;
}

int esp_ds_rsa_decrypt(void *ctx, size_t *olen,
    const unsigned char *input, unsigned char *output,
    size_t output_max_len)
{
    int padding = MBEDTLS_PK_RSA_PKCS_V15;

    if (ctx != NULL) {
        mbedtls_rsa_context *rsa_ctx = (mbedtls_rsa_context *)ctx;
        if (rsa_ctx->MBEDTLS_PRIVATE(padding) == MBEDTLS_RSA_PKCS_V15) {
            padding = MBEDTLS_RSA_PKCS_V15;
        } else if (rsa_ctx->MBEDTLS_PRIVATE(padding) == MBEDTLS_RSA_PKCS_V21) {
            padding = MBEDTLS_RSA_PKCS_V21;
        } else {
            ESP_LOGE(TAG, "Unsupported padding type %d", rsa_ctx->MBEDTLS_PRIVATE(padding));
            return -1;
        }
    }

    size_t ilen = esp_ds_get_keylen(ctx);
    if (ilen == 0) {
        ESP_LOGE(TAG, "Invalid RSA key length");
        return -1;
    }
    size_t data_len = ilen / 4;

    uint32_t *output_tmp = NULL;

    uint32_t *input_tmp = calloc(data_len, sizeof(uint32_t));
    if (input_tmp == NULL) {
        ESP_LOGE(TAG, "Could not allocate memory for internal DS operations");
        return -1;
    }

    for (unsigned int i = 0; i < (data_len); i++) {
        input_tmp[i] = SWAP_INT32(((uint32_t *)input)[(data_len) - (i + 1)]);
    }

    esp_ds_data_t *s_ds_data = esp_ds_get_data_ctx();
    if (s_ds_data == NULL) {
        ESP_LOGE(TAG, "s_ds_data is NULL, cannot perform decryption");
        free(input_tmp);
        return -1;
    }
    hmac_key_id_t s_esp_ds_hmac_key_id = esp_ds_get_hmac_key_id();

    esp_ds_context_t *esp_ds_ctx = NULL;
    esp_err_t ds_r = esp_ds_start_sign((const void *)input_tmp,
            s_ds_data,
            s_esp_ds_hmac_key_id,
            &esp_ds_ctx);

    if (ds_r != ESP_OK) {
        ESP_LOGE(TAG, "Error in esp_ds_start_sign, returned %x ", ds_r);
        goto exit;
    }

    ds_r = esp_ds_finish_sign((void *)input_tmp, esp_ds_ctx);
    if (ds_r != ESP_OK) {
        if (ds_r == ESP_ERR_HW_CRYPTO_DS_INVALID_DIGEST) {
            ESP_LOGE(TAG, "Invalid digest in DS data reported by esp_ds_finish_sign");
        } else {
            ESP_LOGE(TAG, "Error in esp_ds_finish_sign, returned %x ", ds_r);
        }
        goto exit;
    }

    esp_ds_release_ds_lock();

    output_tmp = calloc(data_len, sizeof(uint32_t));
    if (output_tmp == NULL) {
        ESP_LOGE(TAG, "Could not allocate memory for output_tmp");
        goto exit;
    }

    for (unsigned int i = 0; i < (data_len); i++) {
        ((uint32_t *)output_tmp)[i] = SWAP_INT32(((uint32_t *)input_tmp)[(data_len) - (i + 1)]);
    }

    // Unpad the decrypted data
    if (padding == MBEDTLS_PK_RSA_PKCS_V15) {
        if (esp_ds_rsaes_pkcs1_v15_unpadding((uint8_t *)output_tmp, ilen, (uint8_t *)output_tmp, ilen, olen) != 0) {
            ESP_LOGE(TAG, "Error in v15 unpadding");
            goto exit;
        }
    } else if (padding == MBEDTLS_PK_RSA_PKCS_V21) {
        if (esp_ds_rsaes_pkcs1_v21_unpadding((uint8_t *)output_tmp, ilen, (uint8_t *)output_tmp, ilen, olen) != 0) {
            ESP_LOGE(TAG, "Error in v21 unpadding");
            goto exit;
        }
    } else {
        ESP_LOGE(TAG, "Unsupported padding type %d", padding);
        goto exit;
    }

    // Copy the decrypted data to output buffer
    if (output_max_len < *olen) {
        ESP_LOGE(TAG, "Output buffer is too small, output_max_len: %zu, olen: %zu", output_max_len, *olen);
        goto exit;
    }

    memcpy(output, output_tmp, *olen);
    free(output_tmp);
    free(input_tmp);
    return 0;
exit:
    esp_ds_release_ds_lock();
    if (input_tmp) {
        free(input_tmp);
    }
    if (output_tmp) {
        free(output_tmp);
    }
    if (olen) {
        *olen = 0; // Set olen to 0 in case of error
    }
    return -1;
}
