/*
 * SPDX-FileCopyrightText: 2013-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_chip_info.h"

extern void __real_esp_chip_info(esp_chip_info_t* out_info);

// Fake a single core chip for testing purposes only, see CONFIG_SINGLE_CORE_VARIANT
void __wrap_esp_chip_info(esp_chip_info_t* out_info)
{
    __real_esp_chip_info(out_info);
    out_info->cores = 1;
}
