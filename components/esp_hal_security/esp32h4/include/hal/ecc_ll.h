/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdbool.h>
#include <string.h>
#include "hal/assert.h"
#include "hal/ecc_types.h"
#include "soc/ecc_mult_reg.h"
#include "soc/pcr_struct.h"
#include "soc/pcr_reg.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    ECC_PARAM_PX = 0x0,
    ECC_PARAM_PY,
    ECC_PARAM_K,
    ECC_PARAM_QX,
    ECC_PARAM_QY,
    ECC_PARAM_QZ,
} ecc_ll_param_t;

/**
 * @brief Enable the bus clock for ECC peripheral module
 *
 * @param true to enable the module, false to disable the module
 */
static inline void ecc_ll_enable_bus_clock(bool enable)
{
    PCR.ecc_conf.ecc_clk_en = enable;
}

/**
 * @brief Reset the ECC peripheral module
 */
static inline void ecc_ll_reset_register(void)
{
    PCR.ecc_conf.ecc_rst_en = 1;
    PCR.ecc_conf.ecc_rst_en = 0;
    // Clear reset on ECDSA, otherwise ECC is held in reset
    PCR.ecdsa_conf.ecdsa_rst_en = 0;
}

static inline void ecc_ll_power_up(void)
{

    /* Power up the ECC peripheral (default state is power-down) */
    REG_CLR_BIT(PCR_ECC_MEM_LP_CTRL_REG, PCR_ECC_MEM_LP_EN);
    REG_CLR_BIT(PCR_ECC_MEM_LP_CTRL_REG, PCR_ECC_MEM_FORCE_CTRL);
}

static inline void ecc_ll_power_down(void)
{
    /* Power down the ECC peripheral */
    REG_CLR_BIT(PCR_ECC_MEM_LP_CTRL_REG, PCR_ECC_MEM_LP_MODE);
    REG_SET_BIT(PCR_ECC_MEM_LP_CTRL_REG, PCR_ECC_MEM_LP_EN);
}

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
    switch (mode) {
    case ECC_MODE_POINT_MUL:
        REG_SET_FIELD(ECC_MULT_CONF_REG, ECC_MULT_WORK_MODE, 0);
        break;
    case ECC_MODE_VERIFY:
        REG_SET_FIELD(ECC_MULT_CONF_REG, ECC_MULT_WORK_MODE, 2);
        break;
    case ECC_MODE_VERIFY_THEN_POINT_MUL:
        REG_SET_FIELD(ECC_MULT_CONF_REG, ECC_MULT_WORK_MODE, 3);
        break;
    case ECC_MODE_JACOBIAN_POINT_MUL:
        REG_SET_FIELD(ECC_MULT_CONF_REG, ECC_MULT_WORK_MODE, 4);
        break;
    case ECC_MODE_POINT_ADD:
        REG_SET_FIELD(ECC_MULT_CONF_REG, ECC_MULT_WORK_MODE, 5);
        break;
    case ECC_MODE_JACOBIAN_POINT_VERIFY:
        REG_SET_FIELD(ECC_MULT_CONF_REG, ECC_MULT_WORK_MODE, 6);
        break;
    case ECC_MODE_POINT_VERIFY_JACOBIAN_MUL:
        REG_SET_FIELD(ECC_MULT_CONF_REG, ECC_MULT_WORK_MODE, 7);
        break;
    case ECC_MODE_MOD_ADD:
        REG_SET_FIELD(ECC_MULT_CONF_REG, ECC_MULT_WORK_MODE, 8);
        break;
    case ECC_MODE_MOD_SUB:
        REG_SET_FIELD(ECC_MULT_CONF_REG, ECC_MULT_WORK_MODE, 9);
        break;
    case ECC_MODE_MOD_MUL:
        REG_SET_FIELD(ECC_MULT_CONF_REG, ECC_MULT_WORK_MODE, 10);
        break;
    case ECC_MODE_INVERSE_MUL:
        REG_SET_FIELD(ECC_MULT_CONF_REG, ECC_MULT_WORK_MODE, 11);
        break;
    default:
        HAL_ASSERT(false && "Unsupported mode");
        break;
    }
}

