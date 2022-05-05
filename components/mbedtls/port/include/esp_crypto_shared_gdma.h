/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "soc/lldesc.h"
#include "esp_private/gdma.h"
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Start a GDMA transfer on the shared crypto DMA channel
 *
 * @note Will allocate a GDMA channel for AES & SHA if no such channel is already allocated
 *
 * @param input Input linked list descriptor
 * @param output Output linked list descriptor
 * @param peripheral Crypto peripheral to connect the DMA to, either GDMA_TRIG_PERIPH_AES or
 *                   GDMA_TRIG_PERIPH_SHA
 * @return esp_err_t ESP_FAIL if no GDMA channel available
 */
esp_err_t esp_crypto_shared_gdma_start(const lldesc_t *input, const lldesc_t *output, gdma_trigger_peripheral_t peripheral);


/**
 * @brief Frees any shared crypto DMA channel, if esp_crypto_shared_gdma_start is called after
 *        this, new GDMA channels will be allocated.
 *
 * @note Function is meant to be called from user code, and thus takes AES/SHA lock.
 *       This means this function should not be called from code which already takes these locks,
 *       i.e. inside our AES/SHA code.
 *
 *       If you are continously using AES/SHA (e.g. because of a wifi connection) then it's not recommended
 *       to use this API. Freeing the channel is mainly for use cases where you are finished with the crypto peripherals
 *       and need the DMA channel for other peripherals. An example would be doing some processing after disconnecting WiFi
 */
void esp_crypto_shared_gdma_free(void);

#ifdef __cplusplus
}
#endif
