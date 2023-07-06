/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "soc/efuse_periph.h"
#include "hal/assert.h"
#include "esp32s3/rom/efuse.h"

#ifdef __cplusplus
extern "C" {
#endif

// Always inline these functions even no gcc optimization is applied.

/******************* eFuse fields *************************/

__attribute__((always_inline)) static inline uint32_t efuse_ll_get_flash_crypt_cnt(void)
{
    return EFUSE.rd_repeat_data1.spi_boot_crypt_cnt;
}

__attribute__((always_inline)) static inline uint32_t efuse_ll_get_wdt_delay_sel(void)
{
    return EFUSE.rd_repeat_data1.wdt_delay_sel;
}

__attribute__((always_inline)) static inline uint32_t efuse_ll_get_flash_type(void)
{
    return EFUSE.rd_repeat_data3.flash_type;
}

__attribute__((always_inline)) static inline uint32_t efuse_ll_get_mac0(void)
{
    return EFUSE.rd_mac_spi_sys_0.mac_0;
}

__attribute__((always_inline)) static inline uint32_t efuse_ll_get_mac1(void)
{
    return EFUSE.rd_mac_spi_sys_1.mac_1;
}

__attribute__((always_inline)) static inline bool efuse_ll_get_secure_boot_v2_en(void)
{
    return EFUSE.rd_repeat_data2.secure_boot_en;
}

// use efuse_hal_get_major_chip_version() to get major chip version
__attribute__((always_inline)) static inline uint32_t efuse_ll_get_chip_wafer_version_major(void)
{
    return EFUSE.rd_mac_spi_sys_5.wafer_version_major;
}

// use efuse_hal_get_minor_chip_version() to get minor chip version
__attribute__((always_inline)) static inline uint32_t efuse_ll_get_chip_wafer_version_minor(void)
{
    return (EFUSE.rd_mac_spi_sys_5.wafer_version_minor_hi << 3) + EFUSE.rd_mac_spi_sys_3.wafer_version_minor_lo;
}

__attribute__((always_inline)) static inline bool efuse_ll_get_disable_wafer_version_major(void)
{
    return EFUSE.rd_repeat_data4.disable_wafer_version_major;
}

__attribute__((always_inline)) static inline uint32_t efuse_ll_get_blk_version_major(void)
{
    return EFUSE.rd_sys_part1_data4.blk_version_major;
}

__attribute__((always_inline)) static inline uint32_t efuse_ll_get_blk_version_minor(void)
{
    return EFUSE.rd_mac_spi_sys_3.blk_version_minor;
}

__attribute__((always_inline)) static inline bool efuse_ll_get_disable_blk_version_major(void)
{
    return EFUSE.rd_repeat_data4.disable_blk_version_major;
}

__attribute__((always_inline)) static inline uint32_t efuse_ll_get_chip_ver_pkg(void)
{
    return EFUSE.rd_mac_spi_sys_3.pkg_version;
}

__attribute__((always_inline)) static inline uint32_t efuse_ll_get_ocode(void)
{
    // EFUSE_BLK2,  141,    8,     ADC OCode
    return EFUSE.rd_sys_part1_data4.ocode;
}

__attribute__((always_inline)) static inline uint32_t efuse_ll_get_k_rtc_ldo(void)
{
    // EFUSE_BLK1,  141,    7,     BLOCK1 K_RTC_LDO
    return EFUSE.rd_mac_spi_sys_4.k_rtc_ldo;
}

__attribute__((always_inline)) static inline uint32_t efuse_ll_get_k_dig_ldo(void)
{
    // EFUSE_BLK1,  148,    7,     BLOCK1 K_DIG_LDO
    return EFUSE.rd_mac_spi_sys_4.k_dig_ldo;
}

__attribute__((always_inline)) static inline uint32_t efuse_ll_get_v_rtc_dbias20(void)
{
    // EFUSE_BLK1,  155,    8,     BLOCK1 voltage of rtc dbias20
    return (EFUSE.rd_mac_spi_sys_5.v_rtc_dbias20_1 << 5) + EFUSE.rd_mac_spi_sys_4.v_rtc_dbias20;
}

__attribute__((always_inline)) static inline uint32_t efuse_ll_get_v_dig_dbias20(void)
{
    // EFUSE_BLK1,  163,    8,     BLOCK1 voltage of digital dbias20
    return EFUSE.rd_mac_spi_sys_5.v_dig_dbias20;
}

__attribute__((always_inline)) static inline uint32_t efuse_ll_get_dig_dbias_hvt(void)
{
    // EFUSE_BLK1,  171,    5,     BLOCK1 digital dbias when hvt
    return EFUSE.rd_mac_spi_sys_5.dig_dbias_hvt;
}

/******************* eFuse control functions *************************/

__attribute__((always_inline)) static inline bool efuse_ll_get_read_cmd(void)
{
    return EFUSE.cmd.read_cmd;
}

__attribute__((always_inline)) static inline bool efuse_ll_get_pgm_cmd(void)
{
    return EFUSE.cmd.pgm_cmd;
}

__attribute__((always_inline)) static inline void efuse_ll_set_read_cmd(void)
{
    EFUSE.cmd.read_cmd = 1;
}

__attribute__((always_inline)) static inline void efuse_ll_set_pgm_cmd(uint32_t block)
{
    HAL_ASSERT(block < ETS_EFUSE_BLOCK_MAX);
    EFUSE.cmd.val = ((block << EFUSE_BLK_NUM_S) & EFUSE_BLK_NUM_M) | EFUSE_PGM_CMD;
}

__attribute__((always_inline)) static inline void efuse_ll_set_conf_read_op_code(void)
{
    EFUSE.conf.op_code = EFUSE_READ_OP_CODE;
}

__attribute__((always_inline)) static inline void efuse_ll_set_conf_write_op_code(void)
{
    EFUSE.conf.op_code = EFUSE_WRITE_OP_CODE;
}

__attribute__((always_inline)) static inline void efuse_ll_set_dac_num(uint8_t val)
{
    EFUSE.dac_conf.dac_num = val;
}

__attribute__((always_inline)) static inline void efuse_ll_set_dac_clk_div(uint8_t val)
{
    EFUSE.dac_conf.dac_clk_div = val;
}

__attribute__((always_inline)) static inline void efuse_ll_set_pwr_on_num(uint16_t val)
{
    EFUSE.wr_tim_conf1.pwr_on_num = val;
}

__attribute__((always_inline)) static inline void efuse_ll_set_pwr_off_num(uint16_t value)
{
    EFUSE.wr_tim_conf2.pwr_off_num = value;
}

/******************* eFuse control functions *************************/

#ifdef __cplusplus
}
#endif
