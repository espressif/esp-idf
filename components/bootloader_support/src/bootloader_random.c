// Copyright 2010-2016 Espressif Systems (Shanghai) PTE LTD
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
#include "bootloader_random.h"
#include "soc/wdev_reg.h"

#ifndef BOOTLOADER_BUILD
#include "esp_system.h"
#endif

void bootloader_fill_random(void *buffer, size_t length)
{
    uint8_t *buffer_bytes = (uint8_t *)buffer;
    uint32_t random;

    /* TODO: enable HW RNG clock

       Until this clock is enabled, this is not secure
     */

    for (int i = 0; i < length; i++) {
        if (i == 0 || i % 4 == 0) { /* redundant check is for a compiler warning */
#ifdef BOOTLOADER_BUILD
            /* HW RNG generates 32 bits entropy per 16 APB cycles,
               in bootloader CPU clock == APB clock.

               We are being conservative here and waiting at least
               that long, as loop shift overhead, etc will add more
               cycles.
            */
            asm volatile("nop; nop; nop; nop;");
            asm volatile("nop; nop; nop; nop;");
            asm volatile("nop; nop; nop; nop;");
            asm volatile("nop; nop; nop; nop;");
            random = REG_READ(WDEV_RND_REG);
#else
            random = esp_random();
#endif
        }

        buffer_bytes[i] = random >> ((i % 4) * 8);
    }
}
