/*
 * SPDX-FileCopyrightText: 2015-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include "rom/efuse.h"
#include "rom/hmac.h"
#include "rom/ets_sys.h"
#include "esp_efuse.h"
#include "esp_efuse_table.h"
#include "esp_hmac.h"
#include "esp_log.h"
#include "esp_crypto_lock.h"
#include "esp_private/esp_crypto_lock_internal.h"
#include "soc/hwcrypto_reg.h"
#include "soc/system_reg.h"

#if !CONFIG_IDF_TARGET_ESP32S2
#include "hal/ds_ll.h"
#include "hal/hmac_hal.h"
#include "hal/hmac_ll.h"
#include "esp_private/periph_ctrl.h"
#endif

#define SHA256_BLOCK_SZ 64
#define SHA256_PAD_SZ 8

#if defined(CONFIG_IDF_TARGET_ESP32S3) || defined(CONFIG_IDF_TARGET_ESP32S2)
#define JTAG_STATUS_BIT ESP_EFUSE_HARD_DIS_JTAG
#else
/* For ESP32C3, ESP32C6, ESP32H2, ESP32P4 */
#define JTAG_STATUS_BIT ESP_EFUSE_DIS_PAD_JTAG
#endif
static const char *TAG = "esp_hmac";

#if !CONFIG_IDF_TARGET_ESP32S2
/**
 * @brief Apply the HMAC padding without the embedded length.
 *
 * @note This function does not check the data length, it is the responsibility of the other functions in this
 * module to make sure that \c data_len is at most SHA256_BLOCK_SZ - 1 so the padding fits in.
 * Otherwise, this function has undefined behavior.
 * Note however, that for the actual HMAC implementation, the length also needs to be applied at the end
 * of the block. This function alone deosn't do that.
 */
static void write_and_padd(uint8_t *block, const uint8_t *data, uint16_t data_len)
{
    memcpy(block, data, data_len);
    // Apply a one bit, followed by zero bits (refer to the TRM of respective target).
    block[data_len] = 0x80;
    bzero(block + data_len + 1, SHA256_BLOCK_SZ - data_len - 1);
}

esp_err_t esp_hmac_calculate(hmac_key_id_t key_id,
                             const void *message,
                             size_t message_len,
                             uint8_t *hmac)
{
    const uint8_t *message_bytes = (const uint8_t *)message;

    if (!message || !hmac) {
        return ESP_ERR_INVALID_ARG;
    }
    if (key_id >= HMAC_KEY_MAX) {
        return ESP_ERR_INVALID_ARG;
    }

    esp_crypto_hmac_lock_acquire();

    // We also enable SHA and DS here. SHA is used by HMAC, DS will otherwise hold SHA in reset state.
    HMAC_RCC_ATOMIC() {
        hmac_ll_enable_bus_clock(true);
        hmac_ll_reset_register();
    }

    periph_module_enable(PERIPH_SHA_MODULE);

    DS_RCC_ATOMIC() {
        ds_ll_enable_bus_clock(true);
        ds_ll_reset_register();
    }

    hmac_hal_start();

    uint32_t conf_error = hmac_hal_configure(HMAC_OUTPUT_USER, key_id);
    if (conf_error) {
        esp_crypto_hmac_lock_release();
        return ESP_FAIL;
    }

    if (message_len + 1 + SHA256_PAD_SZ <= SHA256_BLOCK_SZ) {
        // If message including padding is only one block...
        // Last message block, so apply SHA-256 padding rules in software
        uint8_t block[SHA256_BLOCK_SZ];
        uint64_t bit_len = __builtin_bswap64(message_len * 8 + 512);

        write_and_padd(block, message_bytes, message_len);
        // Final block: append the bit length in this block and signal padding to peripheral
        memcpy(block + SHA256_BLOCK_SZ - sizeof(bit_len),
               &bit_len, sizeof(bit_len));
        hmac_hal_write_one_block_512(block);
    } else {
        // If message including padding is needs more than one block

        // write all blocks without padding except the last one
        size_t remaining_blocks = message_len / SHA256_BLOCK_SZ;
        for (int i = 1; i < remaining_blocks; i++) {
            hmac_hal_write_block_512(message_bytes);
            message_bytes += SHA256_BLOCK_SZ;
            hmac_hal_next_block_normal();
        }

        // If message fits into one block but without padding, we must not write another block.
        if (remaining_blocks) {
            hmac_hal_write_block_512(message_bytes);
            message_bytes += SHA256_BLOCK_SZ;
        }

        size_t remaining = message_len % SHA256_BLOCK_SZ;
        // Last message block, so apply SHA-256 padding rules in software
        uint8_t block[SHA256_BLOCK_SZ];
        uint64_t bit_len = __builtin_bswap64(message_len * 8 + 512);

        // If the remaining message and appended padding doesn't fit into a single block, we have to write an
        // extra block with the rest of the message and potential padding first.
        if (remaining >= SHA256_BLOCK_SZ - SHA256_PAD_SZ) {
            write_and_padd(block, message_bytes, remaining);
            hmac_hal_next_block_normal();
            hmac_hal_write_block_512(block);
            bzero(block, SHA256_BLOCK_SZ);
        } else {
            write_and_padd(block, message_bytes, remaining);
        }
        memcpy(block + SHA256_BLOCK_SZ - sizeof(bit_len),
               &bit_len, sizeof(bit_len));
        hmac_hal_next_block_padding();
        hmac_hal_write_block_512(block);
    }

    // Read back result (bit swapped)
    hmac_hal_read_result_256(hmac);

    DS_RCC_ATOMIC() {
        ds_ll_enable_bus_clock(false);
    }

    periph_module_disable(PERIPH_SHA_MODULE);

    HMAC_RCC_ATOMIC() {
        hmac_ll_enable_bus_clock(false);
    }

    esp_crypto_hmac_lock_release();

    return ESP_OK;
}

