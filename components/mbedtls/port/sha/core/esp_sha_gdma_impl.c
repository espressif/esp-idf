/*
 * SPDX-FileCopyrightText: 2020-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_sha_dma_priv.h"
#include "esp_crypto_shared_gdma.h"

esp_err_t esp_sha_dma_start(const crypto_dma_desc_t *input)
{
    return esp_crypto_shared_gdma_start_axi_ahb(input, NULL, CRYPTO_DMA_USER_SHA);
}
