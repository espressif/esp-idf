/*
 * SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
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

#include "hal/assert.h"
#include "hal/key_mgr_types.h"
#include "soc/keymng_reg.h"
#include "soc/pcr_struct.h"
#include "soc/pcr_reg.h"
#include "hal/efuse_hal.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Read state of Key Manager
 *
 * @return esp_key_mgr_state_t
 */
static inline esp_key_mgr_state_t key_mgr_ll_get_state(void)
{
    return (esp_key_mgr_state_t) REG_GET_FIELD(KEYMNG_STATE_REG, KEYMNG_STATE);
}

static inline void key_mgr_ll_power_up(void)
{
    /* Power up the Key Manager peripheral (default state is power-down) */
    REG_CLR_BIT(PCR_KM_PD_CTRL_REG, PCR_KM_MEM_FORCE_PD);
    REG_SET_BIT(PCR_KM_PD_CTRL_REG, PCR_KM_MEM_FORCE_PU);
}

#define key_mgr_ll_enable_bus_clock(...) do { \
        _key_mgr_ll_enable_bus_clock(__VA_ARGS__); \
    } while(0)

static inline void key_mgr_ll_power_down(void)
{
    /* Power down the Key Manager peripheral */
    REG_CLR_BIT(PCR_KM_PD_CTRL_REG, PCR_KM_MEM_FORCE_PU);
    REG_SET_BIT(PCR_KM_PD_CTRL_REG, PCR_KM_MEM_FORCE_PD);
}

/**
 * @brief Enable the bus clock for Key Manager peripheral
 *
 * @param true to enable, false to disable
 */
static inline void _key_mgr_ll_enable_bus_clock(bool enable)
{
    // Enable key manager clock
    PCR.km_conf.km_clk_en = enable;
}

/**
 * @brief Enable the peripheral clock for Key Manager
 *
 * @param true to enable, false to disable
 */
static inline void _key_mgr_ll_enable_peripheral_clock(bool enable)
{
    ;    /* Nothing to do here, Kept for compatibility with other SoC */
}

#define key_mgr_ll_enable_peripheral_clock(...) do { \
        _key_mgr_ll_enable_peripheral_clock(__VA_ARGS__); \
    } while(0)

/**
 * @brief Reset the Key Manager peripheral
 *
 */
static inline void _key_mgr_ll_reset_register(void)
{
    PCR.km_conf.km_rst_en = 1;
    PCR.km_conf.km_rst_en = 0;
    // Wait for key manager to be ready
    while (!PCR.km_conf.km_ready) {
    };

    while (key_mgr_ll_get_state() != ESP_KEY_MGR_STATE_IDLE) {
    };

}

#define key_mgr_ll_reset_register(...) do { \
        _key_mgr_ll_reset_register(__VA_ARGS__); \
    } while(0)

/* @brief Start the key manager at IDLE state */
static inline void key_mgr_ll_start(void)
{
    REG_SET_BIT(KEYMNG_START_REG, KEYMNG_START);
}

/* @brief Continue key manager operation at LOAD/GAIN state */
static inline void key_mgr_ll_continue(void)
{
    REG_SET_BIT(KEYMNG_START_REG, KEYMNG_CONTINUE);
}

/* @brief Enable or Disable the KEY_MGR interrupts */
static inline void key_mgr_ll_configure_interrupt(const esp_key_mgr_interrupt_type_t intr, bool en)
{
    switch (intr) {
    case ESP_KEY_MGR_INT_PREP_DONE:
        REG_SET_FIELD(KEYMNG_INT_ENA_REG, KEYMNG_PREP_DONE_INT_ENA, en);
        break;
    case ESP_KEY_MGR_INT_PROC_DONE:
        REG_SET_FIELD(KEYMNG_INT_ENA_REG, KEYMNG_PROC_DONE_INT_ENA, en);
        break;
    case ESP_KEY_MGR_INT_POST_DONE:
        REG_SET_FIELD(KEYMNG_INT_ENA_REG, KEYMNG_POST_DONE_INT_ENA, en);
        break;
    default:
        return;
    }
}

