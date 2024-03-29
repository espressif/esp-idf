/*
 * SPDX-FileCopyrightText: 2021-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include <sys/param.h>
#include "esp_system.h"
#include "soc/soc_memory_layout.h"
#include "esp_private/cache_utils.h"

#define ASSERT_STR              "assert failed: "
#define CACHE_DISABLED_STR      "<cached disabled>"

#if __XTENSA__
#define INST_LEN         3
#elif __riscv
#define INST_LEN         4
#endif

static inline void ra_to_str(char *addr)
{
    addr[0] = '0';
    addr[1] = 'x';
    itoa((uint32_t)(__builtin_return_address(0) - INST_LEN), addr + 2, 16);
}

/* Overriding assert function so that whenever assert is called from critical section,
 * it does not lead to a crash of its own.
 */
void __attribute__((noreturn)) __assert_func(const char *file, int line, const char *func, const char *expr)
{
#if CONFIG_COMPILER_OPTIMIZATION_ASSERTIONS_SILENT
    char buff[sizeof(ASSERT_STR) + 11 + 1] = ASSERT_STR;

    ra_to_str(&buff[sizeof(ASSERT_STR) - 1]);

    esp_system_abort(buff);
#else
    char addr[11] = { 0 };
    char buff[200];
    char lbuf[5];
    uint32_t rem_len = sizeof(buff) - 1;
    uint32_t off = 0;

    itoa(line, lbuf, 10);

#if !CONFIG_APP_BUILD_TYPE_PURE_RAM_APP
    if (!spi_flash_cache_enabled())
#endif
    {
        if (esp_ptr_in_drom(file)) {
            file = CACHE_DISABLED_STR;
        }

        if (esp_ptr_in_drom(func)) {
            ra_to_str(addr);
            func = addr;
        }

        if (esp_ptr_in_drom(expr)) {
            expr = CACHE_DISABLED_STR;
        }
    }

    const char *str[] = {ASSERT_STR, func ? func : "\b", " ", file, ":", lbuf, " (", expr, ")"};

    for (int i = 0; i < sizeof(str) / sizeof(str[0]); i++) {
        uint32_t len = strlen(str[i]);
        uint32_t cpy_len = MIN(len, rem_len);
        memcpy(buff + off, str[i], cpy_len);
        rem_len -= cpy_len;
        off += cpy_len;
        if (rem_len == 0) {
            break;
        }
    }
    buff[off] = '\0';
    esp_system_abort(buff);
#endif  /* CONFIG_COMPILER_OPTIMIZATION_ASSERTIONS_SILENT */
}

void __attribute__((noreturn)) __assert(const char *file, int line, const char *failedexpr)
{
    __assert_func(file, line, NULL, failedexpr);
}

/* No-op function, used to force linker to include these changes */
void newlib_include_assert_impl(void)
{
}
