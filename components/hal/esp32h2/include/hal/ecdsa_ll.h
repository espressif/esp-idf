/*
 * SPDX-FileCopyrightText: 2023-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdbool.h>
#include <string.h>
#include "hal/assert.h"
#include "soc/ecdsa_reg.h"
#include "soc/ecdsa_struct.h"
#include "soc/pcr_struct.h"
#include "hal/ecdsa_types.h"
#include "hal/ecc_ll.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Memory blocks of ECDSA parameters
 */
typedef enum {
    ECDSA_PARAM_R,
    ECDSA_PARAM_S,
    ECDSA_PARAM_Z,
    ECDSA_PARAM_QAX,
    ECDSA_PARAM_QAY
} ecdsa_ll_param_t;

/**
 * @brief Interrupt types in ECDSA
 */
typedef enum {
    ECDSA_INT_PREP_DONE,
    ECDSA_INT_SHA_RELEASE,
} ecdsa_ll_intr_type_t;

/**
 * @brief Stages of ECDSA operation
 */
typedef enum {
    ECDSA_STAGE_START_CALC,
    ECDSA_STAGE_LOAD_DONE,
    ECDSA_STAGE_GET_DONE
} ecdsa_ll_stage_t;

/**
 * @brief States of ECDSA peripheral
 */
typedef enum {
    ECDSA_STATE_IDLE,
    ECDSA_STATE_LOAD,
    ECDSA_STATE_GET,
    ECDSA_STATE_BUSY
} ecdsa_ll_state_t;

/**
 * @brief Types of SHA
 */
typedef enum {
    ECDSA_SHA_224,
    ECDSA_SHA_256
} ecdsa_ll_sha_type_t;

/**
 * @brief Operation modes of SHA
 */
typedef enum {
    ECDSA_MODE_SHA_START,
    ECDSA_MODE_SHA_CONTINUE
} ecdsa_ll_sha_mode_t;

/**
 * @brief Enable the bus clock for ECDSA peripheral module
 *
 * @param true to enable the module, false to disable the module
 */
static inline void ecdsa_ll_enable_bus_clock(bool enable)
{
    PCR.ecdsa_conf.ecdsa_clk_en = enable;
}

/**
 * @brief Reset the ECDSA peripheral module
 */
static inline void ecdsa_ll_reset_register(void)
{
    PCR.ecdsa_conf.ecdsa_rst_en = 1;
    PCR.ecdsa_conf.ecdsa_rst_en = 0;
}

/**
 * @brief Enable interrupt of a given type
 *
 * @param type Interrupt type
 */
static inline void ecdsa_ll_enable_intr(ecdsa_ll_intr_type_t type)
{
    switch (type) {
        case ECDSA_INT_PREP_DONE:
            REG_SET_FIELD(ECDSA_INT_ENA_REG, ECDSA_PREP_DONE_INT_ENA, 1);
            break;
        case ECDSA_INT_SHA_RELEASE:
            REG_SET_FIELD(ECDSA_INT_ENA_REG, ECDSA_SHA_RELEASE_INT_ENA, 1);
            break;
        default:
            HAL_ASSERT(false && "Unsupported interrupt type");
            break;
    }
}

/**
 * @brief Disable interrupt of a given type
 *
 * @param type Interrupt type
 */
static inline void ecdsa_ll_disable_intr(ecdsa_ll_intr_type_t type)
{
    switch (type) {
        case ECDSA_INT_PREP_DONE:
            REG_SET_FIELD(ECDSA_INT_ENA_REG, ECDSA_PREP_DONE_INT_ENA, 0);
            break;
        case ECDSA_INT_SHA_RELEASE:
            REG_SET_FIELD(ECDSA_INT_ENA_REG, ECDSA_SHA_RELEASE_INT_ENA, 0);
            break;
        default:
            HAL_ASSERT(false && "Unsupported interrupt type");
            break;
    }
}

/**
 * @brief Clear interrupt of a given type
 *
 * @param type Interrupt type
 */
static inline void ecdsa_ll_clear_intr(ecdsa_ll_intr_type_t type)
{
    switch (type) {
        case ECDSA_INT_PREP_DONE:
            REG_SET_FIELD(ECDSA_INT_CLR_REG, ECDSA_PREP_DONE_INT_CLR, 1);
            break;
        case ECDSA_INT_SHA_RELEASE:
            REG_SET_FIELD(ECDSA_INT_CLR_REG, ECDSA_SHA_RELEASE_INT_CLR, 1);
            break;
        default:
            HAL_ASSERT(false && "Unsupported interrupt type");
            break;
    }
}

