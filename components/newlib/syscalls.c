// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
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
#include <stdbool.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <sys/reent.h>
#include <stdlib.h>
#include "esp_attr.h"
#include "freertos/FreeRTOS.h"
#include "esp_heap_caps.h"

void* IRAM_ATTR _malloc_r(struct _reent *r, size_t size)
{
    return heap_caps_malloc( size, MALLOC_CAP_8BIT );
}

void IRAM_ATTR _free_r(struct _reent *r, void* ptr)
{
    heap_caps_free( ptr );
}

void* IRAM_ATTR _realloc_r(struct _reent *r, void* ptr, size_t size)
{
    return heap_caps_realloc( ptr, size, MALLOC_CAP_8BIT );
}

void* IRAM_ATTR _calloc_r(struct _reent *r, size_t count, size_t size)
{
    void* result = heap_caps_malloc(count * size, MALLOC_CAP_8BIT);
    if (result) {
        bzero(result, count * size);
    }
    return result;
}

int _system_r(struct _reent *r, const char *str)
{
    __errno_r(r) = ENOSYS;
    return -1;
}

void _raise_r(struct _reent *r)
{
    abort();
}

void* _sbrk_r(struct _reent *r, ptrdiff_t sz)
{
    abort();
}

int _getpid_r(struct _reent *r)
{
    __errno_r(r) = ENOSYS;
    return -1;
}

int _kill_r(struct _reent *r, int pid, int sig)
{
    __errno_r(r) = ENOSYS;
    return -1;
}

void _exit(int __status)
{
    abort();
}

