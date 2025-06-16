/*
 * SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdarg.h>

#include "esp_fault.h"
#include "soc/soc_caps.h"

#include "hal/sha_hal.h"
#include "aes/esp_aes.h"
#include "sha/sha_core.h"
#include "esp_hmac.h"
#include "esp_ds.h"
#include "esp_crypto_periph_clk.h"
#include "ecc_impl.h"

#include "esp_tee.h"
#include "esp_tee_memory_utils.h"
#include "esp_tee_aes_intr.h"

#include "esp_tee_sec_storage.h"
#include "esp_tee_ota_ops.h"

static __attribute__((unused)) const char *TAG = "esp_tee_sec_srv";

void _ss_invalid_secure_service(void)
{
    assert(0);
}

/* ---------------------------------------------- AES ------------------------------------------------- */

void _ss_esp_aes_intr_alloc(void)
{
    esp_tee_aes_intr_alloc();
}

int _ss_esp_aes_crypt_cbc(esp_aes_context *ctx,
                          int mode,
                          size_t length,
                          unsigned char iv[16],
                          const unsigned char *input,
                          unsigned char *output)
{
    bool valid_addr = ((esp_tee_ptr_in_ree((void *)input) && esp_tee_ptr_in_ree((void *)output)) &&
                       (esp_tee_ptr_in_ree((void *)(input + length)) && esp_tee_ptr_in_ree((void *)(output + length))));

    if (!valid_addr) {
        return -1;
    }
    ESP_FAULT_ASSERT(valid_addr);

    return esp_aes_crypt_cbc(ctx, mode, length, iv, input, output);
}

int _ss_esp_aes_crypt_cfb128(esp_aes_context *ctx,
                             int mode,
                             size_t length,
                             size_t *iv_off,
                             unsigned char iv[16],
                             const unsigned char *input,
                             unsigned char *output)
{
    bool valid_addr = ((esp_tee_ptr_in_ree((void *)input) && esp_tee_ptr_in_ree((void *)output)) &&
                       (esp_tee_ptr_in_ree((void *)(input + length)) && esp_tee_ptr_in_ree((void *)(output + length))));

    if (!valid_addr) {
        return -1;
    }
    ESP_FAULT_ASSERT(valid_addr);

    return esp_aes_crypt_cfb128(ctx, mode, length, iv_off, iv, input, output);
}

int _ss_esp_aes_crypt_cfb8(esp_aes_context *ctx,
                           int mode,
                           size_t length,
                           unsigned char iv[16],
                           const unsigned char *input,
                           unsigned char *output)
{
    bool valid_addr = ((esp_tee_ptr_in_ree((void *)input) && esp_tee_ptr_in_ree((void *)output)) &&
                       (esp_tee_ptr_in_ree((void *)(input + length)) && esp_tee_ptr_in_ree((void *)(output + length))));

    if (!valid_addr) {
        return -1;
    }
    ESP_FAULT_ASSERT(valid_addr);

    return esp_aes_crypt_cfb8(ctx, mode, length, iv, input, output);
}

int _ss_esp_aes_crypt_ctr(esp_aes_context *ctx,
                          size_t length,
                          size_t *nc_off,
                          unsigned char nonce_counter[16],
                          unsigned char stream_block[16],
                          const unsigned char *input,
                          unsigned char *output)
{
    bool valid_addr = ((esp_tee_ptr_in_ree((void *)input) && esp_tee_ptr_in_ree((void *)output)) &&
                       (esp_tee_ptr_in_ree((void *)(input + length)) && esp_tee_ptr_in_ree((void *)(output + length))));

    if (!valid_addr) {
        return -1;
    }
    ESP_FAULT_ASSERT(valid_addr);

    return esp_aes_crypt_ctr(ctx, length, nc_off, nonce_counter, stream_block, input, output);
}

int _ss_esp_aes_crypt_ecb(esp_aes_context *ctx,
                          int mode,
                          const unsigned char input[16],
                          unsigned char output[16])
{
    bool valid_addr = ((esp_tee_ptr_in_ree((void *)input) && esp_tee_ptr_in_ree((void *)output)) &&
                       (esp_tee_ptr_in_ree((void *)(input + 16)) && esp_tee_ptr_in_ree((void *)(output + 16))));

    if (!valid_addr) {
        return -1;
    }
    ESP_FAULT_ASSERT(valid_addr);

    return esp_aes_crypt_ecb(ctx, mode, input, output);
}

int _ss_esp_aes_crypt_ofb(esp_aes_context *ctx,
                          size_t length,
                          size_t *iv_off,
                          unsigned char iv[16],
                          const unsigned char *input,
                          unsigned char *output)
{
    bool valid_addr = ((esp_tee_ptr_in_ree((void *)input) && esp_tee_ptr_in_ree((void *)output)) &&
                       (esp_tee_ptr_in_ree((void *)(input + length)) && esp_tee_ptr_in_ree((void *)(output + length))));

    if (!valid_addr) {
        return -1;
    }
    ESP_FAULT_ASSERT(valid_addr);

    return esp_aes_crypt_ofb(ctx, length, iv_off, iv, input, output);
}

