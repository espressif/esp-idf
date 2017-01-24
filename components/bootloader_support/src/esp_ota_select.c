// Copyright 2015-2017 Espressif Systems (Shanghai) PTE LTD
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

#include "rom/crc.h"
#include "esp_ota_select.h"

uint32_t esp_ota_select_crc(const esp_ota_select_entry_t *s)
{
    esp_ota_select_entry_t tmp;
    memcpy(&tmp, s, sizeof(tmp));
    tmp.crc = 0;
    return crc32_le(UINT32_MAX, (uint8_t *) &tmp, sizeof(tmp));
}

bool esp_ota_select_valid(const esp_ota_select_entry_t *s)
{
    return s->seq != UINT32_MAX && s->crc == esp_ota_select_crc(s);
}

const esp_ota_select_entry_t *esp_ota_choose_current(const esp_ota_select_entry_t s[2])
{
    const esp_ota_select_entry_t *cs = NULL;
    bool s0_valid = esp_ota_select_valid(&s[0]);
    bool s1_valid = esp_ota_select_valid(&s[1]);
    if (s0_valid && s1_valid) {
        cs = s[0].seq > s[1].seq ? &s[0] : &s[1];
    } else if (s0_valid) {
        cs = &s[0];
    } else if (s1_valid) {
        cs = &s[1];
    }
    return cs;
}