/* @brief Clear the KEY_MGR interrupts */
static inline void key_mgr_ll_clear_int(const esp_key_mgr_interrupt_type_t intr)
{
    switch (intr) {
    case ESP_KEY_MGR_INT_PREP_DONE:
        REG_SET_FIELD(KEYMNG_INT_CLR_REG, KEYMNG_PREP_DONE_INT_CLR, 1);
        break;
    case ESP_KEY_MGR_INT_PROC_DONE:
        REG_SET_FIELD(KEYMNG_INT_CLR_REG, KEYMNG_PROC_DONE_INT_CLR, 1);
        break;
    case ESP_KEY_MGR_INT_POST_DONE:
        REG_SET_FIELD(KEYMNG_INT_CLR_REG, KEYMNG_POST_DONE_INT_CLR, 1);
        break;
    default:
        return;
    }
}

/**
 * @brief Set the key manager to use the software provided init key
 */
static inline void key_mgr_ll_use_sw_init_key(void)
{
    REG_SET_BIT(KEYMNG_STATIC_REG, KEYMNG_USE_SW_INIT_KEY);
}

/**
 * @brief Configure the key manager key usage policy for a particular key type
 *
 */
static inline void key_mgr_ll_set_key_usage(const esp_key_mgr_key_type_t key_type, const esp_key_mgr_key_usage_t key_usage)
{
    switch (key_type) {
    case ESP_KEY_MGR_ECDSA_KEY:
        if (key_usage == ESP_KEY_MGR_USE_EFUSE_KEY) {
            REG_SET_BIT(KEYMNG_STATIC_REG, KEYMNG_USE_EFUSE_KEY_ECDSA);
        } else {
            REG_CLR_BIT(KEYMNG_STATIC_REG, KEYMNG_USE_EFUSE_KEY_ECDSA);
        }
        break;

    case ESP_KEY_MGR_FLASH_XTS_AES_KEY:
        if (key_usage == ESP_KEY_MGR_USE_EFUSE_KEY) {
            REG_SET_BIT(KEYMNG_STATIC_REG, KEYMNG_USE_EFUSE_KEY_FLASH);
        } else {
            REG_CLR_BIT(KEYMNG_STATIC_REG, KEYMNG_USE_EFUSE_KEY_FLASH);
        }
        break;

    case ESP_KEY_MGR_HMAC_KEY:
        if (key_usage == ESP_KEY_MGR_USE_EFUSE_KEY) {
            REG_SET_BIT(KEYMNG_STATIC_REG, KEYMNG_USE_EFUSE_KEY_HMAC);
        } else {
            REG_CLR_BIT(KEYMNG_STATIC_REG, KEYMNG_USE_EFUSE_KEY_HMAC);
        }
        break;

    case ESP_KEY_MGR_DS_KEY:
        if (key_usage == ESP_KEY_MGR_USE_EFUSE_KEY) {
            REG_SET_BIT(KEYMNG_STATIC_REG, KEYMNG_USE_EFUSE_KEY_DS);
        } else {
            REG_CLR_BIT(KEYMNG_STATIC_REG, KEYMNG_USE_EFUSE_KEY_DS);
        }
        break;

    case ESP_KEY_MGR_PSRAM_XTS_AES_KEY:
        if (key_usage == ESP_KEY_MGR_USE_EFUSE_KEY) {
            REG_SET_BIT(KEYMNG_STATIC_REG, KEYMNG_USE_EFUSE_KEY_PSRAM);
        } else {
            REG_CLR_BIT(KEYMNG_STATIC_REG, KEYMNG_USE_EFUSE_KEY_PSRAM);
        }
        break;
    default:
        HAL_ASSERT(false && "Unsupported key type");
        return;
    }
}

