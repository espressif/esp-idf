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


#ifndef _LWIP_DEBUG_H
#define _LWIP_DEBUG_H

#include "esp_log.h"

typedef struct {
    uint32_t trypost_fail;
    uint32_t fetch_fail;
    uint32_t tryfetch_fail;
} esp_lwip_stats_t;

extern esp_lwip_stats_t g_esp_lwip_stats;

#define SYS_ARCH_INC_WITH_LOCK(_f) do {\
    sys_arch_protect();\
    g_esp_lwip_stats._f ++;\
    sys_arch_unprotect(0);\
}while(0)

#define LWIP_MODULE_SOCKETS    1
#define LWIP_MODULE_STATS      (1<<1)

void esp_lwip_dump(uint64_t modules);

#endif
