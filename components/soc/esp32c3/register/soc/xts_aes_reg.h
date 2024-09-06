/**
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 *  SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#include "soc/soc.h"
#ifdef __cplusplus
extern "C" {
#endif

/** XTS_AES_PLAIN_MEM register
 *  The memory that stores plaintext
 */
#define XTS_AES_PLAIN_MEM (DR_REG_XTS_AES_BASE + 0x0)
#define XTS_AES_PLAIN_MEM_SIZE_BYTES 16

/** XTS_AES_LINESIZE_REG register
 *  XTS-AES line-size register
 */
#define XTS_AES_LINESIZE_REG (DR_REG_XTS_AES_BASE + 0x40)
/** XTS_AES_LINESIZE : R/W; bitpos: [0]; default: 0;
 *  This bit stores the line size parameter. 0: 16Byte, 1: 32Byte.
 */
#define XTS_AES_LINESIZE    (BIT(0))
#define XTS_AES_LINESIZE_M  (XTS_AES_LINESIZE_V << XTS_AES_LINESIZE_S)
#define XTS_AES_LINESIZE_V  0x00000001U
#define XTS_AES_LINESIZE_S  0

/** XTS_AES_DESTINATION_REG register
 *  XTS-AES destination register
 */
#define XTS_AES_DESTINATION_REG (DR_REG_XTS_AES_BASE + 0x44)
/** XTS_AES_DESTINATION : R/W; bitpos: [0]; default: 0;
 *  This bit stores the destination. 0: flash(default). 1: reserved.
 */
#define XTS_AES_DESTINATION    (BIT(0))
#define XTS_AES_DESTINATION_M  (XTS_AES_DESTINATION_V << XTS_AES_DESTINATION_S)
#define XTS_AES_DESTINATION_V  0x00000001U
#define XTS_AES_DESTINATION_S  0

/** XTS_AES_PHYSICAL_ADDRESS_REG register
 *  XTS-AES physical address register
 */
#define XTS_AES_PHYSICAL_ADDRESS_REG (DR_REG_XTS_AES_BASE + 0x48)
/** XTS_AES_PHYSICAL_ADDRESS : R/W; bitpos: [29:0]; default: 0;
 *  Those bits stores the physical address. If linesize is 16-byte, the physical
 *  address should be aligned of 16 bytes. If linesize is 32-byte, the physical address
 *  should be aligned of 32 bytes.
 */
#define XTS_AES_PHYSICAL_ADDRESS    0x3FFFFFFFU
#define XTS_AES_PHYSICAL_ADDRESS_M  (XTS_AES_PHYSICAL_ADDRESS_V << XTS_AES_PHYSICAL_ADDRESS_S)
#define XTS_AES_PHYSICAL_ADDRESS_V  0x3FFFFFFFU
#define XTS_AES_PHYSICAL_ADDRESS_S  0

/** XTS_AES_TRIGGER_REG register
 *  XTS-AES trigger register
 */
#define XTS_AES_TRIGGER_REG (DR_REG_XTS_AES_BASE + 0x4c)
/** XTS_AES_TRIGGER : WT; bitpos: [0]; default: 0;
 *  Set this bit to start manual encryption calculation
 */
#define XTS_AES_TRIGGER    (BIT(0))
#define XTS_AES_TRIGGER_M  (XTS_AES_TRIGGER_V << XTS_AES_TRIGGER_S)
#define XTS_AES_TRIGGER_V  0x00000001U
#define XTS_AES_TRIGGER_S  0

/** XTS_AES_RELEASE_REG register
 *  XTS-AES release register
 */
#define XTS_AES_RELEASE_REG (DR_REG_XTS_AES_BASE + 0x50)
/** XTS_AES_RELEASE : WT; bitpos: [0]; default: 0;
 *  Set this bit to release the manual encrypted result, after that the result will be
 *  visible to spi
 */
#define XTS_AES_RELEASE    (BIT(0))
#define XTS_AES_RELEASE_M  (XTS_AES_RELEASE_V << XTS_AES_RELEASE_S)
#define XTS_AES_RELEASE_V  0x00000001U
#define XTS_AES_RELEASE_S  0

/** XTS_AES_DESTROY_REG register
 *  XTS-AES destroy register
 */
#define XTS_AES_DESTROY_REG (DR_REG_XTS_AES_BASE + 0x54)
/** XTS_AES_DESTROY : WT; bitpos: [0]; default: 0;
 *  Set this bit to destroy XTS-AES result.
 */
#define XTS_AES_DESTROY    (BIT(0))
#define XTS_AES_DESTROY_M  (XTS_AES_DESTROY_V << XTS_AES_DESTROY_S)
#define XTS_AES_DESTROY_V  0x00000001U
#define XTS_AES_DESTROY_S  0

/** XTS_AES_STATE_REG register
 *  XTS-AES status register
 */
#define XTS_AES_STATE_REG (DR_REG_XTS_AES_BASE + 0x58)
/** XTS_AES_STATE : RO; bitpos: [1:0]; default: 0;
 *  Those bits shows XTS-AES status. 0=IDLE, 1=WORK, 2=RELEASE, 3=USE. IDLE means that
 *  XTS-AES is idle. WORK means that XTS-AES is busy with calculation. RELEASE means
 *  the encrypted result is generated but not visible to mspi. USE means that the
 *  encrypted result is visible to mspi.
 */
#define XTS_AES_STATE    0x00000003U
#define XTS_AES_STATE_M  (XTS_AES_STATE_V << XTS_AES_STATE_S)
#define XTS_AES_STATE_V  0x00000003U
#define XTS_AES_STATE_S  0

/** XTS_AES_DATE_REG register
 *  XTS-AES version control register
 */
#define XTS_AES_DATE_REG (DR_REG_XTS_AES_BASE + 0x5c)
/** XTS_AES_DATE : R/W; bitpos: [29:0]; default: 538969635;
 *  Those bits stores the version information of XTS-AES.
 */
#define XTS_AES_DATE    0x3FFFFFFFU
#define XTS_AES_DATE_M  (XTS_AES_DATE_V << XTS_AES_DATE_S)
#define XTS_AES_DATE_V  0x3FFFFFFFU
#define XTS_AES_DATE_S  0

/* For backward compatibility with the older register names */
#define AES_XTS_PLAIN_BASE          XTS_AES_PLAIN_MEM
#define AES_XTS_SIZE_REG            XTS_AES_LINESIZE_REG
#define AES_XTS_DESTINATION_REG     XTS_AES_DESTINATION_REG
#define AES_XTS_PHYSICAL_ADDR_REG   XTS_AES_PHYSICAL_ADDRESS_REG
#define AES_XTS_TRIGGER_REG         XTS_AES_TRIGGER_REG
#define AES_XTS_RELEASE_REG         XTS_AES_RELEASE_REG
#define AES_XTS_DESTROY_REG         XTS_AES_DESTROY_REG
#define AES_XTS_STATE_REG           XTS_AES_STATE_REG
#define AES_XTS_DATE_REG            XTS_AES_DATE_REG

#ifdef __cplusplus
}
#endif
