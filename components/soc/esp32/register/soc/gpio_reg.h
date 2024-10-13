/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef _SOC_GPIO_REG_H_
#define _SOC_GPIO_REG_H_

#include "soc/soc.h"
#define GPIO_BT_SELECT_REG          (DR_REG_GPIO_BASE + 0x0000)
/* GPIO_BT_SEL : R/W ;bitpos:[31:0] ;default: x ; */
/*description: NA*/
#define GPIO_BT_SEL  0xFFFFFFFF
#define GPIO_BT_SEL_M  ((GPIO_BT_SEL_V)<<(GPIO_BT_SEL_S))
#define GPIO_BT_SEL_V  0xFFFFFFFF
#define GPIO_BT_SEL_S  0

#define GPIO_OUT_REG          (DR_REG_GPIO_BASE + 0x0004)
/* GPIO_OUT_DATA : R/W ;bitpos:[31:0] ;default: x ; */
/*description: GPIO0~31 output value*/
#define GPIO_OUT_DATA  0xFFFFFFFF
#define GPIO_OUT_DATA_M  ((GPIO_OUT_DATA_V)<<(GPIO_OUT_DATA_S))
#define GPIO_OUT_DATA_V  0xFFFFFFFF
#define GPIO_OUT_DATA_S  0

#define GPIO_OUT_W1TS_REG          (DR_REG_GPIO_BASE + 0x0008)
/* GPIO_OUT_DATA_W1TS : R/W ;bitpos:[31:0] ;default: x ; */
/*description: GPIO0~31 output value write 1 to set*/
#define GPIO_OUT_DATA_W1TS  0xFFFFFFFF
#define GPIO_OUT_DATA_W1TS_M  ((GPIO_OUT_DATA_W1TS_V)<<(GPIO_OUT_DATA_W1TS_S))
#define GPIO_OUT_DATA_W1TS_V  0xFFFFFFFF
#define GPIO_OUT_DATA_W1TS_S  0

#define GPIO_OUT_W1TC_REG          (DR_REG_GPIO_BASE + 0x000c)
/* GPIO_OUT_DATA_W1TC : R/W ;bitpos:[31:0] ;default: x ; */
/*description: GPIO0~31 output value write 1 to clear*/
#define GPIO_OUT_DATA_W1TC  0xFFFFFFFF
#define GPIO_OUT_DATA_W1TC_M  ((GPIO_OUT_DATA_W1TC_V)<<(GPIO_OUT_DATA_W1TC_S))
#define GPIO_OUT_DATA_W1TC_V  0xFFFFFFFF
#define GPIO_OUT_DATA_W1TC_S  0

#define GPIO_OUT1_REG          (DR_REG_GPIO_BASE + 0x0010)
/* GPIO_OUT1_DATA : R/W ;bitpos:[7:0] ;default: x ; */
/*description: GPIO32~39 output value*/
#define GPIO_OUT1_DATA  0x000000FF
#define GPIO_OUT1_DATA_M  ((GPIO_OUT1_DATA_V)<<(GPIO_OUT1_DATA_S))
#define GPIO_OUT1_DATA_V  0xFF
#define GPIO_OUT1_DATA_S  0

#define GPIO_OUT1_W1TS_REG          (DR_REG_GPIO_BASE + 0x0014)
/* GPIO_OUT1_DATA_W1TS : R/W ;bitpos:[7:0] ;default: x ; */
/*description: GPIO32~39 output value write 1 to set*/
#define GPIO_OUT1_DATA_W1TS  0x000000FF
#define GPIO_OUT1_DATA_W1TS_M  ((GPIO_OUT1_DATA_W1TS_V)<<(GPIO_OUT1_DATA_W1TS_S))
#define GPIO_OUT1_DATA_W1TS_V  0xFF
#define GPIO_OUT1_DATA_W1TS_S  0

#define GPIO_OUT1_W1TC_REG          (DR_REG_GPIO_BASE + 0x0018)
/* GPIO_OUT1_DATA_W1TC : R/W ;bitpos:[7:0] ;default: x ; */
/*description: GPIO32~39 output value write 1 to clear*/
#define GPIO_OUT1_DATA_W1TC  0x000000FF
#define GPIO_OUT1_DATA_W1TC_M  ((GPIO_OUT1_DATA_W1TC_V)<<(GPIO_OUT1_DATA_W1TC_S))
#define GPIO_OUT1_DATA_W1TC_V  0xFF
#define GPIO_OUT1_DATA_W1TC_S  0

#define GPIO_SDIO_SELECT_REG          (DR_REG_GPIO_BASE + 0x001c)
/* GPIO_SDIO_SEL : R/W ;bitpos:[7:0] ;default: x ; */
/*description: SDIO PADS on/off control from outside*/
#define GPIO_SDIO_SEL  0x000000FF
#define GPIO_SDIO_SEL_M  ((GPIO_SDIO_SEL_V)<<(GPIO_SDIO_SEL_S))
#define GPIO_SDIO_SEL_V  0xFF
#define GPIO_SDIO_SEL_S  0

#define GPIO_ENABLE_REG          (DR_REG_GPIO_BASE + 0x0020)
/* GPIO_ENABLE_DATA : R/W ;bitpos:[31:0] ;default: x ; */
/*description: GPIO0~31 output enable*/
#define GPIO_ENABLE_DATA  0xFFFFFFFF
#define GPIO_ENABLE_DATA_M  ((GPIO_ENABLE_DATA_V)<<(GPIO_ENABLE_DATA_S))
#define GPIO_ENABLE_DATA_V  0xFFFFFFFF
#define GPIO_ENABLE_DATA_S  0

#define GPIO_ENABLE_W1TS_REG          (DR_REG_GPIO_BASE + 0x0024)
/* GPIO_ENABLE_DATA_W1TS : R/W ;bitpos:[31:0] ;default: x ; */
/*description: GPIO0~31 output enable write 1 to set*/
#define GPIO_ENABLE_DATA_W1TS  0xFFFFFFFF
#define GPIO_ENABLE_DATA_W1TS_M  ((GPIO_ENABLE_DATA_W1TS_V)<<(GPIO_ENABLE_DATA_W1TS_S))
#define GPIO_ENABLE_DATA_W1TS_V  0xFFFFFFFF
#define GPIO_ENABLE_DATA_W1TS_S  0

#define GPIO_ENABLE_W1TC_REG          (DR_REG_GPIO_BASE + 0x0028)
/* GPIO_ENABLE_DATA_W1TC : R/W ;bitpos:[31:0] ;default: x ; */
/*description: GPIO0~31 output enable write 1 to clear*/
#define GPIO_ENABLE_DATA_W1TC  0xFFFFFFFF
#define GPIO_ENABLE_DATA_W1TC_M  ((GPIO_ENABLE_DATA_W1TC_V)<<(GPIO_ENABLE_DATA_W1TC_S))
#define GPIO_ENABLE_DATA_W1TC_V  0xFFFFFFFF
#define GPIO_ENABLE_DATA_W1TC_S  0

#define GPIO_ENABLE1_REG          (DR_REG_GPIO_BASE + 0x002c)
/* GPIO_ENABLE1_DATA : R/W ;bitpos:[7:0] ;default: x ; */
/*description: GPIO32~39 output enable*/
#define GPIO_ENABLE1_DATA  0x000000FF
#define GPIO_ENABLE1_DATA_M  ((GPIO_ENABLE1_DATA_V)<<(GPIO_ENABLE1_DATA_S))
#define GPIO_ENABLE1_DATA_V  0xFF
#define GPIO_ENABLE1_DATA_S  0

#define GPIO_ENABLE1_W1TS_REG          (DR_REG_GPIO_BASE + 0x0030)
/* GPIO_ENABLE1_DATA_W1TS : R/W ;bitpos:[7:0] ;default: x ; */
/*description: GPIO32~39 output enable write 1 to set*/
#define GPIO_ENABLE1_DATA_W1TS  0x000000FF
#define GPIO_ENABLE1_DATA_W1TS_M  ((GPIO_ENABLE1_DATA_W1TS_V)<<(GPIO_ENABLE1_DATA_W1TS_S))
#define GPIO_ENABLE1_DATA_W1TS_V  0xFF
#define GPIO_ENABLE1_DATA_W1TS_S  0

#define GPIO_ENABLE1_W1TC_REG          (DR_REG_GPIO_BASE + 0x0034)
/* GPIO_ENABLE1_DATA_W1TC : R/W ;bitpos:[7:0] ;default: x ; */
/*description: GPIO32~39 output enable write 1 to clear*/
#define GPIO_ENABLE1_DATA_W1TC  0x000000FF
#define GPIO_ENABLE1_DATA_W1TC_M  ((GPIO_ENABLE1_DATA_W1TC_V)<<(GPIO_ENABLE1_DATA_W1TC_S))
#define GPIO_ENABLE1_DATA_W1TC_V  0xFF
#define GPIO_ENABLE1_DATA_W1TC_S  0

#define GPIO_STRAP_REG          (DR_REG_GPIO_BASE + 0x0038)
/* GPIO_STRAPPING : RO ;bitpos:[15:0] ;default:  ; */
/*description: {10'b0, MTDI, GPIO0, GPIO2, GPIO4, MTDO, GPIO5} */
#define GPIO_STRAPPING  0x0000FFFF
#define GPIO_STRAPPING_M  ((GPIO_STRAPPING_V)<<(GPIO_STRAPPING_S))
#define GPIO_STRAPPING_V  0xFFFF
#define GPIO_STRAPPING_S  0

#define GPIO_IN_REG          (DR_REG_GPIO_BASE + 0x003c)
/* GPIO_IN_DATA : RO ;bitpos:[31:0] ;default:  ; */
/*description: GPIO0~31 input value*/
#define GPIO_IN_DATA  0xFFFFFFFF
#define GPIO_IN_DATA_M  ((GPIO_IN_DATA_V)<<(GPIO_IN_DATA_S))
#define GPIO_IN_DATA_V  0xFFFFFFFF
#define GPIO_IN_DATA_S  0

#define GPIO_IN1_REG          (DR_REG_GPIO_BASE + 0x0040)
/* GPIO_IN1_DATA : RO ;bitpos:[7:0] ;default:  ; */
/*description: GPIO32~39 input value*/
#define GPIO_IN1_DATA  0x000000FF
#define GPIO_IN1_DATA_M  ((GPIO_IN1_DATA_V)<<(GPIO_IN1_DATA_S))
#define GPIO_IN1_DATA_V  0xFF
#define GPIO_IN1_DATA_S  0

#define GPIO_STATUS_REG          (DR_REG_GPIO_BASE + 0x0044)
/* GPIO_STATUS_INT : R/W ;bitpos:[31:0] ;default: x ; */
/*description: GPIO0~31 interrupt status*/
#define GPIO_STATUS_INT  0xFFFFFFFF
#define GPIO_STATUS_INT_M  ((GPIO_STATUS_INT_V)<<(GPIO_STATUS_INT_S))
#define GPIO_STATUS_INT_V  0xFFFFFFFF
#define GPIO_STATUS_INT_S  0

#define GPIO_STATUS_W1TS_REG          (DR_REG_GPIO_BASE + 0x0048)
/* GPIO_STATUS_INT_W1TS : R/W ;bitpos:[31:0] ;default: x ; */
/*description: GPIO0~31 interrupt status write 1 to set*/
#define GPIO_STATUS_INT_W1TS  0xFFFFFFFF
#define GPIO_STATUS_INT_W1TS_M  ((GPIO_STATUS_INT_W1TS_V)<<(GPIO_STATUS_INT_W1TS_S))
#define GPIO_STATUS_INT_W1TS_V  0xFFFFFFFF
#define GPIO_STATUS_INT_W1TS_S  0

#define GPIO_STATUS_W1TC_REG          (DR_REG_GPIO_BASE + 0x004c)
/* GPIO_STATUS_INT_W1TC : R/W ;bitpos:[31:0] ;default: x ; */
/*description: GPIO0~31 interrupt status write 1 to clear*/
#define GPIO_STATUS_INT_W1TC  0xFFFFFFFF
#define GPIO_STATUS_INT_W1TC_M  ((GPIO_STATUS_INT_W1TC_V)<<(GPIO_STATUS_INT_W1TC_S))
#define GPIO_STATUS_INT_W1TC_V  0xFFFFFFFF
#define GPIO_STATUS_INT_W1TC_S  0

#define GPIO_STATUS1_REG          (DR_REG_GPIO_BASE + 0x0050)
/* GPIO_STATUS1_INT : R/W ;bitpos:[7:0] ;default: x ; */
/*description: GPIO32~39 interrupt status*/
#define GPIO_STATUS1_INT  0x000000FF
#define GPIO_STATUS1_INT_M  ((GPIO_STATUS1_INT_V)<<(GPIO_STATUS1_INT_S))
#define GPIO_STATUS1_INT_V  0xFF
#define GPIO_STATUS1_INT_S  0

#define GPIO_STATUS1_W1TS_REG          (DR_REG_GPIO_BASE + 0x0054)
/* GPIO_STATUS1_INT_W1TS : R/W ;bitpos:[7:0] ;default: x ; */
/*description: GPIO32~39 interrupt status write 1 to set*/
#define GPIO_STATUS1_INT_W1TS  0x000000FF
#define GPIO_STATUS1_INT_W1TS_M  ((GPIO_STATUS1_INT_W1TS_V)<<(GPIO_STATUS1_INT_W1TS_S))
#define GPIO_STATUS1_INT_W1TS_V  0xFF
#define GPIO_STATUS1_INT_W1TS_S  0

#define GPIO_STATUS1_W1TC_REG          (DR_REG_GPIO_BASE + 0x0058)
/* GPIO_STATUS1_INT_W1TC : R/W ;bitpos:[7:0] ;default: x ; */
/*description: GPIO32~39 interrupt status write 1 to clear*/
#define GPIO_STATUS1_INT_W1TC  0x000000FF
#define GPIO_STATUS1_INT_W1TC_M  ((GPIO_STATUS1_INT_W1TC_V)<<(GPIO_STATUS1_INT_W1TC_S))
#define GPIO_STATUS1_INT_W1TC_V  0xFF
#define GPIO_STATUS1_INT_W1TC_S  0

#define GPIO_ACPU_INT_REG          (DR_REG_GPIO_BASE + 0x0060)
/* GPIO_APPCPU_INT : RO ;bitpos:[31:0] ;default: x ; */
/*description: GPIO0~31 APP CPU interrupt status*/
#define GPIO_APPCPU_INT  0xFFFFFFFF
#define GPIO_APPCPU_INT_M  ((GPIO_APPCPU_INT_V)<<(GPIO_APPCPU_INT_S))
#define GPIO_APPCPU_INT_V  0xFFFFFFFF
#define GPIO_APPCPU_INT_S  0

#define GPIO_ACPU_NMI_INT_REG          (DR_REG_GPIO_BASE + 0x0064)
/* GPIO_APPCPU_NMI_INT : RO ;bitpos:[31:0] ;default: x ; */
/*description: GPIO0~31 APP CPU non-maskable interrupt status*/
#define GPIO_APPCPU_NMI_INT  0xFFFFFFFF
#define GPIO_APPCPU_NMI_INT_M  ((GPIO_APPCPU_NMI_INT_V)<<(GPIO_APPCPU_NMI_INT_S))
#define GPIO_APPCPU_NMI_INT_V  0xFFFFFFFF
#define GPIO_APPCPU_NMI_INT_S  0

#define GPIO_PCPU_INT_REG          (DR_REG_GPIO_BASE + 0x0068)
/* GPIO_PROCPU_INT : RO ;bitpos:[31:0] ;default: x ; */
/*description: GPIO0~31 PRO CPU interrupt status*/
#define GPIO_PROCPU_INT  0xFFFFFFFF
#define GPIO_PROCPU_INT_M  ((GPIO_PROCPU_INT_V)<<(GPIO_PROCPU_INT_S))
#define GPIO_PROCPU_INT_V  0xFFFFFFFF
#define GPIO_PROCPU_INT_S  0

#define GPIO_PCPU_NMI_INT_REG          (DR_REG_GPIO_BASE + 0x006c)
/* GPIO_PROCPU_NMI_INT : RO ;bitpos:[31:0] ;default: x ; */
/*description: GPIO0~31 PRO CPU non-maskable interrupt status*/
#define GPIO_PROCPU_NMI_INT  0xFFFFFFFF
#define GPIO_PROCPU_NMI_INT_M  ((GPIO_PROCPU_NMI_INT_V)<<(GPIO_PROCPU_NMI_INT_S))
#define GPIO_PROCPU_NMI_INT_V  0xFFFFFFFF
#define GPIO_PROCPU_NMI_INT_S  0

#define GPIO_CPUSDIO_INT_REG          (DR_REG_GPIO_BASE + 0x0070)
/* GPIO_SDIO_INT : RO ;bitpos:[31:0] ;default: x ; */
/*description: SDIO's extent GPIO0~31 interrupt*/
#define GPIO_SDIO_INT  0xFFFFFFFF
#define GPIO_SDIO_INT_M  ((GPIO_SDIO_INT_V)<<(GPIO_SDIO_INT_S))
#define GPIO_SDIO_INT_V  0xFFFFFFFF
#define GPIO_SDIO_INT_S  0

#define GPIO_ACPU_INT1_REG          (DR_REG_GPIO_BASE + 0x0074)
/* GPIO_APPCPU_INT_H : RO ;bitpos:[7:0] ;default: x ; */
/*description: GPIO32~39 APP CPU interrupt status*/
#define GPIO_APPCPU_INT_H  0x000000FF
#define GPIO_APPCPU_INT_H_M  ((GPIO_APPCPU_INT_H_V)<<(GPIO_APPCPU_INT_H_S))
#define GPIO_APPCPU_INT_H_V  0xFF
#define GPIO_APPCPU_INT_H_S  0

#define GPIO_ACPU_NMI_INT1_REG          (DR_REG_GPIO_BASE + 0x0078)
/* GPIO_APPCPU_NMI_INT_H : RO ;bitpos:[7:0] ;default: x ; */
/*description: GPIO32~39 APP CPU non-maskable interrupt status*/
#define GPIO_APPCPU_NMI_INT_H  0x000000FF
#define GPIO_APPCPU_NMI_INT_H_M  ((GPIO_APPCPU_NMI_INT_H_V)<<(GPIO_APPCPU_NMI_INT_H_S))
#define GPIO_APPCPU_NMI_INT_H_V  0xFF
#define GPIO_APPCPU_NMI_INT_H_S  0

#define GPIO_PCPU_INT1_REG          (DR_REG_GPIO_BASE + 0x007c)
/* GPIO_PROCPU_INT_H : RO ;bitpos:[7:0] ;default: x ; */
/*description: GPIO32~39 PRO CPU interrupt status*/
#define GPIO_PROCPU_INT_H  0x000000FF
#define GPIO_PROCPU_INT_H_M  ((GPIO_PROCPU_INT_H_V)<<(GPIO_PROCPU_INT_H_S))
#define GPIO_PROCPU_INT_H_V  0xFF
#define GPIO_PROCPU_INT_H_S  0

#define GPIO_PCPU_NMI_INT1_REG          (DR_REG_GPIO_BASE + 0x0080)
/* GPIO_PROCPU_NMI_INT_H : RO ;bitpos:[7:0] ;default: x ; */
/*description: GPIO32~39 PRO CPU non-maskable interrupt status*/
#define GPIO_PROCPU_NMI_INT_H  0x000000FF
#define GPIO_PROCPU_NMI_INT_H_M  ((GPIO_PROCPU_NMI_INT_H_V)<<(GPIO_PROCPU_NMI_INT_H_S))
#define GPIO_PROCPU_NMI_INT_H_V  0xFF
#define GPIO_PROCPU_NMI_INT_H_S  0

#define GPIO_CPUSDIO_INT1_REG          (DR_REG_GPIO_BASE + 0x0084)
/* GPIO_SDIO_INT_H : RO ;bitpos:[7:0] ;default: x ; */
/*description: SDIO's extent GPIO32~39 interrupt*/
#define GPIO_SDIO_INT_H  0x000000FF
#define GPIO_SDIO_INT_H_M  ((GPIO_SDIO_INT_H_V)<<(GPIO_SDIO_INT_H_S))
#define GPIO_SDIO_INT_H_V  0xFF
#define GPIO_SDIO_INT_H_S  0

#define GPIO_REG(io_num)      (GPIO_PIN0_REG + (io_num)*0x4)
#define GPIO_PIN_INT_ENA 0x0000001F
#define GPIO_PIN_INT_ENA_M ((GPIO_PIN_INT_ENA_V)<<(GPIO_PIN_INT_ENA_S))
#define GPIO_PIN_INT_ENA_V 0x0000001F
#define GPIO_PIN_INT_ENA_S 13
#define GPIO_PIN_CONFIG 0x00000003
#define GPIO_PIN_CONFIG_M ((GPIO_PIN_CONFIG_V)<<(GPIO_PIN_CONFIG_S))
#define GPIO_PIN_CONFIG_V 0x00000003
#define GPIO_PIN_CONFIG_S 11
#define GPIO_PIN_WAKEUP_ENABLE (BIT(10))
#define GPIO_PIN_WAKEUP_ENABLE_M (BIT(10))
#define GPIO_PIN_WAKEUP_ENABLE_V 0x1
#define GPIO_PIN_WAKEUP_ENABLE_S 10
#define GPIO_PIN_INT_TYPE 0x00000007
#define GPIO_PIN_INT_TYPE_M ((GPIO_PIN_INT_TYPE_V)<<(GPIO_PIN_INT_TYPE_S))
#define GPIO_PIN_INT_TYPE_V 0x00000007
#define GPIO_PIN_INT_TYPE_S 7
#define GPIO_PIN_PAD_DRIVER (BIT(2))
#define GPIO_PIN_PAD_DRIVER_M (BIT(2))
#define GPIO_PIN_PAD_DRIVER_V 0x1
#define GPIO_PIN_PAD_DRIVER_S 2

#define GPIO_PIN0_REG          (DR_REG_GPIO_BASE + 0x0088)
/* GPIO_PIN0_INT_ENA : R/W ;bitpos:[17:13] ;default: x ; */
/*description: bit0: APP CPU interrupt enable  bit1: APP CPU non-maskable interrupt
 enable  bit3: PRO CPU interrupt enable  bit4:  PRO CPU non-maskable interrupt enable  bit5: SDIO's extent interrupt enable*/
#define GPIO_PIN0_INT_ENA  0x0000001F
#define GPIO_PIN0_INT_ENA_M  ((GPIO_PIN0_INT_ENA_V)<<(GPIO_PIN0_INT_ENA_S))
#define GPIO_PIN0_INT_ENA_V  0x1F
#define GPIO_PIN0_INT_ENA_S  13
/* GPIO_PIN0_CONFIG : R/W ;bitpos:[12:11] ;default: x ; */
/*description: NA*/
#define GPIO_PIN0_CONFIG  0x00000003
#define GPIO_PIN0_CONFIG_M  ((GPIO_PIN0_CONFIG_V)<<(GPIO_PIN0_CONFIG_S))
#define GPIO_PIN0_CONFIG_V  0x3
#define GPIO_PIN0_CONFIG_S  11
/* GPIO_PIN0_WAKEUP_ENABLE : R/W ;bitpos:[10] ;default: x ; */
/*description: GPIO wake up enable  only available in light sleep*/
#define GPIO_PIN0_WAKEUP_ENABLE  (BIT(10))
#define GPIO_PIN0_WAKEUP_ENABLE_M  (BIT(10))
#define GPIO_PIN0_WAKEUP_ENABLE_V  0x1
#define GPIO_PIN0_WAKEUP_ENABLE_S  10
/* GPIO_PIN0_INT_TYPE : R/W ;bitpos:[9:7] ;default: x ; */
/*description: if set to 0: GPIO interrupt disable  if set to 1: rising edge
 trigger  if set to 2: falling edge trigger  if set to 3: any edge trigger  if set to 4: low level trigger  if set to 5: high level trigger*/
#define GPIO_PIN0_INT_TYPE  0x00000007
#define GPIO_PIN0_INT_TYPE_M  ((GPIO_PIN0_INT_TYPE_V)<<(GPIO_PIN0_INT_TYPE_S))
#define GPIO_PIN0_INT_TYPE_V  0x7
#define GPIO_PIN0_INT_TYPE_S  7
/* GPIO_PIN0_PAD_DRIVER : R/W ;bitpos:[2] ;default: x ; */
/*description: if set to 0: normal output  if set to 1: open drain*/
#define GPIO_PIN0_PAD_DRIVER  (BIT(2))
#define GPIO_PIN0_PAD_DRIVER_M  (BIT(2))
#define GPIO_PIN0_PAD_DRIVER_V  0x1
#define GPIO_PIN0_PAD_DRIVER_S  2

#define GPIO_PIN1_REG          (DR_REG_GPIO_BASE + 0x008c)
/* GPIO_PIN1_INT_ENA : R/W ;bitpos:[17:13] ;default: x ; */
/*description: bit0: APP CPU interrupt enable  bit1: APP CPU non-maskable interrupt
 enable  bit3: PRO CPU interrupt enable  bit4:  PRO CPU non-maskable interrupt enable  bit5: SDIO's extent interrupt enable*/
#define GPIO_PIN1_INT_ENA  0x0000001F
#define GPIO_PIN1_INT_ENA_M  ((GPIO_PIN1_INT_ENA_V)<<(GPIO_PIN1_INT_ENA_S))
#define GPIO_PIN1_INT_ENA_V  0x1F
#define GPIO_PIN1_INT_ENA_S  13
/* GPIO_PIN1_CONFIG : R/W ;bitpos:[12:11] ;default: x ; */
/*description: NA*/
#define GPIO_PIN1_CONFIG  0x00000003
#define GPIO_PIN1_CONFIG_M  ((GPIO_PIN1_CONFIG_V)<<(GPIO_PIN1_CONFIG_S))
#define GPIO_PIN1_CONFIG_V  0x3
#define GPIO_PIN1_CONFIG_S  11
/* GPIO_PIN1_WAKEUP_ENABLE : R/W ;bitpos:[10] ;default: x ; */
/*description: GPIO wake up enable  only available in light sleep*/
#define GPIO_PIN1_WAKEUP_ENABLE  (BIT(10))
#define GPIO_PIN1_WAKEUP_ENABLE_M  (BIT(10))
#define GPIO_PIN1_WAKEUP_ENABLE_V  0x1
#define GPIO_PIN1_WAKEUP_ENABLE_S  10
/* GPIO_PIN1_INT_TYPE : R/W ;bitpos:[9:7] ;default: x ; */
/*description: if set to 0: GPIO interrupt disable  if set to 1: rising edge
 trigger  if set to 2: falling edge trigger  if set to 3: any edge trigger  if set to 4: low level trigger  if set to 5: high level trigger*/
#define GPIO_PIN1_INT_TYPE  0x00000007
#define GPIO_PIN1_INT_TYPE_M  ((GPIO_PIN1_INT_TYPE_V)<<(GPIO_PIN1_INT_TYPE_S))
#define GPIO_PIN1_INT_TYPE_V  0x7
#define GPIO_PIN1_INT_TYPE_S  7
/* GPIO_PIN1_PAD_DRIVER : R/W ;bitpos:[2] ;default: x ; */
/*description: if set to 0: normal output  if set to 1: open drain*/
#define GPIO_PIN1_PAD_DRIVER  (BIT(2))
#define GPIO_PIN1_PAD_DRIVER_M  (BIT(2))
#define GPIO_PIN1_PAD_DRIVER_V  0x1
#define GPIO_PIN1_PAD_DRIVER_S  2

#define GPIO_PIN2_REG          (DR_REG_GPIO_BASE + 0x0090)
/* GPIO_PIN2_INT_ENA : R/W ;bitpos:[17:13] ;default: x ; */
/*description: bit0: APP CPU interrupt enable  bit1: APP CPU non-maskable interrupt
 enable  bit3: PRO CPU interrupt enable  bit4:  PRO CPU non-maskable interrupt enable  bit5: SDIO's extent interrupt enable*/
#define GPIO_PIN2_INT_ENA  0x0000001F
#define GPIO_PIN2_INT_ENA_M  ((GPIO_PIN2_INT_ENA_V)<<(GPIO_PIN2_INT_ENA_S))
#define GPIO_PIN2_INT_ENA_V  0x1F
#define GPIO_PIN2_INT_ENA_S  13
/* GPIO_PIN2_CONFIG : R/W ;bitpos:[12:11] ;default: x ; */
/*description: NA*/
#define GPIO_PIN2_CONFIG  0x00000003
#define GPIO_PIN2_CONFIG_M  ((GPIO_PIN2_CONFIG_V)<<(GPIO_PIN2_CONFIG_S))
#define GPIO_PIN2_CONFIG_V  0x3
#define GPIO_PIN2_CONFIG_S  11
/* GPIO_PIN2_WAKEUP_ENABLE : R/W ;bitpos:[10] ;default: x ; */
/*description: GPIO wake up enable  only available in light sleep*/
#define GPIO_PIN2_WAKEUP_ENABLE  (BIT(10))
#define GPIO_PIN2_WAKEUP_ENABLE_M  (BIT(10))
#define GPIO_PIN2_WAKEUP_ENABLE_V  0x1
#define GPIO_PIN2_WAKEUP_ENABLE_S  10
/* GPIO_PIN2_INT_TYPE : R/W ;bitpos:[9:7] ;default: x ; */
/*description: if set to 0: GPIO interrupt disable  if set to 1: rising edge
 trigger  if set to 2: falling edge trigger  if set to 3: any edge trigger  if set to 4: low level trigger  if set to 5: high level trigger*/
#define GPIO_PIN2_INT_TYPE  0x00000007
#define GPIO_PIN2_INT_TYPE_M  ((GPIO_PIN2_INT_TYPE_V)<<(GPIO_PIN2_INT_TYPE_S))
#define GPIO_PIN2_INT_TYPE_V  0x7
#define GPIO_PIN2_INT_TYPE_S  7
/* GPIO_PIN2_PAD_DRIVER : R/W ;bitpos:[2] ;default: x ; */
/*description: if set to 0: normal output  if set to 1: open drain*/
#define GPIO_PIN2_PAD_DRIVER  (BIT(2))
#define GPIO_PIN2_PAD_DRIVER_M  (BIT(2))
#define GPIO_PIN2_PAD_DRIVER_V  0x1
#define GPIO_PIN2_PAD_DRIVER_S  2

#define GPIO_PIN3_REG          (DR_REG_GPIO_BASE + 0x0094)
/* GPIO_PIN3_INT_ENA : R/W ;bitpos:[17:13] ;default: x ; */
/*description: bit0: APP CPU interrupt enable  bit1: APP CPU non-maskable interrupt
 enable  bit3: PRO CPU interrupt enable  bit4:  PRO CPU non-maskable interrupt enable  bit5: SDIO's extent interrupt enable*/
#define GPIO_PIN3_INT_ENA  0x0000001F
#define GPIO_PIN3_INT_ENA_M  ((GPIO_PIN3_INT_ENA_V)<<(GPIO_PIN3_INT_ENA_S))
#define GPIO_PIN3_INT_ENA_V  0x1F
#define GPIO_PIN3_INT_ENA_S  13
/* GPIO_PIN3_CONFIG : R/W ;bitpos:[12:11] ;default: x ; */
/*description: NA*/
#define GPIO_PIN3_CONFIG  0x00000003
#define GPIO_PIN3_CONFIG_M  ((GPIO_PIN3_CONFIG_V)<<(GPIO_PIN3_CONFIG_S))
#define GPIO_PIN3_CONFIG_V  0x3
#define GPIO_PIN3_CONFIG_S  11
/* GPIO_PIN3_WAKEUP_ENABLE : R/W ;bitpos:[10] ;default: x ; */
/*description: GPIO wake up enable  only available in light sleep*/
#define GPIO_PIN3_WAKEUP_ENABLE  (BIT(10))
#define GPIO_PIN3_WAKEUP_ENABLE_M  (BIT(10))
#define GPIO_PIN3_WAKEUP_ENABLE_V  0x1
#define GPIO_PIN3_WAKEUP_ENABLE_S  10
/* GPIO_PIN3_INT_TYPE : R/W ;bitpos:[9:7] ;default: x ; */
/*description: if set to 0: GPIO interrupt disable  if set to 1: rising edge
 trigger  if set to 2: falling edge trigger  if set to 3: any edge trigger  if set to 4: low level trigger  if set to 5: high level trigger*/
#define GPIO_PIN3_INT_TYPE  0x00000007
#define GPIO_PIN3_INT_TYPE_M  ((GPIO_PIN3_INT_TYPE_V)<<(GPIO_PIN3_INT_TYPE_S))
#define GPIO_PIN3_INT_TYPE_V  0x7
#define GPIO_PIN3_INT_TYPE_S  7
/* GPIO_PIN3_PAD_DRIVER : R/W ;bitpos:[2] ;default: x ; */
/*description: if set to 0: normal output  if set to 1: open drain*/
#define GPIO_PIN3_PAD_DRIVER  (BIT(2))
#define GPIO_PIN3_PAD_DRIVER_M  (BIT(2))
#define GPIO_PIN3_PAD_DRIVER_V  0x1
#define GPIO_PIN3_PAD_DRIVER_S  2

#define GPIO_PIN4_REG          (DR_REG_GPIO_BASE + 0x0098)
/* GPIO_PIN4_INT_ENA : R/W ;bitpos:[17:13] ;default: x ; */
/*description: bit0: APP CPU interrupt enable  bit1: APP CPU non-maskable interrupt
 enable  bit3: PRO CPU interrupt enable  bit4:  PRO CPU non-maskable interrupt enable  bit5: SDIO's extent interrupt enable*/
#define GPIO_PIN4_INT_ENA  0x0000001F
#define GPIO_PIN4_INT_ENA_M  ((GPIO_PIN4_INT_ENA_V)<<(GPIO_PIN4_INT_ENA_S))
#define GPIO_PIN4_INT_ENA_V  0x1F
#define GPIO_PIN4_INT_ENA_S  13
/* GPIO_PIN4_CONFIG : R/W ;bitpos:[12:11] ;default: x ; */
/*description: NA*/
#define GPIO_PIN4_CONFIG  0x00000003
#define GPIO_PIN4_CONFIG_M  ((GPIO_PIN4_CONFIG_V)<<(GPIO_PIN4_CONFIG_S))
#define GPIO_PIN4_CONFIG_V  0x3
#define GPIO_PIN4_CONFIG_S  11
/* GPIO_PIN4_WAKEUP_ENABLE : R/W ;bitpos:[10] ;default: x ; */
/*description: GPIO wake up enable  only available in light sleep*/
#define GPIO_PIN4_WAKEUP_ENABLE  (BIT(10))
#define GPIO_PIN4_WAKEUP_ENABLE_M  (BIT(10))
#define GPIO_PIN4_WAKEUP_ENABLE_V  0x1
#define GPIO_PIN4_WAKEUP_ENABLE_S  10
/* GPIO_PIN4_INT_TYPE : R/W ;bitpos:[9:7] ;default: x ; */
/*description: if set to 0: GPIO interrupt disable  if set to 1: rising edge
 trigger  if set to 2: falling edge trigger  if set to 3: any edge trigger  if set to 4: low level trigger  if set to 5: high level trigger*/
#define GPIO_PIN4_INT_TYPE  0x00000007
#define GPIO_PIN4_INT_TYPE_M  ((GPIO_PIN4_INT_TYPE_V)<<(GPIO_PIN4_INT_TYPE_S))
#define GPIO_PIN4_INT_TYPE_V  0x7
#define GPIO_PIN4_INT_TYPE_S  7
/* GPIO_PIN4_PAD_DRIVER : R/W ;bitpos:[2] ;default: x ; */
/*description: if set to 0: normal output  if set to 1: open drain*/
#define GPIO_PIN4_PAD_DRIVER  (BIT(2))
#define GPIO_PIN4_PAD_DRIVER_M  (BIT(2))
#define GPIO_PIN4_PAD_DRIVER_V  0x1
#define GPIO_PIN4_PAD_DRIVER_S  2

#define GPIO_PIN5_REG          (DR_REG_GPIO_BASE + 0x009c)
/* GPIO_PIN5_INT_ENA : R/W ;bitpos:[17:13] ;default: x ; */
/*description: bit0: APP CPU interrupt enable  bit1: APP CPU non-maskable interrupt
 enable  bit3: PRO CPU interrupt enable  bit4:  PRO CPU non-maskable interrupt enable  bit5: SDIO's extent interrupt enable*/
#define GPIO_PIN5_INT_ENA  0x0000001F
#define GPIO_PIN5_INT_ENA_M  ((GPIO_PIN5_INT_ENA_V)<<(GPIO_PIN5_INT_ENA_S))
#define GPIO_PIN5_INT_ENA_V  0x1F
#define GPIO_PIN5_INT_ENA_S  13
/* GPIO_PIN5_CONFIG : R/W ;bitpos:[12:11] ;default: x ; */
/*description: NA*/
#define GPIO_PIN5_CONFIG  0x00000003
#define GPIO_PIN5_CONFIG_M  ((GPIO_PIN5_CONFIG_V)<<(GPIO_PIN5_CONFIG_S))
#define GPIO_PIN5_CONFIG_V  0x3
#define GPIO_PIN5_CONFIG_S  11
/* GPIO_PIN5_WAKEUP_ENABLE : R/W ;bitpos:[10] ;default: x ; */
/*description: GPIO wake up enable  only available in light sleep*/
#define GPIO_PIN5_WAKEUP_ENABLE  (BIT(10))
#define GPIO_PIN5_WAKEUP_ENABLE_M  (BIT(10))
#define GPIO_PIN5_WAKEUP_ENABLE_V  0x1
#define GPIO_PIN5_WAKEUP_ENABLE_S  10
/* GPIO_PIN5_INT_TYPE : R/W ;bitpos:[9:7] ;default: x ; */
/*description: if set to 0: GPIO interrupt disable  if set to 1: rising edge
 trigger  if set to 2: falling edge trigger  if set to 3: any edge trigger  if set to 4: low level trigger  if set to 5: high level trigger*/
#define GPIO_PIN5_INT_TYPE  0x00000007
#define GPIO_PIN5_INT_TYPE_M  ((GPIO_PIN5_INT_TYPE_V)<<(GPIO_PIN5_INT_TYPE_S))
#define GPIO_PIN5_INT_TYPE_V  0x7
#define GPIO_PIN5_INT_TYPE_S  7
/* GPIO_PIN5_PAD_DRIVER : R/W ;bitpos:[2] ;default: x ; */
/*description: if set to 0: normal output  if set to 1: open drain*/
#define GPIO_PIN5_PAD_DRIVER  (BIT(2))
#define GPIO_PIN5_PAD_DRIVER_M  (BIT(2))
#define GPIO_PIN5_PAD_DRIVER_V  0x1
#define GPIO_PIN5_PAD_DRIVER_S  2

#define GPIO_PIN6_REG          (DR_REG_GPIO_BASE + 0x00a0)
/* GPIO_PIN6_INT_ENA : R/W ;bitpos:[17:13] ;default: x ; */
/*description: bit0: APP CPU interrupt enable  bit1: APP CPU non-maskable interrupt
 enable  bit3: PRO CPU interrupt enable  bit4:  PRO CPU non-maskable interrupt enable  bit5: SDIO's extent interrupt enable*/
#define GPIO_PIN6_INT_ENA  0x0000001F
#define GPIO_PIN6_INT_ENA_M  ((GPIO_PIN6_INT_ENA_V)<<(GPIO_PIN6_INT_ENA_S))
#define GPIO_PIN6_INT_ENA_V  0x1F
#define GPIO_PIN6_INT_ENA_S  13
/* GPIO_PIN6_CONFIG : R/W ;bitpos:[12:11] ;default: x ; */
/*description: NA*/
#define GPIO_PIN6_CONFIG  0x00000003
#define GPIO_PIN6_CONFIG_M  ((GPIO_PIN6_CONFIG_V)<<(GPIO_PIN6_CONFIG_S))
#define GPIO_PIN6_CONFIG_V  0x3
#define GPIO_PIN6_CONFIG_S  11
/* GPIO_PIN6_WAKEUP_ENABLE : R/W ;bitpos:[10] ;default: x ; */
/*description: GPIO wake up enable  only available in light sleep*/
#define GPIO_PIN6_WAKEUP_ENABLE  (BIT(10))
#define GPIO_PIN6_WAKEUP_ENABLE_M  (BIT(10))
#define GPIO_PIN6_WAKEUP_ENABLE_V  0x1
#define GPIO_PIN6_WAKEUP_ENABLE_S  10
/* GPIO_PIN6_INT_TYPE : R/W ;bitpos:[9:7] ;default: x ; */
/*description: if set to 0: GPIO interrupt disable  if set to 1: rising edge
 trigger  if set to 2: falling edge trigger  if set to 3: any edge trigger  if set to 4: low level trigger  if set to 5: high level trigger*/
#define GPIO_PIN6_INT_TYPE  0x00000007
#define GPIO_PIN6_INT_TYPE_M  ((GPIO_PIN6_INT_TYPE_V)<<(GPIO_PIN6_INT_TYPE_S))
#define GPIO_PIN6_INT_TYPE_V  0x7
#define GPIO_PIN6_INT_TYPE_S  7
/* GPIO_PIN6_PAD_DRIVER : R/W ;bitpos:[2] ;default: x ; */
/*description: if set to 0: normal output  if set to 1: open drain*/
#define GPIO_PIN6_PAD_DRIVER  (BIT(2))
#define GPIO_PIN6_PAD_DRIVER_M  (BIT(2))
#define GPIO_PIN6_PAD_DRIVER_V  0x1
#define GPIO_PIN6_PAD_DRIVER_S  2

#define GPIO_PIN7_REG          (DR_REG_GPIO_BASE + 0x00a4)
/* GPIO_PIN7_INT_ENA : R/W ;bitpos:[17:13] ;default: x ; */
/*description: bit0: APP CPU interrupt enable  bit1: APP CPU non-maskable interrupt
 enable  bit3: PRO CPU interrupt enable  bit4:  PRO CPU non-maskable interrupt enable  bit5: SDIO's extent interrupt enable*/
#define GPIO_PIN7_INT_ENA  0x0000001F
#define GPIO_PIN7_INT_ENA_M  ((GPIO_PIN7_INT_ENA_V)<<(GPIO_PIN7_INT_ENA_S))
#define GPIO_PIN7_INT_ENA_V  0x1F
#define GPIO_PIN7_INT_ENA_S  13
/* GPIO_PIN7_CONFIG : R/W ;bitpos:[12:11] ;default: x ; */
/*description: NA*/
#define GPIO_PIN7_CONFIG  0x00000003
#define GPIO_PIN7_CONFIG_M  ((GPIO_PIN7_CONFIG_V)<<(GPIO_PIN7_CONFIG_S))
#define GPIO_PIN7_CONFIG_V  0x3
#define GPIO_PIN7_CONFIG_S  11
/* GPIO_PIN7_WAKEUP_ENABLE : R/W ;bitpos:[10] ;default: x ; */
/*description: GPIO wake up enable  only available in light sleep*/
#define GPIO_PIN7_WAKEUP_ENABLE  (BIT(10))
#define GPIO_PIN7_WAKEUP_ENABLE_M  (BIT(10))
#define GPIO_PIN7_WAKEUP_ENABLE_V  0x1
#define GPIO_PIN7_WAKEUP_ENABLE_S  10
/* GPIO_PIN7_INT_TYPE : R/W ;bitpos:[9:7] ;default: x ; */
/*description: if set to 0: GPIO interrupt disable  if set to 1: rising edge
 trigger  if set to 2: falling edge trigger  if set to 3: any edge trigger  if set to 4: low level trigger  if set to 5: high level trigger*/
#define GPIO_PIN7_INT_TYPE  0x00000007
#define GPIO_PIN7_INT_TYPE_M  ((GPIO_PIN7_INT_TYPE_V)<<(GPIO_PIN7_INT_TYPE_S))
#define GPIO_PIN7_INT_TYPE_V  0x7
#define GPIO_PIN7_INT_TYPE_S  7
/* GPIO_PIN7_PAD_DRIVER : R/W ;bitpos:[2] ;default: x ; */
/*description: if set to 0: normal output  if set to 1: open drain*/
#define GPIO_PIN7_PAD_DRIVER  (BIT(2))
#define GPIO_PIN7_PAD_DRIVER_M  (BIT(2))
#define GPIO_PIN7_PAD_DRIVER_V  0x1
#define GPIO_PIN7_PAD_DRIVER_S  2

#define GPIO_PIN8_REG          (DR_REG_GPIO_BASE + 0x00a8)
/* GPIO_PIN8_INT_ENA : R/W ;bitpos:[17:13] ;default: x ; */
/*description: bit0: APP CPU interrupt enable  bit1: APP CPU non-maskable interrupt
 enable  bit3: PRO CPU interrupt enable  bit4:  PRO CPU non-maskable interrupt enable  bit5: SDIO's extent interrupt enable*/
#define GPIO_PIN8_INT_ENA  0x0000001F
#define GPIO_PIN8_INT_ENA_M  ((GPIO_PIN8_INT_ENA_V)<<(GPIO_PIN8_INT_ENA_S))
#define GPIO_PIN8_INT_ENA_V  0x1F
#define GPIO_PIN8_INT_ENA_S  13
/* GPIO_PIN8_CONFIG : R/W ;bitpos:[12:11] ;default: x ; */
/*description: NA*/
#define GPIO_PIN8_CONFIG  0x00000003
#define GPIO_PIN8_CONFIG_M  ((GPIO_PIN8_CONFIG_V)<<(GPIO_PIN8_CONFIG_S))
#define GPIO_PIN8_CONFIG_V  0x3
#define GPIO_PIN8_CONFIG_S  11
/* GPIO_PIN8_WAKEUP_ENABLE : R/W ;bitpos:[10] ;default: x ; */
/*description: GPIO wake up enable  only available in light sleep*/
#define GPIO_PIN8_WAKEUP_ENABLE  (BIT(10))
#define GPIO_PIN8_WAKEUP_ENABLE_M  (BIT(10))
#define GPIO_PIN8_WAKEUP_ENABLE_V  0x1
#define GPIO_PIN8_WAKEUP_ENABLE_S  10
/* GPIO_PIN8_INT_TYPE : R/W ;bitpos:[9:7] ;default: x ; */
/*description: if set to 0: GPIO interrupt disable  if set to 1: rising edge
 trigger  if set to 2: falling edge trigger  if set to 3: any edge trigger  if set to 4: low level trigger  if set to 5: high level trigger*/
#define GPIO_PIN8_INT_TYPE  0x00000007
#define GPIO_PIN8_INT_TYPE_M  ((GPIO_PIN8_INT_TYPE_V)<<(GPIO_PIN8_INT_TYPE_S))
#define GPIO_PIN8_INT_TYPE_V  0x7
#define GPIO_PIN8_INT_TYPE_S  7
/* GPIO_PIN8_PAD_DRIVER : R/W ;bitpos:[2] ;default: x ; */
/*description: if set to 0: normal output  if set to 1: open drain*/
#define GPIO_PIN8_PAD_DRIVER  (BIT(2))
#define GPIO_PIN8_PAD_DRIVER_M  (BIT(2))
#define GPIO_PIN8_PAD_DRIVER_V  0x1
#define GPIO_PIN8_PAD_DRIVER_S  2

#define GPIO_PIN9_REG          (DR_REG_GPIO_BASE + 0x00ac)
/* GPIO_PIN9_INT_ENA : R/W ;bitpos:[17:13] ;default: x ; */
/*description: bit0: APP CPU interrupt enable  bit1: APP CPU non-maskable interrupt
 enable  bit3: PRO CPU interrupt enable  bit4:  PRO CPU non-maskable interrupt enable  bit5: SDIO's extent interrupt enable*/
#define GPIO_PIN9_INT_ENA  0x0000001F
#define GPIO_PIN9_INT_ENA_M  ((GPIO_PIN9_INT_ENA_V)<<(GPIO_PIN9_INT_ENA_S))
#define GPIO_PIN9_INT_ENA_V  0x1F
#define GPIO_PIN9_INT_ENA_S  13
/* GPIO_PIN9_CONFIG : R/W ;bitpos:[12:11] ;default: x ; */
/*description: NA*/
#define GPIO_PIN9_CONFIG  0x00000003
#define GPIO_PIN9_CONFIG_M  ((GPIO_PIN9_CONFIG_V)<<(GPIO_PIN9_CONFIG_S))
#define GPIO_PIN9_CONFIG_V  0x3
#define GPIO_PIN9_CONFIG_S  11
/* GPIO_PIN9_WAKEUP_ENABLE : R/W ;bitpos:[10] ;default: x ; */
/*description: GPIO wake up enable  only available in light sleep*/
#define GPIO_PIN9_WAKEUP_ENABLE  (BIT(10))
#define GPIO_PIN9_WAKEUP_ENABLE_M  (BIT(10))
#define GPIO_PIN9_WAKEUP_ENABLE_V  0x1
#define GPIO_PIN9_WAKEUP_ENABLE_S  10
/* GPIO_PIN9_INT_TYPE : R/W ;bitpos:[9:7] ;default: x ; */
/*description: if set to 0: GPIO interrupt disable  if set to 1: rising edge
 trigger  if set to 2: falling edge trigger  if set to 3: any edge trigger  if set to 4: low level trigger  if set to 5: high level trigger*/
#define GPIO_PIN9_INT_TYPE  0x00000007
#define GPIO_PIN9_INT_TYPE_M  ((GPIO_PIN9_INT_TYPE_V)<<(GPIO_PIN9_INT_TYPE_S))
#define GPIO_PIN9_INT_TYPE_V  0x7
#define GPIO_PIN9_INT_TYPE_S  7
/* GPIO_PIN9_PAD_DRIVER : R/W ;bitpos:[2] ;default: x ; */
/*description: if set to 0: normal output  if set to 1: open drain*/
#define GPIO_PIN9_PAD_DRIVER  (BIT(2))
#define GPIO_PIN9_PAD_DRIVER_M  (BIT(2))
#define GPIO_PIN9_PAD_DRIVER_V  0x1
#define GPIO_PIN9_PAD_DRIVER_S  2

#define GPIO_PIN10_REG          (DR_REG_GPIO_BASE + 0x00b0)
/* GPIO_PIN10_INT_ENA : R/W ;bitpos:[17:13] ;default: x ; */
/*description: bit0: APP CPU interrupt enable  bit1: APP CPU non-maskable interrupt
 enable  bit3: PRO CPU interrupt enable  bit4:  PRO CPU non-maskable interrupt enable  bit5: SDIO's extent interrupt enable*/
#define GPIO_PIN10_INT_ENA  0x0000001F
#define GPIO_PIN10_INT_ENA_M  ((GPIO_PIN10_INT_ENA_V)<<(GPIO_PIN10_INT_ENA_S))
#define GPIO_PIN10_INT_ENA_V  0x1F
#define GPIO_PIN10_INT_ENA_S  13
/* GPIO_PIN10_CONFIG : R/W ;bitpos:[12:11] ;default: x ; */
/*description: NA*/
#define GPIO_PIN10_CONFIG  0x00000003
#define GPIO_PIN10_CONFIG_M  ((GPIO_PIN10_CONFIG_V)<<(GPIO_PIN10_CONFIG_S))
#define GPIO_PIN10_CONFIG_V  0x3
#define GPIO_PIN10_CONFIG_S  11
/* GPIO_PIN10_WAKEUP_ENABLE : R/W ;bitpos:[10] ;default: x ; */
/*description: GPIO wake up enable  only available in light sleep*/
#define GPIO_PIN10_WAKEUP_ENABLE  (BIT(10))
#define GPIO_PIN10_WAKEUP_ENABLE_M  (BIT(10))
#define GPIO_PIN10_WAKEUP_ENABLE_V  0x1
#define GPIO_PIN10_WAKEUP_ENABLE_S  10
/* GPIO_PIN10_INT_TYPE : R/W ;bitpos:[9:7] ;default: x ; */
/*description: if set to 0: GPIO interrupt disable  if set to 1: rising edge
 trigger  if set to 2: falling edge trigger  if set to 3: any edge trigger  if set to 4: low level trigger  if set to 5: high level trigger*/
#define GPIO_PIN10_INT_TYPE  0x00000007
#define GPIO_PIN10_INT_TYPE_M  ((GPIO_PIN10_INT_TYPE_V)<<(GPIO_PIN10_INT_TYPE_S))
#define GPIO_PIN10_INT_TYPE_V  0x7
#define GPIO_PIN10_INT_TYPE_S  7
/* GPIO_PIN10_PAD_DRIVER : R/W ;bitpos:[2] ;default: x ; */
/*description: if set to 0: normal output  if set to 1: open drain*/
#define GPIO_PIN10_PAD_DRIVER  (BIT(2))
#define GPIO_PIN10_PAD_DRIVER_M  (BIT(2))
#define GPIO_PIN10_PAD_DRIVER_V  0x1
#define GPIO_PIN10_PAD_DRIVER_S  2

#define GPIO_PIN11_REG          (DR_REG_GPIO_BASE + 0x00b4)
/* GPIO_PIN11_INT_ENA : R/W ;bitpos:[17:13] ;default: x ; */
/*description: bit0: APP CPU interrupt enable  bit1: APP CPU non-maskable interrupt
 enable  bit3: PRO CPU interrupt enable  bit4:  PRO CPU non-maskable interrupt enable  bit5: SDIO's extent interrupt enable*/
#define GPIO_PIN11_INT_ENA  0x0000001F
#define GPIO_PIN11_INT_ENA_M  ((GPIO_PIN11_INT_ENA_V)<<(GPIO_PIN11_INT_ENA_S))
#define GPIO_PIN11_INT_ENA_V  0x1F
#define GPIO_PIN11_INT_ENA_S  13
/* GPIO_PIN11_CONFIG : R/W ;bitpos:[12:11] ;default: x ; */
/*description: NA*/
#define GPIO_PIN11_CONFIG  0x00000003
#define GPIO_PIN11_CONFIG_M  ((GPIO_PIN11_CONFIG_V)<<(GPIO_PIN11_CONFIG_S))
#define GPIO_PIN11_CONFIG_V  0x3
#define GPIO_PIN11_CONFIG_S  11
/* GPIO_PIN11_WAKEUP_ENABLE : R/W ;bitpos:[10] ;default: x ; */
/*description: GPIO wake up enable  only available in light sleep*/
#define GPIO_PIN11_WAKEUP_ENABLE  (BIT(10))
#define GPIO_PIN11_WAKEUP_ENABLE_M  (BIT(10))
#define GPIO_PIN11_WAKEUP_ENABLE_V  0x1
#define GPIO_PIN11_WAKEUP_ENABLE_S  10
/* GPIO_PIN11_INT_TYPE : R/W ;bitpos:[9:7] ;default: x ; */
/*description: if set to 0: GPIO interrupt disable  if set to 1: rising edge
 trigger  if set to 2: falling edge trigger  if set to 3: any edge trigger  if set to 4: low level trigger  if set to 5: high level trigger*/
#define GPIO_PIN11_INT_TYPE  0x00000007
#define GPIO_PIN11_INT_TYPE_M  ((GPIO_PIN11_INT_TYPE_V)<<(GPIO_PIN11_INT_TYPE_S))
#define GPIO_PIN11_INT_TYPE_V  0x7
#define GPIO_PIN11_INT_TYPE_S  7
/* GPIO_PIN11_PAD_DRIVER : R/W ;bitpos:[2] ;default: x ; */
/*description: if set to 0: normal output  if set to 1: open drain*/
#define GPIO_PIN11_PAD_DRIVER  (BIT(2))
#define GPIO_PIN11_PAD_DRIVER_M  (BIT(2))
#define GPIO_PIN11_PAD_DRIVER_V  0x1
#define GPIO_PIN11_PAD_DRIVER_S  2

#define GPIO_PIN12_REG          (DR_REG_GPIO_BASE + 0x00b8)
/* GPIO_PIN12_INT_ENA : R/W ;bitpos:[17:13] ;default: x ; */
/*description: bit0: APP CPU interrupt enable  bit1: APP CPU non-maskable interrupt
 enable  bit3: PRO CPU interrupt enable  bit4:  PRO CPU non-maskable interrupt enable  bit5: SDIO's extent interrupt enable*/
#define GPIO_PIN12_INT_ENA  0x0000001F
#define GPIO_PIN12_INT_ENA_M  ((GPIO_PIN12_INT_ENA_V)<<(GPIO_PIN12_INT_ENA_S))
#define GPIO_PIN12_INT_ENA_V  0x1F
#define GPIO_PIN12_INT_ENA_S  13
/* GPIO_PIN12_CONFIG : R/W ;bitpos:[12:11] ;default: x ; */
/*description: NA*/
#define GPIO_PIN12_CONFIG  0x00000003
#define GPIO_PIN12_CONFIG_M  ((GPIO_PIN12_CONFIG_V)<<(GPIO_PIN12_CONFIG_S))
#define GPIO_PIN12_CONFIG_V  0x3
#define GPIO_PIN12_CONFIG_S  11
/* GPIO_PIN12_WAKEUP_ENABLE : R/W ;bitpos:[10] ;default: x ; */
/*description: GPIO wake up enable  only available in light sleep*/
#define GPIO_PIN12_WAKEUP_ENABLE  (BIT(10))
#define GPIO_PIN12_WAKEUP_ENABLE_M  (BIT(10))
#define GPIO_PIN12_WAKEUP_ENABLE_V  0x1
#define GPIO_PIN12_WAKEUP_ENABLE_S  10
/* GPIO_PIN12_INT_TYPE : R/W ;bitpos:[9:7] ;default: x ; */
/*description: if set to 0: GPIO interrupt disable  if set to 1: rising edge
 trigger  if set to 2: falling edge trigger  if set to 3: any edge trigger  if set to 4: low level trigger  if set to 5: high level trigger*/
#define GPIO_PIN12_INT_TYPE  0x00000007
#define GPIO_PIN12_INT_TYPE_M  ((GPIO_PIN12_INT_TYPE_V)<<(GPIO_PIN12_INT_TYPE_S))
#define GPIO_PIN12_INT_TYPE_V  0x7
#define GPIO_PIN12_INT_TYPE_S  7
/* GPIO_PIN12_PAD_DRIVER : R/W ;bitpos:[2] ;default: x ; */
/*description: if set to 0: normal output  if set to 1: open drain*/
#define GPIO_PIN12_PAD_DRIVER  (BIT(2))
#define GPIO_PIN12_PAD_DRIVER_M  (BIT(2))
#define GPIO_PIN12_PAD_DRIVER_V  0x1
#define GPIO_PIN12_PAD_DRIVER_S  2

#define GPIO_PIN13_REG          (DR_REG_GPIO_BASE + 0x00bc)
/* GPIO_PIN13_INT_ENA : R/W ;bitpos:[17:13] ;default: x ; */
/*description: bit0: APP CPU interrupt enable  bit1: APP CPU non-maskable interrupt
 enable  bit3: PRO CPU interrupt enable  bit4:  PRO CPU non-maskable interrupt enable  bit5: SDIO's extent interrupt enable*/
#define GPIO_PIN13_INT_ENA  0x0000001F
#define GPIO_PIN13_INT_ENA_M  ((GPIO_PIN13_INT_ENA_V)<<(GPIO_PIN13_INT_ENA_S))
#define GPIO_PIN13_INT_ENA_V  0x1F
#define GPIO_PIN13_INT_ENA_S  13
/* GPIO_PIN13_CONFIG : R/W ;bitpos:[12:11] ;default: x ; */
/*description: NA*/
#define GPIO_PIN13_CONFIG  0x00000003
#define GPIO_PIN13_CONFIG_M  ((GPIO_PIN13_CONFIG_V)<<(GPIO_PIN13_CONFIG_S))
#define GPIO_PIN13_CONFIG_V  0x3
#define GPIO_PIN13_CONFIG_S  11
/* GPIO_PIN13_WAKEUP_ENABLE : R/W ;bitpos:[10] ;default: x ; */
/*description: GPIO wake up enable  only available in light sleep*/
#define GPIO_PIN13_WAKEUP_ENABLE  (BIT(10))
#define GPIO_PIN13_WAKEUP_ENABLE_M  (BIT(10))
#define GPIO_PIN13_WAKEUP_ENABLE_V  0x1
#define GPIO_PIN13_WAKEUP_ENABLE_S  10
/* GPIO_PIN13_INT_TYPE : R/W ;bitpos:[9:7] ;default: x ; */
/*description: if set to 0: GPIO interrupt disable  if set to 1: rising edge
 trigger  if set to 2: falling edge trigger  if set to 3: any edge trigger  if set to 4: low level trigger  if set to 5: high level trigger*/
#define GPIO_PIN13_INT_TYPE  0x00000007
#define GPIO_PIN13_INT_TYPE_M  ((GPIO_PIN13_INT_TYPE_V)<<(GPIO_PIN13_INT_TYPE_S))
#define GPIO_PIN13_INT_TYPE_V  0x7
#define GPIO_PIN13_INT_TYPE_S  7
/* GPIO_PIN13_PAD_DRIVER : R/W ;bitpos:[2] ;default: x ; */
/*description: if set to 0: normal output  if set to 1: open drain*/
#define GPIO_PIN13_PAD_DRIVER  (BIT(2))
#define GPIO_PIN13_PAD_DRIVER_M  (BIT(2))
#define GPIO_PIN13_PAD_DRIVER_V  0x1
#define GPIO_PIN13_PAD_DRIVER_S  2

#define GPIO_PIN14_REG          (DR_REG_GPIO_BASE + 0x00c0)
/* GPIO_PIN14_INT_ENA : R/W ;bitpos:[17:13] ;default: x ; */
/*description: bit0: APP CPU interrupt enable  bit1: APP CPU non-maskable interrupt
 enable  bit3: PRO CPU interrupt enable  bit4:  PRO CPU non-maskable interrupt enable  bit5: SDIO's extent interrupt enable*/
#define GPIO_PIN14_INT_ENA  0x0000001F
#define GPIO_PIN14_INT_ENA_M  ((GPIO_PIN14_INT_ENA_V)<<(GPIO_PIN14_INT_ENA_S))
#define GPIO_PIN14_INT_ENA_V  0x1F
#define GPIO_PIN14_INT_ENA_S  13
/* GPIO_PIN14_CONFIG : R/W ;bitpos:[12:11] ;default: x ; */
/*description: NA*/
#define GPIO_PIN14_CONFIG  0x00000003
#define GPIO_PIN14_CONFIG_M  ((GPIO_PIN14_CONFIG_V)<<(GPIO_PIN14_CONFIG_S))
#define GPIO_PIN14_CONFIG_V  0x3
#define GPIO_PIN14_CONFIG_S  11
/* GPIO_PIN14_WAKEUP_ENABLE : R/W ;bitpos:[10] ;default: x ; */
/*description: GPIO wake up enable  only available in light sleep*/
#define GPIO_PIN14_WAKEUP_ENABLE  (BIT(10))
#define GPIO_PIN14_WAKEUP_ENABLE_M  (BIT(10))
#define GPIO_PIN14_WAKEUP_ENABLE_V  0x1
#define GPIO_PIN14_WAKEUP_ENABLE_S  10
/* GPIO_PIN14_INT_TYPE : R/W ;bitpos:[9:7] ;default: x ; */
/*description: if set to 0: GPIO interrupt disable  if set to 1: rising edge
 trigger  if set to 2: falling edge trigger  if set to 3: any edge trigger  if set to 4: low level trigger  if set to 5: high level trigger*/
#define GPIO_PIN14_INT_TYPE  0x00000007
#define GPIO_PIN14_INT_TYPE_M  ((GPIO_PIN14_INT_TYPE_V)<<(GPIO_PIN14_INT_TYPE_S))
#define GPIO_PIN14_INT_TYPE_V  0x7
#define GPIO_PIN14_INT_TYPE_S  7
/* GPIO_PIN14_PAD_DRIVER : R/W ;bitpos:[2] ;default: x ; */
/*description: if set to 0: normal output  if set to 1: open drain*/
#define GPIO_PIN14_PAD_DRIVER  (BIT(2))
#define GPIO_PIN14_PAD_DRIVER_M  (BIT(2))
#define GPIO_PIN14_PAD_DRIVER_V  0x1
#define GPIO_PIN14_PAD_DRIVER_S  2

#define GPIO_PIN15_REG          (DR_REG_GPIO_BASE + 0x00c4)
/* GPIO_PIN15_INT_ENA : R/W ;bitpos:[17:13] ;default: x ; */
/*description: bit0: APP CPU interrupt enable  bit1: APP CPU non-maskable interrupt
 enable  bit3: PRO CPU interrupt enable  bit4:  PRO CPU non-maskable interrupt enable  bit5: SDIO's extent interrupt enable*/
#define GPIO_PIN15_INT_ENA  0x0000001F
#define GPIO_PIN15_INT_ENA_M  ((GPIO_PIN15_INT_ENA_V)<<(GPIO_PIN15_INT_ENA_S))
#define GPIO_PIN15_INT_ENA_V  0x1F
#define GPIO_PIN15_INT_ENA_S  13
/* GPIO_PIN15_CONFIG : R/W ;bitpos:[12:11] ;default: x ; */
/*description: NA*/
#define GPIO_PIN15_CONFIG  0x00000003
#define GPIO_PIN15_CONFIG_M  ((GPIO_PIN15_CONFIG_V)<<(GPIO_PIN15_CONFIG_S))
#define GPIO_PIN15_CONFIG_V  0x3
#define GPIO_PIN15_CONFIG_S  11
/* GPIO_PIN15_WAKEUP_ENABLE : R/W ;bitpos:[10] ;default: x ; */
/*description: GPIO wake up enable  only available in light sleep*/
#define GPIO_PIN15_WAKEUP_ENABLE  (BIT(10))
#define GPIO_PIN15_WAKEUP_ENABLE_M  (BIT(10))
#define GPIO_PIN15_WAKEUP_ENABLE_V  0x1
#define GPIO_PIN15_WAKEUP_ENABLE_S  10
/* GPIO_PIN15_INT_TYPE : R/W ;bitpos:[9:7] ;default: x ; */
/*description: if set to 0: GPIO interrupt disable  if set to 1: rising edge
 trigger  if set to 2: falling edge trigger  if set to 3: any edge trigger  if set to 4: low level trigger  if set to 5: high level trigger*/
#define GPIO_PIN15_INT_TYPE  0x00000007
#define GPIO_PIN15_INT_TYPE_M  ((GPIO_PIN15_INT_TYPE_V)<<(GPIO_PIN15_INT_TYPE_S))
#define GPIO_PIN15_INT_TYPE_V  0x7
#define GPIO_PIN15_INT_TYPE_S  7
/* GPIO_PIN15_PAD_DRIVER : R/W ;bitpos:[2] ;default: x ; */
/*description: if set to 0: normal output  if set to 1: open drain*/
#define GPIO_PIN15_PAD_DRIVER  (BIT(2))
#define GPIO_PIN15_PAD_DRIVER_M  (BIT(2))
#define GPIO_PIN15_PAD_DRIVER_V  0x1
#define GPIO_PIN15_PAD_DRIVER_S  2

#define GPIO_PIN16_REG          (DR_REG_GPIO_BASE + 0x00c8)
/* GPIO_PIN16_INT_ENA : R/W ;bitpos:[17:13] ;default: x ; */
/*description: bit0: APP CPU interrupt enable  bit1: APP CPU non-maskable interrupt
 enable  bit3: PRO CPU interrupt enable  bit4:  PRO CPU non-maskable interrupt enable  bit5: SDIO's extent interrupt enable*/
#define GPIO_PIN16_INT_ENA  0x0000001F
#define GPIO_PIN16_INT_ENA_M  ((GPIO_PIN16_INT_ENA_V)<<(GPIO_PIN16_INT_ENA_S))
#define GPIO_PIN16_INT_ENA_V  0x1F
#define GPIO_PIN16_INT_ENA_S  13
/* GPIO_PIN16_CONFIG : R/W ;bitpos:[12:11] ;default: x ; */
/*description: NA*/
#define GPIO_PIN16_CONFIG  0x00000003
#define GPIO_PIN16_CONFIG_M  ((GPIO_PIN16_CONFIG_V)<<(GPIO_PIN16_CONFIG_S))
#define GPIO_PIN16_CONFIG_V  0x3
#define GPIO_PIN16_CONFIG_S  11
/* GPIO_PIN16_WAKEUP_ENABLE : R/W ;bitpos:[10] ;default: x ; */
/*description: GPIO wake up enable  only available in light sleep*/
#define GPIO_PIN16_WAKEUP_ENABLE  (BIT(10))
#define GPIO_PIN16_WAKEUP_ENABLE_M  (BIT(10))
#define GPIO_PIN16_WAKEUP_ENABLE_V  0x1
#define GPIO_PIN16_WAKEUP_ENABLE_S  10
/* GPIO_PIN16_INT_TYPE : R/W ;bitpos:[9:7] ;default: x ; */
/*description: if set to 0: GPIO interrupt disable  if set to 1: rising edge
 trigger  if set to 2: falling edge trigger  if set to 3: any edge trigger  if set to 4: low level trigger  if set to 5: high level trigger*/
#define GPIO_PIN16_INT_TYPE  0x00000007
#define GPIO_PIN16_INT_TYPE_M  ((GPIO_PIN16_INT_TYPE_V)<<(GPIO_PIN16_INT_TYPE_S))
#define GPIO_PIN16_INT_TYPE_V  0x7
#define GPIO_PIN16_INT_TYPE_S  7
/* GPIO_PIN16_PAD_DRIVER : R/W ;bitpos:[2] ;default: x ; */
/*description: if set to 0: normal output  if set to 1: open drain*/
#define GPIO_PIN16_PAD_DRIVER  (BIT(2))
#define GPIO_PIN16_PAD_DRIVER_M  (BIT(2))
#define GPIO_PIN16_PAD_DRIVER_V  0x1
#define GPIO_PIN16_PAD_DRIVER_S  2

#define GPIO_PIN17_REG          (DR_REG_GPIO_BASE + 0x00cc)
/* GPIO_PIN17_INT_ENA : R/W ;bitpos:[17:13] ;default: x ; */
/*description: bit0: APP CPU interrupt enable  bit1: APP CPU non-maskable interrupt
 enable  bit3: PRO CPU interrupt enable  bit4:  PRO CPU non-maskable interrupt enable  bit5: SDIO's extent interrupt enable*/
#define GPIO_PIN17_INT_ENA  0x0000001F
#define GPIO_PIN17_INT_ENA_M  ((GPIO_PIN17_INT_ENA_V)<<(GPIO_PIN17_INT_ENA_S))
#define GPIO_PIN17_INT_ENA_V  0x1F
#define GPIO_PIN17_INT_ENA_S  13
/* GPIO_PIN17_CONFIG : R/W ;bitpos:[12:11] ;default: x ; */
/*description: NA*/
#define GPIO_PIN17_CONFIG  0x00000003
#define GPIO_PIN17_CONFIG_M  ((GPIO_PIN17_CONFIG_V)<<(GPIO_PIN17_CONFIG_S))
#define GPIO_PIN17_CONFIG_V  0x3
#define GPIO_PIN17_CONFIG_S  11
/* GPIO_PIN17_WAKEUP_ENABLE : R/W ;bitpos:[10] ;default: x ; */
/*description: GPIO wake up enable  only available in light sleep*/
#define GPIO_PIN17_WAKEUP_ENABLE  (BIT(10))
#define GPIO_PIN17_WAKEUP_ENABLE_M  (BIT(10))
#define GPIO_PIN17_WAKEUP_ENABLE_V  0x1
#define GPIO_PIN17_WAKEUP_ENABLE_S  10
/* GPIO_PIN17_INT_TYPE : R/W ;bitpos:[9:7] ;default: x ; */
/*description: if set to 0: GPIO interrupt disable  if set to 1: rising edge
 trigger  if set to 2: falling edge trigger  if set to 3: any edge trigger  if set to 4: low level trigger  if set to 5: high level trigger*/
#define GPIO_PIN17_INT_TYPE  0x00000007
#define GPIO_PIN17_INT_TYPE_M  ((GPIO_PIN17_INT_TYPE_V)<<(GPIO_PIN17_INT_TYPE_S))
#define GPIO_PIN17_INT_TYPE_V  0x7
#define GPIO_PIN17_INT_TYPE_S  7
/* GPIO_PIN17_PAD_DRIVER : R/W ;bitpos:[2] ;default: x ; */
/*description: if set to 0: normal output  if set to 1: open drain*/
#define GPIO_PIN17_PAD_DRIVER  (BIT(2))
#define GPIO_PIN17_PAD_DRIVER_M  (BIT(2))
#define GPIO_PIN17_PAD_DRIVER_V  0x1
#define GPIO_PIN17_PAD_DRIVER_S  2

#define GPIO_PIN18_REG          (DR_REG_GPIO_BASE + 0x00d0)
/* GPIO_PIN18_INT_ENA : R/W ;bitpos:[17:13] ;default: x ; */
/*description: bit0: APP CPU interrupt enable  bit1: APP CPU non-maskable interrupt
 enable  bit3: PRO CPU interrupt enable  bit4:  PRO CPU non-maskable interrupt enable  bit5: SDIO's extent interrupt enable*/
#define GPIO_PIN18_INT_ENA  0x0000001F
#define GPIO_PIN18_INT_ENA_M  ((GPIO_PIN18_INT_ENA_V)<<(GPIO_PIN18_INT_ENA_S))
#define GPIO_PIN18_INT_ENA_V  0x1F
#define GPIO_PIN18_INT_ENA_S  13
/* GPIO_PIN18_CONFIG : R/W ;bitpos:[12:11] ;default: x ; */
/*description: NA*/
#define GPIO_PIN18_CONFIG  0x00000003
#define GPIO_PIN18_CONFIG_M  ((GPIO_PIN18_CONFIG_V)<<(GPIO_PIN18_CONFIG_S))
#define GPIO_PIN18_CONFIG_V  0x3
#define GPIO_PIN18_CONFIG_S  11
/* GPIO_PIN18_WAKEUP_ENABLE : R/W ;bitpos:[10] ;default: x ; */
/*description: GPIO wake up enable  only available in light sleep*/
#define GPIO_PIN18_WAKEUP_ENABLE  (BIT(10))
#define GPIO_PIN18_WAKEUP_ENABLE_M  (BIT(10))
#define GPIO_PIN18_WAKEUP_ENABLE_V  0x1
#define GPIO_PIN18_WAKEUP_ENABLE_S  10
/* GPIO_PIN18_INT_TYPE : R/W ;bitpos:[9:7] ;default: x ; */
/*description: if set to 0: GPIO interrupt disable  if set to 1: rising edge
 trigger  if set to 2: falling edge trigger  if set to 3: any edge trigger  if set to 4: low level trigger  if set to 5: high level trigger*/
#define GPIO_PIN18_INT_TYPE  0x00000007
#define GPIO_PIN18_INT_TYPE_M  ((GPIO_PIN18_INT_TYPE_V)<<(GPIO_PIN18_INT_TYPE_S))
#define GPIO_PIN18_INT_TYPE_V  0x7
#define GPIO_PIN18_INT_TYPE_S  7
/* GPIO_PIN18_PAD_DRIVER : R/W ;bitpos:[2] ;default: x ; */
/*description: if set to 0: normal output  if set to 1: open drain*/
#define GPIO_PIN18_PAD_DRIVER  (BIT(2))
#define GPIO_PIN18_PAD_DRIVER_M  (BIT(2))
#define GPIO_PIN18_PAD_DRIVER_V  0x1
#define GPIO_PIN18_PAD_DRIVER_S  2

#define GPIO_PIN19_REG          (DR_REG_GPIO_BASE + 0x00d4)
/* GPIO_PIN19_INT_ENA : R/W ;bitpos:[17:13] ;default: x ; */
/*description: bit0: APP CPU interrupt enable  bit1: APP CPU non-maskable interrupt
 enable  bit3: PRO CPU interrupt enable  bit4:  PRO CPU non-maskable interrupt enable  bit5: SDIO's extent interrupt enable*/
#define GPIO_PIN19_INT_ENA  0x0000001F
#define GPIO_PIN19_INT_ENA_M  ((GPIO_PIN19_INT_ENA_V)<<(GPIO_PIN19_INT_ENA_S))
#define GPIO_PIN19_INT_ENA_V  0x1F
#define GPIO_PIN19_INT_ENA_S  13
/* GPIO_PIN19_CONFIG : R/W ;bitpos:[12:11] ;default: x ; */
/*description: NA*/
#define GPIO_PIN19_CONFIG  0x00000003
#define GPIO_PIN19_CONFIG_M  ((GPIO_PIN19_CONFIG_V)<<(GPIO_PIN19_CONFIG_S))
#define GPIO_PIN19_CONFIG_V  0x3
#define GPIO_PIN19_CONFIG_S  11
/* GPIO_PIN19_WAKEUP_ENABLE : R/W ;bitpos:[10] ;default: x ; */
/*description: GPIO wake up enable  only available in light sleep*/
#define GPIO_PIN19_WAKEUP_ENABLE  (BIT(10))
#define GPIO_PIN19_WAKEUP_ENABLE_M  (BIT(10))
#define GPIO_PIN19_WAKEUP_ENABLE_V  0x1
#define GPIO_PIN19_WAKEUP_ENABLE_S  10
/* GPIO_PIN19_INT_TYPE : R/W ;bitpos:[9:7] ;default: x ; */
/*description: if set to 0: GPIO interrupt disable  if set to 1: rising edge
 trigger  if set to 2: falling edge trigger  if set to 3: any edge trigger  if set to 4: low level trigger  if set to 5: high level trigger*/
#define GPIO_PIN19_INT_TYPE  0x00000007
#define GPIO_PIN19_INT_TYPE_M  ((GPIO_PIN19_INT_TYPE_V)<<(GPIO_PIN19_INT_TYPE_S))
#define GPIO_PIN19_INT_TYPE_V  0x7
#define GPIO_PIN19_INT_TYPE_S  7
/* GPIO_PIN19_PAD_DRIVER : R/W ;bitpos:[2] ;default: x ; */
/*description: if set to 0: normal output  if set to 1: open drain*/
#define GPIO_PIN19_PAD_DRIVER  (BIT(2))
#define GPIO_PIN19_PAD_DRIVER_M  (BIT(2))
#define GPIO_PIN19_PAD_DRIVER_V  0x1
#define GPIO_PIN19_PAD_DRIVER_S  2

#define GPIO_PIN20_REG          (DR_REG_GPIO_BASE + 0x00d8)
/* GPIO_PIN20_INT_ENA : R/W ;bitpos:[17:13] ;default: x ; */
/*description: bit0: APP CPU interrupt enable  bit1: APP CPU non-mask interrupt
 enable  bit3: PRO CPU interrupt enable  bit4:  PRO CPU non-mask interrupt enable  bit5: SDIO's extent interrupt enable*/
#define GPIO_PIN20_INT_ENA  0x0000001F
#define GPIO_PIN20_INT_ENA_M  ((GPIO_PIN20_INT_ENA_V)<<(GPIO_PIN20_INT_ENA_S))
#define GPIO_PIN20_INT_ENA_V  0x1F
#define GPIO_PIN20_INT_ENA_S  13
/* GPIO_PIN20_CONFIG : R/W ;bitpos:[12:11] ;default: x ; */
/*description: NA*/
#define GPIO_PIN20_CONFIG  0x00000003
#define GPIO_PIN20_CONFIG_M  ((GPIO_PIN20_CONFIG_V)<<(GPIO_PIN20_CONFIG_S))
#define GPIO_PIN20_CONFIG_V  0x3
#define GPIO_PIN20_CONFIG_S  11
/* GPIO_PIN20_WAKEUP_ENABLE : R/W ;bitpos:[10] ;default: x ; */
/*description: GPIO wake up enable  only available in light sleep*/
#define GPIO_PIN20_WAKEUP_ENABLE  (BIT(10))
#define GPIO_PIN20_WAKEUP_ENABLE_M  (BIT(10))
#define GPIO_PIN20_WAKEUP_ENABLE_V  0x1
#define GPIO_PIN20_WAKEUP_ENABLE_S  10
/* GPIO_PIN20_INT_TYPE : R/W ;bitpos:[9:7] ;default: x ; */
/*description: if set to 0: GPIO interrupt disable  if set to 1: rising edge
 trigger  if set to 2: falling edge trigger  if set to 3: any edge trigger  if set to 4: low level trigger  if set to 5: high level trigger*/
#define GPIO_PIN20_INT_TYPE  0x00000007
#define GPIO_PIN20_INT_TYPE_M  ((GPIO_PIN20_INT_TYPE_V)<<(GPIO_PIN20_INT_TYPE_S))
#define GPIO_PIN20_INT_TYPE_V  0x7
#define GPIO_PIN20_INT_TYPE_S  7
/* GPIO_PIN20_PAD_DRIVER : R/W ;bitpos:[2] ;default: x ; */
/*description: if set to 0: normal output  if set to 1: open drain*/
#define GPIO_PIN20_PAD_DRIVER  (BIT(2))
#define GPIO_PIN20_PAD_DRIVER_M  (BIT(2))
#define GPIO_PIN20_PAD_DRIVER_V  0x1
#define GPIO_PIN20_PAD_DRIVER_S  2

#define GPIO_PIN21_REG          (DR_REG_GPIO_BASE + 0x00dc)
/* GPIO_PIN21_INT_ENA : R/W ;bitpos:[17:13] ;default: x ; */
/*description: bit0: APP CPU interrupt enable  bit1: APP CPU non-maskable interrupt
 enable  bit3: PRO CPU interrupt enable  bit4:  PRO CPU non-maskable interrupt enable  bit5: SDIO's extent interrupt enable*/
#define GPIO_PIN21_INT_ENA  0x0000001F
#define GPIO_PIN21_INT_ENA_M  ((GPIO_PIN21_INT_ENA_V)<<(GPIO_PIN21_INT_ENA_S))
#define GPIO_PIN21_INT_ENA_V  0x1F
#define GPIO_PIN21_INT_ENA_S  13
/* GPIO_PIN21_CONFIG : R/W ;bitpos:[12:11] ;default: x ; */
/*description: NA*/
#define GPIO_PIN21_CONFIG  0x00000003
#define GPIO_PIN21_CONFIG_M  ((GPIO_PIN21_CONFIG_V)<<(GPIO_PIN21_CONFIG_S))
#define GPIO_PIN21_CONFIG_V  0x3
#define GPIO_PIN21_CONFIG_S  11
/* GPIO_PIN21_WAKEUP_ENABLE : R/W ;bitpos:[10] ;default: x ; */
/*description: GPIO wake up enable  only available in light sleep*/
#define GPIO_PIN21_WAKEUP_ENABLE  (BIT(10))
#define GPIO_PIN21_WAKEUP_ENABLE_M  (BIT(10))
#define GPIO_PIN21_WAKEUP_ENABLE_V  0x1
#define GPIO_PIN21_WAKEUP_ENABLE_S  10
/* GPIO_PIN21_INT_TYPE : R/W ;bitpos:[9:7] ;default: x ; */
/*description: if set to 0: GPIO interrupt disable  if set to 1: rising edge
 trigger  if set to 2: falling edge trigger  if set to 3: any edge trigger  if set to 4: low level trigger  if set to 5: high level trigger*/
#define GPIO_PIN21_INT_TYPE  0x00000007
#define GPIO_PIN21_INT_TYPE_M  ((GPIO_PIN21_INT_TYPE_V)<<(GPIO_PIN21_INT_TYPE_S))
#define GPIO_PIN21_INT_TYPE_V  0x7
#define GPIO_PIN21_INT_TYPE_S  7
/* GPIO_PIN21_PAD_DRIVER : R/W ;bitpos:[2] ;default: x ; */
/*description: if set to 0: normal output  if set to 1: open drain*/
#define GPIO_PIN21_PAD_DRIVER  (BIT(2))
#define GPIO_PIN21_PAD_DRIVER_M  (BIT(2))
#define GPIO_PIN21_PAD_DRIVER_V  0x1
#define GPIO_PIN21_PAD_DRIVER_S  2

#define GPIO_PIN22_REG          (DR_REG_GPIO_BASE + 0x00e0)
/* GPIO_PIN22_INT_ENA : R/W ;bitpos:[17:13] ;default: x ; */
/*description: */
#define GPIO_PIN22_INT_ENA  0x0000001F
#define GPIO_PIN22_INT_ENA_M  ((GPIO_PIN22_INT_ENA_V)<<(GPIO_PIN22_INT_ENA_S))
#define GPIO_PIN22_INT_ENA_V  0x1F
#define GPIO_PIN22_INT_ENA_S  13
/* GPIO_PIN22_CONFIG : R/W ;bitpos:[12:11] ;default: x ; */
/*description: bit0: APP CPU interrupt enable  bit1: APP CPU non-maskable interrupt
 enable  bit3: PRO CPU interrupt enable  bit4:  PRO CPU non-maskable interrupt enable  bit5: SDIO's extent interrupt enable*/
#define GPIO_PIN22_CONFIG  0x00000003
#define GPIO_PIN22_CONFIG_M  ((GPIO_PIN22_CONFIG_V)<<(GPIO_PIN22_CONFIG_S))
#define GPIO_PIN22_CONFIG_V  0x3
#define GPIO_PIN22_CONFIG_S  11
/* GPIO_PIN22_WAKEUP_ENABLE : R/W ;bitpos:[10] ;default: x ; */
/*description: NA*/
#define GPIO_PIN22_WAKEUP_ENABLE  (BIT(10))
#define GPIO_PIN22_WAKEUP_ENABLE_M  (BIT(10))
#define GPIO_PIN22_WAKEUP_ENABLE_V  0x1
#define GPIO_PIN22_WAKEUP_ENABLE_S  10
/* GPIO_PIN22_INT_TYPE : R/W ;bitpos:[9:7] ;default: x ; */
/*description: GPIO wake up enable  only available in light sleep*/
#define GPIO_PIN22_INT_TYPE  0x00000007
#define GPIO_PIN22_INT_TYPE_M  ((GPIO_PIN22_INT_TYPE_V)<<(GPIO_PIN22_INT_TYPE_S))
#define GPIO_PIN22_INT_TYPE_V  0x7
#define GPIO_PIN22_INT_TYPE_S  7
/* GPIO_PIN22_PAD_DRIVER : R/W ;bitpos:[2] ;default: x ; */
/*description: */
#define GPIO_PIN22_PAD_DRIVER  (BIT(2))
#define GPIO_PIN22_PAD_DRIVER_M  (BIT(2))
#define GPIO_PIN22_PAD_DRIVER_V  0x1
#define GPIO_PIN22_PAD_DRIVER_S  2

#define GPIO_PIN23_REG          (DR_REG_GPIO_BASE + 0x00e4)
/* GPIO_PIN23_INT_ENA : R/W ;bitpos:[17:13] ;default: x ; */
/*description: bit0: APP CPU interrupt enable  bit1: APP CPU non-maskable interrupt
 enable  bit3: PRO CPU interrupt enable  bit4:  PRO CPU non-maskable interrupt enable  bit5: SDIO's extent interrupt enable*/
#define GPIO_PIN23_INT_ENA  0x0000001F
#define GPIO_PIN23_INT_ENA_M  ((GPIO_PIN23_INT_ENA_V)<<(GPIO_PIN23_INT_ENA_S))
#define GPIO_PIN23_INT_ENA_V  0x1F
#define GPIO_PIN23_INT_ENA_S  13
/* GPIO_PIN23_CONFIG : R/W ;bitpos:[12:11] ;default: x ; */
/*description: NA*/
#define GPIO_PIN23_CONFIG  0x00000003
#define GPIO_PIN23_CONFIG_M  ((GPIO_PIN23_CONFIG_V)<<(GPIO_PIN23_CONFIG_S))
#define GPIO_PIN23_CONFIG_V  0x3
#define GPIO_PIN23_CONFIG_S  11
/* GPIO_PIN23_WAKEUP_ENABLE : R/W ;bitpos:[10] ;default: x ; */
/*description: GPIO wake up enable  only available in light sleep*/
#define GPIO_PIN23_WAKEUP_ENABLE  (BIT(10))
#define GPIO_PIN23_WAKEUP_ENABLE_M  (BIT(10))
#define GPIO_PIN23_WAKEUP_ENABLE_V  0x1
#define GPIO_PIN23_WAKEUP_ENABLE_S  10
/* GPIO_PIN23_INT_TYPE : R/W ;bitpos:[9:7] ;default: x ; */
/*description: if set to 0: GPIO interrupt disable  if set to 1: rising edge
 trigger  if set to 2: falling edge trigger  if set to 3: any edge trigger  if set to 4: low level trigger  if set to 5: high level trigger*/
#define GPIO_PIN23_INT_TYPE  0x00000007
#define GPIO_PIN23_INT_TYPE_M  ((GPIO_PIN23_INT_TYPE_V)<<(GPIO_PIN23_INT_TYPE_S))
#define GPIO_PIN23_INT_TYPE_V  0x7
#define GPIO_PIN23_INT_TYPE_S  7
/* GPIO_PIN23_PAD_DRIVER : R/W ;bitpos:[2] ;default: x ; */
/*description: if set to 0: normal output  if set to 1: open drain*/
#define GPIO_PIN23_PAD_DRIVER  (BIT(2))
#define GPIO_PIN23_PAD_DRIVER_M  (BIT(2))
#define GPIO_PIN23_PAD_DRIVER_V  0x1
#define GPIO_PIN23_PAD_DRIVER_S  2

#define GPIO_PIN24_REG          (DR_REG_GPIO_BASE + 0x00e8)
/* GPIO_PIN24_INT_ENA : R/W ;bitpos:[17:13] ;default: x ; */
/*description: bit0: APP CPU interrupt enable  bit1: APP CPU non-maskable interrupt
 enable  bit3: PRO CPU interrupt enable  bit4:  PRO CPU non-maskable interrupt enable  bit5: SDIO's extent interrupt enable*/
#define GPIO_PIN24_INT_ENA  0x0000001F
#define GPIO_PIN24_INT_ENA_M  ((GPIO_PIN24_INT_ENA_V)<<(GPIO_PIN24_INT_ENA_S))
#define GPIO_PIN24_INT_ENA_V  0x1F
#define GPIO_PIN24_INT_ENA_S  13
/* GPIO_PIN24_CONFIG : R/W ;bitpos:[12:11] ;default: x ; */
/*description: NA*/
#define GPIO_PIN24_CONFIG  0x00000003
#define GPIO_PIN24_CONFIG_M  ((GPIO_PIN24_CONFIG_V)<<(GPIO_PIN24_CONFIG_S))
#define GPIO_PIN24_CONFIG_V  0x3
#define GPIO_PIN24_CONFIG_S  11
/* GPIO_PIN24_WAKEUP_ENABLE : R/W ;bitpos:[10] ;default: x ; */
/*description: GPIO wake up enable  only available in light sleep*/
#define GPIO_PIN24_WAKEUP_ENABLE  (BIT(10))
#define GPIO_PIN24_WAKEUP_ENABLE_M  (BIT(10))
#define GPIO_PIN24_WAKEUP_ENABLE_V  0x1
#define GPIO_PIN24_WAKEUP_ENABLE_S  10
/* GPIO_PIN24_INT_TYPE : R/W ;bitpos:[9:7] ;default: x ; */
/*description: if set to 0: GPIO interrupt disable  if set to 1: rising edge
 trigger  if set to 2: falling edge trigger  if set to 3: any edge trigger  if set to 4: low level trigger  if set to 5: high level trigger*/
#define GPIO_PIN24_INT_TYPE  0x00000007
#define GPIO_PIN24_INT_TYPE_M  ((GPIO_PIN24_INT_TYPE_V)<<(GPIO_PIN24_INT_TYPE_S))
#define GPIO_PIN24_INT_TYPE_V  0x7
#define GPIO_PIN24_INT_TYPE_S  7
/* GPIO_PIN24_PAD_DRIVER : R/W ;bitpos:[2] ;default: x ; */
/*description: if set to 0: normal output  if set to 1: open drain*/
#define GPIO_PIN24_PAD_DRIVER  (BIT(2))
#define GPIO_PIN24_PAD_DRIVER_M  (BIT(2))
#define GPIO_PIN24_PAD_DRIVER_V  0x1
#define GPIO_PIN24_PAD_DRIVER_S  2

#define GPIO_PIN25_REG          (DR_REG_GPIO_BASE + 0x00ec)
/* GPIO_PIN25_INT_ENA : R/W ;bitpos:[17:13] ;default: x ; */
/*description: bit0: APP CPU interrupt enable  bit1: APP CPU non-maskable interrupt
 enable  bit3: PRO CPU interrupt enable  bit4:  PRO CPU non-maskable interrupt enable  bit5: SDIO's extent interrupt enable*/
#define GPIO_PIN25_INT_ENA  0x0000001F
#define GPIO_PIN25_INT_ENA_M  ((GPIO_PIN25_INT_ENA_V)<<(GPIO_PIN25_INT_ENA_S))
#define GPIO_PIN25_INT_ENA_V  0x1F
#define GPIO_PIN25_INT_ENA_S  13
/* GPIO_PIN25_CONFIG : R/W ;bitpos:[12:11] ;default: x ; */
/*description: NA*/
#define GPIO_PIN25_CONFIG  0x00000003
#define GPIO_PIN25_CONFIG_M  ((GPIO_PIN25_CONFIG_V)<<(GPIO_PIN25_CONFIG_S))
#define GPIO_PIN25_CONFIG_V  0x3
#define GPIO_PIN25_CONFIG_S  11
/* GPIO_PIN25_WAKEUP_ENABLE : R/W ;bitpos:[10] ;default: x ; */
/*description: GPIO wake up enable  only available in light sleep*/
#define GPIO_PIN25_WAKEUP_ENABLE  (BIT(10))
#define GPIO_PIN25_WAKEUP_ENABLE_M  (BIT(10))
#define GPIO_PIN25_WAKEUP_ENABLE_V  0x1
#define GPIO_PIN25_WAKEUP_ENABLE_S  10
/* GPIO_PIN25_INT_TYPE : R/W ;bitpos:[9:7] ;default: x ; */
/*description: if set to 0: GPIO interrupt disable  if set to 1: rising edge
 trigger  if set to 2: falling edge trigger  if set to 3: any edge trigger  if set to 4: low level trigger  if set to 5: high level trigger*/
#define GPIO_PIN25_INT_TYPE  0x00000007
#define GPIO_PIN25_INT_TYPE_M  ((GPIO_PIN25_INT_TYPE_V)<<(GPIO_PIN25_INT_TYPE_S))
#define GPIO_PIN25_INT_TYPE_V  0x7
#define GPIO_PIN25_INT_TYPE_S  7
/* GPIO_PIN25_PAD_DRIVER : R/W ;bitpos:[2] ;default: x ; */
/*description: if set to 0: normal output  if set to 1: open drain*/
#define GPIO_PIN25_PAD_DRIVER  (BIT(2))
#define GPIO_PIN25_PAD_DRIVER_M  (BIT(2))
#define GPIO_PIN25_PAD_DRIVER_V  0x1
#define GPIO_PIN25_PAD_DRIVER_S  2

#define GPIO_PIN26_REG          (DR_REG_GPIO_BASE + 0x00f0)
/* GPIO_PIN26_INT_ENA : R/W ;bitpos:[17:13] ;default: x ; */
/*description: bit0: APP CPU interrupt enable  bit1: APP CPU non-maskable interrupt
 enable  bit3: PRO CPU interrupt enable  bit4:  PRO CPU non-maskable interrupt enable  bit5: SDIO's extent interrupt enable*/
#define GPIO_PIN26_INT_ENA  0x0000001F
#define GPIO_PIN26_INT_ENA_M  ((GPIO_PIN26_INT_ENA_V)<<(GPIO_PIN26_INT_ENA_S))
#define GPIO_PIN26_INT_ENA_V  0x1F
#define GPIO_PIN26_INT_ENA_S  13
/* GPIO_PIN26_CONFIG : R/W ;bitpos:[12:11] ;default: x ; */
/*description: NA*/
#define GPIO_PIN26_CONFIG  0x00000003
#define GPIO_PIN26_CONFIG_M  ((GPIO_PIN26_CONFIG_V)<<(GPIO_PIN26_CONFIG_S))
#define GPIO_PIN26_CONFIG_V  0x3
#define GPIO_PIN26_CONFIG_S  11
/* GPIO_PIN26_WAKEUP_ENABLE : R/W ;bitpos:[10] ;default: x ; */
/*description: GPIO wake up enable  only available in light sleep*/
#define GPIO_PIN26_WAKEUP_ENABLE  (BIT(10))
#define GPIO_PIN26_WAKEUP_ENABLE_M  (BIT(10))
#define GPIO_PIN26_WAKEUP_ENABLE_V  0x1
#define GPIO_PIN26_WAKEUP_ENABLE_S  10
/* GPIO_PIN26_INT_TYPE : R/W ;bitpos:[9:7] ;default: x ; */
/*description: if set to 0: GPIO interrupt disable  if set to 1: rising edge
 trigger  if set to 2: falling edge trigger  if set to 3: any edge trigger  if set to 4: low level trigger  if set to 5: high level trigger*/
#define GPIO_PIN26_INT_TYPE  0x00000007
#define GPIO_PIN26_INT_TYPE_M  ((GPIO_PIN26_INT_TYPE_V)<<(GPIO_PIN26_INT_TYPE_S))
#define GPIO_PIN26_INT_TYPE_V  0x7
#define GPIO_PIN26_INT_TYPE_S  7
/* GPIO_PIN26_PAD_DRIVER : R/W ;bitpos:[2] ;default: x ; */
/*description: if set to 0: normal output  if set to 1: open drain*/
#define GPIO_PIN26_PAD_DRIVER  (BIT(2))
#define GPIO_PIN26_PAD_DRIVER_M  (BIT(2))
#define GPIO_PIN26_PAD_DRIVER_V  0x1
#define GPIO_PIN26_PAD_DRIVER_S  2

#define GPIO_PIN27_REG          (DR_REG_GPIO_BASE + 0x00f4)
/* GPIO_PIN27_INT_ENA : R/W ;bitpos:[17:13] ;default: x ; */
/*description: bit0: APP CPU interrupt enable  bit1: APP CPU non-maskable interrupt
 enable  bit3: PRO CPU interrupt enable  bit4:  PRO CPU non-maskable interrupt enable  bit5: SDIO's extent interrupt enable*/
#define GPIO_PIN27_INT_ENA  0x0000001F
#define GPIO_PIN27_INT_ENA_M  ((GPIO_PIN27_INT_ENA_V)<<(GPIO_PIN27_INT_ENA_S))
#define GPIO_PIN27_INT_ENA_V  0x1F
#define GPIO_PIN27_INT_ENA_S  13
/* GPIO_PIN27_CONFIG : R/W ;bitpos:[12:11] ;default: x ; */
/*description: NA*/
#define GPIO_PIN27_CONFIG  0x00000003
#define GPIO_PIN27_CONFIG_M  ((GPIO_PIN27_CONFIG_V)<<(GPIO_PIN27_CONFIG_S))
#define GPIO_PIN27_CONFIG_V  0x3
#define GPIO_PIN27_CONFIG_S  11
/* GPIO_PIN27_WAKEUP_ENABLE : R/W ;bitpos:[10] ;default: x ; */
/*description: GPIO wake up enable  only available in light sleep*/
#define GPIO_PIN27_WAKEUP_ENABLE  (BIT(10))
#define GPIO_PIN27_WAKEUP_ENABLE_M  (BIT(10))
#define GPIO_PIN27_WAKEUP_ENABLE_V  0x1
#define GPIO_PIN27_WAKEUP_ENABLE_S  10
/* GPIO_PIN27_INT_TYPE : R/W ;bitpos:[9:7] ;default: x ; */
/*description: if set to 0: GPIO interrupt disable  if set to 1: rising edge
 trigger  if set to 2: falling edge trigger  if set to 3: any edge trigger  if set to 4: low level trigger  if set to 5: high level trigger*/
#define GPIO_PIN27_INT_TYPE  0x00000007
#define GPIO_PIN27_INT_TYPE_M  ((GPIO_PIN27_INT_TYPE_V)<<(GPIO_PIN27_INT_TYPE_S))
#define GPIO_PIN27_INT_TYPE_V  0x7
#define GPIO_PIN27_INT_TYPE_S  7
/* GPIO_PIN27_PAD_DRIVER : R/W ;bitpos:[2] ;default: x ; */
/*description: if set to 0: normal output  if set to 1: open drain*/
#define GPIO_PIN27_PAD_DRIVER  (BIT(2))
#define GPIO_PIN27_PAD_DRIVER_M  (BIT(2))
#define GPIO_PIN27_PAD_DRIVER_V  0x1
#define GPIO_PIN27_PAD_DRIVER_S  2

#define GPIO_PIN28_REG          (DR_REG_GPIO_BASE + 0x00f8)
/* GPIO_PIN28_INT_ENA : R/W ;bitpos:[17:13] ;default: x ; */
/*description: bit0: APP CPU interrupt enable  bit1: APP CPU non-maskable interrupt
 enable  bit3: PRO CPU interrupt enable  bit4:  PRO CPU non-maskable interrupt enable  bit5: SDIO's extent interrupt enable*/
#define GPIO_PIN28_INT_ENA  0x0000001F
#define GPIO_PIN28_INT_ENA_M  ((GPIO_PIN28_INT_ENA_V)<<(GPIO_PIN28_INT_ENA_S))
#define GPIO_PIN28_INT_ENA_V  0x1F
#define GPIO_PIN28_INT_ENA_S  13
/* GPIO_PIN28_CONFIG : R/W ;bitpos:[12:11] ;default: x ; */
/*description: NA*/
#define GPIO_PIN28_CONFIG  0x00000003
#define GPIO_PIN28_CONFIG_M  ((GPIO_PIN28_CONFIG_V)<<(GPIO_PIN28_CONFIG_S))
#define GPIO_PIN28_CONFIG_V  0x3
#define GPIO_PIN28_CONFIG_S  11
/* GPIO_PIN28_WAKEUP_ENABLE : R/W ;bitpos:[10] ;default: x ; */
/*description: GPIO wake up enable  only available in light sleep*/
#define GPIO_PIN28_WAKEUP_ENABLE  (BIT(10))
#define GPIO_PIN28_WAKEUP_ENABLE_M  (BIT(10))
#define GPIO_PIN28_WAKEUP_ENABLE_V  0x1
#define GPIO_PIN28_WAKEUP_ENABLE_S  10
/* GPIO_PIN28_INT_TYPE : R/W ;bitpos:[9:7] ;default: x ; */
/*description: if set to 0: GPIO interrupt disable  if set to 1: rising edge
 trigger  if set to 2: falling edge trigger  if set to 3: any edge trigger  if set to 4: low level trigger  if set to 5: high level trigger*/
#define GPIO_PIN28_INT_TYPE  0x00000007
#define GPIO_PIN28_INT_TYPE_M  ((GPIO_PIN28_INT_TYPE_V)<<(GPIO_PIN28_INT_TYPE_S))
#define GPIO_PIN28_INT_TYPE_V  0x7
#define GPIO_PIN28_INT_TYPE_S  7
/* GPIO_PIN28_PAD_DRIVER : R/W ;bitpos:[2] ;default: x ; */
/*description: if set to 0: normal output  if set to 1: open drain*/
#define GPIO_PIN28_PAD_DRIVER  (BIT(2))
#define GPIO_PIN28_PAD_DRIVER_M  (BIT(2))
#define GPIO_PIN28_PAD_DRIVER_V  0x1
#define GPIO_PIN28_PAD_DRIVER_S  2

#define GPIO_PIN29_REG          (DR_REG_GPIO_BASE + 0x00fc)
/* GPIO_PIN29_INT_ENA : R/W ;bitpos:[17:13] ;default: x ; */
/*description: bit0: APP CPU interrupt enable  bit1: APP CPU non-maskable interrupt
 enable  bit3: PRO CPU interrupt enable  bit4:  PRO CPU non-maskable interrupt enable  bit5: SDIO's extent interrupt enable*/
#define GPIO_PIN29_INT_ENA  0x0000001F
#define GPIO_PIN29_INT_ENA_M  ((GPIO_PIN29_INT_ENA_V)<<(GPIO_PIN29_INT_ENA_S))
#define GPIO_PIN29_INT_ENA_V  0x1F
#define GPIO_PIN29_INT_ENA_S  13
/* GPIO_PIN29_CONFIG : R/W ;bitpos:[12:11] ;default: x ; */
/*description: NA*/
#define GPIO_PIN29_CONFIG  0x00000003
#define GPIO_PIN29_CONFIG_M  ((GPIO_PIN29_CONFIG_V)<<(GPIO_PIN29_CONFIG_S))
#define GPIO_PIN29_CONFIG_V  0x3
#define GPIO_PIN29_CONFIG_S  11
/* GPIO_PIN29_WAKEUP_ENABLE : R/W ;bitpos:[10] ;default: x ; */
/*description: GPIO wake up enable  only available in light sleep*/
#define GPIO_PIN29_WAKEUP_ENABLE  (BIT(10))
#define GPIO_PIN29_WAKEUP_ENABLE_M  (BIT(10))
#define GPIO_PIN29_WAKEUP_ENABLE_V  0x1
#define GPIO_PIN29_WAKEUP_ENABLE_S  10
/* GPIO_PIN29_INT_TYPE : R/W ;bitpos:[9:7] ;default: x ; */
/*description: if set to 0: GPIO interrupt disable  if set to 1: rising edge
 trigger  if set to 2: falling edge trigger  if set to 3: any edge trigger  if set to 4: low level trigger  if set to 5: high level trigger*/
#define GPIO_PIN29_INT_TYPE  0x00000007
#define GPIO_PIN29_INT_TYPE_M  ((GPIO_PIN29_INT_TYPE_V)<<(GPIO_PIN29_INT_TYPE_S))
#define GPIO_PIN29_INT_TYPE_V  0x7
#define GPIO_PIN29_INT_TYPE_S  7
/* GPIO_PIN29_PAD_DRIVER : R/W ;bitpos:[2] ;default: x ; */
/*description: if set to 0: normal output  if set to 1: open drain*/
#define GPIO_PIN29_PAD_DRIVER  (BIT(2))
#define GPIO_PIN29_PAD_DRIVER_M  (BIT(2))
#define GPIO_PIN29_PAD_DRIVER_V  0x1
#define GPIO_PIN29_PAD_DRIVER_S  2

#define GPIO_PIN30_REG          (DR_REG_GPIO_BASE + 0x0100)
/* GPIO_PIN30_INT_ENA : R/W ;bitpos:[17:13] ;default: x ; */
/*description: bit0: APP CPU interrupt enable  bit1: APP CPU non-maskable interrupt
 enable  bit3: PRO CPU interrupt enable  bit4:  PRO CPU non-maskable interrupt enable  bit5: SDIO's extent interrupt enable*/
#define GPIO_PIN30_INT_ENA  0x0000001F
#define GPIO_PIN30_INT_ENA_M  ((GPIO_PIN30_INT_ENA_V)<<(GPIO_PIN30_INT_ENA_S))
#define GPIO_PIN30_INT_ENA_V  0x1F
#define GPIO_PIN30_INT_ENA_S  13
/* GPIO_PIN30_CONFIG : R/W ;bitpos:[12:11] ;default: x ; */
/*description: NA*/
#define GPIO_PIN30_CONFIG  0x00000003
#define GPIO_PIN30_CONFIG_M  ((GPIO_PIN30_CONFIG_V)<<(GPIO_PIN30_CONFIG_S))
#define GPIO_PIN30_CONFIG_V  0x3
#define GPIO_PIN30_CONFIG_S  11
/* GPIO_PIN30_WAKEUP_ENABLE : R/W ;bitpos:[10] ;default: x ; */
/*description: GPIO wake up enable  only available in light sleep*/
#define GPIO_PIN30_WAKEUP_ENABLE  (BIT(10))
#define GPIO_PIN30_WAKEUP_ENABLE_M  (BIT(10))
#define GPIO_PIN30_WAKEUP_ENABLE_V  0x1
#define GPIO_PIN30_WAKEUP_ENABLE_S  10
/* GPIO_PIN30_INT_TYPE : R/W ;bitpos:[9:7] ;default: x ; */
/*description: if set to 0: GPIO interrupt disable  if set to 1: rising edge
 trigger  if set to 2: falling edge trigger  if set to 3: any edge trigger  if set to 4: low level trigger  if set to 5: high level trigger*/
#define GPIO_PIN30_INT_TYPE  0x00000007
#define GPIO_PIN30_INT_TYPE_M  ((GPIO_PIN30_INT_TYPE_V)<<(GPIO_PIN30_INT_TYPE_S))
#define GPIO_PIN30_INT_TYPE_V  0x7
#define GPIO_PIN30_INT_TYPE_S  7
/* GPIO_PIN30_PAD_DRIVER : R/W ;bitpos:[2] ;default: x ; */
/*description: if set to 0: normal output  if set to 1: open drain*/
#define GPIO_PIN30_PAD_DRIVER  (BIT(2))
#define GPIO_PIN30_PAD_DRIVER_M  (BIT(2))
#define GPIO_PIN30_PAD_DRIVER_V  0x1
#define GPIO_PIN30_PAD_DRIVER_S  2

#define GPIO_PIN31_REG          (DR_REG_GPIO_BASE + 0x0104)
/* GPIO_PIN31_INT_ENA : R/W ;bitpos:[17:13] ;default: x ; */
/*description: bit0: APP CPU interrupt enable  bit1: APP CPU non-maskable interrupt
 enable  bit3: PRO CPU interrupt enable  bit4:  PRO CPU non-maskable interrupt enable  bit5: SDIO's extent interrupt enable*/
#define GPIO_PIN31_INT_ENA  0x0000001F
#define GPIO_PIN31_INT_ENA_M  ((GPIO_PIN31_INT_ENA_V)<<(GPIO_PIN31_INT_ENA_S))
#define GPIO_PIN31_INT_ENA_V  0x1F
#define GPIO_PIN31_INT_ENA_S  13
/* GPIO_PIN31_CONFIG : R/W ;bitpos:[12:11] ;default: x ; */
/*description: NA*/
#define GPIO_PIN31_CONFIG  0x00000003
#define GPIO_PIN31_CONFIG_M  ((GPIO_PIN31_CONFIG_V)<<(GPIO_PIN31_CONFIG_S))
#define GPIO_PIN31_CONFIG_V  0x3
#define GPIO_PIN31_CONFIG_S  11
/* GPIO_PIN31_WAKEUP_ENABLE : R/W ;bitpos:[10] ;default: x ; */
/*description: GPIO wake up enable  only available in light sleep*/
#define GPIO_PIN31_WAKEUP_ENABLE  (BIT(10))
#define GPIO_PIN31_WAKEUP_ENABLE_M  (BIT(10))
#define GPIO_PIN31_WAKEUP_ENABLE_V  0x1
#define GPIO_PIN31_WAKEUP_ENABLE_S  10
/* GPIO_PIN31_INT_TYPE : R/W ;bitpos:[9:7] ;default: x ; */
/*description: if set to 0: GPIO interrupt disable  if set to 1: rising edge
 trigger  if set to 2: falling edge trigger  if set to 3: any edge trigger  if set to 4: low level trigger  if set to 5: high level trigger*/
#define GPIO_PIN31_INT_TYPE  0x00000007
#define GPIO_PIN31_INT_TYPE_M  ((GPIO_PIN31_INT_TYPE_V)<<(GPIO_PIN31_INT_TYPE_S))
#define GPIO_PIN31_INT_TYPE_V  0x7
#define GPIO_PIN31_INT_TYPE_S  7
/* GPIO_PIN31_PAD_DRIVER : R/W ;bitpos:[2] ;default: x ; */
/*description: if set to 0: normal output  if set to 1: open drain*/
#define GPIO_PIN31_PAD_DRIVER  (BIT(2))
#define GPIO_PIN31_PAD_DRIVER_M  (BIT(2))
#define GPIO_PIN31_PAD_DRIVER_V  0x1
#define GPIO_PIN31_PAD_DRIVER_S  2

#define GPIO_PIN32_REG          (DR_REG_GPIO_BASE + 0x0108)
/* GPIO_PIN32_INT_ENA : R/W ;bitpos:[17:13] ;default: x ; */
/*description: bit0: APP CPU interrupt enable  bit1: APP CPU non-maskable interrupt
 enable  bit3: PRO CPU interrupt enable  bit4:  PRO CPU non-maskable interrupt enable  bit5: SDIO's extent interrupt enable*/
#define GPIO_PIN32_INT_ENA  0x0000001F
#define GPIO_PIN32_INT_ENA_M  ((GPIO_PIN32_INT_ENA_V)<<(GPIO_PIN32_INT_ENA_S))
#define GPIO_PIN32_INT_ENA_V  0x1F
#define GPIO_PIN32_INT_ENA_S  13
/* GPIO_PIN32_CONFIG : R/W ;bitpos:[12:11] ;default: x ; */
/*description: NA*/
#define GPIO_PIN32_CONFIG  0x00000003
#define GPIO_PIN32_CONFIG_M  ((GPIO_PIN32_CONFIG_V)<<(GPIO_PIN32_CONFIG_S))
#define GPIO_PIN32_CONFIG_V  0x3
#define GPIO_PIN32_CONFIG_S  11
/* GPIO_PIN32_WAKEUP_ENABLE : R/W ;bitpos:[10] ;default: x ; */
/*description: GPIO wake up enable  only available in light sleep*/
#define GPIO_PIN32_WAKEUP_ENABLE  (BIT(10))
#define GPIO_PIN32_WAKEUP_ENABLE_M  (BIT(10))
#define GPIO_PIN32_WAKEUP_ENABLE_V  0x1
#define GPIO_PIN32_WAKEUP_ENABLE_S  10
/* GPIO_PIN32_INT_TYPE : R/W ;bitpos:[9:7] ;default: x ; */
/*description: if set to 0: GPIO interrupt disable  if set to 1: rising edge
 trigger  if set to 2: falling edge trigger  if set to 3: any edge trigger  if set to 4: low level trigger  if set to 5: high level trigger*/
#define GPIO_PIN32_INT_TYPE  0x00000007
#define GPIO_PIN32_INT_TYPE_M  ((GPIO_PIN32_INT_TYPE_V)<<(GPIO_PIN32_INT_TYPE_S))
#define GPIO_PIN32_INT_TYPE_V  0x7
#define GPIO_PIN32_INT_TYPE_S  7
/* GPIO_PIN32_PAD_DRIVER : R/W ;bitpos:[2] ;default: x ; */
/*description: if set to 0: normal output  if set to 1: open drain*/
#define GPIO_PIN32_PAD_DRIVER  (BIT(2))
#define GPIO_PIN32_PAD_DRIVER_M  (BIT(2))
#define GPIO_PIN32_PAD_DRIVER_V  0x1
#define GPIO_PIN32_PAD_DRIVER_S  2

#define GPIO_PIN33_REG          (DR_REG_GPIO_BASE + 0x010c)
/* GPIO_PIN33_INT_ENA : R/W ;bitpos:[17:13] ;default: x ; */
/*description: bit0: APP CPU interrupt enable  bit1: APP CPU non-maskable interrupt
 enable  bit3: PRO CPU interrupt enable  bit4:  PRO CPU non-maskable interrupt enable  bit5: SDIO's extent interrupt enable*/
#define GPIO_PIN33_INT_ENA  0x0000001F
#define GPIO_PIN33_INT_ENA_M  ((GPIO_PIN33_INT_ENA_V)<<(GPIO_PIN33_INT_ENA_S))
#define GPIO_PIN33_INT_ENA_V  0x1F
#define GPIO_PIN33_INT_ENA_S  13
/* GPIO_PIN33_CONFIG : R/W ;bitpos:[12:11] ;default: x ; */
/*description: NA*/
#define GPIO_PIN33_CONFIG  0x00000003
#define GPIO_PIN33_CONFIG_M  ((GPIO_PIN33_CONFIG_V)<<(GPIO_PIN33_CONFIG_S))
#define GPIO_PIN33_CONFIG_V  0x3
#define GPIO_PIN33_CONFIG_S  11
/* GPIO_PIN33_WAKEUP_ENABLE : R/W ;bitpos:[10] ;default: x ; */
/*description: GPIO wake up enable  only available in light sleep*/
#define GPIO_PIN33_WAKEUP_ENABLE  (BIT(10))
#define GPIO_PIN33_WAKEUP_ENABLE_M  (BIT(10))
#define GPIO_PIN33_WAKEUP_ENABLE_V  0x1
#define GPIO_PIN33_WAKEUP_ENABLE_S  10
/* GPIO_PIN33_INT_TYPE : R/W ;bitpos:[9:7] ;default: x ; */
/*description: if set to 0: GPIO interrupt disable  if set to 1: rising edge
 trigger  if set to 2: falling edge trigger  if set to 3: any edge trigger  if set to 4: low level trigger  if set to 5: high level trigger*/
#define GPIO_PIN33_INT_TYPE  0x00000007
#define GPIO_PIN33_INT_TYPE_M  ((GPIO_PIN33_INT_TYPE_V)<<(GPIO_PIN33_INT_TYPE_S))
#define GPIO_PIN33_INT_TYPE_V  0x7
#define GPIO_PIN33_INT_TYPE_S  7
/* GPIO_PIN33_PAD_DRIVER : R/W ;bitpos:[2] ;default: x ; */
/*description: if set to 0: normal output  if set to 1: open drain*/
#define GPIO_PIN33_PAD_DRIVER  (BIT(2))
#define GPIO_PIN33_PAD_DRIVER_M  (BIT(2))
#define GPIO_PIN33_PAD_DRIVER_V  0x1
#define GPIO_PIN33_PAD_DRIVER_S  2

#define GPIO_PIN34_REG          (DR_REG_GPIO_BASE + 0x0110)
/* GPIO_PIN34_INT_ENA : R/W ;bitpos:[17:13] ;default: x ; */
/*description: bit0: APP CPU interrupt enable  bit1: APP CPU non-maskable interrupt
 enable  bit3: PRO CPU interrupt enable  bit4:  PRO CPU non-maskable interrupt enable  bit5: SDIO's extent interrupt enable*/
#define GPIO_PIN34_INT_ENA  0x0000001F
#define GPIO_PIN34_INT_ENA_M  ((GPIO_PIN34_INT_ENA_V)<<(GPIO_PIN34_INT_ENA_S))
#define GPIO_PIN34_INT_ENA_V  0x1F
#define GPIO_PIN34_INT_ENA_S  13
/* GPIO_PIN34_CONFIG : R/W ;bitpos:[12:11] ;default: x ; */
/*description: NA*/
#define GPIO_PIN34_CONFIG  0x00000003
#define GPIO_PIN34_CONFIG_M  ((GPIO_PIN34_CONFIG_V)<<(GPIO_PIN34_CONFIG_S))
#define GPIO_PIN34_CONFIG_V  0x3
#define GPIO_PIN34_CONFIG_S  11
/* GPIO_PIN34_WAKEUP_ENABLE : R/W ;bitpos:[10] ;default: x ; */
/*description: GPIO wake up enable  only available in light sleep*/
#define GPIO_PIN34_WAKEUP_ENABLE  (BIT(10))
#define GPIO_PIN34_WAKEUP_ENABLE_M  (BIT(10))
#define GPIO_PIN34_WAKEUP_ENABLE_V  0x1
#define GPIO_PIN34_WAKEUP_ENABLE_S  10
/* GPIO_PIN34_INT_TYPE : R/W ;bitpos:[9:7] ;default: x ; */
/*description: if set to 0: GPIO interrupt disable  if set to 1: rising edge
 trigger  if set to 2: falling edge trigger  if set to 3: any edge trigger  if set to 4: low level trigger  if set to 5: high level trigger*/
#define GPIO_PIN34_INT_TYPE  0x00000007
#define GPIO_PIN34_INT_TYPE_M  ((GPIO_PIN34_INT_TYPE_V)<<(GPIO_PIN34_INT_TYPE_S))
#define GPIO_PIN34_INT_TYPE_V  0x7
#define GPIO_PIN34_INT_TYPE_S  7
/* GPIO_PIN34_PAD_DRIVER : R/W ;bitpos:[2] ;default: x ; */
/*description: if set to 0: normal output  if set to 1: open drain*/
#define GPIO_PIN34_PAD_DRIVER  (BIT(2))
#define GPIO_PIN34_PAD_DRIVER_M  (BIT(2))
#define GPIO_PIN34_PAD_DRIVER_V  0x1
#define GPIO_PIN34_PAD_DRIVER_S  2

#define GPIO_PIN35_REG          (DR_REG_GPIO_BASE + 0x0114)
/* GPIO_PIN35_INT_ENA : R/W ;bitpos:[17:13] ;default: x ; */
/*description: bit0: APP CPU interrupt enable  bit1: APP CPU non-maskable interrupt
 enable  bit3: PRO CPU interrupt enable  bit4:  PRO CPU non-maskable interrupt enable  bit5: SDIO's extent interrupt enable*/
#define GPIO_PIN35_INT_ENA  0x0000001F
#define GPIO_PIN35_INT_ENA_M  ((GPIO_PIN35_INT_ENA_V)<<(GPIO_PIN35_INT_ENA_S))
#define GPIO_PIN35_INT_ENA_V  0x1F
#define GPIO_PIN35_INT_ENA_S  13
/* GPIO_PIN35_CONFIG : R/W ;bitpos:[12:11] ;default: x ; */
/*description: NA*/
#define GPIO_PIN35_CONFIG  0x00000003
#define GPIO_PIN35_CONFIG_M  ((GPIO_PIN35_CONFIG_V)<<(GPIO_PIN35_CONFIG_S))
#define GPIO_PIN35_CONFIG_V  0x3
#define GPIO_PIN35_CONFIG_S  11
/* GPIO_PIN35_WAKEUP_ENABLE : R/W ;bitpos:[10] ;default: x ; */
/*description: GPIO wake up enable  only available in light sleep*/
#define GPIO_PIN35_WAKEUP_ENABLE  (BIT(10))
#define GPIO_PIN35_WAKEUP_ENABLE_M  (BIT(10))
#define GPIO_PIN35_WAKEUP_ENABLE_V  0x1
#define GPIO_PIN35_WAKEUP_ENABLE_S  10
/* GPIO_PIN35_INT_TYPE : R/W ;bitpos:[9:7] ;default: x ; */
/*description: if set to 0: GPIO interrupt disable  if set to 1: rising edge
 trigger  if set to 2: falling edge trigger  if set to 3: any edge trigger  if set to 4: low level trigger  if set to 5: high level trigger*/
#define GPIO_PIN35_INT_TYPE  0x00000007
#define GPIO_PIN35_INT_TYPE_M  ((GPIO_PIN35_INT_TYPE_V)<<(GPIO_PIN35_INT_TYPE_S))
#define GPIO_PIN35_INT_TYPE_V  0x7
#define GPIO_PIN35_INT_TYPE_S  7
/* GPIO_PIN35_PAD_DRIVER : R/W ;bitpos:[2] ;default: x ; */
/*description: if set to 0: normal output  if set to 1: open drain*/
#define GPIO_PIN35_PAD_DRIVER  (BIT(2))
#define GPIO_PIN35_PAD_DRIVER_M  (BIT(2))
#define GPIO_PIN35_PAD_DRIVER_V  0x1
#define GPIO_PIN35_PAD_DRIVER_S  2

#define GPIO_PIN36_REG          (DR_REG_GPIO_BASE + 0x0118)
/* GPIO_PIN36_INT_ENA : R/W ;bitpos:[17:13] ;default: x ; */
/*description: bit0: APP CPU interrupt enable  bit1: APP CPU non-maskable interrupt
 enable  bit3: PRO CPU interrupt enable  bit4:  PRO CPU non-maskable interrupt enable  bit5: SDIO's extent interrupt enable*/
#define GPIO_PIN36_INT_ENA  0x0000001F
#define GPIO_PIN36_INT_ENA_M  ((GPIO_PIN36_INT_ENA_V)<<(GPIO_PIN36_INT_ENA_S))
#define GPIO_PIN36_INT_ENA_V  0x1F
#define GPIO_PIN36_INT_ENA_S  13
/* GPIO_PIN36_CONFIG : R/W ;bitpos:[12:11] ;default: x ; */
/*description: NA*/
#define GPIO_PIN36_CONFIG  0x00000003
#define GPIO_PIN36_CONFIG_M  ((GPIO_PIN36_CONFIG_V)<<(GPIO_PIN36_CONFIG_S))
#define GPIO_PIN36_CONFIG_V  0x3
#define GPIO_PIN36_CONFIG_S  11
/* GPIO_PIN36_WAKEUP_ENABLE : R/W ;bitpos:[10] ;default: x ; */
/*description: GPIO wake up enable  only available in light sleep*/
#define GPIO_PIN36_WAKEUP_ENABLE  (BIT(10))
#define GPIO_PIN36_WAKEUP_ENABLE_M  (BIT(10))
#define GPIO_PIN36_WAKEUP_ENABLE_V  0x1
#define GPIO_PIN36_WAKEUP_ENABLE_S  10
/* GPIO_PIN36_INT_TYPE : R/W ;bitpos:[9:7] ;default: x ; */
/*description: if set to 0: GPIO interrupt disable  if set to 1: rising edge
 trigger  if set to 2: falling edge trigger  if set to 3: any edge trigger  if set to 4: low level trigger  if set to 5: high level trigger*/
#define GPIO_PIN36_INT_TYPE  0x00000007
#define GPIO_PIN36_INT_TYPE_M  ((GPIO_PIN36_INT_TYPE_V)<<(GPIO_PIN36_INT_TYPE_S))
#define GPIO_PIN36_INT_TYPE_V  0x7
#define GPIO_PIN36_INT_TYPE_S  7
/* GPIO_PIN36_PAD_DRIVER : R/W ;bitpos:[2] ;default: x ; */
/*description: if set to 0: normal output  if set to 1: open drain*/
#define GPIO_PIN36_PAD_DRIVER  (BIT(2))
#define GPIO_PIN36_PAD_DRIVER_M  (BIT(2))
#define GPIO_PIN36_PAD_DRIVER_V  0x1
#define GPIO_PIN36_PAD_DRIVER_S  2

#define GPIO_PIN37_REG          (DR_REG_GPIO_BASE + 0x011c)
/* GPIO_PIN37_INT_ENA : R/W ;bitpos:[17:13] ;default: x ; */
/*description: bit0: APP CPU interrupt enable  bit1: APP CPU non-maskable interrupt
 enable  bit3: PRO CPU interrupt enable  bit4:  PRO CPU non-maskable interrupt enable  bit5: SDIO's extent interrupt enable*/
#define GPIO_PIN37_INT_ENA  0x0000001F
#define GPIO_PIN37_INT_ENA_M  ((GPIO_PIN37_INT_ENA_V)<<(GPIO_PIN37_INT_ENA_S))
#define GPIO_PIN37_INT_ENA_V  0x1F
#define GPIO_PIN37_INT_ENA_S  13
/* GPIO_PIN37_CONFIG : R/W ;bitpos:[12:11] ;default: x ; */
/*description: NA*/
#define GPIO_PIN37_CONFIG  0x00000003
#define GPIO_PIN37_CONFIG_M  ((GPIO_PIN37_CONFIG_V)<<(GPIO_PIN37_CONFIG_S))
#define GPIO_PIN37_CONFIG_V  0x3
#define GPIO_PIN37_CONFIG_S  11
/* GPIO_PIN37_WAKEUP_ENABLE : R/W ;bitpos:[10] ;default: x ; */
/*description: GPIO wake up enable  only available in light sleep*/
#define GPIO_PIN37_WAKEUP_ENABLE  (BIT(10))
#define GPIO_PIN37_WAKEUP_ENABLE_M  (BIT(10))
#define GPIO_PIN37_WAKEUP_ENABLE_V  0x1
#define GPIO_PIN37_WAKEUP_ENABLE_S  10
/* GPIO_PIN37_INT_TYPE : R/W ;bitpos:[9:7] ;default: x ; */
/*description: if set to 0: GPIO interrupt disable  if set to 1: rising edge
 trigger  if set to 2: falling edge trigger  if set to 3: any edge trigger  if set to 4: low level trigger  if set to 5: high level trigger*/
#define GPIO_PIN37_INT_TYPE  0x00000007
#define GPIO_PIN37_INT_TYPE_M  ((GPIO_PIN37_INT_TYPE_V)<<(GPIO_PIN37_INT_TYPE_S))
#define GPIO_PIN37_INT_TYPE_V  0x7
#define GPIO_PIN37_INT_TYPE_S  7
/* GPIO_PIN37_PAD_DRIVER : R/W ;bitpos:[2] ;default: x ; */
/*description: if set to 0: normal output  if set to 1: open drain*/
#define GPIO_PIN37_PAD_DRIVER  (BIT(2))
#define GPIO_PIN37_PAD_DRIVER_M  (BIT(2))
#define GPIO_PIN37_PAD_DRIVER_V  0x1
#define GPIO_PIN37_PAD_DRIVER_S  2

#define GPIO_PIN38_REG          (DR_REG_GPIO_BASE + 0x0120)
/* GPIO_PIN38_INT_ENA : R/W ;bitpos:[17:13] ;default: x ; */
/*description: bit0: APP CPU interrupt enable  bit1: APP CPU non-maskable interrupt
 enable  bit3: PRO CPU interrupt enable  bit4:  PRO CPU non-maskable interrupt enable  bit5: SDIO's extent interrupt enable*/
#define GPIO_PIN38_INT_ENA  0x0000001F
#define GPIO_PIN38_INT_ENA_M  ((GPIO_PIN38_INT_ENA_V)<<(GPIO_PIN38_INT_ENA_S))
#define GPIO_PIN38_INT_ENA_V  0x1F
#define GPIO_PIN38_INT_ENA_S  13
/* GPIO_PIN38_CONFIG : R/W ;bitpos:[12:11] ;default: x ; */
/*description: NA*/
#define GPIO_PIN38_CONFIG  0x00000003
#define GPIO_PIN38_CONFIG_M  ((GPIO_PIN38_CONFIG_V)<<(GPIO_PIN38_CONFIG_S))
#define GPIO_PIN38_CONFIG_V  0x3
#define GPIO_PIN38_CONFIG_S  11
/* GPIO_PIN38_WAKEUP_ENABLE : R/W ;bitpos:[10] ;default: x ; */
/*description: GPIO wake up enable  only available in light sleep*/
#define GPIO_PIN38_WAKEUP_ENABLE  (BIT(10))
#define GPIO_PIN38_WAKEUP_ENABLE_M  (BIT(10))
#define GPIO_PIN38_WAKEUP_ENABLE_V  0x1
#define GPIO_PIN38_WAKEUP_ENABLE_S  10
/* GPIO_PIN38_INT_TYPE : R/W ;bitpos:[9:7] ;default: x ; */
/*description: if set to 0: GPIO interrupt disable  if set to 1: rising edge
 trigger  if set to 2: falling edge trigger  if set to 3: any edge trigger  if set to 4: low level trigger  if set to 5: high level trigger*/
#define GPIO_PIN38_INT_TYPE  0x00000007
#define GPIO_PIN38_INT_TYPE_M  ((GPIO_PIN38_INT_TYPE_V)<<(GPIO_PIN38_INT_TYPE_S))
#define GPIO_PIN38_INT_TYPE_V  0x7
#define GPIO_PIN38_INT_TYPE_S  7
/* GPIO_PIN38_PAD_DRIVER : R/W ;bitpos:[2] ;default: x ; */
/*description: if set to 0: normal output  if set to 1: open drain*/
#define GPIO_PIN38_PAD_DRIVER  (BIT(2))
#define GPIO_PIN38_PAD_DRIVER_M  (BIT(2))
#define GPIO_PIN38_PAD_DRIVER_V  0x1
#define GPIO_PIN38_PAD_DRIVER_S  2

#define GPIO_PIN39_REG          (DR_REG_GPIO_BASE + 0x0124)
/* GPIO_PIN39_INT_ENA : R/W ;bitpos:[17:13] ;default: x ; */
/*description: bit0: APP CPU interrupt enable  bit1: APP CPU non-maskable interrupt
 enable  bit3: PRO CPU interrupt enable  bit4:  PRO CPU non-maskable interrupt enable  bit5: SDIO's extent interrupt enable*/
#define GPIO_PIN39_INT_ENA  0x0000001F
#define GPIO_PIN39_INT_ENA_M  ((GPIO_PIN39_INT_ENA_V)<<(GPIO_PIN39_INT_ENA_S))
#define GPIO_PIN39_INT_ENA_V  0x1F
#define GPIO_PIN39_INT_ENA_S  13
/* GPIO_PIN39_CONFIG : R/W ;bitpos:[12:11] ;default: x ; */
/*description: NA*/
#define GPIO_PIN39_CONFIG  0x00000003
#define GPIO_PIN39_CONFIG_M  ((GPIO_PIN39_CONFIG_V)<<(GPIO_PIN39_CONFIG_S))
#define GPIO_PIN39_CONFIG_V  0x3
#define GPIO_PIN39_CONFIG_S  11
/* GPIO_PIN39_WAKEUP_ENABLE : R/W ;bitpos:[10] ;default: x ; */
/*description: GPIO wake up enable  only available in light sleep*/
#define GPIO_PIN39_WAKEUP_ENABLE  (BIT(10))
#define GPIO_PIN39_WAKEUP_ENABLE_M  (BIT(10))
#define GPIO_PIN39_WAKEUP_ENABLE_V  0x1
#define GPIO_PIN39_WAKEUP_ENABLE_S  10
/* GPIO_PIN39_INT_TYPE : R/W ;bitpos:[9:7] ;default: x ; */
/*description: if set to 0: GPIO interrupt disable  if set to 1: rising edge
 trigger  if set to 2: falling edge trigger  if set to 3: any edge trigger  if set to 4: low level trigger  if set to 5: high level trigger*/
#define GPIO_PIN39_INT_TYPE  0x00000007
#define GPIO_PIN39_INT_TYPE_M  ((GPIO_PIN39_INT_TYPE_V)<<(GPIO_PIN39_INT_TYPE_S))
#define GPIO_PIN39_INT_TYPE_V  0x7
#define GPIO_PIN39_INT_TYPE_S  7
/* GPIO_PIN39_PAD_DRIVER : R/W ;bitpos:[2] ;default: x ; */
/*description: if set to 0: normal output  if set to 1: open drain*/
#define GPIO_PIN39_PAD_DRIVER  (BIT(2))
#define GPIO_PIN39_PAD_DRIVER_M  (BIT(2))
#define GPIO_PIN39_PAD_DRIVER_V  0x1
#define GPIO_PIN39_PAD_DRIVER_S  2

#define GPIO_cali_conf_REG          (DR_REG_GPIO_BASE + 0x0128)
/* GPIO_CALI_START : R/W ;bitpos:[31] ;default: x ; */
/*description: */
#define GPIO_CALI_START  (BIT(31))
#define GPIO_CALI_START_M  (BIT(31))
#define GPIO_CALI_START_V  0x1
#define GPIO_CALI_START_S  31
/* GPIO_CALI_RTC_MAX : R/W ;bitpos:[9:0] ;default: x ; */
/*description: */
#define GPIO_CALI_RTC_MAX  0x000003FF
#define GPIO_CALI_RTC_MAX_M  ((GPIO_CALI_RTC_MAX_V)<<(GPIO_CALI_RTC_MAX_S))
#define GPIO_CALI_RTC_MAX_V  0x3FF
#define GPIO_CALI_RTC_MAX_S  0

#define GPIO_cali_data_REG          (DR_REG_GPIO_BASE + 0x012c)
/* GPIO_CALI_RDY_SYNC2 : RO ;bitpos:[31] ;default:  ; */
/*description: */
#define GPIO_CALI_RDY_SYNC2  (BIT(31))
#define GPIO_CALI_RDY_SYNC2_M  (BIT(31))
#define GPIO_CALI_RDY_SYNC2_V  0x1
#define GPIO_CALI_RDY_SYNC2_S  31
/* GPIO_CALI_RDY_REAL : RO ;bitpos:[30] ;default:  ; */
/*description: */
#define GPIO_CALI_RDY_REAL  (BIT(30))
#define GPIO_CALI_RDY_REAL_M  (BIT(30))
#define GPIO_CALI_RDY_REAL_V  0x1
#define GPIO_CALI_RDY_REAL_S  30
/* GPIO_CALI_VALUE_SYNC2 : RO ;bitpos:[19:0] ;default:  ; */
/*description: */
#define GPIO_CALI_VALUE_SYNC2  0x000FFFFF
#define GPIO_CALI_VALUE_SYNC2_M  ((GPIO_CALI_VALUE_SYNC2_V)<<(GPIO_CALI_VALUE_SYNC2_S))
#define GPIO_CALI_VALUE_SYNC2_V  0xFFFFF
#define GPIO_CALI_VALUE_SYNC2_S  0

#define GPIO_FUNC0_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x0130)
/* GPIO_SIG0_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG0_IN_SEL  (BIT(7))
#define GPIO_SIG0_IN_SEL_M  (BIT(7))
#define GPIO_SIG0_IN_SEL_V  0x1
#define GPIO_SIG0_IN_SEL_S  7
/* GPIO_FUNC0_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC0_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC0_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC0_IN_INV_SEL_V  0x1
#define GPIO_FUNC0_IN_INV_SEL_S  6
/* GPIO_FUNC0_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC0_IN_SEL  0x0000003F
#define GPIO_FUNC0_IN_SEL_M  ((GPIO_FUNC0_IN_SEL_V)<<(GPIO_FUNC0_IN_SEL_S))
#define GPIO_FUNC0_IN_SEL_V  0x3F
#define GPIO_FUNC0_IN_SEL_S  0

#define GPIO_FUNC1_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x0134)
/* GPIO_SIG1_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG1_IN_SEL  (BIT(7))
#define GPIO_SIG1_IN_SEL_M  (BIT(7))
#define GPIO_SIG1_IN_SEL_V  0x1
#define GPIO_SIG1_IN_SEL_S  7
/* GPIO_FUNC1_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC1_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC1_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC1_IN_INV_SEL_V  0x1
#define GPIO_FUNC1_IN_INV_SEL_S  6
/* GPIO_FUNC1_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC1_IN_SEL  0x0000003F
#define GPIO_FUNC1_IN_SEL_M  ((GPIO_FUNC1_IN_SEL_V)<<(GPIO_FUNC1_IN_SEL_S))
#define GPIO_FUNC1_IN_SEL_V  0x3F
#define GPIO_FUNC1_IN_SEL_S  0

#define GPIO_FUNC2_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x0138)
/* GPIO_SIG2_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG2_IN_SEL  (BIT(7))
#define GPIO_SIG2_IN_SEL_M  (BIT(7))
#define GPIO_SIG2_IN_SEL_V  0x1
#define GPIO_SIG2_IN_SEL_S  7
/* GPIO_FUNC2_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC2_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC2_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC2_IN_INV_SEL_V  0x1
#define GPIO_FUNC2_IN_INV_SEL_S  6
/* GPIO_FUNC2_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC2_IN_SEL  0x0000003F
#define GPIO_FUNC2_IN_SEL_M  ((GPIO_FUNC2_IN_SEL_V)<<(GPIO_FUNC2_IN_SEL_S))
#define GPIO_FUNC2_IN_SEL_V  0x3F
#define GPIO_FUNC2_IN_SEL_S  0

#define GPIO_FUNC3_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x013c)
/* GPIO_SIG3_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG3_IN_SEL  (BIT(7))
#define GPIO_SIG3_IN_SEL_M  (BIT(7))
#define GPIO_SIG3_IN_SEL_V  0x1
#define GPIO_SIG3_IN_SEL_S  7
/* GPIO_FUNC3_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC3_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC3_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC3_IN_INV_SEL_V  0x1
#define GPIO_FUNC3_IN_INV_SEL_S  6
/* GPIO_FUNC3_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC3_IN_SEL  0x0000003F
#define GPIO_FUNC3_IN_SEL_M  ((GPIO_FUNC3_IN_SEL_V)<<(GPIO_FUNC3_IN_SEL_S))
#define GPIO_FUNC3_IN_SEL_V  0x3F
#define GPIO_FUNC3_IN_SEL_S  0

#define GPIO_FUNC4_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x0140)
/* GPIO_SIG4_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG4_IN_SEL  (BIT(7))
#define GPIO_SIG4_IN_SEL_M  (BIT(7))
#define GPIO_SIG4_IN_SEL_V  0x1
#define GPIO_SIG4_IN_SEL_S  7
/* GPIO_FUNC4_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC4_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC4_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC4_IN_INV_SEL_V  0x1
#define GPIO_FUNC4_IN_INV_SEL_S  6
/* GPIO_FUNC4_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC4_IN_SEL  0x0000003F
#define GPIO_FUNC4_IN_SEL_M  ((GPIO_FUNC4_IN_SEL_V)<<(GPIO_FUNC4_IN_SEL_S))
#define GPIO_FUNC4_IN_SEL_V  0x3F
#define GPIO_FUNC4_IN_SEL_S  0

#define GPIO_FUNC5_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x0144)
/* GPIO_SIG5_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG5_IN_SEL  (BIT(7))
#define GPIO_SIG5_IN_SEL_M  (BIT(7))
#define GPIO_SIG5_IN_SEL_V  0x1
#define GPIO_SIG5_IN_SEL_S  7
/* GPIO_FUNC5_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC5_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC5_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC5_IN_INV_SEL_V  0x1
#define GPIO_FUNC5_IN_INV_SEL_S  6
/* GPIO_FUNC5_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC5_IN_SEL  0x0000003F
#define GPIO_FUNC5_IN_SEL_M  ((GPIO_FUNC5_IN_SEL_V)<<(GPIO_FUNC5_IN_SEL_S))
#define GPIO_FUNC5_IN_SEL_V  0x3F
#define GPIO_FUNC5_IN_SEL_S  0

#define GPIO_FUNC6_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x0148)
/* GPIO_SIG6_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG6_IN_SEL  (BIT(7))
#define GPIO_SIG6_IN_SEL_M  (BIT(7))
#define GPIO_SIG6_IN_SEL_V  0x1
#define GPIO_SIG6_IN_SEL_S  7
/* GPIO_FUNC6_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC6_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC6_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC6_IN_INV_SEL_V  0x1
#define GPIO_FUNC6_IN_INV_SEL_S  6
/* GPIO_FUNC6_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC6_IN_SEL  0x0000003F
#define GPIO_FUNC6_IN_SEL_M  ((GPIO_FUNC6_IN_SEL_V)<<(GPIO_FUNC6_IN_SEL_S))
#define GPIO_FUNC6_IN_SEL_V  0x3F
#define GPIO_FUNC6_IN_SEL_S  0

#define GPIO_FUNC7_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x014c)
/* GPIO_SIG7_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG7_IN_SEL  (BIT(7))
#define GPIO_SIG7_IN_SEL_M  (BIT(7))
#define GPIO_SIG7_IN_SEL_V  0x1
#define GPIO_SIG7_IN_SEL_S  7
/* GPIO_FUNC7_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC7_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC7_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC7_IN_INV_SEL_V  0x1
#define GPIO_FUNC7_IN_INV_SEL_S  6
/* GPIO_FUNC7_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC7_IN_SEL  0x0000003F
#define GPIO_FUNC7_IN_SEL_M  ((GPIO_FUNC7_IN_SEL_V)<<(GPIO_FUNC7_IN_SEL_S))
#define GPIO_FUNC7_IN_SEL_V  0x3F
#define GPIO_FUNC7_IN_SEL_S  0

#define GPIO_FUNC8_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x0150)
/* GPIO_SIG8_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG8_IN_SEL  (BIT(7))
#define GPIO_SIG8_IN_SEL_M  (BIT(7))
#define GPIO_SIG8_IN_SEL_V  0x1
#define GPIO_SIG8_IN_SEL_S  7
/* GPIO_FUNC8_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC8_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC8_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC8_IN_INV_SEL_V  0x1
#define GPIO_FUNC8_IN_INV_SEL_S  6
/* GPIO_FUNC8_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC8_IN_SEL  0x0000003F
#define GPIO_FUNC8_IN_SEL_M  ((GPIO_FUNC8_IN_SEL_V)<<(GPIO_FUNC8_IN_SEL_S))
#define GPIO_FUNC8_IN_SEL_V  0x3F
#define GPIO_FUNC8_IN_SEL_S  0

#define GPIO_FUNC9_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x0154)
/* GPIO_SIG9_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG9_IN_SEL  (BIT(7))
#define GPIO_SIG9_IN_SEL_M  (BIT(7))
#define GPIO_SIG9_IN_SEL_V  0x1
#define GPIO_SIG9_IN_SEL_S  7
/* GPIO_FUNC9_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC9_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC9_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC9_IN_INV_SEL_V  0x1
#define GPIO_FUNC9_IN_INV_SEL_S  6
/* GPIO_FUNC9_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC9_IN_SEL  0x0000003F
#define GPIO_FUNC9_IN_SEL_M  ((GPIO_FUNC9_IN_SEL_V)<<(GPIO_FUNC9_IN_SEL_S))
#define GPIO_FUNC9_IN_SEL_V  0x3F
#define GPIO_FUNC9_IN_SEL_S  0

#define GPIO_FUNC10_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x0158)
/* GPIO_SIG10_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG10_IN_SEL  (BIT(7))
#define GPIO_SIG10_IN_SEL_M  (BIT(7))
#define GPIO_SIG10_IN_SEL_V  0x1
#define GPIO_SIG10_IN_SEL_S  7
/* GPIO_FUNC10_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC10_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC10_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC10_IN_INV_SEL_V  0x1
#define GPIO_FUNC10_IN_INV_SEL_S  6
/* GPIO_FUNC10_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC10_IN_SEL  0x0000003F
#define GPIO_FUNC10_IN_SEL_M  ((GPIO_FUNC10_IN_SEL_V)<<(GPIO_FUNC10_IN_SEL_S))
#define GPIO_FUNC10_IN_SEL_V  0x3F
#define GPIO_FUNC10_IN_SEL_S  0

#define GPIO_FUNC11_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x015c)
/* GPIO_SIG11_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG11_IN_SEL  (BIT(7))
#define GPIO_SIG11_IN_SEL_M  (BIT(7))
#define GPIO_SIG11_IN_SEL_V  0x1
#define GPIO_SIG11_IN_SEL_S  7
/* GPIO_FUNC11_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC11_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC11_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC11_IN_INV_SEL_V  0x1
#define GPIO_FUNC11_IN_INV_SEL_S  6
/* GPIO_FUNC11_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC11_IN_SEL  0x0000003F
#define GPIO_FUNC11_IN_SEL_M  ((GPIO_FUNC11_IN_SEL_V)<<(GPIO_FUNC11_IN_SEL_S))
#define GPIO_FUNC11_IN_SEL_V  0x3F
#define GPIO_FUNC11_IN_SEL_S  0

#define GPIO_FUNC12_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x0160)
/* GPIO_SIG12_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG12_IN_SEL  (BIT(7))
#define GPIO_SIG12_IN_SEL_M  (BIT(7))
#define GPIO_SIG12_IN_SEL_V  0x1
#define GPIO_SIG12_IN_SEL_S  7
/* GPIO_FUNC12_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC12_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC12_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC12_IN_INV_SEL_V  0x1
#define GPIO_FUNC12_IN_INV_SEL_S  6
/* GPIO_FUNC12_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC12_IN_SEL  0x0000003F
#define GPIO_FUNC12_IN_SEL_M  ((GPIO_FUNC12_IN_SEL_V)<<(GPIO_FUNC12_IN_SEL_S))
#define GPIO_FUNC12_IN_SEL_V  0x3F
#define GPIO_FUNC12_IN_SEL_S  0

#define GPIO_FUNC13_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x0164)
/* GPIO_SIG13_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG13_IN_SEL  (BIT(7))
#define GPIO_SIG13_IN_SEL_M  (BIT(7))
#define GPIO_SIG13_IN_SEL_V  0x1
#define GPIO_SIG13_IN_SEL_S  7
/* GPIO_FUNC13_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC13_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC13_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC13_IN_INV_SEL_V  0x1
#define GPIO_FUNC13_IN_INV_SEL_S  6
/* GPIO_FUNC13_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC13_IN_SEL  0x0000003F
#define GPIO_FUNC13_IN_SEL_M  ((GPIO_FUNC13_IN_SEL_V)<<(GPIO_FUNC13_IN_SEL_S))
#define GPIO_FUNC13_IN_SEL_V  0x3F
#define GPIO_FUNC13_IN_SEL_S  0

#define GPIO_FUNC14_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x0168)
/* GPIO_SIG14_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG14_IN_SEL  (BIT(7))
#define GPIO_SIG14_IN_SEL_M  (BIT(7))
#define GPIO_SIG14_IN_SEL_V  0x1
#define GPIO_SIG14_IN_SEL_S  7
/* GPIO_FUNC14_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC14_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC14_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC14_IN_INV_SEL_V  0x1
#define GPIO_FUNC14_IN_INV_SEL_S  6
/* GPIO_FUNC14_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC14_IN_SEL  0x0000003F
#define GPIO_FUNC14_IN_SEL_M  ((GPIO_FUNC14_IN_SEL_V)<<(GPIO_FUNC14_IN_SEL_S))
#define GPIO_FUNC14_IN_SEL_V  0x3F
#define GPIO_FUNC14_IN_SEL_S  0

#define GPIO_FUNC15_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x016c)
/* GPIO_SIG15_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG15_IN_SEL  (BIT(7))
#define GPIO_SIG15_IN_SEL_M  (BIT(7))
#define GPIO_SIG15_IN_SEL_V  0x1
#define GPIO_SIG15_IN_SEL_S  7
/* GPIO_FUNC15_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC15_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC15_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC15_IN_INV_SEL_V  0x1
#define GPIO_FUNC15_IN_INV_SEL_S  6
/* GPIO_FUNC15_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC15_IN_SEL  0x0000003F
#define GPIO_FUNC15_IN_SEL_M  ((GPIO_FUNC15_IN_SEL_V)<<(GPIO_FUNC15_IN_SEL_S))
#define GPIO_FUNC15_IN_SEL_V  0x3F
#define GPIO_FUNC15_IN_SEL_S  0

#define GPIO_FUNC16_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x0170)
/* GPIO_SIG16_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG16_IN_SEL  (BIT(7))
#define GPIO_SIG16_IN_SEL_M  (BIT(7))
#define GPIO_SIG16_IN_SEL_V  0x1
#define GPIO_SIG16_IN_SEL_S  7
/* GPIO_FUNC16_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC16_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC16_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC16_IN_INV_SEL_V  0x1
#define GPIO_FUNC16_IN_INV_SEL_S  6
/* GPIO_FUNC16_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC16_IN_SEL  0x0000003F
#define GPIO_FUNC16_IN_SEL_M  ((GPIO_FUNC16_IN_SEL_V)<<(GPIO_FUNC16_IN_SEL_S))
#define GPIO_FUNC16_IN_SEL_V  0x3F
#define GPIO_FUNC16_IN_SEL_S  0

#define GPIO_FUNC17_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x0174)
/* GPIO_SIG17_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG17_IN_SEL  (BIT(7))
#define GPIO_SIG17_IN_SEL_M  (BIT(7))
#define GPIO_SIG17_IN_SEL_V  0x1
#define GPIO_SIG17_IN_SEL_S  7
/* GPIO_FUNC17_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC17_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC17_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC17_IN_INV_SEL_V  0x1
#define GPIO_FUNC17_IN_INV_SEL_S  6
/* GPIO_FUNC17_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC17_IN_SEL  0x0000003F
#define GPIO_FUNC17_IN_SEL_M  ((GPIO_FUNC17_IN_SEL_V)<<(GPIO_FUNC17_IN_SEL_S))
#define GPIO_FUNC17_IN_SEL_V  0x3F
#define GPIO_FUNC17_IN_SEL_S  0

#define GPIO_FUNC18_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x0178)
/* GPIO_SIG18_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG18_IN_SEL  (BIT(7))
#define GPIO_SIG18_IN_SEL_M  (BIT(7))
#define GPIO_SIG18_IN_SEL_V  0x1
#define GPIO_SIG18_IN_SEL_S  7
/* GPIO_FUNC18_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC18_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC18_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC18_IN_INV_SEL_V  0x1
#define GPIO_FUNC18_IN_INV_SEL_S  6
/* GPIO_FUNC18_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC18_IN_SEL  0x0000003F
#define GPIO_FUNC18_IN_SEL_M  ((GPIO_FUNC18_IN_SEL_V)<<(GPIO_FUNC18_IN_SEL_S))
#define GPIO_FUNC18_IN_SEL_V  0x3F
#define GPIO_FUNC18_IN_SEL_S  0

#define GPIO_FUNC19_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x017c)
/* GPIO_SIG19_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG19_IN_SEL  (BIT(7))
#define GPIO_SIG19_IN_SEL_M  (BIT(7))
#define GPIO_SIG19_IN_SEL_V  0x1
#define GPIO_SIG19_IN_SEL_S  7
/* GPIO_FUNC19_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC19_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC19_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC19_IN_INV_SEL_V  0x1
#define GPIO_FUNC19_IN_INV_SEL_S  6
/* GPIO_FUNC19_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC19_IN_SEL  0x0000003F
#define GPIO_FUNC19_IN_SEL_M  ((GPIO_FUNC19_IN_SEL_V)<<(GPIO_FUNC19_IN_SEL_S))
#define GPIO_FUNC19_IN_SEL_V  0x3F
#define GPIO_FUNC19_IN_SEL_S  0

#define GPIO_FUNC20_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x0180)
/* GPIO_SIG20_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG20_IN_SEL  (BIT(7))
#define GPIO_SIG20_IN_SEL_M  (BIT(7))
#define GPIO_SIG20_IN_SEL_V  0x1
#define GPIO_SIG20_IN_SEL_S  7
/* GPIO_FUNC20_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC20_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC20_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC20_IN_INV_SEL_V  0x1
#define GPIO_FUNC20_IN_INV_SEL_S  6
/* GPIO_FUNC20_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC20_IN_SEL  0x0000003F
#define GPIO_FUNC20_IN_SEL_M  ((GPIO_FUNC20_IN_SEL_V)<<(GPIO_FUNC20_IN_SEL_S))
#define GPIO_FUNC20_IN_SEL_V  0x3F
#define GPIO_FUNC20_IN_SEL_S  0

#define GPIO_FUNC21_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x0184)
/* GPIO_SIG21_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG21_IN_SEL  (BIT(7))
#define GPIO_SIG21_IN_SEL_M  (BIT(7))
#define GPIO_SIG21_IN_SEL_V  0x1
#define GPIO_SIG21_IN_SEL_S  7
/* GPIO_FUNC21_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC21_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC21_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC21_IN_INV_SEL_V  0x1
#define GPIO_FUNC21_IN_INV_SEL_S  6
/* GPIO_FUNC21_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC21_IN_SEL  0x0000003F
#define GPIO_FUNC21_IN_SEL_M  ((GPIO_FUNC21_IN_SEL_V)<<(GPIO_FUNC21_IN_SEL_S))
#define GPIO_FUNC21_IN_SEL_V  0x3F
#define GPIO_FUNC21_IN_SEL_S  0

#define GPIO_FUNC22_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x0188)
/* GPIO_SIG22_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG22_IN_SEL  (BIT(7))
#define GPIO_SIG22_IN_SEL_M  (BIT(7))
#define GPIO_SIG22_IN_SEL_V  0x1
#define GPIO_SIG22_IN_SEL_S  7
/* GPIO_FUNC22_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC22_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC22_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC22_IN_INV_SEL_V  0x1
#define GPIO_FUNC22_IN_INV_SEL_S  6
/* GPIO_FUNC22_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC22_IN_SEL  0x0000003F
#define GPIO_FUNC22_IN_SEL_M  ((GPIO_FUNC22_IN_SEL_V)<<(GPIO_FUNC22_IN_SEL_S))
#define GPIO_FUNC22_IN_SEL_V  0x3F
#define GPIO_FUNC22_IN_SEL_S  0

#define GPIO_FUNC23_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x018c)
/* GPIO_SIG23_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG23_IN_SEL  (BIT(7))
#define GPIO_SIG23_IN_SEL_M  (BIT(7))
#define GPIO_SIG23_IN_SEL_V  0x1
#define GPIO_SIG23_IN_SEL_S  7
/* GPIO_FUNC23_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC23_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC23_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC23_IN_INV_SEL_V  0x1
#define GPIO_FUNC23_IN_INV_SEL_S  6
/* GPIO_FUNC23_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC23_IN_SEL  0x0000003F
#define GPIO_FUNC23_IN_SEL_M  ((GPIO_FUNC23_IN_SEL_V)<<(GPIO_FUNC23_IN_SEL_S))
#define GPIO_FUNC23_IN_SEL_V  0x3F
#define GPIO_FUNC23_IN_SEL_S  0

#define GPIO_FUNC24_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x0190)
/* GPIO_SIG24_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG24_IN_SEL  (BIT(7))
#define GPIO_SIG24_IN_SEL_M  (BIT(7))
#define GPIO_SIG24_IN_SEL_V  0x1
#define GPIO_SIG24_IN_SEL_S  7
/* GPIO_FUNC24_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC24_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC24_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC24_IN_INV_SEL_V  0x1
#define GPIO_FUNC24_IN_INV_SEL_S  6
/* GPIO_FUNC24_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC24_IN_SEL  0x0000003F
#define GPIO_FUNC24_IN_SEL_M  ((GPIO_FUNC24_IN_SEL_V)<<(GPIO_FUNC24_IN_SEL_S))
#define GPIO_FUNC24_IN_SEL_V  0x3F
#define GPIO_FUNC24_IN_SEL_S  0

#define GPIO_FUNC25_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x0194)
/* GPIO_SIG25_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG25_IN_SEL  (BIT(7))
#define GPIO_SIG25_IN_SEL_M  (BIT(7))
#define GPIO_SIG25_IN_SEL_V  0x1
#define GPIO_SIG25_IN_SEL_S  7
/* GPIO_FUNC25_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC25_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC25_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC25_IN_INV_SEL_V  0x1
#define GPIO_FUNC25_IN_INV_SEL_S  6
/* GPIO_FUNC25_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC25_IN_SEL  0x0000003F
#define GPIO_FUNC25_IN_SEL_M  ((GPIO_FUNC25_IN_SEL_V)<<(GPIO_FUNC25_IN_SEL_S))
#define GPIO_FUNC25_IN_SEL_V  0x3F
#define GPIO_FUNC25_IN_SEL_S  0

#define GPIO_FUNC26_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x0198)
/* GPIO_SIG26_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG26_IN_SEL  (BIT(7))
#define GPIO_SIG26_IN_SEL_M  (BIT(7))
#define GPIO_SIG26_IN_SEL_V  0x1
#define GPIO_SIG26_IN_SEL_S  7
/* GPIO_FUNC26_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC26_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC26_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC26_IN_INV_SEL_V  0x1
#define GPIO_FUNC26_IN_INV_SEL_S  6
/* GPIO_FUNC26_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC26_IN_SEL  0x0000003F
#define GPIO_FUNC26_IN_SEL_M  ((GPIO_FUNC26_IN_SEL_V)<<(GPIO_FUNC26_IN_SEL_S))
#define GPIO_FUNC26_IN_SEL_V  0x3F
#define GPIO_FUNC26_IN_SEL_S  0

#define GPIO_FUNC27_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x019c)
/* GPIO_SIG27_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG27_IN_SEL  (BIT(7))
#define GPIO_SIG27_IN_SEL_M  (BIT(7))
#define GPIO_SIG27_IN_SEL_V  0x1
#define GPIO_SIG27_IN_SEL_S  7
/* GPIO_FUNC27_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC27_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC27_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC27_IN_INV_SEL_V  0x1
#define GPIO_FUNC27_IN_INV_SEL_S  6
/* GPIO_FUNC27_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC27_IN_SEL  0x0000003F
#define GPIO_FUNC27_IN_SEL_M  ((GPIO_FUNC27_IN_SEL_V)<<(GPIO_FUNC27_IN_SEL_S))
#define GPIO_FUNC27_IN_SEL_V  0x3F
#define GPIO_FUNC27_IN_SEL_S  0

#define GPIO_FUNC28_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x01a0)
/* GPIO_SIG28_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG28_IN_SEL  (BIT(7))
#define GPIO_SIG28_IN_SEL_M  (BIT(7))
#define GPIO_SIG28_IN_SEL_V  0x1
#define GPIO_SIG28_IN_SEL_S  7
/* GPIO_FUNC28_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC28_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC28_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC28_IN_INV_SEL_V  0x1
#define GPIO_FUNC28_IN_INV_SEL_S  6
/* GPIO_FUNC28_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC28_IN_SEL  0x0000003F
#define GPIO_FUNC28_IN_SEL_M  ((GPIO_FUNC28_IN_SEL_V)<<(GPIO_FUNC28_IN_SEL_S))
#define GPIO_FUNC28_IN_SEL_V  0x3F
#define GPIO_FUNC28_IN_SEL_S  0

#define GPIO_FUNC29_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x01a4)
/* GPIO_SIG29_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG29_IN_SEL  (BIT(7))
#define GPIO_SIG29_IN_SEL_M  (BIT(7))
#define GPIO_SIG29_IN_SEL_V  0x1
#define GPIO_SIG29_IN_SEL_S  7
/* GPIO_FUNC29_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC29_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC29_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC29_IN_INV_SEL_V  0x1
#define GPIO_FUNC29_IN_INV_SEL_S  6
/* GPIO_FUNC29_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC29_IN_SEL  0x0000003F
#define GPIO_FUNC29_IN_SEL_M  ((GPIO_FUNC29_IN_SEL_V)<<(GPIO_FUNC29_IN_SEL_S))
#define GPIO_FUNC29_IN_SEL_V  0x3F
#define GPIO_FUNC29_IN_SEL_S  0

#define GPIO_FUNC30_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x01a8)
/* GPIO_SIG30_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG30_IN_SEL  (BIT(7))
#define GPIO_SIG30_IN_SEL_M  (BIT(7))
#define GPIO_SIG30_IN_SEL_V  0x1
#define GPIO_SIG30_IN_SEL_S  7
/* GPIO_FUNC30_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC30_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC30_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC30_IN_INV_SEL_V  0x1
#define GPIO_FUNC30_IN_INV_SEL_S  6
/* GPIO_FUNC30_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC30_IN_SEL  0x0000003F
#define GPIO_FUNC30_IN_SEL_M  ((GPIO_FUNC30_IN_SEL_V)<<(GPIO_FUNC30_IN_SEL_S))
#define GPIO_FUNC30_IN_SEL_V  0x3F
#define GPIO_FUNC30_IN_SEL_S  0

#define GPIO_FUNC31_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x01ac)
/* GPIO_SIG31_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG31_IN_SEL  (BIT(7))
#define GPIO_SIG31_IN_SEL_M  (BIT(7))
#define GPIO_SIG31_IN_SEL_V  0x1
#define GPIO_SIG31_IN_SEL_S  7
/* GPIO_FUNC31_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC31_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC31_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC31_IN_INV_SEL_V  0x1
#define GPIO_FUNC31_IN_INV_SEL_S  6
/* GPIO_FUNC31_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC31_IN_SEL  0x0000003F
#define GPIO_FUNC31_IN_SEL_M  ((GPIO_FUNC31_IN_SEL_V)<<(GPIO_FUNC31_IN_SEL_S))
#define GPIO_FUNC31_IN_SEL_V  0x3F
#define GPIO_FUNC31_IN_SEL_S  0

#define GPIO_FUNC32_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x01b0)
/* GPIO_SIG32_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG32_IN_SEL  (BIT(7))
#define GPIO_SIG32_IN_SEL_M  (BIT(7))
#define GPIO_SIG32_IN_SEL_V  0x1
#define GPIO_SIG32_IN_SEL_S  7
/* GPIO_FUNC32_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC32_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC32_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC32_IN_INV_SEL_V  0x1
#define GPIO_FUNC32_IN_INV_SEL_S  6
/* GPIO_FUNC32_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC32_IN_SEL  0x0000003F
#define GPIO_FUNC32_IN_SEL_M  ((GPIO_FUNC32_IN_SEL_V)<<(GPIO_FUNC32_IN_SEL_S))
#define GPIO_FUNC32_IN_SEL_V  0x3F
#define GPIO_FUNC32_IN_SEL_S  0

#define GPIO_FUNC33_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x01b4)
/* GPIO_SIG33_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG33_IN_SEL  (BIT(7))
#define GPIO_SIG33_IN_SEL_M  (BIT(7))
#define GPIO_SIG33_IN_SEL_V  0x1
#define GPIO_SIG33_IN_SEL_S  7
/* GPIO_FUNC33_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC33_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC33_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC33_IN_INV_SEL_V  0x1
#define GPIO_FUNC33_IN_INV_SEL_S  6
/* GPIO_FUNC33_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC33_IN_SEL  0x0000003F
#define GPIO_FUNC33_IN_SEL_M  ((GPIO_FUNC33_IN_SEL_V)<<(GPIO_FUNC33_IN_SEL_S))
#define GPIO_FUNC33_IN_SEL_V  0x3F
#define GPIO_FUNC33_IN_SEL_S  0

#define GPIO_FUNC34_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x01b8)
/* GPIO_SIG34_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG34_IN_SEL  (BIT(7))
#define GPIO_SIG34_IN_SEL_M  (BIT(7))
#define GPIO_SIG34_IN_SEL_V  0x1
#define GPIO_SIG34_IN_SEL_S  7
/* GPIO_FUNC34_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC34_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC34_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC34_IN_INV_SEL_V  0x1
#define GPIO_FUNC34_IN_INV_SEL_S  6
/* GPIO_FUNC34_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC34_IN_SEL  0x0000003F
#define GPIO_FUNC34_IN_SEL_M  ((GPIO_FUNC34_IN_SEL_V)<<(GPIO_FUNC34_IN_SEL_S))
#define GPIO_FUNC34_IN_SEL_V  0x3F
#define GPIO_FUNC34_IN_SEL_S  0

#define GPIO_FUNC35_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x01bc)
/* GPIO_SIG35_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG35_IN_SEL  (BIT(7))
#define GPIO_SIG35_IN_SEL_M  (BIT(7))
#define GPIO_SIG35_IN_SEL_V  0x1
#define GPIO_SIG35_IN_SEL_S  7
/* GPIO_FUNC35_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC35_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC35_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC35_IN_INV_SEL_V  0x1
#define GPIO_FUNC35_IN_INV_SEL_S  6
/* GPIO_FUNC35_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC35_IN_SEL  0x0000003F
#define GPIO_FUNC35_IN_SEL_M  ((GPIO_FUNC35_IN_SEL_V)<<(GPIO_FUNC35_IN_SEL_S))
#define GPIO_FUNC35_IN_SEL_V  0x3F
#define GPIO_FUNC35_IN_SEL_S  0

#define GPIO_FUNC36_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x01c0)
/* GPIO_SIG36_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG36_IN_SEL  (BIT(7))
#define GPIO_SIG36_IN_SEL_M  (BIT(7))
#define GPIO_SIG36_IN_SEL_V  0x1
#define GPIO_SIG36_IN_SEL_S  7
/* GPIO_FUNC36_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC36_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC36_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC36_IN_INV_SEL_V  0x1
#define GPIO_FUNC36_IN_INV_SEL_S  6
/* GPIO_FUNC36_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC36_IN_SEL  0x0000003F
#define GPIO_FUNC36_IN_SEL_M  ((GPIO_FUNC36_IN_SEL_V)<<(GPIO_FUNC36_IN_SEL_S))
#define GPIO_FUNC36_IN_SEL_V  0x3F
#define GPIO_FUNC36_IN_SEL_S  0

#define GPIO_FUNC37_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x01c4)
/* GPIO_SIG37_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG37_IN_SEL  (BIT(7))
#define GPIO_SIG37_IN_SEL_M  (BIT(7))
#define GPIO_SIG37_IN_SEL_V  0x1
#define GPIO_SIG37_IN_SEL_S  7
/* GPIO_FUNC37_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC37_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC37_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC37_IN_INV_SEL_V  0x1
#define GPIO_FUNC37_IN_INV_SEL_S  6
/* GPIO_FUNC37_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC37_IN_SEL  0x0000003F
#define GPIO_FUNC37_IN_SEL_M  ((GPIO_FUNC37_IN_SEL_V)<<(GPIO_FUNC37_IN_SEL_S))
#define GPIO_FUNC37_IN_SEL_V  0x3F
#define GPIO_FUNC37_IN_SEL_S  0

#define GPIO_FUNC38_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x01c8)
/* GPIO_SIG38_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG38_IN_SEL  (BIT(7))
#define GPIO_SIG38_IN_SEL_M  (BIT(7))
#define GPIO_SIG38_IN_SEL_V  0x1
#define GPIO_SIG38_IN_SEL_S  7
/* GPIO_FUNC38_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC38_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC38_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC38_IN_INV_SEL_V  0x1
#define GPIO_FUNC38_IN_INV_SEL_S  6
/* GPIO_FUNC38_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC38_IN_SEL  0x0000003F
#define GPIO_FUNC38_IN_SEL_M  ((GPIO_FUNC38_IN_SEL_V)<<(GPIO_FUNC38_IN_SEL_S))
#define GPIO_FUNC38_IN_SEL_V  0x3F
#define GPIO_FUNC38_IN_SEL_S  0

#define GPIO_FUNC39_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x01cc)
/* GPIO_SIG39_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG39_IN_SEL  (BIT(7))
#define GPIO_SIG39_IN_SEL_M  (BIT(7))
#define GPIO_SIG39_IN_SEL_V  0x1
#define GPIO_SIG39_IN_SEL_S  7
/* GPIO_FUNC39_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC39_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC39_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC39_IN_INV_SEL_V  0x1
#define GPIO_FUNC39_IN_INV_SEL_S  6
/* GPIO_FUNC39_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC39_IN_SEL  0x0000003F
#define GPIO_FUNC39_IN_SEL_M  ((GPIO_FUNC39_IN_SEL_V)<<(GPIO_FUNC39_IN_SEL_S))
#define GPIO_FUNC39_IN_SEL_V  0x3F
#define GPIO_FUNC39_IN_SEL_S  0

#define GPIO_FUNC40_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x01d0)
/* GPIO_SIG40_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG40_IN_SEL  (BIT(7))
#define GPIO_SIG40_IN_SEL_M  (BIT(7))
#define GPIO_SIG40_IN_SEL_V  0x1
#define GPIO_SIG40_IN_SEL_S  7
/* GPIO_FUNC40_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC40_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC40_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC40_IN_INV_SEL_V  0x1
#define GPIO_FUNC40_IN_INV_SEL_S  6
/* GPIO_FUNC40_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC40_IN_SEL  0x0000003F
#define GPIO_FUNC40_IN_SEL_M  ((GPIO_FUNC40_IN_SEL_V)<<(GPIO_FUNC40_IN_SEL_S))
#define GPIO_FUNC40_IN_SEL_V  0x3F
#define GPIO_FUNC40_IN_SEL_S  0

#define GPIO_FUNC41_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x01d4)
/* GPIO_SIG41_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG41_IN_SEL  (BIT(7))
#define GPIO_SIG41_IN_SEL_M  (BIT(7))
#define GPIO_SIG41_IN_SEL_V  0x1
#define GPIO_SIG41_IN_SEL_S  7
/* GPIO_FUNC41_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC41_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC41_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC41_IN_INV_SEL_V  0x1
#define GPIO_FUNC41_IN_INV_SEL_S  6
/* GPIO_FUNC41_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC41_IN_SEL  0x0000003F
#define GPIO_FUNC41_IN_SEL_M  ((GPIO_FUNC41_IN_SEL_V)<<(GPIO_FUNC41_IN_SEL_S))
#define GPIO_FUNC41_IN_SEL_V  0x3F
#define GPIO_FUNC41_IN_SEL_S  0

#define GPIO_FUNC42_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x01d8)
/* GPIO_SIG42_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG42_IN_SEL  (BIT(7))
#define GPIO_SIG42_IN_SEL_M  (BIT(7))
#define GPIO_SIG42_IN_SEL_V  0x1
#define GPIO_SIG42_IN_SEL_S  7
/* GPIO_FUNC42_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC42_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC42_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC42_IN_INV_SEL_V  0x1
#define GPIO_FUNC42_IN_INV_SEL_S  6
/* GPIO_FUNC42_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC42_IN_SEL  0x0000003F
#define GPIO_FUNC42_IN_SEL_M  ((GPIO_FUNC42_IN_SEL_V)<<(GPIO_FUNC42_IN_SEL_S))
#define GPIO_FUNC42_IN_SEL_V  0x3F
#define GPIO_FUNC42_IN_SEL_S  0

#define GPIO_FUNC43_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x01dc)
/* GPIO_SIG43_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG43_IN_SEL  (BIT(7))
#define GPIO_SIG43_IN_SEL_M  (BIT(7))
#define GPIO_SIG43_IN_SEL_V  0x1
#define GPIO_SIG43_IN_SEL_S  7
/* GPIO_FUNC43_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC43_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC43_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC43_IN_INV_SEL_V  0x1
#define GPIO_FUNC43_IN_INV_SEL_S  6
/* GPIO_FUNC43_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC43_IN_SEL  0x0000003F
#define GPIO_FUNC43_IN_SEL_M  ((GPIO_FUNC43_IN_SEL_V)<<(GPIO_FUNC43_IN_SEL_S))
#define GPIO_FUNC43_IN_SEL_V  0x3F
#define GPIO_FUNC43_IN_SEL_S  0

#define GPIO_FUNC44_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x01e0)
/* GPIO_SIG44_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG44_IN_SEL  (BIT(7))
#define GPIO_SIG44_IN_SEL_M  (BIT(7))
#define GPIO_SIG44_IN_SEL_V  0x1
#define GPIO_SIG44_IN_SEL_S  7
/* GPIO_FUNC44_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC44_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC44_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC44_IN_INV_SEL_V  0x1
#define GPIO_FUNC44_IN_INV_SEL_S  6
/* GPIO_FUNC44_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC44_IN_SEL  0x0000003F
#define GPIO_FUNC44_IN_SEL_M  ((GPIO_FUNC44_IN_SEL_V)<<(GPIO_FUNC44_IN_SEL_S))
#define GPIO_FUNC44_IN_SEL_V  0x3F
#define GPIO_FUNC44_IN_SEL_S  0

#define GPIO_FUNC45_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x01e4)
/* GPIO_SIG45_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG45_IN_SEL  (BIT(7))
#define GPIO_SIG45_IN_SEL_M  (BIT(7))
#define GPIO_SIG45_IN_SEL_V  0x1
#define GPIO_SIG45_IN_SEL_S  7
/* GPIO_FUNC45_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC45_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC45_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC45_IN_INV_SEL_V  0x1
#define GPIO_FUNC45_IN_INV_SEL_S  6
/* GPIO_FUNC45_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC45_IN_SEL  0x0000003F
#define GPIO_FUNC45_IN_SEL_M  ((GPIO_FUNC45_IN_SEL_V)<<(GPIO_FUNC45_IN_SEL_S))
#define GPIO_FUNC45_IN_SEL_V  0x3F
#define GPIO_FUNC45_IN_SEL_S  0

#define GPIO_FUNC46_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x01e8)
/* GPIO_SIG46_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG46_IN_SEL  (BIT(7))
#define GPIO_SIG46_IN_SEL_M  (BIT(7))
#define GPIO_SIG46_IN_SEL_V  0x1
#define GPIO_SIG46_IN_SEL_S  7
/* GPIO_FUNC46_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC46_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC46_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC46_IN_INV_SEL_V  0x1
#define GPIO_FUNC46_IN_INV_SEL_S  6
/* GPIO_FUNC46_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC46_IN_SEL  0x0000003F
#define GPIO_FUNC46_IN_SEL_M  ((GPIO_FUNC46_IN_SEL_V)<<(GPIO_FUNC46_IN_SEL_S))
#define GPIO_FUNC46_IN_SEL_V  0x3F
#define GPIO_FUNC46_IN_SEL_S  0

#define GPIO_FUNC47_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x01ec)
/* GPIO_SIG47_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG47_IN_SEL  (BIT(7))
#define GPIO_SIG47_IN_SEL_M  (BIT(7))
#define GPIO_SIG47_IN_SEL_V  0x1
#define GPIO_SIG47_IN_SEL_S  7
/* GPIO_FUNC47_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC47_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC47_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC47_IN_INV_SEL_V  0x1
#define GPIO_FUNC47_IN_INV_SEL_S  6
/* GPIO_FUNC47_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC47_IN_SEL  0x0000003F
#define GPIO_FUNC47_IN_SEL_M  ((GPIO_FUNC47_IN_SEL_V)<<(GPIO_FUNC47_IN_SEL_S))
#define GPIO_FUNC47_IN_SEL_V  0x3F
#define GPIO_FUNC47_IN_SEL_S  0

#define GPIO_FUNC48_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x01f0)
/* GPIO_SIG48_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG48_IN_SEL  (BIT(7))
#define GPIO_SIG48_IN_SEL_M  (BIT(7))
#define GPIO_SIG48_IN_SEL_V  0x1
#define GPIO_SIG48_IN_SEL_S  7
/* GPIO_FUNC48_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC48_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC48_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC48_IN_INV_SEL_V  0x1
#define GPIO_FUNC48_IN_INV_SEL_S  6
/* GPIO_FUNC48_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC48_IN_SEL  0x0000003F
#define GPIO_FUNC48_IN_SEL_M  ((GPIO_FUNC48_IN_SEL_V)<<(GPIO_FUNC48_IN_SEL_S))
#define GPIO_FUNC48_IN_SEL_V  0x3F
#define GPIO_FUNC48_IN_SEL_S  0

#define GPIO_FUNC49_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x01f4)
/* GPIO_SIG49_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG49_IN_SEL  (BIT(7))
#define GPIO_SIG49_IN_SEL_M  (BIT(7))
#define GPIO_SIG49_IN_SEL_V  0x1
#define GPIO_SIG49_IN_SEL_S  7
/* GPIO_FUNC49_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC49_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC49_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC49_IN_INV_SEL_V  0x1
#define GPIO_FUNC49_IN_INV_SEL_S  6
/* GPIO_FUNC49_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC49_IN_SEL  0x0000003F
#define GPIO_FUNC49_IN_SEL_M  ((GPIO_FUNC49_IN_SEL_V)<<(GPIO_FUNC49_IN_SEL_S))
#define GPIO_FUNC49_IN_SEL_V  0x3F
#define GPIO_FUNC49_IN_SEL_S  0

#define GPIO_FUNC50_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x01f8)
/* GPIO_SIG50_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG50_IN_SEL  (BIT(7))
#define GPIO_SIG50_IN_SEL_M  (BIT(7))
#define GPIO_SIG50_IN_SEL_V  0x1
#define GPIO_SIG50_IN_SEL_S  7
/* GPIO_FUNC50_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC50_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC50_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC50_IN_INV_SEL_V  0x1
#define GPIO_FUNC50_IN_INV_SEL_S  6
/* GPIO_FUNC50_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC50_IN_SEL  0x0000003F
#define GPIO_FUNC50_IN_SEL_M  ((GPIO_FUNC50_IN_SEL_V)<<(GPIO_FUNC50_IN_SEL_S))
#define GPIO_FUNC50_IN_SEL_V  0x3F
#define GPIO_FUNC50_IN_SEL_S  0

#define GPIO_FUNC51_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x01fc)
/* GPIO_SIG51_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG51_IN_SEL  (BIT(7))
#define GPIO_SIG51_IN_SEL_M  (BIT(7))
#define GPIO_SIG51_IN_SEL_V  0x1
#define GPIO_SIG51_IN_SEL_S  7
/* GPIO_FUNC51_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC51_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC51_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC51_IN_INV_SEL_V  0x1
#define GPIO_FUNC51_IN_INV_SEL_S  6
/* GPIO_FUNC51_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC51_IN_SEL  0x0000003F
#define GPIO_FUNC51_IN_SEL_M  ((GPIO_FUNC51_IN_SEL_V)<<(GPIO_FUNC51_IN_SEL_S))
#define GPIO_FUNC51_IN_SEL_V  0x3F
#define GPIO_FUNC51_IN_SEL_S  0

#define GPIO_FUNC52_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x0200)
/* GPIO_SIG52_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG52_IN_SEL  (BIT(7))
#define GPIO_SIG52_IN_SEL_M  (BIT(7))
#define GPIO_SIG52_IN_SEL_V  0x1
#define GPIO_SIG52_IN_SEL_S  7
/* GPIO_FUNC52_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC52_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC52_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC52_IN_INV_SEL_V  0x1
#define GPIO_FUNC52_IN_INV_SEL_S  6
/* GPIO_FUNC52_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC52_IN_SEL  0x0000003F
#define GPIO_FUNC52_IN_SEL_M  ((GPIO_FUNC52_IN_SEL_V)<<(GPIO_FUNC52_IN_SEL_S))
#define GPIO_FUNC52_IN_SEL_V  0x3F
#define GPIO_FUNC52_IN_SEL_S  0

#define GPIO_FUNC53_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x0204)
/* GPIO_SIG53_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG53_IN_SEL  (BIT(7))
#define GPIO_SIG53_IN_SEL_M  (BIT(7))
#define GPIO_SIG53_IN_SEL_V  0x1
#define GPIO_SIG53_IN_SEL_S  7
/* GPIO_FUNC53_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC53_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC53_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC53_IN_INV_SEL_V  0x1
#define GPIO_FUNC53_IN_INV_SEL_S  6
/* GPIO_FUNC53_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC53_IN_SEL  0x0000003F
#define GPIO_FUNC53_IN_SEL_M  ((GPIO_FUNC53_IN_SEL_V)<<(GPIO_FUNC53_IN_SEL_S))
#define GPIO_FUNC53_IN_SEL_V  0x3F
#define GPIO_FUNC53_IN_SEL_S  0

#define GPIO_FUNC54_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x0208)
/* GPIO_SIG54_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG54_IN_SEL  (BIT(7))
#define GPIO_SIG54_IN_SEL_M  (BIT(7))
#define GPIO_SIG54_IN_SEL_V  0x1
#define GPIO_SIG54_IN_SEL_S  7
/* GPIO_FUNC54_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC54_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC54_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC54_IN_INV_SEL_V  0x1
#define GPIO_FUNC54_IN_INV_SEL_S  6
/* GPIO_FUNC54_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC54_IN_SEL  0x0000003F
#define GPIO_FUNC54_IN_SEL_M  ((GPIO_FUNC54_IN_SEL_V)<<(GPIO_FUNC54_IN_SEL_S))
#define GPIO_FUNC54_IN_SEL_V  0x3F
#define GPIO_FUNC54_IN_SEL_S  0

#define GPIO_FUNC55_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x020c)
/* GPIO_SIG55_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG55_IN_SEL  (BIT(7))
#define GPIO_SIG55_IN_SEL_M  (BIT(7))
#define GPIO_SIG55_IN_SEL_V  0x1
#define GPIO_SIG55_IN_SEL_S  7
/* GPIO_FUNC55_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC55_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC55_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC55_IN_INV_SEL_V  0x1
#define GPIO_FUNC55_IN_INV_SEL_S  6
/* GPIO_FUNC55_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC55_IN_SEL  0x0000003F
#define GPIO_FUNC55_IN_SEL_M  ((GPIO_FUNC55_IN_SEL_V)<<(GPIO_FUNC55_IN_SEL_S))
#define GPIO_FUNC55_IN_SEL_V  0x3F
#define GPIO_FUNC55_IN_SEL_S  0

#define GPIO_FUNC56_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x0210)
/* GPIO_SIG56_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG56_IN_SEL  (BIT(7))
#define GPIO_SIG56_IN_SEL_M  (BIT(7))
#define GPIO_SIG56_IN_SEL_V  0x1
#define GPIO_SIG56_IN_SEL_S  7
/* GPIO_FUNC56_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC56_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC56_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC56_IN_INV_SEL_V  0x1
#define GPIO_FUNC56_IN_INV_SEL_S  6
/* GPIO_FUNC56_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC56_IN_SEL  0x0000003F
#define GPIO_FUNC56_IN_SEL_M  ((GPIO_FUNC56_IN_SEL_V)<<(GPIO_FUNC56_IN_SEL_S))
#define GPIO_FUNC56_IN_SEL_V  0x3F
#define GPIO_FUNC56_IN_SEL_S  0

#define GPIO_FUNC57_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x0214)
/* GPIO_SIG57_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG57_IN_SEL  (BIT(7))
#define GPIO_SIG57_IN_SEL_M  (BIT(7))
#define GPIO_SIG57_IN_SEL_V  0x1
#define GPIO_SIG57_IN_SEL_S  7
/* GPIO_FUNC57_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC57_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC57_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC57_IN_INV_SEL_V  0x1
#define GPIO_FUNC57_IN_INV_SEL_S  6
/* GPIO_FUNC57_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC57_IN_SEL  0x0000003F
#define GPIO_FUNC57_IN_SEL_M  ((GPIO_FUNC57_IN_SEL_V)<<(GPIO_FUNC57_IN_SEL_S))
#define GPIO_FUNC57_IN_SEL_V  0x3F
#define GPIO_FUNC57_IN_SEL_S  0

#define GPIO_FUNC58_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x0218)
/* GPIO_SIG58_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG58_IN_SEL  (BIT(7))
#define GPIO_SIG58_IN_SEL_M  (BIT(7))
#define GPIO_SIG58_IN_SEL_V  0x1
#define GPIO_SIG58_IN_SEL_S  7
/* GPIO_FUNC58_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC58_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC58_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC58_IN_INV_SEL_V  0x1
#define GPIO_FUNC58_IN_INV_SEL_S  6
/* GPIO_FUNC58_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC58_IN_SEL  0x0000003F
#define GPIO_FUNC58_IN_SEL_M  ((GPIO_FUNC58_IN_SEL_V)<<(GPIO_FUNC58_IN_SEL_S))
#define GPIO_FUNC58_IN_SEL_V  0x3F
#define GPIO_FUNC58_IN_SEL_S  0

#define GPIO_FUNC59_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x021c)
/* GPIO_SIG59_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG59_IN_SEL  (BIT(7))
#define GPIO_SIG59_IN_SEL_M  (BIT(7))
#define GPIO_SIG59_IN_SEL_V  0x1
#define GPIO_SIG59_IN_SEL_S  7
/* GPIO_FUNC59_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC59_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC59_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC59_IN_INV_SEL_V  0x1
#define GPIO_FUNC59_IN_INV_SEL_S  6
/* GPIO_FUNC59_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC59_IN_SEL  0x0000003F
#define GPIO_FUNC59_IN_SEL_M  ((GPIO_FUNC59_IN_SEL_V)<<(GPIO_FUNC59_IN_SEL_S))
#define GPIO_FUNC59_IN_SEL_V  0x3F
#define GPIO_FUNC59_IN_SEL_S  0

#define GPIO_FUNC60_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x0220)
/* GPIO_SIG60_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG60_IN_SEL  (BIT(7))
#define GPIO_SIG60_IN_SEL_M  (BIT(7))
#define GPIO_SIG60_IN_SEL_V  0x1
#define GPIO_SIG60_IN_SEL_S  7
/* GPIO_FUNC60_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC60_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC60_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC60_IN_INV_SEL_V  0x1
#define GPIO_FUNC60_IN_INV_SEL_S  6
/* GPIO_FUNC60_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC60_IN_SEL  0x0000003F
#define GPIO_FUNC60_IN_SEL_M  ((GPIO_FUNC60_IN_SEL_V)<<(GPIO_FUNC60_IN_SEL_S))
#define GPIO_FUNC60_IN_SEL_V  0x3F
#define GPIO_FUNC60_IN_SEL_S  0

#define GPIO_FUNC61_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x0224)
/* GPIO_SIG61_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG61_IN_SEL  (BIT(7))
#define GPIO_SIG61_IN_SEL_M  (BIT(7))
#define GPIO_SIG61_IN_SEL_V  0x1
#define GPIO_SIG61_IN_SEL_S  7
/* GPIO_FUNC61_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC61_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC61_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC61_IN_INV_SEL_V  0x1
#define GPIO_FUNC61_IN_INV_SEL_S  6
/* GPIO_FUNC61_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC61_IN_SEL  0x0000003F
#define GPIO_FUNC61_IN_SEL_M  ((GPIO_FUNC61_IN_SEL_V)<<(GPIO_FUNC61_IN_SEL_S))
#define GPIO_FUNC61_IN_SEL_V  0x3F
#define GPIO_FUNC61_IN_SEL_S  0

#define GPIO_FUNC62_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x0228)
/* GPIO_SIG62_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG62_IN_SEL  (BIT(7))
#define GPIO_SIG62_IN_SEL_M  (BIT(7))
#define GPIO_SIG62_IN_SEL_V  0x1
#define GPIO_SIG62_IN_SEL_S  7
/* GPIO_FUNC62_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC62_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC62_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC62_IN_INV_SEL_V  0x1
#define GPIO_FUNC62_IN_INV_SEL_S  6
/* GPIO_FUNC62_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC62_IN_SEL  0x0000003F
#define GPIO_FUNC62_IN_SEL_M  ((GPIO_FUNC62_IN_SEL_V)<<(GPIO_FUNC62_IN_SEL_S))
#define GPIO_FUNC62_IN_SEL_V  0x3F
#define GPIO_FUNC62_IN_SEL_S  0

#define GPIO_FUNC63_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x022c)
/* GPIO_SIG63_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG63_IN_SEL  (BIT(7))
#define GPIO_SIG63_IN_SEL_M  (BIT(7))
#define GPIO_SIG63_IN_SEL_V  0x1
#define GPIO_SIG63_IN_SEL_S  7
/* GPIO_FUNC63_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC63_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC63_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC63_IN_INV_SEL_V  0x1
#define GPIO_FUNC63_IN_INV_SEL_S  6
/* GPIO_FUNC63_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC63_IN_SEL  0x0000003F
#define GPIO_FUNC63_IN_SEL_M  ((GPIO_FUNC63_IN_SEL_V)<<(GPIO_FUNC63_IN_SEL_S))
#define GPIO_FUNC63_IN_SEL_V  0x3F
#define GPIO_FUNC63_IN_SEL_S  0

#define GPIO_FUNC64_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x0230)
/* GPIO_SIG64_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG64_IN_SEL  (BIT(7))
#define GPIO_SIG64_IN_SEL_M  (BIT(7))
#define GPIO_SIG64_IN_SEL_V  0x1
#define GPIO_SIG64_IN_SEL_S  7
/* GPIO_FUNC64_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC64_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC64_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC64_IN_INV_SEL_V  0x1
#define GPIO_FUNC64_IN_INV_SEL_S  6
/* GPIO_FUNC64_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC64_IN_SEL  0x0000003F
#define GPIO_FUNC64_IN_SEL_M  ((GPIO_FUNC64_IN_SEL_V)<<(GPIO_FUNC64_IN_SEL_S))
#define GPIO_FUNC64_IN_SEL_V  0x3F
#define GPIO_FUNC64_IN_SEL_S  0

#define GPIO_FUNC65_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x0234)
/* GPIO_SIG65_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG65_IN_SEL  (BIT(7))
#define GPIO_SIG65_IN_SEL_M  (BIT(7))
#define GPIO_SIG65_IN_SEL_V  0x1
#define GPIO_SIG65_IN_SEL_S  7
/* GPIO_FUNC65_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC65_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC65_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC65_IN_INV_SEL_V  0x1
#define GPIO_FUNC65_IN_INV_SEL_S  6
/* GPIO_FUNC65_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC65_IN_SEL  0x0000003F
#define GPIO_FUNC65_IN_SEL_M  ((GPIO_FUNC65_IN_SEL_V)<<(GPIO_FUNC65_IN_SEL_S))
#define GPIO_FUNC65_IN_SEL_V  0x3F
#define GPIO_FUNC65_IN_SEL_S  0

#define GPIO_FUNC66_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x0238)
/* GPIO_SIG66_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG66_IN_SEL  (BIT(7))
#define GPIO_SIG66_IN_SEL_M  (BIT(7))
#define GPIO_SIG66_IN_SEL_V  0x1
#define GPIO_SIG66_IN_SEL_S  7
/* GPIO_FUNC66_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC66_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC66_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC66_IN_INV_SEL_V  0x1
#define GPIO_FUNC66_IN_INV_SEL_S  6
/* GPIO_FUNC66_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC66_IN_SEL  0x0000003F
#define GPIO_FUNC66_IN_SEL_M  ((GPIO_FUNC66_IN_SEL_V)<<(GPIO_FUNC66_IN_SEL_S))
#define GPIO_FUNC66_IN_SEL_V  0x3F
#define GPIO_FUNC66_IN_SEL_S  0

#define GPIO_FUNC67_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x023c)
/* GPIO_SIG67_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG67_IN_SEL  (BIT(7))
#define GPIO_SIG67_IN_SEL_M  (BIT(7))
#define GPIO_SIG67_IN_SEL_V  0x1
#define GPIO_SIG67_IN_SEL_S  7
/* GPIO_FUNC67_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC67_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC67_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC67_IN_INV_SEL_V  0x1
#define GPIO_FUNC67_IN_INV_SEL_S  6
/* GPIO_FUNC67_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC67_IN_SEL  0x0000003F
#define GPIO_FUNC67_IN_SEL_M  ((GPIO_FUNC67_IN_SEL_V)<<(GPIO_FUNC67_IN_SEL_S))
#define GPIO_FUNC67_IN_SEL_V  0x3F
#define GPIO_FUNC67_IN_SEL_S  0

#define GPIO_FUNC68_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x0240)
/* GPIO_SIG68_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG68_IN_SEL  (BIT(7))
#define GPIO_SIG68_IN_SEL_M  (BIT(7))
#define GPIO_SIG68_IN_SEL_V  0x1
#define GPIO_SIG68_IN_SEL_S  7
/* GPIO_FUNC68_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC68_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC68_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC68_IN_INV_SEL_V  0x1
#define GPIO_FUNC68_IN_INV_SEL_S  6
/* GPIO_FUNC68_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC68_IN_SEL  0x0000003F
#define GPIO_FUNC68_IN_SEL_M  ((GPIO_FUNC68_IN_SEL_V)<<(GPIO_FUNC68_IN_SEL_S))
#define GPIO_FUNC68_IN_SEL_V  0x3F
#define GPIO_FUNC68_IN_SEL_S  0

#define GPIO_FUNC69_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x0244)
/* GPIO_SIG69_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG69_IN_SEL  (BIT(7))
#define GPIO_SIG69_IN_SEL_M  (BIT(7))
#define GPIO_SIG69_IN_SEL_V  0x1
#define GPIO_SIG69_IN_SEL_S  7
/* GPIO_FUNC69_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC69_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC69_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC69_IN_INV_SEL_V  0x1
#define GPIO_FUNC69_IN_INV_SEL_S  6
/* GPIO_FUNC69_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC69_IN_SEL  0x0000003F
#define GPIO_FUNC69_IN_SEL_M  ((GPIO_FUNC69_IN_SEL_V)<<(GPIO_FUNC69_IN_SEL_S))
#define GPIO_FUNC69_IN_SEL_V  0x3F
#define GPIO_FUNC69_IN_SEL_S  0

#define GPIO_FUNC70_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x0248)
/* GPIO_SIG70_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG70_IN_SEL  (BIT(7))
#define GPIO_SIG70_IN_SEL_M  (BIT(7))
#define GPIO_SIG70_IN_SEL_V  0x1
#define GPIO_SIG70_IN_SEL_S  7
/* GPIO_FUNC70_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC70_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC70_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC70_IN_INV_SEL_V  0x1
#define GPIO_FUNC70_IN_INV_SEL_S  6
/* GPIO_FUNC70_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC70_IN_SEL  0x0000003F
#define GPIO_FUNC70_IN_SEL_M  ((GPIO_FUNC70_IN_SEL_V)<<(GPIO_FUNC70_IN_SEL_S))
#define GPIO_FUNC70_IN_SEL_V  0x3F
#define GPIO_FUNC70_IN_SEL_S  0

#define GPIO_FUNC71_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x024c)
/* GPIO_SIG71_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG71_IN_SEL  (BIT(7))
#define GPIO_SIG71_IN_SEL_M  (BIT(7))
#define GPIO_SIG71_IN_SEL_V  0x1
#define GPIO_SIG71_IN_SEL_S  7
/* GPIO_FUNC71_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC71_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC71_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC71_IN_INV_SEL_V  0x1
#define GPIO_FUNC71_IN_INV_SEL_S  6
/* GPIO_FUNC71_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC71_IN_SEL  0x0000003F
#define GPIO_FUNC71_IN_SEL_M  ((GPIO_FUNC71_IN_SEL_V)<<(GPIO_FUNC71_IN_SEL_S))
#define GPIO_FUNC71_IN_SEL_V  0x3F
#define GPIO_FUNC71_IN_SEL_S  0

#define GPIO_FUNC72_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x0250)
/* GPIO_SIG72_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG72_IN_SEL  (BIT(7))
#define GPIO_SIG72_IN_SEL_M  (BIT(7))
#define GPIO_SIG72_IN_SEL_V  0x1
#define GPIO_SIG72_IN_SEL_S  7
/* GPIO_FUNC72_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC72_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC72_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC72_IN_INV_SEL_V  0x1
#define GPIO_FUNC72_IN_INV_SEL_S  6
/* GPIO_FUNC72_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC72_IN_SEL  0x0000003F
#define GPIO_FUNC72_IN_SEL_M  ((GPIO_FUNC72_IN_SEL_V)<<(GPIO_FUNC72_IN_SEL_S))
#define GPIO_FUNC72_IN_SEL_V  0x3F
#define GPIO_FUNC72_IN_SEL_S  0

#define GPIO_FUNC73_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x0254)
/* GPIO_SIG73_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG73_IN_SEL  (BIT(7))
#define GPIO_SIG73_IN_SEL_M  (BIT(7))
#define GPIO_SIG73_IN_SEL_V  0x1
#define GPIO_SIG73_IN_SEL_S  7
/* GPIO_FUNC73_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC73_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC73_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC73_IN_INV_SEL_V  0x1
#define GPIO_FUNC73_IN_INV_SEL_S  6
/* GPIO_FUNC73_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC73_IN_SEL  0x0000003F
#define GPIO_FUNC73_IN_SEL_M  ((GPIO_FUNC73_IN_SEL_V)<<(GPIO_FUNC73_IN_SEL_S))
#define GPIO_FUNC73_IN_SEL_V  0x3F
#define GPIO_FUNC73_IN_SEL_S  0

#define GPIO_FUNC74_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x0258)
/* GPIO_SIG74_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG74_IN_SEL  (BIT(7))
#define GPIO_SIG74_IN_SEL_M  (BIT(7))
#define GPIO_SIG74_IN_SEL_V  0x1
#define GPIO_SIG74_IN_SEL_S  7
/* GPIO_FUNC74_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC74_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC74_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC74_IN_INV_SEL_V  0x1
#define GPIO_FUNC74_IN_INV_SEL_S  6
/* GPIO_FUNC74_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC74_IN_SEL  0x0000003F
#define GPIO_FUNC74_IN_SEL_M  ((GPIO_FUNC74_IN_SEL_V)<<(GPIO_FUNC74_IN_SEL_S))
#define GPIO_FUNC74_IN_SEL_V  0x3F
#define GPIO_FUNC74_IN_SEL_S  0

#define GPIO_FUNC75_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x025c)
/* GPIO_SIG75_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG75_IN_SEL  (BIT(7))
#define GPIO_SIG75_IN_SEL_M  (BIT(7))
#define GPIO_SIG75_IN_SEL_V  0x1
#define GPIO_SIG75_IN_SEL_S  7
/* GPIO_FUNC75_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC75_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC75_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC75_IN_INV_SEL_V  0x1
#define GPIO_FUNC75_IN_INV_SEL_S  6
/* GPIO_FUNC75_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC75_IN_SEL  0x0000003F
#define GPIO_FUNC75_IN_SEL_M  ((GPIO_FUNC75_IN_SEL_V)<<(GPIO_FUNC75_IN_SEL_S))
#define GPIO_FUNC75_IN_SEL_V  0x3F
#define GPIO_FUNC75_IN_SEL_S  0

#define GPIO_FUNC76_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x0260)
/* GPIO_SIG76_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG76_IN_SEL  (BIT(7))
#define GPIO_SIG76_IN_SEL_M  (BIT(7))
#define GPIO_SIG76_IN_SEL_V  0x1
#define GPIO_SIG76_IN_SEL_S  7
/* GPIO_FUNC76_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC76_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC76_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC76_IN_INV_SEL_V  0x1
#define GPIO_FUNC76_IN_INV_SEL_S  6
/* GPIO_FUNC76_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC76_IN_SEL  0x0000003F
#define GPIO_FUNC76_IN_SEL_M  ((GPIO_FUNC76_IN_SEL_V)<<(GPIO_FUNC76_IN_SEL_S))
#define GPIO_FUNC76_IN_SEL_V  0x3F
#define GPIO_FUNC76_IN_SEL_S  0

#define GPIO_FUNC77_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x0264)
/* GPIO_SIG77_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG77_IN_SEL  (BIT(7))
#define GPIO_SIG77_IN_SEL_M  (BIT(7))
#define GPIO_SIG77_IN_SEL_V  0x1
#define GPIO_SIG77_IN_SEL_S  7
/* GPIO_FUNC77_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC77_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC77_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC77_IN_INV_SEL_V  0x1
#define GPIO_FUNC77_IN_INV_SEL_S  6
/* GPIO_FUNC77_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC77_IN_SEL  0x0000003F
#define GPIO_FUNC77_IN_SEL_M  ((GPIO_FUNC77_IN_SEL_V)<<(GPIO_FUNC77_IN_SEL_S))
#define GPIO_FUNC77_IN_SEL_V  0x3F
#define GPIO_FUNC77_IN_SEL_S  0

#define GPIO_FUNC78_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x0268)
/* GPIO_SIG78_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG78_IN_SEL  (BIT(7))
#define GPIO_SIG78_IN_SEL_M  (BIT(7))
#define GPIO_SIG78_IN_SEL_V  0x1
#define GPIO_SIG78_IN_SEL_S  7
/* GPIO_FUNC78_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC78_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC78_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC78_IN_INV_SEL_V  0x1
#define GPIO_FUNC78_IN_INV_SEL_S  6
/* GPIO_FUNC78_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC78_IN_SEL  0x0000003F
#define GPIO_FUNC78_IN_SEL_M  ((GPIO_FUNC78_IN_SEL_V)<<(GPIO_FUNC78_IN_SEL_S))
#define GPIO_FUNC78_IN_SEL_V  0x3F
#define GPIO_FUNC78_IN_SEL_S  0

#define GPIO_FUNC79_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x026c)
/* GPIO_SIG79_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG79_IN_SEL  (BIT(7))
#define GPIO_SIG79_IN_SEL_M  (BIT(7))
#define GPIO_SIG79_IN_SEL_V  0x1
#define GPIO_SIG79_IN_SEL_S  7
/* GPIO_FUNC79_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC79_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC79_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC79_IN_INV_SEL_V  0x1
#define GPIO_FUNC79_IN_INV_SEL_S  6
/* GPIO_FUNC79_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC79_IN_SEL  0x0000003F
#define GPIO_FUNC79_IN_SEL_M  ((GPIO_FUNC79_IN_SEL_V)<<(GPIO_FUNC79_IN_SEL_S))
#define GPIO_FUNC79_IN_SEL_V  0x3F
#define GPIO_FUNC79_IN_SEL_S  0

#define GPIO_FUNC80_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x0270)
/* GPIO_SIG80_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG80_IN_SEL  (BIT(7))
#define GPIO_SIG80_IN_SEL_M  (BIT(7))
#define GPIO_SIG80_IN_SEL_V  0x1
#define GPIO_SIG80_IN_SEL_S  7
/* GPIO_FUNC80_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC80_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC80_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC80_IN_INV_SEL_V  0x1
#define GPIO_FUNC80_IN_INV_SEL_S  6
/* GPIO_FUNC80_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC80_IN_SEL  0x0000003F
#define GPIO_FUNC80_IN_SEL_M  ((GPIO_FUNC80_IN_SEL_V)<<(GPIO_FUNC80_IN_SEL_S))
#define GPIO_FUNC80_IN_SEL_V  0x3F
#define GPIO_FUNC80_IN_SEL_S  0

#define GPIO_FUNC81_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x0274)
/* GPIO_SIG81_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG81_IN_SEL  (BIT(7))
#define GPIO_SIG81_IN_SEL_M  (BIT(7))
#define GPIO_SIG81_IN_SEL_V  0x1
#define GPIO_SIG81_IN_SEL_S  7
/* GPIO_FUNC81_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC81_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC81_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC81_IN_INV_SEL_V  0x1
#define GPIO_FUNC81_IN_INV_SEL_S  6
/* GPIO_FUNC81_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC81_IN_SEL  0x0000003F
#define GPIO_FUNC81_IN_SEL_M  ((GPIO_FUNC81_IN_SEL_V)<<(GPIO_FUNC81_IN_SEL_S))
#define GPIO_FUNC81_IN_SEL_V  0x3F
#define GPIO_FUNC81_IN_SEL_S  0

#define GPIO_FUNC82_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x0278)
/* GPIO_SIG82_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG82_IN_SEL  (BIT(7))
#define GPIO_SIG82_IN_SEL_M  (BIT(7))
#define GPIO_SIG82_IN_SEL_V  0x1
#define GPIO_SIG82_IN_SEL_S  7
/* GPIO_FUNC82_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC82_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC82_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC82_IN_INV_SEL_V  0x1
#define GPIO_FUNC82_IN_INV_SEL_S  6
/* GPIO_FUNC82_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC82_IN_SEL  0x0000003F
#define GPIO_FUNC82_IN_SEL_M  ((GPIO_FUNC82_IN_SEL_V)<<(GPIO_FUNC82_IN_SEL_S))
#define GPIO_FUNC82_IN_SEL_V  0x3F
#define GPIO_FUNC82_IN_SEL_S  0

#define GPIO_FUNC83_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x027c)
/* GPIO_SIG83_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG83_IN_SEL  (BIT(7))
#define GPIO_SIG83_IN_SEL_M  (BIT(7))
#define GPIO_SIG83_IN_SEL_V  0x1
#define GPIO_SIG83_IN_SEL_S  7
/* GPIO_FUNC83_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC83_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC83_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC83_IN_INV_SEL_V  0x1
#define GPIO_FUNC83_IN_INV_SEL_S  6
/* GPIO_FUNC83_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC83_IN_SEL  0x0000003F
#define GPIO_FUNC83_IN_SEL_M  ((GPIO_FUNC83_IN_SEL_V)<<(GPIO_FUNC83_IN_SEL_S))
#define GPIO_FUNC83_IN_SEL_V  0x3F
#define GPIO_FUNC83_IN_SEL_S  0

#define GPIO_FUNC84_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x0280)
/* GPIO_SIG84_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG84_IN_SEL  (BIT(7))
#define GPIO_SIG84_IN_SEL_M  (BIT(7))
#define GPIO_SIG84_IN_SEL_V  0x1
#define GPIO_SIG84_IN_SEL_S  7
/* GPIO_FUNC84_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC84_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC84_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC84_IN_INV_SEL_V  0x1
#define GPIO_FUNC84_IN_INV_SEL_S  6
/* GPIO_FUNC84_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC84_IN_SEL  0x0000003F
#define GPIO_FUNC84_IN_SEL_M  ((GPIO_FUNC84_IN_SEL_V)<<(GPIO_FUNC84_IN_SEL_S))
#define GPIO_FUNC84_IN_SEL_V  0x3F
#define GPIO_FUNC84_IN_SEL_S  0

#define GPIO_FUNC85_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x0284)
/* GPIO_SIG85_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG85_IN_SEL  (BIT(7))
#define GPIO_SIG85_IN_SEL_M  (BIT(7))
#define GPIO_SIG85_IN_SEL_V  0x1
#define GPIO_SIG85_IN_SEL_S  7
/* GPIO_FUNC85_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC85_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC85_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC85_IN_INV_SEL_V  0x1
#define GPIO_FUNC85_IN_INV_SEL_S  6
/* GPIO_FUNC85_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC85_IN_SEL  0x0000003F
#define GPIO_FUNC85_IN_SEL_M  ((GPIO_FUNC85_IN_SEL_V)<<(GPIO_FUNC85_IN_SEL_S))
#define GPIO_FUNC85_IN_SEL_V  0x3F
#define GPIO_FUNC85_IN_SEL_S  0

#define GPIO_FUNC86_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x0288)
/* GPIO_SIG86_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG86_IN_SEL  (BIT(7))
#define GPIO_SIG86_IN_SEL_M  (BIT(7))
#define GPIO_SIG86_IN_SEL_V  0x1
#define GPIO_SIG86_IN_SEL_S  7
/* GPIO_FUNC86_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC86_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC86_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC86_IN_INV_SEL_V  0x1
#define GPIO_FUNC86_IN_INV_SEL_S  6
/* GPIO_FUNC86_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC86_IN_SEL  0x0000003F
#define GPIO_FUNC86_IN_SEL_M  ((GPIO_FUNC86_IN_SEL_V)<<(GPIO_FUNC86_IN_SEL_S))
#define GPIO_FUNC86_IN_SEL_V  0x3F
#define GPIO_FUNC86_IN_SEL_S  0

#define GPIO_FUNC87_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x028c)
/* GPIO_SIG87_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG87_IN_SEL  (BIT(7))
#define GPIO_SIG87_IN_SEL_M  (BIT(7))
#define GPIO_SIG87_IN_SEL_V  0x1
#define GPIO_SIG87_IN_SEL_S  7
/* GPIO_FUNC87_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC87_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC87_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC87_IN_INV_SEL_V  0x1
#define GPIO_FUNC87_IN_INV_SEL_S  6
/* GPIO_FUNC87_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC87_IN_SEL  0x0000003F
#define GPIO_FUNC87_IN_SEL_M  ((GPIO_FUNC87_IN_SEL_V)<<(GPIO_FUNC87_IN_SEL_S))
#define GPIO_FUNC87_IN_SEL_V  0x3F
#define GPIO_FUNC87_IN_SEL_S  0

#define GPIO_FUNC88_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x0290)
/* GPIO_SIG88_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG88_IN_SEL  (BIT(7))
#define GPIO_SIG88_IN_SEL_M  (BIT(7))
#define GPIO_SIG88_IN_SEL_V  0x1
#define GPIO_SIG88_IN_SEL_S  7
/* GPIO_FUNC88_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC88_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC88_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC88_IN_INV_SEL_V  0x1
#define GPIO_FUNC88_IN_INV_SEL_S  6
/* GPIO_FUNC88_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC88_IN_SEL  0x0000003F
#define GPIO_FUNC88_IN_SEL_M  ((GPIO_FUNC88_IN_SEL_V)<<(GPIO_FUNC88_IN_SEL_S))
#define GPIO_FUNC88_IN_SEL_V  0x3F
#define GPIO_FUNC88_IN_SEL_S  0

#define GPIO_FUNC89_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x0294)
/* GPIO_SIG89_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG89_IN_SEL  (BIT(7))
#define GPIO_SIG89_IN_SEL_M  (BIT(7))
#define GPIO_SIG89_IN_SEL_V  0x1
#define GPIO_SIG89_IN_SEL_S  7
/* GPIO_FUNC89_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC89_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC89_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC89_IN_INV_SEL_V  0x1
#define GPIO_FUNC89_IN_INV_SEL_S  6
/* GPIO_FUNC89_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC89_IN_SEL  0x0000003F
#define GPIO_FUNC89_IN_SEL_M  ((GPIO_FUNC89_IN_SEL_V)<<(GPIO_FUNC89_IN_SEL_S))
#define GPIO_FUNC89_IN_SEL_V  0x3F
#define GPIO_FUNC89_IN_SEL_S  0

#define GPIO_FUNC90_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x0298)
/* GPIO_SIG90_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG90_IN_SEL  (BIT(7))
#define GPIO_SIG90_IN_SEL_M  (BIT(7))
#define GPIO_SIG90_IN_SEL_V  0x1
#define GPIO_SIG90_IN_SEL_S  7
/* GPIO_FUNC90_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC90_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC90_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC90_IN_INV_SEL_V  0x1
#define GPIO_FUNC90_IN_INV_SEL_S  6
/* GPIO_FUNC90_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC90_IN_SEL  0x0000003F
#define GPIO_FUNC90_IN_SEL_M  ((GPIO_FUNC90_IN_SEL_V)<<(GPIO_FUNC90_IN_SEL_S))
#define GPIO_FUNC90_IN_SEL_V  0x3F
#define GPIO_FUNC90_IN_SEL_S  0

#define GPIO_FUNC91_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x029c)
/* GPIO_SIG91_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG91_IN_SEL  (BIT(7))
#define GPIO_SIG91_IN_SEL_M  (BIT(7))
#define GPIO_SIG91_IN_SEL_V  0x1
#define GPIO_SIG91_IN_SEL_S  7
/* GPIO_FUNC91_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC91_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC91_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC91_IN_INV_SEL_V  0x1
#define GPIO_FUNC91_IN_INV_SEL_S  6
/* GPIO_FUNC91_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC91_IN_SEL  0x0000003F
#define GPIO_FUNC91_IN_SEL_M  ((GPIO_FUNC91_IN_SEL_V)<<(GPIO_FUNC91_IN_SEL_S))
#define GPIO_FUNC91_IN_SEL_V  0x3F
#define GPIO_FUNC91_IN_SEL_S  0

#define GPIO_FUNC92_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x02a0)
/* GPIO_SIG92_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG92_IN_SEL  (BIT(7))
#define GPIO_SIG92_IN_SEL_M  (BIT(7))
#define GPIO_SIG92_IN_SEL_V  0x1
#define GPIO_SIG92_IN_SEL_S  7
/* GPIO_FUNC92_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC92_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC92_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC92_IN_INV_SEL_V  0x1
#define GPIO_FUNC92_IN_INV_SEL_S  6
/* GPIO_FUNC92_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC92_IN_SEL  0x0000003F
#define GPIO_FUNC92_IN_SEL_M  ((GPIO_FUNC92_IN_SEL_V)<<(GPIO_FUNC92_IN_SEL_S))
#define GPIO_FUNC92_IN_SEL_V  0x3F
#define GPIO_FUNC92_IN_SEL_S  0

#define GPIO_FUNC93_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x02a4)
/* GPIO_SIG93_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG93_IN_SEL  (BIT(7))
#define GPIO_SIG93_IN_SEL_M  (BIT(7))
#define GPIO_SIG93_IN_SEL_V  0x1
#define GPIO_SIG93_IN_SEL_S  7
/* GPIO_FUNC93_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC93_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC93_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC93_IN_INV_SEL_V  0x1
#define GPIO_FUNC93_IN_INV_SEL_S  6
/* GPIO_FUNC93_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC93_IN_SEL  0x0000003F
#define GPIO_FUNC93_IN_SEL_M  ((GPIO_FUNC93_IN_SEL_V)<<(GPIO_FUNC93_IN_SEL_S))
#define GPIO_FUNC93_IN_SEL_V  0x3F
#define GPIO_FUNC93_IN_SEL_S  0

#define GPIO_FUNC94_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x02a8)
/* GPIO_SIG94_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG94_IN_SEL  (BIT(7))
#define GPIO_SIG94_IN_SEL_M  (BIT(7))
#define GPIO_SIG94_IN_SEL_V  0x1
#define GPIO_SIG94_IN_SEL_S  7
/* GPIO_FUNC94_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC94_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC94_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC94_IN_INV_SEL_V  0x1
#define GPIO_FUNC94_IN_INV_SEL_S  6
/* GPIO_FUNC94_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC94_IN_SEL  0x0000003F
#define GPIO_FUNC94_IN_SEL_M  ((GPIO_FUNC94_IN_SEL_V)<<(GPIO_FUNC94_IN_SEL_S))
#define GPIO_FUNC94_IN_SEL_V  0x3F
#define GPIO_FUNC94_IN_SEL_S  0

#define GPIO_FUNC95_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x02ac)
/* GPIO_SIG95_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG95_IN_SEL  (BIT(7))
#define GPIO_SIG95_IN_SEL_M  (BIT(7))
#define GPIO_SIG95_IN_SEL_V  0x1
#define GPIO_SIG95_IN_SEL_S  7
/* GPIO_FUNC95_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC95_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC95_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC95_IN_INV_SEL_V  0x1
#define GPIO_FUNC95_IN_INV_SEL_S  6
/* GPIO_FUNC95_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC95_IN_SEL  0x0000003F
#define GPIO_FUNC95_IN_SEL_M  ((GPIO_FUNC95_IN_SEL_V)<<(GPIO_FUNC95_IN_SEL_S))
#define GPIO_FUNC95_IN_SEL_V  0x3F
#define GPIO_FUNC95_IN_SEL_S  0

#define GPIO_FUNC96_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x02b0)
/* GPIO_SIG96_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG96_IN_SEL  (BIT(7))
#define GPIO_SIG96_IN_SEL_M  (BIT(7))
#define GPIO_SIG96_IN_SEL_V  0x1
#define GPIO_SIG96_IN_SEL_S  7
/* GPIO_FUNC96_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC96_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC96_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC96_IN_INV_SEL_V  0x1
#define GPIO_FUNC96_IN_INV_SEL_S  6
/* GPIO_FUNC96_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC96_IN_SEL  0x0000003F
#define GPIO_FUNC96_IN_SEL_M  ((GPIO_FUNC96_IN_SEL_V)<<(GPIO_FUNC96_IN_SEL_S))
#define GPIO_FUNC96_IN_SEL_V  0x3F
#define GPIO_FUNC96_IN_SEL_S  0

#define GPIO_FUNC97_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x02b4)
/* GPIO_SIG97_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG97_IN_SEL  (BIT(7))
#define GPIO_SIG97_IN_SEL_M  (BIT(7))
#define GPIO_SIG97_IN_SEL_V  0x1
#define GPIO_SIG97_IN_SEL_S  7
/* GPIO_FUNC97_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC97_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC97_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC97_IN_INV_SEL_V  0x1
#define GPIO_FUNC97_IN_INV_SEL_S  6
/* GPIO_FUNC97_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC97_IN_SEL  0x0000003F
#define GPIO_FUNC97_IN_SEL_M  ((GPIO_FUNC97_IN_SEL_V)<<(GPIO_FUNC97_IN_SEL_S))
#define GPIO_FUNC97_IN_SEL_V  0x3F
#define GPIO_FUNC97_IN_SEL_S  0

#define GPIO_FUNC98_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x02b8)
/* GPIO_SIG98_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG98_IN_SEL  (BIT(7))
#define GPIO_SIG98_IN_SEL_M  (BIT(7))
#define GPIO_SIG98_IN_SEL_V  0x1
#define GPIO_SIG98_IN_SEL_S  7
/* GPIO_FUNC98_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC98_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC98_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC98_IN_INV_SEL_V  0x1
#define GPIO_FUNC98_IN_INV_SEL_S  6
/* GPIO_FUNC98_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC98_IN_SEL  0x0000003F
#define GPIO_FUNC98_IN_SEL_M  ((GPIO_FUNC98_IN_SEL_V)<<(GPIO_FUNC98_IN_SEL_S))
#define GPIO_FUNC98_IN_SEL_V  0x3F
#define GPIO_FUNC98_IN_SEL_S  0

#define GPIO_FUNC99_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x02bc)
/* GPIO_SIG99_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG99_IN_SEL  (BIT(7))
#define GPIO_SIG99_IN_SEL_M  (BIT(7))
#define GPIO_SIG99_IN_SEL_V  0x1
#define GPIO_SIG99_IN_SEL_S  7
/* GPIO_FUNC99_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC99_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC99_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC99_IN_INV_SEL_V  0x1
#define GPIO_FUNC99_IN_INV_SEL_S  6
/* GPIO_FUNC99_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC99_IN_SEL  0x0000003F
#define GPIO_FUNC99_IN_SEL_M  ((GPIO_FUNC99_IN_SEL_V)<<(GPIO_FUNC99_IN_SEL_S))
#define GPIO_FUNC99_IN_SEL_V  0x3F
#define GPIO_FUNC99_IN_SEL_S  0

#define GPIO_FUNC100_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x02c0)
/* GPIO_SIG100_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG100_IN_SEL  (BIT(7))
#define GPIO_SIG100_IN_SEL_M  (BIT(7))
#define GPIO_SIG100_IN_SEL_V  0x1
#define GPIO_SIG100_IN_SEL_S  7
/* GPIO_FUNC100_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC100_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC100_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC100_IN_INV_SEL_V  0x1
#define GPIO_FUNC100_IN_INV_SEL_S  6
/* GPIO_FUNC100_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC100_IN_SEL  0x0000003F
#define GPIO_FUNC100_IN_SEL_M  ((GPIO_FUNC100_IN_SEL_V)<<(GPIO_FUNC100_IN_SEL_S))
#define GPIO_FUNC100_IN_SEL_V  0x3F
#define GPIO_FUNC100_IN_SEL_S  0

#define GPIO_FUNC101_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x02c4)
/* GPIO_SIG101_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG101_IN_SEL  (BIT(7))
#define GPIO_SIG101_IN_SEL_M  (BIT(7))
#define GPIO_SIG101_IN_SEL_V  0x1
#define GPIO_SIG101_IN_SEL_S  7
/* GPIO_FUNC101_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC101_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC101_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC101_IN_INV_SEL_V  0x1
#define GPIO_FUNC101_IN_INV_SEL_S  6
/* GPIO_FUNC101_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC101_IN_SEL  0x0000003F
#define GPIO_FUNC101_IN_SEL_M  ((GPIO_FUNC101_IN_SEL_V)<<(GPIO_FUNC101_IN_SEL_S))
#define GPIO_FUNC101_IN_SEL_V  0x3F
#define GPIO_FUNC101_IN_SEL_S  0

#define GPIO_FUNC102_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x02c8)
/* GPIO_SIG102_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG102_IN_SEL  (BIT(7))
#define GPIO_SIG102_IN_SEL_M  (BIT(7))
#define GPIO_SIG102_IN_SEL_V  0x1
#define GPIO_SIG102_IN_SEL_S  7
/* GPIO_FUNC102_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC102_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC102_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC102_IN_INV_SEL_V  0x1
#define GPIO_FUNC102_IN_INV_SEL_S  6
/* GPIO_FUNC102_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC102_IN_SEL  0x0000003F
#define GPIO_FUNC102_IN_SEL_M  ((GPIO_FUNC102_IN_SEL_V)<<(GPIO_FUNC102_IN_SEL_S))
#define GPIO_FUNC102_IN_SEL_V  0x3F
#define GPIO_FUNC102_IN_SEL_S  0

#define GPIO_FUNC103_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x02cc)
/* GPIO_SIG103_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG103_IN_SEL  (BIT(7))
#define GPIO_SIG103_IN_SEL_M  (BIT(7))
#define GPIO_SIG103_IN_SEL_V  0x1
#define GPIO_SIG103_IN_SEL_S  7
/* GPIO_FUNC103_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC103_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC103_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC103_IN_INV_SEL_V  0x1
#define GPIO_FUNC103_IN_INV_SEL_S  6
/* GPIO_FUNC103_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC103_IN_SEL  0x0000003F
#define GPIO_FUNC103_IN_SEL_M  ((GPIO_FUNC103_IN_SEL_V)<<(GPIO_FUNC103_IN_SEL_S))
#define GPIO_FUNC103_IN_SEL_V  0x3F
#define GPIO_FUNC103_IN_SEL_S  0

#define GPIO_FUNC104_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x02d0)
/* GPIO_SIG104_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG104_IN_SEL  (BIT(7))
#define GPIO_SIG104_IN_SEL_M  (BIT(7))
#define GPIO_SIG104_IN_SEL_V  0x1
#define GPIO_SIG104_IN_SEL_S  7
/* GPIO_FUNC104_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC104_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC104_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC104_IN_INV_SEL_V  0x1
#define GPIO_FUNC104_IN_INV_SEL_S  6
/* GPIO_FUNC104_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC104_IN_SEL  0x0000003F
#define GPIO_FUNC104_IN_SEL_M  ((GPIO_FUNC104_IN_SEL_V)<<(GPIO_FUNC104_IN_SEL_S))
#define GPIO_FUNC104_IN_SEL_V  0x3F
#define GPIO_FUNC104_IN_SEL_S  0

#define GPIO_FUNC105_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x02d4)
/* GPIO_SIG105_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG105_IN_SEL  (BIT(7))
#define GPIO_SIG105_IN_SEL_M  (BIT(7))
#define GPIO_SIG105_IN_SEL_V  0x1
#define GPIO_SIG105_IN_SEL_S  7
/* GPIO_FUNC105_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC105_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC105_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC105_IN_INV_SEL_V  0x1
#define GPIO_FUNC105_IN_INV_SEL_S  6
/* GPIO_FUNC105_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC105_IN_SEL  0x0000003F
#define GPIO_FUNC105_IN_SEL_M  ((GPIO_FUNC105_IN_SEL_V)<<(GPIO_FUNC105_IN_SEL_S))
#define GPIO_FUNC105_IN_SEL_V  0x3F
#define GPIO_FUNC105_IN_SEL_S  0

#define GPIO_FUNC106_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x02d8)
/* GPIO_SIG106_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG106_IN_SEL  (BIT(7))
#define GPIO_SIG106_IN_SEL_M  (BIT(7))
#define GPIO_SIG106_IN_SEL_V  0x1
#define GPIO_SIG106_IN_SEL_S  7
/* GPIO_FUNC106_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC106_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC106_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC106_IN_INV_SEL_V  0x1
#define GPIO_FUNC106_IN_INV_SEL_S  6
/* GPIO_FUNC106_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC106_IN_SEL  0x0000003F
#define GPIO_FUNC106_IN_SEL_M  ((GPIO_FUNC106_IN_SEL_V)<<(GPIO_FUNC106_IN_SEL_S))
#define GPIO_FUNC106_IN_SEL_V  0x3F
#define GPIO_FUNC106_IN_SEL_S  0

#define GPIO_FUNC107_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x02dc)
/* GPIO_SIG107_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG107_IN_SEL  (BIT(7))
#define GPIO_SIG107_IN_SEL_M  (BIT(7))
#define GPIO_SIG107_IN_SEL_V  0x1
#define GPIO_SIG107_IN_SEL_S  7
/* GPIO_FUNC107_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC107_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC107_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC107_IN_INV_SEL_V  0x1
#define GPIO_FUNC107_IN_INV_SEL_S  6
/* GPIO_FUNC107_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC107_IN_SEL  0x0000003F
#define GPIO_FUNC107_IN_SEL_M  ((GPIO_FUNC107_IN_SEL_V)<<(GPIO_FUNC107_IN_SEL_S))
#define GPIO_FUNC107_IN_SEL_V  0x3F
#define GPIO_FUNC107_IN_SEL_S  0

#define GPIO_FUNC108_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x02e0)
/* GPIO_SIG108_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG108_IN_SEL  (BIT(7))
#define GPIO_SIG108_IN_SEL_M  (BIT(7))
#define GPIO_SIG108_IN_SEL_V  0x1
#define GPIO_SIG108_IN_SEL_S  7
/* GPIO_FUNC108_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC108_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC108_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC108_IN_INV_SEL_V  0x1
#define GPIO_FUNC108_IN_INV_SEL_S  6
/* GPIO_FUNC108_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC108_IN_SEL  0x0000003F
#define GPIO_FUNC108_IN_SEL_M  ((GPIO_FUNC108_IN_SEL_V)<<(GPIO_FUNC108_IN_SEL_S))
#define GPIO_FUNC108_IN_SEL_V  0x3F
#define GPIO_FUNC108_IN_SEL_S  0

#define GPIO_FUNC109_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x02e4)
/* GPIO_SIG109_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG109_IN_SEL  (BIT(7))
#define GPIO_SIG109_IN_SEL_M  (BIT(7))
#define GPIO_SIG109_IN_SEL_V  0x1
#define GPIO_SIG109_IN_SEL_S  7
/* GPIO_FUNC109_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC109_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC109_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC109_IN_INV_SEL_V  0x1
#define GPIO_FUNC109_IN_INV_SEL_S  6
/* GPIO_FUNC109_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC109_IN_SEL  0x0000003F
#define GPIO_FUNC109_IN_SEL_M  ((GPIO_FUNC109_IN_SEL_V)<<(GPIO_FUNC109_IN_SEL_S))
#define GPIO_FUNC109_IN_SEL_V  0x3F
#define GPIO_FUNC109_IN_SEL_S  0

#define GPIO_FUNC110_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x02e8)
/* GPIO_SIG110_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG110_IN_SEL  (BIT(7))
#define GPIO_SIG110_IN_SEL_M  (BIT(7))
#define GPIO_SIG110_IN_SEL_V  0x1
#define GPIO_SIG110_IN_SEL_S  7
/* GPIO_FUNC110_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC110_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC110_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC110_IN_INV_SEL_V  0x1
#define GPIO_FUNC110_IN_INV_SEL_S  6
/* GPIO_FUNC110_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC110_IN_SEL  0x0000003F
#define GPIO_FUNC110_IN_SEL_M  ((GPIO_FUNC110_IN_SEL_V)<<(GPIO_FUNC110_IN_SEL_S))
#define GPIO_FUNC110_IN_SEL_V  0x3F
#define GPIO_FUNC110_IN_SEL_S  0

#define GPIO_FUNC111_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x02ec)
/* GPIO_SIG111_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG111_IN_SEL  (BIT(7))
#define GPIO_SIG111_IN_SEL_M  (BIT(7))
#define GPIO_SIG111_IN_SEL_V  0x1
#define GPIO_SIG111_IN_SEL_S  7
/* GPIO_FUNC111_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC111_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC111_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC111_IN_INV_SEL_V  0x1
#define GPIO_FUNC111_IN_INV_SEL_S  6
/* GPIO_FUNC111_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC111_IN_SEL  0x0000003F
#define GPIO_FUNC111_IN_SEL_M  ((GPIO_FUNC111_IN_SEL_V)<<(GPIO_FUNC111_IN_SEL_S))
#define GPIO_FUNC111_IN_SEL_V  0x3F
#define GPIO_FUNC111_IN_SEL_S  0

#define GPIO_FUNC112_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x02f0)
/* GPIO_SIG112_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG112_IN_SEL  (BIT(7))
#define GPIO_SIG112_IN_SEL_M  (BIT(7))
#define GPIO_SIG112_IN_SEL_V  0x1
#define GPIO_SIG112_IN_SEL_S  7
/* GPIO_FUNC112_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC112_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC112_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC112_IN_INV_SEL_V  0x1
#define GPIO_FUNC112_IN_INV_SEL_S  6
/* GPIO_FUNC112_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC112_IN_SEL  0x0000003F
#define GPIO_FUNC112_IN_SEL_M  ((GPIO_FUNC112_IN_SEL_V)<<(GPIO_FUNC112_IN_SEL_S))
#define GPIO_FUNC112_IN_SEL_V  0x3F
#define GPIO_FUNC112_IN_SEL_S  0

#define GPIO_FUNC113_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x02f4)
/* GPIO_SIG113_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG113_IN_SEL  (BIT(7))
#define GPIO_SIG113_IN_SEL_M  (BIT(7))
#define GPIO_SIG113_IN_SEL_V  0x1
#define GPIO_SIG113_IN_SEL_S  7
/* GPIO_FUNC113_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC113_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC113_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC113_IN_INV_SEL_V  0x1
#define GPIO_FUNC113_IN_INV_SEL_S  6
/* GPIO_FUNC113_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC113_IN_SEL  0x0000003F
#define GPIO_FUNC113_IN_SEL_M  ((GPIO_FUNC113_IN_SEL_V)<<(GPIO_FUNC113_IN_SEL_S))
#define GPIO_FUNC113_IN_SEL_V  0x3F
#define GPIO_FUNC113_IN_SEL_S  0

#define GPIO_FUNC114_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x02f8)
/* GPIO_SIG114_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG114_IN_SEL  (BIT(7))
#define GPIO_SIG114_IN_SEL_M  (BIT(7))
#define GPIO_SIG114_IN_SEL_V  0x1
#define GPIO_SIG114_IN_SEL_S  7
/* GPIO_FUNC114_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC114_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC114_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC114_IN_INV_SEL_V  0x1
#define GPIO_FUNC114_IN_INV_SEL_S  6
/* GPIO_FUNC114_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC114_IN_SEL  0x0000003F
#define GPIO_FUNC114_IN_SEL_M  ((GPIO_FUNC114_IN_SEL_V)<<(GPIO_FUNC114_IN_SEL_S))
#define GPIO_FUNC114_IN_SEL_V  0x3F
#define GPIO_FUNC114_IN_SEL_S  0

#define GPIO_FUNC115_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x02fc)
/* GPIO_SIG115_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG115_IN_SEL  (BIT(7))
#define GPIO_SIG115_IN_SEL_M  (BIT(7))
#define GPIO_SIG115_IN_SEL_V  0x1
#define GPIO_SIG115_IN_SEL_S  7
/* GPIO_FUNC115_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC115_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC115_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC115_IN_INV_SEL_V  0x1
#define GPIO_FUNC115_IN_INV_SEL_S  6
/* GPIO_FUNC115_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC115_IN_SEL  0x0000003F
#define GPIO_FUNC115_IN_SEL_M  ((GPIO_FUNC115_IN_SEL_V)<<(GPIO_FUNC115_IN_SEL_S))
#define GPIO_FUNC115_IN_SEL_V  0x3F
#define GPIO_FUNC115_IN_SEL_S  0

#define GPIO_FUNC116_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x0300)
/* GPIO_SIG116_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG116_IN_SEL  (BIT(7))
#define GPIO_SIG116_IN_SEL_M  (BIT(7))
#define GPIO_SIG116_IN_SEL_V  0x1
#define GPIO_SIG116_IN_SEL_S  7
/* GPIO_FUNC116_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC116_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC116_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC116_IN_INV_SEL_V  0x1
#define GPIO_FUNC116_IN_INV_SEL_S  6
/* GPIO_FUNC116_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC116_IN_SEL  0x0000003F
#define GPIO_FUNC116_IN_SEL_M  ((GPIO_FUNC116_IN_SEL_V)<<(GPIO_FUNC116_IN_SEL_S))
#define GPIO_FUNC116_IN_SEL_V  0x3F
#define GPIO_FUNC116_IN_SEL_S  0

#define GPIO_FUNC117_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x0304)
/* GPIO_SIG117_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG117_IN_SEL  (BIT(7))
#define GPIO_SIG117_IN_SEL_M  (BIT(7))
#define GPIO_SIG117_IN_SEL_V  0x1
#define GPIO_SIG117_IN_SEL_S  7
/* GPIO_FUNC117_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC117_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC117_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC117_IN_INV_SEL_V  0x1
#define GPIO_FUNC117_IN_INV_SEL_S  6
/* GPIO_FUNC117_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC117_IN_SEL  0x0000003F
#define GPIO_FUNC117_IN_SEL_M  ((GPIO_FUNC117_IN_SEL_V)<<(GPIO_FUNC117_IN_SEL_S))
#define GPIO_FUNC117_IN_SEL_V  0x3F
#define GPIO_FUNC117_IN_SEL_S  0

#define GPIO_FUNC118_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x0308)
/* GPIO_SIG118_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG118_IN_SEL  (BIT(7))
#define GPIO_SIG118_IN_SEL_M  (BIT(7))
#define GPIO_SIG118_IN_SEL_V  0x1
#define GPIO_SIG118_IN_SEL_S  7
/* GPIO_FUNC118_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC118_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC118_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC118_IN_INV_SEL_V  0x1
#define GPIO_FUNC118_IN_INV_SEL_S  6
/* GPIO_FUNC118_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC118_IN_SEL  0x0000003F
#define GPIO_FUNC118_IN_SEL_M  ((GPIO_FUNC118_IN_SEL_V)<<(GPIO_FUNC118_IN_SEL_S))
#define GPIO_FUNC118_IN_SEL_V  0x3F
#define GPIO_FUNC118_IN_SEL_S  0

#define GPIO_FUNC119_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x030c)
/* GPIO_SIG119_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG119_IN_SEL  (BIT(7))
#define GPIO_SIG119_IN_SEL_M  (BIT(7))
#define GPIO_SIG119_IN_SEL_V  0x1
#define GPIO_SIG119_IN_SEL_S  7
/* GPIO_FUNC119_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC119_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC119_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC119_IN_INV_SEL_V  0x1
#define GPIO_FUNC119_IN_INV_SEL_S  6
/* GPIO_FUNC119_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC119_IN_SEL  0x0000003F
#define GPIO_FUNC119_IN_SEL_M  ((GPIO_FUNC119_IN_SEL_V)<<(GPIO_FUNC119_IN_SEL_S))
#define GPIO_FUNC119_IN_SEL_V  0x3F
#define GPIO_FUNC119_IN_SEL_S  0

#define GPIO_FUNC120_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x0310)
/* GPIO_SIG120_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG120_IN_SEL  (BIT(7))
#define GPIO_SIG120_IN_SEL_M  (BIT(7))
#define GPIO_SIG120_IN_SEL_V  0x1
#define GPIO_SIG120_IN_SEL_S  7
/* GPIO_FUNC120_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC120_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC120_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC120_IN_INV_SEL_V  0x1
#define GPIO_FUNC120_IN_INV_SEL_S  6
/* GPIO_FUNC120_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC120_IN_SEL  0x0000003F
#define GPIO_FUNC120_IN_SEL_M  ((GPIO_FUNC120_IN_SEL_V)<<(GPIO_FUNC120_IN_SEL_S))
#define GPIO_FUNC120_IN_SEL_V  0x3F
#define GPIO_FUNC120_IN_SEL_S  0

#define GPIO_FUNC121_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x0314)
/* GPIO_SIG121_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG121_IN_SEL  (BIT(7))
#define GPIO_SIG121_IN_SEL_M  (BIT(7))
#define GPIO_SIG121_IN_SEL_V  0x1
#define GPIO_SIG121_IN_SEL_S  7
/* GPIO_FUNC121_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC121_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC121_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC121_IN_INV_SEL_V  0x1
#define GPIO_FUNC121_IN_INV_SEL_S  6
/* GPIO_FUNC121_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC121_IN_SEL  0x0000003F
#define GPIO_FUNC121_IN_SEL_M  ((GPIO_FUNC121_IN_SEL_V)<<(GPIO_FUNC121_IN_SEL_S))
#define GPIO_FUNC121_IN_SEL_V  0x3F
#define GPIO_FUNC121_IN_SEL_S  0

#define GPIO_FUNC122_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x0318)
/* GPIO_SIG122_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG122_IN_SEL  (BIT(7))
#define GPIO_SIG122_IN_SEL_M  (BIT(7))
#define GPIO_SIG122_IN_SEL_V  0x1
#define GPIO_SIG122_IN_SEL_S  7
/* GPIO_FUNC122_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC122_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC122_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC122_IN_INV_SEL_V  0x1
#define GPIO_FUNC122_IN_INV_SEL_S  6
/* GPIO_FUNC122_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC122_IN_SEL  0x0000003F
#define GPIO_FUNC122_IN_SEL_M  ((GPIO_FUNC122_IN_SEL_V)<<(GPIO_FUNC122_IN_SEL_S))
#define GPIO_FUNC122_IN_SEL_V  0x3F
#define GPIO_FUNC122_IN_SEL_S  0

#define GPIO_FUNC123_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x031c)
/* GPIO_SIG123_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG123_IN_SEL  (BIT(7))
#define GPIO_SIG123_IN_SEL_M  (BIT(7))
#define GPIO_SIG123_IN_SEL_V  0x1
#define GPIO_SIG123_IN_SEL_S  7
/* GPIO_FUNC123_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC123_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC123_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC123_IN_INV_SEL_V  0x1
#define GPIO_FUNC123_IN_INV_SEL_S  6
/* GPIO_FUNC123_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC123_IN_SEL  0x0000003F
#define GPIO_FUNC123_IN_SEL_M  ((GPIO_FUNC123_IN_SEL_V)<<(GPIO_FUNC123_IN_SEL_S))
#define GPIO_FUNC123_IN_SEL_V  0x3F
#define GPIO_FUNC123_IN_SEL_S  0

#define GPIO_FUNC124_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x0320)
/* GPIO_SIG124_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG124_IN_SEL  (BIT(7))
#define GPIO_SIG124_IN_SEL_M  (BIT(7))
#define GPIO_SIG124_IN_SEL_V  0x1
#define GPIO_SIG124_IN_SEL_S  7
/* GPIO_FUNC124_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC124_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC124_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC124_IN_INV_SEL_V  0x1
#define GPIO_FUNC124_IN_INV_SEL_S  6
/* GPIO_FUNC124_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC124_IN_SEL  0x0000003F
#define GPIO_FUNC124_IN_SEL_M  ((GPIO_FUNC124_IN_SEL_V)<<(GPIO_FUNC124_IN_SEL_S))
#define GPIO_FUNC124_IN_SEL_V  0x3F
#define GPIO_FUNC124_IN_SEL_S  0

#define GPIO_FUNC125_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x0324)
/* GPIO_SIG125_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG125_IN_SEL  (BIT(7))
#define GPIO_SIG125_IN_SEL_M  (BIT(7))
#define GPIO_SIG125_IN_SEL_V  0x1
#define GPIO_SIG125_IN_SEL_S  7
/* GPIO_FUNC125_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC125_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC125_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC125_IN_INV_SEL_V  0x1
#define GPIO_FUNC125_IN_INV_SEL_S  6
/* GPIO_FUNC125_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC125_IN_SEL  0x0000003F
#define GPIO_FUNC125_IN_SEL_M  ((GPIO_FUNC125_IN_SEL_V)<<(GPIO_FUNC125_IN_SEL_S))
#define GPIO_FUNC125_IN_SEL_V  0x3F
#define GPIO_FUNC125_IN_SEL_S  0

#define GPIO_FUNC126_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x0328)
/* GPIO_SIG126_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG126_IN_SEL  (BIT(7))
#define GPIO_SIG126_IN_SEL_M  (BIT(7))
#define GPIO_SIG126_IN_SEL_V  0x1
#define GPIO_SIG126_IN_SEL_S  7
/* GPIO_FUNC126_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC126_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC126_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC126_IN_INV_SEL_V  0x1
#define GPIO_FUNC126_IN_INV_SEL_S  6
/* GPIO_FUNC126_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC126_IN_SEL  0x0000003F
#define GPIO_FUNC126_IN_SEL_M  ((GPIO_FUNC126_IN_SEL_V)<<(GPIO_FUNC126_IN_SEL_S))
#define GPIO_FUNC126_IN_SEL_V  0x3F
#define GPIO_FUNC126_IN_SEL_S  0

#define GPIO_FUNC127_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x032c)
/* GPIO_SIG127_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG127_IN_SEL  (BIT(7))
#define GPIO_SIG127_IN_SEL_M  (BIT(7))
#define GPIO_SIG127_IN_SEL_V  0x1
#define GPIO_SIG127_IN_SEL_S  7
/* GPIO_FUNC127_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC127_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC127_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC127_IN_INV_SEL_V  0x1
#define GPIO_FUNC127_IN_INV_SEL_S  6
/* GPIO_FUNC127_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC127_IN_SEL  0x0000003F
#define GPIO_FUNC127_IN_SEL_M  ((GPIO_FUNC127_IN_SEL_V)<<(GPIO_FUNC127_IN_SEL_S))
#define GPIO_FUNC127_IN_SEL_V  0x3F
#define GPIO_FUNC127_IN_SEL_S  0

#define GPIO_FUNC128_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x0330)
/* GPIO_SIG128_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG128_IN_SEL  (BIT(7))
#define GPIO_SIG128_IN_SEL_M  (BIT(7))
#define GPIO_SIG128_IN_SEL_V  0x1
#define GPIO_SIG128_IN_SEL_S  7
/* GPIO_FUNC128_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC128_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC128_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC128_IN_INV_SEL_V  0x1
#define GPIO_FUNC128_IN_INV_SEL_S  6
/* GPIO_FUNC128_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC128_IN_SEL  0x0000003F
#define GPIO_FUNC128_IN_SEL_M  ((GPIO_FUNC128_IN_SEL_V)<<(GPIO_FUNC128_IN_SEL_S))
#define GPIO_FUNC128_IN_SEL_V  0x3F
#define GPIO_FUNC128_IN_SEL_S  0

#define GPIO_FUNC129_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x0334)
/* GPIO_SIG129_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG129_IN_SEL  (BIT(7))
#define GPIO_SIG129_IN_SEL_M  (BIT(7))
#define GPIO_SIG129_IN_SEL_V  0x1
#define GPIO_SIG129_IN_SEL_S  7
/* GPIO_FUNC129_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC129_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC129_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC129_IN_INV_SEL_V  0x1
#define GPIO_FUNC129_IN_INV_SEL_S  6
/* GPIO_FUNC129_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC129_IN_SEL  0x0000003F
#define GPIO_FUNC129_IN_SEL_M  ((GPIO_FUNC129_IN_SEL_V)<<(GPIO_FUNC129_IN_SEL_S))
#define GPIO_FUNC129_IN_SEL_V  0x3F
#define GPIO_FUNC129_IN_SEL_S  0

#define GPIO_FUNC130_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x0338)
/* GPIO_SIG130_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG130_IN_SEL  (BIT(7))
#define GPIO_SIG130_IN_SEL_M  (BIT(7))
#define GPIO_SIG130_IN_SEL_V  0x1
#define GPIO_SIG130_IN_SEL_S  7
/* GPIO_FUNC130_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC130_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC130_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC130_IN_INV_SEL_V  0x1
#define GPIO_FUNC130_IN_INV_SEL_S  6
/* GPIO_FUNC130_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC130_IN_SEL  0x0000003F
#define GPIO_FUNC130_IN_SEL_M  ((GPIO_FUNC130_IN_SEL_V)<<(GPIO_FUNC130_IN_SEL_S))
#define GPIO_FUNC130_IN_SEL_V  0x3F
#define GPIO_FUNC130_IN_SEL_S  0

#define GPIO_FUNC131_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x033c)
/* GPIO_SIG131_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG131_IN_SEL  (BIT(7))
#define GPIO_SIG131_IN_SEL_M  (BIT(7))
#define GPIO_SIG131_IN_SEL_V  0x1
#define GPIO_SIG131_IN_SEL_S  7
/* GPIO_FUNC131_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC131_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC131_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC131_IN_INV_SEL_V  0x1
#define GPIO_FUNC131_IN_INV_SEL_S  6
/* GPIO_FUNC131_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC131_IN_SEL  0x0000003F
#define GPIO_FUNC131_IN_SEL_M  ((GPIO_FUNC131_IN_SEL_V)<<(GPIO_FUNC131_IN_SEL_S))
#define GPIO_FUNC131_IN_SEL_V  0x3F
#define GPIO_FUNC131_IN_SEL_S  0

#define GPIO_FUNC132_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x0340)
/* GPIO_SIG132_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG132_IN_SEL  (BIT(7))
#define GPIO_SIG132_IN_SEL_M  (BIT(7))
#define GPIO_SIG132_IN_SEL_V  0x1
#define GPIO_SIG132_IN_SEL_S  7
/* GPIO_FUNC132_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC132_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC132_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC132_IN_INV_SEL_V  0x1
#define GPIO_FUNC132_IN_INV_SEL_S  6
/* GPIO_FUNC132_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC132_IN_SEL  0x0000003F
#define GPIO_FUNC132_IN_SEL_M  ((GPIO_FUNC132_IN_SEL_V)<<(GPIO_FUNC132_IN_SEL_S))
#define GPIO_FUNC132_IN_SEL_V  0x3F
#define GPIO_FUNC132_IN_SEL_S  0

#define GPIO_FUNC133_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x0344)
/* GPIO_SIG133_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG133_IN_SEL  (BIT(7))
#define GPIO_SIG133_IN_SEL_M  (BIT(7))
#define GPIO_SIG133_IN_SEL_V  0x1
#define GPIO_SIG133_IN_SEL_S  7
/* GPIO_FUNC133_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC133_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC133_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC133_IN_INV_SEL_V  0x1
#define GPIO_FUNC133_IN_INV_SEL_S  6
/* GPIO_FUNC133_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC133_IN_SEL  0x0000003F
#define GPIO_FUNC133_IN_SEL_M  ((GPIO_FUNC133_IN_SEL_V)<<(GPIO_FUNC133_IN_SEL_S))
#define GPIO_FUNC133_IN_SEL_V  0x3F
#define GPIO_FUNC133_IN_SEL_S  0

#define GPIO_FUNC134_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x0348)
/* GPIO_SIG134_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG134_IN_SEL  (BIT(7))
#define GPIO_SIG134_IN_SEL_M  (BIT(7))
#define GPIO_SIG134_IN_SEL_V  0x1
#define GPIO_SIG134_IN_SEL_S  7
/* GPIO_FUNC134_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC134_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC134_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC134_IN_INV_SEL_V  0x1
#define GPIO_FUNC134_IN_INV_SEL_S  6
/* GPIO_FUNC134_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC134_IN_SEL  0x0000003F
#define GPIO_FUNC134_IN_SEL_M  ((GPIO_FUNC134_IN_SEL_V)<<(GPIO_FUNC134_IN_SEL_S))
#define GPIO_FUNC134_IN_SEL_V  0x3F
#define GPIO_FUNC134_IN_SEL_S  0

#define GPIO_FUNC135_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x034c)
/* GPIO_SIG135_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG135_IN_SEL  (BIT(7))
#define GPIO_SIG135_IN_SEL_M  (BIT(7))
#define GPIO_SIG135_IN_SEL_V  0x1
#define GPIO_SIG135_IN_SEL_S  7
/* GPIO_FUNC135_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC135_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC135_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC135_IN_INV_SEL_V  0x1
#define GPIO_FUNC135_IN_INV_SEL_S  6
/* GPIO_FUNC135_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC135_IN_SEL  0x0000003F
#define GPIO_FUNC135_IN_SEL_M  ((GPIO_FUNC135_IN_SEL_V)<<(GPIO_FUNC135_IN_SEL_S))
#define GPIO_FUNC135_IN_SEL_V  0x3F
#define GPIO_FUNC135_IN_SEL_S  0

#define GPIO_FUNC136_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x0350)
/* GPIO_SIG136_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG136_IN_SEL  (BIT(7))
#define GPIO_SIG136_IN_SEL_M  (BIT(7))
#define GPIO_SIG136_IN_SEL_V  0x1
#define GPIO_SIG136_IN_SEL_S  7
/* GPIO_FUNC136_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC136_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC136_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC136_IN_INV_SEL_V  0x1
#define GPIO_FUNC136_IN_INV_SEL_S  6
/* GPIO_FUNC136_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC136_IN_SEL  0x0000003F
#define GPIO_FUNC136_IN_SEL_M  ((GPIO_FUNC136_IN_SEL_V)<<(GPIO_FUNC136_IN_SEL_S))
#define GPIO_FUNC136_IN_SEL_V  0x3F
#define GPIO_FUNC136_IN_SEL_S  0

#define GPIO_FUNC137_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x0354)
/* GPIO_SIG137_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG137_IN_SEL  (BIT(7))
#define GPIO_SIG137_IN_SEL_M  (BIT(7))
#define GPIO_SIG137_IN_SEL_V  0x1
#define GPIO_SIG137_IN_SEL_S  7
/* GPIO_FUNC137_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC137_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC137_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC137_IN_INV_SEL_V  0x1
#define GPIO_FUNC137_IN_INV_SEL_S  6
/* GPIO_FUNC137_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC137_IN_SEL  0x0000003F
#define GPIO_FUNC137_IN_SEL_M  ((GPIO_FUNC137_IN_SEL_V)<<(GPIO_FUNC137_IN_SEL_S))
#define GPIO_FUNC137_IN_SEL_V  0x3F
#define GPIO_FUNC137_IN_SEL_S  0

#define GPIO_FUNC138_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x0358)
/* GPIO_SIG138_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG138_IN_SEL  (BIT(7))
#define GPIO_SIG138_IN_SEL_M  (BIT(7))
#define GPIO_SIG138_IN_SEL_V  0x1
#define GPIO_SIG138_IN_SEL_S  7
/* GPIO_FUNC138_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC138_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC138_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC138_IN_INV_SEL_V  0x1
#define GPIO_FUNC138_IN_INV_SEL_S  6
/* GPIO_FUNC138_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC138_IN_SEL  0x0000003F
#define GPIO_FUNC138_IN_SEL_M  ((GPIO_FUNC138_IN_SEL_V)<<(GPIO_FUNC138_IN_SEL_S))
#define GPIO_FUNC138_IN_SEL_V  0x3F
#define GPIO_FUNC138_IN_SEL_S  0

#define GPIO_FUNC139_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x035c)
/* GPIO_SIG139_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG139_IN_SEL  (BIT(7))
#define GPIO_SIG139_IN_SEL_M  (BIT(7))
#define GPIO_SIG139_IN_SEL_V  0x1
#define GPIO_SIG139_IN_SEL_S  7
/* GPIO_FUNC139_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC139_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC139_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC139_IN_INV_SEL_V  0x1
#define GPIO_FUNC139_IN_INV_SEL_S  6
/* GPIO_FUNC139_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC139_IN_SEL  0x0000003F
#define GPIO_FUNC139_IN_SEL_M  ((GPIO_FUNC139_IN_SEL_V)<<(GPIO_FUNC139_IN_SEL_S))
#define GPIO_FUNC139_IN_SEL_V  0x3F
#define GPIO_FUNC139_IN_SEL_S  0

#define GPIO_FUNC140_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x0360)
/* GPIO_SIG140_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG140_IN_SEL  (BIT(7))
#define GPIO_SIG140_IN_SEL_M  (BIT(7))
#define GPIO_SIG140_IN_SEL_V  0x1
#define GPIO_SIG140_IN_SEL_S  7
/* GPIO_FUNC140_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC140_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC140_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC140_IN_INV_SEL_V  0x1
#define GPIO_FUNC140_IN_INV_SEL_S  6
/* GPIO_FUNC140_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC140_IN_SEL  0x0000003F
#define GPIO_FUNC140_IN_SEL_M  ((GPIO_FUNC140_IN_SEL_V)<<(GPIO_FUNC140_IN_SEL_S))
#define GPIO_FUNC140_IN_SEL_V  0x3F
#define GPIO_FUNC140_IN_SEL_S  0

#define GPIO_FUNC141_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x0364)
/* GPIO_SIG141_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG141_IN_SEL  (BIT(7))
#define GPIO_SIG141_IN_SEL_M  (BIT(7))
#define GPIO_SIG141_IN_SEL_V  0x1
#define GPIO_SIG141_IN_SEL_S  7
/* GPIO_FUNC141_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC141_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC141_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC141_IN_INV_SEL_V  0x1
#define GPIO_FUNC141_IN_INV_SEL_S  6
/* GPIO_FUNC141_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC141_IN_SEL  0x0000003F
#define GPIO_FUNC141_IN_SEL_M  ((GPIO_FUNC141_IN_SEL_V)<<(GPIO_FUNC141_IN_SEL_S))
#define GPIO_FUNC141_IN_SEL_V  0x3F
#define GPIO_FUNC141_IN_SEL_S  0

#define GPIO_FUNC142_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x0368)
/* GPIO_SIG142_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG142_IN_SEL  (BIT(7))
#define GPIO_SIG142_IN_SEL_M  (BIT(7))
#define GPIO_SIG142_IN_SEL_V  0x1
#define GPIO_SIG142_IN_SEL_S  7
/* GPIO_FUNC142_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC142_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC142_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC142_IN_INV_SEL_V  0x1
#define GPIO_FUNC142_IN_INV_SEL_S  6
/* GPIO_FUNC142_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC142_IN_SEL  0x0000003F
#define GPIO_FUNC142_IN_SEL_M  ((GPIO_FUNC142_IN_SEL_V)<<(GPIO_FUNC142_IN_SEL_S))
#define GPIO_FUNC142_IN_SEL_V  0x3F
#define GPIO_FUNC142_IN_SEL_S  0

#define GPIO_FUNC143_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x036c)
/* GPIO_SIG143_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG143_IN_SEL  (BIT(7))
#define GPIO_SIG143_IN_SEL_M  (BIT(7))
#define GPIO_SIG143_IN_SEL_V  0x1
#define GPIO_SIG143_IN_SEL_S  7
/* GPIO_FUNC143_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC143_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC143_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC143_IN_INV_SEL_V  0x1
#define GPIO_FUNC143_IN_INV_SEL_S  6
/* GPIO_FUNC143_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC143_IN_SEL  0x0000003F
#define GPIO_FUNC143_IN_SEL_M  ((GPIO_FUNC143_IN_SEL_V)<<(GPIO_FUNC143_IN_SEL_S))
#define GPIO_FUNC143_IN_SEL_V  0x3F
#define GPIO_FUNC143_IN_SEL_S  0

#define GPIO_FUNC144_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x0370)
/* GPIO_SIG144_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG144_IN_SEL  (BIT(7))
#define GPIO_SIG144_IN_SEL_M  (BIT(7))
#define GPIO_SIG144_IN_SEL_V  0x1
#define GPIO_SIG144_IN_SEL_S  7
/* GPIO_FUNC144_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC144_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC144_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC144_IN_INV_SEL_V  0x1
#define GPIO_FUNC144_IN_INV_SEL_S  6
/* GPIO_FUNC144_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC144_IN_SEL  0x0000003F
#define GPIO_FUNC144_IN_SEL_M  ((GPIO_FUNC144_IN_SEL_V)<<(GPIO_FUNC144_IN_SEL_S))
#define GPIO_FUNC144_IN_SEL_V  0x3F
#define GPIO_FUNC144_IN_SEL_S  0

#define GPIO_FUNC145_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x0374)
/* GPIO_SIG145_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG145_IN_SEL  (BIT(7))
#define GPIO_SIG145_IN_SEL_M  (BIT(7))
#define GPIO_SIG145_IN_SEL_V  0x1
#define GPIO_SIG145_IN_SEL_S  7
/* GPIO_FUNC145_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC145_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC145_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC145_IN_INV_SEL_V  0x1
#define GPIO_FUNC145_IN_INV_SEL_S  6
/* GPIO_FUNC145_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC145_IN_SEL  0x0000003F
#define GPIO_FUNC145_IN_SEL_M  ((GPIO_FUNC145_IN_SEL_V)<<(GPIO_FUNC145_IN_SEL_S))
#define GPIO_FUNC145_IN_SEL_V  0x3F
#define GPIO_FUNC145_IN_SEL_S  0

#define GPIO_FUNC146_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x0378)
/* GPIO_SIG146_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG146_IN_SEL  (BIT(7))
#define GPIO_SIG146_IN_SEL_M  (BIT(7))
#define GPIO_SIG146_IN_SEL_V  0x1
#define GPIO_SIG146_IN_SEL_S  7
/* GPIO_FUNC146_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC146_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC146_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC146_IN_INV_SEL_V  0x1
#define GPIO_FUNC146_IN_INV_SEL_S  6
/* GPIO_FUNC146_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC146_IN_SEL  0x0000003F
#define GPIO_FUNC146_IN_SEL_M  ((GPIO_FUNC146_IN_SEL_V)<<(GPIO_FUNC146_IN_SEL_S))
#define GPIO_FUNC146_IN_SEL_V  0x3F
#define GPIO_FUNC146_IN_SEL_S  0

#define GPIO_FUNC147_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x037c)
/* GPIO_SIG147_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG147_IN_SEL  (BIT(7))
#define GPIO_SIG147_IN_SEL_M  (BIT(7))
#define GPIO_SIG147_IN_SEL_V  0x1
#define GPIO_SIG147_IN_SEL_S  7
/* GPIO_FUNC147_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC147_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC147_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC147_IN_INV_SEL_V  0x1
#define GPIO_FUNC147_IN_INV_SEL_S  6
/* GPIO_FUNC147_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC147_IN_SEL  0x0000003F
#define GPIO_FUNC147_IN_SEL_M  ((GPIO_FUNC147_IN_SEL_V)<<(GPIO_FUNC147_IN_SEL_S))
#define GPIO_FUNC147_IN_SEL_V  0x3F
#define GPIO_FUNC147_IN_SEL_S  0

#define GPIO_FUNC148_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x0380)
/* GPIO_SIG148_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG148_IN_SEL  (BIT(7))
#define GPIO_SIG148_IN_SEL_M  (BIT(7))
#define GPIO_SIG148_IN_SEL_V  0x1
#define GPIO_SIG148_IN_SEL_S  7
/* GPIO_FUNC148_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC148_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC148_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC148_IN_INV_SEL_V  0x1
#define GPIO_FUNC148_IN_INV_SEL_S  6
/* GPIO_FUNC148_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC148_IN_SEL  0x0000003F
#define GPIO_FUNC148_IN_SEL_M  ((GPIO_FUNC148_IN_SEL_V)<<(GPIO_FUNC148_IN_SEL_S))
#define GPIO_FUNC148_IN_SEL_V  0x3F
#define GPIO_FUNC148_IN_SEL_S  0

#define GPIO_FUNC149_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x0384)
/* GPIO_SIG149_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG149_IN_SEL  (BIT(7))
#define GPIO_SIG149_IN_SEL_M  (BIT(7))
#define GPIO_SIG149_IN_SEL_V  0x1
#define GPIO_SIG149_IN_SEL_S  7
/* GPIO_FUNC149_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC149_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC149_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC149_IN_INV_SEL_V  0x1
#define GPIO_FUNC149_IN_INV_SEL_S  6
/* GPIO_FUNC149_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC149_IN_SEL  0x0000003F
#define GPIO_FUNC149_IN_SEL_M  ((GPIO_FUNC149_IN_SEL_V)<<(GPIO_FUNC149_IN_SEL_S))
#define GPIO_FUNC149_IN_SEL_V  0x3F
#define GPIO_FUNC149_IN_SEL_S  0

#define GPIO_FUNC150_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x0388)
/* GPIO_SIG150_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG150_IN_SEL  (BIT(7))
#define GPIO_SIG150_IN_SEL_M  (BIT(7))
#define GPIO_SIG150_IN_SEL_V  0x1
#define GPIO_SIG150_IN_SEL_S  7
/* GPIO_FUNC150_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC150_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC150_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC150_IN_INV_SEL_V  0x1
#define GPIO_FUNC150_IN_INV_SEL_S  6
/* GPIO_FUNC150_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC150_IN_SEL  0x0000003F
#define GPIO_FUNC150_IN_SEL_M  ((GPIO_FUNC150_IN_SEL_V)<<(GPIO_FUNC150_IN_SEL_S))
#define GPIO_FUNC150_IN_SEL_V  0x3F
#define GPIO_FUNC150_IN_SEL_S  0

#define GPIO_FUNC151_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x038c)
/* GPIO_SIG151_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG151_IN_SEL  (BIT(7))
#define GPIO_SIG151_IN_SEL_M  (BIT(7))
#define GPIO_SIG151_IN_SEL_V  0x1
#define GPIO_SIG151_IN_SEL_S  7
/* GPIO_FUNC151_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC151_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC151_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC151_IN_INV_SEL_V  0x1
#define GPIO_FUNC151_IN_INV_SEL_S  6
/* GPIO_FUNC151_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC151_IN_SEL  0x0000003F
#define GPIO_FUNC151_IN_SEL_M  ((GPIO_FUNC151_IN_SEL_V)<<(GPIO_FUNC151_IN_SEL_S))
#define GPIO_FUNC151_IN_SEL_V  0x3F
#define GPIO_FUNC151_IN_SEL_S  0

#define GPIO_FUNC152_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x0390)
/* GPIO_SIG152_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG152_IN_SEL  (BIT(7))
#define GPIO_SIG152_IN_SEL_M  (BIT(7))
#define GPIO_SIG152_IN_SEL_V  0x1
#define GPIO_SIG152_IN_SEL_S  7
/* GPIO_FUNC152_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC152_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC152_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC152_IN_INV_SEL_V  0x1
#define GPIO_FUNC152_IN_INV_SEL_S  6
/* GPIO_FUNC152_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC152_IN_SEL  0x0000003F
#define GPIO_FUNC152_IN_SEL_M  ((GPIO_FUNC152_IN_SEL_V)<<(GPIO_FUNC152_IN_SEL_S))
#define GPIO_FUNC152_IN_SEL_V  0x3F
#define GPIO_FUNC152_IN_SEL_S  0

#define GPIO_FUNC153_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x0394)
/* GPIO_SIG153_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG153_IN_SEL  (BIT(7))
#define GPIO_SIG153_IN_SEL_M  (BIT(7))
#define GPIO_SIG153_IN_SEL_V  0x1
#define GPIO_SIG153_IN_SEL_S  7
/* GPIO_FUNC153_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC153_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC153_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC153_IN_INV_SEL_V  0x1
#define GPIO_FUNC153_IN_INV_SEL_S  6
/* GPIO_FUNC153_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC153_IN_SEL  0x0000003F
#define GPIO_FUNC153_IN_SEL_M  ((GPIO_FUNC153_IN_SEL_V)<<(GPIO_FUNC153_IN_SEL_S))
#define GPIO_FUNC153_IN_SEL_V  0x3F
#define GPIO_FUNC153_IN_SEL_S  0

#define GPIO_FUNC154_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x0398)
/* GPIO_SIG154_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG154_IN_SEL  (BIT(7))
#define GPIO_SIG154_IN_SEL_M  (BIT(7))
#define GPIO_SIG154_IN_SEL_V  0x1
#define GPIO_SIG154_IN_SEL_S  7
/* GPIO_FUNC154_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC154_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC154_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC154_IN_INV_SEL_V  0x1
#define GPIO_FUNC154_IN_INV_SEL_S  6
/* GPIO_FUNC154_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC154_IN_SEL  0x0000003F
#define GPIO_FUNC154_IN_SEL_M  ((GPIO_FUNC154_IN_SEL_V)<<(GPIO_FUNC154_IN_SEL_S))
#define GPIO_FUNC154_IN_SEL_V  0x3F
#define GPIO_FUNC154_IN_SEL_S  0

#define GPIO_FUNC155_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x039c)
/* GPIO_SIG155_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG155_IN_SEL  (BIT(7))
#define GPIO_SIG155_IN_SEL_M  (BIT(7))
#define GPIO_SIG155_IN_SEL_V  0x1
#define GPIO_SIG155_IN_SEL_S  7
/* GPIO_FUNC155_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC155_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC155_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC155_IN_INV_SEL_V  0x1
#define GPIO_FUNC155_IN_INV_SEL_S  6
/* GPIO_FUNC155_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC155_IN_SEL  0x0000003F
#define GPIO_FUNC155_IN_SEL_M  ((GPIO_FUNC155_IN_SEL_V)<<(GPIO_FUNC155_IN_SEL_S))
#define GPIO_FUNC155_IN_SEL_V  0x3F
#define GPIO_FUNC155_IN_SEL_S  0

#define GPIO_FUNC156_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x03a0)
/* GPIO_SIG156_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG156_IN_SEL  (BIT(7))
#define GPIO_SIG156_IN_SEL_M  (BIT(7))
#define GPIO_SIG156_IN_SEL_V  0x1
#define GPIO_SIG156_IN_SEL_S  7
/* GPIO_FUNC156_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC156_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC156_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC156_IN_INV_SEL_V  0x1
#define GPIO_FUNC156_IN_INV_SEL_S  6
/* GPIO_FUNC156_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC156_IN_SEL  0x0000003F
#define GPIO_FUNC156_IN_SEL_M  ((GPIO_FUNC156_IN_SEL_V)<<(GPIO_FUNC156_IN_SEL_S))
#define GPIO_FUNC156_IN_SEL_V  0x3F
#define GPIO_FUNC156_IN_SEL_S  0

#define GPIO_FUNC157_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x03a4)
/* GPIO_SIG157_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG157_IN_SEL  (BIT(7))
#define GPIO_SIG157_IN_SEL_M  (BIT(7))
#define GPIO_SIG157_IN_SEL_V  0x1
#define GPIO_SIG157_IN_SEL_S  7
/* GPIO_FUNC157_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC157_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC157_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC157_IN_INV_SEL_V  0x1
#define GPIO_FUNC157_IN_INV_SEL_S  6
/* GPIO_FUNC157_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC157_IN_SEL  0x0000003F
#define GPIO_FUNC157_IN_SEL_M  ((GPIO_FUNC157_IN_SEL_V)<<(GPIO_FUNC157_IN_SEL_S))
#define GPIO_FUNC157_IN_SEL_V  0x3F
#define GPIO_FUNC157_IN_SEL_S  0

#define GPIO_FUNC158_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x03a8)
/* GPIO_SIG158_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG158_IN_SEL  (BIT(7))
#define GPIO_SIG158_IN_SEL_M  (BIT(7))
#define GPIO_SIG158_IN_SEL_V  0x1
#define GPIO_SIG158_IN_SEL_S  7
/* GPIO_FUNC158_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC158_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC158_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC158_IN_INV_SEL_V  0x1
#define GPIO_FUNC158_IN_INV_SEL_S  6
/* GPIO_FUNC158_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC158_IN_SEL  0x0000003F
#define GPIO_FUNC158_IN_SEL_M  ((GPIO_FUNC158_IN_SEL_V)<<(GPIO_FUNC158_IN_SEL_S))
#define GPIO_FUNC158_IN_SEL_V  0x3F
#define GPIO_FUNC158_IN_SEL_S  0

#define GPIO_FUNC159_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x03ac)
/* GPIO_SIG159_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG159_IN_SEL  (BIT(7))
#define GPIO_SIG159_IN_SEL_M  (BIT(7))
#define GPIO_SIG159_IN_SEL_V  0x1
#define GPIO_SIG159_IN_SEL_S  7
/* GPIO_FUNC159_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC159_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC159_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC159_IN_INV_SEL_V  0x1
#define GPIO_FUNC159_IN_INV_SEL_S  6
/* GPIO_FUNC159_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC159_IN_SEL  0x0000003F
#define GPIO_FUNC159_IN_SEL_M  ((GPIO_FUNC159_IN_SEL_V)<<(GPIO_FUNC159_IN_SEL_S))
#define GPIO_FUNC159_IN_SEL_V  0x3F
#define GPIO_FUNC159_IN_SEL_S  0

#define GPIO_FUNC160_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x03b0)
/* GPIO_SIG160_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG160_IN_SEL  (BIT(7))
#define GPIO_SIG160_IN_SEL_M  (BIT(7))
#define GPIO_SIG160_IN_SEL_V  0x1
#define GPIO_SIG160_IN_SEL_S  7
/* GPIO_FUNC160_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC160_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC160_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC160_IN_INV_SEL_V  0x1
#define GPIO_FUNC160_IN_INV_SEL_S  6
/* GPIO_FUNC160_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC160_IN_SEL  0x0000003F
#define GPIO_FUNC160_IN_SEL_M  ((GPIO_FUNC160_IN_SEL_V)<<(GPIO_FUNC160_IN_SEL_S))
#define GPIO_FUNC160_IN_SEL_V  0x3F
#define GPIO_FUNC160_IN_SEL_S  0

#define GPIO_FUNC161_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x03b4)
/* GPIO_SIG161_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG161_IN_SEL  (BIT(7))
#define GPIO_SIG161_IN_SEL_M  (BIT(7))
#define GPIO_SIG161_IN_SEL_V  0x1
#define GPIO_SIG161_IN_SEL_S  7
/* GPIO_FUNC161_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC161_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC161_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC161_IN_INV_SEL_V  0x1
#define GPIO_FUNC161_IN_INV_SEL_S  6
/* GPIO_FUNC161_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC161_IN_SEL  0x0000003F
#define GPIO_FUNC161_IN_SEL_M  ((GPIO_FUNC161_IN_SEL_V)<<(GPIO_FUNC161_IN_SEL_S))
#define GPIO_FUNC161_IN_SEL_V  0x3F
#define GPIO_FUNC161_IN_SEL_S  0

#define GPIO_FUNC162_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x03b8)
/* GPIO_SIG162_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG162_IN_SEL  (BIT(7))
#define GPIO_SIG162_IN_SEL_M  (BIT(7))
#define GPIO_SIG162_IN_SEL_V  0x1
#define GPIO_SIG162_IN_SEL_S  7
/* GPIO_FUNC162_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC162_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC162_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC162_IN_INV_SEL_V  0x1
#define GPIO_FUNC162_IN_INV_SEL_S  6
/* GPIO_FUNC162_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC162_IN_SEL  0x0000003F
#define GPIO_FUNC162_IN_SEL_M  ((GPIO_FUNC162_IN_SEL_V)<<(GPIO_FUNC162_IN_SEL_S))
#define GPIO_FUNC162_IN_SEL_V  0x3F
#define GPIO_FUNC162_IN_SEL_S  0

#define GPIO_FUNC163_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x03bc)
/* GPIO_SIG163_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG163_IN_SEL  (BIT(7))
#define GPIO_SIG163_IN_SEL_M  (BIT(7))
#define GPIO_SIG163_IN_SEL_V  0x1
#define GPIO_SIG163_IN_SEL_S  7
/* GPIO_FUNC163_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC163_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC163_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC163_IN_INV_SEL_V  0x1
#define GPIO_FUNC163_IN_INV_SEL_S  6
/* GPIO_FUNC163_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC163_IN_SEL  0x0000003F
#define GPIO_FUNC163_IN_SEL_M  ((GPIO_FUNC163_IN_SEL_V)<<(GPIO_FUNC163_IN_SEL_S))
#define GPIO_FUNC163_IN_SEL_V  0x3F
#define GPIO_FUNC163_IN_SEL_S  0

#define GPIO_FUNC164_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x03c0)
/* GPIO_SIG164_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG164_IN_SEL  (BIT(7))
#define GPIO_SIG164_IN_SEL_M  (BIT(7))
#define GPIO_SIG164_IN_SEL_V  0x1
#define GPIO_SIG164_IN_SEL_S  7
/* GPIO_FUNC164_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC164_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC164_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC164_IN_INV_SEL_V  0x1
#define GPIO_FUNC164_IN_INV_SEL_S  6
/* GPIO_FUNC164_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC164_IN_SEL  0x0000003F
#define GPIO_FUNC164_IN_SEL_M  ((GPIO_FUNC164_IN_SEL_V)<<(GPIO_FUNC164_IN_SEL_S))
#define GPIO_FUNC164_IN_SEL_V  0x3F
#define GPIO_FUNC164_IN_SEL_S  0

#define GPIO_FUNC165_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x03c4)
/* GPIO_SIG165_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG165_IN_SEL  (BIT(7))
#define GPIO_SIG165_IN_SEL_M  (BIT(7))
#define GPIO_SIG165_IN_SEL_V  0x1
#define GPIO_SIG165_IN_SEL_S  7
/* GPIO_FUNC165_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC165_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC165_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC165_IN_INV_SEL_V  0x1
#define GPIO_FUNC165_IN_INV_SEL_S  6
/* GPIO_FUNC165_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC165_IN_SEL  0x0000003F
#define GPIO_FUNC165_IN_SEL_M  ((GPIO_FUNC165_IN_SEL_V)<<(GPIO_FUNC165_IN_SEL_S))
#define GPIO_FUNC165_IN_SEL_V  0x3F
#define GPIO_FUNC165_IN_SEL_S  0

#define GPIO_FUNC166_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x03c8)
/* GPIO_SIG166_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG166_IN_SEL  (BIT(7))
#define GPIO_SIG166_IN_SEL_M  (BIT(7))
#define GPIO_SIG166_IN_SEL_V  0x1
#define GPIO_SIG166_IN_SEL_S  7
/* GPIO_FUNC166_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC166_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC166_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC166_IN_INV_SEL_V  0x1
#define GPIO_FUNC166_IN_INV_SEL_S  6
/* GPIO_FUNC166_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC166_IN_SEL  0x0000003F
#define GPIO_FUNC166_IN_SEL_M  ((GPIO_FUNC166_IN_SEL_V)<<(GPIO_FUNC166_IN_SEL_S))
#define GPIO_FUNC166_IN_SEL_V  0x3F
#define GPIO_FUNC166_IN_SEL_S  0

#define GPIO_FUNC167_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x03cc)
/* GPIO_SIG167_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG167_IN_SEL  (BIT(7))
#define GPIO_SIG167_IN_SEL_M  (BIT(7))
#define GPIO_SIG167_IN_SEL_V  0x1
#define GPIO_SIG167_IN_SEL_S  7
/* GPIO_FUNC167_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC167_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC167_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC167_IN_INV_SEL_V  0x1
#define GPIO_FUNC167_IN_INV_SEL_S  6
/* GPIO_FUNC167_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC167_IN_SEL  0x0000003F
#define GPIO_FUNC167_IN_SEL_M  ((GPIO_FUNC167_IN_SEL_V)<<(GPIO_FUNC167_IN_SEL_S))
#define GPIO_FUNC167_IN_SEL_V  0x3F
#define GPIO_FUNC167_IN_SEL_S  0

#define GPIO_FUNC168_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x03d0)
/* GPIO_SIG168_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG168_IN_SEL  (BIT(7))
#define GPIO_SIG168_IN_SEL_M  (BIT(7))
#define GPIO_SIG168_IN_SEL_V  0x1
#define GPIO_SIG168_IN_SEL_S  7
/* GPIO_FUNC168_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC168_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC168_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC168_IN_INV_SEL_V  0x1
#define GPIO_FUNC168_IN_INV_SEL_S  6
/* GPIO_FUNC168_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC168_IN_SEL  0x0000003F
#define GPIO_FUNC168_IN_SEL_M  ((GPIO_FUNC168_IN_SEL_V)<<(GPIO_FUNC168_IN_SEL_S))
#define GPIO_FUNC168_IN_SEL_V  0x3F
#define GPIO_FUNC168_IN_SEL_S  0

#define GPIO_FUNC169_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x03d4)
/* GPIO_SIG169_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG169_IN_SEL  (BIT(7))
#define GPIO_SIG169_IN_SEL_M  (BIT(7))
#define GPIO_SIG169_IN_SEL_V  0x1
#define GPIO_SIG169_IN_SEL_S  7
/* GPIO_FUNC169_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC169_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC169_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC169_IN_INV_SEL_V  0x1
#define GPIO_FUNC169_IN_INV_SEL_S  6
/* GPIO_FUNC169_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC169_IN_SEL  0x0000003F
#define GPIO_FUNC169_IN_SEL_M  ((GPIO_FUNC169_IN_SEL_V)<<(GPIO_FUNC169_IN_SEL_S))
#define GPIO_FUNC169_IN_SEL_V  0x3F
#define GPIO_FUNC169_IN_SEL_S  0

#define GPIO_FUNC170_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x03d8)
/* GPIO_SIG170_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG170_IN_SEL  (BIT(7))
#define GPIO_SIG170_IN_SEL_M  (BIT(7))
#define GPIO_SIG170_IN_SEL_V  0x1
#define GPIO_SIG170_IN_SEL_S  7
/* GPIO_FUNC170_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC170_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC170_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC170_IN_INV_SEL_V  0x1
#define GPIO_FUNC170_IN_INV_SEL_S  6
/* GPIO_FUNC170_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC170_IN_SEL  0x0000003F
#define GPIO_FUNC170_IN_SEL_M  ((GPIO_FUNC170_IN_SEL_V)<<(GPIO_FUNC170_IN_SEL_S))
#define GPIO_FUNC170_IN_SEL_V  0x3F
#define GPIO_FUNC170_IN_SEL_S  0

#define GPIO_FUNC171_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x03dc)
/* GPIO_SIG171_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG171_IN_SEL  (BIT(7))
#define GPIO_SIG171_IN_SEL_M  (BIT(7))
#define GPIO_SIG171_IN_SEL_V  0x1
#define GPIO_SIG171_IN_SEL_S  7
/* GPIO_FUNC171_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC171_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC171_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC171_IN_INV_SEL_V  0x1
#define GPIO_FUNC171_IN_INV_SEL_S  6
/* GPIO_FUNC171_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC171_IN_SEL  0x0000003F
#define GPIO_FUNC171_IN_SEL_M  ((GPIO_FUNC171_IN_SEL_V)<<(GPIO_FUNC171_IN_SEL_S))
#define GPIO_FUNC171_IN_SEL_V  0x3F
#define GPIO_FUNC171_IN_SEL_S  0

#define GPIO_FUNC172_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x03e0)
/* GPIO_SIG172_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG172_IN_SEL  (BIT(7))
#define GPIO_SIG172_IN_SEL_M  (BIT(7))
#define GPIO_SIG172_IN_SEL_V  0x1
#define GPIO_SIG172_IN_SEL_S  7
/* GPIO_FUNC172_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC172_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC172_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC172_IN_INV_SEL_V  0x1
#define GPIO_FUNC172_IN_INV_SEL_S  6
/* GPIO_FUNC172_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC172_IN_SEL  0x0000003F
#define GPIO_FUNC172_IN_SEL_M  ((GPIO_FUNC172_IN_SEL_V)<<(GPIO_FUNC172_IN_SEL_S))
#define GPIO_FUNC172_IN_SEL_V  0x3F
#define GPIO_FUNC172_IN_SEL_S  0

#define GPIO_FUNC173_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x03e4)
/* GPIO_SIG173_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG173_IN_SEL  (BIT(7))
#define GPIO_SIG173_IN_SEL_M  (BIT(7))
#define GPIO_SIG173_IN_SEL_V  0x1
#define GPIO_SIG173_IN_SEL_S  7
/* GPIO_FUNC173_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC173_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC173_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC173_IN_INV_SEL_V  0x1
#define GPIO_FUNC173_IN_INV_SEL_S  6
/* GPIO_FUNC173_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC173_IN_SEL  0x0000003F
#define GPIO_FUNC173_IN_SEL_M  ((GPIO_FUNC173_IN_SEL_V)<<(GPIO_FUNC173_IN_SEL_S))
#define GPIO_FUNC173_IN_SEL_V  0x3F
#define GPIO_FUNC173_IN_SEL_S  0

#define GPIO_FUNC174_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x03e8)
/* GPIO_SIG174_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG174_IN_SEL  (BIT(7))
#define GPIO_SIG174_IN_SEL_M  (BIT(7))
#define GPIO_SIG174_IN_SEL_V  0x1
#define GPIO_SIG174_IN_SEL_S  7
/* GPIO_FUNC174_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC174_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC174_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC174_IN_INV_SEL_V  0x1
#define GPIO_FUNC174_IN_INV_SEL_S  6
/* GPIO_FUNC174_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC174_IN_SEL  0x0000003F
#define GPIO_FUNC174_IN_SEL_M  ((GPIO_FUNC174_IN_SEL_V)<<(GPIO_FUNC174_IN_SEL_S))
#define GPIO_FUNC174_IN_SEL_V  0x3F
#define GPIO_FUNC174_IN_SEL_S  0

#define GPIO_FUNC175_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x03ec)
/* GPIO_SIG175_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG175_IN_SEL  (BIT(7))
#define GPIO_SIG175_IN_SEL_M  (BIT(7))
#define GPIO_SIG175_IN_SEL_V  0x1
#define GPIO_SIG175_IN_SEL_S  7
/* GPIO_FUNC175_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC175_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC175_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC175_IN_INV_SEL_V  0x1
#define GPIO_FUNC175_IN_INV_SEL_S  6
/* GPIO_FUNC175_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC175_IN_SEL  0x0000003F
#define GPIO_FUNC175_IN_SEL_M  ((GPIO_FUNC175_IN_SEL_V)<<(GPIO_FUNC175_IN_SEL_S))
#define GPIO_FUNC175_IN_SEL_V  0x3F
#define GPIO_FUNC175_IN_SEL_S  0

#define GPIO_FUNC176_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x03f0)
/* GPIO_SIG176_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG176_IN_SEL  (BIT(7))
#define GPIO_SIG176_IN_SEL_M  (BIT(7))
#define GPIO_SIG176_IN_SEL_V  0x1
#define GPIO_SIG176_IN_SEL_S  7
/* GPIO_FUNC176_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC176_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC176_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC176_IN_INV_SEL_V  0x1
#define GPIO_FUNC176_IN_INV_SEL_S  6
/* GPIO_FUNC176_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC176_IN_SEL  0x0000003F
#define GPIO_FUNC176_IN_SEL_M  ((GPIO_FUNC176_IN_SEL_V)<<(GPIO_FUNC176_IN_SEL_S))
#define GPIO_FUNC176_IN_SEL_V  0x3F
#define GPIO_FUNC176_IN_SEL_S  0

#define GPIO_FUNC177_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x03f4)
/* GPIO_SIG177_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG177_IN_SEL  (BIT(7))
#define GPIO_SIG177_IN_SEL_M  (BIT(7))
#define GPIO_SIG177_IN_SEL_V  0x1
#define GPIO_SIG177_IN_SEL_S  7
/* GPIO_FUNC177_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC177_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC177_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC177_IN_INV_SEL_V  0x1
#define GPIO_FUNC177_IN_INV_SEL_S  6
/* GPIO_FUNC177_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC177_IN_SEL  0x0000003F
#define GPIO_FUNC177_IN_SEL_M  ((GPIO_FUNC177_IN_SEL_V)<<(GPIO_FUNC177_IN_SEL_S))
#define GPIO_FUNC177_IN_SEL_V  0x3F
#define GPIO_FUNC177_IN_SEL_S  0

#define GPIO_FUNC178_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x03f8)
/* GPIO_SIG178_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG178_IN_SEL  (BIT(7))
#define GPIO_SIG178_IN_SEL_M  (BIT(7))
#define GPIO_SIG178_IN_SEL_V  0x1
#define GPIO_SIG178_IN_SEL_S  7
/* GPIO_FUNC178_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC178_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC178_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC178_IN_INV_SEL_V  0x1
#define GPIO_FUNC178_IN_INV_SEL_S  6
/* GPIO_FUNC178_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC178_IN_SEL  0x0000003F
#define GPIO_FUNC178_IN_SEL_M  ((GPIO_FUNC178_IN_SEL_V)<<(GPIO_FUNC178_IN_SEL_S))
#define GPIO_FUNC178_IN_SEL_V  0x3F
#define GPIO_FUNC178_IN_SEL_S  0

#define GPIO_FUNC179_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x03fc)
/* GPIO_SIG179_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG179_IN_SEL  (BIT(7))
#define GPIO_SIG179_IN_SEL_M  (BIT(7))
#define GPIO_SIG179_IN_SEL_V  0x1
#define GPIO_SIG179_IN_SEL_S  7
/* GPIO_FUNC179_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC179_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC179_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC179_IN_INV_SEL_V  0x1
#define GPIO_FUNC179_IN_INV_SEL_S  6
/* GPIO_FUNC179_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC179_IN_SEL  0x0000003F
#define GPIO_FUNC179_IN_SEL_M  ((GPIO_FUNC179_IN_SEL_V)<<(GPIO_FUNC179_IN_SEL_S))
#define GPIO_FUNC179_IN_SEL_V  0x3F
#define GPIO_FUNC179_IN_SEL_S  0

#define GPIO_FUNC180_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x0400)
/* GPIO_SIG180_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG180_IN_SEL  (BIT(7))
#define GPIO_SIG180_IN_SEL_M  (BIT(7))
#define GPIO_SIG180_IN_SEL_V  0x1
#define GPIO_SIG180_IN_SEL_S  7
/* GPIO_FUNC180_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC180_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC180_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC180_IN_INV_SEL_V  0x1
#define GPIO_FUNC180_IN_INV_SEL_S  6
/* GPIO_FUNC180_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC180_IN_SEL  0x0000003F
#define GPIO_FUNC180_IN_SEL_M  ((GPIO_FUNC180_IN_SEL_V)<<(GPIO_FUNC180_IN_SEL_S))
#define GPIO_FUNC180_IN_SEL_V  0x3F
#define GPIO_FUNC180_IN_SEL_S  0

#define GPIO_FUNC181_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x0404)
/* GPIO_SIG181_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG181_IN_SEL  (BIT(7))
#define GPIO_SIG181_IN_SEL_M  (BIT(7))
#define GPIO_SIG181_IN_SEL_V  0x1
#define GPIO_SIG181_IN_SEL_S  7
/* GPIO_FUNC181_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC181_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC181_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC181_IN_INV_SEL_V  0x1
#define GPIO_FUNC181_IN_INV_SEL_S  6
/* GPIO_FUNC181_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC181_IN_SEL  0x0000003F
#define GPIO_FUNC181_IN_SEL_M  ((GPIO_FUNC181_IN_SEL_V)<<(GPIO_FUNC181_IN_SEL_S))
#define GPIO_FUNC181_IN_SEL_V  0x3F
#define GPIO_FUNC181_IN_SEL_S  0

#define GPIO_FUNC182_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x0408)
/* GPIO_SIG182_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG182_IN_SEL  (BIT(7))
#define GPIO_SIG182_IN_SEL_M  (BIT(7))
#define GPIO_SIG182_IN_SEL_V  0x1
#define GPIO_SIG182_IN_SEL_S  7
/* GPIO_FUNC182_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC182_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC182_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC182_IN_INV_SEL_V  0x1
#define GPIO_FUNC182_IN_INV_SEL_S  6
/* GPIO_FUNC182_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC182_IN_SEL  0x0000003F
#define GPIO_FUNC182_IN_SEL_M  ((GPIO_FUNC182_IN_SEL_V)<<(GPIO_FUNC182_IN_SEL_S))
#define GPIO_FUNC182_IN_SEL_V  0x3F
#define GPIO_FUNC182_IN_SEL_S  0

#define GPIO_FUNC183_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x040c)
/* GPIO_SIG183_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG183_IN_SEL  (BIT(7))
#define GPIO_SIG183_IN_SEL_M  (BIT(7))
#define GPIO_SIG183_IN_SEL_V  0x1
#define GPIO_SIG183_IN_SEL_S  7
/* GPIO_FUNC183_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC183_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC183_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC183_IN_INV_SEL_V  0x1
#define GPIO_FUNC183_IN_INV_SEL_S  6
/* GPIO_FUNC183_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC183_IN_SEL  0x0000003F
#define GPIO_FUNC183_IN_SEL_M  ((GPIO_FUNC183_IN_SEL_V)<<(GPIO_FUNC183_IN_SEL_S))
#define GPIO_FUNC183_IN_SEL_V  0x3F
#define GPIO_FUNC183_IN_SEL_S  0

#define GPIO_FUNC184_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x0410)
/* GPIO_SIG184_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG184_IN_SEL  (BIT(7))
#define GPIO_SIG184_IN_SEL_M  (BIT(7))
#define GPIO_SIG184_IN_SEL_V  0x1
#define GPIO_SIG184_IN_SEL_S  7
/* GPIO_FUNC184_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC184_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC184_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC184_IN_INV_SEL_V  0x1
#define GPIO_FUNC184_IN_INV_SEL_S  6
/* GPIO_FUNC184_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC184_IN_SEL  0x0000003F
#define GPIO_FUNC184_IN_SEL_M  ((GPIO_FUNC184_IN_SEL_V)<<(GPIO_FUNC184_IN_SEL_S))
#define GPIO_FUNC184_IN_SEL_V  0x3F
#define GPIO_FUNC184_IN_SEL_S  0

#define GPIO_FUNC185_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x0414)
/* GPIO_SIG185_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG185_IN_SEL  (BIT(7))
#define GPIO_SIG185_IN_SEL_M  (BIT(7))
#define GPIO_SIG185_IN_SEL_V  0x1
#define GPIO_SIG185_IN_SEL_S  7
/* GPIO_FUNC185_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC185_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC185_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC185_IN_INV_SEL_V  0x1
#define GPIO_FUNC185_IN_INV_SEL_S  6
/* GPIO_FUNC185_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC185_IN_SEL  0x0000003F
#define GPIO_FUNC185_IN_SEL_M  ((GPIO_FUNC185_IN_SEL_V)<<(GPIO_FUNC185_IN_SEL_S))
#define GPIO_FUNC185_IN_SEL_V  0x3F
#define GPIO_FUNC185_IN_SEL_S  0

#define GPIO_FUNC186_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x0418)
/* GPIO_SIG186_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG186_IN_SEL  (BIT(7))
#define GPIO_SIG186_IN_SEL_M  (BIT(7))
#define GPIO_SIG186_IN_SEL_V  0x1
#define GPIO_SIG186_IN_SEL_S  7
/* GPIO_FUNC186_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC186_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC186_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC186_IN_INV_SEL_V  0x1
#define GPIO_FUNC186_IN_INV_SEL_S  6
/* GPIO_FUNC186_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC186_IN_SEL  0x0000003F
#define GPIO_FUNC186_IN_SEL_M  ((GPIO_FUNC186_IN_SEL_V)<<(GPIO_FUNC186_IN_SEL_S))
#define GPIO_FUNC186_IN_SEL_V  0x3F
#define GPIO_FUNC186_IN_SEL_S  0

#define GPIO_FUNC187_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x041c)
/* GPIO_SIG187_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG187_IN_SEL  (BIT(7))
#define GPIO_SIG187_IN_SEL_M  (BIT(7))
#define GPIO_SIG187_IN_SEL_V  0x1
#define GPIO_SIG187_IN_SEL_S  7
/* GPIO_FUNC187_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC187_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC187_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC187_IN_INV_SEL_V  0x1
#define GPIO_FUNC187_IN_INV_SEL_S  6
/* GPIO_FUNC187_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC187_IN_SEL  0x0000003F
#define GPIO_FUNC187_IN_SEL_M  ((GPIO_FUNC187_IN_SEL_V)<<(GPIO_FUNC187_IN_SEL_S))
#define GPIO_FUNC187_IN_SEL_V  0x3F
#define GPIO_FUNC187_IN_SEL_S  0

#define GPIO_FUNC188_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x0420)
/* GPIO_SIG188_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG188_IN_SEL  (BIT(7))
#define GPIO_SIG188_IN_SEL_M  (BIT(7))
#define GPIO_SIG188_IN_SEL_V  0x1
#define GPIO_SIG188_IN_SEL_S  7
/* GPIO_FUNC188_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC188_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC188_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC188_IN_INV_SEL_V  0x1
#define GPIO_FUNC188_IN_INV_SEL_S  6
/* GPIO_FUNC188_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC188_IN_SEL  0x0000003F
#define GPIO_FUNC188_IN_SEL_M  ((GPIO_FUNC188_IN_SEL_V)<<(GPIO_FUNC188_IN_SEL_S))
#define GPIO_FUNC188_IN_SEL_V  0x3F
#define GPIO_FUNC188_IN_SEL_S  0

#define GPIO_FUNC189_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x0424)
/* GPIO_SIG189_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG189_IN_SEL  (BIT(7))
#define GPIO_SIG189_IN_SEL_M  (BIT(7))
#define GPIO_SIG189_IN_SEL_V  0x1
#define GPIO_SIG189_IN_SEL_S  7
/* GPIO_FUNC189_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC189_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC189_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC189_IN_INV_SEL_V  0x1
#define GPIO_FUNC189_IN_INV_SEL_S  6
/* GPIO_FUNC189_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC189_IN_SEL  0x0000003F
#define GPIO_FUNC189_IN_SEL_M  ((GPIO_FUNC189_IN_SEL_V)<<(GPIO_FUNC189_IN_SEL_S))
#define GPIO_FUNC189_IN_SEL_V  0x3F
#define GPIO_FUNC189_IN_SEL_S  0

#define GPIO_FUNC190_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x0428)
/* GPIO_SIG190_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG190_IN_SEL  (BIT(7))
#define GPIO_SIG190_IN_SEL_M  (BIT(7))
#define GPIO_SIG190_IN_SEL_V  0x1
#define GPIO_SIG190_IN_SEL_S  7
/* GPIO_FUNC190_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC190_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC190_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC190_IN_INV_SEL_V  0x1
#define GPIO_FUNC190_IN_INV_SEL_S  6
/* GPIO_FUNC190_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC190_IN_SEL  0x0000003F
#define GPIO_FUNC190_IN_SEL_M  ((GPIO_FUNC190_IN_SEL_V)<<(GPIO_FUNC190_IN_SEL_S))
#define GPIO_FUNC190_IN_SEL_V  0x3F
#define GPIO_FUNC190_IN_SEL_S  0

#define GPIO_FUNC191_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x042c)
/* GPIO_SIG191_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG191_IN_SEL  (BIT(7))
#define GPIO_SIG191_IN_SEL_M  (BIT(7))
#define GPIO_SIG191_IN_SEL_V  0x1
#define GPIO_SIG191_IN_SEL_S  7
/* GPIO_FUNC191_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC191_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC191_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC191_IN_INV_SEL_V  0x1
#define GPIO_FUNC191_IN_INV_SEL_S  6
/* GPIO_FUNC191_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC191_IN_SEL  0x0000003F
#define GPIO_FUNC191_IN_SEL_M  ((GPIO_FUNC191_IN_SEL_V)<<(GPIO_FUNC191_IN_SEL_S))
#define GPIO_FUNC191_IN_SEL_V  0x3F
#define GPIO_FUNC191_IN_SEL_S  0

#define GPIO_FUNC192_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x0430)
/* GPIO_SIG192_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG192_IN_SEL  (BIT(7))
#define GPIO_SIG192_IN_SEL_M  (BIT(7))
#define GPIO_SIG192_IN_SEL_V  0x1
#define GPIO_SIG192_IN_SEL_S  7
/* GPIO_FUNC192_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC192_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC192_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC192_IN_INV_SEL_V  0x1
#define GPIO_FUNC192_IN_INV_SEL_S  6
/* GPIO_FUNC192_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC192_IN_SEL  0x0000003F
#define GPIO_FUNC192_IN_SEL_M  ((GPIO_FUNC192_IN_SEL_V)<<(GPIO_FUNC192_IN_SEL_S))
#define GPIO_FUNC192_IN_SEL_V  0x3F
#define GPIO_FUNC192_IN_SEL_S  0

#define GPIO_FUNC193_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x0434)
/* GPIO_SIG193_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG193_IN_SEL  (BIT(7))
#define GPIO_SIG193_IN_SEL_M  (BIT(7))
#define GPIO_SIG193_IN_SEL_V  0x1
#define GPIO_SIG193_IN_SEL_S  7
/* GPIO_FUNC193_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC193_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC193_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC193_IN_INV_SEL_V  0x1
#define GPIO_FUNC193_IN_INV_SEL_S  6
/* GPIO_FUNC193_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC193_IN_SEL  0x0000003F
#define GPIO_FUNC193_IN_SEL_M  ((GPIO_FUNC193_IN_SEL_V)<<(GPIO_FUNC193_IN_SEL_S))
#define GPIO_FUNC193_IN_SEL_V  0x3F
#define GPIO_FUNC193_IN_SEL_S  0

#define GPIO_FUNC194_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x0438)
/* GPIO_SIG194_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG194_IN_SEL  (BIT(7))
#define GPIO_SIG194_IN_SEL_M  (BIT(7))
#define GPIO_SIG194_IN_SEL_V  0x1
#define GPIO_SIG194_IN_SEL_S  7
/* GPIO_FUNC194_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC194_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC194_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC194_IN_INV_SEL_V  0x1
#define GPIO_FUNC194_IN_INV_SEL_S  6
/* GPIO_FUNC194_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC194_IN_SEL  0x0000003F
#define GPIO_FUNC194_IN_SEL_M  ((GPIO_FUNC194_IN_SEL_V)<<(GPIO_FUNC194_IN_SEL_S))
#define GPIO_FUNC194_IN_SEL_V  0x3F
#define GPIO_FUNC194_IN_SEL_S  0

#define GPIO_FUNC195_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x043c)
/* GPIO_SIG195_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG195_IN_SEL  (BIT(7))
#define GPIO_SIG195_IN_SEL_M  (BIT(7))
#define GPIO_SIG195_IN_SEL_V  0x1
#define GPIO_SIG195_IN_SEL_S  7
/* GPIO_FUNC195_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC195_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC195_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC195_IN_INV_SEL_V  0x1
#define GPIO_FUNC195_IN_INV_SEL_S  6
/* GPIO_FUNC195_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC195_IN_SEL  0x0000003F
#define GPIO_FUNC195_IN_SEL_M  ((GPIO_FUNC195_IN_SEL_V)<<(GPIO_FUNC195_IN_SEL_S))
#define GPIO_FUNC195_IN_SEL_V  0x3F
#define GPIO_FUNC195_IN_SEL_S  0

#define GPIO_FUNC196_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x0440)
/* GPIO_SIG196_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG196_IN_SEL  (BIT(7))
#define GPIO_SIG196_IN_SEL_M  (BIT(7))
#define GPIO_SIG196_IN_SEL_V  0x1
#define GPIO_SIG196_IN_SEL_S  7
/* GPIO_FUNC196_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC196_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC196_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC196_IN_INV_SEL_V  0x1
#define GPIO_FUNC196_IN_INV_SEL_S  6
/* GPIO_FUNC196_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC196_IN_SEL  0x0000003F
#define GPIO_FUNC196_IN_SEL_M  ((GPIO_FUNC196_IN_SEL_V)<<(GPIO_FUNC196_IN_SEL_S))
#define GPIO_FUNC196_IN_SEL_V  0x3F
#define GPIO_FUNC196_IN_SEL_S  0

#define GPIO_FUNC197_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x0444)
/* GPIO_SIG197_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG197_IN_SEL  (BIT(7))
#define GPIO_SIG197_IN_SEL_M  (BIT(7))
#define GPIO_SIG197_IN_SEL_V  0x1
#define GPIO_SIG197_IN_SEL_S  7
/* GPIO_FUNC197_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC197_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC197_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC197_IN_INV_SEL_V  0x1
#define GPIO_FUNC197_IN_INV_SEL_S  6
/* GPIO_FUNC197_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC197_IN_SEL  0x0000003F
#define GPIO_FUNC197_IN_SEL_M  ((GPIO_FUNC197_IN_SEL_V)<<(GPIO_FUNC197_IN_SEL_S))
#define GPIO_FUNC197_IN_SEL_V  0x3F
#define GPIO_FUNC197_IN_SEL_S  0

#define GPIO_FUNC198_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x0448)
/* GPIO_SIG198_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG198_IN_SEL  (BIT(7))
#define GPIO_SIG198_IN_SEL_M  (BIT(7))
#define GPIO_SIG198_IN_SEL_V  0x1
#define GPIO_SIG198_IN_SEL_S  7
/* GPIO_FUNC198_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC198_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC198_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC198_IN_INV_SEL_V  0x1
#define GPIO_FUNC198_IN_INV_SEL_S  6
/* GPIO_FUNC198_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC198_IN_SEL  0x0000003F
#define GPIO_FUNC198_IN_SEL_M  ((GPIO_FUNC198_IN_SEL_V)<<(GPIO_FUNC198_IN_SEL_S))
#define GPIO_FUNC198_IN_SEL_V  0x3F
#define GPIO_FUNC198_IN_SEL_S  0

#define GPIO_FUNC199_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x044c)
/* GPIO_SIG199_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG199_IN_SEL  (BIT(7))
#define GPIO_SIG199_IN_SEL_M  (BIT(7))
#define GPIO_SIG199_IN_SEL_V  0x1
#define GPIO_SIG199_IN_SEL_S  7
/* GPIO_FUNC199_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC199_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC199_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC199_IN_INV_SEL_V  0x1
#define GPIO_FUNC199_IN_INV_SEL_S  6
/* GPIO_FUNC199_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC199_IN_SEL  0x0000003F
#define GPIO_FUNC199_IN_SEL_M  ((GPIO_FUNC199_IN_SEL_V)<<(GPIO_FUNC199_IN_SEL_S))
#define GPIO_FUNC199_IN_SEL_V  0x3F
#define GPIO_FUNC199_IN_SEL_S  0

#define GPIO_FUNC200_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x0450)
/* GPIO_SIG200_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG200_IN_SEL  (BIT(7))
#define GPIO_SIG200_IN_SEL_M  (BIT(7))
#define GPIO_SIG200_IN_SEL_V  0x1
#define GPIO_SIG200_IN_SEL_S  7
/* GPIO_FUNC200_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC200_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC200_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC200_IN_INV_SEL_V  0x1
#define GPIO_FUNC200_IN_INV_SEL_S  6
/* GPIO_FUNC200_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC200_IN_SEL  0x0000003F
#define GPIO_FUNC200_IN_SEL_M  ((GPIO_FUNC200_IN_SEL_V)<<(GPIO_FUNC200_IN_SEL_S))
#define GPIO_FUNC200_IN_SEL_V  0x3F
#define GPIO_FUNC200_IN_SEL_S  0

#define GPIO_FUNC201_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x0454)
/* GPIO_SIG201_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG201_IN_SEL  (BIT(7))
#define GPIO_SIG201_IN_SEL_M  (BIT(7))
#define GPIO_SIG201_IN_SEL_V  0x1
#define GPIO_SIG201_IN_SEL_S  7
/* GPIO_FUNC201_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC201_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC201_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC201_IN_INV_SEL_V  0x1
#define GPIO_FUNC201_IN_INV_SEL_S  6
/* GPIO_FUNC201_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC201_IN_SEL  0x0000003F
#define GPIO_FUNC201_IN_SEL_M  ((GPIO_FUNC201_IN_SEL_V)<<(GPIO_FUNC201_IN_SEL_S))
#define GPIO_FUNC201_IN_SEL_V  0x3F
#define GPIO_FUNC201_IN_SEL_S  0

#define GPIO_FUNC202_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x0458)
/* GPIO_SIG202_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG202_IN_SEL  (BIT(7))
#define GPIO_SIG202_IN_SEL_M  (BIT(7))
#define GPIO_SIG202_IN_SEL_V  0x1
#define GPIO_SIG202_IN_SEL_S  7
/* GPIO_FUNC202_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC202_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC202_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC202_IN_INV_SEL_V  0x1
#define GPIO_FUNC202_IN_INV_SEL_S  6
/* GPIO_FUNC202_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC202_IN_SEL  0x0000003F
#define GPIO_FUNC202_IN_SEL_M  ((GPIO_FUNC202_IN_SEL_V)<<(GPIO_FUNC202_IN_SEL_S))
#define GPIO_FUNC202_IN_SEL_V  0x3F
#define GPIO_FUNC202_IN_SEL_S  0

#define GPIO_FUNC203_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x045c)
/* GPIO_SIG203_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG203_IN_SEL  (BIT(7))
#define GPIO_SIG203_IN_SEL_M  (BIT(7))
#define GPIO_SIG203_IN_SEL_V  0x1
#define GPIO_SIG203_IN_SEL_S  7
/* GPIO_FUNC203_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC203_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC203_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC203_IN_INV_SEL_V  0x1
#define GPIO_FUNC203_IN_INV_SEL_S  6
/* GPIO_FUNC203_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC203_IN_SEL  0x0000003F
#define GPIO_FUNC203_IN_SEL_M  ((GPIO_FUNC203_IN_SEL_V)<<(GPIO_FUNC203_IN_SEL_S))
#define GPIO_FUNC203_IN_SEL_V  0x3F
#define GPIO_FUNC203_IN_SEL_S  0

#define GPIO_FUNC204_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x0460)
/* GPIO_SIG204_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG204_IN_SEL  (BIT(7))
#define GPIO_SIG204_IN_SEL_M  (BIT(7))
#define GPIO_SIG204_IN_SEL_V  0x1
#define GPIO_SIG204_IN_SEL_S  7
/* GPIO_FUNC204_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC204_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC204_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC204_IN_INV_SEL_V  0x1
#define GPIO_FUNC204_IN_INV_SEL_S  6
/* GPIO_FUNC204_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC204_IN_SEL  0x0000003F
#define GPIO_FUNC204_IN_SEL_M  ((GPIO_FUNC204_IN_SEL_V)<<(GPIO_FUNC204_IN_SEL_S))
#define GPIO_FUNC204_IN_SEL_V  0x3F
#define GPIO_FUNC204_IN_SEL_S  0

#define GPIO_FUNC205_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x0464)
/* GPIO_SIG205_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG205_IN_SEL  (BIT(7))
#define GPIO_SIG205_IN_SEL_M  (BIT(7))
#define GPIO_SIG205_IN_SEL_V  0x1
#define GPIO_SIG205_IN_SEL_S  7
/* GPIO_FUNC205_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC205_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC205_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC205_IN_INV_SEL_V  0x1
#define GPIO_FUNC205_IN_INV_SEL_S  6
/* GPIO_FUNC205_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC205_IN_SEL  0x0000003F
#define GPIO_FUNC205_IN_SEL_M  ((GPIO_FUNC205_IN_SEL_V)<<(GPIO_FUNC205_IN_SEL_S))
#define GPIO_FUNC205_IN_SEL_V  0x3F
#define GPIO_FUNC205_IN_SEL_S  0

#define GPIO_FUNC206_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x0468)
/* GPIO_SIG206_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG206_IN_SEL  (BIT(7))
#define GPIO_SIG206_IN_SEL_M  (BIT(7))
#define GPIO_SIG206_IN_SEL_V  0x1
#define GPIO_SIG206_IN_SEL_S  7
/* GPIO_FUNC206_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC206_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC206_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC206_IN_INV_SEL_V  0x1
#define GPIO_FUNC206_IN_INV_SEL_S  6
/* GPIO_FUNC206_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC206_IN_SEL  0x0000003F
#define GPIO_FUNC206_IN_SEL_M  ((GPIO_FUNC206_IN_SEL_V)<<(GPIO_FUNC206_IN_SEL_S))
#define GPIO_FUNC206_IN_SEL_V  0x3F
#define GPIO_FUNC206_IN_SEL_S  0

#define GPIO_FUNC207_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x046c)
/* GPIO_SIG207_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG207_IN_SEL  (BIT(7))
#define GPIO_SIG207_IN_SEL_M  (BIT(7))
#define GPIO_SIG207_IN_SEL_V  0x1
#define GPIO_SIG207_IN_SEL_S  7
/* GPIO_FUNC207_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC207_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC207_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC207_IN_INV_SEL_V  0x1
#define GPIO_FUNC207_IN_INV_SEL_S  6
/* GPIO_FUNC207_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC207_IN_SEL  0x0000003F
#define GPIO_FUNC207_IN_SEL_M  ((GPIO_FUNC207_IN_SEL_V)<<(GPIO_FUNC207_IN_SEL_S))
#define GPIO_FUNC207_IN_SEL_V  0x3F
#define GPIO_FUNC207_IN_SEL_S  0

#define GPIO_FUNC208_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x0470)
/* GPIO_SIG208_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG208_IN_SEL  (BIT(7))
#define GPIO_SIG208_IN_SEL_M  (BIT(7))
#define GPIO_SIG208_IN_SEL_V  0x1
#define GPIO_SIG208_IN_SEL_S  7
/* GPIO_FUNC208_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC208_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC208_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC208_IN_INV_SEL_V  0x1
#define GPIO_FUNC208_IN_INV_SEL_S  6
/* GPIO_FUNC208_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC208_IN_SEL  0x0000003F
#define GPIO_FUNC208_IN_SEL_M  ((GPIO_FUNC208_IN_SEL_V)<<(GPIO_FUNC208_IN_SEL_S))
#define GPIO_FUNC208_IN_SEL_V  0x3F
#define GPIO_FUNC208_IN_SEL_S  0

#define GPIO_FUNC209_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x0474)
/* GPIO_SIG209_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG209_IN_SEL  (BIT(7))
#define GPIO_SIG209_IN_SEL_M  (BIT(7))
#define GPIO_SIG209_IN_SEL_V  0x1
#define GPIO_SIG209_IN_SEL_S  7
/* GPIO_FUNC209_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC209_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC209_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC209_IN_INV_SEL_V  0x1
#define GPIO_FUNC209_IN_INV_SEL_S  6
/* GPIO_FUNC209_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC209_IN_SEL  0x0000003F
#define GPIO_FUNC209_IN_SEL_M  ((GPIO_FUNC209_IN_SEL_V)<<(GPIO_FUNC209_IN_SEL_S))
#define GPIO_FUNC209_IN_SEL_V  0x3F
#define GPIO_FUNC209_IN_SEL_S  0

#define GPIO_FUNC210_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x0478)
/* GPIO_SIG210_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG210_IN_SEL  (BIT(7))
#define GPIO_SIG210_IN_SEL_M  (BIT(7))
#define GPIO_SIG210_IN_SEL_V  0x1
#define GPIO_SIG210_IN_SEL_S  7
/* GPIO_FUNC210_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC210_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC210_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC210_IN_INV_SEL_V  0x1
#define GPIO_FUNC210_IN_INV_SEL_S  6
/* GPIO_FUNC210_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC210_IN_SEL  0x0000003F
#define GPIO_FUNC210_IN_SEL_M  ((GPIO_FUNC210_IN_SEL_V)<<(GPIO_FUNC210_IN_SEL_S))
#define GPIO_FUNC210_IN_SEL_V  0x3F
#define GPIO_FUNC210_IN_SEL_S  0

#define GPIO_FUNC211_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x047c)
/* GPIO_SIG211_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG211_IN_SEL  (BIT(7))
#define GPIO_SIG211_IN_SEL_M  (BIT(7))
#define GPIO_SIG211_IN_SEL_V  0x1
#define GPIO_SIG211_IN_SEL_S  7
/* GPIO_FUNC211_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC211_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC211_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC211_IN_INV_SEL_V  0x1
#define GPIO_FUNC211_IN_INV_SEL_S  6
/* GPIO_FUNC211_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC211_IN_SEL  0x0000003F
#define GPIO_FUNC211_IN_SEL_M  ((GPIO_FUNC211_IN_SEL_V)<<(GPIO_FUNC211_IN_SEL_S))
#define GPIO_FUNC211_IN_SEL_V  0x3F
#define GPIO_FUNC211_IN_SEL_S  0

#define GPIO_FUNC212_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x0480)
/* GPIO_SIG212_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG212_IN_SEL  (BIT(7))
#define GPIO_SIG212_IN_SEL_M  (BIT(7))
#define GPIO_SIG212_IN_SEL_V  0x1
#define GPIO_SIG212_IN_SEL_S  7
/* GPIO_FUNC212_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC212_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC212_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC212_IN_INV_SEL_V  0x1
#define GPIO_FUNC212_IN_INV_SEL_S  6
/* GPIO_FUNC212_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC212_IN_SEL  0x0000003F
#define GPIO_FUNC212_IN_SEL_M  ((GPIO_FUNC212_IN_SEL_V)<<(GPIO_FUNC212_IN_SEL_S))
#define GPIO_FUNC212_IN_SEL_V  0x3F
#define GPIO_FUNC212_IN_SEL_S  0

#define GPIO_FUNC213_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x0484)
/* GPIO_SIG213_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG213_IN_SEL  (BIT(7))
#define GPIO_SIG213_IN_SEL_M  (BIT(7))
#define GPIO_SIG213_IN_SEL_V  0x1
#define GPIO_SIG213_IN_SEL_S  7
/* GPIO_FUNC213_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC213_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC213_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC213_IN_INV_SEL_V  0x1
#define GPIO_FUNC213_IN_INV_SEL_S  6
/* GPIO_FUNC213_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC213_IN_SEL  0x0000003F
#define GPIO_FUNC213_IN_SEL_M  ((GPIO_FUNC213_IN_SEL_V)<<(GPIO_FUNC213_IN_SEL_S))
#define GPIO_FUNC213_IN_SEL_V  0x3F
#define GPIO_FUNC213_IN_SEL_S  0

#define GPIO_FUNC214_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x0488)
/* GPIO_SIG214_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG214_IN_SEL  (BIT(7))
#define GPIO_SIG214_IN_SEL_M  (BIT(7))
#define GPIO_SIG214_IN_SEL_V  0x1
#define GPIO_SIG214_IN_SEL_S  7
/* GPIO_FUNC214_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC214_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC214_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC214_IN_INV_SEL_V  0x1
#define GPIO_FUNC214_IN_INV_SEL_S  6
/* GPIO_FUNC214_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC214_IN_SEL  0x0000003F
#define GPIO_FUNC214_IN_SEL_M  ((GPIO_FUNC214_IN_SEL_V)<<(GPIO_FUNC214_IN_SEL_S))
#define GPIO_FUNC214_IN_SEL_V  0x3F
#define GPIO_FUNC214_IN_SEL_S  0

#define GPIO_FUNC215_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x048c)
/* GPIO_SIG215_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG215_IN_SEL  (BIT(7))
#define GPIO_SIG215_IN_SEL_M  (BIT(7))
#define GPIO_SIG215_IN_SEL_V  0x1
#define GPIO_SIG215_IN_SEL_S  7
/* GPIO_FUNC215_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC215_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC215_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC215_IN_INV_SEL_V  0x1
#define GPIO_FUNC215_IN_INV_SEL_S  6
/* GPIO_FUNC215_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC215_IN_SEL  0x0000003F
#define GPIO_FUNC215_IN_SEL_M  ((GPIO_FUNC215_IN_SEL_V)<<(GPIO_FUNC215_IN_SEL_S))
#define GPIO_FUNC215_IN_SEL_V  0x3F
#define GPIO_FUNC215_IN_SEL_S  0

#define GPIO_FUNC216_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x0490)
/* GPIO_SIG216_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG216_IN_SEL  (BIT(7))
#define GPIO_SIG216_IN_SEL_M  (BIT(7))
#define GPIO_SIG216_IN_SEL_V  0x1
#define GPIO_SIG216_IN_SEL_S  7
/* GPIO_FUNC216_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC216_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC216_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC216_IN_INV_SEL_V  0x1
#define GPIO_FUNC216_IN_INV_SEL_S  6
/* GPIO_FUNC216_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC216_IN_SEL  0x0000003F
#define GPIO_FUNC216_IN_SEL_M  ((GPIO_FUNC216_IN_SEL_V)<<(GPIO_FUNC216_IN_SEL_S))
#define GPIO_FUNC216_IN_SEL_V  0x3F
#define GPIO_FUNC216_IN_SEL_S  0

#define GPIO_FUNC217_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x0494)
/* GPIO_SIG217_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG217_IN_SEL  (BIT(7))
#define GPIO_SIG217_IN_SEL_M  (BIT(7))
#define GPIO_SIG217_IN_SEL_V  0x1
#define GPIO_SIG217_IN_SEL_S  7
/* GPIO_FUNC217_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC217_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC217_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC217_IN_INV_SEL_V  0x1
#define GPIO_FUNC217_IN_INV_SEL_S  6
/* GPIO_FUNC217_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC217_IN_SEL  0x0000003F
#define GPIO_FUNC217_IN_SEL_M  ((GPIO_FUNC217_IN_SEL_V)<<(GPIO_FUNC217_IN_SEL_S))
#define GPIO_FUNC217_IN_SEL_V  0x3F
#define GPIO_FUNC217_IN_SEL_S  0

#define GPIO_FUNC218_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x0498)
/* GPIO_SIG218_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG218_IN_SEL  (BIT(7))
#define GPIO_SIG218_IN_SEL_M  (BIT(7))
#define GPIO_SIG218_IN_SEL_V  0x1
#define GPIO_SIG218_IN_SEL_S  7
/* GPIO_FUNC218_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC218_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC218_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC218_IN_INV_SEL_V  0x1
#define GPIO_FUNC218_IN_INV_SEL_S  6
/* GPIO_FUNC218_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC218_IN_SEL  0x0000003F
#define GPIO_FUNC218_IN_SEL_M  ((GPIO_FUNC218_IN_SEL_V)<<(GPIO_FUNC218_IN_SEL_S))
#define GPIO_FUNC218_IN_SEL_V  0x3F
#define GPIO_FUNC218_IN_SEL_S  0

#define GPIO_FUNC219_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x049c)
/* GPIO_SIG219_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG219_IN_SEL  (BIT(7))
#define GPIO_SIG219_IN_SEL_M  (BIT(7))
#define GPIO_SIG219_IN_SEL_V  0x1
#define GPIO_SIG219_IN_SEL_S  7
/* GPIO_FUNC219_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC219_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC219_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC219_IN_INV_SEL_V  0x1
#define GPIO_FUNC219_IN_INV_SEL_S  6
/* GPIO_FUNC219_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC219_IN_SEL  0x0000003F
#define GPIO_FUNC219_IN_SEL_M  ((GPIO_FUNC219_IN_SEL_V)<<(GPIO_FUNC219_IN_SEL_S))
#define GPIO_FUNC219_IN_SEL_V  0x3F
#define GPIO_FUNC219_IN_SEL_S  0

#define GPIO_FUNC220_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x04a0)
/* GPIO_SIG220_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG220_IN_SEL  (BIT(7))
#define GPIO_SIG220_IN_SEL_M  (BIT(7))
#define GPIO_SIG220_IN_SEL_V  0x1
#define GPIO_SIG220_IN_SEL_S  7
/* GPIO_FUNC220_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC220_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC220_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC220_IN_INV_SEL_V  0x1
#define GPIO_FUNC220_IN_INV_SEL_S  6
/* GPIO_FUNC220_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC220_IN_SEL  0x0000003F
#define GPIO_FUNC220_IN_SEL_M  ((GPIO_FUNC220_IN_SEL_V)<<(GPIO_FUNC220_IN_SEL_S))
#define GPIO_FUNC220_IN_SEL_V  0x3F
#define GPIO_FUNC220_IN_SEL_S  0

#define GPIO_FUNC221_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x04a4)
/* GPIO_SIG221_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG221_IN_SEL  (BIT(7))
#define GPIO_SIG221_IN_SEL_M  (BIT(7))
#define GPIO_SIG221_IN_SEL_V  0x1
#define GPIO_SIG221_IN_SEL_S  7
/* GPIO_FUNC221_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC221_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC221_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC221_IN_INV_SEL_V  0x1
#define GPIO_FUNC221_IN_INV_SEL_S  6
/* GPIO_FUNC221_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC221_IN_SEL  0x0000003F
#define GPIO_FUNC221_IN_SEL_M  ((GPIO_FUNC221_IN_SEL_V)<<(GPIO_FUNC221_IN_SEL_S))
#define GPIO_FUNC221_IN_SEL_V  0x3F
#define GPIO_FUNC221_IN_SEL_S  0

#define GPIO_FUNC222_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x04a8)
/* GPIO_SIG222_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG222_IN_SEL  (BIT(7))
#define GPIO_SIG222_IN_SEL_M  (BIT(7))
#define GPIO_SIG222_IN_SEL_V  0x1
#define GPIO_SIG222_IN_SEL_S  7
/* GPIO_FUNC222_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC222_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC222_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC222_IN_INV_SEL_V  0x1
#define GPIO_FUNC222_IN_INV_SEL_S  6
/* GPIO_FUNC222_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC222_IN_SEL  0x0000003F
#define GPIO_FUNC222_IN_SEL_M  ((GPIO_FUNC222_IN_SEL_V)<<(GPIO_FUNC222_IN_SEL_S))
#define GPIO_FUNC222_IN_SEL_V  0x3F
#define GPIO_FUNC222_IN_SEL_S  0

#define GPIO_FUNC223_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x04ac)
/* GPIO_SIG223_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG223_IN_SEL  (BIT(7))
#define GPIO_SIG223_IN_SEL_M  (BIT(7))
#define GPIO_SIG223_IN_SEL_V  0x1
#define GPIO_SIG223_IN_SEL_S  7
/* GPIO_FUNC223_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC223_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC223_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC223_IN_INV_SEL_V  0x1
#define GPIO_FUNC223_IN_INV_SEL_S  6
/* GPIO_FUNC223_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC223_IN_SEL  0x0000003F
#define GPIO_FUNC223_IN_SEL_M  ((GPIO_FUNC223_IN_SEL_V)<<(GPIO_FUNC223_IN_SEL_S))
#define GPIO_FUNC223_IN_SEL_V  0x3F
#define GPIO_FUNC223_IN_SEL_S  0

#define GPIO_FUNC224_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x04b0)
/* GPIO_SIG224_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG224_IN_SEL  (BIT(7))
#define GPIO_SIG224_IN_SEL_M  (BIT(7))
#define GPIO_SIG224_IN_SEL_V  0x1
#define GPIO_SIG224_IN_SEL_S  7
/* GPIO_FUNC224_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC224_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC224_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC224_IN_INV_SEL_V  0x1
#define GPIO_FUNC224_IN_INV_SEL_S  6
/* GPIO_FUNC224_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC224_IN_SEL  0x0000003F
#define GPIO_FUNC224_IN_SEL_M  ((GPIO_FUNC224_IN_SEL_V)<<(GPIO_FUNC224_IN_SEL_S))
#define GPIO_FUNC224_IN_SEL_V  0x3F
#define GPIO_FUNC224_IN_SEL_S  0

#define GPIO_FUNC225_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x04b4)
/* GPIO_SIG225_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG225_IN_SEL  (BIT(7))
#define GPIO_SIG225_IN_SEL_M  (BIT(7))
#define GPIO_SIG225_IN_SEL_V  0x1
#define GPIO_SIG225_IN_SEL_S  7
/* GPIO_FUNC225_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC225_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC225_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC225_IN_INV_SEL_V  0x1
#define GPIO_FUNC225_IN_INV_SEL_S  6
/* GPIO_FUNC225_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC225_IN_SEL  0x0000003F
#define GPIO_FUNC225_IN_SEL_M  ((GPIO_FUNC225_IN_SEL_V)<<(GPIO_FUNC225_IN_SEL_S))
#define GPIO_FUNC225_IN_SEL_V  0x3F
#define GPIO_FUNC225_IN_SEL_S  0

#define GPIO_FUNC226_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x04b8)
/* GPIO_SIG226_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG226_IN_SEL  (BIT(7))
#define GPIO_SIG226_IN_SEL_M  (BIT(7))
#define GPIO_SIG226_IN_SEL_V  0x1
#define GPIO_SIG226_IN_SEL_S  7
/* GPIO_FUNC226_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC226_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC226_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC226_IN_INV_SEL_V  0x1
#define GPIO_FUNC226_IN_INV_SEL_S  6
/* GPIO_FUNC226_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC226_IN_SEL  0x0000003F
#define GPIO_FUNC226_IN_SEL_M  ((GPIO_FUNC226_IN_SEL_V)<<(GPIO_FUNC226_IN_SEL_S))
#define GPIO_FUNC226_IN_SEL_V  0x3F
#define GPIO_FUNC226_IN_SEL_S  0

#define GPIO_FUNC227_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x04bc)
/* GPIO_SIG227_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG227_IN_SEL  (BIT(7))
#define GPIO_SIG227_IN_SEL_M  (BIT(7))
#define GPIO_SIG227_IN_SEL_V  0x1
#define GPIO_SIG227_IN_SEL_S  7
/* GPIO_FUNC227_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC227_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC227_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC227_IN_INV_SEL_V  0x1
#define GPIO_FUNC227_IN_INV_SEL_S  6
/* GPIO_FUNC227_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC227_IN_SEL  0x0000003F
#define GPIO_FUNC227_IN_SEL_M  ((GPIO_FUNC227_IN_SEL_V)<<(GPIO_FUNC227_IN_SEL_S))
#define GPIO_FUNC227_IN_SEL_V  0x3F
#define GPIO_FUNC227_IN_SEL_S  0

#define GPIO_FUNC228_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x04c0)
/* GPIO_SIG228_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG228_IN_SEL  (BIT(7))
#define GPIO_SIG228_IN_SEL_M  (BIT(7))
#define GPIO_SIG228_IN_SEL_V  0x1
#define GPIO_SIG228_IN_SEL_S  7
/* GPIO_FUNC228_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC228_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC228_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC228_IN_INV_SEL_V  0x1
#define GPIO_FUNC228_IN_INV_SEL_S  6
/* GPIO_FUNC228_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC228_IN_SEL  0x0000003F
#define GPIO_FUNC228_IN_SEL_M  ((GPIO_FUNC228_IN_SEL_V)<<(GPIO_FUNC228_IN_SEL_S))
#define GPIO_FUNC228_IN_SEL_V  0x3F
#define GPIO_FUNC228_IN_SEL_S  0

#define GPIO_FUNC229_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x04c4)
/* GPIO_SIG229_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG229_IN_SEL  (BIT(7))
#define GPIO_SIG229_IN_SEL_M  (BIT(7))
#define GPIO_SIG229_IN_SEL_V  0x1
#define GPIO_SIG229_IN_SEL_S  7
/* GPIO_FUNC229_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC229_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC229_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC229_IN_INV_SEL_V  0x1
#define GPIO_FUNC229_IN_INV_SEL_S  6
/* GPIO_FUNC229_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC229_IN_SEL  0x0000003F
#define GPIO_FUNC229_IN_SEL_M  ((GPIO_FUNC229_IN_SEL_V)<<(GPIO_FUNC229_IN_SEL_S))
#define GPIO_FUNC229_IN_SEL_V  0x3F
#define GPIO_FUNC229_IN_SEL_S  0

#define GPIO_FUNC230_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x04c8)
/* GPIO_SIG230_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG230_IN_SEL  (BIT(7))
#define GPIO_SIG230_IN_SEL_M  (BIT(7))
#define GPIO_SIG230_IN_SEL_V  0x1
#define GPIO_SIG230_IN_SEL_S  7
/* GPIO_FUNC230_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC230_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC230_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC230_IN_INV_SEL_V  0x1
#define GPIO_FUNC230_IN_INV_SEL_S  6
/* GPIO_FUNC230_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC230_IN_SEL  0x0000003F
#define GPIO_FUNC230_IN_SEL_M  ((GPIO_FUNC230_IN_SEL_V)<<(GPIO_FUNC230_IN_SEL_S))
#define GPIO_FUNC230_IN_SEL_V  0x3F
#define GPIO_FUNC230_IN_SEL_S  0

#define GPIO_FUNC231_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x04cc)
/* GPIO_SIG231_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG231_IN_SEL  (BIT(7))
#define GPIO_SIG231_IN_SEL_M  (BIT(7))
#define GPIO_SIG231_IN_SEL_V  0x1
#define GPIO_SIG231_IN_SEL_S  7
/* GPIO_FUNC231_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC231_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC231_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC231_IN_INV_SEL_V  0x1
#define GPIO_FUNC231_IN_INV_SEL_S  6
/* GPIO_FUNC231_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC231_IN_SEL  0x0000003F
#define GPIO_FUNC231_IN_SEL_M  ((GPIO_FUNC231_IN_SEL_V)<<(GPIO_FUNC231_IN_SEL_S))
#define GPIO_FUNC231_IN_SEL_V  0x3F
#define GPIO_FUNC231_IN_SEL_S  0

#define GPIO_FUNC232_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x04d0)
/* GPIO_SIG232_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG232_IN_SEL  (BIT(7))
#define GPIO_SIG232_IN_SEL_M  (BIT(7))
#define GPIO_SIG232_IN_SEL_V  0x1
#define GPIO_SIG232_IN_SEL_S  7
/* GPIO_FUNC232_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC232_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC232_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC232_IN_INV_SEL_V  0x1
#define GPIO_FUNC232_IN_INV_SEL_S  6
/* GPIO_FUNC232_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC232_IN_SEL  0x0000003F
#define GPIO_FUNC232_IN_SEL_M  ((GPIO_FUNC232_IN_SEL_V)<<(GPIO_FUNC232_IN_SEL_S))
#define GPIO_FUNC232_IN_SEL_V  0x3F
#define GPIO_FUNC232_IN_SEL_S  0

#define GPIO_FUNC233_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x04d4)
/* GPIO_SIG233_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG233_IN_SEL  (BIT(7))
#define GPIO_SIG233_IN_SEL_M  (BIT(7))
#define GPIO_SIG233_IN_SEL_V  0x1
#define GPIO_SIG233_IN_SEL_S  7
/* GPIO_FUNC233_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC233_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC233_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC233_IN_INV_SEL_V  0x1
#define GPIO_FUNC233_IN_INV_SEL_S  6
/* GPIO_FUNC233_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC233_IN_SEL  0x0000003F
#define GPIO_FUNC233_IN_SEL_M  ((GPIO_FUNC233_IN_SEL_V)<<(GPIO_FUNC233_IN_SEL_S))
#define GPIO_FUNC233_IN_SEL_V  0x3F
#define GPIO_FUNC233_IN_SEL_S  0

#define GPIO_FUNC234_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x04d8)
/* GPIO_SIG234_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG234_IN_SEL  (BIT(7))
#define GPIO_SIG234_IN_SEL_M  (BIT(7))
#define GPIO_SIG234_IN_SEL_V  0x1
#define GPIO_SIG234_IN_SEL_S  7
/* GPIO_FUNC234_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC234_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC234_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC234_IN_INV_SEL_V  0x1
#define GPIO_FUNC234_IN_INV_SEL_S  6
/* GPIO_FUNC234_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC234_IN_SEL  0x0000003F
#define GPIO_FUNC234_IN_SEL_M  ((GPIO_FUNC234_IN_SEL_V)<<(GPIO_FUNC234_IN_SEL_S))
#define GPIO_FUNC234_IN_SEL_V  0x3F
#define GPIO_FUNC234_IN_SEL_S  0

#define GPIO_FUNC235_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x04dc)
/* GPIO_SIG235_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG235_IN_SEL  (BIT(7))
#define GPIO_SIG235_IN_SEL_M  (BIT(7))
#define GPIO_SIG235_IN_SEL_V  0x1
#define GPIO_SIG235_IN_SEL_S  7
/* GPIO_FUNC235_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC235_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC235_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC235_IN_INV_SEL_V  0x1
#define GPIO_FUNC235_IN_INV_SEL_S  6
/* GPIO_FUNC235_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC235_IN_SEL  0x0000003F
#define GPIO_FUNC235_IN_SEL_M  ((GPIO_FUNC235_IN_SEL_V)<<(GPIO_FUNC235_IN_SEL_S))
#define GPIO_FUNC235_IN_SEL_V  0x3F
#define GPIO_FUNC235_IN_SEL_S  0

#define GPIO_FUNC236_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x04e0)
/* GPIO_SIG236_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG236_IN_SEL  (BIT(7))
#define GPIO_SIG236_IN_SEL_M  (BIT(7))
#define GPIO_SIG236_IN_SEL_V  0x1
#define GPIO_SIG236_IN_SEL_S  7
/* GPIO_FUNC236_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC236_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC236_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC236_IN_INV_SEL_V  0x1
#define GPIO_FUNC236_IN_INV_SEL_S  6
/* GPIO_FUNC236_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC236_IN_SEL  0x0000003F
#define GPIO_FUNC236_IN_SEL_M  ((GPIO_FUNC236_IN_SEL_V)<<(GPIO_FUNC236_IN_SEL_S))
#define GPIO_FUNC236_IN_SEL_V  0x3F
#define GPIO_FUNC236_IN_SEL_S  0

#define GPIO_FUNC237_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x04e4)
/* GPIO_SIG237_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG237_IN_SEL  (BIT(7))
#define GPIO_SIG237_IN_SEL_M  (BIT(7))
#define GPIO_SIG237_IN_SEL_V  0x1
#define GPIO_SIG237_IN_SEL_S  7
/* GPIO_FUNC237_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC237_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC237_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC237_IN_INV_SEL_V  0x1
#define GPIO_FUNC237_IN_INV_SEL_S  6
/* GPIO_FUNC237_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC237_IN_SEL  0x0000003F
#define GPIO_FUNC237_IN_SEL_M  ((GPIO_FUNC237_IN_SEL_V)<<(GPIO_FUNC237_IN_SEL_S))
#define GPIO_FUNC237_IN_SEL_V  0x3F
#define GPIO_FUNC237_IN_SEL_S  0

#define GPIO_FUNC238_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x04e8)
/* GPIO_SIG238_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG238_IN_SEL  (BIT(7))
#define GPIO_SIG238_IN_SEL_M  (BIT(7))
#define GPIO_SIG238_IN_SEL_V  0x1
#define GPIO_SIG238_IN_SEL_S  7
/* GPIO_FUNC238_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC238_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC238_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC238_IN_INV_SEL_V  0x1
#define GPIO_FUNC238_IN_INV_SEL_S  6
/* GPIO_FUNC238_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC238_IN_SEL  0x0000003F
#define GPIO_FUNC238_IN_SEL_M  ((GPIO_FUNC238_IN_SEL_V)<<(GPIO_FUNC238_IN_SEL_S))
#define GPIO_FUNC238_IN_SEL_V  0x3F
#define GPIO_FUNC238_IN_SEL_S  0

#define GPIO_FUNC239_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x04ec)
/* GPIO_SIG239_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG239_IN_SEL  (BIT(7))
#define GPIO_SIG239_IN_SEL_M  (BIT(7))
#define GPIO_SIG239_IN_SEL_V  0x1
#define GPIO_SIG239_IN_SEL_S  7
/* GPIO_FUNC239_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC239_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC239_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC239_IN_INV_SEL_V  0x1
#define GPIO_FUNC239_IN_INV_SEL_S  6
/* GPIO_FUNC239_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC239_IN_SEL  0x0000003F
#define GPIO_FUNC239_IN_SEL_M  ((GPIO_FUNC239_IN_SEL_V)<<(GPIO_FUNC239_IN_SEL_S))
#define GPIO_FUNC239_IN_SEL_V  0x3F
#define GPIO_FUNC239_IN_SEL_S  0

#define GPIO_FUNC240_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x04f0)
/* GPIO_SIG240_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG240_IN_SEL  (BIT(7))
#define GPIO_SIG240_IN_SEL_M  (BIT(7))
#define GPIO_SIG240_IN_SEL_V  0x1
#define GPIO_SIG240_IN_SEL_S  7
/* GPIO_FUNC240_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC240_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC240_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC240_IN_INV_SEL_V  0x1
#define GPIO_FUNC240_IN_INV_SEL_S  6
/* GPIO_FUNC240_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC240_IN_SEL  0x0000003F
#define GPIO_FUNC240_IN_SEL_M  ((GPIO_FUNC240_IN_SEL_V)<<(GPIO_FUNC240_IN_SEL_S))
#define GPIO_FUNC240_IN_SEL_V  0x3F
#define GPIO_FUNC240_IN_SEL_S  0

#define GPIO_FUNC241_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x04f4)
/* GPIO_SIG241_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG241_IN_SEL  (BIT(7))
#define GPIO_SIG241_IN_SEL_M  (BIT(7))
#define GPIO_SIG241_IN_SEL_V  0x1
#define GPIO_SIG241_IN_SEL_S  7
/* GPIO_FUNC241_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC241_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC241_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC241_IN_INV_SEL_V  0x1
#define GPIO_FUNC241_IN_INV_SEL_S  6
/* GPIO_FUNC241_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC241_IN_SEL  0x0000003F
#define GPIO_FUNC241_IN_SEL_M  ((GPIO_FUNC241_IN_SEL_V)<<(GPIO_FUNC241_IN_SEL_S))
#define GPIO_FUNC241_IN_SEL_V  0x3F
#define GPIO_FUNC241_IN_SEL_S  0

#define GPIO_FUNC242_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x04f8)
/* GPIO_SIG242_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG242_IN_SEL  (BIT(7))
#define GPIO_SIG242_IN_SEL_M  (BIT(7))
#define GPIO_SIG242_IN_SEL_V  0x1
#define GPIO_SIG242_IN_SEL_S  7
/* GPIO_FUNC242_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC242_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC242_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC242_IN_INV_SEL_V  0x1
#define GPIO_FUNC242_IN_INV_SEL_S  6
/* GPIO_FUNC242_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC242_IN_SEL  0x0000003F
#define GPIO_FUNC242_IN_SEL_M  ((GPIO_FUNC242_IN_SEL_V)<<(GPIO_FUNC242_IN_SEL_S))
#define GPIO_FUNC242_IN_SEL_V  0x3F
#define GPIO_FUNC242_IN_SEL_S  0

#define GPIO_FUNC243_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x04fc)
/* GPIO_SIG243_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG243_IN_SEL  (BIT(7))
#define GPIO_SIG243_IN_SEL_M  (BIT(7))
#define GPIO_SIG243_IN_SEL_V  0x1
#define GPIO_SIG243_IN_SEL_S  7
/* GPIO_FUNC243_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC243_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC243_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC243_IN_INV_SEL_V  0x1
#define GPIO_FUNC243_IN_INV_SEL_S  6
/* GPIO_FUNC243_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC243_IN_SEL  0x0000003F
#define GPIO_FUNC243_IN_SEL_M  ((GPIO_FUNC243_IN_SEL_V)<<(GPIO_FUNC243_IN_SEL_S))
#define GPIO_FUNC243_IN_SEL_V  0x3F
#define GPIO_FUNC243_IN_SEL_S  0

#define GPIO_FUNC244_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x0500)
/* GPIO_SIG244_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG244_IN_SEL  (BIT(7))
#define GPIO_SIG244_IN_SEL_M  (BIT(7))
#define GPIO_SIG244_IN_SEL_V  0x1
#define GPIO_SIG244_IN_SEL_S  7
/* GPIO_FUNC244_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC244_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC244_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC244_IN_INV_SEL_V  0x1
#define GPIO_FUNC244_IN_INV_SEL_S  6
/* GPIO_FUNC244_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC244_IN_SEL  0x0000003F
#define GPIO_FUNC244_IN_SEL_M  ((GPIO_FUNC244_IN_SEL_V)<<(GPIO_FUNC244_IN_SEL_S))
#define GPIO_FUNC244_IN_SEL_V  0x3F
#define GPIO_FUNC244_IN_SEL_S  0

#define GPIO_FUNC245_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x0504)
/* GPIO_SIG245_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG245_IN_SEL  (BIT(7))
#define GPIO_SIG245_IN_SEL_M  (BIT(7))
#define GPIO_SIG245_IN_SEL_V  0x1
#define GPIO_SIG245_IN_SEL_S  7
/* GPIO_FUNC245_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC245_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC245_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC245_IN_INV_SEL_V  0x1
#define GPIO_FUNC245_IN_INV_SEL_S  6
/* GPIO_FUNC245_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC245_IN_SEL  0x0000003F
#define GPIO_FUNC245_IN_SEL_M  ((GPIO_FUNC245_IN_SEL_V)<<(GPIO_FUNC245_IN_SEL_S))
#define GPIO_FUNC245_IN_SEL_V  0x3F
#define GPIO_FUNC245_IN_SEL_S  0

#define GPIO_FUNC246_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x0508)
/* GPIO_SIG246_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG246_IN_SEL  (BIT(7))
#define GPIO_SIG246_IN_SEL_M  (BIT(7))
#define GPIO_SIG246_IN_SEL_V  0x1
#define GPIO_SIG246_IN_SEL_S  7
/* GPIO_FUNC246_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC246_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC246_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC246_IN_INV_SEL_V  0x1
#define GPIO_FUNC246_IN_INV_SEL_S  6
/* GPIO_FUNC246_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC246_IN_SEL  0x0000003F
#define GPIO_FUNC246_IN_SEL_M  ((GPIO_FUNC246_IN_SEL_V)<<(GPIO_FUNC246_IN_SEL_S))
#define GPIO_FUNC246_IN_SEL_V  0x3F
#define GPIO_FUNC246_IN_SEL_S  0

#define GPIO_FUNC247_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x050c)
/* GPIO_SIG247_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG247_IN_SEL  (BIT(7))
#define GPIO_SIG247_IN_SEL_M  (BIT(7))
#define GPIO_SIG247_IN_SEL_V  0x1
#define GPIO_SIG247_IN_SEL_S  7
/* GPIO_FUNC247_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC247_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC247_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC247_IN_INV_SEL_V  0x1
#define GPIO_FUNC247_IN_INV_SEL_S  6
/* GPIO_FUNC247_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC247_IN_SEL  0x0000003F
#define GPIO_FUNC247_IN_SEL_M  ((GPIO_FUNC247_IN_SEL_V)<<(GPIO_FUNC247_IN_SEL_S))
#define GPIO_FUNC247_IN_SEL_V  0x3F
#define GPIO_FUNC247_IN_SEL_S  0

#define GPIO_FUNC248_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x0510)
/* GPIO_SIG248_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG248_IN_SEL  (BIT(7))
#define GPIO_SIG248_IN_SEL_M  (BIT(7))
#define GPIO_SIG248_IN_SEL_V  0x1
#define GPIO_SIG248_IN_SEL_S  7
/* GPIO_FUNC248_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC248_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC248_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC248_IN_INV_SEL_V  0x1
#define GPIO_FUNC248_IN_INV_SEL_S  6
/* GPIO_FUNC248_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC248_IN_SEL  0x0000003F
#define GPIO_FUNC248_IN_SEL_M  ((GPIO_FUNC248_IN_SEL_V)<<(GPIO_FUNC248_IN_SEL_S))
#define GPIO_FUNC248_IN_SEL_V  0x3F
#define GPIO_FUNC248_IN_SEL_S  0

#define GPIO_FUNC249_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x0514)
/* GPIO_SIG249_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG249_IN_SEL  (BIT(7))
#define GPIO_SIG249_IN_SEL_M  (BIT(7))
#define GPIO_SIG249_IN_SEL_V  0x1
#define GPIO_SIG249_IN_SEL_S  7
/* GPIO_FUNC249_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC249_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC249_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC249_IN_INV_SEL_V  0x1
#define GPIO_FUNC249_IN_INV_SEL_S  6
/* GPIO_FUNC249_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC249_IN_SEL  0x0000003F
#define GPIO_FUNC249_IN_SEL_M  ((GPIO_FUNC249_IN_SEL_V)<<(GPIO_FUNC249_IN_SEL_S))
#define GPIO_FUNC249_IN_SEL_V  0x3F
#define GPIO_FUNC249_IN_SEL_S  0

#define GPIO_FUNC250_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x0518)
/* GPIO_SIG250_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG250_IN_SEL  (BIT(7))
#define GPIO_SIG250_IN_SEL_M  (BIT(7))
#define GPIO_SIG250_IN_SEL_V  0x1
#define GPIO_SIG250_IN_SEL_S  7
/* GPIO_FUNC250_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC250_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC250_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC250_IN_INV_SEL_V  0x1
#define GPIO_FUNC250_IN_INV_SEL_S  6
/* GPIO_FUNC250_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC250_IN_SEL  0x0000003F
#define GPIO_FUNC250_IN_SEL_M  ((GPIO_FUNC250_IN_SEL_V)<<(GPIO_FUNC250_IN_SEL_S))
#define GPIO_FUNC250_IN_SEL_V  0x3F
#define GPIO_FUNC250_IN_SEL_S  0

#define GPIO_FUNC251_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x051c)
/* GPIO_SIG251_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG251_IN_SEL  (BIT(7))
#define GPIO_SIG251_IN_SEL_M  (BIT(7))
#define GPIO_SIG251_IN_SEL_V  0x1
#define GPIO_SIG251_IN_SEL_S  7
/* GPIO_FUNC251_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC251_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC251_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC251_IN_INV_SEL_V  0x1
#define GPIO_FUNC251_IN_INV_SEL_S  6
/* GPIO_FUNC251_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC251_IN_SEL  0x0000003F
#define GPIO_FUNC251_IN_SEL_M  ((GPIO_FUNC251_IN_SEL_V)<<(GPIO_FUNC251_IN_SEL_S))
#define GPIO_FUNC251_IN_SEL_V  0x3F
#define GPIO_FUNC251_IN_SEL_S  0

#define GPIO_FUNC252_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x0520)
/* GPIO_SIG252_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG252_IN_SEL  (BIT(7))
#define GPIO_SIG252_IN_SEL_M  (BIT(7))
#define GPIO_SIG252_IN_SEL_V  0x1
#define GPIO_SIG252_IN_SEL_S  7
/* GPIO_FUNC252_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC252_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC252_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC252_IN_INV_SEL_V  0x1
#define GPIO_FUNC252_IN_INV_SEL_S  6
/* GPIO_FUNC252_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC252_IN_SEL  0x0000003F
#define GPIO_FUNC252_IN_SEL_M  ((GPIO_FUNC252_IN_SEL_V)<<(GPIO_FUNC252_IN_SEL_S))
#define GPIO_FUNC252_IN_SEL_V  0x3F
#define GPIO_FUNC252_IN_SEL_S  0

#define GPIO_FUNC253_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x0524)
/* GPIO_SIG253_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG253_IN_SEL  (BIT(7))
#define GPIO_SIG253_IN_SEL_M  (BIT(7))
#define GPIO_SIG253_IN_SEL_V  0x1
#define GPIO_SIG253_IN_SEL_S  7
/* GPIO_FUNC253_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC253_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC253_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC253_IN_INV_SEL_V  0x1
#define GPIO_FUNC253_IN_INV_SEL_S  6
/* GPIO_FUNC253_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC253_IN_SEL  0x0000003F
#define GPIO_FUNC253_IN_SEL_M  ((GPIO_FUNC253_IN_SEL_V)<<(GPIO_FUNC253_IN_SEL_S))
#define GPIO_FUNC253_IN_SEL_V  0x3F
#define GPIO_FUNC253_IN_SEL_S  0

#define GPIO_FUNC254_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x0528)
/* GPIO_SIG254_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG254_IN_SEL  (BIT(7))
#define GPIO_SIG254_IN_SEL_M  (BIT(7))
#define GPIO_SIG254_IN_SEL_V  0x1
#define GPIO_SIG254_IN_SEL_S  7
/* GPIO_FUNC254_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC254_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC254_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC254_IN_INV_SEL_V  0x1
#define GPIO_FUNC254_IN_INV_SEL_S  6
/* GPIO_FUNC254_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC254_IN_SEL  0x0000003F
#define GPIO_FUNC254_IN_SEL_M  ((GPIO_FUNC254_IN_SEL_V)<<(GPIO_FUNC254_IN_SEL_S))
#define GPIO_FUNC254_IN_SEL_V  0x3F
#define GPIO_FUNC254_IN_SEL_S  0

#define GPIO_FUNC255_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x052c)
/* GPIO_SIG255_IN_SEL : R/W ;bitpos:[7] ;default: x ; */
/*description: if the slow signal bypass the io matrix or not if you want  setting
 the value to 1*/
#define GPIO_SIG255_IN_SEL  (BIT(7))
#define GPIO_SIG255_IN_SEL_M  (BIT(7))
#define GPIO_SIG255_IN_SEL_V  0x1
#define GPIO_SIG255_IN_SEL_S  7
/* GPIO_FUNC255_IN_INV_SEL : R/W ;bitpos:[6] ;default: x ; */
/*description: revert the value of the input if you want to revert  please set the value to 1*/
#define GPIO_FUNC255_IN_INV_SEL  (BIT(6))
#define GPIO_FUNC255_IN_INV_SEL_M  (BIT(6))
#define GPIO_FUNC255_IN_INV_SEL_V  0x1
#define GPIO_FUNC255_IN_INV_SEL_S  6
/* GPIO_FUNC255_IN_SEL : R/W ;bitpos:[5:0] ;default: x ; */
/*description: select one of the 256 inputs*/
#define GPIO_FUNC255_IN_SEL  0x0000003F
#define GPIO_FUNC255_IN_SEL_M  ((GPIO_FUNC255_IN_SEL_V)<<(GPIO_FUNC255_IN_SEL_S))
#define GPIO_FUNC255_IN_SEL_V  0x3F
#define GPIO_FUNC255_IN_SEL_S  0

#define GPIO_FUNC0_OUT_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x0530)
/* GPIO_FUNC0_OEN_INV_SEL : R/W ;bitpos:[11] ;default: x ; */
/*description: invert the output enable value  if you want to revert the output
 enable value  setting the value to 1*/
#define GPIO_FUNC0_OEN_INV_SEL  (BIT(11))
#define GPIO_FUNC0_OEN_INV_SEL_M  (BIT(11))
#define GPIO_FUNC0_OEN_INV_SEL_V  0x1
#define GPIO_FUNC0_OEN_INV_SEL_S  11
/* GPIO_FUNC0_OEN_SEL : R/W ;bitpos:[10] ;default: x ; */
/*description: weather using the logical oen signal or not using the value setting
 by the register*/
#define GPIO_FUNC0_OEN_SEL  (BIT(10))
#define GPIO_FUNC0_OEN_SEL_M  (BIT(10))
#define GPIO_FUNC0_OEN_SEL_V  0x1
#define GPIO_FUNC0_OEN_SEL_S  10
/* GPIO_FUNC0_OUT_INV_SEL : R/W ;bitpos:[9] ;default: x ; */
/*description: invert the output value  if you want to revert the output value
  setting the value to 1*/
#define GPIO_FUNC0_OUT_INV_SEL  (BIT(9))
#define GPIO_FUNC0_OUT_INV_SEL_M  (BIT(9))
#define GPIO_FUNC0_OUT_INV_SEL_V  0x1
#define GPIO_FUNC0_OUT_INV_SEL_S  9
/* GPIO_FUNC0_OUT_SEL : R/W ;bitpos:[8:0] ;default: x ; */
/*description: select one of the 256 output to 40 GPIO*/
#define GPIO_FUNC0_OUT_SEL  0x000001FF
#define GPIO_FUNC0_OUT_SEL_M  ((GPIO_FUNC0_OUT_SEL_V)<<(GPIO_FUNC0_OUT_SEL_S))
#define GPIO_FUNC0_OUT_SEL_V  0x1FF
#define GPIO_FUNC0_OUT_SEL_S  0

#define GPIO_FUNC1_OUT_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x0534)
/* GPIO_FUNC1_OEN_INV_SEL : R/W ;bitpos:[11] ;default: x ; */
/*description: invert the output enable value  if you want to revert the output
 enable value  setting the value to 1*/
#define GPIO_FUNC1_OEN_INV_SEL  (BIT(11))
#define GPIO_FUNC1_OEN_INV_SEL_M  (BIT(11))
#define GPIO_FUNC1_OEN_INV_SEL_V  0x1
#define GPIO_FUNC1_OEN_INV_SEL_S  11
/* GPIO_FUNC1_OEN_SEL : R/W ;bitpos:[10] ;default: x ; */
/*description: weather using the logical oen signal or not using the value setting
 by the register*/
#define GPIO_FUNC1_OEN_SEL  (BIT(10))
#define GPIO_FUNC1_OEN_SEL_M  (BIT(10))
#define GPIO_FUNC1_OEN_SEL_V  0x1
#define GPIO_FUNC1_OEN_SEL_S  10
/* GPIO_FUNC1_OUT_INV_SEL : R/W ;bitpos:[9] ;default: x ; */
/*description: invert the output value  if you want to revert the output value
  setting the value to 1*/
#define GPIO_FUNC1_OUT_INV_SEL  (BIT(9))
#define GPIO_FUNC1_OUT_INV_SEL_M  (BIT(9))
#define GPIO_FUNC1_OUT_INV_SEL_V  0x1
#define GPIO_FUNC1_OUT_INV_SEL_S  9
/* GPIO_FUNC1_OUT_SEL : R/W ;bitpos:[8:0] ;default: x ; */
/*description: select one of the 256 output to 40 GPIO*/
#define GPIO_FUNC1_OUT_SEL  0x000001FF
#define GPIO_FUNC1_OUT_SEL_M  ((GPIO_FUNC1_OUT_SEL_V)<<(GPIO_FUNC1_OUT_SEL_S))
#define GPIO_FUNC1_OUT_SEL_V  0x1FF
#define GPIO_FUNC1_OUT_SEL_S  0

#define GPIO_FUNC2_OUT_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x0538)
/* GPIO_FUNC2_OEN_INV_SEL : R/W ;bitpos:[11] ;default: x ; */
/*description: invert the output enable value  if you want to revert the output
 enable value  setting the value to 1*/
#define GPIO_FUNC2_OEN_INV_SEL  (BIT(11))
#define GPIO_FUNC2_OEN_INV_SEL_M  (BIT(11))
#define GPIO_FUNC2_OEN_INV_SEL_V  0x1
#define GPIO_FUNC2_OEN_INV_SEL_S  11
/* GPIO_FUNC2_OEN_SEL : R/W ;bitpos:[10] ;default: x ; */
/*description: weather using the logical oen signal or not using the value setting
 by the register*/
#define GPIO_FUNC2_OEN_SEL  (BIT(10))
#define GPIO_FUNC2_OEN_SEL_M  (BIT(10))
#define GPIO_FUNC2_OEN_SEL_V  0x1
#define GPIO_FUNC2_OEN_SEL_S  10
/* GPIO_FUNC2_OUT_INV_SEL : R/W ;bitpos:[9] ;default: x ; */
/*description: invert the output value  if you want to revert the output value
  setting the value to 1*/
#define GPIO_FUNC2_OUT_INV_SEL  (BIT(9))
#define GPIO_FUNC2_OUT_INV_SEL_M  (BIT(9))
#define GPIO_FUNC2_OUT_INV_SEL_V  0x1
#define GPIO_FUNC2_OUT_INV_SEL_S  9
/* GPIO_FUNC2_OUT_SEL : R/W ;bitpos:[8:0] ;default: x ; */
/*description: select one of the 256 output to 40 GPIO*/
#define GPIO_FUNC2_OUT_SEL  0x000001FF
#define GPIO_FUNC2_OUT_SEL_M  ((GPIO_FUNC2_OUT_SEL_V)<<(GPIO_FUNC2_OUT_SEL_S))
#define GPIO_FUNC2_OUT_SEL_V  0x1FF
#define GPIO_FUNC2_OUT_SEL_S  0

#define GPIO_FUNC3_OUT_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x053c)
/* GPIO_FUNC3_OEN_INV_SEL : R/W ;bitpos:[11] ;default: x ; */
/*description: invert the output enable value  if you want to revert the output
 enable value  setting the value to 1*/
#define GPIO_FUNC3_OEN_INV_SEL  (BIT(11))
#define GPIO_FUNC3_OEN_INV_SEL_M  (BIT(11))
#define GPIO_FUNC3_OEN_INV_SEL_V  0x1
#define GPIO_FUNC3_OEN_INV_SEL_S  11
/* GPIO_FUNC3_OEN_SEL : R/W ;bitpos:[10] ;default: x ; */
/*description: weather using the logical oen signal or not using the value setting
 by the register*/
#define GPIO_FUNC3_OEN_SEL  (BIT(10))
#define GPIO_FUNC3_OEN_SEL_M  (BIT(10))
#define GPIO_FUNC3_OEN_SEL_V  0x1
#define GPIO_FUNC3_OEN_SEL_S  10
/* GPIO_FUNC3_OUT_INV_SEL : R/W ;bitpos:[9] ;default: x ; */
/*description: invert the output value  if you want to revert the output value
  setting the value to 1*/
#define GPIO_FUNC3_OUT_INV_SEL  (BIT(9))
#define GPIO_FUNC3_OUT_INV_SEL_M  (BIT(9))
#define GPIO_FUNC3_OUT_INV_SEL_V  0x1
#define GPIO_FUNC3_OUT_INV_SEL_S  9
/* GPIO_FUNC3_OUT_SEL : R/W ;bitpos:[8:0] ;default: x ; */
/*description: select one of the 256 output to 40 GPIO*/
#define GPIO_FUNC3_OUT_SEL  0x000001FF
#define GPIO_FUNC3_OUT_SEL_M  ((GPIO_FUNC3_OUT_SEL_V)<<(GPIO_FUNC3_OUT_SEL_S))
#define GPIO_FUNC3_OUT_SEL_V  0x1FF
#define GPIO_FUNC3_OUT_SEL_S  0

#define GPIO_FUNC4_OUT_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x0540)
/* GPIO_FUNC4_OEN_INV_SEL : R/W ;bitpos:[11] ;default: x ; */
/*description: invert the output enable value  if you want to revert the output
 enable value  setting the value to 1*/
#define GPIO_FUNC4_OEN_INV_SEL  (BIT(11))
#define GPIO_FUNC4_OEN_INV_SEL_M  (BIT(11))
#define GPIO_FUNC4_OEN_INV_SEL_V  0x1
#define GPIO_FUNC4_OEN_INV_SEL_S  11
/* GPIO_FUNC4_OEN_SEL : R/W ;bitpos:[10] ;default: x ; */
/*description: weather using the logical oen signal or not using the value setting
 by the register*/
#define GPIO_FUNC4_OEN_SEL  (BIT(10))
#define GPIO_FUNC4_OEN_SEL_M  (BIT(10))
#define GPIO_FUNC4_OEN_SEL_V  0x1
#define GPIO_FUNC4_OEN_SEL_S  10
/* GPIO_FUNC4_OUT_INV_SEL : R/W ;bitpos:[9] ;default: x ; */
/*description: invert the output value  if you want to revert the output value
  setting the value to 1*/
#define GPIO_FUNC4_OUT_INV_SEL  (BIT(9))
#define GPIO_FUNC4_OUT_INV_SEL_M  (BIT(9))
#define GPIO_FUNC4_OUT_INV_SEL_V  0x1
#define GPIO_FUNC4_OUT_INV_SEL_S  9
/* GPIO_FUNC4_OUT_SEL : R/W ;bitpos:[8:0] ;default: x ; */
/*description: select one of the 256 output to 40 GPIO*/
#define GPIO_FUNC4_OUT_SEL  0x000001FF
#define GPIO_FUNC4_OUT_SEL_M  ((GPIO_FUNC4_OUT_SEL_V)<<(GPIO_FUNC4_OUT_SEL_S))
#define GPIO_FUNC4_OUT_SEL_V  0x1FF
#define GPIO_FUNC4_OUT_SEL_S  0

#define GPIO_FUNC5_OUT_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x0544)
/* GPIO_FUNC5_OEN_INV_SEL : R/W ;bitpos:[11] ;default: x ; */
/*description: invert the output enable value  if you want to revert the output
 enable value  setting the value to 1*/
#define GPIO_FUNC5_OEN_INV_SEL  (BIT(11))
#define GPIO_FUNC5_OEN_INV_SEL_M  (BIT(11))
#define GPIO_FUNC5_OEN_INV_SEL_V  0x1
#define GPIO_FUNC5_OEN_INV_SEL_S  11
/* GPIO_FUNC5_OEN_SEL : R/W ;bitpos:[10] ;default: x ; */
/*description: weather using the logical oen signal or not using the value setting
 by the register*/
#define GPIO_FUNC5_OEN_SEL  (BIT(10))
#define GPIO_FUNC5_OEN_SEL_M  (BIT(10))
#define GPIO_FUNC5_OEN_SEL_V  0x1
#define GPIO_FUNC5_OEN_SEL_S  10
/* GPIO_FUNC5_OUT_INV_SEL : R/W ;bitpos:[9] ;default: x ; */
/*description: invert the output value  if you want to revert the output value
  setting the value to 1*/
#define GPIO_FUNC5_OUT_INV_SEL  (BIT(9))
#define GPIO_FUNC5_OUT_INV_SEL_M  (BIT(9))
#define GPIO_FUNC5_OUT_INV_SEL_V  0x1
#define GPIO_FUNC5_OUT_INV_SEL_S  9
/* GPIO_FUNC5_OUT_SEL : R/W ;bitpos:[8:0] ;default: x ; */
/*description: select one of the 256 output to 40 GPIO*/
#define GPIO_FUNC5_OUT_SEL  0x000001FF
#define GPIO_FUNC5_OUT_SEL_M  ((GPIO_FUNC5_OUT_SEL_V)<<(GPIO_FUNC5_OUT_SEL_S))
#define GPIO_FUNC5_OUT_SEL_V  0x1FF
#define GPIO_FUNC5_OUT_SEL_S  0

#define GPIO_FUNC6_OUT_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x0548)
/* GPIO_FUNC6_OEN_INV_SEL : R/W ;bitpos:[11] ;default: x ; */
/*description: invert the output enable value  if you want to revert the output
 enable value  setting the value to 1*/
#define GPIO_FUNC6_OEN_INV_SEL  (BIT(11))
#define GPIO_FUNC6_OEN_INV_SEL_M  (BIT(11))
#define GPIO_FUNC6_OEN_INV_SEL_V  0x1
#define GPIO_FUNC6_OEN_INV_SEL_S  11
/* GPIO_FUNC6_OEN_SEL : R/W ;bitpos:[10] ;default: x ; */
/*description: weather using the logical oen signal or not using the value setting
 by the register*/
#define GPIO_FUNC6_OEN_SEL  (BIT(10))
#define GPIO_FUNC6_OEN_SEL_M  (BIT(10))
#define GPIO_FUNC6_OEN_SEL_V  0x1
#define GPIO_FUNC6_OEN_SEL_S  10
/* GPIO_FUNC6_OUT_INV_SEL : R/W ;bitpos:[9] ;default: x ; */
/*description: invert the output value  if you want to revert the output value
  setting the value to 1*/
#define GPIO_FUNC6_OUT_INV_SEL  (BIT(9))
#define GPIO_FUNC6_OUT_INV_SEL_M  (BIT(9))
#define GPIO_FUNC6_OUT_INV_SEL_V  0x1
#define GPIO_FUNC6_OUT_INV_SEL_S  9
/* GPIO_FUNC6_OUT_SEL : R/W ;bitpos:[8:0] ;default: x ; */
/*description: select one of the 256 output to 40 GPIO*/
#define GPIO_FUNC6_OUT_SEL  0x000001FF
#define GPIO_FUNC6_OUT_SEL_M  ((GPIO_FUNC6_OUT_SEL_V)<<(GPIO_FUNC6_OUT_SEL_S))
#define GPIO_FUNC6_OUT_SEL_V  0x1FF
#define GPIO_FUNC6_OUT_SEL_S  0

#define GPIO_FUNC7_OUT_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x054c)
/* GPIO_FUNC7_OEN_INV_SEL : R/W ;bitpos:[11] ;default: x ; */
/*description: invert the output enable value  if you want to revert the output
 enable value  setting the value to 1*/
#define GPIO_FUNC7_OEN_INV_SEL  (BIT(11))
#define GPIO_FUNC7_OEN_INV_SEL_M  (BIT(11))
#define GPIO_FUNC7_OEN_INV_SEL_V  0x1
#define GPIO_FUNC7_OEN_INV_SEL_S  11
/* GPIO_FUNC7_OEN_SEL : R/W ;bitpos:[10] ;default: x ; */
/*description: weather using the logical oen signal or not using the value setting
 by the register*/
#define GPIO_FUNC7_OEN_SEL  (BIT(10))
#define GPIO_FUNC7_OEN_SEL_M  (BIT(10))
#define GPIO_FUNC7_OEN_SEL_V  0x1
#define GPIO_FUNC7_OEN_SEL_S  10
/* GPIO_FUNC7_OUT_INV_SEL : R/W ;bitpos:[9] ;default: x ; */
/*description: invert the output value  if you want to revert the output value
  setting the value to 1*/
#define GPIO_FUNC7_OUT_INV_SEL  (BIT(9))
#define GPIO_FUNC7_OUT_INV_SEL_M  (BIT(9))
#define GPIO_FUNC7_OUT_INV_SEL_V  0x1
#define GPIO_FUNC7_OUT_INV_SEL_S  9
/* GPIO_FUNC7_OUT_SEL : R/W ;bitpos:[8:0] ;default: x ; */
/*description: select one of the 256 output to 40 GPIO*/
#define GPIO_FUNC7_OUT_SEL  0x000001FF
#define GPIO_FUNC7_OUT_SEL_M  ((GPIO_FUNC7_OUT_SEL_V)<<(GPIO_FUNC7_OUT_SEL_S))
#define GPIO_FUNC7_OUT_SEL_V  0x1FF
#define GPIO_FUNC7_OUT_SEL_S  0

#define GPIO_FUNC8_OUT_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x0550)
/* GPIO_FUNC8_OEN_INV_SEL : R/W ;bitpos:[11] ;default: x ; */
/*description: invert the output enable value  if you want to revert the output
 enable value  setting the value to 1*/
#define GPIO_FUNC8_OEN_INV_SEL  (BIT(11))
#define GPIO_FUNC8_OEN_INV_SEL_M  (BIT(11))
#define GPIO_FUNC8_OEN_INV_SEL_V  0x1
#define GPIO_FUNC8_OEN_INV_SEL_S  11
/* GPIO_FUNC8_OEN_SEL : R/W ;bitpos:[10] ;default: x ; */
/*description: weather using the logical oen signal or not using the value setting
 by the register*/
#define GPIO_FUNC8_OEN_SEL  (BIT(10))
#define GPIO_FUNC8_OEN_SEL_M  (BIT(10))
#define GPIO_FUNC8_OEN_SEL_V  0x1
#define GPIO_FUNC8_OEN_SEL_S  10
/* GPIO_FUNC8_OUT_INV_SEL : R/W ;bitpos:[9] ;default: x ; */
/*description: invert the output value  if you want to revert the output value
  setting the value to 1*/
#define GPIO_FUNC8_OUT_INV_SEL  (BIT(9))
#define GPIO_FUNC8_OUT_INV_SEL_M  (BIT(9))
#define GPIO_FUNC8_OUT_INV_SEL_V  0x1
#define GPIO_FUNC8_OUT_INV_SEL_S  9
/* GPIO_FUNC8_OUT_SEL : R/W ;bitpos:[8:0] ;default: x ; */
/*description: select one of the 256 output to 40 GPIO*/
#define GPIO_FUNC8_OUT_SEL  0x000001FF
#define GPIO_FUNC8_OUT_SEL_M  ((GPIO_FUNC8_OUT_SEL_V)<<(GPIO_FUNC8_OUT_SEL_S))
#define GPIO_FUNC8_OUT_SEL_V  0x1FF
#define GPIO_FUNC8_OUT_SEL_S  0

#define GPIO_FUNC9_OUT_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x0554)
/* GPIO_FUNC9_OEN_INV_SEL : R/W ;bitpos:[11] ;default: x ; */
/*description: invert the output enable value  if you want to revert the output
 enable value  setting the value to 1*/
#define GPIO_FUNC9_OEN_INV_SEL  (BIT(11))
#define GPIO_FUNC9_OEN_INV_SEL_M  (BIT(11))
#define GPIO_FUNC9_OEN_INV_SEL_V  0x1
#define GPIO_FUNC9_OEN_INV_SEL_S  11
/* GPIO_FUNC9_OEN_SEL : R/W ;bitpos:[10] ;default: x ; */
/*description: weather using the logical oen signal or not using the value setting
 by the register*/
#define GPIO_FUNC9_OEN_SEL  (BIT(10))
#define GPIO_FUNC9_OEN_SEL_M  (BIT(10))
#define GPIO_FUNC9_OEN_SEL_V  0x1
#define GPIO_FUNC9_OEN_SEL_S  10
/* GPIO_FUNC9_OUT_INV_SEL : R/W ;bitpos:[9] ;default: x ; */
/*description: invert the output value  if you want to revert the output value
  setting the value to 1*/
#define GPIO_FUNC9_OUT_INV_SEL  (BIT(9))
#define GPIO_FUNC9_OUT_INV_SEL_M  (BIT(9))
#define GPIO_FUNC9_OUT_INV_SEL_V  0x1
#define GPIO_FUNC9_OUT_INV_SEL_S  9
/* GPIO_FUNC9_OUT_SEL : R/W ;bitpos:[8:0] ;default: x ; */
/*description: select one of the 256 output to 40 GPIO*/
#define GPIO_FUNC9_OUT_SEL  0x000001FF
#define GPIO_FUNC9_OUT_SEL_M  ((GPIO_FUNC9_OUT_SEL_V)<<(GPIO_FUNC9_OUT_SEL_S))
#define GPIO_FUNC9_OUT_SEL_V  0x1FF
#define GPIO_FUNC9_OUT_SEL_S  0

#define GPIO_FUNC10_OUT_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x0558)
/* GPIO_FUNC10_OEN_INV_SEL : R/W ;bitpos:[11] ;default: x ; */
/*description: invert the output enable value  if you want to revert the output
 enable value  setting the value to 1*/
#define GPIO_FUNC10_OEN_INV_SEL  (BIT(11))
#define GPIO_FUNC10_OEN_INV_SEL_M  (BIT(11))
#define GPIO_FUNC10_OEN_INV_SEL_V  0x1
#define GPIO_FUNC10_OEN_INV_SEL_S  11
/* GPIO_FUNC10_OEN_SEL : R/W ;bitpos:[10] ;default: x ; */
/*description: weather using the logical oen signal or not using the value setting
 by the register*/
#define GPIO_FUNC10_OEN_SEL  (BIT(10))
#define GPIO_FUNC10_OEN_SEL_M  (BIT(10))
#define GPIO_FUNC10_OEN_SEL_V  0x1
#define GPIO_FUNC10_OEN_SEL_S  10
/* GPIO_FUNC10_OUT_INV_SEL : R/W ;bitpos:[9] ;default: x ; */
/*description: invert the output value  if you want to revert the output value
  setting the value to 1*/
#define GPIO_FUNC10_OUT_INV_SEL  (BIT(9))
#define GPIO_FUNC10_OUT_INV_SEL_M  (BIT(9))
#define GPIO_FUNC10_OUT_INV_SEL_V  0x1
#define GPIO_FUNC10_OUT_INV_SEL_S  9
/* GPIO_FUNC10_OUT_SEL : R/W ;bitpos:[8:0] ;default: x ; */
/*description: select one of the 256 output to 40 GPIO*/
#define GPIO_FUNC10_OUT_SEL  0x000001FF
#define GPIO_FUNC10_OUT_SEL_M  ((GPIO_FUNC10_OUT_SEL_V)<<(GPIO_FUNC10_OUT_SEL_S))
#define GPIO_FUNC10_OUT_SEL_V  0x1FF
#define GPIO_FUNC10_OUT_SEL_S  0

#define GPIO_FUNC11_OUT_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x055c)
/* GPIO_FUNC11_OEN_INV_SEL : R/W ;bitpos:[11] ;default: x ; */
/*description: invert the output enable value  if you want to revert the output
 enable value  setting the value to 1*/
#define GPIO_FUNC11_OEN_INV_SEL  (BIT(11))
#define GPIO_FUNC11_OEN_INV_SEL_M  (BIT(11))
#define GPIO_FUNC11_OEN_INV_SEL_V  0x1
#define GPIO_FUNC11_OEN_INV_SEL_S  11
/* GPIO_FUNC11_OEN_SEL : R/W ;bitpos:[10] ;default: x ; */
/*description: weather using the logical oen signal or not using the value setting
 by the register*/
#define GPIO_FUNC11_OEN_SEL  (BIT(10))
#define GPIO_FUNC11_OEN_SEL_M  (BIT(10))
#define GPIO_FUNC11_OEN_SEL_V  0x1
#define GPIO_FUNC11_OEN_SEL_S  10
/* GPIO_FUNC11_OUT_INV_SEL : R/W ;bitpos:[9] ;default: x ; */
/*description: invert the output value  if you want to revert the output value
  setting the value to 1*/
#define GPIO_FUNC11_OUT_INV_SEL  (BIT(9))
#define GPIO_FUNC11_OUT_INV_SEL_M  (BIT(9))
#define GPIO_FUNC11_OUT_INV_SEL_V  0x1
#define GPIO_FUNC11_OUT_INV_SEL_S  9
/* GPIO_FUNC11_OUT_SEL : R/W ;bitpos:[8:0] ;default: x ; */
/*description: select one of the 256 output to 40 GPIO*/
#define GPIO_FUNC11_OUT_SEL  0x000001FF
#define GPIO_FUNC11_OUT_SEL_M  ((GPIO_FUNC11_OUT_SEL_V)<<(GPIO_FUNC11_OUT_SEL_S))
#define GPIO_FUNC11_OUT_SEL_V  0x1FF
#define GPIO_FUNC11_OUT_SEL_S  0

#define GPIO_FUNC12_OUT_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x0560)
/* GPIO_FUNC12_OEN_INV_SEL : R/W ;bitpos:[11] ;default: x ; */
/*description: invert the output enable value  if you want to revert the output
 enable value  setting the value to 1*/
#define GPIO_FUNC12_OEN_INV_SEL  (BIT(11))
#define GPIO_FUNC12_OEN_INV_SEL_M  (BIT(11))
#define GPIO_FUNC12_OEN_INV_SEL_V  0x1
#define GPIO_FUNC12_OEN_INV_SEL_S  11
/* GPIO_FUNC12_OEN_SEL : R/W ;bitpos:[10] ;default: x ; */
/*description: weather using the logical oen signal or not using the value setting
 by the register*/
#define GPIO_FUNC12_OEN_SEL  (BIT(10))
#define GPIO_FUNC12_OEN_SEL_M  (BIT(10))
#define GPIO_FUNC12_OEN_SEL_V  0x1
#define GPIO_FUNC12_OEN_SEL_S  10
/* GPIO_FUNC12_OUT_INV_SEL : R/W ;bitpos:[9] ;default: x ; */
/*description: invert the output value  if you want to revert the output value
  setting the value to 1*/
#define GPIO_FUNC12_OUT_INV_SEL  (BIT(9))
#define GPIO_FUNC12_OUT_INV_SEL_M  (BIT(9))
#define GPIO_FUNC12_OUT_INV_SEL_V  0x1
#define GPIO_FUNC12_OUT_INV_SEL_S  9
/* GPIO_FUNC12_OUT_SEL : R/W ;bitpos:[8:0] ;default: x ; */
/*description: select one of the 256 output to 40 GPIO*/
#define GPIO_FUNC12_OUT_SEL  0x000001FF
#define GPIO_FUNC12_OUT_SEL_M  ((GPIO_FUNC12_OUT_SEL_V)<<(GPIO_FUNC12_OUT_SEL_S))
#define GPIO_FUNC12_OUT_SEL_V  0x1FF
#define GPIO_FUNC12_OUT_SEL_S  0

#define GPIO_FUNC13_OUT_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x0564)
/* GPIO_FUNC13_OEN_INV_SEL : R/W ;bitpos:[11] ;default: x ; */
/*description: invert the output enable value  if you want to revert the output
 enable value  setting the value to 1*/
#define GPIO_FUNC13_OEN_INV_SEL  (BIT(11))
#define GPIO_FUNC13_OEN_INV_SEL_M  (BIT(11))
#define GPIO_FUNC13_OEN_INV_SEL_V  0x1
#define GPIO_FUNC13_OEN_INV_SEL_S  11
/* GPIO_FUNC13_OEN_SEL : R/W ;bitpos:[10] ;default: x ; */
/*description: weather using the logical oen signal or not using the value setting
 by the register*/
#define GPIO_FUNC13_OEN_SEL  (BIT(10))
#define GPIO_FUNC13_OEN_SEL_M  (BIT(10))
#define GPIO_FUNC13_OEN_SEL_V  0x1
#define GPIO_FUNC13_OEN_SEL_S  10
/* GPIO_FUNC13_OUT_INV_SEL : R/W ;bitpos:[9] ;default: x ; */
/*description: invert the output value  if you want to revert the output value
  setting the value to 1*/
#define GPIO_FUNC13_OUT_INV_SEL  (BIT(9))
#define GPIO_FUNC13_OUT_INV_SEL_M  (BIT(9))
#define GPIO_FUNC13_OUT_INV_SEL_V  0x1
#define GPIO_FUNC13_OUT_INV_SEL_S  9
/* GPIO_FUNC13_OUT_SEL : R/W ;bitpos:[8:0] ;default: x ; */
/*description: select one of the 256 output to 40 GPIO*/
#define GPIO_FUNC13_OUT_SEL  0x000001FF
#define GPIO_FUNC13_OUT_SEL_M  ((GPIO_FUNC13_OUT_SEL_V)<<(GPIO_FUNC13_OUT_SEL_S))
#define GPIO_FUNC13_OUT_SEL_V  0x1FF
#define GPIO_FUNC13_OUT_SEL_S  0

#define GPIO_FUNC14_OUT_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x0568)
/* GPIO_FUNC14_OEN_INV_SEL : R/W ;bitpos:[11] ;default: x ; */
/*description: invert the output enable value  if you want to revert the output
 enable value  setting the value to 1*/
#define GPIO_FUNC14_OEN_INV_SEL  (BIT(11))
#define GPIO_FUNC14_OEN_INV_SEL_M  (BIT(11))
#define GPIO_FUNC14_OEN_INV_SEL_V  0x1
#define GPIO_FUNC14_OEN_INV_SEL_S  11
/* GPIO_FUNC14_OEN_SEL : R/W ;bitpos:[10] ;default: x ; */
/*description: weather using the logical oen signal or not using the value setting
 by the register*/
#define GPIO_FUNC14_OEN_SEL  (BIT(10))
#define GPIO_FUNC14_OEN_SEL_M  (BIT(10))
#define GPIO_FUNC14_OEN_SEL_V  0x1
#define GPIO_FUNC14_OEN_SEL_S  10
/* GPIO_FUNC14_OUT_INV_SEL : R/W ;bitpos:[9] ;default: x ; */
/*description: invert the output value  if you want to revert the output value
  setting the value to 1*/
#define GPIO_FUNC14_OUT_INV_SEL  (BIT(9))
#define GPIO_FUNC14_OUT_INV_SEL_M  (BIT(9))
#define GPIO_FUNC14_OUT_INV_SEL_V  0x1
#define GPIO_FUNC14_OUT_INV_SEL_S  9
/* GPIO_FUNC14_OUT_SEL : R/W ;bitpos:[8:0] ;default: x ; */
/*description: select one of the 256 output to 40 GPIO*/
#define GPIO_FUNC14_OUT_SEL  0x000001FF
#define GPIO_FUNC14_OUT_SEL_M  ((GPIO_FUNC14_OUT_SEL_V)<<(GPIO_FUNC14_OUT_SEL_S))
#define GPIO_FUNC14_OUT_SEL_V  0x1FF
#define GPIO_FUNC14_OUT_SEL_S  0

#define GPIO_FUNC15_OUT_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x056c)
/* GPIO_FUNC15_OEN_INV_SEL : R/W ;bitpos:[11] ;default: x ; */
/*description: invert the output enable value  if you want to revert the output
 enable value  setting the value to 1*/
#define GPIO_FUNC15_OEN_INV_SEL  (BIT(11))
#define GPIO_FUNC15_OEN_INV_SEL_M  (BIT(11))
#define GPIO_FUNC15_OEN_INV_SEL_V  0x1
#define GPIO_FUNC15_OEN_INV_SEL_S  11
/* GPIO_FUNC15_OEN_SEL : R/W ;bitpos:[10] ;default: x ; */
/*description: weather using the logical oen signal or not using the value setting
 by the register*/
#define GPIO_FUNC15_OEN_SEL  (BIT(10))
#define GPIO_FUNC15_OEN_SEL_M  (BIT(10))
#define GPIO_FUNC15_OEN_SEL_V  0x1
#define GPIO_FUNC15_OEN_SEL_S  10
/* GPIO_FUNC15_OUT_INV_SEL : R/W ;bitpos:[9] ;default: x ; */
/*description: invert the output value  if you want to revert the output value
  setting the value to 1*/
#define GPIO_FUNC15_OUT_INV_SEL  (BIT(9))
#define GPIO_FUNC15_OUT_INV_SEL_M  (BIT(9))
#define GPIO_FUNC15_OUT_INV_SEL_V  0x1
#define GPIO_FUNC15_OUT_INV_SEL_S  9
/* GPIO_FUNC15_OUT_SEL : R/W ;bitpos:[8:0] ;default: x ; */
/*description: select one of the 256 output to 40 GPIO*/
#define GPIO_FUNC15_OUT_SEL  0x000001FF
#define GPIO_FUNC15_OUT_SEL_M  ((GPIO_FUNC15_OUT_SEL_V)<<(GPIO_FUNC15_OUT_SEL_S))
#define GPIO_FUNC15_OUT_SEL_V  0x1FF
#define GPIO_FUNC15_OUT_SEL_S  0

#define GPIO_FUNC16_OUT_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x0570)
/* GPIO_FUNC16_OEN_INV_SEL : R/W ;bitpos:[11] ;default: x ; */
/*description: invert the output enable value  if you want to revert the output
 enable value  setting the value to 1*/
#define GPIO_FUNC16_OEN_INV_SEL  (BIT(11))
#define GPIO_FUNC16_OEN_INV_SEL_M  (BIT(11))
#define GPIO_FUNC16_OEN_INV_SEL_V  0x1
#define GPIO_FUNC16_OEN_INV_SEL_S  11
/* GPIO_FUNC16_OEN_SEL : R/W ;bitpos:[10] ;default: x ; */
/*description: weather using the logical oen signal or not using the value setting
 by the register*/
#define GPIO_FUNC16_OEN_SEL  (BIT(10))
#define GPIO_FUNC16_OEN_SEL_M  (BIT(10))
#define GPIO_FUNC16_OEN_SEL_V  0x1
#define GPIO_FUNC16_OEN_SEL_S  10
/* GPIO_FUNC16_OUT_INV_SEL : R/W ;bitpos:[9] ;default: x ; */
/*description: invert the output value  if you want to revert the output value
  setting the value to 1*/
#define GPIO_FUNC16_OUT_INV_SEL  (BIT(9))
#define GPIO_FUNC16_OUT_INV_SEL_M  (BIT(9))
#define GPIO_FUNC16_OUT_INV_SEL_V  0x1
#define GPIO_FUNC16_OUT_INV_SEL_S  9
/* GPIO_FUNC16_OUT_SEL : R/W ;bitpos:[8:0] ;default: x ; */
/*description: select one of the 256 output to 40 GPIO*/
#define GPIO_FUNC16_OUT_SEL  0x000001FF
#define GPIO_FUNC16_OUT_SEL_M  ((GPIO_FUNC16_OUT_SEL_V)<<(GPIO_FUNC16_OUT_SEL_S))
#define GPIO_FUNC16_OUT_SEL_V  0x1FF
#define GPIO_FUNC16_OUT_SEL_S  0

#define GPIO_FUNC17_OUT_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x0574)
/* GPIO_FUNC17_OEN_INV_SEL : R/W ;bitpos:[11] ;default: x ; */
/*description: invert the output enable value  if you want to revert the output
 enable value  setting the value to 1*/
#define GPIO_FUNC17_OEN_INV_SEL  (BIT(11))
#define GPIO_FUNC17_OEN_INV_SEL_M  (BIT(11))
#define GPIO_FUNC17_OEN_INV_SEL_V  0x1
#define GPIO_FUNC17_OEN_INV_SEL_S  11
/* GPIO_FUNC17_OEN_SEL : R/W ;bitpos:[10] ;default: x ; */
/*description: weather using the logical oen signal or not using the value setting
 by the register*/
#define GPIO_FUNC17_OEN_SEL  (BIT(10))
#define GPIO_FUNC17_OEN_SEL_M  (BIT(10))
#define GPIO_FUNC17_OEN_SEL_V  0x1
#define GPIO_FUNC17_OEN_SEL_S  10
/* GPIO_FUNC17_OUT_INV_SEL : R/W ;bitpos:[9] ;default: x ; */
/*description: invert the output value  if you want to revert the output value
  setting the value to 1*/
#define GPIO_FUNC17_OUT_INV_SEL  (BIT(9))
#define GPIO_FUNC17_OUT_INV_SEL_M  (BIT(9))
#define GPIO_FUNC17_OUT_INV_SEL_V  0x1
#define GPIO_FUNC17_OUT_INV_SEL_S  9
/* GPIO_FUNC17_OUT_SEL : R/W ;bitpos:[8:0] ;default: x ; */
/*description: select one of the 256 output to 40 GPIO*/
#define GPIO_FUNC17_OUT_SEL  0x000001FF
#define GPIO_FUNC17_OUT_SEL_M  ((GPIO_FUNC17_OUT_SEL_V)<<(GPIO_FUNC17_OUT_SEL_S))
#define GPIO_FUNC17_OUT_SEL_V  0x1FF
#define GPIO_FUNC17_OUT_SEL_S  0

#define GPIO_FUNC18_OUT_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x0578)
/* GPIO_FUNC18_OEN_INV_SEL : R/W ;bitpos:[11] ;default: x ; */
/*description: invert the output enable value  if you want to revert the output
 enable value  setting the value to 1*/
#define GPIO_FUNC18_OEN_INV_SEL  (BIT(11))
#define GPIO_FUNC18_OEN_INV_SEL_M  (BIT(11))
#define GPIO_FUNC18_OEN_INV_SEL_V  0x1
#define GPIO_FUNC18_OEN_INV_SEL_S  11
/* GPIO_FUNC18_OEN_SEL : R/W ;bitpos:[10] ;default: x ; */
/*description: weather using the logical oen signal or not using the value setting
 by the register*/
#define GPIO_FUNC18_OEN_SEL  (BIT(10))
#define GPIO_FUNC18_OEN_SEL_M  (BIT(10))
#define GPIO_FUNC18_OEN_SEL_V  0x1
#define GPIO_FUNC18_OEN_SEL_S  10
/* GPIO_FUNC18_OUT_INV_SEL : R/W ;bitpos:[9] ;default: x ; */
/*description: invert the output value  if you want to revert the output value
  setting the value to 1*/
#define GPIO_FUNC18_OUT_INV_SEL  (BIT(9))
#define GPIO_FUNC18_OUT_INV_SEL_M  (BIT(9))
#define GPIO_FUNC18_OUT_INV_SEL_V  0x1
#define GPIO_FUNC18_OUT_INV_SEL_S  9
/* GPIO_FUNC18_OUT_SEL : R/W ;bitpos:[8:0] ;default: x ; */
/*description: select one of the 256 output to 40 GPIO*/
#define GPIO_FUNC18_OUT_SEL  0x000001FF
#define GPIO_FUNC18_OUT_SEL_M  ((GPIO_FUNC18_OUT_SEL_V)<<(GPIO_FUNC18_OUT_SEL_S))
#define GPIO_FUNC18_OUT_SEL_V  0x1FF
#define GPIO_FUNC18_OUT_SEL_S  0

#define GPIO_FUNC19_OUT_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x057c)
/* GPIO_FUNC19_OEN_INV_SEL : R/W ;bitpos:[11] ;default: x ; */
/*description: invert the output enable value  if you want to revert the output
 enable value  setting the value to 1*/
#define GPIO_FUNC19_OEN_INV_SEL  (BIT(11))
#define GPIO_FUNC19_OEN_INV_SEL_M  (BIT(11))
#define GPIO_FUNC19_OEN_INV_SEL_V  0x1
#define GPIO_FUNC19_OEN_INV_SEL_S  11
/* GPIO_FUNC19_OEN_SEL : R/W ;bitpos:[10] ;default: x ; */
/*description: weather using the logical oen signal or not using the value setting
 by the register*/
#define GPIO_FUNC19_OEN_SEL  (BIT(10))
#define GPIO_FUNC19_OEN_SEL_M  (BIT(10))
#define GPIO_FUNC19_OEN_SEL_V  0x1
#define GPIO_FUNC19_OEN_SEL_S  10
/* GPIO_FUNC19_OUT_INV_SEL : R/W ;bitpos:[9] ;default: x ; */
/*description: invert the output value  if you want to revert the output value
  setting the value to 1*/
#define GPIO_FUNC19_OUT_INV_SEL  (BIT(9))
#define GPIO_FUNC19_OUT_INV_SEL_M  (BIT(9))
#define GPIO_FUNC19_OUT_INV_SEL_V  0x1
#define GPIO_FUNC19_OUT_INV_SEL_S  9
/* GPIO_FUNC19_OUT_SEL : R/W ;bitpos:[8:0] ;default: x ; */
/*description: select one of the 256 output to 40 GPIO*/
#define GPIO_FUNC19_OUT_SEL  0x000001FF
#define GPIO_FUNC19_OUT_SEL_M  ((GPIO_FUNC19_OUT_SEL_V)<<(GPIO_FUNC19_OUT_SEL_S))
#define GPIO_FUNC19_OUT_SEL_V  0x1FF
#define GPIO_FUNC19_OUT_SEL_S  0

#define GPIO_FUNC20_OUT_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x0580)
/* GPIO_FUNC20_OEN_INV_SEL : R/W ;bitpos:[11] ;default: x ; */
/*description: invert the output enable value  if you want to revert the output
 enable value  setting the value to 1*/
#define GPIO_FUNC20_OEN_INV_SEL  (BIT(11))
#define GPIO_FUNC20_OEN_INV_SEL_M  (BIT(11))
#define GPIO_FUNC20_OEN_INV_SEL_V  0x1
#define GPIO_FUNC20_OEN_INV_SEL_S  11
/* GPIO_FUNC20_OEN_SEL : R/W ;bitpos:[10] ;default: x ; */
/*description: weather using the logical oen signal or not using the value setting
 by the register*/
#define GPIO_FUNC20_OEN_SEL  (BIT(10))
#define GPIO_FUNC20_OEN_SEL_M  (BIT(10))
#define GPIO_FUNC20_OEN_SEL_V  0x1
#define GPIO_FUNC20_OEN_SEL_S  10
/* GPIO_FUNC20_OUT_INV_SEL : R/W ;bitpos:[9] ;default: x ; */
/*description: invert the output value  if you want to revert the output value
  setting the value to 1*/
#define GPIO_FUNC20_OUT_INV_SEL  (BIT(9))
#define GPIO_FUNC20_OUT_INV_SEL_M  (BIT(9))
#define GPIO_FUNC20_OUT_INV_SEL_V  0x1
#define GPIO_FUNC20_OUT_INV_SEL_S  9
/* GPIO_FUNC20_OUT_SEL : R/W ;bitpos:[8:0] ;default: x ; */
/*description: select one of the 256 output to 40 GPIO*/
#define GPIO_FUNC20_OUT_SEL  0x000001FF
#define GPIO_FUNC20_OUT_SEL_M  ((GPIO_FUNC20_OUT_SEL_V)<<(GPIO_FUNC20_OUT_SEL_S))
#define GPIO_FUNC20_OUT_SEL_V  0x1FF
#define GPIO_FUNC20_OUT_SEL_S  0

#define GPIO_FUNC21_OUT_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x0584)
/* GPIO_FUNC21_OEN_INV_SEL : R/W ;bitpos:[11] ;default: x ; */
/*description: invert the output enable value  if you want to revert the output
 enable value  setting the value to 1*/
#define GPIO_FUNC21_OEN_INV_SEL  (BIT(11))
#define GPIO_FUNC21_OEN_INV_SEL_M  (BIT(11))
#define GPIO_FUNC21_OEN_INV_SEL_V  0x1
#define GPIO_FUNC21_OEN_INV_SEL_S  11
/* GPIO_FUNC21_OEN_SEL : R/W ;bitpos:[10] ;default: x ; */
/*description: weather using the logical oen signal or not using the value setting
 by the register*/
#define GPIO_FUNC21_OEN_SEL  (BIT(10))
#define GPIO_FUNC21_OEN_SEL_M  (BIT(10))
#define GPIO_FUNC21_OEN_SEL_V  0x1
#define GPIO_FUNC21_OEN_SEL_S  10
/* GPIO_FUNC21_OUT_INV_SEL : R/W ;bitpos:[9] ;default: x ; */
/*description: invert the output value  if you want to revert the output value
  setting the value to 1*/
#define GPIO_FUNC21_OUT_INV_SEL  (BIT(9))
#define GPIO_FUNC21_OUT_INV_SEL_M  (BIT(9))
#define GPIO_FUNC21_OUT_INV_SEL_V  0x1
#define GPIO_FUNC21_OUT_INV_SEL_S  9
/* GPIO_FUNC21_OUT_SEL : R/W ;bitpos:[8:0] ;default: x ; */
/*description: select one of the 256 output to 40 GPIO*/
#define GPIO_FUNC21_OUT_SEL  0x000001FF
#define GPIO_FUNC21_OUT_SEL_M  ((GPIO_FUNC21_OUT_SEL_V)<<(GPIO_FUNC21_OUT_SEL_S))
#define GPIO_FUNC21_OUT_SEL_V  0x1FF
#define GPIO_FUNC21_OUT_SEL_S  0

#define GPIO_FUNC22_OUT_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x0588)
/* GPIO_FUNC22_OEN_INV_SEL : R/W ;bitpos:[11] ;default: x ; */
/*description: invert the output enable value  if you want to revert the output
 enable value  setting the value to 1*/
#define GPIO_FUNC22_OEN_INV_SEL  (BIT(11))
#define GPIO_FUNC22_OEN_INV_SEL_M  (BIT(11))
#define GPIO_FUNC22_OEN_INV_SEL_V  0x1
#define GPIO_FUNC22_OEN_INV_SEL_S  11
/* GPIO_FUNC22_OEN_SEL : R/W ;bitpos:[10] ;default: x ; */
/*description: weather using the logical oen signal or not using the value setting
 by the register*/
#define GPIO_FUNC22_OEN_SEL  (BIT(10))
#define GPIO_FUNC22_OEN_SEL_M  (BIT(10))
#define GPIO_FUNC22_OEN_SEL_V  0x1
#define GPIO_FUNC22_OEN_SEL_S  10
/* GPIO_FUNC22_OUT_INV_SEL : R/W ;bitpos:[9] ;default: x ; */
/*description: invert the output value  if you want to revert the output value
  setting the value to 1*/
#define GPIO_FUNC22_OUT_INV_SEL  (BIT(9))
#define GPIO_FUNC22_OUT_INV_SEL_M  (BIT(9))
#define GPIO_FUNC22_OUT_INV_SEL_V  0x1
#define GPIO_FUNC22_OUT_INV_SEL_S  9
/* GPIO_FUNC22_OUT_SEL : R/W ;bitpos:[8:0] ;default: x ; */
/*description: select one of the 256 output to 40 GPIO*/
#define GPIO_FUNC22_OUT_SEL  0x000001FF
#define GPIO_FUNC22_OUT_SEL_M  ((GPIO_FUNC22_OUT_SEL_V)<<(GPIO_FUNC22_OUT_SEL_S))
#define GPIO_FUNC22_OUT_SEL_V  0x1FF
#define GPIO_FUNC22_OUT_SEL_S  0

#define GPIO_FUNC23_OUT_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x058c)
/* GPIO_FUNC23_OEN_INV_SEL : R/W ;bitpos:[11] ;default: x ; */
/*description: invert the output enable value  if you want to revert the output
 enable value  setting the value to 1*/
#define GPIO_FUNC23_OEN_INV_SEL  (BIT(11))
#define GPIO_FUNC23_OEN_INV_SEL_M  (BIT(11))
#define GPIO_FUNC23_OEN_INV_SEL_V  0x1
#define GPIO_FUNC23_OEN_INV_SEL_S  11
/* GPIO_FUNC23_OEN_SEL : R/W ;bitpos:[10] ;default: x ; */
/*description: weather using the logical oen signal or not using the value setting
 by the register*/
#define GPIO_FUNC23_OEN_SEL  (BIT(10))
#define GPIO_FUNC23_OEN_SEL_M  (BIT(10))
#define GPIO_FUNC23_OEN_SEL_V  0x1
#define GPIO_FUNC23_OEN_SEL_S  10
/* GPIO_FUNC23_OUT_INV_SEL : R/W ;bitpos:[9] ;default: x ; */
/*description: invert the output value  if you want to revert the output value
  setting the value to 1*/
#define GPIO_FUNC23_OUT_INV_SEL  (BIT(9))
#define GPIO_FUNC23_OUT_INV_SEL_M  (BIT(9))
#define GPIO_FUNC23_OUT_INV_SEL_V  0x1
#define GPIO_FUNC23_OUT_INV_SEL_S  9
/* GPIO_FUNC23_OUT_SEL : R/W ;bitpos:[8:0] ;default: x ; */
/*description: select one of the 256 output to 40 GPIO*/
#define GPIO_FUNC23_OUT_SEL  0x000001FF
#define GPIO_FUNC23_OUT_SEL_M  ((GPIO_FUNC23_OUT_SEL_V)<<(GPIO_FUNC23_OUT_SEL_S))
#define GPIO_FUNC23_OUT_SEL_V  0x1FF
#define GPIO_FUNC23_OUT_SEL_S  0

#define GPIO_FUNC24_OUT_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x0590)
/* GPIO_FUNC24_OEN_INV_SEL : R/W ;bitpos:[11] ;default: x ; */
/*description: invert the output enable value  if you want to revert the output
 enable value  setting the value to 1*/
#define GPIO_FUNC24_OEN_INV_SEL  (BIT(11))
#define GPIO_FUNC24_OEN_INV_SEL_M  (BIT(11))
#define GPIO_FUNC24_OEN_INV_SEL_V  0x1
#define GPIO_FUNC24_OEN_INV_SEL_S  11
/* GPIO_FUNC24_OEN_SEL : R/W ;bitpos:[10] ;default: x ; */
/*description: weather using the logical oen signal or not using the value setting
 by the register*/
#define GPIO_FUNC24_OEN_SEL  (BIT(10))
#define GPIO_FUNC24_OEN_SEL_M  (BIT(10))
#define GPIO_FUNC24_OEN_SEL_V  0x1
#define GPIO_FUNC24_OEN_SEL_S  10
/* GPIO_FUNC24_OUT_INV_SEL : R/W ;bitpos:[9] ;default: x ; */
/*description: invert the output value  if you want to revert the output value
  setting the value to 1*/
#define GPIO_FUNC24_OUT_INV_SEL  (BIT(9))
#define GPIO_FUNC24_OUT_INV_SEL_M  (BIT(9))
#define GPIO_FUNC24_OUT_INV_SEL_V  0x1
#define GPIO_FUNC24_OUT_INV_SEL_S  9
/* GPIO_FUNC24_OUT_SEL : R/W ;bitpos:[8:0] ;default: x ; */
/*description: select one of the 256 output to 40 GPIO*/
#define GPIO_FUNC24_OUT_SEL  0x000001FF
#define GPIO_FUNC24_OUT_SEL_M  ((GPIO_FUNC24_OUT_SEL_V)<<(GPIO_FUNC24_OUT_SEL_S))
#define GPIO_FUNC24_OUT_SEL_V  0x1FF
#define GPIO_FUNC24_OUT_SEL_S  0

#define GPIO_FUNC25_OUT_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x0594)
/* GPIO_FUNC25_OEN_INV_SEL : R/W ;bitpos:[11] ;default: x ; */
/*description: invert the output enable value  if you want to revert the output
 enable value  setting the value to 1*/
#define GPIO_FUNC25_OEN_INV_SEL  (BIT(11))
#define GPIO_FUNC25_OEN_INV_SEL_M  (BIT(11))
#define GPIO_FUNC25_OEN_INV_SEL_V  0x1
#define GPIO_FUNC25_OEN_INV_SEL_S  11
/* GPIO_FUNC25_OEN_SEL : R/W ;bitpos:[10] ;default: x ; */
/*description: weather using the logical oen signal or not using the value setting
 by the register*/
#define GPIO_FUNC25_OEN_SEL  (BIT(10))
#define GPIO_FUNC25_OEN_SEL_M  (BIT(10))
#define GPIO_FUNC25_OEN_SEL_V  0x1
#define GPIO_FUNC25_OEN_SEL_S  10
/* GPIO_FUNC25_OUT_INV_SEL : R/W ;bitpos:[9] ;default: x ; */
/*description: invert the output value  if you want to revert the output value
  setting the value to 1*/
#define GPIO_FUNC25_OUT_INV_SEL  (BIT(9))
#define GPIO_FUNC25_OUT_INV_SEL_M  (BIT(9))
#define GPIO_FUNC25_OUT_INV_SEL_V  0x1
#define GPIO_FUNC25_OUT_INV_SEL_S  9
/* GPIO_FUNC25_OUT_SEL : R/W ;bitpos:[8:0] ;default: x ; */
/*description: select one of the 256 output to 40 GPIO*/
#define GPIO_FUNC25_OUT_SEL  0x000001FF
#define GPIO_FUNC25_OUT_SEL_M  ((GPIO_FUNC25_OUT_SEL_V)<<(GPIO_FUNC25_OUT_SEL_S))
#define GPIO_FUNC25_OUT_SEL_V  0x1FF
#define GPIO_FUNC25_OUT_SEL_S  0

#define GPIO_FUNC26_OUT_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x0598)
/* GPIO_FUNC26_OEN_INV_SEL : R/W ;bitpos:[11] ;default: x ; */
/*description: invert the output enable value  if you want to revert the output
 enable value  setting the value to 1*/
#define GPIO_FUNC26_OEN_INV_SEL  (BIT(11))
#define GPIO_FUNC26_OEN_INV_SEL_M  (BIT(11))
#define GPIO_FUNC26_OEN_INV_SEL_V  0x1
#define GPIO_FUNC26_OEN_INV_SEL_S  11
/* GPIO_FUNC26_OEN_SEL : R/W ;bitpos:[10] ;default: x ; */
/*description: weather using the logical oen signal or not using the value setting
 by the register*/
#define GPIO_FUNC26_OEN_SEL  (BIT(10))
#define GPIO_FUNC26_OEN_SEL_M  (BIT(10))
#define GPIO_FUNC26_OEN_SEL_V  0x1
#define GPIO_FUNC26_OEN_SEL_S  10
/* GPIO_FUNC26_OUT_INV_SEL : R/W ;bitpos:[9] ;default: x ; */
/*description: invert the output value  if you want to revert the output value
  setting the value to 1*/
#define GPIO_FUNC26_OUT_INV_SEL  (BIT(9))
#define GPIO_FUNC26_OUT_INV_SEL_M  (BIT(9))
#define GPIO_FUNC26_OUT_INV_SEL_V  0x1
#define GPIO_FUNC26_OUT_INV_SEL_S  9
/* GPIO_FUNC26_OUT_SEL : R/W ;bitpos:[8:0] ;default: x ; */
/*description: select one of the 256 output to 40 GPIO*/
#define GPIO_FUNC26_OUT_SEL  0x000001FF
#define GPIO_FUNC26_OUT_SEL_M  ((GPIO_FUNC26_OUT_SEL_V)<<(GPIO_FUNC26_OUT_SEL_S))
#define GPIO_FUNC26_OUT_SEL_V  0x1FF
#define GPIO_FUNC26_OUT_SEL_S  0

#define GPIO_FUNC27_OUT_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x059c)
/* GPIO_FUNC27_OEN_INV_SEL : R/W ;bitpos:[11] ;default: x ; */
/*description: invert the output enable value  if you want to revert the output
 enable value  setting the value to 1*/
#define GPIO_FUNC27_OEN_INV_SEL  (BIT(11))
#define GPIO_FUNC27_OEN_INV_SEL_M  (BIT(11))
#define GPIO_FUNC27_OEN_INV_SEL_V  0x1
#define GPIO_FUNC27_OEN_INV_SEL_S  11
/* GPIO_FUNC27_OEN_SEL : R/W ;bitpos:[10] ;default: x ; */
/*description: weather using the logical oen signal or not using the value setting
 by the register*/
#define GPIO_FUNC27_OEN_SEL  (BIT(10))
#define GPIO_FUNC27_OEN_SEL_M  (BIT(10))
#define GPIO_FUNC27_OEN_SEL_V  0x1
#define GPIO_FUNC27_OEN_SEL_S  10
/* GPIO_FUNC27_OUT_INV_SEL : R/W ;bitpos:[9] ;default: x ; */
/*description: invert the output value  if you want to revert the output value
  setting the value to 1*/
#define GPIO_FUNC27_OUT_INV_SEL  (BIT(9))
#define GPIO_FUNC27_OUT_INV_SEL_M  (BIT(9))
#define GPIO_FUNC27_OUT_INV_SEL_V  0x1
#define GPIO_FUNC27_OUT_INV_SEL_S  9
/* GPIO_FUNC27_OUT_SEL : R/W ;bitpos:[8:0] ;default: x ; */
/*description: select one of the 256 output to 40 GPIO*/
#define GPIO_FUNC27_OUT_SEL  0x000001FF
#define GPIO_FUNC27_OUT_SEL_M  ((GPIO_FUNC27_OUT_SEL_V)<<(GPIO_FUNC27_OUT_SEL_S))
#define GPIO_FUNC27_OUT_SEL_V  0x1FF
#define GPIO_FUNC27_OUT_SEL_S  0

#define GPIO_FUNC28_OUT_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x05a0)
/* GPIO_FUNC28_OEN_INV_SEL : R/W ;bitpos:[11] ;default: x ; */
/*description: invert the output enable value  if you want to revert the output
 enable value  setting the value to 1*/
#define GPIO_FUNC28_OEN_INV_SEL  (BIT(11))
#define GPIO_FUNC28_OEN_INV_SEL_M  (BIT(11))
#define GPIO_FUNC28_OEN_INV_SEL_V  0x1
#define GPIO_FUNC28_OEN_INV_SEL_S  11
/* GPIO_FUNC28_OEN_SEL : R/W ;bitpos:[10] ;default: x ; */
/*description: weather using the logical oen signal or not using the value setting
 by the register*/
#define GPIO_FUNC28_OEN_SEL  (BIT(10))
#define GPIO_FUNC28_OEN_SEL_M  (BIT(10))
#define GPIO_FUNC28_OEN_SEL_V  0x1
#define GPIO_FUNC28_OEN_SEL_S  10
/* GPIO_FUNC28_OUT_INV_SEL : R/W ;bitpos:[9] ;default: x ; */
/*description: invert the output value  if you want to revert the output value
  setting the value to 1*/
#define GPIO_FUNC28_OUT_INV_SEL  (BIT(9))
#define GPIO_FUNC28_OUT_INV_SEL_M  (BIT(9))
#define GPIO_FUNC28_OUT_INV_SEL_V  0x1
#define GPIO_FUNC28_OUT_INV_SEL_S  9
/* GPIO_FUNC28_OUT_SEL : R/W ;bitpos:[8:0] ;default: x ; */
/*description: select one of the 256 output to 40 GPIO*/
#define GPIO_FUNC28_OUT_SEL  0x000001FF
#define GPIO_FUNC28_OUT_SEL_M  ((GPIO_FUNC28_OUT_SEL_V)<<(GPIO_FUNC28_OUT_SEL_S))
#define GPIO_FUNC28_OUT_SEL_V  0x1FF
#define GPIO_FUNC28_OUT_SEL_S  0

#define GPIO_FUNC29_OUT_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x05a4)
/* GPIO_FUNC29_OEN_INV_SEL : R/W ;bitpos:[11] ;default: x ; */
/*description: invert the output enable value  if you want to revert the output
 enable value  setting the value to 1*/
#define GPIO_FUNC29_OEN_INV_SEL  (BIT(11))
#define GPIO_FUNC29_OEN_INV_SEL_M  (BIT(11))
#define GPIO_FUNC29_OEN_INV_SEL_V  0x1
#define GPIO_FUNC29_OEN_INV_SEL_S  11
/* GPIO_FUNC29_OEN_SEL : R/W ;bitpos:[10] ;default: x ; */
/*description: weather using the logical oen signal or not using the value setting
 by the register*/
#define GPIO_FUNC29_OEN_SEL  (BIT(10))
#define GPIO_FUNC29_OEN_SEL_M  (BIT(10))
#define GPIO_FUNC29_OEN_SEL_V  0x1
#define GPIO_FUNC29_OEN_SEL_S  10
/* GPIO_FUNC29_OUT_INV_SEL : R/W ;bitpos:[9] ;default: x ; */
/*description: invert the output value  if you want to revert the output value
  setting the value to 1*/
#define GPIO_FUNC29_OUT_INV_SEL  (BIT(9))
#define GPIO_FUNC29_OUT_INV_SEL_M  (BIT(9))
#define GPIO_FUNC29_OUT_INV_SEL_V  0x1
#define GPIO_FUNC29_OUT_INV_SEL_S  9
/* GPIO_FUNC29_OUT_SEL : R/W ;bitpos:[8:0] ;default: x ; */
/*description: select one of the 256 output to 40 GPIO*/
#define GPIO_FUNC29_OUT_SEL  0x000001FF
#define GPIO_FUNC29_OUT_SEL_M  ((GPIO_FUNC29_OUT_SEL_V)<<(GPIO_FUNC29_OUT_SEL_S))
#define GPIO_FUNC29_OUT_SEL_V  0x1FF
#define GPIO_FUNC29_OUT_SEL_S  0

#define GPIO_FUNC30_OUT_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x05a8)
/* GPIO_FUNC30_OEN_INV_SEL : R/W ;bitpos:[11] ;default: x ; */
/*description: invert the output enable value  if you want to revert the output
 enable value  setting the value to 1*/
#define GPIO_FUNC30_OEN_INV_SEL  (BIT(11))
#define GPIO_FUNC30_OEN_INV_SEL_M  (BIT(11))
#define GPIO_FUNC30_OEN_INV_SEL_V  0x1
#define GPIO_FUNC30_OEN_INV_SEL_S  11
/* GPIO_FUNC30_OEN_SEL : R/W ;bitpos:[10] ;default: x ; */
/*description: weather using the logical oen signal or not using the value setting
 by the register*/
#define GPIO_FUNC30_OEN_SEL  (BIT(10))
#define GPIO_FUNC30_OEN_SEL_M  (BIT(10))
#define GPIO_FUNC30_OEN_SEL_V  0x1
#define GPIO_FUNC30_OEN_SEL_S  10
/* GPIO_FUNC30_OUT_INV_SEL : R/W ;bitpos:[9] ;default: x ; */
/*description: invert the output value  if you want to revert the output value
  setting the value to 1*/
#define GPIO_FUNC30_OUT_INV_SEL  (BIT(9))
#define GPIO_FUNC30_OUT_INV_SEL_M  (BIT(9))
#define GPIO_FUNC30_OUT_INV_SEL_V  0x1
#define GPIO_FUNC30_OUT_INV_SEL_S  9
/* GPIO_FUNC30_OUT_SEL : R/W ;bitpos:[8:0] ;default: x ; */
/*description: select one of the 256 output to 40 GPIO*/
#define GPIO_FUNC30_OUT_SEL  0x000001FF
#define GPIO_FUNC30_OUT_SEL_M  ((GPIO_FUNC30_OUT_SEL_V)<<(GPIO_FUNC30_OUT_SEL_S))
#define GPIO_FUNC30_OUT_SEL_V  0x1FF
#define GPIO_FUNC30_OUT_SEL_S  0

#define GPIO_FUNC31_OUT_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x05ac)
/* GPIO_FUNC31_OEN_INV_SEL : R/W ;bitpos:[11] ;default: x ; */
/*description: invert the output enable value  if you want to revert the output
 enable value  setting the value to 1*/
#define GPIO_FUNC31_OEN_INV_SEL  (BIT(11))
#define GPIO_FUNC31_OEN_INV_SEL_M  (BIT(11))
#define GPIO_FUNC31_OEN_INV_SEL_V  0x1
#define GPIO_FUNC31_OEN_INV_SEL_S  11
/* GPIO_FUNC31_OEN_SEL : R/W ;bitpos:[10] ;default: x ; */
/*description: weather using the logical oen signal or not using the value setting
 by the register*/
#define GPIO_FUNC31_OEN_SEL  (BIT(10))
#define GPIO_FUNC31_OEN_SEL_M  (BIT(10))
#define GPIO_FUNC31_OEN_SEL_V  0x1
#define GPIO_FUNC31_OEN_SEL_S  10
/* GPIO_FUNC31_OUT_INV_SEL : R/W ;bitpos:[9] ;default: x ; */
/*description: invert the output value  if you want to revert the output value
  setting the value to 1*/
#define GPIO_FUNC31_OUT_INV_SEL  (BIT(9))
#define GPIO_FUNC31_OUT_INV_SEL_M  (BIT(9))
#define GPIO_FUNC31_OUT_INV_SEL_V  0x1
#define GPIO_FUNC31_OUT_INV_SEL_S  9
/* GPIO_FUNC31_OUT_SEL : R/W ;bitpos:[8:0] ;default: x ; */
/*description: select one of the 256 output to 40 GPIO*/
#define GPIO_FUNC31_OUT_SEL  0x000001FF
#define GPIO_FUNC31_OUT_SEL_M  ((GPIO_FUNC31_OUT_SEL_V)<<(GPIO_FUNC31_OUT_SEL_S))
#define GPIO_FUNC31_OUT_SEL_V  0x1FF
#define GPIO_FUNC31_OUT_SEL_S  0

#define GPIO_FUNC32_OUT_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x05b0)
/* GPIO_FUNC32_OEN_INV_SEL : R/W ;bitpos:[11] ;default: x ; */
/*description: invert the output enable value  if you want to revert the output
 enable value  setting the value to 1*/
#define GPIO_FUNC32_OEN_INV_SEL  (BIT(11))
#define GPIO_FUNC32_OEN_INV_SEL_M  (BIT(11))
#define GPIO_FUNC32_OEN_INV_SEL_V  0x1
#define GPIO_FUNC32_OEN_INV_SEL_S  11
/* GPIO_FUNC32_OEN_SEL : R/W ;bitpos:[10] ;default: x ; */
/*description: weather using the logical oen signal or not using the value setting
 by the register*/
#define GPIO_FUNC32_OEN_SEL  (BIT(10))
#define GPIO_FUNC32_OEN_SEL_M  (BIT(10))
#define GPIO_FUNC32_OEN_SEL_V  0x1
#define GPIO_FUNC32_OEN_SEL_S  10
/* GPIO_FUNC32_OUT_INV_SEL : R/W ;bitpos:[9] ;default: x ; */
/*description: invert the output value  if you want to revert the output value
  setting the value to 1*/
#define GPIO_FUNC32_OUT_INV_SEL  (BIT(9))
#define GPIO_FUNC32_OUT_INV_SEL_M  (BIT(9))
#define GPIO_FUNC32_OUT_INV_SEL_V  0x1
#define GPIO_FUNC32_OUT_INV_SEL_S  9
/* GPIO_FUNC32_OUT_SEL : R/W ;bitpos:[8:0] ;default: x ; */
/*description: select one of the 256 output to 40 GPIO*/
#define GPIO_FUNC32_OUT_SEL  0x000001FF
#define GPIO_FUNC32_OUT_SEL_M  ((GPIO_FUNC32_OUT_SEL_V)<<(GPIO_FUNC32_OUT_SEL_S))
#define GPIO_FUNC32_OUT_SEL_V  0x1FF
#define GPIO_FUNC32_OUT_SEL_S  0

#define GPIO_FUNC33_OUT_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x05b4)
/* GPIO_FUNC33_OEN_INV_SEL : R/W ;bitpos:[11] ;default: x ; */
/*description: invert the output enable value  if you want to revert the output
 enable value  setting the value to 1*/
#define GPIO_FUNC33_OEN_INV_SEL  (BIT(11))
#define GPIO_FUNC33_OEN_INV_SEL_M  (BIT(11))
#define GPIO_FUNC33_OEN_INV_SEL_V  0x1
#define GPIO_FUNC33_OEN_INV_SEL_S  11
/* GPIO_FUNC33_OEN_SEL : R/W ;bitpos:[10] ;default: x ; */
/*description: weather using the logical oen signal or not using the value setting
 by the register*/
#define GPIO_FUNC33_OEN_SEL  (BIT(10))
#define GPIO_FUNC33_OEN_SEL_M  (BIT(10))
#define GPIO_FUNC33_OEN_SEL_V  0x1
#define GPIO_FUNC33_OEN_SEL_S  10
/* GPIO_FUNC33_OUT_INV_SEL : R/W ;bitpos:[9] ;default: x ; */
/*description: invert the output value  if you want to revert the output value
  setting the value to 1*/
#define GPIO_FUNC33_OUT_INV_SEL  (BIT(9))
#define GPIO_FUNC33_OUT_INV_SEL_M  (BIT(9))
#define GPIO_FUNC33_OUT_INV_SEL_V  0x1
#define GPIO_FUNC33_OUT_INV_SEL_S  9
/* GPIO_FUNC33_OUT_SEL : R/W ;bitpos:[8:0] ;default: x ; */
/*description: select one of the 256 output to 40 GPIO*/
#define GPIO_FUNC33_OUT_SEL  0x000001FF
#define GPIO_FUNC33_OUT_SEL_M  ((GPIO_FUNC33_OUT_SEL_V)<<(GPIO_FUNC33_OUT_SEL_S))
#define GPIO_FUNC33_OUT_SEL_V  0x1FF
#define GPIO_FUNC33_OUT_SEL_S  0

#define GPIO_FUNC34_OUT_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x05b8)
/* GPIO_FUNC34_OEN_INV_SEL : R/W ;bitpos:[11] ;default: x ; */
/*description: invert the output enable value  if you want to revert the output
 enable value  setting the value to 1*/
#define GPIO_FUNC34_OEN_INV_SEL  (BIT(11))
#define GPIO_FUNC34_OEN_INV_SEL_M  (BIT(11))
#define GPIO_FUNC34_OEN_INV_SEL_V  0x1
#define GPIO_FUNC34_OEN_INV_SEL_S  11
/* GPIO_FUNC34_OEN_SEL : R/W ;bitpos:[10] ;default: x ; */
/*description: weather using the logical oen signal or not using the value setting
 by the register*/
#define GPIO_FUNC34_OEN_SEL  (BIT(10))
#define GPIO_FUNC34_OEN_SEL_M  (BIT(10))
#define GPIO_FUNC34_OEN_SEL_V  0x1
#define GPIO_FUNC34_OEN_SEL_S  10
/* GPIO_FUNC34_OUT_INV_SEL : R/W ;bitpos:[9] ;default: x ; */
/*description: invert the output value  if you want to revert the output value
  setting the value to 1*/
#define GPIO_FUNC34_OUT_INV_SEL  (BIT(9))
#define GPIO_FUNC34_OUT_INV_SEL_M  (BIT(9))
#define GPIO_FUNC34_OUT_INV_SEL_V  0x1
#define GPIO_FUNC34_OUT_INV_SEL_S  9
/* GPIO_FUNC34_OUT_SEL : R/W ;bitpos:[8:0] ;default: x ; */
/*description: select one of the 256 output to 40 GPIO*/
#define GPIO_FUNC34_OUT_SEL  0x000001FF
#define GPIO_FUNC34_OUT_SEL_M  ((GPIO_FUNC34_OUT_SEL_V)<<(GPIO_FUNC34_OUT_SEL_S))
#define GPIO_FUNC34_OUT_SEL_V  0x1FF
#define GPIO_FUNC34_OUT_SEL_S  0

#define GPIO_FUNC35_OUT_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x05bc)
/* GPIO_FUNC35_OEN_INV_SEL : R/W ;bitpos:[11] ;default: x ; */
/*description: invert the output enable value  if you want to revert the output
 enable value  setting the value to 1*/
#define GPIO_FUNC35_OEN_INV_SEL  (BIT(11))
#define GPIO_FUNC35_OEN_INV_SEL_M  (BIT(11))
#define GPIO_FUNC35_OEN_INV_SEL_V  0x1
#define GPIO_FUNC35_OEN_INV_SEL_S  11
/* GPIO_FUNC35_OEN_SEL : R/W ;bitpos:[10] ;default: x ; */
/*description: weather using the logical oen signal or not using the value setting
 by the register*/
#define GPIO_FUNC35_OEN_SEL  (BIT(10))
#define GPIO_FUNC35_OEN_SEL_M  (BIT(10))
#define GPIO_FUNC35_OEN_SEL_V  0x1
#define GPIO_FUNC35_OEN_SEL_S  10
/* GPIO_FUNC35_OUT_INV_SEL : R/W ;bitpos:[9] ;default: x ; */
/*description: invert the output value  if you want to revert the output value
  setting the value to 1*/
#define GPIO_FUNC35_OUT_INV_SEL  (BIT(9))
#define GPIO_FUNC35_OUT_INV_SEL_M  (BIT(9))
#define GPIO_FUNC35_OUT_INV_SEL_V  0x1
#define GPIO_FUNC35_OUT_INV_SEL_S  9
/* GPIO_FUNC35_OUT_SEL : R/W ;bitpos:[8:0] ;default: x ; */
/*description: select one of the 256 output to 40 GPIO*/
#define GPIO_FUNC35_OUT_SEL  0x000001FF
#define GPIO_FUNC35_OUT_SEL_M  ((GPIO_FUNC35_OUT_SEL_V)<<(GPIO_FUNC35_OUT_SEL_S))
#define GPIO_FUNC35_OUT_SEL_V  0x1FF
#define GPIO_FUNC35_OUT_SEL_S  0

#define GPIO_FUNC36_OUT_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x05c0)
/* GPIO_FUNC36_OEN_INV_SEL : R/W ;bitpos:[11] ;default: x ; */
/*description: invert the output enable value  if you want to revert the output
 enable value  setting the value to 1*/
#define GPIO_FUNC36_OEN_INV_SEL  (BIT(11))
#define GPIO_FUNC36_OEN_INV_SEL_M  (BIT(11))
#define GPIO_FUNC36_OEN_INV_SEL_V  0x1
#define GPIO_FUNC36_OEN_INV_SEL_S  11
/* GPIO_FUNC36_OEN_SEL : R/W ;bitpos:[10] ;default: x ; */
/*description: weather using the logical oen signal or not using the value setting
 by the register*/
#define GPIO_FUNC36_OEN_SEL  (BIT(10))
#define GPIO_FUNC36_OEN_SEL_M  (BIT(10))
#define GPIO_FUNC36_OEN_SEL_V  0x1
#define GPIO_FUNC36_OEN_SEL_S  10
/* GPIO_FUNC36_OUT_INV_SEL : R/W ;bitpos:[9] ;default: x ; */
/*description: invert the output value  if you want to revert the output value
  setting the value to 1*/
#define GPIO_FUNC36_OUT_INV_SEL  (BIT(9))
#define GPIO_FUNC36_OUT_INV_SEL_M  (BIT(9))
#define GPIO_FUNC36_OUT_INV_SEL_V  0x1
#define GPIO_FUNC36_OUT_INV_SEL_S  9
/* GPIO_FUNC36_OUT_SEL : R/W ;bitpos:[8:0] ;default: x ; */
/*description: select one of the 256 output to 40 GPIO*/
#define GPIO_FUNC36_OUT_SEL  0x000001FF
#define GPIO_FUNC36_OUT_SEL_M  ((GPIO_FUNC36_OUT_SEL_V)<<(GPIO_FUNC36_OUT_SEL_S))
#define GPIO_FUNC36_OUT_SEL_V  0x1FF
#define GPIO_FUNC36_OUT_SEL_S  0

#define GPIO_FUNC37_OUT_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x05c4)
/* GPIO_FUNC37_OEN_INV_SEL : R/W ;bitpos:[11] ;default: x ; */
/*description: invert the output enable value  if you want to revert the output
 enable value  setting the value to 1*/
#define GPIO_FUNC37_OEN_INV_SEL  (BIT(11))
#define GPIO_FUNC37_OEN_INV_SEL_M  (BIT(11))
#define GPIO_FUNC37_OEN_INV_SEL_V  0x1
#define GPIO_FUNC37_OEN_INV_SEL_S  11
/* GPIO_FUNC37_OEN_SEL : R/W ;bitpos:[10] ;default: x ; */
/*description: weather using the logical oen signal or not using the value setting
 by the register*/
#define GPIO_FUNC37_OEN_SEL  (BIT(10))
#define GPIO_FUNC37_OEN_SEL_M  (BIT(10))
#define GPIO_FUNC37_OEN_SEL_V  0x1
#define GPIO_FUNC37_OEN_SEL_S  10
/* GPIO_FUNC37_OUT_INV_SEL : R/W ;bitpos:[9] ;default: x ; */
/*description: invert the output value  if you want to revert the output value
  setting the value to 1*/
#define GPIO_FUNC37_OUT_INV_SEL  (BIT(9))
#define GPIO_FUNC37_OUT_INV_SEL_M  (BIT(9))
#define GPIO_FUNC37_OUT_INV_SEL_V  0x1
#define GPIO_FUNC37_OUT_INV_SEL_S  9
/* GPIO_FUNC37_OUT_SEL : R/W ;bitpos:[8:0] ;default: x ; */
/*description: select one of the 256 output to 40 GPIO*/
#define GPIO_FUNC37_OUT_SEL  0x000001FF
#define GPIO_FUNC37_OUT_SEL_M  ((GPIO_FUNC37_OUT_SEL_V)<<(GPIO_FUNC37_OUT_SEL_S))
#define GPIO_FUNC37_OUT_SEL_V  0x1FF
#define GPIO_FUNC37_OUT_SEL_S  0

#define GPIO_FUNC38_OUT_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x05c8)
/* GPIO_FUNC38_OEN_INV_SEL : R/W ;bitpos:[11] ;default: x ; */
/*description: invert the output enable value  if you want to revert the output
 enable value  setting the value to 1*/
#define GPIO_FUNC38_OEN_INV_SEL  (BIT(11))
#define GPIO_FUNC38_OEN_INV_SEL_M  (BIT(11))
#define GPIO_FUNC38_OEN_INV_SEL_V  0x1
#define GPIO_FUNC38_OEN_INV_SEL_S  11
/* GPIO_FUNC38_OEN_SEL : R/W ;bitpos:[10] ;default: x ; */
/*description: weather using the logical oen signal or not using the value setting
 by the register*/
#define GPIO_FUNC38_OEN_SEL  (BIT(10))
#define GPIO_FUNC38_OEN_SEL_M  (BIT(10))
#define GPIO_FUNC38_OEN_SEL_V  0x1
#define GPIO_FUNC38_OEN_SEL_S  10
/* GPIO_FUNC38_OUT_INV_SEL : R/W ;bitpos:[9] ;default: x ; */
/*description: invert the output value  if you want to revert the output value
  setting the value to 1*/
#define GPIO_FUNC38_OUT_INV_SEL  (BIT(9))
#define GPIO_FUNC38_OUT_INV_SEL_M  (BIT(9))
#define GPIO_FUNC38_OUT_INV_SEL_V  0x1
#define GPIO_FUNC38_OUT_INV_SEL_S  9
/* GPIO_FUNC38_OUT_SEL : R/W ;bitpos:[8:0] ;default: x ; */
/*description: select one of the 256 output to 40 GPIO*/
#define GPIO_FUNC38_OUT_SEL  0x000001FF
#define GPIO_FUNC38_OUT_SEL_M  ((GPIO_FUNC38_OUT_SEL_V)<<(GPIO_FUNC38_OUT_SEL_S))
#define GPIO_FUNC38_OUT_SEL_V  0x1FF
#define GPIO_FUNC38_OUT_SEL_S  0

#define GPIO_FUNC39_OUT_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x05cc)
/* GPIO_FUNC39_OEN_INV_SEL : R/W ;bitpos:[11] ;default: x ; */
/*description: invert the output enable value  if you want to revert the output
 enable value  setting the value to 1*/
#define GPIO_FUNC39_OEN_INV_SEL  (BIT(11))
#define GPIO_FUNC39_OEN_INV_SEL_M  (BIT(11))
#define GPIO_FUNC39_OEN_INV_SEL_V  0x1
#define GPIO_FUNC39_OEN_INV_SEL_S  11
/* GPIO_FUNC39_OEN_SEL : R/W ;bitpos:[10] ;default: x ; */
/*description: weather using the logical oen signal or not using the value setting
 by the register*/
#define GPIO_FUNC39_OEN_SEL  (BIT(10))
#define GPIO_FUNC39_OEN_SEL_M  (BIT(10))
#define GPIO_FUNC39_OEN_SEL_V  0x1
#define GPIO_FUNC39_OEN_SEL_S  10
/* GPIO_FUNC39_OUT_INV_SEL : R/W ;bitpos:[9] ;default: x ; */
/*description: invert the output value  if you want to revert the output value
  setting the value to 1*/
#define GPIO_FUNC39_OUT_INV_SEL  (BIT(9))
#define GPIO_FUNC39_OUT_INV_SEL_M  (BIT(9))
#define GPIO_FUNC39_OUT_INV_SEL_V  0x1
#define GPIO_FUNC39_OUT_INV_SEL_S  9
/* GPIO_FUNC39_OUT_SEL : R/W ;bitpos:[8:0] ;default: x ; */
/*description: select one of the 256 output to 40 GPIO*/
#define GPIO_FUNC39_OUT_SEL  0x000001FF
#define GPIO_FUNC39_OUT_SEL_M  ((GPIO_FUNC39_OUT_SEL_V)<<(GPIO_FUNC39_OUT_SEL_S))
#define GPIO_FUNC39_OUT_SEL_V  0x1FF
#define GPIO_FUNC39_OUT_SEL_S  0




#endif /*_SOC_GPIO_REG_H_ */