static inline esp_key_mgr_key_usage_t key_mgr_ll_get_key_usage(esp_key_mgr_key_type_t key_type)
{
    switch (key_type) {
    case ESP_KEY_MGR_ECDSA_KEY:
        return (esp_key_mgr_key_usage_t)(REG_GET_BIT(KEYMNG_STATIC_REG, KEYMNG_USE_EFUSE_KEY_ECDSA));

    case ESP_KEY_MGR_FLASH_XTS_AES_KEY:
        return (esp_key_mgr_key_usage_t)(REG_GET_BIT(KEYMNG_STATIC_REG, KEYMNG_USE_EFUSE_KEY_FLASH));

    case ESP_KEY_MGR_HMAC_KEY:
        return (esp_key_mgr_key_usage_t)(REG_GET_BIT(KEYMNG_STATIC_REG, KEYMNG_USE_EFUSE_KEY_HMAC));

    case ESP_KEY_MGR_DS_KEY:
        return (esp_key_mgr_key_usage_t)(REG_GET_BIT(KEYMNG_STATIC_REG, KEYMNG_USE_EFUSE_KEY_DS));

    case ESP_KEY_MGR_PSRAM_XTS_AES_KEY:
        return (esp_key_mgr_key_usage_t)(REG_GET_BIT(KEYMNG_STATIC_REG, KEYMNG_USE_EFUSE_KEY_PSRAM));

    default:
        HAL_ASSERT(false && "Unsupported key type");
        return ESP_KEY_MGR_USAGE_INVALID;
    }
}

/**
 * @brief Set the lock for the use_sw_init_key_reg
 *        After this lock has been set,
 *        The Key manager configuration about the use of software init key cannot be changed
 */
static inline void key_mgr_ll_lock_use_sw_init_key_reg(void)
{
    REG_SET_BIT(KEYMNG_LOCK_REG, KEYMNG_USE_SW_INIT_KEY_LOCK);
}

/**
 * @brief Set the lock for the use_sw_init_key_reg
 *        After this lock has been set,
 *        The Key manager configuration about whether to use a particular key from efuse or key manager cannot be changed.
 */
static inline void key_mgr_ll_lock_use_efuse_key_reg(esp_key_mgr_key_type_t key_type)
{
    switch (key_type) {
    case ESP_KEY_MGR_ECDSA_KEY:
        REG_SET_BIT(KEYMNG_LOCK_REG, KEYMNG_USE_EFUSE_KEY_LOCK_ECDSA);
        break;

    case ESP_KEY_MGR_FLASH_XTS_AES_KEY:
        REG_SET_BIT(KEYMNG_LOCK_REG, KEYMNG_USE_EFUSE_KEY_LOCK_FLASH);
        break;

    case ESP_KEY_MGR_HMAC_KEY:
        REG_SET_BIT(KEYMNG_LOCK_REG, KEYMNG_USE_EFUSE_KEY_LOCK_HMAC);
        break;

    case ESP_KEY_MGR_DS_KEY:
        REG_SET_BIT(KEYMNG_LOCK_REG, KEYMNG_USE_EFUSE_KEY_LOCK_DS);
        break;

    case ESP_KEY_MGR_PSRAM_XTS_AES_KEY:
        REG_SET_BIT(KEYMNG_LOCK_REG, KEYMNG_USE_EFUSE_KEY_LOCK_PSRAM);
        break;

    default:
        HAL_ASSERT(false && "Unsupported key type");
        return;
    }
}

/* @brief Configure the key purpose to be used by the Key Manager for key generator operation */
static inline void key_mgr_ll_set_key_purpose(const esp_key_mgr_key_purpose_t key_purpose)
{
    REG_SET_FIELD(KEYMNG_CONF_REG, KEYMNG_KEY_PURPOSE, key_purpose);
}

/**
 * @brief Configure the mode which is used by the Key Manager for the generator key deployment process
 */
static inline void key_mgr_ll_set_key_generator_mode(const esp_key_mgr_key_generator_mode_t mode)
{
    REG_SET_FIELD(KEYMNG_CONF_REG, KEYMNG_KGEN_MODE, mode);
}

/**
 * @brief Read the key manager process result
 * @return  1 for Success
 *          0 for failure
 */
static inline bool key_mgr_ll_is_result_success(void)
{
    return REG_GET_FIELD(KEYMNG_RESULT_REG, KEYMNG_PROC_RESULT);
}

/**
 * @brief Check if the deployed key is valid or not
 * @return  1 for Success
 *          0 for failure
 */
