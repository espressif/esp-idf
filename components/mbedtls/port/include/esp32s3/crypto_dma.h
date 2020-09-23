/**
 *  Copyright (C) 2006-2015, ARM Limited, All Rights Reserved
 *  Additions Copyright (C) 2016, Espressif Systems (Shanghai) PTE Ltd
 *  SPDX-License-Identifier: Apache-2.0
 *
 *  Licensed under the Apache License, Version 2.0 (the "License"); you may
 *  not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 *  WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *
 */

#ifndef ESP_CRYPTO_DMA_H
#define ESP_CRYPTO_DMA_H

#include <freertos/FreeRTOS.h>

#ifdef __cplusplus
extern "C" {
#endif


/* Since crypto DMA is shared between DMA-AES and SHA blocks
 * Needs to be taken by respective blocks before using Crypto DMA
 */
extern _lock_t crypto_dma_lock;

#ifdef __cplusplus
}
#endif

#endif /* crypto_dma.h */
