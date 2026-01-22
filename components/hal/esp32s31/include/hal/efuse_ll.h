/*
 * SPDX-FileCopyrightText: 2025-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "soc/efuse_periph.h"
#include "hal/assert.h"
#include "rom/efuse.h"
#include "soc/efuse_defs.h"

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

// Always inline these functions even no gcc optimization is applied.

// TODO: [ESP32S31] IDF-14688 This file is inherited from verification branch, need to check all functions.

/******************* eFuse fields *************************/

__attribute__((always_inline)) static inline uint32_t efuse_ll_get_flash_crypt_cnt(void)
{
    // TODO: [ESP32S31] IDF-14688
    return EFUSE.rd_repeat_data1.spi_boot_crypt_cnt;
}

__attribute__((always_inline)) static inline uint32_t efuse_ll_get_wdt_delay_sel(void)
{
    // TODO: [ESP32S31] IDF-14688
    return EFUSE.rd_repeat_data1.wdt_delay_sel;
}

__attribute__((always_inline)) static inline uint32_t efuse_ll_get_mac0(void)
{
    // TODO: [ESP32S31] IDF-14688
    return EFUSE.rd_mac_sys0.mac_0;
}

__attribute__((always_inline)) static inline uint32_t efuse_ll_get_mac1(void)
{
    return EFUSE.rd_mac_sys1.mac_1;
}

__attribute__((always_inline)) static inline bool efuse_ll_get_secure_boot_v2_en(void)
{
    return EFUSE.rd_repeat_data3.secure_boot_en;
}

// use efuse_hal_get_major_chip_version() to get major chip version
__attribute__((always_inline)) static inline uint32_t efuse_ll_get_chip_wafer_version_major(void)
{
    return 0;
    // TODO: [ESP32S31] IDF-14688
}

// use efuse_hal_get_minor_chip_version() to get minor chip version
__attribute__((always_inline)) static inline uint32_t efuse_ll_get_chip_wafer_version_minor(void)
{
    return 0;
    // TODO: [ESP32S31] IDF-14688
}

__attribute__((always_inline)) static inline bool efuse_ll_get_disable_wafer_version_major(void)
{
    return 0;
    // TODO: [ESP32S31] IDF-14688
}

__attribute__((always_inline)) static inline uint32_t efuse_ll_get_blk_version_major(void)
{
    return 0;
    // TODO: [ESP32S31] IDF-14688
}

__attribute__((always_inline)) static inline uint32_t efuse_ll_get_blk_version_minor(void)
{
    return 0;
    // TODO: [ESP32S31] IDF-14688
}

__attribute__((always_inline)) static inline bool efuse_ll_get_disable_blk_version_major(void)
{
    return 0;
    // TODO: [ESP32S31] IDF-14688
}

__attribute__((always_inline)) static inline uint32_t efuse_ll_get_chip_ver_pkg(void)
{
    return 0;
    // TODO: [ESP32S31] IDF-14688
}

// TODO: [ESP32S31] IDF-14688
// __attribute__((always_inline)) static inline void efuse_ll_set_ecdsa_key_blk(ecdsa_curve_t curve, int efuse_blk)
// {
//     (void) curve;
//
// }

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
    // TODO: [ESP32S31] IDF-14688
}

__attribute__((always_inline)) static inline void efuse_ll_set_pgm_cmd(uint32_t block)
{
    // TODO: [ESP32S31] IDF-14688
}

__attribute__((always_inline)) static inline void efuse_ll_set_conf_read_op_code(void)
{
    // TODO: [ESP32S31] IDF-14688
}

__attribute__((always_inline)) static inline void efuse_ll_set_conf_write_op_code(void)
{
    // TODO: [ESP32S31] IDF-14688
}

__attribute__((always_inline)) static inline void efuse_ll_set_pwr_off_num(uint16_t value)
{
    // TODO: [ESP32S31] IDF-14688
}

__attribute__((always_inline)) static inline void efuse_ll_rs_bypass_update(void)
{
    // TODO: [ESP32S31] IDF-14688
}

__attribute__((always_inline)) static inline uint32_t efuse_ll_get_controller_state(void)
{
    // TODO: [ESP32S31] IDF-14688
    return EFUSE.status.state;
}

/******************* eFuse control functions *************************/

#ifdef __cplusplus
}
#endif
