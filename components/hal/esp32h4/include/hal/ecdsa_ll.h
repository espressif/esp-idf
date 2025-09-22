/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdbool.h>
#include <string.h>
#include "hal/assert.h"
#include "soc/ecdsa_reg.h"
#include "soc/pcr_struct.h"
#include "soc/efuse_periph.h"
#include "hal/ecdsa_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Set the ECDSA key block in eFuse
 *
 * @param curve    ECDSA curve type
 * @param efuse_blk eFuse block number
 */
__attribute__((always_inline)) static inline void ecdsa_ll_set_ecdsa_key_blk(ecdsa_curve_t curve, int efuse_blk)
{
    //ESP32H4 TODO
    (void)curve;
    (void)efuse_blk;
}

/**
 * @brief Check if the ECDSA peripheral is supported on this chip revision
 * For ESP32-H4, ECDSA is always supported
 */
static inline bool ecdsa_ll_is_supported(void)
{
    return true;
}

#ifdef __cplusplus
}
#endif
