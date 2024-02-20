/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
// The HAL layer for Key Manager

#include "hal/key_mgr_hal.h"
#include "hal/key_mgr_ll.h"
#include "hal/key_mgr_types.h"

void key_mgr_hal_start(void)
{
    key_mgr_ll_start();
}

void key_mgr_hal_use_sw_init_key(void)
{
    return key_mgr_ll_use_sw_init_key();
}

void key_mgr_hal_set_key_usage(const esp_key_mgr_key_type_t key_type, const esp_key_mgr_key_usage_t key_usage)
{
    return key_mgr_ll_set_key_usage(key_type, key_usage);
}

esp_key_mgr_key_usage_t key_mgr_hal_get_key_usage(const esp_key_mgr_key_type_t key_type)
{
    return key_mgr_ll_get_key_usage(key_type);
}

void key_mgr_hal_set_key_purpose(const esp_key_mgr_key_purpose_t key_purpose)
{
    return key_mgr_ll_set_key_purpose(key_purpose);
}

void key_mgr_hal_set_key_generator_mode(const esp_key_mgr_key_generator_mode_t mode)
{
    return key_mgr_ll_set_key_generator_mode(mode);
}

bool key_mgr_hal_is_result_success(void)
{
    return key_mgr_ll_is_result_success();
}

bool key_mgr_hal_is_key_deployment_valid(const esp_key_mgr_key_type_t key_type)
{
    return key_mgr_ll_is_key_deployment_valid(key_type);
}

void key_mgr_hal_write_sw_init_key(const uint8_t *sw_init_key_buf, const size_t data_len)
{
    key_mgr_ll_write_sw_init_key(sw_init_key_buf, data_len);
}

void key_mgr_hal_write_assist_info(const uint8_t *assist_info_buf, const size_t data_len)
{
    key_mgr_ll_write_assist_info(assist_info_buf, data_len);
}

void key_mgr_hal_read_assist_info(uint8_t *assist_info_buf)
{
    key_mgr_ll_read_assist_info(assist_info_buf);
}

void key_mgr_hal_write_public_info(const uint8_t *public_info_buf, const size_t data_len)
{
    key_mgr_ll_write_public_info(public_info_buf, data_len);
}

void key_mgr_hal_read_public_info(uint8_t *public_info_buf, const size_t read_len)
{
    key_mgr_ll_read_public_info(public_info_buf, read_len);
}

bool key_mgr_hal_is_huk_valid(void)
{
    return key_mgr_ll_is_huk_valid();
}

void key_mgr_hal_set_xts_aes_key_len(const esp_key_mgr_xts_aes_key_len_t key_len)
{
    key_mgr_ll_set_xts_aes_key_len(key_len);
}

esp_key_mgr_xts_aes_key_len_t key_mgr_hal_get_xts_aes_key_len(void)
{
    return key_mgr_ll_get_xts_aes_key_len();
}

void key_mgr_hal_continue(void)
{
    key_mgr_ll_continue();
}

esp_key_mgr_state_t key_mgr_hal_get_state(void)
{
    return key_mgr_ll_get_state();
}

uint32_t key_mgr_hal_get_date_info(void)
{
    return key_mgr_ll_get_date_info();
}
