/*
 * SPDX-FileCopyrightText: 2023-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <string.h>

#include "esp_err.h"
#include "soc/soc_caps.h"

#include "esp_crypto_dma.h"

#include "soc/soc_caps.h"

#include "esp_tee_crypto_shared_gdma.h"
#include "esp_aes_dma_priv.h"
#include "esp_sha_dma_priv.h"

#define TEE_CRYPTO_GDMA_CH  (0)

#if SOC_AHB_GDMA_VERSION == 2
#include "hal/ahb_dma_ll.h"
#include "soc/ahb_dma_struct.h"
#define DMA_DEV (AHB_DMA)
#define DMA_LL_FUNC(func) ahb_dma_ll_##func
#elif SOC_AHB_GDMA_VERSION == 1
#include "hal/gdma_ll.h"
#include "soc/gdma_struct.h"
#define DMA_DEV (GDMA)
#define DMA_LL_FUNC(func) gdma_ll_##func
#endif

#define dma_ll_force_enable_reg_clock       DMA_LL_FUNC(force_enable_reg_clock)
#define dma_ll_tx_enable_data_burst         DMA_LL_FUNC(tx_enable_data_burst)
#define dma_ll_tx_enable_descriptor_burst   DMA_LL_FUNC(tx_enable_descriptor_burst)
#define dma_ll_rx_enable_data_burst         DMA_LL_FUNC(rx_enable_data_burst)
#define dma_ll_rx_enable_descriptor_burst   DMA_LL_FUNC(rx_enable_descriptor_burst)
#define dma_ll_tx_reset_channel             DMA_LL_FUNC(tx_reset_channel)
#define dma_ll_tx_connect_to_periph         DMA_LL_FUNC(tx_connect_to_periph)
#define dma_ll_tx_connect_to_memory         DMA_LL_FUNC(tx_connect_to_mem)
#define dma_ll_rx_reset_channel             DMA_LL_FUNC(rx_reset_channel)
#define dma_ll_rx_connect_to_periph         DMA_LL_FUNC(rx_connect_to_periph)
#define dma_ll_rx_connect_to_memory         DMA_LL_FUNC(rx_connect_to_mem)
#define dma_ll_tx_disconnect_all            DMA_LL_FUNC(tx_disconnect_all)
#define dma_ll_rx_disconnect_all            DMA_LL_FUNC(rx_disconnect_all)
#define dma_ll_tx_set_desc_addr             DMA_LL_FUNC(tx_set_desc_addr)
#define dma_ll_tx_start                     DMA_LL_FUNC(tx_start)
#define dma_ll_rx_set_desc_addr             DMA_LL_FUNC(rx_set_desc_addr)
#define dma_ll_rx_start                     DMA_LL_FUNC(rx_start)
#define dma_ll_tx_stop                      DMA_LL_FUNC(tx_stop)
#define dma_ll_rx_stop                      DMA_LL_FUNC(rx_stop)
#define dma_ll_tx_set_priority              DMA_LL_FUNC(tx_set_priority)
#define dma_ll_rx_set_priority              DMA_LL_FUNC(rx_set_priority)
#if SOC_AHB_GDMA_VERSION == 2
#define dma_ll_tx_set_burst_size            DMA_LL_FUNC(tx_set_burst_size)
#define dma_ll_rx_set_burst_size            DMA_LL_FUNC(rx_set_burst_size)
#endif

/*
 * NOTE: [ESP-TEE] This is a low-level (LL), non-OS version of
 * port/crypto_shared_gdma/esp_crypto_shared_gdma.c that defines
 * the shared DMA layer for the AES and SHA peripherals for ESP-TEE.
 */

/* ---------------------------------------------- Shared GDMA layer for AES/SHA crypto ------------------------------------------------- */

static void crypto_shared_gdma_init(void)
{
    // enable gdma clock
    gdma_ll_enable_bus_clock(0, true);
    gdma_ll_reset_register(0);
    dma_ll_force_enable_reg_clock(&DMA_DEV, true);

    // setting the transfer ability
#if SOC_AHB_GDMA_VERSION == 2
    dma_ll_rx_enable_data_burst(&DMA_DEV, TEE_CRYPTO_GDMA_CH, true);
    dma_ll_rx_set_burst_size(&DMA_DEV, TEE_CRYPTO_GDMA_CH, 16);
    dma_ll_tx_set_burst_size(&DMA_DEV, TEE_CRYPTO_GDMA_CH, 16);
#else
    dma_ll_rx_enable_data_burst(&DMA_DEV, TEE_CRYPTO_GDMA_CH, false);
#endif

    dma_ll_tx_enable_data_burst(&DMA_DEV, TEE_CRYPTO_GDMA_CH, true);
    dma_ll_tx_enable_descriptor_burst(&DMA_DEV, TEE_CRYPTO_GDMA_CH, true);
    dma_ll_rx_enable_descriptor_burst(&DMA_DEV, TEE_CRYPTO_GDMA_CH, true);

    dma_ll_tx_reset_channel(&DMA_DEV, TEE_CRYPTO_GDMA_CH);
    dma_ll_tx_connect_to_memory(&DMA_DEV, TEE_CRYPTO_GDMA_CH, SOC_GDMA_TRIG_PERIPH_M2M0);

    dma_ll_rx_reset_channel(&DMA_DEV, TEE_CRYPTO_GDMA_CH);
    dma_ll_rx_connect_to_memory(&DMA_DEV, TEE_CRYPTO_GDMA_CH, SOC_GDMA_TRIG_PERIPH_M2M0);
}