/* ---------------------------------------------- SHA ------------------------------------------------- */

void _ss_esp_sha(esp_sha_type sha_type, const unsigned char *input, size_t ilen, unsigned char *output)
{
    bool valid_addr = ((esp_tee_ptr_in_ree((void *)input) && esp_tee_ptr_in_ree((void *)output)) &&
                       (esp_tee_ptr_in_ree((void *)(input + ilen))));

    if (!valid_addr) {
        return;
    }
    ESP_FAULT_ASSERT(valid_addr);

    esp_sha(sha_type, input, ilen, output);
}

int _ss_esp_sha_dma(esp_sha_type sha_type, const void *input, uint32_t ilen,
                    const void *buf, uint32_t buf_len, bool is_first_block)
{
    bool valid_addr = (esp_tee_ptr_in_ree((void *)input) &&
                       esp_tee_ptr_in_ree((void *)((char *)input + ilen)));
    if (buf_len) {
        valid_addr &= (esp_tee_ptr_in_ree((void *)buf) &&
                       esp_tee_ptr_in_ree((void *)((char *)buf + buf_len)));
    }

    if (!valid_addr) {
        return -1;
    }
    ESP_FAULT_ASSERT(valid_addr);

    return esp_sha_dma(sha_type, input, ilen, buf, buf_len, is_first_block);
}

void _ss_esp_sha_read_digest_state(esp_sha_type sha_type, void *digest_state)
{
    sha_hal_read_digest(sha_type, digest_state);
}

void _ss_esp_sha_write_digest_state(esp_sha_type sha_type, void *digest_state)
{
    sha_hal_write_digest(sha_type, digest_state);
}

void _ss_esp_sha_block(esp_sha_type sha_type, const void *data_block, bool is_first_block)
{
    esp_sha_block(sha_type, data_block, is_first_block);
}

void _ss_esp_crypto_sha_enable_periph_clk(bool enable)
{
    esp_crypto_sha_enable_periph_clk(enable);
}

/* ---------------------------------------------- HMAC ------------------------------------------------- */

esp_err_t _ss_esp_hmac_calculate(hmac_key_id_t key_id, const void *message, size_t message_len, uint8_t *hmac)
{
    bool valid_addr = ((esp_tee_ptr_in_ree((void *)message) && esp_tee_ptr_in_ree((void *)hmac)) &&
                       esp_tee_ptr_in_ree((void *)((char *)message + message_len)));

#if CONFIG_SECURE_TEE_SEC_STG_MODE_RELEASE
    valid_addr &= (key_id != (hmac_key_id_t)CONFIG_SECURE_TEE_SEC_STG_EFUSE_HMAC_KEY_ID);
#endif

    if (!valid_addr) {
        return ESP_ERR_INVALID_ARG;
    }
    ESP_FAULT_ASSERT(valid_addr);

    return esp_hmac_calculate(key_id, message, message_len, hmac);
}

esp_err_t _ss_esp_hmac_jtag_enable(hmac_key_id_t key_id, const uint8_t *token)
{
    bool valid_addr = (esp_tee_ptr_in_ree((void *)token));

#if CONFIG_SECURE_TEE_SEC_STG_MODE_RELEASE
    valid_addr &= (key_id != (hmac_key_id_t)CONFIG_SECURE_TEE_SEC_STG_EFUSE_HMAC_KEY_ID);
#endif

    if (!valid_addr) {
        return ESP_ERR_INVALID_ARG;
    }
    ESP_FAULT_ASSERT(valid_addr);

    return esp_hmac_jtag_enable(key_id, token);
}

esp_err_t _ss_esp_hmac_jtag_disable(void)
{
    return esp_hmac_jtag_disable();
}

esp_err_t _ss_esp_ds_sign(const void *message,
                          const esp_ds_data_t *data,
                          hmac_key_id_t key_id,
                          void *signature)
{
    bool valid_addr = (esp_tee_ptr_in_ree((void *)message) &&
                       esp_tee_ptr_in_ree((void *)data) &&
                       esp_tee_ptr_in_ree((void *)signature));

#if CONFIG_SECURE_TEE_SEC_STG_MODE_RELEASE
    valid_addr &= (key_id != (hmac_key_id_t)CONFIG_SECURE_TEE_SEC_STG_EFUSE_HMAC_KEY_ID);
#endif

    if (!valid_addr) {
        return ESP_ERR_INVALID_ARG;
    }
    ESP_FAULT_ASSERT(valid_addr);

    return esp_ds_sign(message, data, key_id, signature);
}

