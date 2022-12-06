/*
 * SPDX-FileCopyrightText: 2017-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "sdkconfig.h"
#include "esp_system.h"
#include "esp_random.h"
#include "esp_rom_sys.h"

#if CONFIG_COMPILER_STACK_CHECK

#include "esp_log.h"
const static char *TAG = "stack_chk";

void *__stack_chk_guard = NULL;

static void __attribute__ ((constructor))
__esp_stack_guard_setup (void)
{
    ESP_LOGD(TAG, "Intialize random stack guard");
    __stack_chk_guard = (void *)esp_random();
}

IRAM_ATTR void __stack_chk_fail (void)
{
    esp_system_abort(DRAM_STR("Stack smashing protect failure!"));
}

#endif
