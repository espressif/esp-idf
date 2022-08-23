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
#include "esp32s2/rom/efuse.h"

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

__attribute__((always_inline)) static inline uint32_t efuse_ll_get_mac0(void)
{
    return EFUSE.rd_mac_spi_8m_0;
}

__attribute__((always_inline)) static inline uint32_t efuse_ll_get_mac1(void)
{
    return EFUSE.rd_mac_spi_8m_1.mac_1;
}

__attribute__((always_inline)) static inline bool efuse_ll_get_secure_boot_v2_en(void)
{
    return EFUSE.rd_repeat_data2.secure_boot_en;
}

// use efuse_hal_get_major_chip_version() to get major chip version
__attribute__((always_inline)) static inline uint32_t efuse_ll_get_chip_wafer_version_major(void)
{
    return EFUSE.rd_mac_spi_8m_3.wafer_version_major;
}

// use efuse_hal_get_minor_chip_version() to get minor chip version
__attribute__((always_inline)) static inline uint32_t efuse_ll_get_chip_wafer_version_minor(void)
{
    return (EFUSE.rd_mac_spi_8m_3.wafer_version_minor_high << 3) + EFUSE.rd_mac_spi_8m_4.wafer_version_minor_low;
}

__attribute__((always_inline)) static inline bool efuse_ll_get_disable_wafer_version_major(void)
{
    return EFUSE.rd_repeat_data4.disable_wafer_version_major;
}

__attribute__((always_inline)) static inline uint32_t efuse_ll_get_blk_version_major(void)
{
    return EFUSE.rd_mac_spi_8m_3.blk_version_major;
}

__attribute__((always_inline)) static inline uint32_t efuse_ll_get_blk_version_minor(void)
{
    return EFUSE.rd_sys_data4.blk_version_minor;
}

__attribute__((always_inline)) static inline bool efuse_ll_get_disable_blk_version_major(void)
{
    return EFUSE.rd_repeat_data4.disable_blk_version_major;
}

__attribute__((always_inline)) static inline uint32_t efuse_ll_get_chip_ver_pkg(void)
{
    return EFUSE.rd_mac_spi_8m_4.pkg_version;
}

__attribute__((always_inline)) static inline bool efuse_ll_get_sdio_force(void)
{
    return EFUSE.rd_repeat_data1.sdio_force;
}

__attribute__((always_inline)) static inline bool efuse_ll_get_sdio_tieh(void)
{
    return EFUSE.rd_repeat_data1.sdio_tieh;
}

__attribute__((always_inline)) static inline bool efuse_ll_get_sdio_xpd(void)
{
    return EFUSE.rd_repeat_data1.sdio_xpd;
}

__attribute__((always_inline)) static inline uint32_t efuse_ll_get_sdio_drefl(void)
{
    return EFUSE.rd_repeat_data1.sdio_drefl;
}

__attribute__((always_inline)) static inline uint32_t efuse_ll_get_sdio_drefm(void)
{
    return EFUSE.rd_repeat_data1.sdio_drefm;
}

__attribute__((always_inline)) static inline uint32_t efuse_ll_get_sdio_drefh(void)
{
    return EFUSE.rd_repeat_data0.sdio_drefh;
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

/******************* eFuse control functions *************************/

#ifdef __cplusplus
}
#endif