esp_err_t _ss_esp_ds_start_sign(const void *message,
                                const esp_ds_data_t *data,
                                hmac_key_id_t key_id,
                                esp_ds_context_t **esp_ds_ctx)
{
    bool valid_addr = (esp_tee_ptr_in_ree((void *)message) &&
                       esp_tee_ptr_in_ree((void *)data) &&
                       esp_tee_ptr_in_ree((void *)esp_ds_ctx));

#if CONFIG_SECURE_TEE_SEC_STG_MODE_RELEASE
    valid_addr &= (key_id != (hmac_key_id_t)CONFIG_SECURE_TEE_SEC_STG_EFUSE_HMAC_KEY_ID);
#endif

    if (!valid_addr) {
        return ESP_ERR_INVALID_ARG;
    }
    ESP_FAULT_ASSERT(valid_addr);

    return esp_ds_start_sign(message, data, key_id, esp_ds_ctx);
}

bool _ss_esp_ds_is_busy(void)
{
    return esp_ds_is_busy();
}

esp_err_t _ss_esp_ds_finish_sign(void *signature, esp_ds_context_t *esp_ds_ctx)
{
    bool valid_addr = (esp_tee_ptr_in_ree((void *)signature) &&
                       esp_tee_ptr_in_ree((void *)esp_ds_ctx));

    valid_addr &= esp_tee_ptr_in_ree((void *)((char *)esp_ds_ctx + sizeof(esp_ds_data_t)));

    if (!valid_addr) {
        return ESP_ERR_INVALID_ARG;
    }
    ESP_FAULT_ASSERT(valid_addr);

    return esp_ds_finish_sign(signature, esp_ds_ctx);
}

esp_err_t _ss_esp_ds_encrypt_params(esp_ds_data_t *data,
                                    const void *iv,
                                    const esp_ds_p_data_t *p_data,
                                    const void *key)
{
    bool valid_addr = ((esp_tee_ptr_in_ree((void *)data) && esp_tee_ptr_in_ree((void *)p_data)) &&
                       (esp_tee_ptr_in_ree((void *)iv) && esp_tee_ptr_in_ree((void *)key)));

    valid_addr &= esp_tee_ptr_in_ree((void *)((char *)data + sizeof(esp_ds_data_t)));

    if (!valid_addr) {
        return ESP_ERR_INVALID_ARG;
    }
    ESP_FAULT_ASSERT(valid_addr);

    return esp_ds_encrypt_params(data, iv, p_data, key);
}

/* ---------------------------------------------- MPI ------------------------------------------------- */

void _ss_esp_crypto_mpi_enable_periph_clk(bool enable)
{
    esp_crypto_mpi_enable_periph_clk(enable);
}

/* ---------------------------------------------- ECC ------------------------------------------------- */

int _ss_esp_ecc_point_multiply(const ecc_point_t *point, const uint8_t *scalar, ecc_point_t *result, bool verify_first)
{
    bool valid_addr = (esp_tee_ptr_in_ree((void *)result)) &&
                      esp_tee_ptr_in_ree((void *)((char *)result + sizeof(ecc_point_t)));

    if (!valid_addr) {
        return -1;
    }
    ESP_FAULT_ASSERT(valid_addr);

    return esp_ecc_point_multiply(point, scalar, result, verify_first);
}

int _ss_esp_ecc_point_verify(const ecc_point_t *point)
{
    return esp_ecc_point_verify(point);
}

void _ss_esp_crypto_ecc_enable_periph_clk(bool enable)
{
    esp_crypto_ecc_enable_periph_clk(enable);
}

/* ---------------------------------------------- OTA ------------------------------------------------- */

int _ss_esp_tee_ota_begin(void)
{
    return esp_tee_ota_begin();
}

int _ss_esp_tee_ota_write(uint32_t rel_offset, void *data, size_t size)
{
    bool valid_addr = ((esp_tee_ptr_in_ree((void *)data)) &&
                       (esp_tee_ptr_in_ree((void *)((char *)data + size))));

    if (!valid_addr) {
        return -1;
    }
    ESP_FAULT_ASSERT(valid_addr);

    return esp_tee_ota_write(rel_offset, data, size);
}

int _ss_esp_tee_ota_end(void)
{
    return esp_tee_ota_end();
}

/* ---------------------------------------------- Secure Storage ------------------------------------------------- */

esp_err_t _ss_esp_tee_sec_storage_clear_key(const char *key_id)
{
    return esp_tee_sec_storage_clear_key(key_id);
}

esp_err_t _ss_esp_tee_sec_storage_gen_key(const esp_tee_sec_storage_key_cfg_t *cfg)
{
    return esp_tee_sec_storage_gen_key(cfg);
}
