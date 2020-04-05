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
#include <sys/param.h>
#include "esp_ota_ops.h"
#include "esp_attr.h"
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

#ifdef CONFIG_BOOTLOADER_APP_SECURE_VERSION
    .secure_version = CONFIG_BOOTLOADER_APP_SECURE_VERSION,
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

/* The following two functions may be called from the panic handler
 * or core dump, hence IRAM_ATTR.
 */

static inline char IRAM_ATTR to_hex_digit(unsigned val)
{
    return (val < 10) ? ('0' + val) : ('a' + val - 10);
}

__attribute__((constructor)) void esp_ota_init_app_elf_sha256(void)
{
    esp_ota_get_app_elf_sha256(NULL, 0);
}

/* The esp_app_desc.app_elf_sha256 should be possible to print in panic handler during cache is disabled.
 * But because the cache is disabled the reading esp_app_desc.app_elf_sha256 is not right and
 * can lead to a complete lock-up of the CPU.
 * For this reason we do a reading of esp_app_desc.app_elf_sha256 while start up in esp_ota_init_app_elf_sha256() 
 * and keep it in the static s_app_elf_sha256 value.
 */
int IRAM_ATTR esp_ota_get_app_elf_sha256(char* dst, size_t size)
{
    static char s_app_elf_sha256[CONFIG_APP_RETRIEVE_LEN_ELF_SHA / 2];
    static bool first_call = true;
    if (first_call) {
        first_call = false;
        const uint8_t* src = esp_app_desc.app_elf_sha256;
        for (size_t i = 0; i < sizeof(s_app_elf_sha256); ++i) {
            s_app_elf_sha256[i] = src[i];
        }
    }
    if (dst == NULL || size == 0) {
        return 0;
    }
    size_t n = MIN((size - 1) / 2, sizeof(s_app_elf_sha256));
    for (size_t i = 0; i < n; ++i) {
        dst[2*i] = to_hex_digit(s_app_elf_sha256[i] >> 4);
        dst[2*i + 1] = to_hex_digit(s_app_elf_sha256[i] & 0xf);
    }
    dst[2*n] = 0;
    return 2*n + 1;
}
