/*
 * SPDX-FileCopyrightText: 2020-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_crypto_dma.h"
#include "esp_aes_dma_priv.h"

#include "soc/soc_caps.h"
#include "soc/crypto_dma_reg.h"
#include "hal/crypto_dma_ll.h"


esp_err_t esp_aes_dma_start(const crypto_dma_desc_t *input, const crypto_dma_desc_t *output)
{
    crypto_dma_ll_reset();
    crypto_dma_ll_set_mode(CRYPTO_DMA_AES);

    /* Set descriptors, input to AES comes from outlink DMA and vice-versa */
    crypto_dma_ll_outlink_set((uint32_t)input);
    crypto_dma_ll_inlink_set((uint32_t)output);

    /* Start transfer */
    crypto_dma_ll_outlink_start();
    crypto_dma_ll_inlink_start();

    return ESP_OK;
}

bool esp_aes_dma_done(const crypto_dma_desc_t *output)
{
    return (crypto_dma_ll_inlink_is_eof() && (output->dw0.owner == 0));
}