static inline bool key_mgr_ll_is_key_deployment_valid(const esp_key_mgr_key_type_t key_type, const esp_key_mgr_key_len_t key_len)
{
    switch (key_type) {
    case ESP_KEY_MGR_ECDSA_KEY:
        switch (key_len) {
        case ESP_KEY_MGR_ECDSA_LEN_192:
            return REG_GET_FIELD(KEYMNG_KEY_VLD_REG, KEYMNG_KEY_ECDSA_192_VLD);
        case ESP_KEY_MGR_ECDSA_LEN_256:
            return REG_GET_FIELD(KEYMNG_KEY_VLD_REG, KEYMNG_KEY_ECDSA_256_VLD);
        case ESP_KEY_MGR_ECDSA_LEN_384:
            return REG_GET_FIELD(KEYMNG_KEY_VLD_REG, KEYMNG_KEY_ECDSA_384_VLD);
        default:
            HAL_ASSERT(false && "Unsupported key type");
            return 0;
        }
    case ESP_KEY_MGR_FLASH_XTS_AES_KEY:
        switch (key_len) {
        case ESP_KEY_MGR_XTS_AES_LEN_128:
        case ESP_KEY_MGR_XTS_AES_LEN_256:
            return REG_GET_FIELD(KEYMNG_KEY_VLD_REG, KEYMNG_KEY_FLASH_VLD);
        default:
            HAL_ASSERT(false && "Unsupported key type");
            return 0;
        }

    case ESP_KEY_MGR_HMAC_KEY:
        return REG_GET_FIELD(KEYMNG_KEY_VLD_REG, KEYMNG_KEY_HMAC_VLD);

    case ESP_KEY_MGR_DS_KEY:
        return REG_GET_FIELD(KEYMNG_KEY_VLD_REG, KEYMNG_KEY_DS_VLD);

    case ESP_KEY_MGR_PSRAM_XTS_AES_KEY:
        switch (key_len) {
        case ESP_KEY_MGR_XTS_AES_LEN_128:
        case ESP_KEY_MGR_XTS_AES_LEN_256:
            return REG_GET_FIELD(KEYMNG_KEY_VLD_REG, KEYMNG_KEY_PSRAM_VLD);
        default:
            HAL_ASSERT(false && "Unsupported key type");
            return 0;
        }

    default:
        HAL_ASSERT(false && "Unsupported mode");
        return 0;
    }
}

/*
 * @brief Write the SW init key in the key manager registers
 *
 * @input
 * sw_init_key_buf      Init key buffer, this should be a readable buffer of data_len size which should contain the sw init key. The buffer must be 32 bit aligned
 * data_len             Length of the init key buffer
 */
static inline void key_mgr_ll_write_sw_init_key(const uint8_t *sw_init_key_buf, const size_t data_len)
{
    memcpy((uint8_t *)KEYMNG_SW_INIT_KEY_MEM, sw_init_key_buf, data_len);
}

/*
 * @brief Write the Assist info in the key manager registers
 *
 * @input
 * assist_info_buf      Assist info buffer, this should be a readable buffer of data_len size which should contain the assist info.  The buffer must be 32 bit aligned
 * data_len             Length of the assist info buffer
 */
static inline void key_mgr_ll_write_assist_info(const uint8_t *assist_info_buf, const size_t data_len)
{
    memcpy((uint8_t *)KEYMNG_ASSIST_INFO_MEM, assist_info_buf, data_len);
}

/*
 * @brief Read the Assist info from the key manager registers
 *
 * @input
 * assist_info_buf      Assist info buffer, this should be a writable buffer of size KEY_MGR_ASSIST_INFO_LEN. The buffer must be 32 bit aligned
 */
static inline void key_mgr_ll_read_assist_info(uint8_t *assist_info_buf)
{
    memcpy(assist_info_buf, (uint8_t *)KEYMNG_ASSIST_INFO_MEM, KEY_MGR_ASSIST_INFO_LEN);
}