static inline void ecc_ll_set_curve(ecc_curve_t curve)
{
    switch (curve) {
    case ECC_CURVE_SECP192R1:
    case ECC_CURVE_SECP256R1:
    case ECC_CURVE_SECP384R1:
    case ECC_CURVE_SM2:
        REG_SET_FIELD(ECC_MULT_CONF_REG, ECC_MULT_KEY_LENGTH, curve);
        break;
    default:
        HAL_ASSERT(false && "Unsupported curve");
        return;
    }
}

static inline void ecc_ll_set_mod_base(ecc_mod_base_t base)
{
    switch (base) {
    case ECC_MOD_N:
        REG_CLR_BIT(ECC_MULT_CONF_REG, ECC_MULT_MOD_BASE);
        break;
    case ECC_MOD_P:
        REG_SET_BIT(ECC_MULT_CONF_REG, ECC_MULT_MOD_BASE);
        break;
    default:
        HAL_ASSERT(false && "Unsupported curve");
        return;
    }
}

static inline void ecc_ll_enable_constant_time_point_mul(bool enable)
{
    if (enable) {
        REG_SET_BIT(ECC_MULT_CONF_REG, ECC_MULT_SECURITY_MODE);
    } else {
        REG_CLR_BIT(ECC_MULT_CONF_REG, ECC_MULT_SECURITY_MODE);
    }
}

static inline void ecc_ll_write_param(ecc_ll_param_t param, const uint8_t *buf, uint16_t len)
{
    uint32_t reg;
    uint32_t word;
    switch (param) {
    case ECC_PARAM_PX:
        reg = ECC_MULT_PX_MEM;
        break;
    case ECC_PARAM_PY:
        reg = ECC_MULT_PY_MEM;
        break;
    case ECC_PARAM_K:
        reg = ECC_MULT_K_MEM;
        break;
    case ECC_PARAM_QX:
        reg = ECC_MULT_QX_MEM;
        break;
    case ECC_PARAM_QY:
        reg = ECC_MULT_QY_MEM;
        break;
    case ECC_PARAM_QZ:
        reg = ECC_MULT_QZ_MEM;
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
    return (ecc_mode_t)(REG_GET_FIELD(ECC_MULT_CONF_REG, ECC_MULT_WORK_MODE));
}

static inline int ecc_ll_get_verification_result(void)
{
    return REG_GET_FIELD(ECC_MULT_CONF_REG, ECC_MULT_VERIFICATION_RESULT);
}

static inline ecc_curve_t ecc_ll_get_curve(void)
{
    return (ecc_curve_t)(REG_GET_FIELD(ECC_MULT_CONF_REG, ECC_MULT_KEY_LENGTH));
}

static inline ecc_mod_base_t ecc_ll_get_mod_base(void)
{
    return (ecc_mod_base_t)(REG_GET_FIELD(ECC_MULT_CONF_REG, ECC_MULT_MOD_BASE));
}

static inline void ecc_ll_read_param(ecc_ll_param_t param, uint8_t *buf, uint16_t len)
{
    uint32_t reg;
    switch (param) {
    case ECC_PARAM_PX:
        reg = ECC_MULT_PX_MEM;
        break;
    case ECC_PARAM_PY:
        reg = ECC_MULT_PY_MEM;
        break;
    case ECC_PARAM_K:
        reg = ECC_MULT_K_MEM;
        break;
    case ECC_PARAM_QX:
        reg = ECC_MULT_QX_MEM;
        break;
    case ECC_PARAM_QY:
        reg = ECC_MULT_QY_MEM;
        break;
    case ECC_PARAM_QZ:
        reg = ECC_MULT_QZ_MEM;
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
