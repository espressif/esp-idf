/*
 * SPDX-FileCopyrightText: 2020-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdbool.h>

#include "hal/mpu_hal.h"
#include "hal/mpu_types.h"
#include "soc/soc_caps.h"
#include "bootloader_mem.h"
#include "esp_cpu.h"

#if SOC_APM_SUPPORTED
#include "hal/apm_hal.h"
#endif

#if CONFIG_IDF_TARGET_ESP32C5_BETA3_VERSION // TODO: IDF-8615 Remove the workaround when APM supported on C5!
#include "soc/hp_apm_reg.h"
#include "soc/lp_apm_reg.h"
#include "soc/lp_apm0_reg.h"
#endif

void bootloader_init_mem(void)
{

#if !defined(BOOTLOADER_BUILD) && defined(SOC_APM_SUPPORTED)
    /* By default, these access path filters are enable and allow the
     * access to masters only if they are in TEE mode. Since all masters
     * except HP CPU boots in REE mode, default setting of these filters
     * will deny the access to all masters except HP CPU.
     * So, at boot disabling these filters. They will enable as per the
     * use case by TEE initialization code.
     */
    apm_hal_apm_ctrl_filter_enable_all(false);
#endif

#if CONFIG_IDF_TARGET_ESP32C5_BETA3_VERSION // TODO: IDF-8615 Remove the workaround when APM supported on C5!
    // disable apm filter
    REG_WRITE(LP_APM_FUNC_CTRL_REG, 0);
    REG_WRITE(LP_APM0_FUNC_CTRL_REG, 0);
    REG_WRITE(HP_APM_FUNC_CTRL_REG, 0);
#endif

#ifdef CONFIG_BOOTLOADER_REGION_PROTECTION_ENABLE
    // protect memory region
    esp_cpu_configure_region_protection();  // TODO: [ESP32C5] IDF-8833 PSRAM support write
#endif
}
