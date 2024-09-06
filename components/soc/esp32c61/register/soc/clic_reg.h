/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#define NLBITS                   3
#define CLIC_EXT_INTR_NUM_OFFSET 16

#define DR_REG_CLIC_BASE      (0x20800000)
#define DR_REG_CLIC_CTRL_BASE (0x20801000)

#define CLIC_INT_CONFIG_REG (DR_REG_CLIC_BASE + 0x0)
/* CLIC_INT_CONFIG_NMBITS : R/W ;bitpos:[6:5] ;default: 2'd0 ; */
/*description: .*/
#define CLIC_INT_CONFIG_NMBITS   0x00000003
#define CLIC_INT_CONFIG_NMBITS_M ((CLIC_INT_CONFIG_NMBITS_V) << (CLIC_INT_CONFIG_NMBITS_S))
#define CLIC_INT_CONFIG_NMBITS_V 0x3
#define CLIC_INT_CONFIG_NMBITS_S 5
/* CLIC_INT_CONFIG_NLBITS : R/W ;bitpos:[4:1] ;default: 4'd0 ; */
/*description: .*/
#define CLIC_INT_CONFIG_NLBITS   0x0000000F
#define CLIC_INT_CONFIG_NLBITS_M ((CLIC_INT_CONFIG_NLBITS_V) << (CCLIC_INT_CONFIG_NLBITS_S))
#define CLIC_INT_CONFIG_NLBITS_V 0xF
#define CLIC_INT_CONFIG_NLBITS_S 1
/* CLIC_INT_CONFIG_NVBITS : R/W ;bitpos:[0] ;default: 1'd1 ; */
/*description: .*/
#define CLIC_INT_CONFIG_NVBITS   (BIT(0))
#define CLIC_INT_CONFIG_NVBITS_M (BIT(0))
#define CLIC_INT_CONFIG_NVBITS_V 0x1
#define CLIC_INT_CONFIG_NVBITS_S 0

#define CLIC_INT_INFO_REG (DR_REG_CLIC_BASE + 0x4)
/* CLIC_INT_INFO_NUM_INT : R/W ;bitpos:[24:21] ;default: 4'd0 ; */
/*description: .*/
#define CLIC_INT_INFO_CTLBITS   0x0000000F
#define CLIC_INT_INFO_CTLBITS_M ((CLIC_INT_INFO_CTLBITS_V) << (CLIC_INT_INFO_CTLBITS_S))
#define CLIC_INT_INFO_CTLBITS_V 0xF
#define CLIC_INT_INFO_CTLBITS_S 21
/* CLIC_INT_INFO_VERSION : R/W ;bitpos:[20:13] ;default: 8'd0 ; */
/*description: .*/
#define CLIC_INT_INFO_VERSION   0x000000FF
#define CLIC_INT_INFO_VERSION_M ((CLIC_INT_INFO_VERSION_V) << (CLIC_INT_INFO_VERSION_S))
#define CLIC_INT_INFO_VERSION_V 0xFF
#define CLIC_INT_INFO_VERSION_S 13
/* CLIC_INT_INFO_NUM_INT : R/W ;bitpos:[12:0] ;default: 13'd0 ; */
/*description: .*/
#define CLIC_INT_INFO_NUM_INT   0x00001FFF
#define CLIC_INT_INFO_NUM_INT_M ((CLIC_INT_INFO_NUM_INT_V) << (CLIC_INT_INFO_NUM_INT_S))
#define CLIC_INT_INFO_NUM_INT_V 0x1FFF
#define CLIC_INT_INFO_NUM_INT_S 0

#define CLIC_INT_THRESH_REG (DR_REG_CLIC_BASE + 0x8)
/* CLIC_CPU_INT_THRESH : R/W ;bitpos:[31:24] ;default: 8'd0 ; */
/*description: .*/
#define CLIC_CPU_INT_THRESH   0x000000FF
#define CLIC_CPU_INT_THRESH_M ((CLIC_CPU_INT_THRESH_V) << (CLIC_CPU_INT_THRESH_S))
#define CLIC_CPU_INT_THRESH_V 0xFF
#define CLIC_CPU_INT_THRESH_S 24

