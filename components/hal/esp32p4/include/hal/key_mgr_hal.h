/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
// The HAL layer for Key Manager

#pragma once

#if SOC_KEY_MANAGER_SUPPORTED
#include "hal/key_mgr_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/* @brief Start the key manager at IDLE state */
void key_mgr_hal_start(void);

/* @brief Continue the key manager operation */
void key_mgr_hal_continue(void);

/**
 * @brief Set the key manager to use the software provided init key
 */
void key_mgr_hal_use_sw_init_key(void);

/**
 * @brief Configure the key manager key usage policy for a particular key type
 */
void key_mgr_hal_set_key_usage(const esp_key_mgr_key_type_t key_type, const esp_key_mgr_key_usage_t key_usage);

/*
 * @brief Get the configured key usage for a given key type
 */
esp_key_mgr_key_usage_t key_mgr_hal_get_key_usage(const esp_key_mgr_key_type_t key_type);

/* @brief Configure the key purpose to be used by the Key Manager for key generator opearation */
void key_mgr_hal_set_key_purpose(const esp_key_mgr_key_purpose_t key_purpose);

/**
 * @bfief Configure the mode which is used by the Key Manager for the generator key deployement process
 */
void key_mgr_hal_set_key_generator_mode(const esp_key_mgr_key_generator_mode_t mode);

/**
 * @brief Read the key manager process result
 * @return  1 for Success
 *          0 for failure
 */
bool key_mgr_hal_is_result_success(void);

/**
 * @brief Check if the deployed key is valid or not
 * @return  1 for Success
 *          0 for failure
 */
bool key_mgr_hal_is_key_deployment_valid(const esp_key_mgr_key_type_t key_type);

/**
 * @brief Check if the HUK is valid or not
 * @return  1 for Success
 *          0 for failure
 */
bool key_mgr_hal_is_huk_valid(void);

/*
 * @brief Write the SW init key in the key manager registers
 *
 * @input
 * sw_init_key_buf      Init key buffer, this should be a readable buffer of data_len size which should contain the sw init key. The buffer must be 32 bit aligned
 * data_len             Length of the init key buffer
 */
void key_mgr_hal_write_sw_init_key(const uint8_t *sw_init_key_buf, const size_t data_len);

/*
 * @brief Write the Assist info in the key manager registers
 *
 * @input
 * assist_info_buf      Assist info buffer, this should be a readable buffer of data_len size which should contain the assist info.  The buffer must be 32 bit aligned
 * data_len             Length of the assist info buffer
 */
void key_mgr_hal_write_assist_info(const uint8_t *assist_info_buf, const size_t data_len);

/*
 * @brief Read the Assist info from the key manager registers
 *
 * @input
 * assist_info_buf      Assist info buffer, this should be a writable buffer of size KEY_MGR_ASSIST_INFO_LEN.  The buffer must be 32 bit aligned
 * data_len             Length of the assist info buffer
 */
void key_mgr_hal_read_assist_info(uint8_t *assist_info_buf);

/*
 * @brief Write the Public info in the key manager registers
 *
 * @input
 * public_info_buf      Public info buffer, this should be a readable buffer of data_len size which should contain the public info.  The buffer must be 32 bit aligned
 * data_len             Length of the public info buffer
 */
void key_mgr_hal_write_public_info(const uint8_t *public_info_buf, const size_t data_len);

/*
 * @brief Read the Public info in the key manager registers
 *
 * @input
 * public_info_buf      Public info buffer, this should be a writable buffer of read_len,  The buffer must be 32 bit aligned
 * read_len             Length of the public info buffer
 */
void key_mgr_hal_read_public_info(uint8_t *public_info_buf, const size_t read_len);

/* @brief Set the AES-XTS key length for the Key Manager */
void key_mgr_hal_set_aes_xts_key_len(const esp_key_mgr_xts_aes_key_len_t key_len);

/* @brief Get the AES-XTS key length for the Key Manager */
esp_key_mgr_xts_aes_key_len_t key_mgr_hal_get_aes_xts_key_len(void);

/**
 * @brief Read state of Key Manager
 *
 * @return esp_key_mgr_state_t
 */
esp_key_mgr_state_t key_mgr_hal_get_state(void);

/**
 * @brief Read the Key Manager date information
 */
uint32_t key_mgr_hal_get_date_info(void);

/**
 * @brief Set the Key Manager date information
 *        Only the least siginificant 28 bits shall be considered
 */
void key_mgr_hal_set_date_info(const uint32_t date_info);

#ifdef __cplusplus
}
#endif
#endif
