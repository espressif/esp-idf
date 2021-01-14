// Copyright 2020 Espressif Systems (Shanghai) PTE LTD
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
#include "esp_aes_dma_priv.h"

#include <sys/lock.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "driver/periph_ctrl.h"
#include "hal/gdma_ll.h"
#include "soc/soc_caps.h"
#include "esp_private/gdma.h"
#include "esp_log.h"

#define NEW_CHANNEL_TIMEOUT_MS  1000
#define NEW_CHANNEL_DELAY_MS    100

static const char *TAG = "esp_aes_gdma";

static _lock_t gdma_ch_lock;
/* For GDMA we allocate and reserve a single DMA pair for AES at esp_aes_init
   and release it esp_aes_free
   This is done to avoid the GDMA associated overhead when doing multiple AES transforms in a row.

    The channel is shared between any AES operations that are running in parallel,
    access will be limited by the peripheral lock
 */
static uint8_t ref_counts;

/* The GDMA channel is protected from concurrent access by the general AES peripheral lock */
static gdma_channel_handle_t tx_channel;
static gdma_channel_handle_t rx_channel;

/* Allocate a new GDMA channel, will keep trying until NEW_CHANNEL_TIMEOUT_MS */
static inline esp_err_t esp_aes_gdma_new_channel(gdma_channel_alloc_config_t *channel_config, gdma_channel_handle_t *channel)
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

/* Initialize GDMA module and channels */
static inline void esp_aes_gdma_init(void)
{
    esp_err_t ret;

    gdma_channel_alloc_config_t channel_config = {
        .direction = GDMA_CHANNEL_DIRECTION_TX,
    };

    ret = esp_aes_gdma_new_channel(&channel_config, &tx_channel);
    if (ret != ESP_OK) {
        goto err;
    }

    channel_config.direction = GDMA_CHANNEL_DIRECTION_RX;
    ret = esp_aes_gdma_new_channel(&channel_config, &rx_channel);
    if (ret != ESP_OK) {
        gdma_del_channel(tx_channel); // Clean up already allocated TX channel
        goto err;
    }

    gdma_connect(tx_channel, GDMA_MAKE_TRIGGER(GDMA_TRIG_PERIPH_AES, 0));
    gdma_connect(rx_channel, GDMA_MAKE_TRIGGER(GDMA_TRIG_PERIPH_AES, 0));

    return;

err:
    /* mbedtls_aes_init do not have a way of signaling errors to the caller
       so we set the channel to NULL and detect it in esp_aes_dma_start */
    ESP_LOGE(TAG, "Failed to acquire DMA channel, Err=0x%X", ret);
    tx_channel = NULL;
    rx_channel = NULL;
}

void esp_aes_dma_init()
{

    _lock_acquire(&gdma_ch_lock);
    if (ref_counts == 0) {
        esp_aes_gdma_init();
    }
    ref_counts++;
    _lock_release(&gdma_ch_lock);
}

static inline void esp_aes_gdma_free(void)
{
    gdma_disconnect(tx_channel);
    gdma_disconnect(rx_channel);
    gdma_del_channel(tx_channel);
    gdma_del_channel(rx_channel);

    tx_channel = NULL;
    rx_channel = NULL;
}

void esp_aes_dma_free()
{
    _lock_acquire(&gdma_ch_lock);
    ref_counts--;
    if (ref_counts == 0) {
        esp_aes_gdma_free();
    }
    _lock_release(&gdma_ch_lock);
}


esp_err_t esp_aes_dma_start(const lldesc_t *input, const lldesc_t *output)
{
#if SOC_GDMA_SUPPORT_EXTMEM
    int tx_ch_id = 0;
    int rx_ch_id = 0;
#endif //SOC_GDMA_SUPPORT_EXTMEM

    if (!tx_channel || !rx_channel) {
        /* Will happen if no channel was acquired before timeout */
        return ESP_FAIL;
    }

#if SOC_GDMA_SUPPORT_EXTMEM
    gdma_get_channel_id(tx_channel, &tx_ch_id);
    gdma_get_channel_id(rx_channel, &rx_ch_id);
    /* An L2 FIFO bigger than 40 bytes is need when accessing external ram */
    gdma_ll_tx_extend_fifo_size_to(&GDMA, tx_ch_id, 40);
    gdma_ll_rx_extend_l2_fifo_size_to(&GDMA, rx_ch_id, 40);
    gdma_ll_tx_set_block_size_psram(&GDMA, tx_ch_id, GDMA_OUT_EXT_MEM_BK_SIZE_16B);
    gdma_ll_rx_set_block_size_psram(&GDMA, rx_ch_id, GDMA_OUT_EXT_MEM_BK_SIZE_16B);
#endif //SOC_GDMA_SUPPORT_EXTMEM

    gdma_start(tx_channel, (intptr_t)input);
    gdma_start(rx_channel, (intptr_t)output);

    return ESP_OK;
}



bool esp_aes_dma_done(const lldesc_t *output)
{
    return (output->owner == 0);
}
