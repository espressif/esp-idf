/**
 * SPDX-FileCopyrightText: 2025-2026 Espressif Systems (Shanghai) CO LTD
 *
 *  SPDX-License-Identifier: Apache-2.0 OR MIT
 */
#pragma once

#include <stdint.h>
#include "soc/soc.h"
#ifdef __cplusplus
extern "C" {
#endif

/** HP_PERI0_PMS_TRACE0_CTRL_REG register
 *  trace0 read/write control register
 */
#define HP_PERI0_PMS_TRACE0_CTRL_REG (DR_REG_HP_PERI0_PMS_BASE + 0x0)
/** HP_PERI0_PMS_READ_TEE_TRACE0 : R/W; bitpos: [0]; default: 1;
 *  Configures trace0 registers read permission in tee mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI0_PMS_READ_TEE_TRACE0    (BIT(0))
#define HP_PERI0_PMS_READ_TEE_TRACE0_M  (HP_PERI0_PMS_READ_TEE_TRACE0_V << HP_PERI0_PMS_READ_TEE_TRACE0_S)
#define HP_PERI0_PMS_READ_TEE_TRACE0_V  0x00000001U
#define HP_PERI0_PMS_READ_TEE_TRACE0_S  0
/** HP_PERI0_PMS_READ_REE0_TRACE0 : R/W; bitpos: [1]; default: 0;
 *  Configures trace0 registers read permission in ree0 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI0_PMS_READ_REE0_TRACE0    (BIT(1))
#define HP_PERI0_PMS_READ_REE0_TRACE0_M  (HP_PERI0_PMS_READ_REE0_TRACE0_V << HP_PERI0_PMS_READ_REE0_TRACE0_S)
#define HP_PERI0_PMS_READ_REE0_TRACE0_V  0x00000001U
#define HP_PERI0_PMS_READ_REE0_TRACE0_S  1
/** HP_PERI0_PMS_READ_REE1_TRACE0 : R/W; bitpos: [2]; default: 0;
 *  Configures trace0 registers read permission in ree1 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI0_PMS_READ_REE1_TRACE0    (BIT(2))
#define HP_PERI0_PMS_READ_REE1_TRACE0_M  (HP_PERI0_PMS_READ_REE1_TRACE0_V << HP_PERI0_PMS_READ_REE1_TRACE0_S)
#define HP_PERI0_PMS_READ_REE1_TRACE0_V  0x00000001U
#define HP_PERI0_PMS_READ_REE1_TRACE0_S  2
/** HP_PERI0_PMS_READ_REE2_TRACE0 : R/W; bitpos: [3]; default: 0;
 *  Configures trace0 registers read permission in ree2 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI0_PMS_READ_REE2_TRACE0    (BIT(3))
#define HP_PERI0_PMS_READ_REE2_TRACE0_M  (HP_PERI0_PMS_READ_REE2_TRACE0_V << HP_PERI0_PMS_READ_REE2_TRACE0_S)
#define HP_PERI0_PMS_READ_REE2_TRACE0_V  0x00000001U
#define HP_PERI0_PMS_READ_REE2_TRACE0_S  3
/** HP_PERI0_PMS_WRITE_TEE_TRACE0 : R/W; bitpos: [4]; default: 1;
 *  Configures trace0 registers write permission in tee mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI0_PMS_WRITE_TEE_TRACE0    (BIT(4))
#define HP_PERI0_PMS_WRITE_TEE_TRACE0_M  (HP_PERI0_PMS_WRITE_TEE_TRACE0_V << HP_PERI0_PMS_WRITE_TEE_TRACE0_S)
#define HP_PERI0_PMS_WRITE_TEE_TRACE0_V  0x00000001U
#define HP_PERI0_PMS_WRITE_TEE_TRACE0_S  4
/** HP_PERI0_PMS_WRITE_REE0_TRACE0 : R/W; bitpos: [5]; default: 0;
 *  Configures trace0 registers write permission in ree0 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI0_PMS_WRITE_REE0_TRACE0    (BIT(5))
#define HP_PERI0_PMS_WRITE_REE0_TRACE0_M  (HP_PERI0_PMS_WRITE_REE0_TRACE0_V << HP_PERI0_PMS_WRITE_REE0_TRACE0_S)
#define HP_PERI0_PMS_WRITE_REE0_TRACE0_V  0x00000001U
#define HP_PERI0_PMS_WRITE_REE0_TRACE0_S  5
/** HP_PERI0_PMS_WRITE_REE1_TRACE0 : R/W; bitpos: [6]; default: 0;
 *  Configures trace0 registers write permission in ree1 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI0_PMS_WRITE_REE1_TRACE0    (BIT(6))
#define HP_PERI0_PMS_WRITE_REE1_TRACE0_M  (HP_PERI0_PMS_WRITE_REE1_TRACE0_V << HP_PERI0_PMS_WRITE_REE1_TRACE0_S)
#define HP_PERI0_PMS_WRITE_REE1_TRACE0_V  0x00000001U
#define HP_PERI0_PMS_WRITE_REE1_TRACE0_S  6
/** HP_PERI0_PMS_WRITE_REE2_TRACE0 : R/W; bitpos: [7]; default: 0;
 *  Configures trace0 registers write permission in ree2 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI0_PMS_WRITE_REE2_TRACE0    (BIT(7))
#define HP_PERI0_PMS_WRITE_REE2_TRACE0_M  (HP_PERI0_PMS_WRITE_REE2_TRACE0_V << HP_PERI0_PMS_WRITE_REE2_TRACE0_S)
#define HP_PERI0_PMS_WRITE_REE2_TRACE0_V  0x00000001U
#define HP_PERI0_PMS_WRITE_REE2_TRACE0_S  7
/** HP_PERI0_PMS_TRACE0_LOCK : R/W; bitpos: [8]; default: 0;
 *  Set 1 to lock trace0 peri_apm configuration
 */
#define HP_PERI0_PMS_TRACE0_LOCK    (BIT(8))
#define HP_PERI0_PMS_TRACE0_LOCK_M  (HP_PERI0_PMS_TRACE0_LOCK_V << HP_PERI0_PMS_TRACE0_LOCK_S)
#define HP_PERI0_PMS_TRACE0_LOCK_V  0x00000001U
#define HP_PERI0_PMS_TRACE0_LOCK_S  8

/** HP_PERI0_PMS_TRACE1_CTRL_REG register
 *  trace1 read/write control register
 */
#define HP_PERI0_PMS_TRACE1_CTRL_REG (DR_REG_HP_PERI0_PMS_BASE + 0x4)
/** HP_PERI0_PMS_READ_TEE_TRACE1 : R/W; bitpos: [0]; default: 1;
 *  Configures trace1 registers read permission in tee mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI0_PMS_READ_TEE_TRACE1    (BIT(0))
#define HP_PERI0_PMS_READ_TEE_TRACE1_M  (HP_PERI0_PMS_READ_TEE_TRACE1_V << HP_PERI0_PMS_READ_TEE_TRACE1_S)
#define HP_PERI0_PMS_READ_TEE_TRACE1_V  0x00000001U
#define HP_PERI0_PMS_READ_TEE_TRACE1_S  0
/** HP_PERI0_PMS_READ_REE0_TRACE1 : R/W; bitpos: [1]; default: 0;
 *  Configures trace1 registers read permission in ree0 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI0_PMS_READ_REE0_TRACE1    (BIT(1))
#define HP_PERI0_PMS_READ_REE0_TRACE1_M  (HP_PERI0_PMS_READ_REE0_TRACE1_V << HP_PERI0_PMS_READ_REE0_TRACE1_S)
#define HP_PERI0_PMS_READ_REE0_TRACE1_V  0x00000001U
#define HP_PERI0_PMS_READ_REE0_TRACE1_S  1
/** HP_PERI0_PMS_READ_REE1_TRACE1 : R/W; bitpos: [2]; default: 0;
 *  Configures trace1 registers read permission in ree1 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI0_PMS_READ_REE1_TRACE1    (BIT(2))
#define HP_PERI0_PMS_READ_REE1_TRACE1_M  (HP_PERI0_PMS_READ_REE1_TRACE1_V << HP_PERI0_PMS_READ_REE1_TRACE1_S)
#define HP_PERI0_PMS_READ_REE1_TRACE1_V  0x00000001U
#define HP_PERI0_PMS_READ_REE1_TRACE1_S  2
/** HP_PERI0_PMS_READ_REE2_TRACE1 : R/W; bitpos: [3]; default: 0;
 *  Configures trace1 registers read permission in ree2 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI0_PMS_READ_REE2_TRACE1    (BIT(3))
#define HP_PERI0_PMS_READ_REE2_TRACE1_M  (HP_PERI0_PMS_READ_REE2_TRACE1_V << HP_PERI0_PMS_READ_REE2_TRACE1_S)
#define HP_PERI0_PMS_READ_REE2_TRACE1_V  0x00000001U
#define HP_PERI0_PMS_READ_REE2_TRACE1_S  3
/** HP_PERI0_PMS_WRITE_TEE_TRACE1 : R/W; bitpos: [4]; default: 1;
 *  Configures trace1 registers write permission in tee mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI0_PMS_WRITE_TEE_TRACE1    (BIT(4))
#define HP_PERI0_PMS_WRITE_TEE_TRACE1_M  (HP_PERI0_PMS_WRITE_TEE_TRACE1_V << HP_PERI0_PMS_WRITE_TEE_TRACE1_S)
#define HP_PERI0_PMS_WRITE_TEE_TRACE1_V  0x00000001U
#define HP_PERI0_PMS_WRITE_TEE_TRACE1_S  4
/** HP_PERI0_PMS_WRITE_REE0_TRACE1 : R/W; bitpos: [5]; default: 0;
 *  Configures trace1 registers write permission in ree0 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI0_PMS_WRITE_REE0_TRACE1    (BIT(5))
#define HP_PERI0_PMS_WRITE_REE0_TRACE1_M  (HP_PERI0_PMS_WRITE_REE0_TRACE1_V << HP_PERI0_PMS_WRITE_REE0_TRACE1_S)
#define HP_PERI0_PMS_WRITE_REE0_TRACE1_V  0x00000001U
#define HP_PERI0_PMS_WRITE_REE0_TRACE1_S  5
/** HP_PERI0_PMS_WRITE_REE1_TRACE1 : R/W; bitpos: [6]; default: 0;
 *  Configures trace1 registers write permission in ree1 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI0_PMS_WRITE_REE1_TRACE1    (BIT(6))
#define HP_PERI0_PMS_WRITE_REE1_TRACE1_M  (HP_PERI0_PMS_WRITE_REE1_TRACE1_V << HP_PERI0_PMS_WRITE_REE1_TRACE1_S)
#define HP_PERI0_PMS_WRITE_REE1_TRACE1_V  0x00000001U
#define HP_PERI0_PMS_WRITE_REE1_TRACE1_S  6
/** HP_PERI0_PMS_WRITE_REE2_TRACE1 : R/W; bitpos: [7]; default: 0;
 *  Configures trace1 registers write permission in ree2 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI0_PMS_WRITE_REE2_TRACE1    (BIT(7))
#define HP_PERI0_PMS_WRITE_REE2_TRACE1_M  (HP_PERI0_PMS_WRITE_REE2_TRACE1_V << HP_PERI0_PMS_WRITE_REE2_TRACE1_S)
#define HP_PERI0_PMS_WRITE_REE2_TRACE1_V  0x00000001U
#define HP_PERI0_PMS_WRITE_REE2_TRACE1_S  7
/** HP_PERI0_PMS_TRACE1_LOCK : R/W; bitpos: [8]; default: 0;
 *  Set 1 to lock trace1 peri_apm configuration
 */
#define HP_PERI0_PMS_TRACE1_LOCK    (BIT(8))
#define HP_PERI0_PMS_TRACE1_LOCK_M  (HP_PERI0_PMS_TRACE1_LOCK_V << HP_PERI0_PMS_TRACE1_LOCK_S)
#define HP_PERI0_PMS_TRACE1_LOCK_V  0x00000001U
#define HP_PERI0_PMS_TRACE1_LOCK_S  8

/** HP_PERI0_PMS_CPU_BUS_MON_CTRL_REG register
 *  cpu_bus_mon read/write control register
 */
#define HP_PERI0_PMS_CPU_BUS_MON_CTRL_REG (DR_REG_HP_PERI0_PMS_BASE + 0x8)
/** HP_PERI0_PMS_READ_TEE_CPU_BUS_MON : R/W; bitpos: [0]; default: 1;
 *  Configures cpu_bus_mon registers read permission in tee mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI0_PMS_READ_TEE_CPU_BUS_MON    (BIT(0))
#define HP_PERI0_PMS_READ_TEE_CPU_BUS_MON_M  (HP_PERI0_PMS_READ_TEE_CPU_BUS_MON_V << HP_PERI0_PMS_READ_TEE_CPU_BUS_MON_S)
#define HP_PERI0_PMS_READ_TEE_CPU_BUS_MON_V  0x00000001U
#define HP_PERI0_PMS_READ_TEE_CPU_BUS_MON_S  0
/** HP_PERI0_PMS_READ_REE0_CPU_BUS_MON : R/W; bitpos: [1]; default: 0;
 *  Configures cpu_bus_mon registers read permission in ree0 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI0_PMS_READ_REE0_CPU_BUS_MON    (BIT(1))
#define HP_PERI0_PMS_READ_REE0_CPU_BUS_MON_M  (HP_PERI0_PMS_READ_REE0_CPU_BUS_MON_V << HP_PERI0_PMS_READ_REE0_CPU_BUS_MON_S)
#define HP_PERI0_PMS_READ_REE0_CPU_BUS_MON_V  0x00000001U
#define HP_PERI0_PMS_READ_REE0_CPU_BUS_MON_S  1
/** HP_PERI0_PMS_READ_REE1_CPU_BUS_MON : R/W; bitpos: [2]; default: 0;
 *  Configures cpu_bus_mon registers read permission in ree1 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI0_PMS_READ_REE1_CPU_BUS_MON    (BIT(2))
#define HP_PERI0_PMS_READ_REE1_CPU_BUS_MON_M  (HP_PERI0_PMS_READ_REE1_CPU_BUS_MON_V << HP_PERI0_PMS_READ_REE1_CPU_BUS_MON_S)
#define HP_PERI0_PMS_READ_REE1_CPU_BUS_MON_V  0x00000001U
#define HP_PERI0_PMS_READ_REE1_CPU_BUS_MON_S  2
/** HP_PERI0_PMS_READ_REE2_CPU_BUS_MON : R/W; bitpos: [3]; default: 0;
 *  Configures cpu_bus_mon registers read permission in ree2 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI0_PMS_READ_REE2_CPU_BUS_MON    (BIT(3))
#define HP_PERI0_PMS_READ_REE2_CPU_BUS_MON_M  (HP_PERI0_PMS_READ_REE2_CPU_BUS_MON_V << HP_PERI0_PMS_READ_REE2_CPU_BUS_MON_S)
#define HP_PERI0_PMS_READ_REE2_CPU_BUS_MON_V  0x00000001U
#define HP_PERI0_PMS_READ_REE2_CPU_BUS_MON_S  3
/** HP_PERI0_PMS_WRITE_TEE_CPU_BUS_MON : R/W; bitpos: [4]; default: 1;
 *  Configures cpu_bus_mon registers write permission in tee mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI0_PMS_WRITE_TEE_CPU_BUS_MON    (BIT(4))
#define HP_PERI0_PMS_WRITE_TEE_CPU_BUS_MON_M  (HP_PERI0_PMS_WRITE_TEE_CPU_BUS_MON_V << HP_PERI0_PMS_WRITE_TEE_CPU_BUS_MON_S)
#define HP_PERI0_PMS_WRITE_TEE_CPU_BUS_MON_V  0x00000001U
#define HP_PERI0_PMS_WRITE_TEE_CPU_BUS_MON_S  4
/** HP_PERI0_PMS_WRITE_REE0_CPU_BUS_MON : R/W; bitpos: [5]; default: 0;
 *  Configures cpu_bus_mon registers write permission in ree0 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI0_PMS_WRITE_REE0_CPU_BUS_MON    (BIT(5))
#define HP_PERI0_PMS_WRITE_REE0_CPU_BUS_MON_M  (HP_PERI0_PMS_WRITE_REE0_CPU_BUS_MON_V << HP_PERI0_PMS_WRITE_REE0_CPU_BUS_MON_S)
#define HP_PERI0_PMS_WRITE_REE0_CPU_BUS_MON_V  0x00000001U
#define HP_PERI0_PMS_WRITE_REE0_CPU_BUS_MON_S  5
/** HP_PERI0_PMS_WRITE_REE1_CPU_BUS_MON : R/W; bitpos: [6]; default: 0;
 *  Configures cpu_bus_mon registers write permission in ree1 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI0_PMS_WRITE_REE1_CPU_BUS_MON    (BIT(6))
#define HP_PERI0_PMS_WRITE_REE1_CPU_BUS_MON_M  (HP_PERI0_PMS_WRITE_REE1_CPU_BUS_MON_V << HP_PERI0_PMS_WRITE_REE1_CPU_BUS_MON_S)
#define HP_PERI0_PMS_WRITE_REE1_CPU_BUS_MON_V  0x00000001U
#define HP_PERI0_PMS_WRITE_REE1_CPU_BUS_MON_S  6
/** HP_PERI0_PMS_WRITE_REE2_CPU_BUS_MON : R/W; bitpos: [7]; default: 0;
 *  Configures cpu_bus_mon registers write permission in ree2 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI0_PMS_WRITE_REE2_CPU_BUS_MON    (BIT(7))
#define HP_PERI0_PMS_WRITE_REE2_CPU_BUS_MON_M  (HP_PERI0_PMS_WRITE_REE2_CPU_BUS_MON_V << HP_PERI0_PMS_WRITE_REE2_CPU_BUS_MON_S)
#define HP_PERI0_PMS_WRITE_REE2_CPU_BUS_MON_V  0x00000001U
#define HP_PERI0_PMS_WRITE_REE2_CPU_BUS_MON_S  7
/** HP_PERI0_PMS_CPU_BUS_MON_LOCK : R/W; bitpos: [8]; default: 0;
 *  Set 1 to lock cpu_bus_mon peri_apm configuration
 */
#define HP_PERI0_PMS_CPU_BUS_MON_LOCK    (BIT(8))
#define HP_PERI0_PMS_CPU_BUS_MON_LOCK_M  (HP_PERI0_PMS_CPU_BUS_MON_LOCK_V << HP_PERI0_PMS_CPU_BUS_MON_LOCK_S)
#define HP_PERI0_PMS_CPU_BUS_MON_LOCK_V  0x00000001U
#define HP_PERI0_PMS_CPU_BUS_MON_LOCK_S  8

/** HP_PERI0_PMS_PSRAM_MON_CTRL_REG register
 *  psram_mon read/write control register
 */
#define HP_PERI0_PMS_PSRAM_MON_CTRL_REG (DR_REG_HP_PERI0_PMS_BASE + 0xc)
/** HP_PERI0_PMS_READ_TEE_PSRAM_MON : R/W; bitpos: [0]; default: 1;
 *  Configures psram_mon registers read permission in tee mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI0_PMS_READ_TEE_PSRAM_MON    (BIT(0))
#define HP_PERI0_PMS_READ_TEE_PSRAM_MON_M  (HP_PERI0_PMS_READ_TEE_PSRAM_MON_V << HP_PERI0_PMS_READ_TEE_PSRAM_MON_S)
#define HP_PERI0_PMS_READ_TEE_PSRAM_MON_V  0x00000001U
#define HP_PERI0_PMS_READ_TEE_PSRAM_MON_S  0
/** HP_PERI0_PMS_READ_REE0_PSRAM_MON : R/W; bitpos: [1]; default: 0;
 *  Configures psram_mon registers read permission in ree0 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI0_PMS_READ_REE0_PSRAM_MON    (BIT(1))
#define HP_PERI0_PMS_READ_REE0_PSRAM_MON_M  (HP_PERI0_PMS_READ_REE0_PSRAM_MON_V << HP_PERI0_PMS_READ_REE0_PSRAM_MON_S)
#define HP_PERI0_PMS_READ_REE0_PSRAM_MON_V  0x00000001U
#define HP_PERI0_PMS_READ_REE0_PSRAM_MON_S  1
/** HP_PERI0_PMS_READ_REE1_PSRAM_MON : R/W; bitpos: [2]; default: 0;
 *  Configures psram_mon registers read permission in ree1 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI0_PMS_READ_REE1_PSRAM_MON    (BIT(2))
#define HP_PERI0_PMS_READ_REE1_PSRAM_MON_M  (HP_PERI0_PMS_READ_REE1_PSRAM_MON_V << HP_PERI0_PMS_READ_REE1_PSRAM_MON_S)
#define HP_PERI0_PMS_READ_REE1_PSRAM_MON_V  0x00000001U
#define HP_PERI0_PMS_READ_REE1_PSRAM_MON_S  2
/** HP_PERI0_PMS_READ_REE2_PSRAM_MON : R/W; bitpos: [3]; default: 0;
 *  Configures psram_mon registers read permission in ree2 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI0_PMS_READ_REE2_PSRAM_MON    (BIT(3))
#define HP_PERI0_PMS_READ_REE2_PSRAM_MON_M  (HP_PERI0_PMS_READ_REE2_PSRAM_MON_V << HP_PERI0_PMS_READ_REE2_PSRAM_MON_S)
#define HP_PERI0_PMS_READ_REE2_PSRAM_MON_V  0x00000001U
#define HP_PERI0_PMS_READ_REE2_PSRAM_MON_S  3
/** HP_PERI0_PMS_WRITE_TEE_PSRAM_MON : R/W; bitpos: [4]; default: 1;
 *  Configures psram_mon registers write permission in tee mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI0_PMS_WRITE_TEE_PSRAM_MON    (BIT(4))
#define HP_PERI0_PMS_WRITE_TEE_PSRAM_MON_M  (HP_PERI0_PMS_WRITE_TEE_PSRAM_MON_V << HP_PERI0_PMS_WRITE_TEE_PSRAM_MON_S)
#define HP_PERI0_PMS_WRITE_TEE_PSRAM_MON_V  0x00000001U
#define HP_PERI0_PMS_WRITE_TEE_PSRAM_MON_S  4
/** HP_PERI0_PMS_WRITE_REE0_PSRAM_MON : R/W; bitpos: [5]; default: 0;
 *  Configures psram_mon registers write permission in ree0 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI0_PMS_WRITE_REE0_PSRAM_MON    (BIT(5))
#define HP_PERI0_PMS_WRITE_REE0_PSRAM_MON_M  (HP_PERI0_PMS_WRITE_REE0_PSRAM_MON_V << HP_PERI0_PMS_WRITE_REE0_PSRAM_MON_S)
#define HP_PERI0_PMS_WRITE_REE0_PSRAM_MON_V  0x00000001U
#define HP_PERI0_PMS_WRITE_REE0_PSRAM_MON_S  5
/** HP_PERI0_PMS_WRITE_REE1_PSRAM_MON : R/W; bitpos: [6]; default: 0;
 *  Configures psram_mon registers write permission in ree1 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI0_PMS_WRITE_REE1_PSRAM_MON    (BIT(6))
#define HP_PERI0_PMS_WRITE_REE1_PSRAM_MON_M  (HP_PERI0_PMS_WRITE_REE1_PSRAM_MON_V << HP_PERI0_PMS_WRITE_REE1_PSRAM_MON_S)
#define HP_PERI0_PMS_WRITE_REE1_PSRAM_MON_V  0x00000001U
#define HP_PERI0_PMS_WRITE_REE1_PSRAM_MON_S  6
/** HP_PERI0_PMS_WRITE_REE2_PSRAM_MON : R/W; bitpos: [7]; default: 0;
 *  Configures psram_mon registers write permission in ree2 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI0_PMS_WRITE_REE2_PSRAM_MON    (BIT(7))
#define HP_PERI0_PMS_WRITE_REE2_PSRAM_MON_M  (HP_PERI0_PMS_WRITE_REE2_PSRAM_MON_V << HP_PERI0_PMS_WRITE_REE2_PSRAM_MON_S)
#define HP_PERI0_PMS_WRITE_REE2_PSRAM_MON_V  0x00000001U
#define HP_PERI0_PMS_WRITE_REE2_PSRAM_MON_S  7
/** HP_PERI0_PMS_PSRAM_MON_LOCK : R/W; bitpos: [8]; default: 0;
 *  Set 1 to lock psram_mon peri_apm configuration
 */
#define HP_PERI0_PMS_PSRAM_MON_LOCK    (BIT(8))
#define HP_PERI0_PMS_PSRAM_MON_LOCK_M  (HP_PERI0_PMS_PSRAM_MON_LOCK_V << HP_PERI0_PMS_PSRAM_MON_LOCK_S)
#define HP_PERI0_PMS_PSRAM_MON_LOCK_V  0x00000001U
#define HP_PERI0_PMS_PSRAM_MON_LOCK_S  8

/** HP_PERI0_PMS_TCM_MON_CTRL_REG register
 *  tcm_mon read/write control register
 */
#define HP_PERI0_PMS_TCM_MON_CTRL_REG (DR_REG_HP_PERI0_PMS_BASE + 0x10)
/** HP_PERI0_PMS_READ_TEE_TCM_MON : R/W; bitpos: [0]; default: 1;
 *  Configures tcm_mon registers read permission in tee mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI0_PMS_READ_TEE_TCM_MON    (BIT(0))
#define HP_PERI0_PMS_READ_TEE_TCM_MON_M  (HP_PERI0_PMS_READ_TEE_TCM_MON_V << HP_PERI0_PMS_READ_TEE_TCM_MON_S)
#define HP_PERI0_PMS_READ_TEE_TCM_MON_V  0x00000001U
#define HP_PERI0_PMS_READ_TEE_TCM_MON_S  0
/** HP_PERI0_PMS_READ_REE0_TCM_MON : R/W; bitpos: [1]; default: 0;
 *  Configures tcm_mon registers read permission in ree0 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI0_PMS_READ_REE0_TCM_MON    (BIT(1))
#define HP_PERI0_PMS_READ_REE0_TCM_MON_M  (HP_PERI0_PMS_READ_REE0_TCM_MON_V << HP_PERI0_PMS_READ_REE0_TCM_MON_S)
#define HP_PERI0_PMS_READ_REE0_TCM_MON_V  0x00000001U
#define HP_PERI0_PMS_READ_REE0_TCM_MON_S  1
/** HP_PERI0_PMS_READ_REE1_TCM_MON : R/W; bitpos: [2]; default: 0;
 *  Configures tcm_mon registers read permission in ree1 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI0_PMS_READ_REE1_TCM_MON    (BIT(2))
#define HP_PERI0_PMS_READ_REE1_TCM_MON_M  (HP_PERI0_PMS_READ_REE1_TCM_MON_V << HP_PERI0_PMS_READ_REE1_TCM_MON_S)
#define HP_PERI0_PMS_READ_REE1_TCM_MON_V  0x00000001U
#define HP_PERI0_PMS_READ_REE1_TCM_MON_S  2
/** HP_PERI0_PMS_READ_REE2_TCM_MON : R/W; bitpos: [3]; default: 0;
 *  Configures tcm_mon registers read permission in ree2 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI0_PMS_READ_REE2_TCM_MON    (BIT(3))
#define HP_PERI0_PMS_READ_REE2_TCM_MON_M  (HP_PERI0_PMS_READ_REE2_TCM_MON_V << HP_PERI0_PMS_READ_REE2_TCM_MON_S)
#define HP_PERI0_PMS_READ_REE2_TCM_MON_V  0x00000001U
#define HP_PERI0_PMS_READ_REE2_TCM_MON_S  3
/** HP_PERI0_PMS_WRITE_TEE_TCM_MON : R/W; bitpos: [4]; default: 1;
 *  Configures tcm_mon registers write permission in tee mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI0_PMS_WRITE_TEE_TCM_MON    (BIT(4))
#define HP_PERI0_PMS_WRITE_TEE_TCM_MON_M  (HP_PERI0_PMS_WRITE_TEE_TCM_MON_V << HP_PERI0_PMS_WRITE_TEE_TCM_MON_S)
#define HP_PERI0_PMS_WRITE_TEE_TCM_MON_V  0x00000001U
#define HP_PERI0_PMS_WRITE_TEE_TCM_MON_S  4
/** HP_PERI0_PMS_WRITE_REE0_TCM_MON : R/W; bitpos: [5]; default: 0;
 *  Configures tcm_mon registers write permission in ree0 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI0_PMS_WRITE_REE0_TCM_MON    (BIT(5))
#define HP_PERI0_PMS_WRITE_REE0_TCM_MON_M  (HP_PERI0_PMS_WRITE_REE0_TCM_MON_V << HP_PERI0_PMS_WRITE_REE0_TCM_MON_S)
#define HP_PERI0_PMS_WRITE_REE0_TCM_MON_V  0x00000001U
#define HP_PERI0_PMS_WRITE_REE0_TCM_MON_S  5
/** HP_PERI0_PMS_WRITE_REE1_TCM_MON : R/W; bitpos: [6]; default: 0;
 *  Configures tcm_mon registers write permission in ree1 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI0_PMS_WRITE_REE1_TCM_MON    (BIT(6))
#define HP_PERI0_PMS_WRITE_REE1_TCM_MON_M  (HP_PERI0_PMS_WRITE_REE1_TCM_MON_V << HP_PERI0_PMS_WRITE_REE1_TCM_MON_S)
#define HP_PERI0_PMS_WRITE_REE1_TCM_MON_V  0x00000001U
#define HP_PERI0_PMS_WRITE_REE1_TCM_MON_S  6
/** HP_PERI0_PMS_WRITE_REE2_TCM_MON : R/W; bitpos: [7]; default: 0;
 *  Configures tcm_mon registers write permission in ree2 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI0_PMS_WRITE_REE2_TCM_MON    (BIT(7))
#define HP_PERI0_PMS_WRITE_REE2_TCM_MON_M  (HP_PERI0_PMS_WRITE_REE2_TCM_MON_V << HP_PERI0_PMS_WRITE_REE2_TCM_MON_S)
#define HP_PERI0_PMS_WRITE_REE2_TCM_MON_V  0x00000001U
#define HP_PERI0_PMS_WRITE_REE2_TCM_MON_S  7
/** HP_PERI0_PMS_TCM_MON_LOCK : R/W; bitpos: [8]; default: 0;
 *  Set 1 to lock tcm_mon peri_apm configuration
 */
#define HP_PERI0_PMS_TCM_MON_LOCK    (BIT(8))
#define HP_PERI0_PMS_TCM_MON_LOCK_M  (HP_PERI0_PMS_TCM_MON_LOCK_V << HP_PERI0_PMS_TCM_MON_LOCK_S)
#define HP_PERI0_PMS_TCM_MON_LOCK_V  0x00000001U
#define HP_PERI0_PMS_TCM_MON_LOCK_S  8

/** HP_PERI0_PMS_CACHE_CTRL_REG register
 *  cache read/write control register
 */
#define HP_PERI0_PMS_CACHE_CTRL_REG (DR_REG_HP_PERI0_PMS_BASE + 0x14)
/** HP_PERI0_PMS_READ_TEE_CACHE : R/W; bitpos: [0]; default: 1;
 *  Configures cache registers read permission in tee mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI0_PMS_READ_TEE_CACHE    (BIT(0))
#define HP_PERI0_PMS_READ_TEE_CACHE_M  (HP_PERI0_PMS_READ_TEE_CACHE_V << HP_PERI0_PMS_READ_TEE_CACHE_S)
#define HP_PERI0_PMS_READ_TEE_CACHE_V  0x00000001U
#define HP_PERI0_PMS_READ_TEE_CACHE_S  0
/** HP_PERI0_PMS_READ_REE0_CACHE : R/W; bitpos: [1]; default: 0;
 *  Configures cache registers read permission in ree0 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI0_PMS_READ_REE0_CACHE    (BIT(1))
#define HP_PERI0_PMS_READ_REE0_CACHE_M  (HP_PERI0_PMS_READ_REE0_CACHE_V << HP_PERI0_PMS_READ_REE0_CACHE_S)
#define HP_PERI0_PMS_READ_REE0_CACHE_V  0x00000001U
#define HP_PERI0_PMS_READ_REE0_CACHE_S  1
/** HP_PERI0_PMS_READ_REE1_CACHE : R/W; bitpos: [2]; default: 0;
 *  Configures cache registers read permission in ree1 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI0_PMS_READ_REE1_CACHE    (BIT(2))
#define HP_PERI0_PMS_READ_REE1_CACHE_M  (HP_PERI0_PMS_READ_REE1_CACHE_V << HP_PERI0_PMS_READ_REE1_CACHE_S)
#define HP_PERI0_PMS_READ_REE1_CACHE_V  0x00000001U
#define HP_PERI0_PMS_READ_REE1_CACHE_S  2
/** HP_PERI0_PMS_READ_REE2_CACHE : R/W; bitpos: [3]; default: 0;
 *  Configures cache registers read permission in ree2 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI0_PMS_READ_REE2_CACHE    (BIT(3))
#define HP_PERI0_PMS_READ_REE2_CACHE_M  (HP_PERI0_PMS_READ_REE2_CACHE_V << HP_PERI0_PMS_READ_REE2_CACHE_S)
#define HP_PERI0_PMS_READ_REE2_CACHE_V  0x00000001U
#define HP_PERI0_PMS_READ_REE2_CACHE_S  3
/** HP_PERI0_PMS_WRITE_TEE_CACHE : R/W; bitpos: [4]; default: 1;
 *  Configures cache registers write permission in tee mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI0_PMS_WRITE_TEE_CACHE    (BIT(4))
#define HP_PERI0_PMS_WRITE_TEE_CACHE_M  (HP_PERI0_PMS_WRITE_TEE_CACHE_V << HP_PERI0_PMS_WRITE_TEE_CACHE_S)
#define HP_PERI0_PMS_WRITE_TEE_CACHE_V  0x00000001U
#define HP_PERI0_PMS_WRITE_TEE_CACHE_S  4
/** HP_PERI0_PMS_WRITE_REE0_CACHE : R/W; bitpos: [5]; default: 0;
 *  Configures cache registers write permission in ree0 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI0_PMS_WRITE_REE0_CACHE    (BIT(5))
#define HP_PERI0_PMS_WRITE_REE0_CACHE_M  (HP_PERI0_PMS_WRITE_REE0_CACHE_V << HP_PERI0_PMS_WRITE_REE0_CACHE_S)
#define HP_PERI0_PMS_WRITE_REE0_CACHE_V  0x00000001U
#define HP_PERI0_PMS_WRITE_REE0_CACHE_S  5
/** HP_PERI0_PMS_WRITE_REE1_CACHE : R/W; bitpos: [6]; default: 0;
 *  Configures cache registers write permission in ree1 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI0_PMS_WRITE_REE1_CACHE    (BIT(6))
#define HP_PERI0_PMS_WRITE_REE1_CACHE_M  (HP_PERI0_PMS_WRITE_REE1_CACHE_V << HP_PERI0_PMS_WRITE_REE1_CACHE_S)
#define HP_PERI0_PMS_WRITE_REE1_CACHE_V  0x00000001U
#define HP_PERI0_PMS_WRITE_REE1_CACHE_S  6
/** HP_PERI0_PMS_WRITE_REE2_CACHE : R/W; bitpos: [7]; default: 0;
 *  Configures cache registers write permission in ree2 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI0_PMS_WRITE_REE2_CACHE    (BIT(7))
#define HP_PERI0_PMS_WRITE_REE2_CACHE_M  (HP_PERI0_PMS_WRITE_REE2_CACHE_V << HP_PERI0_PMS_WRITE_REE2_CACHE_S)
#define HP_PERI0_PMS_WRITE_REE2_CACHE_V  0x00000001U
#define HP_PERI0_PMS_WRITE_REE2_CACHE_S  7
/** HP_PERI0_PMS_CACHE_LOCK : R/W; bitpos: [8]; default: 0;
 *  Set 1 to lock cache peri_apm configuration
 */
#define HP_PERI0_PMS_CACHE_LOCK    (BIT(8))
#define HP_PERI0_PMS_CACHE_LOCK_M  (HP_PERI0_PMS_CACHE_LOCK_V << HP_PERI0_PMS_CACHE_LOCK_S)
#define HP_PERI0_PMS_CACHE_LOCK_V  0x00000001U
#define HP_PERI0_PMS_CACHE_LOCK_S  8

/** HP_PERI0_PMS_HP_USBOTG_CORE_CTRL_REG register
 *  hp_usbotg_core read/write control register
 */
#define HP_PERI0_PMS_HP_USBOTG_CORE_CTRL_REG (DR_REG_HP_PERI0_PMS_BASE + 0x18)
/** HP_PERI0_PMS_READ_TEE_HP_USBOTG_CORE : R/W; bitpos: [0]; default: 1;
 *  Configures hp_usbotg_core registers read permission in tee mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI0_PMS_READ_TEE_HP_USBOTG_CORE    (BIT(0))
#define HP_PERI0_PMS_READ_TEE_HP_USBOTG_CORE_M  (HP_PERI0_PMS_READ_TEE_HP_USBOTG_CORE_V << HP_PERI0_PMS_READ_TEE_HP_USBOTG_CORE_S)
#define HP_PERI0_PMS_READ_TEE_HP_USBOTG_CORE_V  0x00000001U
#define HP_PERI0_PMS_READ_TEE_HP_USBOTG_CORE_S  0
/** HP_PERI0_PMS_READ_REE0_HP_USBOTG_CORE : R/W; bitpos: [1]; default: 0;
 *  Configures hp_usbotg_core registers read permission in ree0 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI0_PMS_READ_REE0_HP_USBOTG_CORE    (BIT(1))
#define HP_PERI0_PMS_READ_REE0_HP_USBOTG_CORE_M  (HP_PERI0_PMS_READ_REE0_HP_USBOTG_CORE_V << HP_PERI0_PMS_READ_REE0_HP_USBOTG_CORE_S)
#define HP_PERI0_PMS_READ_REE0_HP_USBOTG_CORE_V  0x00000001U
#define HP_PERI0_PMS_READ_REE0_HP_USBOTG_CORE_S  1
/** HP_PERI0_PMS_READ_REE1_HP_USBOTG_CORE : R/W; bitpos: [2]; default: 0;
 *  Configures hp_usbotg_core registers read permission in ree1 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI0_PMS_READ_REE1_HP_USBOTG_CORE    (BIT(2))
#define HP_PERI0_PMS_READ_REE1_HP_USBOTG_CORE_M  (HP_PERI0_PMS_READ_REE1_HP_USBOTG_CORE_V << HP_PERI0_PMS_READ_REE1_HP_USBOTG_CORE_S)
#define HP_PERI0_PMS_READ_REE1_HP_USBOTG_CORE_V  0x00000001U
#define HP_PERI0_PMS_READ_REE1_HP_USBOTG_CORE_S  2
/** HP_PERI0_PMS_READ_REE2_HP_USBOTG_CORE : R/W; bitpos: [3]; default: 0;
 *  Configures hp_usbotg_core registers read permission in ree2 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI0_PMS_READ_REE2_HP_USBOTG_CORE    (BIT(3))
#define HP_PERI0_PMS_READ_REE2_HP_USBOTG_CORE_M  (HP_PERI0_PMS_READ_REE2_HP_USBOTG_CORE_V << HP_PERI0_PMS_READ_REE2_HP_USBOTG_CORE_S)
#define HP_PERI0_PMS_READ_REE2_HP_USBOTG_CORE_V  0x00000001U
#define HP_PERI0_PMS_READ_REE2_HP_USBOTG_CORE_S  3
/** HP_PERI0_PMS_WRITE_TEE_HP_USBOTG_CORE : R/W; bitpos: [4]; default: 1;
 *  Configures hp_usbotg_core registers write permission in tee mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI0_PMS_WRITE_TEE_HP_USBOTG_CORE    (BIT(4))
#define HP_PERI0_PMS_WRITE_TEE_HP_USBOTG_CORE_M  (HP_PERI0_PMS_WRITE_TEE_HP_USBOTG_CORE_V << HP_PERI0_PMS_WRITE_TEE_HP_USBOTG_CORE_S)
#define HP_PERI0_PMS_WRITE_TEE_HP_USBOTG_CORE_V  0x00000001U
#define HP_PERI0_PMS_WRITE_TEE_HP_USBOTG_CORE_S  4
/** HP_PERI0_PMS_WRITE_REE0_HP_USBOTG_CORE : R/W; bitpos: [5]; default: 0;
 *  Configures hp_usbotg_core registers write permission in ree0 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI0_PMS_WRITE_REE0_HP_USBOTG_CORE    (BIT(5))
#define HP_PERI0_PMS_WRITE_REE0_HP_USBOTG_CORE_M  (HP_PERI0_PMS_WRITE_REE0_HP_USBOTG_CORE_V << HP_PERI0_PMS_WRITE_REE0_HP_USBOTG_CORE_S)
#define HP_PERI0_PMS_WRITE_REE0_HP_USBOTG_CORE_V  0x00000001U
#define HP_PERI0_PMS_WRITE_REE0_HP_USBOTG_CORE_S  5
/** HP_PERI0_PMS_WRITE_REE1_HP_USBOTG_CORE : R/W; bitpos: [6]; default: 0;
 *  Configures hp_usbotg_core registers write permission in ree1 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI0_PMS_WRITE_REE1_HP_USBOTG_CORE    (BIT(6))
#define HP_PERI0_PMS_WRITE_REE1_HP_USBOTG_CORE_M  (HP_PERI0_PMS_WRITE_REE1_HP_USBOTG_CORE_V << HP_PERI0_PMS_WRITE_REE1_HP_USBOTG_CORE_S)
#define HP_PERI0_PMS_WRITE_REE1_HP_USBOTG_CORE_V  0x00000001U
#define HP_PERI0_PMS_WRITE_REE1_HP_USBOTG_CORE_S  6
/** HP_PERI0_PMS_WRITE_REE2_HP_USBOTG_CORE : R/W; bitpos: [7]; default: 0;
 *  Configures hp_usbotg_core registers write permission in ree2 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI0_PMS_WRITE_REE2_HP_USBOTG_CORE    (BIT(7))
#define HP_PERI0_PMS_WRITE_REE2_HP_USBOTG_CORE_M  (HP_PERI0_PMS_WRITE_REE2_HP_USBOTG_CORE_V << HP_PERI0_PMS_WRITE_REE2_HP_USBOTG_CORE_S)
#define HP_PERI0_PMS_WRITE_REE2_HP_USBOTG_CORE_V  0x00000001U
#define HP_PERI0_PMS_WRITE_REE2_HP_USBOTG_CORE_S  7
/** HP_PERI0_PMS_HP_USBOTG_CORE_LOCK : R/W; bitpos: [8]; default: 0;
 *  Set 1 to lock hp_usbotg_core peri_apm configuration
 */
#define HP_PERI0_PMS_HP_USBOTG_CORE_LOCK    (BIT(8))
#define HP_PERI0_PMS_HP_USBOTG_CORE_LOCK_M  (HP_PERI0_PMS_HP_USBOTG_CORE_LOCK_V << HP_PERI0_PMS_HP_USBOTG_CORE_LOCK_S)
#define HP_PERI0_PMS_HP_USBOTG_CORE_LOCK_V  0x00000001U
#define HP_PERI0_PMS_HP_USBOTG_CORE_LOCK_S  8

/** HP_PERI0_PMS_HP_REGDMA_CTRL_REG register
 *  hp_regdma read/write control register
 */
#define HP_PERI0_PMS_HP_REGDMA_CTRL_REG (DR_REG_HP_PERI0_PMS_BASE + 0x1c)
/** HP_PERI0_PMS_READ_TEE_HP_REGDMA : R/W; bitpos: [0]; default: 1;
 *  Configures hp_regdma registers read permission in tee mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI0_PMS_READ_TEE_HP_REGDMA    (BIT(0))
#define HP_PERI0_PMS_READ_TEE_HP_REGDMA_M  (HP_PERI0_PMS_READ_TEE_HP_REGDMA_V << HP_PERI0_PMS_READ_TEE_HP_REGDMA_S)
#define HP_PERI0_PMS_READ_TEE_HP_REGDMA_V  0x00000001U
#define HP_PERI0_PMS_READ_TEE_HP_REGDMA_S  0
/** HP_PERI0_PMS_READ_REE0_HP_REGDMA : R/W; bitpos: [1]; default: 0;
 *  Configures hp_regdma registers read permission in ree0 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI0_PMS_READ_REE0_HP_REGDMA    (BIT(1))
#define HP_PERI0_PMS_READ_REE0_HP_REGDMA_M  (HP_PERI0_PMS_READ_REE0_HP_REGDMA_V << HP_PERI0_PMS_READ_REE0_HP_REGDMA_S)
#define HP_PERI0_PMS_READ_REE0_HP_REGDMA_V  0x00000001U
#define HP_PERI0_PMS_READ_REE0_HP_REGDMA_S  1
/** HP_PERI0_PMS_READ_REE1_HP_REGDMA : R/W; bitpos: [2]; default: 0;
 *  Configures hp_regdma registers read permission in ree1 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI0_PMS_READ_REE1_HP_REGDMA    (BIT(2))
#define HP_PERI0_PMS_READ_REE1_HP_REGDMA_M  (HP_PERI0_PMS_READ_REE1_HP_REGDMA_V << HP_PERI0_PMS_READ_REE1_HP_REGDMA_S)
#define HP_PERI0_PMS_READ_REE1_HP_REGDMA_V  0x00000001U
#define HP_PERI0_PMS_READ_REE1_HP_REGDMA_S  2
/** HP_PERI0_PMS_READ_REE2_HP_REGDMA : R/W; bitpos: [3]; default: 0;
 *  Configures hp_regdma registers read permission in ree2 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI0_PMS_READ_REE2_HP_REGDMA    (BIT(3))
#define HP_PERI0_PMS_READ_REE2_HP_REGDMA_M  (HP_PERI0_PMS_READ_REE2_HP_REGDMA_V << HP_PERI0_PMS_READ_REE2_HP_REGDMA_S)
#define HP_PERI0_PMS_READ_REE2_HP_REGDMA_V  0x00000001U
#define HP_PERI0_PMS_READ_REE2_HP_REGDMA_S  3
/** HP_PERI0_PMS_WRITE_TEE_HP_REGDMA : R/W; bitpos: [4]; default: 1;
 *  Configures hp_regdma registers write permission in tee mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI0_PMS_WRITE_TEE_HP_REGDMA    (BIT(4))
#define HP_PERI0_PMS_WRITE_TEE_HP_REGDMA_M  (HP_PERI0_PMS_WRITE_TEE_HP_REGDMA_V << HP_PERI0_PMS_WRITE_TEE_HP_REGDMA_S)
#define HP_PERI0_PMS_WRITE_TEE_HP_REGDMA_V  0x00000001U
#define HP_PERI0_PMS_WRITE_TEE_HP_REGDMA_S  4
/** HP_PERI0_PMS_WRITE_REE0_HP_REGDMA : R/W; bitpos: [5]; default: 0;
 *  Configures hp_regdma registers write permission in ree0 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI0_PMS_WRITE_REE0_HP_REGDMA    (BIT(5))
#define HP_PERI0_PMS_WRITE_REE0_HP_REGDMA_M  (HP_PERI0_PMS_WRITE_REE0_HP_REGDMA_V << HP_PERI0_PMS_WRITE_REE0_HP_REGDMA_S)
#define HP_PERI0_PMS_WRITE_REE0_HP_REGDMA_V  0x00000001U
#define HP_PERI0_PMS_WRITE_REE0_HP_REGDMA_S  5
/** HP_PERI0_PMS_WRITE_REE1_HP_REGDMA : R/W; bitpos: [6]; default: 0;
 *  Configures hp_regdma registers write permission in ree1 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI0_PMS_WRITE_REE1_HP_REGDMA    (BIT(6))
#define HP_PERI0_PMS_WRITE_REE1_HP_REGDMA_M  (HP_PERI0_PMS_WRITE_REE1_HP_REGDMA_V << HP_PERI0_PMS_WRITE_REE1_HP_REGDMA_S)
#define HP_PERI0_PMS_WRITE_REE1_HP_REGDMA_V  0x00000001U
#define HP_PERI0_PMS_WRITE_REE1_HP_REGDMA_S  6
/** HP_PERI0_PMS_WRITE_REE2_HP_REGDMA : R/W; bitpos: [7]; default: 0;
 *  Configures hp_regdma registers write permission in ree2 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI0_PMS_WRITE_REE2_HP_REGDMA    (BIT(7))
#define HP_PERI0_PMS_WRITE_REE2_HP_REGDMA_M  (HP_PERI0_PMS_WRITE_REE2_HP_REGDMA_V << HP_PERI0_PMS_WRITE_REE2_HP_REGDMA_S)
#define HP_PERI0_PMS_WRITE_REE2_HP_REGDMA_V  0x00000001U
#define HP_PERI0_PMS_WRITE_REE2_HP_REGDMA_S  7
/** HP_PERI0_PMS_HP_REGDMA_LOCK : R/W; bitpos: [8]; default: 0;
 *  Set 1 to lock hp_regdma peri_apm configuration
 */
#define HP_PERI0_PMS_HP_REGDMA_LOCK    (BIT(8))
#define HP_PERI0_PMS_HP_REGDMA_LOCK_M  (HP_PERI0_PMS_HP_REGDMA_LOCK_V << HP_PERI0_PMS_HP_REGDMA_LOCK_S)
#define HP_PERI0_PMS_HP_REGDMA_LOCK_V  0x00000001U
#define HP_PERI0_PMS_HP_REGDMA_LOCK_S  8

/** HP_PERI0_PMS_HP_SDMMC_CTRL_REG register
 *  hp_sdmmc read/write control register
 */
#define HP_PERI0_PMS_HP_SDMMC_CTRL_REG (DR_REG_HP_PERI0_PMS_BASE + 0x20)
/** HP_PERI0_PMS_READ_TEE_HP_SDMMC : R/W; bitpos: [0]; default: 1;
 *  Configures hp_sdmmc registers read permission in tee mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI0_PMS_READ_TEE_HP_SDMMC    (BIT(0))
#define HP_PERI0_PMS_READ_TEE_HP_SDMMC_M  (HP_PERI0_PMS_READ_TEE_HP_SDMMC_V << HP_PERI0_PMS_READ_TEE_HP_SDMMC_S)
#define HP_PERI0_PMS_READ_TEE_HP_SDMMC_V  0x00000001U
#define HP_PERI0_PMS_READ_TEE_HP_SDMMC_S  0
/** HP_PERI0_PMS_READ_REE0_HP_SDMMC : R/W; bitpos: [1]; default: 0;
 *  Configures hp_sdmmc registers read permission in ree0 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI0_PMS_READ_REE0_HP_SDMMC    (BIT(1))
#define HP_PERI0_PMS_READ_REE0_HP_SDMMC_M  (HP_PERI0_PMS_READ_REE0_HP_SDMMC_V << HP_PERI0_PMS_READ_REE0_HP_SDMMC_S)
#define HP_PERI0_PMS_READ_REE0_HP_SDMMC_V  0x00000001U
#define HP_PERI0_PMS_READ_REE0_HP_SDMMC_S  1
/** HP_PERI0_PMS_READ_REE1_HP_SDMMC : R/W; bitpos: [2]; default: 0;
 *  Configures hp_sdmmc registers read permission in ree1 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI0_PMS_READ_REE1_HP_SDMMC    (BIT(2))
#define HP_PERI0_PMS_READ_REE1_HP_SDMMC_M  (HP_PERI0_PMS_READ_REE1_HP_SDMMC_V << HP_PERI0_PMS_READ_REE1_HP_SDMMC_S)
#define HP_PERI0_PMS_READ_REE1_HP_SDMMC_V  0x00000001U
#define HP_PERI0_PMS_READ_REE1_HP_SDMMC_S  2
/** HP_PERI0_PMS_READ_REE2_HP_SDMMC : R/W; bitpos: [3]; default: 0;
 *  Configures hp_sdmmc registers read permission in ree2 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI0_PMS_READ_REE2_HP_SDMMC    (BIT(3))
#define HP_PERI0_PMS_READ_REE2_HP_SDMMC_M  (HP_PERI0_PMS_READ_REE2_HP_SDMMC_V << HP_PERI0_PMS_READ_REE2_HP_SDMMC_S)
#define HP_PERI0_PMS_READ_REE2_HP_SDMMC_V  0x00000001U
#define HP_PERI0_PMS_READ_REE2_HP_SDMMC_S  3
/** HP_PERI0_PMS_WRITE_TEE_HP_SDMMC : R/W; bitpos: [4]; default: 1;
 *  Configures hp_sdmmc registers write permission in tee mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI0_PMS_WRITE_TEE_HP_SDMMC    (BIT(4))
#define HP_PERI0_PMS_WRITE_TEE_HP_SDMMC_M  (HP_PERI0_PMS_WRITE_TEE_HP_SDMMC_V << HP_PERI0_PMS_WRITE_TEE_HP_SDMMC_S)
#define HP_PERI0_PMS_WRITE_TEE_HP_SDMMC_V  0x00000001U
#define HP_PERI0_PMS_WRITE_TEE_HP_SDMMC_S  4
/** HP_PERI0_PMS_WRITE_REE0_HP_SDMMC : R/W; bitpos: [5]; default: 0;
 *  Configures hp_sdmmc registers write permission in ree0 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI0_PMS_WRITE_REE0_HP_SDMMC    (BIT(5))
#define HP_PERI0_PMS_WRITE_REE0_HP_SDMMC_M  (HP_PERI0_PMS_WRITE_REE0_HP_SDMMC_V << HP_PERI0_PMS_WRITE_REE0_HP_SDMMC_S)
#define HP_PERI0_PMS_WRITE_REE0_HP_SDMMC_V  0x00000001U
#define HP_PERI0_PMS_WRITE_REE0_HP_SDMMC_S  5
/** HP_PERI0_PMS_WRITE_REE1_HP_SDMMC : R/W; bitpos: [6]; default: 0;
 *  Configures hp_sdmmc registers write permission in ree1 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI0_PMS_WRITE_REE1_HP_SDMMC    (BIT(6))
#define HP_PERI0_PMS_WRITE_REE1_HP_SDMMC_M  (HP_PERI0_PMS_WRITE_REE1_HP_SDMMC_V << HP_PERI0_PMS_WRITE_REE1_HP_SDMMC_S)
#define HP_PERI0_PMS_WRITE_REE1_HP_SDMMC_V  0x00000001U
#define HP_PERI0_PMS_WRITE_REE1_HP_SDMMC_S  6
/** HP_PERI0_PMS_WRITE_REE2_HP_SDMMC : R/W; bitpos: [7]; default: 0;
 *  Configures hp_sdmmc registers write permission in ree2 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI0_PMS_WRITE_REE2_HP_SDMMC    (BIT(7))
#define HP_PERI0_PMS_WRITE_REE2_HP_SDMMC_M  (HP_PERI0_PMS_WRITE_REE2_HP_SDMMC_V << HP_PERI0_PMS_WRITE_REE2_HP_SDMMC_S)
#define HP_PERI0_PMS_WRITE_REE2_HP_SDMMC_V  0x00000001U
#define HP_PERI0_PMS_WRITE_REE2_HP_SDMMC_S  7
/** HP_PERI0_PMS_HP_SDMMC_LOCK : R/W; bitpos: [8]; default: 0;
 *  Set 1 to lock hp_sdmmc peri_apm configuration
 */
#define HP_PERI0_PMS_HP_SDMMC_LOCK    (BIT(8))
#define HP_PERI0_PMS_HP_SDMMC_LOCK_M  (HP_PERI0_PMS_HP_SDMMC_LOCK_V << HP_PERI0_PMS_HP_SDMMC_LOCK_S)
#define HP_PERI0_PMS_HP_SDMMC_LOCK_V  0x00000001U
#define HP_PERI0_PMS_HP_SDMMC_LOCK_S  8

/** HP_PERI0_PMS_HP_AHB_PDMA_CTRL_REG register
 *  hp_ahb_pdma read/write control register
 */
#define HP_PERI0_PMS_HP_AHB_PDMA_CTRL_REG (DR_REG_HP_PERI0_PMS_BASE + 0x24)
/** HP_PERI0_PMS_READ_TEE_HP_AHB_PDMA : R/W; bitpos: [0]; default: 1;
 *  Configures hp_ahb_pdma registers read permission in tee mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI0_PMS_READ_TEE_HP_AHB_PDMA    (BIT(0))
#define HP_PERI0_PMS_READ_TEE_HP_AHB_PDMA_M  (HP_PERI0_PMS_READ_TEE_HP_AHB_PDMA_V << HP_PERI0_PMS_READ_TEE_HP_AHB_PDMA_S)
#define HP_PERI0_PMS_READ_TEE_HP_AHB_PDMA_V  0x00000001U
#define HP_PERI0_PMS_READ_TEE_HP_AHB_PDMA_S  0
/** HP_PERI0_PMS_READ_REE0_HP_AHB_PDMA : R/W; bitpos: [1]; default: 0;
 *  Configures hp_ahb_pdma registers read permission in ree0 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI0_PMS_READ_REE0_HP_AHB_PDMA    (BIT(1))
#define HP_PERI0_PMS_READ_REE0_HP_AHB_PDMA_M  (HP_PERI0_PMS_READ_REE0_HP_AHB_PDMA_V << HP_PERI0_PMS_READ_REE0_HP_AHB_PDMA_S)
#define HP_PERI0_PMS_READ_REE0_HP_AHB_PDMA_V  0x00000001U
#define HP_PERI0_PMS_READ_REE0_HP_AHB_PDMA_S  1
/** HP_PERI0_PMS_READ_REE1_HP_AHB_PDMA : R/W; bitpos: [2]; default: 0;
 *  Configures hp_ahb_pdma registers read permission in ree1 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI0_PMS_READ_REE1_HP_AHB_PDMA    (BIT(2))
#define HP_PERI0_PMS_READ_REE1_HP_AHB_PDMA_M  (HP_PERI0_PMS_READ_REE1_HP_AHB_PDMA_V << HP_PERI0_PMS_READ_REE1_HP_AHB_PDMA_S)
#define HP_PERI0_PMS_READ_REE1_HP_AHB_PDMA_V  0x00000001U
#define HP_PERI0_PMS_READ_REE1_HP_AHB_PDMA_S  2
/** HP_PERI0_PMS_READ_REE2_HP_AHB_PDMA : R/W; bitpos: [3]; default: 0;
 *  Configures hp_ahb_pdma registers read permission in ree2 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI0_PMS_READ_REE2_HP_AHB_PDMA    (BIT(3))
#define HP_PERI0_PMS_READ_REE2_HP_AHB_PDMA_M  (HP_PERI0_PMS_READ_REE2_HP_AHB_PDMA_V << HP_PERI0_PMS_READ_REE2_HP_AHB_PDMA_S)
#define HP_PERI0_PMS_READ_REE2_HP_AHB_PDMA_V  0x00000001U
#define HP_PERI0_PMS_READ_REE2_HP_AHB_PDMA_S  3
/** HP_PERI0_PMS_WRITE_TEE_HP_AHB_PDMA : R/W; bitpos: [4]; default: 1;
 *  Configures hp_ahb_pdma registers write permission in tee mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI0_PMS_WRITE_TEE_HP_AHB_PDMA    (BIT(4))
#define HP_PERI0_PMS_WRITE_TEE_HP_AHB_PDMA_M  (HP_PERI0_PMS_WRITE_TEE_HP_AHB_PDMA_V << HP_PERI0_PMS_WRITE_TEE_HP_AHB_PDMA_S)
#define HP_PERI0_PMS_WRITE_TEE_HP_AHB_PDMA_V  0x00000001U
#define HP_PERI0_PMS_WRITE_TEE_HP_AHB_PDMA_S  4
/** HP_PERI0_PMS_WRITE_REE0_HP_AHB_PDMA : R/W; bitpos: [5]; default: 0;
 *  Configures hp_ahb_pdma registers write permission in ree0 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI0_PMS_WRITE_REE0_HP_AHB_PDMA    (BIT(5))
#define HP_PERI0_PMS_WRITE_REE0_HP_AHB_PDMA_M  (HP_PERI0_PMS_WRITE_REE0_HP_AHB_PDMA_V << HP_PERI0_PMS_WRITE_REE0_HP_AHB_PDMA_S)
#define HP_PERI0_PMS_WRITE_REE0_HP_AHB_PDMA_V  0x00000001U
#define HP_PERI0_PMS_WRITE_REE0_HP_AHB_PDMA_S  5
/** HP_PERI0_PMS_WRITE_REE1_HP_AHB_PDMA : R/W; bitpos: [6]; default: 0;
 *  Configures hp_ahb_pdma registers write permission in ree1 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI0_PMS_WRITE_REE1_HP_AHB_PDMA    (BIT(6))
#define HP_PERI0_PMS_WRITE_REE1_HP_AHB_PDMA_M  (HP_PERI0_PMS_WRITE_REE1_HP_AHB_PDMA_V << HP_PERI0_PMS_WRITE_REE1_HP_AHB_PDMA_S)
#define HP_PERI0_PMS_WRITE_REE1_HP_AHB_PDMA_V  0x00000001U
#define HP_PERI0_PMS_WRITE_REE1_HP_AHB_PDMA_S  6
/** HP_PERI0_PMS_WRITE_REE2_HP_AHB_PDMA : R/W; bitpos: [7]; default: 0;
 *  Configures hp_ahb_pdma registers write permission in ree2 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI0_PMS_WRITE_REE2_HP_AHB_PDMA    (BIT(7))
#define HP_PERI0_PMS_WRITE_REE2_HP_AHB_PDMA_M  (HP_PERI0_PMS_WRITE_REE2_HP_AHB_PDMA_V << HP_PERI0_PMS_WRITE_REE2_HP_AHB_PDMA_S)
#define HP_PERI0_PMS_WRITE_REE2_HP_AHB_PDMA_V  0x00000001U
#define HP_PERI0_PMS_WRITE_REE2_HP_AHB_PDMA_S  7
/** HP_PERI0_PMS_HP_AHB_PDMA_LOCK : R/W; bitpos: [8]; default: 0;
 *  Set 1 to lock hp_ahb_pdma peri_apm configuration
 */
#define HP_PERI0_PMS_HP_AHB_PDMA_LOCK    (BIT(8))
#define HP_PERI0_PMS_HP_AHB_PDMA_LOCK_M  (HP_PERI0_PMS_HP_AHB_PDMA_LOCK_V << HP_PERI0_PMS_HP_AHB_PDMA_LOCK_S)
#define HP_PERI0_PMS_HP_AHB_PDMA_LOCK_V  0x00000001U
#define HP_PERI0_PMS_HP_AHB_PDMA_LOCK_S  8

/** HP_PERI0_PMS_HP_JPEG_CTRL_REG register
 *  hp_jpeg read/write control register
 */
#define HP_PERI0_PMS_HP_JPEG_CTRL_REG (DR_REG_HP_PERI0_PMS_BASE + 0x28)
/** HP_PERI0_PMS_READ_TEE_HP_JPEG : R/W; bitpos: [0]; default: 1;
 *  Configures hp_jpeg registers read permission in tee mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI0_PMS_READ_TEE_HP_JPEG    (BIT(0))
#define HP_PERI0_PMS_READ_TEE_HP_JPEG_M  (HP_PERI0_PMS_READ_TEE_HP_JPEG_V << HP_PERI0_PMS_READ_TEE_HP_JPEG_S)
#define HP_PERI0_PMS_READ_TEE_HP_JPEG_V  0x00000001U
#define HP_PERI0_PMS_READ_TEE_HP_JPEG_S  0
/** HP_PERI0_PMS_READ_REE0_HP_JPEG : R/W; bitpos: [1]; default: 0;
 *  Configures hp_jpeg registers read permission in ree0 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI0_PMS_READ_REE0_HP_JPEG    (BIT(1))
#define HP_PERI0_PMS_READ_REE0_HP_JPEG_M  (HP_PERI0_PMS_READ_REE0_HP_JPEG_V << HP_PERI0_PMS_READ_REE0_HP_JPEG_S)
#define HP_PERI0_PMS_READ_REE0_HP_JPEG_V  0x00000001U
#define HP_PERI0_PMS_READ_REE0_HP_JPEG_S  1
/** HP_PERI0_PMS_READ_REE1_HP_JPEG : R/W; bitpos: [2]; default: 0;
 *  Configures hp_jpeg registers read permission in ree1 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI0_PMS_READ_REE1_HP_JPEG    (BIT(2))
#define HP_PERI0_PMS_READ_REE1_HP_JPEG_M  (HP_PERI0_PMS_READ_REE1_HP_JPEG_V << HP_PERI0_PMS_READ_REE1_HP_JPEG_S)
#define HP_PERI0_PMS_READ_REE1_HP_JPEG_V  0x00000001U
#define HP_PERI0_PMS_READ_REE1_HP_JPEG_S  2
/** HP_PERI0_PMS_READ_REE2_HP_JPEG : R/W; bitpos: [3]; default: 0;
 *  Configures hp_jpeg registers read permission in ree2 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI0_PMS_READ_REE2_HP_JPEG    (BIT(3))
#define HP_PERI0_PMS_READ_REE2_HP_JPEG_M  (HP_PERI0_PMS_READ_REE2_HP_JPEG_V << HP_PERI0_PMS_READ_REE2_HP_JPEG_S)
#define HP_PERI0_PMS_READ_REE2_HP_JPEG_V  0x00000001U
#define HP_PERI0_PMS_READ_REE2_HP_JPEG_S  3
/** HP_PERI0_PMS_WRITE_TEE_HP_JPEG : R/W; bitpos: [4]; default: 1;
 *  Configures hp_jpeg registers write permission in tee mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI0_PMS_WRITE_TEE_HP_JPEG    (BIT(4))
#define HP_PERI0_PMS_WRITE_TEE_HP_JPEG_M  (HP_PERI0_PMS_WRITE_TEE_HP_JPEG_V << HP_PERI0_PMS_WRITE_TEE_HP_JPEG_S)
#define HP_PERI0_PMS_WRITE_TEE_HP_JPEG_V  0x00000001U
#define HP_PERI0_PMS_WRITE_TEE_HP_JPEG_S  4
/** HP_PERI0_PMS_WRITE_REE0_HP_JPEG : R/W; bitpos: [5]; default: 0;
 *  Configures hp_jpeg registers write permission in ree0 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI0_PMS_WRITE_REE0_HP_JPEG    (BIT(5))
#define HP_PERI0_PMS_WRITE_REE0_HP_JPEG_M  (HP_PERI0_PMS_WRITE_REE0_HP_JPEG_V << HP_PERI0_PMS_WRITE_REE0_HP_JPEG_S)
#define HP_PERI0_PMS_WRITE_REE0_HP_JPEG_V  0x00000001U
#define HP_PERI0_PMS_WRITE_REE0_HP_JPEG_S  5
/** HP_PERI0_PMS_WRITE_REE1_HP_JPEG : R/W; bitpos: [6]; default: 0;
 *  Configures hp_jpeg registers write permission in ree1 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI0_PMS_WRITE_REE1_HP_JPEG    (BIT(6))
#define HP_PERI0_PMS_WRITE_REE1_HP_JPEG_M  (HP_PERI0_PMS_WRITE_REE1_HP_JPEG_V << HP_PERI0_PMS_WRITE_REE1_HP_JPEG_S)
#define HP_PERI0_PMS_WRITE_REE1_HP_JPEG_V  0x00000001U
#define HP_PERI0_PMS_WRITE_REE1_HP_JPEG_S  6
/** HP_PERI0_PMS_WRITE_REE2_HP_JPEG : R/W; bitpos: [7]; default: 0;
 *  Configures hp_jpeg registers write permission in ree2 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI0_PMS_WRITE_REE2_HP_JPEG    (BIT(7))
#define HP_PERI0_PMS_WRITE_REE2_HP_JPEG_M  (HP_PERI0_PMS_WRITE_REE2_HP_JPEG_V << HP_PERI0_PMS_WRITE_REE2_HP_JPEG_S)
#define HP_PERI0_PMS_WRITE_REE2_HP_JPEG_V  0x00000001U
#define HP_PERI0_PMS_WRITE_REE2_HP_JPEG_S  7
/** HP_PERI0_PMS_HP_JPEG_LOCK : R/W; bitpos: [8]; default: 0;
 *  Set 1 to lock hp_jpeg peri_apm configuration
 */
#define HP_PERI0_PMS_HP_JPEG_LOCK    (BIT(8))
#define HP_PERI0_PMS_HP_JPEG_LOCK_M  (HP_PERI0_PMS_HP_JPEG_LOCK_V << HP_PERI0_PMS_HP_JPEG_LOCK_S)
#define HP_PERI0_PMS_HP_JPEG_LOCK_V  0x00000001U
#define HP_PERI0_PMS_HP_JPEG_LOCK_S  8

/** HP_PERI0_PMS_HP_PPA_CTRL_REG register
 *  hp_ppa read/write control register
 */
#define HP_PERI0_PMS_HP_PPA_CTRL_REG (DR_REG_HP_PERI0_PMS_BASE + 0x2c)
/** HP_PERI0_PMS_READ_TEE_HP_PPA : R/W; bitpos: [0]; default: 1;
 *  Configures hp_ppa registers read permission in tee mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI0_PMS_READ_TEE_HP_PPA    (BIT(0))
#define HP_PERI0_PMS_READ_TEE_HP_PPA_M  (HP_PERI0_PMS_READ_TEE_HP_PPA_V << HP_PERI0_PMS_READ_TEE_HP_PPA_S)
#define HP_PERI0_PMS_READ_TEE_HP_PPA_V  0x00000001U
#define HP_PERI0_PMS_READ_TEE_HP_PPA_S  0
/** HP_PERI0_PMS_READ_REE0_HP_PPA : R/W; bitpos: [1]; default: 0;
 *  Configures hp_ppa registers read permission in ree0 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI0_PMS_READ_REE0_HP_PPA    (BIT(1))
#define HP_PERI0_PMS_READ_REE0_HP_PPA_M  (HP_PERI0_PMS_READ_REE0_HP_PPA_V << HP_PERI0_PMS_READ_REE0_HP_PPA_S)
#define HP_PERI0_PMS_READ_REE0_HP_PPA_V  0x00000001U
#define HP_PERI0_PMS_READ_REE0_HP_PPA_S  1
/** HP_PERI0_PMS_READ_REE1_HP_PPA : R/W; bitpos: [2]; default: 0;
 *  Configures hp_ppa registers read permission in ree1 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI0_PMS_READ_REE1_HP_PPA    (BIT(2))
#define HP_PERI0_PMS_READ_REE1_HP_PPA_M  (HP_PERI0_PMS_READ_REE1_HP_PPA_V << HP_PERI0_PMS_READ_REE1_HP_PPA_S)
#define HP_PERI0_PMS_READ_REE1_HP_PPA_V  0x00000001U
#define HP_PERI0_PMS_READ_REE1_HP_PPA_S  2
/** HP_PERI0_PMS_READ_REE2_HP_PPA : R/W; bitpos: [3]; default: 0;
 *  Configures hp_ppa registers read permission in ree2 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI0_PMS_READ_REE2_HP_PPA    (BIT(3))
#define HP_PERI0_PMS_READ_REE2_HP_PPA_M  (HP_PERI0_PMS_READ_REE2_HP_PPA_V << HP_PERI0_PMS_READ_REE2_HP_PPA_S)
#define HP_PERI0_PMS_READ_REE2_HP_PPA_V  0x00000001U
#define HP_PERI0_PMS_READ_REE2_HP_PPA_S  3
/** HP_PERI0_PMS_WRITE_TEE_HP_PPA : R/W; bitpos: [4]; default: 1;
 *  Configures hp_ppa registers write permission in tee mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI0_PMS_WRITE_TEE_HP_PPA    (BIT(4))
#define HP_PERI0_PMS_WRITE_TEE_HP_PPA_M  (HP_PERI0_PMS_WRITE_TEE_HP_PPA_V << HP_PERI0_PMS_WRITE_TEE_HP_PPA_S)
#define HP_PERI0_PMS_WRITE_TEE_HP_PPA_V  0x00000001U
#define HP_PERI0_PMS_WRITE_TEE_HP_PPA_S  4
/** HP_PERI0_PMS_WRITE_REE0_HP_PPA : R/W; bitpos: [5]; default: 0;
 *  Configures hp_ppa registers write permission in ree0 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI0_PMS_WRITE_REE0_HP_PPA    (BIT(5))
#define HP_PERI0_PMS_WRITE_REE0_HP_PPA_M  (HP_PERI0_PMS_WRITE_REE0_HP_PPA_V << HP_PERI0_PMS_WRITE_REE0_HP_PPA_S)
#define HP_PERI0_PMS_WRITE_REE0_HP_PPA_V  0x00000001U
#define HP_PERI0_PMS_WRITE_REE0_HP_PPA_S  5
/** HP_PERI0_PMS_WRITE_REE1_HP_PPA : R/W; bitpos: [6]; default: 0;
 *  Configures hp_ppa registers write permission in ree1 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI0_PMS_WRITE_REE1_HP_PPA    (BIT(6))
#define HP_PERI0_PMS_WRITE_REE1_HP_PPA_M  (HP_PERI0_PMS_WRITE_REE1_HP_PPA_V << HP_PERI0_PMS_WRITE_REE1_HP_PPA_S)
#define HP_PERI0_PMS_WRITE_REE1_HP_PPA_V  0x00000001U
#define HP_PERI0_PMS_WRITE_REE1_HP_PPA_S  6
/** HP_PERI0_PMS_WRITE_REE2_HP_PPA : R/W; bitpos: [7]; default: 0;
 *  Configures hp_ppa registers write permission in ree2 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI0_PMS_WRITE_REE2_HP_PPA    (BIT(7))
#define HP_PERI0_PMS_WRITE_REE2_HP_PPA_M  (HP_PERI0_PMS_WRITE_REE2_HP_PPA_V << HP_PERI0_PMS_WRITE_REE2_HP_PPA_S)
#define HP_PERI0_PMS_WRITE_REE2_HP_PPA_V  0x00000001U
#define HP_PERI0_PMS_WRITE_REE2_HP_PPA_S  7
/** HP_PERI0_PMS_HP_PPA_LOCK : R/W; bitpos: [8]; default: 0;
 *  Set 1 to lock hp_ppa peri_apm configuration
 */
#define HP_PERI0_PMS_HP_PPA_LOCK    (BIT(8))
#define HP_PERI0_PMS_HP_PPA_LOCK_M  (HP_PERI0_PMS_HP_PPA_LOCK_V << HP_PERI0_PMS_HP_PPA_LOCK_S)
#define HP_PERI0_PMS_HP_PPA_LOCK_V  0x00000001U
#define HP_PERI0_PMS_HP_PPA_LOCK_S  8

/** HP_PERI0_PMS_HP_DMA2D_CTRL_REG register
 *  hp_dma2d read/write control register
 */
#define HP_PERI0_PMS_HP_DMA2D_CTRL_REG (DR_REG_HP_PERI0_PMS_BASE + 0x30)
/** HP_PERI0_PMS_READ_TEE_HP_DMA2D : R/W; bitpos: [0]; default: 1;
 *  Configures hp_dma2d registers read permission in tee mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI0_PMS_READ_TEE_HP_DMA2D    (BIT(0))
#define HP_PERI0_PMS_READ_TEE_HP_DMA2D_M  (HP_PERI0_PMS_READ_TEE_HP_DMA2D_V << HP_PERI0_PMS_READ_TEE_HP_DMA2D_S)
#define HP_PERI0_PMS_READ_TEE_HP_DMA2D_V  0x00000001U
#define HP_PERI0_PMS_READ_TEE_HP_DMA2D_S  0
/** HP_PERI0_PMS_READ_REE0_HP_DMA2D : R/W; bitpos: [1]; default: 0;
 *  Configures hp_dma2d registers read permission in ree0 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI0_PMS_READ_REE0_HP_DMA2D    (BIT(1))
#define HP_PERI0_PMS_READ_REE0_HP_DMA2D_M  (HP_PERI0_PMS_READ_REE0_HP_DMA2D_V << HP_PERI0_PMS_READ_REE0_HP_DMA2D_S)
#define HP_PERI0_PMS_READ_REE0_HP_DMA2D_V  0x00000001U
#define HP_PERI0_PMS_READ_REE0_HP_DMA2D_S  1
/** HP_PERI0_PMS_READ_REE1_HP_DMA2D : R/W; bitpos: [2]; default: 0;
 *  Configures hp_dma2d registers read permission in ree1 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI0_PMS_READ_REE1_HP_DMA2D    (BIT(2))
#define HP_PERI0_PMS_READ_REE1_HP_DMA2D_M  (HP_PERI0_PMS_READ_REE1_HP_DMA2D_V << HP_PERI0_PMS_READ_REE1_HP_DMA2D_S)
#define HP_PERI0_PMS_READ_REE1_HP_DMA2D_V  0x00000001U
#define HP_PERI0_PMS_READ_REE1_HP_DMA2D_S  2
/** HP_PERI0_PMS_READ_REE2_HP_DMA2D : R/W; bitpos: [3]; default: 0;
 *  Configures hp_dma2d registers read permission in ree2 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI0_PMS_READ_REE2_HP_DMA2D    (BIT(3))
#define HP_PERI0_PMS_READ_REE2_HP_DMA2D_M  (HP_PERI0_PMS_READ_REE2_HP_DMA2D_V << HP_PERI0_PMS_READ_REE2_HP_DMA2D_S)
#define HP_PERI0_PMS_READ_REE2_HP_DMA2D_V  0x00000001U
#define HP_PERI0_PMS_READ_REE2_HP_DMA2D_S  3
/** HP_PERI0_PMS_WRITE_TEE_HP_DMA2D : R/W; bitpos: [4]; default: 1;
 *  Configures hp_dma2d registers write permission in tee mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI0_PMS_WRITE_TEE_HP_DMA2D    (BIT(4))
#define HP_PERI0_PMS_WRITE_TEE_HP_DMA2D_M  (HP_PERI0_PMS_WRITE_TEE_HP_DMA2D_V << HP_PERI0_PMS_WRITE_TEE_HP_DMA2D_S)
#define HP_PERI0_PMS_WRITE_TEE_HP_DMA2D_V  0x00000001U
#define HP_PERI0_PMS_WRITE_TEE_HP_DMA2D_S  4
/** HP_PERI0_PMS_WRITE_REE0_HP_DMA2D : R/W; bitpos: [5]; default: 0;
 *  Configures hp_dma2d registers write permission in ree0 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI0_PMS_WRITE_REE0_HP_DMA2D    (BIT(5))
#define HP_PERI0_PMS_WRITE_REE0_HP_DMA2D_M  (HP_PERI0_PMS_WRITE_REE0_HP_DMA2D_V << HP_PERI0_PMS_WRITE_REE0_HP_DMA2D_S)
#define HP_PERI0_PMS_WRITE_REE0_HP_DMA2D_V  0x00000001U
#define HP_PERI0_PMS_WRITE_REE0_HP_DMA2D_S  5
/** HP_PERI0_PMS_WRITE_REE1_HP_DMA2D : R/W; bitpos: [6]; default: 0;
 *  Configures hp_dma2d registers write permission in ree1 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI0_PMS_WRITE_REE1_HP_DMA2D    (BIT(6))
#define HP_PERI0_PMS_WRITE_REE1_HP_DMA2D_M  (HP_PERI0_PMS_WRITE_REE1_HP_DMA2D_V << HP_PERI0_PMS_WRITE_REE1_HP_DMA2D_S)
#define HP_PERI0_PMS_WRITE_REE1_HP_DMA2D_V  0x00000001U
#define HP_PERI0_PMS_WRITE_REE1_HP_DMA2D_S  6
/** HP_PERI0_PMS_WRITE_REE2_HP_DMA2D : R/W; bitpos: [7]; default: 0;
 *  Configures hp_dma2d registers write permission in ree2 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI0_PMS_WRITE_REE2_HP_DMA2D    (BIT(7))
#define HP_PERI0_PMS_WRITE_REE2_HP_DMA2D_M  (HP_PERI0_PMS_WRITE_REE2_HP_DMA2D_V << HP_PERI0_PMS_WRITE_REE2_HP_DMA2D_S)
#define HP_PERI0_PMS_WRITE_REE2_HP_DMA2D_V  0x00000001U
#define HP_PERI0_PMS_WRITE_REE2_HP_DMA2D_S  7
/** HP_PERI0_PMS_HP_DMA2D_LOCK : R/W; bitpos: [8]; default: 0;
 *  Set 1 to lock hp_dma2d peri_apm configuration
 */
#define HP_PERI0_PMS_HP_DMA2D_LOCK    (BIT(8))
#define HP_PERI0_PMS_HP_DMA2D_LOCK_M  (HP_PERI0_PMS_HP_DMA2D_LOCK_V << HP_PERI0_PMS_HP_DMA2D_LOCK_S)
#define HP_PERI0_PMS_HP_DMA2D_LOCK_V  0x00000001U
#define HP_PERI0_PMS_HP_DMA2D_LOCK_S  8

/** HP_PERI0_PMS_HP_AXI_PDMA_CTRL_REG register
 *  hp_axi_pdma read/write control register
 */
#define HP_PERI0_PMS_HP_AXI_PDMA_CTRL_REG (DR_REG_HP_PERI0_PMS_BASE + 0x34)
/** HP_PERI0_PMS_READ_TEE_HP_AXI_PDMA : R/W; bitpos: [0]; default: 1;
 *  Configures hp_axi_pdma registers read permission in tee mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI0_PMS_READ_TEE_HP_AXI_PDMA    (BIT(0))
#define HP_PERI0_PMS_READ_TEE_HP_AXI_PDMA_M  (HP_PERI0_PMS_READ_TEE_HP_AXI_PDMA_V << HP_PERI0_PMS_READ_TEE_HP_AXI_PDMA_S)
#define HP_PERI0_PMS_READ_TEE_HP_AXI_PDMA_V  0x00000001U
#define HP_PERI0_PMS_READ_TEE_HP_AXI_PDMA_S  0
/** HP_PERI0_PMS_READ_REE0_HP_AXI_PDMA : R/W; bitpos: [1]; default: 0;
 *  Configures hp_axi_pdma registers read permission in ree0 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI0_PMS_READ_REE0_HP_AXI_PDMA    (BIT(1))
#define HP_PERI0_PMS_READ_REE0_HP_AXI_PDMA_M  (HP_PERI0_PMS_READ_REE0_HP_AXI_PDMA_V << HP_PERI0_PMS_READ_REE0_HP_AXI_PDMA_S)
#define HP_PERI0_PMS_READ_REE0_HP_AXI_PDMA_V  0x00000001U
#define HP_PERI0_PMS_READ_REE0_HP_AXI_PDMA_S  1
/** HP_PERI0_PMS_READ_REE1_HP_AXI_PDMA : R/W; bitpos: [2]; default: 0;
 *  Configures hp_axi_pdma registers read permission in ree1 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI0_PMS_READ_REE1_HP_AXI_PDMA    (BIT(2))
#define HP_PERI0_PMS_READ_REE1_HP_AXI_PDMA_M  (HP_PERI0_PMS_READ_REE1_HP_AXI_PDMA_V << HP_PERI0_PMS_READ_REE1_HP_AXI_PDMA_S)
#define HP_PERI0_PMS_READ_REE1_HP_AXI_PDMA_V  0x00000001U
#define HP_PERI0_PMS_READ_REE1_HP_AXI_PDMA_S  2
/** HP_PERI0_PMS_READ_REE2_HP_AXI_PDMA : R/W; bitpos: [3]; default: 0;
 *  Configures hp_axi_pdma registers read permission in ree2 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI0_PMS_READ_REE2_HP_AXI_PDMA    (BIT(3))
#define HP_PERI0_PMS_READ_REE2_HP_AXI_PDMA_M  (HP_PERI0_PMS_READ_REE2_HP_AXI_PDMA_V << HP_PERI0_PMS_READ_REE2_HP_AXI_PDMA_S)
#define HP_PERI0_PMS_READ_REE2_HP_AXI_PDMA_V  0x00000001U
#define HP_PERI0_PMS_READ_REE2_HP_AXI_PDMA_S  3
/** HP_PERI0_PMS_WRITE_TEE_HP_AXI_PDMA : R/W; bitpos: [4]; default: 1;
 *  Configures hp_axi_pdma registers write permission in tee mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI0_PMS_WRITE_TEE_HP_AXI_PDMA    (BIT(4))
#define HP_PERI0_PMS_WRITE_TEE_HP_AXI_PDMA_M  (HP_PERI0_PMS_WRITE_TEE_HP_AXI_PDMA_V << HP_PERI0_PMS_WRITE_TEE_HP_AXI_PDMA_S)
#define HP_PERI0_PMS_WRITE_TEE_HP_AXI_PDMA_V  0x00000001U
#define HP_PERI0_PMS_WRITE_TEE_HP_AXI_PDMA_S  4
/** HP_PERI0_PMS_WRITE_REE0_HP_AXI_PDMA : R/W; bitpos: [5]; default: 0;
 *  Configures hp_axi_pdma registers write permission in ree0 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI0_PMS_WRITE_REE0_HP_AXI_PDMA    (BIT(5))
#define HP_PERI0_PMS_WRITE_REE0_HP_AXI_PDMA_M  (HP_PERI0_PMS_WRITE_REE0_HP_AXI_PDMA_V << HP_PERI0_PMS_WRITE_REE0_HP_AXI_PDMA_S)
#define HP_PERI0_PMS_WRITE_REE0_HP_AXI_PDMA_V  0x00000001U
#define HP_PERI0_PMS_WRITE_REE0_HP_AXI_PDMA_S  5
/** HP_PERI0_PMS_WRITE_REE1_HP_AXI_PDMA : R/W; bitpos: [6]; default: 0;
 *  Configures hp_axi_pdma registers write permission in ree1 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI0_PMS_WRITE_REE1_HP_AXI_PDMA    (BIT(6))
#define HP_PERI0_PMS_WRITE_REE1_HP_AXI_PDMA_M  (HP_PERI0_PMS_WRITE_REE1_HP_AXI_PDMA_V << HP_PERI0_PMS_WRITE_REE1_HP_AXI_PDMA_S)
#define HP_PERI0_PMS_WRITE_REE1_HP_AXI_PDMA_V  0x00000001U
#define HP_PERI0_PMS_WRITE_REE1_HP_AXI_PDMA_S  6
/** HP_PERI0_PMS_WRITE_REE2_HP_AXI_PDMA : R/W; bitpos: [7]; default: 0;
 *  Configures hp_axi_pdma registers write permission in ree2 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI0_PMS_WRITE_REE2_HP_AXI_PDMA    (BIT(7))
#define HP_PERI0_PMS_WRITE_REE2_HP_AXI_PDMA_M  (HP_PERI0_PMS_WRITE_REE2_HP_AXI_PDMA_V << HP_PERI0_PMS_WRITE_REE2_HP_AXI_PDMA_S)
#define HP_PERI0_PMS_WRITE_REE2_HP_AXI_PDMA_V  0x00000001U
#define HP_PERI0_PMS_WRITE_REE2_HP_AXI_PDMA_S  7
/** HP_PERI0_PMS_HP_AXI_PDMA_LOCK : R/W; bitpos: [8]; default: 0;
 *  Set 1 to lock hp_axi_pdma peri_apm configuration
 */
#define HP_PERI0_PMS_HP_AXI_PDMA_LOCK    (BIT(8))
#define HP_PERI0_PMS_HP_AXI_PDMA_LOCK_M  (HP_PERI0_PMS_HP_AXI_PDMA_LOCK_V << HP_PERI0_PMS_HP_AXI_PDMA_LOCK_S)
#define HP_PERI0_PMS_HP_AXI_PDMA_LOCK_V  0x00000001U
#define HP_PERI0_PMS_HP_AXI_PDMA_LOCK_S  8

/** HP_PERI0_PMS_HP_GMAC_CTRL_REG register
 *  hp_gmac read/write control register
 */
#define HP_PERI0_PMS_HP_GMAC_CTRL_REG (DR_REG_HP_PERI0_PMS_BASE + 0x38)
/** HP_PERI0_PMS_READ_TEE_HP_GMAC : R/W; bitpos: [0]; default: 1;
 *  Configures hp_gmac registers read permission in tee mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI0_PMS_READ_TEE_HP_GMAC    (BIT(0))
#define HP_PERI0_PMS_READ_TEE_HP_GMAC_M  (HP_PERI0_PMS_READ_TEE_HP_GMAC_V << HP_PERI0_PMS_READ_TEE_HP_GMAC_S)
#define HP_PERI0_PMS_READ_TEE_HP_GMAC_V  0x00000001U
#define HP_PERI0_PMS_READ_TEE_HP_GMAC_S  0
/** HP_PERI0_PMS_READ_REE0_HP_GMAC : R/W; bitpos: [1]; default: 0;
 *  Configures hp_gmac registers read permission in ree0 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI0_PMS_READ_REE0_HP_GMAC    (BIT(1))
#define HP_PERI0_PMS_READ_REE0_HP_GMAC_M  (HP_PERI0_PMS_READ_REE0_HP_GMAC_V << HP_PERI0_PMS_READ_REE0_HP_GMAC_S)
#define HP_PERI0_PMS_READ_REE0_HP_GMAC_V  0x00000001U
#define HP_PERI0_PMS_READ_REE0_HP_GMAC_S  1
/** HP_PERI0_PMS_READ_REE1_HP_GMAC : R/W; bitpos: [2]; default: 0;
 *  Configures hp_gmac registers read permission in ree1 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI0_PMS_READ_REE1_HP_GMAC    (BIT(2))
#define HP_PERI0_PMS_READ_REE1_HP_GMAC_M  (HP_PERI0_PMS_READ_REE1_HP_GMAC_V << HP_PERI0_PMS_READ_REE1_HP_GMAC_S)
#define HP_PERI0_PMS_READ_REE1_HP_GMAC_V  0x00000001U
#define HP_PERI0_PMS_READ_REE1_HP_GMAC_S  2
/** HP_PERI0_PMS_READ_REE2_HP_GMAC : R/W; bitpos: [3]; default: 0;
 *  Configures hp_gmac registers read permission in ree2 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI0_PMS_READ_REE2_HP_GMAC    (BIT(3))
#define HP_PERI0_PMS_READ_REE2_HP_GMAC_M  (HP_PERI0_PMS_READ_REE2_HP_GMAC_V << HP_PERI0_PMS_READ_REE2_HP_GMAC_S)
#define HP_PERI0_PMS_READ_REE2_HP_GMAC_V  0x00000001U
#define HP_PERI0_PMS_READ_REE2_HP_GMAC_S  3
/** HP_PERI0_PMS_WRITE_TEE_HP_GMAC : R/W; bitpos: [4]; default: 1;
 *  Configures hp_gmac registers write permission in tee mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI0_PMS_WRITE_TEE_HP_GMAC    (BIT(4))
#define HP_PERI0_PMS_WRITE_TEE_HP_GMAC_M  (HP_PERI0_PMS_WRITE_TEE_HP_GMAC_V << HP_PERI0_PMS_WRITE_TEE_HP_GMAC_S)
#define HP_PERI0_PMS_WRITE_TEE_HP_GMAC_V  0x00000001U
#define HP_PERI0_PMS_WRITE_TEE_HP_GMAC_S  4
/** HP_PERI0_PMS_WRITE_REE0_HP_GMAC : R/W; bitpos: [5]; default: 0;
 *  Configures hp_gmac registers write permission in ree0 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI0_PMS_WRITE_REE0_HP_GMAC    (BIT(5))
#define HP_PERI0_PMS_WRITE_REE0_HP_GMAC_M  (HP_PERI0_PMS_WRITE_REE0_HP_GMAC_V << HP_PERI0_PMS_WRITE_REE0_HP_GMAC_S)
#define HP_PERI0_PMS_WRITE_REE0_HP_GMAC_V  0x00000001U
#define HP_PERI0_PMS_WRITE_REE0_HP_GMAC_S  5
/** HP_PERI0_PMS_WRITE_REE1_HP_GMAC : R/W; bitpos: [6]; default: 0;
 *  Configures hp_gmac registers write permission in ree1 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI0_PMS_WRITE_REE1_HP_GMAC    (BIT(6))
#define HP_PERI0_PMS_WRITE_REE1_HP_GMAC_M  (HP_PERI0_PMS_WRITE_REE1_HP_GMAC_V << HP_PERI0_PMS_WRITE_REE1_HP_GMAC_S)
#define HP_PERI0_PMS_WRITE_REE1_HP_GMAC_V  0x00000001U
#define HP_PERI0_PMS_WRITE_REE1_HP_GMAC_S  6
/** HP_PERI0_PMS_WRITE_REE2_HP_GMAC : R/W; bitpos: [7]; default: 0;
 *  Configures hp_gmac registers write permission in ree2 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI0_PMS_WRITE_REE2_HP_GMAC    (BIT(7))
#define HP_PERI0_PMS_WRITE_REE2_HP_GMAC_M  (HP_PERI0_PMS_WRITE_REE2_HP_GMAC_V << HP_PERI0_PMS_WRITE_REE2_HP_GMAC_S)
#define HP_PERI0_PMS_WRITE_REE2_HP_GMAC_V  0x00000001U
#define HP_PERI0_PMS_WRITE_REE2_HP_GMAC_S  7
/** HP_PERI0_PMS_HP_GMAC_LOCK : R/W; bitpos: [8]; default: 0;
 *  Set 1 to lock hp_gmac peri_apm configuration
 */
#define HP_PERI0_PMS_HP_GMAC_LOCK    (BIT(8))
#define HP_PERI0_PMS_HP_GMAC_LOCK_M  (HP_PERI0_PMS_HP_GMAC_LOCK_V << HP_PERI0_PMS_HP_GMAC_LOCK_S)
#define HP_PERI0_PMS_HP_GMAC_LOCK_V  0x00000001U
#define HP_PERI0_PMS_HP_GMAC_LOCK_S  8

/** HP_PERI0_PMS_HP_PVT0_CTRL_REG register
 *  hp_pvt0 read/write control register
 */
#define HP_PERI0_PMS_HP_PVT0_CTRL_REG (DR_REG_HP_PERI0_PMS_BASE + 0x3c)
/** HP_PERI0_PMS_READ_TEE_HP_PVT0 : R/W; bitpos: [0]; default: 1;
 *  Configures hp_pvt0 registers read permission in tee mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI0_PMS_READ_TEE_HP_PVT0    (BIT(0))
#define HP_PERI0_PMS_READ_TEE_HP_PVT0_M  (HP_PERI0_PMS_READ_TEE_HP_PVT0_V << HP_PERI0_PMS_READ_TEE_HP_PVT0_S)
#define HP_PERI0_PMS_READ_TEE_HP_PVT0_V  0x00000001U
#define HP_PERI0_PMS_READ_TEE_HP_PVT0_S  0
/** HP_PERI0_PMS_READ_REE0_HP_PVT0 : R/W; bitpos: [1]; default: 0;
 *  Configures hp_pvt0 registers read permission in ree0 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI0_PMS_READ_REE0_HP_PVT0    (BIT(1))
#define HP_PERI0_PMS_READ_REE0_HP_PVT0_M  (HP_PERI0_PMS_READ_REE0_HP_PVT0_V << HP_PERI0_PMS_READ_REE0_HP_PVT0_S)
#define HP_PERI0_PMS_READ_REE0_HP_PVT0_V  0x00000001U
#define HP_PERI0_PMS_READ_REE0_HP_PVT0_S  1
/** HP_PERI0_PMS_READ_REE1_HP_PVT0 : R/W; bitpos: [2]; default: 0;
 *  Configures hp_pvt0 registers read permission in ree1 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI0_PMS_READ_REE1_HP_PVT0    (BIT(2))
#define HP_PERI0_PMS_READ_REE1_HP_PVT0_M  (HP_PERI0_PMS_READ_REE1_HP_PVT0_V << HP_PERI0_PMS_READ_REE1_HP_PVT0_S)
#define HP_PERI0_PMS_READ_REE1_HP_PVT0_V  0x00000001U
#define HP_PERI0_PMS_READ_REE1_HP_PVT0_S  2
/** HP_PERI0_PMS_READ_REE2_HP_PVT0 : R/W; bitpos: [3]; default: 0;
 *  Configures hp_pvt0 registers read permission in ree2 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI0_PMS_READ_REE2_HP_PVT0    (BIT(3))
#define HP_PERI0_PMS_READ_REE2_HP_PVT0_M  (HP_PERI0_PMS_READ_REE2_HP_PVT0_V << HP_PERI0_PMS_READ_REE2_HP_PVT0_S)
#define HP_PERI0_PMS_READ_REE2_HP_PVT0_V  0x00000001U
#define HP_PERI0_PMS_READ_REE2_HP_PVT0_S  3
/** HP_PERI0_PMS_WRITE_TEE_HP_PVT0 : R/W; bitpos: [4]; default: 1;
 *  Configures hp_pvt0 registers write permission in tee mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI0_PMS_WRITE_TEE_HP_PVT0    (BIT(4))
#define HP_PERI0_PMS_WRITE_TEE_HP_PVT0_M  (HP_PERI0_PMS_WRITE_TEE_HP_PVT0_V << HP_PERI0_PMS_WRITE_TEE_HP_PVT0_S)
#define HP_PERI0_PMS_WRITE_TEE_HP_PVT0_V  0x00000001U
#define HP_PERI0_PMS_WRITE_TEE_HP_PVT0_S  4
/** HP_PERI0_PMS_WRITE_REE0_HP_PVT0 : R/W; bitpos: [5]; default: 0;
 *  Configures hp_pvt0 registers write permission in ree0 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI0_PMS_WRITE_REE0_HP_PVT0    (BIT(5))
#define HP_PERI0_PMS_WRITE_REE0_HP_PVT0_M  (HP_PERI0_PMS_WRITE_REE0_HP_PVT0_V << HP_PERI0_PMS_WRITE_REE0_HP_PVT0_S)
#define HP_PERI0_PMS_WRITE_REE0_HP_PVT0_V  0x00000001U
#define HP_PERI0_PMS_WRITE_REE0_HP_PVT0_S  5
/** HP_PERI0_PMS_WRITE_REE1_HP_PVT0 : R/W; bitpos: [6]; default: 0;
 *  Configures hp_pvt0 registers write permission in ree1 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI0_PMS_WRITE_REE1_HP_PVT0    (BIT(6))
#define HP_PERI0_PMS_WRITE_REE1_HP_PVT0_M  (HP_PERI0_PMS_WRITE_REE1_HP_PVT0_V << HP_PERI0_PMS_WRITE_REE1_HP_PVT0_S)
#define HP_PERI0_PMS_WRITE_REE1_HP_PVT0_V  0x00000001U
#define HP_PERI0_PMS_WRITE_REE1_HP_PVT0_S  6
/** HP_PERI0_PMS_WRITE_REE2_HP_PVT0 : R/W; bitpos: [7]; default: 0;
 *  Configures hp_pvt0 registers write permission in ree2 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI0_PMS_WRITE_REE2_HP_PVT0    (BIT(7))
#define HP_PERI0_PMS_WRITE_REE2_HP_PVT0_M  (HP_PERI0_PMS_WRITE_REE2_HP_PVT0_V << HP_PERI0_PMS_WRITE_REE2_HP_PVT0_S)
#define HP_PERI0_PMS_WRITE_REE2_HP_PVT0_V  0x00000001U
#define HP_PERI0_PMS_WRITE_REE2_HP_PVT0_S  7
/** HP_PERI0_PMS_HP_PVT0_LOCK : R/W; bitpos: [8]; default: 0;
 *  Set 1 to lock hp_pvt0 peri_apm configuration
 */
#define HP_PERI0_PMS_HP_PVT0_LOCK    (BIT(8))
#define HP_PERI0_PMS_HP_PVT0_LOCK_M  (HP_PERI0_PMS_HP_PVT0_LOCK_V << HP_PERI0_PMS_HP_PVT0_LOCK_S)
#define HP_PERI0_PMS_HP_PVT0_LOCK_V  0x00000001U
#define HP_PERI0_PMS_HP_PVT0_LOCK_S  8

/** HP_PERI0_PMS_HP_RMT_CTRL_REG register
 *  hp_rmt read/write control register
 */
#define HP_PERI0_PMS_HP_RMT_CTRL_REG (DR_REG_HP_PERI0_PMS_BASE + 0x40)
/** HP_PERI0_PMS_READ_TEE_HP_RMT : R/W; bitpos: [0]; default: 1;
 *  Configures hp_rmt registers read permission in tee mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI0_PMS_READ_TEE_HP_RMT    (BIT(0))
#define HP_PERI0_PMS_READ_TEE_HP_RMT_M  (HP_PERI0_PMS_READ_TEE_HP_RMT_V << HP_PERI0_PMS_READ_TEE_HP_RMT_S)
#define HP_PERI0_PMS_READ_TEE_HP_RMT_V  0x00000001U
#define HP_PERI0_PMS_READ_TEE_HP_RMT_S  0
/** HP_PERI0_PMS_READ_REE0_HP_RMT : R/W; bitpos: [1]; default: 0;
 *  Configures hp_rmt registers read permission in ree0 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI0_PMS_READ_REE0_HP_RMT    (BIT(1))
#define HP_PERI0_PMS_READ_REE0_HP_RMT_M  (HP_PERI0_PMS_READ_REE0_HP_RMT_V << HP_PERI0_PMS_READ_REE0_HP_RMT_S)
#define HP_PERI0_PMS_READ_REE0_HP_RMT_V  0x00000001U
#define HP_PERI0_PMS_READ_REE0_HP_RMT_S  1
/** HP_PERI0_PMS_READ_REE1_HP_RMT : R/W; bitpos: [2]; default: 0;
 *  Configures hp_rmt registers read permission in ree1 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI0_PMS_READ_REE1_HP_RMT    (BIT(2))
#define HP_PERI0_PMS_READ_REE1_HP_RMT_M  (HP_PERI0_PMS_READ_REE1_HP_RMT_V << HP_PERI0_PMS_READ_REE1_HP_RMT_S)
#define HP_PERI0_PMS_READ_REE1_HP_RMT_V  0x00000001U
#define HP_PERI0_PMS_READ_REE1_HP_RMT_S  2
/** HP_PERI0_PMS_READ_REE2_HP_RMT : R/W; bitpos: [3]; default: 0;
 *  Configures hp_rmt registers read permission in ree2 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI0_PMS_READ_REE2_HP_RMT    (BIT(3))
#define HP_PERI0_PMS_READ_REE2_HP_RMT_M  (HP_PERI0_PMS_READ_REE2_HP_RMT_V << HP_PERI0_PMS_READ_REE2_HP_RMT_S)
#define HP_PERI0_PMS_READ_REE2_HP_RMT_V  0x00000001U
#define HP_PERI0_PMS_READ_REE2_HP_RMT_S  3
/** HP_PERI0_PMS_WRITE_TEE_HP_RMT : R/W; bitpos: [4]; default: 1;
 *  Configures hp_rmt registers write permission in tee mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI0_PMS_WRITE_TEE_HP_RMT    (BIT(4))
#define HP_PERI0_PMS_WRITE_TEE_HP_RMT_M  (HP_PERI0_PMS_WRITE_TEE_HP_RMT_V << HP_PERI0_PMS_WRITE_TEE_HP_RMT_S)
#define HP_PERI0_PMS_WRITE_TEE_HP_RMT_V  0x00000001U
#define HP_PERI0_PMS_WRITE_TEE_HP_RMT_S  4
/** HP_PERI0_PMS_WRITE_REE0_HP_RMT : R/W; bitpos: [5]; default: 0;
 *  Configures hp_rmt registers write permission in ree0 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI0_PMS_WRITE_REE0_HP_RMT    (BIT(5))
#define HP_PERI0_PMS_WRITE_REE0_HP_RMT_M  (HP_PERI0_PMS_WRITE_REE0_HP_RMT_V << HP_PERI0_PMS_WRITE_REE0_HP_RMT_S)
#define HP_PERI0_PMS_WRITE_REE0_HP_RMT_V  0x00000001U
#define HP_PERI0_PMS_WRITE_REE0_HP_RMT_S  5
/** HP_PERI0_PMS_WRITE_REE1_HP_RMT : R/W; bitpos: [6]; default: 0;
 *  Configures hp_rmt registers write permission in ree1 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI0_PMS_WRITE_REE1_HP_RMT    (BIT(6))
#define HP_PERI0_PMS_WRITE_REE1_HP_RMT_M  (HP_PERI0_PMS_WRITE_REE1_HP_RMT_V << HP_PERI0_PMS_WRITE_REE1_HP_RMT_S)
#define HP_PERI0_PMS_WRITE_REE1_HP_RMT_V  0x00000001U
#define HP_PERI0_PMS_WRITE_REE1_HP_RMT_S  6
/** HP_PERI0_PMS_WRITE_REE2_HP_RMT : R/W; bitpos: [7]; default: 0;
 *  Configures hp_rmt registers write permission in ree2 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI0_PMS_WRITE_REE2_HP_RMT    (BIT(7))
#define HP_PERI0_PMS_WRITE_REE2_HP_RMT_M  (HP_PERI0_PMS_WRITE_REE2_HP_RMT_V << HP_PERI0_PMS_WRITE_REE2_HP_RMT_S)
#define HP_PERI0_PMS_WRITE_REE2_HP_RMT_V  0x00000001U
#define HP_PERI0_PMS_WRITE_REE2_HP_RMT_S  7
/** HP_PERI0_PMS_HP_RMT_LOCK : R/W; bitpos: [8]; default: 0;
 *  Set 1 to lock hp_rmt peri_apm configuration
 */
#define HP_PERI0_PMS_HP_RMT_LOCK    (BIT(8))
#define HP_PERI0_PMS_HP_RMT_LOCK_M  (HP_PERI0_PMS_HP_RMT_LOCK_V << HP_PERI0_PMS_HP_RMT_LOCK_S)
#define HP_PERI0_PMS_HP_RMT_LOCK_V  0x00000001U
#define HP_PERI0_PMS_HP_RMT_LOCK_S  8

/** HP_PERI0_PMS_HP_BITSCRAMBLER_CTRL_REG register
 *  hp_bitscrambler read/write control register
 */
#define HP_PERI0_PMS_HP_BITSCRAMBLER_CTRL_REG (DR_REG_HP_PERI0_PMS_BASE + 0x44)
/** HP_PERI0_PMS_READ_TEE_HP_BITSCRAMBLER : R/W; bitpos: [0]; default: 1;
 *  Configures hp_bitscrambler registers read permission in tee mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI0_PMS_READ_TEE_HP_BITSCRAMBLER    (BIT(0))
#define HP_PERI0_PMS_READ_TEE_HP_BITSCRAMBLER_M  (HP_PERI0_PMS_READ_TEE_HP_BITSCRAMBLER_V << HP_PERI0_PMS_READ_TEE_HP_BITSCRAMBLER_S)
#define HP_PERI0_PMS_READ_TEE_HP_BITSCRAMBLER_V  0x00000001U
#define HP_PERI0_PMS_READ_TEE_HP_BITSCRAMBLER_S  0
/** HP_PERI0_PMS_READ_REE0_HP_BITSCRAMBLER : R/W; bitpos: [1]; default: 0;
 *  Configures hp_bitscrambler registers read permission in ree0 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI0_PMS_READ_REE0_HP_BITSCRAMBLER    (BIT(1))
#define HP_PERI0_PMS_READ_REE0_HP_BITSCRAMBLER_M  (HP_PERI0_PMS_READ_REE0_HP_BITSCRAMBLER_V << HP_PERI0_PMS_READ_REE0_HP_BITSCRAMBLER_S)
#define HP_PERI0_PMS_READ_REE0_HP_BITSCRAMBLER_V  0x00000001U
#define HP_PERI0_PMS_READ_REE0_HP_BITSCRAMBLER_S  1
/** HP_PERI0_PMS_READ_REE1_HP_BITSCRAMBLER : R/W; bitpos: [2]; default: 0;
 *  Configures hp_bitscrambler registers read permission in ree1 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI0_PMS_READ_REE1_HP_BITSCRAMBLER    (BIT(2))
#define HP_PERI0_PMS_READ_REE1_HP_BITSCRAMBLER_M  (HP_PERI0_PMS_READ_REE1_HP_BITSCRAMBLER_V << HP_PERI0_PMS_READ_REE1_HP_BITSCRAMBLER_S)
#define HP_PERI0_PMS_READ_REE1_HP_BITSCRAMBLER_V  0x00000001U
#define HP_PERI0_PMS_READ_REE1_HP_BITSCRAMBLER_S  2
/** HP_PERI0_PMS_READ_REE2_HP_BITSCRAMBLER : R/W; bitpos: [3]; default: 0;
 *  Configures hp_bitscrambler registers read permission in ree2 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI0_PMS_READ_REE2_HP_BITSCRAMBLER    (BIT(3))
#define HP_PERI0_PMS_READ_REE2_HP_BITSCRAMBLER_M  (HP_PERI0_PMS_READ_REE2_HP_BITSCRAMBLER_V << HP_PERI0_PMS_READ_REE2_HP_BITSCRAMBLER_S)
#define HP_PERI0_PMS_READ_REE2_HP_BITSCRAMBLER_V  0x00000001U
#define HP_PERI0_PMS_READ_REE2_HP_BITSCRAMBLER_S  3
/** HP_PERI0_PMS_WRITE_TEE_HP_BITSCRAMBLER : R/W; bitpos: [4]; default: 1;
 *  Configures hp_bitscrambler registers write permission in tee mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI0_PMS_WRITE_TEE_HP_BITSCRAMBLER    (BIT(4))
#define HP_PERI0_PMS_WRITE_TEE_HP_BITSCRAMBLER_M  (HP_PERI0_PMS_WRITE_TEE_HP_BITSCRAMBLER_V << HP_PERI0_PMS_WRITE_TEE_HP_BITSCRAMBLER_S)
#define HP_PERI0_PMS_WRITE_TEE_HP_BITSCRAMBLER_V  0x00000001U
#define HP_PERI0_PMS_WRITE_TEE_HP_BITSCRAMBLER_S  4
/** HP_PERI0_PMS_WRITE_REE0_HP_BITSCRAMBLER : R/W; bitpos: [5]; default: 0;
 *  Configures hp_bitscrambler registers write permission in ree0 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI0_PMS_WRITE_REE0_HP_BITSCRAMBLER    (BIT(5))
#define HP_PERI0_PMS_WRITE_REE0_HP_BITSCRAMBLER_M  (HP_PERI0_PMS_WRITE_REE0_HP_BITSCRAMBLER_V << HP_PERI0_PMS_WRITE_REE0_HP_BITSCRAMBLER_S)
#define HP_PERI0_PMS_WRITE_REE0_HP_BITSCRAMBLER_V  0x00000001U
#define HP_PERI0_PMS_WRITE_REE0_HP_BITSCRAMBLER_S  5
/** HP_PERI0_PMS_WRITE_REE1_HP_BITSCRAMBLER : R/W; bitpos: [6]; default: 0;
 *  Configures hp_bitscrambler registers write permission in ree1 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI0_PMS_WRITE_REE1_HP_BITSCRAMBLER    (BIT(6))
#define HP_PERI0_PMS_WRITE_REE1_HP_BITSCRAMBLER_M  (HP_PERI0_PMS_WRITE_REE1_HP_BITSCRAMBLER_V << HP_PERI0_PMS_WRITE_REE1_HP_BITSCRAMBLER_S)
#define HP_PERI0_PMS_WRITE_REE1_HP_BITSCRAMBLER_V  0x00000001U
#define HP_PERI0_PMS_WRITE_REE1_HP_BITSCRAMBLER_S  6
/** HP_PERI0_PMS_WRITE_REE2_HP_BITSCRAMBLER : R/W; bitpos: [7]; default: 0;
 *  Configures hp_bitscrambler registers write permission in ree2 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI0_PMS_WRITE_REE2_HP_BITSCRAMBLER    (BIT(7))
#define HP_PERI0_PMS_WRITE_REE2_HP_BITSCRAMBLER_M  (HP_PERI0_PMS_WRITE_REE2_HP_BITSCRAMBLER_V << HP_PERI0_PMS_WRITE_REE2_HP_BITSCRAMBLER_S)
#define HP_PERI0_PMS_WRITE_REE2_HP_BITSCRAMBLER_V  0x00000001U
#define HP_PERI0_PMS_WRITE_REE2_HP_BITSCRAMBLER_S  7
/** HP_PERI0_PMS_HP_BITSCRAMBLER_LOCK : R/W; bitpos: [8]; default: 0;
 *  Set 1 to lock hp_bitscrambler peri_apm configuration
 */
#define HP_PERI0_PMS_HP_BITSCRAMBLER_LOCK    (BIT(8))
#define HP_PERI0_PMS_HP_BITSCRAMBLER_LOCK_M  (HP_PERI0_PMS_HP_BITSCRAMBLER_LOCK_V << HP_PERI0_PMS_HP_BITSCRAMBLER_LOCK_S)
#define HP_PERI0_PMS_HP_BITSCRAMBLER_LOCK_V  0x00000001U
#define HP_PERI0_PMS_HP_BITSCRAMBLER_LOCK_S  8

/** HP_PERI0_PMS_HP_ASRC_CTRL_REG register
 *  hp_asrc read/write control register
 */
#define HP_PERI0_PMS_HP_ASRC_CTRL_REG (DR_REG_HP_PERI0_PMS_BASE + 0x48)
/** HP_PERI0_PMS_READ_TEE_HP_ASRC : R/W; bitpos: [0]; default: 1;
 *  Configures hp_asrc registers read permission in tee mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI0_PMS_READ_TEE_HP_ASRC    (BIT(0))
#define HP_PERI0_PMS_READ_TEE_HP_ASRC_M  (HP_PERI0_PMS_READ_TEE_HP_ASRC_V << HP_PERI0_PMS_READ_TEE_HP_ASRC_S)
#define HP_PERI0_PMS_READ_TEE_HP_ASRC_V  0x00000001U
#define HP_PERI0_PMS_READ_TEE_HP_ASRC_S  0
/** HP_PERI0_PMS_READ_REE0_HP_ASRC : R/W; bitpos: [1]; default: 0;
 *  Configures hp_asrc registers read permission in ree0 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI0_PMS_READ_REE0_HP_ASRC    (BIT(1))
#define HP_PERI0_PMS_READ_REE0_HP_ASRC_M  (HP_PERI0_PMS_READ_REE0_HP_ASRC_V << HP_PERI0_PMS_READ_REE0_HP_ASRC_S)
#define HP_PERI0_PMS_READ_REE0_HP_ASRC_V  0x00000001U
#define HP_PERI0_PMS_READ_REE0_HP_ASRC_S  1
/** HP_PERI0_PMS_READ_REE1_HP_ASRC : R/W; bitpos: [2]; default: 0;
 *  Configures hp_asrc registers read permission in ree1 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI0_PMS_READ_REE1_HP_ASRC    (BIT(2))
#define HP_PERI0_PMS_READ_REE1_HP_ASRC_M  (HP_PERI0_PMS_READ_REE1_HP_ASRC_V << HP_PERI0_PMS_READ_REE1_HP_ASRC_S)
#define HP_PERI0_PMS_READ_REE1_HP_ASRC_V  0x00000001U
#define HP_PERI0_PMS_READ_REE1_HP_ASRC_S  2
/** HP_PERI0_PMS_READ_REE2_HP_ASRC : R/W; bitpos: [3]; default: 0;
 *  Configures hp_asrc registers read permission in ree2 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI0_PMS_READ_REE2_HP_ASRC    (BIT(3))
#define HP_PERI0_PMS_READ_REE2_HP_ASRC_M  (HP_PERI0_PMS_READ_REE2_HP_ASRC_V << HP_PERI0_PMS_READ_REE2_HP_ASRC_S)
#define HP_PERI0_PMS_READ_REE2_HP_ASRC_V  0x00000001U
#define HP_PERI0_PMS_READ_REE2_HP_ASRC_S  3
/** HP_PERI0_PMS_WRITE_TEE_HP_ASRC : R/W; bitpos: [4]; default: 1;
 *  Configures hp_asrc registers write permission in tee mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI0_PMS_WRITE_TEE_HP_ASRC    (BIT(4))
#define HP_PERI0_PMS_WRITE_TEE_HP_ASRC_M  (HP_PERI0_PMS_WRITE_TEE_HP_ASRC_V << HP_PERI0_PMS_WRITE_TEE_HP_ASRC_S)
#define HP_PERI0_PMS_WRITE_TEE_HP_ASRC_V  0x00000001U
#define HP_PERI0_PMS_WRITE_TEE_HP_ASRC_S  4
/** HP_PERI0_PMS_WRITE_REE0_HP_ASRC : R/W; bitpos: [5]; default: 0;
 *  Configures hp_asrc registers write permission in ree0 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI0_PMS_WRITE_REE0_HP_ASRC    (BIT(5))
#define HP_PERI0_PMS_WRITE_REE0_HP_ASRC_M  (HP_PERI0_PMS_WRITE_REE0_HP_ASRC_V << HP_PERI0_PMS_WRITE_REE0_HP_ASRC_S)
#define HP_PERI0_PMS_WRITE_REE0_HP_ASRC_V  0x00000001U
#define HP_PERI0_PMS_WRITE_REE0_HP_ASRC_S  5
/** HP_PERI0_PMS_WRITE_REE1_HP_ASRC : R/W; bitpos: [6]; default: 0;
 *  Configures hp_asrc registers write permission in ree1 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI0_PMS_WRITE_REE1_HP_ASRC    (BIT(6))
#define HP_PERI0_PMS_WRITE_REE1_HP_ASRC_M  (HP_PERI0_PMS_WRITE_REE1_HP_ASRC_V << HP_PERI0_PMS_WRITE_REE1_HP_ASRC_S)
#define HP_PERI0_PMS_WRITE_REE1_HP_ASRC_V  0x00000001U
#define HP_PERI0_PMS_WRITE_REE1_HP_ASRC_S  6
/** HP_PERI0_PMS_WRITE_REE2_HP_ASRC : R/W; bitpos: [7]; default: 0;
 *  Configures hp_asrc registers write permission in ree2 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI0_PMS_WRITE_REE2_HP_ASRC    (BIT(7))
#define HP_PERI0_PMS_WRITE_REE2_HP_ASRC_M  (HP_PERI0_PMS_WRITE_REE2_HP_ASRC_V << HP_PERI0_PMS_WRITE_REE2_HP_ASRC_S)
#define HP_PERI0_PMS_WRITE_REE2_HP_ASRC_V  0x00000001U
#define HP_PERI0_PMS_WRITE_REE2_HP_ASRC_S  7
/** HP_PERI0_PMS_HP_ASRC_LOCK : R/W; bitpos: [8]; default: 0;
 *  Set 1 to lock hp_asrc peri_apm configuration
 */
#define HP_PERI0_PMS_HP_ASRC_LOCK    (BIT(8))
#define HP_PERI0_PMS_HP_ASRC_LOCK_M  (HP_PERI0_PMS_HP_ASRC_LOCK_V << HP_PERI0_PMS_HP_ASRC_LOCK_S)
#define HP_PERI0_PMS_HP_ASRC_LOCK_V  0x00000001U
#define HP_PERI0_PMS_HP_ASRC_LOCK_S  8

/** HP_PERI0_PMS_CNNT_SYS_REG_CTRL_REG register
 *  cnnt_sys_reg read/write control register
 */
#define HP_PERI0_PMS_CNNT_SYS_REG_CTRL_REG (DR_REG_HP_PERI0_PMS_BASE + 0x4c)
/** HP_PERI0_PMS_READ_TEE_CNNT_SYS_REG : R/W; bitpos: [0]; default: 1;
 *  Configures cnnt_sys_reg registers read permission in tee mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI0_PMS_READ_TEE_CNNT_SYS_REG    (BIT(0))
#define HP_PERI0_PMS_READ_TEE_CNNT_SYS_REG_M  (HP_PERI0_PMS_READ_TEE_CNNT_SYS_REG_V << HP_PERI0_PMS_READ_TEE_CNNT_SYS_REG_S)
#define HP_PERI0_PMS_READ_TEE_CNNT_SYS_REG_V  0x00000001U
#define HP_PERI0_PMS_READ_TEE_CNNT_SYS_REG_S  0
/** HP_PERI0_PMS_READ_REE0_CNNT_SYS_REG : R/W; bitpos: [1]; default: 0;
 *  Configures cnnt_sys_reg registers read permission in ree0 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI0_PMS_READ_REE0_CNNT_SYS_REG    (BIT(1))
#define HP_PERI0_PMS_READ_REE0_CNNT_SYS_REG_M  (HP_PERI0_PMS_READ_REE0_CNNT_SYS_REG_V << HP_PERI0_PMS_READ_REE0_CNNT_SYS_REG_S)
#define HP_PERI0_PMS_READ_REE0_CNNT_SYS_REG_V  0x00000001U
#define HP_PERI0_PMS_READ_REE0_CNNT_SYS_REG_S  1
/** HP_PERI0_PMS_READ_REE1_CNNT_SYS_REG : R/W; bitpos: [2]; default: 0;
 *  Configures cnnt_sys_reg registers read permission in ree1 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI0_PMS_READ_REE1_CNNT_SYS_REG    (BIT(2))
#define HP_PERI0_PMS_READ_REE1_CNNT_SYS_REG_M  (HP_PERI0_PMS_READ_REE1_CNNT_SYS_REG_V << HP_PERI0_PMS_READ_REE1_CNNT_SYS_REG_S)
#define HP_PERI0_PMS_READ_REE1_CNNT_SYS_REG_V  0x00000001U
#define HP_PERI0_PMS_READ_REE1_CNNT_SYS_REG_S  2
/** HP_PERI0_PMS_READ_REE2_CNNT_SYS_REG : R/W; bitpos: [3]; default: 0;
 *  Configures cnnt_sys_reg registers read permission in ree2 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI0_PMS_READ_REE2_CNNT_SYS_REG    (BIT(3))
#define HP_PERI0_PMS_READ_REE2_CNNT_SYS_REG_M  (HP_PERI0_PMS_READ_REE2_CNNT_SYS_REG_V << HP_PERI0_PMS_READ_REE2_CNNT_SYS_REG_S)
#define HP_PERI0_PMS_READ_REE2_CNNT_SYS_REG_V  0x00000001U
#define HP_PERI0_PMS_READ_REE2_CNNT_SYS_REG_S  3
/** HP_PERI0_PMS_WRITE_TEE_CNNT_SYS_REG : R/W; bitpos: [4]; default: 1;
 *  Configures cnnt_sys_reg registers write permission in tee mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI0_PMS_WRITE_TEE_CNNT_SYS_REG    (BIT(4))
#define HP_PERI0_PMS_WRITE_TEE_CNNT_SYS_REG_M  (HP_PERI0_PMS_WRITE_TEE_CNNT_SYS_REG_V << HP_PERI0_PMS_WRITE_TEE_CNNT_SYS_REG_S)
#define HP_PERI0_PMS_WRITE_TEE_CNNT_SYS_REG_V  0x00000001U
#define HP_PERI0_PMS_WRITE_TEE_CNNT_SYS_REG_S  4
/** HP_PERI0_PMS_WRITE_REE0_CNNT_SYS_REG : R/W; bitpos: [5]; default: 0;
 *  Configures cnnt_sys_reg registers write permission in ree0 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI0_PMS_WRITE_REE0_CNNT_SYS_REG    (BIT(5))
#define HP_PERI0_PMS_WRITE_REE0_CNNT_SYS_REG_M  (HP_PERI0_PMS_WRITE_REE0_CNNT_SYS_REG_V << HP_PERI0_PMS_WRITE_REE0_CNNT_SYS_REG_S)
#define HP_PERI0_PMS_WRITE_REE0_CNNT_SYS_REG_V  0x00000001U
#define HP_PERI0_PMS_WRITE_REE0_CNNT_SYS_REG_S  5
/** HP_PERI0_PMS_WRITE_REE1_CNNT_SYS_REG : R/W; bitpos: [6]; default: 0;
 *  Configures cnnt_sys_reg registers write permission in ree1 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI0_PMS_WRITE_REE1_CNNT_SYS_REG    (BIT(6))
#define HP_PERI0_PMS_WRITE_REE1_CNNT_SYS_REG_M  (HP_PERI0_PMS_WRITE_REE1_CNNT_SYS_REG_V << HP_PERI0_PMS_WRITE_REE1_CNNT_SYS_REG_S)
#define HP_PERI0_PMS_WRITE_REE1_CNNT_SYS_REG_V  0x00000001U
#define HP_PERI0_PMS_WRITE_REE1_CNNT_SYS_REG_S  6
/** HP_PERI0_PMS_WRITE_REE2_CNNT_SYS_REG : R/W; bitpos: [7]; default: 0;
 *  Configures cnnt_sys_reg registers write permission in ree2 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI0_PMS_WRITE_REE2_CNNT_SYS_REG    (BIT(7))
#define HP_PERI0_PMS_WRITE_REE2_CNNT_SYS_REG_M  (HP_PERI0_PMS_WRITE_REE2_CNNT_SYS_REG_V << HP_PERI0_PMS_WRITE_REE2_CNNT_SYS_REG_S)
#define HP_PERI0_PMS_WRITE_REE2_CNNT_SYS_REG_V  0x00000001U
#define HP_PERI0_PMS_WRITE_REE2_CNNT_SYS_REG_S  7
/** HP_PERI0_PMS_CNNT_SYS_REG_LOCK : R/W; bitpos: [8]; default: 0;
 *  Set 1 to lock cnnt_sys_reg peri_apm configuration
 */
#define HP_PERI0_PMS_CNNT_SYS_REG_LOCK    (BIT(8))
#define HP_PERI0_PMS_CNNT_SYS_REG_LOCK_M  (HP_PERI0_PMS_CNNT_SYS_REG_LOCK_V << HP_PERI0_PMS_CNNT_SYS_REG_LOCK_S)
#define HP_PERI0_PMS_CNNT_SYS_REG_LOCK_V  0x00000001U
#define HP_PERI0_PMS_CNNT_SYS_REG_LOCK_S  8

/** HP_PERI0_PMS_HP_FLASH_CTRL_REG register
 *  hp_flash read/write control register
 */
#define HP_PERI0_PMS_HP_FLASH_CTRL_REG (DR_REG_HP_PERI0_PMS_BASE + 0x50)
/** HP_PERI0_PMS_READ_TEE_HP_FLASH : R/W; bitpos: [0]; default: 1;
 *  Configures hp_flash registers read permission in tee mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI0_PMS_READ_TEE_HP_FLASH    (BIT(0))
#define HP_PERI0_PMS_READ_TEE_HP_FLASH_M  (HP_PERI0_PMS_READ_TEE_HP_FLASH_V << HP_PERI0_PMS_READ_TEE_HP_FLASH_S)
#define HP_PERI0_PMS_READ_TEE_HP_FLASH_V  0x00000001U
#define HP_PERI0_PMS_READ_TEE_HP_FLASH_S  0
/** HP_PERI0_PMS_READ_REE0_HP_FLASH : R/W; bitpos: [1]; default: 0;
 *  Configures hp_flash registers read permission in ree0 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI0_PMS_READ_REE0_HP_FLASH    (BIT(1))
#define HP_PERI0_PMS_READ_REE0_HP_FLASH_M  (HP_PERI0_PMS_READ_REE0_HP_FLASH_V << HP_PERI0_PMS_READ_REE0_HP_FLASH_S)
#define HP_PERI0_PMS_READ_REE0_HP_FLASH_V  0x00000001U
#define HP_PERI0_PMS_READ_REE0_HP_FLASH_S  1
/** HP_PERI0_PMS_READ_REE1_HP_FLASH : R/W; bitpos: [2]; default: 0;
 *  Configures hp_flash registers read permission in ree1 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI0_PMS_READ_REE1_HP_FLASH    (BIT(2))
#define HP_PERI0_PMS_READ_REE1_HP_FLASH_M  (HP_PERI0_PMS_READ_REE1_HP_FLASH_V << HP_PERI0_PMS_READ_REE1_HP_FLASH_S)
#define HP_PERI0_PMS_READ_REE1_HP_FLASH_V  0x00000001U
#define HP_PERI0_PMS_READ_REE1_HP_FLASH_S  2
/** HP_PERI0_PMS_READ_REE2_HP_FLASH : R/W; bitpos: [3]; default: 0;
 *  Configures hp_flash registers read permission in ree2 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI0_PMS_READ_REE2_HP_FLASH    (BIT(3))
#define HP_PERI0_PMS_READ_REE2_HP_FLASH_M  (HP_PERI0_PMS_READ_REE2_HP_FLASH_V << HP_PERI0_PMS_READ_REE2_HP_FLASH_S)
#define HP_PERI0_PMS_READ_REE2_HP_FLASH_V  0x00000001U
#define HP_PERI0_PMS_READ_REE2_HP_FLASH_S  3
/** HP_PERI0_PMS_WRITE_TEE_HP_FLASH : R/W; bitpos: [4]; default: 1;
 *  Configures hp_flash registers write permission in tee mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI0_PMS_WRITE_TEE_HP_FLASH    (BIT(4))
#define HP_PERI0_PMS_WRITE_TEE_HP_FLASH_M  (HP_PERI0_PMS_WRITE_TEE_HP_FLASH_V << HP_PERI0_PMS_WRITE_TEE_HP_FLASH_S)
#define HP_PERI0_PMS_WRITE_TEE_HP_FLASH_V  0x00000001U
#define HP_PERI0_PMS_WRITE_TEE_HP_FLASH_S  4
/** HP_PERI0_PMS_WRITE_REE0_HP_FLASH : R/W; bitpos: [5]; default: 0;
 *  Configures hp_flash registers write permission in ree0 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI0_PMS_WRITE_REE0_HP_FLASH    (BIT(5))
#define HP_PERI0_PMS_WRITE_REE0_HP_FLASH_M  (HP_PERI0_PMS_WRITE_REE0_HP_FLASH_V << HP_PERI0_PMS_WRITE_REE0_HP_FLASH_S)
#define HP_PERI0_PMS_WRITE_REE0_HP_FLASH_V  0x00000001U
#define HP_PERI0_PMS_WRITE_REE0_HP_FLASH_S  5
/** HP_PERI0_PMS_WRITE_REE1_HP_FLASH : R/W; bitpos: [6]; default: 0;
 *  Configures hp_flash registers write permission in ree1 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI0_PMS_WRITE_REE1_HP_FLASH    (BIT(6))
#define HP_PERI0_PMS_WRITE_REE1_HP_FLASH_M  (HP_PERI0_PMS_WRITE_REE1_HP_FLASH_V << HP_PERI0_PMS_WRITE_REE1_HP_FLASH_S)
#define HP_PERI0_PMS_WRITE_REE1_HP_FLASH_V  0x00000001U
#define HP_PERI0_PMS_WRITE_REE1_HP_FLASH_S  6
/** HP_PERI0_PMS_WRITE_REE2_HP_FLASH : R/W; bitpos: [7]; default: 0;
 *  Configures hp_flash registers write permission in ree2 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI0_PMS_WRITE_REE2_HP_FLASH    (BIT(7))
#define HP_PERI0_PMS_WRITE_REE2_HP_FLASH_M  (HP_PERI0_PMS_WRITE_REE2_HP_FLASH_V << HP_PERI0_PMS_WRITE_REE2_HP_FLASH_S)
#define HP_PERI0_PMS_WRITE_REE2_HP_FLASH_V  0x00000001U
#define HP_PERI0_PMS_WRITE_REE2_HP_FLASH_S  7
/** HP_PERI0_PMS_HP_FLASH_LOCK : R/W; bitpos: [8]; default: 0;
 *  Set 1 to lock hp_flash peri_apm configuration
 */
#define HP_PERI0_PMS_HP_FLASH_LOCK    (BIT(8))
#define HP_PERI0_PMS_HP_FLASH_LOCK_M  (HP_PERI0_PMS_HP_FLASH_LOCK_V << HP_PERI0_PMS_HP_FLASH_LOCK_S)
#define HP_PERI0_PMS_HP_FLASH_LOCK_V  0x00000001U
#define HP_PERI0_PMS_HP_FLASH_LOCK_S  8

/** HP_PERI0_PMS_HP_PSRAM_CTRL_REG register
 *  hp_psram read/write control register
 */
#define HP_PERI0_PMS_HP_PSRAM_CTRL_REG (DR_REG_HP_PERI0_PMS_BASE + 0x54)
/** HP_PERI0_PMS_READ_TEE_HP_PSRAM : R/W; bitpos: [0]; default: 1;
 *  Configures hp_psram registers read permission in tee mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI0_PMS_READ_TEE_HP_PSRAM    (BIT(0))
#define HP_PERI0_PMS_READ_TEE_HP_PSRAM_M  (HP_PERI0_PMS_READ_TEE_HP_PSRAM_V << HP_PERI0_PMS_READ_TEE_HP_PSRAM_S)
#define HP_PERI0_PMS_READ_TEE_HP_PSRAM_V  0x00000001U
#define HP_PERI0_PMS_READ_TEE_HP_PSRAM_S  0
/** HP_PERI0_PMS_READ_REE0_HP_PSRAM : R/W; bitpos: [1]; default: 0;
 *  Configures hp_psram registers read permission in ree0 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI0_PMS_READ_REE0_HP_PSRAM    (BIT(1))
#define HP_PERI0_PMS_READ_REE0_HP_PSRAM_M  (HP_PERI0_PMS_READ_REE0_HP_PSRAM_V << HP_PERI0_PMS_READ_REE0_HP_PSRAM_S)
#define HP_PERI0_PMS_READ_REE0_HP_PSRAM_V  0x00000001U
#define HP_PERI0_PMS_READ_REE0_HP_PSRAM_S  1
/** HP_PERI0_PMS_READ_REE1_HP_PSRAM : R/W; bitpos: [2]; default: 0;
 *  Configures hp_psram registers read permission in ree1 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI0_PMS_READ_REE1_HP_PSRAM    (BIT(2))
#define HP_PERI0_PMS_READ_REE1_HP_PSRAM_M  (HP_PERI0_PMS_READ_REE1_HP_PSRAM_V << HP_PERI0_PMS_READ_REE1_HP_PSRAM_S)
#define HP_PERI0_PMS_READ_REE1_HP_PSRAM_V  0x00000001U
#define HP_PERI0_PMS_READ_REE1_HP_PSRAM_S  2
/** HP_PERI0_PMS_READ_REE2_HP_PSRAM : R/W; bitpos: [3]; default: 0;
 *  Configures hp_psram registers read permission in ree2 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI0_PMS_READ_REE2_HP_PSRAM    (BIT(3))
#define HP_PERI0_PMS_READ_REE2_HP_PSRAM_M  (HP_PERI0_PMS_READ_REE2_HP_PSRAM_V << HP_PERI0_PMS_READ_REE2_HP_PSRAM_S)
#define HP_PERI0_PMS_READ_REE2_HP_PSRAM_V  0x00000001U
#define HP_PERI0_PMS_READ_REE2_HP_PSRAM_S  3
/** HP_PERI0_PMS_WRITE_TEE_HP_PSRAM : R/W; bitpos: [4]; default: 1;
 *  Configures hp_psram registers write permission in tee mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI0_PMS_WRITE_TEE_HP_PSRAM    (BIT(4))
#define HP_PERI0_PMS_WRITE_TEE_HP_PSRAM_M  (HP_PERI0_PMS_WRITE_TEE_HP_PSRAM_V << HP_PERI0_PMS_WRITE_TEE_HP_PSRAM_S)
#define HP_PERI0_PMS_WRITE_TEE_HP_PSRAM_V  0x00000001U
#define HP_PERI0_PMS_WRITE_TEE_HP_PSRAM_S  4
/** HP_PERI0_PMS_WRITE_REE0_HP_PSRAM : R/W; bitpos: [5]; default: 0;
 *  Configures hp_psram registers write permission in ree0 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI0_PMS_WRITE_REE0_HP_PSRAM    (BIT(5))
#define HP_PERI0_PMS_WRITE_REE0_HP_PSRAM_M  (HP_PERI0_PMS_WRITE_REE0_HP_PSRAM_V << HP_PERI0_PMS_WRITE_REE0_HP_PSRAM_S)
#define HP_PERI0_PMS_WRITE_REE0_HP_PSRAM_V  0x00000001U
#define HP_PERI0_PMS_WRITE_REE0_HP_PSRAM_S  5
/** HP_PERI0_PMS_WRITE_REE1_HP_PSRAM : R/W; bitpos: [6]; default: 0;
 *  Configures hp_psram registers write permission in ree1 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI0_PMS_WRITE_REE1_HP_PSRAM    (BIT(6))
#define HP_PERI0_PMS_WRITE_REE1_HP_PSRAM_M  (HP_PERI0_PMS_WRITE_REE1_HP_PSRAM_V << HP_PERI0_PMS_WRITE_REE1_HP_PSRAM_S)
#define HP_PERI0_PMS_WRITE_REE1_HP_PSRAM_V  0x00000001U
#define HP_PERI0_PMS_WRITE_REE1_HP_PSRAM_S  6
/** HP_PERI0_PMS_WRITE_REE2_HP_PSRAM : R/W; bitpos: [7]; default: 0;
 *  Configures hp_psram registers write permission in ree2 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI0_PMS_WRITE_REE2_HP_PSRAM    (BIT(7))
#define HP_PERI0_PMS_WRITE_REE2_HP_PSRAM_M  (HP_PERI0_PMS_WRITE_REE2_HP_PSRAM_V << HP_PERI0_PMS_WRITE_REE2_HP_PSRAM_S)
#define HP_PERI0_PMS_WRITE_REE2_HP_PSRAM_V  0x00000001U
#define HP_PERI0_PMS_WRITE_REE2_HP_PSRAM_S  7
/** HP_PERI0_PMS_HP_PSRAM_LOCK : R/W; bitpos: [8]; default: 0;
 *  Set 1 to lock hp_psram peri_apm configuration
 */
#define HP_PERI0_PMS_HP_PSRAM_LOCK    (BIT(8))
#define HP_PERI0_PMS_HP_PSRAM_LOCK_M  (HP_PERI0_PMS_HP_PSRAM_LOCK_V << HP_PERI0_PMS_HP_PSRAM_LOCK_S)
#define HP_PERI0_PMS_HP_PSRAM_LOCK_V  0x00000001U
#define HP_PERI0_PMS_HP_PSRAM_LOCK_S  8

/** HP_PERI0_PMS_TEE_CTRL_REG register
 *  tee read/write control register
 */
#define HP_PERI0_PMS_TEE_CTRL_REG (DR_REG_HP_PERI0_PMS_BASE + 0x58)
/** HP_PERI0_PMS_READ_TEE_TEE : R/W; bitpos: [0]; default: 1;
 *  Configures tee registers read permission in tee mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI0_PMS_READ_TEE_TEE    (BIT(0))
#define HP_PERI0_PMS_READ_TEE_TEE_M  (HP_PERI0_PMS_READ_TEE_TEE_V << HP_PERI0_PMS_READ_TEE_TEE_S)
#define HP_PERI0_PMS_READ_TEE_TEE_V  0x00000001U
#define HP_PERI0_PMS_READ_TEE_TEE_S  0
/** HP_PERI0_PMS_READ_REE0_TEE : HRO; bitpos: [1]; default: 0;
 *  Configures tee registers read permission in ree0 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI0_PMS_READ_REE0_TEE    (BIT(1))
#define HP_PERI0_PMS_READ_REE0_TEE_M  (HP_PERI0_PMS_READ_REE0_TEE_V << HP_PERI0_PMS_READ_REE0_TEE_S)
#define HP_PERI0_PMS_READ_REE0_TEE_V  0x00000001U
#define HP_PERI0_PMS_READ_REE0_TEE_S  1
/** HP_PERI0_PMS_READ_REE1_TEE : HRO; bitpos: [2]; default: 0;
 *  Configures tee registers read permission in ree1 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI0_PMS_READ_REE1_TEE    (BIT(2))
#define HP_PERI0_PMS_READ_REE1_TEE_M  (HP_PERI0_PMS_READ_REE1_TEE_V << HP_PERI0_PMS_READ_REE1_TEE_S)
#define HP_PERI0_PMS_READ_REE1_TEE_V  0x00000001U
#define HP_PERI0_PMS_READ_REE1_TEE_S  2
/** HP_PERI0_PMS_READ_REE2_TEE : HRO; bitpos: [3]; default: 0;
 *  Configures tee registers read permission in ree2 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI0_PMS_READ_REE2_TEE    (BIT(3))
#define HP_PERI0_PMS_READ_REE2_TEE_M  (HP_PERI0_PMS_READ_REE2_TEE_V << HP_PERI0_PMS_READ_REE2_TEE_S)
#define HP_PERI0_PMS_READ_REE2_TEE_V  0x00000001U
#define HP_PERI0_PMS_READ_REE2_TEE_S  3
/** HP_PERI0_PMS_WRITE_TEE_TEE : R/W; bitpos: [4]; default: 1;
 *  Configures tee registers write permission in tee mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI0_PMS_WRITE_TEE_TEE    (BIT(4))
#define HP_PERI0_PMS_WRITE_TEE_TEE_M  (HP_PERI0_PMS_WRITE_TEE_TEE_V << HP_PERI0_PMS_WRITE_TEE_TEE_S)
#define HP_PERI0_PMS_WRITE_TEE_TEE_V  0x00000001U
#define HP_PERI0_PMS_WRITE_TEE_TEE_S  4
/** HP_PERI0_PMS_WRITE_REE0_TEE : HRO; bitpos: [5]; default: 0;
 *  Configures tee registers write permission in ree0 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI0_PMS_WRITE_REE0_TEE    (BIT(5))
#define HP_PERI0_PMS_WRITE_REE0_TEE_M  (HP_PERI0_PMS_WRITE_REE0_TEE_V << HP_PERI0_PMS_WRITE_REE0_TEE_S)
#define HP_PERI0_PMS_WRITE_REE0_TEE_V  0x00000001U
#define HP_PERI0_PMS_WRITE_REE0_TEE_S  5
/** HP_PERI0_PMS_WRITE_REE1_TEE : HRO; bitpos: [6]; default: 0;
 *  Configures tee registers write permission in ree1 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI0_PMS_WRITE_REE1_TEE    (BIT(6))
#define HP_PERI0_PMS_WRITE_REE1_TEE_M  (HP_PERI0_PMS_WRITE_REE1_TEE_V << HP_PERI0_PMS_WRITE_REE1_TEE_S)
#define HP_PERI0_PMS_WRITE_REE1_TEE_V  0x00000001U
#define HP_PERI0_PMS_WRITE_REE1_TEE_S  6
/** HP_PERI0_PMS_WRITE_REE2_TEE : HRO; bitpos: [7]; default: 0;
 *  Configures tee registers write permission in ree2 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI0_PMS_WRITE_REE2_TEE    (BIT(7))
#define HP_PERI0_PMS_WRITE_REE2_TEE_M  (HP_PERI0_PMS_WRITE_REE2_TEE_V << HP_PERI0_PMS_WRITE_REE2_TEE_S)
#define HP_PERI0_PMS_WRITE_REE2_TEE_V  0x00000001U
#define HP_PERI0_PMS_WRITE_REE2_TEE_S  7
/** HP_PERI0_PMS_TEE_LOCK : R/W; bitpos: [8]; default: 0;
 *  Set 1 to lock tee peri_apm configuration
 */
#define HP_PERI0_PMS_TEE_LOCK    (BIT(8))
#define HP_PERI0_PMS_TEE_LOCK_M  (HP_PERI0_PMS_TEE_LOCK_V << HP_PERI0_PMS_TEE_LOCK_S)
#define HP_PERI0_PMS_TEE_LOCK_V  0x00000001U
#define HP_PERI0_PMS_TEE_LOCK_S  8

/** HP_PERI0_PMS_HP_APM_CTRL_REG register
 *  hp_apm read/write control register
 */
#define HP_PERI0_PMS_HP_APM_CTRL_REG (DR_REG_HP_PERI0_PMS_BASE + 0x5c)
/** HP_PERI0_PMS_READ_TEE_HP_APM : R/W; bitpos: [0]; default: 1;
 *  Configures hp_apm registers read permission in tee mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI0_PMS_READ_TEE_HP_APM    (BIT(0))
#define HP_PERI0_PMS_READ_TEE_HP_APM_M  (HP_PERI0_PMS_READ_TEE_HP_APM_V << HP_PERI0_PMS_READ_TEE_HP_APM_S)
#define HP_PERI0_PMS_READ_TEE_HP_APM_V  0x00000001U
#define HP_PERI0_PMS_READ_TEE_HP_APM_S  0
/** HP_PERI0_PMS_READ_REE0_HP_APM : HRO; bitpos: [1]; default: 0;
 *  Configures hp_apm registers read permission in ree0 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI0_PMS_READ_REE0_HP_APM    (BIT(1))
#define HP_PERI0_PMS_READ_REE0_HP_APM_M  (HP_PERI0_PMS_READ_REE0_HP_APM_V << HP_PERI0_PMS_READ_REE0_HP_APM_S)
#define HP_PERI0_PMS_READ_REE0_HP_APM_V  0x00000001U
#define HP_PERI0_PMS_READ_REE0_HP_APM_S  1
/** HP_PERI0_PMS_READ_REE1_HP_APM : HRO; bitpos: [2]; default: 0;
 *  Configures hp_apm registers read permission in ree1 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI0_PMS_READ_REE1_HP_APM    (BIT(2))
#define HP_PERI0_PMS_READ_REE1_HP_APM_M  (HP_PERI0_PMS_READ_REE1_HP_APM_V << HP_PERI0_PMS_READ_REE1_HP_APM_S)
#define HP_PERI0_PMS_READ_REE1_HP_APM_V  0x00000001U
#define HP_PERI0_PMS_READ_REE1_HP_APM_S  2
/** HP_PERI0_PMS_READ_REE2_HP_APM : HRO; bitpos: [3]; default: 0;
 *  Configures hp_apm registers read permission in ree2 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI0_PMS_READ_REE2_HP_APM    (BIT(3))
#define HP_PERI0_PMS_READ_REE2_HP_APM_M  (HP_PERI0_PMS_READ_REE2_HP_APM_V << HP_PERI0_PMS_READ_REE2_HP_APM_S)
#define HP_PERI0_PMS_READ_REE2_HP_APM_V  0x00000001U
#define HP_PERI0_PMS_READ_REE2_HP_APM_S  3
/** HP_PERI0_PMS_WRITE_TEE_HP_APM : R/W; bitpos: [4]; default: 1;
 *  Configures hp_apm registers write permission in tee mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI0_PMS_WRITE_TEE_HP_APM    (BIT(4))
#define HP_PERI0_PMS_WRITE_TEE_HP_APM_M  (HP_PERI0_PMS_WRITE_TEE_HP_APM_V << HP_PERI0_PMS_WRITE_TEE_HP_APM_S)
#define HP_PERI0_PMS_WRITE_TEE_HP_APM_V  0x00000001U
#define HP_PERI0_PMS_WRITE_TEE_HP_APM_S  4
/** HP_PERI0_PMS_WRITE_REE0_HP_APM : HRO; bitpos: [5]; default: 0;
 *  Configures hp_apm registers write permission in ree0 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI0_PMS_WRITE_REE0_HP_APM    (BIT(5))
#define HP_PERI0_PMS_WRITE_REE0_HP_APM_M  (HP_PERI0_PMS_WRITE_REE0_HP_APM_V << HP_PERI0_PMS_WRITE_REE0_HP_APM_S)
#define HP_PERI0_PMS_WRITE_REE0_HP_APM_V  0x00000001U
#define HP_PERI0_PMS_WRITE_REE0_HP_APM_S  5
/** HP_PERI0_PMS_WRITE_REE1_HP_APM : HRO; bitpos: [6]; default: 0;
 *  Configures hp_apm registers write permission in ree1 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI0_PMS_WRITE_REE1_HP_APM    (BIT(6))
#define HP_PERI0_PMS_WRITE_REE1_HP_APM_M  (HP_PERI0_PMS_WRITE_REE1_HP_APM_V << HP_PERI0_PMS_WRITE_REE1_HP_APM_S)
#define HP_PERI0_PMS_WRITE_REE1_HP_APM_V  0x00000001U
#define HP_PERI0_PMS_WRITE_REE1_HP_APM_S  6
/** HP_PERI0_PMS_WRITE_REE2_HP_APM : HRO; bitpos: [7]; default: 0;
 *  Configures hp_apm registers write permission in ree2 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI0_PMS_WRITE_REE2_HP_APM    (BIT(7))
#define HP_PERI0_PMS_WRITE_REE2_HP_APM_M  (HP_PERI0_PMS_WRITE_REE2_HP_APM_V << HP_PERI0_PMS_WRITE_REE2_HP_APM_S)
#define HP_PERI0_PMS_WRITE_REE2_HP_APM_V  0x00000001U
#define HP_PERI0_PMS_WRITE_REE2_HP_APM_S  7
/** HP_PERI0_PMS_HP_APM_LOCK : R/W; bitpos: [8]; default: 0;
 *  Set 1 to lock hp_apm peri_apm configuration
 */
#define HP_PERI0_PMS_HP_APM_LOCK    (BIT(8))
#define HP_PERI0_PMS_HP_APM_LOCK_M  (HP_PERI0_PMS_HP_APM_LOCK_V << HP_PERI0_PMS_HP_APM_LOCK_S)
#define HP_PERI0_PMS_HP_APM_LOCK_V  0x00000001U
#define HP_PERI0_PMS_HP_APM_LOCK_S  8

/** HP_PERI0_PMS_HP_MEM_APM_CTRL_REG register
 *  hp_mem_apm read/write control register
 */
#define HP_PERI0_PMS_HP_MEM_APM_CTRL_REG (DR_REG_HP_PERI0_PMS_BASE + 0x60)
/** HP_PERI0_PMS_READ_TEE_HP_MEM_APM : R/W; bitpos: [0]; default: 1;
 *  Configures hp_mem_apm registers read permission in tee mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI0_PMS_READ_TEE_HP_MEM_APM    (BIT(0))
#define HP_PERI0_PMS_READ_TEE_HP_MEM_APM_M  (HP_PERI0_PMS_READ_TEE_HP_MEM_APM_V << HP_PERI0_PMS_READ_TEE_HP_MEM_APM_S)
#define HP_PERI0_PMS_READ_TEE_HP_MEM_APM_V  0x00000001U
#define HP_PERI0_PMS_READ_TEE_HP_MEM_APM_S  0
/** HP_PERI0_PMS_READ_REE0_HP_MEM_APM : HRO; bitpos: [1]; default: 0;
 *  Configures hp_mem_apm registers read permission in ree0 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI0_PMS_READ_REE0_HP_MEM_APM    (BIT(1))
#define HP_PERI0_PMS_READ_REE0_HP_MEM_APM_M  (HP_PERI0_PMS_READ_REE0_HP_MEM_APM_V << HP_PERI0_PMS_READ_REE0_HP_MEM_APM_S)
#define HP_PERI0_PMS_READ_REE0_HP_MEM_APM_V  0x00000001U
#define HP_PERI0_PMS_READ_REE0_HP_MEM_APM_S  1
/** HP_PERI0_PMS_READ_REE1_HP_MEM_APM : HRO; bitpos: [2]; default: 0;
 *  Configures hp_mem_apm registers read permission in ree1 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI0_PMS_READ_REE1_HP_MEM_APM    (BIT(2))
#define HP_PERI0_PMS_READ_REE1_HP_MEM_APM_M  (HP_PERI0_PMS_READ_REE1_HP_MEM_APM_V << HP_PERI0_PMS_READ_REE1_HP_MEM_APM_S)
#define HP_PERI0_PMS_READ_REE1_HP_MEM_APM_V  0x00000001U
#define HP_PERI0_PMS_READ_REE1_HP_MEM_APM_S  2
/** HP_PERI0_PMS_READ_REE2_HP_MEM_APM : HRO; bitpos: [3]; default: 0;
 *  Configures hp_mem_apm registers read permission in ree2 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI0_PMS_READ_REE2_HP_MEM_APM    (BIT(3))
#define HP_PERI0_PMS_READ_REE2_HP_MEM_APM_M  (HP_PERI0_PMS_READ_REE2_HP_MEM_APM_V << HP_PERI0_PMS_READ_REE2_HP_MEM_APM_S)
#define HP_PERI0_PMS_READ_REE2_HP_MEM_APM_V  0x00000001U
#define HP_PERI0_PMS_READ_REE2_HP_MEM_APM_S  3
/** HP_PERI0_PMS_WRITE_TEE_HP_MEM_APM : R/W; bitpos: [4]; default: 1;
 *  Configures hp_mem_apm registers write permission in tee mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI0_PMS_WRITE_TEE_HP_MEM_APM    (BIT(4))
#define HP_PERI0_PMS_WRITE_TEE_HP_MEM_APM_M  (HP_PERI0_PMS_WRITE_TEE_HP_MEM_APM_V << HP_PERI0_PMS_WRITE_TEE_HP_MEM_APM_S)
#define HP_PERI0_PMS_WRITE_TEE_HP_MEM_APM_V  0x00000001U
#define HP_PERI0_PMS_WRITE_TEE_HP_MEM_APM_S  4
/** HP_PERI0_PMS_WRITE_REE0_HP_MEM_APM : HRO; bitpos: [5]; default: 0;
 *  Configures hp_mem_apm registers write permission in ree0 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI0_PMS_WRITE_REE0_HP_MEM_APM    (BIT(5))
#define HP_PERI0_PMS_WRITE_REE0_HP_MEM_APM_M  (HP_PERI0_PMS_WRITE_REE0_HP_MEM_APM_V << HP_PERI0_PMS_WRITE_REE0_HP_MEM_APM_S)
#define HP_PERI0_PMS_WRITE_REE0_HP_MEM_APM_V  0x00000001U
#define HP_PERI0_PMS_WRITE_REE0_HP_MEM_APM_S  5
/** HP_PERI0_PMS_WRITE_REE1_HP_MEM_APM : HRO; bitpos: [6]; default: 0;
 *  Configures hp_mem_apm registers write permission in ree1 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI0_PMS_WRITE_REE1_HP_MEM_APM    (BIT(6))
#define HP_PERI0_PMS_WRITE_REE1_HP_MEM_APM_M  (HP_PERI0_PMS_WRITE_REE1_HP_MEM_APM_V << HP_PERI0_PMS_WRITE_REE1_HP_MEM_APM_S)
#define HP_PERI0_PMS_WRITE_REE1_HP_MEM_APM_V  0x00000001U
#define HP_PERI0_PMS_WRITE_REE1_HP_MEM_APM_S  6
/** HP_PERI0_PMS_WRITE_REE2_HP_MEM_APM : HRO; bitpos: [7]; default: 0;
 *  Configures hp_mem_apm registers write permission in ree2 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI0_PMS_WRITE_REE2_HP_MEM_APM    (BIT(7))
#define HP_PERI0_PMS_WRITE_REE2_HP_MEM_APM_M  (HP_PERI0_PMS_WRITE_REE2_HP_MEM_APM_V << HP_PERI0_PMS_WRITE_REE2_HP_MEM_APM_S)
#define HP_PERI0_PMS_WRITE_REE2_HP_MEM_APM_V  0x00000001U
#define HP_PERI0_PMS_WRITE_REE2_HP_MEM_APM_S  7
/** HP_PERI0_PMS_HP_MEM_APM_LOCK : R/W; bitpos: [8]; default: 0;
 *  Set 1 to lock hp_mem_apm peri_apm configuration
 */
#define HP_PERI0_PMS_HP_MEM_APM_LOCK    (BIT(8))
#define HP_PERI0_PMS_HP_MEM_APM_LOCK_M  (HP_PERI0_PMS_HP_MEM_APM_LOCK_V << HP_PERI0_PMS_HP_MEM_APM_LOCK_S)
#define HP_PERI0_PMS_HP_MEM_APM_LOCK_V  0x00000001U
#define HP_PERI0_PMS_HP_MEM_APM_LOCK_S  8

/** HP_PERI0_PMS_CPU_APM_CTRL_REG register
 *  cpu_apm read/write control register
 */
#define HP_PERI0_PMS_CPU_APM_CTRL_REG (DR_REG_HP_PERI0_PMS_BASE + 0x64)
/** HP_PERI0_PMS_READ_TEE_CPU_APM : R/W; bitpos: [0]; default: 1;
 *  Configures cpu_apm registers read permission in tee mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI0_PMS_READ_TEE_CPU_APM    (BIT(0))
#define HP_PERI0_PMS_READ_TEE_CPU_APM_M  (HP_PERI0_PMS_READ_TEE_CPU_APM_V << HP_PERI0_PMS_READ_TEE_CPU_APM_S)
#define HP_PERI0_PMS_READ_TEE_CPU_APM_V  0x00000001U
#define HP_PERI0_PMS_READ_TEE_CPU_APM_S  0
/** HP_PERI0_PMS_READ_REE0_CPU_APM : HRO; bitpos: [1]; default: 0;
 *  Configures cpu_apm registers read permission in ree0 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI0_PMS_READ_REE0_CPU_APM    (BIT(1))
#define HP_PERI0_PMS_READ_REE0_CPU_APM_M  (HP_PERI0_PMS_READ_REE0_CPU_APM_V << HP_PERI0_PMS_READ_REE0_CPU_APM_S)
#define HP_PERI0_PMS_READ_REE0_CPU_APM_V  0x00000001U
#define HP_PERI0_PMS_READ_REE0_CPU_APM_S  1
/** HP_PERI0_PMS_READ_REE1_CPU_APM : HRO; bitpos: [2]; default: 0;
 *  Configures cpu_apm registers read permission in ree1 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI0_PMS_READ_REE1_CPU_APM    (BIT(2))
#define HP_PERI0_PMS_READ_REE1_CPU_APM_M  (HP_PERI0_PMS_READ_REE1_CPU_APM_V << HP_PERI0_PMS_READ_REE1_CPU_APM_S)
#define HP_PERI0_PMS_READ_REE1_CPU_APM_V  0x00000001U
#define HP_PERI0_PMS_READ_REE1_CPU_APM_S  2
/** HP_PERI0_PMS_READ_REE2_CPU_APM : HRO; bitpos: [3]; default: 0;
 *  Configures cpu_apm registers read permission in ree2 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI0_PMS_READ_REE2_CPU_APM    (BIT(3))
#define HP_PERI0_PMS_READ_REE2_CPU_APM_M  (HP_PERI0_PMS_READ_REE2_CPU_APM_V << HP_PERI0_PMS_READ_REE2_CPU_APM_S)
#define HP_PERI0_PMS_READ_REE2_CPU_APM_V  0x00000001U
#define HP_PERI0_PMS_READ_REE2_CPU_APM_S  3
/** HP_PERI0_PMS_WRITE_TEE_CPU_APM : R/W; bitpos: [4]; default: 1;
 *  Configures cpu_apm registers write permission in tee mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI0_PMS_WRITE_TEE_CPU_APM    (BIT(4))
#define HP_PERI0_PMS_WRITE_TEE_CPU_APM_M  (HP_PERI0_PMS_WRITE_TEE_CPU_APM_V << HP_PERI0_PMS_WRITE_TEE_CPU_APM_S)
#define HP_PERI0_PMS_WRITE_TEE_CPU_APM_V  0x00000001U
#define HP_PERI0_PMS_WRITE_TEE_CPU_APM_S  4
/** HP_PERI0_PMS_WRITE_REE0_CPU_APM : HRO; bitpos: [5]; default: 0;
 *  Configures cpu_apm registers write permission in ree0 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI0_PMS_WRITE_REE0_CPU_APM    (BIT(5))
#define HP_PERI0_PMS_WRITE_REE0_CPU_APM_M  (HP_PERI0_PMS_WRITE_REE0_CPU_APM_V << HP_PERI0_PMS_WRITE_REE0_CPU_APM_S)
#define HP_PERI0_PMS_WRITE_REE0_CPU_APM_V  0x00000001U
#define HP_PERI0_PMS_WRITE_REE0_CPU_APM_S  5
/** HP_PERI0_PMS_WRITE_REE1_CPU_APM : HRO; bitpos: [6]; default: 0;
 *  Configures cpu_apm registers write permission in ree1 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI0_PMS_WRITE_REE1_CPU_APM    (BIT(6))
#define HP_PERI0_PMS_WRITE_REE1_CPU_APM_M  (HP_PERI0_PMS_WRITE_REE1_CPU_APM_V << HP_PERI0_PMS_WRITE_REE1_CPU_APM_S)
#define HP_PERI0_PMS_WRITE_REE1_CPU_APM_V  0x00000001U
#define HP_PERI0_PMS_WRITE_REE1_CPU_APM_S  6
/** HP_PERI0_PMS_WRITE_REE2_CPU_APM : HRO; bitpos: [7]; default: 0;
 *  Configures cpu_apm registers write permission in ree2 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI0_PMS_WRITE_REE2_CPU_APM    (BIT(7))
#define HP_PERI0_PMS_WRITE_REE2_CPU_APM_M  (HP_PERI0_PMS_WRITE_REE2_CPU_APM_V << HP_PERI0_PMS_WRITE_REE2_CPU_APM_S)
#define HP_PERI0_PMS_WRITE_REE2_CPU_APM_V  0x00000001U
#define HP_PERI0_PMS_WRITE_REE2_CPU_APM_S  7
/** HP_PERI0_PMS_CPU_APM_LOCK : R/W; bitpos: [8]; default: 0;
 *  Set 1 to lock cpu_apm peri_apm configuration
 */
#define HP_PERI0_PMS_CPU_APM_LOCK    (BIT(8))
#define HP_PERI0_PMS_CPU_APM_LOCK_M  (HP_PERI0_PMS_CPU_APM_LOCK_V << HP_PERI0_PMS_CPU_APM_LOCK_S)
#define HP_PERI0_PMS_CPU_APM_LOCK_V  0x00000001U
#define HP_PERI0_PMS_CPU_APM_LOCK_S  8

/** HP_PERI0_PMS_HP_PERI0_PMS_CTRL_REG register
 *  hp_peri0_pms read/write control register
 */
#define HP_PERI0_PMS_HP_PERI0_PMS_CTRL_REG (DR_REG_HP_PERI0_PMS_BASE + 0x68)
/** HP_PERI0_PMS_READ_TEE_HP_PERI0_PMS : R/W; bitpos: [0]; default: 1;
 *  Configures hp_peri0_pms registers read permission in tee mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI0_PMS_READ_TEE_HP_PERI0_PMS    (BIT(0))
#define HP_PERI0_PMS_READ_TEE_HP_PERI0_PMS_M  (HP_PERI0_PMS_READ_TEE_HP_PERI0_PMS_V << HP_PERI0_PMS_READ_TEE_HP_PERI0_PMS_S)
#define HP_PERI0_PMS_READ_TEE_HP_PERI0_PMS_V  0x00000001U
#define HP_PERI0_PMS_READ_TEE_HP_PERI0_PMS_S  0
/** HP_PERI0_PMS_READ_REE0_HP_PERI0_PMS : HRO; bitpos: [1]; default: 0;
 *  Configures hp_peri0_pms registers read permission in ree0 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI0_PMS_READ_REE0_HP_PERI0_PMS    (BIT(1))
#define HP_PERI0_PMS_READ_REE0_HP_PERI0_PMS_M  (HP_PERI0_PMS_READ_REE0_HP_PERI0_PMS_V << HP_PERI0_PMS_READ_REE0_HP_PERI0_PMS_S)
#define HP_PERI0_PMS_READ_REE0_HP_PERI0_PMS_V  0x00000001U
#define HP_PERI0_PMS_READ_REE0_HP_PERI0_PMS_S  1
/** HP_PERI0_PMS_READ_REE1_HP_PERI0_PMS : HRO; bitpos: [2]; default: 0;
 *  Configures hp_peri0_pms registers read permission in ree1 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI0_PMS_READ_REE1_HP_PERI0_PMS    (BIT(2))
#define HP_PERI0_PMS_READ_REE1_HP_PERI0_PMS_M  (HP_PERI0_PMS_READ_REE1_HP_PERI0_PMS_V << HP_PERI0_PMS_READ_REE1_HP_PERI0_PMS_S)
#define HP_PERI0_PMS_READ_REE1_HP_PERI0_PMS_V  0x00000001U
#define HP_PERI0_PMS_READ_REE1_HP_PERI0_PMS_S  2
/** HP_PERI0_PMS_READ_REE2_HP_PERI0_PMS : HRO; bitpos: [3]; default: 0;
 *  Configures hp_peri0_pms registers read permission in ree2 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI0_PMS_READ_REE2_HP_PERI0_PMS    (BIT(3))
#define HP_PERI0_PMS_READ_REE2_HP_PERI0_PMS_M  (HP_PERI0_PMS_READ_REE2_HP_PERI0_PMS_V << HP_PERI0_PMS_READ_REE2_HP_PERI0_PMS_S)
#define HP_PERI0_PMS_READ_REE2_HP_PERI0_PMS_V  0x00000001U
#define HP_PERI0_PMS_READ_REE2_HP_PERI0_PMS_S  3
/** HP_PERI0_PMS_WRITE_TEE_HP_PERI0_PMS : R/W; bitpos: [4]; default: 1;
 *  Configures hp_peri0_pms registers write permission in tee mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI0_PMS_WRITE_TEE_HP_PERI0_PMS    (BIT(4))
#define HP_PERI0_PMS_WRITE_TEE_HP_PERI0_PMS_M  (HP_PERI0_PMS_WRITE_TEE_HP_PERI0_PMS_V << HP_PERI0_PMS_WRITE_TEE_HP_PERI0_PMS_S)
#define HP_PERI0_PMS_WRITE_TEE_HP_PERI0_PMS_V  0x00000001U
#define HP_PERI0_PMS_WRITE_TEE_HP_PERI0_PMS_S  4
/** HP_PERI0_PMS_WRITE_REE0_HP_PERI0_PMS : HRO; bitpos: [5]; default: 0;
 *  Configures hp_peri0_pms registers write permission in ree0 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI0_PMS_WRITE_REE0_HP_PERI0_PMS    (BIT(5))
#define HP_PERI0_PMS_WRITE_REE0_HP_PERI0_PMS_M  (HP_PERI0_PMS_WRITE_REE0_HP_PERI0_PMS_V << HP_PERI0_PMS_WRITE_REE0_HP_PERI0_PMS_S)
#define HP_PERI0_PMS_WRITE_REE0_HP_PERI0_PMS_V  0x00000001U
#define HP_PERI0_PMS_WRITE_REE0_HP_PERI0_PMS_S  5
/** HP_PERI0_PMS_WRITE_REE1_HP_PERI0_PMS : HRO; bitpos: [6]; default: 0;
 *  Configures hp_peri0_pms registers write permission in ree1 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI0_PMS_WRITE_REE1_HP_PERI0_PMS    (BIT(6))
#define HP_PERI0_PMS_WRITE_REE1_HP_PERI0_PMS_M  (HP_PERI0_PMS_WRITE_REE1_HP_PERI0_PMS_V << HP_PERI0_PMS_WRITE_REE1_HP_PERI0_PMS_S)
#define HP_PERI0_PMS_WRITE_REE1_HP_PERI0_PMS_V  0x00000001U
#define HP_PERI0_PMS_WRITE_REE1_HP_PERI0_PMS_S  6
/** HP_PERI0_PMS_WRITE_REE2_HP_PERI0_PMS : HRO; bitpos: [7]; default: 0;
 *  Configures hp_peri0_pms registers write permission in ree2 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI0_PMS_WRITE_REE2_HP_PERI0_PMS    (BIT(7))
#define HP_PERI0_PMS_WRITE_REE2_HP_PERI0_PMS_M  (HP_PERI0_PMS_WRITE_REE2_HP_PERI0_PMS_V << HP_PERI0_PMS_WRITE_REE2_HP_PERI0_PMS_S)
#define HP_PERI0_PMS_WRITE_REE2_HP_PERI0_PMS_V  0x00000001U
#define HP_PERI0_PMS_WRITE_REE2_HP_PERI0_PMS_S  7
/** HP_PERI0_PMS_HP_PERI0_PMS_LOCK : R/W; bitpos: [8]; default: 0;
 *  Set 1 to lock hp_peri0_pms peri_apm configuration
 */
#define HP_PERI0_PMS_HP_PERI0_PMS_LOCK    (BIT(8))
#define HP_PERI0_PMS_HP_PERI0_PMS_LOCK_M  (HP_PERI0_PMS_HP_PERI0_PMS_LOCK_V << HP_PERI0_PMS_HP_PERI0_PMS_LOCK_S)
#define HP_PERI0_PMS_HP_PERI0_PMS_LOCK_V  0x00000001U
#define HP_PERI0_PMS_HP_PERI0_PMS_LOCK_S  8

/** HP_PERI0_PMS_HP_KEY_MANAGER_CTRL_REG register
 *  hp_key_manager read/write control register
 */
#define HP_PERI0_PMS_HP_KEY_MANAGER_CTRL_REG (DR_REG_HP_PERI0_PMS_BASE + 0x6c)
/** HP_PERI0_PMS_READ_TEE_HP_KEY_MANAGER : R/W; bitpos: [0]; default: 1;
 *  Configures hp_key_manager registers read permission in tee mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI0_PMS_READ_TEE_HP_KEY_MANAGER    (BIT(0))
#define HP_PERI0_PMS_READ_TEE_HP_KEY_MANAGER_M  (HP_PERI0_PMS_READ_TEE_HP_KEY_MANAGER_V << HP_PERI0_PMS_READ_TEE_HP_KEY_MANAGER_S)
#define HP_PERI0_PMS_READ_TEE_HP_KEY_MANAGER_V  0x00000001U
#define HP_PERI0_PMS_READ_TEE_HP_KEY_MANAGER_S  0
/** HP_PERI0_PMS_READ_REE0_HP_KEY_MANAGER : R/W; bitpos: [1]; default: 0;
 *  Configures hp_key_manager registers read permission in ree0 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI0_PMS_READ_REE0_HP_KEY_MANAGER    (BIT(1))
#define HP_PERI0_PMS_READ_REE0_HP_KEY_MANAGER_M  (HP_PERI0_PMS_READ_REE0_HP_KEY_MANAGER_V << HP_PERI0_PMS_READ_REE0_HP_KEY_MANAGER_S)
#define HP_PERI0_PMS_READ_REE0_HP_KEY_MANAGER_V  0x00000001U
#define HP_PERI0_PMS_READ_REE0_HP_KEY_MANAGER_S  1
/** HP_PERI0_PMS_READ_REE1_HP_KEY_MANAGER : R/W; bitpos: [2]; default: 0;
 *  Configures hp_key_manager registers read permission in ree1 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI0_PMS_READ_REE1_HP_KEY_MANAGER    (BIT(2))
#define HP_PERI0_PMS_READ_REE1_HP_KEY_MANAGER_M  (HP_PERI0_PMS_READ_REE1_HP_KEY_MANAGER_V << HP_PERI0_PMS_READ_REE1_HP_KEY_MANAGER_S)
#define HP_PERI0_PMS_READ_REE1_HP_KEY_MANAGER_V  0x00000001U
#define HP_PERI0_PMS_READ_REE1_HP_KEY_MANAGER_S  2
/** HP_PERI0_PMS_READ_REE2_HP_KEY_MANAGER : R/W; bitpos: [3]; default: 0;
 *  Configures hp_key_manager registers read permission in ree2 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI0_PMS_READ_REE2_HP_KEY_MANAGER    (BIT(3))
#define HP_PERI0_PMS_READ_REE2_HP_KEY_MANAGER_M  (HP_PERI0_PMS_READ_REE2_HP_KEY_MANAGER_V << HP_PERI0_PMS_READ_REE2_HP_KEY_MANAGER_S)
#define HP_PERI0_PMS_READ_REE2_HP_KEY_MANAGER_V  0x00000001U
#define HP_PERI0_PMS_READ_REE2_HP_KEY_MANAGER_S  3
/** HP_PERI0_PMS_WRITE_TEE_HP_KEY_MANAGER : R/W; bitpos: [4]; default: 1;
 *  Configures hp_key_manager registers write permission in tee mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI0_PMS_WRITE_TEE_HP_KEY_MANAGER    (BIT(4))
#define HP_PERI0_PMS_WRITE_TEE_HP_KEY_MANAGER_M  (HP_PERI0_PMS_WRITE_TEE_HP_KEY_MANAGER_V << HP_PERI0_PMS_WRITE_TEE_HP_KEY_MANAGER_S)
#define HP_PERI0_PMS_WRITE_TEE_HP_KEY_MANAGER_V  0x00000001U
#define HP_PERI0_PMS_WRITE_TEE_HP_KEY_MANAGER_S  4
/** HP_PERI0_PMS_WRITE_REE0_HP_KEY_MANAGER : R/W; bitpos: [5]; default: 0;
 *  Configures hp_key_manager registers write permission in ree0 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI0_PMS_WRITE_REE0_HP_KEY_MANAGER    (BIT(5))
#define HP_PERI0_PMS_WRITE_REE0_HP_KEY_MANAGER_M  (HP_PERI0_PMS_WRITE_REE0_HP_KEY_MANAGER_V << HP_PERI0_PMS_WRITE_REE0_HP_KEY_MANAGER_S)
#define HP_PERI0_PMS_WRITE_REE0_HP_KEY_MANAGER_V  0x00000001U
#define HP_PERI0_PMS_WRITE_REE0_HP_KEY_MANAGER_S  5
/** HP_PERI0_PMS_WRITE_REE1_HP_KEY_MANAGER : R/W; bitpos: [6]; default: 0;
 *  Configures hp_key_manager registers write permission in ree1 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI0_PMS_WRITE_REE1_HP_KEY_MANAGER    (BIT(6))
#define HP_PERI0_PMS_WRITE_REE1_HP_KEY_MANAGER_M  (HP_PERI0_PMS_WRITE_REE1_HP_KEY_MANAGER_V << HP_PERI0_PMS_WRITE_REE1_HP_KEY_MANAGER_S)
#define HP_PERI0_PMS_WRITE_REE1_HP_KEY_MANAGER_V  0x00000001U
#define HP_PERI0_PMS_WRITE_REE1_HP_KEY_MANAGER_S  6
/** HP_PERI0_PMS_WRITE_REE2_HP_KEY_MANAGER : R/W; bitpos: [7]; default: 0;
 *  Configures hp_key_manager registers write permission in ree2 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI0_PMS_WRITE_REE2_HP_KEY_MANAGER    (BIT(7))
#define HP_PERI0_PMS_WRITE_REE2_HP_KEY_MANAGER_M  (HP_PERI0_PMS_WRITE_REE2_HP_KEY_MANAGER_V << HP_PERI0_PMS_WRITE_REE2_HP_KEY_MANAGER_S)
#define HP_PERI0_PMS_WRITE_REE2_HP_KEY_MANAGER_V  0x00000001U
#define HP_PERI0_PMS_WRITE_REE2_HP_KEY_MANAGER_S  7
/** HP_PERI0_PMS_HP_KEY_MANAGER_LOCK : R/W; bitpos: [8]; default: 0;
 *  Set 1 to lock hp_key_manager peri_apm configuration
 */
#define HP_PERI0_PMS_HP_KEY_MANAGER_LOCK    (BIT(8))
#define HP_PERI0_PMS_HP_KEY_MANAGER_LOCK_M  (HP_PERI0_PMS_HP_KEY_MANAGER_LOCK_V << HP_PERI0_PMS_HP_KEY_MANAGER_LOCK_S)
#define HP_PERI0_PMS_HP_KEY_MANAGER_LOCK_V  0x00000001U
#define HP_PERI0_PMS_HP_KEY_MANAGER_LOCK_S  8

/** HP_PERI0_PMS_HP_CRYPTO_CTRL_REG register
 *  hp_crypto read/write control register
 */
#define HP_PERI0_PMS_HP_CRYPTO_CTRL_REG (DR_REG_HP_PERI0_PMS_BASE + 0x70)
/** HP_PERI0_PMS_READ_TEE_HP_CRYPTO : R/W; bitpos: [0]; default: 1;
 *  Configures hp_crypto registers read permission in tee mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI0_PMS_READ_TEE_HP_CRYPTO    (BIT(0))
#define HP_PERI0_PMS_READ_TEE_HP_CRYPTO_M  (HP_PERI0_PMS_READ_TEE_HP_CRYPTO_V << HP_PERI0_PMS_READ_TEE_HP_CRYPTO_S)
#define HP_PERI0_PMS_READ_TEE_HP_CRYPTO_V  0x00000001U
#define HP_PERI0_PMS_READ_TEE_HP_CRYPTO_S  0
/** HP_PERI0_PMS_READ_REE0_HP_CRYPTO : R/W; bitpos: [1]; default: 0;
 *  Configures hp_crypto registers read permission in ree0 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI0_PMS_READ_REE0_HP_CRYPTO    (BIT(1))
#define HP_PERI0_PMS_READ_REE0_HP_CRYPTO_M  (HP_PERI0_PMS_READ_REE0_HP_CRYPTO_V << HP_PERI0_PMS_READ_REE0_HP_CRYPTO_S)
#define HP_PERI0_PMS_READ_REE0_HP_CRYPTO_V  0x00000001U
#define HP_PERI0_PMS_READ_REE0_HP_CRYPTO_S  1
/** HP_PERI0_PMS_READ_REE1_HP_CRYPTO : R/W; bitpos: [2]; default: 0;
 *  Configures hp_crypto registers read permission in ree1 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI0_PMS_READ_REE1_HP_CRYPTO    (BIT(2))
#define HP_PERI0_PMS_READ_REE1_HP_CRYPTO_M  (HP_PERI0_PMS_READ_REE1_HP_CRYPTO_V << HP_PERI0_PMS_READ_REE1_HP_CRYPTO_S)
#define HP_PERI0_PMS_READ_REE1_HP_CRYPTO_V  0x00000001U
#define HP_PERI0_PMS_READ_REE1_HP_CRYPTO_S  2
/** HP_PERI0_PMS_READ_REE2_HP_CRYPTO : R/W; bitpos: [3]; default: 0;
 *  Configures hp_crypto registers read permission in ree2 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI0_PMS_READ_REE2_HP_CRYPTO    (BIT(3))
#define HP_PERI0_PMS_READ_REE2_HP_CRYPTO_M  (HP_PERI0_PMS_READ_REE2_HP_CRYPTO_V << HP_PERI0_PMS_READ_REE2_HP_CRYPTO_S)
#define HP_PERI0_PMS_READ_REE2_HP_CRYPTO_V  0x00000001U
#define HP_PERI0_PMS_READ_REE2_HP_CRYPTO_S  3
/** HP_PERI0_PMS_WRITE_TEE_HP_CRYPTO : R/W; bitpos: [4]; default: 1;
 *  Configures hp_crypto registers write permission in tee mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI0_PMS_WRITE_TEE_HP_CRYPTO    (BIT(4))
#define HP_PERI0_PMS_WRITE_TEE_HP_CRYPTO_M  (HP_PERI0_PMS_WRITE_TEE_HP_CRYPTO_V << HP_PERI0_PMS_WRITE_TEE_HP_CRYPTO_S)
#define HP_PERI0_PMS_WRITE_TEE_HP_CRYPTO_V  0x00000001U
#define HP_PERI0_PMS_WRITE_TEE_HP_CRYPTO_S  4
/** HP_PERI0_PMS_WRITE_REE0_HP_CRYPTO : R/W; bitpos: [5]; default: 0;
 *  Configures hp_crypto registers write permission in ree0 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI0_PMS_WRITE_REE0_HP_CRYPTO    (BIT(5))
#define HP_PERI0_PMS_WRITE_REE0_HP_CRYPTO_M  (HP_PERI0_PMS_WRITE_REE0_HP_CRYPTO_V << HP_PERI0_PMS_WRITE_REE0_HP_CRYPTO_S)
#define HP_PERI0_PMS_WRITE_REE0_HP_CRYPTO_V  0x00000001U
#define HP_PERI0_PMS_WRITE_REE0_HP_CRYPTO_S  5
/** HP_PERI0_PMS_WRITE_REE1_HP_CRYPTO : R/W; bitpos: [6]; default: 0;
 *  Configures hp_crypto registers write permission in ree1 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI0_PMS_WRITE_REE1_HP_CRYPTO    (BIT(6))
#define HP_PERI0_PMS_WRITE_REE1_HP_CRYPTO_M  (HP_PERI0_PMS_WRITE_REE1_HP_CRYPTO_V << HP_PERI0_PMS_WRITE_REE1_HP_CRYPTO_S)
#define HP_PERI0_PMS_WRITE_REE1_HP_CRYPTO_V  0x00000001U
#define HP_PERI0_PMS_WRITE_REE1_HP_CRYPTO_S  6
/** HP_PERI0_PMS_WRITE_REE2_HP_CRYPTO : R/W; bitpos: [7]; default: 0;
 *  Configures hp_crypto registers write permission in ree2 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI0_PMS_WRITE_REE2_HP_CRYPTO    (BIT(7))
#define HP_PERI0_PMS_WRITE_REE2_HP_CRYPTO_M  (HP_PERI0_PMS_WRITE_REE2_HP_CRYPTO_V << HP_PERI0_PMS_WRITE_REE2_HP_CRYPTO_S)
#define HP_PERI0_PMS_WRITE_REE2_HP_CRYPTO_V  0x00000001U
#define HP_PERI0_PMS_WRITE_REE2_HP_CRYPTO_S  7
/** HP_PERI0_PMS_HP_CRYPTO_LOCK : R/W; bitpos: [8]; default: 0;
 *  Set 1 to lock hp_crypto peri_apm configuration
 */
#define HP_PERI0_PMS_HP_CRYPTO_LOCK    (BIT(8))
#define HP_PERI0_PMS_HP_CRYPTO_LOCK_M  (HP_PERI0_PMS_HP_CRYPTO_LOCK_V << HP_PERI0_PMS_HP_CRYPTO_LOCK_S)
#define HP_PERI0_PMS_HP_CRYPTO_LOCK_V  0x00000001U
#define HP_PERI0_PMS_HP_CRYPTO_LOCK_S  8

/** HP_PERI0_PMS_HP_AXI_ICM_CTRL_REG register
 *  hp_axi_icm read/write control register
 */
#define HP_PERI0_PMS_HP_AXI_ICM_CTRL_REG (DR_REG_HP_PERI0_PMS_BASE + 0x74)
/** HP_PERI0_PMS_READ_TEE_HP_AXI_ICM : R/W; bitpos: [0]; default: 1;
 *  Configures hp_axi_icm registers read permission in tee mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI0_PMS_READ_TEE_HP_AXI_ICM    (BIT(0))
#define HP_PERI0_PMS_READ_TEE_HP_AXI_ICM_M  (HP_PERI0_PMS_READ_TEE_HP_AXI_ICM_V << HP_PERI0_PMS_READ_TEE_HP_AXI_ICM_S)
#define HP_PERI0_PMS_READ_TEE_HP_AXI_ICM_V  0x00000001U
#define HP_PERI0_PMS_READ_TEE_HP_AXI_ICM_S  0
/** HP_PERI0_PMS_READ_REE0_HP_AXI_ICM : R/W; bitpos: [1]; default: 0;
 *  Configures hp_axi_icm registers read permission in ree0 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI0_PMS_READ_REE0_HP_AXI_ICM    (BIT(1))
#define HP_PERI0_PMS_READ_REE0_HP_AXI_ICM_M  (HP_PERI0_PMS_READ_REE0_HP_AXI_ICM_V << HP_PERI0_PMS_READ_REE0_HP_AXI_ICM_S)
#define HP_PERI0_PMS_READ_REE0_HP_AXI_ICM_V  0x00000001U
#define HP_PERI0_PMS_READ_REE0_HP_AXI_ICM_S  1
/** HP_PERI0_PMS_READ_REE1_HP_AXI_ICM : R/W; bitpos: [2]; default: 0;
 *  Configures hp_axi_icm registers read permission in ree1 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI0_PMS_READ_REE1_HP_AXI_ICM    (BIT(2))
#define HP_PERI0_PMS_READ_REE1_HP_AXI_ICM_M  (HP_PERI0_PMS_READ_REE1_HP_AXI_ICM_V << HP_PERI0_PMS_READ_REE1_HP_AXI_ICM_S)
#define HP_PERI0_PMS_READ_REE1_HP_AXI_ICM_V  0x00000001U
#define HP_PERI0_PMS_READ_REE1_HP_AXI_ICM_S  2
/** HP_PERI0_PMS_READ_REE2_HP_AXI_ICM : R/W; bitpos: [3]; default: 0;
 *  Configures hp_axi_icm registers read permission in ree2 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI0_PMS_READ_REE2_HP_AXI_ICM    (BIT(3))
#define HP_PERI0_PMS_READ_REE2_HP_AXI_ICM_M  (HP_PERI0_PMS_READ_REE2_HP_AXI_ICM_V << HP_PERI0_PMS_READ_REE2_HP_AXI_ICM_S)
#define HP_PERI0_PMS_READ_REE2_HP_AXI_ICM_V  0x00000001U
#define HP_PERI0_PMS_READ_REE2_HP_AXI_ICM_S  3
/** HP_PERI0_PMS_WRITE_TEE_HP_AXI_ICM : R/W; bitpos: [4]; default: 1;
 *  Configures hp_axi_icm registers write permission in tee mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI0_PMS_WRITE_TEE_HP_AXI_ICM    (BIT(4))
#define HP_PERI0_PMS_WRITE_TEE_HP_AXI_ICM_M  (HP_PERI0_PMS_WRITE_TEE_HP_AXI_ICM_V << HP_PERI0_PMS_WRITE_TEE_HP_AXI_ICM_S)
#define HP_PERI0_PMS_WRITE_TEE_HP_AXI_ICM_V  0x00000001U
#define HP_PERI0_PMS_WRITE_TEE_HP_AXI_ICM_S  4
/** HP_PERI0_PMS_WRITE_REE0_HP_AXI_ICM : R/W; bitpos: [5]; default: 0;
 *  Configures hp_axi_icm registers write permission in ree0 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI0_PMS_WRITE_REE0_HP_AXI_ICM    (BIT(5))
#define HP_PERI0_PMS_WRITE_REE0_HP_AXI_ICM_M  (HP_PERI0_PMS_WRITE_REE0_HP_AXI_ICM_V << HP_PERI0_PMS_WRITE_REE0_HP_AXI_ICM_S)
#define HP_PERI0_PMS_WRITE_REE0_HP_AXI_ICM_V  0x00000001U
#define HP_PERI0_PMS_WRITE_REE0_HP_AXI_ICM_S  5
/** HP_PERI0_PMS_WRITE_REE1_HP_AXI_ICM : R/W; bitpos: [6]; default: 0;
 *  Configures hp_axi_icm registers write permission in ree1 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI0_PMS_WRITE_REE1_HP_AXI_ICM    (BIT(6))
#define HP_PERI0_PMS_WRITE_REE1_HP_AXI_ICM_M  (HP_PERI0_PMS_WRITE_REE1_HP_AXI_ICM_V << HP_PERI0_PMS_WRITE_REE1_HP_AXI_ICM_S)
#define HP_PERI0_PMS_WRITE_REE1_HP_AXI_ICM_V  0x00000001U
#define HP_PERI0_PMS_WRITE_REE1_HP_AXI_ICM_S  6
/** HP_PERI0_PMS_WRITE_REE2_HP_AXI_ICM : R/W; bitpos: [7]; default: 0;
 *  Configures hp_axi_icm registers write permission in ree2 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI0_PMS_WRITE_REE2_HP_AXI_ICM    (BIT(7))
#define HP_PERI0_PMS_WRITE_REE2_HP_AXI_ICM_M  (HP_PERI0_PMS_WRITE_REE2_HP_AXI_ICM_V << HP_PERI0_PMS_WRITE_REE2_HP_AXI_ICM_S)
#define HP_PERI0_PMS_WRITE_REE2_HP_AXI_ICM_V  0x00000001U
#define HP_PERI0_PMS_WRITE_REE2_HP_AXI_ICM_S  7
/** HP_PERI0_PMS_HP_AXI_ICM_LOCK : R/W; bitpos: [8]; default: 0;
 *  Set 1 to lock hp_axi_icm peri_apm configuration
 */
#define HP_PERI0_PMS_HP_AXI_ICM_LOCK    (BIT(8))
#define HP_PERI0_PMS_HP_AXI_ICM_LOCK_M  (HP_PERI0_PMS_HP_AXI_ICM_LOCK_V << HP_PERI0_PMS_HP_AXI_ICM_LOCK_S)
#define HP_PERI0_PMS_HP_AXI_ICM_LOCK_V  0x00000001U
#define HP_PERI0_PMS_HP_AXI_ICM_LOCK_S  8

/** HP_PERI0_PMS_AXI_PERF_MON_CTRL_REG register
 *  axi_perf_mon read/write control register
 */
#define HP_PERI0_PMS_AXI_PERF_MON_CTRL_REG (DR_REG_HP_PERI0_PMS_BASE + 0x78)
/** HP_PERI0_PMS_READ_TEE_AXI_PERF_MON : R/W; bitpos: [0]; default: 1;
 *  Configures axi_perf_mon registers read permission in tee mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI0_PMS_READ_TEE_AXI_PERF_MON    (BIT(0))
#define HP_PERI0_PMS_READ_TEE_AXI_PERF_MON_M  (HP_PERI0_PMS_READ_TEE_AXI_PERF_MON_V << HP_PERI0_PMS_READ_TEE_AXI_PERF_MON_S)
#define HP_PERI0_PMS_READ_TEE_AXI_PERF_MON_V  0x00000001U
#define HP_PERI0_PMS_READ_TEE_AXI_PERF_MON_S  0
/** HP_PERI0_PMS_READ_REE0_AXI_PERF_MON : R/W; bitpos: [1]; default: 0;
 *  Configures axi_perf_mon registers read permission in ree0 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI0_PMS_READ_REE0_AXI_PERF_MON    (BIT(1))
#define HP_PERI0_PMS_READ_REE0_AXI_PERF_MON_M  (HP_PERI0_PMS_READ_REE0_AXI_PERF_MON_V << HP_PERI0_PMS_READ_REE0_AXI_PERF_MON_S)
#define HP_PERI0_PMS_READ_REE0_AXI_PERF_MON_V  0x00000001U
#define HP_PERI0_PMS_READ_REE0_AXI_PERF_MON_S  1
/** HP_PERI0_PMS_READ_REE1_AXI_PERF_MON : R/W; bitpos: [2]; default: 0;
 *  Configures axi_perf_mon registers read permission in ree1 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI0_PMS_READ_REE1_AXI_PERF_MON    (BIT(2))
#define HP_PERI0_PMS_READ_REE1_AXI_PERF_MON_M  (HP_PERI0_PMS_READ_REE1_AXI_PERF_MON_V << HP_PERI0_PMS_READ_REE1_AXI_PERF_MON_S)
#define HP_PERI0_PMS_READ_REE1_AXI_PERF_MON_V  0x00000001U
#define HP_PERI0_PMS_READ_REE1_AXI_PERF_MON_S  2
/** HP_PERI0_PMS_READ_REE2_AXI_PERF_MON : R/W; bitpos: [3]; default: 0;
 *  Configures axi_perf_mon registers read permission in ree2 mode.
 *  0: can not be read
 *  1: can be read
 */
#define HP_PERI0_PMS_READ_REE2_AXI_PERF_MON    (BIT(3))
#define HP_PERI0_PMS_READ_REE2_AXI_PERF_MON_M  (HP_PERI0_PMS_READ_REE2_AXI_PERF_MON_V << HP_PERI0_PMS_READ_REE2_AXI_PERF_MON_S)
#define HP_PERI0_PMS_READ_REE2_AXI_PERF_MON_V  0x00000001U
#define HP_PERI0_PMS_READ_REE2_AXI_PERF_MON_S  3
/** HP_PERI0_PMS_WRITE_TEE_AXI_PERF_MON : R/W; bitpos: [4]; default: 1;
 *  Configures axi_perf_mon registers write permission in tee mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI0_PMS_WRITE_TEE_AXI_PERF_MON    (BIT(4))
#define HP_PERI0_PMS_WRITE_TEE_AXI_PERF_MON_M  (HP_PERI0_PMS_WRITE_TEE_AXI_PERF_MON_V << HP_PERI0_PMS_WRITE_TEE_AXI_PERF_MON_S)
#define HP_PERI0_PMS_WRITE_TEE_AXI_PERF_MON_V  0x00000001U
#define HP_PERI0_PMS_WRITE_TEE_AXI_PERF_MON_S  4
/** HP_PERI0_PMS_WRITE_REE0_AXI_PERF_MON : R/W; bitpos: [5]; default: 0;
 *  Configures axi_perf_mon registers write permission in ree0 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI0_PMS_WRITE_REE0_AXI_PERF_MON    (BIT(5))
#define HP_PERI0_PMS_WRITE_REE0_AXI_PERF_MON_M  (HP_PERI0_PMS_WRITE_REE0_AXI_PERF_MON_V << HP_PERI0_PMS_WRITE_REE0_AXI_PERF_MON_S)
#define HP_PERI0_PMS_WRITE_REE0_AXI_PERF_MON_V  0x00000001U
#define HP_PERI0_PMS_WRITE_REE0_AXI_PERF_MON_S  5
/** HP_PERI0_PMS_WRITE_REE1_AXI_PERF_MON : R/W; bitpos: [6]; default: 0;
 *  Configures axi_perf_mon registers write permission in ree1 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI0_PMS_WRITE_REE1_AXI_PERF_MON    (BIT(6))
#define HP_PERI0_PMS_WRITE_REE1_AXI_PERF_MON_M  (HP_PERI0_PMS_WRITE_REE1_AXI_PERF_MON_V << HP_PERI0_PMS_WRITE_REE1_AXI_PERF_MON_S)
#define HP_PERI0_PMS_WRITE_REE1_AXI_PERF_MON_V  0x00000001U
#define HP_PERI0_PMS_WRITE_REE1_AXI_PERF_MON_S  6
/** HP_PERI0_PMS_WRITE_REE2_AXI_PERF_MON : R/W; bitpos: [7]; default: 0;
 *  Configures axi_perf_mon registers write permission in ree2 mode.
 *  0: can not be write
 *  1: can be write
 */
#define HP_PERI0_PMS_WRITE_REE2_AXI_PERF_MON    (BIT(7))
#define HP_PERI0_PMS_WRITE_REE2_AXI_PERF_MON_M  (HP_PERI0_PMS_WRITE_REE2_AXI_PERF_MON_V << HP_PERI0_PMS_WRITE_REE2_AXI_PERF_MON_S)
#define HP_PERI0_PMS_WRITE_REE2_AXI_PERF_MON_V  0x00000001U
#define HP_PERI0_PMS_WRITE_REE2_AXI_PERF_MON_S  7
/** HP_PERI0_PMS_AXI_PERF_MON_LOCK : R/W; bitpos: [8]; default: 0;
 *  Set 1 to lock axi_perf_mon peri_apm configuration
 */
#define HP_PERI0_PMS_AXI_PERF_MON_LOCK    (BIT(8))
#define HP_PERI0_PMS_AXI_PERF_MON_LOCK_M  (HP_PERI0_PMS_AXI_PERF_MON_LOCK_V << HP_PERI0_PMS_AXI_PERF_MON_LOCK_S)
#define HP_PERI0_PMS_AXI_PERF_MON_LOCK_V  0x00000001U
#define HP_PERI0_PMS_AXI_PERF_MON_LOCK_S  8

/** HP_PERI0_PMS_HP_CPUPERI0_0_REG register
 *  HP_CPUPERI0 PMS configuration & info register
 */
#define HP_PERI0_PMS_HP_CPUPERI0_0_REG (DR_REG_HP_PERI0_PMS_BASE + 0x200)
/** HP_PERI0_PMS_HP_CPUPERI0_PMS_EXCEPTION_CLR : WT; bitpos: [0]; default: 0;
 *  Configures whether or not to clear hp_cpuperi0 peri_pms_record_reg.
 *  0: No clear
 *  1: Clear peri_pms_record_reg
 */
#define HP_PERI0_PMS_HP_CPUPERI0_PMS_EXCEPTION_CLR    (BIT(0))
#define HP_PERI0_PMS_HP_CPUPERI0_PMS_EXCEPTION_CLR_M  (HP_PERI0_PMS_HP_CPUPERI0_PMS_EXCEPTION_CLR_V << HP_PERI0_PMS_HP_CPUPERI0_PMS_EXCEPTION_CLR_S)
#define HP_PERI0_PMS_HP_CPUPERI0_PMS_EXCEPTION_CLR_V  0x00000001U
#define HP_PERI0_PMS_HP_CPUPERI0_PMS_EXCEPTION_CLR_S  0
/** HP_PERI0_PMS_HP_CPUPERI0_PMS_EXCEPTION_DET : RO; bitpos: [1]; default: 0;
 *  Represents whether the hp_cpuperi0 pms has been triggered.
 *  0: No triggered
 *  1: Has been triggered
 */
#define HP_PERI0_PMS_HP_CPUPERI0_PMS_EXCEPTION_DET    (BIT(1))
#define HP_PERI0_PMS_HP_CPUPERI0_PMS_EXCEPTION_DET_M  (HP_PERI0_PMS_HP_CPUPERI0_PMS_EXCEPTION_DET_V << HP_PERI0_PMS_HP_CPUPERI0_PMS_EXCEPTION_DET_S)
#define HP_PERI0_PMS_HP_CPUPERI0_PMS_EXCEPTION_DET_V  0x00000001U
#define HP_PERI0_PMS_HP_CPUPERI0_PMS_EXCEPTION_DET_S  1
/** HP_PERI0_PMS_HP_CPUPERI0_PMS_EXCEPTION_ID : RO; bitpos: [7:2]; default: 0;
 *  Represents the master id when hp_cpuperi0 pms has been triggered.
 */
#define HP_PERI0_PMS_HP_CPUPERI0_PMS_EXCEPTION_ID    0x0000003FU
#define HP_PERI0_PMS_HP_CPUPERI0_PMS_EXCEPTION_ID_M  (HP_PERI0_PMS_HP_CPUPERI0_PMS_EXCEPTION_ID_V << HP_PERI0_PMS_HP_CPUPERI0_PMS_EXCEPTION_ID_S)
#define HP_PERI0_PMS_HP_CPUPERI0_PMS_EXCEPTION_ID_V  0x0000003FU
#define HP_PERI0_PMS_HP_CPUPERI0_PMS_EXCEPTION_ID_S  2
/** HP_PERI0_PMS_HP_CPUPERI0_PMS_EXCEPTION_MODE : RO; bitpos: [9:8]; default: 0;
 *  Represents the security mode when hp_cpuperi0 pms has been triggered.
 */
#define HP_PERI0_PMS_HP_CPUPERI0_PMS_EXCEPTION_MODE    0x00000003U
#define HP_PERI0_PMS_HP_CPUPERI0_PMS_EXCEPTION_MODE_M  (HP_PERI0_PMS_HP_CPUPERI0_PMS_EXCEPTION_MODE_V << HP_PERI0_PMS_HP_CPUPERI0_PMS_EXCEPTION_MODE_S)
#define HP_PERI0_PMS_HP_CPUPERI0_PMS_EXCEPTION_MODE_V  0x00000003U
#define HP_PERI0_PMS_HP_CPUPERI0_PMS_EXCEPTION_MODE_S  8

/** HP_PERI0_PMS_HP_CPUPERI0_1_REG register
 *  HP_CPUPERI0 PMS exception addr record register
 */
#define HP_PERI0_PMS_HP_CPUPERI0_1_REG (DR_REG_HP_PERI0_PMS_BASE + 0x204)
/** HP_PERI0_PMS_HP_CPUPERI0_PMS_EXCEPTION_ADDR : RO; bitpos: [27:0]; default: 0;
 *  Represents the access address (bit27~bit0) when hp_cpuperi0 pms has been triggered.
 */
#define HP_PERI0_PMS_HP_CPUPERI0_PMS_EXCEPTION_ADDR    0x0FFFFFFFU
#define HP_PERI0_PMS_HP_CPUPERI0_PMS_EXCEPTION_ADDR_M  (HP_PERI0_PMS_HP_CPUPERI0_PMS_EXCEPTION_ADDR_V << HP_PERI0_PMS_HP_CPUPERI0_PMS_EXCEPTION_ADDR_S)
#define HP_PERI0_PMS_HP_CPUPERI0_PMS_EXCEPTION_ADDR_V  0x0FFFFFFFU
#define HP_PERI0_PMS_HP_CPUPERI0_PMS_EXCEPTION_ADDR_S  0
/** HP_PERI0_PMS_HP_CPUPERI0_PMS_EXCEPTION_ADDR_CONST : HRO; bitpos: [31:28]; default:
 *  2;
 *  Represents the access address (bit31~bit28) when hp_cpuperi0 pms has been triggered.
 */
#define HP_PERI0_PMS_HP_CPUPERI0_PMS_EXCEPTION_ADDR_CONST    0x0000000FU
#define HP_PERI0_PMS_HP_CPUPERI0_PMS_EXCEPTION_ADDR_CONST_M  (HP_PERI0_PMS_HP_CPUPERI0_PMS_EXCEPTION_ADDR_CONST_V << HP_PERI0_PMS_HP_CPUPERI0_PMS_EXCEPTION_ADDR_CONST_S)
#define HP_PERI0_PMS_HP_CPUPERI0_PMS_EXCEPTION_ADDR_CONST_V  0x0000000FU
#define HP_PERI0_PMS_HP_CPUPERI0_PMS_EXCEPTION_ADDR_CONST_S  28

/** HP_PERI0_PMS_HP_CPUPERI1_0_REG register
 *  HP_CPUPERI1 PMS configuration & info register
 */
#define HP_PERI0_PMS_HP_CPUPERI1_0_REG (DR_REG_HP_PERI0_PMS_BASE + 0x208)
/** HP_PERI0_PMS_HP_CPUPERI1_PMS_EXCEPTION_CLR : WT; bitpos: [0]; default: 0;
 *  Configures whether or not to clear hp_cpuperi1 peri_pms_record_reg.
 *  0: No clear
 *  1: Clear peri_pms_record_reg
 */
#define HP_PERI0_PMS_HP_CPUPERI1_PMS_EXCEPTION_CLR    (BIT(0))
#define HP_PERI0_PMS_HP_CPUPERI1_PMS_EXCEPTION_CLR_M  (HP_PERI0_PMS_HP_CPUPERI1_PMS_EXCEPTION_CLR_V << HP_PERI0_PMS_HP_CPUPERI1_PMS_EXCEPTION_CLR_S)
#define HP_PERI0_PMS_HP_CPUPERI1_PMS_EXCEPTION_CLR_V  0x00000001U
#define HP_PERI0_PMS_HP_CPUPERI1_PMS_EXCEPTION_CLR_S  0
/** HP_PERI0_PMS_HP_CPUPERI1_PMS_EXCEPTION_DET : RO; bitpos: [1]; default: 0;
 *  Represents whether the hp_cpuperi1 pms has been triggered.
 *  0: No triggered
 *  1: Has been triggered
 */
#define HP_PERI0_PMS_HP_CPUPERI1_PMS_EXCEPTION_DET    (BIT(1))
#define HP_PERI0_PMS_HP_CPUPERI1_PMS_EXCEPTION_DET_M  (HP_PERI0_PMS_HP_CPUPERI1_PMS_EXCEPTION_DET_V << HP_PERI0_PMS_HP_CPUPERI1_PMS_EXCEPTION_DET_S)
#define HP_PERI0_PMS_HP_CPUPERI1_PMS_EXCEPTION_DET_V  0x00000001U
#define HP_PERI0_PMS_HP_CPUPERI1_PMS_EXCEPTION_DET_S  1
/** HP_PERI0_PMS_HP_CPUPERI1_PMS_EXCEPTION_ID : RO; bitpos: [7:2]; default: 0;
 *  Represents the master id when hp_cpuperi1 pms has been triggered.
 */
#define HP_PERI0_PMS_HP_CPUPERI1_PMS_EXCEPTION_ID    0x0000003FU
#define HP_PERI0_PMS_HP_CPUPERI1_PMS_EXCEPTION_ID_M  (HP_PERI0_PMS_HP_CPUPERI1_PMS_EXCEPTION_ID_V << HP_PERI0_PMS_HP_CPUPERI1_PMS_EXCEPTION_ID_S)
#define HP_PERI0_PMS_HP_CPUPERI1_PMS_EXCEPTION_ID_V  0x0000003FU
#define HP_PERI0_PMS_HP_CPUPERI1_PMS_EXCEPTION_ID_S  2
/** HP_PERI0_PMS_HP_CPUPERI1_PMS_EXCEPTION_MODE : RO; bitpos: [9:8]; default: 0;
 *  Represents the security mode when hp_cpuperi1 pms has been triggered.
 */
#define HP_PERI0_PMS_HP_CPUPERI1_PMS_EXCEPTION_MODE    0x00000003U
#define HP_PERI0_PMS_HP_CPUPERI1_PMS_EXCEPTION_MODE_M  (HP_PERI0_PMS_HP_CPUPERI1_PMS_EXCEPTION_MODE_V << HP_PERI0_PMS_HP_CPUPERI1_PMS_EXCEPTION_MODE_S)
#define HP_PERI0_PMS_HP_CPUPERI1_PMS_EXCEPTION_MODE_V  0x00000003U
#define HP_PERI0_PMS_HP_CPUPERI1_PMS_EXCEPTION_MODE_S  8

/** HP_PERI0_PMS_HP_CPUPERI1_1_REG register
 *  HP_CPUPERI1 PMS exception addr record register
 */
#define HP_PERI0_PMS_HP_CPUPERI1_1_REG (DR_REG_HP_PERI0_PMS_BASE + 0x20c)
/** HP_PERI0_PMS_HP_CPUPERI1_PMS_EXCEPTION_ADDR : RO; bitpos: [27:0]; default: 0;
 *  Represents the access address (bit27~bit0) when hp_cpuperi1 pms has been triggered.
 */
#define HP_PERI0_PMS_HP_CPUPERI1_PMS_EXCEPTION_ADDR    0x0FFFFFFFU
#define HP_PERI0_PMS_HP_CPUPERI1_PMS_EXCEPTION_ADDR_M  (HP_PERI0_PMS_HP_CPUPERI1_PMS_EXCEPTION_ADDR_V << HP_PERI0_PMS_HP_CPUPERI1_PMS_EXCEPTION_ADDR_S)
#define HP_PERI0_PMS_HP_CPUPERI1_PMS_EXCEPTION_ADDR_V  0x0FFFFFFFU
#define HP_PERI0_PMS_HP_CPUPERI1_PMS_EXCEPTION_ADDR_S  0
/** HP_PERI0_PMS_HP_CPUPERI1_PMS_EXCEPTION_ADDR_CONST : HRO; bitpos: [31:28]; default:
 *  2;
 *  Represents the access address (bit31~bit28) when hp_cpuperi1 pms has been triggered.
 */
#define HP_PERI0_PMS_HP_CPUPERI1_PMS_EXCEPTION_ADDR_CONST    0x0000000FU
#define HP_PERI0_PMS_HP_CPUPERI1_PMS_EXCEPTION_ADDR_CONST_M  (HP_PERI0_PMS_HP_CPUPERI1_PMS_EXCEPTION_ADDR_CONST_V << HP_PERI0_PMS_HP_CPUPERI1_PMS_EXCEPTION_ADDR_CONST_S)
#define HP_PERI0_PMS_HP_CPUPERI1_PMS_EXCEPTION_ADDR_CONST_V  0x0000000FU
#define HP_PERI0_PMS_HP_CPUPERI1_PMS_EXCEPTION_ADDR_CONST_S  28

/** HP_PERI0_PMS_HP_PERI0_0_REG register
 *  HP_PERI0 PMS configuration & info register
 */
#define HP_PERI0_PMS_HP_PERI0_0_REG (DR_REG_HP_PERI0_PMS_BASE + 0x210)
/** HP_PERI0_PMS_HP_PERI0_PMS_EXCEPTION_CLR : WT; bitpos: [0]; default: 0;
 *  Configures whether or not to clear hp_peri0 peri_pms_record_reg.
 *  0: No clear
 *  1: Clear peri_pms_record_reg
 */
#define HP_PERI0_PMS_HP_PERI0_PMS_EXCEPTION_CLR    (BIT(0))
#define HP_PERI0_PMS_HP_PERI0_PMS_EXCEPTION_CLR_M  (HP_PERI0_PMS_HP_PERI0_PMS_EXCEPTION_CLR_V << HP_PERI0_PMS_HP_PERI0_PMS_EXCEPTION_CLR_S)
#define HP_PERI0_PMS_HP_PERI0_PMS_EXCEPTION_CLR_V  0x00000001U
#define HP_PERI0_PMS_HP_PERI0_PMS_EXCEPTION_CLR_S  0
/** HP_PERI0_PMS_HP_PERI0_PMS_EXCEPTION_DET : RO; bitpos: [1]; default: 0;
 *  Represents whether the hp_peri0 pms has been triggered.
 *  0: No triggered
 *  1: Has been triggered
 */
#define HP_PERI0_PMS_HP_PERI0_PMS_EXCEPTION_DET    (BIT(1))
#define HP_PERI0_PMS_HP_PERI0_PMS_EXCEPTION_DET_M  (HP_PERI0_PMS_HP_PERI0_PMS_EXCEPTION_DET_V << HP_PERI0_PMS_HP_PERI0_PMS_EXCEPTION_DET_S)
#define HP_PERI0_PMS_HP_PERI0_PMS_EXCEPTION_DET_V  0x00000001U
#define HP_PERI0_PMS_HP_PERI0_PMS_EXCEPTION_DET_S  1
/** HP_PERI0_PMS_HP_PERI0_PMS_EXCEPTION_ID : RO; bitpos: [7:2]; default: 0;
 *  Represents the master id when hp_peri0 pms has been triggered.
 */
#define HP_PERI0_PMS_HP_PERI0_PMS_EXCEPTION_ID    0x0000003FU
#define HP_PERI0_PMS_HP_PERI0_PMS_EXCEPTION_ID_M  (HP_PERI0_PMS_HP_PERI0_PMS_EXCEPTION_ID_V << HP_PERI0_PMS_HP_PERI0_PMS_EXCEPTION_ID_S)
#define HP_PERI0_PMS_HP_PERI0_PMS_EXCEPTION_ID_V  0x0000003FU
#define HP_PERI0_PMS_HP_PERI0_PMS_EXCEPTION_ID_S  2
/** HP_PERI0_PMS_HP_PERI0_PMS_EXCEPTION_MODE : RO; bitpos: [9:8]; default: 0;
 *  Represents the security mode when hp_peri0 pms has been triggered.
 */
#define HP_PERI0_PMS_HP_PERI0_PMS_EXCEPTION_MODE    0x00000003U
#define HP_PERI0_PMS_HP_PERI0_PMS_EXCEPTION_MODE_M  (HP_PERI0_PMS_HP_PERI0_PMS_EXCEPTION_MODE_V << HP_PERI0_PMS_HP_PERI0_PMS_EXCEPTION_MODE_S)
#define HP_PERI0_PMS_HP_PERI0_PMS_EXCEPTION_MODE_V  0x00000003U
#define HP_PERI0_PMS_HP_PERI0_PMS_EXCEPTION_MODE_S  8

/** HP_PERI0_PMS_HP_PERI0_1_REG register
 *  HP_PERI0 PMS exception addr record register
 */
#define HP_PERI0_PMS_HP_PERI0_1_REG (DR_REG_HP_PERI0_PMS_BASE + 0x214)
/** HP_PERI0_PMS_HP_PERI0_PMS_EXCEPTION_ADDR : RO; bitpos: [27:0]; default: 0;
 *  Represents the access address (bit27~bit0) when hp_peri0 pms has been triggered.
 */
#define HP_PERI0_PMS_HP_PERI0_PMS_EXCEPTION_ADDR    0x0FFFFFFFU
#define HP_PERI0_PMS_HP_PERI0_PMS_EXCEPTION_ADDR_M  (HP_PERI0_PMS_HP_PERI0_PMS_EXCEPTION_ADDR_V << HP_PERI0_PMS_HP_PERI0_PMS_EXCEPTION_ADDR_S)
#define HP_PERI0_PMS_HP_PERI0_PMS_EXCEPTION_ADDR_V  0x0FFFFFFFU
#define HP_PERI0_PMS_HP_PERI0_PMS_EXCEPTION_ADDR_S  0
/** HP_PERI0_PMS_HP_PERI0_PMS_EXCEPTION_ADDR_CONST : HRO; bitpos: [31:28]; default: 2;
 *  Represents the access address (bit31~bit28) when hp_peri0 pms has been triggered.
 */
#define HP_PERI0_PMS_HP_PERI0_PMS_EXCEPTION_ADDR_CONST    0x0000000FU
#define HP_PERI0_PMS_HP_PERI0_PMS_EXCEPTION_ADDR_CONST_M  (HP_PERI0_PMS_HP_PERI0_PMS_EXCEPTION_ADDR_CONST_V << HP_PERI0_PMS_HP_PERI0_PMS_EXCEPTION_ADDR_CONST_S)
#define HP_PERI0_PMS_HP_PERI0_PMS_EXCEPTION_ADDR_CONST_V  0x0000000FU
#define HP_PERI0_PMS_HP_PERI0_PMS_EXCEPTION_ADDR_CONST_S  28

/** HP_PERI0_PMS_INT_EN_REG register
 *  APM interrupt enable register
 */
#define HP_PERI0_PMS_INT_EN_REG (DR_REG_HP_PERI0_PMS_BASE + 0x300)
/** HP_PERI0_PMS_HP_PERI0_PMS_INT_EN : R/W; bitpos: [0]; default: 0;
 *  Configures to enable hp_peri0 pms interrupt.
 *  0: disable
 *  1: enable
 */
#define HP_PERI0_PMS_HP_PERI0_PMS_INT_EN    (BIT(0))
#define HP_PERI0_PMS_HP_PERI0_PMS_INT_EN_M  (HP_PERI0_PMS_HP_PERI0_PMS_INT_EN_V << HP_PERI0_PMS_HP_PERI0_PMS_INT_EN_S)
#define HP_PERI0_PMS_HP_PERI0_PMS_INT_EN_V  0x00000001U
#define HP_PERI0_PMS_HP_PERI0_PMS_INT_EN_S  0
/** HP_PERI0_PMS_HP_CPUPERI1_PMS_INT_EN : R/W; bitpos: [1]; default: 0;
 *  Configures to enable hp_cpuperi1 pms interrupt.
 *  0: disable
 *  1: enable
 */
#define HP_PERI0_PMS_HP_CPUPERI1_PMS_INT_EN    (BIT(1))
#define HP_PERI0_PMS_HP_CPUPERI1_PMS_INT_EN_M  (HP_PERI0_PMS_HP_CPUPERI1_PMS_INT_EN_V << HP_PERI0_PMS_HP_CPUPERI1_PMS_INT_EN_S)
#define HP_PERI0_PMS_HP_CPUPERI1_PMS_INT_EN_V  0x00000001U
#define HP_PERI0_PMS_HP_CPUPERI1_PMS_INT_EN_S  1
/** HP_PERI0_PMS_HP_CPUPERI0_PMS_INT_EN : R/W; bitpos: [2]; default: 0;
 *  Configures to enable hp_cpuperi0 pms interrupt.
 *  0: disable
 *  1: enable
 */
#define HP_PERI0_PMS_HP_CPUPERI0_PMS_INT_EN    (BIT(2))
#define HP_PERI0_PMS_HP_CPUPERI0_PMS_INT_EN_M  (HP_PERI0_PMS_HP_CPUPERI0_PMS_INT_EN_V << HP_PERI0_PMS_HP_CPUPERI0_PMS_INT_EN_S)
#define HP_PERI0_PMS_HP_CPUPERI0_PMS_INT_EN_V  0x00000001U
#define HP_PERI0_PMS_HP_CPUPERI0_PMS_INT_EN_S  2

/** HP_PERI0_PMS_BUS_ERR_CONF_REG register
 *  Clock gating register
 */
#define HP_PERI0_PMS_BUS_ERR_CONF_REG (DR_REG_HP_PERI0_PMS_BASE + 0xff0)
/** HP_PERI0_PMS_BUS_ERR_RESP_EN : R/W; bitpos: [0]; default: 0;
 *  Configures whether return error response to cpu when access blocked
 *  0: disable error response
 *  1: enable error response
 */
#define HP_PERI0_PMS_BUS_ERR_RESP_EN    (BIT(0))
#define HP_PERI0_PMS_BUS_ERR_RESP_EN_M  (HP_PERI0_PMS_BUS_ERR_RESP_EN_V << HP_PERI0_PMS_BUS_ERR_RESP_EN_S)
#define HP_PERI0_PMS_BUS_ERR_RESP_EN_V  0x00000001U
#define HP_PERI0_PMS_BUS_ERR_RESP_EN_S  0

/** HP_PERI0_PMS_CLOCK_GATE_REG register
 *  Clock gating register
 */
#define HP_PERI0_PMS_CLOCK_GATE_REG (DR_REG_HP_PERI0_PMS_BASE + 0xff8)
/** HP_PERI0_PMS_CLK_EN : R/W; bitpos: [0]; default: 1;
 *  Configures whether to keep the clock always on.
 *  0: enable automatic clock gating
 *  1: keep the clock always on
 */
#define HP_PERI0_PMS_CLK_EN    (BIT(0))
#define HP_PERI0_PMS_CLK_EN_M  (HP_PERI0_PMS_CLK_EN_V << HP_PERI0_PMS_CLK_EN_S)
#define HP_PERI0_PMS_CLK_EN_V  0x00000001U
#define HP_PERI0_PMS_CLK_EN_S  0

/** HP_PERI0_PMS_DATE_REG register
 *  Version control register
 */
#define HP_PERI0_PMS_DATE_REG (DR_REG_HP_PERI0_PMS_BASE + 0xffc)
/** HP_PERI0_PMS_DATE : R/W; bitpos: [27:0]; default: 38834736;
 *  Version control register
 */
#define HP_PERI0_PMS_DATE    0x0FFFFFFFU
#define HP_PERI0_PMS_DATE_M  (HP_PERI0_PMS_DATE_V << HP_PERI0_PMS_DATE_S)
#define HP_PERI0_PMS_DATE_V  0x0FFFFFFFU
#define HP_PERI0_PMS_DATE_S  0

#ifdef __cplusplus
}
#endif
