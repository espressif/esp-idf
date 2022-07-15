/*
 * SPDX-FileCopyrightText: 2020-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdbool.h>
#include <string.h>
#include "hal/assert.h"
#include "rev2/soc/ecc_mult_reg.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    ECC_PARAM_PX = 0x0,
    ECC_PARAM_PY,
    ECC_PARAM_K,
} ecc_ll_param_t;

static inline void ecc_ll_enable_interrupt(void)
{
    REG_SET_FIELD(ECC_MULT_INT_ENA_REG, ECC_MULT_CALC_DONE_INT_ENA, 1);
}

static inline void ecc_ll_disable_interrupt(void)
{
    REG_SET_FIELD(ECC_MULT_INT_ENA_REG, ECC_MULT_CALC_DONE_INT_ENA, 0);
}

static inline void ecc_ll_clear_interrupt(void)
{
    REG_SET_FIELD(ECC_MULT_INT_CLR_REG, ECC_MULT_CALC_DONE_INT_CLR, 1);
}

static inline void ecc_ll_set_mode(ecc_mode_t mode)
{
    switch(mode) {
        case ECC_MODE_POINT_MUL:
            REG_SET_FIELD(ECC_MULT_CONF_REG, ECC_MULT_WORK_MODE, 0);
            break;
        case ECC_MODE_INVERSE_MUL:
            REG_SET_FIELD(ECC_MULT_CONF_REG, ECC_MULT_WORK_MODE, 1);
            break;
        case ECC_MODE_VERIFY:
            REG_SET_FIELD(ECC_MULT_CONF_REG, ECC_MULT_WORK_MODE, 2);
            break;
        case ECC_MODE_VERIFY_THEN_POINT_MUL:
            REG_SET_FIELD(ECC_MULT_CONF_REG, ECC_MULT_WORK_MODE, 3);
            break;
        default:
            HAL_ASSERT(false && "Unsupported mode");
            break;
    }
}

static inline void ecc_ll_set_curve(ecc_curve_t curve)
{
    switch(curve) {
        case ECC_CURVE_SECP256R1:
            REG_SET_BIT(ECC_MULT_CONF_REG, ECC_MULT_KEY_LENGTH);
            break;
        case ECC_CURVE_SECP192R1:
            REG_CLR_BIT(ECC_MULT_CONF_REG, ECC_MULT_KEY_LENGTH);
            break;
        default:
            HAL_ASSERT(false && "Unsupported curve");
            return;
    }
}

static inline void ecc_ll_write_param(ecc_ll_param_t param, const uint8_t *buf, uint16_t len)
{
    uint32_t reg;
    uint32_t word;
    switch (param) {
        case ECC_PARAM_PX:
            reg = ECC_MULT_PX_1_REG;
            break;
        case ECC_PARAM_PY:
            reg = ECC_MULT_PY_1_REG;
            break;
        case ECC_PARAM_K:
            reg = ECC_MULT_K_1_REG;
            break;
        default:
            HAL_ASSERT(false && "Invalid parameter");
            return;
    }

    for (int i = 0; i < len; i += 4) {
        memcpy(&word, buf + i, 4);
        REG_WRITE(reg + i, word);
    }
}

static inline void ecc_ll_start_calc(void)
{
    REG_SET_BIT(ECC_MULT_CONF_REG, ECC_MULT_START);
}

static inline int ecc_ll_is_calc_finished(void)
{
    return REG_GET_FIELD(ECC_MULT_INT_RAW_REG, ECC_MULT_CALC_DONE_INT_RAW);
}

static inline ecc_mode_t ecc_ll_get_mode(void)
{
    return REG_GET_FIELD(ECC_MULT_CONF_REG, ECC_MULT_WORK_MODE);
}

static inline int ecc_ll_get_verification_result(void)
{
    return REG_GET_FIELD(ECC_MULT_CONF_REG, ECC_MULT_VERIFICATION_RESULT);
}

static inline ecc_curve_t ecc_ll_get_curve(void)
{
    return REG_GET_FIELD(ECC_MULT_CONF_REG, ECC_MULT_KEY_LENGTH);
}

static inline void ecc_ll_read_param(ecc_ll_param_t param, uint8_t *buf, uint16_t len)
{
    uint32_t reg;
    switch (param) {
        case ECC_PARAM_PX:
            reg = ECC_MULT_PX_1_REG;
            break;
        case ECC_PARAM_PY:
            reg = ECC_MULT_PY_1_REG;
            break;
        case ECC_PARAM_K:
            reg = ECC_MULT_K_1_REG;
            break;
        default:
            HAL_ASSERT(false && "Invalid parameter");
            return;
    }

    memcpy(buf, (void *)reg, len);
}

#ifdef __cplusplus
}
#endif
