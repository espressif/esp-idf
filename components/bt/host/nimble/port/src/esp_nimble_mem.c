/*
 * Copyright 2020 Espressif Systems (Shanghai) PTE LTD
 *
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

#include "esp_attr.h"
#include "esp_heap_caps.h"
#include "sdkconfig.h"
#include "esp_nimble_mem.h"

IRAM_ATTR void *nimble_platform_mem_malloc(size_t size)
{
#ifdef CONFIG_BT_NIMBLE_MEM_ALLOC_MODE_INTERNAL
    return heap_caps_malloc(size, MALLOC_CAP_INTERNAL|MALLOC_CAP_8BIT);
#elif CONFIG_BT_NIMBLE_MEM_ALLOC_MODE_EXTERNAL
    return heap_caps_malloc(size, MALLOC_CAP_SPIRAM|MALLOC_CAP_8BIT);
#elif CONFIG_BT_NIMBLE_MEM_ALLOC_MODE_IRAM_8BIT
    return heap_caps_malloc_prefer(size, 2, MALLOC_CAP_INTERNAL|MALLOC_CAP_IRAM_8BIT, MALLOC_CAP_INTERNAL|MALLOC_CAP_8BIT);
#else
    return malloc(size);
#endif
}

IRAM_ATTR void *nimble_platform_mem_calloc(size_t n, size_t size)
{
#ifdef CONFIG_BT_NIMBLE_MEM_ALLOC_MODE_INTERNAL
    return heap_caps_calloc(n, size, MALLOC_CAP_INTERNAL|MALLOC_CAP_8BIT);
#elif CONFIG_BT_NIMBLE_MEM_ALLOC_MODE_EXTERNAL
    return heap_caps_calloc(n, size, MALLOC_CAP_SPIRAM|MALLOC_CAP_8BIT);
#elif CONFIG_BT_NIMBLE_MEM_ALLOC_MODE_IRAM_8BIT
    return heap_caps_calloc_prefer(n, size, 2, MALLOC_CAP_INTERNAL|MALLOC_CAP_IRAM_8BIT, MALLOC_CAP_INTERNAL|MALLOC_CAP_8BIT);
#else
    return calloc(n, size);
#endif
}

IRAM_ATTR void nimble_platform_mem_free(void *ptr)
{
    heap_caps_free(ptr);
}
