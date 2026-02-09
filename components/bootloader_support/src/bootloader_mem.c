/*
 * SPDX-FileCopyrightText: 2020-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdbool.h>

#include "hal/mpu_hal.h"
#include "hal/mpu_types.h"
#include "soc/soc_caps.h"
#include "bootloader_mem.h"
#include "esp_cpu.h"

#include "hal/apm_hal.h"

void bootloader_init_mem(void)
{

#if !defined(BOOTLOADER_BUILD)
    /* By default, these access path filters are enable and allow the
     * access to masters only if they are in TEE mode. Since all masters
     * except HP CPU boots in REE mode, default setting of these filters
     * will deny the access to all masters except HP CPU.
     * So, at boot disabling these filters. They will enable as per the
     * use case by TEE initialization code.
     */
#if SOC_APM_CTRL_FILTER_SUPPORTED
    apm_hal_enable_ctrl_filter_all(false);
    /* [APM] On power-up, only the HP CPU starts in TEE mode; others
     * default to REE2. APM blocks REE0–REE2 access by default.
     * Thus, all masters are set to TEE mode.
     */
#if SOC_APM_SUPPORT_TEE_PERI_ACCESS_CTRL
    apm_hal_set_master_sec_mode_all(APM_SEC_MODE_TEE);
#endif // SOC_APM_SUPPORT_TEE_PERI_ACCESS_CTRL
#endif // SOC_APM_CTRL_FILTER_SUPPORTED
#endif

#if CONFIG_BOOTLOADER_REGION_PROTECTION_ENABLE
    // protect memory region
    esp_cpu_configure_region_protection();
#endif
}
