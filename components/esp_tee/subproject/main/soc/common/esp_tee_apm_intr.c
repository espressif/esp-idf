/*
 * SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdint.h>

#include "soc/soc_caps.h"
#include "hal/apm_types.h"
#include "esp_bit_defs.h"

#include "esp_tee.h"
#include "esp_tee_apm_intr.h"

static const char *const excp_mid_strs[] = {
    [APM_MASTER_HPCORE]        = "HPCORE",
#if SOC_LP_CORE_SUPPORTED
    [APM_MASTER_LPCORE]        = "LPCORE",
#endif /* SOC_LP_CORE_SUPPORTED */
    [APM_MASTER_REGDMA]        = "REGDMA",
#if SOC_SDIO_SLAVE_SUPPORTED
    [APM_MASTER_SDIOSLV]       = "SDIOSLV",
#endif /* SOC_SDIO_SLAVE_SUPPORTED */
    [APM_MASTER_MODEM]         = "MODEM",
    [APM_MASTER_MEM_MON]       = "MEM_MONITOR",
    [APM_MASTER_TRACE]         = "TRACE",
#if SOC_SPIRAM_SUPPORTED
    [APM_MASTER_PSRAM_MEM_MON] = "PSRAM_MEM_MONITOR",
#endif /* SOC_SPIRAM_SUPPORTED */
#if SOC_GPSPI_SUPPORTED
    [APM_MASTER_GDMA_GPSPI]    = "GDMA_GPSPI",
#endif /* SOC_GPSPI_SUPPORTED */
#if SOC_UHCI_SUPPORTED
    [APM_MASTER_GDMA_UHCI]     = "GDMA_UHCI",
#endif /* SOC_UHCI_SUPPORTED */
    [APM_MASTER_GDMA_I2S]      = "GDMA_I2S",
#if SOC_AES_SUPPORTED
    [APM_MASTER_GDMA_AES]      = "GDMA_AES",
#endif /* SOC_AES_SUPPORTED */
    [APM_MASTER_GDMA_SHA]      = "GDMA_SHA",
    [APM_MASTER_GDMA_ADC]      = "GDMA_ADC",
#if SOC_PARLIO_SUPPORTED
    [APM_MASTER_GDMA_PARLIO]   = "GDMA_PARLIO",
#endif /* SOC_PARLIO_SUPPORTED */
};

const char *esp_tee_apm_excp_mid_to_str(uint8_t mid)
{
    if (mid < sizeof(excp_mid_strs) / sizeof(excp_mid_strs[0]) && excp_mid_strs[mid]) {
        return excp_mid_strs[mid];
    }
    return "Unknown";
}

const char *esp_tee_apm_excp_type_to_str(uint8_t type)
{
    char *excp_type = "APM - Unknown exception";

    if (type & BIT(0)) {
        excp_type = "APM - Authority exception";
    } else if (type & BIT(1)) {
        excp_type = "APM - Space exception";
    }

    return excp_type;
}

const char *esp_tee_apm_excp_ctrl_to_str(apm_ctrl_module_t ctrl_mod)
{
    char *excp_ctrl = NULL;

    switch (ctrl_mod) {
    case APM_CTRL_HP_APM:
        excp_ctrl = "HP_APM";
        break;
    case APM_CTRL_LP_APM:
        excp_ctrl = "LP_APM";
        break;
#if SOC_APM_LP_APM0_SUPPORTED
    case APM_CTRL_LP_APM0:
        excp_ctrl = "LP_APM0";
        break;
#endif
#if SOC_APM_CPU_APM_SUPPORTED
    case APM_CTRL_CPU_APM:
        excp_ctrl = "CPU_APM";
        break;
#endif
    default:
        excp_ctrl = "Unknown";
        break;
    }

    return excp_ctrl;
}

const char *esp_tee_apm_excp_mode_to_str(uint8_t mode)
{
    char *excp_mode = NULL;

    switch (mode) {
    case APM_SEC_MODE_TEE:
    case APM_SEC_MODE_REE0:
        excp_mode = "REE0";
        break;
    case APM_SEC_MODE_REE1:
        excp_mode = "REE1";
        break;
    case APM_SEC_MODE_REE2:
        excp_mode = "REE2";
        break;
    default:
        excp_mode = "Unknown";
        break;
    }

    return excp_mode;
}
