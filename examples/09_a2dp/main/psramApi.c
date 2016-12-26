// Copyright 2010-2016 Espressif Systems (Shanghai) PTE LTD
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

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include "Utility.h"
#include "psram.h"

#define mutex_t                 xSemaphoreHandle
mutex_t psram_mem_mutex         = NULL;

void psram_cache_enable(void)
{
    psram_enable(PSRAM_CACHE_F40M_S40M);
}

/**
 * @brief Memcpy with a cache, atomic protected, speaking, reading and writing
 */
void psram_cache_memcpy(uint8_t *dst, uint8_t *src, uint16_t len)
{
    uint16_t    temp    = 0;
    uint8_t     *dest   = dst;
    uint8_t     *source = src;
    uint16_t    n       = len;
    if (psram_mem_mutex == NULL) {
        mutex_init(&psram_mem_mutex);
    }
    mutex_lock(&psram_mem_mutex);
    while (n--) {
        temp = *source++;
        *dest++ = temp;
    }
    mutex_unlock(&psram_mem_mutex);
}

/**
 * @brief Memset with a cache, atomic protected, speaking, reading and writing
 */
void psram_cache_memset(uint8_t *addr, uint8_t c, uint32_t len)
{
    if (psram_mem_mutex == NULL) {
        mutex_init(&psram_mem_mutex);
    }
    mutex_lock(&psram_mem_mutex);
    memset(addr, c, len);
    mutex_unlock(&psram_mem_mutex);
}