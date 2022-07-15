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

#ifdef __cplusplus
extern "C" {
#endif

#define ESP_EFUSE_BLOCK_ERROR_BITS(error_reg, block) ((error_reg) & (0x0F << (4 * (block))))

// Always inline these functions even no gcc optimization is applied.

/******************* eFuse fields *************************/

__attribute__((always_inline)) static inline uint32_t efuse_ll_get_flash_crypt_cnt(void)
{
    return REG_GET_FIELD(EFUSE_BLK0_RDATA0_REG, EFUSE_RD_FLASH_CRYPT_CNT);
}

__attribute__((always_inline)) static inline uint32_t efuse_ll_get_mac0(void)
{
    return REG_READ(EFUSE_BLK0_RDATA1_REG);
}

__attribute__((always_inline)) static inline uint32_t efuse_ll_get_mac1(void)
{
    return REG_GET_FIELD(EFUSE_BLK0_RDATA2_REG, EFUSE_RD_WIFI_MAC_CRC_HIGH) & 0x0000FFFF;
}

__attribute__((always_inline)) static inline bool efuse_ll_get_secure_boot_v1_en(void)
{
    return REG_GET_BIT(EFUSE_BLK0_RDATA6_REG, EFUSE_RD_ABS_DONE_0);
}

__attribute__((always_inline)) static inline bool efuse_ll_get_secure_boot_v2_en(void)
{
    return REG_GET_BIT(EFUSE_BLK0_RDATA6_REG, EFUSE_RD_ABS_DONE_1);
}

__attribute__((always_inline)) static inline bool efuse_ll_get_sdio_force(void)
{
    return REG_GET_BIT(EFUSE_BLK0_RDATA4_REG, EFUSE_RD_SDIO_FORCE);
}

__attribute__((always_inline)) static inline bool efuse_ll_get_xpd_sdio(void)
{
    return REG_GET_BIT(EFUSE_BLK0_RDATA4_REG, EFUSE_RD_XPD_SDIO_REG);
}

__attribute__((always_inline)) static inline bool efuse_ll_get_sdio_tieh(void)
{
    return REG_GET_BIT(EFUSE_BLK0_RDATA4_REG, EFUSE_RD_SDIO_TIEH);
}

__attribute__((always_inline)) static inline uint32_t efuse_ll_get_sdio_drefh(void)
{
    return REG_GET_FIELD(EFUSE_BLK0_RDATA0_REG, EFUSE_RD_SDIO_DREFH);
}

__attribute__((always_inline)) static inline uint32_t efuse_ll_get_sdio_drefm(void)
{
    return REG_GET_FIELD(EFUSE_BLK0_RDATA0_REG, EFUSE_RD_SDIO_DREFM);
}

__attribute__((always_inline)) static inline uint32_t efuse_ll_get_sdio_drefl(void)
{
    return REG_GET_FIELD(EFUSE_BLK0_RDATA0_REG, EFUSE_RD_SDIO_DREFL);
}

__attribute__((always_inline)) static inline bool efuse_ll_get_blk3_part_reserve(void)
{
    return REG_GET_BIT(EFUSE_BLK0_RDATA3_REG, EFUSE_RD_BLK3_PART_RESERVE);
}

__attribute__((always_inline)) static inline bool efuse_ll_get_chip_cpu_freq_rated(void)
{
    return REG_GET_BIT(EFUSE_BLK0_RDATA3_REG, EFUSE_RD_CHIP_CPU_FREQ_RATED);
}

__attribute__((always_inline)) static inline bool efuse_ll_get_chip_cpu_freq_low(void)
{
    return REG_GET_BIT(EFUSE_BLK0_RDATA3_REG, EFUSE_RD_CHIP_CPU_FREQ_LOW);
}

__attribute__((always_inline)) static inline uint32_t efuse_ll_get_chip_ver_pkg(void)
{
    uint32_t pkg_version = REG_GET_FIELD(EFUSE_BLK0_RDATA3_REG, EFUSE_RD_CHIP_VER_PKG);
    uint32_t pkg_version_4bit = REG_GET_FIELD(EFUSE_BLK0_RDATA3_REG, EFUSE_RD_CHIP_VER_PKG_4BIT);
    return (pkg_version_4bit << 3) | pkg_version;
}

// use efuse_hal_get_major_chip_version() to get full major chip version
__attribute__((always_inline)) static inline bool efuse_ll_get_chip_ver_rev1(void)
{
    return REG_GET_BIT(EFUSE_BLK0_RDATA3_REG, EFUSE_RD_CHIP_VER_REV1);
}

// use efuse_hal_get_major_chip_version() to get full major chip version
__attribute__((always_inline)) static inline bool efuse_ll_get_chip_ver_rev2(void)
{
    return REG_GET_BIT(EFUSE_BLK0_RDATA5_REG, EFUSE_RD_CHIP_VER_REV2);
}

// use efuse_hal_get_minor_chip_version() to get minor chip version
__attribute__((always_inline)) static inline uint32_t efuse_ll_get_chip_wafer_version_minor(void)
{
    return REG_GET_FIELD(EFUSE_BLK0_RDATA5_REG, EFUSE_RD_WAFER_VERSION_MINOR);
}

__attribute__((always_inline)) static inline bool efuse_ll_get_disable_wafer_version_major(void)
{
    return false;
}

__attribute__((always_inline)) static inline uint32_t efuse_ll_get_coding_scheme(void)
{
    return REG_GET_FIELD(EFUSE_BLK0_RDATA6_REG, EFUSE_CODING_SCHEME);
}

__attribute__((always_inline)) static inline bool efuse_ll_get_disable_app_cpu(void)
{
    return REG_GET_BIT(EFUSE_BLK0_RDATA3_REG, EFUSE_RD_CHIP_VER_DIS_APP_CPU);
}

__attribute__((always_inline)) static inline bool efuse_ll_get_disable_bt(void)
{
    return REG_GET_BIT(EFUSE_BLK0_RDATA3_REG, EFUSE_RD_CHIP_VER_DIS_BT);
}

__attribute__((always_inline)) static inline uint32_t efuse_ll_get_vol_level_hp_inv(void)
{
    return REG_GET_FIELD(EFUSE_BLK0_RDATA5_REG, EFUSE_RD_VOL_LEVEL_HP_INV);
}

__attribute__((always_inline)) static inline uint32_t efuse_ll_get_adc_vref(void)
{
    return REG_GET_FIELD(EFUSE_BLK0_RDATA4_REG, EFUSE_RD_ADC_VREF);
}

__attribute__((always_inline)) static inline uint32_t efuse_ll_get_adc1_tp_low(void)
{
    return REG_GET_FIELD(EFUSE_BLK3_RDATA3_REG, EFUSE_RD_ADC1_TP_LOW);
}

__attribute__((always_inline)) static inline uint32_t efuse_ll_get_adc2_tp_low(void)
{
    return REG_GET_FIELD(EFUSE_BLK3_RDATA3_REG, EFUSE_RD_ADC2_TP_LOW);
}

__attribute__((always_inline)) static inline uint32_t efuse_ll_get_adc1_tp_high(void)
{
    return REG_GET_FIELD(EFUSE_BLK3_RDATA3_REG, EFUSE_RD_ADC1_TP_HIGH);
}

__attribute__((always_inline)) static inline uint32_t efuse_ll_get_adc2_tp_high(void)
{
    return REG_GET_FIELD(EFUSE_BLK3_RDATA3_REG, EFUSE_RD_ADC2_TP_HIGH);
}

__attribute__((always_inline)) static inline bool efuse_ll_get_dec_warnings(unsigned block)
{
    if (block == 0 || block > 4) {
        return false;
    }
    uint32_t error_reg = REG_GET_FIELD(EFUSE_DEC_STATUS_REG, EFUSE_DEC_WARNINGS);
    return ESP_EFUSE_BLOCK_ERROR_BITS(error_reg, block - 1) != 0;
}

/******************* eFuse control functions *************************/

__attribute__((always_inline)) static inline bool efuse_ll_get_cmd(void)
{
    return REG_READ(EFUSE_CMD_REG);
}

__attribute__((always_inline)) static inline void efuse_ll_set_read_cmd(void)
{
    REG_WRITE(EFUSE_CMD_REG, EFUSE_READ_CMD);
}

__attribute__((always_inline)) static inline void efuse_ll_set_pgm_cmd(void)
{
    REG_WRITE(EFUSE_CMD_REG, EFUSE_PGM_CMD);
}

__attribute__((always_inline)) static inline void efuse_ll_set_conf_read_op_code(void)
{
    REG_WRITE(EFUSE_CONF_REG, EFUSE_READ_OP_CODE);
}

__attribute__((always_inline)) static inline void efuse_ll_set_conf_write_op_code(void)
{
    REG_WRITE(EFUSE_CONF_REG, EFUSE_WRITE_OP_CODE);
}

__attribute__((always_inline)) static inline void efuse_ll_set_dac_clk_div(uint32_t value)
{
    REG_SET_FIELD(EFUSE_DAC_CONF_REG, EFUSE_DAC_CLK_DIV, value);
}

__attribute__((always_inline)) static inline void efuse_ll_set_dac_clk_sel0(uint32_t value)
{
    REG_SET_FIELD(EFUSE_CLK_REG, EFUSE_CLK_SEL0, value);
}

__attribute__((always_inline)) static inline void efuse_ll_set_dac_clk_sel1(uint32_t value)
{
    REG_SET_FIELD(EFUSE_CLK_REG, EFUSE_CLK_SEL1, value);
}

/******************* eFuse control functions *************************/

#ifdef __cplusplus
}
#endif
