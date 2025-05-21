/*
 * SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdint.h>

#include "soc/soc_caps.h"
#include "esp_bit_defs.h"

#include "hal/apm_hal.h"
#include "hal/apm_ll.h"

static const char *const excp_mid_strs[] = {
    [APM_LL_MASTER_HPCORE]        = "HPCORE",
    [APM_LL_MASTER_LPCORE]        = "LPCORE",
    [APM_LL_MASTER_REGDMA]        = "REGDMA",
    [APM_LL_MASTER_SDIOSLV]       = "SDIOSLV",
    [APM_LL_MASTER_MODEM]         = "MODEM",
    [APM_LL_MASTER_MEM_MONITOR]   = "MEM_MONITOR",
    [APM_LL_MASTER_TRACE]         = "TRACE",
    [APM_LL_MASTER_GDMA_SPI2]     = "GDMA_SPI2",
    [APM_LL_MASTER_GDMA_UHCI0]    = "GDMA_UHCI0",
    [APM_LL_MASTER_GDMA_I2S0]     = "GDMA_I2S0",
    [APM_LL_MASTER_GDMA_AES]      = "GDMA_AES",
    [APM_LL_MASTER_GDMA_SHA]      = "GDMA_SHA",
    [APM_LL_MASTER_GDMA_ADC]      = "GDMA_ADC",
    [APM_LL_MASTER_GDMA_PARLIO]   = "GDMA_PARLIO",
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

const char *esp_tee_apm_excp_ctrl_to_str(apm_ll_apm_ctrl_t apm_ctrl)
{
    char *excp_ctrl = NULL;

    switch (apm_ctrl) {
#if SOC_APM_LP_APM0_SUPPORTED
    case LP_APM0_CTRL:
        excp_ctrl = "LP_APM0";
        break;
#endif
    case HP_APM_CTRL:
        excp_ctrl = "HP_APM";
        break;
    case LP_APM_CTRL:
        excp_ctrl = "LP_APM";
        break;
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
    case APM_LL_SECURE_MODE_TEE:
    case APM_LL_SECURE_MODE_REE0:
        excp_mode = "REE0";
        break;
    case APM_LL_SECURE_MODE_REE1:
        excp_mode = "REE1";
        break;
    case APM_LL_SECURE_MODE_REE2:
        excp_mode = "REE2";
        break;
    default:
        excp_mode = "Unknown";
        break;
    }

    return excp_mode;
}
