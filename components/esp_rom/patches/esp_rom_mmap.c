/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include <stdbool.h>
#include "sdkconfig.h"
#include "esp_rom_sys.h"
#include "assert.h"

uint32_t Cache_Get_IROM_MMU_End(void)
{
#if CONFIG_IDF_TARGET_ESP32S3
	esp_rom_printf("0x800\n");
    return 0x800;
#elif CONFIG_IDF_TARGET_ESP32C3 || CONFIG_IDF_TARGET_ESP32H2
    return 0x200;
#else
    assert(false);
#endif
}

uint32_t Cache_Get_DROM_MMU_End(void)
{
#if CONFIG_IDF_TARGET_ESP32S3
    return 0x800;
#elif CONFIG_IDF_TARGET_ESP32C3 || CONFIG_IDF_TARGET_ESP32H2
    return 0x200;
#else
    assert(false);
#endif
}
