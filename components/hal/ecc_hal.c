/*
 * SPDX-FileCopyrightText: 2021-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "hal/ecc_hal.h"
#include "hal/ecc_ll.h"
#include "soc/soc_caps.h"

void ecc_hal_set_mode(ecc_mode_t mode)
{
    ecc_ll_set_mode(mode);
}

void ecc_hal_set_curve(ecc_curve_t curve)
{
    ecc_ll_set_curve(curve);
}

void ecc_hal_start_calc(void)
{
    ecc_ll_clear_interrupt();
    ecc_ll_start_calc();
}

int ecc_hal_is_calc_finished(void)
{
    return ecc_ll_is_calc_finished();
}

static void clear_param_registers(void)
{
    uint8_t buf[32] = {0};

    ecc_ll_write_param(ECC_PARAM_PX, buf, sizeof(buf));
    ecc_ll_write_param(ECC_PARAM_PY, buf, sizeof(buf));
    ecc_ll_write_param(ECC_PARAM_K, buf, sizeof(buf));
#if SOC_ECC_EXTENDED_MODES_SUPPORTED
    ecc_ll_write_param(ECC_PARAM_QX, buf, sizeof(buf));
    ecc_ll_write_param(ECC_PARAM_QY, buf, sizeof(buf));
    ecc_ll_write_param(ECC_PARAM_QZ, buf, sizeof(buf));
#endif
}

void ecc_hal_write_mul_param(const uint8_t *k, const uint8_t *px, const uint8_t *py, uint16_t len)
{
    ecc_curve_t curve = len == 32 ? ECC_CURVE_SECP256R1 : ECC_CURVE_SECP192R1;
    ecc_ll_set_curve(curve);

    clear_param_registers();

    ecc_ll_write_param(ECC_PARAM_K, k, len);
    ecc_ll_write_param(ECC_PARAM_PX, px, len);
    ecc_ll_write_param(ECC_PARAM_PY, py, len);
}

void ecc_hal_write_verify_param(const uint8_t *px, const uint8_t *py, uint16_t len)
{
    ecc_curve_t curve = len == 32 ? ECC_CURVE_SECP256R1 : ECC_CURVE_SECP192R1;
    ecc_ll_set_curve(curve);

    clear_param_registers();

    ecc_ll_write_param(ECC_PARAM_PX, px, len);
    ecc_ll_write_param(ECC_PARAM_PY, py, len);
}

int ecc_hal_read_mul_result(uint8_t *rx, uint8_t *ry, uint16_t len)
{
    ecc_mode_t mode = ecc_ll_get_mode();

    if (mode == ECC_MODE_VERIFY_THEN_POINT_MUL) {
        if (!ecc_ll_get_verification_result()) {
            memset(rx, 0x0, len);
            memset(ry, 0x0, len);
            return -1;
        }
    }

    ecc_ll_read_param(ECC_PARAM_PX, rx, len);
    ecc_ll_read_param(ECC_PARAM_PY, ry, len);
    return 0;
}

int ecc_hal_read_verify_result(void)
{
    return ecc_ll_get_verification_result();
}

#if SOC_ECC_EXTENDED_MODES_SUPPORTED

void ecc_hal_set_mod_base(ecc_mod_base_t base)
{
    ecc_ll_set_mod_base(base);
}

void ecc_hal_write_jacob_verify_param(const uint8_t *qx, const uint8_t *qy, const uint8_t *qz, uint16_t len)
{
    ecc_curve_t curve = len == 32 ? ECC_CURVE_SECP256R1 : ECC_CURVE_SECP192R1;
    ecc_ll_set_curve(curve);

    clear_param_registers();

    ecc_ll_write_param(ECC_PARAM_QX, qx, len);
    ecc_ll_write_param(ECC_PARAM_QY, qy, len);
    ecc_ll_write_param(ECC_PARAM_QZ, qz, len);
}

int ecc_hal_read_jacob_mul_result(uint8_t *rx, uint8_t *ry, uint8_t *rz, uint16_t len)
{
    ecc_mode_t mode = ecc_ll_get_mode();

    if (mode == ECC_MODE_POINT_VERIFY_JACOBIAN_MUL) {
        if (!ecc_ll_get_verification_result()) {
            memset(rx, 0x0, len);
            memset(ry, 0x0, len);
            memset(rz, 0x0, len);
            return -1;
        }
    }

    ecc_ll_read_param(ECC_PARAM_QX, rx, len);
    ecc_ll_read_param(ECC_PARAM_QY, ry, len);
    ecc_ll_read_param(ECC_PARAM_QZ, rz, len);
    return 0;
}

void ecc_hal_write_point_add_param(const uint8_t *px, const uint8_t *py, const uint8_t *qx, const uint8_t *qy, const uint8_t *qz, uint16_t len)
{
    ecc_curve_t curve = len == 32 ? ECC_CURVE_SECP256R1 : ECC_CURVE_SECP192R1;
    ecc_ll_set_curve(curve);

    clear_param_registers();

    ecc_ll_write_param(ECC_PARAM_PX, px, len);
    ecc_ll_write_param(ECC_PARAM_PY, py, len);
    ecc_ll_write_param(ECC_PARAM_QX, qx, len);
    ecc_ll_write_param(ECC_PARAM_QY, qy, len);
    ecc_ll_write_param(ECC_PARAM_QZ, qz, len);
}

int ecc_hal_read_point_add_result(uint8_t *rx, uint8_t *ry, uint8_t *rz, uint16_t len, bool read_jacob)
{
    if (read_jacob) {
        ecc_ll_read_param(ECC_PARAM_QX, rx, len);
        ecc_ll_read_param(ECC_PARAM_QY, ry, len);
        ecc_ll_read_param(ECC_PARAM_QZ, rz, len);
    } else {
        ecc_ll_read_param(ECC_PARAM_PX, rx, len);
        ecc_ll_read_param(ECC_PARAM_PY, ry, len);
    }
    return 0;
}

void ecc_hal_write_mod_op_param(const uint8_t *a, const uint8_t *b, uint16_t len)
{
    ecc_curve_t curve = len == 32 ? ECC_CURVE_SECP256R1 : ECC_CURVE_SECP192R1;
    ecc_ll_set_curve(curve);

    clear_param_registers();

    ecc_ll_write_param(ECC_PARAM_PX, a, len);
    ecc_ll_write_param(ECC_PARAM_PY, b, len);
}

int ecc_hal_read_mod_op_result(uint8_t *r, uint16_t len)
{
    ecc_mode_t mode = ecc_ll_get_mode();
    if (mode == ECC_MODE_MOD_ADD || mode == ECC_MODE_MOD_SUB) {
        ecc_ll_read_param(ECC_PARAM_PX, r, len);
    } else if (mode == ECC_MODE_MOD_MUL || mode == ECC_MODE_INVERSE_MUL) {
        ecc_ll_read_param(ECC_PARAM_PY, r, len);
    } else {
        return -1;
    }
    return 0;
}

#endif /* SOC_ECC_EXTENDED_MODES_SUPPORTED */

#ifdef SOC_ECC_CONSTANT_TIME_POINT_MUL
void ecc_hal_enable_constant_time_point_mul(bool enable)
{
    ecc_ll_enable_constant_time_point_mul(enable);
}
#endif /* SOC_ECC_CONSTANT_TIME_POINT_MUL */
