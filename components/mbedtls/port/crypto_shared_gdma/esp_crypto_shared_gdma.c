/*
 * SPDX-FileCopyrightText: 2021-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_crypto_shared_gdma.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_err.h"
#include "esp_cache.h"
#include "esp_crypto_dma.h"
#include "esp_crypto_lock.h"
#include "soc/soc_caps.h"

#if SOC_AHB_GDMA_VERSION == 1
#include "hal/gdma_ll.h"
#elif SOC_AXI_GDMA_SUPPORTED
#include "hal/axi_dma_ll.h"
#endif /* SOC_AHB_GDMA_VERSION */

#define NEW_CHANNEL_TIMEOUT_MS  1000
#define NEW_CHANNEL_DELAY_MS    100

static const char *TAG = "crypto_shared_gdma";

static gdma_channel_handle_t rx_channel;
static gdma_channel_handle_t tx_channel;

/* Allocate a new GDMA channel, will keep trying until NEW_CHANNEL_TIMEOUT_MS */
static inline esp_err_t crypto_shared_gdma_new_channel(gdma_channel_alloc_config_t *channel_config, gdma_channel_handle_t *channel)
{
    esp_err_t ret = ESP_FAIL;
    int time_waited_ms = 0;

    while (1) {
#if SOC_AXI_GDMA_SUPPORTED
        ret = gdma_new_axi_channel(channel_config, channel);
#else /* !SOC_AXI_GDMA_SUPPORTED */
        ret = gdma_new_ahb_channel(channel_config, channel);
#endif /* SOC_AXI_GDMA_SUPPORTED */
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
static esp_err_t crypto_shared_gdma_init(void)
{
    esp_err_t ret;

    gdma_channel_alloc_config_t channel_config_tx = {
        .direction = GDMA_CHANNEL_DIRECTION_TX,
    };

    gdma_channel_alloc_config_t channel_config_rx = {
        .direction = GDMA_CHANNEL_DIRECTION_RX,
    };

    gdma_transfer_ability_t transfer_ability = {
        .sram_trans_align = 1,
        .psram_trans_align = 16,
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


    gdma_set_transfer_ability(tx_channel, &transfer_ability);
    gdma_set_transfer_ability(rx_channel, &transfer_ability);

    gdma_connect(rx_channel, GDMA_MAKE_TRIGGER(GDMA_TRIG_PERIPH_AES, 0));
    gdma_connect(tx_channel, GDMA_MAKE_TRIGGER(GDMA_TRIG_PERIPH_AES, 0));

    return ESP_OK;

err:
    ESP_LOGE(TAG, "Failed to acquire DMA channel, Err=%d", ret);
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
    gdma_get_channel_id(rx_channel, &rx_ch_id);

#if SOC_AHB_GDMA_VERSION == 1
    gdma_ll_rx_reset_channel(&GDMA, rx_ch_id);
#endif /* SOC_AHB_GDMA_VERSION */

    gdma_start(tx_channel, (intptr_t)input);
    gdma_start(rx_channel, (intptr_t)output);

    return ESP_OK;
}

esp_err_t esp_crypto_shared_gdma_start_axi_ahb(const crypto_dma_desc_t *input, const crypto_dma_desc_t *output, gdma_trigger_peripheral_t peripheral)
{
    int rx_ch_id = 0;

    if (tx_channel == NULL) {
        /* Allocate a pair of RX and TX for crypto, should only happen the first time we use the GMDA
           or if user called esp_crypto_shared_gdma_release */
        esp_err_t ret = crypto_shared_gdma_init();
        if (ret != ESP_OK) {
            return ret;
        }
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
    gdma_get_channel_id(rx_channel, &rx_ch_id);

#if SOC_AHB_GDMA_VERSION == 1
    gdma_ll_rx_reset_channel(&GDMA, rx_ch_id);
#elif SOC_AXI_GDMA_SUPPORTED
    axi_dma_ll_rx_reset_channel(&AXI_DMA, rx_ch_id);
#endif /* SOC_AHB_GDMA_VERSION */

    gdma_start(tx_channel, (intptr_t)input);
    gdma_start(rx_channel, (intptr_t)output);

    return ESP_OK;
}

#if SOC_AXI_GDMA_SUPPORTED
bool esp_crypto_shared_gdma_done(void)
{
    int rx_ch_id = 0;
    gdma_get_channel_id(rx_channel, &rx_ch_id);
    while(1) {
        if ((axi_dma_ll_rx_get_interrupt_status(&AXI_DMA, rx_ch_id, true) & 1)) {
            break;
        }
    }
    return true;
}
#endif /* SOC_AXI_GDMA_SUPPORTED */

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
