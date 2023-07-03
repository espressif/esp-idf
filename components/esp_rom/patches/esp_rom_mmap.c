/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include "sdkconfig.h"
#include "soc/ext_mem_defs.h"


uint32_t Cache_Get_IROM_MMU_End(void)
{
#if CONFIG_IDF_TARGET_ESP32S3
    return 0x800;
#elif CONFIG_IDF_TARGET_ESP32C3 || CONFIG_IDF_TARGET_ESP32H2
    return 0x200;
#elif CONFIG_IDF_TARGET_ESP32C2
    return 0x100;
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
#elif CONFIG_IDF_TARGET_ESP32C2
    return 0x100;
#else
    assert(false);
#endif
}
