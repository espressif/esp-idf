// Copyright 2015-2017 Espressif Systems (Shanghai) PTE LTD
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

#include "esp_attr.h"
#include "esp_heap_caps.h"
#include "sdkconfig.h"

/*
 If CONFIG_WIFI_LWIP_ALLOCATION_FROM_SPIRAM_FIRST is enabled. Prefer to allocate a chunk of memory in SPIRAM firstly.
 If failed, try to allocate it in internal memory then.
 */
IRAM_ATTR void *wifi_malloc( size_t size )
{
#if CONFIG_WIFI_LWIP_ALLOCATION_FROM_SPIRAM_FIRST
    return heap_caps_malloc_prefer(size, 2, MALLOC_CAP_DEFAULT|MALLOC_CAP_SPIRAM, MALLOC_CAP_DEFAULT|MALLOC_CAP_INTERNAL);
#else
    return malloc(size);
#endif
}

/*
 If CONFIG_WIFI_LWIP_ALLOCATION_FROM_SPIRAM_FIRST is enabled. Prefer to allocate a chunk of memory in SPIRAM firstly.
 If failed, try to allocate it in internal memory then.
 */
IRAM_ATTR void *wifi_realloc( void *ptr, size_t size )
{
#if CONFIG_WIFI_LWIP_ALLOCATION_FROM_SPIRAM_FIRST
    return heap_caps_realloc_prefer(ptr, size, 2, MALLOC_CAP_DEFAULT|MALLOC_CAP_SPIRAM, MALLOC_CAP_DEFAULT|MALLOC_CAP_INTERNAL);
#else
    return realloc(ptr, size);
#endif
}

/*
 If CONFIG_WIFI_LWIP_ALLOCATION_FROM_SPIRAM_FIRST is enabled. Prefer to allocate a chunk of memory in SPIRAM firstly.
 If failed, try to allocate it in internal memory then.
 */
IRAM_ATTR void *wifi_calloc( size_t n, size_t size )
{
#if CONFIG_WIFI_LWIP_ALLOCATION_FROM_SPIRAM_FIRST
    return heap_caps_calloc_prefer(n, size, 2, MALLOC_CAP_DEFAULT|MALLOC_CAP_SPIRAM, MALLOC_CAP_DEFAULT|MALLOC_CAP_INTERNAL);
#else
    return calloc(n, size);
#endif
}
