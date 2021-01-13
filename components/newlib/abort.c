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
#include <stdint.h>
#include <string.h>

#include "esp_system.h"

#include "soc/soc_caps.h"
#include "hal/cpu_hal.h"

void __attribute__((noreturn)) abort(void)
{
    #define ERR_STR1  "abort() was called at PC 0x"
    #define ERR_STR2  " on core "

    _Static_assert(UINTPTR_MAX == 0xffffffff, "abort() assumes 32-bit addresses");
    _Static_assert(SOC_CPU_CORES_NUM < 10, "abort() assumes number of cores is 1 to 9");

    char addr_buf[9] = { 0 };
    char core_buf[2] = { 0 };

    char buf[sizeof(ERR_STR1) + sizeof(addr_buf) + sizeof(core_buf) + sizeof(ERR_STR2) + 1 /* null char */] = { 0 };

    itoa((uint32_t)(__builtin_return_address(0) - 3), addr_buf, 16);
    itoa(cpu_ll_get_core_id(), core_buf, 10);

    const char *str[] = { ERR_STR1, addr_buf, ERR_STR2, core_buf };

    char *dest = buf;

    for (size_t i = 0; i < sizeof(str) / sizeof(str[0]); i++) {
        strcat(dest, str[i]);
    }

    esp_system_abort(buf);
}
