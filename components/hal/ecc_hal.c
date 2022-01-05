/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "hal/ecc_hal.h"
#include "hal/ecc_ll.h"

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
