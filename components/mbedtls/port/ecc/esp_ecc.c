/*
 * SPDX-FileCopyrightText: 2021-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include <stdio.h>

#include "esp_crypto_lock.h"
#include "esp_private/esp_crypto_lock_internal.h"
#include "ecc_impl.h"
#include "hal/ecc_hal.h"
#include "hal/ecc_ll.h"
#include "soc/soc_caps.h"

static void esp_ecc_acquire_hardware(void)
{
    esp_crypto_ecc_lock_acquire();

    ECC_RCC_ATOMIC() {
        ecc_ll_enable_bus_clock(true);
        ecc_ll_power_up();
        ecc_ll_reset_register();
    }
}

static void esp_ecc_release_hardware(void)
{
    ECC_RCC_ATOMIC() {
        ecc_ll_enable_bus_clock(false);
        ecc_ll_power_down();
    }

    esp_crypto_ecc_lock_release();
}

int esp_ecc_point_multiply(const ecc_point_t *point, const uint8_t *scalar, ecc_point_t *result, bool verify_first)
{
    int ret = -1;
    uint16_t len = point->len;
    ecc_mode_t work_mode = verify_first ? ECC_MODE_VERIFY_THEN_POINT_MUL : ECC_MODE_POINT_MUL;

    esp_ecc_acquire_hardware();

    ecc_hal_write_mul_param(scalar, point->x, point->y, len);
    ecc_hal_set_mode(work_mode);
#ifdef SOC_ECC_CONSTANT_TIME_POINT_MUL
    /*  Enable constant-time point multiplication operations for the ECC hardware accelerator
        This protects the ECC multiplication operation from timing attacks.
        This increases the time taken (by almost 50%) for some point multiplication
        operations performed by the ECC hardware accelerator.
    */
    ecc_hal_enable_constant_time_point_mul(true);
#endif /* SOC_ECC_CONSTANT_TIME_POINT_MUL */
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
