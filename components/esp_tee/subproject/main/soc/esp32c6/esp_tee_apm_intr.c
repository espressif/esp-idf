/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdint.h>
#include <assert.h>

#include "hal/apm_ll.h"
#include "hal/apm_hal.h"

#include "esp_tee.h"
#include "esp_tee_apm_intr.h"

const char *esp_tee_apm_excp_mid_to_str(uint8_t mid)
{
    char *excp_mid = NULL;

    switch (mid) {
    case APM_LL_MASTER_HPCORE:
        excp_mid = "HPCORE";
        break;
    case APM_LL_MASTER_LPCORE:
        excp_mid = "LPCORE";
        break;
    case APM_LL_MASTER_REGDMA:
        excp_mid = "REGDMA";
        break;
    case APM_LL_MASTER_SDIOSLV:
        excp_mid = "SDIOSLV";
        break;
    case APM_LL_MASTER_MODEM:
        excp_mid = "MODEM";
        break;
    case APM_LL_MASTER_MEM_MONITOR:
        excp_mid = "MEM_MONITOR";
        break;
    case APM_LL_MASTER_TRACE:
        excp_mid = "TRACE";
        break;
    case APM_LL_MASTER_GDMA_SPI2:
        excp_mid = "GDMA_SPI2";
        break;
    case APM_LL_MASTER_GDMA_UHCI0:
        excp_mid = "GDMA_UHCI0";
        break;
    case APM_LL_MASTER_GDMA_I2S0:
        excp_mid = "GDMA_I2S0";
        break;
    case APM_LL_MASTER_GDMA_AES:
        excp_mid = "GDMA_AES";
        break;
    case APM_LL_MASTER_GDMA_SHA:
        excp_mid = "GDMA_SHA";
        break;
    case APM_LL_MASTER_GDMA_ADC:
        excp_mid = "GDMA_ADC";
        break;
    case APM_LL_MASTER_GDMA_PARLIO:
        excp_mid = "GDMA_PARLIO";
        break;
    default:
        excp_mid = "Unknown";
        break;
    }

    return excp_mid;
}

const char *esp_tee_apm_excp_type_to_str(uint8_t type)
{
    char *excp_type = "Unknown exception";

    if (type & 0x01) {
        excp_type = "Authority exception";
    } else if (type & 0x02) {
        excp_type = "Space exception";
    }

    return excp_type;
}

const char *esp_tee_apm_excp_ctrl_to_str(apm_ll_apm_ctrl_t apm_ctrl)
{
    char *excp_ctrl = NULL;

    switch (apm_ctrl) {
    case LP_APM0_CTRL:
        excp_ctrl = "LP_APM0";
        break;
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