/**
 * @brief Set working mode of ECDSA
 *
 * @param mode Mode of operation
 */
static inline void ecdsa_ll_set_mode(ecdsa_mode_t mode)
{
    switch (mode) {
        case ECDSA_MODE_SIGN_VERIFY:
            REG_SET_FIELD(ECDSA_CONF_REG, ECDSA_WORK_MODE, 0);
            break;
        case ECDSA_MODE_SIGN_GEN:
            REG_SET_FIELD(ECDSA_CONF_REG, ECDSA_WORK_MODE, 1);
            break;
        default:
            HAL_ASSERT(false && "Unsupported mode");
            break;
    }
}

/**
 * @brief Set curve for ECDSA operation
 *
 * @param curve ECDSA curve
 */
static inline void ecdsa_ll_set_curve(ecdsa_curve_t curve)
{
    switch (curve) {
        case ECDSA_CURVE_SECP256R1:
            REG_SET_BIT(ECDSA_CONF_REG, ECDSA_ECC_CURVE);
            break;
        case ECDSA_CURVE_SECP192R1:
            REG_CLR_BIT(ECDSA_CONF_REG, ECDSA_ECC_CURVE);
            break;
        default:
            HAL_ASSERT(false && "Unsupported curve");
            return;
    }
}

/**
 * @brief Set the source of `Z` (SHA message)
 *
 * @param mode Mode of SHA generation
 */
static inline void ecdsa_ll_set_z_mode(ecdsa_ll_sha_mode_t mode)
{
    switch (mode) {
        case ECDSA_Z_USE_SHA_PERI:
            REG_CLR_BIT(ECDSA_CONF_REG, ECDSA_SOFTWARE_SET_Z);
            break;
        case ECDSA_Z_USER_PROVIDED:
            REG_SET_BIT(ECDSA_CONF_REG, ECDSA_SOFTWARE_SET_Z);
            break;
        default:
            HAL_ASSERT(false && "Unsupported curve");
            break;
    }
}

/**
 * @brief Set the signature generation type of ECDSA operation
 *
 * @param type Type of the ECDSA signature
 */
static inline void ecdsa_ll_set_k_type(ecdsa_sign_type_t type)
{
    switch (type) {
        case ECDSA_K_TYPE_TRNG:
            REG_CLR_BIT(ECDSA_CONF_REG, ECDSA_DETERMINISTIC_K);
            break;
        case ECDSA_K_TYPE_DETERMINISITIC:
            REG_SET_BIT(ECDSA_CONF_REG, ECDSA_DETERMINISTIC_K);
            break;
        default:
            HAL_ASSERT(false && "Unsupported K type");
            break;
    }
}

/**
 * @brief Set the stage of ECDSA operation
 *
 * @param stage Stage of operation
 */
static inline void ecdsa_ll_set_stage(ecdsa_ll_stage_t stage)
{
    switch (stage) {
        case ECDSA_STAGE_START_CALC:
            REG_SET_BIT(ECDSA_START_REG, ECDSA_START);
            break;
        case ECDSA_STAGE_LOAD_DONE:
            REG_SET_BIT(ECDSA_START_REG, ECDSA_LOAD_DONE);
            break;
        case ECDSA_STAGE_GET_DONE:
            REG_SET_BIT(ECDSA_START_REG, ECDSA_GET_DONE);
            break;
        default:
            HAL_ASSERT(false && "Unsupported state");
            break;
    }
}

/**
 * @brief Get the state of ECDSA peripheral
 *
 * @return State of ECDSA
 */
static inline ecdsa_ll_state_t ecdsa_ll_get_state(void)
{
    return (ecdsa_ll_state_t)(REG_GET_FIELD(ECDSA_STATE_REG, ECDSA_BUSY));
}

/**
 * @brief Set the SHA type
 *
 * @param type Type of SHA
 */
