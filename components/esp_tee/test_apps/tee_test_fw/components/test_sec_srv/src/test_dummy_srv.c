/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "secure_service_num.h"
#include "esp_tee.h"
#include "esp_err.h"
#include "esp_rom_sys.h"
#include "esp_attr.h"

void NOINLINE_ATTR _ss_dummy_secure_service(int a, int b, int c, int d, int e, int f, int g, int h, int *i)
{
    esp_rom_printf("Dummy secure service\n");
    *i = a + b + c + d + e + f + g + h;
}
