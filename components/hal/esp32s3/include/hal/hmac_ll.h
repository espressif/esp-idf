/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*******************************************************************************
 * NOTICE
 * The hal is not public api, don't use it in application code.
 * See readme.md in soc/include/hal/readme.md
 ******************************************************************************/

#pragma once

#include <stddef.h> /* For size_t type */
#include "soc/hwcrypto_reg.h"

#define SHA256_BLOCK_SZ 64
#define SHA256_DIGEST_SZ 32

#define HMAC_LL_EFUSE_KEY_PURPOSE_DOWN_JTAG 6
#define HMAC_LL_EFUSE_KEY_PURPOSE_DOWN_DIGITAL_SIGNATURE 7
#define HMAC_LL_EFUSE_KEY_PURPOSE_UP 8
#define HMAC_LL_EFUSE_KEY_PURPOSE_DOWN_ALL 5

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Makes the peripheral ready for use, after enabling it.
 */
static inline void hmac_ll_start(void)
{
    REG_WRITE(HMAC_SET_START_REG, 1);
}

/**
 * @brief Determine where the HMAC output should go.
 *
 * The HMAC peripheral can be configured to deliver its output to the user directly, or to deliver
 * the output directly to another peripheral instead, e.g. the Digital Signature peripheral.
 */
static inline void hmac_ll_config_output(hmac_hal_output_t config)
{
    switch(config) {
    case HMAC_OUTPUT_USER:
        REG_WRITE(HMAC_SET_PARA_PURPOSE_REG, HMAC_LL_EFUSE_KEY_PURPOSE_UP);
        break;
    case HMAC_OUTPUT_DS:
        REG_WRITE(HMAC_SET_PARA_PURPOSE_REG, HMAC_LL_EFUSE_KEY_PURPOSE_DOWN_DIGITAL_SIGNATURE);
        break;
    case HMAC_OUTPUT_JTAG_ENABLE:
        REG_WRITE(HMAC_SET_PARA_PURPOSE_REG, HMAC_LL_EFUSE_KEY_PURPOSE_DOWN_JTAG);
        break;
    case HMAC_OUTPUT_ALL:
        REG_WRITE(HMAC_SET_PARA_PURPOSE_REG, HMAC_LL_EFUSE_KEY_PURPOSE_DOWN_ALL);
        break;
    default:
        ; // do nothing, error will be indicated by hmac_hal_config_error()
    }
}

/**
 * @brief Selects which hardware key should be used.
 */
static inline void hmac_ll_config_hw_key_id(uint32_t key_id)
{
    REG_WRITE(HMAC_SET_PARA_KEY_REG, key_id);
}

/**
 * @brief Apply and check configuration.
 *
 * Afterwards, the configuration can be checked for errors with hmac_hal_config_error().
 */
static inline void hmac_ll_config_finish(void)
{
    REG_WRITE(HMAC_SET_PARA_FINISH_REG, 1);
}

/**
 *
 * @brief Query HMAC error state after configuration actions.
 *
 * @return
 *  - 1 or greater on error
 *  - 0 on success
 */
static inline uint32_t hmac_ll_query_config_error(void)
{
    return REG_READ(HMAC_QUERY_ERROR_REG);
}

/**
 * Wait until the HAL is ready for the next interaction.
 */
static inline void hmac_ll_wait_idle(void)
{
    uint32_t query;
    do {
        query = REG_READ(HMAC_QUERY_BUSY_REG);
    } while(query != 0);
}

/**
 * @brief Write a message block of 512 bits to the HMAC peripheral.
 */
static inline void hmac_ll_write_block_512(const uint32_t *block)
{
    const size_t REG_WIDTH = sizeof(uint32_t);
    for (size_t i = 0; i < SHA256_BLOCK_SZ / REG_WIDTH; i++) {
        REG_WRITE(HMAC_WDATA_BASE + (i * REG_WIDTH), block[i]);
    }

    REG_WRITE(HMAC_SET_MESSAGE_ONE_REG, 1);
}

/**
 * @brief Read the 256 bit HMAC.
 */
static inline void hmac_ll_read_result_256(uint32_t *result)
{
    const size_t REG_WIDTH = sizeof(uint32_t);
    for (size_t i = 0; i < SHA256_DIGEST_SZ / REG_WIDTH; i++) {
        result[i] = REG_READ(HMAC_RDATA_BASE + (i * REG_WIDTH));
    }
}

/**
 * @brief Clean the HMAC result provided to other hardware.
 */
static inline void hmac_ll_clean(void)
{
    REG_WRITE(HMAC_SET_INVALIDATE_DS_REG, 1);
    REG_WRITE(HMAC_SET_INVALIDATE_JTAG_REG, 1);
}

/**
 * @brief Signals that the following block will be the padded last block.
 */
static inline void hmac_ll_msg_padding(void)
{
    REG_WRITE(HMAC_SET_MESSAGE_PAD_REG, 1);
}

/**
 * @brief Signals that all blocks have been written and a padding block will automatically be applied by hardware.
 *
 * Only applies if the message length is a multiple of 512 bits.
 * See ESP32S3 TRM HMAC chapter for more details.
 */
static inline void hmac_ll_msg_end(void)
{
    REG_WRITE(HMAC_SET_MESSAGE_END_REG, 1);
}

/**
 * @brief The message including padding fits into one block, so no further action needs to be taken.
 *
 * This is called after the one-block-message has been written.
 */
static inline void hmac_ll_msg_one_block(void)
{
    REG_WRITE(HMAC_ONE_BLOCK_REG, 1);
}

/**
 * @brief Indicate that more blocks will be written after the last block.
 */
static inline void hmac_ll_msg_continue(void)
{
    REG_WRITE(HMAC_SET_MESSAGE_ING_REG, 1);
}

/**
 * @brief Clear the HMAC result.
 *
 * Use this after reading the HMAC result or if aborting after any of the other steps above.
 */
static inline void hmac_ll_calc_finish(void)
{
    REG_WRITE(HMAC_SET_RESULT_FINISH_REG, 2);
}

#ifdef __cplusplus
}
#endif