#define CLIC_INT_CTRL_REG(i) (DR_REG_CLIC_CTRL_BASE + (i)*4)
/* CLIC_INT_CTL : R/W ;bitpos:[31:24] ;default: 8'd0 ; */
/*description: .*/
#define CLIC_INT_CTL   0x000000FF
#define CLIC_INT_CTL_M ((CLIC_INT_CTL_V) << (CLIC_INT_CTL_S))
#define CLIC_INT_CTL_V 0xFF
#define CLIC_INT_CTL_S 24
/* CLIC_INT_ATTR_MODE : R/W ;bitpos:[23:22] ;default: 2'b11 ; */
/*description: .*/
#define CLIC_INT_ATTR_MODE   0x00000003
#define CLIC_INT_ATTR_MODE_M ((CLIC_INT_ATTR_MODE_V) << (CLIC_INT_ATTR_MODE_S))
#define CLIC_INT_ATTR_MODE_V 0x3
#define CLIC_INT_ATTR_MODE_S 22
/* CLIC_INT_ATTR_TRIG : R/W ;bitpos:[18:17] ;default: 2'd0 ; */
/*description: .*/
#define CLIC_INT_ATTR_TRIG   0x00000003
#define CLIC_INT_ATTR_TRIG_M ((CLIC_INT_ATTR_TRIG_V) << (CLIC_INT_ATTR_TRIG_S))
#define CLIC_INT_ATTR_TRIG_V 0x3
#define CLIC_INT_ATTR_TRIG_S 17
/* CLIC_INT_ATTR_SHV : R/W ;bitpos:[16] ;default: 1'd0 ; */
/*description: .*/
#define CLIC_INT_ATTR_SHV   (BIT(16))
#define CLIC_INT_ATTR_SHV_M (BIT(16))
#define CLIC_INT_ATTR_SHV_V 0x1
#define CLIC_INT_ATTR_SHV_S 16
/* CLIC_INT_IE : R/W ;bitpos:[8] ;default: 1'd0 ; */
/*description: .*/
#define CLIC_INT_IE   (BIT(8))
#define CLIC_INT_IE_M (BIT(8))
#define CLIC_INT_IE_V 0x1
#define CLIC_INT_IE_S 8
/* CLIC_INT_IP : R/W ;bitpos:[0] ;default: 1'd0 ; */
/*description: .*/
#define CLIC_INT_IP   (BIT(0))
#define CLIC_INT_IP_M (BIT(0))
#define CLIC_INT_IP_V 0x1
#define CLIC_INT_IP_S 0

// each of following registers are 8bits
#define BYTE_CLIC_INT_IP_REG(i) (DR_REG_CLIC_CTRL_BASE + (i)*4)
/* BYTE_CLIC_INT_IP : R/W ;bitpos:[0] ;default: 1'd0 ; */
/*description: .*/
#define BYTE_CLIC_INT_IP   (BIT(0))
#define BYTE_CLIC_INT_IP_M (BIT(0))
#define BYTE_CLIC_INT_IP_V 0x1
#define BYTE_CLIC_INT_IP_S 0

#define BYTE_CLIC_INT_IE_REG(i) (DR_REG_CLIC_CTRL_BASE + 1 + (i)*4)
/* BYTE_CLIC_INT_IE : R/W ;bitpos:[0] ;default: 1'd0 ; */
/*description: .*/
#define BYTE_CLIC_INT_IE   (BIT(0))
#define BYTE_CLIC_INT_IE_M (BIT(0))
#define BYTE_CLIC_INT_IE_V 0x1
#define BYTE_CLIC_INT_IE_S 0

#define BYTE_CLIC_INT_ATTR_REG(i) (DR_REG_CLIC_CTRL_BASE + 2 + (i)*4)
/* BYTE_CLIC_INT_ATTR_SHV : R/W ;bitpos:[0] ;default: 1'd0 ; */
/*description: 1 means hardware vector interrupt.*/
#define BYTE_CLIC_INT_ATTR_SHV   (BIT(0))
#define BYTE_CLIC_INT_ATTR_SHV_M (BIT(0))
#define BYTE_CLIC_INT_ATTR_SHV_V 0x1
#define BYTE_CLIC_INT_ATTR_SHV_S 0
/* BYTE_CLIC_INT_ATTR_TRIG: R/W ; bitpos:[2:1] ;default: 2'd0 ; */
/*description:
    [X0] -> level trigger
    [01] -> rising edge trigger
    [11] -> falling edge trigger */
#define BYTE_CLIC_INT_ATTR_TRIG   0x00000003
#define BYTE_CLIC_INT_ATTR_TRIG_M ((BYTE_CLIC_INT_ATTR_TRIG_V) << (BYTE_CLIC_INT_ATTR_TRIG_S))
#define BYTE_CLIC_INT_ATTR_TRIG_V 0x3
#define BYTE_CLIC_INT_ATTR_TRIG_S 1
/* BYTE_CLIC_INT_ATTR_MODE: R/W ; bitpos:[7:6] ;default: 2'd0 ; */
/*description: privilege level for interrupt, fixed to 2'b11 */
#define BYTE_CLIC_INT_ATTR_MODE   0x00000003
#define BYTE_CLIC_INT_ATTR_MODE_M ((BYTE_CLIC_INT_ATTR_MODE_V) << (BYTE_CLIC_INT_ATTR_MODE_S))
#define BYTE_CLIC_INT_ATTR_MODE_V 0x3
#define BYTE_CLIC_INT_ATTR_MODE_S 6

#define BYTE_CLIC_INT_CTL_REG(i) (DR_REG_CLIC_CTRL_BASE + 3 + (i)*4)
/* BYTE_CLIC_INT_ATTR_MODE: R/W ; bitpos:[7:5] ;default: 3'd0 ; */
/*description: interrupt priority */
#define BYTE_CLIC_INT_CTL   0x00000007
#define BYTE_CLIC_INT_CTL_M ((BYTE_CLIC_INT_CTL_V) << (BYTE_CLIC_INT_CTL_S))
#define BYTE_CLIC_INT_CTL_V 0x7
#define BYTE_CLIC_INT_CTL_S 5

#ifdef __cplusplus
}
#endif