esp_err_t esp_tee_crypto_shared_gdma_start(const crypto_dma_desc_t *input, const crypto_dma_desc_t *output, crypto_dma_user_t periph)
{
    int periph_inst_id = SOC_GDMA_TRIG_PERIPH_M2M0;
    switch (periph) {
#if SOC_SHA_SUPPORTED
    case CRYPTO_DMA_USER_SHA:
        periph_inst_id = SOC_GDMA_TRIG_PERIPH_SHA0;
        break;
#endif
#if SOC_AES_SUPPORTED
    case CRYPTO_DMA_USER_AES:
        periph_inst_id = SOC_GDMA_TRIG_PERIPH_AES0;
        break;
#endif
    default:
        return ESP_ERR_INVALID_ARG;
    }

    crypto_shared_gdma_init();

    dma_ll_tx_disconnect_all(&DMA_DEV, TEE_CRYPTO_GDMA_CH);
    dma_ll_rx_disconnect_all(&DMA_DEV, TEE_CRYPTO_GDMA_CH);

    dma_ll_tx_reset_channel(&DMA_DEV, TEE_CRYPTO_GDMA_CH);
    dma_ll_tx_connect_to_periph(&DMA_DEV, TEE_CRYPTO_GDMA_CH, periph_inst_id);

    dma_ll_rx_reset_channel(&DMA_DEV, TEE_CRYPTO_GDMA_CH);
    dma_ll_rx_connect_to_periph(&DMA_DEV, TEE_CRYPTO_GDMA_CH, periph_inst_id);

    dma_ll_tx_set_desc_addr(&DMA_DEV, TEE_CRYPTO_GDMA_CH, (intptr_t)input);
    dma_ll_tx_start(&DMA_DEV, TEE_CRYPTO_GDMA_CH);

    dma_ll_rx_set_desc_addr(&DMA_DEV, TEE_CRYPTO_GDMA_CH, (intptr_t)output);
    dma_ll_rx_start(&DMA_DEV, TEE_CRYPTO_GDMA_CH);

    return ESP_OK;
}

void esp_tee_crypto_shared_gdma_free(void)
{
    dma_ll_tx_stop(&DMA_DEV, TEE_CRYPTO_GDMA_CH);
    dma_ll_rx_stop(&DMA_DEV, TEE_CRYPTO_GDMA_CH);

    dma_ll_tx_disconnect_all(&DMA_DEV, TEE_CRYPTO_GDMA_CH);
    dma_ll_rx_disconnect_all(&DMA_DEV, TEE_CRYPTO_GDMA_CH);

    dma_ll_tx_set_priority(&DMA_DEV, TEE_CRYPTO_GDMA_CH, 0);
    dma_ll_rx_set_priority(&DMA_DEV, TEE_CRYPTO_GDMA_CH, 0);

    // disable gdma clock
    gdma_ll_enable_bus_clock(0, false);
    gdma_ll_reset_register(0);
    dma_ll_force_enable_reg_clock(&DMA_DEV, false);
}

/* ---------------------------------------------- DMA Implementations: AES ------------------------------------------------- */

#if SOC_AES_SUPPORTED
esp_err_t esp_aes_dma_start(const crypto_dma_desc_t *input, const crypto_dma_desc_t *output)
{
    return esp_tee_crypto_shared_gdma_start(input, output, CRYPTO_DMA_USER_AES);
}

bool esp_aes_dma_done(const crypto_dma_desc_t *output)
{
    return (output->dw0.owner == 0);
}
#endif

/* ---------------------------------------------- DMA Implementations: SHA ------------------------------------------------- */

#if SOC_SHA_SUPPORTED
esp_err_t esp_sha_dma_start(const crypto_dma_desc_t *input)
{
    return esp_tee_crypto_shared_gdma_start(input, NULL, CRYPTO_DMA_USER_SHA);
}
#endif
