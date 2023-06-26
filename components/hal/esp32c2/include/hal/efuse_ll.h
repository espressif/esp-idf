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
#include "esp32c2/rom/efuse.h"

#ifdef __cplusplus
extern "C" {
#endif

// Always inline these functions even no gcc optimization is applied.

/******************* eFuse fields *************************/

__attribute__((always_inline)) static inline uint32_t efuse_ll_get_flash_crypt_cnt(void)
{
    return EFUSE.rd_repeat_data0.spi_boot_crypt_cnt;
}

__attribute__((always_inline)) static inline uint32_t efuse_ll_get_wdt_delay_sel(void)
{
    return EFUSE.rd_repeat_data0.wdt_delay_sel;
}

__attribute__((always_inline)) static inline uint32_t efuse_ll_get_mac0(void)
{
    return EFUSE.rd_blk2_data0.mac;
}

__attribute__((always_inline)) static inline uint32_t efuse_ll_get_mac1(void)
{
    return EFUSE.rd_blk2_data1.mac_1;
}

__attribute__((always_inline)) static inline bool efuse_ll_get_secure_boot_v2_en(void)
{
    return EFUSE.rd_repeat_data0.secure_boot_en;
}

// use efuse_hal_get_major_chip_version() to get major chip version
__attribute__((always_inline)) static inline uint32_t efuse_ll_get_chip_wafer_version_major(void)
{
    return EFUSE.rd_blk2_data1.wafer_version_major;
}

// use efuse_hal_get_minor_chip_version() to get minor chip version
__attribute__((always_inline)) static inline uint32_t efuse_ll_get_chip_wafer_version_minor(void)
{
    return EFUSE.rd_blk2_data1.wafer_version_minor;
}

__attribute__((always_inline)) static inline bool efuse_ll_get_disable_wafer_version_major(void)
{
    return EFUSE.rd_repeat_data0.disable_wafer_version_major;
}

__attribute__((always_inline)) static inline uint32_t efuse_ll_get_blk_version_major(void)
{
    return EFUSE.rd_blk2_data1.blk_version_major;
}

__attribute__((always_inline)) static inline uint32_t efuse_ll_get_blk_version_minor(void)
{
    return EFUSE.rd_blk2_data1.blk_version_minor;
}

__attribute__((always_inline)) static inline bool efuse_ll_get_disable_blk_version_major(void)
{
    return EFUSE.rd_repeat_data0.disable_blk_version_major;
}

__attribute__((always_inline)) static inline uint32_t efuse_ll_get_chip_ver_pkg(void)
{
    return EFUSE.rd_blk2_data1.pkg_version;
}

__attribute__((always_inline)) static inline uint32_t efuse_ll_get_ocode(void)
{
    // OCODE,                                  EFUSE_BLK2,     62,    7,      OCode
    return (EFUSE.rd_blk2_data2.ocode_1 << 2) + EFUSE.rd_blk2_data1.ocode;
}

__attribute__((always_inline)) static inline uint32_t efuse_ll_get_dig_dbias_hvt(void)
{
    // DIG_DBIAS_HVT,                          EFUSE_BLK2,    105,    5,      BLOCK2 digital dbias when hvt
    return EFUSE.rd_blk2_data3.dig_dbias_hvt;
}

__attribute__((always_inline)) static inline uint32_t efuse_ll_get_dig_ldo_slp_dbias2(void)
{
    // DIG_LDO_SLP_DBIAS2,                     EFUSE_BLK2,    110,    7,      BLOCK2 DIG_LDO_DBG0_DBIAS2
    return EFUSE.rd_blk2_data3.dig_ldo_slp_dbias2;
}

__attribute__((always_inline)) static inline uint32_t efuse_ll_get_dig_ldo_slp_dbias26(void)
{
    // DIG_LDO_SLP_DBIAS26,                    EFUSE_BLK2,    117,    8,      BLOCK2 DIG_LDO_DBG0_DBIAS26
    return EFUSE.rd_blk2_data3.dig_ldo_slp_dbias26;
}

__attribute__((always_inline)) static inline uint32_t efuse_ll_get_dig_ldo_act_dbias26(void)
{
    // DIG_LDO_ACT_DBIAS26,                    EFUSE_BLK2,    125,    6,      BLOCK2 DIG_LDO_ACT_DBIAS26
    return (EFUSE.rd_blk2_data4.dig_ldo_act_dbias26_1 << 3) + EFUSE.rd_blk2_data3.dig_ldo_act_dbias26;
}

__attribute__((always_inline)) static inline uint32_t efuse_ll_get_dig_ldo_act_stepd10(void)
{
    // DIG_LDO_ACT_STEPD10,                    EFUSE_BLK2,    131,    4,      BLOCK2 DIG_LDO_ACT_STEPD10
    return EFUSE.rd_blk2_data4.dig_ldo_act_stepd10;
}

__attribute__((always_inline)) static inline uint32_t efuse_ll_get_rtc_ldo_slp_dbias13(void)
{
    // RTC_LDO_SLP_DBIAS13,                    EFUSE_BLK2,    135,    7,      BLOCK2 DIG_LDO_SLP_DBIAS13
    return EFUSE.rd_blk2_data4.rtc_ldo_slp_dbias13;
}

__attribute__((always_inline)) static inline uint32_t efuse_ll_get_rtc_ldo_slp_dbias29(void)
{
    // RTC_LDO_SLP_DBIAS29,                    EFUSE_BLK2,    142,    9,      BLOCK2 DIG_LDO_SLP_DBIAS29
    return EFUSE.rd_blk2_data4.rtc_ldo_slp_dbias29;
}

__attribute__((always_inline)) static inline uint32_t efuse_ll_get_rtc_ldo_slp_dbias31(void)
{
    // RTC_LDO_SLP_DBIAS31,                    EFUSE_BLK2,    151,    6,      BLOCK2 DIG_LDO_SLP_DBIAS31
    return EFUSE.rd_blk2_data4.rtc_ldo_slp_dbias31;
}

__attribute__((always_inline)) static inline uint32_t efuse_ll_get_rtc_ldo_act_dbias31(void)
{
    // RTC_LDO_ACT_DBIAS31,                    EFUSE_BLK2,    157,    6,      BLOCK2 DIG_LDO_ACT_DBIAS31
    return (EFUSE.rd_blk2_data5.rtc_ldo_act_dbias31_1 << 3) + EFUSE.rd_blk2_data4.rtc_ldo_act_dbias31;
}

__attribute__((always_inline)) static inline uint32_t efuse_ll_get_rtc_ldo_act_dbias13(void)
{
    // RTC_LDO_ACT_DBIAS13,                    EFUSE_BLK2,    163,    8,      BLOCK2 DIG_LDO_ACT_DBIAS13
    return EFUSE.rd_blk2_data5.rtc_ldo_act_dbias13;
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

__attribute__((always_inline)) static inline void efuse_ll_set_tpgm_inactive(uint8_t value)
{
    EFUSE.wr_tim_conf0.tpgm_inactive = value;
}
/******************* eFuse control functions *************************/

#ifdef __cplusplus
}
#endif
