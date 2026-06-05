/*
 * SPDX-FileCopyrightText: 2022-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "hal/misc.h"
#include "soc/efuse_periph.h"
#include "hal/assert.h"
#include "rom/efuse.h"
#include "hal/config.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    EFUSE_CONTROLLER_STATE_RESET            = 0,    ///< efuse_controllerid is on reset state.
    EFUSE_CONTROLLER_STATE_IDLE             = 1,    ///< efuse_controllerid is on idle state.
    EFUSE_CONTROLLER_STATE_READ_INIT        = 2,    ///< efuse_controllerid is on read init state.
    EFUSE_CONTROLLER_STATE_READ_BLK0        = 3,    ///< efuse_controllerid is on reading block0 state.
    EFUSE_CONTROLLER_STATE_BLK0_CRC_CHECK   = 4,    ///< efuse_controllerid is on checking block0 crc state.
    EFUSE_CONTROLLER_STATE_READ_RS_BLK      = 5,    ///< efuse_controllerid is on reading RS block state.
} efuse_controller_state_t;

/* Revision-aware eFuse feature macros
 *
 * These macros indicate whether an eFuse feature is available given the
 * configured minimum supported chip revision (HAL_CONFIG(CHIP_SUPPORT_MIN_REV)).
 * Use them when a feature's presence depends on the chosen minimum revision.
 *
 * Note: SOC_* capability macros describe silicon capabilities; these
 * EFUSE_LL_HAS_* macros reflect availability relative to the configured min revision.
 * If a feature is present in silicon and does not depend on the chip revision,
 * then add SOC_* macro in soc_caps.h instead.
 */
#if HAL_CONFIG(CHIP_SUPPORT_MIN_REV) >= 300
// Rev 3.00+: key_purpose fields expanded from 4 to 5 bits, enabling additional key types.
#define EFUSE_LL_HAS_ECDSA_KEY_P192    (1)
#define EFUSE_LL_HAS_ECDSA_KEY_P384    (1)
#define EFUSE_LL_HAS_PSRAM_ENCRYPTION_XTS_AES_128  (1)
#define EFUSE_LL_HAS_PSRAM_ENCRYPTION_XTS_AES_256  (1)
// Rev 3.00+: Recovery bootloader eFuse field is present.
#define EFUSE_LL_HAS_RECOVERY_BOOTLOADER  (1)
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
    return EFUSE.rd_mac_sys_0.mac_0;
}

__attribute__((always_inline)) static inline uint32_t efuse_ll_get_mac1(void)
{
    return EFUSE.rd_mac_sys_1.mac_1;
}

__attribute__((always_inline)) static inline bool efuse_ll_get_secure_boot_v2_en(void)
{
    return EFUSE.rd_repeat_data2.secure_boot_en;
}

// use efuse_hal_get_major_chip_version() to get major chip version
__attribute__((always_inline)) static inline uint32_t efuse_ll_get_chip_wafer_version_major(void)
{
    return (EFUSE.rd_mac_sys_2.wafer_version_major_hi << 2) | EFUSE.rd_mac_sys_2.wafer_version_major_lo;
}

// use efuse_hal_get_minor_chip_version() to get minor chip version
__attribute__((always_inline)) static inline uint32_t efuse_ll_get_chip_wafer_version_minor(void)
{
    return EFUSE.rd_mac_sys_2.wafer_version_minor;
}

__attribute__((always_inline)) static inline bool efuse_ll_get_disable_wafer_version_major(void)
{
    return EFUSE.rd_mac_sys_2.disable_wafer_version_major;
}

__attribute__((always_inline)) static inline uint32_t efuse_ll_get_blk_version_major(void)
{
    return EFUSE.rd_mac_sys_2.blk_version_major;
}

__attribute__((always_inline)) static inline uint32_t efuse_ll_get_blk_version_minor(void)
{
    return EFUSE.rd_mac_sys_2.blk_version_minor;
}

__attribute__((always_inline)) static inline bool efuse_ll_get_disable_blk_version_major(void)
{
    return EFUSE.rd_mac_sys_2.disable_blk_version_major;
}

