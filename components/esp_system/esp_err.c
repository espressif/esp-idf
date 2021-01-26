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
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "esp_err.h"
#include "esp_spi_flash.h"

#include "esp_rom_sys.h"

static void esp_error_check_failed_print(const char *msg, esp_err_t rc, const char *file, int line, const char *function, const char *expression)
{
    esp_rom_printf("%s failed: esp_err_t 0x%x", msg, rc);
#ifdef CONFIG_ESP_ERR_TO_NAME_LOOKUP
    esp_rom_printf(" (%s)", esp_err_to_name(rc));
#endif //CONFIG_ESP_ERR_TO_NAME_LOOKUP
    esp_rom_printf(" at 0x%08x\n", (intptr_t)__builtin_return_address(0) - 3);
    if (spi_flash_cache_enabled()) { // strings may be in flash cache
        esp_rom_printf("file: \"%s\" line %d\nfunc: %s\nexpression: %s\n", file, line, function, expression);
    }
}

void _esp_error_check_failed_without_abort(esp_err_t rc, const char *file, int line, const char *function, const char *expression)
{
    esp_error_check_failed_print("ESP_ERROR_CHECK_WITHOUT_ABORT", rc, file, line, function, expression);
}

void _esp_error_check_failed(esp_err_t rc, const char *file, int line, const char *function, const char *expression)
{
    esp_error_check_failed_print("ESP_ERROR_CHECK", rc, file, line, function, expression);
    abort();
}
