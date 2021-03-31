// Copyright 2013-2016 Espressif Systems (Shanghai) PTE LTD
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

#include "esp_system.h"

extern void __real_esp_chip_info(esp_chip_info_t* out_info);

// Fake a single core chip for testing purposes only, see CONFIG_SINGLE_CORE_VARIANT
void __wrap_esp_chip_info(esp_chip_info_t* out_info)
{
    __real_esp_chip_info(out_info);
    out_info->cores = 1;
}