__attribute__((always_inline)) static inline uint32_t efuse_ll_get_chip_ver_pkg(void)
{
    return EFUSE.rd_mac_sys_2.pkg_version;
}

#if EFUSE_LL_HAS_RECOVERY_BOOTLOADER
__attribute__((always_inline)) static inline uint32_t efuse_ll_get_recovery_bootloader_sector(void)
{
    return ((uint32_t)EFUSE.rd_repeat_data1.recovery_bootloader_flash_sector_11_11 << 11) |
           ((uint32_t)EFUSE.rd_repeat_data1.recovery_bootloader_flash_sector_8_10  <<  8) |
           ((uint32_t)EFUSE.rd_repeat_data0.recovery_bootloader_flash_sector_7_7   <<  7) |
           ((uint32_t)EFUSE.rd_repeat_data0.recovery_bootloader_flash_sector_3_6   <<  3) |
           ((uint32_t)EFUSE.rd_repeat_data0.recovery_bootloader_flash_sector_2_2   <<  2) |
           ((uint32_t)EFUSE.rd_repeat_data0.recovery_bootloader_flash_sector_0_1);
}
#else
__attribute__((always_inline)) static inline uint32_t efuse_ll_get_recovery_bootloader_sector(void)
{
    // Rev < 3.0 has no recovery bootloader eFuse field.
    return 0;
}
#endif

__attribute__((always_inline)) static inline uint32_t efuse_ll_get_coding_error(unsigned index)
{
    switch (index) {
    case 0:
        return EFUSE.rd_repeat_err0.val;
    case 1:
        return EFUSE.rd_repeat_err1.val;
    case 2:
        return EFUSE.rd_repeat_err2.val;
    case 3:
        return EFUSE.rd_repeat_err3.val;
    case 4:
        return EFUSE.rd_repeat_err4.val;
    case 5:
        return EFUSE.rd_rs_err0.val;
    case 6:
        return EFUSE.rd_rs_err1.val;
    default:
        return 0;
    }
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
    HAL_FORCE_MODIFY_U32_REG_FIELD(EFUSE.cmd, val, ((block << EFUSE_BLK_NUM_S) & EFUSE_BLK_NUM_M) | EFUSE_PGM_CMD);
}

__attribute__((always_inline)) static inline void efuse_ll_set_conf_read_op_code(void)
{
    HAL_FORCE_MODIFY_U32_REG_FIELD(EFUSE.conf, op_code, EFUSE_READ_OP_CODE);
}

__attribute__((always_inline)) static inline void efuse_ll_set_conf_write_op_code(void)
{
    HAL_FORCE_MODIFY_U32_REG_FIELD(EFUSE.conf, op_code, EFUSE_WRITE_OP_CODE);
}

__attribute__((always_inline)) static inline void efuse_ll_set_pwr_off_num(uint16_t value)
{
    HAL_FORCE_MODIFY_U32_REG_FIELD(EFUSE.wr_tim_conf2, pwr_off_num, value);
}

__attribute__((always_inline)) static inline void efuse_ll_rs_bypass_update(void)
{
    EFUSE.wr_tim_conf0_rs_bypass.update = 1;
}

__attribute__((always_inline)) static inline uint32_t efuse_ll_get_controller_state(void)
{
    return EFUSE.status.state;
}

__attribute__((always_inline)) static inline uint32_t efuse_ll_get_active_hp_dbias(void)
{
    return EFUSE.rd_mac_sys_4.active_hp_dbias;
}

__attribute__((always_inline)) static inline uint32_t efuse_ll_get_active_lp_dbias(void)
{
    return EFUSE.rd_mac_sys_4.active_lp_dbias;
}

__attribute__((always_inline)) static inline int32_t efuse_ll_get_dbias_vol_gap(void)
{
    return EFUSE.rd_mac_sys_5.lp_dcdc_dbias_vol_gap;
}

/******************* eFuse control functions *************************/

#ifdef __cplusplus
}
#endif
