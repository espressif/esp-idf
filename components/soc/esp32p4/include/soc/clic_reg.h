/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "soc/soc.h"
#ifdef __cplusplus
extern "C" {
#endif

#define NLBITS                   3
#define CLIC_EXT_INTR_NUM_OFFSET 16
#define DUALCORE_CLIC_CTRL_OFF   0x10000

#define DR_REG_CLIC_BASE      (0x20800000)
#define DR_REG_CLIC_CTRL_BASE (0x20801000)

#define CLIC_INT_CONFIG_REG          (DR_REG_CLIC_BASE + 0x0)
/* CLIC_INT_CONFIG_NMBITS : RO ;bitpos:[6:5] ;default: 2'd0 ; */
/*description: the effective number of bits in the privileged state.*/
#define CLIC_INT_CONFIG_NMBITS    0x00000003
#define CLIC_INT_CONFIG_NMBITS_M  ((CLIC_INT_CONFIG_NMBITS_V) << (CLIC_INT_CONFIG_NMBITS_S))
#define CLIC_INT_CONFIG_NMBITS_V  0x3
#define CLIC_INT_CONFIG_NMBITS_S  5
/* CLIC_INT_CONFIG_NLBITS : R/W ;bitpos:[4:1] ;default: 4'd0 ; */
/*description: interrupt priority effective digits, the maximum value is 8.*/
#define CLIC_INT_CONFIG_MNLBITS    0x0000000F
#define CLIC_INT_CONFIG_MNLBITS_M  ((CLIC_INT_CONFIG_MNLBITS_V) << (CLIC_INT_CONFIG_MNLBITS_S))
#define CLIC_INT_CONFIG_MNLBITS_V  0xF
#define CLIC_INT_CONFIG_MNLBITS_S  1
/* CLIC_INT_CONFIG_NVBITS : RO ;bitpos:[0] ;default: 1'd1 ; */
/*description: hardware vector interrupt implementation flag bit.*/
#define CLIC_INT_CONFIG_NVBITS    (BIT(0))
#define CLIC_INT_CONFIG_NVBITS_M  (BIT(0))
#define CLIC_INT_CONFIG_NVBITS_V  0x1
#define CLIC_INT_CONFIG_NVBITS_S  0

#define CLIC_INT_INFO_REG          (DR_REG_CLIC_BASE + 0x4)
/* CLIC_INT_INFO_NUM_INT : RO ;bitpos:[24:21] ;default: 4'd3 ; */
/*description: The effective bits of priority in the CLICINTCTL register.*/
#define CLIC_INT_INFO_CTLBITS    0x0000000F
#define CLIC_INT_INFO_CTLBITS_M  ((CLIC_INT_INFO_CTLBITS_V) << (CLIC_INT_INFO_CTLBITS_S))
#define CLIC_INT_INFO_CTLBITS_V  0xF
#define CLIC_INT_INFO_CTLBITS_S  21
/* CLIC_INT_INFO_VERSION : RO ;bitpos:[20:13] ;default: 8'd0 ; */
/*description: The lower 4 bits are the modified version of the hardware implementation; the upper 4 bits are the CLIC
architecture version information.*/
#define CLIC_INT_INFO_VERSION    0x000000FF
#define CLIC_INT_INFO_VERSION_M  ((CLIC_INT_INFO_VERSION_V) << (CLIC_INT_INFO_VERSION_S))
#define CLIC_INT_INFO_VERSION_V  0xFF
#define CLIC_INT_INFO_VERSION_S  13
/* CLIC_INT_INFO_NUM_INT : RO ;bitpos:[12:0] ;default: 13'd48 ; */
/*description: number of interrupt sources.*/
#define CLIC_INT_INFO_NUM_INT    0x00001FFF
#define CLIC_INT_INFO_NUM_INT_M  ((CLIC_INT_INFO_NUM_INT_V) << (CLIC_INT_INFO_NUM_INT_S))
#define CLIC_INT_INFO_NUM_INT_V  0x1FFF
#define CLIC_INT_INFO_NUM_INT_S  0

#define CLIC_INT_THRESH_REG          (DR_REG_CLIC_BASE + 0x8)
/* CLIC_CPU_INT_THRESH : R/W ;bitpos:[31:24] ;default: 8'd0 ; */
/*description: Threshold for machine mode interruption..*/
#define CLIC_CPU_INT_THRESH    0x000000FF
#define CLIC_CPU_INT_THRESH_M  ((CLIC_CPU_INT_THRESH_V) << (CLIC_CPU_INT_THRESH_S))
#define CLIC_CPU_INT_THRESH_V  0xFF
#define CLIC_CPU_INT_THRESH_S  24

#define CLIC_INT_CTRL_REG(i)          (DR_REG_CLIC_CTRL_BASE + (i)*4)
/* CLIC_INT_CTL : R/W ;bitpos:[31:24] ;default: 8'h1f ; */
/*description: This register is used to indicate the priority of each interrupt
source participating in the arbitration, and at the same time to cooperate with
CLICCFG.nlbits to generate the interrupt priority to the CPU.*/
#define CLIC_INT_CTL    0x000000FF
#define CLIC_INT_CTL_M  ((CLIC_INT_CTL_V) << (CLIC_INT_CTL_S))
#define CLIC_INT_CTL_V  0xFF
#define CLIC_INT_CTL_S  24
/* CLIC_INT_ATTR_MODE : RO ;bitpos:[23:22] ;default: 2'b11 ; */
/*description: This field is used to configure the privileged mode of the interrupt.*/
#define CLIC_INT_ATTR_MODE    0x00000003
#define CLIC_INT_ATTR_MODE_M  ((CLIC_INT_ATTR_MODE_V) << (CLIC_INT_ATTR_MODE_S))
#define CLIC_INT_ATTR_MODE_V  0x3
#define CLIC_INT_ATTR_MODE_S  22
/* CLIC_INT_ATTR_TRIG : R/W ;bitpos:[18:17] ;default: 2'd0 ; */
/*description: This field is used to distinguish pulse interruption and level
interruption. When trig[0] is 0, it means level interruption. When trig[0] is 1,
trig[1] is 0, it means interrupt on rising edge, and trig[1] is 1 means interrupt
on falling edge.*/
#define CLIC_INT_ATTR_TRIG    0x00000003
#define CLIC_INT_ATTR_TRIG_M  ((CLIC_INT_ATTR_TRIG_V) << (CLIC_INT_ATTR_TRIG_S))
#define CLIC_INT_ATTR_TRIG_V  0x3
#define CLIC_INT_ATTR_TRIG_S  17
/* CLIC_INT_ATTR_SHV : R/W ;bitpos:[16] ;default: 1'd0 ; */
/*description: Represents whether the interrupt is a hardware vector interrupt.*/
#define CLIC_INT_ATTR_SHV    (BIT(16))
#define CLIC_INT_ATTR_SHV_M  (BIT(16))
#define CLIC_INT_ATTR_SHV_V  0x1
#define CLIC_INT_ATTR_SHV_S  16
/* CLIC_INT_IE : R/W ;bitpos:[8] ;default: 1'd0 ; */
/*description: Interrupt Enable Register.*/
#define CLIC_INT_IE    (BIT(8))
#define CLIC_INT_IE_M  (BIT(8))
#define CLIC_INT_IE_V  0x1
#define CLIC_INT_IE_S  8
/* CLIC_INT_IP : R/W ;bitpos:[0] ;default: 1'd0 ; */
/*description: Interrupt Pending Register. This bit has different set and clear logic in the
case of level interrupt and edge interrupt*/
#define CLIC_INT_IP    (BIT(0))
#define CLIC_INT_IP_M  (BIT(0))
#define CLIC_INT_IP_V  0x1
#define CLIC_INT_IP_S  0

// each of following registers are 8bits
#define BYTE_CLIC_INT_IP_REG(i)          (DR_REG_CLIC_CTRL_BASE + (i) * 4)
/* BYTE_CLIC_INT_IP : R/W ;bitpos:[0] ;default: 1'd0 ; */
/*description: Interrupt Pending Register. This bit has different set and clear logic in the
case of level interrupt and edge interrupt*/
#define BYTE_CLIC_INT_IP    (BIT(0))
#define BYTE_CLIC_INT_IP_M  (BIT(0))
#define BYTE_CLIC_INT_IP_V  0x1
#define BYTE_CLIC_INT_IP_S  0

#define BYTE_CLIC_INT_IE_REG(i)          (DR_REG_CLIC_CTRL_BASE + 1 + (i) * 4)
/* BYTE_CLIC_INT_IE : R/W ;bitpos:[0] ;default: 1'd0 ; */
/*description: Interrupt Enable Register.*/
#define BYTE_CLIC_INT_IE    (BIT(0))
#define BYTE_CLIC_INT_IE_M  (BIT(0))
#define BYTE_CLIC_INT_IE_V  0x1
#define BYTE_CLIC_INT_IE_S  0

#define BYTE_CLIC_INT_ATTR_REG(i)          (DR_REG_CLIC_CTRL_BASE + 2 + (i) * 4)
/* BYTE_CLIC_INT_ATTR_SHV : R/W ;bitpos:[0] ;default: 1'd0 ; */
/*description: 1 means hardware vector interrupt.*/
#define BYTE_CLIC_INT_ATTR_SHV    (BIT(0))
#define BYTE_CLIC_INT_ATTR_SHV_M  (BIT(0))
#define BYTE_CLIC_INT_ATTR_SHV_V  0x1
#define BYTE_CLIC_INT_ATTR_SHV_S  0
/* BYTE_CLIC_INT_ATTR_TRIG: R/W ; bitpos:[2:1] ;default: 2'd0 ; */
/*description:
    [X0] -> level trigger
    [01] -> rising edge trigger
    [11] -> falling edge trigger */
#define BYTE_CLIC_INT_ATTR_TRIG    0x00000003
#define BYTE_CLIC_INT_ATTR_TRIG_M  ((BYTE_CLIC_INT_ATTR_TRIG_V) << (BYTE_CLIC_INT_ATTR_TRIG_S))
#define BYTE_CLIC_INT_ATTR_TRIG_V  0x3
#define BYTE_CLIC_INT_ATTR_TRIG_S  1
/* BYTE_CLIC_INT_ATTR_MODE: RO ; bitpos:[7:6] ;default: 2'd0 ; */
/*description: privilege level for interrupt, fixed to 2'b11 */
#define BYTE_CLIC_INT_ATTR_MODE    0x00000003
#define BYTE_CLIC_INT_ATTR_MODE_M  ((BYTE_CLIC_INT_ATTR_MODE_V) << (BYTE_CLIC_INT_ATTR_MODE_S))
#define BYTE_CLIC_INT_ATTR_MODE_V  0x3
#define BYTE_CLIC_INT_ATTR_MODE_S  6

#define BYTE_CLIC_INT_CTL_REG(i)          (DR_REG_CLIC_CTRL_BASE + 3 + (i) * 4)
/* BYTE_CLIC_INT_ATTR_MODE: R/W ; bitpos:[7:5] ;default: 3'd0 ; */
/*description: interrupt priority */
#define BYTE_CLIC_INT_CTL    0x00000007
#define BYTE_CLIC_INT_CTL_M  ((BYTE_CLIC_INT_CTL_V) << (BYTE_CLIC_INT_CTL_S))
#define BYTE_CLIC_INT_CTL_V  0x7
#define BYTE_CLIC_INT_CTL_S  5

#ifdef __cplusplus
}
#endif
