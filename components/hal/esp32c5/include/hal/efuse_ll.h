/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "soc/efuse_periph.h"
#include "hal/assert.h"

#ifdef __cplusplus
extern "C" {
#endif

// Always inline these functions even no gcc optimization is applied.

/******************* eFuse fields *************************/

__attribute__((always_inline)) static inline uint32_t efuse_ll_get_flash_crypt_cnt(void)
{
    // TODO: [ESP32C5] IDF-8674 (inherit from C6)
    // return EFUSE.rd_repeat_data1.spi_boot_crypt_cnt;
    return (uint32_t)0;
}

__attribute__((always_inline)) static inline uint32_t efuse_ll_get_wdt_delay_sel(void)
{
    // TODO: [ESP32C5] IDF-8674 (inherit from C6)
    // return EFUSE.rd_repeat_data1.wdt_delay_sel;
    return (uint32_t)0;
}

__attribute__((always_inline)) static inline uint32_t efuse_ll_get_mac0(void)
{
    // TODO: [ESP32C5] IDF-8674 (inherit from C6)
    // return EFUSE.rd_mac_sys_0.mac_0;
    return (uint32_t)0;
}

__attribute__((always_inline)) static inline uint32_t efuse_ll_get_mac1(void)
{
    // TODO: [ESP32C5] IDF-8674 (inherit from C6)
    // return EFUSE.rd_mac_sys_1.mac_1;
    return (uint32_t)0;
}

__attribute__((always_inline)) static inline bool efuse_ll_get_secure_boot_v2_en(void)
{
    // TODO: [ESP32C5] IDF-8674 (inherit from C6)
    // return EFUSE.rd_repeat_data2.secure_boot_en;
    return (bool)0;
}

// use efuse_hal_get_major_chip_version() to get major chip version
__attribute__((always_inline)) static inline uint32_t efuse_ll_get_chip_wafer_version_major(void)
{
    // TODO: [ESP32C5] IDF-8674 (inherit from C6)
    // // return EFUSE.rd_mac_sys_5;
    // return 0;
    return (uint32_t)0;
}

// use efuse_hal_get_minor_chip_version() to get minor chip version
__attribute__((always_inline)) static inline uint32_t efuse_ll_get_chip_wafer_version_minor(void)
{
    // TODO: [ESP32C5] IDF-8674 (inherit from C6)
    // return 0;
    return (uint32_t)0;
}

__attribute__((always_inline)) static inline bool efuse_ll_get_disable_wafer_version_major(void)
{
    // TODO: [ESP32C5] IDF-8674 (inherit from C6)
    // return 0;
    return (bool)0;
}

__attribute__((always_inline)) static inline uint32_t efuse_ll_get_blk_version_major(void)
{
    // TODO: [ESP32C5] IDF-8674 (inherit from C6)
    // return 0;
    return (uint32_t)0;
}

__attribute__((always_inline)) static inline uint32_t efuse_ll_get_blk_version_minor(void)
{
    // TODO: [ESP32C5] IDF-8674 (inherit from C6)
    // return 0;
    return (uint32_t)0;
}

__attribute__((always_inline)) static inline bool efuse_ll_get_disable_blk_version_major(void)
{
    // TODO: [ESP32C5] IDF-8674 (inherit from C6)
    // return 0;
    return (bool)0;
}

__attribute__((always_inline)) static inline uint32_t efuse_ll_get_chip_ver_pkg(void)
{
    // TODO: [ESP32C5] IDF-8674 (inherit from C6)
    // return 0;
    return (uint32_t)0;
}

__attribute__((always_inline)) static inline void efuse_ll_set_ecdsa_key_blk(int efuse_blk)
{
    // TODO: [ESP32C5] IDF-8674 (inherit from C6)
    // EFUSE.conf.cfg_ecdsa_blk = efuse_blk;
}

/******************* eFuse control functions *************************/

__attribute__((always_inline)) static inline bool efuse_ll_get_read_cmd(void)
{
    // TODO: [ESP32C5] IDF-8674 (inherit from C6)
    // return EFUSE.cmd.read_cmd;
    return (bool)0;
}

__attribute__((always_inline)) static inline bool efuse_ll_get_pgm_cmd(void)
{
    // TODO: [ESP32C5] IDF-8674 (inherit from C6)
    // return EFUSE.cmd.pgm_cmd;
    return (bool)0;
}

__attribute__((always_inline)) static inline void efuse_ll_set_read_cmd(void)
{
    // TODO: [ESP32C5] IDF-8674 (inherit from C6)
    // EFUSE.cmd.read_cmd = 1;
}

__attribute__((always_inline)) static inline void efuse_ll_set_pgm_cmd(uint32_t block)
{
    // TODO: [ESP32C5] IDF-8674 (inherit from C6)
    // HAL_ASSERT(block < ETS_EFUSE_BLOCK_MAX);
    // EFUSE.cmd.val = ((block << EFUSE_BLK_NUM_S) & EFUSE_BLK_NUM_M) | EFUSE_PGM_CMD;
}

__attribute__((always_inline)) static inline void efuse_ll_set_conf_read_op_code(void)
{
    // TODO: [ESP32C5] IDF-8674 (inherit from C6)
    // EFUSE.conf.op_code = EFUSE_READ_OP_CODE;
}

__attribute__((always_inline)) static inline void efuse_ll_set_conf_write_op_code(void)
{
    // TODO: [ESP32C5] IDF-8674 (inherit from C6)
    // EFUSE.conf.op_code = EFUSE_WRITE_OP_CODE;
}

__attribute__((always_inline)) static inline void efuse_ll_set_pwr_off_num(uint16_t value)
{
    // TODO: [ESP32C5] IDF-8674 (inherit from C6)
    // EFUSE.wr_tim_conf2.pwr_off_num = value;
}

__attribute__((always_inline)) static inline void efuse_ll_rs_bypass_update(void)
{
    // TODO: [ESP32C5] IDF-8674 (inherit from C6)
    // EFUSE.wr_tim_conf0_rs_bypass.update = 1;
}

/******************* eFuse control functions *************************/

#ifdef __cplusplus
}
#endif
