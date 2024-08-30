/*
 * SPDX-FileCopyrightText: 2021-2024 Espressif Systems (Shanghai) CO LTD
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
    return EFUSE.blk0_rdata0.rd_flash_crypt_cnt;
}

__attribute__((always_inline)) static inline uint32_t efuse_ll_get_mac0(void)
{
    return EFUSE.blk0_rdata1.rd_mac;
}

__attribute__((always_inline)) static inline uint32_t efuse_ll_get_mac1(void)
{
    return EFUSE.blk0_rdata2.rd_mac_1;
}

__attribute__((always_inline)) static inline bool efuse_ll_get_secure_boot_v1_en(void)
{
    return EFUSE.blk0_rdata6.rd_abs_done_0;
}

__attribute__((always_inline)) static inline bool efuse_ll_get_secure_boot_v2_en(void)
{
    return EFUSE.blk0_rdata6.rd_abs_done_1;
}

__attribute__((always_inline)) static inline bool efuse_ll_get_sdio_force(void)
{
    return EFUSE.blk0_rdata4.rd_xpd_sdio_force;
}

__attribute__((always_inline)) static inline bool efuse_ll_get_xpd_sdio(void)
{
    return EFUSE.blk0_rdata4.rd_xpd_sdio_reg;
}

__attribute__((always_inline)) static inline bool efuse_ll_get_sdio_tieh(void)
{
    return EFUSE.blk0_rdata4.rd_xpd_sdio_tieh;
}

__attribute__((always_inline)) static inline uint32_t efuse_ll_get_sdio_drefh(void)
{
    return (EFUSE.blk0_rdata4.val >> 8) & 0x3;
}

__attribute__((always_inline)) static inline uint32_t efuse_ll_get_sdio_drefm(void)
{
    return (EFUSE.blk0_rdata4.val >> 10) & 0x3;
}

__attribute__((always_inline)) static inline uint32_t efuse_ll_get_sdio_drefl(void)
{
    return (EFUSE.blk0_rdata4.val >> 12) & 0x3;
}

__attribute__((always_inline)) static inline bool efuse_ll_get_blk3_part_reserve(void)
{
    return EFUSE.blk0_rdata3.rd_blk3_part_reserve;
}

__attribute__((always_inline)) static inline bool efuse_ll_get_chip_cpu_freq_rated(void)
{
    return EFUSE.blk0_rdata3.rd_chip_cpu_freq_rated;
}

__attribute__((always_inline)) static inline bool efuse_ll_get_chip_cpu_freq_low(void)
{
    return EFUSE.blk0_rdata3.rd_chip_cpu_freq_low;
}

__attribute__((always_inline)) static inline uint32_t efuse_ll_get_chip_ver_pkg(void)
{
    return (EFUSE.blk0_rdata3.rd_chip_package_4bit << 3) | EFUSE.blk0_rdata3.rd_chip_package;
}

// use efuse_hal_get_major_chip_version() to get full major chip version
__attribute__((always_inline)) static inline bool efuse_ll_get_chip_ver_rev1(void)
{
    return EFUSE.blk0_rdata3.rd_chip_ver_rev1;
}

// use efuse_hal_get_major_chip_version() to get full major chip version
__attribute__((always_inline)) static inline bool efuse_ll_get_chip_ver_rev2(void)
{
    return EFUSE.blk0_rdata5.rd_chip_ver_rev2;
}

// use efuse_hal_get_minor_chip_version() to get minor chip version
__attribute__((always_inline)) static inline uint32_t efuse_ll_get_chip_wafer_version_minor(void)
{
    return EFUSE.blk0_rdata5.rd_wafer_version_minor;
}

__attribute__((always_inline)) static inline bool efuse_ll_get_disable_wafer_version_major(void)
{
    return false;
}

__attribute__((always_inline)) static inline bool efuse_ll_get_disable_blk_version_major(void)
{
    return false;
}

__attribute__((always_inline)) static inline bool efuse_ll_get_blk_version_major(void)
{
    return 0;
}

__attribute__((always_inline)) static inline uint32_t efuse_ll_get_blk_version_minor(void)
{
    return 0;
}

__attribute__((always_inline)) static inline uint32_t efuse_ll_get_coding_scheme(void)
{
    return EFUSE.blk0_rdata6.rd_coding_scheme;
}

__attribute__((always_inline)) static inline bool efuse_ll_get_disable_app_cpu(void)
{
    return EFUSE.blk0_rdata3.rd_disable_app_cpu;
}

__attribute__((always_inline)) static inline bool efuse_ll_get_disable_bt(void)
{
    return EFUSE.blk0_rdata3.rd_disable_bt;
}

__attribute__((always_inline)) static inline uint32_t efuse_ll_get_vol_level_hp_inv(void)
{
    return EFUSE.blk0_rdata5.rd_vol_level_hp_inv;
}

__attribute__((always_inline)) static inline uint32_t efuse_ll_get_adc_vref(void)
{
    return EFUSE.blk0_rdata4.rd_adc_vref;
}

__attribute__((always_inline)) static inline uint32_t efuse_ll_get_adc1_tp_low(void)
{
    return EFUSE.blk3_rdata3.rd_adc1_tp_low;
}

__attribute__((always_inline)) static inline uint32_t efuse_ll_get_adc2_tp_low(void)
{
    return EFUSE.blk3_rdata3.rd_adc2_tp_low;
}

__attribute__((always_inline)) static inline uint32_t efuse_ll_get_adc1_tp_high(void)
{
    return EFUSE.blk3_rdata3.rd_adc1_tp_high;
}

__attribute__((always_inline)) static inline uint32_t efuse_ll_get_adc2_tp_high(void)
{
    return EFUSE.blk3_rdata3.rd_adc2_tp_high;
}

__attribute__((always_inline)) static inline bool efuse_ll_get_dec_warnings(unsigned block)
{
    if (block == 0 || block > 4) {
        return false;
    }
    uint32_t error_reg = EFUSE.dec_status.dec_warnings;
    return ESP_EFUSE_BLOCK_ERROR_BITS(error_reg, block - 1) != 0;
}

/******************* eFuse control functions *************************/

__attribute__((always_inline)) static inline bool efuse_ll_get_cmd(void)
{
    return EFUSE.cmd.val;
}

__attribute__((always_inline)) static inline void efuse_ll_set_read_cmd(void)
{
    EFUSE.cmd.read_cmd = 1;
}

__attribute__((always_inline)) static inline void efuse_ll_set_pgm_cmd(void)
{
    EFUSE.cmd.pgm_cmd = 1;
}

__attribute__((always_inline)) static inline void efuse_ll_set_conf_read_op_code(void)
{
    EFUSE.conf.op_code = EFUSE_READ_OP_CODE;
}

__attribute__((always_inline)) static inline void efuse_ll_set_conf_write_op_code(void)
{
    EFUSE.conf.op_code = EFUSE_WRITE_OP_CODE;
}

__attribute__((always_inline)) static inline void efuse_ll_set_dac_clk_div(uint32_t value)
{
    EFUSE.dac_conf.dac_clk_div = value;
}

__attribute__((always_inline)) static inline void efuse_ll_set_dac_clk_sel0(uint32_t value)
{
    EFUSE.clk.clk_sel0 = value;
}

__attribute__((always_inline)) static inline void efuse_ll_set_dac_clk_sel1(uint32_t value)
{
    EFUSE.clk.clk_sel1 = value;
}

/******************* eFuse control functions *************************/

#ifdef __cplusplus
}
#endif
