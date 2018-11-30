// Copyright 2018 Espressif Systems (Shanghai) PTE LTD
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

#include <esp_attr.h>
#include <esp_heap_caps.h>
#include <sdkconfig.h>
#include "esp_mem.h"

#ifndef CONFIG_MBEDTLS_CUSTOM_MEM_ALLOC

IRAM_ATTR void *esp_mbedtls_mem_calloc(size_t n, size_t size)
{
#ifdef CONFIG_MBEDTLS_INTERNAL_MEM_ALLOC
    return heap_caps_calloc(n, size, MALLOC_CAP_INTERNAL|MALLOC_CAP_8BIT);
#elif CONFIG_MBEDTLS_EXTERNAL_MEM_ALLOC
    return heap_caps_calloc(n, size, MALLOC_CAP_SPIRAM|MALLOC_CAP_8BIT);
#else
    return calloc(n, size);
#endif
}

IRAM_ATTR void esp_mbedtls_mem_free(void *ptr)
{
    return heap_caps_free(ptr);
}

#endif /* !CONFIG_MBEDTLS_CUSTOM_MEM_ALLOC */
