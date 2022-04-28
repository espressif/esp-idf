/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include "esp32s2/rom/hmac.h"
#include "esp_hmac.h"
#include "esp_crypto_lock.h"
#include "esp_efuse.h"
#include "esp_efuse_table.h"
#include "soc/hwcrypto_reg.h"
#include "soc/system_reg.h"
#include "esp_log.h"

static const char *TAG = "esp_hmac";

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

    REG_WRITE(HMAC_SET_INVALIDATE_JTAG_REG, 1);

    esp_crypto_dma_lock_release();

    ESP_LOGD(TAG, "Invalidate JTAG result register. JTAG disabled.");

    return ESP_OK;
}