/*
 * @brief Write the Public info in the key manager registers
 * @input
 * public_info_buf      Public info buffer, this should be a readable buffer of data_len size which should contain the public info.  The buffer must be 32 bit aligned
 * data_len             Length of the public info buffer
 */
static inline void key_mgr_ll_write_public_info(const uint8_t *public_info_buf, const size_t data_len)
{
    memcpy((uint8_t *)KEYMNG_PUBLIC_INFO_MEM, public_info_buf, data_len);
}

/*
 * @brief Read the Public info in the key manager registers
 * @input
 * public_info_buf      Public info buffer, this should be a writable buffer of read_len,  The buffer must be 32 bit aligned
 * read_len             Length of the public info buffer
 */
static inline void key_mgr_ll_read_public_info(uint8_t *public_info_buf, const size_t read_len)
{
    memcpy(public_info_buf, (uint8_t *)KEYMNG_PUBLIC_INFO_MEM, read_len);
}

static inline bool key_mgr_ll_is_huk_valid(void)
{
    return REG_GET_FIELD(KEYMNG_HUK_VLD_REG, KEYMNG_HUK_VALID);
}

/* @brief Set the XTS-AES (Flash Encryption) key length for the Key Manager */
static inline void key_mgr_ll_set_xts_aes_key_len(const esp_key_mgr_key_type_t key_type, const esp_key_mgr_key_len_t key_len)
{
    uint32_t key_len_bit_mask;

    if (key_type == ESP_KEY_MGR_FLASH_XTS_AES_KEY) {
        key_len_bit_mask = KEYMNG_FLASH_KEY_LEN;
    } else if (key_type == ESP_KEY_MGR_PSRAM_XTS_AES_KEY) {
        key_len_bit_mask = KEYMNG_PSRAM_KEY_LEN;
    } else {
        HAL_ASSERT(false && "Unsupported key type");
        return;
    }

    switch (key_len) {
    case ESP_KEY_MGR_XTS_AES_LEN_128:
        REG_CLR_BIT(KEYMNG_STATIC_REG, key_len_bit_mask);
        break;
    case ESP_KEY_MGR_XTS_AES_LEN_256:
        REG_SET_BIT(KEYMNG_STATIC_REG, key_len_bit_mask);
        break;
    default:
        HAL_ASSERT(false && "Unsupported key length");
        return;
    }
}

/* @brief Get the XTS-AES (Flash Encryption) key length for the Key Manager */
static inline esp_key_mgr_key_len_t key_mgr_ll_get_xts_aes_key_len(const esp_key_mgr_key_type_t key_type)
{
    uint32_t key_len_bit = 0;

    if (key_type == ESP_KEY_MGR_FLASH_XTS_AES_KEY) {
        key_len_bit = REG_GET_BIT(KEYMNG_STATIC_REG, KEYMNG_FLASH_KEY_LEN);
    } else if (key_type == ESP_KEY_MGR_PSRAM_XTS_AES_KEY) {
        key_len_bit = REG_GET_BIT(KEYMNG_STATIC_REG, KEYMNG_PSRAM_KEY_LEN);
    } else {
        HAL_ASSERT(false && "Unsupported key type");
        return (esp_key_mgr_key_len_t) key_len_bit;
    }

    switch (key_len_bit) {
    case 0:
        return ESP_KEY_MGR_XTS_AES_LEN_128;
    case 1:
        return ESP_KEY_MGR_XTS_AES_LEN_256;
    default:
        HAL_ASSERT(false && "Unsupported key length");
        return (esp_key_mgr_key_len_t) key_len_bit;
    }
}

/**
 * @brief Read the Key Manager date information
 */
static inline uint32_t key_mgr_ll_get_date_info(void)
{
    // Only the least significant 28 bits have desired information
    return (uint32_t)(0x0FFFFFFF & REG_READ(KEYMNG_DATE_REG));
}

static inline bool key_mgr_ll_is_supported(void)
{
    return true;
}

static inline bool key_mgr_ll_flash_encryption_supported(void)
{
    if (!key_mgr_ll_is_supported() || efuse_hal_chip_revision() <= 100) {
        return false;
    }
    return true;
}

#ifdef __cplusplus
}
#endif
