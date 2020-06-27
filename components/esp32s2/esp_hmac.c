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

#include "esp32s2/rom/hmac.h"
#include "esp32s2/rom/ets_sys.h"
#include "esp_hmac.h"
#include "esp_crypto_lock.h"

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

    if (hmac_ret != ETS_OK) {
        return ESP_FAIL;
    } else {
        return ESP_OK;
    }

}
