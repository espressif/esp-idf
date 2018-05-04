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

#include <string.h>
#include "esp_attr.h"
#include "esp_heap_caps.h"
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "esp_wifi.h"
#include "esp_wifi_internal.h"

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

wifi_static_queue_t* wifi_create_queue( int queue_len, int item_size)
{
    wifi_static_queue_t *queue = NULL;

    queue = (wifi_static_queue_t*)heap_caps_malloc(sizeof(wifi_static_queue_t), MALLOC_CAP_INTERNAL|MALLOC_CAP_8BIT);
    if (!queue) {
        return NULL;
    }

#if CONFIG_SPIRAM_USE_MALLOC

    queue->storage = heap_caps_calloc(1, sizeof(StaticQueue_t) + (queue_len*item_size), MALLOC_CAP_INTERNAL|MALLOC_CAP_8BIT);
    if (!queue->storage) {
        goto _error;
    }

    queue->handle = xQueueCreateStatic( queue_len, item_size, ((uint8_t*)(queue->storage)) + sizeof(StaticQueue_t), (StaticQueue_t*)(queue->storage));

    if (!queue->handle) {
        goto _error;
    }

    return queue;

_error:
    if (queue) {
        if (queue->storage) {
            free(queue->storage);
        }

        free(queue);
    }

    return NULL;
#else
    queue->handle = xQueueCreate( queue_len, item_size);
    return queue;
#endif
}

void wifi_delete_queue(wifi_static_queue_t *queue)
{
    if (queue) {
        vQueueDelete(queue->handle);

#if CONFIG_SPIRAM_USE_MALLOC
        if (queue->storage) {
            free(queue->storage);
        }
#endif

        free(queue);
    }
}
