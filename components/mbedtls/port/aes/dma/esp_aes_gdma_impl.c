/*
 * SPDX-FileCopyrightText: 2020-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "soc/soc_caps.h"
#include "esp_aes_dma_priv.h"
#include "esp_crypto_dma.h"
#include "esp_crypto_shared_gdma.h"

esp_err_t esp_aes_dma_start(const crypto_dma_desc_t *input, const crypto_dma_desc_t *output)
{
    return esp_crypto_shared_gdma_start_axi_ahb(input, output, CRYPTO_DMA_USER_AES);
}

bool esp_aes_dma_done(const crypto_dma_desc_t *output)
{
#if SOC_AXI_GDMA_SUPPORTED && SOC_CACHE_INTERNAL_MEM_VIA_L1CACHE
    return esp_crypto_shared_gdma_done();
#else
    return (output->dw0.owner == 0);
#endif
}
