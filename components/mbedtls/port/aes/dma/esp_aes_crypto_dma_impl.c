// Copyright 2020 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.


#include "esp_aes_dma_priv.h"

#include "soc/soc_caps.h"
#include "soc/crypto_dma_reg.h"
#include "hal/crypto_dma_ll.h"


esp_err_t esp_aes_dma_start(const lldesc_t *input, const lldesc_t *output)
{
    crypto_dma_ll_reset();
    crypto_dma_ll_set_mode(CRYPTO_DMA_AES);

    /* Set descriptors, input to AES comes from outlink DMA and viceversa */
    crypto_dma_ll_outlink_set((uint32_t)input);
    crypto_dma_ll_inlink_set((uint32_t)output);

    /* Start transfer */
    crypto_dma_ll_outlink_start();
    crypto_dma_ll_inlink_start();

    return ESP_OK;
}

bool esp_aes_dma_done(const lldesc_t *output)
{
    return (crypto_dma_ll_inlink_is_eof() && (output->owner == 0));
}
