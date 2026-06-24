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
#if CONFIG_IDF_TARGET_ESP32S31
#include "soc/hp_peri0_pms_reg.h"
#include "soc/hp_peri1_pms_reg.h"
#include "soc/lp_peri_pms_reg.h"
#endif

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

#if CONFIG_IDF_TARGET_ESP32S31
    /* TODO: [ESP32S31] IDF-14620
       As a temp. fix to allow ULP access to all peripherals during bringup.
       Open all LP/HP peripheral PMS control registers for all security modes.
    */
    for (uint32_t reg = LP_PERI_PMS_LP_SYSREG_CTRL_REG; reg <= LP_PERI_PMS_LP_DAC_CTRL_REG; reg += 4) {
        REG_WRITE(reg, 0xFF);
    }
    for (uint32_t reg = HP_PERI0_PMS_TRACE0_CTRL_REG; reg <= HP_PERI0_PMS_AXI_PERF_MON_CTRL_REG; reg += 4) {
        REG_WRITE(reg, 0xFF);
    }
    for (uint32_t reg = HP_PERI1_PMS_HP_USBOTG_PHY_CTRL_REG; reg <= HP_PERI1_PMS_HP_PERI1_PMS_CTRL_REG; reg += 4) {
        REG_WRITE(reg, 0xFF);
    }
#endif

#if CONFIG_BOOTLOADER_REGION_PROTECTION_ENABLE
    // protect memory region
    esp_cpu_configure_region_protection();
#endif
}