static ets_efuse_block_t convert_key_type(hmac_key_id_t key_id) {
    return ETS_EFUSE_BLOCK_KEY0 + (ets_efuse_block_t) key_id;
}

esp_err_t esp_hmac_jtag_enable(hmac_key_id_t key_id, const uint8_t *token)
{
    int ets_status;
    esp_err_t err = ESP_OK;

    if ((!token) || (key_id >= HMAC_KEY_MAX))
        return ESP_ERR_INVALID_ARG;

    /* Check if JTAG is permanently disabled by HW Disable eFuse */
    if (esp_efuse_read_field_bit(JTAG_STATUS_BIT)) {
        ESP_LOGE(TAG, "JTAG disabled permanently.");
        return ESP_FAIL;
    }

    esp_crypto_hmac_lock_acquire();

    ets_status = ets_jtag_enable_temporarily(token, convert_key_type(key_id));

    if (ets_status != ETS_OK) {
        // ets_jtag_enable_temporarily returns either ETS_OK or ETS_FAIL
        err = ESP_FAIL;
        ESP_LOGE(TAG, "JTAG re-enabling failed (%d)", err);
    }

    ESP_LOGD(TAG, "HMAC computation in downstream mode is completed.");

    ets_hmac_disable();

    esp_crypto_hmac_lock_release();

    return err;
}

esp_err_t esp_hmac_jtag_disable()
{
    esp_crypto_hmac_lock_acquire();
    ets_hmac_enable();
    REG_WRITE(HMAC_SET_INVALIDATE_JTAG_REG, 1);
    ets_hmac_disable();
    esp_crypto_hmac_lock_release();

    ESP_LOGD(TAG, "Invalidate JTAG result register. JTAG disabled.");

    return ESP_OK;
}
#else /* !CONFIG_IDF_TARGET_ESP32S2 */

static ets_efuse_block_t convert_key_type(hmac_key_id_t key_id) {
    return ETS_EFUSE_BLOCK_KEY0 + (ets_efuse_block_t) key_id;
}

esp_err_t esp_hmac_calculate(hmac_key_id_t key_id,
        const void *message,
        size_t message_len,
        uint8_t *hmac)
{
    int hmac_ret;
    if (!message || !hmac) return ESP_ERR_INVALID_ARG;
    if (key_id >= HMAC_KEY_MAX) return ESP_ERR_INVALID_ARG;

    esp_crypto_dma_lock_acquire();

    ets_hmac_enable();
    hmac_ret = ets_hmac_calculate_message(convert_key_type(key_id), message, message_len, hmac);
    ets_hmac_disable();

    esp_crypto_dma_lock_release();

    if (hmac_ret != 0) {
        return ESP_FAIL;
    } else {
        return ESP_OK;
    }

}

esp_err_t esp_hmac_jtag_enable(hmac_key_id_t key_id, const uint8_t *token)
{
    int ets_status;
    esp_err_t err = ESP_OK;

    if ((!token) || (key_id >= HMAC_KEY_MAX))
        return ESP_ERR_INVALID_ARG;

    /* Check if JTAG is permanently disabled by HW Disable eFuse */
    if (esp_efuse_read_field_bit(ESP_EFUSE_HARD_DIS_JTAG)) {
        ESP_LOGE(TAG, "JTAG disabled permanently.");
        return ESP_FAIL;
    }

    esp_crypto_dma_lock_acquire();

    ets_hmac_enable();

    /* Token updating into HMAC module. */
    for (int i = 0; i < 32; i += 4) {
        uint32_t key_word;
        memcpy(&key_word, &token[i], 4);
        REG_WRITE(DPORT_JTAG_CTRL_0_REG + i, __builtin_bswap32(key_word));
    }

    ets_status = ets_hmac_calculate_downstream(convert_key_type(key_id), ETS_EFUSE_KEY_PURPOSE_HMAC_DOWN_JTAG);
    if (ets_status != ETS_OK) {
        err = ESP_FAIL;
        ESP_LOGE(TAG, "HMAC downstream JTAG enable mode setting failed. (%d)", err);
    }

    ESP_LOGD(TAG, "HMAC computation in downstream mode is completed.");

    ets_hmac_disable();

    esp_crypto_dma_lock_release();

    return err;
}

esp_err_t esp_hmac_jtag_disable()
{
    esp_crypto_dma_lock_acquire();

    ets_hmac_enable();
    REG_WRITE(HMAC_SET_INVALIDATE_JTAG_REG, 1);
    ets_hmac_disable();

    esp_crypto_dma_lock_release();

    ESP_LOGD(TAG, "Invalidate JTAG result register. JTAG disabled.");

    return ESP_OK;
}
#endif /* CONFIG_IDF_TARGET_ESP32S2*/
