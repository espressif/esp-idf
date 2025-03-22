/*
 * SPDX-FileCopyrightText: 2023-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <string.h>

#include "esp_err.h"

#include "mbedtls/aes.h"
#include "esp_crypto_dma.h"

#include "hal/clk_gate_ll.h"
#include "hal/gdma_ll.h"
#include "hal/gdma_types.h"
#include "hal/aes_hal.h"

#include "soc/lldesc.h"
#include "soc/periph_defs.h"
#include "soc/gdma_channel.h"
#include "soc/gdma_struct.h"
#include "soc/soc_caps.h"

#include "esp_tee_crypto_shared_gdma.h"
#include "esp_aes_dma_priv.h"
#include "esp_sha_dma_priv.h"

#define TEE_CRYPTO_GDMA_CH  (0)

/*
 * NOTE: [ESP-TEE] This is a low-level (LL), non-OS version of
 * port/crypto_shared_gdma/esp_crypto_shared_gdma.c that defines
 * the shared DMA layer for the AES and SHA peripherals for ESP-TEE.
 */

/* ---------------------------------------------- Shared GDMA layer for AES/SHA crypto ------------------------------------------------- */

static void crypto_shared_gdma_init(void)
{
    // enable APB to access GDMA registers
    periph_ll_enable_clk_clear_rst(PERIPH_GDMA_MODULE);

    // enable gdma clock
    gdma_ll_force_enable_reg_clock(&GDMA, true);

    // setting the transfer ability
    gdma_ll_tx_enable_data_burst(&GDMA, TEE_CRYPTO_GDMA_CH, true);
    gdma_ll_tx_enable_descriptor_burst(&GDMA, TEE_CRYPTO_GDMA_CH, true);

    gdma_ll_rx_enable_data_burst(&GDMA, TEE_CRYPTO_GDMA_CH, false);
    gdma_ll_rx_enable_descriptor_burst(&GDMA, TEE_CRYPTO_GDMA_CH, true);

#if SOC_GDMA_SUPPORT_PSRAM
    gdma_ll_tx_set_block_size_psram(&GDMA, TEE_CRYPTO_GDMA_CH, GDMA_LL_EXT_MEM_BK_SIZE_16B);
    gdma_ll_rx_set_block_size_psram(&GDMA, TEE_CRYPTO_GDMA_CH, GDMA_LL_EXT_MEM_BK_SIZE_16B);
#endif // SOC_GDMA_SUPPORT_PSRAM

    gdma_ll_tx_reset_channel(&GDMA, TEE_CRYPTO_GDMA_CH);
    gdma_ll_tx_connect_to_periph(&GDMA, TEE_CRYPTO_GDMA_CH, GDMA_TRIG_PERIPH_M2M, SOC_GDMA_TRIG_PERIPH_M2M0);

    gdma_ll_rx_reset_channel(&GDMA, TEE_CRYPTO_GDMA_CH);
    gdma_ll_rx_connect_to_periph(&GDMA, TEE_CRYPTO_GDMA_CH, GDMA_TRIG_PERIPH_M2M, SOC_GDMA_TRIG_PERIPH_M2M0);

}

esp_err_t esp_tee_crypto_shared_gdma_start(const crypto_dma_desc_t *input, const crypto_dma_desc_t *output, gdma_trigger_peripheral_t periph)
{
    int periph_inst_id = SOC_GDMA_TRIG_PERIPH_M2M0;
    if (periph == GDMA_TRIG_PERIPH_SHA) {
        periph_inst_id = SOC_GDMA_TRIG_PERIPH_SHA0;
    } else if (periph == GDMA_TRIG_PERIPH_AES) {
        periph_inst_id = SOC_GDMA_TRIG_PERIPH_AES0;
    } else {
        return ESP_ERR_INVALID_ARG;
    }

    crypto_shared_gdma_init();

    gdma_ll_tx_disconnect_from_periph(&GDMA, TEE_CRYPTO_GDMA_CH);
    gdma_ll_rx_disconnect_from_periph(&GDMA, TEE_CRYPTO_GDMA_CH);

    gdma_ll_tx_reset_channel(&GDMA, TEE_CRYPTO_GDMA_CH);
    gdma_ll_tx_connect_to_periph(&GDMA, TEE_CRYPTO_GDMA_CH, periph, periph_inst_id);

    gdma_ll_rx_reset_channel(&GDMA, TEE_CRYPTO_GDMA_CH);
    gdma_ll_rx_connect_to_periph(&GDMA, TEE_CRYPTO_GDMA_CH, periph, periph_inst_id);

    gdma_ll_tx_set_desc_addr(&GDMA, TEE_CRYPTO_GDMA_CH, (intptr_t)input);
    gdma_ll_tx_start(&GDMA, TEE_CRYPTO_GDMA_CH);

    gdma_ll_rx_set_desc_addr(&GDMA, TEE_CRYPTO_GDMA_CH, (intptr_t)output);
    gdma_ll_rx_start(&GDMA, TEE_CRYPTO_GDMA_CH);

    return ESP_OK;
}

void esp_tee_crypto_shared_gdma_free(void)
{
    gdma_ll_tx_stop(&GDMA, TEE_CRYPTO_GDMA_CH);
    gdma_ll_rx_stop(&GDMA, TEE_CRYPTO_GDMA_CH);

    gdma_ll_tx_disconnect_from_periph(&GDMA, TEE_CRYPTO_GDMA_CH);
    gdma_ll_rx_disconnect_from_periph(&GDMA, TEE_CRYPTO_GDMA_CH);

    gdma_ll_tx_set_priority(&GDMA, TEE_CRYPTO_GDMA_CH, 0);
    gdma_ll_rx_set_priority(&GDMA, TEE_CRYPTO_GDMA_CH, 0);

    // disable gdma clock
    gdma_ll_force_enable_reg_clock(&GDMA, false);

    // disable APB for GDMA registers
    periph_ll_disable_clk_set_rst(PERIPH_GDMA_MODULE);
}

/* ---------------------------------------------- DMA Implementations: AES ------------------------------------------------- */

esp_err_t esp_aes_dma_start(const crypto_dma_desc_t *input, const crypto_dma_desc_t *output)
{
    return esp_tee_crypto_shared_gdma_start(input, output, GDMA_TRIG_PERIPH_AES);
}

bool esp_aes_dma_done(const crypto_dma_desc_t *output)
{
    return (output->dw0.owner == 0);
}

/* ---------------------------------------------- DMA Implementations: SHA ------------------------------------------------- */

esp_err_t esp_sha_dma_start(const crypto_dma_desc_t *input)
{
    return esp_tee_crypto_shared_gdma_start(input, NULL, GDMA_TRIG_PERIPH_SHA);
}
