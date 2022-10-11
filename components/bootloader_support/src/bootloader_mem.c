/*
 * SPDX-FileCopyrightText: 2020-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdbool.h>

#include "hal/mpu_hal.h"
#include "hal/mpu_types.h"
#include "soc/soc_caps.h"
#include "bootloader_mem.h"
#include "esp_cpu.h"

#if CONFIG_IDF_TARGET_ESP32C6
#include "soc/hp_apm_reg.h"
#include "soc/lp_apm_reg.h"
#include "soc/lp_apm0_reg.h"
#endif

void bootloader_init_mem(void)
{
#if CONFIG_IDF_TARGET_ESP32C6
    // disable apm filter // TODO: IDF-5909
    REG_WRITE(LP_APM_FUNC_CTRL_REG, 0);
    REG_WRITE(LP_APM0_FUNC_CTRL_REG, 0);
    REG_WRITE(HP_APM_FUNC_CTRL_REG, 0);
#endif

#ifdef CONFIG_BOOTLOADER_REGION_PROTECTION_ENABLE
    // protect memory region
    esp_cpu_configure_region_protection();
#endif
}
