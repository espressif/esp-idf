/*
 * SPDX-FileCopyrightText: 2020-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_aes_dma_priv.h"
#include "esp_crypto_shared_gdma.h"

esp_err_t esp_aes_dma_start(const lldesc_t *input, const lldesc_t *output)
{
    return esp_crypto_shared_gdma_start(input, output, GDMA_TRIG_PERIPH_AES);
}

bool esp_aes_dma_done(const lldesc_t *output)
{
    return (output->owner == 0);
}
