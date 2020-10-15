// Copyright 2015-2020 Espressif Systems (Shanghai) PTE LTD
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
    esp_err_t err;

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

    err = ets_hmac_calculate_downstream(convert_key_type(key_id), ETS_EFUSE_KEY_PURPOSE_HMAC_DOWN_JTAG);
    if (err != ETS_OK) {
        ESP_LOGE(TAG, "HMAC downstream JTAG enable mode setting failed.");
        return ESP_FAIL;
    }

    ESP_LOGD(TAG, "HMAC computation in downstream mode is completed.");

    ets_hmac_disable();

    esp_crypto_dma_lock_release();

    return ESP_OK;
}

esp_err_t esp_hmac_jtag_disable()
{
    esp_crypto_dma_lock_acquire();

    REG_WRITE(HMAC_SET_INVALIDATE_JTAG_REG, 1);

    esp_crypto_dma_lock_release();

    ESP_LOGD(TAG, "Invalidate JTAG result register. JTAG disabled.");

    return ESP_OK;
}
