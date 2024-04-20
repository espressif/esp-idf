/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "rom/efuse.h"
#include "soc/efuse_periph.h"
#include "hal/assert.h"

#ifdef __cplusplus
extern "C" {
#endif

// Always inline these functions even no gcc optimization is applied.

/******************* eFuse fields *************************/

__attribute__((always_inline)) static inline uint32_t efuse_ll_get_flash_crypt_cnt(void)
{
    return 0;
}

__attribute__((always_inline)) static inline uint32_t efuse_ll_get_wdt_delay_sel(void)
{
    return 0;
}

__attribute__((always_inline)) static inline uint32_t efuse_ll_get_mac0(void)
{
    return 0;
}

__attribute__((always_inline)) static inline uint32_t efuse_ll_get_mac1(void)
{
    return 0;
}

__attribute__((always_inline)) static inline bool efuse_ll_get_secure_boot_v2_en(void)
{
    return 0;
}

__attribute__((always_inline)) static inline bool efuse_ll_get_err_rst_enable(void)
{
    return 0;
}

// use efuse_hal_get_major_chip_version() to get major chip version
__attribute__((always_inline)) static inline uint32_t efuse_ll_get_chip_wafer_version_major(void)
{
    return 0;
}

// use efuse_hal_get_minor_chip_version() to get minor chip version
__attribute__((always_inline)) static inline uint32_t efuse_ll_get_chip_wafer_version_minor(void)
{
    return 0;
}

__attribute__((always_inline)) static inline bool efuse_ll_get_disable_wafer_version_major(void)
{
    return 0;
}

__attribute__((always_inline)) static inline uint32_t efuse_ll_get_blk_version_major(void)
{
    return 0;
}

__attribute__((always_inline)) static inline uint32_t efuse_ll_get_blk_version_minor(void)
{
    return 0;
}

__attribute__((always_inline)) static inline bool efuse_ll_get_disable_blk_version_major(void)
{
    return 0;
}

__attribute__((always_inline)) static inline uint32_t efuse_ll_get_chip_ver_pkg(void)
{
    return 0;
}

__attribute__((always_inline)) static inline uint32_t efuse_ll_get_ocode(void)
{
    return 0;
}

__attribute__((always_inline)) static inline uint32_t efuse_ll_get_k_rtc_ldo(void)
{
    return 0;
}

__attribute__((always_inline)) static inline uint32_t efuse_ll_get_k_dig_ldo(void)
{
    return 0;
}

__attribute__((always_inline)) static inline uint32_t efuse_ll_get_v_rtc_dbias20(void)
{
    return 0;
}

__attribute__((always_inline)) static inline uint32_t efuse_ll_get_v_dig_dbias20(void)
{
    return 0;
}

__attribute__((always_inline)) static inline uint32_t efuse_ll_get_dig_dbias_hvt(void)
{
    return 0;
}

/******************* eFuse control functions *************************/

__attribute__((always_inline)) static inline bool efuse_ll_get_read_cmd(void)
{
    return 0;
}

__attribute__((always_inline)) static inline bool efuse_ll_get_pgm_cmd(void)
{
    return 0;
}

__attribute__((always_inline)) static inline void efuse_ll_set_read_cmd(void)
{
}

__attribute__((always_inline)) static inline void efuse_ll_set_pgm_cmd(uint32_t block)
{
    HAL_ASSERT(block < ETS_EFUSE_BLOCK_MAX);
    (void) block;
}

__attribute__((always_inline)) static inline void efuse_ll_set_conf_read_op_code(void)
{
}

__attribute__((always_inline)) static inline void efuse_ll_set_conf_write_op_code(void)
{
}

__attribute__((always_inline)) static inline void efuse_ll_set_dac_num(uint8_t val)
{
    (void) val;
}

__attribute__((always_inline)) static inline void efuse_ll_set_dac_clk_div(uint8_t val)
{
    (void) val;
}

__attribute__((always_inline)) static inline void efuse_ll_set_pwr_on_num(uint16_t val)
{
    (void) val;
}

__attribute__((always_inline)) static inline void efuse_ll_set_pwr_off_num(uint16_t value)
{
    (void) value;
}

/******************* eFuse control functions *************************/

#ifdef __cplusplus
}
#endif
