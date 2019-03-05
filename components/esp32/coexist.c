// Copyright 2018-2018 Espressif Systems (Shanghai) PTE LTD
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

#include "esp_coexist.h"
#include "esp_coexist_internal.h"

const char *esp_coex_version_get(void)
{
    return coex_version_get();
}

esp_err_t esp_coex_preference_set(esp_coex_prefer_t prefer)
{
    return coex_preference_set((coex_prefer_t)prefer);
}
