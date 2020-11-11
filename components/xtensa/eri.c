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
#include "eri.h"

uint32_t eri_read(int addr) {
    uint32_t ret;
    asm volatile (
        "RER %0,%1"
        :"=r"(ret):"r"(addr)
        );
    return ret;
}

void eri_write(int addr, uint32_t data) {
    asm volatile (
        "WER %0,%1"
        ::"r"(data),"r"(addr)
    );
}
