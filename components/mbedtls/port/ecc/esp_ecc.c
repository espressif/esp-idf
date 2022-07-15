/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include <stdio.h>

#include "esp_private/periph_ctrl.h"
#include "ecc_impl.h"
#include "hal/ecc_hal.h"

static _lock_t s_crypto_ecc_lock;

static void esp_ecc_acquire_hardware(void)
{
    _lock_acquire(&s_crypto_ecc_lock);

    periph_module_enable(PERIPH_ECC_MODULE);
}

static void esp_ecc_release_hardware(void)
{
    periph_module_disable(PERIPH_ECC_MODULE);

    _lock_release(&s_crypto_ecc_lock);
}

int esp_ecc_point_multiply(const ecc_point_t *point, const uint8_t *scalar, ecc_point_t *result, bool verify_first)
{
    int ret = -1;
    uint16_t len = point->len;
    ecc_mode_t work_mode = verify_first ? ECC_MODE_VERIFY_THEN_POINT_MUL : ECC_MODE_POINT_MUL;

    esp_ecc_acquire_hardware();

    ecc_hal_write_mul_param(scalar, point->x, point->y, len);
    ecc_hal_set_mode(work_mode);
    ecc_hal_start_calc();

    memset(result, 0, sizeof(ecc_point_t));

    result->len = len;

    while (!ecc_hal_is_calc_finished()) {
        ;
    }

    ret = ecc_hal_read_mul_result(result->x, result->y, len);

    esp_ecc_release_hardware();

    return ret;
}

int esp_ecc_point_verify(const ecc_point_t *point)
{
    int result;

    esp_ecc_acquire_hardware();
    ecc_hal_write_verify_param(point->x, point->y, point->len);
    ecc_hal_set_mode(ECC_MODE_VERIFY);
    ecc_hal_start_calc();

    while (!ecc_hal_is_calc_finished()) {
        ;
    }

    result = ecc_hal_read_verify_result();

    esp_ecc_release_hardware();

    return result;
}
