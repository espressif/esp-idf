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
#ifdef SOC_APM_CTRL_FILTER_SUPPORTED
    apm_hal_apm_ctrl_filter_enable_all(false);
#endif
#endif

#ifdef CONFIG_BOOTLOADER_REGION_PROTECTION_ENABLE
    // protect memory region
    esp_cpu_configure_region_protection();
#endif
}
