/*
 * SPDX-FileCopyrightText: 2020-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "sdkconfig.h"
#include "bootloader_random.h"
#include "esp_log.h"
#include "soc/syscon_reg.h"
#include "soc/rtc_cntl_reg.h"
#include "soc/apb_saradc_reg.h"
#include "soc/system_reg.h"
#include "esp_private/regi2c_ctrl.h"

// ESP32H4-TODO: IDF-3381
void bootloader_random_enable(void)
{

}

void bootloader_random_disable(void)
{

}
