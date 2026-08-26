/*
 * SPDX-FileCopyrightText: 2020-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

/*
Note: This is a compatibility header. Call the interfaces in esp_cpu.h instead
*/

#include "esp_attr.h"
#include "esp_cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

FORCE_INLINE_ATTR __attribute__((deprecated)) void soc_ll_stall_core(int core)
{
    esp_cpu_stall(core);
}

FORCE_INLINE_ATTR __attribute__((deprecated)) void soc_ll_unstall_core(int core)
{
    esp_cpu_unstall(core);
}

FORCE_INLINE_ATTR __attribute__((deprecated)) void soc_ll_reset_core(int core)
{
    esp_cpu_reset(core);
}

#ifdef __cplusplus
}
#endif
