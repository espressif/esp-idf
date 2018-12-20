// Copyright 2017-2018 Espressif Systems (Shanghai) PTE LTD
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

#include <assert.h>
#include "esp_ota_ops.h"
#include "sdkconfig.h"

// Application version info
const __attribute__((section(".rodata_desc"))) esp_app_desc_t esp_app_desc = {
    .magic_word = ESP_APP_DESC_MAGIC_WORD,
#ifdef CONFIG_APP_EXCLUDE_PROJECT_VER_VAR
    .version = "",
#else
    .version = PROJECT_VER,
#endif

#ifdef CONFIG_APP_EXCLUDE_PROJECT_NAME_VAR
    .project_name = "",
#else
    .project_name = PROJECT_NAME,
#endif
    .idf_ver = IDF_VER,

#ifdef CONFIG_APP_SECURE_VERSION
    .secure_version = CONFIG_APP_SECURE_VERSION,
#else
    .secure_version = 0,
#endif

#ifdef CONFIG_APP_COMPILE_TIME_DATE
    .time = __TIME__,
    .date = __DATE__,
#else
    .time = "",
    .date = "",
#endif
};


#ifndef CONFIG_APP_EXCLUDE_PROJECT_VER_VAR
_Static_assert(sizeof(PROJECT_VER) <= sizeof(esp_app_desc.version), "PROJECT_VER is longer than version field in structure");
#endif
_Static_assert(sizeof(IDF_VER) <= sizeof(esp_app_desc.idf_ver), "IDF_VER is longer than idf_ver field in structure");
#ifndef CONFIG_APP_EXCLUDE_PROJECT_NAME_VAR
_Static_assert(sizeof(PROJECT_NAME) <= sizeof(esp_app_desc.project_name), "PROJECT_NAME is longer than project_name field in structure");
#endif

const esp_app_desc_t *esp_ota_get_app_description(void)
{
    return &esp_app_desc;
}
