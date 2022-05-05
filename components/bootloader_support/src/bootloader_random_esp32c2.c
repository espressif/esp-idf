/*
 * SPDX-FileCopyrightText: 2020-2022 Espressif Systems (Shanghai) CO LTD
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

void bootloader_random_enable(void)
{
    // TODO: IDF-4021
}

void bootloader_random_disable(void)
{
    // TODO: IDF-4021
}
