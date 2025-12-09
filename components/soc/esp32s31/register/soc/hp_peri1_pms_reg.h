/**
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 *  SPDX-License-Identifier: Apache-2.0 OR MIT
 */
#pragma once

#include <stdint.h>
#include "soc/soc.h"
#ifdef __cplusplus
extern "C" {
#endif

/** HP_PERI1_PMS_HP_USBOTG_PHY_CTRL_REG register
 *  hp_usbotg_phy read/write control register
 */
#define HP_PERI1_PMS_HP_USBOTG_PHY_CTRL_REG (DR_REG_HP_PERI1_PMS_BASE + 0x0)
/** HP_PERI1_PMS_READ_TEE_HP_USBOTG_PHY : R/W; bitpos: [0]; default: 1;
 *  Configures hp_usbotg_phy registers read permission in tee mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI1_PMS_READ_TEE_HP_USBOTG_PHY    (BIT(0))
#define HP_PERI1_PMS_READ_TEE_HP_USBOTG_PHY_M  (HP_PERI1_PMS_READ_TEE_HP_USBOTG_PHY_V << HP_PERI1_PMS_READ_TEE_HP_USBOTG_PHY_S)
#define HP_PERI1_PMS_READ_TEE_HP_USBOTG_PHY_V  0x00000001U
#define HP_PERI1_PMS_READ_TEE_HP_USBOTG_PHY_S  0
/** HP_PERI1_PMS_READ_REE0_HP_USBOTG_PHY : R/W; bitpos: [1]; default: 0;
 *  Configures hp_usbotg_phy registers read permission in ree0 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI1_PMS_READ_REE0_HP_USBOTG_PHY    (BIT(1))
#define HP_PERI1_PMS_READ_REE0_HP_USBOTG_PHY_M  (HP_PERI1_PMS_READ_REE0_HP_USBOTG_PHY_V << HP_PERI1_PMS_READ_REE0_HP_USBOTG_PHY_S)
#define HP_PERI1_PMS_READ_REE0_HP_USBOTG_PHY_V  0x00000001U
#define HP_PERI1_PMS_READ_REE0_HP_USBOTG_PHY_S  1
/** HP_PERI1_PMS_READ_REE1_HP_USBOTG_PHY : R/W; bitpos: [2]; default: 0;
 *  Configures hp_usbotg_phy registers read permission in ree1 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI1_PMS_READ_REE1_HP_USBOTG_PHY    (BIT(2))
#define HP_PERI1_PMS_READ_REE1_HP_USBOTG_PHY_M  (HP_PERI1_PMS_READ_REE1_HP_USBOTG_PHY_V << HP_PERI1_PMS_READ_REE1_HP_USBOTG_PHY_S)
#define HP_PERI1_PMS_READ_REE1_HP_USBOTG_PHY_V  0x00000001U
#define HP_PERI1_PMS_READ_REE1_HP_USBOTG_PHY_S  2
/** HP_PERI1_PMS_READ_REE2_HP_USBOTG_PHY : R/W; bitpos: [3]; default: 0;
 *  Configures hp_usbotg_phy registers read permission in ree2 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI1_PMS_READ_REE2_HP_USBOTG_PHY    (BIT(3))
#define HP_PERI1_PMS_READ_REE2_HP_USBOTG_PHY_M  (HP_PERI1_PMS_READ_REE2_HP_USBOTG_PHY_V << HP_PERI1_PMS_READ_REE2_HP_USBOTG_PHY_S)
#define HP_PERI1_PMS_READ_REE2_HP_USBOTG_PHY_V  0x00000001U
#define HP_PERI1_PMS_READ_REE2_HP_USBOTG_PHY_S  3
/** HP_PERI1_PMS_WRITE_TEE_HP_USBOTG_PHY : R/W; bitpos: [4]; default: 1;
 *  Configures hp_usbotg_phy registers write permission in tee mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI1_PMS_WRITE_TEE_HP_USBOTG_PHY    (BIT(4))
#define HP_PERI1_PMS_WRITE_TEE_HP_USBOTG_PHY_M  (HP_PERI1_PMS_WRITE_TEE_HP_USBOTG_PHY_V << HP_PERI1_PMS_WRITE_TEE_HP_USBOTG_PHY_S)
#define HP_PERI1_PMS_WRITE_TEE_HP_USBOTG_PHY_V  0x00000001U
#define HP_PERI1_PMS_WRITE_TEE_HP_USBOTG_PHY_S  4
/** HP_PERI1_PMS_WRITE_REE0_HP_USBOTG_PHY : R/W; bitpos: [5]; default: 0;
 *  Configures hp_usbotg_phy registers write permission in ree0 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI1_PMS_WRITE_REE0_HP_USBOTG_PHY    (BIT(5))
#define HP_PERI1_PMS_WRITE_REE0_HP_USBOTG_PHY_M  (HP_PERI1_PMS_WRITE_REE0_HP_USBOTG_PHY_V << HP_PERI1_PMS_WRITE_REE0_HP_USBOTG_PHY_S)
#define HP_PERI1_PMS_WRITE_REE0_HP_USBOTG_PHY_V  0x00000001U
#define HP_PERI1_PMS_WRITE_REE0_HP_USBOTG_PHY_S  5
/** HP_PERI1_PMS_WRITE_REE1_HP_USBOTG_PHY : R/W; bitpos: [6]; default: 0;
 *  Configures hp_usbotg_phy registers write permission in ree1 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI1_PMS_WRITE_REE1_HP_USBOTG_PHY    (BIT(6))
#define HP_PERI1_PMS_WRITE_REE1_HP_USBOTG_PHY_M  (HP_PERI1_PMS_WRITE_REE1_HP_USBOTG_PHY_V << HP_PERI1_PMS_WRITE_REE1_HP_USBOTG_PHY_S)
#define HP_PERI1_PMS_WRITE_REE1_HP_USBOTG_PHY_V  0x00000001U
#define HP_PERI1_PMS_WRITE_REE1_HP_USBOTG_PHY_S  6
/** HP_PERI1_PMS_WRITE_REE2_HP_USBOTG_PHY : R/W; bitpos: [7]; default: 0;
 *  Configures hp_usbotg_phy registers write permission in ree2 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI1_PMS_WRITE_REE2_HP_USBOTG_PHY    (BIT(7))
#define HP_PERI1_PMS_WRITE_REE2_HP_USBOTG_PHY_M  (HP_PERI1_PMS_WRITE_REE2_HP_USBOTG_PHY_V << HP_PERI1_PMS_WRITE_REE2_HP_USBOTG_PHY_S)
#define HP_PERI1_PMS_WRITE_REE2_HP_USBOTG_PHY_V  0x00000001U
#define HP_PERI1_PMS_WRITE_REE2_HP_USBOTG_PHY_S  7
/** HP_PERI1_PMS_HP_USBOTG_PHY_LOCK : R/W; bitpos: [8]; default: 0;
 *  Set 1 to lock hp_usbotg_phy peri_apm configuration
 */
#define HP_PERI1_PMS_HP_USBOTG_PHY_LOCK    (BIT(8))
#define HP_PERI1_PMS_HP_USBOTG_PHY_LOCK_M  (HP_PERI1_PMS_HP_USBOTG_PHY_LOCK_V << HP_PERI1_PMS_HP_USBOTG_PHY_LOCK_S)
#define HP_PERI1_PMS_HP_USBOTG_PHY_LOCK_V  0x00000001U
#define HP_PERI1_PMS_HP_USBOTG_PHY_LOCK_S  8

/** HP_PERI1_PMS_HP_MCPWM0_CTRL_REG register
 *  hp_mcpwm0 read/write control register
 */
#define HP_PERI1_PMS_HP_MCPWM0_CTRL_REG (DR_REG_HP_PERI1_PMS_BASE + 0x4)
/** HP_PERI1_PMS_READ_TEE_HP_MCPWM0 : R/W; bitpos: [0]; default: 1;
 *  Configures hp_mcpwm0 registers read permission in tee mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI1_PMS_READ_TEE_HP_MCPWM0    (BIT(0))
#define HP_PERI1_PMS_READ_TEE_HP_MCPWM0_M  (HP_PERI1_PMS_READ_TEE_HP_MCPWM0_V << HP_PERI1_PMS_READ_TEE_HP_MCPWM0_S)
#define HP_PERI1_PMS_READ_TEE_HP_MCPWM0_V  0x00000001U
#define HP_PERI1_PMS_READ_TEE_HP_MCPWM0_S  0
/** HP_PERI1_PMS_READ_REE0_HP_MCPWM0 : R/W; bitpos: [1]; default: 0;
 *  Configures hp_mcpwm0 registers read permission in ree0 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI1_PMS_READ_REE0_HP_MCPWM0    (BIT(1))
#define HP_PERI1_PMS_READ_REE0_HP_MCPWM0_M  (HP_PERI1_PMS_READ_REE0_HP_MCPWM0_V << HP_PERI1_PMS_READ_REE0_HP_MCPWM0_S)
#define HP_PERI1_PMS_READ_REE0_HP_MCPWM0_V  0x00000001U
#define HP_PERI1_PMS_READ_REE0_HP_MCPWM0_S  1
/** HP_PERI1_PMS_READ_REE1_HP_MCPWM0 : R/W; bitpos: [2]; default: 0;
 *  Configures hp_mcpwm0 registers read permission in ree1 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI1_PMS_READ_REE1_HP_MCPWM0    (BIT(2))
#define HP_PERI1_PMS_READ_REE1_HP_MCPWM0_M  (HP_PERI1_PMS_READ_REE1_HP_MCPWM0_V << HP_PERI1_PMS_READ_REE1_HP_MCPWM0_S)
#define HP_PERI1_PMS_READ_REE1_HP_MCPWM0_V  0x00000001U
#define HP_PERI1_PMS_READ_REE1_HP_MCPWM0_S  2
/** HP_PERI1_PMS_READ_REE2_HP_MCPWM0 : R/W; bitpos: [3]; default: 0;
 *  Configures hp_mcpwm0 registers read permission in ree2 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI1_PMS_READ_REE2_HP_MCPWM0    (BIT(3))
#define HP_PERI1_PMS_READ_REE2_HP_MCPWM0_M  (HP_PERI1_PMS_READ_REE2_HP_MCPWM0_V << HP_PERI1_PMS_READ_REE2_HP_MCPWM0_S)
#define HP_PERI1_PMS_READ_REE2_HP_MCPWM0_V  0x00000001U
#define HP_PERI1_PMS_READ_REE2_HP_MCPWM0_S  3
/** HP_PERI1_PMS_WRITE_TEE_HP_MCPWM0 : R/W; bitpos: [4]; default: 1;
 *  Configures hp_mcpwm0 registers write permission in tee mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI1_PMS_WRITE_TEE_HP_MCPWM0    (BIT(4))
#define HP_PERI1_PMS_WRITE_TEE_HP_MCPWM0_M  (HP_PERI1_PMS_WRITE_TEE_HP_MCPWM0_V << HP_PERI1_PMS_WRITE_TEE_HP_MCPWM0_S)
#define HP_PERI1_PMS_WRITE_TEE_HP_MCPWM0_V  0x00000001U
#define HP_PERI1_PMS_WRITE_TEE_HP_MCPWM0_S  4
/** HP_PERI1_PMS_WRITE_REE0_HP_MCPWM0 : R/W; bitpos: [5]; default: 0;
 *  Configures hp_mcpwm0 registers write permission in ree0 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI1_PMS_WRITE_REE0_HP_MCPWM0    (BIT(5))
#define HP_PERI1_PMS_WRITE_REE0_HP_MCPWM0_M  (HP_PERI1_PMS_WRITE_REE0_HP_MCPWM0_V << HP_PERI1_PMS_WRITE_REE0_HP_MCPWM0_S)
#define HP_PERI1_PMS_WRITE_REE0_HP_MCPWM0_V  0x00000001U
#define HP_PERI1_PMS_WRITE_REE0_HP_MCPWM0_S  5
/** HP_PERI1_PMS_WRITE_REE1_HP_MCPWM0 : R/W; bitpos: [6]; default: 0;
 *  Configures hp_mcpwm0 registers write permission in ree1 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI1_PMS_WRITE_REE1_HP_MCPWM0    (BIT(6))
#define HP_PERI1_PMS_WRITE_REE1_HP_MCPWM0_M  (HP_PERI1_PMS_WRITE_REE1_HP_MCPWM0_V << HP_PERI1_PMS_WRITE_REE1_HP_MCPWM0_S)
#define HP_PERI1_PMS_WRITE_REE1_HP_MCPWM0_V  0x00000001U
#define HP_PERI1_PMS_WRITE_REE1_HP_MCPWM0_S  6
/** HP_PERI1_PMS_WRITE_REE2_HP_MCPWM0 : R/W; bitpos: [7]; default: 0;
 *  Configures hp_mcpwm0 registers write permission in ree2 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI1_PMS_WRITE_REE2_HP_MCPWM0    (BIT(7))
#define HP_PERI1_PMS_WRITE_REE2_HP_MCPWM0_M  (HP_PERI1_PMS_WRITE_REE2_HP_MCPWM0_V << HP_PERI1_PMS_WRITE_REE2_HP_MCPWM0_S)
#define HP_PERI1_PMS_WRITE_REE2_HP_MCPWM0_V  0x00000001U
#define HP_PERI1_PMS_WRITE_REE2_HP_MCPWM0_S  7
/** HP_PERI1_PMS_HP_MCPWM0_LOCK : R/W; bitpos: [8]; default: 0;
 *  Set 1 to lock hp_mcpwm0 peri_apm configuration
 */
#define HP_PERI1_PMS_HP_MCPWM0_LOCK    (BIT(8))
#define HP_PERI1_PMS_HP_MCPWM0_LOCK_M  (HP_PERI1_PMS_HP_MCPWM0_LOCK_V << HP_PERI1_PMS_HP_MCPWM0_LOCK_S)
#define HP_PERI1_PMS_HP_MCPWM0_LOCK_V  0x00000001U
#define HP_PERI1_PMS_HP_MCPWM0_LOCK_S  8

/** HP_PERI1_PMS_HP_MCPWM1_CTRL_REG register
 *  hp_mcpwm1 read/write control register
 */
#define HP_PERI1_PMS_HP_MCPWM1_CTRL_REG (DR_REG_HP_PERI1_PMS_BASE + 0x8)
/** HP_PERI1_PMS_READ_TEE_HP_MCPWM1 : R/W; bitpos: [0]; default: 1;
 *  Configures hp_mcpwm1 registers read permission in tee mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI1_PMS_READ_TEE_HP_MCPWM1    (BIT(0))
#define HP_PERI1_PMS_READ_TEE_HP_MCPWM1_M  (HP_PERI1_PMS_READ_TEE_HP_MCPWM1_V << HP_PERI1_PMS_READ_TEE_HP_MCPWM1_S)
#define HP_PERI1_PMS_READ_TEE_HP_MCPWM1_V  0x00000001U
#define HP_PERI1_PMS_READ_TEE_HP_MCPWM1_S  0
/** HP_PERI1_PMS_READ_REE0_HP_MCPWM1 : R/W; bitpos: [1]; default: 0;
 *  Configures hp_mcpwm1 registers read permission in ree0 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI1_PMS_READ_REE0_HP_MCPWM1    (BIT(1))
#define HP_PERI1_PMS_READ_REE0_HP_MCPWM1_M  (HP_PERI1_PMS_READ_REE0_HP_MCPWM1_V << HP_PERI1_PMS_READ_REE0_HP_MCPWM1_S)
#define HP_PERI1_PMS_READ_REE0_HP_MCPWM1_V  0x00000001U
#define HP_PERI1_PMS_READ_REE0_HP_MCPWM1_S  1
/** HP_PERI1_PMS_READ_REE1_HP_MCPWM1 : R/W; bitpos: [2]; default: 0;
 *  Configures hp_mcpwm1 registers read permission in ree1 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI1_PMS_READ_REE1_HP_MCPWM1    (BIT(2))
#define HP_PERI1_PMS_READ_REE1_HP_MCPWM1_M  (HP_PERI1_PMS_READ_REE1_HP_MCPWM1_V << HP_PERI1_PMS_READ_REE1_HP_MCPWM1_S)
#define HP_PERI1_PMS_READ_REE1_HP_MCPWM1_V  0x00000001U
#define HP_PERI1_PMS_READ_REE1_HP_MCPWM1_S  2
/** HP_PERI1_PMS_READ_REE2_HP_MCPWM1 : R/W; bitpos: [3]; default: 0;
 *  Configures hp_mcpwm1 registers read permission in ree2 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI1_PMS_READ_REE2_HP_MCPWM1    (BIT(3))
#define HP_PERI1_PMS_READ_REE2_HP_MCPWM1_M  (HP_PERI1_PMS_READ_REE2_HP_MCPWM1_V << HP_PERI1_PMS_READ_REE2_HP_MCPWM1_S)
#define HP_PERI1_PMS_READ_REE2_HP_MCPWM1_V  0x00000001U
#define HP_PERI1_PMS_READ_REE2_HP_MCPWM1_S  3
/** HP_PERI1_PMS_WRITE_TEE_HP_MCPWM1 : R/W; bitpos: [4]; default: 1;
 *  Configures hp_mcpwm1 registers write permission in tee mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI1_PMS_WRITE_TEE_HP_MCPWM1    (BIT(4))
#define HP_PERI1_PMS_WRITE_TEE_HP_MCPWM1_M  (HP_PERI1_PMS_WRITE_TEE_HP_MCPWM1_V << HP_PERI1_PMS_WRITE_TEE_HP_MCPWM1_S)
#define HP_PERI1_PMS_WRITE_TEE_HP_MCPWM1_V  0x00000001U
#define HP_PERI1_PMS_WRITE_TEE_HP_MCPWM1_S  4
/** HP_PERI1_PMS_WRITE_REE0_HP_MCPWM1 : R/W; bitpos: [5]; default: 0;
 *  Configures hp_mcpwm1 registers write permission in ree0 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI1_PMS_WRITE_REE0_HP_MCPWM1    (BIT(5))
#define HP_PERI1_PMS_WRITE_REE0_HP_MCPWM1_M  (HP_PERI1_PMS_WRITE_REE0_HP_MCPWM1_V << HP_PERI1_PMS_WRITE_REE0_HP_MCPWM1_S)
#define HP_PERI1_PMS_WRITE_REE0_HP_MCPWM1_V  0x00000001U
#define HP_PERI1_PMS_WRITE_REE0_HP_MCPWM1_S  5
/** HP_PERI1_PMS_WRITE_REE1_HP_MCPWM1 : R/W; bitpos: [6]; default: 0;
 *  Configures hp_mcpwm1 registers write permission in ree1 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI1_PMS_WRITE_REE1_HP_MCPWM1    (BIT(6))
#define HP_PERI1_PMS_WRITE_REE1_HP_MCPWM1_M  (HP_PERI1_PMS_WRITE_REE1_HP_MCPWM1_V << HP_PERI1_PMS_WRITE_REE1_HP_MCPWM1_S)
#define HP_PERI1_PMS_WRITE_REE1_HP_MCPWM1_V  0x00000001U
#define HP_PERI1_PMS_WRITE_REE1_HP_MCPWM1_S  6
/** HP_PERI1_PMS_WRITE_REE2_HP_MCPWM1 : R/W; bitpos: [7]; default: 0;
 *  Configures hp_mcpwm1 registers write permission in ree2 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI1_PMS_WRITE_REE2_HP_MCPWM1    (BIT(7))
#define HP_PERI1_PMS_WRITE_REE2_HP_MCPWM1_M  (HP_PERI1_PMS_WRITE_REE2_HP_MCPWM1_V << HP_PERI1_PMS_WRITE_REE2_HP_MCPWM1_S)
#define HP_PERI1_PMS_WRITE_REE2_HP_MCPWM1_V  0x00000001U
#define HP_PERI1_PMS_WRITE_REE2_HP_MCPWM1_S  7
/** HP_PERI1_PMS_HP_MCPWM1_LOCK : R/W; bitpos: [8]; default: 0;
 *  Set 1 to lock hp_mcpwm1 peri_apm configuration
 */
#define HP_PERI1_PMS_HP_MCPWM1_LOCK    (BIT(8))
#define HP_PERI1_PMS_HP_MCPWM1_LOCK_M  (HP_PERI1_PMS_HP_MCPWM1_LOCK_V << HP_PERI1_PMS_HP_MCPWM1_LOCK_S)
#define HP_PERI1_PMS_HP_MCPWM1_LOCK_V  0x00000001U
#define HP_PERI1_PMS_HP_MCPWM1_LOCK_S  8

/** HP_PERI1_PMS_HP_MCPWM2_CTRL_REG register
 *  hp_mcpwm2 read/write control register
 */
#define HP_PERI1_PMS_HP_MCPWM2_CTRL_REG (DR_REG_HP_PERI1_PMS_BASE + 0xc)
/** HP_PERI1_PMS_READ_TEE_HP_MCPWM2 : R/W; bitpos: [0]; default: 1;
 *  Configures hp_mcpwm2 registers read permission in tee mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI1_PMS_READ_TEE_HP_MCPWM2    (BIT(0))
#define HP_PERI1_PMS_READ_TEE_HP_MCPWM2_M  (HP_PERI1_PMS_READ_TEE_HP_MCPWM2_V << HP_PERI1_PMS_READ_TEE_HP_MCPWM2_S)
#define HP_PERI1_PMS_READ_TEE_HP_MCPWM2_V  0x00000001U
#define HP_PERI1_PMS_READ_TEE_HP_MCPWM2_S  0
/** HP_PERI1_PMS_READ_REE0_HP_MCPWM2 : R/W; bitpos: [1]; default: 0;
 *  Configures hp_mcpwm2 registers read permission in ree0 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI1_PMS_READ_REE0_HP_MCPWM2    (BIT(1))
#define HP_PERI1_PMS_READ_REE0_HP_MCPWM2_M  (HP_PERI1_PMS_READ_REE0_HP_MCPWM2_V << HP_PERI1_PMS_READ_REE0_HP_MCPWM2_S)
#define HP_PERI1_PMS_READ_REE0_HP_MCPWM2_V  0x00000001U
#define HP_PERI1_PMS_READ_REE0_HP_MCPWM2_S  1
/** HP_PERI1_PMS_READ_REE1_HP_MCPWM2 : R/W; bitpos: [2]; default: 0;
 *  Configures hp_mcpwm2 registers read permission in ree1 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI1_PMS_READ_REE1_HP_MCPWM2    (BIT(2))
#define HP_PERI1_PMS_READ_REE1_HP_MCPWM2_M  (HP_PERI1_PMS_READ_REE1_HP_MCPWM2_V << HP_PERI1_PMS_READ_REE1_HP_MCPWM2_S)
#define HP_PERI1_PMS_READ_REE1_HP_MCPWM2_V  0x00000001U
#define HP_PERI1_PMS_READ_REE1_HP_MCPWM2_S  2
/** HP_PERI1_PMS_READ_REE2_HP_MCPWM2 : R/W; bitpos: [3]; default: 0;
 *  Configures hp_mcpwm2 registers read permission in ree2 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI1_PMS_READ_REE2_HP_MCPWM2    (BIT(3))
#define HP_PERI1_PMS_READ_REE2_HP_MCPWM2_M  (HP_PERI1_PMS_READ_REE2_HP_MCPWM2_V << HP_PERI1_PMS_READ_REE2_HP_MCPWM2_S)
#define HP_PERI1_PMS_READ_REE2_HP_MCPWM2_V  0x00000001U
#define HP_PERI1_PMS_READ_REE2_HP_MCPWM2_S  3
/** HP_PERI1_PMS_WRITE_TEE_HP_MCPWM2 : R/W; bitpos: [4]; default: 1;
 *  Configures hp_mcpwm2 registers write permission in tee mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI1_PMS_WRITE_TEE_HP_MCPWM2    (BIT(4))
#define HP_PERI1_PMS_WRITE_TEE_HP_MCPWM2_M  (HP_PERI1_PMS_WRITE_TEE_HP_MCPWM2_V << HP_PERI1_PMS_WRITE_TEE_HP_MCPWM2_S)
#define HP_PERI1_PMS_WRITE_TEE_HP_MCPWM2_V  0x00000001U
#define HP_PERI1_PMS_WRITE_TEE_HP_MCPWM2_S  4
/** HP_PERI1_PMS_WRITE_REE0_HP_MCPWM2 : R/W; bitpos: [5]; default: 0;
 *  Configures hp_mcpwm2 registers write permission in ree0 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI1_PMS_WRITE_REE0_HP_MCPWM2    (BIT(5))
#define HP_PERI1_PMS_WRITE_REE0_HP_MCPWM2_M  (HP_PERI1_PMS_WRITE_REE0_HP_MCPWM2_V << HP_PERI1_PMS_WRITE_REE0_HP_MCPWM2_S)
#define HP_PERI1_PMS_WRITE_REE0_HP_MCPWM2_V  0x00000001U
#define HP_PERI1_PMS_WRITE_REE0_HP_MCPWM2_S  5
/** HP_PERI1_PMS_WRITE_REE1_HP_MCPWM2 : R/W; bitpos: [6]; default: 0;
 *  Configures hp_mcpwm2 registers write permission in ree1 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI1_PMS_WRITE_REE1_HP_MCPWM2    (BIT(6))
#define HP_PERI1_PMS_WRITE_REE1_HP_MCPWM2_M  (HP_PERI1_PMS_WRITE_REE1_HP_MCPWM2_V << HP_PERI1_PMS_WRITE_REE1_HP_MCPWM2_S)
#define HP_PERI1_PMS_WRITE_REE1_HP_MCPWM2_V  0x00000001U
#define HP_PERI1_PMS_WRITE_REE1_HP_MCPWM2_S  6
/** HP_PERI1_PMS_WRITE_REE2_HP_MCPWM2 : R/W; bitpos: [7]; default: 0;
 *  Configures hp_mcpwm2 registers write permission in ree2 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI1_PMS_WRITE_REE2_HP_MCPWM2    (BIT(7))
#define HP_PERI1_PMS_WRITE_REE2_HP_MCPWM2_M  (HP_PERI1_PMS_WRITE_REE2_HP_MCPWM2_V << HP_PERI1_PMS_WRITE_REE2_HP_MCPWM2_S)
#define HP_PERI1_PMS_WRITE_REE2_HP_MCPWM2_V  0x00000001U
#define HP_PERI1_PMS_WRITE_REE2_HP_MCPWM2_S  7
/** HP_PERI1_PMS_HP_MCPWM2_LOCK : R/W; bitpos: [8]; default: 0;
 *  Set 1 to lock hp_mcpwm2 peri_apm configuration
 */
#define HP_PERI1_PMS_HP_MCPWM2_LOCK    (BIT(8))
#define HP_PERI1_PMS_HP_MCPWM2_LOCK_M  (HP_PERI1_PMS_HP_MCPWM2_LOCK_V << HP_PERI1_PMS_HP_MCPWM2_LOCK_S)
#define HP_PERI1_PMS_HP_MCPWM2_LOCK_V  0x00000001U
#define HP_PERI1_PMS_HP_MCPWM2_LOCK_S  8

/** HP_PERI1_PMS_HP_MCPWM3_CTRL_REG register
 *  hp_mcpwm3 read/write control register
 */
#define HP_PERI1_PMS_HP_MCPWM3_CTRL_REG (DR_REG_HP_PERI1_PMS_BASE + 0x10)
/** HP_PERI1_PMS_READ_TEE_HP_MCPWM3 : R/W; bitpos: [0]; default: 1;
 *  Configures hp_mcpwm3 registers read permission in tee mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI1_PMS_READ_TEE_HP_MCPWM3    (BIT(0))
#define HP_PERI1_PMS_READ_TEE_HP_MCPWM3_M  (HP_PERI1_PMS_READ_TEE_HP_MCPWM3_V << HP_PERI1_PMS_READ_TEE_HP_MCPWM3_S)
#define HP_PERI1_PMS_READ_TEE_HP_MCPWM3_V  0x00000001U
#define HP_PERI1_PMS_READ_TEE_HP_MCPWM3_S  0
/** HP_PERI1_PMS_READ_REE0_HP_MCPWM3 : R/W; bitpos: [1]; default: 0;
 *  Configures hp_mcpwm3 registers read permission in ree0 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI1_PMS_READ_REE0_HP_MCPWM3    (BIT(1))
#define HP_PERI1_PMS_READ_REE0_HP_MCPWM3_M  (HP_PERI1_PMS_READ_REE0_HP_MCPWM3_V << HP_PERI1_PMS_READ_REE0_HP_MCPWM3_S)
#define HP_PERI1_PMS_READ_REE0_HP_MCPWM3_V  0x00000001U
#define HP_PERI1_PMS_READ_REE0_HP_MCPWM3_S  1
/** HP_PERI1_PMS_READ_REE1_HP_MCPWM3 : R/W; bitpos: [2]; default: 0;
 *  Configures hp_mcpwm3 registers read permission in ree1 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI1_PMS_READ_REE1_HP_MCPWM3    (BIT(2))
#define HP_PERI1_PMS_READ_REE1_HP_MCPWM3_M  (HP_PERI1_PMS_READ_REE1_HP_MCPWM3_V << HP_PERI1_PMS_READ_REE1_HP_MCPWM3_S)
#define HP_PERI1_PMS_READ_REE1_HP_MCPWM3_V  0x00000001U
#define HP_PERI1_PMS_READ_REE1_HP_MCPWM3_S  2
/** HP_PERI1_PMS_READ_REE2_HP_MCPWM3 : R/W; bitpos: [3]; default: 0;
 *  Configures hp_mcpwm3 registers read permission in ree2 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI1_PMS_READ_REE2_HP_MCPWM3    (BIT(3))
#define HP_PERI1_PMS_READ_REE2_HP_MCPWM3_M  (HP_PERI1_PMS_READ_REE2_HP_MCPWM3_V << HP_PERI1_PMS_READ_REE2_HP_MCPWM3_S)
#define HP_PERI1_PMS_READ_REE2_HP_MCPWM3_V  0x00000001U
#define HP_PERI1_PMS_READ_REE2_HP_MCPWM3_S  3
/** HP_PERI1_PMS_WRITE_TEE_HP_MCPWM3 : R/W; bitpos: [4]; default: 1;
 *  Configures hp_mcpwm3 registers write permission in tee mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI1_PMS_WRITE_TEE_HP_MCPWM3    (BIT(4))
#define HP_PERI1_PMS_WRITE_TEE_HP_MCPWM3_M  (HP_PERI1_PMS_WRITE_TEE_HP_MCPWM3_V << HP_PERI1_PMS_WRITE_TEE_HP_MCPWM3_S)
#define HP_PERI1_PMS_WRITE_TEE_HP_MCPWM3_V  0x00000001U
#define HP_PERI1_PMS_WRITE_TEE_HP_MCPWM3_S  4
/** HP_PERI1_PMS_WRITE_REE0_HP_MCPWM3 : R/W; bitpos: [5]; default: 0;
 *  Configures hp_mcpwm3 registers write permission in ree0 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI1_PMS_WRITE_REE0_HP_MCPWM3    (BIT(5))
#define HP_PERI1_PMS_WRITE_REE0_HP_MCPWM3_M  (HP_PERI1_PMS_WRITE_REE0_HP_MCPWM3_V << HP_PERI1_PMS_WRITE_REE0_HP_MCPWM3_S)
#define HP_PERI1_PMS_WRITE_REE0_HP_MCPWM3_V  0x00000001U
#define HP_PERI1_PMS_WRITE_REE0_HP_MCPWM3_S  5
/** HP_PERI1_PMS_WRITE_REE1_HP_MCPWM3 : R/W; bitpos: [6]; default: 0;
 *  Configures hp_mcpwm3 registers write permission in ree1 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI1_PMS_WRITE_REE1_HP_MCPWM3    (BIT(6))
#define HP_PERI1_PMS_WRITE_REE1_HP_MCPWM3_M  (HP_PERI1_PMS_WRITE_REE1_HP_MCPWM3_V << HP_PERI1_PMS_WRITE_REE1_HP_MCPWM3_S)
#define HP_PERI1_PMS_WRITE_REE1_HP_MCPWM3_V  0x00000001U
#define HP_PERI1_PMS_WRITE_REE1_HP_MCPWM3_S  6
/** HP_PERI1_PMS_WRITE_REE2_HP_MCPWM3 : R/W; bitpos: [7]; default: 0;
 *  Configures hp_mcpwm3 registers write permission in ree2 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI1_PMS_WRITE_REE2_HP_MCPWM3    (BIT(7))
#define HP_PERI1_PMS_WRITE_REE2_HP_MCPWM3_M  (HP_PERI1_PMS_WRITE_REE2_HP_MCPWM3_V << HP_PERI1_PMS_WRITE_REE2_HP_MCPWM3_S)
#define HP_PERI1_PMS_WRITE_REE2_HP_MCPWM3_V  0x00000001U
#define HP_PERI1_PMS_WRITE_REE2_HP_MCPWM3_S  7
/** HP_PERI1_PMS_HP_MCPWM3_LOCK : R/W; bitpos: [8]; default: 0;
 *  Set 1 to lock hp_mcpwm3 peri_apm configuration
 */
#define HP_PERI1_PMS_HP_MCPWM3_LOCK    (BIT(8))
#define HP_PERI1_PMS_HP_MCPWM3_LOCK_M  (HP_PERI1_PMS_HP_MCPWM3_LOCK_V << HP_PERI1_PMS_HP_MCPWM3_LOCK_S)
#define HP_PERI1_PMS_HP_MCPWM3_LOCK_V  0x00000001U
#define HP_PERI1_PMS_HP_MCPWM3_LOCK_S  8

/** HP_PERI1_PMS_HP_I2C0_CTRL_REG register
 *  hp_i2c0 read/write control register
 */
#define HP_PERI1_PMS_HP_I2C0_CTRL_REG (DR_REG_HP_PERI1_PMS_BASE + 0x14)
/** HP_PERI1_PMS_READ_TEE_HP_I2C0 : R/W; bitpos: [0]; default: 1;
 *  Configures hp_i2c0 registers read permission in tee mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI1_PMS_READ_TEE_HP_I2C0    (BIT(0))
#define HP_PERI1_PMS_READ_TEE_HP_I2C0_M  (HP_PERI1_PMS_READ_TEE_HP_I2C0_V << HP_PERI1_PMS_READ_TEE_HP_I2C0_S)
#define HP_PERI1_PMS_READ_TEE_HP_I2C0_V  0x00000001U
#define HP_PERI1_PMS_READ_TEE_HP_I2C0_S  0
/** HP_PERI1_PMS_READ_REE0_HP_I2C0 : R/W; bitpos: [1]; default: 0;
 *  Configures hp_i2c0 registers read permission in ree0 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI1_PMS_READ_REE0_HP_I2C0    (BIT(1))
#define HP_PERI1_PMS_READ_REE0_HP_I2C0_M  (HP_PERI1_PMS_READ_REE0_HP_I2C0_V << HP_PERI1_PMS_READ_REE0_HP_I2C0_S)
#define HP_PERI1_PMS_READ_REE0_HP_I2C0_V  0x00000001U
#define HP_PERI1_PMS_READ_REE0_HP_I2C0_S  1
/** HP_PERI1_PMS_READ_REE1_HP_I2C0 : R/W; bitpos: [2]; default: 0;
 *  Configures hp_i2c0 registers read permission in ree1 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI1_PMS_READ_REE1_HP_I2C0    (BIT(2))
#define HP_PERI1_PMS_READ_REE1_HP_I2C0_M  (HP_PERI1_PMS_READ_REE1_HP_I2C0_V << HP_PERI1_PMS_READ_REE1_HP_I2C0_S)
#define HP_PERI1_PMS_READ_REE1_HP_I2C0_V  0x00000001U
#define HP_PERI1_PMS_READ_REE1_HP_I2C0_S  2
/** HP_PERI1_PMS_READ_REE2_HP_I2C0 : R/W; bitpos: [3]; default: 0;
 *  Configures hp_i2c0 registers read permission in ree2 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI1_PMS_READ_REE2_HP_I2C0    (BIT(3))
#define HP_PERI1_PMS_READ_REE2_HP_I2C0_M  (HP_PERI1_PMS_READ_REE2_HP_I2C0_V << HP_PERI1_PMS_READ_REE2_HP_I2C0_S)
#define HP_PERI1_PMS_READ_REE2_HP_I2C0_V  0x00000001U
#define HP_PERI1_PMS_READ_REE2_HP_I2C0_S  3
/** HP_PERI1_PMS_WRITE_TEE_HP_I2C0 : R/W; bitpos: [4]; default: 1;
 *  Configures hp_i2c0 registers write permission in tee mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI1_PMS_WRITE_TEE_HP_I2C0    (BIT(4))
#define HP_PERI1_PMS_WRITE_TEE_HP_I2C0_M  (HP_PERI1_PMS_WRITE_TEE_HP_I2C0_V << HP_PERI1_PMS_WRITE_TEE_HP_I2C0_S)
#define HP_PERI1_PMS_WRITE_TEE_HP_I2C0_V  0x00000001U
#define HP_PERI1_PMS_WRITE_TEE_HP_I2C0_S  4
/** HP_PERI1_PMS_WRITE_REE0_HP_I2C0 : R/W; bitpos: [5]; default: 0;
 *  Configures hp_i2c0 registers write permission in ree0 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI1_PMS_WRITE_REE0_HP_I2C0    (BIT(5))
#define HP_PERI1_PMS_WRITE_REE0_HP_I2C0_M  (HP_PERI1_PMS_WRITE_REE0_HP_I2C0_V << HP_PERI1_PMS_WRITE_REE0_HP_I2C0_S)
#define HP_PERI1_PMS_WRITE_REE0_HP_I2C0_V  0x00000001U
#define HP_PERI1_PMS_WRITE_REE0_HP_I2C0_S  5
/** HP_PERI1_PMS_WRITE_REE1_HP_I2C0 : R/W; bitpos: [6]; default: 0;
 *  Configures hp_i2c0 registers write permission in ree1 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI1_PMS_WRITE_REE1_HP_I2C0    (BIT(6))
#define HP_PERI1_PMS_WRITE_REE1_HP_I2C0_M  (HP_PERI1_PMS_WRITE_REE1_HP_I2C0_V << HP_PERI1_PMS_WRITE_REE1_HP_I2C0_S)
#define HP_PERI1_PMS_WRITE_REE1_HP_I2C0_V  0x00000001U
#define HP_PERI1_PMS_WRITE_REE1_HP_I2C0_S  6
/** HP_PERI1_PMS_WRITE_REE2_HP_I2C0 : R/W; bitpos: [7]; default: 0;
 *  Configures hp_i2c0 registers write permission in ree2 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI1_PMS_WRITE_REE2_HP_I2C0    (BIT(7))
#define HP_PERI1_PMS_WRITE_REE2_HP_I2C0_M  (HP_PERI1_PMS_WRITE_REE2_HP_I2C0_V << HP_PERI1_PMS_WRITE_REE2_HP_I2C0_S)
#define HP_PERI1_PMS_WRITE_REE2_HP_I2C0_V  0x00000001U
#define HP_PERI1_PMS_WRITE_REE2_HP_I2C0_S  7
/** HP_PERI1_PMS_HP_I2C0_LOCK : R/W; bitpos: [8]; default: 0;
 *  Set 1 to lock hp_i2c0 peri_apm configuration
 */
#define HP_PERI1_PMS_HP_I2C0_LOCK    (BIT(8))
#define HP_PERI1_PMS_HP_I2C0_LOCK_M  (HP_PERI1_PMS_HP_I2C0_LOCK_V << HP_PERI1_PMS_HP_I2C0_LOCK_S)
#define HP_PERI1_PMS_HP_I2C0_LOCK_V  0x00000001U
#define HP_PERI1_PMS_HP_I2C0_LOCK_S  8

/** HP_PERI1_PMS_HP_I2C1_CTRL_REG register
 *  hp_i2c1 read/write control register
 */
#define HP_PERI1_PMS_HP_I2C1_CTRL_REG (DR_REG_HP_PERI1_PMS_BASE + 0x18)
/** HP_PERI1_PMS_READ_TEE_HP_I2C1 : R/W; bitpos: [0]; default: 1;
 *  Configures hp_i2c1 registers read permission in tee mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI1_PMS_READ_TEE_HP_I2C1    (BIT(0))
#define HP_PERI1_PMS_READ_TEE_HP_I2C1_M  (HP_PERI1_PMS_READ_TEE_HP_I2C1_V << HP_PERI1_PMS_READ_TEE_HP_I2C1_S)
#define HP_PERI1_PMS_READ_TEE_HP_I2C1_V  0x00000001U
#define HP_PERI1_PMS_READ_TEE_HP_I2C1_S  0
/** HP_PERI1_PMS_READ_REE0_HP_I2C1 : R/W; bitpos: [1]; default: 0;
 *  Configures hp_i2c1 registers read permission in ree0 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI1_PMS_READ_REE0_HP_I2C1    (BIT(1))
#define HP_PERI1_PMS_READ_REE0_HP_I2C1_M  (HP_PERI1_PMS_READ_REE0_HP_I2C1_V << HP_PERI1_PMS_READ_REE0_HP_I2C1_S)
#define HP_PERI1_PMS_READ_REE0_HP_I2C1_V  0x00000001U
#define HP_PERI1_PMS_READ_REE0_HP_I2C1_S  1
/** HP_PERI1_PMS_READ_REE1_HP_I2C1 : R/W; bitpos: [2]; default: 0;
 *  Configures hp_i2c1 registers read permission in ree1 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI1_PMS_READ_REE1_HP_I2C1    (BIT(2))
#define HP_PERI1_PMS_READ_REE1_HP_I2C1_M  (HP_PERI1_PMS_READ_REE1_HP_I2C1_V << HP_PERI1_PMS_READ_REE1_HP_I2C1_S)
#define HP_PERI1_PMS_READ_REE1_HP_I2C1_V  0x00000001U
#define HP_PERI1_PMS_READ_REE1_HP_I2C1_S  2
/** HP_PERI1_PMS_READ_REE2_HP_I2C1 : R/W; bitpos: [3]; default: 0;
 *  Configures hp_i2c1 registers read permission in ree2 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI1_PMS_READ_REE2_HP_I2C1    (BIT(3))
#define HP_PERI1_PMS_READ_REE2_HP_I2C1_M  (HP_PERI1_PMS_READ_REE2_HP_I2C1_V << HP_PERI1_PMS_READ_REE2_HP_I2C1_S)
#define HP_PERI1_PMS_READ_REE2_HP_I2C1_V  0x00000001U
#define HP_PERI1_PMS_READ_REE2_HP_I2C1_S  3
/** HP_PERI1_PMS_WRITE_TEE_HP_I2C1 : R/W; bitpos: [4]; default: 1;
 *  Configures hp_i2c1 registers write permission in tee mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI1_PMS_WRITE_TEE_HP_I2C1    (BIT(4))
#define HP_PERI1_PMS_WRITE_TEE_HP_I2C1_M  (HP_PERI1_PMS_WRITE_TEE_HP_I2C1_V << HP_PERI1_PMS_WRITE_TEE_HP_I2C1_S)
#define HP_PERI1_PMS_WRITE_TEE_HP_I2C1_V  0x00000001U
#define HP_PERI1_PMS_WRITE_TEE_HP_I2C1_S  4
/** HP_PERI1_PMS_WRITE_REE0_HP_I2C1 : R/W; bitpos: [5]; default: 0;
 *  Configures hp_i2c1 registers write permission in ree0 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI1_PMS_WRITE_REE0_HP_I2C1    (BIT(5))
#define HP_PERI1_PMS_WRITE_REE0_HP_I2C1_M  (HP_PERI1_PMS_WRITE_REE0_HP_I2C1_V << HP_PERI1_PMS_WRITE_REE0_HP_I2C1_S)
#define HP_PERI1_PMS_WRITE_REE0_HP_I2C1_V  0x00000001U
#define HP_PERI1_PMS_WRITE_REE0_HP_I2C1_S  5
/** HP_PERI1_PMS_WRITE_REE1_HP_I2C1 : R/W; bitpos: [6]; default: 0;
 *  Configures hp_i2c1 registers write permission in ree1 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI1_PMS_WRITE_REE1_HP_I2C1    (BIT(6))
#define HP_PERI1_PMS_WRITE_REE1_HP_I2C1_M  (HP_PERI1_PMS_WRITE_REE1_HP_I2C1_V << HP_PERI1_PMS_WRITE_REE1_HP_I2C1_S)
#define HP_PERI1_PMS_WRITE_REE1_HP_I2C1_V  0x00000001U
#define HP_PERI1_PMS_WRITE_REE1_HP_I2C1_S  6
/** HP_PERI1_PMS_WRITE_REE2_HP_I2C1 : R/W; bitpos: [7]; default: 0;
 *  Configures hp_i2c1 registers write permission in ree2 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI1_PMS_WRITE_REE2_HP_I2C1    (BIT(7))
#define HP_PERI1_PMS_WRITE_REE2_HP_I2C1_M  (HP_PERI1_PMS_WRITE_REE2_HP_I2C1_V << HP_PERI1_PMS_WRITE_REE2_HP_I2C1_S)
#define HP_PERI1_PMS_WRITE_REE2_HP_I2C1_V  0x00000001U
#define HP_PERI1_PMS_WRITE_REE2_HP_I2C1_S  7
/** HP_PERI1_PMS_HP_I2C1_LOCK : R/W; bitpos: [8]; default: 0;
 *  Set 1 to lock hp_i2c1 peri_apm configuration
 */
#define HP_PERI1_PMS_HP_I2C1_LOCK    (BIT(8))
#define HP_PERI1_PMS_HP_I2C1_LOCK_M  (HP_PERI1_PMS_HP_I2C1_LOCK_V << HP_PERI1_PMS_HP_I2C1_LOCK_S)
#define HP_PERI1_PMS_HP_I2C1_LOCK_V  0x00000001U
#define HP_PERI1_PMS_HP_I2C1_LOCK_S  8

/** HP_PERI1_PMS_HP_I2S0_CTRL_REG register
 *  hp_i2s0 read/write control register
 */
#define HP_PERI1_PMS_HP_I2S0_CTRL_REG (DR_REG_HP_PERI1_PMS_BASE + 0x1c)
/** HP_PERI1_PMS_READ_TEE_HP_I2S0 : R/W; bitpos: [0]; default: 1;
 *  Configures hp_i2s0 registers read permission in tee mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI1_PMS_READ_TEE_HP_I2S0    (BIT(0))
#define HP_PERI1_PMS_READ_TEE_HP_I2S0_M  (HP_PERI1_PMS_READ_TEE_HP_I2S0_V << HP_PERI1_PMS_READ_TEE_HP_I2S0_S)
#define HP_PERI1_PMS_READ_TEE_HP_I2S0_V  0x00000001U
#define HP_PERI1_PMS_READ_TEE_HP_I2S0_S  0
/** HP_PERI1_PMS_READ_REE0_HP_I2S0 : R/W; bitpos: [1]; default: 0;
 *  Configures hp_i2s0 registers read permission in ree0 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI1_PMS_READ_REE0_HP_I2S0    (BIT(1))
#define HP_PERI1_PMS_READ_REE0_HP_I2S0_M  (HP_PERI1_PMS_READ_REE0_HP_I2S0_V << HP_PERI1_PMS_READ_REE0_HP_I2S0_S)
#define HP_PERI1_PMS_READ_REE0_HP_I2S0_V  0x00000001U
#define HP_PERI1_PMS_READ_REE0_HP_I2S0_S  1
/** HP_PERI1_PMS_READ_REE1_HP_I2S0 : R/W; bitpos: [2]; default: 0;
 *  Configures hp_i2s0 registers read permission in ree1 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI1_PMS_READ_REE1_HP_I2S0    (BIT(2))
#define HP_PERI1_PMS_READ_REE1_HP_I2S0_M  (HP_PERI1_PMS_READ_REE1_HP_I2S0_V << HP_PERI1_PMS_READ_REE1_HP_I2S0_S)
#define HP_PERI1_PMS_READ_REE1_HP_I2S0_V  0x00000001U
#define HP_PERI1_PMS_READ_REE1_HP_I2S0_S  2
/** HP_PERI1_PMS_READ_REE2_HP_I2S0 : R/W; bitpos: [3]; default: 0;
 *  Configures hp_i2s0 registers read permission in ree2 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI1_PMS_READ_REE2_HP_I2S0    (BIT(3))
#define HP_PERI1_PMS_READ_REE2_HP_I2S0_M  (HP_PERI1_PMS_READ_REE2_HP_I2S0_V << HP_PERI1_PMS_READ_REE2_HP_I2S0_S)
#define HP_PERI1_PMS_READ_REE2_HP_I2S0_V  0x00000001U
#define HP_PERI1_PMS_READ_REE2_HP_I2S0_S  3
/** HP_PERI1_PMS_WRITE_TEE_HP_I2S0 : R/W; bitpos: [4]; default: 1;
 *  Configures hp_i2s0 registers write permission in tee mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI1_PMS_WRITE_TEE_HP_I2S0    (BIT(4))
#define HP_PERI1_PMS_WRITE_TEE_HP_I2S0_M  (HP_PERI1_PMS_WRITE_TEE_HP_I2S0_V << HP_PERI1_PMS_WRITE_TEE_HP_I2S0_S)
#define HP_PERI1_PMS_WRITE_TEE_HP_I2S0_V  0x00000001U
#define HP_PERI1_PMS_WRITE_TEE_HP_I2S0_S  4
/** HP_PERI1_PMS_WRITE_REE0_HP_I2S0 : R/W; bitpos: [5]; default: 0;
 *  Configures hp_i2s0 registers write permission in ree0 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI1_PMS_WRITE_REE0_HP_I2S0    (BIT(5))
#define HP_PERI1_PMS_WRITE_REE0_HP_I2S0_M  (HP_PERI1_PMS_WRITE_REE0_HP_I2S0_V << HP_PERI1_PMS_WRITE_REE0_HP_I2S0_S)
#define HP_PERI1_PMS_WRITE_REE0_HP_I2S0_V  0x00000001U
#define HP_PERI1_PMS_WRITE_REE0_HP_I2S0_S  5
/** HP_PERI1_PMS_WRITE_REE1_HP_I2S0 : R/W; bitpos: [6]; default: 0;
 *  Configures hp_i2s0 registers write permission in ree1 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI1_PMS_WRITE_REE1_HP_I2S0    (BIT(6))
#define HP_PERI1_PMS_WRITE_REE1_HP_I2S0_M  (HP_PERI1_PMS_WRITE_REE1_HP_I2S0_V << HP_PERI1_PMS_WRITE_REE1_HP_I2S0_S)
#define HP_PERI1_PMS_WRITE_REE1_HP_I2S0_V  0x00000001U
#define HP_PERI1_PMS_WRITE_REE1_HP_I2S0_S  6
/** HP_PERI1_PMS_WRITE_REE2_HP_I2S0 : R/W; bitpos: [7]; default: 0;
 *  Configures hp_i2s0 registers write permission in ree2 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI1_PMS_WRITE_REE2_HP_I2S0    (BIT(7))
#define HP_PERI1_PMS_WRITE_REE2_HP_I2S0_M  (HP_PERI1_PMS_WRITE_REE2_HP_I2S0_V << HP_PERI1_PMS_WRITE_REE2_HP_I2S0_S)
#define HP_PERI1_PMS_WRITE_REE2_HP_I2S0_V  0x00000001U
#define HP_PERI1_PMS_WRITE_REE2_HP_I2S0_S  7
/** HP_PERI1_PMS_HP_I2S0_LOCK : R/W; bitpos: [8]; default: 0;
 *  Set 1 to lock hp_i2s0 peri_apm configuration
 */
#define HP_PERI1_PMS_HP_I2S0_LOCK    (BIT(8))
#define HP_PERI1_PMS_HP_I2S0_LOCK_M  (HP_PERI1_PMS_HP_I2S0_LOCK_V << HP_PERI1_PMS_HP_I2S0_LOCK_S)
#define HP_PERI1_PMS_HP_I2S0_LOCK_V  0x00000001U
#define HP_PERI1_PMS_HP_I2S0_LOCK_S  8

/** HP_PERI1_PMS_HP_I2S1_CTRL_REG register
 *  hp_i2s1 read/write control register
 */
#define HP_PERI1_PMS_HP_I2S1_CTRL_REG (DR_REG_HP_PERI1_PMS_BASE + 0x20)
/** HP_PERI1_PMS_READ_TEE_HP_I2S1 : R/W; bitpos: [0]; default: 1;
 *  Configures hp_i2s1 registers read permission in tee mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI1_PMS_READ_TEE_HP_I2S1    (BIT(0))
#define HP_PERI1_PMS_READ_TEE_HP_I2S1_M  (HP_PERI1_PMS_READ_TEE_HP_I2S1_V << HP_PERI1_PMS_READ_TEE_HP_I2S1_S)
#define HP_PERI1_PMS_READ_TEE_HP_I2S1_V  0x00000001U
#define HP_PERI1_PMS_READ_TEE_HP_I2S1_S  0
/** HP_PERI1_PMS_READ_REE0_HP_I2S1 : R/W; bitpos: [1]; default: 0;
 *  Configures hp_i2s1 registers read permission in ree0 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI1_PMS_READ_REE0_HP_I2S1    (BIT(1))
#define HP_PERI1_PMS_READ_REE0_HP_I2S1_M  (HP_PERI1_PMS_READ_REE0_HP_I2S1_V << HP_PERI1_PMS_READ_REE0_HP_I2S1_S)
#define HP_PERI1_PMS_READ_REE0_HP_I2S1_V  0x00000001U
#define HP_PERI1_PMS_READ_REE0_HP_I2S1_S  1
/** HP_PERI1_PMS_READ_REE1_HP_I2S1 : R/W; bitpos: [2]; default: 0;
 *  Configures hp_i2s1 registers read permission in ree1 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI1_PMS_READ_REE1_HP_I2S1    (BIT(2))
#define HP_PERI1_PMS_READ_REE1_HP_I2S1_M  (HP_PERI1_PMS_READ_REE1_HP_I2S1_V << HP_PERI1_PMS_READ_REE1_HP_I2S1_S)
#define HP_PERI1_PMS_READ_REE1_HP_I2S1_V  0x00000001U
#define HP_PERI1_PMS_READ_REE1_HP_I2S1_S  2
/** HP_PERI1_PMS_READ_REE2_HP_I2S1 : R/W; bitpos: [3]; default: 0;
 *  Configures hp_i2s1 registers read permission in ree2 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI1_PMS_READ_REE2_HP_I2S1    (BIT(3))
#define HP_PERI1_PMS_READ_REE2_HP_I2S1_M  (HP_PERI1_PMS_READ_REE2_HP_I2S1_V << HP_PERI1_PMS_READ_REE2_HP_I2S1_S)
#define HP_PERI1_PMS_READ_REE2_HP_I2S1_V  0x00000001U
#define HP_PERI1_PMS_READ_REE2_HP_I2S1_S  3
/** HP_PERI1_PMS_WRITE_TEE_HP_I2S1 : R/W; bitpos: [4]; default: 1;
 *  Configures hp_i2s1 registers write permission in tee mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI1_PMS_WRITE_TEE_HP_I2S1    (BIT(4))
#define HP_PERI1_PMS_WRITE_TEE_HP_I2S1_M  (HP_PERI1_PMS_WRITE_TEE_HP_I2S1_V << HP_PERI1_PMS_WRITE_TEE_HP_I2S1_S)
#define HP_PERI1_PMS_WRITE_TEE_HP_I2S1_V  0x00000001U
#define HP_PERI1_PMS_WRITE_TEE_HP_I2S1_S  4
/** HP_PERI1_PMS_WRITE_REE0_HP_I2S1 : R/W; bitpos: [5]; default: 0;
 *  Configures hp_i2s1 registers write permission in ree0 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI1_PMS_WRITE_REE0_HP_I2S1    (BIT(5))
#define HP_PERI1_PMS_WRITE_REE0_HP_I2S1_M  (HP_PERI1_PMS_WRITE_REE0_HP_I2S1_V << HP_PERI1_PMS_WRITE_REE0_HP_I2S1_S)
#define HP_PERI1_PMS_WRITE_REE0_HP_I2S1_V  0x00000001U
#define HP_PERI1_PMS_WRITE_REE0_HP_I2S1_S  5
/** HP_PERI1_PMS_WRITE_REE1_HP_I2S1 : R/W; bitpos: [6]; default: 0;
 *  Configures hp_i2s1 registers write permission in ree1 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI1_PMS_WRITE_REE1_HP_I2S1    (BIT(6))
#define HP_PERI1_PMS_WRITE_REE1_HP_I2S1_M  (HP_PERI1_PMS_WRITE_REE1_HP_I2S1_V << HP_PERI1_PMS_WRITE_REE1_HP_I2S1_S)
#define HP_PERI1_PMS_WRITE_REE1_HP_I2S1_V  0x00000001U
#define HP_PERI1_PMS_WRITE_REE1_HP_I2S1_S  6
/** HP_PERI1_PMS_WRITE_REE2_HP_I2S1 : R/W; bitpos: [7]; default: 0;
 *  Configures hp_i2s1 registers write permission in ree2 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI1_PMS_WRITE_REE2_HP_I2S1    (BIT(7))
#define HP_PERI1_PMS_WRITE_REE2_HP_I2S1_M  (HP_PERI1_PMS_WRITE_REE2_HP_I2S1_V << HP_PERI1_PMS_WRITE_REE2_HP_I2S1_S)
#define HP_PERI1_PMS_WRITE_REE2_HP_I2S1_V  0x00000001U
#define HP_PERI1_PMS_WRITE_REE2_HP_I2S1_S  7
/** HP_PERI1_PMS_HP_I2S1_LOCK : R/W; bitpos: [8]; default: 0;
 *  Set 1 to lock hp_i2s1 peri_apm configuration
 */
#define HP_PERI1_PMS_HP_I2S1_LOCK    (BIT(8))
#define HP_PERI1_PMS_HP_I2S1_LOCK_M  (HP_PERI1_PMS_HP_I2S1_LOCK_V << HP_PERI1_PMS_HP_I2S1_LOCK_S)
#define HP_PERI1_PMS_HP_I2S1_LOCK_V  0x00000001U
#define HP_PERI1_PMS_HP_I2S1_LOCK_S  8

/** HP_PERI1_PMS_HP_PCNT0_CTRL_REG register
 *  hp_pcnt0 read/write control register
 */
#define HP_PERI1_PMS_HP_PCNT0_CTRL_REG (DR_REG_HP_PERI1_PMS_BASE + 0x24)
/** HP_PERI1_PMS_READ_TEE_HP_PCNT0 : R/W; bitpos: [0]; default: 1;
 *  Configures hp_pcnt0 registers read permission in tee mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI1_PMS_READ_TEE_HP_PCNT0    (BIT(0))
#define HP_PERI1_PMS_READ_TEE_HP_PCNT0_M  (HP_PERI1_PMS_READ_TEE_HP_PCNT0_V << HP_PERI1_PMS_READ_TEE_HP_PCNT0_S)
#define HP_PERI1_PMS_READ_TEE_HP_PCNT0_V  0x00000001U
#define HP_PERI1_PMS_READ_TEE_HP_PCNT0_S  0
/** HP_PERI1_PMS_READ_REE0_HP_PCNT0 : R/W; bitpos: [1]; default: 0;
 *  Configures hp_pcnt0 registers read permission in ree0 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI1_PMS_READ_REE0_HP_PCNT0    (BIT(1))
#define HP_PERI1_PMS_READ_REE0_HP_PCNT0_M  (HP_PERI1_PMS_READ_REE0_HP_PCNT0_V << HP_PERI1_PMS_READ_REE0_HP_PCNT0_S)
#define HP_PERI1_PMS_READ_REE0_HP_PCNT0_V  0x00000001U
#define HP_PERI1_PMS_READ_REE0_HP_PCNT0_S  1
/** HP_PERI1_PMS_READ_REE1_HP_PCNT0 : R/W; bitpos: [2]; default: 0;
 *  Configures hp_pcnt0 registers read permission in ree1 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI1_PMS_READ_REE1_HP_PCNT0    (BIT(2))
#define HP_PERI1_PMS_READ_REE1_HP_PCNT0_M  (HP_PERI1_PMS_READ_REE1_HP_PCNT0_V << HP_PERI1_PMS_READ_REE1_HP_PCNT0_S)
#define HP_PERI1_PMS_READ_REE1_HP_PCNT0_V  0x00000001U
#define HP_PERI1_PMS_READ_REE1_HP_PCNT0_S  2
/** HP_PERI1_PMS_READ_REE2_HP_PCNT0 : R/W; bitpos: [3]; default: 0;
 *  Configures hp_pcnt0 registers read permission in ree2 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI1_PMS_READ_REE2_HP_PCNT0    (BIT(3))
#define HP_PERI1_PMS_READ_REE2_HP_PCNT0_M  (HP_PERI1_PMS_READ_REE2_HP_PCNT0_V << HP_PERI1_PMS_READ_REE2_HP_PCNT0_S)
#define HP_PERI1_PMS_READ_REE2_HP_PCNT0_V  0x00000001U
#define HP_PERI1_PMS_READ_REE2_HP_PCNT0_S  3
/** HP_PERI1_PMS_WRITE_TEE_HP_PCNT0 : R/W; bitpos: [4]; default: 1;
 *  Configures hp_pcnt0 registers write permission in tee mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI1_PMS_WRITE_TEE_HP_PCNT0    (BIT(4))
#define HP_PERI1_PMS_WRITE_TEE_HP_PCNT0_M  (HP_PERI1_PMS_WRITE_TEE_HP_PCNT0_V << HP_PERI1_PMS_WRITE_TEE_HP_PCNT0_S)
#define HP_PERI1_PMS_WRITE_TEE_HP_PCNT0_V  0x00000001U
#define HP_PERI1_PMS_WRITE_TEE_HP_PCNT0_S  4
/** HP_PERI1_PMS_WRITE_REE0_HP_PCNT0 : R/W; bitpos: [5]; default: 0;
 *  Configures hp_pcnt0 registers write permission in ree0 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI1_PMS_WRITE_REE0_HP_PCNT0    (BIT(5))
#define HP_PERI1_PMS_WRITE_REE0_HP_PCNT0_M  (HP_PERI1_PMS_WRITE_REE0_HP_PCNT0_V << HP_PERI1_PMS_WRITE_REE0_HP_PCNT0_S)
#define HP_PERI1_PMS_WRITE_REE0_HP_PCNT0_V  0x00000001U
#define HP_PERI1_PMS_WRITE_REE0_HP_PCNT0_S  5
/** HP_PERI1_PMS_WRITE_REE1_HP_PCNT0 : R/W; bitpos: [6]; default: 0;
 *  Configures hp_pcnt0 registers write permission in ree1 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI1_PMS_WRITE_REE1_HP_PCNT0    (BIT(6))
#define HP_PERI1_PMS_WRITE_REE1_HP_PCNT0_M  (HP_PERI1_PMS_WRITE_REE1_HP_PCNT0_V << HP_PERI1_PMS_WRITE_REE1_HP_PCNT0_S)
#define HP_PERI1_PMS_WRITE_REE1_HP_PCNT0_V  0x00000001U
#define HP_PERI1_PMS_WRITE_REE1_HP_PCNT0_S  6
/** HP_PERI1_PMS_WRITE_REE2_HP_PCNT0 : R/W; bitpos: [7]; default: 0;
 *  Configures hp_pcnt0 registers write permission in ree2 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI1_PMS_WRITE_REE2_HP_PCNT0    (BIT(7))
#define HP_PERI1_PMS_WRITE_REE2_HP_PCNT0_M  (HP_PERI1_PMS_WRITE_REE2_HP_PCNT0_V << HP_PERI1_PMS_WRITE_REE2_HP_PCNT0_S)
#define HP_PERI1_PMS_WRITE_REE2_HP_PCNT0_V  0x00000001U
#define HP_PERI1_PMS_WRITE_REE2_HP_PCNT0_S  7
/** HP_PERI1_PMS_HP_PCNT0_LOCK : R/W; bitpos: [8]; default: 0;
 *  Set 1 to lock hp_pcnt0 peri_apm configuration
 */
#define HP_PERI1_PMS_HP_PCNT0_LOCK    (BIT(8))
#define HP_PERI1_PMS_HP_PCNT0_LOCK_M  (HP_PERI1_PMS_HP_PCNT0_LOCK_V << HP_PERI1_PMS_HP_PCNT0_LOCK_S)
#define HP_PERI1_PMS_HP_PCNT0_LOCK_V  0x00000001U
#define HP_PERI1_PMS_HP_PCNT0_LOCK_S  8

/** HP_PERI1_PMS_HP_UART0_CTRL_REG register
 *  hp_uart0 read/write control register
 */
#define HP_PERI1_PMS_HP_UART0_CTRL_REG (DR_REG_HP_PERI1_PMS_BASE + 0x28)
/** HP_PERI1_PMS_READ_TEE_HP_UART0 : R/W; bitpos: [0]; default: 1;
 *  Configures hp_uart0 registers read permission in tee mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI1_PMS_READ_TEE_HP_UART0    (BIT(0))
#define HP_PERI1_PMS_READ_TEE_HP_UART0_M  (HP_PERI1_PMS_READ_TEE_HP_UART0_V << HP_PERI1_PMS_READ_TEE_HP_UART0_S)
#define HP_PERI1_PMS_READ_TEE_HP_UART0_V  0x00000001U
#define HP_PERI1_PMS_READ_TEE_HP_UART0_S  0
/** HP_PERI1_PMS_READ_REE0_HP_UART0 : R/W; bitpos: [1]; default: 0;
 *  Configures hp_uart0 registers read permission in ree0 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI1_PMS_READ_REE0_HP_UART0    (BIT(1))
#define HP_PERI1_PMS_READ_REE0_HP_UART0_M  (HP_PERI1_PMS_READ_REE0_HP_UART0_V << HP_PERI1_PMS_READ_REE0_HP_UART0_S)
#define HP_PERI1_PMS_READ_REE0_HP_UART0_V  0x00000001U
#define HP_PERI1_PMS_READ_REE0_HP_UART0_S  1
/** HP_PERI1_PMS_READ_REE1_HP_UART0 : R/W; bitpos: [2]; default: 0;
 *  Configures hp_uart0 registers read permission in ree1 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI1_PMS_READ_REE1_HP_UART0    (BIT(2))
#define HP_PERI1_PMS_READ_REE1_HP_UART0_M  (HP_PERI1_PMS_READ_REE1_HP_UART0_V << HP_PERI1_PMS_READ_REE1_HP_UART0_S)
#define HP_PERI1_PMS_READ_REE1_HP_UART0_V  0x00000001U
#define HP_PERI1_PMS_READ_REE1_HP_UART0_S  2
/** HP_PERI1_PMS_READ_REE2_HP_UART0 : R/W; bitpos: [3]; default: 0;
 *  Configures hp_uart0 registers read permission in ree2 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI1_PMS_READ_REE2_HP_UART0    (BIT(3))
#define HP_PERI1_PMS_READ_REE2_HP_UART0_M  (HP_PERI1_PMS_READ_REE2_HP_UART0_V << HP_PERI1_PMS_READ_REE2_HP_UART0_S)
#define HP_PERI1_PMS_READ_REE2_HP_UART0_V  0x00000001U
#define HP_PERI1_PMS_READ_REE2_HP_UART0_S  3
/** HP_PERI1_PMS_WRITE_TEE_HP_UART0 : R/W; bitpos: [4]; default: 1;
 *  Configures hp_uart0 registers write permission in tee mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI1_PMS_WRITE_TEE_HP_UART0    (BIT(4))
#define HP_PERI1_PMS_WRITE_TEE_HP_UART0_M  (HP_PERI1_PMS_WRITE_TEE_HP_UART0_V << HP_PERI1_PMS_WRITE_TEE_HP_UART0_S)
#define HP_PERI1_PMS_WRITE_TEE_HP_UART0_V  0x00000001U
#define HP_PERI1_PMS_WRITE_TEE_HP_UART0_S  4
/** HP_PERI1_PMS_WRITE_REE0_HP_UART0 : R/W; bitpos: [5]; default: 0;
 *  Configures hp_uart0 registers write permission in ree0 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI1_PMS_WRITE_REE0_HP_UART0    (BIT(5))
#define HP_PERI1_PMS_WRITE_REE0_HP_UART0_M  (HP_PERI1_PMS_WRITE_REE0_HP_UART0_V << HP_PERI1_PMS_WRITE_REE0_HP_UART0_S)
#define HP_PERI1_PMS_WRITE_REE0_HP_UART0_V  0x00000001U
#define HP_PERI1_PMS_WRITE_REE0_HP_UART0_S  5
/** HP_PERI1_PMS_WRITE_REE1_HP_UART0 : R/W; bitpos: [6]; default: 0;
 *  Configures hp_uart0 registers write permission in ree1 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI1_PMS_WRITE_REE1_HP_UART0    (BIT(6))
#define HP_PERI1_PMS_WRITE_REE1_HP_UART0_M  (HP_PERI1_PMS_WRITE_REE1_HP_UART0_V << HP_PERI1_PMS_WRITE_REE1_HP_UART0_S)
#define HP_PERI1_PMS_WRITE_REE1_HP_UART0_V  0x00000001U
#define HP_PERI1_PMS_WRITE_REE1_HP_UART0_S  6
/** HP_PERI1_PMS_WRITE_REE2_HP_UART0 : R/W; bitpos: [7]; default: 0;
 *  Configures hp_uart0 registers write permission in ree2 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI1_PMS_WRITE_REE2_HP_UART0    (BIT(7))
#define HP_PERI1_PMS_WRITE_REE2_HP_UART0_M  (HP_PERI1_PMS_WRITE_REE2_HP_UART0_V << HP_PERI1_PMS_WRITE_REE2_HP_UART0_S)
#define HP_PERI1_PMS_WRITE_REE2_HP_UART0_V  0x00000001U
#define HP_PERI1_PMS_WRITE_REE2_HP_UART0_S  7
/** HP_PERI1_PMS_HP_UART0_LOCK : R/W; bitpos: [8]; default: 0;
 *  Set 1 to lock hp_uart0 peri_apm configuration
 */
#define HP_PERI1_PMS_HP_UART0_LOCK    (BIT(8))
#define HP_PERI1_PMS_HP_UART0_LOCK_M  (HP_PERI1_PMS_HP_UART0_LOCK_V << HP_PERI1_PMS_HP_UART0_LOCK_S)
#define HP_PERI1_PMS_HP_UART0_LOCK_V  0x00000001U
#define HP_PERI1_PMS_HP_UART0_LOCK_S  8

/** HP_PERI1_PMS_HP_UART1_CTRL_REG register
 *  hp_uart1 read/write control register
 */
#define HP_PERI1_PMS_HP_UART1_CTRL_REG (DR_REG_HP_PERI1_PMS_BASE + 0x2c)
/** HP_PERI1_PMS_READ_TEE_HP_UART1 : R/W; bitpos: [0]; default: 1;
 *  Configures hp_uart1 registers read permission in tee mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI1_PMS_READ_TEE_HP_UART1    (BIT(0))
#define HP_PERI1_PMS_READ_TEE_HP_UART1_M  (HP_PERI1_PMS_READ_TEE_HP_UART1_V << HP_PERI1_PMS_READ_TEE_HP_UART1_S)
#define HP_PERI1_PMS_READ_TEE_HP_UART1_V  0x00000001U
#define HP_PERI1_PMS_READ_TEE_HP_UART1_S  0
/** HP_PERI1_PMS_READ_REE0_HP_UART1 : R/W; bitpos: [1]; default: 0;
 *  Configures hp_uart1 registers read permission in ree0 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI1_PMS_READ_REE0_HP_UART1    (BIT(1))
#define HP_PERI1_PMS_READ_REE0_HP_UART1_M  (HP_PERI1_PMS_READ_REE0_HP_UART1_V << HP_PERI1_PMS_READ_REE0_HP_UART1_S)
#define HP_PERI1_PMS_READ_REE0_HP_UART1_V  0x00000001U
#define HP_PERI1_PMS_READ_REE0_HP_UART1_S  1
/** HP_PERI1_PMS_READ_REE1_HP_UART1 : R/W; bitpos: [2]; default: 0;
 *  Configures hp_uart1 registers read permission in ree1 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI1_PMS_READ_REE1_HP_UART1    (BIT(2))
#define HP_PERI1_PMS_READ_REE1_HP_UART1_M  (HP_PERI1_PMS_READ_REE1_HP_UART1_V << HP_PERI1_PMS_READ_REE1_HP_UART1_S)
#define HP_PERI1_PMS_READ_REE1_HP_UART1_V  0x00000001U
#define HP_PERI1_PMS_READ_REE1_HP_UART1_S  2
/** HP_PERI1_PMS_READ_REE2_HP_UART1 : R/W; bitpos: [3]; default: 0;
 *  Configures hp_uart1 registers read permission in ree2 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI1_PMS_READ_REE2_HP_UART1    (BIT(3))
#define HP_PERI1_PMS_READ_REE2_HP_UART1_M  (HP_PERI1_PMS_READ_REE2_HP_UART1_V << HP_PERI1_PMS_READ_REE2_HP_UART1_S)
#define HP_PERI1_PMS_READ_REE2_HP_UART1_V  0x00000001U
#define HP_PERI1_PMS_READ_REE2_HP_UART1_S  3
/** HP_PERI1_PMS_WRITE_TEE_HP_UART1 : R/W; bitpos: [4]; default: 1;
 *  Configures hp_uart1 registers write permission in tee mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI1_PMS_WRITE_TEE_HP_UART1    (BIT(4))
#define HP_PERI1_PMS_WRITE_TEE_HP_UART1_M  (HP_PERI1_PMS_WRITE_TEE_HP_UART1_V << HP_PERI1_PMS_WRITE_TEE_HP_UART1_S)
#define HP_PERI1_PMS_WRITE_TEE_HP_UART1_V  0x00000001U
#define HP_PERI1_PMS_WRITE_TEE_HP_UART1_S  4
/** HP_PERI1_PMS_WRITE_REE0_HP_UART1 : R/W; bitpos: [5]; default: 0;
 *  Configures hp_uart1 registers write permission in ree0 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI1_PMS_WRITE_REE0_HP_UART1    (BIT(5))
#define HP_PERI1_PMS_WRITE_REE0_HP_UART1_M  (HP_PERI1_PMS_WRITE_REE0_HP_UART1_V << HP_PERI1_PMS_WRITE_REE0_HP_UART1_S)
#define HP_PERI1_PMS_WRITE_REE0_HP_UART1_V  0x00000001U
#define HP_PERI1_PMS_WRITE_REE0_HP_UART1_S  5
/** HP_PERI1_PMS_WRITE_REE1_HP_UART1 : R/W; bitpos: [6]; default: 0;
 *  Configures hp_uart1 registers write permission in ree1 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI1_PMS_WRITE_REE1_HP_UART1    (BIT(6))
#define HP_PERI1_PMS_WRITE_REE1_HP_UART1_M  (HP_PERI1_PMS_WRITE_REE1_HP_UART1_V << HP_PERI1_PMS_WRITE_REE1_HP_UART1_S)
#define HP_PERI1_PMS_WRITE_REE1_HP_UART1_V  0x00000001U
#define HP_PERI1_PMS_WRITE_REE1_HP_UART1_S  6
/** HP_PERI1_PMS_WRITE_REE2_HP_UART1 : R/W; bitpos: [7]; default: 0;
 *  Configures hp_uart1 registers write permission in ree2 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI1_PMS_WRITE_REE2_HP_UART1    (BIT(7))
#define HP_PERI1_PMS_WRITE_REE2_HP_UART1_M  (HP_PERI1_PMS_WRITE_REE2_HP_UART1_V << HP_PERI1_PMS_WRITE_REE2_HP_UART1_S)
#define HP_PERI1_PMS_WRITE_REE2_HP_UART1_V  0x00000001U
#define HP_PERI1_PMS_WRITE_REE2_HP_UART1_S  7
/** HP_PERI1_PMS_HP_UART1_LOCK : R/W; bitpos: [8]; default: 0;
 *  Set 1 to lock hp_uart1 peri_apm configuration
 */
#define HP_PERI1_PMS_HP_UART1_LOCK    (BIT(8))
#define HP_PERI1_PMS_HP_UART1_LOCK_M  (HP_PERI1_PMS_HP_UART1_LOCK_V << HP_PERI1_PMS_HP_UART1_LOCK_S)
#define HP_PERI1_PMS_HP_UART1_LOCK_V  0x00000001U
#define HP_PERI1_PMS_HP_UART1_LOCK_S  8

/** HP_PERI1_PMS_HP_UART2_CTRL_REG register
 *  hp_uart2 read/write control register
 */
#define HP_PERI1_PMS_HP_UART2_CTRL_REG (DR_REG_HP_PERI1_PMS_BASE + 0x30)
/** HP_PERI1_PMS_READ_TEE_HP_UART2 : R/W; bitpos: [0]; default: 1;
 *  Configures hp_uart2 registers read permission in tee mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI1_PMS_READ_TEE_HP_UART2    (BIT(0))
#define HP_PERI1_PMS_READ_TEE_HP_UART2_M  (HP_PERI1_PMS_READ_TEE_HP_UART2_V << HP_PERI1_PMS_READ_TEE_HP_UART2_S)
#define HP_PERI1_PMS_READ_TEE_HP_UART2_V  0x00000001U
#define HP_PERI1_PMS_READ_TEE_HP_UART2_S  0
/** HP_PERI1_PMS_READ_REE0_HP_UART2 : R/W; bitpos: [1]; default: 0;
 *  Configures hp_uart2 registers read permission in ree0 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI1_PMS_READ_REE0_HP_UART2    (BIT(1))
#define HP_PERI1_PMS_READ_REE0_HP_UART2_M  (HP_PERI1_PMS_READ_REE0_HP_UART2_V << HP_PERI1_PMS_READ_REE0_HP_UART2_S)
#define HP_PERI1_PMS_READ_REE0_HP_UART2_V  0x00000001U
#define HP_PERI1_PMS_READ_REE0_HP_UART2_S  1
/** HP_PERI1_PMS_READ_REE1_HP_UART2 : R/W; bitpos: [2]; default: 0;
 *  Configures hp_uart2 registers read permission in ree1 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI1_PMS_READ_REE1_HP_UART2    (BIT(2))
#define HP_PERI1_PMS_READ_REE1_HP_UART2_M  (HP_PERI1_PMS_READ_REE1_HP_UART2_V << HP_PERI1_PMS_READ_REE1_HP_UART2_S)
#define HP_PERI1_PMS_READ_REE1_HP_UART2_V  0x00000001U
#define HP_PERI1_PMS_READ_REE1_HP_UART2_S  2
/** HP_PERI1_PMS_READ_REE2_HP_UART2 : R/W; bitpos: [3]; default: 0;
 *  Configures hp_uart2 registers read permission in ree2 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI1_PMS_READ_REE2_HP_UART2    (BIT(3))
#define HP_PERI1_PMS_READ_REE2_HP_UART2_M  (HP_PERI1_PMS_READ_REE2_HP_UART2_V << HP_PERI1_PMS_READ_REE2_HP_UART2_S)
#define HP_PERI1_PMS_READ_REE2_HP_UART2_V  0x00000001U
#define HP_PERI1_PMS_READ_REE2_HP_UART2_S  3
/** HP_PERI1_PMS_WRITE_TEE_HP_UART2 : R/W; bitpos: [4]; default: 1;
 *  Configures hp_uart2 registers write permission in tee mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI1_PMS_WRITE_TEE_HP_UART2    (BIT(4))
#define HP_PERI1_PMS_WRITE_TEE_HP_UART2_M  (HP_PERI1_PMS_WRITE_TEE_HP_UART2_V << HP_PERI1_PMS_WRITE_TEE_HP_UART2_S)
#define HP_PERI1_PMS_WRITE_TEE_HP_UART2_V  0x00000001U
#define HP_PERI1_PMS_WRITE_TEE_HP_UART2_S  4
/** HP_PERI1_PMS_WRITE_REE0_HP_UART2 : R/W; bitpos: [5]; default: 0;
 *  Configures hp_uart2 registers write permission in ree0 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI1_PMS_WRITE_REE0_HP_UART2    (BIT(5))
#define HP_PERI1_PMS_WRITE_REE0_HP_UART2_M  (HP_PERI1_PMS_WRITE_REE0_HP_UART2_V << HP_PERI1_PMS_WRITE_REE0_HP_UART2_S)
#define HP_PERI1_PMS_WRITE_REE0_HP_UART2_V  0x00000001U
#define HP_PERI1_PMS_WRITE_REE0_HP_UART2_S  5
/** HP_PERI1_PMS_WRITE_REE1_HP_UART2 : R/W; bitpos: [6]; default: 0;
 *  Configures hp_uart2 registers write permission in ree1 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI1_PMS_WRITE_REE1_HP_UART2    (BIT(6))
#define HP_PERI1_PMS_WRITE_REE1_HP_UART2_M  (HP_PERI1_PMS_WRITE_REE1_HP_UART2_V << HP_PERI1_PMS_WRITE_REE1_HP_UART2_S)
#define HP_PERI1_PMS_WRITE_REE1_HP_UART2_V  0x00000001U
#define HP_PERI1_PMS_WRITE_REE1_HP_UART2_S  6
/** HP_PERI1_PMS_WRITE_REE2_HP_UART2 : R/W; bitpos: [7]; default: 0;
 *  Configures hp_uart2 registers write permission in ree2 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI1_PMS_WRITE_REE2_HP_UART2    (BIT(7))
#define HP_PERI1_PMS_WRITE_REE2_HP_UART2_M  (HP_PERI1_PMS_WRITE_REE2_HP_UART2_V << HP_PERI1_PMS_WRITE_REE2_HP_UART2_S)
#define HP_PERI1_PMS_WRITE_REE2_HP_UART2_V  0x00000001U
#define HP_PERI1_PMS_WRITE_REE2_HP_UART2_S  7
/** HP_PERI1_PMS_HP_UART2_LOCK : R/W; bitpos: [8]; default: 0;
 *  Set 1 to lock hp_uart2 peri_apm configuration
 */
#define HP_PERI1_PMS_HP_UART2_LOCK    (BIT(8))
#define HP_PERI1_PMS_HP_UART2_LOCK_M  (HP_PERI1_PMS_HP_UART2_LOCK_V << HP_PERI1_PMS_HP_UART2_LOCK_S)
#define HP_PERI1_PMS_HP_UART2_LOCK_V  0x00000001U
#define HP_PERI1_PMS_HP_UART2_LOCK_S  8

/** HP_PERI1_PMS_HP_UART3_CTRL_REG register
 *  hp_uart3 read/write control register
 */
#define HP_PERI1_PMS_HP_UART3_CTRL_REG (DR_REG_HP_PERI1_PMS_BASE + 0x34)
/** HP_PERI1_PMS_READ_TEE_HP_UART3 : R/W; bitpos: [0]; default: 1;
 *  Configures hp_uart3 registers read permission in tee mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI1_PMS_READ_TEE_HP_UART3    (BIT(0))
#define HP_PERI1_PMS_READ_TEE_HP_UART3_M  (HP_PERI1_PMS_READ_TEE_HP_UART3_V << HP_PERI1_PMS_READ_TEE_HP_UART3_S)
#define HP_PERI1_PMS_READ_TEE_HP_UART3_V  0x00000001U
#define HP_PERI1_PMS_READ_TEE_HP_UART3_S  0
/** HP_PERI1_PMS_READ_REE0_HP_UART3 : R/W; bitpos: [1]; default: 0;
 *  Configures hp_uart3 registers read permission in ree0 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI1_PMS_READ_REE0_HP_UART3    (BIT(1))
#define HP_PERI1_PMS_READ_REE0_HP_UART3_M  (HP_PERI1_PMS_READ_REE0_HP_UART3_V << HP_PERI1_PMS_READ_REE0_HP_UART3_S)
#define HP_PERI1_PMS_READ_REE0_HP_UART3_V  0x00000001U
#define HP_PERI1_PMS_READ_REE0_HP_UART3_S  1
/** HP_PERI1_PMS_READ_REE1_HP_UART3 : R/W; bitpos: [2]; default: 0;
 *  Configures hp_uart3 registers read permission in ree1 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI1_PMS_READ_REE1_HP_UART3    (BIT(2))
#define HP_PERI1_PMS_READ_REE1_HP_UART3_M  (HP_PERI1_PMS_READ_REE1_HP_UART3_V << HP_PERI1_PMS_READ_REE1_HP_UART3_S)
#define HP_PERI1_PMS_READ_REE1_HP_UART3_V  0x00000001U
#define HP_PERI1_PMS_READ_REE1_HP_UART3_S  2
/** HP_PERI1_PMS_READ_REE2_HP_UART3 : R/W; bitpos: [3]; default: 0;
 *  Configures hp_uart3 registers read permission in ree2 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI1_PMS_READ_REE2_HP_UART3    (BIT(3))
#define HP_PERI1_PMS_READ_REE2_HP_UART3_M  (HP_PERI1_PMS_READ_REE2_HP_UART3_V << HP_PERI1_PMS_READ_REE2_HP_UART3_S)
#define HP_PERI1_PMS_READ_REE2_HP_UART3_V  0x00000001U
#define HP_PERI1_PMS_READ_REE2_HP_UART3_S  3
/** HP_PERI1_PMS_WRITE_TEE_HP_UART3 : R/W; bitpos: [4]; default: 1;
 *  Configures hp_uart3 registers write permission in tee mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI1_PMS_WRITE_TEE_HP_UART3    (BIT(4))
#define HP_PERI1_PMS_WRITE_TEE_HP_UART3_M  (HP_PERI1_PMS_WRITE_TEE_HP_UART3_V << HP_PERI1_PMS_WRITE_TEE_HP_UART3_S)
#define HP_PERI1_PMS_WRITE_TEE_HP_UART3_V  0x00000001U
#define HP_PERI1_PMS_WRITE_TEE_HP_UART3_S  4
/** HP_PERI1_PMS_WRITE_REE0_HP_UART3 : R/W; bitpos: [5]; default: 0;
 *  Configures hp_uart3 registers write permission in ree0 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI1_PMS_WRITE_REE0_HP_UART3    (BIT(5))
#define HP_PERI1_PMS_WRITE_REE0_HP_UART3_M  (HP_PERI1_PMS_WRITE_REE0_HP_UART3_V << HP_PERI1_PMS_WRITE_REE0_HP_UART3_S)
#define HP_PERI1_PMS_WRITE_REE0_HP_UART3_V  0x00000001U
#define HP_PERI1_PMS_WRITE_REE0_HP_UART3_S  5
/** HP_PERI1_PMS_WRITE_REE1_HP_UART3 : R/W; bitpos: [6]; default: 0;
 *  Configures hp_uart3 registers write permission in ree1 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI1_PMS_WRITE_REE1_HP_UART3    (BIT(6))
#define HP_PERI1_PMS_WRITE_REE1_HP_UART3_M  (HP_PERI1_PMS_WRITE_REE1_HP_UART3_V << HP_PERI1_PMS_WRITE_REE1_HP_UART3_S)
#define HP_PERI1_PMS_WRITE_REE1_HP_UART3_V  0x00000001U
#define HP_PERI1_PMS_WRITE_REE1_HP_UART3_S  6
/** HP_PERI1_PMS_WRITE_REE2_HP_UART3 : R/W; bitpos: [7]; default: 0;
 *  Configures hp_uart3 registers write permission in ree2 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI1_PMS_WRITE_REE2_HP_UART3    (BIT(7))
#define HP_PERI1_PMS_WRITE_REE2_HP_UART3_M  (HP_PERI1_PMS_WRITE_REE2_HP_UART3_V << HP_PERI1_PMS_WRITE_REE2_HP_UART3_S)
#define HP_PERI1_PMS_WRITE_REE2_HP_UART3_V  0x00000001U
#define HP_PERI1_PMS_WRITE_REE2_HP_UART3_S  7
/** HP_PERI1_PMS_HP_UART3_LOCK : R/W; bitpos: [8]; default: 0;
 *  Set 1 to lock hp_uart3 peri_apm configuration
 */
#define HP_PERI1_PMS_HP_UART3_LOCK    (BIT(8))
#define HP_PERI1_PMS_HP_UART3_LOCK_M  (HP_PERI1_PMS_HP_UART3_LOCK_V << HP_PERI1_PMS_HP_UART3_LOCK_S)
#define HP_PERI1_PMS_HP_UART3_LOCK_V  0x00000001U
#define HP_PERI1_PMS_HP_UART3_LOCK_S  8

/** HP_PERI1_PMS_HP_PARLIO_CTRL_REG register
 *  hp_parlio read/write control register
 */
#define HP_PERI1_PMS_HP_PARLIO_CTRL_REG (DR_REG_HP_PERI1_PMS_BASE + 0x38)
/** HP_PERI1_PMS_READ_TEE_HP_PARLIO : R/W; bitpos: [0]; default: 1;
 *  Configures hp_parlio registers read permission in tee mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI1_PMS_READ_TEE_HP_PARLIO    (BIT(0))
#define HP_PERI1_PMS_READ_TEE_HP_PARLIO_M  (HP_PERI1_PMS_READ_TEE_HP_PARLIO_V << HP_PERI1_PMS_READ_TEE_HP_PARLIO_S)
#define HP_PERI1_PMS_READ_TEE_HP_PARLIO_V  0x00000001U
#define HP_PERI1_PMS_READ_TEE_HP_PARLIO_S  0
/** HP_PERI1_PMS_READ_REE0_HP_PARLIO : R/W; bitpos: [1]; default: 0;
 *  Configures hp_parlio registers read permission in ree0 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI1_PMS_READ_REE0_HP_PARLIO    (BIT(1))
#define HP_PERI1_PMS_READ_REE0_HP_PARLIO_M  (HP_PERI1_PMS_READ_REE0_HP_PARLIO_V << HP_PERI1_PMS_READ_REE0_HP_PARLIO_S)
#define HP_PERI1_PMS_READ_REE0_HP_PARLIO_V  0x00000001U
#define HP_PERI1_PMS_READ_REE0_HP_PARLIO_S  1
/** HP_PERI1_PMS_READ_REE1_HP_PARLIO : R/W; bitpos: [2]; default: 0;
 *  Configures hp_parlio registers read permission in ree1 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI1_PMS_READ_REE1_HP_PARLIO    (BIT(2))
#define HP_PERI1_PMS_READ_REE1_HP_PARLIO_M  (HP_PERI1_PMS_READ_REE1_HP_PARLIO_V << HP_PERI1_PMS_READ_REE1_HP_PARLIO_S)
#define HP_PERI1_PMS_READ_REE1_HP_PARLIO_V  0x00000001U
#define HP_PERI1_PMS_READ_REE1_HP_PARLIO_S  2
/** HP_PERI1_PMS_READ_REE2_HP_PARLIO : R/W; bitpos: [3]; default: 0;
 *  Configures hp_parlio registers read permission in ree2 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI1_PMS_READ_REE2_HP_PARLIO    (BIT(3))
#define HP_PERI1_PMS_READ_REE2_HP_PARLIO_M  (HP_PERI1_PMS_READ_REE2_HP_PARLIO_V << HP_PERI1_PMS_READ_REE2_HP_PARLIO_S)
#define HP_PERI1_PMS_READ_REE2_HP_PARLIO_V  0x00000001U
#define HP_PERI1_PMS_READ_REE2_HP_PARLIO_S  3
/** HP_PERI1_PMS_WRITE_TEE_HP_PARLIO : R/W; bitpos: [4]; default: 1;
 *  Configures hp_parlio registers write permission in tee mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI1_PMS_WRITE_TEE_HP_PARLIO    (BIT(4))
#define HP_PERI1_PMS_WRITE_TEE_HP_PARLIO_M  (HP_PERI1_PMS_WRITE_TEE_HP_PARLIO_V << HP_PERI1_PMS_WRITE_TEE_HP_PARLIO_S)
#define HP_PERI1_PMS_WRITE_TEE_HP_PARLIO_V  0x00000001U
#define HP_PERI1_PMS_WRITE_TEE_HP_PARLIO_S  4
/** HP_PERI1_PMS_WRITE_REE0_HP_PARLIO : R/W; bitpos: [5]; default: 0;
 *  Configures hp_parlio registers write permission in ree0 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI1_PMS_WRITE_REE0_HP_PARLIO    (BIT(5))
#define HP_PERI1_PMS_WRITE_REE0_HP_PARLIO_M  (HP_PERI1_PMS_WRITE_REE0_HP_PARLIO_V << HP_PERI1_PMS_WRITE_REE0_HP_PARLIO_S)
#define HP_PERI1_PMS_WRITE_REE0_HP_PARLIO_V  0x00000001U
#define HP_PERI1_PMS_WRITE_REE0_HP_PARLIO_S  5
/** HP_PERI1_PMS_WRITE_REE1_HP_PARLIO : R/W; bitpos: [6]; default: 0;
 *  Configures hp_parlio registers write permission in ree1 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI1_PMS_WRITE_REE1_HP_PARLIO    (BIT(6))
#define HP_PERI1_PMS_WRITE_REE1_HP_PARLIO_M  (HP_PERI1_PMS_WRITE_REE1_HP_PARLIO_V << HP_PERI1_PMS_WRITE_REE1_HP_PARLIO_S)
#define HP_PERI1_PMS_WRITE_REE1_HP_PARLIO_V  0x00000001U
#define HP_PERI1_PMS_WRITE_REE1_HP_PARLIO_S  6
/** HP_PERI1_PMS_WRITE_REE2_HP_PARLIO : R/W; bitpos: [7]; default: 0;
 *  Configures hp_parlio registers write permission in ree2 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI1_PMS_WRITE_REE2_HP_PARLIO    (BIT(7))
#define HP_PERI1_PMS_WRITE_REE2_HP_PARLIO_M  (HP_PERI1_PMS_WRITE_REE2_HP_PARLIO_V << HP_PERI1_PMS_WRITE_REE2_HP_PARLIO_S)
#define HP_PERI1_PMS_WRITE_REE2_HP_PARLIO_V  0x00000001U
#define HP_PERI1_PMS_WRITE_REE2_HP_PARLIO_S  7
/** HP_PERI1_PMS_HP_PARLIO_LOCK : R/W; bitpos: [8]; default: 0;
 *  Set 1 to lock hp_parlio peri_apm configuration
 */
#define HP_PERI1_PMS_HP_PARLIO_LOCK    (BIT(8))
#define HP_PERI1_PMS_HP_PARLIO_LOCK_M  (HP_PERI1_PMS_HP_PARLIO_LOCK_V << HP_PERI1_PMS_HP_PARLIO_LOCK_S)
#define HP_PERI1_PMS_HP_PARLIO_LOCK_V  0x00000001U
#define HP_PERI1_PMS_HP_PARLIO_LOCK_S  8

/** HP_PERI1_PMS_HP_GPSPI2_CTRL_REG register
 *  hp_gpspi2 read/write control register
 */
#define HP_PERI1_PMS_HP_GPSPI2_CTRL_REG (DR_REG_HP_PERI1_PMS_BASE + 0x3c)
/** HP_PERI1_PMS_READ_TEE_HP_GPSPI2 : R/W; bitpos: [0]; default: 1;
 *  Configures hp_gpspi2 registers read permission in tee mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI1_PMS_READ_TEE_HP_GPSPI2    (BIT(0))
#define HP_PERI1_PMS_READ_TEE_HP_GPSPI2_M  (HP_PERI1_PMS_READ_TEE_HP_GPSPI2_V << HP_PERI1_PMS_READ_TEE_HP_GPSPI2_S)
#define HP_PERI1_PMS_READ_TEE_HP_GPSPI2_V  0x00000001U
#define HP_PERI1_PMS_READ_TEE_HP_GPSPI2_S  0
/** HP_PERI1_PMS_READ_REE0_HP_GPSPI2 : R/W; bitpos: [1]; default: 0;
 *  Configures hp_gpspi2 registers read permission in ree0 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI1_PMS_READ_REE0_HP_GPSPI2    (BIT(1))
#define HP_PERI1_PMS_READ_REE0_HP_GPSPI2_M  (HP_PERI1_PMS_READ_REE0_HP_GPSPI2_V << HP_PERI1_PMS_READ_REE0_HP_GPSPI2_S)
#define HP_PERI1_PMS_READ_REE0_HP_GPSPI2_V  0x00000001U
#define HP_PERI1_PMS_READ_REE0_HP_GPSPI2_S  1
/** HP_PERI1_PMS_READ_REE1_HP_GPSPI2 : R/W; bitpos: [2]; default: 0;
 *  Configures hp_gpspi2 registers read permission in ree1 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI1_PMS_READ_REE1_HP_GPSPI2    (BIT(2))
#define HP_PERI1_PMS_READ_REE1_HP_GPSPI2_M  (HP_PERI1_PMS_READ_REE1_HP_GPSPI2_V << HP_PERI1_PMS_READ_REE1_HP_GPSPI2_S)
#define HP_PERI1_PMS_READ_REE1_HP_GPSPI2_V  0x00000001U
#define HP_PERI1_PMS_READ_REE1_HP_GPSPI2_S  2
/** HP_PERI1_PMS_READ_REE2_HP_GPSPI2 : R/W; bitpos: [3]; default: 0;
 *  Configures hp_gpspi2 registers read permission in ree2 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI1_PMS_READ_REE2_HP_GPSPI2    (BIT(3))
#define HP_PERI1_PMS_READ_REE2_HP_GPSPI2_M  (HP_PERI1_PMS_READ_REE2_HP_GPSPI2_V << HP_PERI1_PMS_READ_REE2_HP_GPSPI2_S)
#define HP_PERI1_PMS_READ_REE2_HP_GPSPI2_V  0x00000001U
#define HP_PERI1_PMS_READ_REE2_HP_GPSPI2_S  3
/** HP_PERI1_PMS_WRITE_TEE_HP_GPSPI2 : R/W; bitpos: [4]; default: 1;
 *  Configures hp_gpspi2 registers write permission in tee mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI1_PMS_WRITE_TEE_HP_GPSPI2    (BIT(4))
#define HP_PERI1_PMS_WRITE_TEE_HP_GPSPI2_M  (HP_PERI1_PMS_WRITE_TEE_HP_GPSPI2_V << HP_PERI1_PMS_WRITE_TEE_HP_GPSPI2_S)
#define HP_PERI1_PMS_WRITE_TEE_HP_GPSPI2_V  0x00000001U
#define HP_PERI1_PMS_WRITE_TEE_HP_GPSPI2_S  4
/** HP_PERI1_PMS_WRITE_REE0_HP_GPSPI2 : R/W; bitpos: [5]; default: 0;
 *  Configures hp_gpspi2 registers write permission in ree0 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI1_PMS_WRITE_REE0_HP_GPSPI2    (BIT(5))
#define HP_PERI1_PMS_WRITE_REE0_HP_GPSPI2_M  (HP_PERI1_PMS_WRITE_REE0_HP_GPSPI2_V << HP_PERI1_PMS_WRITE_REE0_HP_GPSPI2_S)
#define HP_PERI1_PMS_WRITE_REE0_HP_GPSPI2_V  0x00000001U
#define HP_PERI1_PMS_WRITE_REE0_HP_GPSPI2_S  5
/** HP_PERI1_PMS_WRITE_REE1_HP_GPSPI2 : R/W; bitpos: [6]; default: 0;
 *  Configures hp_gpspi2 registers write permission in ree1 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI1_PMS_WRITE_REE1_HP_GPSPI2    (BIT(6))
#define HP_PERI1_PMS_WRITE_REE1_HP_GPSPI2_M  (HP_PERI1_PMS_WRITE_REE1_HP_GPSPI2_V << HP_PERI1_PMS_WRITE_REE1_HP_GPSPI2_S)
#define HP_PERI1_PMS_WRITE_REE1_HP_GPSPI2_V  0x00000001U
#define HP_PERI1_PMS_WRITE_REE1_HP_GPSPI2_S  6
/** HP_PERI1_PMS_WRITE_REE2_HP_GPSPI2 : R/W; bitpos: [7]; default: 0;
 *  Configures hp_gpspi2 registers write permission in ree2 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI1_PMS_WRITE_REE2_HP_GPSPI2    (BIT(7))
#define HP_PERI1_PMS_WRITE_REE2_HP_GPSPI2_M  (HP_PERI1_PMS_WRITE_REE2_HP_GPSPI2_V << HP_PERI1_PMS_WRITE_REE2_HP_GPSPI2_S)
#define HP_PERI1_PMS_WRITE_REE2_HP_GPSPI2_V  0x00000001U
#define HP_PERI1_PMS_WRITE_REE2_HP_GPSPI2_S  7
/** HP_PERI1_PMS_HP_GPSPI2_LOCK : R/W; bitpos: [8]; default: 0;
 *  Set 1 to lock hp_gpspi2 peri_apm configuration
 */
#define HP_PERI1_PMS_HP_GPSPI2_LOCK    (BIT(8))
#define HP_PERI1_PMS_HP_GPSPI2_LOCK_M  (HP_PERI1_PMS_HP_GPSPI2_LOCK_V << HP_PERI1_PMS_HP_GPSPI2_LOCK_S)
#define HP_PERI1_PMS_HP_GPSPI2_LOCK_V  0x00000001U
#define HP_PERI1_PMS_HP_GPSPI2_LOCK_S  8

/** HP_PERI1_PMS_HP_GPSPI3_CTRL_REG register
 *  hp_gpspi3 read/write control register
 */
#define HP_PERI1_PMS_HP_GPSPI3_CTRL_REG (DR_REG_HP_PERI1_PMS_BASE + 0x40)
/** HP_PERI1_PMS_READ_TEE_HP_GPSPI3 : R/W; bitpos: [0]; default: 1;
 *  Configures hp_gpspi3 registers read permission in tee mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI1_PMS_READ_TEE_HP_GPSPI3    (BIT(0))
#define HP_PERI1_PMS_READ_TEE_HP_GPSPI3_M  (HP_PERI1_PMS_READ_TEE_HP_GPSPI3_V << HP_PERI1_PMS_READ_TEE_HP_GPSPI3_S)
#define HP_PERI1_PMS_READ_TEE_HP_GPSPI3_V  0x00000001U
#define HP_PERI1_PMS_READ_TEE_HP_GPSPI3_S  0
/** HP_PERI1_PMS_READ_REE0_HP_GPSPI3 : R/W; bitpos: [1]; default: 0;
 *  Configures hp_gpspi3 registers read permission in ree0 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI1_PMS_READ_REE0_HP_GPSPI3    (BIT(1))
#define HP_PERI1_PMS_READ_REE0_HP_GPSPI3_M  (HP_PERI1_PMS_READ_REE0_HP_GPSPI3_V << HP_PERI1_PMS_READ_REE0_HP_GPSPI3_S)
#define HP_PERI1_PMS_READ_REE0_HP_GPSPI3_V  0x00000001U
#define HP_PERI1_PMS_READ_REE0_HP_GPSPI3_S  1
/** HP_PERI1_PMS_READ_REE1_HP_GPSPI3 : R/W; bitpos: [2]; default: 0;
 *  Configures hp_gpspi3 registers read permission in ree1 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI1_PMS_READ_REE1_HP_GPSPI3    (BIT(2))
#define HP_PERI1_PMS_READ_REE1_HP_GPSPI3_M  (HP_PERI1_PMS_READ_REE1_HP_GPSPI3_V << HP_PERI1_PMS_READ_REE1_HP_GPSPI3_S)
#define HP_PERI1_PMS_READ_REE1_HP_GPSPI3_V  0x00000001U
#define HP_PERI1_PMS_READ_REE1_HP_GPSPI3_S  2
/** HP_PERI1_PMS_READ_REE2_HP_GPSPI3 : R/W; bitpos: [3]; default: 0;
 *  Configures hp_gpspi3 registers read permission in ree2 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI1_PMS_READ_REE2_HP_GPSPI3    (BIT(3))
#define HP_PERI1_PMS_READ_REE2_HP_GPSPI3_M  (HP_PERI1_PMS_READ_REE2_HP_GPSPI3_V << HP_PERI1_PMS_READ_REE2_HP_GPSPI3_S)
#define HP_PERI1_PMS_READ_REE2_HP_GPSPI3_V  0x00000001U
#define HP_PERI1_PMS_READ_REE2_HP_GPSPI3_S  3
/** HP_PERI1_PMS_WRITE_TEE_HP_GPSPI3 : R/W; bitpos: [4]; default: 1;
 *  Configures hp_gpspi3 registers write permission in tee mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI1_PMS_WRITE_TEE_HP_GPSPI3    (BIT(4))
#define HP_PERI1_PMS_WRITE_TEE_HP_GPSPI3_M  (HP_PERI1_PMS_WRITE_TEE_HP_GPSPI3_V << HP_PERI1_PMS_WRITE_TEE_HP_GPSPI3_S)
#define HP_PERI1_PMS_WRITE_TEE_HP_GPSPI3_V  0x00000001U
#define HP_PERI1_PMS_WRITE_TEE_HP_GPSPI3_S  4
/** HP_PERI1_PMS_WRITE_REE0_HP_GPSPI3 : R/W; bitpos: [5]; default: 0;
 *  Configures hp_gpspi3 registers write permission in ree0 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI1_PMS_WRITE_REE0_HP_GPSPI3    (BIT(5))
#define HP_PERI1_PMS_WRITE_REE0_HP_GPSPI3_M  (HP_PERI1_PMS_WRITE_REE0_HP_GPSPI3_V << HP_PERI1_PMS_WRITE_REE0_HP_GPSPI3_S)
#define HP_PERI1_PMS_WRITE_REE0_HP_GPSPI3_V  0x00000001U
#define HP_PERI1_PMS_WRITE_REE0_HP_GPSPI3_S  5
/** HP_PERI1_PMS_WRITE_REE1_HP_GPSPI3 : R/W; bitpos: [6]; default: 0;
 *  Configures hp_gpspi3 registers write permission in ree1 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI1_PMS_WRITE_REE1_HP_GPSPI3    (BIT(6))
#define HP_PERI1_PMS_WRITE_REE1_HP_GPSPI3_M  (HP_PERI1_PMS_WRITE_REE1_HP_GPSPI3_V << HP_PERI1_PMS_WRITE_REE1_HP_GPSPI3_S)
#define HP_PERI1_PMS_WRITE_REE1_HP_GPSPI3_V  0x00000001U
#define HP_PERI1_PMS_WRITE_REE1_HP_GPSPI3_S  6
/** HP_PERI1_PMS_WRITE_REE2_HP_GPSPI3 : R/W; bitpos: [7]; default: 0;
 *  Configures hp_gpspi3 registers write permission in ree2 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI1_PMS_WRITE_REE2_HP_GPSPI3    (BIT(7))
#define HP_PERI1_PMS_WRITE_REE2_HP_GPSPI3_M  (HP_PERI1_PMS_WRITE_REE2_HP_GPSPI3_V << HP_PERI1_PMS_WRITE_REE2_HP_GPSPI3_S)
#define HP_PERI1_PMS_WRITE_REE2_HP_GPSPI3_V  0x00000001U
#define HP_PERI1_PMS_WRITE_REE2_HP_GPSPI3_S  7
/** HP_PERI1_PMS_HP_GPSPI3_LOCK : R/W; bitpos: [8]; default: 0;
 *  Set 1 to lock hp_gpspi3 peri_apm configuration
 */
#define HP_PERI1_PMS_HP_GPSPI3_LOCK    (BIT(8))
#define HP_PERI1_PMS_HP_GPSPI3_LOCK_M  (HP_PERI1_PMS_HP_GPSPI3_LOCK_V << HP_PERI1_PMS_HP_GPSPI3_LOCK_S)
#define HP_PERI1_PMS_HP_GPSPI3_LOCK_V  0x00000001U
#define HP_PERI1_PMS_HP_GPSPI3_LOCK_S  8

/** HP_PERI1_PMS_HP_USBDEVICE_CTRL_REG register
 *  hp_usbdevice read/write control register
 */
#define HP_PERI1_PMS_HP_USBDEVICE_CTRL_REG (DR_REG_HP_PERI1_PMS_BASE + 0x44)
/** HP_PERI1_PMS_READ_TEE_HP_USBDEVICE : R/W; bitpos: [0]; default: 1;
 *  Configures hp_usbdevice registers read permission in tee mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI1_PMS_READ_TEE_HP_USBDEVICE    (BIT(0))
#define HP_PERI1_PMS_READ_TEE_HP_USBDEVICE_M  (HP_PERI1_PMS_READ_TEE_HP_USBDEVICE_V << HP_PERI1_PMS_READ_TEE_HP_USBDEVICE_S)
#define HP_PERI1_PMS_READ_TEE_HP_USBDEVICE_V  0x00000001U
#define HP_PERI1_PMS_READ_TEE_HP_USBDEVICE_S  0
/** HP_PERI1_PMS_READ_REE0_HP_USBDEVICE : R/W; bitpos: [1]; default: 0;
 *  Configures hp_usbdevice registers read permission in ree0 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI1_PMS_READ_REE0_HP_USBDEVICE    (BIT(1))
#define HP_PERI1_PMS_READ_REE0_HP_USBDEVICE_M  (HP_PERI1_PMS_READ_REE0_HP_USBDEVICE_V << HP_PERI1_PMS_READ_REE0_HP_USBDEVICE_S)
#define HP_PERI1_PMS_READ_REE0_HP_USBDEVICE_V  0x00000001U
#define HP_PERI1_PMS_READ_REE0_HP_USBDEVICE_S  1
/** HP_PERI1_PMS_READ_REE1_HP_USBDEVICE : R/W; bitpos: [2]; default: 0;
 *  Configures hp_usbdevice registers read permission in ree1 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI1_PMS_READ_REE1_HP_USBDEVICE    (BIT(2))
#define HP_PERI1_PMS_READ_REE1_HP_USBDEVICE_M  (HP_PERI1_PMS_READ_REE1_HP_USBDEVICE_V << HP_PERI1_PMS_READ_REE1_HP_USBDEVICE_S)
#define HP_PERI1_PMS_READ_REE1_HP_USBDEVICE_V  0x00000001U
#define HP_PERI1_PMS_READ_REE1_HP_USBDEVICE_S  2
/** HP_PERI1_PMS_READ_REE2_HP_USBDEVICE : R/W; bitpos: [3]; default: 0;
 *  Configures hp_usbdevice registers read permission in ree2 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI1_PMS_READ_REE2_HP_USBDEVICE    (BIT(3))
#define HP_PERI1_PMS_READ_REE2_HP_USBDEVICE_M  (HP_PERI1_PMS_READ_REE2_HP_USBDEVICE_V << HP_PERI1_PMS_READ_REE2_HP_USBDEVICE_S)
#define HP_PERI1_PMS_READ_REE2_HP_USBDEVICE_V  0x00000001U
#define HP_PERI1_PMS_READ_REE2_HP_USBDEVICE_S  3
/** HP_PERI1_PMS_WRITE_TEE_HP_USBDEVICE : R/W; bitpos: [4]; default: 1;
 *  Configures hp_usbdevice registers write permission in tee mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI1_PMS_WRITE_TEE_HP_USBDEVICE    (BIT(4))
#define HP_PERI1_PMS_WRITE_TEE_HP_USBDEVICE_M  (HP_PERI1_PMS_WRITE_TEE_HP_USBDEVICE_V << HP_PERI1_PMS_WRITE_TEE_HP_USBDEVICE_S)
#define HP_PERI1_PMS_WRITE_TEE_HP_USBDEVICE_V  0x00000001U
#define HP_PERI1_PMS_WRITE_TEE_HP_USBDEVICE_S  4
/** HP_PERI1_PMS_WRITE_REE0_HP_USBDEVICE : R/W; bitpos: [5]; default: 0;
 *  Configures hp_usbdevice registers write permission in ree0 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI1_PMS_WRITE_REE0_HP_USBDEVICE    (BIT(5))
#define HP_PERI1_PMS_WRITE_REE0_HP_USBDEVICE_M  (HP_PERI1_PMS_WRITE_REE0_HP_USBDEVICE_V << HP_PERI1_PMS_WRITE_REE0_HP_USBDEVICE_S)
#define HP_PERI1_PMS_WRITE_REE0_HP_USBDEVICE_V  0x00000001U
#define HP_PERI1_PMS_WRITE_REE0_HP_USBDEVICE_S  5
/** HP_PERI1_PMS_WRITE_REE1_HP_USBDEVICE : R/W; bitpos: [6]; default: 0;
 *  Configures hp_usbdevice registers write permission in ree1 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI1_PMS_WRITE_REE1_HP_USBDEVICE    (BIT(6))
#define HP_PERI1_PMS_WRITE_REE1_HP_USBDEVICE_M  (HP_PERI1_PMS_WRITE_REE1_HP_USBDEVICE_V << HP_PERI1_PMS_WRITE_REE1_HP_USBDEVICE_S)
#define HP_PERI1_PMS_WRITE_REE1_HP_USBDEVICE_V  0x00000001U
#define HP_PERI1_PMS_WRITE_REE1_HP_USBDEVICE_S  6
/** HP_PERI1_PMS_WRITE_REE2_HP_USBDEVICE : R/W; bitpos: [7]; default: 0;
 *  Configures hp_usbdevice registers write permission in ree2 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI1_PMS_WRITE_REE2_HP_USBDEVICE    (BIT(7))
#define HP_PERI1_PMS_WRITE_REE2_HP_USBDEVICE_M  (HP_PERI1_PMS_WRITE_REE2_HP_USBDEVICE_V << HP_PERI1_PMS_WRITE_REE2_HP_USBDEVICE_S)
#define HP_PERI1_PMS_WRITE_REE2_HP_USBDEVICE_V  0x00000001U
#define HP_PERI1_PMS_WRITE_REE2_HP_USBDEVICE_S  7
/** HP_PERI1_PMS_HP_USBDEVICE_LOCK : R/W; bitpos: [8]; default: 0;
 *  Set 1 to lock hp_usbdevice peri_apm configuration
 */
#define HP_PERI1_PMS_HP_USBDEVICE_LOCK    (BIT(8))
#define HP_PERI1_PMS_HP_USBDEVICE_LOCK_M  (HP_PERI1_PMS_HP_USBDEVICE_LOCK_V << HP_PERI1_PMS_HP_USBDEVICE_LOCK_S)
#define HP_PERI1_PMS_HP_USBDEVICE_LOCK_V  0x00000001U
#define HP_PERI1_PMS_HP_USBDEVICE_LOCK_S  8

/** HP_PERI1_PMS_HP_LEDC0_CTRL_REG register
 *  hp_ledc0 read/write control register
 */
#define HP_PERI1_PMS_HP_LEDC0_CTRL_REG (DR_REG_HP_PERI1_PMS_BASE + 0x48)
/** HP_PERI1_PMS_READ_TEE_HP_LEDC0 : R/W; bitpos: [0]; default: 1;
 *  Configures hp_ledc0 registers read permission in tee mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI1_PMS_READ_TEE_HP_LEDC0    (BIT(0))
#define HP_PERI1_PMS_READ_TEE_HP_LEDC0_M  (HP_PERI1_PMS_READ_TEE_HP_LEDC0_V << HP_PERI1_PMS_READ_TEE_HP_LEDC0_S)
#define HP_PERI1_PMS_READ_TEE_HP_LEDC0_V  0x00000001U
#define HP_PERI1_PMS_READ_TEE_HP_LEDC0_S  0
/** HP_PERI1_PMS_READ_REE0_HP_LEDC0 : R/W; bitpos: [1]; default: 0;
 *  Configures hp_ledc0 registers read permission in ree0 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI1_PMS_READ_REE0_HP_LEDC0    (BIT(1))
#define HP_PERI1_PMS_READ_REE0_HP_LEDC0_M  (HP_PERI1_PMS_READ_REE0_HP_LEDC0_V << HP_PERI1_PMS_READ_REE0_HP_LEDC0_S)
#define HP_PERI1_PMS_READ_REE0_HP_LEDC0_V  0x00000001U
#define HP_PERI1_PMS_READ_REE0_HP_LEDC0_S  1
/** HP_PERI1_PMS_READ_REE1_HP_LEDC0 : R/W; bitpos: [2]; default: 0;
 *  Configures hp_ledc0 registers read permission in ree1 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI1_PMS_READ_REE1_HP_LEDC0    (BIT(2))
#define HP_PERI1_PMS_READ_REE1_HP_LEDC0_M  (HP_PERI1_PMS_READ_REE1_HP_LEDC0_V << HP_PERI1_PMS_READ_REE1_HP_LEDC0_S)
#define HP_PERI1_PMS_READ_REE1_HP_LEDC0_V  0x00000001U
#define HP_PERI1_PMS_READ_REE1_HP_LEDC0_S  2
/** HP_PERI1_PMS_READ_REE2_HP_LEDC0 : R/W; bitpos: [3]; default: 0;
 *  Configures hp_ledc0 registers read permission in ree2 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI1_PMS_READ_REE2_HP_LEDC0    (BIT(3))
#define HP_PERI1_PMS_READ_REE2_HP_LEDC0_M  (HP_PERI1_PMS_READ_REE2_HP_LEDC0_V << HP_PERI1_PMS_READ_REE2_HP_LEDC0_S)
#define HP_PERI1_PMS_READ_REE2_HP_LEDC0_V  0x00000001U
#define HP_PERI1_PMS_READ_REE2_HP_LEDC0_S  3
/** HP_PERI1_PMS_WRITE_TEE_HP_LEDC0 : R/W; bitpos: [4]; default: 1;
 *  Configures hp_ledc0 registers write permission in tee mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI1_PMS_WRITE_TEE_HP_LEDC0    (BIT(4))
#define HP_PERI1_PMS_WRITE_TEE_HP_LEDC0_M  (HP_PERI1_PMS_WRITE_TEE_HP_LEDC0_V << HP_PERI1_PMS_WRITE_TEE_HP_LEDC0_S)
#define HP_PERI1_PMS_WRITE_TEE_HP_LEDC0_V  0x00000001U
#define HP_PERI1_PMS_WRITE_TEE_HP_LEDC0_S  4
/** HP_PERI1_PMS_WRITE_REE0_HP_LEDC0 : R/W; bitpos: [5]; default: 0;
 *  Configures hp_ledc0 registers write permission in ree0 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI1_PMS_WRITE_REE0_HP_LEDC0    (BIT(5))
#define HP_PERI1_PMS_WRITE_REE0_HP_LEDC0_M  (HP_PERI1_PMS_WRITE_REE0_HP_LEDC0_V << HP_PERI1_PMS_WRITE_REE0_HP_LEDC0_S)
#define HP_PERI1_PMS_WRITE_REE0_HP_LEDC0_V  0x00000001U
#define HP_PERI1_PMS_WRITE_REE0_HP_LEDC0_S  5
/** HP_PERI1_PMS_WRITE_REE1_HP_LEDC0 : R/W; bitpos: [6]; default: 0;
 *  Configures hp_ledc0 registers write permission in ree1 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI1_PMS_WRITE_REE1_HP_LEDC0    (BIT(6))
#define HP_PERI1_PMS_WRITE_REE1_HP_LEDC0_M  (HP_PERI1_PMS_WRITE_REE1_HP_LEDC0_V << HP_PERI1_PMS_WRITE_REE1_HP_LEDC0_S)
#define HP_PERI1_PMS_WRITE_REE1_HP_LEDC0_V  0x00000001U
#define HP_PERI1_PMS_WRITE_REE1_HP_LEDC0_S  6
/** HP_PERI1_PMS_WRITE_REE2_HP_LEDC0 : R/W; bitpos: [7]; default: 0;
 *  Configures hp_ledc0 registers write permission in ree2 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI1_PMS_WRITE_REE2_HP_LEDC0    (BIT(7))
#define HP_PERI1_PMS_WRITE_REE2_HP_LEDC0_M  (HP_PERI1_PMS_WRITE_REE2_HP_LEDC0_V << HP_PERI1_PMS_WRITE_REE2_HP_LEDC0_S)
#define HP_PERI1_PMS_WRITE_REE2_HP_LEDC0_V  0x00000001U
#define HP_PERI1_PMS_WRITE_REE2_HP_LEDC0_S  7
/** HP_PERI1_PMS_HP_LEDC0_LOCK : R/W; bitpos: [8]; default: 0;
 *  Set 1 to lock hp_ledc0 peri_apm configuration
 */
#define HP_PERI1_PMS_HP_LEDC0_LOCK    (BIT(8))
#define HP_PERI1_PMS_HP_LEDC0_LOCK_M  (HP_PERI1_PMS_HP_LEDC0_LOCK_V << HP_PERI1_PMS_HP_LEDC0_LOCK_S)
#define HP_PERI1_PMS_HP_LEDC0_LOCK_V  0x00000001U
#define HP_PERI1_PMS_HP_LEDC0_LOCK_S  8

/** HP_PERI1_PMS_HP_ETM_CTRL_REG register
 *  hp_etm read/write control register
 */
#define HP_PERI1_PMS_HP_ETM_CTRL_REG (DR_REG_HP_PERI1_PMS_BASE + 0x4c)
/** HP_PERI1_PMS_READ_TEE_HP_ETM : R/W; bitpos: [0]; default: 1;
 *  Configures hp_etm registers read permission in tee mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI1_PMS_READ_TEE_HP_ETM    (BIT(0))
#define HP_PERI1_PMS_READ_TEE_HP_ETM_M  (HP_PERI1_PMS_READ_TEE_HP_ETM_V << HP_PERI1_PMS_READ_TEE_HP_ETM_S)
#define HP_PERI1_PMS_READ_TEE_HP_ETM_V  0x00000001U
#define HP_PERI1_PMS_READ_TEE_HP_ETM_S  0
/** HP_PERI1_PMS_READ_REE0_HP_ETM : R/W; bitpos: [1]; default: 0;
 *  Configures hp_etm registers read permission in ree0 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI1_PMS_READ_REE0_HP_ETM    (BIT(1))
#define HP_PERI1_PMS_READ_REE0_HP_ETM_M  (HP_PERI1_PMS_READ_REE0_HP_ETM_V << HP_PERI1_PMS_READ_REE0_HP_ETM_S)
#define HP_PERI1_PMS_READ_REE0_HP_ETM_V  0x00000001U
#define HP_PERI1_PMS_READ_REE0_HP_ETM_S  1
/** HP_PERI1_PMS_READ_REE1_HP_ETM : R/W; bitpos: [2]; default: 0;
 *  Configures hp_etm registers read permission in ree1 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI1_PMS_READ_REE1_HP_ETM    (BIT(2))
#define HP_PERI1_PMS_READ_REE1_HP_ETM_M  (HP_PERI1_PMS_READ_REE1_HP_ETM_V << HP_PERI1_PMS_READ_REE1_HP_ETM_S)
#define HP_PERI1_PMS_READ_REE1_HP_ETM_V  0x00000001U
#define HP_PERI1_PMS_READ_REE1_HP_ETM_S  2
/** HP_PERI1_PMS_READ_REE2_HP_ETM : R/W; bitpos: [3]; default: 0;
 *  Configures hp_etm registers read permission in ree2 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI1_PMS_READ_REE2_HP_ETM    (BIT(3))
#define HP_PERI1_PMS_READ_REE2_HP_ETM_M  (HP_PERI1_PMS_READ_REE2_HP_ETM_V << HP_PERI1_PMS_READ_REE2_HP_ETM_S)
#define HP_PERI1_PMS_READ_REE2_HP_ETM_V  0x00000001U
#define HP_PERI1_PMS_READ_REE2_HP_ETM_S  3
/** HP_PERI1_PMS_WRITE_TEE_HP_ETM : R/W; bitpos: [4]; default: 1;
 *  Configures hp_etm registers write permission in tee mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI1_PMS_WRITE_TEE_HP_ETM    (BIT(4))
#define HP_PERI1_PMS_WRITE_TEE_HP_ETM_M  (HP_PERI1_PMS_WRITE_TEE_HP_ETM_V << HP_PERI1_PMS_WRITE_TEE_HP_ETM_S)
#define HP_PERI1_PMS_WRITE_TEE_HP_ETM_V  0x00000001U
#define HP_PERI1_PMS_WRITE_TEE_HP_ETM_S  4
/** HP_PERI1_PMS_WRITE_REE0_HP_ETM : R/W; bitpos: [5]; default: 0;
 *  Configures hp_etm registers write permission in ree0 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI1_PMS_WRITE_REE0_HP_ETM    (BIT(5))
#define HP_PERI1_PMS_WRITE_REE0_HP_ETM_M  (HP_PERI1_PMS_WRITE_REE0_HP_ETM_V << HP_PERI1_PMS_WRITE_REE0_HP_ETM_S)
#define HP_PERI1_PMS_WRITE_REE0_HP_ETM_V  0x00000001U
#define HP_PERI1_PMS_WRITE_REE0_HP_ETM_S  5
/** HP_PERI1_PMS_WRITE_REE1_HP_ETM : R/W; bitpos: [6]; default: 0;
 *  Configures hp_etm registers write permission in ree1 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI1_PMS_WRITE_REE1_HP_ETM    (BIT(6))
#define HP_PERI1_PMS_WRITE_REE1_HP_ETM_M  (HP_PERI1_PMS_WRITE_REE1_HP_ETM_V << HP_PERI1_PMS_WRITE_REE1_HP_ETM_S)
#define HP_PERI1_PMS_WRITE_REE1_HP_ETM_V  0x00000001U
#define HP_PERI1_PMS_WRITE_REE1_HP_ETM_S  6
/** HP_PERI1_PMS_WRITE_REE2_HP_ETM : R/W; bitpos: [7]; default: 0;
 *  Configures hp_etm registers write permission in ree2 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI1_PMS_WRITE_REE2_HP_ETM    (BIT(7))
#define HP_PERI1_PMS_WRITE_REE2_HP_ETM_M  (HP_PERI1_PMS_WRITE_REE2_HP_ETM_V << HP_PERI1_PMS_WRITE_REE2_HP_ETM_S)
#define HP_PERI1_PMS_WRITE_REE2_HP_ETM_V  0x00000001U
#define HP_PERI1_PMS_WRITE_REE2_HP_ETM_S  7
/** HP_PERI1_PMS_HP_ETM_LOCK : R/W; bitpos: [8]; default: 0;
 *  Set 1 to lock hp_etm peri_apm configuration
 */
#define HP_PERI1_PMS_HP_ETM_LOCK    (BIT(8))
#define HP_PERI1_PMS_HP_ETM_LOCK_M  (HP_PERI1_PMS_HP_ETM_LOCK_V << HP_PERI1_PMS_HP_ETM_LOCK_S)
#define HP_PERI1_PMS_HP_ETM_LOCK_V  0x00000001U
#define HP_PERI1_PMS_HP_ETM_LOCK_S  8

/** HP_PERI1_PMS_HP_TWAI0_CTRL_REG register
 *  hp_twai0 read/write control register
 */
#define HP_PERI1_PMS_HP_TWAI0_CTRL_REG (DR_REG_HP_PERI1_PMS_BASE + 0x50)
/** HP_PERI1_PMS_READ_TEE_HP_TWAI0 : R/W; bitpos: [0]; default: 1;
 *  Configures hp_twai0 registers read permission in tee mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI1_PMS_READ_TEE_HP_TWAI0    (BIT(0))
#define HP_PERI1_PMS_READ_TEE_HP_TWAI0_M  (HP_PERI1_PMS_READ_TEE_HP_TWAI0_V << HP_PERI1_PMS_READ_TEE_HP_TWAI0_S)
#define HP_PERI1_PMS_READ_TEE_HP_TWAI0_V  0x00000001U
#define HP_PERI1_PMS_READ_TEE_HP_TWAI0_S  0
/** HP_PERI1_PMS_READ_REE0_HP_TWAI0 : R/W; bitpos: [1]; default: 0;
 *  Configures hp_twai0 registers read permission in ree0 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI1_PMS_READ_REE0_HP_TWAI0    (BIT(1))
#define HP_PERI1_PMS_READ_REE0_HP_TWAI0_M  (HP_PERI1_PMS_READ_REE0_HP_TWAI0_V << HP_PERI1_PMS_READ_REE0_HP_TWAI0_S)
#define HP_PERI1_PMS_READ_REE0_HP_TWAI0_V  0x00000001U
#define HP_PERI1_PMS_READ_REE0_HP_TWAI0_S  1
/** HP_PERI1_PMS_READ_REE1_HP_TWAI0 : R/W; bitpos: [2]; default: 0;
 *  Configures hp_twai0 registers read permission in ree1 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI1_PMS_READ_REE1_HP_TWAI0    (BIT(2))
#define HP_PERI1_PMS_READ_REE1_HP_TWAI0_M  (HP_PERI1_PMS_READ_REE1_HP_TWAI0_V << HP_PERI1_PMS_READ_REE1_HP_TWAI0_S)
#define HP_PERI1_PMS_READ_REE1_HP_TWAI0_V  0x00000001U
#define HP_PERI1_PMS_READ_REE1_HP_TWAI0_S  2
/** HP_PERI1_PMS_READ_REE2_HP_TWAI0 : R/W; bitpos: [3]; default: 0;
 *  Configures hp_twai0 registers read permission in ree2 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI1_PMS_READ_REE2_HP_TWAI0    (BIT(3))
#define HP_PERI1_PMS_READ_REE2_HP_TWAI0_M  (HP_PERI1_PMS_READ_REE2_HP_TWAI0_V << HP_PERI1_PMS_READ_REE2_HP_TWAI0_S)
#define HP_PERI1_PMS_READ_REE2_HP_TWAI0_V  0x00000001U
#define HP_PERI1_PMS_READ_REE2_HP_TWAI0_S  3
/** HP_PERI1_PMS_WRITE_TEE_HP_TWAI0 : R/W; bitpos: [4]; default: 1;
 *  Configures hp_twai0 registers write permission in tee mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI1_PMS_WRITE_TEE_HP_TWAI0    (BIT(4))
#define HP_PERI1_PMS_WRITE_TEE_HP_TWAI0_M  (HP_PERI1_PMS_WRITE_TEE_HP_TWAI0_V << HP_PERI1_PMS_WRITE_TEE_HP_TWAI0_S)
#define HP_PERI1_PMS_WRITE_TEE_HP_TWAI0_V  0x00000001U
#define HP_PERI1_PMS_WRITE_TEE_HP_TWAI0_S  4
/** HP_PERI1_PMS_WRITE_REE0_HP_TWAI0 : R/W; bitpos: [5]; default: 0;
 *  Configures hp_twai0 registers write permission in ree0 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI1_PMS_WRITE_REE0_HP_TWAI0    (BIT(5))
#define HP_PERI1_PMS_WRITE_REE0_HP_TWAI0_M  (HP_PERI1_PMS_WRITE_REE0_HP_TWAI0_V << HP_PERI1_PMS_WRITE_REE0_HP_TWAI0_S)
#define HP_PERI1_PMS_WRITE_REE0_HP_TWAI0_V  0x00000001U
#define HP_PERI1_PMS_WRITE_REE0_HP_TWAI0_S  5
/** HP_PERI1_PMS_WRITE_REE1_HP_TWAI0 : R/W; bitpos: [6]; default: 0;
 *  Configures hp_twai0 registers write permission in ree1 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI1_PMS_WRITE_REE1_HP_TWAI0    (BIT(6))
#define HP_PERI1_PMS_WRITE_REE1_HP_TWAI0_M  (HP_PERI1_PMS_WRITE_REE1_HP_TWAI0_V << HP_PERI1_PMS_WRITE_REE1_HP_TWAI0_S)
#define HP_PERI1_PMS_WRITE_REE1_HP_TWAI0_V  0x00000001U
#define HP_PERI1_PMS_WRITE_REE1_HP_TWAI0_S  6
/** HP_PERI1_PMS_WRITE_REE2_HP_TWAI0 : R/W; bitpos: [7]; default: 0;
 *  Configures hp_twai0 registers write permission in ree2 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI1_PMS_WRITE_REE2_HP_TWAI0    (BIT(7))
#define HP_PERI1_PMS_WRITE_REE2_HP_TWAI0_M  (HP_PERI1_PMS_WRITE_REE2_HP_TWAI0_V << HP_PERI1_PMS_WRITE_REE2_HP_TWAI0_S)
#define HP_PERI1_PMS_WRITE_REE2_HP_TWAI0_V  0x00000001U
#define HP_PERI1_PMS_WRITE_REE2_HP_TWAI0_S  7
/** HP_PERI1_PMS_HP_TWAI0_LOCK : R/W; bitpos: [8]; default: 0;
 *  Set 1 to lock hp_twai0 peri_apm configuration
 */
#define HP_PERI1_PMS_HP_TWAI0_LOCK    (BIT(8))
#define HP_PERI1_PMS_HP_TWAI0_LOCK_M  (HP_PERI1_PMS_HP_TWAI0_LOCK_V << HP_PERI1_PMS_HP_TWAI0_LOCK_S)
#define HP_PERI1_PMS_HP_TWAI0_LOCK_V  0x00000001U
#define HP_PERI1_PMS_HP_TWAI0_LOCK_S  8

/** HP_PERI1_PMS_HP_TWAI1_CTRL_REG register
 *  hp_twai1 read/write control register
 */
#define HP_PERI1_PMS_HP_TWAI1_CTRL_REG (DR_REG_HP_PERI1_PMS_BASE + 0x54)
/** HP_PERI1_PMS_READ_TEE_HP_TWAI1 : R/W; bitpos: [0]; default: 1;
 *  Configures hp_twai1 registers read permission in tee mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI1_PMS_READ_TEE_HP_TWAI1    (BIT(0))
#define HP_PERI1_PMS_READ_TEE_HP_TWAI1_M  (HP_PERI1_PMS_READ_TEE_HP_TWAI1_V << HP_PERI1_PMS_READ_TEE_HP_TWAI1_S)
#define HP_PERI1_PMS_READ_TEE_HP_TWAI1_V  0x00000001U
#define HP_PERI1_PMS_READ_TEE_HP_TWAI1_S  0
/** HP_PERI1_PMS_READ_REE0_HP_TWAI1 : R/W; bitpos: [1]; default: 0;
 *  Configures hp_twai1 registers read permission in ree0 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI1_PMS_READ_REE0_HP_TWAI1    (BIT(1))
#define HP_PERI1_PMS_READ_REE0_HP_TWAI1_M  (HP_PERI1_PMS_READ_REE0_HP_TWAI1_V << HP_PERI1_PMS_READ_REE0_HP_TWAI1_S)
#define HP_PERI1_PMS_READ_REE0_HP_TWAI1_V  0x00000001U
#define HP_PERI1_PMS_READ_REE0_HP_TWAI1_S  1
/** HP_PERI1_PMS_READ_REE1_HP_TWAI1 : R/W; bitpos: [2]; default: 0;
 *  Configures hp_twai1 registers read permission in ree1 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI1_PMS_READ_REE1_HP_TWAI1    (BIT(2))
#define HP_PERI1_PMS_READ_REE1_HP_TWAI1_M  (HP_PERI1_PMS_READ_REE1_HP_TWAI1_V << HP_PERI1_PMS_READ_REE1_HP_TWAI1_S)
#define HP_PERI1_PMS_READ_REE1_HP_TWAI1_V  0x00000001U
#define HP_PERI1_PMS_READ_REE1_HP_TWAI1_S  2
/** HP_PERI1_PMS_READ_REE2_HP_TWAI1 : R/W; bitpos: [3]; default: 0;
 *  Configures hp_twai1 registers read permission in ree2 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI1_PMS_READ_REE2_HP_TWAI1    (BIT(3))
#define HP_PERI1_PMS_READ_REE2_HP_TWAI1_M  (HP_PERI1_PMS_READ_REE2_HP_TWAI1_V << HP_PERI1_PMS_READ_REE2_HP_TWAI1_S)
#define HP_PERI1_PMS_READ_REE2_HP_TWAI1_V  0x00000001U
#define HP_PERI1_PMS_READ_REE2_HP_TWAI1_S  3
/** HP_PERI1_PMS_WRITE_TEE_HP_TWAI1 : R/W; bitpos: [4]; default: 1;
 *  Configures hp_twai1 registers write permission in tee mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI1_PMS_WRITE_TEE_HP_TWAI1    (BIT(4))
#define HP_PERI1_PMS_WRITE_TEE_HP_TWAI1_M  (HP_PERI1_PMS_WRITE_TEE_HP_TWAI1_V << HP_PERI1_PMS_WRITE_TEE_HP_TWAI1_S)
#define HP_PERI1_PMS_WRITE_TEE_HP_TWAI1_V  0x00000001U
#define HP_PERI1_PMS_WRITE_TEE_HP_TWAI1_S  4
/** HP_PERI1_PMS_WRITE_REE0_HP_TWAI1 : R/W; bitpos: [5]; default: 0;
 *  Configures hp_twai1 registers write permission in ree0 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI1_PMS_WRITE_REE0_HP_TWAI1    (BIT(5))
#define HP_PERI1_PMS_WRITE_REE0_HP_TWAI1_M  (HP_PERI1_PMS_WRITE_REE0_HP_TWAI1_V << HP_PERI1_PMS_WRITE_REE0_HP_TWAI1_S)
#define HP_PERI1_PMS_WRITE_REE0_HP_TWAI1_V  0x00000001U
#define HP_PERI1_PMS_WRITE_REE0_HP_TWAI1_S  5
/** HP_PERI1_PMS_WRITE_REE1_HP_TWAI1 : R/W; bitpos: [6]; default: 0;
 *  Configures hp_twai1 registers write permission in ree1 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI1_PMS_WRITE_REE1_HP_TWAI1    (BIT(6))
#define HP_PERI1_PMS_WRITE_REE1_HP_TWAI1_M  (HP_PERI1_PMS_WRITE_REE1_HP_TWAI1_V << HP_PERI1_PMS_WRITE_REE1_HP_TWAI1_S)
#define HP_PERI1_PMS_WRITE_REE1_HP_TWAI1_V  0x00000001U
#define HP_PERI1_PMS_WRITE_REE1_HP_TWAI1_S  6
/** HP_PERI1_PMS_WRITE_REE2_HP_TWAI1 : R/W; bitpos: [7]; default: 0;
 *  Configures hp_twai1 registers write permission in ree2 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI1_PMS_WRITE_REE2_HP_TWAI1    (BIT(7))
#define HP_PERI1_PMS_WRITE_REE2_HP_TWAI1_M  (HP_PERI1_PMS_WRITE_REE2_HP_TWAI1_V << HP_PERI1_PMS_WRITE_REE2_HP_TWAI1_S)
#define HP_PERI1_PMS_WRITE_REE2_HP_TWAI1_V  0x00000001U
#define HP_PERI1_PMS_WRITE_REE2_HP_TWAI1_S  7
/** HP_PERI1_PMS_HP_TWAI1_LOCK : R/W; bitpos: [8]; default: 0;
 *  Set 1 to lock hp_twai1 peri_apm configuration
 */
#define HP_PERI1_PMS_HP_TWAI1_LOCK    (BIT(8))
#define HP_PERI1_PMS_HP_TWAI1_LOCK_M  (HP_PERI1_PMS_HP_TWAI1_LOCK_V << HP_PERI1_PMS_HP_TWAI1_LOCK_S)
#define HP_PERI1_PMS_HP_TWAI1_LOCK_V  0x00000001U
#define HP_PERI1_PMS_HP_TWAI1_LOCK_S  8

/** HP_PERI1_PMS_HP_LCDCAM_CTRL_REG register
 *  hp_lcdcam read/write control register
 */
#define HP_PERI1_PMS_HP_LCDCAM_CTRL_REG (DR_REG_HP_PERI1_PMS_BASE + 0x58)
/** HP_PERI1_PMS_READ_TEE_HP_LCDCAM : R/W; bitpos: [0]; default: 1;
 *  Configures hp_lcdcam registers read permission in tee mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI1_PMS_READ_TEE_HP_LCDCAM    (BIT(0))
#define HP_PERI1_PMS_READ_TEE_HP_LCDCAM_M  (HP_PERI1_PMS_READ_TEE_HP_LCDCAM_V << HP_PERI1_PMS_READ_TEE_HP_LCDCAM_S)
#define HP_PERI1_PMS_READ_TEE_HP_LCDCAM_V  0x00000001U
#define HP_PERI1_PMS_READ_TEE_HP_LCDCAM_S  0
/** HP_PERI1_PMS_READ_REE0_HP_LCDCAM : R/W; bitpos: [1]; default: 0;
 *  Configures hp_lcdcam registers read permission in ree0 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI1_PMS_READ_REE0_HP_LCDCAM    (BIT(1))
#define HP_PERI1_PMS_READ_REE0_HP_LCDCAM_M  (HP_PERI1_PMS_READ_REE0_HP_LCDCAM_V << HP_PERI1_PMS_READ_REE0_HP_LCDCAM_S)
#define HP_PERI1_PMS_READ_REE0_HP_LCDCAM_V  0x00000001U
#define HP_PERI1_PMS_READ_REE0_HP_LCDCAM_S  1
/** HP_PERI1_PMS_READ_REE1_HP_LCDCAM : R/W; bitpos: [2]; default: 0;
 *  Configures hp_lcdcam registers read permission in ree1 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI1_PMS_READ_REE1_HP_LCDCAM    (BIT(2))
#define HP_PERI1_PMS_READ_REE1_HP_LCDCAM_M  (HP_PERI1_PMS_READ_REE1_HP_LCDCAM_V << HP_PERI1_PMS_READ_REE1_HP_LCDCAM_S)
#define HP_PERI1_PMS_READ_REE1_HP_LCDCAM_V  0x00000001U
#define HP_PERI1_PMS_READ_REE1_HP_LCDCAM_S  2
/** HP_PERI1_PMS_READ_REE2_HP_LCDCAM : R/W; bitpos: [3]; default: 0;
 *  Configures hp_lcdcam registers read permission in ree2 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI1_PMS_READ_REE2_HP_LCDCAM    (BIT(3))
#define HP_PERI1_PMS_READ_REE2_HP_LCDCAM_M  (HP_PERI1_PMS_READ_REE2_HP_LCDCAM_V << HP_PERI1_PMS_READ_REE2_HP_LCDCAM_S)
#define HP_PERI1_PMS_READ_REE2_HP_LCDCAM_V  0x00000001U
#define HP_PERI1_PMS_READ_REE2_HP_LCDCAM_S  3
/** HP_PERI1_PMS_WRITE_TEE_HP_LCDCAM : R/W; bitpos: [4]; default: 1;
 *  Configures hp_lcdcam registers write permission in tee mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI1_PMS_WRITE_TEE_HP_LCDCAM    (BIT(4))
#define HP_PERI1_PMS_WRITE_TEE_HP_LCDCAM_M  (HP_PERI1_PMS_WRITE_TEE_HP_LCDCAM_V << HP_PERI1_PMS_WRITE_TEE_HP_LCDCAM_S)
#define HP_PERI1_PMS_WRITE_TEE_HP_LCDCAM_V  0x00000001U
#define HP_PERI1_PMS_WRITE_TEE_HP_LCDCAM_S  4
/** HP_PERI1_PMS_WRITE_REE0_HP_LCDCAM : R/W; bitpos: [5]; default: 0;
 *  Configures hp_lcdcam registers write permission in ree0 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI1_PMS_WRITE_REE0_HP_LCDCAM    (BIT(5))
#define HP_PERI1_PMS_WRITE_REE0_HP_LCDCAM_M  (HP_PERI1_PMS_WRITE_REE0_HP_LCDCAM_V << HP_PERI1_PMS_WRITE_REE0_HP_LCDCAM_S)
#define HP_PERI1_PMS_WRITE_REE0_HP_LCDCAM_V  0x00000001U
#define HP_PERI1_PMS_WRITE_REE0_HP_LCDCAM_S  5
/** HP_PERI1_PMS_WRITE_REE1_HP_LCDCAM : R/W; bitpos: [6]; default: 0;
 *  Configures hp_lcdcam registers write permission in ree1 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI1_PMS_WRITE_REE1_HP_LCDCAM    (BIT(6))
#define HP_PERI1_PMS_WRITE_REE1_HP_LCDCAM_M  (HP_PERI1_PMS_WRITE_REE1_HP_LCDCAM_V << HP_PERI1_PMS_WRITE_REE1_HP_LCDCAM_S)
#define HP_PERI1_PMS_WRITE_REE1_HP_LCDCAM_V  0x00000001U
#define HP_PERI1_PMS_WRITE_REE1_HP_LCDCAM_S  6
/** HP_PERI1_PMS_WRITE_REE2_HP_LCDCAM : R/W; bitpos: [7]; default: 0;
 *  Configures hp_lcdcam registers write permission in ree2 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI1_PMS_WRITE_REE2_HP_LCDCAM    (BIT(7))
#define HP_PERI1_PMS_WRITE_REE2_HP_LCDCAM_M  (HP_PERI1_PMS_WRITE_REE2_HP_LCDCAM_V << HP_PERI1_PMS_WRITE_REE2_HP_LCDCAM_S)
#define HP_PERI1_PMS_WRITE_REE2_HP_LCDCAM_V  0x00000001U
#define HP_PERI1_PMS_WRITE_REE2_HP_LCDCAM_S  7
/** HP_PERI1_PMS_HP_LCDCAM_LOCK : R/W; bitpos: [8]; default: 0;
 *  Set 1 to lock hp_lcdcam peri_apm configuration
 */
#define HP_PERI1_PMS_HP_LCDCAM_LOCK    (BIT(8))
#define HP_PERI1_PMS_HP_LCDCAM_LOCK_M  (HP_PERI1_PMS_HP_LCDCAM_LOCK_V << HP_PERI1_PMS_HP_LCDCAM_LOCK_S)
#define HP_PERI1_PMS_HP_LCDCAM_LOCK_V  0x00000001U
#define HP_PERI1_PMS_HP_LCDCAM_LOCK_S  8

/** HP_PERI1_PMS_HP_UHCI_CTRL_REG register
 *  hp_uhci read/write control register
 */
#define HP_PERI1_PMS_HP_UHCI_CTRL_REG (DR_REG_HP_PERI1_PMS_BASE + 0x5c)
/** HP_PERI1_PMS_READ_TEE_HP_UHCI : R/W; bitpos: [0]; default: 1;
 *  Configures hp_uhci registers read permission in tee mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI1_PMS_READ_TEE_HP_UHCI    (BIT(0))
#define HP_PERI1_PMS_READ_TEE_HP_UHCI_M  (HP_PERI1_PMS_READ_TEE_HP_UHCI_V << HP_PERI1_PMS_READ_TEE_HP_UHCI_S)
#define HP_PERI1_PMS_READ_TEE_HP_UHCI_V  0x00000001U
#define HP_PERI1_PMS_READ_TEE_HP_UHCI_S  0
/** HP_PERI1_PMS_READ_REE0_HP_UHCI : R/W; bitpos: [1]; default: 0;
 *  Configures hp_uhci registers read permission in ree0 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI1_PMS_READ_REE0_HP_UHCI    (BIT(1))
#define HP_PERI1_PMS_READ_REE0_HP_UHCI_M  (HP_PERI1_PMS_READ_REE0_HP_UHCI_V << HP_PERI1_PMS_READ_REE0_HP_UHCI_S)
#define HP_PERI1_PMS_READ_REE0_HP_UHCI_V  0x00000001U
#define HP_PERI1_PMS_READ_REE0_HP_UHCI_S  1
/** HP_PERI1_PMS_READ_REE1_HP_UHCI : R/W; bitpos: [2]; default: 0;
 *  Configures hp_uhci registers read permission in ree1 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI1_PMS_READ_REE1_HP_UHCI    (BIT(2))
#define HP_PERI1_PMS_READ_REE1_HP_UHCI_M  (HP_PERI1_PMS_READ_REE1_HP_UHCI_V << HP_PERI1_PMS_READ_REE1_HP_UHCI_S)
#define HP_PERI1_PMS_READ_REE1_HP_UHCI_V  0x00000001U
#define HP_PERI1_PMS_READ_REE1_HP_UHCI_S  2
/** HP_PERI1_PMS_READ_REE2_HP_UHCI : R/W; bitpos: [3]; default: 0;
 *  Configures hp_uhci registers read permission in ree2 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI1_PMS_READ_REE2_HP_UHCI    (BIT(3))
#define HP_PERI1_PMS_READ_REE2_HP_UHCI_M  (HP_PERI1_PMS_READ_REE2_HP_UHCI_V << HP_PERI1_PMS_READ_REE2_HP_UHCI_S)
#define HP_PERI1_PMS_READ_REE2_HP_UHCI_V  0x00000001U
#define HP_PERI1_PMS_READ_REE2_HP_UHCI_S  3
/** HP_PERI1_PMS_WRITE_TEE_HP_UHCI : R/W; bitpos: [4]; default: 1;
 *  Configures hp_uhci registers write permission in tee mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI1_PMS_WRITE_TEE_HP_UHCI    (BIT(4))
#define HP_PERI1_PMS_WRITE_TEE_HP_UHCI_M  (HP_PERI1_PMS_WRITE_TEE_HP_UHCI_V << HP_PERI1_PMS_WRITE_TEE_HP_UHCI_S)
#define HP_PERI1_PMS_WRITE_TEE_HP_UHCI_V  0x00000001U
#define HP_PERI1_PMS_WRITE_TEE_HP_UHCI_S  4
/** HP_PERI1_PMS_WRITE_REE0_HP_UHCI : R/W; bitpos: [5]; default: 0;
 *  Configures hp_uhci registers write permission in ree0 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI1_PMS_WRITE_REE0_HP_UHCI    (BIT(5))
#define HP_PERI1_PMS_WRITE_REE0_HP_UHCI_M  (HP_PERI1_PMS_WRITE_REE0_HP_UHCI_V << HP_PERI1_PMS_WRITE_REE0_HP_UHCI_S)
#define HP_PERI1_PMS_WRITE_REE0_HP_UHCI_V  0x00000001U
#define HP_PERI1_PMS_WRITE_REE0_HP_UHCI_S  5
/** HP_PERI1_PMS_WRITE_REE1_HP_UHCI : R/W; bitpos: [6]; default: 0;
 *  Configures hp_uhci registers write permission in ree1 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI1_PMS_WRITE_REE1_HP_UHCI    (BIT(6))
#define HP_PERI1_PMS_WRITE_REE1_HP_UHCI_M  (HP_PERI1_PMS_WRITE_REE1_HP_UHCI_V << HP_PERI1_PMS_WRITE_REE1_HP_UHCI_S)
#define HP_PERI1_PMS_WRITE_REE1_HP_UHCI_V  0x00000001U
#define HP_PERI1_PMS_WRITE_REE1_HP_UHCI_S  6
/** HP_PERI1_PMS_WRITE_REE2_HP_UHCI : R/W; bitpos: [7]; default: 0;
 *  Configures hp_uhci registers write permission in ree2 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI1_PMS_WRITE_REE2_HP_UHCI    (BIT(7))
#define HP_PERI1_PMS_WRITE_REE2_HP_UHCI_M  (HP_PERI1_PMS_WRITE_REE2_HP_UHCI_V << HP_PERI1_PMS_WRITE_REE2_HP_UHCI_S)
#define HP_PERI1_PMS_WRITE_REE2_HP_UHCI_V  0x00000001U
#define HP_PERI1_PMS_WRITE_REE2_HP_UHCI_S  7
/** HP_PERI1_PMS_HP_UHCI_LOCK : R/W; bitpos: [8]; default: 0;
 *  Set 1 to lock hp_uhci peri_apm configuration
 */
#define HP_PERI1_PMS_HP_UHCI_LOCK    (BIT(8))
#define HP_PERI1_PMS_HP_UHCI_LOCK_M  (HP_PERI1_PMS_HP_UHCI_LOCK_V << HP_PERI1_PMS_HP_UHCI_LOCK_S)
#define HP_PERI1_PMS_HP_UHCI_LOCK_V  0x00000001U
#define HP_PERI1_PMS_HP_UHCI_LOCK_S  8

/** HP_PERI1_PMS_HP_SYSTIMER_CTRL_REG register
 *  hp_systimer read/write control register
 */
#define HP_PERI1_PMS_HP_SYSTIMER_CTRL_REG (DR_REG_HP_PERI1_PMS_BASE + 0x60)
/** HP_PERI1_PMS_READ_TEE_HP_SYSTIMER : R/W; bitpos: [0]; default: 1;
 *  Configures hp_systimer registers read permission in tee mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI1_PMS_READ_TEE_HP_SYSTIMER    (BIT(0))
#define HP_PERI1_PMS_READ_TEE_HP_SYSTIMER_M  (HP_PERI1_PMS_READ_TEE_HP_SYSTIMER_V << HP_PERI1_PMS_READ_TEE_HP_SYSTIMER_S)
#define HP_PERI1_PMS_READ_TEE_HP_SYSTIMER_V  0x00000001U
#define HP_PERI1_PMS_READ_TEE_HP_SYSTIMER_S  0
/** HP_PERI1_PMS_READ_REE0_HP_SYSTIMER : R/W; bitpos: [1]; default: 0;
 *  Configures hp_systimer registers read permission in ree0 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI1_PMS_READ_REE0_HP_SYSTIMER    (BIT(1))
#define HP_PERI1_PMS_READ_REE0_HP_SYSTIMER_M  (HP_PERI1_PMS_READ_REE0_HP_SYSTIMER_V << HP_PERI1_PMS_READ_REE0_HP_SYSTIMER_S)
#define HP_PERI1_PMS_READ_REE0_HP_SYSTIMER_V  0x00000001U
#define HP_PERI1_PMS_READ_REE0_HP_SYSTIMER_S  1
/** HP_PERI1_PMS_READ_REE1_HP_SYSTIMER : R/W; bitpos: [2]; default: 0;
 *  Configures hp_systimer registers read permission in ree1 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI1_PMS_READ_REE1_HP_SYSTIMER    (BIT(2))
#define HP_PERI1_PMS_READ_REE1_HP_SYSTIMER_M  (HP_PERI1_PMS_READ_REE1_HP_SYSTIMER_V << HP_PERI1_PMS_READ_REE1_HP_SYSTIMER_S)
#define HP_PERI1_PMS_READ_REE1_HP_SYSTIMER_V  0x00000001U
#define HP_PERI1_PMS_READ_REE1_HP_SYSTIMER_S  2
/** HP_PERI1_PMS_READ_REE2_HP_SYSTIMER : R/W; bitpos: [3]; default: 0;
 *  Configures hp_systimer registers read permission in ree2 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI1_PMS_READ_REE2_HP_SYSTIMER    (BIT(3))
#define HP_PERI1_PMS_READ_REE2_HP_SYSTIMER_M  (HP_PERI1_PMS_READ_REE2_HP_SYSTIMER_V << HP_PERI1_PMS_READ_REE2_HP_SYSTIMER_S)
#define HP_PERI1_PMS_READ_REE2_HP_SYSTIMER_V  0x00000001U
#define HP_PERI1_PMS_READ_REE2_HP_SYSTIMER_S  3
/** HP_PERI1_PMS_WRITE_TEE_HP_SYSTIMER : R/W; bitpos: [4]; default: 1;
 *  Configures hp_systimer registers write permission in tee mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI1_PMS_WRITE_TEE_HP_SYSTIMER    (BIT(4))
#define HP_PERI1_PMS_WRITE_TEE_HP_SYSTIMER_M  (HP_PERI1_PMS_WRITE_TEE_HP_SYSTIMER_V << HP_PERI1_PMS_WRITE_TEE_HP_SYSTIMER_S)
#define HP_PERI1_PMS_WRITE_TEE_HP_SYSTIMER_V  0x00000001U
#define HP_PERI1_PMS_WRITE_TEE_HP_SYSTIMER_S  4
/** HP_PERI1_PMS_WRITE_REE0_HP_SYSTIMER : R/W; bitpos: [5]; default: 0;
 *  Configures hp_systimer registers write permission in ree0 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI1_PMS_WRITE_REE0_HP_SYSTIMER    (BIT(5))
#define HP_PERI1_PMS_WRITE_REE0_HP_SYSTIMER_M  (HP_PERI1_PMS_WRITE_REE0_HP_SYSTIMER_V << HP_PERI1_PMS_WRITE_REE0_HP_SYSTIMER_S)
#define HP_PERI1_PMS_WRITE_REE0_HP_SYSTIMER_V  0x00000001U
#define HP_PERI1_PMS_WRITE_REE0_HP_SYSTIMER_S  5
/** HP_PERI1_PMS_WRITE_REE1_HP_SYSTIMER : R/W; bitpos: [6]; default: 0;
 *  Configures hp_systimer registers write permission in ree1 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI1_PMS_WRITE_REE1_HP_SYSTIMER    (BIT(6))
#define HP_PERI1_PMS_WRITE_REE1_HP_SYSTIMER_M  (HP_PERI1_PMS_WRITE_REE1_HP_SYSTIMER_V << HP_PERI1_PMS_WRITE_REE1_HP_SYSTIMER_S)
#define HP_PERI1_PMS_WRITE_REE1_HP_SYSTIMER_V  0x00000001U
#define HP_PERI1_PMS_WRITE_REE1_HP_SYSTIMER_S  6
/** HP_PERI1_PMS_WRITE_REE2_HP_SYSTIMER : R/W; bitpos: [7]; default: 0;
 *  Configures hp_systimer registers write permission in ree2 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI1_PMS_WRITE_REE2_HP_SYSTIMER    (BIT(7))
#define HP_PERI1_PMS_WRITE_REE2_HP_SYSTIMER_M  (HP_PERI1_PMS_WRITE_REE2_HP_SYSTIMER_V << HP_PERI1_PMS_WRITE_REE2_HP_SYSTIMER_S)
#define HP_PERI1_PMS_WRITE_REE2_HP_SYSTIMER_V  0x00000001U
#define HP_PERI1_PMS_WRITE_REE2_HP_SYSTIMER_S  7
/** HP_PERI1_PMS_HP_SYSTIMER_LOCK : R/W; bitpos: [8]; default: 0;
 *  Set 1 to lock hp_systimer peri_apm configuration
 */
#define HP_PERI1_PMS_HP_SYSTIMER_LOCK    (BIT(8))
#define HP_PERI1_PMS_HP_SYSTIMER_LOCK_M  (HP_PERI1_PMS_HP_SYSTIMER_LOCK_V << HP_PERI1_PMS_HP_SYSTIMER_LOCK_S)
#define HP_PERI1_PMS_HP_SYSTIMER_LOCK_V  0x00000001U
#define HP_PERI1_PMS_HP_SYSTIMER_LOCK_S  8

/** HP_PERI1_PMS_HP_ZERO_DET_CTRL_REG register
 *  hp_zero_det read/write control register
 */
#define HP_PERI1_PMS_HP_ZERO_DET_CTRL_REG (DR_REG_HP_PERI1_PMS_BASE + 0x64)
/** HP_PERI1_PMS_READ_TEE_HP_ZERO_DET : R/W; bitpos: [0]; default: 1;
 *  Configures hp_zero_det registers read permission in tee mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI1_PMS_READ_TEE_HP_ZERO_DET    (BIT(0))
#define HP_PERI1_PMS_READ_TEE_HP_ZERO_DET_M  (HP_PERI1_PMS_READ_TEE_HP_ZERO_DET_V << HP_PERI1_PMS_READ_TEE_HP_ZERO_DET_S)
#define HP_PERI1_PMS_READ_TEE_HP_ZERO_DET_V  0x00000001U
#define HP_PERI1_PMS_READ_TEE_HP_ZERO_DET_S  0
/** HP_PERI1_PMS_READ_REE0_HP_ZERO_DET : R/W; bitpos: [1]; default: 0;
 *  Configures hp_zero_det registers read permission in ree0 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI1_PMS_READ_REE0_HP_ZERO_DET    (BIT(1))
#define HP_PERI1_PMS_READ_REE0_HP_ZERO_DET_M  (HP_PERI1_PMS_READ_REE0_HP_ZERO_DET_V << HP_PERI1_PMS_READ_REE0_HP_ZERO_DET_S)
#define HP_PERI1_PMS_READ_REE0_HP_ZERO_DET_V  0x00000001U
#define HP_PERI1_PMS_READ_REE0_HP_ZERO_DET_S  1
/** HP_PERI1_PMS_READ_REE1_HP_ZERO_DET : R/W; bitpos: [2]; default: 0;
 *  Configures hp_zero_det registers read permission in ree1 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI1_PMS_READ_REE1_HP_ZERO_DET    (BIT(2))
#define HP_PERI1_PMS_READ_REE1_HP_ZERO_DET_M  (HP_PERI1_PMS_READ_REE1_HP_ZERO_DET_V << HP_PERI1_PMS_READ_REE1_HP_ZERO_DET_S)
#define HP_PERI1_PMS_READ_REE1_HP_ZERO_DET_V  0x00000001U
#define HP_PERI1_PMS_READ_REE1_HP_ZERO_DET_S  2
/** HP_PERI1_PMS_READ_REE2_HP_ZERO_DET : R/W; bitpos: [3]; default: 0;
 *  Configures hp_zero_det registers read permission in ree2 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI1_PMS_READ_REE2_HP_ZERO_DET    (BIT(3))
#define HP_PERI1_PMS_READ_REE2_HP_ZERO_DET_M  (HP_PERI1_PMS_READ_REE2_HP_ZERO_DET_V << HP_PERI1_PMS_READ_REE2_HP_ZERO_DET_S)
#define HP_PERI1_PMS_READ_REE2_HP_ZERO_DET_V  0x00000001U
#define HP_PERI1_PMS_READ_REE2_HP_ZERO_DET_S  3
/** HP_PERI1_PMS_WRITE_TEE_HP_ZERO_DET : R/W; bitpos: [4]; default: 1;
 *  Configures hp_zero_det registers write permission in tee mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI1_PMS_WRITE_TEE_HP_ZERO_DET    (BIT(4))
#define HP_PERI1_PMS_WRITE_TEE_HP_ZERO_DET_M  (HP_PERI1_PMS_WRITE_TEE_HP_ZERO_DET_V << HP_PERI1_PMS_WRITE_TEE_HP_ZERO_DET_S)
#define HP_PERI1_PMS_WRITE_TEE_HP_ZERO_DET_V  0x00000001U
#define HP_PERI1_PMS_WRITE_TEE_HP_ZERO_DET_S  4
/** HP_PERI1_PMS_WRITE_REE0_HP_ZERO_DET : R/W; bitpos: [5]; default: 0;
 *  Configures hp_zero_det registers write permission in ree0 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI1_PMS_WRITE_REE0_HP_ZERO_DET    (BIT(5))
#define HP_PERI1_PMS_WRITE_REE0_HP_ZERO_DET_M  (HP_PERI1_PMS_WRITE_REE0_HP_ZERO_DET_V << HP_PERI1_PMS_WRITE_REE0_HP_ZERO_DET_S)
#define HP_PERI1_PMS_WRITE_REE0_HP_ZERO_DET_V  0x00000001U
#define HP_PERI1_PMS_WRITE_REE0_HP_ZERO_DET_S  5
/** HP_PERI1_PMS_WRITE_REE1_HP_ZERO_DET : R/W; bitpos: [6]; default: 0;
 *  Configures hp_zero_det registers write permission in ree1 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI1_PMS_WRITE_REE1_HP_ZERO_DET    (BIT(6))
#define HP_PERI1_PMS_WRITE_REE1_HP_ZERO_DET_M  (HP_PERI1_PMS_WRITE_REE1_HP_ZERO_DET_V << HP_PERI1_PMS_WRITE_REE1_HP_ZERO_DET_S)
#define HP_PERI1_PMS_WRITE_REE1_HP_ZERO_DET_V  0x00000001U
#define HP_PERI1_PMS_WRITE_REE1_HP_ZERO_DET_S  6
/** HP_PERI1_PMS_WRITE_REE2_HP_ZERO_DET : R/W; bitpos: [7]; default: 0;
 *  Configures hp_zero_det registers write permission in ree2 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI1_PMS_WRITE_REE2_HP_ZERO_DET    (BIT(7))
#define HP_PERI1_PMS_WRITE_REE2_HP_ZERO_DET_M  (HP_PERI1_PMS_WRITE_REE2_HP_ZERO_DET_V << HP_PERI1_PMS_WRITE_REE2_HP_ZERO_DET_S)
#define HP_PERI1_PMS_WRITE_REE2_HP_ZERO_DET_V  0x00000001U
#define HP_PERI1_PMS_WRITE_REE2_HP_ZERO_DET_S  7
/** HP_PERI1_PMS_HP_ZERO_DET_LOCK : R/W; bitpos: [8]; default: 0;
 *  Set 1 to lock hp_zero_det peri_apm configuration
 */
#define HP_PERI1_PMS_HP_ZERO_DET_LOCK    (BIT(8))
#define HP_PERI1_PMS_HP_ZERO_DET_LOCK_M  (HP_PERI1_PMS_HP_ZERO_DET_LOCK_V << HP_PERI1_PMS_HP_ZERO_DET_LOCK_S)
#define HP_PERI1_PMS_HP_ZERO_DET_LOCK_V  0x00000001U
#define HP_PERI1_PMS_HP_ZERO_DET_LOCK_S  8

/** HP_PERI1_PMS_HP_CORDIC_CTRL_REG register
 *  hp_cordic read/write control register
 */
#define HP_PERI1_PMS_HP_CORDIC_CTRL_REG (DR_REG_HP_PERI1_PMS_BASE + 0x68)
/** HP_PERI1_PMS_READ_TEE_HP_CORDIC : R/W; bitpos: [0]; default: 1;
 *  Configures hp_cordic registers read permission in tee mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI1_PMS_READ_TEE_HP_CORDIC    (BIT(0))
#define HP_PERI1_PMS_READ_TEE_HP_CORDIC_M  (HP_PERI1_PMS_READ_TEE_HP_CORDIC_V << HP_PERI1_PMS_READ_TEE_HP_CORDIC_S)
#define HP_PERI1_PMS_READ_TEE_HP_CORDIC_V  0x00000001U
#define HP_PERI1_PMS_READ_TEE_HP_CORDIC_S  0
/** HP_PERI1_PMS_READ_REE0_HP_CORDIC : R/W; bitpos: [1]; default: 0;
 *  Configures hp_cordic registers read permission in ree0 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI1_PMS_READ_REE0_HP_CORDIC    (BIT(1))
#define HP_PERI1_PMS_READ_REE0_HP_CORDIC_M  (HP_PERI1_PMS_READ_REE0_HP_CORDIC_V << HP_PERI1_PMS_READ_REE0_HP_CORDIC_S)
#define HP_PERI1_PMS_READ_REE0_HP_CORDIC_V  0x00000001U
#define HP_PERI1_PMS_READ_REE0_HP_CORDIC_S  1
/** HP_PERI1_PMS_READ_REE1_HP_CORDIC : R/W; bitpos: [2]; default: 0;
 *  Configures hp_cordic registers read permission in ree1 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI1_PMS_READ_REE1_HP_CORDIC    (BIT(2))
#define HP_PERI1_PMS_READ_REE1_HP_CORDIC_M  (HP_PERI1_PMS_READ_REE1_HP_CORDIC_V << HP_PERI1_PMS_READ_REE1_HP_CORDIC_S)
#define HP_PERI1_PMS_READ_REE1_HP_CORDIC_V  0x00000001U
#define HP_PERI1_PMS_READ_REE1_HP_CORDIC_S  2
/** HP_PERI1_PMS_READ_REE2_HP_CORDIC : R/W; bitpos: [3]; default: 0;
 *  Configures hp_cordic registers read permission in ree2 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI1_PMS_READ_REE2_HP_CORDIC    (BIT(3))
#define HP_PERI1_PMS_READ_REE2_HP_CORDIC_M  (HP_PERI1_PMS_READ_REE2_HP_CORDIC_V << HP_PERI1_PMS_READ_REE2_HP_CORDIC_S)
#define HP_PERI1_PMS_READ_REE2_HP_CORDIC_V  0x00000001U
#define HP_PERI1_PMS_READ_REE2_HP_CORDIC_S  3
/** HP_PERI1_PMS_WRITE_TEE_HP_CORDIC : R/W; bitpos: [4]; default: 1;
 *  Configures hp_cordic registers write permission in tee mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI1_PMS_WRITE_TEE_HP_CORDIC    (BIT(4))
#define HP_PERI1_PMS_WRITE_TEE_HP_CORDIC_M  (HP_PERI1_PMS_WRITE_TEE_HP_CORDIC_V << HP_PERI1_PMS_WRITE_TEE_HP_CORDIC_S)
#define HP_PERI1_PMS_WRITE_TEE_HP_CORDIC_V  0x00000001U
#define HP_PERI1_PMS_WRITE_TEE_HP_CORDIC_S  4
/** HP_PERI1_PMS_WRITE_REE0_HP_CORDIC : R/W; bitpos: [5]; default: 0;
 *  Configures hp_cordic registers write permission in ree0 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI1_PMS_WRITE_REE0_HP_CORDIC    (BIT(5))
#define HP_PERI1_PMS_WRITE_REE0_HP_CORDIC_M  (HP_PERI1_PMS_WRITE_REE0_HP_CORDIC_V << HP_PERI1_PMS_WRITE_REE0_HP_CORDIC_S)
#define HP_PERI1_PMS_WRITE_REE0_HP_CORDIC_V  0x00000001U
#define HP_PERI1_PMS_WRITE_REE0_HP_CORDIC_S  5
/** HP_PERI1_PMS_WRITE_REE1_HP_CORDIC : R/W; bitpos: [6]; default: 0;
 *  Configures hp_cordic registers write permission in ree1 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI1_PMS_WRITE_REE1_HP_CORDIC    (BIT(6))
#define HP_PERI1_PMS_WRITE_REE1_HP_CORDIC_M  (HP_PERI1_PMS_WRITE_REE1_HP_CORDIC_V << HP_PERI1_PMS_WRITE_REE1_HP_CORDIC_S)
#define HP_PERI1_PMS_WRITE_REE1_HP_CORDIC_V  0x00000001U
#define HP_PERI1_PMS_WRITE_REE1_HP_CORDIC_S  6
/** HP_PERI1_PMS_WRITE_REE2_HP_CORDIC : R/W; bitpos: [7]; default: 0;
 *  Configures hp_cordic registers write permission in ree2 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI1_PMS_WRITE_REE2_HP_CORDIC    (BIT(7))
#define HP_PERI1_PMS_WRITE_REE2_HP_CORDIC_M  (HP_PERI1_PMS_WRITE_REE2_HP_CORDIC_V << HP_PERI1_PMS_WRITE_REE2_HP_CORDIC_S)
#define HP_PERI1_PMS_WRITE_REE2_HP_CORDIC_V  0x00000001U
#define HP_PERI1_PMS_WRITE_REE2_HP_CORDIC_S  7
/** HP_PERI1_PMS_HP_CORDIC_LOCK : R/W; bitpos: [8]; default: 0;
 *  Set 1 to lock hp_cordic peri_apm configuration
 */
#define HP_PERI1_PMS_HP_CORDIC_LOCK    (BIT(8))
#define HP_PERI1_PMS_HP_CORDIC_LOCK_M  (HP_PERI1_PMS_HP_CORDIC_LOCK_V << HP_PERI1_PMS_HP_CORDIC_LOCK_S)
#define HP_PERI1_PMS_HP_CORDIC_LOCK_V  0x00000001U
#define HP_PERI1_PMS_HP_CORDIC_LOCK_S  8

/** HP_PERI1_PMS_HP_LEDC1_CTRL_REG register
 *  hp_ledc1 read/write control register
 */
#define HP_PERI1_PMS_HP_LEDC1_CTRL_REG (DR_REG_HP_PERI1_PMS_BASE + 0x6c)
/** HP_PERI1_PMS_READ_TEE_HP_LEDC1 : R/W; bitpos: [0]; default: 1;
 *  Configures hp_ledc1 registers read permission in tee mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI1_PMS_READ_TEE_HP_LEDC1    (BIT(0))
#define HP_PERI1_PMS_READ_TEE_HP_LEDC1_M  (HP_PERI1_PMS_READ_TEE_HP_LEDC1_V << HP_PERI1_PMS_READ_TEE_HP_LEDC1_S)
#define HP_PERI1_PMS_READ_TEE_HP_LEDC1_V  0x00000001U
#define HP_PERI1_PMS_READ_TEE_HP_LEDC1_S  0
/** HP_PERI1_PMS_READ_REE0_HP_LEDC1 : R/W; bitpos: [1]; default: 0;
 *  Configures hp_ledc1 registers read permission in ree0 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI1_PMS_READ_REE0_HP_LEDC1    (BIT(1))
#define HP_PERI1_PMS_READ_REE0_HP_LEDC1_M  (HP_PERI1_PMS_READ_REE0_HP_LEDC1_V << HP_PERI1_PMS_READ_REE0_HP_LEDC1_S)
#define HP_PERI1_PMS_READ_REE0_HP_LEDC1_V  0x00000001U
#define HP_PERI1_PMS_READ_REE0_HP_LEDC1_S  1
/** HP_PERI1_PMS_READ_REE1_HP_LEDC1 : R/W; bitpos: [2]; default: 0;
 *  Configures hp_ledc1 registers read permission in ree1 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI1_PMS_READ_REE1_HP_LEDC1    (BIT(2))
#define HP_PERI1_PMS_READ_REE1_HP_LEDC1_M  (HP_PERI1_PMS_READ_REE1_HP_LEDC1_V << HP_PERI1_PMS_READ_REE1_HP_LEDC1_S)
#define HP_PERI1_PMS_READ_REE1_HP_LEDC1_V  0x00000001U
#define HP_PERI1_PMS_READ_REE1_HP_LEDC1_S  2
/** HP_PERI1_PMS_READ_REE2_HP_LEDC1 : R/W; bitpos: [3]; default: 0;
 *  Configures hp_ledc1 registers read permission in ree2 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI1_PMS_READ_REE2_HP_LEDC1    (BIT(3))
#define HP_PERI1_PMS_READ_REE2_HP_LEDC1_M  (HP_PERI1_PMS_READ_REE2_HP_LEDC1_V << HP_PERI1_PMS_READ_REE2_HP_LEDC1_S)
#define HP_PERI1_PMS_READ_REE2_HP_LEDC1_V  0x00000001U
#define HP_PERI1_PMS_READ_REE2_HP_LEDC1_S  3
/** HP_PERI1_PMS_WRITE_TEE_HP_LEDC1 : R/W; bitpos: [4]; default: 1;
 *  Configures hp_ledc1 registers write permission in tee mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI1_PMS_WRITE_TEE_HP_LEDC1    (BIT(4))
#define HP_PERI1_PMS_WRITE_TEE_HP_LEDC1_M  (HP_PERI1_PMS_WRITE_TEE_HP_LEDC1_V << HP_PERI1_PMS_WRITE_TEE_HP_LEDC1_S)
#define HP_PERI1_PMS_WRITE_TEE_HP_LEDC1_V  0x00000001U
#define HP_PERI1_PMS_WRITE_TEE_HP_LEDC1_S  4
/** HP_PERI1_PMS_WRITE_REE0_HP_LEDC1 : R/W; bitpos: [5]; default: 0;
 *  Configures hp_ledc1 registers write permission in ree0 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI1_PMS_WRITE_REE0_HP_LEDC1    (BIT(5))
#define HP_PERI1_PMS_WRITE_REE0_HP_LEDC1_M  (HP_PERI1_PMS_WRITE_REE0_HP_LEDC1_V << HP_PERI1_PMS_WRITE_REE0_HP_LEDC1_S)
#define HP_PERI1_PMS_WRITE_REE0_HP_LEDC1_V  0x00000001U
#define HP_PERI1_PMS_WRITE_REE0_HP_LEDC1_S  5
/** HP_PERI1_PMS_WRITE_REE1_HP_LEDC1 : R/W; bitpos: [6]; default: 0;
 *  Configures hp_ledc1 registers write permission in ree1 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI1_PMS_WRITE_REE1_HP_LEDC1    (BIT(6))
#define HP_PERI1_PMS_WRITE_REE1_HP_LEDC1_M  (HP_PERI1_PMS_WRITE_REE1_HP_LEDC1_V << HP_PERI1_PMS_WRITE_REE1_HP_LEDC1_S)
#define HP_PERI1_PMS_WRITE_REE1_HP_LEDC1_V  0x00000001U
#define HP_PERI1_PMS_WRITE_REE1_HP_LEDC1_S  6
/** HP_PERI1_PMS_WRITE_REE2_HP_LEDC1 : R/W; bitpos: [7]; default: 0;
 *  Configures hp_ledc1 registers write permission in ree2 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI1_PMS_WRITE_REE2_HP_LEDC1    (BIT(7))
#define HP_PERI1_PMS_WRITE_REE2_HP_LEDC1_M  (HP_PERI1_PMS_WRITE_REE2_HP_LEDC1_V << HP_PERI1_PMS_WRITE_REE2_HP_LEDC1_S)
#define HP_PERI1_PMS_WRITE_REE2_HP_LEDC1_V  0x00000001U
#define HP_PERI1_PMS_WRITE_REE2_HP_LEDC1_S  7
/** HP_PERI1_PMS_HP_LEDC1_LOCK : R/W; bitpos: [8]; default: 0;
 *  Set 1 to lock hp_ledc1 peri_apm configuration
 */
#define HP_PERI1_PMS_HP_LEDC1_LOCK    (BIT(8))
#define HP_PERI1_PMS_HP_LEDC1_LOCK_M  (HP_PERI1_PMS_HP_LEDC1_LOCK_V << HP_PERI1_PMS_HP_LEDC1_LOCK_S)
#define HP_PERI1_PMS_HP_LEDC1_LOCK_V  0x00000001U
#define HP_PERI1_PMS_HP_LEDC1_LOCK_S  8

/** HP_PERI1_PMS_HP_PCNT1_CTRL_REG register
 *  hp_pcnt1 read/write control register
 */
#define HP_PERI1_PMS_HP_PCNT1_CTRL_REG (DR_REG_HP_PERI1_PMS_BASE + 0x70)
/** HP_PERI1_PMS_READ_TEE_HP_PCNT1 : R/W; bitpos: [0]; default: 1;
 *  Configures hp_pcnt1 registers read permission in tee mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI1_PMS_READ_TEE_HP_PCNT1    (BIT(0))
#define HP_PERI1_PMS_READ_TEE_HP_PCNT1_M  (HP_PERI1_PMS_READ_TEE_HP_PCNT1_V << HP_PERI1_PMS_READ_TEE_HP_PCNT1_S)
#define HP_PERI1_PMS_READ_TEE_HP_PCNT1_V  0x00000001U
#define HP_PERI1_PMS_READ_TEE_HP_PCNT1_S  0
/** HP_PERI1_PMS_READ_REE0_HP_PCNT1 : R/W; bitpos: [1]; default: 0;
 *  Configures hp_pcnt1 registers read permission in ree0 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI1_PMS_READ_REE0_HP_PCNT1    (BIT(1))
#define HP_PERI1_PMS_READ_REE0_HP_PCNT1_M  (HP_PERI1_PMS_READ_REE0_HP_PCNT1_V << HP_PERI1_PMS_READ_REE0_HP_PCNT1_S)
#define HP_PERI1_PMS_READ_REE0_HP_PCNT1_V  0x00000001U
#define HP_PERI1_PMS_READ_REE0_HP_PCNT1_S  1
/** HP_PERI1_PMS_READ_REE1_HP_PCNT1 : R/W; bitpos: [2]; default: 0;
 *  Configures hp_pcnt1 registers read permission in ree1 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI1_PMS_READ_REE1_HP_PCNT1    (BIT(2))
#define HP_PERI1_PMS_READ_REE1_HP_PCNT1_M  (HP_PERI1_PMS_READ_REE1_HP_PCNT1_V << HP_PERI1_PMS_READ_REE1_HP_PCNT1_S)
#define HP_PERI1_PMS_READ_REE1_HP_PCNT1_V  0x00000001U
#define HP_PERI1_PMS_READ_REE1_HP_PCNT1_S  2
/** HP_PERI1_PMS_READ_REE2_HP_PCNT1 : R/W; bitpos: [3]; default: 0;
 *  Configures hp_pcnt1 registers read permission in ree2 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI1_PMS_READ_REE2_HP_PCNT1    (BIT(3))
#define HP_PERI1_PMS_READ_REE2_HP_PCNT1_M  (HP_PERI1_PMS_READ_REE2_HP_PCNT1_V << HP_PERI1_PMS_READ_REE2_HP_PCNT1_S)
#define HP_PERI1_PMS_READ_REE2_HP_PCNT1_V  0x00000001U
#define HP_PERI1_PMS_READ_REE2_HP_PCNT1_S  3
/** HP_PERI1_PMS_WRITE_TEE_HP_PCNT1 : R/W; bitpos: [4]; default: 1;
 *  Configures hp_pcnt1 registers write permission in tee mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI1_PMS_WRITE_TEE_HP_PCNT1    (BIT(4))
#define HP_PERI1_PMS_WRITE_TEE_HP_PCNT1_M  (HP_PERI1_PMS_WRITE_TEE_HP_PCNT1_V << HP_PERI1_PMS_WRITE_TEE_HP_PCNT1_S)
#define HP_PERI1_PMS_WRITE_TEE_HP_PCNT1_V  0x00000001U
#define HP_PERI1_PMS_WRITE_TEE_HP_PCNT1_S  4
/** HP_PERI1_PMS_WRITE_REE0_HP_PCNT1 : R/W; bitpos: [5]; default: 0;
 *  Configures hp_pcnt1 registers write permission in ree0 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI1_PMS_WRITE_REE0_HP_PCNT1    (BIT(5))
#define HP_PERI1_PMS_WRITE_REE0_HP_PCNT1_M  (HP_PERI1_PMS_WRITE_REE0_HP_PCNT1_V << HP_PERI1_PMS_WRITE_REE0_HP_PCNT1_S)
#define HP_PERI1_PMS_WRITE_REE0_HP_PCNT1_V  0x00000001U
#define HP_PERI1_PMS_WRITE_REE0_HP_PCNT1_S  5
/** HP_PERI1_PMS_WRITE_REE1_HP_PCNT1 : R/W; bitpos: [6]; default: 0;
 *  Configures hp_pcnt1 registers write permission in ree1 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI1_PMS_WRITE_REE1_HP_PCNT1    (BIT(6))
#define HP_PERI1_PMS_WRITE_REE1_HP_PCNT1_M  (HP_PERI1_PMS_WRITE_REE1_HP_PCNT1_V << HP_PERI1_PMS_WRITE_REE1_HP_PCNT1_S)
#define HP_PERI1_PMS_WRITE_REE1_HP_PCNT1_V  0x00000001U
#define HP_PERI1_PMS_WRITE_REE1_HP_PCNT1_S  6
/** HP_PERI1_PMS_WRITE_REE2_HP_PCNT1 : R/W; bitpos: [7]; default: 0;
 *  Configures hp_pcnt1 registers write permission in ree2 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI1_PMS_WRITE_REE2_HP_PCNT1    (BIT(7))
#define HP_PERI1_PMS_WRITE_REE2_HP_PCNT1_M  (HP_PERI1_PMS_WRITE_REE2_HP_PCNT1_V << HP_PERI1_PMS_WRITE_REE2_HP_PCNT1_S)
#define HP_PERI1_PMS_WRITE_REE2_HP_PCNT1_V  0x00000001U
#define HP_PERI1_PMS_WRITE_REE2_HP_PCNT1_S  7
/** HP_PERI1_PMS_HP_PCNT1_LOCK : R/W; bitpos: [8]; default: 0;
 *  Set 1 to lock hp_pcnt1 peri_apm configuration
 */
#define HP_PERI1_PMS_HP_PCNT1_LOCK    (BIT(8))
#define HP_PERI1_PMS_HP_PCNT1_LOCK_M  (HP_PERI1_PMS_HP_PCNT1_LOCK_V << HP_PERI1_PMS_HP_PCNT1_LOCK_S)
#define HP_PERI1_PMS_HP_PCNT1_LOCK_V  0x00000001U
#define HP_PERI1_PMS_HP_PCNT1_LOCK_S  8

/** HP_PERI1_PMS_HP_TIMER_GROUP0_CTRL_REG register
 *  hp_timer_group0 read/write control register
 */
#define HP_PERI1_PMS_HP_TIMER_GROUP0_CTRL_REG (DR_REG_HP_PERI1_PMS_BASE + 0x74)
/** HP_PERI1_PMS_READ_TEE_HP_TIMER_GROUP0 : R/W; bitpos: [0]; default: 1;
 *  Configures hp_timer_group0 registers read permission in tee mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI1_PMS_READ_TEE_HP_TIMER_GROUP0    (BIT(0))
#define HP_PERI1_PMS_READ_TEE_HP_TIMER_GROUP0_M  (HP_PERI1_PMS_READ_TEE_HP_TIMER_GROUP0_V << HP_PERI1_PMS_READ_TEE_HP_TIMER_GROUP0_S)
#define HP_PERI1_PMS_READ_TEE_HP_TIMER_GROUP0_V  0x00000001U
#define HP_PERI1_PMS_READ_TEE_HP_TIMER_GROUP0_S  0
/** HP_PERI1_PMS_READ_REE0_HP_TIMER_GROUP0 : R/W; bitpos: [1]; default: 0;
 *  Configures hp_timer_group0 registers read permission in ree0 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI1_PMS_READ_REE0_HP_TIMER_GROUP0    (BIT(1))
#define HP_PERI1_PMS_READ_REE0_HP_TIMER_GROUP0_M  (HP_PERI1_PMS_READ_REE0_HP_TIMER_GROUP0_V << HP_PERI1_PMS_READ_REE0_HP_TIMER_GROUP0_S)
#define HP_PERI1_PMS_READ_REE0_HP_TIMER_GROUP0_V  0x00000001U
#define HP_PERI1_PMS_READ_REE0_HP_TIMER_GROUP0_S  1
/** HP_PERI1_PMS_READ_REE1_HP_TIMER_GROUP0 : R/W; bitpos: [2]; default: 0;
 *  Configures hp_timer_group0 registers read permission in ree1 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI1_PMS_READ_REE1_HP_TIMER_GROUP0    (BIT(2))
#define HP_PERI1_PMS_READ_REE1_HP_TIMER_GROUP0_M  (HP_PERI1_PMS_READ_REE1_HP_TIMER_GROUP0_V << HP_PERI1_PMS_READ_REE1_HP_TIMER_GROUP0_S)
#define HP_PERI1_PMS_READ_REE1_HP_TIMER_GROUP0_V  0x00000001U
#define HP_PERI1_PMS_READ_REE1_HP_TIMER_GROUP0_S  2
/** HP_PERI1_PMS_READ_REE2_HP_TIMER_GROUP0 : R/W; bitpos: [3]; default: 0;
 *  Configures hp_timer_group0 registers read permission in ree2 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI1_PMS_READ_REE2_HP_TIMER_GROUP0    (BIT(3))
#define HP_PERI1_PMS_READ_REE2_HP_TIMER_GROUP0_M  (HP_PERI1_PMS_READ_REE2_HP_TIMER_GROUP0_V << HP_PERI1_PMS_READ_REE2_HP_TIMER_GROUP0_S)
#define HP_PERI1_PMS_READ_REE2_HP_TIMER_GROUP0_V  0x00000001U
#define HP_PERI1_PMS_READ_REE2_HP_TIMER_GROUP0_S  3
/** HP_PERI1_PMS_WRITE_TEE_HP_TIMER_GROUP0 : R/W; bitpos: [4]; default: 1;
 *  Configures hp_timer_group0 registers write permission in tee mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI1_PMS_WRITE_TEE_HP_TIMER_GROUP0    (BIT(4))
#define HP_PERI1_PMS_WRITE_TEE_HP_TIMER_GROUP0_M  (HP_PERI1_PMS_WRITE_TEE_HP_TIMER_GROUP0_V << HP_PERI1_PMS_WRITE_TEE_HP_TIMER_GROUP0_S)
#define HP_PERI1_PMS_WRITE_TEE_HP_TIMER_GROUP0_V  0x00000001U
#define HP_PERI1_PMS_WRITE_TEE_HP_TIMER_GROUP0_S  4
/** HP_PERI1_PMS_WRITE_REE0_HP_TIMER_GROUP0 : R/W; bitpos: [5]; default: 0;
 *  Configures hp_timer_group0 registers write permission in ree0 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI1_PMS_WRITE_REE0_HP_TIMER_GROUP0    (BIT(5))
#define HP_PERI1_PMS_WRITE_REE0_HP_TIMER_GROUP0_M  (HP_PERI1_PMS_WRITE_REE0_HP_TIMER_GROUP0_V << HP_PERI1_PMS_WRITE_REE0_HP_TIMER_GROUP0_S)
#define HP_PERI1_PMS_WRITE_REE0_HP_TIMER_GROUP0_V  0x00000001U
#define HP_PERI1_PMS_WRITE_REE0_HP_TIMER_GROUP0_S  5
/** HP_PERI1_PMS_WRITE_REE1_HP_TIMER_GROUP0 : R/W; bitpos: [6]; default: 0;
 *  Configures hp_timer_group0 registers write permission in ree1 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI1_PMS_WRITE_REE1_HP_TIMER_GROUP0    (BIT(6))
#define HP_PERI1_PMS_WRITE_REE1_HP_TIMER_GROUP0_M  (HP_PERI1_PMS_WRITE_REE1_HP_TIMER_GROUP0_V << HP_PERI1_PMS_WRITE_REE1_HP_TIMER_GROUP0_S)
#define HP_PERI1_PMS_WRITE_REE1_HP_TIMER_GROUP0_V  0x00000001U
#define HP_PERI1_PMS_WRITE_REE1_HP_TIMER_GROUP0_S  6
/** HP_PERI1_PMS_WRITE_REE2_HP_TIMER_GROUP0 : R/W; bitpos: [7]; default: 0;
 *  Configures hp_timer_group0 registers write permission in ree2 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI1_PMS_WRITE_REE2_HP_TIMER_GROUP0    (BIT(7))
#define HP_PERI1_PMS_WRITE_REE2_HP_TIMER_GROUP0_M  (HP_PERI1_PMS_WRITE_REE2_HP_TIMER_GROUP0_V << HP_PERI1_PMS_WRITE_REE2_HP_TIMER_GROUP0_S)
#define HP_PERI1_PMS_WRITE_REE2_HP_TIMER_GROUP0_V  0x00000001U
#define HP_PERI1_PMS_WRITE_REE2_HP_TIMER_GROUP0_S  7
/** HP_PERI1_PMS_HP_TIMER_GROUP0_LOCK : R/W; bitpos: [8]; default: 0;
 *  Set 1 to lock hp_timer_group0 peri_apm configuration
 */
#define HP_PERI1_PMS_HP_TIMER_GROUP0_LOCK    (BIT(8))
#define HP_PERI1_PMS_HP_TIMER_GROUP0_LOCK_M  (HP_PERI1_PMS_HP_TIMER_GROUP0_LOCK_V << HP_PERI1_PMS_HP_TIMER_GROUP0_LOCK_S)
#define HP_PERI1_PMS_HP_TIMER_GROUP0_LOCK_V  0x00000001U
#define HP_PERI1_PMS_HP_TIMER_GROUP0_LOCK_S  8

/** HP_PERI1_PMS_HP_TIMER_GROUP1_CTRL_REG register
 *  hp_timer_group1 read/write control register
 */
#define HP_PERI1_PMS_HP_TIMER_GROUP1_CTRL_REG (DR_REG_HP_PERI1_PMS_BASE + 0x78)
/** HP_PERI1_PMS_READ_TEE_HP_TIMER_GROUP1 : R/W; bitpos: [0]; default: 1;
 *  Configures hp_timer_group1 registers read permission in tee mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI1_PMS_READ_TEE_HP_TIMER_GROUP1    (BIT(0))
#define HP_PERI1_PMS_READ_TEE_HP_TIMER_GROUP1_M  (HP_PERI1_PMS_READ_TEE_HP_TIMER_GROUP1_V << HP_PERI1_PMS_READ_TEE_HP_TIMER_GROUP1_S)
#define HP_PERI1_PMS_READ_TEE_HP_TIMER_GROUP1_V  0x00000001U
#define HP_PERI1_PMS_READ_TEE_HP_TIMER_GROUP1_S  0
/** HP_PERI1_PMS_READ_REE0_HP_TIMER_GROUP1 : R/W; bitpos: [1]; default: 0;
 *  Configures hp_timer_group1 registers read permission in ree0 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI1_PMS_READ_REE0_HP_TIMER_GROUP1    (BIT(1))
#define HP_PERI1_PMS_READ_REE0_HP_TIMER_GROUP1_M  (HP_PERI1_PMS_READ_REE0_HP_TIMER_GROUP1_V << HP_PERI1_PMS_READ_REE0_HP_TIMER_GROUP1_S)
#define HP_PERI1_PMS_READ_REE0_HP_TIMER_GROUP1_V  0x00000001U
#define HP_PERI1_PMS_READ_REE0_HP_TIMER_GROUP1_S  1
/** HP_PERI1_PMS_READ_REE1_HP_TIMER_GROUP1 : R/W; bitpos: [2]; default: 0;
 *  Configures hp_timer_group1 registers read permission in ree1 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI1_PMS_READ_REE1_HP_TIMER_GROUP1    (BIT(2))
#define HP_PERI1_PMS_READ_REE1_HP_TIMER_GROUP1_M  (HP_PERI1_PMS_READ_REE1_HP_TIMER_GROUP1_V << HP_PERI1_PMS_READ_REE1_HP_TIMER_GROUP1_S)
#define HP_PERI1_PMS_READ_REE1_HP_TIMER_GROUP1_V  0x00000001U
#define HP_PERI1_PMS_READ_REE1_HP_TIMER_GROUP1_S  2
/** HP_PERI1_PMS_READ_REE2_HP_TIMER_GROUP1 : R/W; bitpos: [3]; default: 0;
 *  Configures hp_timer_group1 registers read permission in ree2 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI1_PMS_READ_REE2_HP_TIMER_GROUP1    (BIT(3))
#define HP_PERI1_PMS_READ_REE2_HP_TIMER_GROUP1_M  (HP_PERI1_PMS_READ_REE2_HP_TIMER_GROUP1_V << HP_PERI1_PMS_READ_REE2_HP_TIMER_GROUP1_S)
#define HP_PERI1_PMS_READ_REE2_HP_TIMER_GROUP1_V  0x00000001U
#define HP_PERI1_PMS_READ_REE2_HP_TIMER_GROUP1_S  3
/** HP_PERI1_PMS_WRITE_TEE_HP_TIMER_GROUP1 : R/W; bitpos: [4]; default: 1;
 *  Configures hp_timer_group1 registers write permission in tee mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI1_PMS_WRITE_TEE_HP_TIMER_GROUP1    (BIT(4))
#define HP_PERI1_PMS_WRITE_TEE_HP_TIMER_GROUP1_M  (HP_PERI1_PMS_WRITE_TEE_HP_TIMER_GROUP1_V << HP_PERI1_PMS_WRITE_TEE_HP_TIMER_GROUP1_S)
#define HP_PERI1_PMS_WRITE_TEE_HP_TIMER_GROUP1_V  0x00000001U
#define HP_PERI1_PMS_WRITE_TEE_HP_TIMER_GROUP1_S  4
/** HP_PERI1_PMS_WRITE_REE0_HP_TIMER_GROUP1 : R/W; bitpos: [5]; default: 0;
 *  Configures hp_timer_group1 registers write permission in ree0 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI1_PMS_WRITE_REE0_HP_TIMER_GROUP1    (BIT(5))
#define HP_PERI1_PMS_WRITE_REE0_HP_TIMER_GROUP1_M  (HP_PERI1_PMS_WRITE_REE0_HP_TIMER_GROUP1_V << HP_PERI1_PMS_WRITE_REE0_HP_TIMER_GROUP1_S)
#define HP_PERI1_PMS_WRITE_REE0_HP_TIMER_GROUP1_V  0x00000001U
#define HP_PERI1_PMS_WRITE_REE0_HP_TIMER_GROUP1_S  5
/** HP_PERI1_PMS_WRITE_REE1_HP_TIMER_GROUP1 : R/W; bitpos: [6]; default: 0;
 *  Configures hp_timer_group1 registers write permission in ree1 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI1_PMS_WRITE_REE1_HP_TIMER_GROUP1    (BIT(6))
#define HP_PERI1_PMS_WRITE_REE1_HP_TIMER_GROUP1_M  (HP_PERI1_PMS_WRITE_REE1_HP_TIMER_GROUP1_V << HP_PERI1_PMS_WRITE_REE1_HP_TIMER_GROUP1_S)
#define HP_PERI1_PMS_WRITE_REE1_HP_TIMER_GROUP1_V  0x00000001U
#define HP_PERI1_PMS_WRITE_REE1_HP_TIMER_GROUP1_S  6
/** HP_PERI1_PMS_WRITE_REE2_HP_TIMER_GROUP1 : R/W; bitpos: [7]; default: 0;
 *  Configures hp_timer_group1 registers write permission in ree2 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI1_PMS_WRITE_REE2_HP_TIMER_GROUP1    (BIT(7))
#define HP_PERI1_PMS_WRITE_REE2_HP_TIMER_GROUP1_M  (HP_PERI1_PMS_WRITE_REE2_HP_TIMER_GROUP1_V << HP_PERI1_PMS_WRITE_REE2_HP_TIMER_GROUP1_S)
#define HP_PERI1_PMS_WRITE_REE2_HP_TIMER_GROUP1_V  0x00000001U
#define HP_PERI1_PMS_WRITE_REE2_HP_TIMER_GROUP1_S  7
/** HP_PERI1_PMS_HP_TIMER_GROUP1_LOCK : R/W; bitpos: [8]; default: 0;
 *  Set 1 to lock hp_timer_group1 peri_apm configuration
 */
#define HP_PERI1_PMS_HP_TIMER_GROUP1_LOCK    (BIT(8))
#define HP_PERI1_PMS_HP_TIMER_GROUP1_LOCK_M  (HP_PERI1_PMS_HP_TIMER_GROUP1_LOCK_V << HP_PERI1_PMS_HP_TIMER_GROUP1_LOCK_S)
#define HP_PERI1_PMS_HP_TIMER_GROUP1_LOCK_V  0x00000001U
#define HP_PERI1_PMS_HP_TIMER_GROUP1_LOCK_S  8

/** HP_PERI1_PMS_HP_IOMUX_CTRL_REG register
 *  hp_iomux read/write control register
 */
#define HP_PERI1_PMS_HP_IOMUX_CTRL_REG (DR_REG_HP_PERI1_PMS_BASE + 0x7c)
/** HP_PERI1_PMS_READ_TEE_HP_IOMUX : R/W; bitpos: [0]; default: 1;
 *  Configures hp_iomux registers read permission in tee mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI1_PMS_READ_TEE_HP_IOMUX    (BIT(0))
#define HP_PERI1_PMS_READ_TEE_HP_IOMUX_M  (HP_PERI1_PMS_READ_TEE_HP_IOMUX_V << HP_PERI1_PMS_READ_TEE_HP_IOMUX_S)
#define HP_PERI1_PMS_READ_TEE_HP_IOMUX_V  0x00000001U
#define HP_PERI1_PMS_READ_TEE_HP_IOMUX_S  0
/** HP_PERI1_PMS_READ_REE0_HP_IOMUX : R/W; bitpos: [1]; default: 0;
 *  Configures hp_iomux registers read permission in ree0 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI1_PMS_READ_REE0_HP_IOMUX    (BIT(1))
#define HP_PERI1_PMS_READ_REE0_HP_IOMUX_M  (HP_PERI1_PMS_READ_REE0_HP_IOMUX_V << HP_PERI1_PMS_READ_REE0_HP_IOMUX_S)
#define HP_PERI1_PMS_READ_REE0_HP_IOMUX_V  0x00000001U
#define HP_PERI1_PMS_READ_REE0_HP_IOMUX_S  1
/** HP_PERI1_PMS_READ_REE1_HP_IOMUX : R/W; bitpos: [2]; default: 0;
 *  Configures hp_iomux registers read permission in ree1 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI1_PMS_READ_REE1_HP_IOMUX    (BIT(2))
#define HP_PERI1_PMS_READ_REE1_HP_IOMUX_M  (HP_PERI1_PMS_READ_REE1_HP_IOMUX_V << HP_PERI1_PMS_READ_REE1_HP_IOMUX_S)
#define HP_PERI1_PMS_READ_REE1_HP_IOMUX_V  0x00000001U
#define HP_PERI1_PMS_READ_REE1_HP_IOMUX_S  2
/** HP_PERI1_PMS_READ_REE2_HP_IOMUX : R/W; bitpos: [3]; default: 0;
 *  Configures hp_iomux registers read permission in ree2 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI1_PMS_READ_REE2_HP_IOMUX    (BIT(3))
#define HP_PERI1_PMS_READ_REE2_HP_IOMUX_M  (HP_PERI1_PMS_READ_REE2_HP_IOMUX_V << HP_PERI1_PMS_READ_REE2_HP_IOMUX_S)
#define HP_PERI1_PMS_READ_REE2_HP_IOMUX_V  0x00000001U
#define HP_PERI1_PMS_READ_REE2_HP_IOMUX_S  3
/** HP_PERI1_PMS_WRITE_TEE_HP_IOMUX : R/W; bitpos: [4]; default: 1;
 *  Configures hp_iomux registers write permission in tee mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI1_PMS_WRITE_TEE_HP_IOMUX    (BIT(4))
#define HP_PERI1_PMS_WRITE_TEE_HP_IOMUX_M  (HP_PERI1_PMS_WRITE_TEE_HP_IOMUX_V << HP_PERI1_PMS_WRITE_TEE_HP_IOMUX_S)
#define HP_PERI1_PMS_WRITE_TEE_HP_IOMUX_V  0x00000001U
#define HP_PERI1_PMS_WRITE_TEE_HP_IOMUX_S  4
/** HP_PERI1_PMS_WRITE_REE0_HP_IOMUX : R/W; bitpos: [5]; default: 0;
 *  Configures hp_iomux registers write permission in ree0 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI1_PMS_WRITE_REE0_HP_IOMUX    (BIT(5))
#define HP_PERI1_PMS_WRITE_REE0_HP_IOMUX_M  (HP_PERI1_PMS_WRITE_REE0_HP_IOMUX_V << HP_PERI1_PMS_WRITE_REE0_HP_IOMUX_S)
#define HP_PERI1_PMS_WRITE_REE0_HP_IOMUX_V  0x00000001U
#define HP_PERI1_PMS_WRITE_REE0_HP_IOMUX_S  5
/** HP_PERI1_PMS_WRITE_REE1_HP_IOMUX : R/W; bitpos: [6]; default: 0;
 *  Configures hp_iomux registers write permission in ree1 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI1_PMS_WRITE_REE1_HP_IOMUX    (BIT(6))
#define HP_PERI1_PMS_WRITE_REE1_HP_IOMUX_M  (HP_PERI1_PMS_WRITE_REE1_HP_IOMUX_V << HP_PERI1_PMS_WRITE_REE1_HP_IOMUX_S)
#define HP_PERI1_PMS_WRITE_REE1_HP_IOMUX_V  0x00000001U
#define HP_PERI1_PMS_WRITE_REE1_HP_IOMUX_S  6
/** HP_PERI1_PMS_WRITE_REE2_HP_IOMUX : R/W; bitpos: [7]; default: 0;
 *  Configures hp_iomux registers write permission in ree2 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI1_PMS_WRITE_REE2_HP_IOMUX    (BIT(7))
#define HP_PERI1_PMS_WRITE_REE2_HP_IOMUX_M  (HP_PERI1_PMS_WRITE_REE2_HP_IOMUX_V << HP_PERI1_PMS_WRITE_REE2_HP_IOMUX_S)
#define HP_PERI1_PMS_WRITE_REE2_HP_IOMUX_V  0x00000001U
#define HP_PERI1_PMS_WRITE_REE2_HP_IOMUX_S  7
/** HP_PERI1_PMS_HP_IOMUX_LOCK : R/W; bitpos: [8]; default: 0;
 *  Set 1 to lock hp_iomux peri_apm configuration
 */
#define HP_PERI1_PMS_HP_IOMUX_LOCK    (BIT(8))
#define HP_PERI1_PMS_HP_IOMUX_LOCK_M  (HP_PERI1_PMS_HP_IOMUX_LOCK_V << HP_PERI1_PMS_HP_IOMUX_LOCK_S)
#define HP_PERI1_PMS_HP_IOMUX_LOCK_V  0x00000001U
#define HP_PERI1_PMS_HP_IOMUX_LOCK_S  8

/** HP_PERI1_PMS_HP_MSPI_PADCTRL_CTRL_REG register
 *  hp_mspi_padctrl read/write control register
 */
#define HP_PERI1_PMS_HP_MSPI_PADCTRL_CTRL_REG (DR_REG_HP_PERI1_PMS_BASE + 0x80)
/** HP_PERI1_PMS_READ_TEE_HP_MSPI_PADCTRL : R/W; bitpos: [0]; default: 1;
 *  Configures hp_mspi_padctrl registers read permission in tee mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI1_PMS_READ_TEE_HP_MSPI_PADCTRL    (BIT(0))
#define HP_PERI1_PMS_READ_TEE_HP_MSPI_PADCTRL_M  (HP_PERI1_PMS_READ_TEE_HP_MSPI_PADCTRL_V << HP_PERI1_PMS_READ_TEE_HP_MSPI_PADCTRL_S)
#define HP_PERI1_PMS_READ_TEE_HP_MSPI_PADCTRL_V  0x00000001U
#define HP_PERI1_PMS_READ_TEE_HP_MSPI_PADCTRL_S  0
/** HP_PERI1_PMS_READ_REE0_HP_MSPI_PADCTRL : R/W; bitpos: [1]; default: 0;
 *  Configures hp_mspi_padctrl registers read permission in ree0 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI1_PMS_READ_REE0_HP_MSPI_PADCTRL    (BIT(1))
#define HP_PERI1_PMS_READ_REE0_HP_MSPI_PADCTRL_M  (HP_PERI1_PMS_READ_REE0_HP_MSPI_PADCTRL_V << HP_PERI1_PMS_READ_REE0_HP_MSPI_PADCTRL_S)
#define HP_PERI1_PMS_READ_REE0_HP_MSPI_PADCTRL_V  0x00000001U
#define HP_PERI1_PMS_READ_REE0_HP_MSPI_PADCTRL_S  1
/** HP_PERI1_PMS_READ_REE1_HP_MSPI_PADCTRL : R/W; bitpos: [2]; default: 0;
 *  Configures hp_mspi_padctrl registers read permission in ree1 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI1_PMS_READ_REE1_HP_MSPI_PADCTRL    (BIT(2))
#define HP_PERI1_PMS_READ_REE1_HP_MSPI_PADCTRL_M  (HP_PERI1_PMS_READ_REE1_HP_MSPI_PADCTRL_V << HP_PERI1_PMS_READ_REE1_HP_MSPI_PADCTRL_S)
#define HP_PERI1_PMS_READ_REE1_HP_MSPI_PADCTRL_V  0x00000001U
#define HP_PERI1_PMS_READ_REE1_HP_MSPI_PADCTRL_S  2
/** HP_PERI1_PMS_READ_REE2_HP_MSPI_PADCTRL : R/W; bitpos: [3]; default: 0;
 *  Configures hp_mspi_padctrl registers read permission in ree2 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI1_PMS_READ_REE2_HP_MSPI_PADCTRL    (BIT(3))
#define HP_PERI1_PMS_READ_REE2_HP_MSPI_PADCTRL_M  (HP_PERI1_PMS_READ_REE2_HP_MSPI_PADCTRL_V << HP_PERI1_PMS_READ_REE2_HP_MSPI_PADCTRL_S)
#define HP_PERI1_PMS_READ_REE2_HP_MSPI_PADCTRL_V  0x00000001U
#define HP_PERI1_PMS_READ_REE2_HP_MSPI_PADCTRL_S  3
/** HP_PERI1_PMS_WRITE_TEE_HP_MSPI_PADCTRL : R/W; bitpos: [4]; default: 1;
 *  Configures hp_mspi_padctrl registers write permission in tee mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI1_PMS_WRITE_TEE_HP_MSPI_PADCTRL    (BIT(4))
#define HP_PERI1_PMS_WRITE_TEE_HP_MSPI_PADCTRL_M  (HP_PERI1_PMS_WRITE_TEE_HP_MSPI_PADCTRL_V << HP_PERI1_PMS_WRITE_TEE_HP_MSPI_PADCTRL_S)
#define HP_PERI1_PMS_WRITE_TEE_HP_MSPI_PADCTRL_V  0x00000001U
#define HP_PERI1_PMS_WRITE_TEE_HP_MSPI_PADCTRL_S  4
/** HP_PERI1_PMS_WRITE_REE0_HP_MSPI_PADCTRL : R/W; bitpos: [5]; default: 0;
 *  Configures hp_mspi_padctrl registers write permission in ree0 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI1_PMS_WRITE_REE0_HP_MSPI_PADCTRL    (BIT(5))
#define HP_PERI1_PMS_WRITE_REE0_HP_MSPI_PADCTRL_M  (HP_PERI1_PMS_WRITE_REE0_HP_MSPI_PADCTRL_V << HP_PERI1_PMS_WRITE_REE0_HP_MSPI_PADCTRL_S)
#define HP_PERI1_PMS_WRITE_REE0_HP_MSPI_PADCTRL_V  0x00000001U
#define HP_PERI1_PMS_WRITE_REE0_HP_MSPI_PADCTRL_S  5
/** HP_PERI1_PMS_WRITE_REE1_HP_MSPI_PADCTRL : R/W; bitpos: [6]; default: 0;
 *  Configures hp_mspi_padctrl registers write permission in ree1 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI1_PMS_WRITE_REE1_HP_MSPI_PADCTRL    (BIT(6))
#define HP_PERI1_PMS_WRITE_REE1_HP_MSPI_PADCTRL_M  (HP_PERI1_PMS_WRITE_REE1_HP_MSPI_PADCTRL_V << HP_PERI1_PMS_WRITE_REE1_HP_MSPI_PADCTRL_S)
#define HP_PERI1_PMS_WRITE_REE1_HP_MSPI_PADCTRL_V  0x00000001U
#define HP_PERI1_PMS_WRITE_REE1_HP_MSPI_PADCTRL_S  6
/** HP_PERI1_PMS_WRITE_REE2_HP_MSPI_PADCTRL : R/W; bitpos: [7]; default: 0;
 *  Configures hp_mspi_padctrl registers write permission in ree2 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI1_PMS_WRITE_REE2_HP_MSPI_PADCTRL    (BIT(7))
#define HP_PERI1_PMS_WRITE_REE2_HP_MSPI_PADCTRL_M  (HP_PERI1_PMS_WRITE_REE2_HP_MSPI_PADCTRL_V << HP_PERI1_PMS_WRITE_REE2_HP_MSPI_PADCTRL_S)
#define HP_PERI1_PMS_WRITE_REE2_HP_MSPI_PADCTRL_V  0x00000001U
#define HP_PERI1_PMS_WRITE_REE2_HP_MSPI_PADCTRL_S  7
/** HP_PERI1_PMS_HP_MSPI_PADCTRL_LOCK : R/W; bitpos: [8]; default: 0;
 *  Set 1 to lock hp_mspi_padctrl peri_apm configuration
 */
#define HP_PERI1_PMS_HP_MSPI_PADCTRL_LOCK    (BIT(8))
#define HP_PERI1_PMS_HP_MSPI_PADCTRL_LOCK_M  (HP_PERI1_PMS_HP_MSPI_PADCTRL_LOCK_V << HP_PERI1_PMS_HP_MSPI_PADCTRL_LOCK_S)
#define HP_PERI1_PMS_HP_MSPI_PADCTRL_LOCK_V  0x00000001U
#define HP_PERI1_PMS_HP_MSPI_PADCTRL_LOCK_S  8

/** HP_PERI1_PMS_HP_INTRMTX_CTRL_REG register
 *  hp_intrmtx read/write control register
 */
#define HP_PERI1_PMS_HP_INTRMTX_CTRL_REG (DR_REG_HP_PERI1_PMS_BASE + 0x84)
/** HP_PERI1_PMS_READ_TEE_HP_INTRMTX : R/W; bitpos: [0]; default: 1;
 *  Configures hp_intrmtx registers read permission in tee mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI1_PMS_READ_TEE_HP_INTRMTX    (BIT(0))
#define HP_PERI1_PMS_READ_TEE_HP_INTRMTX_M  (HP_PERI1_PMS_READ_TEE_HP_INTRMTX_V << HP_PERI1_PMS_READ_TEE_HP_INTRMTX_S)
#define HP_PERI1_PMS_READ_TEE_HP_INTRMTX_V  0x00000001U
#define HP_PERI1_PMS_READ_TEE_HP_INTRMTX_S  0
/** HP_PERI1_PMS_READ_REE0_HP_INTRMTX : R/W; bitpos: [1]; default: 0;
 *  Configures hp_intrmtx registers read permission in ree0 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI1_PMS_READ_REE0_HP_INTRMTX    (BIT(1))
#define HP_PERI1_PMS_READ_REE0_HP_INTRMTX_M  (HP_PERI1_PMS_READ_REE0_HP_INTRMTX_V << HP_PERI1_PMS_READ_REE0_HP_INTRMTX_S)
#define HP_PERI1_PMS_READ_REE0_HP_INTRMTX_V  0x00000001U
#define HP_PERI1_PMS_READ_REE0_HP_INTRMTX_S  1
/** HP_PERI1_PMS_READ_REE1_HP_INTRMTX : R/W; bitpos: [2]; default: 0;
 *  Configures hp_intrmtx registers read permission in ree1 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI1_PMS_READ_REE1_HP_INTRMTX    (BIT(2))
#define HP_PERI1_PMS_READ_REE1_HP_INTRMTX_M  (HP_PERI1_PMS_READ_REE1_HP_INTRMTX_V << HP_PERI1_PMS_READ_REE1_HP_INTRMTX_S)
#define HP_PERI1_PMS_READ_REE1_HP_INTRMTX_V  0x00000001U
#define HP_PERI1_PMS_READ_REE1_HP_INTRMTX_S  2
/** HP_PERI1_PMS_READ_REE2_HP_INTRMTX : R/W; bitpos: [3]; default: 0;
 *  Configures hp_intrmtx registers read permission in ree2 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI1_PMS_READ_REE2_HP_INTRMTX    (BIT(3))
#define HP_PERI1_PMS_READ_REE2_HP_INTRMTX_M  (HP_PERI1_PMS_READ_REE2_HP_INTRMTX_V << HP_PERI1_PMS_READ_REE2_HP_INTRMTX_S)
#define HP_PERI1_PMS_READ_REE2_HP_INTRMTX_V  0x00000001U
#define HP_PERI1_PMS_READ_REE2_HP_INTRMTX_S  3
/** HP_PERI1_PMS_WRITE_TEE_HP_INTRMTX : R/W; bitpos: [4]; default: 1;
 *  Configures hp_intrmtx registers write permission in tee mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI1_PMS_WRITE_TEE_HP_INTRMTX    (BIT(4))
#define HP_PERI1_PMS_WRITE_TEE_HP_INTRMTX_M  (HP_PERI1_PMS_WRITE_TEE_HP_INTRMTX_V << HP_PERI1_PMS_WRITE_TEE_HP_INTRMTX_S)
#define HP_PERI1_PMS_WRITE_TEE_HP_INTRMTX_V  0x00000001U
#define HP_PERI1_PMS_WRITE_TEE_HP_INTRMTX_S  4
/** HP_PERI1_PMS_WRITE_REE0_HP_INTRMTX : R/W; bitpos: [5]; default: 0;
 *  Configures hp_intrmtx registers write permission in ree0 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI1_PMS_WRITE_REE0_HP_INTRMTX    (BIT(5))
#define HP_PERI1_PMS_WRITE_REE0_HP_INTRMTX_M  (HP_PERI1_PMS_WRITE_REE0_HP_INTRMTX_V << HP_PERI1_PMS_WRITE_REE0_HP_INTRMTX_S)
#define HP_PERI1_PMS_WRITE_REE0_HP_INTRMTX_V  0x00000001U
#define HP_PERI1_PMS_WRITE_REE0_HP_INTRMTX_S  5
/** HP_PERI1_PMS_WRITE_REE1_HP_INTRMTX : R/W; bitpos: [6]; default: 0;
 *  Configures hp_intrmtx registers write permission in ree1 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI1_PMS_WRITE_REE1_HP_INTRMTX    (BIT(6))
#define HP_PERI1_PMS_WRITE_REE1_HP_INTRMTX_M  (HP_PERI1_PMS_WRITE_REE1_HP_INTRMTX_V << HP_PERI1_PMS_WRITE_REE1_HP_INTRMTX_S)
#define HP_PERI1_PMS_WRITE_REE1_HP_INTRMTX_V  0x00000001U
#define HP_PERI1_PMS_WRITE_REE1_HP_INTRMTX_S  6
/** HP_PERI1_PMS_WRITE_REE2_HP_INTRMTX : R/W; bitpos: [7]; default: 0;
 *  Configures hp_intrmtx registers write permission in ree2 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI1_PMS_WRITE_REE2_HP_INTRMTX    (BIT(7))
#define HP_PERI1_PMS_WRITE_REE2_HP_INTRMTX_M  (HP_PERI1_PMS_WRITE_REE2_HP_INTRMTX_V << HP_PERI1_PMS_WRITE_REE2_HP_INTRMTX_S)
#define HP_PERI1_PMS_WRITE_REE2_HP_INTRMTX_V  0x00000001U
#define HP_PERI1_PMS_WRITE_REE2_HP_INTRMTX_S  7
/** HP_PERI1_PMS_HP_INTRMTX_LOCK : R/W; bitpos: [8]; default: 0;
 *  Set 1 to lock hp_intrmtx peri_apm configuration
 */
#define HP_PERI1_PMS_HP_INTRMTX_LOCK    (BIT(8))
#define HP_PERI1_PMS_HP_INTRMTX_LOCK_M  (HP_PERI1_PMS_HP_INTRMTX_LOCK_V << HP_PERI1_PMS_HP_INTRMTX_LOCK_S)
#define HP_PERI1_PMS_HP_INTRMTX_LOCK_V  0x00000001U
#define HP_PERI1_PMS_HP_INTRMTX_LOCK_S  8

/** HP_PERI1_PMS_HP_SYS_REG_CTRL_REG register
 *  hp_sys_reg read/write control register
 */
#define HP_PERI1_PMS_HP_SYS_REG_CTRL_REG (DR_REG_HP_PERI1_PMS_BASE + 0x88)
/** HP_PERI1_PMS_READ_TEE_HP_SYS_REG : R/W; bitpos: [0]; default: 1;
 *  Configures hp_sys_reg registers read permission in tee mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI1_PMS_READ_TEE_HP_SYS_REG    (BIT(0))
#define HP_PERI1_PMS_READ_TEE_HP_SYS_REG_M  (HP_PERI1_PMS_READ_TEE_HP_SYS_REG_V << HP_PERI1_PMS_READ_TEE_HP_SYS_REG_S)
#define HP_PERI1_PMS_READ_TEE_HP_SYS_REG_V  0x00000001U
#define HP_PERI1_PMS_READ_TEE_HP_SYS_REG_S  0
/** HP_PERI1_PMS_READ_REE0_HP_SYS_REG : R/W; bitpos: [1]; default: 0;
 *  Configures hp_sys_reg registers read permission in ree0 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI1_PMS_READ_REE0_HP_SYS_REG    (BIT(1))
#define HP_PERI1_PMS_READ_REE0_HP_SYS_REG_M  (HP_PERI1_PMS_READ_REE0_HP_SYS_REG_V << HP_PERI1_PMS_READ_REE0_HP_SYS_REG_S)
#define HP_PERI1_PMS_READ_REE0_HP_SYS_REG_V  0x00000001U
#define HP_PERI1_PMS_READ_REE0_HP_SYS_REG_S  1
/** HP_PERI1_PMS_READ_REE1_HP_SYS_REG : R/W; bitpos: [2]; default: 0;
 *  Configures hp_sys_reg registers read permission in ree1 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI1_PMS_READ_REE1_HP_SYS_REG    (BIT(2))
#define HP_PERI1_PMS_READ_REE1_HP_SYS_REG_M  (HP_PERI1_PMS_READ_REE1_HP_SYS_REG_V << HP_PERI1_PMS_READ_REE1_HP_SYS_REG_S)
#define HP_PERI1_PMS_READ_REE1_HP_SYS_REG_V  0x00000001U
#define HP_PERI1_PMS_READ_REE1_HP_SYS_REG_S  2
/** HP_PERI1_PMS_READ_REE2_HP_SYS_REG : R/W; bitpos: [3]; default: 0;
 *  Configures hp_sys_reg registers read permission in ree2 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI1_PMS_READ_REE2_HP_SYS_REG    (BIT(3))
#define HP_PERI1_PMS_READ_REE2_HP_SYS_REG_M  (HP_PERI1_PMS_READ_REE2_HP_SYS_REG_V << HP_PERI1_PMS_READ_REE2_HP_SYS_REG_S)
#define HP_PERI1_PMS_READ_REE2_HP_SYS_REG_V  0x00000001U
#define HP_PERI1_PMS_READ_REE2_HP_SYS_REG_S  3
/** HP_PERI1_PMS_WRITE_TEE_HP_SYS_REG : R/W; bitpos: [4]; default: 1;
 *  Configures hp_sys_reg registers write permission in tee mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI1_PMS_WRITE_TEE_HP_SYS_REG    (BIT(4))
#define HP_PERI1_PMS_WRITE_TEE_HP_SYS_REG_M  (HP_PERI1_PMS_WRITE_TEE_HP_SYS_REG_V << HP_PERI1_PMS_WRITE_TEE_HP_SYS_REG_S)
#define HP_PERI1_PMS_WRITE_TEE_HP_SYS_REG_V  0x00000001U
#define HP_PERI1_PMS_WRITE_TEE_HP_SYS_REG_S  4
/** HP_PERI1_PMS_WRITE_REE0_HP_SYS_REG : R/W; bitpos: [5]; default: 0;
 *  Configures hp_sys_reg registers write permission in ree0 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI1_PMS_WRITE_REE0_HP_SYS_REG    (BIT(5))
#define HP_PERI1_PMS_WRITE_REE0_HP_SYS_REG_M  (HP_PERI1_PMS_WRITE_REE0_HP_SYS_REG_V << HP_PERI1_PMS_WRITE_REE0_HP_SYS_REG_S)
#define HP_PERI1_PMS_WRITE_REE0_HP_SYS_REG_V  0x00000001U
#define HP_PERI1_PMS_WRITE_REE0_HP_SYS_REG_S  5
/** HP_PERI1_PMS_WRITE_REE1_HP_SYS_REG : R/W; bitpos: [6]; default: 0;
 *  Configures hp_sys_reg registers write permission in ree1 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI1_PMS_WRITE_REE1_HP_SYS_REG    (BIT(6))
#define HP_PERI1_PMS_WRITE_REE1_HP_SYS_REG_M  (HP_PERI1_PMS_WRITE_REE1_HP_SYS_REG_V << HP_PERI1_PMS_WRITE_REE1_HP_SYS_REG_S)
#define HP_PERI1_PMS_WRITE_REE1_HP_SYS_REG_V  0x00000001U
#define HP_PERI1_PMS_WRITE_REE1_HP_SYS_REG_S  6
/** HP_PERI1_PMS_WRITE_REE2_HP_SYS_REG : R/W; bitpos: [7]; default: 0;
 *  Configures hp_sys_reg registers write permission in ree2 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI1_PMS_WRITE_REE2_HP_SYS_REG    (BIT(7))
#define HP_PERI1_PMS_WRITE_REE2_HP_SYS_REG_M  (HP_PERI1_PMS_WRITE_REE2_HP_SYS_REG_V << HP_PERI1_PMS_WRITE_REE2_HP_SYS_REG_S)
#define HP_PERI1_PMS_WRITE_REE2_HP_SYS_REG_V  0x00000001U
#define HP_PERI1_PMS_WRITE_REE2_HP_SYS_REG_S  7
/** HP_PERI1_PMS_HP_SYS_REG_LOCK : R/W; bitpos: [8]; default: 0;
 *  Set 1 to lock hp_sys_reg peri_apm configuration
 */
#define HP_PERI1_PMS_HP_SYS_REG_LOCK    (BIT(8))
#define HP_PERI1_PMS_HP_SYS_REG_LOCK_M  (HP_PERI1_PMS_HP_SYS_REG_LOCK_V << HP_PERI1_PMS_HP_SYS_REG_LOCK_S)
#define HP_PERI1_PMS_HP_SYS_REG_LOCK_V  0x00000001U
#define HP_PERI1_PMS_HP_SYS_REG_LOCK_S  8

/** HP_PERI1_PMS_HP_CLKRST_CTRL_REG register
 *  hp_clkrst read/write control register
 */
#define HP_PERI1_PMS_HP_CLKRST_CTRL_REG (DR_REG_HP_PERI1_PMS_BASE + 0x8c)
/** HP_PERI1_PMS_READ_TEE_HP_CLKRST : R/W; bitpos: [0]; default: 1;
 *  Configures hp_clkrst registers read permission in tee mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI1_PMS_READ_TEE_HP_CLKRST    (BIT(0))
#define HP_PERI1_PMS_READ_TEE_HP_CLKRST_M  (HP_PERI1_PMS_READ_TEE_HP_CLKRST_V << HP_PERI1_PMS_READ_TEE_HP_CLKRST_S)
#define HP_PERI1_PMS_READ_TEE_HP_CLKRST_V  0x00000001U
#define HP_PERI1_PMS_READ_TEE_HP_CLKRST_S  0
/** HP_PERI1_PMS_READ_REE0_HP_CLKRST : R/W; bitpos: [1]; default: 0;
 *  Configures hp_clkrst registers read permission in ree0 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI1_PMS_READ_REE0_HP_CLKRST    (BIT(1))
#define HP_PERI1_PMS_READ_REE0_HP_CLKRST_M  (HP_PERI1_PMS_READ_REE0_HP_CLKRST_V << HP_PERI1_PMS_READ_REE0_HP_CLKRST_S)
#define HP_PERI1_PMS_READ_REE0_HP_CLKRST_V  0x00000001U
#define HP_PERI1_PMS_READ_REE0_HP_CLKRST_S  1
/** HP_PERI1_PMS_READ_REE1_HP_CLKRST : R/W; bitpos: [2]; default: 0;
 *  Configures hp_clkrst registers read permission in ree1 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI1_PMS_READ_REE1_HP_CLKRST    (BIT(2))
#define HP_PERI1_PMS_READ_REE1_HP_CLKRST_M  (HP_PERI1_PMS_READ_REE1_HP_CLKRST_V << HP_PERI1_PMS_READ_REE1_HP_CLKRST_S)
#define HP_PERI1_PMS_READ_REE1_HP_CLKRST_V  0x00000001U
#define HP_PERI1_PMS_READ_REE1_HP_CLKRST_S  2
/** HP_PERI1_PMS_READ_REE2_HP_CLKRST : R/W; bitpos: [3]; default: 0;
 *  Configures hp_clkrst registers read permission in ree2 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI1_PMS_READ_REE2_HP_CLKRST    (BIT(3))
#define HP_PERI1_PMS_READ_REE2_HP_CLKRST_M  (HP_PERI1_PMS_READ_REE2_HP_CLKRST_V << HP_PERI1_PMS_READ_REE2_HP_CLKRST_S)
#define HP_PERI1_PMS_READ_REE2_HP_CLKRST_V  0x00000001U
#define HP_PERI1_PMS_READ_REE2_HP_CLKRST_S  3
/** HP_PERI1_PMS_WRITE_TEE_HP_CLKRST : R/W; bitpos: [4]; default: 1;
 *  Configures hp_clkrst registers write permission in tee mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI1_PMS_WRITE_TEE_HP_CLKRST    (BIT(4))
#define HP_PERI1_PMS_WRITE_TEE_HP_CLKRST_M  (HP_PERI1_PMS_WRITE_TEE_HP_CLKRST_V << HP_PERI1_PMS_WRITE_TEE_HP_CLKRST_S)
#define HP_PERI1_PMS_WRITE_TEE_HP_CLKRST_V  0x00000001U
#define HP_PERI1_PMS_WRITE_TEE_HP_CLKRST_S  4
/** HP_PERI1_PMS_WRITE_REE0_HP_CLKRST : R/W; bitpos: [5]; default: 0;
 *  Configures hp_clkrst registers write permission in ree0 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI1_PMS_WRITE_REE0_HP_CLKRST    (BIT(5))
#define HP_PERI1_PMS_WRITE_REE0_HP_CLKRST_M  (HP_PERI1_PMS_WRITE_REE0_HP_CLKRST_V << HP_PERI1_PMS_WRITE_REE0_HP_CLKRST_S)
#define HP_PERI1_PMS_WRITE_REE0_HP_CLKRST_V  0x00000001U
#define HP_PERI1_PMS_WRITE_REE0_HP_CLKRST_S  5
/** HP_PERI1_PMS_WRITE_REE1_HP_CLKRST : R/W; bitpos: [6]; default: 0;
 *  Configures hp_clkrst registers write permission in ree1 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI1_PMS_WRITE_REE1_HP_CLKRST    (BIT(6))
#define HP_PERI1_PMS_WRITE_REE1_HP_CLKRST_M  (HP_PERI1_PMS_WRITE_REE1_HP_CLKRST_V << HP_PERI1_PMS_WRITE_REE1_HP_CLKRST_S)
#define HP_PERI1_PMS_WRITE_REE1_HP_CLKRST_V  0x00000001U
#define HP_PERI1_PMS_WRITE_REE1_HP_CLKRST_S  6
/** HP_PERI1_PMS_WRITE_REE2_HP_CLKRST : R/W; bitpos: [7]; default: 0;
 *  Configures hp_clkrst registers write permission in ree2 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI1_PMS_WRITE_REE2_HP_CLKRST    (BIT(7))
#define HP_PERI1_PMS_WRITE_REE2_HP_CLKRST_M  (HP_PERI1_PMS_WRITE_REE2_HP_CLKRST_V << HP_PERI1_PMS_WRITE_REE2_HP_CLKRST_S)
#define HP_PERI1_PMS_WRITE_REE2_HP_CLKRST_V  0x00000001U
#define HP_PERI1_PMS_WRITE_REE2_HP_CLKRST_S  7
/** HP_PERI1_PMS_HP_CLKRST_LOCK : R/W; bitpos: [8]; default: 0;
 *  Set 1 to lock hp_clkrst peri_apm configuration
 */
#define HP_PERI1_PMS_HP_CLKRST_LOCK    (BIT(8))
#define HP_PERI1_PMS_HP_CLKRST_LOCK_M  (HP_PERI1_PMS_HP_CLKRST_LOCK_V << HP_PERI1_PMS_HP_CLKRST_LOCK_S)
#define HP_PERI1_PMS_HP_CLKRST_LOCK_V  0x00000001U
#define HP_PERI1_PMS_HP_CLKRST_LOCK_S  8

/** HP_PERI1_PMS_CNNT_PAD_CTRL_CTRL_REG register
 *  cnnt_pad_ctrl read/write control register
 */
#define HP_PERI1_PMS_CNNT_PAD_CTRL_CTRL_REG (DR_REG_HP_PERI1_PMS_BASE + 0x90)
/** HP_PERI1_PMS_READ_TEE_CNNT_PAD_CTRL : R/W; bitpos: [0]; default: 1;
 *  Configures cnnt_pad_ctrl registers read permission in tee mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI1_PMS_READ_TEE_CNNT_PAD_CTRL    (BIT(0))
#define HP_PERI1_PMS_READ_TEE_CNNT_PAD_CTRL_M  (HP_PERI1_PMS_READ_TEE_CNNT_PAD_CTRL_V << HP_PERI1_PMS_READ_TEE_CNNT_PAD_CTRL_S)
#define HP_PERI1_PMS_READ_TEE_CNNT_PAD_CTRL_V  0x00000001U
#define HP_PERI1_PMS_READ_TEE_CNNT_PAD_CTRL_S  0
/** HP_PERI1_PMS_READ_REE0_CNNT_PAD_CTRL : R/W; bitpos: [1]; default: 0;
 *  Configures cnnt_pad_ctrl registers read permission in ree0 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI1_PMS_READ_REE0_CNNT_PAD_CTRL    (BIT(1))
#define HP_PERI1_PMS_READ_REE0_CNNT_PAD_CTRL_M  (HP_PERI1_PMS_READ_REE0_CNNT_PAD_CTRL_V << HP_PERI1_PMS_READ_REE0_CNNT_PAD_CTRL_S)
#define HP_PERI1_PMS_READ_REE0_CNNT_PAD_CTRL_V  0x00000001U
#define HP_PERI1_PMS_READ_REE0_CNNT_PAD_CTRL_S  1
/** HP_PERI1_PMS_READ_REE1_CNNT_PAD_CTRL : R/W; bitpos: [2]; default: 0;
 *  Configures cnnt_pad_ctrl registers read permission in ree1 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI1_PMS_READ_REE1_CNNT_PAD_CTRL    (BIT(2))
#define HP_PERI1_PMS_READ_REE1_CNNT_PAD_CTRL_M  (HP_PERI1_PMS_READ_REE1_CNNT_PAD_CTRL_V << HP_PERI1_PMS_READ_REE1_CNNT_PAD_CTRL_S)
#define HP_PERI1_PMS_READ_REE1_CNNT_PAD_CTRL_V  0x00000001U
#define HP_PERI1_PMS_READ_REE1_CNNT_PAD_CTRL_S  2
/** HP_PERI1_PMS_READ_REE2_CNNT_PAD_CTRL : R/W; bitpos: [3]; default: 0;
 *  Configures cnnt_pad_ctrl registers read permission in ree2 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI1_PMS_READ_REE2_CNNT_PAD_CTRL    (BIT(3))
#define HP_PERI1_PMS_READ_REE2_CNNT_PAD_CTRL_M  (HP_PERI1_PMS_READ_REE2_CNNT_PAD_CTRL_V << HP_PERI1_PMS_READ_REE2_CNNT_PAD_CTRL_S)
#define HP_PERI1_PMS_READ_REE2_CNNT_PAD_CTRL_V  0x00000001U
#define HP_PERI1_PMS_READ_REE2_CNNT_PAD_CTRL_S  3
/** HP_PERI1_PMS_WRITE_TEE_CNNT_PAD_CTRL : R/W; bitpos: [4]; default: 1;
 *  Configures cnnt_pad_ctrl registers write permission in tee mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI1_PMS_WRITE_TEE_CNNT_PAD_CTRL    (BIT(4))
#define HP_PERI1_PMS_WRITE_TEE_CNNT_PAD_CTRL_M  (HP_PERI1_PMS_WRITE_TEE_CNNT_PAD_CTRL_V << HP_PERI1_PMS_WRITE_TEE_CNNT_PAD_CTRL_S)
#define HP_PERI1_PMS_WRITE_TEE_CNNT_PAD_CTRL_V  0x00000001U
#define HP_PERI1_PMS_WRITE_TEE_CNNT_PAD_CTRL_S  4
/** HP_PERI1_PMS_WRITE_REE0_CNNT_PAD_CTRL : R/W; bitpos: [5]; default: 0;
 *  Configures cnnt_pad_ctrl registers write permission in ree0 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI1_PMS_WRITE_REE0_CNNT_PAD_CTRL    (BIT(5))
#define HP_PERI1_PMS_WRITE_REE0_CNNT_PAD_CTRL_M  (HP_PERI1_PMS_WRITE_REE0_CNNT_PAD_CTRL_V << HP_PERI1_PMS_WRITE_REE0_CNNT_PAD_CTRL_S)
#define HP_PERI1_PMS_WRITE_REE0_CNNT_PAD_CTRL_V  0x00000001U
#define HP_PERI1_PMS_WRITE_REE0_CNNT_PAD_CTRL_S  5
/** HP_PERI1_PMS_WRITE_REE1_CNNT_PAD_CTRL : R/W; bitpos: [6]; default: 0;
 *  Configures cnnt_pad_ctrl registers write permission in ree1 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI1_PMS_WRITE_REE1_CNNT_PAD_CTRL    (BIT(6))
#define HP_PERI1_PMS_WRITE_REE1_CNNT_PAD_CTRL_M  (HP_PERI1_PMS_WRITE_REE1_CNNT_PAD_CTRL_V << HP_PERI1_PMS_WRITE_REE1_CNNT_PAD_CTRL_S)
#define HP_PERI1_PMS_WRITE_REE1_CNNT_PAD_CTRL_V  0x00000001U
#define HP_PERI1_PMS_WRITE_REE1_CNNT_PAD_CTRL_S  6
/** HP_PERI1_PMS_WRITE_REE2_CNNT_PAD_CTRL : R/W; bitpos: [7]; default: 0;
 *  Configures cnnt_pad_ctrl registers write permission in ree2 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI1_PMS_WRITE_REE2_CNNT_PAD_CTRL    (BIT(7))
#define HP_PERI1_PMS_WRITE_REE2_CNNT_PAD_CTRL_M  (HP_PERI1_PMS_WRITE_REE2_CNNT_PAD_CTRL_V << HP_PERI1_PMS_WRITE_REE2_CNNT_PAD_CTRL_S)
#define HP_PERI1_PMS_WRITE_REE2_CNNT_PAD_CTRL_V  0x00000001U
#define HP_PERI1_PMS_WRITE_REE2_CNNT_PAD_CTRL_S  7
/** HP_PERI1_PMS_CNNT_PAD_CTRL_LOCK : R/W; bitpos: [8]; default: 0;
 *  Set 1 to lock cnnt_pad_ctrl peri_apm configuration
 */
#define HP_PERI1_PMS_CNNT_PAD_CTRL_LOCK    (BIT(8))
#define HP_PERI1_PMS_CNNT_PAD_CTRL_LOCK_M  (HP_PERI1_PMS_CNNT_PAD_CTRL_LOCK_V << HP_PERI1_PMS_CNNT_PAD_CTRL_LOCK_S)
#define HP_PERI1_PMS_CNNT_PAD_CTRL_LOCK_V  0x00000001U
#define HP_PERI1_PMS_CNNT_PAD_CTRL_LOCK_S  8

/** HP_PERI1_PMS_HP_ALIVE_SYS_REG_CTRL_REG register
 *  hp_alive_sys_reg read/write control register
 */
#define HP_PERI1_PMS_HP_ALIVE_SYS_REG_CTRL_REG (DR_REG_HP_PERI1_PMS_BASE + 0x94)
/** HP_PERI1_PMS_READ_TEE_HP_ALIVE_SYS_REG : R/W; bitpos: [0]; default: 1;
 *  Configures hp_alive_sys_reg registers read permission in tee mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI1_PMS_READ_TEE_HP_ALIVE_SYS_REG    (BIT(0))
#define HP_PERI1_PMS_READ_TEE_HP_ALIVE_SYS_REG_M  (HP_PERI1_PMS_READ_TEE_HP_ALIVE_SYS_REG_V << HP_PERI1_PMS_READ_TEE_HP_ALIVE_SYS_REG_S)
#define HP_PERI1_PMS_READ_TEE_HP_ALIVE_SYS_REG_V  0x00000001U
#define HP_PERI1_PMS_READ_TEE_HP_ALIVE_SYS_REG_S  0
/** HP_PERI1_PMS_READ_REE0_HP_ALIVE_SYS_REG : R/W; bitpos: [1]; default: 0;
 *  Configures hp_alive_sys_reg registers read permission in ree0 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI1_PMS_READ_REE0_HP_ALIVE_SYS_REG    (BIT(1))
#define HP_PERI1_PMS_READ_REE0_HP_ALIVE_SYS_REG_M  (HP_PERI1_PMS_READ_REE0_HP_ALIVE_SYS_REG_V << HP_PERI1_PMS_READ_REE0_HP_ALIVE_SYS_REG_S)
#define HP_PERI1_PMS_READ_REE0_HP_ALIVE_SYS_REG_V  0x00000001U
#define HP_PERI1_PMS_READ_REE0_HP_ALIVE_SYS_REG_S  1
/** HP_PERI1_PMS_READ_REE1_HP_ALIVE_SYS_REG : R/W; bitpos: [2]; default: 0;
 *  Configures hp_alive_sys_reg registers read permission in ree1 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI1_PMS_READ_REE1_HP_ALIVE_SYS_REG    (BIT(2))
#define HP_PERI1_PMS_READ_REE1_HP_ALIVE_SYS_REG_M  (HP_PERI1_PMS_READ_REE1_HP_ALIVE_SYS_REG_V << HP_PERI1_PMS_READ_REE1_HP_ALIVE_SYS_REG_S)
#define HP_PERI1_PMS_READ_REE1_HP_ALIVE_SYS_REG_V  0x00000001U
#define HP_PERI1_PMS_READ_REE1_HP_ALIVE_SYS_REG_S  2
/** HP_PERI1_PMS_READ_REE2_HP_ALIVE_SYS_REG : R/W; bitpos: [3]; default: 0;
 *  Configures hp_alive_sys_reg registers read permission in ree2 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI1_PMS_READ_REE2_HP_ALIVE_SYS_REG    (BIT(3))
#define HP_PERI1_PMS_READ_REE2_HP_ALIVE_SYS_REG_M  (HP_PERI1_PMS_READ_REE2_HP_ALIVE_SYS_REG_V << HP_PERI1_PMS_READ_REE2_HP_ALIVE_SYS_REG_S)
#define HP_PERI1_PMS_READ_REE2_HP_ALIVE_SYS_REG_V  0x00000001U
#define HP_PERI1_PMS_READ_REE2_HP_ALIVE_SYS_REG_S  3
/** HP_PERI1_PMS_WRITE_TEE_HP_ALIVE_SYS_REG : R/W; bitpos: [4]; default: 1;
 *  Configures hp_alive_sys_reg registers write permission in tee mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI1_PMS_WRITE_TEE_HP_ALIVE_SYS_REG    (BIT(4))
#define HP_PERI1_PMS_WRITE_TEE_HP_ALIVE_SYS_REG_M  (HP_PERI1_PMS_WRITE_TEE_HP_ALIVE_SYS_REG_V << HP_PERI1_PMS_WRITE_TEE_HP_ALIVE_SYS_REG_S)
#define HP_PERI1_PMS_WRITE_TEE_HP_ALIVE_SYS_REG_V  0x00000001U
#define HP_PERI1_PMS_WRITE_TEE_HP_ALIVE_SYS_REG_S  4
/** HP_PERI1_PMS_WRITE_REE0_HP_ALIVE_SYS_REG : R/W; bitpos: [5]; default: 0;
 *  Configures hp_alive_sys_reg registers write permission in ree0 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI1_PMS_WRITE_REE0_HP_ALIVE_SYS_REG    (BIT(5))
#define HP_PERI1_PMS_WRITE_REE0_HP_ALIVE_SYS_REG_M  (HP_PERI1_PMS_WRITE_REE0_HP_ALIVE_SYS_REG_V << HP_PERI1_PMS_WRITE_REE0_HP_ALIVE_SYS_REG_S)
#define HP_PERI1_PMS_WRITE_REE0_HP_ALIVE_SYS_REG_V  0x00000001U
#define HP_PERI1_PMS_WRITE_REE0_HP_ALIVE_SYS_REG_S  5
/** HP_PERI1_PMS_WRITE_REE1_HP_ALIVE_SYS_REG : R/W; bitpos: [6]; default: 0;
 *  Configures hp_alive_sys_reg registers write permission in ree1 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI1_PMS_WRITE_REE1_HP_ALIVE_SYS_REG    (BIT(6))
#define HP_PERI1_PMS_WRITE_REE1_HP_ALIVE_SYS_REG_M  (HP_PERI1_PMS_WRITE_REE1_HP_ALIVE_SYS_REG_V << HP_PERI1_PMS_WRITE_REE1_HP_ALIVE_SYS_REG_S)
#define HP_PERI1_PMS_WRITE_REE1_HP_ALIVE_SYS_REG_V  0x00000001U
#define HP_PERI1_PMS_WRITE_REE1_HP_ALIVE_SYS_REG_S  6
/** HP_PERI1_PMS_WRITE_REE2_HP_ALIVE_SYS_REG : R/W; bitpos: [7]; default: 0;
 *  Configures hp_alive_sys_reg registers write permission in ree2 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI1_PMS_WRITE_REE2_HP_ALIVE_SYS_REG    (BIT(7))
#define HP_PERI1_PMS_WRITE_REE2_HP_ALIVE_SYS_REG_M  (HP_PERI1_PMS_WRITE_REE2_HP_ALIVE_SYS_REG_V << HP_PERI1_PMS_WRITE_REE2_HP_ALIVE_SYS_REG_S)
#define HP_PERI1_PMS_WRITE_REE2_HP_ALIVE_SYS_REG_V  0x00000001U
#define HP_PERI1_PMS_WRITE_REE2_HP_ALIVE_SYS_REG_S  7
/** HP_PERI1_PMS_HP_ALIVE_SYS_REG_LOCK : R/W; bitpos: [8]; default: 0;
 *  Set 1 to lock hp_alive_sys_reg peri_apm configuration
 */
#define HP_PERI1_PMS_HP_ALIVE_SYS_REG_LOCK    (BIT(8))
#define HP_PERI1_PMS_HP_ALIVE_SYS_REG_LOCK_M  (HP_PERI1_PMS_HP_ALIVE_SYS_REG_LOCK_V << HP_PERI1_PMS_HP_ALIVE_SYS_REG_LOCK_S)
#define HP_PERI1_PMS_HP_ALIVE_SYS_REG_LOCK_V  0x00000001U
#define HP_PERI1_PMS_HP_ALIVE_SYS_REG_LOCK_S  8

/** HP_PERI1_PMS_HP_PERI1_PMS_CTRL_REG register
 *  hp_peri1_pms read/write control register
 */
#define HP_PERI1_PMS_HP_PERI1_PMS_CTRL_REG (DR_REG_HP_PERI1_PMS_BASE + 0x98)
/** HP_PERI1_PMS_READ_TEE_HP_PERI1_PMS : R/W; bitpos: [0]; default: 1;
 *  Configures hp_peri1_pms registers read permission in tee mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI1_PMS_READ_TEE_HP_PERI1_PMS    (BIT(0))
#define HP_PERI1_PMS_READ_TEE_HP_PERI1_PMS_M  (HP_PERI1_PMS_READ_TEE_HP_PERI1_PMS_V << HP_PERI1_PMS_READ_TEE_HP_PERI1_PMS_S)
#define HP_PERI1_PMS_READ_TEE_HP_PERI1_PMS_V  0x00000001U
#define HP_PERI1_PMS_READ_TEE_HP_PERI1_PMS_S  0
/** HP_PERI1_PMS_READ_REE0_HP_PERI1_PMS : HRO; bitpos: [1]; default: 0;
 *  Configures hp_peri1_pms registers read permission in ree0 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI1_PMS_READ_REE0_HP_PERI1_PMS    (BIT(1))
#define HP_PERI1_PMS_READ_REE0_HP_PERI1_PMS_M  (HP_PERI1_PMS_READ_REE0_HP_PERI1_PMS_V << HP_PERI1_PMS_READ_REE0_HP_PERI1_PMS_S)
#define HP_PERI1_PMS_READ_REE0_HP_PERI1_PMS_V  0x00000001U
#define HP_PERI1_PMS_READ_REE0_HP_PERI1_PMS_S  1
/** HP_PERI1_PMS_READ_REE1_HP_PERI1_PMS : HRO; bitpos: [2]; default: 0;
 *  Configures hp_peri1_pms registers read permission in ree1 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI1_PMS_READ_REE1_HP_PERI1_PMS    (BIT(2))
#define HP_PERI1_PMS_READ_REE1_HP_PERI1_PMS_M  (HP_PERI1_PMS_READ_REE1_HP_PERI1_PMS_V << HP_PERI1_PMS_READ_REE1_HP_PERI1_PMS_S)
#define HP_PERI1_PMS_READ_REE1_HP_PERI1_PMS_V  0x00000001U
#define HP_PERI1_PMS_READ_REE1_HP_PERI1_PMS_S  2
/** HP_PERI1_PMS_READ_REE2_HP_PERI1_PMS : HRO; bitpos: [3]; default: 0;
 *  Configures hp_peri1_pms registers read permission in ree2 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI1_PMS_READ_REE2_HP_PERI1_PMS    (BIT(3))
#define HP_PERI1_PMS_READ_REE2_HP_PERI1_PMS_M  (HP_PERI1_PMS_READ_REE2_HP_PERI1_PMS_V << HP_PERI1_PMS_READ_REE2_HP_PERI1_PMS_S)
#define HP_PERI1_PMS_READ_REE2_HP_PERI1_PMS_V  0x00000001U
#define HP_PERI1_PMS_READ_REE2_HP_PERI1_PMS_S  3
/** HP_PERI1_PMS_WRITE_TEE_HP_PERI1_PMS : R/W; bitpos: [4]; default: 1;
 *  Configures hp_peri1_pms registers write permission in tee mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI1_PMS_WRITE_TEE_HP_PERI1_PMS    (BIT(4))
#define HP_PERI1_PMS_WRITE_TEE_HP_PERI1_PMS_M  (HP_PERI1_PMS_WRITE_TEE_HP_PERI1_PMS_V << HP_PERI1_PMS_WRITE_TEE_HP_PERI1_PMS_S)
#define HP_PERI1_PMS_WRITE_TEE_HP_PERI1_PMS_V  0x00000001U
#define HP_PERI1_PMS_WRITE_TEE_HP_PERI1_PMS_S  4
/** HP_PERI1_PMS_WRITE_REE0_HP_PERI1_PMS : HRO; bitpos: [5]; default: 0;
 *  Configures hp_peri1_pms registers write permission in ree0 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI1_PMS_WRITE_REE0_HP_PERI1_PMS    (BIT(5))
#define HP_PERI1_PMS_WRITE_REE0_HP_PERI1_PMS_M  (HP_PERI1_PMS_WRITE_REE0_HP_PERI1_PMS_V << HP_PERI1_PMS_WRITE_REE0_HP_PERI1_PMS_S)
#define HP_PERI1_PMS_WRITE_REE0_HP_PERI1_PMS_V  0x00000001U
#define HP_PERI1_PMS_WRITE_REE0_HP_PERI1_PMS_S  5
/** HP_PERI1_PMS_WRITE_REE1_HP_PERI1_PMS : HRO; bitpos: [6]; default: 0;
 *  Configures hp_peri1_pms registers write permission in ree1 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI1_PMS_WRITE_REE1_HP_PERI1_PMS    (BIT(6))
#define HP_PERI1_PMS_WRITE_REE1_HP_PERI1_PMS_M  (HP_PERI1_PMS_WRITE_REE1_HP_PERI1_PMS_V << HP_PERI1_PMS_WRITE_REE1_HP_PERI1_PMS_S)
#define HP_PERI1_PMS_WRITE_REE1_HP_PERI1_PMS_V  0x00000001U
#define HP_PERI1_PMS_WRITE_REE1_HP_PERI1_PMS_S  6
/** HP_PERI1_PMS_WRITE_REE2_HP_PERI1_PMS : HRO; bitpos: [7]; default: 0;
 *  Configures hp_peri1_pms registers write permission in ree2 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI1_PMS_WRITE_REE2_HP_PERI1_PMS    (BIT(7))
#define HP_PERI1_PMS_WRITE_REE2_HP_PERI1_PMS_M  (HP_PERI1_PMS_WRITE_REE2_HP_PERI1_PMS_V << HP_PERI1_PMS_WRITE_REE2_HP_PERI1_PMS_S)
#define HP_PERI1_PMS_WRITE_REE2_HP_PERI1_PMS_V  0x00000001U
#define HP_PERI1_PMS_WRITE_REE2_HP_PERI1_PMS_S  7
/** HP_PERI1_PMS_HP_PERI1_PMS_LOCK : R/W; bitpos: [8]; default: 0;
 *  Set 1 to lock hp_peri1_pms peri_apm configuration
 */
#define HP_PERI1_PMS_HP_PERI1_PMS_LOCK    (BIT(8))
#define HP_PERI1_PMS_HP_PERI1_PMS_LOCK_M  (HP_PERI1_PMS_HP_PERI1_PMS_LOCK_V << HP_PERI1_PMS_HP_PERI1_PMS_LOCK_S)
#define HP_PERI1_PMS_HP_PERI1_PMS_LOCK_V  0x00000001U
#define HP_PERI1_PMS_HP_PERI1_PMS_LOCK_S  8

/** HP_PERI1_PMS_HP_PERI1_0_REG register
 *  HP_PERI1 PMS configuration & info register
 */
#define HP_PERI1_PMS_HP_PERI1_0_REG (DR_REG_HP_PERI1_PMS_BASE + 0x200)
/** HP_PERI1_PMS_HP_PERI1_PMS_EXCEPTION_CLR : WT; bitpos: [0]; default: 0;
 *  Configures whether or not to clear hp_peri1 peri_pms_record_reg.
 *  0: No clear
 *  1: Clear peri_pms_record_reg
 */
#define HP_PERI1_PMS_HP_PERI1_PMS_EXCEPTION_CLR    (BIT(0))
#define HP_PERI1_PMS_HP_PERI1_PMS_EXCEPTION_CLR_M  (HP_PERI1_PMS_HP_PERI1_PMS_EXCEPTION_CLR_V << HP_PERI1_PMS_HP_PERI1_PMS_EXCEPTION_CLR_S)
#define HP_PERI1_PMS_HP_PERI1_PMS_EXCEPTION_CLR_V  0x00000001U
#define HP_PERI1_PMS_HP_PERI1_PMS_EXCEPTION_CLR_S  0
/** HP_PERI1_PMS_HP_PERI1_PMS_EXCEPTION_DET : RO; bitpos: [1]; default: 0;
 *  Represents whether the hp_peri1 pms has been triggered.
 *  0: No triggered
 *  1: Has been triggered
 */
#define HP_PERI1_PMS_HP_PERI1_PMS_EXCEPTION_DET    (BIT(1))
#define HP_PERI1_PMS_HP_PERI1_PMS_EXCEPTION_DET_M  (HP_PERI1_PMS_HP_PERI1_PMS_EXCEPTION_DET_V << HP_PERI1_PMS_HP_PERI1_PMS_EXCEPTION_DET_S)
#define HP_PERI1_PMS_HP_PERI1_PMS_EXCEPTION_DET_V  0x00000001U
#define HP_PERI1_PMS_HP_PERI1_PMS_EXCEPTION_DET_S  1
/** HP_PERI1_PMS_HP_PERI1_PMS_EXCEPTION_ID : RO; bitpos: [7:2]; default: 0;
 *  Represents the master id when hp_peri1 pms has been triggered.
 */
#define HP_PERI1_PMS_HP_PERI1_PMS_EXCEPTION_ID    0x0000003FU
#define HP_PERI1_PMS_HP_PERI1_PMS_EXCEPTION_ID_M  (HP_PERI1_PMS_HP_PERI1_PMS_EXCEPTION_ID_V << HP_PERI1_PMS_HP_PERI1_PMS_EXCEPTION_ID_S)
#define HP_PERI1_PMS_HP_PERI1_PMS_EXCEPTION_ID_V  0x0000003FU
#define HP_PERI1_PMS_HP_PERI1_PMS_EXCEPTION_ID_S  2
/** HP_PERI1_PMS_HP_PERI1_PMS_EXCEPTION_MODE : RO; bitpos: [9:8]; default: 0;
 *  Represents the security mode when hp_peri1 pms has been triggered.
 */
#define HP_PERI1_PMS_HP_PERI1_PMS_EXCEPTION_MODE    0x00000003U
#define HP_PERI1_PMS_HP_PERI1_PMS_EXCEPTION_MODE_M  (HP_PERI1_PMS_HP_PERI1_PMS_EXCEPTION_MODE_V << HP_PERI1_PMS_HP_PERI1_PMS_EXCEPTION_MODE_S)
#define HP_PERI1_PMS_HP_PERI1_PMS_EXCEPTION_MODE_V  0x00000003U
#define HP_PERI1_PMS_HP_PERI1_PMS_EXCEPTION_MODE_S  8

/** HP_PERI1_PMS_HP_PERI1_1_REG register
 *  HP_PERI1 PMS exception addr record register
 */
#define HP_PERI1_PMS_HP_PERI1_1_REG (DR_REG_HP_PERI1_PMS_BASE + 0x204)
/** HP_PERI1_PMS_HP_PERI1_PMS_EXCEPTION_ADDR : RO; bitpos: [27:0]; default: 0;
 *  Represents the access address (bit27~bit0) when hp_peri1 pms has been triggered.
 */
#define HP_PERI1_PMS_HP_PERI1_PMS_EXCEPTION_ADDR    0x0FFFFFFFU
#define HP_PERI1_PMS_HP_PERI1_PMS_EXCEPTION_ADDR_M  (HP_PERI1_PMS_HP_PERI1_PMS_EXCEPTION_ADDR_V << HP_PERI1_PMS_HP_PERI1_PMS_EXCEPTION_ADDR_S)
#define HP_PERI1_PMS_HP_PERI1_PMS_EXCEPTION_ADDR_V  0x0FFFFFFFU
#define HP_PERI1_PMS_HP_PERI1_PMS_EXCEPTION_ADDR_S  0
/** HP_PERI1_PMS_HP_PERI1_PMS_EXCEPTION_ADDR_CONST : HRO; bitpos: [31:28]; default: 2;
 *  Represents the access address (bit31~bit28) when hp_peri1 pms has been triggered.
 */
#define HP_PERI1_PMS_HP_PERI1_PMS_EXCEPTION_ADDR_CONST    0x0000000FU
#define HP_PERI1_PMS_HP_PERI1_PMS_EXCEPTION_ADDR_CONST_M  (HP_PERI1_PMS_HP_PERI1_PMS_EXCEPTION_ADDR_CONST_V << HP_PERI1_PMS_HP_PERI1_PMS_EXCEPTION_ADDR_CONST_S)
#define HP_PERI1_PMS_HP_PERI1_PMS_EXCEPTION_ADDR_CONST_V  0x0000000FU
#define HP_PERI1_PMS_HP_PERI1_PMS_EXCEPTION_ADDR_CONST_S  28

/** HP_PERI1_PMS_INT_EN_REG register
 *  APM interrupt enable register
 */
#define HP_PERI1_PMS_INT_EN_REG (DR_REG_HP_PERI1_PMS_BASE + 0x300)
/** HP_PERI1_PMS_HP_PERI1_PMS_INT_EN : R/W; bitpos: [0]; default: 0;
 *  Configures to enable hp_peri1 pms interrupt.
 *  0: disable
 *  1: enable
 */
#define HP_PERI1_PMS_HP_PERI1_PMS_INT_EN    (BIT(0))
#define HP_PERI1_PMS_HP_PERI1_PMS_INT_EN_M  (HP_PERI1_PMS_HP_PERI1_PMS_INT_EN_V << HP_PERI1_PMS_HP_PERI1_PMS_INT_EN_S)
#define HP_PERI1_PMS_HP_PERI1_PMS_INT_EN_V  0x00000001U
#define HP_PERI1_PMS_HP_PERI1_PMS_INT_EN_S  0

/** HP_PERI1_PMS_BUS_ERR_CONF_REG register
 *  Clock gating register
 */
#define HP_PERI1_PMS_BUS_ERR_CONF_REG (DR_REG_HP_PERI1_PMS_BASE + 0xff0)
/** HP_PERI1_PMS_BUS_ERR_RESP_EN : R/W; bitpos: [0]; default: 0;
 *  Configures whether return error response to cpu when access blocked
 *  0: disable error response
 *  1: enable error response
 */
#define HP_PERI1_PMS_BUS_ERR_RESP_EN    (BIT(0))
#define HP_PERI1_PMS_BUS_ERR_RESP_EN_M  (HP_PERI1_PMS_BUS_ERR_RESP_EN_V << HP_PERI1_PMS_BUS_ERR_RESP_EN_S)
#define HP_PERI1_PMS_BUS_ERR_RESP_EN_V  0x00000001U
#define HP_PERI1_PMS_BUS_ERR_RESP_EN_S  0

/** HP_PERI1_PMS_CLOCK_GATE_REG register
 *  Clock gating register
 */
#define HP_PERI1_PMS_CLOCK_GATE_REG (DR_REG_HP_PERI1_PMS_BASE + 0xff8)
/** HP_PERI1_PMS_CLK_EN : R/W; bitpos: [0]; default: 1;
 *  Configures whether to keep the clock always on.
 *  0: enable automatic clock gating
 *  1: keep the clock always on
 */
#define HP_PERI1_PMS_CLK_EN    (BIT(0))
#define HP_PERI1_PMS_CLK_EN_M  (HP_PERI1_PMS_CLK_EN_V << HP_PERI1_PMS_CLK_EN_S)
#define HP_PERI1_PMS_CLK_EN_V  0x00000001U
#define HP_PERI1_PMS_CLK_EN_S  0

/** HP_PERI1_PMS_DATE_REG register
 *  Version control register
 */
#define HP_PERI1_PMS_DATE_REG (DR_REG_HP_PERI1_PMS_BASE + 0xffc)
/** HP_PERI1_PMS_DATE : R/W; bitpos: [27:0]; default: 38834720;
 *  Version control register
 */
#define HP_PERI1_PMS_DATE    0x0FFFFFFFU
#define HP_PERI1_PMS_DATE_M  (HP_PERI1_PMS_DATE_V << HP_PERI1_PMS_DATE_S)
#define HP_PERI1_PMS_DATE_V  0x0FFFFFFFU
#define HP_PERI1_PMS_DATE_S  0

#ifdef __cplusplus
}
#endif
