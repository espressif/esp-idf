// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#pragma once
#warning "This header is deprecated, please use functions defined in esp_heap_caps.h instead."
#include "esp_heap_caps.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Deprecated FreeRTOS-style esp_heap_alloc_caps.h functions follow */

/* Please use heap_caps_malloc() instead of this function */
void *pvPortMallocCaps(size_t xWantedSize, uint32_t caps) asm("heap_caps_malloc") __attribute__((deprecated));

/* Please use heap_caps_get_minimum_free_heap_size() instead of this function */
size_t xPortGetMinimumEverFreeHeapSizeCaps( uint32_t caps ) asm("heap_caps_get_minimum_free_heap_size") __attribute__((deprecated));

/* Please use heap_caps_get_free_heap_size() instead of this function */
size_t xPortGetFreeHeapSizeCaps( uint32_t caps ) asm("heap_caps_get_free_heap_size") __attribute__((deprecated));

#ifdef __cplusplus
}
#endif
