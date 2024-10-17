/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*******************************************************************************
 * NOTICE
 * The hal is not public api, don't use it in application code.
 ******************************************************************************/

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "soc/hwcrypto_reg.h"
#include "soc/hp_sys_clkrst_struct.h"
#include "soc/soc_caps.h"
#include "hal/ds_types.h"


#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Enable the bus clock for DS peripheral module
 *
 * @param true to enable the module, false to disable the module
 */
static inline void _ds_ll_enable_bus_clock(bool enable)
{
    HP_SYS_CLKRST.peri_clk_ctrl25.reg_crypto_ds_clk_en = enable;
}

/// use a macro to wrap the function, force the caller to use it in a critical section
/// the critical section needs to declare the __DECLARE_RCC_ATOMIC_ENV variable in advance
#define ds_ll_enable_bus_clock(...) (void)__DECLARE_RCC_ATOMIC_ENV; _ds_ll_enable_bus_clock(__VA_ARGS__)

/**
 * @brief Reset the DS peripheral module
 */
static inline void ds_ll_reset_register(void)
{
    HP_SYS_CLKRST.hp_rst_en2.reg_rst_en_ds = 1;
    HP_SYS_CLKRST.hp_rst_en2.reg_rst_en_ds = 0;

    // Clear reset on parent crypto, otherwise DS is held in reset
    HP_SYS_CLKRST.hp_rst_en2.reg_rst_en_crypto = 0;
}

/// use a macro to wrap the function, force the caller to use it in a critical section
/// the critical section needs to declare the __DECLARE_RCC_ATOMIC_ENV variable in advance
#define ds_ll_reset_register(...) (void)__DECLARE_RCC_ATOMIC_ENV; ds_ll_reset_register(__VA_ARGS__)

static inline void ds_ll_start(void)
{
    REG_WRITE(DS_SET_START_REG, 1);
}

/**
 * @brief Wait until DS peripheral has finished any outstanding operation.
 */
static inline bool ds_ll_busy(void)
{
    return (REG_READ(DS_QUERY_BUSY_REG) > 0) ? true : false;
}

/**
 * @brief Busy wait until the hardware is ready.
 */
static inline void ds_ll_wait_busy(void)
{
    while (ds_ll_busy());
}

/**
 * @brief In case of a key error, check what caused it.
 */
static inline ds_key_check_t ds_ll_key_error_source(void)
{
    uint32_t key_error = REG_READ(DS_QUERY_KEY_WRONG_REG);
    if (key_error == 0) {
        return DS_NO_KEY_INPUT;
    } else {
        return DS_OTHER_WRONG;
    }
}

/**
 * @brief Write the initialization vector to the corresponding register field.
 */
static inline void ds_ll_configure_iv(const uint32_t *iv)
{
    for (size_t i = 0; i < (SOC_DS_KEY_PARAM_MD_IV_LENGTH / sizeof(uint32_t)); i++) {
        REG_WRITE(DS_IV_MEM + (i * 4) , iv[i]);
    }
}

/**
 * @brief Write the message which should be signed.
 *
 * @param msg Pointer to the message.
 * @param size Length of msg in bytes. It is the RSA signature length in bytes.
 */
static inline void ds_ll_write_message(const uint8_t *msg, size_t size)
{
    memcpy((uint8_t*) DS_X_MEM, msg, size);
    asm volatile ("fence");
}

/**
 * @brief Write the encrypted private key parameters.
 */
static inline void ds_ll_write_private_key_params(const uint8_t *encrypted_key_params)
{
    /* Note: as the internal peripheral still has RSA 4096 structure,
       but C is encrypted based on the actual max RSA length (ETS_DS_MAX_BITS), need to fragment it
       when copying to hardware...

       (note if ETS_DS_MAX_BITS == 4096, this should be the same as copying data->c to hardware in one fragment)
    */
    typedef struct { uint32_t addr; size_t len; } frag_t;
    const frag_t frags[] = {
                            {DS_Y_MEM,  SOC_DS_SIGNATURE_MAX_BIT_LEN / 8},
                            {DS_M_MEM,  SOC_DS_SIGNATURE_MAX_BIT_LEN / 8},
                            {DS_RB_MEM, SOC_DS_SIGNATURE_MAX_BIT_LEN / 8},
                            {DS_BOX_MEM, DS_IV_MEM - DS_BOX_MEM},
    };
    const size_t NUM_FRAGS = sizeof(frags)/sizeof(frag_t);
    const uint8_t *from = encrypted_key_params;

    for (int i = 0; i < NUM_FRAGS; i++) {
        memcpy((uint8_t *)frags[i].addr, from, frags[i].len);
        asm volatile ("fence");
        from += frags[i].len;
    }
}

/**
 * @brief Begin signing procedure.
 */
static inline void ds_ll_start_sign(void)
{
    REG_WRITE(DS_SET_CONTINUE_REG, 1);
}

/**
 * @brief check the calculated signature.
 *
 * @return
 * - DS_SIGNATURE_OK if no issue is detected with the signature.
 * - DS_SIGNATURE_PADDING_FAIL if the padding of the private key parameters is wrong.
 * - DS_SIGNATURE_MD_FAIL if the message digest check failed. This means that the message digest calculated using
 *      the private key parameters fails, i.e., the integrity of the private key parameters is not protected.
 * - DS_SIGNATURE_PADDING_AND_MD_FAIL if both padding and message digest check fail.
 */
static inline ds_signature_check_t ds_ll_check_signature(void)
{
    uint32_t result = REG_READ(DS_QUERY_CHECK_REG);
    switch(result) {
    case 0:
        return DS_SIGNATURE_OK;
    case 1:
        return DS_SIGNATURE_MD_FAIL;
    case 2:
        return DS_SIGNATURE_PADDING_FAIL;
    default:
        return DS_SIGNATURE_PADDING_AND_MD_FAIL;
    }
}

/**
 * @brief Read the signature from the hardware.
 *
 * @param result The signature result.
 * @param size Length of signature result in bytes. It is the RSA signature length in bytes.
 */
static inline void ds_ll_read_result(uint8_t *result, size_t size)
{
    memcpy(result, (uint8_t*) DS_Z_MEM, size);
    asm volatile ("fence");
}

/**
 * @brief Exit the signature operation.
 *
 * @note This does not deactivate the module. Corresponding clock/reset bits have to be triggered for deactivation.
 */
static inline void ds_ll_finish(void)
{
    REG_WRITE(DS_SET_FINISH_REG, 1);
    ds_ll_wait_busy();
}

#ifdef __cplusplus
}
#endif
