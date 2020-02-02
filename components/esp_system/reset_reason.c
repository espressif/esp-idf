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


#include "esp_private/system_internal.h"

/* These two weak stubs for esp_reset_reason_{get,set}_hint are used when
 * the application does not call esp_reset_reason() function, and
 * reset_reason.c is not linked into the output file.
 */
void __attribute__((weak)) esp_reset_reason_set_hint(esp_reset_reason_t hint)
{
}

esp_reset_reason_t __attribute__((weak)) esp_reset_reason_get_hint(void)
{
    return ESP_RST_UNKNOWN;
}

