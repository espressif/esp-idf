// Copyright 2010-2019 Espressif Systems (Shanghai) PTE LTD
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

#pragma once
#include <stdbool.h>
#include "sdkconfig.h"

#if CONFIG_IDF_TARGET_ESP32
#include "esp32/rom/lldesc.h"
#elif CONFIG_IDF_TARGET_ESP32S2BETA
#include "esp32s2beta/rom/lldesc.h"
#endif

//the size field has 12 bits, but 0 not for 4096.
//to avoid possible problem when the size is not word-aligned, we only use 4096-4 per desc.
/** Maximum size of data in the buffer that a DMA descriptor can hold. */
#define LLDESC_MAX_NUM_PER_DESC (4096-4)

/**
 * Generate a linked list pointing to a (huge) buffer in an descriptor array.
 *
 * The caller should ensure there is enough size to hold the array, by calling
 * ``lldesc_get_required_num``.
 *
 * @param out_desc_array Output of a descriptor array, the head should be fed to the DMA.
 * @param buffer Buffer for the descriptors to point to.
 * @param size Size (or length for TX) of the buffer
 * @param isrx The RX DMA may require the buffer to be word-aligned, set to true for a RX link, otherwise false.
 */
void lldesc_setup_link(lldesc_t *out_desc_array, const void *buffer, int size, bool isrx);

/**
 * Get the number of descriptors required for a given buffer size.
 *
 * @param data_size Size to check descriptor num.
 *
 * @return Numbers required.
 */
static inline int lldesc_get_required_num(int data_size)
{
    return (data_size + LLDESC_MAX_NUM_PER_DESC - 1) / LLDESC_MAX_NUM_PER_DESC;
}
