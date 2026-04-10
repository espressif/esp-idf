/*
 * SPDX-FileCopyrightText: 2021-2025 Espressif Systems (Shanghai) CO LTD
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
#include "esp_memory_utils.h"
#include "soc/soc_caps.h"
#include "sdkconfig.h"

#ifdef SOC_GDMA_EXT_MEM_ENC_ALIGNMENT
#include "hal/efuse_hal.h"
#endif /* SOC_GDMA_EXT_MEM_ENC_ALIGNMENT */

#if SOC_AXI_GDMA_SUPPORTED
#include "hal/axi_dma_ll.h"
#elif SOC_AHB_GDMA_VERSION == 1
#include "hal/gdma_ll.h"
#elif SOC_AHB_GDMA_VERSION == 2
#include "hal/ahb_dma_ll.h"
#endif /* SOC_AHB_GDMA_VERSION */

#define NEW_CHANNEL_TIMEOUT_MS  1000
#define NEW_CHANNEL_DELAY_MS    100

static const char *TAG = "crypto_shared_gdma";

static gdma_channel_handle_t s_rx_channel;
static gdma_channel_handle_t s_tx_channel;

/* Allocate a new GDMA channel, will keep trying until NEW_CHANNEL_TIMEOUT_MS */
static inline esp_err_t crypto_shared_gdma_new_channel(gdma_channel_alloc_config_t *channel_config,
                                                        gdma_channel_handle_t *tx_channel, gdma_channel_handle_t *rx_channel)
{
    esp_err_t ret = ESP_FAIL;
    int time_waited_ms = 0;

    while (1) {
#if SOC_AXI_GDMA_SUPPORTED
        ret = gdma_new_axi_channel(channel_config, tx_channel, rx_channel);
#else /* !SOC_AXI_GDMA_SUPPORTED */
        ret = gdma_new_ahb_channel(channel_config, tx_channel, rx_channel);
#endif /* SOC_AXI_GDMA_SUPPORTED */
        if (ret == ESP_OK) {
            break;
        } else if (time_waited_ms >= NEW_CHANNEL_TIMEOUT_MS) {
            if (tx_channel && *tx_channel) *tx_channel = NULL;
            if (rx_channel && *rx_channel) *rx_channel = NULL;
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

    gdma_channel_alloc_config_t channel_config = {0};

    // Allocate TX and RX channels separately (they don't need to be in the same pair)
    ret = crypto_shared_gdma_new_channel(&channel_config, &s_tx_channel, NULL);
    if (ret != ESP_OK) {
        goto err;
    }

    ret = crypto_shared_gdma_new_channel(&channel_config, NULL, &s_rx_channel);
    if (ret != ESP_OK) {
        gdma_del_channel(s_tx_channel); // Clean up already allocated TX channel
        goto err;
    }

    gdma_transfer_config_t transfer_cfg = {
        .max_data_burst_size = 16,
        .access_ext_mem = true, // crypto peripheral may want to access PSRAM
    };
    gdma_config_transfer(s_tx_channel, &transfer_cfg);

    /* When using AHB-GDMA version 1, the max data burst size must be 0, otherwise buffers need to be aligned as well.
     * Whereas, in case of the other GDMA versions, the RX max burst size is default enabled, but with default burst size of 4,
     * but it case of Flash Encryption, the buffers can be allocated from the external memory, which requires 16 byte alignment.
     * Thus, we set the max data burst size to 16, similar to the TX channel.
     */
#if SOC_AHB_GDMA_VERSION == 1 || SOC_AXI_GDMA_SUPPORTED // IDF-14335: SOC_AXI_GDMA_SUPPORTED might not be needed here
    transfer_cfg.max_data_burst_size = 0;
#endif

    gdma_config_transfer(s_rx_channel, &transfer_cfg);

#ifdef SOC_AES_SUPPORTED
    gdma_connect(s_rx_channel, GDMA_MAKE_TRIGGER(GDMA_TRIG_PERIPH_AES, 0));
    gdma_connect(s_tx_channel, GDMA_MAKE_TRIGGER(GDMA_TRIG_PERIPH_AES, 0));
#elif SOC_SHA_SUPPORTED
    gdma_connect(s_tx_channel, GDMA_MAKE_TRIGGER(GDMA_TRIG_PERIPH_SHA, 0));
#endif

    return ESP_OK;

err:
    ESP_LOGE(TAG, "Failed to acquire DMA channel, Err=%d", ret);
    s_tx_channel = NULL;
    s_rx_channel = NULL;

    return ret;
}


/* The external memory ecc-aes access must be enabled when there exists
   at least one buffer in the DMA descriptors that resides in external memory. */
#if (SOC_GDMA_EXT_MEM_ENC_ALIGNMENT && SOC_AXI_GDMA_SUPPORTED)
static bool check_dma_descs_need_ext_mem_ecc_aes_access(const crypto_dma_desc_t *dmadesc)
{
    crypto_dma_desc_t* desc = (crypto_dma_desc_t*) dmadesc;
    while (desc) {
        if (esp_ptr_in_drom(desc->buffer) || esp_ptr_external_ram(desc->buffer)) {
            return true;
        }
        desc = desc->next;
    }
    return false;
}
#endif /* (SOC_GDMA_EXT_MEM_ENC_ALIGNMENT && SOC_AXI_GDMA_SUPPORTED) */

esp_err_t esp_crypto_shared_gdma_start_axi_ahb(const crypto_dma_desc_t *input, const crypto_dma_desc_t *output, crypto_dma_user_t peripheral)
{
    int rx_ch_id = 0;

    if (s_tx_channel == NULL) {
        /* Allocate a pair of RX and TX for crypto, should only happen the first time we use the GMDA
           or if user called esp_crypto_shared_gdma_release */
        esp_err_t ret = crypto_shared_gdma_init();
        if (ret != ESP_OK) {
            return ret;
        }
    }

    /* Tx channel is shared between AES and SHA, need to connect to peripheral every time */
    gdma_disconnect(s_tx_channel);

#ifdef SOC_SHA_SUPPORTED
    if (peripheral == CRYPTO_DMA_USER_SHA) {
        gdma_connect(s_tx_channel, GDMA_MAKE_TRIGGER(GDMA_TRIG_PERIPH_SHA, 0));
    } else
#endif // SOC_SHA_SUPPORTED
#ifdef SOC_AES_SUPPORTED
    if (peripheral == CRYPTO_DMA_USER_AES) {
        gdma_connect(s_tx_channel, GDMA_MAKE_TRIGGER(GDMA_TRIG_PERIPH_AES, 0));
    } else
#endif // SOC_AES_SUPPORTED
    {
        return ESP_ERR_INVALID_ARG;
    }

    /* tx channel is reset by gdma_connect(), also reset rx to ensure a known state */
    gdma_get_channel_id(s_rx_channel, &rx_ch_id);

    // IDF-14335: Use gdma_reset() instead
#if SOC_AXI_GDMA_SUPPORTED
    axi_dma_ll_rx_reset_channel(&AXI_DMA, rx_ch_id);
#elif SOC_AHB_GDMA_VERSION == 1
    gdma_ll_rx_reset_channel(&GDMA, rx_ch_id);
#elif SOC_AHB_GDMA_VERSION == 2
    ahb_dma_ll_rx_reset_channel(&AHB_DMA, rx_ch_id);
#endif /* SOC_AXI_GDMA_SUPPORTED */

/* When GDMA operations are carried out using external memory with external memory encryption enabled,
   we need to enable AXI-DMA's AES-ECC mean access bit. */
#if (SOC_GDMA_EXT_MEM_ENC_ALIGNMENT && SOC_AXI_GDMA_SUPPORTED)
    if (efuse_hal_flash_encryption_enabled()) {
        int tx_ch_id = 0;
        gdma_get_channel_id(s_tx_channel, &tx_ch_id);

        if (check_dma_descs_need_ext_mem_ecc_aes_access(input) || check_dma_descs_need_ext_mem_ecc_aes_access(output)) {
            axi_dma_ll_rx_enable_ext_mem_ecc_aes_access(&AXI_DMA, rx_ch_id, true);
            axi_dma_ll_tx_enable_ext_mem_ecc_aes_access(&AXI_DMA, tx_ch_id, true);
        } else {
            axi_dma_ll_rx_enable_ext_mem_ecc_aes_access(&AXI_DMA, rx_ch_id, false);
            axi_dma_ll_tx_enable_ext_mem_ecc_aes_access(&AXI_DMA, tx_ch_id, false);
        }
    }
#endif /* SOC_GDMA_EXT_MEM_ENC_ALIGNMENT */

    gdma_start(s_tx_channel, (intptr_t)input);
    gdma_start(s_rx_channel, (intptr_t)output);

    return ESP_OK;
}

#if SOC_AXI_GDMA_SUPPORTED
bool esp_crypto_shared_gdma_done(void)
{
    int rx_ch_id = 0;
    gdma_get_channel_id(s_rx_channel, &rx_ch_id);
    while (1) {
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

    if (s_rx_channel != NULL) {
        gdma_disconnect(s_rx_channel);
        gdma_del_channel(s_rx_channel);
        s_rx_channel = NULL;
    }

    if (s_tx_channel != NULL) {
        gdma_disconnect(s_tx_channel);
        gdma_del_channel(s_tx_channel);
        s_tx_channel = NULL;
    }

    esp_crypto_sha_aes_lock_release();
}
