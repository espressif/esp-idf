/*
 * SPDX-FileCopyrightText: 2020-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_sha_dma_priv.h"
#include "esp_crypto_shared_gdma.h"

esp_err_t esp_sha_dma_start(const lldesc_t *input)
{
    return esp_crypto_shared_gdma_start(input, NULL, GDMA_TRIG_PERIPH_SHA);
}
