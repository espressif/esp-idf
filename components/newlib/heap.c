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
#include <stdlib.h>
#include <sys/reent.h>
#include <malloc.h>
#include "esp_heap_caps.h"


/*
 These contain the business logic for the malloc() and realloc() implementation. Because of heap tracing
 wrapping reasons, we do not want these to be a public api, however, so they're not defined publicly.
*/
extern void *heap_caps_malloc_default( size_t size );
extern void *heap_caps_realloc_default( void *ptr, size_t size );


void* malloc(size_t size)
{
    return heap_caps_malloc_default(size);
}

void* calloc(size_t n, size_t size)
{
    return _calloc_r(_REENT, n, size);
}

void* realloc(void* ptr, size_t size)
{
    return heap_caps_realloc_default(ptr, size);
}

void free(void *ptr)
{
    heap_caps_free(ptr);
}

void* _malloc_r(struct _reent *r, size_t size)
{
    return heap_caps_malloc_default(size);
}

void _free_r(struct _reent *r, void* ptr)
{
    heap_caps_free(ptr);
}

void* _realloc_r(struct _reent *r, void* ptr, size_t size)
{
    return heap_caps_realloc_default( ptr, size );
}

void* _calloc_r(struct _reent *r, size_t nmemb, size_t size)
{
    void *result;
    size_t size_bytes;
    if (__builtin_mul_overflow(nmemb, size, &size_bytes)) {
        return NULL;
    }

    result = heap_caps_malloc_default(size_bytes);
    if (result != NULL) {
        bzero(result, size_bytes);
    }
    return result;
}

/* No-op function, used to force linking this file,
   instead of the heap implementation from newlib.
 */
void newlib_include_heap_impl()
{
}

/* The following functions are implemented by newlib's heap allocator,
   but aren't available in the heap component.
   Define them as non-functional stubs here, so that the application
   can not cause the newlib heap implementation to be linked in
 */
void* memalign(size_t alignment, size_t n)
{
    extern void memalign_function_was_linked_but_unsupported_in_esp_idf(void);
    memalign_function_was_linked_but_unsupported_in_esp_idf();
    return NULL;
}

int malloc_trim(size_t pad)
{
    return 0; // indicates failure
}

size_t malloc_usable_size(void* p)
{
    return 0;
}

void malloc_stats()
{
}

int mallopt(int parameter_number, int parameter_value)
{
    return 0; // indicates failure
}

struct mallinfo mallinfo()
{
    struct mallinfo dummy = {0};
    return dummy;
}

void* valloc(size_t n) __attribute__((alias("malloc")));
void* pvalloc(size_t n) __attribute__((alias("malloc")));
void cfree(void* p) __attribute__((alias("free")));
