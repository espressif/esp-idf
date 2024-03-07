/*
 * SPDX-FileCopyrightText: 2020-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */


#include "esp_sha_dma_priv.h"

#include "soc/soc_caps.h"
#include "soc/crypto_dma_reg.h"
#include "hal/crypto_dma_ll.h"


esp_err_t esp_sha_dma_start(const crypto_dma_desc_t *input)
{
    crypto_dma_ll_set_mode(CRYPTO_DMA_SHA);
    crypto_dma_ll_reset();

    crypto_dma_ll_outlink_set((intptr_t)input);
    crypto_dma_ll_outlink_start();

    return ESP_OK;
}
