// Copyright 2020 Espressif Systems (Shanghai) PTE LTD
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
#pragma once

void esp_time_impl_init(void);

uint64_t esp_time_impl_get_time(void);

uint64_t esp_time_impl_get_time_since_boot(void);

uint32_t esp_time_impl_get_time_resolution(void);

void esp_time_impl_set_boot_time(uint64_t t);

uint64_t esp_time_impl_get_boot_time(void);