static inline void ecdsa_ll_sha_set_type(ecdsa_ll_sha_type_t type)
{
    switch (type) {
        case ECDSA_SHA_224:
            REG_SET_FIELD(ECDSA_SHA_MODE_REG, ECDSA_SHA_MODE, 1);
            break;
        case ECDSA_SHA_256:
            REG_SET_FIELD(ECDSA_SHA_MODE_REG, ECDSA_SHA_MODE, 2);
            break;
        default:
            HAL_ASSERT(false && "Unsupported type");
            break;
    }
}

/**
 * @brief Set the SHA operation mode
 *
 * @param mode Mode of SHA operation
 */
static inline void ecdsa_ll_sha_set_mode(ecdsa_ll_sha_mode_t mode)
{
    switch (mode) {
        case ECDSA_MODE_SHA_START:
            REG_SET_BIT(ECDSA_SHA_START_REG, ECDSA_SHA_START);
            break;
        case ECDSA_MODE_SHA_CONTINUE:
            REG_SET_BIT(ECDSA_SHA_CONTINUE_REG, ECDSA_SHA_CONTINUE);
            break;
        default:
            HAL_ASSERT(false && "Unsupported type");
            break;
    }
}

/**
 * @brief Check if SHA is busy
 *
 * @return - true, if SHA is busy
 *         - false, if SHA is IDLE
 */
static inline bool ecdsa_ll_sha_is_busy(void)
{
    return REG_GET_BIT(ECDSA_SHA_BUSY_REG, ECDSA_SHA_BUSY);
}

/**
 * @brief Write the ECDSA parameter
 *
 * @param param Parameter to be written
 * @param buf   Buffer containing data
 * @param len   Length of buffer
 */
static inline void ecdsa_ll_write_param(ecdsa_ll_param_t param, const uint8_t *buf, uint16_t len)
{
    uint32_t reg;
    uint32_t word;
    switch (param) {
        case ECDSA_PARAM_R:
            reg = ECDSA_R_MEM;
            break;
        case ECDSA_PARAM_S:
            reg = ECDSA_S_MEM;
            break;
        case ECDSA_PARAM_Z:
            reg = ECDSA_Z_MEM;
            break;
        case ECDSA_PARAM_QAX:
            reg = ECDSA_QAX_MEM;
            break;
        case ECDSA_PARAM_QAY:
            reg = ECDSA_QAY_MEM;
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

/**
 * @brief Read the ECDSA parameter
 *
 * @param param Parameter to be read
 * @param buf   Buffer where the data will be written
 * @param len   Length of buffer
 */
static inline void ecdsa_ll_read_param(ecdsa_ll_param_t param, uint8_t *buf, uint16_t len)
{
    uint32_t reg;
    switch (param) {
        case ECDSA_PARAM_R:
            reg = ECDSA_R_MEM;
            break;
        case ECDSA_PARAM_S:
            reg = ECDSA_S_MEM;
            break;
        case ECDSA_PARAM_Z:
            reg = ECDSA_Z_MEM;
            break;
        case ECDSA_PARAM_QAX:
            reg = ECDSA_QAX_MEM;
            break;
        case ECDSA_PARAM_QAY:
            reg = ECDSA_QAY_MEM;
            break;
        default:
            HAL_ASSERT(false && "Invalid parameter");
            return;
    }

    memcpy(buf, (void *)reg, len);
}

/**
 * @brief Check if the ECDSA operation is successful
 *
 * @return - 1, if ECDSA operation succeeds
 *         - 0, otherwise
 */
static inline int ecdsa_ll_get_operation_result(void)
{
    return REG_GET_BIT(ECDSA_RESULT_REG, ECDSA_OPERATION_RESULT);
}

/**
 * @brief Check if the ECDSA curves configuration is supported
 * The ECDSA curves configuration is only avliable in chip version
 * above 1.2 in ESP32-H2
 */
static inline bool ecdsa_ll_is_configurable_curve_supported(void)
{
    return ESP_CHIP_REV_ABOVE(efuse_hal_chip_revision(), 102);
}

/**
 * @brief Check if the ECDSA deterministic mode is supported
 * The ECDSA deterministic mode is only available in chip version
 * above 1.2 in ESP32-H2
 */
static inline bool ecdsa_ll_is_deterministic_mode_supported(void)
{
    return ESP_CHIP_REV_ABOVE(efuse_hal_chip_revision(), 102);
}

#ifdef __cplusplus
}
#endif
