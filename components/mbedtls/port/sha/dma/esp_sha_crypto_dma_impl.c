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


#include "esp_sha_dma_priv.h"

#include "soc/soc_caps.h"
#include "soc/crypto_dma_reg.h"
#include "hal/crypto_dma_ll.h"


esp_err_t esp_sha_dma_start(const lldesc_t *input)
{
    crypto_dma_ll_set_mode(CRYPTO_DMA_SHA);
    crypto_dma_ll_reset();

    crypto_dma_ll_outlink_set((intptr_t)input);
    crypto_dma_ll_outlink_start();

    return ESP_OK;
}
