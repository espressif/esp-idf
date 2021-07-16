// Copyright 2010-2017 Espressif Systems (Shanghai) PTE LTD
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
#include "soc/dport_access.h"

// Read a sequence of DPORT registers to the buffer.
void esp_dport_access_read_buffer(uint32_t *buff_out, uint32_t address, uint32_t num_words)
{
    for (uint32_t i = 0;  i < num_words; ++i) {
        buff_out[i] = DPORT_SEQUENCE_REG_READ(address + i * 4);
    }
}
