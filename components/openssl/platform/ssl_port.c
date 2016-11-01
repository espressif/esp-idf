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

#include "ssl_port.h"

#ifdef ESP32_IDF_PLATFORM

#include "string.h"
#include "malloc.h"

/*********************************************************************************************/
/********************************* SSL general interface *************************************/

void* ssl_mem_zalloc(size_t size)
{
    void *p = malloc(size);

    if (p)
        memset(p, 0, size);

    return p;
}

void *ssl_mem_malloc(size_t size)
{
    return malloc(size);
}

void ssl_mem_free(void *p)
{
    free(p);
}

void* ssl_memcpy(void *to, const void *from, size_t size)
{
    return memcpy(to, from, size);
}

size_t ssl_strlen(const char *src)
{
    return strlen(src);
}

void ssl_speed_up_enter(void)
{

}

void ssl_speed_up_exit(void)
{

}

#endif

