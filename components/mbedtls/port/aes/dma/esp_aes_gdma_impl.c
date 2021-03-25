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
#include "hal/gdma_ll.h"
#include "esp_crypto_shared_gdma.h"

esp_err_t esp_aes_dma_start(const lldesc_t *input, const lldesc_t *output)
{
    return esp_crypto_shared_gdma_start(input, output, GDMA_TRIG_PERIPH_AES);
}

bool esp_aes_dma_done(const lldesc_t *output)
{
    return (output->owner == 0);
}
