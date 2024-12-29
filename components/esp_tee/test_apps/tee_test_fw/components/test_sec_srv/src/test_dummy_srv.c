/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "secure_service_num.h"
#include "esp_tee.h"
#include "esp_err.h"
#include "esp_rom_sys.h"

void _ss_dummy_secure_service(void)
{
    esp_rom_printf("Dummy secure service\n");
}
