// Copyright 2021 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "esp_crypto_shared_gdma.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "hal/gdma_ll.h"
#include "soc/soc_caps.h"
#include "esp_log.h"
#include "esp_err.h"
#include "esp_crypto_lock.h"

#define NEW_CHANNEL_TIMEOUT_MS  1000
#define NEW_CHANNEL_DELAY_MS    100

static const char *TAG = "crypto_shared_gdma";

static gdma_channel_handle_t rx_channel;
static gdma_channel_handle_t tx_channel;

/* Allocate a new GDMA channel, will keep trying until NEW_CHANNEL_TIMEOUT_MS */
static inline esp_err_t crypto_shared_gdma_new_channel(gdma_channel_alloc_config_t *channel_config, gdma_channel_handle_t *channel)
{
    esp_err_t ret;
    int time_waited_ms = 0;

    while(1) {
        ret = gdma_new_channel(channel_config, channel);

        if (ret == ESP_OK) {
            break;
        } else if (time_waited_ms >= NEW_CHANNEL_TIMEOUT_MS) {
            *channel = NULL;
            break;
        }

        time_waited_ms += NEW_CHANNEL_DELAY_MS;
        vTaskDelay(NEW_CHANNEL_DELAY_MS / portTICK_PERIOD_MS);
    }
    return ret;
}


#if SOC_GDMA_SUPPORT_EXTMEM
/* Initialize external memory specific DMA configs */
static void esp_crypto_shared_dma_init_extmem(void)
{
    int tx_ch_id = 0;
    int rx_ch_id = 0;

    gdma_get_channel_id(tx_channel, &tx_ch_id);
    gdma_get_channel_id(rx_channel, &rx_ch_id);

    /* An L2 FIFO bigger than 40 bytes is need when accessing external ram */
    gdma_ll_tx_extend_fifo_size_to(&GDMA, tx_ch_id, 40);
    gdma_ll_rx_extend_l2_fifo_size_to(&GDMA, rx_ch_id, 40);
    gdma_ll_tx_set_block_size_psram(&GDMA, tx_ch_id, GDMA_OUT_EXT_MEM_BK_SIZE_16B);
    gdma_ll_rx_set_block_size_psram(&GDMA, rx_ch_id, GDMA_OUT_EXT_MEM_BK_SIZE_16B);
}
#endif //SOC_GDMA_SUPPORT_EXTMEM

/* Initialize GDMA module and channels */
static esp_err_t crypto_shared_gdma_init(void)
{
    esp_err_t ret;

    gdma_channel_alloc_config_t channel_config_tx = {
        .direction = GDMA_CHANNEL_DIRECTION_TX,
    };

    gdma_channel_alloc_config_t channel_config_rx = {
        .direction = GDMA_CHANNEL_DIRECTION_RX,
    };

    ret = crypto_shared_gdma_new_channel(&channel_config_tx, &tx_channel);
    if (ret != ESP_OK) {
        goto err;
    }

    ret = crypto_shared_gdma_new_channel(&channel_config_rx, &rx_channel);
    if (ret != ESP_OK) {
        gdma_del_channel(tx_channel); // Clean up already allocated TX channel
        goto err;
    }

#if SOC_GDMA_SUPPORT_EXTMEM
    esp_crypto_shared_dma_init_extmem();
#endif //SOC_GDMA_SUPPORT_EXTMEM

    gdma_connect(rx_channel, GDMA_MAKE_TRIGGER(GDMA_TRIG_PERIPH_AES, 0));
    gdma_connect(tx_channel, GDMA_MAKE_TRIGGER(GDMA_TRIG_PERIPH_AES, 0));

    return ESP_OK;

err:
    ESP_LOGE(TAG, "Failed to acquire DMA channel, Err=0x%X", ret);
    tx_channel = NULL;
    rx_channel = NULL;

    return ret;
}


esp_err_t esp_crypto_shared_gdma_start(const lldesc_t *input, const lldesc_t *output, gdma_trigger_peripheral_t peripheral)
{
    int rx_ch_id = 0;
    esp_err_t ret = ESP_OK;

    if (tx_channel == NULL) {
        /* Allocate a pair of RX and TX for crypto, should only happen the first time we use the GMDA
           or if user called esp_crypto_shared_gdma_release */
        ret = crypto_shared_gdma_init();
    }

    if (ret != ESP_OK) {
        return ret;
    }

    /* Tx channel is shared between AES and SHA, need to connect to peripheral every time */
    gdma_disconnect(tx_channel);

    if (peripheral == GDMA_TRIG_PERIPH_SHA) {
        gdma_connect(tx_channel, GDMA_MAKE_TRIGGER(GDMA_TRIG_PERIPH_SHA, 0));
    } else if (peripheral == GDMA_TRIG_PERIPH_AES) {
        gdma_connect(tx_channel, GDMA_MAKE_TRIGGER(GDMA_TRIG_PERIPH_AES, 0));
    } else {
        return ESP_ERR_INVALID_ARG;
    }

  /* tx channel is reset by gdma_connect(), also reset rx to ensure a known state */
    gdma_get_channel_id(tx_channel, &rx_ch_id);
    gdma_ll_rx_reset_channel(&GDMA, rx_ch_id);

    gdma_start(tx_channel, (intptr_t)input);
    gdma_start(rx_channel, (intptr_t)output);

    return ESP_OK;
}

void esp_crypto_shared_gdma_free()
{
    esp_crypto_sha_aes_lock_acquire();

    if (rx_channel != NULL) {
        gdma_disconnect(rx_channel);
        gdma_del_channel(rx_channel);
        rx_channel = NULL;
    }

    if (tx_channel != NULL) {
        gdma_disconnect(tx_channel);
        gdma_del_channel(tx_channel);
        tx_channel = NULL;
    }

    esp_crypto_sha_aes_lock_release();
}
