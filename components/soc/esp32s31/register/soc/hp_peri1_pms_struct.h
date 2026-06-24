/**
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 *  SPDX-License-Identifier: Apache-2.0 OR MIT
 */
#pragma once

#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

/** Group: read write control register */
/** Type of hp_usbotg_phy_ctrl register
 *  hp_usbotg_phy read/write control register
 */
typedef union {
    struct {
        /** read_tee_hp_usbotg_phy : R/W; bitpos: [0]; default: 1;
         *  Configures hp_usbotg_phy registers read permission in tee mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_tee_hp_usbotg_phy:1;
        /** read_ree0_hp_usbotg_phy : R/W; bitpos: [1]; default: 0;
         *  Configures hp_usbotg_phy registers read permission in ree0 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree0_hp_usbotg_phy:1;
        /** read_ree1_hp_usbotg_phy : R/W; bitpos: [2]; default: 0;
         *  Configures hp_usbotg_phy registers read permission in ree1 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree1_hp_usbotg_phy:1;
        /** read_ree2_hp_usbotg_phy : R/W; bitpos: [3]; default: 0;
         *  Configures hp_usbotg_phy registers read permission in ree2 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree2_hp_usbotg_phy:1;
        /** write_tee_hp_usbotg_phy : R/W; bitpos: [4]; default: 1;
         *  Configures hp_usbotg_phy registers write permission in tee mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_tee_hp_usbotg_phy:1;
        /** write_ree0_hp_usbotg_phy : R/W; bitpos: [5]; default: 0;
         *  Configures hp_usbotg_phy registers write permission in ree0 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree0_hp_usbotg_phy:1;
        /** write_ree1_hp_usbotg_phy : R/W; bitpos: [6]; default: 0;
         *  Configures hp_usbotg_phy registers write permission in ree1 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree1_hp_usbotg_phy:1;
        /** write_ree2_hp_usbotg_phy : R/W; bitpos: [7]; default: 0;
         *  Configures hp_usbotg_phy registers write permission in ree2 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree2_hp_usbotg_phy:1;
        /** hp_usbotg_phy_lock : R/W; bitpos: [8]; default: 0;
         *  Set 1 to lock hp_usbotg_phy peri_apm configuration
         */
        uint32_t hp_usbotg_phy_lock:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} hp_peri1_pms_hp_usbotg_phy_ctrl_reg_t;

/** Type of hp_mcpwm0_ctrl register
 *  hp_mcpwm0 read/write control register
 */
typedef union {
    struct {
        /** read_tee_hp_mcpwm0 : R/W; bitpos: [0]; default: 1;
         *  Configures hp_mcpwm0 registers read permission in tee mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_tee_hp_mcpwm0:1;
        /** read_ree0_hp_mcpwm0 : R/W; bitpos: [1]; default: 0;
         *  Configures hp_mcpwm0 registers read permission in ree0 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree0_hp_mcpwm0:1;
        /** read_ree1_hp_mcpwm0 : R/W; bitpos: [2]; default: 0;
         *  Configures hp_mcpwm0 registers read permission in ree1 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree1_hp_mcpwm0:1;
        /** read_ree2_hp_mcpwm0 : R/W; bitpos: [3]; default: 0;
         *  Configures hp_mcpwm0 registers read permission in ree2 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree2_hp_mcpwm0:1;
        /** write_tee_hp_mcpwm0 : R/W; bitpos: [4]; default: 1;
         *  Configures hp_mcpwm0 registers write permission in tee mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_tee_hp_mcpwm0:1;
        /** write_ree0_hp_mcpwm0 : R/W; bitpos: [5]; default: 0;
         *  Configures hp_mcpwm0 registers write permission in ree0 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree0_hp_mcpwm0:1;
        /** write_ree1_hp_mcpwm0 : R/W; bitpos: [6]; default: 0;
         *  Configures hp_mcpwm0 registers write permission in ree1 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree1_hp_mcpwm0:1;
        /** write_ree2_hp_mcpwm0 : R/W; bitpos: [7]; default: 0;
         *  Configures hp_mcpwm0 registers write permission in ree2 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree2_hp_mcpwm0:1;
        /** hp_mcpwm0_lock : R/W; bitpos: [8]; default: 0;
         *  Set 1 to lock hp_mcpwm0 peri_apm configuration
         */
        uint32_t hp_mcpwm0_lock:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} hp_peri1_pms_hp_mcpwm0_ctrl_reg_t;

/** Type of hp_mcpwm1_ctrl register
 *  hp_mcpwm1 read/write control register
 */
typedef union {
    struct {
        /** read_tee_hp_mcpwm1 : R/W; bitpos: [0]; default: 1;
         *  Configures hp_mcpwm1 registers read permission in tee mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_tee_hp_mcpwm1:1;
        /** read_ree0_hp_mcpwm1 : R/W; bitpos: [1]; default: 0;
         *  Configures hp_mcpwm1 registers read permission in ree0 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree0_hp_mcpwm1:1;
        /** read_ree1_hp_mcpwm1 : R/W; bitpos: [2]; default: 0;
         *  Configures hp_mcpwm1 registers read permission in ree1 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree1_hp_mcpwm1:1;
        /** read_ree2_hp_mcpwm1 : R/W; bitpos: [3]; default: 0;
         *  Configures hp_mcpwm1 registers read permission in ree2 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree2_hp_mcpwm1:1;
        /** write_tee_hp_mcpwm1 : R/W; bitpos: [4]; default: 1;
         *  Configures hp_mcpwm1 registers write permission in tee mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_tee_hp_mcpwm1:1;
        /** write_ree0_hp_mcpwm1 : R/W; bitpos: [5]; default: 0;
         *  Configures hp_mcpwm1 registers write permission in ree0 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree0_hp_mcpwm1:1;
        /** write_ree1_hp_mcpwm1 : R/W; bitpos: [6]; default: 0;
         *  Configures hp_mcpwm1 registers write permission in ree1 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree1_hp_mcpwm1:1;
        /** write_ree2_hp_mcpwm1 : R/W; bitpos: [7]; default: 0;
         *  Configures hp_mcpwm1 registers write permission in ree2 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree2_hp_mcpwm1:1;
        /** hp_mcpwm1_lock : R/W; bitpos: [8]; default: 0;
         *  Set 1 to lock hp_mcpwm1 peri_apm configuration
         */
        uint32_t hp_mcpwm1_lock:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} hp_peri1_pms_hp_mcpwm1_ctrl_reg_t;

/** Type of hp_mcpwm2_ctrl register
 *  hp_mcpwm2 read/write control register
 */
typedef union {
    struct {
        /** read_tee_hp_mcpwm2 : R/W; bitpos: [0]; default: 1;
         *  Configures hp_mcpwm2 registers read permission in tee mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_tee_hp_mcpwm2:1;
        /** read_ree0_hp_mcpwm2 : R/W; bitpos: [1]; default: 0;
         *  Configures hp_mcpwm2 registers read permission in ree0 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree0_hp_mcpwm2:1;
        /** read_ree1_hp_mcpwm2 : R/W; bitpos: [2]; default: 0;
         *  Configures hp_mcpwm2 registers read permission in ree1 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree1_hp_mcpwm2:1;
        /** read_ree2_hp_mcpwm2 : R/W; bitpos: [3]; default: 0;
         *  Configures hp_mcpwm2 registers read permission in ree2 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree2_hp_mcpwm2:1;
        /** write_tee_hp_mcpwm2 : R/W; bitpos: [4]; default: 1;
         *  Configures hp_mcpwm2 registers write permission in tee mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_tee_hp_mcpwm2:1;
        /** write_ree0_hp_mcpwm2 : R/W; bitpos: [5]; default: 0;
         *  Configures hp_mcpwm2 registers write permission in ree0 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree0_hp_mcpwm2:1;
        /** write_ree1_hp_mcpwm2 : R/W; bitpos: [6]; default: 0;
         *  Configures hp_mcpwm2 registers write permission in ree1 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree1_hp_mcpwm2:1;
        /** write_ree2_hp_mcpwm2 : R/W; bitpos: [7]; default: 0;
         *  Configures hp_mcpwm2 registers write permission in ree2 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree2_hp_mcpwm2:1;
        /** hp_mcpwm2_lock : R/W; bitpos: [8]; default: 0;
         *  Set 1 to lock hp_mcpwm2 peri_apm configuration
         */
        uint32_t hp_mcpwm2_lock:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} hp_peri1_pms_hp_mcpwm2_ctrl_reg_t;

/** Type of hp_mcpwm3_ctrl register
 *  hp_mcpwm3 read/write control register
 */
typedef union {
    struct {
        /** read_tee_hp_mcpwm3 : R/W; bitpos: [0]; default: 1;
         *  Configures hp_mcpwm3 registers read permission in tee mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_tee_hp_mcpwm3:1;
        /** read_ree0_hp_mcpwm3 : R/W; bitpos: [1]; default: 0;
         *  Configures hp_mcpwm3 registers read permission in ree0 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree0_hp_mcpwm3:1;
        /** read_ree1_hp_mcpwm3 : R/W; bitpos: [2]; default: 0;
         *  Configures hp_mcpwm3 registers read permission in ree1 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree1_hp_mcpwm3:1;
        /** read_ree2_hp_mcpwm3 : R/W; bitpos: [3]; default: 0;
         *  Configures hp_mcpwm3 registers read permission in ree2 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree2_hp_mcpwm3:1;
        /** write_tee_hp_mcpwm3 : R/W; bitpos: [4]; default: 1;
         *  Configures hp_mcpwm3 registers write permission in tee mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_tee_hp_mcpwm3:1;
        /** write_ree0_hp_mcpwm3 : R/W; bitpos: [5]; default: 0;
         *  Configures hp_mcpwm3 registers write permission in ree0 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree0_hp_mcpwm3:1;
        /** write_ree1_hp_mcpwm3 : R/W; bitpos: [6]; default: 0;
         *  Configures hp_mcpwm3 registers write permission in ree1 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree1_hp_mcpwm3:1;
        /** write_ree2_hp_mcpwm3 : R/W; bitpos: [7]; default: 0;
         *  Configures hp_mcpwm3 registers write permission in ree2 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree2_hp_mcpwm3:1;
        /** hp_mcpwm3_lock : R/W; bitpos: [8]; default: 0;
         *  Set 1 to lock hp_mcpwm3 peri_apm configuration
         */
        uint32_t hp_mcpwm3_lock:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} hp_peri1_pms_hp_mcpwm3_ctrl_reg_t;

/** Type of hp_i2c0_ctrl register
 *  hp_i2c0 read/write control register
 */
typedef union {
    struct {
        /** read_tee_hp_i2c0 : R/W; bitpos: [0]; default: 1;
         *  Configures hp_i2c0 registers read permission in tee mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_tee_hp_i2c0:1;
        /** read_ree0_hp_i2c0 : R/W; bitpos: [1]; default: 0;
         *  Configures hp_i2c0 registers read permission in ree0 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree0_hp_i2c0:1;
        /** read_ree1_hp_i2c0 : R/W; bitpos: [2]; default: 0;
         *  Configures hp_i2c0 registers read permission in ree1 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree1_hp_i2c0:1;
        /** read_ree2_hp_i2c0 : R/W; bitpos: [3]; default: 0;
         *  Configures hp_i2c0 registers read permission in ree2 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree2_hp_i2c0:1;
        /** write_tee_hp_i2c0 : R/W; bitpos: [4]; default: 1;
         *  Configures hp_i2c0 registers write permission in tee mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_tee_hp_i2c0:1;
        /** write_ree0_hp_i2c0 : R/W; bitpos: [5]; default: 0;
         *  Configures hp_i2c0 registers write permission in ree0 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree0_hp_i2c0:1;
        /** write_ree1_hp_i2c0 : R/W; bitpos: [6]; default: 0;
         *  Configures hp_i2c0 registers write permission in ree1 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree1_hp_i2c0:1;
        /** write_ree2_hp_i2c0 : R/W; bitpos: [7]; default: 0;
         *  Configures hp_i2c0 registers write permission in ree2 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree2_hp_i2c0:1;
        /** hp_i2c0_lock : R/W; bitpos: [8]; default: 0;
         *  Set 1 to lock hp_i2c0 peri_apm configuration
         */
        uint32_t hp_i2c0_lock:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} hp_peri1_pms_hp_i2c0_ctrl_reg_t;

/** Type of hp_i2c1_ctrl register
 *  hp_i2c1 read/write control register
 */
typedef union {
    struct {
        /** read_tee_hp_i2c1 : R/W; bitpos: [0]; default: 1;
         *  Configures hp_i2c1 registers read permission in tee mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_tee_hp_i2c1:1;
        /** read_ree0_hp_i2c1 : R/W; bitpos: [1]; default: 0;
         *  Configures hp_i2c1 registers read permission in ree0 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree0_hp_i2c1:1;
        /** read_ree1_hp_i2c1 : R/W; bitpos: [2]; default: 0;
         *  Configures hp_i2c1 registers read permission in ree1 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree1_hp_i2c1:1;
        /** read_ree2_hp_i2c1 : R/W; bitpos: [3]; default: 0;
         *  Configures hp_i2c1 registers read permission in ree2 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree2_hp_i2c1:1;
        /** write_tee_hp_i2c1 : R/W; bitpos: [4]; default: 1;
         *  Configures hp_i2c1 registers write permission in tee mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_tee_hp_i2c1:1;
        /** write_ree0_hp_i2c1 : R/W; bitpos: [5]; default: 0;
         *  Configures hp_i2c1 registers write permission in ree0 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree0_hp_i2c1:1;
        /** write_ree1_hp_i2c1 : R/W; bitpos: [6]; default: 0;
         *  Configures hp_i2c1 registers write permission in ree1 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree1_hp_i2c1:1;
        /** write_ree2_hp_i2c1 : R/W; bitpos: [7]; default: 0;
         *  Configures hp_i2c1 registers write permission in ree2 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree2_hp_i2c1:1;
        /** hp_i2c1_lock : R/W; bitpos: [8]; default: 0;
         *  Set 1 to lock hp_i2c1 peri_apm configuration
         */
        uint32_t hp_i2c1_lock:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} hp_peri1_pms_hp_i2c1_ctrl_reg_t;

/** Type of hp_i2s0_ctrl register
 *  hp_i2s0 read/write control register
 */
typedef union {
    struct {
        /** read_tee_hp_i2s0 : R/W; bitpos: [0]; default: 1;
         *  Configures hp_i2s0 registers read permission in tee mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_tee_hp_i2s0:1;
        /** read_ree0_hp_i2s0 : R/W; bitpos: [1]; default: 0;
         *  Configures hp_i2s0 registers read permission in ree0 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree0_hp_i2s0:1;
        /** read_ree1_hp_i2s0 : R/W; bitpos: [2]; default: 0;
         *  Configures hp_i2s0 registers read permission in ree1 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree1_hp_i2s0:1;
        /** read_ree2_hp_i2s0 : R/W; bitpos: [3]; default: 0;
         *  Configures hp_i2s0 registers read permission in ree2 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree2_hp_i2s0:1;
        /** write_tee_hp_i2s0 : R/W; bitpos: [4]; default: 1;
         *  Configures hp_i2s0 registers write permission in tee mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_tee_hp_i2s0:1;
        /** write_ree0_hp_i2s0 : R/W; bitpos: [5]; default: 0;
         *  Configures hp_i2s0 registers write permission in ree0 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree0_hp_i2s0:1;
        /** write_ree1_hp_i2s0 : R/W; bitpos: [6]; default: 0;
         *  Configures hp_i2s0 registers write permission in ree1 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree1_hp_i2s0:1;
        /** write_ree2_hp_i2s0 : R/W; bitpos: [7]; default: 0;
         *  Configures hp_i2s0 registers write permission in ree2 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree2_hp_i2s0:1;
        /** hp_i2s0_lock : R/W; bitpos: [8]; default: 0;
         *  Set 1 to lock hp_i2s0 peri_apm configuration
         */
        uint32_t hp_i2s0_lock:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} hp_peri1_pms_hp_i2s0_ctrl_reg_t;

/** Type of hp_i2s1_ctrl register
 *  hp_i2s1 read/write control register
 */
typedef union {
    struct {
        /** read_tee_hp_i2s1 : R/W; bitpos: [0]; default: 1;
         *  Configures hp_i2s1 registers read permission in tee mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_tee_hp_i2s1:1;
        /** read_ree0_hp_i2s1 : R/W; bitpos: [1]; default: 0;
         *  Configures hp_i2s1 registers read permission in ree0 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree0_hp_i2s1:1;
        /** read_ree1_hp_i2s1 : R/W; bitpos: [2]; default: 0;
         *  Configures hp_i2s1 registers read permission in ree1 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree1_hp_i2s1:1;
        /** read_ree2_hp_i2s1 : R/W; bitpos: [3]; default: 0;
         *  Configures hp_i2s1 registers read permission in ree2 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree2_hp_i2s1:1;
        /** write_tee_hp_i2s1 : R/W; bitpos: [4]; default: 1;
         *  Configures hp_i2s1 registers write permission in tee mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_tee_hp_i2s1:1;
        /** write_ree0_hp_i2s1 : R/W; bitpos: [5]; default: 0;
         *  Configures hp_i2s1 registers write permission in ree0 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree0_hp_i2s1:1;
        /** write_ree1_hp_i2s1 : R/W; bitpos: [6]; default: 0;
         *  Configures hp_i2s1 registers write permission in ree1 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree1_hp_i2s1:1;
        /** write_ree2_hp_i2s1 : R/W; bitpos: [7]; default: 0;
         *  Configures hp_i2s1 registers write permission in ree2 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree2_hp_i2s1:1;
        /** hp_i2s1_lock : R/W; bitpos: [8]; default: 0;
         *  Set 1 to lock hp_i2s1 peri_apm configuration
         */
        uint32_t hp_i2s1_lock:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} hp_peri1_pms_hp_i2s1_ctrl_reg_t;

/** Type of hp_pcnt0_ctrl register
 *  hp_pcnt0 read/write control register
 */
typedef union {
    struct {
        /** read_tee_hp_pcnt0 : R/W; bitpos: [0]; default: 1;
         *  Configures hp_pcnt0 registers read permission in tee mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_tee_hp_pcnt0:1;
        /** read_ree0_hp_pcnt0 : R/W; bitpos: [1]; default: 0;
         *  Configures hp_pcnt0 registers read permission in ree0 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree0_hp_pcnt0:1;
        /** read_ree1_hp_pcnt0 : R/W; bitpos: [2]; default: 0;
         *  Configures hp_pcnt0 registers read permission in ree1 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree1_hp_pcnt0:1;
        /** read_ree2_hp_pcnt0 : R/W; bitpos: [3]; default: 0;
         *  Configures hp_pcnt0 registers read permission in ree2 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree2_hp_pcnt0:1;
        /** write_tee_hp_pcnt0 : R/W; bitpos: [4]; default: 1;
         *  Configures hp_pcnt0 registers write permission in tee mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_tee_hp_pcnt0:1;
        /** write_ree0_hp_pcnt0 : R/W; bitpos: [5]; default: 0;
         *  Configures hp_pcnt0 registers write permission in ree0 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree0_hp_pcnt0:1;
        /** write_ree1_hp_pcnt0 : R/W; bitpos: [6]; default: 0;
         *  Configures hp_pcnt0 registers write permission in ree1 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree1_hp_pcnt0:1;
        /** write_ree2_hp_pcnt0 : R/W; bitpos: [7]; default: 0;
         *  Configures hp_pcnt0 registers write permission in ree2 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree2_hp_pcnt0:1;
        /** hp_pcnt0_lock : R/W; bitpos: [8]; default: 0;
         *  Set 1 to lock hp_pcnt0 peri_apm configuration
         */
        uint32_t hp_pcnt0_lock:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} hp_peri1_pms_hp_pcnt0_ctrl_reg_t;

/** Type of hp_uart0_ctrl register
 *  hp_uart0 read/write control register
 */
typedef union {
    struct {
        /** read_tee_hp_uart0 : R/W; bitpos: [0]; default: 1;
         *  Configures hp_uart0 registers read permission in tee mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_tee_hp_uart0:1;
        /** read_ree0_hp_uart0 : R/W; bitpos: [1]; default: 0;
         *  Configures hp_uart0 registers read permission in ree0 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree0_hp_uart0:1;
        /** read_ree1_hp_uart0 : R/W; bitpos: [2]; default: 0;
         *  Configures hp_uart0 registers read permission in ree1 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree1_hp_uart0:1;
        /** read_ree2_hp_uart0 : R/W; bitpos: [3]; default: 0;
         *  Configures hp_uart0 registers read permission in ree2 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree2_hp_uart0:1;
        /** write_tee_hp_uart0 : R/W; bitpos: [4]; default: 1;
         *  Configures hp_uart0 registers write permission in tee mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_tee_hp_uart0:1;
        /** write_ree0_hp_uart0 : R/W; bitpos: [5]; default: 0;
         *  Configures hp_uart0 registers write permission in ree0 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree0_hp_uart0:1;
        /** write_ree1_hp_uart0 : R/W; bitpos: [6]; default: 0;
         *  Configures hp_uart0 registers write permission in ree1 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree1_hp_uart0:1;
        /** write_ree2_hp_uart0 : R/W; bitpos: [7]; default: 0;
         *  Configures hp_uart0 registers write permission in ree2 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree2_hp_uart0:1;
        /** hp_uart0_lock : R/W; bitpos: [8]; default: 0;
         *  Set 1 to lock hp_uart0 peri_apm configuration
         */
        uint32_t hp_uart0_lock:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} hp_peri1_pms_hp_uart0_ctrl_reg_t;

/** Type of hp_uart1_ctrl register
 *  hp_uart1 read/write control register
 */
typedef union {
    struct {
        /** read_tee_hp_uart1 : R/W; bitpos: [0]; default: 1;
         *  Configures hp_uart1 registers read permission in tee mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_tee_hp_uart1:1;
        /** read_ree0_hp_uart1 : R/W; bitpos: [1]; default: 0;
         *  Configures hp_uart1 registers read permission in ree0 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree0_hp_uart1:1;
        /** read_ree1_hp_uart1 : R/W; bitpos: [2]; default: 0;
         *  Configures hp_uart1 registers read permission in ree1 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree1_hp_uart1:1;
        /** read_ree2_hp_uart1 : R/W; bitpos: [3]; default: 0;
         *  Configures hp_uart1 registers read permission in ree2 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree2_hp_uart1:1;
        /** write_tee_hp_uart1 : R/W; bitpos: [4]; default: 1;
         *  Configures hp_uart1 registers write permission in tee mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_tee_hp_uart1:1;
        /** write_ree0_hp_uart1 : R/W; bitpos: [5]; default: 0;
         *  Configures hp_uart1 registers write permission in ree0 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree0_hp_uart1:1;
        /** write_ree1_hp_uart1 : R/W; bitpos: [6]; default: 0;
         *  Configures hp_uart1 registers write permission in ree1 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree1_hp_uart1:1;
        /** write_ree2_hp_uart1 : R/W; bitpos: [7]; default: 0;
         *  Configures hp_uart1 registers write permission in ree2 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree2_hp_uart1:1;
        /** hp_uart1_lock : R/W; bitpos: [8]; default: 0;
         *  Set 1 to lock hp_uart1 peri_apm configuration
         */
        uint32_t hp_uart1_lock:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} hp_peri1_pms_hp_uart1_ctrl_reg_t;

/** Type of hp_uart2_ctrl register
 *  hp_uart2 read/write control register
 */
typedef union {
    struct {
        /** read_tee_hp_uart2 : R/W; bitpos: [0]; default: 1;
         *  Configures hp_uart2 registers read permission in tee mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_tee_hp_uart2:1;
        /** read_ree0_hp_uart2 : R/W; bitpos: [1]; default: 0;
         *  Configures hp_uart2 registers read permission in ree0 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree0_hp_uart2:1;
        /** read_ree1_hp_uart2 : R/W; bitpos: [2]; default: 0;
         *  Configures hp_uart2 registers read permission in ree1 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree1_hp_uart2:1;
        /** read_ree2_hp_uart2 : R/W; bitpos: [3]; default: 0;
         *  Configures hp_uart2 registers read permission in ree2 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree2_hp_uart2:1;
        /** write_tee_hp_uart2 : R/W; bitpos: [4]; default: 1;
         *  Configures hp_uart2 registers write permission in tee mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_tee_hp_uart2:1;
        /** write_ree0_hp_uart2 : R/W; bitpos: [5]; default: 0;
         *  Configures hp_uart2 registers write permission in ree0 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree0_hp_uart2:1;
        /** write_ree1_hp_uart2 : R/W; bitpos: [6]; default: 0;
         *  Configures hp_uart2 registers write permission in ree1 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree1_hp_uart2:1;
        /** write_ree2_hp_uart2 : R/W; bitpos: [7]; default: 0;
         *  Configures hp_uart2 registers write permission in ree2 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree2_hp_uart2:1;
        /** hp_uart2_lock : R/W; bitpos: [8]; default: 0;
         *  Set 1 to lock hp_uart2 peri_apm configuration
         */
        uint32_t hp_uart2_lock:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} hp_peri1_pms_hp_uart2_ctrl_reg_t;

/** Type of hp_uart3_ctrl register
 *  hp_uart3 read/write control register
 */
typedef union {
    struct {
        /** read_tee_hp_uart3 : R/W; bitpos: [0]; default: 1;
         *  Configures hp_uart3 registers read permission in tee mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_tee_hp_uart3:1;
        /** read_ree0_hp_uart3 : R/W; bitpos: [1]; default: 0;
         *  Configures hp_uart3 registers read permission in ree0 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree0_hp_uart3:1;
        /** read_ree1_hp_uart3 : R/W; bitpos: [2]; default: 0;
         *  Configures hp_uart3 registers read permission in ree1 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree1_hp_uart3:1;
        /** read_ree2_hp_uart3 : R/W; bitpos: [3]; default: 0;
         *  Configures hp_uart3 registers read permission in ree2 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree2_hp_uart3:1;
        /** write_tee_hp_uart3 : R/W; bitpos: [4]; default: 1;
         *  Configures hp_uart3 registers write permission in tee mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_tee_hp_uart3:1;
        /** write_ree0_hp_uart3 : R/W; bitpos: [5]; default: 0;
         *  Configures hp_uart3 registers write permission in ree0 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree0_hp_uart3:1;
        /** write_ree1_hp_uart3 : R/W; bitpos: [6]; default: 0;
         *  Configures hp_uart3 registers write permission in ree1 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree1_hp_uart3:1;
        /** write_ree2_hp_uart3 : R/W; bitpos: [7]; default: 0;
         *  Configures hp_uart3 registers write permission in ree2 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree2_hp_uart3:1;
        /** hp_uart3_lock : R/W; bitpos: [8]; default: 0;
         *  Set 1 to lock hp_uart3 peri_apm configuration
         */
        uint32_t hp_uart3_lock:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} hp_peri1_pms_hp_uart3_ctrl_reg_t;

/** Type of hp_parlio_ctrl register
 *  hp_parlio read/write control register
 */
typedef union {
    struct {
        /** read_tee_hp_parlio : R/W; bitpos: [0]; default: 1;
         *  Configures hp_parlio registers read permission in tee mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_tee_hp_parlio:1;
        /** read_ree0_hp_parlio : R/W; bitpos: [1]; default: 0;
         *  Configures hp_parlio registers read permission in ree0 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree0_hp_parlio:1;
        /** read_ree1_hp_parlio : R/W; bitpos: [2]; default: 0;
         *  Configures hp_parlio registers read permission in ree1 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree1_hp_parlio:1;
        /** read_ree2_hp_parlio : R/W; bitpos: [3]; default: 0;
         *  Configures hp_parlio registers read permission in ree2 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree2_hp_parlio:1;
        /** write_tee_hp_parlio : R/W; bitpos: [4]; default: 1;
         *  Configures hp_parlio registers write permission in tee mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_tee_hp_parlio:1;
        /** write_ree0_hp_parlio : R/W; bitpos: [5]; default: 0;
         *  Configures hp_parlio registers write permission in ree0 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree0_hp_parlio:1;
        /** write_ree1_hp_parlio : R/W; bitpos: [6]; default: 0;
         *  Configures hp_parlio registers write permission in ree1 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree1_hp_parlio:1;
        /** write_ree2_hp_parlio : R/W; bitpos: [7]; default: 0;
         *  Configures hp_parlio registers write permission in ree2 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree2_hp_parlio:1;
        /** hp_parlio_lock : R/W; bitpos: [8]; default: 0;
         *  Set 1 to lock hp_parlio peri_apm configuration
         */
        uint32_t hp_parlio_lock:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} hp_peri1_pms_hp_parlio_ctrl_reg_t;

/** Type of hp_gpspi2_ctrl register
 *  hp_gpspi2 read/write control register
 */
typedef union {
    struct {
        /** read_tee_hp_gpspi2 : R/W; bitpos: [0]; default: 1;
         *  Configures hp_gpspi2 registers read permission in tee mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_tee_hp_gpspi2:1;
        /** read_ree0_hp_gpspi2 : R/W; bitpos: [1]; default: 0;
         *  Configures hp_gpspi2 registers read permission in ree0 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree0_hp_gpspi2:1;
        /** read_ree1_hp_gpspi2 : R/W; bitpos: [2]; default: 0;
         *  Configures hp_gpspi2 registers read permission in ree1 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree1_hp_gpspi2:1;
        /** read_ree2_hp_gpspi2 : R/W; bitpos: [3]; default: 0;
         *  Configures hp_gpspi2 registers read permission in ree2 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree2_hp_gpspi2:1;
        /** write_tee_hp_gpspi2 : R/W; bitpos: [4]; default: 1;
         *  Configures hp_gpspi2 registers write permission in tee mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_tee_hp_gpspi2:1;
        /** write_ree0_hp_gpspi2 : R/W; bitpos: [5]; default: 0;
         *  Configures hp_gpspi2 registers write permission in ree0 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree0_hp_gpspi2:1;
        /** write_ree1_hp_gpspi2 : R/W; bitpos: [6]; default: 0;
         *  Configures hp_gpspi2 registers write permission in ree1 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree1_hp_gpspi2:1;
        /** write_ree2_hp_gpspi2 : R/W; bitpos: [7]; default: 0;
         *  Configures hp_gpspi2 registers write permission in ree2 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree2_hp_gpspi2:1;
        /** hp_gpspi2_lock : R/W; bitpos: [8]; default: 0;
         *  Set 1 to lock hp_gpspi2 peri_apm configuration
         */
        uint32_t hp_gpspi2_lock:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} hp_peri1_pms_hp_gpspi2_ctrl_reg_t;

/** Type of hp_gpspi3_ctrl register
 *  hp_gpspi3 read/write control register
 */
typedef union {
    struct {
        /** read_tee_hp_gpspi3 : R/W; bitpos: [0]; default: 1;
         *  Configures hp_gpspi3 registers read permission in tee mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_tee_hp_gpspi3:1;
        /** read_ree0_hp_gpspi3 : R/W; bitpos: [1]; default: 0;
         *  Configures hp_gpspi3 registers read permission in ree0 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree0_hp_gpspi3:1;
        /** read_ree1_hp_gpspi3 : R/W; bitpos: [2]; default: 0;
         *  Configures hp_gpspi3 registers read permission in ree1 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree1_hp_gpspi3:1;
        /** read_ree2_hp_gpspi3 : R/W; bitpos: [3]; default: 0;
         *  Configures hp_gpspi3 registers read permission in ree2 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree2_hp_gpspi3:1;
        /** write_tee_hp_gpspi3 : R/W; bitpos: [4]; default: 1;
         *  Configures hp_gpspi3 registers write permission in tee mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_tee_hp_gpspi3:1;
        /** write_ree0_hp_gpspi3 : R/W; bitpos: [5]; default: 0;
         *  Configures hp_gpspi3 registers write permission in ree0 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree0_hp_gpspi3:1;
        /** write_ree1_hp_gpspi3 : R/W; bitpos: [6]; default: 0;
         *  Configures hp_gpspi3 registers write permission in ree1 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree1_hp_gpspi3:1;
        /** write_ree2_hp_gpspi3 : R/W; bitpos: [7]; default: 0;
         *  Configures hp_gpspi3 registers write permission in ree2 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree2_hp_gpspi3:1;
        /** hp_gpspi3_lock : R/W; bitpos: [8]; default: 0;
         *  Set 1 to lock hp_gpspi3 peri_apm configuration
         */
        uint32_t hp_gpspi3_lock:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} hp_peri1_pms_hp_gpspi3_ctrl_reg_t;

/** Type of hp_usbdevice_ctrl register
 *  hp_usbdevice read/write control register
 */
typedef union {
    struct {
        /** read_tee_hp_usbdevice : R/W; bitpos: [0]; default: 1;
         *  Configures hp_usbdevice registers read permission in tee mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_tee_hp_usbdevice:1;
        /** read_ree0_hp_usbdevice : R/W; bitpos: [1]; default: 0;
         *  Configures hp_usbdevice registers read permission in ree0 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree0_hp_usbdevice:1;
        /** read_ree1_hp_usbdevice : R/W; bitpos: [2]; default: 0;
         *  Configures hp_usbdevice registers read permission in ree1 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree1_hp_usbdevice:1;
        /** read_ree2_hp_usbdevice : R/W; bitpos: [3]; default: 0;
         *  Configures hp_usbdevice registers read permission in ree2 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree2_hp_usbdevice:1;
        /** write_tee_hp_usbdevice : R/W; bitpos: [4]; default: 1;
         *  Configures hp_usbdevice registers write permission in tee mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_tee_hp_usbdevice:1;
        /** write_ree0_hp_usbdevice : R/W; bitpos: [5]; default: 0;
         *  Configures hp_usbdevice registers write permission in ree0 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree0_hp_usbdevice:1;
        /** write_ree1_hp_usbdevice : R/W; bitpos: [6]; default: 0;
         *  Configures hp_usbdevice registers write permission in ree1 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree1_hp_usbdevice:1;
        /** write_ree2_hp_usbdevice : R/W; bitpos: [7]; default: 0;
         *  Configures hp_usbdevice registers write permission in ree2 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree2_hp_usbdevice:1;
        /** hp_usbdevice_lock : R/W; bitpos: [8]; default: 0;
         *  Set 1 to lock hp_usbdevice peri_apm configuration
         */
        uint32_t hp_usbdevice_lock:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} hp_peri1_pms_hp_usbdevice_ctrl_reg_t;

/** Type of hp_ledc0_ctrl register
 *  hp_ledc0 read/write control register
 */
typedef union {
    struct {
        /** read_tee_hp_ledc0 : R/W; bitpos: [0]; default: 1;
         *  Configures hp_ledc0 registers read permission in tee mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_tee_hp_ledc0:1;
        /** read_ree0_hp_ledc0 : R/W; bitpos: [1]; default: 0;
         *  Configures hp_ledc0 registers read permission in ree0 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree0_hp_ledc0:1;
        /** read_ree1_hp_ledc0 : R/W; bitpos: [2]; default: 0;
         *  Configures hp_ledc0 registers read permission in ree1 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree1_hp_ledc0:1;
        /** read_ree2_hp_ledc0 : R/W; bitpos: [3]; default: 0;
         *  Configures hp_ledc0 registers read permission in ree2 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree2_hp_ledc0:1;
        /** write_tee_hp_ledc0 : R/W; bitpos: [4]; default: 1;
         *  Configures hp_ledc0 registers write permission in tee mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_tee_hp_ledc0:1;
        /** write_ree0_hp_ledc0 : R/W; bitpos: [5]; default: 0;
         *  Configures hp_ledc0 registers write permission in ree0 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree0_hp_ledc0:1;
        /** write_ree1_hp_ledc0 : R/W; bitpos: [6]; default: 0;
         *  Configures hp_ledc0 registers write permission in ree1 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree1_hp_ledc0:1;
        /** write_ree2_hp_ledc0 : R/W; bitpos: [7]; default: 0;
         *  Configures hp_ledc0 registers write permission in ree2 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree2_hp_ledc0:1;
        /** hp_ledc0_lock : R/W; bitpos: [8]; default: 0;
         *  Set 1 to lock hp_ledc0 peri_apm configuration
         */
        uint32_t hp_ledc0_lock:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} hp_peri1_pms_hp_ledc0_ctrl_reg_t;

/** Type of hp_etm_ctrl register
 *  hp_etm read/write control register
 */
typedef union {
    struct {
        /** read_tee_hp_etm : R/W; bitpos: [0]; default: 1;
         *  Configures hp_etm registers read permission in tee mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_tee_hp_etm:1;
        /** read_ree0_hp_etm : R/W; bitpos: [1]; default: 0;
         *  Configures hp_etm registers read permission in ree0 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree0_hp_etm:1;
        /** read_ree1_hp_etm : R/W; bitpos: [2]; default: 0;
         *  Configures hp_etm registers read permission in ree1 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree1_hp_etm:1;
        /** read_ree2_hp_etm : R/W; bitpos: [3]; default: 0;
         *  Configures hp_etm registers read permission in ree2 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree2_hp_etm:1;
        /** write_tee_hp_etm : R/W; bitpos: [4]; default: 1;
         *  Configures hp_etm registers write permission in tee mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_tee_hp_etm:1;
        /** write_ree0_hp_etm : R/W; bitpos: [5]; default: 0;
         *  Configures hp_etm registers write permission in ree0 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree0_hp_etm:1;
        /** write_ree1_hp_etm : R/W; bitpos: [6]; default: 0;
         *  Configures hp_etm registers write permission in ree1 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree1_hp_etm:1;
        /** write_ree2_hp_etm : R/W; bitpos: [7]; default: 0;
         *  Configures hp_etm registers write permission in ree2 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree2_hp_etm:1;
        /** hp_etm_lock : R/W; bitpos: [8]; default: 0;
         *  Set 1 to lock hp_etm peri_apm configuration
         */
        uint32_t hp_etm_lock:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} hp_peri1_pms_hp_etm_ctrl_reg_t;

/** Type of hp_twai0_ctrl register
 *  hp_twai0 read/write control register
 */
typedef union {
    struct {
        /** read_tee_hp_twai0 : R/W; bitpos: [0]; default: 1;
         *  Configures hp_twai0 registers read permission in tee mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_tee_hp_twai0:1;
        /** read_ree0_hp_twai0 : R/W; bitpos: [1]; default: 0;
         *  Configures hp_twai0 registers read permission in ree0 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree0_hp_twai0:1;
        /** read_ree1_hp_twai0 : R/W; bitpos: [2]; default: 0;
         *  Configures hp_twai0 registers read permission in ree1 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree1_hp_twai0:1;
        /** read_ree2_hp_twai0 : R/W; bitpos: [3]; default: 0;
         *  Configures hp_twai0 registers read permission in ree2 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree2_hp_twai0:1;
        /** write_tee_hp_twai0 : R/W; bitpos: [4]; default: 1;
         *  Configures hp_twai0 registers write permission in tee mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_tee_hp_twai0:1;
        /** write_ree0_hp_twai0 : R/W; bitpos: [5]; default: 0;
         *  Configures hp_twai0 registers write permission in ree0 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree0_hp_twai0:1;
        /** write_ree1_hp_twai0 : R/W; bitpos: [6]; default: 0;
         *  Configures hp_twai0 registers write permission in ree1 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree1_hp_twai0:1;
        /** write_ree2_hp_twai0 : R/W; bitpos: [7]; default: 0;
         *  Configures hp_twai0 registers write permission in ree2 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree2_hp_twai0:1;
        /** hp_twai0_lock : R/W; bitpos: [8]; default: 0;
         *  Set 1 to lock hp_twai0 peri_apm configuration
         */
        uint32_t hp_twai0_lock:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} hp_peri1_pms_hp_twai0_ctrl_reg_t;

/** Type of hp_twai1_ctrl register
 *  hp_twai1 read/write control register
 */
typedef union {
    struct {
        /** read_tee_hp_twai1 : R/W; bitpos: [0]; default: 1;
         *  Configures hp_twai1 registers read permission in tee mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_tee_hp_twai1:1;
        /** read_ree0_hp_twai1 : R/W; bitpos: [1]; default: 0;
         *  Configures hp_twai1 registers read permission in ree0 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree0_hp_twai1:1;
        /** read_ree1_hp_twai1 : R/W; bitpos: [2]; default: 0;
         *  Configures hp_twai1 registers read permission in ree1 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree1_hp_twai1:1;
        /** read_ree2_hp_twai1 : R/W; bitpos: [3]; default: 0;
         *  Configures hp_twai1 registers read permission in ree2 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree2_hp_twai1:1;
        /** write_tee_hp_twai1 : R/W; bitpos: [4]; default: 1;
         *  Configures hp_twai1 registers write permission in tee mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_tee_hp_twai1:1;
        /** write_ree0_hp_twai1 : R/W; bitpos: [5]; default: 0;
         *  Configures hp_twai1 registers write permission in ree0 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree0_hp_twai1:1;
        /** write_ree1_hp_twai1 : R/W; bitpos: [6]; default: 0;
         *  Configures hp_twai1 registers write permission in ree1 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree1_hp_twai1:1;
        /** write_ree2_hp_twai1 : R/W; bitpos: [7]; default: 0;
         *  Configures hp_twai1 registers write permission in ree2 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree2_hp_twai1:1;
        /** hp_twai1_lock : R/W; bitpos: [8]; default: 0;
         *  Set 1 to lock hp_twai1 peri_apm configuration
         */
        uint32_t hp_twai1_lock:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} hp_peri1_pms_hp_twai1_ctrl_reg_t;

/** Type of hp_lcdcam_ctrl register
 *  hp_lcdcam read/write control register
 */
typedef union {
    struct {
        /** read_tee_hp_lcdcam : R/W; bitpos: [0]; default: 1;
         *  Configures hp_lcdcam registers read permission in tee mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_tee_hp_lcdcam:1;
        /** read_ree0_hp_lcdcam : R/W; bitpos: [1]; default: 0;
         *  Configures hp_lcdcam registers read permission in ree0 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree0_hp_lcdcam:1;
        /** read_ree1_hp_lcdcam : R/W; bitpos: [2]; default: 0;
         *  Configures hp_lcdcam registers read permission in ree1 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree1_hp_lcdcam:1;
        /** read_ree2_hp_lcdcam : R/W; bitpos: [3]; default: 0;
         *  Configures hp_lcdcam registers read permission in ree2 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree2_hp_lcdcam:1;
        /** write_tee_hp_lcdcam : R/W; bitpos: [4]; default: 1;
         *  Configures hp_lcdcam registers write permission in tee mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_tee_hp_lcdcam:1;
        /** write_ree0_hp_lcdcam : R/W; bitpos: [5]; default: 0;
         *  Configures hp_lcdcam registers write permission in ree0 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree0_hp_lcdcam:1;
        /** write_ree1_hp_lcdcam : R/W; bitpos: [6]; default: 0;
         *  Configures hp_lcdcam registers write permission in ree1 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree1_hp_lcdcam:1;
        /** write_ree2_hp_lcdcam : R/W; bitpos: [7]; default: 0;
         *  Configures hp_lcdcam registers write permission in ree2 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree2_hp_lcdcam:1;
        /** hp_lcdcam_lock : R/W; bitpos: [8]; default: 0;
         *  Set 1 to lock hp_lcdcam peri_apm configuration
         */
        uint32_t hp_lcdcam_lock:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} hp_peri1_pms_hp_lcdcam_ctrl_reg_t;

/** Type of hp_uhci_ctrl register
 *  hp_uhci read/write control register
 */
typedef union {
    struct {
        /** read_tee_hp_uhci : R/W; bitpos: [0]; default: 1;
         *  Configures hp_uhci registers read permission in tee mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_tee_hp_uhci:1;
        /** read_ree0_hp_uhci : R/W; bitpos: [1]; default: 0;
         *  Configures hp_uhci registers read permission in ree0 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree0_hp_uhci:1;
        /** read_ree1_hp_uhci : R/W; bitpos: [2]; default: 0;
         *  Configures hp_uhci registers read permission in ree1 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree1_hp_uhci:1;
        /** read_ree2_hp_uhci : R/W; bitpos: [3]; default: 0;
         *  Configures hp_uhci registers read permission in ree2 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree2_hp_uhci:1;
        /** write_tee_hp_uhci : R/W; bitpos: [4]; default: 1;
         *  Configures hp_uhci registers write permission in tee mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_tee_hp_uhci:1;
        /** write_ree0_hp_uhci : R/W; bitpos: [5]; default: 0;
         *  Configures hp_uhci registers write permission in ree0 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree0_hp_uhci:1;
        /** write_ree1_hp_uhci : R/W; bitpos: [6]; default: 0;
         *  Configures hp_uhci registers write permission in ree1 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree1_hp_uhci:1;
        /** write_ree2_hp_uhci : R/W; bitpos: [7]; default: 0;
         *  Configures hp_uhci registers write permission in ree2 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree2_hp_uhci:1;
        /** hp_uhci_lock : R/W; bitpos: [8]; default: 0;
         *  Set 1 to lock hp_uhci peri_apm configuration
         */
        uint32_t hp_uhci_lock:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} hp_peri1_pms_hp_uhci_ctrl_reg_t;

/** Type of hp_systimer_ctrl register
 *  hp_systimer read/write control register
 */
typedef union {
    struct {
        /** read_tee_hp_systimer : R/W; bitpos: [0]; default: 1;
         *  Configures hp_systimer registers read permission in tee mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_tee_hp_systimer:1;
        /** read_ree0_hp_systimer : R/W; bitpos: [1]; default: 0;
         *  Configures hp_systimer registers read permission in ree0 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree0_hp_systimer:1;
        /** read_ree1_hp_systimer : R/W; bitpos: [2]; default: 0;
         *  Configures hp_systimer registers read permission in ree1 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree1_hp_systimer:1;
        /** read_ree2_hp_systimer : R/W; bitpos: [3]; default: 0;
         *  Configures hp_systimer registers read permission in ree2 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree2_hp_systimer:1;
        /** write_tee_hp_systimer : R/W; bitpos: [4]; default: 1;
         *  Configures hp_systimer registers write permission in tee mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_tee_hp_systimer:1;
        /** write_ree0_hp_systimer : R/W; bitpos: [5]; default: 0;
         *  Configures hp_systimer registers write permission in ree0 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree0_hp_systimer:1;
        /** write_ree1_hp_systimer : R/W; bitpos: [6]; default: 0;
         *  Configures hp_systimer registers write permission in ree1 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree1_hp_systimer:1;
        /** write_ree2_hp_systimer : R/W; bitpos: [7]; default: 0;
         *  Configures hp_systimer registers write permission in ree2 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree2_hp_systimer:1;
        /** hp_systimer_lock : R/W; bitpos: [8]; default: 0;
         *  Set 1 to lock hp_systimer peri_apm configuration
         */
        uint32_t hp_systimer_lock:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} hp_peri1_pms_hp_systimer_ctrl_reg_t;

/** Type of hp_zero_det_ctrl register
 *  hp_zero_det read/write control register
 */
typedef union {
    struct {
        /** read_tee_hp_zero_det : R/W; bitpos: [0]; default: 1;
         *  Configures hp_zero_det registers read permission in tee mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_tee_hp_zero_det:1;
        /** read_ree0_hp_zero_det : R/W; bitpos: [1]; default: 0;
         *  Configures hp_zero_det registers read permission in ree0 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree0_hp_zero_det:1;
        /** read_ree1_hp_zero_det : R/W; bitpos: [2]; default: 0;
         *  Configures hp_zero_det registers read permission in ree1 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree1_hp_zero_det:1;
        /** read_ree2_hp_zero_det : R/W; bitpos: [3]; default: 0;
         *  Configures hp_zero_det registers read permission in ree2 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree2_hp_zero_det:1;
        /** write_tee_hp_zero_det : R/W; bitpos: [4]; default: 1;
         *  Configures hp_zero_det registers write permission in tee mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_tee_hp_zero_det:1;
        /** write_ree0_hp_zero_det : R/W; bitpos: [5]; default: 0;
         *  Configures hp_zero_det registers write permission in ree0 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree0_hp_zero_det:1;
        /** write_ree1_hp_zero_det : R/W; bitpos: [6]; default: 0;
         *  Configures hp_zero_det registers write permission in ree1 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree1_hp_zero_det:1;
        /** write_ree2_hp_zero_det : R/W; bitpos: [7]; default: 0;
         *  Configures hp_zero_det registers write permission in ree2 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree2_hp_zero_det:1;
        /** hp_zero_det_lock : R/W; bitpos: [8]; default: 0;
         *  Set 1 to lock hp_zero_det peri_apm configuration
         */
        uint32_t hp_zero_det_lock:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} hp_peri1_pms_hp_zero_det_ctrl_reg_t;

/** Type of hp_cordic_ctrl register
 *  hp_cordic read/write control register
 */
typedef union {
    struct {
        /** read_tee_hp_cordic : R/W; bitpos: [0]; default: 1;
         *  Configures hp_cordic registers read permission in tee mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_tee_hp_cordic:1;
        /** read_ree0_hp_cordic : R/W; bitpos: [1]; default: 0;
         *  Configures hp_cordic registers read permission in ree0 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree0_hp_cordic:1;
        /** read_ree1_hp_cordic : R/W; bitpos: [2]; default: 0;
         *  Configures hp_cordic registers read permission in ree1 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree1_hp_cordic:1;
        /** read_ree2_hp_cordic : R/W; bitpos: [3]; default: 0;
         *  Configures hp_cordic registers read permission in ree2 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree2_hp_cordic:1;
        /** write_tee_hp_cordic : R/W; bitpos: [4]; default: 1;
         *  Configures hp_cordic registers write permission in tee mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_tee_hp_cordic:1;
        /** write_ree0_hp_cordic : R/W; bitpos: [5]; default: 0;
         *  Configures hp_cordic registers write permission in ree0 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree0_hp_cordic:1;
        /** write_ree1_hp_cordic : R/W; bitpos: [6]; default: 0;
         *  Configures hp_cordic registers write permission in ree1 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree1_hp_cordic:1;
        /** write_ree2_hp_cordic : R/W; bitpos: [7]; default: 0;
         *  Configures hp_cordic registers write permission in ree2 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree2_hp_cordic:1;
        /** hp_cordic_lock : R/W; bitpos: [8]; default: 0;
         *  Set 1 to lock hp_cordic peri_apm configuration
         */
        uint32_t hp_cordic_lock:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} hp_peri1_pms_hp_cordic_ctrl_reg_t;

/** Type of hp_ledc1_ctrl register
 *  hp_ledc1 read/write control register
 */
typedef union {
    struct {
        /** read_tee_hp_ledc1 : R/W; bitpos: [0]; default: 1;
         *  Configures hp_ledc1 registers read permission in tee mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_tee_hp_ledc1:1;
        /** read_ree0_hp_ledc1 : R/W; bitpos: [1]; default: 0;
         *  Configures hp_ledc1 registers read permission in ree0 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree0_hp_ledc1:1;
        /** read_ree1_hp_ledc1 : R/W; bitpos: [2]; default: 0;
         *  Configures hp_ledc1 registers read permission in ree1 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree1_hp_ledc1:1;
        /** read_ree2_hp_ledc1 : R/W; bitpos: [3]; default: 0;
         *  Configures hp_ledc1 registers read permission in ree2 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree2_hp_ledc1:1;
        /** write_tee_hp_ledc1 : R/W; bitpos: [4]; default: 1;
         *  Configures hp_ledc1 registers write permission in tee mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_tee_hp_ledc1:1;
        /** write_ree0_hp_ledc1 : R/W; bitpos: [5]; default: 0;
         *  Configures hp_ledc1 registers write permission in ree0 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree0_hp_ledc1:1;
        /** write_ree1_hp_ledc1 : R/W; bitpos: [6]; default: 0;
         *  Configures hp_ledc1 registers write permission in ree1 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree1_hp_ledc1:1;
        /** write_ree2_hp_ledc1 : R/W; bitpos: [7]; default: 0;
         *  Configures hp_ledc1 registers write permission in ree2 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree2_hp_ledc1:1;
        /** hp_ledc1_lock : R/W; bitpos: [8]; default: 0;
         *  Set 1 to lock hp_ledc1 peri_apm configuration
         */
        uint32_t hp_ledc1_lock:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} hp_peri1_pms_hp_ledc1_ctrl_reg_t;

/** Type of hp_pcnt1_ctrl register
 *  hp_pcnt1 read/write control register
 */
typedef union {
    struct {
        /** read_tee_hp_pcnt1 : R/W; bitpos: [0]; default: 1;
         *  Configures hp_pcnt1 registers read permission in tee mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_tee_hp_pcnt1:1;
        /** read_ree0_hp_pcnt1 : R/W; bitpos: [1]; default: 0;
         *  Configures hp_pcnt1 registers read permission in ree0 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree0_hp_pcnt1:1;
        /** read_ree1_hp_pcnt1 : R/W; bitpos: [2]; default: 0;
         *  Configures hp_pcnt1 registers read permission in ree1 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree1_hp_pcnt1:1;
        /** read_ree2_hp_pcnt1 : R/W; bitpos: [3]; default: 0;
         *  Configures hp_pcnt1 registers read permission in ree2 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree2_hp_pcnt1:1;
        /** write_tee_hp_pcnt1 : R/W; bitpos: [4]; default: 1;
         *  Configures hp_pcnt1 registers write permission in tee mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_tee_hp_pcnt1:1;
        /** write_ree0_hp_pcnt1 : R/W; bitpos: [5]; default: 0;
         *  Configures hp_pcnt1 registers write permission in ree0 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree0_hp_pcnt1:1;
        /** write_ree1_hp_pcnt1 : R/W; bitpos: [6]; default: 0;
         *  Configures hp_pcnt1 registers write permission in ree1 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree1_hp_pcnt1:1;
        /** write_ree2_hp_pcnt1 : R/W; bitpos: [7]; default: 0;
         *  Configures hp_pcnt1 registers write permission in ree2 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree2_hp_pcnt1:1;
        /** hp_pcnt1_lock : R/W; bitpos: [8]; default: 0;
         *  Set 1 to lock hp_pcnt1 peri_apm configuration
         */
        uint32_t hp_pcnt1_lock:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} hp_peri1_pms_hp_pcnt1_ctrl_reg_t;

/** Type of hp_timer_group0_ctrl register
 *  hp_timer_group0 read/write control register
 */
typedef union {
    struct {
        /** read_tee_hp_timer_group0 : R/W; bitpos: [0]; default: 1;
         *  Configures hp_timer_group0 registers read permission in tee mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_tee_hp_timer_group0:1;
        /** read_ree0_hp_timer_group0 : R/W; bitpos: [1]; default: 0;
         *  Configures hp_timer_group0 registers read permission in ree0 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree0_hp_timer_group0:1;
        /** read_ree1_hp_timer_group0 : R/W; bitpos: [2]; default: 0;
         *  Configures hp_timer_group0 registers read permission in ree1 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree1_hp_timer_group0:1;
        /** read_ree2_hp_timer_group0 : R/W; bitpos: [3]; default: 0;
         *  Configures hp_timer_group0 registers read permission in ree2 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree2_hp_timer_group0:1;
        /** write_tee_hp_timer_group0 : R/W; bitpos: [4]; default: 1;
         *  Configures hp_timer_group0 registers write permission in tee mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_tee_hp_timer_group0:1;
        /** write_ree0_hp_timer_group0 : R/W; bitpos: [5]; default: 0;
         *  Configures hp_timer_group0 registers write permission in ree0 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree0_hp_timer_group0:1;
        /** write_ree1_hp_timer_group0 : R/W; bitpos: [6]; default: 0;
         *  Configures hp_timer_group0 registers write permission in ree1 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree1_hp_timer_group0:1;
        /** write_ree2_hp_timer_group0 : R/W; bitpos: [7]; default: 0;
         *  Configures hp_timer_group0 registers write permission in ree2 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree2_hp_timer_group0:1;
        /** hp_timer_group0_lock : R/W; bitpos: [8]; default: 0;
         *  Set 1 to lock hp_timer_group0 peri_apm configuration
         */
        uint32_t hp_timer_group0_lock:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} hp_peri1_pms_hp_timer_group0_ctrl_reg_t;

/** Type of hp_timer_group1_ctrl register
 *  hp_timer_group1 read/write control register
 */
typedef union {
    struct {
        /** read_tee_hp_timer_group1 : R/W; bitpos: [0]; default: 1;
         *  Configures hp_timer_group1 registers read permission in tee mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_tee_hp_timer_group1:1;
        /** read_ree0_hp_timer_group1 : R/W; bitpos: [1]; default: 0;
         *  Configures hp_timer_group1 registers read permission in ree0 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree0_hp_timer_group1:1;
        /** read_ree1_hp_timer_group1 : R/W; bitpos: [2]; default: 0;
         *  Configures hp_timer_group1 registers read permission in ree1 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree1_hp_timer_group1:1;
        /** read_ree2_hp_timer_group1 : R/W; bitpos: [3]; default: 0;
         *  Configures hp_timer_group1 registers read permission in ree2 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree2_hp_timer_group1:1;
        /** write_tee_hp_timer_group1 : R/W; bitpos: [4]; default: 1;
         *  Configures hp_timer_group1 registers write permission in tee mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_tee_hp_timer_group1:1;
        /** write_ree0_hp_timer_group1 : R/W; bitpos: [5]; default: 0;
         *  Configures hp_timer_group1 registers write permission in ree0 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree0_hp_timer_group1:1;
        /** write_ree1_hp_timer_group1 : R/W; bitpos: [6]; default: 0;
         *  Configures hp_timer_group1 registers write permission in ree1 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree1_hp_timer_group1:1;
        /** write_ree2_hp_timer_group1 : R/W; bitpos: [7]; default: 0;
         *  Configures hp_timer_group1 registers write permission in ree2 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree2_hp_timer_group1:1;
        /** hp_timer_group1_lock : R/W; bitpos: [8]; default: 0;
         *  Set 1 to lock hp_timer_group1 peri_apm configuration
         */
        uint32_t hp_timer_group1_lock:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} hp_peri1_pms_hp_timer_group1_ctrl_reg_t;

/** Type of hp_iomux_ctrl register
 *  hp_iomux read/write control register
 */
typedef union {
    struct {
        /** read_tee_hp_iomux : R/W; bitpos: [0]; default: 1;
         *  Configures hp_iomux registers read permission in tee mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_tee_hp_iomux:1;
        /** read_ree0_hp_iomux : R/W; bitpos: [1]; default: 0;
         *  Configures hp_iomux registers read permission in ree0 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree0_hp_iomux:1;
        /** read_ree1_hp_iomux : R/W; bitpos: [2]; default: 0;
         *  Configures hp_iomux registers read permission in ree1 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree1_hp_iomux:1;
        /** read_ree2_hp_iomux : R/W; bitpos: [3]; default: 0;
         *  Configures hp_iomux registers read permission in ree2 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree2_hp_iomux:1;
        /** write_tee_hp_iomux : R/W; bitpos: [4]; default: 1;
         *  Configures hp_iomux registers write permission in tee mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_tee_hp_iomux:1;
        /** write_ree0_hp_iomux : R/W; bitpos: [5]; default: 0;
         *  Configures hp_iomux registers write permission in ree0 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree0_hp_iomux:1;
        /** write_ree1_hp_iomux : R/W; bitpos: [6]; default: 0;
         *  Configures hp_iomux registers write permission in ree1 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree1_hp_iomux:1;
        /** write_ree2_hp_iomux : R/W; bitpos: [7]; default: 0;
         *  Configures hp_iomux registers write permission in ree2 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree2_hp_iomux:1;
        /** hp_iomux_lock : R/W; bitpos: [8]; default: 0;
         *  Set 1 to lock hp_iomux peri_apm configuration
         */
        uint32_t hp_iomux_lock:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} hp_peri1_pms_hp_iomux_ctrl_reg_t;

/** Type of hp_mspi_padctrl_ctrl register
 *  hp_mspi_padctrl read/write control register
 */
typedef union {
    struct {
        /** read_tee_hp_mspi_padctrl : R/W; bitpos: [0]; default: 1;
         *  Configures hp_mspi_padctrl registers read permission in tee mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_tee_hp_mspi_padctrl:1;
        /** read_ree0_hp_mspi_padctrl : R/W; bitpos: [1]; default: 0;
         *  Configures hp_mspi_padctrl registers read permission in ree0 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree0_hp_mspi_padctrl:1;
        /** read_ree1_hp_mspi_padctrl : R/W; bitpos: [2]; default: 0;
         *  Configures hp_mspi_padctrl registers read permission in ree1 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree1_hp_mspi_padctrl:1;
        /** read_ree2_hp_mspi_padctrl : R/W; bitpos: [3]; default: 0;
         *  Configures hp_mspi_padctrl registers read permission in ree2 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree2_hp_mspi_padctrl:1;
        /** write_tee_hp_mspi_padctrl : R/W; bitpos: [4]; default: 1;
         *  Configures hp_mspi_padctrl registers write permission in tee mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_tee_hp_mspi_padctrl:1;
        /** write_ree0_hp_mspi_padctrl : R/W; bitpos: [5]; default: 0;
         *  Configures hp_mspi_padctrl registers write permission in ree0 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree0_hp_mspi_padctrl:1;
        /** write_ree1_hp_mspi_padctrl : R/W; bitpos: [6]; default: 0;
         *  Configures hp_mspi_padctrl registers write permission in ree1 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree1_hp_mspi_padctrl:1;
        /** write_ree2_hp_mspi_padctrl : R/W; bitpos: [7]; default: 0;
         *  Configures hp_mspi_padctrl registers write permission in ree2 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree2_hp_mspi_padctrl:1;
        /** hp_mspi_padctrl_lock : R/W; bitpos: [8]; default: 0;
         *  Set 1 to lock hp_mspi_padctrl peri_apm configuration
         */
        uint32_t hp_mspi_padctrl_lock:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} hp_peri1_pms_hp_mspi_padctrl_ctrl_reg_t;

/** Type of hp_intrmtx_ctrl register
 *  hp_intrmtx read/write control register
 */
typedef union {
    struct {
        /** read_tee_hp_intrmtx : R/W; bitpos: [0]; default: 1;
         *  Configures hp_intrmtx registers read permission in tee mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_tee_hp_intrmtx:1;
        /** read_ree0_hp_intrmtx : R/W; bitpos: [1]; default: 0;
         *  Configures hp_intrmtx registers read permission in ree0 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree0_hp_intrmtx:1;
        /** read_ree1_hp_intrmtx : R/W; bitpos: [2]; default: 0;
         *  Configures hp_intrmtx registers read permission in ree1 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree1_hp_intrmtx:1;
        /** read_ree2_hp_intrmtx : R/W; bitpos: [3]; default: 0;
         *  Configures hp_intrmtx registers read permission in ree2 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree2_hp_intrmtx:1;
        /** write_tee_hp_intrmtx : R/W; bitpos: [4]; default: 1;
         *  Configures hp_intrmtx registers write permission in tee mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_tee_hp_intrmtx:1;
        /** write_ree0_hp_intrmtx : R/W; bitpos: [5]; default: 0;
         *  Configures hp_intrmtx registers write permission in ree0 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree0_hp_intrmtx:1;
        /** write_ree1_hp_intrmtx : R/W; bitpos: [6]; default: 0;
         *  Configures hp_intrmtx registers write permission in ree1 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree1_hp_intrmtx:1;
        /** write_ree2_hp_intrmtx : R/W; bitpos: [7]; default: 0;
         *  Configures hp_intrmtx registers write permission in ree2 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree2_hp_intrmtx:1;
        /** hp_intrmtx_lock : R/W; bitpos: [8]; default: 0;
         *  Set 1 to lock hp_intrmtx peri_apm configuration
         */
        uint32_t hp_intrmtx_lock:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} hp_peri1_pms_hp_intrmtx_ctrl_reg_t;

/** Type of hp_sys_reg_ctrl register
 *  hp_sys_reg read/write control register
 */
typedef union {
    struct {
        /** read_tee_hp_sys_reg : R/W; bitpos: [0]; default: 1;
         *  Configures hp_sys_reg registers read permission in tee mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_tee_hp_sys_reg:1;
        /** read_ree0_hp_sys_reg : R/W; bitpos: [1]; default: 0;
         *  Configures hp_sys_reg registers read permission in ree0 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree0_hp_sys_reg:1;
        /** read_ree1_hp_sys_reg : R/W; bitpos: [2]; default: 0;
         *  Configures hp_sys_reg registers read permission in ree1 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree1_hp_sys_reg:1;
        /** read_ree2_hp_sys_reg : R/W; bitpos: [3]; default: 0;
         *  Configures hp_sys_reg registers read permission in ree2 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree2_hp_sys_reg:1;
        /** write_tee_hp_sys_reg : R/W; bitpos: [4]; default: 1;
         *  Configures hp_sys_reg registers write permission in tee mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_tee_hp_sys_reg:1;
        /** write_ree0_hp_sys_reg : R/W; bitpos: [5]; default: 0;
         *  Configures hp_sys_reg registers write permission in ree0 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree0_hp_sys_reg:1;
        /** write_ree1_hp_sys_reg : R/W; bitpos: [6]; default: 0;
         *  Configures hp_sys_reg registers write permission in ree1 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree1_hp_sys_reg:1;
        /** write_ree2_hp_sys_reg : R/W; bitpos: [7]; default: 0;
         *  Configures hp_sys_reg registers write permission in ree2 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree2_hp_sys_reg:1;
        /** hp_sys_reg_lock : R/W; bitpos: [8]; default: 0;
         *  Set 1 to lock hp_sys_reg peri_apm configuration
         */
        uint32_t hp_sys_reg_lock:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} hp_peri1_pms_hp_sys_reg_ctrl_reg_t;

/** Type of hp_clkrst_ctrl register
 *  hp_clkrst read/write control register
 */
typedef union {
    struct {
        /** read_tee_hp_clkrst : R/W; bitpos: [0]; default: 1;
         *  Configures hp_clkrst registers read permission in tee mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_tee_hp_clkrst:1;
        /** read_ree0_hp_clkrst : R/W; bitpos: [1]; default: 0;
         *  Configures hp_clkrst registers read permission in ree0 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree0_hp_clkrst:1;
        /** read_ree1_hp_clkrst : R/W; bitpos: [2]; default: 0;
         *  Configures hp_clkrst registers read permission in ree1 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree1_hp_clkrst:1;
        /** read_ree2_hp_clkrst : R/W; bitpos: [3]; default: 0;
         *  Configures hp_clkrst registers read permission in ree2 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree2_hp_clkrst:1;
        /** write_tee_hp_clkrst : R/W; bitpos: [4]; default: 1;
         *  Configures hp_clkrst registers write permission in tee mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_tee_hp_clkrst:1;
        /** write_ree0_hp_clkrst : R/W; bitpos: [5]; default: 0;
         *  Configures hp_clkrst registers write permission in ree0 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree0_hp_clkrst:1;
        /** write_ree1_hp_clkrst : R/W; bitpos: [6]; default: 0;
         *  Configures hp_clkrst registers write permission in ree1 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree1_hp_clkrst:1;
        /** write_ree2_hp_clkrst : R/W; bitpos: [7]; default: 0;
         *  Configures hp_clkrst registers write permission in ree2 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree2_hp_clkrst:1;
        /** hp_clkrst_lock : R/W; bitpos: [8]; default: 0;
         *  Set 1 to lock hp_clkrst peri_apm configuration
         */
        uint32_t hp_clkrst_lock:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} hp_peri1_pms_hp_clkrst_ctrl_reg_t;

/** Type of cnnt_pad_ctrl_ctrl register
 *  cnnt_pad_ctrl read/write control register
 */
typedef union {
    struct {
        /** read_tee_cnnt_pad_ctrl : R/W; bitpos: [0]; default: 1;
         *  Configures cnnt_pad_ctrl registers read permission in tee mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_tee_cnnt_pad_ctrl:1;
        /** read_ree0_cnnt_pad_ctrl : R/W; bitpos: [1]; default: 0;
         *  Configures cnnt_pad_ctrl registers read permission in ree0 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree0_cnnt_pad_ctrl:1;
        /** read_ree1_cnnt_pad_ctrl : R/W; bitpos: [2]; default: 0;
         *  Configures cnnt_pad_ctrl registers read permission in ree1 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree1_cnnt_pad_ctrl:1;
        /** read_ree2_cnnt_pad_ctrl : R/W; bitpos: [3]; default: 0;
         *  Configures cnnt_pad_ctrl registers read permission in ree2 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree2_cnnt_pad_ctrl:1;
        /** write_tee_cnnt_pad_ctrl : R/W; bitpos: [4]; default: 1;
         *  Configures cnnt_pad_ctrl registers write permission in tee mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_tee_cnnt_pad_ctrl:1;
        /** write_ree0_cnnt_pad_ctrl : R/W; bitpos: [5]; default: 0;
         *  Configures cnnt_pad_ctrl registers write permission in ree0 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree0_cnnt_pad_ctrl:1;
        /** write_ree1_cnnt_pad_ctrl : R/W; bitpos: [6]; default: 0;
         *  Configures cnnt_pad_ctrl registers write permission in ree1 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree1_cnnt_pad_ctrl:1;
        /** write_ree2_cnnt_pad_ctrl : R/W; bitpos: [7]; default: 0;
         *  Configures cnnt_pad_ctrl registers write permission in ree2 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree2_cnnt_pad_ctrl:1;
        /** cnnt_pad_ctrl_lock : R/W; bitpos: [8]; default: 0;
         *  Set 1 to lock cnnt_pad_ctrl peri_apm configuration
         */
        uint32_t cnnt_pad_ctrl_lock:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} hp_peri1_pms_cnnt_pad_ctrl_ctrl_reg_t;

/** Type of hp_alive_sys_reg_ctrl register
 *  hp_alive_sys_reg read/write control register
 */
typedef union {
    struct {
        /** read_tee_hp_alive_sys_reg : R/W; bitpos: [0]; default: 1;
         *  Configures hp_alive_sys_reg registers read permission in tee mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_tee_hp_alive_sys_reg:1;
        /** read_ree0_hp_alive_sys_reg : R/W; bitpos: [1]; default: 0;
         *  Configures hp_alive_sys_reg registers read permission in ree0 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree0_hp_alive_sys_reg:1;
        /** read_ree1_hp_alive_sys_reg : R/W; bitpos: [2]; default: 0;
         *  Configures hp_alive_sys_reg registers read permission in ree1 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree1_hp_alive_sys_reg:1;
        /** read_ree2_hp_alive_sys_reg : R/W; bitpos: [3]; default: 0;
         *  Configures hp_alive_sys_reg registers read permission in ree2 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree2_hp_alive_sys_reg:1;
        /** write_tee_hp_alive_sys_reg : R/W; bitpos: [4]; default: 1;
         *  Configures hp_alive_sys_reg registers write permission in tee mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_tee_hp_alive_sys_reg:1;
        /** write_ree0_hp_alive_sys_reg : R/W; bitpos: [5]; default: 0;
         *  Configures hp_alive_sys_reg registers write permission in ree0 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree0_hp_alive_sys_reg:1;
        /** write_ree1_hp_alive_sys_reg : R/W; bitpos: [6]; default: 0;
         *  Configures hp_alive_sys_reg registers write permission in ree1 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree1_hp_alive_sys_reg:1;
        /** write_ree2_hp_alive_sys_reg : R/W; bitpos: [7]; default: 0;
         *  Configures hp_alive_sys_reg registers write permission in ree2 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree2_hp_alive_sys_reg:1;
        /** hp_alive_sys_reg_lock : R/W; bitpos: [8]; default: 0;
         *  Set 1 to lock hp_alive_sys_reg peri_apm configuration
         */
        uint32_t hp_alive_sys_reg_lock:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} hp_peri1_pms_hp_alive_sys_reg_ctrl_reg_t;

/** Type of hp_peri1_pms_ctrl register
 *  hp_peri1_pms read/write control register
 */
typedef union {
    struct {
        /** read_tee_hp_peri1_pms : R/W; bitpos: [0]; default: 1;
         *  Configures hp_peri1_pms registers read permission in tee mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_tee_hp_peri1_pms:1;
        /** read_ree0_hp_peri1_pms : HRO; bitpos: [1]; default: 0;
         *  Configures hp_peri1_pms registers read permission in ree0 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree0_hp_peri1_pms:1;
        /** read_ree1_hp_peri1_pms : HRO; bitpos: [2]; default: 0;
         *  Configures hp_peri1_pms registers read permission in ree1 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree1_hp_peri1_pms:1;
        /** read_ree2_hp_peri1_pms : HRO; bitpos: [3]; default: 0;
         *  Configures hp_peri1_pms registers read permission in ree2 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree2_hp_peri1_pms:1;
        /** write_tee_hp_peri1_pms : R/W; bitpos: [4]; default: 1;
         *  Configures hp_peri1_pms registers write permission in tee mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_tee_hp_peri1_pms:1;
        /** write_ree0_hp_peri1_pms : HRO; bitpos: [5]; default: 0;
         *  Configures hp_peri1_pms registers write permission in ree0 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree0_hp_peri1_pms:1;
        /** write_ree1_hp_peri1_pms : HRO; bitpos: [6]; default: 0;
         *  Configures hp_peri1_pms registers write permission in ree1 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree1_hp_peri1_pms:1;
        /** write_ree2_hp_peri1_pms : HRO; bitpos: [7]; default: 0;
         *  Configures hp_peri1_pms registers write permission in ree2 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree2_hp_peri1_pms:1;
        /** hp_peri1_pms_lock : R/W; bitpos: [8]; default: 0;
         *  Set 1 to lock hp_peri1_pms peri_apm configuration
         */
        uint32_t hp_peri1_pms_lock:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} hp_peri1_pms_hp_peri1_pms_ctrl_reg_t;


/** Group: PMS Register */
/** Type of hp_peri1_0 register
 *  HP_PERI1 PMS configuration & info register
 */
typedef union {
    struct {
        /** hp_peri1_pms_exception_clr : WT; bitpos: [0]; default: 0;
         *  Configures whether or not to clear hp_peri1 peri_pms_record_reg.
         *  0: No clear
         *  1: Clear peri_pms_record_reg
         */
        uint32_t hp_peri1_pms_exception_clr:1;
        /** hp_peri1_pms_exception_det : RO; bitpos: [1]; default: 0;
         *  Represents whether the hp_peri1 pms has been triggered.
         *  0: No triggered
         *  1: Has been triggered
         */
        uint32_t hp_peri1_pms_exception_det:1;
        /** hp_peri1_pms_exception_id : RO; bitpos: [7:2]; default: 0;
         *  Represents the master id when hp_peri1 pms has been triggered.
         */
        uint32_t hp_peri1_pms_exception_id:6;
        /** hp_peri1_pms_exception_mode : RO; bitpos: [9:8]; default: 0;
         *  Represents the security mode when hp_peri1 pms has been triggered.
         */
        uint32_t hp_peri1_pms_exception_mode:2;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} hp_peri1_pms_hp_peri1_0_reg_t;

/** Type of hp_peri1_1 register
 *  HP_PERI1 PMS exception addr record register
 */
typedef union {
    struct {
        /** hp_peri1_pms_exception_addr : RO; bitpos: [27:0]; default: 0;
         *  Represents the access address (bit27~bit0) when hp_peri1 pms has been triggered.
         */
        uint32_t hp_peri1_pms_exception_addr:28;
        /** hp_peri1_pms_exception_addr_const : HRO; bitpos: [31:28]; default: 2;
         *  Represents the access address (bit31~bit28) when hp_peri1 pms has been triggered.
         */
        uint32_t hp_peri1_pms_exception_addr_const:4;
    };
    uint32_t val;
} hp_peri1_pms_hp_peri1_1_reg_t;

/** Type of int_en register
 *  APM interrupt enable register
 */
typedef union {
    struct {
        /** hp_peri1_pms_int_en : R/W; bitpos: [0]; default: 0;
         *  Configures to enable hp_peri1 pms interrupt.
         *  0: disable
         *  1: enable
         */
        uint32_t hp_peri1_pms_int_en:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} hp_peri1_pms_int_en_reg_t;


/** Group: config register */
/** Type of bus_err_conf register
 *  Clock gating register
 */
typedef union {
    struct {
        /** bus_err_resp_en : R/W; bitpos: [0]; default: 0;
         *  Configures whether return error response to cpu when access blocked
         *  0: disable error response
         *  1: enable error response
         */
        uint32_t bus_err_resp_en:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} hp_peri1_pms_bus_err_conf_reg_t;


/** Group: clock gating register */
/** Type of clock_gate register
 *  Clock gating register
 */
typedef union {
    struct {
        /** clk_en : R/W; bitpos: [0]; default: 1;
         *  Configures whether to keep the clock always on.
         *  0: enable automatic clock gating
         *  1: keep the clock always on
         */
        uint32_t clk_en:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} hp_peri1_pms_clock_gate_reg_t;


/** Group: Version control register */
/** Type of date register
 *  Version control register
 */
typedef union {
    struct {
        /** date : R/W; bitpos: [27:0]; default: 38834720;
         *  Version control register
         */
        uint32_t date:28;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} hp_peri1_pms_date_reg_t;


typedef struct {
    volatile hp_peri1_pms_hp_usbotg_phy_ctrl_reg_t hp_usbotg_phy_ctrl;
    volatile hp_peri1_pms_hp_mcpwm0_ctrl_reg_t hp_mcpwm0_ctrl;
    volatile hp_peri1_pms_hp_mcpwm1_ctrl_reg_t hp_mcpwm1_ctrl;
    volatile hp_peri1_pms_hp_mcpwm2_ctrl_reg_t hp_mcpwm2_ctrl;
    volatile hp_peri1_pms_hp_mcpwm3_ctrl_reg_t hp_mcpwm3_ctrl;
    volatile hp_peri1_pms_hp_i2c0_ctrl_reg_t hp_i2c0_ctrl;
    volatile hp_peri1_pms_hp_i2c1_ctrl_reg_t hp_i2c1_ctrl;
    volatile hp_peri1_pms_hp_i2s0_ctrl_reg_t hp_i2s0_ctrl;
    volatile hp_peri1_pms_hp_i2s1_ctrl_reg_t hp_i2s1_ctrl;
    volatile hp_peri1_pms_hp_pcnt0_ctrl_reg_t hp_pcnt0_ctrl;
    volatile hp_peri1_pms_hp_uart0_ctrl_reg_t hp_uart0_ctrl;
    volatile hp_peri1_pms_hp_uart1_ctrl_reg_t hp_uart1_ctrl;
    volatile hp_peri1_pms_hp_uart2_ctrl_reg_t hp_uart2_ctrl;
    volatile hp_peri1_pms_hp_uart3_ctrl_reg_t hp_uart3_ctrl;
    volatile hp_peri1_pms_hp_parlio_ctrl_reg_t hp_parlio_ctrl;
    volatile hp_peri1_pms_hp_gpspi2_ctrl_reg_t hp_gpspi2_ctrl;
    volatile hp_peri1_pms_hp_gpspi3_ctrl_reg_t hp_gpspi3_ctrl;
    volatile hp_peri1_pms_hp_usbdevice_ctrl_reg_t hp_usbdevice_ctrl;
    volatile hp_peri1_pms_hp_ledc0_ctrl_reg_t hp_ledc0_ctrl;
    volatile hp_peri1_pms_hp_etm_ctrl_reg_t hp_etm_ctrl;
    volatile hp_peri1_pms_hp_twai0_ctrl_reg_t hp_twai0_ctrl;
    volatile hp_peri1_pms_hp_twai1_ctrl_reg_t hp_twai1_ctrl;
    volatile hp_peri1_pms_hp_lcdcam_ctrl_reg_t hp_lcdcam_ctrl;
    volatile hp_peri1_pms_hp_uhci_ctrl_reg_t hp_uhci_ctrl;
    volatile hp_peri1_pms_hp_systimer_ctrl_reg_t hp_systimer_ctrl;
    volatile hp_peri1_pms_hp_zero_det_ctrl_reg_t hp_zero_det_ctrl;
    volatile hp_peri1_pms_hp_cordic_ctrl_reg_t hp_cordic_ctrl;
    volatile hp_peri1_pms_hp_ledc1_ctrl_reg_t hp_ledc1_ctrl;
    volatile hp_peri1_pms_hp_pcnt1_ctrl_reg_t hp_pcnt1_ctrl;
    volatile hp_peri1_pms_hp_timer_group0_ctrl_reg_t hp_timer_group0_ctrl;
    volatile hp_peri1_pms_hp_timer_group1_ctrl_reg_t hp_timer_group1_ctrl;
    volatile hp_peri1_pms_hp_iomux_ctrl_reg_t hp_iomux_ctrl;
    volatile hp_peri1_pms_hp_mspi_padctrl_ctrl_reg_t hp_mspi_padctrl_ctrl;
    volatile hp_peri1_pms_hp_intrmtx_ctrl_reg_t hp_intrmtx_ctrl;
    volatile hp_peri1_pms_hp_sys_reg_ctrl_reg_t hp_sys_reg_ctrl;
    volatile hp_peri1_pms_hp_clkrst_ctrl_reg_t hp_clkrst_ctrl;
    volatile hp_peri1_pms_cnnt_pad_ctrl_ctrl_reg_t cnnt_pad_ctrl_ctrl;
    volatile hp_peri1_pms_hp_alive_sys_reg_ctrl_reg_t hp_alive_sys_reg_ctrl;
    volatile hp_peri1_pms_hp_peri1_pms_ctrl_reg_t hp_peri1_pms_ctrl;
    uint32_t reserved_09c[89];
    volatile hp_peri1_pms_hp_peri1_0_reg_t hp_peri1_0;
    volatile hp_peri1_pms_hp_peri1_1_reg_t hp_peri1_1;
    uint32_t reserved_208[62];
    volatile hp_peri1_pms_int_en_reg_t int_en;
    uint32_t reserved_304[827];
    volatile hp_peri1_pms_bus_err_conf_reg_t bus_err_conf;
    uint32_t reserved_ff4;
    volatile hp_peri1_pms_clock_gate_reg_t clock_gate;
    volatile hp_peri1_pms_date_reg_t date;
} hp_peri1_pms_dev_t;


#ifndef __cplusplus
_Static_assert(sizeof(hp_peri1_pms_dev_t) == 0x1000, "Invalid size of hp_peri1_pms_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
