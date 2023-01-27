/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "soc/tee_reg.h"

#define TEE_LL_MODE_CTRL_REG(master_id) (TEE_M0_MODE_CTRL_REG + 4 * (master_id))

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief APM Master ID
 */
typedef enum {
    APM_LL_MASTER_HPCORE       = 0,
    APM_LL_MASTER_LPCORE       = 1,
    APM_LL_MASTER_REGDMA       = 2,
    APM_LL_MASTER_SDIOSLV      = 3,
    APM_LL_MASTER_MODEM        = 4,
    APM_LL_MASTER_MEM_MONITOR  = 5,
    APM_LL_MASTER_TRACE        = 6,
    APM_LL_MASTER_GDMA_SPI2    = 16,
    APM_LL_MASTER_GDMA_M2M     = 17, // a dummy GDMA trigger, used by M2M copy
    APM_LL_MASTER_GDMA_UHCI0   = 18,
    APM_LL_MASTER_GDMA_I2S0    = 19,
    APM_LL_MASTER_GDMA_AES     = 22,
    APM_LL_MASTER_GDMA_SHA     = 23,
    APM_LL_MASTER_GDMA_ADC     = 24,
    APM_LL_MASTER_GDMA_PARLIO  = 25,
} apm_ll_master_id_t;

/**
 * @brief APM Secure Mode
 */
typedef enum {
    APM_LL_SECURE_MODE_TEE = 0,  /* Trusted execution environment mode */
    APM_LL_SECURE_MODE_REE0 = 1, /* Rich execution environment mode0 (need to configure APM strategy for this mode) */
    APM_LL_SECURE_MODE_REE1 = 2, /* Rich execution environment mode1 (need to configure APM strategy for this mode) */
    APM_LL_SECURE_MODE_REE2 = 3, /* Rich execution environment mode2 (need to configure APM strategy for this mode) */
} apm_ll_secure_mode_t;

/**
 * @brief Set secure mode
 *
 * @param master_id APM master ID
 * @param mode Secure mode
 */
static inline void apm_ll_set_master_secure_mode(apm_ll_master_id_t master_id, apm_ll_secure_mode_t mode)
{
    REG_WRITE(TEE_LL_MODE_CTRL_REG(master_id), mode);
}

#ifdef __cplusplus
}
#endif
