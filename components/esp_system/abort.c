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
#include <string.h>

#include "esp_err.h"
#include "esp_spi_flash.h"
#include "esp_system.h"

#include "esp_private/system_internal.h"

#include "soc/cpu.h"
#include "soc/soc_caps.h"
#include "hal/cpu_hal.h"

#if CONFIG_IDF_TARGET_ESP32
#include "esp32/rom/ets_sys.h"
#else
#include "esp32s2/rom/ets_sys.h"
#endif

#include "panic.h"

void __attribute__((noreturn)) abort(void)
{
    char buf[47] = { 0 };

    _Static_assert(UINTPTR_MAX == 0xffffffff, "abort() assumes 32-bit addresses");
    _Static_assert(SOC_CPU_CORES_NUM < 10, "abort() assumes number of cores is [1..9]");

    char addr_buf[9] = { 0 };
    char core_buf[2] = { 0 };

    itoa((uint32_t)(__builtin_return_address(0) - 3), addr_buf, 16);
    itoa(cpu_ll_get_core_id(), core_buf, 10);

	const char *str[4] = { "abort() was called at PC 0x", addr_buf, " on core ", core_buf };
	for (int i = 0, k = 0; i < 4; i++) {
		for (int j = 0; str[i][j] != '\0'; j++) {
			buf[k] = str[i][j];
			k++;
		}
	}

	esp_system_abort(buf);
}

static void esp_error_check_failed_print(const char *msg, esp_err_t rc, const char *file, int line, const char *function, const char *expression)
{
    ets_printf("%s failed: esp_err_t 0x%x", msg, rc);
#ifdef CONFIG_ESP_ERR_TO_NAME_LOOKUP
    ets_printf(" (%s)", esp_err_to_name(rc));
#endif //CONFIG_ESP_ERR_TO_NAME_LOOKUP
    ets_printf(" at 0x%08x\n", (intptr_t)__builtin_return_address(0) - 3);
    if (spi_flash_cache_enabled()) { // strings may be in flash cache
        ets_printf("file: \"%s\" line %d\nfunc: %s\nexpression: %s\n", file, line, function, expression);
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