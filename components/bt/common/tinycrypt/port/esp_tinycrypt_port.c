/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "esp_tinycrypt_port.h"
#if SOC_ECC_SUPPORTED
#include "esp_crypto_lock.h"
#include "esp_private/periph_ctrl.h"

#include "hal/ecc_hal.h"
#include "hal/ecc_ll.h"

static void  esp_tinycrypt_acquire_ecc_hardware(void)
{
    esp_crypto_ecc_lock_acquire();

    periph_module_enable(PERIPH_ECC_MODULE);
    ecc_ll_power_up();
}

static void  esp_tinycrypt_release_ecc_hardware(void)
{
    periph_module_disable(PERIPH_ECC_MODULE);
    ecc_ll_power_down();

    esp_crypto_ecc_lock_release();
}

int esp_tinycrypt_verify_ecc_point(const uint8_t *pk_x, const uint8_t *pk_y, uint8_t length)
{
    int result;

    esp_tinycrypt_acquire_ecc_hardware();

    ecc_hal_write_verify_param(pk_x, pk_y, length);
    ecc_hal_set_mode(ECC_MODE_VERIFY);
    ecc_hal_start_calc();
    while (!ecc_hal_is_calc_finished());
    result = ecc_hal_read_verify_result();

    esp_tinycrypt_release_ecc_hardware();

    if (result == 1) {
        return 0;
    } else {
        return -1;
    }
}

int esp_tinycrypt_calc_ecc_mult(const uint8_t *p_x, const uint8_t *p_y, const uint8_t *scalar,
                                uint8_t *r_x, uint8_t *r_y, uint8_t num_bytes, bool verify_first)
{
    int ret = -1;
    ecc_mode_t work_mode = verify_first ? ECC_MODE_VERIFY_THEN_POINT_MUL : ECC_MODE_POINT_MUL;

    esp_tinycrypt_acquire_ecc_hardware();

    ecc_hal_write_mul_param(scalar, p_x, p_y, num_bytes);
    ecc_hal_set_mode(work_mode);
    /*
     * Enable constant-time point multiplication operations for the ECC hardware accelerator,
     * if supported for the given target. This protects the ECC multiplication operation from
     * timing attacks. This increases the time taken (by almost 50%) for some point
     * multiplication operations performed by the ECC hardware accelerator.
     */
    ecc_hal_enable_constant_time_point_mul(true);
    ecc_hal_start_calc();

    while (!ecc_hal_is_calc_finished());

    ret = ecc_hal_read_mul_result(r_x, r_y, num_bytes);

    esp_tinycrypt_release_ecc_hardware();

    return ret;
}
#endif /* SOC_ECC_SUPPORTED */
