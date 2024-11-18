/*
 * SPDX-FileCopyrightText: 2017-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef _SOC_RTC_IO_REG_H_
#define _SOC_RTC_IO_REG_H_


#include "soc/soc.h"
#ifdef __cplusplus
extern "C" {
#endif

#define RTC_GPIO_OUT_REG          (DR_REG_RTCIO_BASE + 0x0)
/* RTC_GPIO_OUT_DATA : R/W ;bitpos:[31:10] ;default: 0 ; */
/*description: RTC GPIO 0 ~ 21 output data.*/
#define RTC_GPIO_OUT_DATA    0x003FFFFF
#define RTC_GPIO_OUT_DATA_M  ((RTC_GPIO_OUT_DATA_V)<<(RTC_GPIO_OUT_DATA_S))
#define RTC_GPIO_OUT_DATA_V  0x3FFFFF
#define RTC_GPIO_OUT_DATA_S  10

#define RTC_GPIO_OUT_W1TS_REG          (DR_REG_RTCIO_BASE + 0x4)
/* RTC_GPIO_OUT_DATA_W1TS : WO ;bitpos:[31:10] ;default: 0 ; */
/*description: RTC GPIO 0 ~ 21 output data write 1 to set.*/
#define RTC_GPIO_OUT_DATA_W1TS    0x003FFFFF
#define RTC_GPIO_OUT_DATA_W1TS_M  ((RTC_GPIO_OUT_DATA_W1TS_V)<<(RTC_GPIO_OUT_DATA_W1TS_S))
#define RTC_GPIO_OUT_DATA_W1TS_V  0x3FFFFF
#define RTC_GPIO_OUT_DATA_W1TS_S  10

#define RTC_GPIO_OUT_W1TC_REG          (DR_REG_RTCIO_BASE + 0x8)
/* RTC_GPIO_OUT_DATA_W1TC : WO ;bitpos:[31:10] ;default: 0 ; */
/*description: RTC GPIO 0 ~ 21 output data write 1 to clear.*/
#define RTC_GPIO_OUT_DATA_W1TC    0x003FFFFF
#define RTC_GPIO_OUT_DATA_W1TC_M  ((RTC_GPIO_OUT_DATA_W1TC_V)<<(RTC_GPIO_OUT_DATA_W1TC_S))
#define RTC_GPIO_OUT_DATA_W1TC_V  0x3FFFFF
#define RTC_GPIO_OUT_DATA_W1TC_S  10

#define RTC_GPIO_ENABLE_REG          (DR_REG_RTCIO_BASE + 0xC)
/* RTC_GPIO_ENABLE : R/W ;bitpos:[31:10] ;default: 0 ; */
/*description: RTC GPIO 0 ~ 21 enable.*/
#define RTC_GPIO_ENABLE    0x003FFFFF
#define RTC_GPIO_ENABLE_M  ((RTC_GPIO_ENABLE_V)<<(RTC_GPIO_ENABLE_S))
#define RTC_GPIO_ENABLE_V  0x3FFFFF
#define RTC_GPIO_ENABLE_S  10

#define RTC_GPIO_ENABLE_W1TS_REG          (DR_REG_RTCIO_BASE + 0x10)
/* RTC_GPIO_ENABLE_W1TS : WO ;bitpos:[31:10] ;default: 0 ; */
/*description: RTC GPIO 0 ~ 21 enable write 1 to set.*/
#define RTC_GPIO_ENABLE_W1TS    0x003FFFFF
#define RTC_GPIO_ENABLE_W1TS_M  ((RTC_GPIO_ENABLE_W1TS_V)<<(RTC_GPIO_ENABLE_W1TS_S))
#define RTC_GPIO_ENABLE_W1TS_V  0x3FFFFF
#define RTC_GPIO_ENABLE_W1TS_S  10

#define RTC_GPIO_ENABLE_W1TC_REG          (DR_REG_RTCIO_BASE + 0x14)
/* RTC_GPIO_ENABLE_W1TC : WO ;bitpos:[31:10] ;default: 0 ; */
/*description: RTC GPIO 0 ~ 21 enable write 1 to clear.*/
#define RTC_GPIO_ENABLE_W1TC    0x003FFFFF
#define RTC_GPIO_ENABLE_W1TC_M  ((RTC_GPIO_ENABLE_W1TC_V)<<(RTC_GPIO_ENABLE_W1TC_S))
#define RTC_GPIO_ENABLE_W1TC_V  0x3FFFFF
#define RTC_GPIO_ENABLE_W1TC_S  10

#define RTC_GPIO_STATUS_REG          (DR_REG_RTCIO_BASE + 0x18)
/* RTC_GPIO_STATUS_INT : R/W ;bitpos:[31:10] ;default: 0 ; */
/*description: RTC GPIO 0 ~ 21 interrupt status.*/
#define RTC_GPIO_STATUS_INT    0x003FFFFF
#define RTC_GPIO_STATUS_INT_M  ((RTC_GPIO_STATUS_INT_V)<<(RTC_GPIO_STATUS_INT_S))
#define RTC_GPIO_STATUS_INT_V  0x3FFFFF
#define RTC_GPIO_STATUS_INT_S  10

#define RTC_GPIO_STATUS_W1TS_REG          (DR_REG_RTCIO_BASE + 0x1C)
/* RTC_GPIO_STATUS_INT_W1TS : WO ;bitpos:[31:10] ;default: 0 ; */
/*description: RTC GPIO 0 ~ 21 interrupt status write 1 to set.*/
#define RTC_GPIO_STATUS_INT_W1TS    0x003FFFFF
#define RTC_GPIO_STATUS_INT_W1TS_M  ((RTC_GPIO_STATUS_INT_W1TS_V)<<(RTC_GPIO_STATUS_INT_W1TS_S))
#define RTC_GPIO_STATUS_INT_W1TS_V  0x3FFFFF
#define RTC_GPIO_STATUS_INT_W1TS_S  10

#define RTC_GPIO_STATUS_W1TC_REG          (DR_REG_RTCIO_BASE + 0x20)
/* RTC_GPIO_STATUS_INT_W1TC : WO ;bitpos:[31:10] ;default: 0 ; */
/*description: RTC GPIO 0 ~ 21 interrupt status write 1 to clear.*/
#define RTC_GPIO_STATUS_INT_W1TC    0x003FFFFF
#define RTC_GPIO_STATUS_INT_W1TC_M  ((RTC_GPIO_STATUS_INT_W1TC_V)<<(RTC_GPIO_STATUS_INT_W1TC_S))
#define RTC_GPIO_STATUS_INT_W1TC_V  0x3FFFFF
#define RTC_GPIO_STATUS_INT_W1TC_S  10

#define RTC_GPIO_IN_REG          (DR_REG_RTCIO_BASE + 0x24)
/* RTC_GPIO_IN_NEXT : RO ;bitpos:[31:10] ;default:  ; */
/*description: RTC GPIO input data.*/
#define RTC_GPIO_IN_NEXT    0x003FFFFF
#define RTC_GPIO_IN_NEXT_M  ((RTC_GPIO_IN_NEXT_V)<<(RTC_GPIO_IN_NEXT_S))
#define RTC_GPIO_IN_NEXT_V  0x3FFFFF
#define RTC_GPIO_IN_NEXT_S  10

#define RTC_GPIO_PIN0_REG          (DR_REG_RTCIO_BASE + 0x28)
/* RTC_GPIO_PIN0_WAKEUP_ENABLE : R/W ;bitpos:[10] ;default: 0 ; */
/*description: RTC GPIO wakeup enable bit.*/
#define RTC_GPIO_PIN0_WAKEUP_ENABLE    (BIT(10))
#define RTC_GPIO_PIN0_WAKEUP_ENABLE_M  (BIT(10))
#define RTC_GPIO_PIN0_WAKEUP_ENABLE_V  0x1
#define RTC_GPIO_PIN0_WAKEUP_ENABLE_S  10
/* RTC_GPIO_PIN0_INT_TYPE : R/W ;bitpos:[9:7] ;default: 0 ; */
/*description: if set to 0: GPIO interrupt disable,  if set to 1: rising edge trigger,  if set
to 2: falling edge trigger, if set to 3: any edge trigger, if set to 4: low
level trigger,  if set to 5: high level trigger.*/
#define RTC_GPIO_PIN0_INT_TYPE    0x00000007
#define RTC_GPIO_PIN0_INT_TYPE_M  ((RTC_GPIO_PIN0_INT_TYPE_V)<<(RTC_GPIO_PIN0_INT_TYPE_S))
#define RTC_GPIO_PIN0_INT_TYPE_V  0x7
#define RTC_GPIO_PIN0_INT_TYPE_S  7
/* RTC_GPIO_PIN0_PAD_DRIVER : R/W ;bitpos:[2] ;default: 0 ; */
/*description: if set to 0: normal output, if set to 1: open drain.*/
#define RTC_GPIO_PIN0_PAD_DRIVER    (BIT(2))
#define RTC_GPIO_PIN0_PAD_DRIVER_M  (BIT(2))
#define RTC_GPIO_PIN0_PAD_DRIVER_V  0x1
#define RTC_GPIO_PIN0_PAD_DRIVER_S  2

#define RTC_GPIO_PIN1_REG          (DR_REG_RTCIO_BASE + 0x2C)
/* RTC_GPIO_PIN1_WAKEUP_ENABLE : R/W ;bitpos:[10] ;default: 0 ; */
/*description: RTC GPIO wakeup enable bit.*/
#define RTC_GPIO_PIN1_WAKEUP_ENABLE    (BIT(10))
#define RTC_GPIO_PIN1_WAKEUP_ENABLE_M  (BIT(10))
#define RTC_GPIO_PIN1_WAKEUP_ENABLE_V  0x1
#define RTC_GPIO_PIN1_WAKEUP_ENABLE_S  10
/* RTC_GPIO_PIN1_INT_TYPE : R/W ;bitpos:[9:7] ;default: 0 ; */
/*description: if set to 0: GPIO interrupt disable,  if set to 1: rising edge trigger,  if set
to 2: falling edge trigger, if set to 3: any edge trigger, if set to 4: low
level trigger,  if set to 5: high level trigger.*/
#define RTC_GPIO_PIN1_INT_TYPE    0x00000007
#define RTC_GPIO_PIN1_INT_TYPE_M  ((RTC_GPIO_PIN1_INT_TYPE_V)<<(RTC_GPIO_PIN1_INT_TYPE_S))
#define RTC_GPIO_PIN1_INT_TYPE_V  0x7
#define RTC_GPIO_PIN1_INT_TYPE_S  7
/* RTC_GPIO_PIN1_PAD_DRIVER : R/W ;bitpos:[2] ;default: 0 ; */
/*description: if set to 0: normal output, if set to 1: open drain.*/
#define RTC_GPIO_PIN1_PAD_DRIVER    (BIT(2))
#define RTC_GPIO_PIN1_PAD_DRIVER_M  (BIT(2))
#define RTC_GPIO_PIN1_PAD_DRIVER_V  0x1
#define RTC_GPIO_PIN1_PAD_DRIVER_S  2

#define RTC_GPIO_PIN2_REG          (DR_REG_RTCIO_BASE + 0x30)
/* RTC_GPIO_PIN2_WAKEUP_ENABLE : R/W ;bitpos:[10] ;default: 0 ; */
/*description: RTC GPIO wakeup enable bit.*/
#define RTC_GPIO_PIN2_WAKEUP_ENABLE    (BIT(10))
#define RTC_GPIO_PIN2_WAKEUP_ENABLE_M  (BIT(10))
#define RTC_GPIO_PIN2_WAKEUP_ENABLE_V  0x1
#define RTC_GPIO_PIN2_WAKEUP_ENABLE_S  10
/* RTC_GPIO_PIN2_INT_TYPE : R/W ;bitpos:[9:7] ;default: 0 ; */
/*description: if set to 0: GPIO interrupt disable,  if set to 1: rising edge trigger,  if set
to 2: falling edge trigger, if set to 3: any edge trigger, if set to 4: low
level trigger,  if set to 5: high level trigger.*/
#define RTC_GPIO_PIN2_INT_TYPE    0x00000007
#define RTC_GPIO_PIN2_INT_TYPE_M  ((RTC_GPIO_PIN2_INT_TYPE_V)<<(RTC_GPIO_PIN2_INT_TYPE_S))
#define RTC_GPIO_PIN2_INT_TYPE_V  0x7
#define RTC_GPIO_PIN2_INT_TYPE_S  7
/* RTC_GPIO_PIN2_PAD_DRIVER : R/W ;bitpos:[2] ;default: 0 ; */
/*description: if set to 0: normal output, if set to 1: open drain.*/
#define RTC_GPIO_PIN2_PAD_DRIVER    (BIT(2))
#define RTC_GPIO_PIN2_PAD_DRIVER_M  (BIT(2))
#define RTC_GPIO_PIN2_PAD_DRIVER_V  0x1
#define RTC_GPIO_PIN2_PAD_DRIVER_S  2

#define RTC_GPIO_PIN3_REG          (DR_REG_RTCIO_BASE + 0x34)
/* RTC_GPIO_PIN3_WAKEUP_ENABLE : R/W ;bitpos:[10] ;default: 0 ; */
/*description: RTC GPIO wakeup enable bit.*/
#define RTC_GPIO_PIN3_WAKEUP_ENABLE    (BIT(10))
#define RTC_GPIO_PIN3_WAKEUP_ENABLE_M  (BIT(10))
#define RTC_GPIO_PIN3_WAKEUP_ENABLE_V  0x1
#define RTC_GPIO_PIN3_WAKEUP_ENABLE_S  10
/* RTC_GPIO_PIN3_INT_TYPE : R/W ;bitpos:[9:7] ;default: 0 ; */
/*description: if set to 0: GPIO interrupt disable,  if set to 1: rising edge trigger,  if set
to 2: falling edge trigger, if set to 3: any edge trigger, if set to 4: low
level trigger,  if set to 5: high level trigger.*/
#define RTC_GPIO_PIN3_INT_TYPE    0x00000007
#define RTC_GPIO_PIN3_INT_TYPE_M  ((RTC_GPIO_PIN3_INT_TYPE_V)<<(RTC_GPIO_PIN3_INT_TYPE_S))
#define RTC_GPIO_PIN3_INT_TYPE_V  0x7
#define RTC_GPIO_PIN3_INT_TYPE_S  7
/* RTC_GPIO_PIN3_PAD_DRIVER : R/W ;bitpos:[2] ;default: 0 ; */
/*description: if set to 0: normal output, if set to 1: open drain.*/
#define RTC_GPIO_PIN3_PAD_DRIVER    (BIT(2))
#define RTC_GPIO_PIN3_PAD_DRIVER_M  (BIT(2))
#define RTC_GPIO_PIN3_PAD_DRIVER_V  0x1
#define RTC_GPIO_PIN3_PAD_DRIVER_S  2

#define RTC_GPIO_PIN4_REG          (DR_REG_RTCIO_BASE + 0x38)
/* RTC_GPIO_PIN4_WAKEUP_ENABLE : R/W ;bitpos:[10] ;default: 0 ; */
/*description: RTC GPIO wakeup enable bit.*/
#define RTC_GPIO_PIN4_WAKEUP_ENABLE    (BIT(10))
#define RTC_GPIO_PIN4_WAKEUP_ENABLE_M  (BIT(10))
#define RTC_GPIO_PIN4_WAKEUP_ENABLE_V  0x1
#define RTC_GPIO_PIN4_WAKEUP_ENABLE_S  10
/* RTC_GPIO_PIN4_INT_TYPE : R/W ;bitpos:[9:7] ;default: 0 ; */
/*description: if set to 0: GPIO interrupt disable,  if set to 1: rising edge trigger,  if set
to 2: falling edge trigger, if set to 3: any edge trigger, if set to 4: low
level trigger,  if set to 5: high level trigger.*/
#define RTC_GPIO_PIN4_INT_TYPE    0x00000007
#define RTC_GPIO_PIN4_INT_TYPE_M  ((RTC_GPIO_PIN4_INT_TYPE_V)<<(RTC_GPIO_PIN4_INT_TYPE_S))
#define RTC_GPIO_PIN4_INT_TYPE_V  0x7
#define RTC_GPIO_PIN4_INT_TYPE_S  7
/* RTC_GPIO_PIN4_PAD_DRIVER : R/W ;bitpos:[2] ;default: 0 ; */
/*description: if set to 0: normal output, if set to 1: open drain.*/
#define RTC_GPIO_PIN4_PAD_DRIVER    (BIT(2))
#define RTC_GPIO_PIN4_PAD_DRIVER_M  (BIT(2))
#define RTC_GPIO_PIN4_PAD_DRIVER_V  0x1
#define RTC_GPIO_PIN4_PAD_DRIVER_S  2

#define RTC_GPIO_PIN5_REG          (DR_REG_RTCIO_BASE + 0x3C)
/* RTC_GPIO_PIN5_WAKEUP_ENABLE : R/W ;bitpos:[10] ;default: 0 ; */
/*description: RTC GPIO wakeup enable bit.*/
#define RTC_GPIO_PIN5_WAKEUP_ENABLE    (BIT(10))
#define RTC_GPIO_PIN5_WAKEUP_ENABLE_M  (BIT(10))
#define RTC_GPIO_PIN5_WAKEUP_ENABLE_V  0x1
#define RTC_GPIO_PIN5_WAKEUP_ENABLE_S  10
/* RTC_GPIO_PIN5_INT_TYPE : R/W ;bitpos:[9:7] ;default: 0 ; */
/*description: if set to 0: GPIO interrupt disable,  if set to 1: rising edge trigger,  if set
to 2: falling edge trigger, if set to 3: any edge trigger, if set to 4: low
level trigger,  if set to 5: high level trigger.*/
#define RTC_GPIO_PIN5_INT_TYPE    0x00000007
#define RTC_GPIO_PIN5_INT_TYPE_M  ((RTC_GPIO_PIN5_INT_TYPE_V)<<(RTC_GPIO_PIN5_INT_TYPE_S))
#define RTC_GPIO_PIN5_INT_TYPE_V  0x7
#define RTC_GPIO_PIN5_INT_TYPE_S  7
/* RTC_GPIO_PIN5_PAD_DRIVER : R/W ;bitpos:[2] ;default: 0 ; */
/*description: if set to 0: normal output, if set to 1: open drain.*/
#define RTC_GPIO_PIN5_PAD_DRIVER    (BIT(2))
#define RTC_GPIO_PIN5_PAD_DRIVER_M  (BIT(2))
#define RTC_GPIO_PIN5_PAD_DRIVER_V  0x1
#define RTC_GPIO_PIN5_PAD_DRIVER_S  2

#define RTC_GPIO_PIN6_REG          (DR_REG_RTCIO_BASE + 0x40)
/* RTC_GPIO_PIN6_WAKEUP_ENABLE : R/W ;bitpos:[10] ;default: 0 ; */
/*description: RTC GPIO wakeup enable bit.*/
#define RTC_GPIO_PIN6_WAKEUP_ENABLE    (BIT(10))
#define RTC_GPIO_PIN6_WAKEUP_ENABLE_M  (BIT(10))
#define RTC_GPIO_PIN6_WAKEUP_ENABLE_V  0x1
#define RTC_GPIO_PIN6_WAKEUP_ENABLE_S  10
/* RTC_GPIO_PIN6_INT_TYPE : R/W ;bitpos:[9:7] ;default: 0 ; */
/*description: if set to 0: GPIO interrupt disable,  if set to 1: rising edge trigger,  if set
to 2: falling edge trigger, if set to 3: any edge trigger, if set to 4: low
level trigger,  if set to 5: high level trigger.*/
#define RTC_GPIO_PIN6_INT_TYPE    0x00000007
#define RTC_GPIO_PIN6_INT_TYPE_M  ((RTC_GPIO_PIN6_INT_TYPE_V)<<(RTC_GPIO_PIN6_INT_TYPE_S))
#define RTC_GPIO_PIN6_INT_TYPE_V  0x7
#define RTC_GPIO_PIN6_INT_TYPE_S  7
/* RTC_GPIO_PIN6_PAD_DRIVER : R/W ;bitpos:[2] ;default: 0 ; */
/*description: if set to 0: normal output, if set to 1: open drain.*/
#define RTC_GPIO_PIN6_PAD_DRIVER    (BIT(2))
#define RTC_GPIO_PIN6_PAD_DRIVER_M  (BIT(2))
#define RTC_GPIO_PIN6_PAD_DRIVER_V  0x1
#define RTC_GPIO_PIN6_PAD_DRIVER_S  2

#define RTC_GPIO_PIN7_REG          (DR_REG_RTCIO_BASE + 0x44)
/* RTC_GPIO_PIN7_WAKEUP_ENABLE : R/W ;bitpos:[10] ;default: 0 ; */
/*description: RTC GPIO wakeup enable bit.*/
#define RTC_GPIO_PIN7_WAKEUP_ENABLE    (BIT(10))
#define RTC_GPIO_PIN7_WAKEUP_ENABLE_M  (BIT(10))
#define RTC_GPIO_PIN7_WAKEUP_ENABLE_V  0x1
#define RTC_GPIO_PIN7_WAKEUP_ENABLE_S  10
/* RTC_GPIO_PIN7_INT_TYPE : R/W ;bitpos:[9:7] ;default: 0 ; */
/*description: if set to 0: GPIO interrupt disable,  if set to 1: rising edge trigger,  if set
to 2: falling edge trigger, if set to 3: any edge trigger, if set to 4: low
level trigger,  if set to 5: high level trigger.*/
#define RTC_GPIO_PIN7_INT_TYPE    0x00000007
#define RTC_GPIO_PIN7_INT_TYPE_M  ((RTC_GPIO_PIN7_INT_TYPE_V)<<(RTC_GPIO_PIN7_INT_TYPE_S))
#define RTC_GPIO_PIN7_INT_TYPE_V  0x7
#define RTC_GPIO_PIN7_INT_TYPE_S  7
/* RTC_GPIO_PIN7_PAD_DRIVER : R/W ;bitpos:[2] ;default: 0 ; */
/*description: if set to 0: normal output, if set to 1: open drain.*/
#define RTC_GPIO_PIN7_PAD_DRIVER    (BIT(2))
#define RTC_GPIO_PIN7_PAD_DRIVER_M  (BIT(2))
#define RTC_GPIO_PIN7_PAD_DRIVER_V  0x1
#define RTC_GPIO_PIN7_PAD_DRIVER_S  2

#define RTC_GPIO_PIN8_REG          (DR_REG_RTCIO_BASE + 0x48)
/* RTC_GPIO_PIN8_WAKEUP_ENABLE : R/W ;bitpos:[10] ;default: 0 ; */
/*description: RTC GPIO wakeup enable bit.*/
#define RTC_GPIO_PIN8_WAKEUP_ENABLE    (BIT(10))
#define RTC_GPIO_PIN8_WAKEUP_ENABLE_M  (BIT(10))
#define RTC_GPIO_PIN8_WAKEUP_ENABLE_V  0x1
#define RTC_GPIO_PIN8_WAKEUP_ENABLE_S  10
/* RTC_GPIO_PIN8_INT_TYPE : R/W ;bitpos:[9:7] ;default: 0 ; */
/*description: if set to 0: GPIO interrupt disable,  if set to 1: rising edge trigger,  if set
to 2: falling edge trigger, if set to 3: any edge trigger, if set to 4: low
level trigger,  if set to 5: high level trigger.*/
#define RTC_GPIO_PIN8_INT_TYPE    0x00000007
#define RTC_GPIO_PIN8_INT_TYPE_M  ((RTC_GPIO_PIN8_INT_TYPE_V)<<(RTC_GPIO_PIN8_INT_TYPE_S))
#define RTC_GPIO_PIN8_INT_TYPE_V  0x7
#define RTC_GPIO_PIN8_INT_TYPE_S  7
/* RTC_GPIO_PIN8_PAD_DRIVER : R/W ;bitpos:[2] ;default: 0 ; */
/*description: if set to 0: normal output, if set to 1: open drain.*/
#define RTC_GPIO_PIN8_PAD_DRIVER    (BIT(2))
#define RTC_GPIO_PIN8_PAD_DRIVER_M  (BIT(2))
#define RTC_GPIO_PIN8_PAD_DRIVER_V  0x1
#define RTC_GPIO_PIN8_PAD_DRIVER_S  2

#define RTC_GPIO_PIN9_REG          (DR_REG_RTCIO_BASE + 0x4C)
/* RTC_GPIO_PIN9_WAKEUP_ENABLE : R/W ;bitpos:[10] ;default: 0 ; */
/*description: RTC GPIO wakeup enable bit.*/
#define RTC_GPIO_PIN9_WAKEUP_ENABLE    (BIT(10))
#define RTC_GPIO_PIN9_WAKEUP_ENABLE_M  (BIT(10))
#define RTC_GPIO_PIN9_WAKEUP_ENABLE_V  0x1
#define RTC_GPIO_PIN9_WAKEUP_ENABLE_S  10
/* RTC_GPIO_PIN9_INT_TYPE : R/W ;bitpos:[9:7] ;default: 0 ; */
/*description: if set to 0: GPIO interrupt disable,  if set to 1: rising edge trigger,  if set
to 2: falling edge trigger, if set to 3: any edge trigger, if set to 4: low
level trigger,  if set to 5: high level trigger.*/
#define RTC_GPIO_PIN9_INT_TYPE    0x00000007
#define RTC_GPIO_PIN9_INT_TYPE_M  ((RTC_GPIO_PIN9_INT_TYPE_V)<<(RTC_GPIO_PIN9_INT_TYPE_S))
#define RTC_GPIO_PIN9_INT_TYPE_V  0x7
#define RTC_GPIO_PIN9_INT_TYPE_S  7
/* RTC_GPIO_PIN9_PAD_DRIVER : R/W ;bitpos:[2] ;default: 0 ; */
/*description: if set to 0: normal output, if set to 1: open drain.*/
#define RTC_GPIO_PIN9_PAD_DRIVER    (BIT(2))
#define RTC_GPIO_PIN9_PAD_DRIVER_M  (BIT(2))
#define RTC_GPIO_PIN9_PAD_DRIVER_V  0x1
#define RTC_GPIO_PIN9_PAD_DRIVER_S  2

#define RTC_GPIO_PIN10_REG          (DR_REG_RTCIO_BASE + 0x50)
/* RTC_GPIO_PIN10_WAKEUP_ENABLE : R/W ;bitpos:[10] ;default: 0 ; */
/*description: RTC GPIO wakeup enable bit.*/
#define RTC_GPIO_PIN10_WAKEUP_ENABLE    (BIT(10))
#define RTC_GPIO_PIN10_WAKEUP_ENABLE_M  (BIT(10))
#define RTC_GPIO_PIN10_WAKEUP_ENABLE_V  0x1
#define RTC_GPIO_PIN10_WAKEUP_ENABLE_S  10
/* RTC_GPIO_PIN10_INT_TYPE : R/W ;bitpos:[9:7] ;default: 0 ; */
/*description: if set to 0: GPIO interrupt disable,  if set to 1: rising edge trigger,  if set
to 2: falling edge trigger, if set to 3: any edge trigger, if set to 4: low
level trigger,  if set to 5: high level trigger.*/
#define RTC_GPIO_PIN10_INT_TYPE    0x00000007
#define RTC_GPIO_PIN10_INT_TYPE_M  ((RTC_GPIO_PIN10_INT_TYPE_V)<<(RTC_GPIO_PIN10_INT_TYPE_S))
#define RTC_GPIO_PIN10_INT_TYPE_V  0x7
#define RTC_GPIO_PIN10_INT_TYPE_S  7
/* RTC_GPIO_PIN10_PAD_DRIVER : R/W ;bitpos:[2] ;default: 0 ; */
/*description: if set to 0: normal output, if set to 1: open drain.*/
#define RTC_GPIO_PIN10_PAD_DRIVER    (BIT(2))
#define RTC_GPIO_PIN10_PAD_DRIVER_M  (BIT(2))
#define RTC_GPIO_PIN10_PAD_DRIVER_V  0x1
#define RTC_GPIO_PIN10_PAD_DRIVER_S  2

#define RTC_GPIO_PIN11_REG          (DR_REG_RTCIO_BASE + 0x54)
/* RTC_GPIO_PIN11_WAKEUP_ENABLE : R/W ;bitpos:[10] ;default: 0 ; */
/*description: RTC GPIO wakeup enable bit.*/
#define RTC_GPIO_PIN11_WAKEUP_ENABLE    (BIT(10))
#define RTC_GPIO_PIN11_WAKEUP_ENABLE_M  (BIT(10))
#define RTC_GPIO_PIN11_WAKEUP_ENABLE_V  0x1
#define RTC_GPIO_PIN11_WAKEUP_ENABLE_S  10
/* RTC_GPIO_PIN11_INT_TYPE : R/W ;bitpos:[9:7] ;default: 0 ; */
/*description: if set to 0: GPIO interrupt disable,  if set to 1: rising edge trigger,  if set
to 2: falling edge trigger, if set to 3: any edge trigger, if set to 4: low
level trigger,  if set to 5: high level trigger.*/
#define RTC_GPIO_PIN11_INT_TYPE    0x00000007
#define RTC_GPIO_PIN11_INT_TYPE_M  ((RTC_GPIO_PIN11_INT_TYPE_V)<<(RTC_GPIO_PIN11_INT_TYPE_S))
#define RTC_GPIO_PIN11_INT_TYPE_V  0x7
#define RTC_GPIO_PIN11_INT_TYPE_S  7
/* RTC_GPIO_PIN11_PAD_DRIVER : R/W ;bitpos:[2] ;default: 0 ; */
/*description: if set to 0: normal output, if set to 1: open drain.*/
#define RTC_GPIO_PIN11_PAD_DRIVER    (BIT(2))
#define RTC_GPIO_PIN11_PAD_DRIVER_M  (BIT(2))
#define RTC_GPIO_PIN11_PAD_DRIVER_V  0x1
#define RTC_GPIO_PIN11_PAD_DRIVER_S  2

#define RTC_GPIO_PIN12_REG          (DR_REG_RTCIO_BASE + 0x58)
/* RTC_GPIO_PIN12_WAKEUP_ENABLE : R/W ;bitpos:[10] ;default: 0 ; */
/*description: RTC GPIO wakeup enable bit.*/
#define RTC_GPIO_PIN12_WAKEUP_ENABLE    (BIT(10))
#define RTC_GPIO_PIN12_WAKEUP_ENABLE_M  (BIT(10))
#define RTC_GPIO_PIN12_WAKEUP_ENABLE_V  0x1
#define RTC_GPIO_PIN12_WAKEUP_ENABLE_S  10
/* RTC_GPIO_PIN12_INT_TYPE : R/W ;bitpos:[9:7] ;default: 0 ; */
/*description: if set to 0: GPIO interrupt disable,  if set to 1: rising edge trigger,  if set
to 2: falling edge trigger, if set to 3: any edge trigger, if set to 4: low
level trigger,  if set to 5: high level trigger.*/
#define RTC_GPIO_PIN12_INT_TYPE    0x00000007
#define RTC_GPIO_PIN12_INT_TYPE_M  ((RTC_GPIO_PIN12_INT_TYPE_V)<<(RTC_GPIO_PIN12_INT_TYPE_S))
#define RTC_GPIO_PIN12_INT_TYPE_V  0x7
#define RTC_GPIO_PIN12_INT_TYPE_S  7
/* RTC_GPIO_PIN12_PAD_DRIVER : R/W ;bitpos:[2] ;default: 0 ; */
/*description: if set to 0: normal output, if set to 1: open drain.*/
#define RTC_GPIO_PIN12_PAD_DRIVER    (BIT(2))
#define RTC_GPIO_PIN12_PAD_DRIVER_M  (BIT(2))
#define RTC_GPIO_PIN12_PAD_DRIVER_V  0x1
#define RTC_GPIO_PIN12_PAD_DRIVER_S  2

#define RTC_GPIO_PIN13_REG          (DR_REG_RTCIO_BASE + 0x5C)
/* RTC_GPIO_PIN13_WAKEUP_ENABLE : R/W ;bitpos:[10] ;default: 0 ; */
/*description: RTC GPIO wakeup enable bit.*/
#define RTC_GPIO_PIN13_WAKEUP_ENABLE    (BIT(10))
#define RTC_GPIO_PIN13_WAKEUP_ENABLE_M  (BIT(10))
#define RTC_GPIO_PIN13_WAKEUP_ENABLE_V  0x1
#define RTC_GPIO_PIN13_WAKEUP_ENABLE_S  10
/* RTC_GPIO_PIN13_INT_TYPE : R/W ;bitpos:[9:7] ;default: 0 ; */
/*description: if set to 0: GPIO interrupt disable,  if set to 1: rising edge trigger,  if set
to 2: falling edge trigger, if set to 3: any edge trigger, if set to 4: low
level trigger,  if set to 5: high level trigger.*/
#define RTC_GPIO_PIN13_INT_TYPE    0x00000007
#define RTC_GPIO_PIN13_INT_TYPE_M  ((RTC_GPIO_PIN13_INT_TYPE_V)<<(RTC_GPIO_PIN13_INT_TYPE_S))
#define RTC_GPIO_PIN13_INT_TYPE_V  0x7
#define RTC_GPIO_PIN13_INT_TYPE_S  7
/* RTC_GPIO_PIN13_PAD_DRIVER : R/W ;bitpos:[2] ;default: 0 ; */
/*description: if set to 0: normal output, if set to 1: open drain.*/
#define RTC_GPIO_PIN13_PAD_DRIVER    (BIT(2))
#define RTC_GPIO_PIN13_PAD_DRIVER_M  (BIT(2))
#define RTC_GPIO_PIN13_PAD_DRIVER_V  0x1
#define RTC_GPIO_PIN13_PAD_DRIVER_S  2

#define RTC_GPIO_PIN14_REG          (DR_REG_RTCIO_BASE + 0x60)
/* RTC_GPIO_PIN14_WAKEUP_ENABLE : R/W ;bitpos:[10] ;default: 0 ; */
/*description: RTC GPIO wakeup enable bit.*/
#define RTC_GPIO_PIN14_WAKEUP_ENABLE    (BIT(10))
#define RTC_GPIO_PIN14_WAKEUP_ENABLE_M  (BIT(10))
#define RTC_GPIO_PIN14_WAKEUP_ENABLE_V  0x1
#define RTC_GPIO_PIN14_WAKEUP_ENABLE_S  10
/* RTC_GPIO_PIN14_INT_TYPE : R/W ;bitpos:[9:7] ;default: 0 ; */
/*description: if set to 0: GPIO interrupt disable,  if set to 1: rising edge trigger,  if set
to 2: falling edge trigger, if set to 3: any edge trigger, if set to 4: low
level trigger,  if set to 5: high level trigger.*/
#define RTC_GPIO_PIN14_INT_TYPE    0x00000007
#define RTC_GPIO_PIN14_INT_TYPE_M  ((RTC_GPIO_PIN14_INT_TYPE_V)<<(RTC_GPIO_PIN14_INT_TYPE_S))
#define RTC_GPIO_PIN14_INT_TYPE_V  0x7
#define RTC_GPIO_PIN14_INT_TYPE_S  7
/* RTC_GPIO_PIN14_PAD_DRIVER : R/W ;bitpos:[2] ;default: 0 ; */
/*description: if set to 0: normal output, if set to 1: open drain.*/
#define RTC_GPIO_PIN14_PAD_DRIVER    (BIT(2))
#define RTC_GPIO_PIN14_PAD_DRIVER_M  (BIT(2))
#define RTC_GPIO_PIN14_PAD_DRIVER_V  0x1
#define RTC_GPIO_PIN14_PAD_DRIVER_S  2

#define RTC_GPIO_PIN15_REG          (DR_REG_RTCIO_BASE + 0x64)
/* RTC_GPIO_PIN15_WAKEUP_ENABLE : R/W ;bitpos:[10] ;default: 0 ; */
/*description: RTC GPIO wakeup enable bit.*/
#define RTC_GPIO_PIN15_WAKEUP_ENABLE    (BIT(10))
#define RTC_GPIO_PIN15_WAKEUP_ENABLE_M  (BIT(10))
#define RTC_GPIO_PIN15_WAKEUP_ENABLE_V  0x1
#define RTC_GPIO_PIN15_WAKEUP_ENABLE_S  10
/* RTC_GPIO_PIN15_INT_TYPE : R/W ;bitpos:[9:7] ;default: 0 ; */
/*description: if set to 0: GPIO interrupt disable,  if set to 1: rising edge trigger,  if set
to 2: falling edge trigger, if set to 3: any edge trigger, if set to 4: low
level trigger,  if set to 5: high level trigger.*/
#define RTC_GPIO_PIN15_INT_TYPE    0x00000007
#define RTC_GPIO_PIN15_INT_TYPE_M  ((RTC_GPIO_PIN15_INT_TYPE_V)<<(RTC_GPIO_PIN15_INT_TYPE_S))
#define RTC_GPIO_PIN15_INT_TYPE_V  0x7
#define RTC_GPIO_PIN15_INT_TYPE_S  7
/* RTC_GPIO_PIN15_PAD_DRIVER : R/W ;bitpos:[2] ;default: 0 ; */
/*description: if set to 0: normal output, if set to 1: open drain.*/
#define RTC_GPIO_PIN15_PAD_DRIVER    (BIT(2))
#define RTC_GPIO_PIN15_PAD_DRIVER_M  (BIT(2))
#define RTC_GPIO_PIN15_PAD_DRIVER_V  0x1
#define RTC_GPIO_PIN15_PAD_DRIVER_S  2

#define RTC_GPIO_PIN16_REG          (DR_REG_RTCIO_BASE + 0x68)
/* RTC_GPIO_PIN16_WAKEUP_ENABLE : R/W ;bitpos:[10] ;default: 0 ; */
/*description: RTC GPIO wakeup enable bit.*/
#define RTC_GPIO_PIN16_WAKEUP_ENABLE    (BIT(10))
#define RTC_GPIO_PIN16_WAKEUP_ENABLE_M  (BIT(10))
#define RTC_GPIO_PIN16_WAKEUP_ENABLE_V  0x1
#define RTC_GPIO_PIN16_WAKEUP_ENABLE_S  10
/* RTC_GPIO_PIN16_INT_TYPE : R/W ;bitpos:[9:7] ;default: 0 ; */
/*description: if set to 0: GPIO interrupt disable,  if set to 1: rising edge trigger,  if set
to 2: falling edge trigger, if set to 3: any edge trigger, if set to 4: low
level trigger,  if set to 5: high level trigger.*/
#define RTC_GPIO_PIN16_INT_TYPE    0x00000007
#define RTC_GPIO_PIN16_INT_TYPE_M  ((RTC_GPIO_PIN16_INT_TYPE_V)<<(RTC_GPIO_PIN16_INT_TYPE_S))
#define RTC_GPIO_PIN16_INT_TYPE_V  0x7
#define RTC_GPIO_PIN16_INT_TYPE_S  7
/* RTC_GPIO_PIN16_PAD_DRIVER : R/W ;bitpos:[2] ;default: 0 ; */
/*description: if set to 0: normal output, if set to 1: open drain.*/
#define RTC_GPIO_PIN16_PAD_DRIVER    (BIT(2))
#define RTC_GPIO_PIN16_PAD_DRIVER_M  (BIT(2))
#define RTC_GPIO_PIN16_PAD_DRIVER_V  0x1
#define RTC_GPIO_PIN16_PAD_DRIVER_S  2

#define RTC_GPIO_PIN17_REG          (DR_REG_RTCIO_BASE + 0x6C)
/* RTC_GPIO_PIN17_WAKEUP_ENABLE : R/W ;bitpos:[10] ;default: 0 ; */
/*description: RTC GPIO wakeup enable bit.*/
#define RTC_GPIO_PIN17_WAKEUP_ENABLE    (BIT(10))
#define RTC_GPIO_PIN17_WAKEUP_ENABLE_M  (BIT(10))
#define RTC_GPIO_PIN17_WAKEUP_ENABLE_V  0x1
#define RTC_GPIO_PIN17_WAKEUP_ENABLE_S  10
/* RTC_GPIO_PIN17_INT_TYPE : R/W ;bitpos:[9:7] ;default: 0 ; */
/*description: if set to 0: GPIO interrupt disable,  if set to 1: rising edge trigger,  if set
to 2: falling edge trigger, if set to 3: any edge trigger, if set to 4: low
level trigger,  if set to 5: high level trigger.*/
#define RTC_GPIO_PIN17_INT_TYPE    0x00000007
#define RTC_GPIO_PIN17_INT_TYPE_M  ((RTC_GPIO_PIN17_INT_TYPE_V)<<(RTC_GPIO_PIN17_INT_TYPE_S))
#define RTC_GPIO_PIN17_INT_TYPE_V  0x7
#define RTC_GPIO_PIN17_INT_TYPE_S  7
/* RTC_GPIO_PIN17_PAD_DRIVER : R/W ;bitpos:[2] ;default: 0 ; */
/*description: if set to 0: normal output, if set to 1: open drain.*/
#define RTC_GPIO_PIN17_PAD_DRIVER    (BIT(2))
#define RTC_GPIO_PIN17_PAD_DRIVER_M  (BIT(2))
#define RTC_GPIO_PIN17_PAD_DRIVER_V  0x1
#define RTC_GPIO_PIN17_PAD_DRIVER_S  2

#define RTC_GPIO_PIN18_REG          (DR_REG_RTCIO_BASE + 0x70)
/* RTC_GPIO_PIN18_WAKEUP_ENABLE : R/W ;bitpos:[10] ;default: 0 ; */
/*description: RTC GPIO wakeup enable bit.*/
#define RTC_GPIO_PIN18_WAKEUP_ENABLE    (BIT(10))
#define RTC_GPIO_PIN18_WAKEUP_ENABLE_M  (BIT(10))
#define RTC_GPIO_PIN18_WAKEUP_ENABLE_V  0x1
#define RTC_GPIO_PIN18_WAKEUP_ENABLE_S  10
/* RTC_GPIO_PIN18_INT_TYPE : R/W ;bitpos:[9:7] ;default: 0 ; */
/*description: if set to 0: GPIO interrupt disable,  if set to 1: rising edge trigger,  if set
to 2: falling edge trigger, if set to 3: any edge trigger, if set to 4: low
level trigger,  if set to 5: high level trigger.*/
#define RTC_GPIO_PIN18_INT_TYPE    0x00000007
#define RTC_GPIO_PIN18_INT_TYPE_M  ((RTC_GPIO_PIN18_INT_TYPE_V)<<(RTC_GPIO_PIN18_INT_TYPE_S))
#define RTC_GPIO_PIN18_INT_TYPE_V  0x7
#define RTC_GPIO_PIN18_INT_TYPE_S  7
/* RTC_GPIO_PIN18_PAD_DRIVER : R/W ;bitpos:[2] ;default: 0 ; */
/*description: if set to 0: normal output, if set to 1: open drain.*/
#define RTC_GPIO_PIN18_PAD_DRIVER    (BIT(2))
#define RTC_GPIO_PIN18_PAD_DRIVER_M  (BIT(2))
#define RTC_GPIO_PIN18_PAD_DRIVER_V  0x1
#define RTC_GPIO_PIN18_PAD_DRIVER_S  2

#define RTC_GPIO_PIN19_REG          (DR_REG_RTCIO_BASE + 0x74)
/* RTC_GPIO_PIN19_WAKEUP_ENABLE : R/W ;bitpos:[10] ;default: 0 ; */
/*description: RTC GPIO wakeup enable bit.*/
#define RTC_GPIO_PIN19_WAKEUP_ENABLE    (BIT(10))
#define RTC_GPIO_PIN19_WAKEUP_ENABLE_M  (BIT(10))
#define RTC_GPIO_PIN19_WAKEUP_ENABLE_V  0x1
#define RTC_GPIO_PIN19_WAKEUP_ENABLE_S  10
/* RTC_GPIO_PIN19_INT_TYPE : R/W ;bitpos:[9:7] ;default: 0 ; */
/*description: if set to 0: GPIO interrupt disable,  if set to 1: rising edge trigger,  if set
to 2: falling edge trigger, if set to 3: any edge trigger, if set to 4: low
level trigger,  if set to 5: high level trigger.*/
#define RTC_GPIO_PIN19_INT_TYPE    0x00000007
#define RTC_GPIO_PIN19_INT_TYPE_M  ((RTC_GPIO_PIN19_INT_TYPE_V)<<(RTC_GPIO_PIN19_INT_TYPE_S))
#define RTC_GPIO_PIN19_INT_TYPE_V  0x7
#define RTC_GPIO_PIN19_INT_TYPE_S  7
/* RTC_GPIO_PIN19_PAD_DRIVER : R/W ;bitpos:[2] ;default: 0 ; */
/*description: if set to 0: normal output, if set to 1: open drain.*/
#define RTC_GPIO_PIN19_PAD_DRIVER    (BIT(2))
#define RTC_GPIO_PIN19_PAD_DRIVER_M  (BIT(2))
#define RTC_GPIO_PIN19_PAD_DRIVER_V  0x1
#define RTC_GPIO_PIN19_PAD_DRIVER_S  2

#define RTC_GPIO_PIN20_REG          (DR_REG_RTCIO_BASE + 0x78)
/* RTC_GPIO_PIN20_WAKEUP_ENABLE : R/W ;bitpos:[10] ;default: 0 ; */
/*description: RTC GPIO wakeup enable bit.*/
#define RTC_GPIO_PIN20_WAKEUP_ENABLE    (BIT(10))
#define RTC_GPIO_PIN20_WAKEUP_ENABLE_M  (BIT(10))
#define RTC_GPIO_PIN20_WAKEUP_ENABLE_V  0x1
#define RTC_GPIO_PIN20_WAKEUP_ENABLE_S  10
/* RTC_GPIO_PIN20_INT_TYPE : R/W ;bitpos:[9:7] ;default: 0 ; */
/*description: if set to 0: GPIO interrupt disable,  if set to 1: rising edge trigger,  if set
to 2: falling edge trigger, if set to 3: any edge trigger, if set to 4: low
level trigger,  if set to 5: high level trigger.*/
#define RTC_GPIO_PIN20_INT_TYPE    0x00000007
#define RTC_GPIO_PIN20_INT_TYPE_M  ((RTC_GPIO_PIN20_INT_TYPE_V)<<(RTC_GPIO_PIN20_INT_TYPE_S))
#define RTC_GPIO_PIN20_INT_TYPE_V  0x7
#define RTC_GPIO_PIN20_INT_TYPE_S  7
/* RTC_GPIO_PIN20_PAD_DRIVER : R/W ;bitpos:[2] ;default: 0 ; */
/*description: if set to 0: normal output, if set to 1: open drain.*/
#define RTC_GPIO_PIN20_PAD_DRIVER    (BIT(2))
#define RTC_GPIO_PIN20_PAD_DRIVER_M  (BIT(2))
#define RTC_GPIO_PIN20_PAD_DRIVER_V  0x1
#define RTC_GPIO_PIN20_PAD_DRIVER_S  2

#define RTC_GPIO_PIN21_REG          (DR_REG_RTCIO_BASE + 0x7C)
/* RTC_GPIO_PIN21_WAKEUP_ENABLE : R/W ;bitpos:[10] ;default: 0 ; */
/*description: RTC GPIO wakeup enable bit.*/
#define RTC_GPIO_PIN21_WAKEUP_ENABLE    (BIT(10))
#define RTC_GPIO_PIN21_WAKEUP_ENABLE_M  (BIT(10))
#define RTC_GPIO_PIN21_WAKEUP_ENABLE_V  0x1
#define RTC_GPIO_PIN21_WAKEUP_ENABLE_S  10
/* RTC_GPIO_PIN21_INT_TYPE : R/W ;bitpos:[9:7] ;default: 0 ; */
/*description: if set to 0: GPIO interrupt disable,  if set to 1: rising edge trigger,  if set
to 2: falling edge trigger, if set to 3: any edge trigger, if set to 4: low
level trigger,  if set to 5: high level trigger.*/
#define RTC_GPIO_PIN21_INT_TYPE    0x00000007
#define RTC_GPIO_PIN21_INT_TYPE_M  ((RTC_GPIO_PIN21_INT_TYPE_V)<<(RTC_GPIO_PIN21_INT_TYPE_S))
#define RTC_GPIO_PIN21_INT_TYPE_V  0x7
#define RTC_GPIO_PIN21_INT_TYPE_S  7
/* RTC_GPIO_PIN21_PAD_DRIVER : R/W ;bitpos:[2] ;default: 0 ; */
/*description: if set to 0: normal output, if set to 1: open drain.*/
#define RTC_GPIO_PIN21_PAD_DRIVER    (BIT(2))
#define RTC_GPIO_PIN21_PAD_DRIVER_M  (BIT(2))
#define RTC_GPIO_PIN21_PAD_DRIVER_V  0x1
#define RTC_GPIO_PIN21_PAD_DRIVER_S  2

#define RTC_IO_RTC_DEBUG_SEL_REG          (DR_REG_RTCIO_BASE + 0x80)
/* RTC_IO_DEBUG_12M_NO_GATING : R/W ;bitpos:[25] ;default: 1'd0 ; */
/*description: .*/
#define RTC_IO_DEBUG_12M_NO_GATING    (BIT(25))
#define RTC_IO_DEBUG_12M_NO_GATING_M  (BIT(25))
#define RTC_IO_DEBUG_12M_NO_GATING_V  0x1
#define RTC_IO_DEBUG_12M_NO_GATING_S  25
/* RTC_IO_DEBUG_SEL4 : R/W ;bitpos:[24:20] ;default: 5'd0 ; */
/*description: .*/
#define RTC_IO_DEBUG_SEL4    0x0000001F
#define RTC_IO_DEBUG_SEL4_M  ((RTC_IO_DEBUG_SEL4_V)<<(RTC_IO_DEBUG_SEL4_S))
#define RTC_IO_DEBUG_SEL4_V  0x1F
#define RTC_IO_DEBUG_SEL4_S  20
/* RTC_IO_DEBUG_SEL3 : R/W ;bitpos:[19:15] ;default: 5'd0 ; */
/*description: .*/
#define RTC_IO_DEBUG_SEL3    0x0000001F
#define RTC_IO_DEBUG_SEL3_M  ((RTC_IO_DEBUG_SEL3_V)<<(RTC_IO_DEBUG_SEL3_S))
#define RTC_IO_DEBUG_SEL3_V  0x1F
#define RTC_IO_DEBUG_SEL3_S  15
/* RTC_IO_DEBUG_SEL2 : R/W ;bitpos:[14:10] ;default: 5'd0 ; */
/*description: .*/
#define RTC_IO_DEBUG_SEL2    0x0000001F
#define RTC_IO_DEBUG_SEL2_M  ((RTC_IO_DEBUG_SEL2_V)<<(RTC_IO_DEBUG_SEL2_S))
#define RTC_IO_DEBUG_SEL2_V  0x1F
#define RTC_IO_DEBUG_SEL2_S  10
/* RTC_IO_DEBUG_SEL1 : R/W ;bitpos:[9:5] ;default: 5'd0 ; */
/*description: .*/
#define RTC_IO_DEBUG_SEL1    0x0000001F
#define RTC_IO_DEBUG_SEL1_M  ((RTC_IO_DEBUG_SEL1_V)<<(RTC_IO_DEBUG_SEL1_S))
#define RTC_IO_DEBUG_SEL1_V  0x1F
#define RTC_IO_DEBUG_SEL1_S  5
/* RTC_IO_DEBUG_SEL0 : R/W ;bitpos:[4:0] ;default: 5'd0 ; */
/*description: .*/
#define RTC_IO_DEBUG_SEL0    0x0000001F
#define RTC_IO_DEBUG_SEL0_M  ((RTC_IO_DEBUG_SEL0_V)<<(RTC_IO_DEBUG_SEL0_S))
#define RTC_IO_DEBUG_SEL0_V  0x1F
#define RTC_IO_DEBUG_SEL0_S  0

#define RTC_IO_TOUCH_PAD0_REG          (DR_REG_RTCIO_BASE + 0x84)
/* RTC_IO_TOUCH_PAD0_DRV : R/W ;bitpos:[30:29] ;default: 2'd2 ; */
/*description: DRV.*/
#define RTC_IO_TOUCH_PAD0_DRV    0x00000003
#define RTC_IO_TOUCH_PAD0_DRV_M  ((RTC_IO_TOUCH_PAD0_DRV_V)<<(RTC_IO_TOUCH_PAD0_DRV_S))
#define RTC_IO_TOUCH_PAD0_DRV_V  0x3
#define RTC_IO_TOUCH_PAD0_DRV_S  29
/* RTC_IO_TOUCH_PAD0_RDE : R/W ;bitpos:[28] ;default: 1'd1 ; */
/*description: RDE.*/
#define RTC_IO_TOUCH_PAD0_RDE    (BIT(28))
#define RTC_IO_TOUCH_PAD0_RDE_M  (BIT(28))
#define RTC_IO_TOUCH_PAD0_RDE_V  0x1
#define RTC_IO_TOUCH_PAD0_RDE_S  28
/* RTC_IO_TOUCH_PAD0_RUE : R/W ;bitpos:[27] ;default: 1'd0 ; */
/*description: RUE.*/
#define RTC_IO_TOUCH_PAD0_RUE    (BIT(27))
#define RTC_IO_TOUCH_PAD0_RUE_M  (BIT(27))
#define RTC_IO_TOUCH_PAD0_RUE_V  0x1
#define RTC_IO_TOUCH_PAD0_RUE_S  27
/* RTC_IO_TOUCH_PAD0_START : R/W ;bitpos:[22] ;default: 1'd0 ; */
/*description: TOUCH_START.*/
#define RTC_IO_TOUCH_PAD0_START    (BIT(22))
#define RTC_IO_TOUCH_PAD0_START_M  (BIT(22))
#define RTC_IO_TOUCH_PAD0_START_V  0x1
#define RTC_IO_TOUCH_PAD0_START_S  22
/* RTC_IO_TOUCH_PAD0_TIE_OPT : R/W ;bitpos:[21] ;default: 1'd0 ; */
/*description: TOUCH_TIE_OPT.*/
#define RTC_IO_TOUCH_PAD0_TIE_OPT    (BIT(21))
#define RTC_IO_TOUCH_PAD0_TIE_OPT_M  (BIT(21))
#define RTC_IO_TOUCH_PAD0_TIE_OPT_V  0x1
#define RTC_IO_TOUCH_PAD0_TIE_OPT_S  21
/* RTC_IO_TOUCH_PAD0_XPD : R/W ;bitpos:[20] ;default: 1'd0 ; */
/*description: TOUCH_XPD.*/
#define RTC_IO_TOUCH_PAD0_XPD    (BIT(20))
#define RTC_IO_TOUCH_PAD0_XPD_M  (BIT(20))
#define RTC_IO_TOUCH_PAD0_XPD_V  0x1
#define RTC_IO_TOUCH_PAD0_XPD_S  20
/* RTC_IO_TOUCH_PAD0_MUX_SEL : R/W ;bitpos:[19] ;default: 1'd0 ; */
/*description: 1: use RTC GPIO,0: use digital GPIO.*/
#define RTC_IO_TOUCH_PAD0_MUX_SEL    (BIT(19))
#define RTC_IO_TOUCH_PAD0_MUX_SEL_M  (BIT(19))
#define RTC_IO_TOUCH_PAD0_MUX_SEL_V  0x1
#define RTC_IO_TOUCH_PAD0_MUX_SEL_S  19
/* RTC_IO_TOUCH_PAD0_FUN_SEL : R/W ;bitpos:[18:17] ;default: 2'd0 ; */
/*description: function sel.*/
#define RTC_IO_TOUCH_PAD0_FUN_SEL    0x00000003
#define RTC_IO_TOUCH_PAD0_FUN_SEL_M  ((RTC_IO_TOUCH_PAD0_FUN_SEL_V)<<(RTC_IO_TOUCH_PAD0_FUN_SEL_S))
#define RTC_IO_TOUCH_PAD0_FUN_SEL_V  0x3
#define RTC_IO_TOUCH_PAD0_FUN_SEL_S  17
/* RTC_IO_TOUCH_PAD0_SLP_SEL : R/W ;bitpos:[16] ;default: 1'd0 ; */
/*description: 1: enable sleep mode during sleep,0: no sleep mode.*/
#define RTC_IO_TOUCH_PAD0_SLP_SEL    (BIT(16))
#define RTC_IO_TOUCH_PAD0_SLP_SEL_M  (BIT(16))
#define RTC_IO_TOUCH_PAD0_SLP_SEL_V  0x1
#define RTC_IO_TOUCH_PAD0_SLP_SEL_S  16
/* RTC_IO_TOUCH_PAD0_SLP_IE : R/W ;bitpos:[15] ;default: 1'd0 ; */
/*description: input enable in sleep mode.*/
#define RTC_IO_TOUCH_PAD0_SLP_IE    (BIT(15))
#define RTC_IO_TOUCH_PAD0_SLP_IE_M  (BIT(15))
#define RTC_IO_TOUCH_PAD0_SLP_IE_V  0x1
#define RTC_IO_TOUCH_PAD0_SLP_IE_S  15
/* RTC_IO_TOUCH_PAD0_SLP_OE : R/W ;bitpos:[14] ;default: 1'd0 ; */
/*description: output enable in sleep mode.*/
#define RTC_IO_TOUCH_PAD0_SLP_OE    (BIT(14))
#define RTC_IO_TOUCH_PAD0_SLP_OE_M  (BIT(14))
#define RTC_IO_TOUCH_PAD0_SLP_OE_V  0x1
#define RTC_IO_TOUCH_PAD0_SLP_OE_S  14
/* RTC_IO_TOUCH_PAD0_FUN_IE : R/W ;bitpos:[13] ;default: 1'd0 ; */
/*description: input enable in work mode.*/
#define RTC_IO_TOUCH_PAD0_FUN_IE    (BIT(13))
#define RTC_IO_TOUCH_PAD0_FUN_IE_M  (BIT(13))
#define RTC_IO_TOUCH_PAD0_FUN_IE_V  0x1
#define RTC_IO_TOUCH_PAD0_FUN_IE_S  13

#define RTC_IO_TOUCH_PAD1_REG          (DR_REG_RTCIO_BASE + 0x88)
/* RTC_IO_TOUCH_PAD1_DRV : R/W ;bitpos:[30:29] ;default: 2'd2 ; */
/*description: DRV.*/
#define RTC_IO_TOUCH_PAD1_DRV    0x00000003
#define RTC_IO_TOUCH_PAD1_DRV_M  ((RTC_IO_TOUCH_PAD1_DRV_V)<<(RTC_IO_TOUCH_PAD1_DRV_S))
#define RTC_IO_TOUCH_PAD1_DRV_V  0x3
#define RTC_IO_TOUCH_PAD1_DRV_S  29
/* RTC_IO_TOUCH_PAD1_RDE : R/W ;bitpos:[28] ;default: 1'd0 ; */
/*description: RDE.*/
#define RTC_IO_TOUCH_PAD1_RDE    (BIT(28))
#define RTC_IO_TOUCH_PAD1_RDE_M  (BIT(28))
#define RTC_IO_TOUCH_PAD1_RDE_V  0x1
#define RTC_IO_TOUCH_PAD1_RDE_S  28
/* RTC_IO_TOUCH_PAD1_RUE : R/W ;bitpos:[27] ;default: 1'd1 ; */
/*description: RUE.*/
#define RTC_IO_TOUCH_PAD1_RUE    (BIT(27))
#define RTC_IO_TOUCH_PAD1_RUE_M  (BIT(27))
#define RTC_IO_TOUCH_PAD1_RUE_V  0x1
#define RTC_IO_TOUCH_PAD1_RUE_S  27
/* RTC_IO_TOUCH_PAD1_START : R/W ;bitpos:[22] ;default: 1'd0 ; */
/*description: TOUCH_START.*/
#define RTC_IO_TOUCH_PAD1_START    (BIT(22))
#define RTC_IO_TOUCH_PAD1_START_M  (BIT(22))
#define RTC_IO_TOUCH_PAD1_START_V  0x1
#define RTC_IO_TOUCH_PAD1_START_S  22
/* RTC_IO_TOUCH_PAD1_TIE_OPT : R/W ;bitpos:[21] ;default: 1'd0 ; */
/*description: TOUCH_TIE_OPT.*/
#define RTC_IO_TOUCH_PAD1_TIE_OPT    (BIT(21))
#define RTC_IO_TOUCH_PAD1_TIE_OPT_M  (BIT(21))
#define RTC_IO_TOUCH_PAD1_TIE_OPT_V  0x1
#define RTC_IO_TOUCH_PAD1_TIE_OPT_S  21
/* RTC_IO_TOUCH_PAD1_XPD : R/W ;bitpos:[20] ;default: 1'd0 ; */
/*description: TOUCH_XPD.*/
#define RTC_IO_TOUCH_PAD1_XPD    (BIT(20))
#define RTC_IO_TOUCH_PAD1_XPD_M  (BIT(20))
#define RTC_IO_TOUCH_PAD1_XPD_V  0x1
#define RTC_IO_TOUCH_PAD1_XPD_S  20
/* RTC_IO_TOUCH_PAD1_MUX_SEL : R/W ;bitpos:[19] ;default: 1'd0 ; */
/*description: 1: use RTC GPIO,0: use digital GPIO.*/
#define RTC_IO_TOUCH_PAD1_MUX_SEL    (BIT(19))
#define RTC_IO_TOUCH_PAD1_MUX_SEL_M  (BIT(19))
#define RTC_IO_TOUCH_PAD1_MUX_SEL_V  0x1
#define RTC_IO_TOUCH_PAD1_MUX_SEL_S  19
/* RTC_IO_TOUCH_PAD1_FUN_SEL : R/W ;bitpos:[18:17] ;default: 2'd0 ; */
/*description: function sel.*/
#define RTC_IO_TOUCH_PAD1_FUN_SEL    0x00000003
#define RTC_IO_TOUCH_PAD1_FUN_SEL_M  ((RTC_IO_TOUCH_PAD1_FUN_SEL_V)<<(RTC_IO_TOUCH_PAD1_FUN_SEL_S))
#define RTC_IO_TOUCH_PAD1_FUN_SEL_V  0x3
#define RTC_IO_TOUCH_PAD1_FUN_SEL_S  17
/* RTC_IO_TOUCH_PAD1_SLP_SEL : R/W ;bitpos:[16] ;default: 1'd0 ; */
/*description: 1: enable sleep mode during sleep,0: no sleep mode.*/
#define RTC_IO_TOUCH_PAD1_SLP_SEL    (BIT(16))
#define RTC_IO_TOUCH_PAD1_SLP_SEL_M  (BIT(16))
#define RTC_IO_TOUCH_PAD1_SLP_SEL_V  0x1
#define RTC_IO_TOUCH_PAD1_SLP_SEL_S  16
/* RTC_IO_TOUCH_PAD1_SLP_IE : R/W ;bitpos:[15] ;default: 1'd0 ; */
/*description: input enable in sleep mode.*/
#define RTC_IO_TOUCH_PAD1_SLP_IE    (BIT(15))
#define RTC_IO_TOUCH_PAD1_SLP_IE_M  (BIT(15))
#define RTC_IO_TOUCH_PAD1_SLP_IE_V  0x1
#define RTC_IO_TOUCH_PAD1_SLP_IE_S  15
/* RTC_IO_TOUCH_PAD1_SLP_OE : R/W ;bitpos:[14] ;default: 1'd0 ; */
/*description: output enable in sleep mode.*/
#define RTC_IO_TOUCH_PAD1_SLP_OE    (BIT(14))
#define RTC_IO_TOUCH_PAD1_SLP_OE_M  (BIT(14))
#define RTC_IO_TOUCH_PAD1_SLP_OE_V  0x1
#define RTC_IO_TOUCH_PAD1_SLP_OE_S  14
/* RTC_IO_TOUCH_PAD1_FUN_IE : R/W ;bitpos:[13] ;default: 1'd0 ; */
/*description: input enable in work mode.*/
#define RTC_IO_TOUCH_PAD1_FUN_IE    (BIT(13))
#define RTC_IO_TOUCH_PAD1_FUN_IE_M  (BIT(13))
#define RTC_IO_TOUCH_PAD1_FUN_IE_V  0x1
#define RTC_IO_TOUCH_PAD1_FUN_IE_S  13

#define RTC_IO_TOUCH_PAD2_REG          (DR_REG_RTCIO_BASE + 0x8C)
/* RTC_IO_TOUCH_PAD2_DRV : R/W ;bitpos:[30:29] ;default: 2'd2 ; */
/*description: DRV.*/
#define RTC_IO_TOUCH_PAD2_DRV    0x00000003
#define RTC_IO_TOUCH_PAD2_DRV_M  ((RTC_IO_TOUCH_PAD2_DRV_V)<<(RTC_IO_TOUCH_PAD2_DRV_S))
#define RTC_IO_TOUCH_PAD2_DRV_V  0x3
#define RTC_IO_TOUCH_PAD2_DRV_S  29
/* RTC_IO_TOUCH_PAD2_RDE : R/W ;bitpos:[28] ;default: 1'd1 ; */
/*description: RDE.*/
#define RTC_IO_TOUCH_PAD2_RDE    (BIT(28))
#define RTC_IO_TOUCH_PAD2_RDE_M  (BIT(28))
#define RTC_IO_TOUCH_PAD2_RDE_V  0x1
#define RTC_IO_TOUCH_PAD2_RDE_S  28
/* RTC_IO_TOUCH_PAD2_RUE : R/W ;bitpos:[27] ;default: 1'd0 ; */
/*description: RUE.*/
#define RTC_IO_TOUCH_PAD2_RUE    (BIT(27))
#define RTC_IO_TOUCH_PAD2_RUE_M  (BIT(27))
#define RTC_IO_TOUCH_PAD2_RUE_V  0x1
#define RTC_IO_TOUCH_PAD2_RUE_S  27
/* RTC_IO_TOUCH_PAD2_START : R/W ;bitpos:[22] ;default: 1'd0 ; */
/*description: TOUCH_START.*/
#define RTC_IO_TOUCH_PAD2_START    (BIT(22))
#define RTC_IO_TOUCH_PAD2_START_M  (BIT(22))
#define RTC_IO_TOUCH_PAD2_START_V  0x1
#define RTC_IO_TOUCH_PAD2_START_S  22
/* RTC_IO_TOUCH_PAD2_TIE_OPT : R/W ;bitpos:[21] ;default: 1'd0 ; */
/*description: TOUCH_TIE_OPT.*/
#define RTC_IO_TOUCH_PAD2_TIE_OPT    (BIT(21))
#define RTC_IO_TOUCH_PAD2_TIE_OPT_M  (BIT(21))
#define RTC_IO_TOUCH_PAD2_TIE_OPT_V  0x1
#define RTC_IO_TOUCH_PAD2_TIE_OPT_S  21
/* RTC_IO_TOUCH_PAD2_XPD : R/W ;bitpos:[20] ;default: 1'd0 ; */
/*description: TOUCH_XPD.*/
#define RTC_IO_TOUCH_PAD2_XPD    (BIT(20))
#define RTC_IO_TOUCH_PAD2_XPD_M  (BIT(20))
#define RTC_IO_TOUCH_PAD2_XPD_V  0x1
#define RTC_IO_TOUCH_PAD2_XPD_S  20
/* RTC_IO_TOUCH_PAD2_MUX_SEL : R/W ;bitpos:[19] ;default: 1'd0 ; */
/*description: 1: use RTC GPIO,0: use digital GPIO.*/
#define RTC_IO_TOUCH_PAD2_MUX_SEL    (BIT(19))
#define RTC_IO_TOUCH_PAD2_MUX_SEL_M  (BIT(19))
#define RTC_IO_TOUCH_PAD2_MUX_SEL_V  0x1
#define RTC_IO_TOUCH_PAD2_MUX_SEL_S  19
/* RTC_IO_TOUCH_PAD2_FUN_SEL : R/W ;bitpos:[18:17] ;default: 2'd0 ; */
/*description: function sel.*/
#define RTC_IO_TOUCH_PAD2_FUN_SEL    0x00000003
#define RTC_IO_TOUCH_PAD2_FUN_SEL_M  ((RTC_IO_TOUCH_PAD2_FUN_SEL_V)<<(RTC_IO_TOUCH_PAD2_FUN_SEL_S))
#define RTC_IO_TOUCH_PAD2_FUN_SEL_V  0x3
#define RTC_IO_TOUCH_PAD2_FUN_SEL_S  17
/* RTC_IO_TOUCH_PAD2_SLP_SEL : R/W ;bitpos:[16] ;default: 1'd0 ; */
/*description: 1: enable sleep mode during sleep,0: no sleep mode.*/
#define RTC_IO_TOUCH_PAD2_SLP_SEL    (BIT(16))
#define RTC_IO_TOUCH_PAD2_SLP_SEL_M  (BIT(16))
#define RTC_IO_TOUCH_PAD2_SLP_SEL_V  0x1
#define RTC_IO_TOUCH_PAD2_SLP_SEL_S  16
/* RTC_IO_TOUCH_PAD2_SLP_IE : R/W ;bitpos:[15] ;default: 1'd0 ; */
/*description: input enable in sleep mode.*/
#define RTC_IO_TOUCH_PAD2_SLP_IE    (BIT(15))
#define RTC_IO_TOUCH_PAD2_SLP_IE_M  (BIT(15))
#define RTC_IO_TOUCH_PAD2_SLP_IE_V  0x1
#define RTC_IO_TOUCH_PAD2_SLP_IE_S  15
/* RTC_IO_TOUCH_PAD2_SLP_OE : R/W ;bitpos:[14] ;default: 1'd0 ; */
/*description: output enable in sleep mode.*/
#define RTC_IO_TOUCH_PAD2_SLP_OE    (BIT(14))
#define RTC_IO_TOUCH_PAD2_SLP_OE_M  (BIT(14))
#define RTC_IO_TOUCH_PAD2_SLP_OE_V  0x1
#define RTC_IO_TOUCH_PAD2_SLP_OE_S  14
/* RTC_IO_TOUCH_PAD2_FUN_IE : R/W ;bitpos:[13] ;default: 1'd0 ; */
/*description: input enable in work mode.*/
#define RTC_IO_TOUCH_PAD2_FUN_IE    (BIT(13))
#define RTC_IO_TOUCH_PAD2_FUN_IE_M  (BIT(13))
#define RTC_IO_TOUCH_PAD2_FUN_IE_V  0x1
#define RTC_IO_TOUCH_PAD2_FUN_IE_S  13

#define RTC_IO_TOUCH_PAD3_REG          (DR_REG_RTCIO_BASE + 0x90)
/* RTC_IO_TOUCH_PAD3_DRV : R/W ;bitpos:[30:29] ;default: 2'd2 ; */
/*description: DRV.*/
#define RTC_IO_TOUCH_PAD3_DRV    0x00000003
#define RTC_IO_TOUCH_PAD3_DRV_M  ((RTC_IO_TOUCH_PAD3_DRV_V)<<(RTC_IO_TOUCH_PAD3_DRV_S))
#define RTC_IO_TOUCH_PAD3_DRV_V  0x3
#define RTC_IO_TOUCH_PAD3_DRV_S  29
/* RTC_IO_TOUCH_PAD3_RDE : R/W ;bitpos:[28] ;default: 1'd0 ; */
/*description: RDE.*/
#define RTC_IO_TOUCH_PAD3_RDE    (BIT(28))
#define RTC_IO_TOUCH_PAD3_RDE_M  (BIT(28))
#define RTC_IO_TOUCH_PAD3_RDE_V  0x1
#define RTC_IO_TOUCH_PAD3_RDE_S  28
/* RTC_IO_TOUCH_PAD3_RUE : R/W ;bitpos:[27] ;default: 1'd1 ; */
/*description: RUE.*/
#define RTC_IO_TOUCH_PAD3_RUE    (BIT(27))
#define RTC_IO_TOUCH_PAD3_RUE_M  (BIT(27))
#define RTC_IO_TOUCH_PAD3_RUE_V  0x1
#define RTC_IO_TOUCH_PAD3_RUE_S  27
/* RTC_IO_TOUCH_PAD3_START : R/W ;bitpos:[22] ;default: 1'd0 ; */
/*description: TOUCH_START.*/
#define RTC_IO_TOUCH_PAD3_START    (BIT(22))
#define RTC_IO_TOUCH_PAD3_START_M  (BIT(22))
#define RTC_IO_TOUCH_PAD3_START_V  0x1
#define RTC_IO_TOUCH_PAD3_START_S  22
/* RTC_IO_TOUCH_PAD3_TIE_OPT : R/W ;bitpos:[21] ;default: 1'd0 ; */
/*description: TOUCH_TIE_OPT.*/
#define RTC_IO_TOUCH_PAD3_TIE_OPT    (BIT(21))
#define RTC_IO_TOUCH_PAD3_TIE_OPT_M  (BIT(21))
#define RTC_IO_TOUCH_PAD3_TIE_OPT_V  0x1
#define RTC_IO_TOUCH_PAD3_TIE_OPT_S  21
/* RTC_IO_TOUCH_PAD3_XPD : R/W ;bitpos:[20] ;default: 1'd0 ; */
/*description: TOUCH_XPD.*/
#define RTC_IO_TOUCH_PAD3_XPD    (BIT(20))
#define RTC_IO_TOUCH_PAD3_XPD_M  (BIT(20))
#define RTC_IO_TOUCH_PAD3_XPD_V  0x1
#define RTC_IO_TOUCH_PAD3_XPD_S  20
/* RTC_IO_TOUCH_PAD3_MUX_SEL : R/W ;bitpos:[19] ;default: 1'd0 ; */
/*description: 1: use RTC GPIO,0: use digital GPIO.*/
#define RTC_IO_TOUCH_PAD3_MUX_SEL    (BIT(19))
#define RTC_IO_TOUCH_PAD3_MUX_SEL_M  (BIT(19))
#define RTC_IO_TOUCH_PAD3_MUX_SEL_V  0x1
#define RTC_IO_TOUCH_PAD3_MUX_SEL_S  19
/* RTC_IO_TOUCH_PAD3_FUN_SEL : R/W ;bitpos:[18:17] ;default: 2'd0 ; */
/*description: function sel.*/
#define RTC_IO_TOUCH_PAD3_FUN_SEL    0x00000003
#define RTC_IO_TOUCH_PAD3_FUN_SEL_M  ((RTC_IO_TOUCH_PAD3_FUN_SEL_V)<<(RTC_IO_TOUCH_PAD3_FUN_SEL_S))
#define RTC_IO_TOUCH_PAD3_FUN_SEL_V  0x3
#define RTC_IO_TOUCH_PAD3_FUN_SEL_S  17
/* RTC_IO_TOUCH_PAD3_SLP_SEL : R/W ;bitpos:[16] ;default: 1'd0 ; */
/*description: 1: enable sleep mode during sleep,0: no sleep mode.*/
#define RTC_IO_TOUCH_PAD3_SLP_SEL    (BIT(16))
#define RTC_IO_TOUCH_PAD3_SLP_SEL_M  (BIT(16))
#define RTC_IO_TOUCH_PAD3_SLP_SEL_V  0x1
#define RTC_IO_TOUCH_PAD3_SLP_SEL_S  16
/* RTC_IO_TOUCH_PAD3_SLP_IE : R/W ;bitpos:[15] ;default: 1'd0 ; */
/*description: input enable in sleep mode.*/
#define RTC_IO_TOUCH_PAD3_SLP_IE    (BIT(15))
#define RTC_IO_TOUCH_PAD3_SLP_IE_M  (BIT(15))
#define RTC_IO_TOUCH_PAD3_SLP_IE_V  0x1
#define RTC_IO_TOUCH_PAD3_SLP_IE_S  15
/* RTC_IO_TOUCH_PAD3_SLP_OE : R/W ;bitpos:[14] ;default: 1'd0 ; */
/*description: output enable in sleep mode.*/
#define RTC_IO_TOUCH_PAD3_SLP_OE    (BIT(14))
#define RTC_IO_TOUCH_PAD3_SLP_OE_M  (BIT(14))
#define RTC_IO_TOUCH_PAD3_SLP_OE_V  0x1
#define RTC_IO_TOUCH_PAD3_SLP_OE_S  14
/* RTC_IO_TOUCH_PAD3_FUN_IE : R/W ;bitpos:[13] ;default: 1'd0 ; */
/*description: input enable in work mode.*/
#define RTC_IO_TOUCH_PAD3_FUN_IE    (BIT(13))
#define RTC_IO_TOUCH_PAD3_FUN_IE_M  (BIT(13))
#define RTC_IO_TOUCH_PAD3_FUN_IE_V  0x1
#define RTC_IO_TOUCH_PAD3_FUN_IE_S  13

#define RTC_IO_TOUCH_PAD4_REG          (DR_REG_RTCIO_BASE + 0x94)
/* RTC_IO_TOUCH_PAD4_DRV : R/W ;bitpos:[30:29] ;default: 2'd2 ; */
/*description: DRV.*/
#define RTC_IO_TOUCH_PAD4_DRV    0x00000003
#define RTC_IO_TOUCH_PAD4_DRV_M  ((RTC_IO_TOUCH_PAD4_DRV_V)<<(RTC_IO_TOUCH_PAD4_DRV_S))
#define RTC_IO_TOUCH_PAD4_DRV_V  0x3
#define RTC_IO_TOUCH_PAD4_DRV_S  29
/* RTC_IO_TOUCH_PAD4_RDE : R/W ;bitpos:[28] ;default: 1'd1 ; */
/*description: RDE.*/
#define RTC_IO_TOUCH_PAD4_RDE    (BIT(28))
#define RTC_IO_TOUCH_PAD4_RDE_M  (BIT(28))
#define RTC_IO_TOUCH_PAD4_RDE_V  0x1
#define RTC_IO_TOUCH_PAD4_RDE_S  28
/* RTC_IO_TOUCH_PAD4_RUE : R/W ;bitpos:[27] ;default: 1'd0 ; */
/*description: RUE.*/
#define RTC_IO_TOUCH_PAD4_RUE    (BIT(27))
#define RTC_IO_TOUCH_PAD4_RUE_M  (BIT(27))
#define RTC_IO_TOUCH_PAD4_RUE_V  0x1
#define RTC_IO_TOUCH_PAD4_RUE_S  27
/* RTC_IO_TOUCH_PAD4_START : R/W ;bitpos:[22] ;default: 1'd0 ; */
/*description: TOUCH_START.*/
#define RTC_IO_TOUCH_PAD4_START    (BIT(22))
#define RTC_IO_TOUCH_PAD4_START_M  (BIT(22))
#define RTC_IO_TOUCH_PAD4_START_V  0x1
#define RTC_IO_TOUCH_PAD4_START_S  22
/* RTC_IO_TOUCH_PAD4_TIE_OPT : R/W ;bitpos:[21] ;default: 1'd0 ; */
/*description: TOUCH_TIE_OPT.*/
#define RTC_IO_TOUCH_PAD4_TIE_OPT    (BIT(21))
#define RTC_IO_TOUCH_PAD4_TIE_OPT_M  (BIT(21))
#define RTC_IO_TOUCH_PAD4_TIE_OPT_V  0x1
#define RTC_IO_TOUCH_PAD4_TIE_OPT_S  21
/* RTC_IO_TOUCH_PAD4_XPD : R/W ;bitpos:[20] ;default: 1'd0 ; */
/*description: TOUCH_XPD.*/
#define RTC_IO_TOUCH_PAD4_XPD    (BIT(20))
#define RTC_IO_TOUCH_PAD4_XPD_M  (BIT(20))
#define RTC_IO_TOUCH_PAD4_XPD_V  0x1
#define RTC_IO_TOUCH_PAD4_XPD_S  20
/* RTC_IO_TOUCH_PAD4_MUX_SEL : R/W ;bitpos:[19] ;default: 1'd0 ; */
/*description: 1: use RTC GPIO,0: use digital GPIO.*/
#define RTC_IO_TOUCH_PAD4_MUX_SEL    (BIT(19))
#define RTC_IO_TOUCH_PAD4_MUX_SEL_M  (BIT(19))
#define RTC_IO_TOUCH_PAD4_MUX_SEL_V  0x1
#define RTC_IO_TOUCH_PAD4_MUX_SEL_S  19
/* RTC_IO_TOUCH_PAD4_FUN_SEL : R/W ;bitpos:[18:17] ;default: 2'd0 ; */
/*description: function sel.*/
#define RTC_IO_TOUCH_PAD4_FUN_SEL    0x00000003
#define RTC_IO_TOUCH_PAD4_FUN_SEL_M  ((RTC_IO_TOUCH_PAD4_FUN_SEL_V)<<(RTC_IO_TOUCH_PAD4_FUN_SEL_S))
#define RTC_IO_TOUCH_PAD4_FUN_SEL_V  0x3
#define RTC_IO_TOUCH_PAD4_FUN_SEL_S  17
/* RTC_IO_TOUCH_PAD4_SLP_SEL : R/W ;bitpos:[16] ;default: 1'd0 ; */
/*description: 1: enable sleep mode during sleep,0: no sleep mode.*/
#define RTC_IO_TOUCH_PAD4_SLP_SEL    (BIT(16))
#define RTC_IO_TOUCH_PAD4_SLP_SEL_M  (BIT(16))
#define RTC_IO_TOUCH_PAD4_SLP_SEL_V  0x1
#define RTC_IO_TOUCH_PAD4_SLP_SEL_S  16
/* RTC_IO_TOUCH_PAD4_SLP_IE : R/W ;bitpos:[15] ;default: 1'd0 ; */
/*description: input enable in sleep mode.*/
#define RTC_IO_TOUCH_PAD4_SLP_IE    (BIT(15))
#define RTC_IO_TOUCH_PAD4_SLP_IE_M  (BIT(15))
#define RTC_IO_TOUCH_PAD4_SLP_IE_V  0x1
#define RTC_IO_TOUCH_PAD4_SLP_IE_S  15
/* RTC_IO_TOUCH_PAD4_SLP_OE : R/W ;bitpos:[14] ;default: 1'd0 ; */
/*description: output enable in sleep mode.*/
#define RTC_IO_TOUCH_PAD4_SLP_OE    (BIT(14))
#define RTC_IO_TOUCH_PAD4_SLP_OE_M  (BIT(14))
#define RTC_IO_TOUCH_PAD4_SLP_OE_V  0x1
#define RTC_IO_TOUCH_PAD4_SLP_OE_S  14
/* RTC_IO_TOUCH_PAD4_FUN_IE : R/W ;bitpos:[13] ;default: 1'd0 ; */
/*description: input enable in work mode.*/
#define RTC_IO_TOUCH_PAD4_FUN_IE    (BIT(13))
#define RTC_IO_TOUCH_PAD4_FUN_IE_M  (BIT(13))
#define RTC_IO_TOUCH_PAD4_FUN_IE_V  0x1
#define RTC_IO_TOUCH_PAD4_FUN_IE_S  13

#define RTC_IO_TOUCH_PAD5_REG          (DR_REG_RTCIO_BASE + 0x98)
/* RTC_IO_TOUCH_PAD5_DRV : R/W ;bitpos:[30:29] ;default: 2'd2 ; */
/*description: DRV.*/
#define RTC_IO_TOUCH_PAD5_DRV    0x00000003
#define RTC_IO_TOUCH_PAD5_DRV_M  ((RTC_IO_TOUCH_PAD5_DRV_V)<<(RTC_IO_TOUCH_PAD5_DRV_S))
#define RTC_IO_TOUCH_PAD5_DRV_V  0x3
#define RTC_IO_TOUCH_PAD5_DRV_S  29
/* RTC_IO_TOUCH_PAD5_RDE : R/W ;bitpos:[28] ;default: 1'd1 ; */
/*description: RDE.*/
#define RTC_IO_TOUCH_PAD5_RDE    (BIT(28))
#define RTC_IO_TOUCH_PAD5_RDE_M  (BIT(28))
#define RTC_IO_TOUCH_PAD5_RDE_V  0x1
#define RTC_IO_TOUCH_PAD5_RDE_S  28
/* RTC_IO_TOUCH_PAD5_RUE : R/W ;bitpos:[27] ;default: 1'd0 ; */
/*description: RUE.*/
#define RTC_IO_TOUCH_PAD5_RUE    (BIT(27))
#define RTC_IO_TOUCH_PAD5_RUE_M  (BIT(27))
#define RTC_IO_TOUCH_PAD5_RUE_V  0x1
#define RTC_IO_TOUCH_PAD5_RUE_S  27
/* RTC_IO_TOUCH_PAD5_START : R/W ;bitpos:[22] ;default: 1'd0 ; */
/*description: TOUCH_START.*/
#define RTC_IO_TOUCH_PAD5_START    (BIT(22))
#define RTC_IO_TOUCH_PAD5_START_M  (BIT(22))
#define RTC_IO_TOUCH_PAD5_START_V  0x1
#define RTC_IO_TOUCH_PAD5_START_S  22
/* RTC_IO_TOUCH_PAD5_TIE_OPT : R/W ;bitpos:[21] ;default: 1'd0 ; */
/*description: TOUCH_TIE_OPT.*/
#define RTC_IO_TOUCH_PAD5_TIE_OPT    (BIT(21))
#define RTC_IO_TOUCH_PAD5_TIE_OPT_M  (BIT(21))
#define RTC_IO_TOUCH_PAD5_TIE_OPT_V  0x1
#define RTC_IO_TOUCH_PAD5_TIE_OPT_S  21
/* RTC_IO_TOUCH_PAD5_XPD : R/W ;bitpos:[20] ;default: 1'd0 ; */
/*description: TOUCH_XPD.*/
#define RTC_IO_TOUCH_PAD5_XPD    (BIT(20))
#define RTC_IO_TOUCH_PAD5_XPD_M  (BIT(20))
#define RTC_IO_TOUCH_PAD5_XPD_V  0x1
#define RTC_IO_TOUCH_PAD5_XPD_S  20
/* RTC_IO_TOUCH_PAD5_MUX_SEL : R/W ;bitpos:[19] ;default: 1'd0 ; */
/*description: 1: use RTC GPIO,0: use digital GPIO.*/
#define RTC_IO_TOUCH_PAD5_MUX_SEL    (BIT(19))
#define RTC_IO_TOUCH_PAD5_MUX_SEL_M  (BIT(19))
#define RTC_IO_TOUCH_PAD5_MUX_SEL_V  0x1
#define RTC_IO_TOUCH_PAD5_MUX_SEL_S  19
/* RTC_IO_TOUCH_PAD5_FUN_SEL : R/W ;bitpos:[18:17] ;default: 2'd0 ; */
/*description: function sel.*/
#define RTC_IO_TOUCH_PAD5_FUN_SEL    0x00000003
#define RTC_IO_TOUCH_PAD5_FUN_SEL_M  ((RTC_IO_TOUCH_PAD5_FUN_SEL_V)<<(RTC_IO_TOUCH_PAD5_FUN_SEL_S))
#define RTC_IO_TOUCH_PAD5_FUN_SEL_V  0x3
#define RTC_IO_TOUCH_PAD5_FUN_SEL_S  17
/* RTC_IO_TOUCH_PAD5_SLP_SEL : R/W ;bitpos:[16] ;default: 1'd0 ; */
/*description: 1: enable sleep mode during sleep,0: no sleep mode.*/
#define RTC_IO_TOUCH_PAD5_SLP_SEL    (BIT(16))
#define RTC_IO_TOUCH_PAD5_SLP_SEL_M  (BIT(16))
#define RTC_IO_TOUCH_PAD5_SLP_SEL_V  0x1
#define RTC_IO_TOUCH_PAD5_SLP_SEL_S  16
/* RTC_IO_TOUCH_PAD5_SLP_IE : R/W ;bitpos:[15] ;default: 1'd0 ; */
/*description: input enable in sleep mode.*/
#define RTC_IO_TOUCH_PAD5_SLP_IE    (BIT(15))
#define RTC_IO_TOUCH_PAD5_SLP_IE_M  (BIT(15))
#define RTC_IO_TOUCH_PAD5_SLP_IE_V  0x1
#define RTC_IO_TOUCH_PAD5_SLP_IE_S  15
/* RTC_IO_TOUCH_PAD5_SLP_OE : R/W ;bitpos:[14] ;default: 1'd0 ; */
/*description: output enable in sleep mode.*/
#define RTC_IO_TOUCH_PAD5_SLP_OE    (BIT(14))
#define RTC_IO_TOUCH_PAD5_SLP_OE_M  (BIT(14))
#define RTC_IO_TOUCH_PAD5_SLP_OE_V  0x1
#define RTC_IO_TOUCH_PAD5_SLP_OE_S  14
/* RTC_IO_TOUCH_PAD5_FUN_IE : R/W ;bitpos:[13] ;default: 1'd0 ; */
/*description: input enable in work mode.*/
#define RTC_IO_TOUCH_PAD5_FUN_IE    (BIT(13))
#define RTC_IO_TOUCH_PAD5_FUN_IE_M  (BIT(13))
#define RTC_IO_TOUCH_PAD5_FUN_IE_V  0x1
#define RTC_IO_TOUCH_PAD5_FUN_IE_S  13

#define RTC_IO_TOUCH_PAD6_REG          (DR_REG_RTCIO_BASE + 0x9C)
/* RTC_IO_TOUCH_PAD6_DRV : R/W ;bitpos:[30:29] ;default: 2'd2 ; */
/*description: DRV.*/
#define RTC_IO_TOUCH_PAD6_DRV    0x00000003
#define RTC_IO_TOUCH_PAD6_DRV_M  ((RTC_IO_TOUCH_PAD6_DRV_V)<<(RTC_IO_TOUCH_PAD6_DRV_S))
#define RTC_IO_TOUCH_PAD6_DRV_V  0x3
#define RTC_IO_TOUCH_PAD6_DRV_S  29
/* RTC_IO_TOUCH_PAD6_RDE : R/W ;bitpos:[28] ;default: 1'd0 ; */
/*description: RDE.*/
#define RTC_IO_TOUCH_PAD6_RDE    (BIT(28))
#define RTC_IO_TOUCH_PAD6_RDE_M  (BIT(28))
#define RTC_IO_TOUCH_PAD6_RDE_V  0x1
#define RTC_IO_TOUCH_PAD6_RDE_S  28
/* RTC_IO_TOUCH_PAD6_RUE : R/W ;bitpos:[27] ;default: 1'd1 ; */
/*description: RUE.*/
#define RTC_IO_TOUCH_PAD6_RUE    (BIT(27))
#define RTC_IO_TOUCH_PAD6_RUE_M  (BIT(27))
#define RTC_IO_TOUCH_PAD6_RUE_V  0x1
#define RTC_IO_TOUCH_PAD6_RUE_S  27
/* RTC_IO_TOUCH_PAD6_START : R/W ;bitpos:[22] ;default: 1'd0 ; */
/*description: TOUCH_START.*/
#define RTC_IO_TOUCH_PAD6_START    (BIT(22))
#define RTC_IO_TOUCH_PAD6_START_M  (BIT(22))
#define RTC_IO_TOUCH_PAD6_START_V  0x1
#define RTC_IO_TOUCH_PAD6_START_S  22
/* RTC_IO_TOUCH_PAD6_TIE_OPT : R/W ;bitpos:[21] ;default: 1'd0 ; */
/*description: TOUCH_TIE_OPT.*/
#define RTC_IO_TOUCH_PAD6_TIE_OPT    (BIT(21))
#define RTC_IO_TOUCH_PAD6_TIE_OPT_M  (BIT(21))
#define RTC_IO_TOUCH_PAD6_TIE_OPT_V  0x1
#define RTC_IO_TOUCH_PAD6_TIE_OPT_S  21
/* RTC_IO_TOUCH_PAD6_XPD : R/W ;bitpos:[20] ;default: 1'd0 ; */
/*description: TOUCH_XPD.*/
#define RTC_IO_TOUCH_PAD6_XPD    (BIT(20))
#define RTC_IO_TOUCH_PAD6_XPD_M  (BIT(20))
#define RTC_IO_TOUCH_PAD6_XPD_V  0x1
#define RTC_IO_TOUCH_PAD6_XPD_S  20
/* RTC_IO_TOUCH_PAD6_MUX_SEL : R/W ;bitpos:[19] ;default: 1'd0 ; */
/*description: 1: use RTC GPIO,0: use digital GPIO.*/
#define RTC_IO_TOUCH_PAD6_MUX_SEL    (BIT(19))
#define RTC_IO_TOUCH_PAD6_MUX_SEL_M  (BIT(19))
#define RTC_IO_TOUCH_PAD6_MUX_SEL_V  0x1
#define RTC_IO_TOUCH_PAD6_MUX_SEL_S  19
/* RTC_IO_TOUCH_PAD6_FUN_SEL : R/W ;bitpos:[18:17] ;default: 2'd0 ; */
/*description: function sel.*/
#define RTC_IO_TOUCH_PAD6_FUN_SEL    0x00000003
#define RTC_IO_TOUCH_PAD6_FUN_SEL_M  ((RTC_IO_TOUCH_PAD6_FUN_SEL_V)<<(RTC_IO_TOUCH_PAD6_FUN_SEL_S))
#define RTC_IO_TOUCH_PAD6_FUN_SEL_V  0x3
#define RTC_IO_TOUCH_PAD6_FUN_SEL_S  17
/* RTC_IO_TOUCH_PAD6_SLP_SEL : R/W ;bitpos:[16] ;default: 1'd0 ; */
/*description: 1: enable sleep mode during sleep,0: no sleep mode.*/
#define RTC_IO_TOUCH_PAD6_SLP_SEL    (BIT(16))
#define RTC_IO_TOUCH_PAD6_SLP_SEL_M  (BIT(16))
#define RTC_IO_TOUCH_PAD6_SLP_SEL_V  0x1
#define RTC_IO_TOUCH_PAD6_SLP_SEL_S  16
/* RTC_IO_TOUCH_PAD6_SLP_IE : R/W ;bitpos:[15] ;default: 1'd0 ; */
/*description: input enable in sleep mode.*/
#define RTC_IO_TOUCH_PAD6_SLP_IE    (BIT(15))
#define RTC_IO_TOUCH_PAD6_SLP_IE_M  (BIT(15))
#define RTC_IO_TOUCH_PAD6_SLP_IE_V  0x1
#define RTC_IO_TOUCH_PAD6_SLP_IE_S  15
/* RTC_IO_TOUCH_PAD6_SLP_OE : R/W ;bitpos:[14] ;default: 1'd0 ; */
/*description: output enable in sleep mode.*/
#define RTC_IO_TOUCH_PAD6_SLP_OE    (BIT(14))
#define RTC_IO_TOUCH_PAD6_SLP_OE_M  (BIT(14))
#define RTC_IO_TOUCH_PAD6_SLP_OE_V  0x1
#define RTC_IO_TOUCH_PAD6_SLP_OE_S  14
/* RTC_IO_TOUCH_PAD6_FUN_IE : R/W ;bitpos:[13] ;default: 1'd0 ; */
/*description: input enable in work mode.*/
#define RTC_IO_TOUCH_PAD6_FUN_IE    (BIT(13))
#define RTC_IO_TOUCH_PAD6_FUN_IE_M  (BIT(13))
#define RTC_IO_TOUCH_PAD6_FUN_IE_V  0x1
#define RTC_IO_TOUCH_PAD6_FUN_IE_S  13

#define RTC_IO_TOUCH_PAD7_REG          (DR_REG_RTCIO_BASE + 0xA0)
/* RTC_IO_TOUCH_PAD7_DRV : R/W ;bitpos:[30:29] ;default: 2'd2 ; */
/*description: DRV.*/
#define RTC_IO_TOUCH_PAD7_DRV    0x00000003
#define RTC_IO_TOUCH_PAD7_DRV_M  ((RTC_IO_TOUCH_PAD7_DRV_V)<<(RTC_IO_TOUCH_PAD7_DRV_S))
#define RTC_IO_TOUCH_PAD7_DRV_V  0x3
#define RTC_IO_TOUCH_PAD7_DRV_S  29
/* RTC_IO_TOUCH_PAD7_RDE : R/W ;bitpos:[28] ;default: 1'd0 ; */
/*description: RDE.*/
#define RTC_IO_TOUCH_PAD7_RDE    (BIT(28))
#define RTC_IO_TOUCH_PAD7_RDE_M  (BIT(28))
#define RTC_IO_TOUCH_PAD7_RDE_V  0x1
#define RTC_IO_TOUCH_PAD7_RDE_S  28
/* RTC_IO_TOUCH_PAD7_RUE : R/W ;bitpos:[27] ;default: 1'd0 ; */
/*description: RUE.*/
#define RTC_IO_TOUCH_PAD7_RUE    (BIT(27))
#define RTC_IO_TOUCH_PAD7_RUE_M  (BIT(27))
#define RTC_IO_TOUCH_PAD7_RUE_V  0x1
#define RTC_IO_TOUCH_PAD7_RUE_S  27
/* RTC_IO_TOUCH_PAD7_START : R/W ;bitpos:[22] ;default: 1'd0 ; */
/*description: TOUCH_START.*/
#define RTC_IO_TOUCH_PAD7_START    (BIT(22))
#define RTC_IO_TOUCH_PAD7_START_M  (BIT(22))
#define RTC_IO_TOUCH_PAD7_START_V  0x1
#define RTC_IO_TOUCH_PAD7_START_S  22
/* RTC_IO_TOUCH_PAD7_TIE_OPT : R/W ;bitpos:[21] ;default: 1'd0 ; */
/*description: TOUCH_TIE_OPT.*/
#define RTC_IO_TOUCH_PAD7_TIE_OPT    (BIT(21))
#define RTC_IO_TOUCH_PAD7_TIE_OPT_M  (BIT(21))
#define RTC_IO_TOUCH_PAD7_TIE_OPT_V  0x1
#define RTC_IO_TOUCH_PAD7_TIE_OPT_S  21
/* RTC_IO_TOUCH_PAD7_XPD : R/W ;bitpos:[20] ;default: 1'd0 ; */
/*description: TOUCH_XPD.*/
#define RTC_IO_TOUCH_PAD7_XPD    (BIT(20))
#define RTC_IO_TOUCH_PAD7_XPD_M  (BIT(20))
#define RTC_IO_TOUCH_PAD7_XPD_V  0x1
#define RTC_IO_TOUCH_PAD7_XPD_S  20
/* RTC_IO_TOUCH_PAD7_MUX_SEL : R/W ;bitpos:[19] ;default: 1'd0 ; */
/*description: 1: use RTC GPIO,0: use digital GPIO.*/
#define RTC_IO_TOUCH_PAD7_MUX_SEL    (BIT(19))
#define RTC_IO_TOUCH_PAD7_MUX_SEL_M  (BIT(19))
#define RTC_IO_TOUCH_PAD7_MUX_SEL_V  0x1
#define RTC_IO_TOUCH_PAD7_MUX_SEL_S  19
/* RTC_IO_TOUCH_PAD7_FUN_SEL : R/W ;bitpos:[18:17] ;default: 2'd0 ; */
/*description: function sel.*/
#define RTC_IO_TOUCH_PAD7_FUN_SEL    0x00000003
#define RTC_IO_TOUCH_PAD7_FUN_SEL_M  ((RTC_IO_TOUCH_PAD7_FUN_SEL_V)<<(RTC_IO_TOUCH_PAD7_FUN_SEL_S))
#define RTC_IO_TOUCH_PAD7_FUN_SEL_V  0x3
#define RTC_IO_TOUCH_PAD7_FUN_SEL_S  17
/* RTC_IO_TOUCH_PAD7_SLP_SEL : R/W ;bitpos:[16] ;default: 1'd0 ; */
/*description: 1: enable sleep mode during sleep,0: no sleep mode.*/
#define RTC_IO_TOUCH_PAD7_SLP_SEL    (BIT(16))
#define RTC_IO_TOUCH_PAD7_SLP_SEL_M  (BIT(16))
#define RTC_IO_TOUCH_PAD7_SLP_SEL_V  0x1
#define RTC_IO_TOUCH_PAD7_SLP_SEL_S  16
/* RTC_IO_TOUCH_PAD7_SLP_IE : R/W ;bitpos:[15] ;default: 1'd0 ; */
/*description: input enable in sleep mode.*/
#define RTC_IO_TOUCH_PAD7_SLP_IE    (BIT(15))
#define RTC_IO_TOUCH_PAD7_SLP_IE_M  (BIT(15))
#define RTC_IO_TOUCH_PAD7_SLP_IE_V  0x1
#define RTC_IO_TOUCH_PAD7_SLP_IE_S  15
/* RTC_IO_TOUCH_PAD7_SLP_OE : R/W ;bitpos:[14] ;default: 1'd0 ; */
/*description: output enable in sleep mode.*/
#define RTC_IO_TOUCH_PAD7_SLP_OE    (BIT(14))
#define RTC_IO_TOUCH_PAD7_SLP_OE_M  (BIT(14))
#define RTC_IO_TOUCH_PAD7_SLP_OE_V  0x1
#define RTC_IO_TOUCH_PAD7_SLP_OE_S  14
/* RTC_IO_TOUCH_PAD7_FUN_IE : R/W ;bitpos:[13] ;default: 1'd0 ; */
/*description: input enable in work mode.*/
#define RTC_IO_TOUCH_PAD7_FUN_IE    (BIT(13))
#define RTC_IO_TOUCH_PAD7_FUN_IE_M  (BIT(13))
#define RTC_IO_TOUCH_PAD7_FUN_IE_V  0x1
#define RTC_IO_TOUCH_PAD7_FUN_IE_S  13

#define RTC_IO_TOUCH_PAD8_REG          (DR_REG_RTCIO_BASE + 0xA4)
/* RTC_IO_TOUCH_PAD8_DRV : R/W ;bitpos:[30:29] ;default: 2'd2 ; */
/*description: DRV.*/
#define RTC_IO_TOUCH_PAD8_DRV    0x00000003
#define RTC_IO_TOUCH_PAD8_DRV_M  ((RTC_IO_TOUCH_PAD8_DRV_V)<<(RTC_IO_TOUCH_PAD8_DRV_S))
#define RTC_IO_TOUCH_PAD8_DRV_V  0x3
#define RTC_IO_TOUCH_PAD8_DRV_S  29
/* RTC_IO_TOUCH_PAD8_RDE : R/W ;bitpos:[28] ;default: 1'd0 ; */
/*description: RDE.*/
#define RTC_IO_TOUCH_PAD8_RDE    (BIT(28))
#define RTC_IO_TOUCH_PAD8_RDE_M  (BIT(28))
#define RTC_IO_TOUCH_PAD8_RDE_V  0x1
#define RTC_IO_TOUCH_PAD8_RDE_S  28
/* RTC_IO_TOUCH_PAD8_RUE : R/W ;bitpos:[27] ;default: 1'd0 ; */
/*description: RUE.*/
#define RTC_IO_TOUCH_PAD8_RUE    (BIT(27))
#define RTC_IO_TOUCH_PAD8_RUE_M  (BIT(27))
#define RTC_IO_TOUCH_PAD8_RUE_V  0x1
#define RTC_IO_TOUCH_PAD8_RUE_S  27
/* RTC_IO_TOUCH_PAD8_START : R/W ;bitpos:[22] ;default: 1'd0 ; */
/*description: TOUCH_START.*/
#define RTC_IO_TOUCH_PAD8_START    (BIT(22))
#define RTC_IO_TOUCH_PAD8_START_M  (BIT(22))
#define RTC_IO_TOUCH_PAD8_START_V  0x1
#define RTC_IO_TOUCH_PAD8_START_S  22
/* RTC_IO_TOUCH_PAD8_TIE_OPT : R/W ;bitpos:[21] ;default: 1'd0 ; */
/*description: TOUCH_TIE_OPT.*/
#define RTC_IO_TOUCH_PAD8_TIE_OPT    (BIT(21))
#define RTC_IO_TOUCH_PAD8_TIE_OPT_M  (BIT(21))
#define RTC_IO_TOUCH_PAD8_TIE_OPT_V  0x1
#define RTC_IO_TOUCH_PAD8_TIE_OPT_S  21
/* RTC_IO_TOUCH_PAD8_XPD : R/W ;bitpos:[20] ;default: 1'd0 ; */
/*description: TOUCH_XPD.*/
#define RTC_IO_TOUCH_PAD8_XPD    (BIT(20))
#define RTC_IO_TOUCH_PAD8_XPD_M  (BIT(20))
#define RTC_IO_TOUCH_PAD8_XPD_V  0x1
#define RTC_IO_TOUCH_PAD8_XPD_S  20
/* RTC_IO_TOUCH_PAD8_MUX_SEL : R/W ;bitpos:[19] ;default: 1'd0 ; */
/*description: 1: use RTC GPIO,0: use digital GPIO.*/
#define RTC_IO_TOUCH_PAD8_MUX_SEL    (BIT(19))
#define RTC_IO_TOUCH_PAD8_MUX_SEL_M  (BIT(19))
#define RTC_IO_TOUCH_PAD8_MUX_SEL_V  0x1
#define RTC_IO_TOUCH_PAD8_MUX_SEL_S  19
/* RTC_IO_TOUCH_PAD8_FUN_SEL : R/W ;bitpos:[18:17] ;default: 2'd0 ; */
/*description: function sel.*/
#define RTC_IO_TOUCH_PAD8_FUN_SEL    0x00000003
#define RTC_IO_TOUCH_PAD8_FUN_SEL_M  ((RTC_IO_TOUCH_PAD8_FUN_SEL_V)<<(RTC_IO_TOUCH_PAD8_FUN_SEL_S))
#define RTC_IO_TOUCH_PAD8_FUN_SEL_V  0x3
#define RTC_IO_TOUCH_PAD8_FUN_SEL_S  17
/* RTC_IO_TOUCH_PAD8_SLP_SEL : R/W ;bitpos:[16] ;default: 1'd0 ; */
/*description: 1: enable sleep mode during sleep,0: no sleep mode.*/
#define RTC_IO_TOUCH_PAD8_SLP_SEL    (BIT(16))
#define RTC_IO_TOUCH_PAD8_SLP_SEL_M  (BIT(16))
#define RTC_IO_TOUCH_PAD8_SLP_SEL_V  0x1
#define RTC_IO_TOUCH_PAD8_SLP_SEL_S  16
/* RTC_IO_TOUCH_PAD8_SLP_IE : R/W ;bitpos:[15] ;default: 1'd0 ; */
/*description: input enable in sleep mode.*/
#define RTC_IO_TOUCH_PAD8_SLP_IE    (BIT(15))
#define RTC_IO_TOUCH_PAD8_SLP_IE_M  (BIT(15))
#define RTC_IO_TOUCH_PAD8_SLP_IE_V  0x1
#define RTC_IO_TOUCH_PAD8_SLP_IE_S  15
/* RTC_IO_TOUCH_PAD8_SLP_OE : R/W ;bitpos:[14] ;default: 1'd0 ; */
/*description: output enable in sleep mode.*/
#define RTC_IO_TOUCH_PAD8_SLP_OE    (BIT(14))
#define RTC_IO_TOUCH_PAD8_SLP_OE_M  (BIT(14))
#define RTC_IO_TOUCH_PAD8_SLP_OE_V  0x1
#define RTC_IO_TOUCH_PAD8_SLP_OE_S  14
/* RTC_IO_TOUCH_PAD8_FUN_IE : R/W ;bitpos:[13] ;default: 1'd0 ; */
/*description: input enable in work mode.*/
#define RTC_IO_TOUCH_PAD8_FUN_IE    (BIT(13))
#define RTC_IO_TOUCH_PAD8_FUN_IE_M  (BIT(13))
#define RTC_IO_TOUCH_PAD8_FUN_IE_V  0x1
#define RTC_IO_TOUCH_PAD8_FUN_IE_S  13

#define RTC_IO_TOUCH_PAD9_REG          (DR_REG_RTCIO_BASE + 0xA8)
/* RTC_IO_TOUCH_PAD9_DRV : R/W ;bitpos:[30:29] ;default: 2'd2 ; */
/*description: DRV.*/
#define RTC_IO_TOUCH_PAD9_DRV    0x00000003
#define RTC_IO_TOUCH_PAD9_DRV_M  ((RTC_IO_TOUCH_PAD9_DRV_V)<<(RTC_IO_TOUCH_PAD9_DRV_S))
#define RTC_IO_TOUCH_PAD9_DRV_V  0x3
#define RTC_IO_TOUCH_PAD9_DRV_S  29
/* RTC_IO_TOUCH_PAD9_RDE : R/W ;bitpos:[28] ;default: 1'd0 ; */
/*description: RDE.*/
#define RTC_IO_TOUCH_PAD9_RDE    (BIT(28))
#define RTC_IO_TOUCH_PAD9_RDE_M  (BIT(28))
#define RTC_IO_TOUCH_PAD9_RDE_V  0x1
#define RTC_IO_TOUCH_PAD9_RDE_S  28
/* RTC_IO_TOUCH_PAD9_RUE : R/W ;bitpos:[27] ;default: 1'd0 ; */
/*description: RUE.*/
#define RTC_IO_TOUCH_PAD9_RUE    (BIT(27))
#define RTC_IO_TOUCH_PAD9_RUE_M  (BIT(27))
#define RTC_IO_TOUCH_PAD9_RUE_V  0x1
#define RTC_IO_TOUCH_PAD9_RUE_S  27
/* RTC_IO_TOUCH_PAD9_START : R/W ;bitpos:[22] ;default: 1'd0 ; */
/*description: TOUCH_START.*/
#define RTC_IO_TOUCH_PAD9_START    (BIT(22))
#define RTC_IO_TOUCH_PAD9_START_M  (BIT(22))
#define RTC_IO_TOUCH_PAD9_START_V  0x1
#define RTC_IO_TOUCH_PAD9_START_S  22
/* RTC_IO_TOUCH_PAD9_TIE_OPT : R/W ;bitpos:[21] ;default: 1'd0 ; */
/*description: TOUCH_TIE_OPT.*/
#define RTC_IO_TOUCH_PAD9_TIE_OPT    (BIT(21))
#define RTC_IO_TOUCH_PAD9_TIE_OPT_M  (BIT(21))
#define RTC_IO_TOUCH_PAD9_TIE_OPT_V  0x1
#define RTC_IO_TOUCH_PAD9_TIE_OPT_S  21
/* RTC_IO_TOUCH_PAD9_XPD : R/W ;bitpos:[20] ;default: 1'd0 ; */
/*description: TOUCH_XPD.*/
#define RTC_IO_TOUCH_PAD9_XPD    (BIT(20))
#define RTC_IO_TOUCH_PAD9_XPD_M  (BIT(20))
#define RTC_IO_TOUCH_PAD9_XPD_V  0x1
#define RTC_IO_TOUCH_PAD9_XPD_S  20
/* RTC_IO_TOUCH_PAD9_MUX_SEL : R/W ;bitpos:[19] ;default: 1'd0 ; */
/*description: 1: use RTC GPIO,0: use digital GPIO.*/
#define RTC_IO_TOUCH_PAD9_MUX_SEL    (BIT(19))
#define RTC_IO_TOUCH_PAD9_MUX_SEL_M  (BIT(19))
#define RTC_IO_TOUCH_PAD9_MUX_SEL_V  0x1
#define RTC_IO_TOUCH_PAD9_MUX_SEL_S  19
/* RTC_IO_TOUCH_PAD9_FUN_SEL : R/W ;bitpos:[18:17] ;default: 2'd0 ; */
/*description: function sel.*/
#define RTC_IO_TOUCH_PAD9_FUN_SEL    0x00000003
#define RTC_IO_TOUCH_PAD9_FUN_SEL_M  ((RTC_IO_TOUCH_PAD9_FUN_SEL_V)<<(RTC_IO_TOUCH_PAD9_FUN_SEL_S))
#define RTC_IO_TOUCH_PAD9_FUN_SEL_V  0x3
#define RTC_IO_TOUCH_PAD9_FUN_SEL_S  17
/* RTC_IO_TOUCH_PAD9_SLP_SEL : R/W ;bitpos:[16] ;default: 1'd0 ; */
/*description: 1: enable sleep mode during sleep,0: no sleep mode.*/
#define RTC_IO_TOUCH_PAD9_SLP_SEL    (BIT(16))
#define RTC_IO_TOUCH_PAD9_SLP_SEL_M  (BIT(16))
#define RTC_IO_TOUCH_PAD9_SLP_SEL_V  0x1
#define RTC_IO_TOUCH_PAD9_SLP_SEL_S  16
/* RTC_IO_TOUCH_PAD9_SLP_IE : R/W ;bitpos:[15] ;default: 1'd0 ; */
/*description: input enable in sleep mode.*/
#define RTC_IO_TOUCH_PAD9_SLP_IE    (BIT(15))
#define RTC_IO_TOUCH_PAD9_SLP_IE_M  (BIT(15))
#define RTC_IO_TOUCH_PAD9_SLP_IE_V  0x1
#define RTC_IO_TOUCH_PAD9_SLP_IE_S  15
/* RTC_IO_TOUCH_PAD9_SLP_OE : R/W ;bitpos:[14] ;default: 1'd0 ; */
/*description: output enable in sleep mode.*/
#define RTC_IO_TOUCH_PAD9_SLP_OE    (BIT(14))
#define RTC_IO_TOUCH_PAD9_SLP_OE_M  (BIT(14))
#define RTC_IO_TOUCH_PAD9_SLP_OE_V  0x1
#define RTC_IO_TOUCH_PAD9_SLP_OE_S  14
/* RTC_IO_TOUCH_PAD9_FUN_IE : R/W ;bitpos:[13] ;default: 1'd0 ; */
/*description: input enable in work mode.*/
#define RTC_IO_TOUCH_PAD9_FUN_IE    (BIT(13))
#define RTC_IO_TOUCH_PAD9_FUN_IE_M  (BIT(13))
#define RTC_IO_TOUCH_PAD9_FUN_IE_V  0x1
#define RTC_IO_TOUCH_PAD9_FUN_IE_S  13

#define RTC_IO_TOUCH_PAD10_REG          (DR_REG_RTCIO_BASE + 0xAC)
/* RTC_IO_TOUCH_PAD10_DRV : R/W ;bitpos:[30:29] ;default: 2'd2 ; */
/*description: DRV.*/
#define RTC_IO_TOUCH_PAD10_DRV    0x00000003
#define RTC_IO_TOUCH_PAD10_DRV_M  ((RTC_IO_TOUCH_PAD10_DRV_V)<<(RTC_IO_TOUCH_PAD10_DRV_S))
#define RTC_IO_TOUCH_PAD10_DRV_V  0x3
#define RTC_IO_TOUCH_PAD10_DRV_S  29
/* RTC_IO_TOUCH_PAD10_RDE : R/W ;bitpos:[28] ;default: 1'd0 ; */
/*description: RDE.*/
#define RTC_IO_TOUCH_PAD10_RDE    (BIT(28))
#define RTC_IO_TOUCH_PAD10_RDE_M  (BIT(28))
#define RTC_IO_TOUCH_PAD10_RDE_V  0x1
#define RTC_IO_TOUCH_PAD10_RDE_S  28
/* RTC_IO_TOUCH_PAD10_RUE : R/W ;bitpos:[27] ;default: 1'd0 ; */
/*description: RUE.*/
#define RTC_IO_TOUCH_PAD10_RUE    (BIT(27))
#define RTC_IO_TOUCH_PAD10_RUE_M  (BIT(27))
#define RTC_IO_TOUCH_PAD10_RUE_V  0x1
#define RTC_IO_TOUCH_PAD10_RUE_S  27
/* RTC_IO_TOUCH_PAD10_START : R/W ;bitpos:[22] ;default: 1'd0 ; */
/*description: TOUCH_START.*/
#define RTC_IO_TOUCH_PAD10_START    (BIT(22))
#define RTC_IO_TOUCH_PAD10_START_M  (BIT(22))
#define RTC_IO_TOUCH_PAD10_START_V  0x1
#define RTC_IO_TOUCH_PAD10_START_S  22
/* RTC_IO_TOUCH_PAD10_TIE_OPT : R/W ;bitpos:[21] ;default: 1'd0 ; */
/*description: TOUCH_TIE_OPT.*/
#define RTC_IO_TOUCH_PAD10_TIE_OPT    (BIT(21))
#define RTC_IO_TOUCH_PAD10_TIE_OPT_M  (BIT(21))
#define RTC_IO_TOUCH_PAD10_TIE_OPT_V  0x1
#define RTC_IO_TOUCH_PAD10_TIE_OPT_S  21
/* RTC_IO_TOUCH_PAD10_XPD : R/W ;bitpos:[20] ;default: 1'd0 ; */
/*description: TOUCH_XPD.*/
#define RTC_IO_TOUCH_PAD10_XPD    (BIT(20))
#define RTC_IO_TOUCH_PAD10_XPD_M  (BIT(20))
#define RTC_IO_TOUCH_PAD10_XPD_V  0x1
#define RTC_IO_TOUCH_PAD10_XPD_S  20
/* RTC_IO_TOUCH_PAD10_MUX_SEL : R/W ;bitpos:[19] ;default: 1'd0 ; */
/*description: 1: use RTC GPIO,0: use digital GPIO.*/
#define RTC_IO_TOUCH_PAD10_MUX_SEL    (BIT(19))
#define RTC_IO_TOUCH_PAD10_MUX_SEL_M  (BIT(19))
#define RTC_IO_TOUCH_PAD10_MUX_SEL_V  0x1
#define RTC_IO_TOUCH_PAD10_MUX_SEL_S  19
/* RTC_IO_TOUCH_PAD10_FUN_SEL : R/W ;bitpos:[18:17] ;default: 2'd0 ; */
/*description: function sel.*/
#define RTC_IO_TOUCH_PAD10_FUN_SEL    0x00000003
#define RTC_IO_TOUCH_PAD10_FUN_SEL_M  ((RTC_IO_TOUCH_PAD10_FUN_SEL_V)<<(RTC_IO_TOUCH_PAD10_FUN_SEL_S))
#define RTC_IO_TOUCH_PAD10_FUN_SEL_V  0x3
#define RTC_IO_TOUCH_PAD10_FUN_SEL_S  17
/* RTC_IO_TOUCH_PAD10_SLP_SEL : R/W ;bitpos:[16] ;default: 1'd0 ; */
/*description: 1: enable sleep mode during sleep,0: no sleep mode.*/
#define RTC_IO_TOUCH_PAD10_SLP_SEL    (BIT(16))
#define RTC_IO_TOUCH_PAD10_SLP_SEL_M  (BIT(16))
#define RTC_IO_TOUCH_PAD10_SLP_SEL_V  0x1
#define RTC_IO_TOUCH_PAD10_SLP_SEL_S  16
/* RTC_IO_TOUCH_PAD10_SLP_IE : R/W ;bitpos:[15] ;default: 1'd0 ; */
/*description: input enable in sleep mode.*/
#define RTC_IO_TOUCH_PAD10_SLP_IE    (BIT(15))
#define RTC_IO_TOUCH_PAD10_SLP_IE_M  (BIT(15))
#define RTC_IO_TOUCH_PAD10_SLP_IE_V  0x1
#define RTC_IO_TOUCH_PAD10_SLP_IE_S  15
/* RTC_IO_TOUCH_PAD10_SLP_OE : R/W ;bitpos:[14] ;default: 1'd0 ; */
/*description: output enable in sleep mode.*/
#define RTC_IO_TOUCH_PAD10_SLP_OE    (BIT(14))
#define RTC_IO_TOUCH_PAD10_SLP_OE_M  (BIT(14))
#define RTC_IO_TOUCH_PAD10_SLP_OE_V  0x1
#define RTC_IO_TOUCH_PAD10_SLP_OE_S  14
/* RTC_IO_TOUCH_PAD10_FUN_IE : R/W ;bitpos:[13] ;default: 1'd0 ; */
/*description: input enable in work mode.*/
#define RTC_IO_TOUCH_PAD10_FUN_IE    (BIT(13))
#define RTC_IO_TOUCH_PAD10_FUN_IE_M  (BIT(13))
#define RTC_IO_TOUCH_PAD10_FUN_IE_V  0x1
#define RTC_IO_TOUCH_PAD10_FUN_IE_S  13

#define RTC_IO_TOUCH_PAD11_REG          (DR_REG_RTCIO_BASE + 0xB0)
/* RTC_IO_TOUCH_PAD11_DRV : R/W ;bitpos:[30:29] ;default: 2'd2 ; */
/*description: DRV.*/
#define RTC_IO_TOUCH_PAD11_DRV    0x00000003
#define RTC_IO_TOUCH_PAD11_DRV_M  ((RTC_IO_TOUCH_PAD11_DRV_V)<<(RTC_IO_TOUCH_PAD11_DRV_S))
#define RTC_IO_TOUCH_PAD11_DRV_V  0x3
#define RTC_IO_TOUCH_PAD11_DRV_S  29
/* RTC_IO_TOUCH_PAD11_RDE : R/W ;bitpos:[28] ;default: 1'd0 ; */
/*description: RDE.*/
#define RTC_IO_TOUCH_PAD11_RDE    (BIT(28))
#define RTC_IO_TOUCH_PAD11_RDE_M  (BIT(28))
#define RTC_IO_TOUCH_PAD11_RDE_V  0x1
#define RTC_IO_TOUCH_PAD11_RDE_S  28
/* RTC_IO_TOUCH_PAD11_RUE : R/W ;bitpos:[27] ;default: 1'd0 ; */
/*description: RUE.*/
#define RTC_IO_TOUCH_PAD11_RUE    (BIT(27))
#define RTC_IO_TOUCH_PAD11_RUE_M  (BIT(27))
#define RTC_IO_TOUCH_PAD11_RUE_V  0x1
#define RTC_IO_TOUCH_PAD11_RUE_S  27
/* RTC_IO_TOUCH_PAD11_START : R/W ;bitpos:[22] ;default: 1'd0 ; */
/*description: TOUCH_START.*/
#define RTC_IO_TOUCH_PAD11_START    (BIT(22))
#define RTC_IO_TOUCH_PAD11_START_M  (BIT(22))
#define RTC_IO_TOUCH_PAD11_START_V  0x1
#define RTC_IO_TOUCH_PAD11_START_S  22
/* RTC_IO_TOUCH_PAD11_TIE_OPT : R/W ;bitpos:[21] ;default: 1'd0 ; */
/*description: TOUCH_TIE_OPT.*/
#define RTC_IO_TOUCH_PAD11_TIE_OPT    (BIT(21))
#define RTC_IO_TOUCH_PAD11_TIE_OPT_M  (BIT(21))
#define RTC_IO_TOUCH_PAD11_TIE_OPT_V  0x1
#define RTC_IO_TOUCH_PAD11_TIE_OPT_S  21
/* RTC_IO_TOUCH_PAD11_XPD : R/W ;bitpos:[20] ;default: 1'd0 ; */
/*description: TOUCH_XPD.*/
#define RTC_IO_TOUCH_PAD11_XPD    (BIT(20))
#define RTC_IO_TOUCH_PAD11_XPD_M  (BIT(20))
#define RTC_IO_TOUCH_PAD11_XPD_V  0x1
#define RTC_IO_TOUCH_PAD11_XPD_S  20
/* RTC_IO_TOUCH_PAD11_MUX_SEL : R/W ;bitpos:[19] ;default: 1'd0 ; */
/*description: 1: use RTC GPIO,0: use digital GPIO.*/
#define RTC_IO_TOUCH_PAD11_MUX_SEL    (BIT(19))
#define RTC_IO_TOUCH_PAD11_MUX_SEL_M  (BIT(19))
#define RTC_IO_TOUCH_PAD11_MUX_SEL_V  0x1
#define RTC_IO_TOUCH_PAD11_MUX_SEL_S  19
/* RTC_IO_TOUCH_PAD11_FUN_SEL : R/W ;bitpos:[18:17] ;default: 2'd0 ; */
/*description: function sel.*/
#define RTC_IO_TOUCH_PAD11_FUN_SEL    0x00000003
#define RTC_IO_TOUCH_PAD11_FUN_SEL_M  ((RTC_IO_TOUCH_PAD11_FUN_SEL_V)<<(RTC_IO_TOUCH_PAD11_FUN_SEL_S))
#define RTC_IO_TOUCH_PAD11_FUN_SEL_V  0x3
#define RTC_IO_TOUCH_PAD11_FUN_SEL_S  17
/* RTC_IO_TOUCH_PAD11_SLP_SEL : R/W ;bitpos:[16] ;default: 1'd0 ; */
/*description: 1: enable sleep mode during sleep,0: no sleep mode.*/
#define RTC_IO_TOUCH_PAD11_SLP_SEL    (BIT(16))
#define RTC_IO_TOUCH_PAD11_SLP_SEL_M  (BIT(16))
#define RTC_IO_TOUCH_PAD11_SLP_SEL_V  0x1
#define RTC_IO_TOUCH_PAD11_SLP_SEL_S  16
/* RTC_IO_TOUCH_PAD11_SLP_IE : R/W ;bitpos:[15] ;default: 1'd0 ; */
/*description: input enable in sleep mode.*/
#define RTC_IO_TOUCH_PAD11_SLP_IE    (BIT(15))
#define RTC_IO_TOUCH_PAD11_SLP_IE_M  (BIT(15))
#define RTC_IO_TOUCH_PAD11_SLP_IE_V  0x1
#define RTC_IO_TOUCH_PAD11_SLP_IE_S  15
/* RTC_IO_TOUCH_PAD11_SLP_OE : R/W ;bitpos:[14] ;default: 1'd0 ; */
/*description: output enable in sleep mode.*/
#define RTC_IO_TOUCH_PAD11_SLP_OE    (BIT(14))
#define RTC_IO_TOUCH_PAD11_SLP_OE_M  (BIT(14))
#define RTC_IO_TOUCH_PAD11_SLP_OE_V  0x1
#define RTC_IO_TOUCH_PAD11_SLP_OE_S  14
/* RTC_IO_TOUCH_PAD11_FUN_IE : R/W ;bitpos:[13] ;default: 1'd0 ; */
/*description: input enable in work mode.*/
#define RTC_IO_TOUCH_PAD11_FUN_IE    (BIT(13))
#define RTC_IO_TOUCH_PAD11_FUN_IE_M  (BIT(13))
#define RTC_IO_TOUCH_PAD11_FUN_IE_V  0x1
#define RTC_IO_TOUCH_PAD11_FUN_IE_S  13

#define RTC_IO_TOUCH_PAD12_REG          (DR_REG_RTCIO_BASE + 0xB4)
/* RTC_IO_TOUCH_PAD12_DRV : R/W ;bitpos:[30:29] ;default: 2'd2 ; */
/*description: DRV.*/
#define RTC_IO_TOUCH_PAD12_DRV    0x00000003
#define RTC_IO_TOUCH_PAD12_DRV_M  ((RTC_IO_TOUCH_PAD12_DRV_V)<<(RTC_IO_TOUCH_PAD12_DRV_S))
#define RTC_IO_TOUCH_PAD12_DRV_V  0x3
#define RTC_IO_TOUCH_PAD12_DRV_S  29
/* RTC_IO_TOUCH_PAD12_RDE : R/W ;bitpos:[28] ;default: 1'd0 ; */
/*description: RDE.*/
#define RTC_IO_TOUCH_PAD12_RDE    (BIT(28))
#define RTC_IO_TOUCH_PAD12_RDE_M  (BIT(28))
#define RTC_IO_TOUCH_PAD12_RDE_V  0x1
#define RTC_IO_TOUCH_PAD12_RDE_S  28
/* RTC_IO_TOUCH_PAD12_RUE : R/W ;bitpos:[27] ;default: 1'd0 ; */
/*description: RUE.*/
#define RTC_IO_TOUCH_PAD12_RUE    (BIT(27))
#define RTC_IO_TOUCH_PAD12_RUE_M  (BIT(27))
#define RTC_IO_TOUCH_PAD12_RUE_V  0x1
#define RTC_IO_TOUCH_PAD12_RUE_S  27
/* RTC_IO_TOUCH_PAD12_START : R/W ;bitpos:[22] ;default: 1'd0 ; */
/*description: TOUCH_START.*/
#define RTC_IO_TOUCH_PAD12_START    (BIT(22))
#define RTC_IO_TOUCH_PAD12_START_M  (BIT(22))
#define RTC_IO_TOUCH_PAD12_START_V  0x1
#define RTC_IO_TOUCH_PAD12_START_S  22
/* RTC_IO_TOUCH_PAD12_TIE_OPT : R/W ;bitpos:[21] ;default: 1'd0 ; */
/*description: TOUCH_TIE_OPT.*/
#define RTC_IO_TOUCH_PAD12_TIE_OPT    (BIT(21))
#define RTC_IO_TOUCH_PAD12_TIE_OPT_M  (BIT(21))
#define RTC_IO_TOUCH_PAD12_TIE_OPT_V  0x1
#define RTC_IO_TOUCH_PAD12_TIE_OPT_S  21
/* RTC_IO_TOUCH_PAD12_XPD : R/W ;bitpos:[20] ;default: 1'd0 ; */
/*description: TOUCH_XPD.*/
#define RTC_IO_TOUCH_PAD12_XPD    (BIT(20))
#define RTC_IO_TOUCH_PAD12_XPD_M  (BIT(20))
#define RTC_IO_TOUCH_PAD12_XPD_V  0x1
#define RTC_IO_TOUCH_PAD12_XPD_S  20
/* RTC_IO_TOUCH_PAD12_MUX_SEL : R/W ;bitpos:[19] ;default: 1'd0 ; */
/*description: 1: use RTC GPIO,0: use digital GPIO.*/
#define RTC_IO_TOUCH_PAD12_MUX_SEL    (BIT(19))
#define RTC_IO_TOUCH_PAD12_MUX_SEL_M  (BIT(19))
#define RTC_IO_TOUCH_PAD12_MUX_SEL_V  0x1
#define RTC_IO_TOUCH_PAD12_MUX_SEL_S  19
/* RTC_IO_TOUCH_PAD12_FUN_SEL : R/W ;bitpos:[18:17] ;default: 2'd0 ; */
/*description: function sel.*/
#define RTC_IO_TOUCH_PAD12_FUN_SEL    0x00000003
#define RTC_IO_TOUCH_PAD12_FUN_SEL_M  ((RTC_IO_TOUCH_PAD12_FUN_SEL_V)<<(RTC_IO_TOUCH_PAD12_FUN_SEL_S))
#define RTC_IO_TOUCH_PAD12_FUN_SEL_V  0x3
#define RTC_IO_TOUCH_PAD12_FUN_SEL_S  17
/* RTC_IO_TOUCH_PAD12_SLP_SEL : R/W ;bitpos:[16] ;default: 1'd0 ; */
/*description: 1: enable sleep mode during sleep,0: no sleep mode.*/
#define RTC_IO_TOUCH_PAD12_SLP_SEL    (BIT(16))
#define RTC_IO_TOUCH_PAD12_SLP_SEL_M  (BIT(16))
#define RTC_IO_TOUCH_PAD12_SLP_SEL_V  0x1
#define RTC_IO_TOUCH_PAD12_SLP_SEL_S  16
/* RTC_IO_TOUCH_PAD12_SLP_IE : R/W ;bitpos:[15] ;default: 1'd0 ; */
/*description: input enable in sleep mode.*/
#define RTC_IO_TOUCH_PAD12_SLP_IE    (BIT(15))
#define RTC_IO_TOUCH_PAD12_SLP_IE_M  (BIT(15))
#define RTC_IO_TOUCH_PAD12_SLP_IE_V  0x1
#define RTC_IO_TOUCH_PAD12_SLP_IE_S  15
/* RTC_IO_TOUCH_PAD12_SLP_OE : R/W ;bitpos:[14] ;default: 1'd0 ; */
/*description: output enable in sleep mode.*/
#define RTC_IO_TOUCH_PAD12_SLP_OE    (BIT(14))
#define RTC_IO_TOUCH_PAD12_SLP_OE_M  (BIT(14))
#define RTC_IO_TOUCH_PAD12_SLP_OE_V  0x1
#define RTC_IO_TOUCH_PAD12_SLP_OE_S  14
/* RTC_IO_TOUCH_PAD12_FUN_IE : R/W ;bitpos:[13] ;default: 1'd0 ; */
/*description: input enable in work mode.*/
#define RTC_IO_TOUCH_PAD12_FUN_IE    (BIT(13))
#define RTC_IO_TOUCH_PAD12_FUN_IE_M  (BIT(13))
#define RTC_IO_TOUCH_PAD12_FUN_IE_V  0x1
#define RTC_IO_TOUCH_PAD12_FUN_IE_S  13

#define RTC_IO_TOUCH_PAD13_REG          (DR_REG_RTCIO_BASE + 0xB8)
/* RTC_IO_TOUCH_PAD13_DRV : R/W ;bitpos:[30:29] ;default: 2'd2 ; */
/*description: DRV.*/
#define RTC_IO_TOUCH_PAD13_DRV    0x00000003
#define RTC_IO_TOUCH_PAD13_DRV_M  ((RTC_IO_TOUCH_PAD13_DRV_V)<<(RTC_IO_TOUCH_PAD13_DRV_S))
#define RTC_IO_TOUCH_PAD13_DRV_V  0x3
#define RTC_IO_TOUCH_PAD13_DRV_S  29
/* RTC_IO_TOUCH_PAD13_RDE : R/W ;bitpos:[28] ;default: 1'd0 ; */
/*description: RDE.*/
#define RTC_IO_TOUCH_PAD13_RDE    (BIT(28))
#define RTC_IO_TOUCH_PAD13_RDE_M  (BIT(28))
#define RTC_IO_TOUCH_PAD13_RDE_V  0x1
#define RTC_IO_TOUCH_PAD13_RDE_S  28
/* RTC_IO_TOUCH_PAD13_RUE : R/W ;bitpos:[27] ;default: 1'd0 ; */
/*description: RUE.*/
#define RTC_IO_TOUCH_PAD13_RUE    (BIT(27))
#define RTC_IO_TOUCH_PAD13_RUE_M  (BIT(27))
#define RTC_IO_TOUCH_PAD13_RUE_V  0x1
#define RTC_IO_TOUCH_PAD13_RUE_S  27
/* RTC_IO_TOUCH_PAD13_START : R/W ;bitpos:[22] ;default: 1'd0 ; */
/*description: TOUCH_START.*/
#define RTC_IO_TOUCH_PAD13_START    (BIT(22))
#define RTC_IO_TOUCH_PAD13_START_M  (BIT(22))
#define RTC_IO_TOUCH_PAD13_START_V  0x1
#define RTC_IO_TOUCH_PAD13_START_S  22
/* RTC_IO_TOUCH_PAD13_TIE_OPT : R/W ;bitpos:[21] ;default: 1'd0 ; */
/*description: TOUCH_TIE_OPT.*/
#define RTC_IO_TOUCH_PAD13_TIE_OPT    (BIT(21))
#define RTC_IO_TOUCH_PAD13_TIE_OPT_M  (BIT(21))
#define RTC_IO_TOUCH_PAD13_TIE_OPT_V  0x1
#define RTC_IO_TOUCH_PAD13_TIE_OPT_S  21
/* RTC_IO_TOUCH_PAD13_XPD : R/W ;bitpos:[20] ;default: 1'd0 ; */
/*description: TOUCH_XPD.*/
#define RTC_IO_TOUCH_PAD13_XPD    (BIT(20))
#define RTC_IO_TOUCH_PAD13_XPD_M  (BIT(20))
#define RTC_IO_TOUCH_PAD13_XPD_V  0x1
#define RTC_IO_TOUCH_PAD13_XPD_S  20
/* RTC_IO_TOUCH_PAD13_MUX_SEL : R/W ;bitpos:[19] ;default: 1'd0 ; */
/*description: 1: use RTC GPIO,0: use digital GPIO.*/
#define RTC_IO_TOUCH_PAD13_MUX_SEL    (BIT(19))
#define RTC_IO_TOUCH_PAD13_MUX_SEL_M  (BIT(19))
#define RTC_IO_TOUCH_PAD13_MUX_SEL_V  0x1
#define RTC_IO_TOUCH_PAD13_MUX_SEL_S  19
/* RTC_IO_TOUCH_PAD13_FUN_SEL : R/W ;bitpos:[18:17] ;default: 2'd0 ; */
/*description: function sel.*/
#define RTC_IO_TOUCH_PAD13_FUN_SEL    0x00000003
#define RTC_IO_TOUCH_PAD13_FUN_SEL_M  ((RTC_IO_TOUCH_PAD13_FUN_SEL_V)<<(RTC_IO_TOUCH_PAD13_FUN_SEL_S))
#define RTC_IO_TOUCH_PAD13_FUN_SEL_V  0x3
#define RTC_IO_TOUCH_PAD13_FUN_SEL_S  17
/* RTC_IO_TOUCH_PAD13_SLP_SEL : R/W ;bitpos:[16] ;default: 1'd0 ; */
/*description: 1: enable sleep mode during sleep,0: no sleep mode.*/
#define RTC_IO_TOUCH_PAD13_SLP_SEL    (BIT(16))
#define RTC_IO_TOUCH_PAD13_SLP_SEL_M  (BIT(16))
#define RTC_IO_TOUCH_PAD13_SLP_SEL_V  0x1
#define RTC_IO_TOUCH_PAD13_SLP_SEL_S  16
/* RTC_IO_TOUCH_PAD13_SLP_IE : R/W ;bitpos:[15] ;default: 1'd0 ; */
/*description: input enable in sleep mode.*/
#define RTC_IO_TOUCH_PAD13_SLP_IE    (BIT(15))
#define RTC_IO_TOUCH_PAD13_SLP_IE_M  (BIT(15))
#define RTC_IO_TOUCH_PAD13_SLP_IE_V  0x1
#define RTC_IO_TOUCH_PAD13_SLP_IE_S  15
/* RTC_IO_TOUCH_PAD13_SLP_OE : R/W ;bitpos:[14] ;default: 1'd0 ; */
/*description: output enable in sleep mode.*/
#define RTC_IO_TOUCH_PAD13_SLP_OE    (BIT(14))
#define RTC_IO_TOUCH_PAD13_SLP_OE_M  (BIT(14))
#define RTC_IO_TOUCH_PAD13_SLP_OE_V  0x1
#define RTC_IO_TOUCH_PAD13_SLP_OE_S  14
/* RTC_IO_TOUCH_PAD13_FUN_IE : R/W ;bitpos:[13] ;default: 1'd0 ; */
/*description: input enable in work mode.*/
#define RTC_IO_TOUCH_PAD13_FUN_IE    (BIT(13))
#define RTC_IO_TOUCH_PAD13_FUN_IE_M  (BIT(13))
#define RTC_IO_TOUCH_PAD13_FUN_IE_V  0x1
#define RTC_IO_TOUCH_PAD13_FUN_IE_S  13

#define RTC_IO_TOUCH_PAD14_REG          (DR_REG_RTCIO_BASE + 0xBC)
/* RTC_IO_TOUCH_PAD14_DRV : R/W ;bitpos:[30:29] ;default: 2'd2 ; */
/*description: DRV.*/
#define RTC_IO_TOUCH_PAD14_DRV    0x00000003
#define RTC_IO_TOUCH_PAD14_DRV_M  ((RTC_IO_TOUCH_PAD14_DRV_V)<<(RTC_IO_TOUCH_PAD14_DRV_S))
#define RTC_IO_TOUCH_PAD14_DRV_V  0x3
#define RTC_IO_TOUCH_PAD14_DRV_S  29
/* RTC_IO_TOUCH_PAD14_RDE : R/W ;bitpos:[28] ;default: 1'd0 ; */
/*description: RDE.*/
#define RTC_IO_TOUCH_PAD14_RDE    (BIT(28))
#define RTC_IO_TOUCH_PAD14_RDE_M  (BIT(28))
#define RTC_IO_TOUCH_PAD14_RDE_V  0x1
#define RTC_IO_TOUCH_PAD14_RDE_S  28
/* RTC_IO_TOUCH_PAD14_RUE : R/W ;bitpos:[27] ;default: 1'd0 ; */
/*description: RUE.*/
#define RTC_IO_TOUCH_PAD14_RUE    (BIT(27))
#define RTC_IO_TOUCH_PAD14_RUE_M  (BIT(27))
#define RTC_IO_TOUCH_PAD14_RUE_V  0x1
#define RTC_IO_TOUCH_PAD14_RUE_S  27
/* RTC_IO_TOUCH_PAD14_START : R/W ;bitpos:[22] ;default: 1'd0 ; */
/*description: TOUCH_START.*/
#define RTC_IO_TOUCH_PAD14_START    (BIT(22))
#define RTC_IO_TOUCH_PAD14_START_M  (BIT(22))
#define RTC_IO_TOUCH_PAD14_START_V  0x1
#define RTC_IO_TOUCH_PAD14_START_S  22
/* RTC_IO_TOUCH_PAD14_TIE_OPT : R/W ;bitpos:[21] ;default: 1'd0 ; */
/*description: TOUCH_TIE_OPT.*/
#define RTC_IO_TOUCH_PAD14_TIE_OPT    (BIT(21))
#define RTC_IO_TOUCH_PAD14_TIE_OPT_M  (BIT(21))
#define RTC_IO_TOUCH_PAD14_TIE_OPT_V  0x1
#define RTC_IO_TOUCH_PAD14_TIE_OPT_S  21
/* RTC_IO_TOUCH_PAD14_XPD : R/W ;bitpos:[20] ;default: 1'd0 ; */
/*description: TOUCH_XPD.*/
#define RTC_IO_TOUCH_PAD14_XPD    (BIT(20))
#define RTC_IO_TOUCH_PAD14_XPD_M  (BIT(20))
#define RTC_IO_TOUCH_PAD14_XPD_V  0x1
#define RTC_IO_TOUCH_PAD14_XPD_S  20
/* RTC_IO_TOUCH_PAD14_MUX_SEL : R/W ;bitpos:[19] ;default: 1'd0 ; */
/*description: 1: use RTC GPIO,0: use digital GPIO.*/
#define RTC_IO_TOUCH_PAD14_MUX_SEL    (BIT(19))
#define RTC_IO_TOUCH_PAD14_MUX_SEL_M  (BIT(19))
#define RTC_IO_TOUCH_PAD14_MUX_SEL_V  0x1
#define RTC_IO_TOUCH_PAD14_MUX_SEL_S  19
/* RTC_IO_TOUCH_PAD14_FUN_SEL : R/W ;bitpos:[18:17] ;default: 2'd0 ; */
/*description: function sel.*/
#define RTC_IO_TOUCH_PAD14_FUN_SEL    0x00000003
#define RTC_IO_TOUCH_PAD14_FUN_SEL_M  ((RTC_IO_TOUCH_PAD14_FUN_SEL_V)<<(RTC_IO_TOUCH_PAD14_FUN_SEL_S))
#define RTC_IO_TOUCH_PAD14_FUN_SEL_V  0x3
#define RTC_IO_TOUCH_PAD14_FUN_SEL_S  17
/* RTC_IO_TOUCH_PAD14_SLP_SEL : R/W ;bitpos:[16] ;default: 1'd0 ; */
/*description: 1: enable sleep mode during sleep,0: no sleep mode.*/
#define RTC_IO_TOUCH_PAD14_SLP_SEL    (BIT(16))
#define RTC_IO_TOUCH_PAD14_SLP_SEL_M  (BIT(16))
#define RTC_IO_TOUCH_PAD14_SLP_SEL_V  0x1
#define RTC_IO_TOUCH_PAD14_SLP_SEL_S  16
/* RTC_IO_TOUCH_PAD14_SLP_IE : R/W ;bitpos:[15] ;default: 1'd0 ; */
/*description: input enable in sleep mode.*/
#define RTC_IO_TOUCH_PAD14_SLP_IE    (BIT(15))
#define RTC_IO_TOUCH_PAD14_SLP_IE_M  (BIT(15))
#define RTC_IO_TOUCH_PAD14_SLP_IE_V  0x1
#define RTC_IO_TOUCH_PAD14_SLP_IE_S  15
/* RTC_IO_TOUCH_PAD14_SLP_OE : R/W ;bitpos:[14] ;default: 1'd0 ; */
/*description: output enable in sleep mode.*/
#define RTC_IO_TOUCH_PAD14_SLP_OE    (BIT(14))
#define RTC_IO_TOUCH_PAD14_SLP_OE_M  (BIT(14))
#define RTC_IO_TOUCH_PAD14_SLP_OE_V  0x1
#define RTC_IO_TOUCH_PAD14_SLP_OE_S  14
/* RTC_IO_TOUCH_PAD14_FUN_IE : R/W ;bitpos:[13] ;default: 1'd0 ; */
/*description: input enable in work mode.*/
#define RTC_IO_TOUCH_PAD14_FUN_IE    (BIT(13))
#define RTC_IO_TOUCH_PAD14_FUN_IE_M  (BIT(13))
#define RTC_IO_TOUCH_PAD14_FUN_IE_V  0x1
#define RTC_IO_TOUCH_PAD14_FUN_IE_S  13

#define RTC_IO_XTAL_32P_PAD_REG          (DR_REG_RTCIO_BASE + 0xC0)
/* RTC_IO_X32P_DRV : R/W ;bitpos:[30:29] ;default: 2'd2 ; */
/*description: DRV.*/
#define RTC_IO_X32P_DRV    0x00000003
#define RTC_IO_X32P_DRV_M  ((RTC_IO_X32P_DRV_V)<<(RTC_IO_X32P_DRV_S))
#define RTC_IO_X32P_DRV_V  0x3
#define RTC_IO_X32P_DRV_S  29
/* RTC_IO_X32P_RDE : R/W ;bitpos:[28] ;default: 1'd0 ; */
/*description: RDE.*/
#define RTC_IO_X32P_RDE    (BIT(28))
#define RTC_IO_X32P_RDE_M  (BIT(28))
#define RTC_IO_X32P_RDE_V  0x1
#define RTC_IO_X32P_RDE_S  28
/* RTC_IO_X32P_RUE : R/W ;bitpos:[27] ;default: 1'd0 ; */
/*description: RUE.*/
#define RTC_IO_X32P_RUE    (BIT(27))
#define RTC_IO_X32P_RUE_M  (BIT(27))
#define RTC_IO_X32P_RUE_V  0x1
#define RTC_IO_X32P_RUE_S  27
/* RTC_IO_X32P_MUX_SEL : R/W ;bitpos:[19] ;default: 1'd0 ; */
/*description: 1: use RTC GPIO,0: use digital GPIO.*/
#define RTC_IO_X32P_MUX_SEL    (BIT(19))
#define RTC_IO_X32P_MUX_SEL_M  (BIT(19))
#define RTC_IO_X32P_MUX_SEL_V  0x1
#define RTC_IO_X32P_MUX_SEL_S  19
/* RTC_IO_X32P_FUN_SEL : R/W ;bitpos:[18:17] ;default: 2'd0 ; */
/*description: function sel.*/
#define RTC_IO_X32P_FUN_SEL    0x00000003
#define RTC_IO_X32P_FUN_SEL_M  ((RTC_IO_X32P_FUN_SEL_V)<<(RTC_IO_X32P_FUN_SEL_S))
#define RTC_IO_X32P_FUN_SEL_V  0x3
#define RTC_IO_X32P_FUN_SEL_S  17
/* RTC_IO_X32P_SLP_SEL : R/W ;bitpos:[16] ;default: 1'd0 ; */
/*description: 1: enable sleep mode during sleep,0: no sleep mode.*/
#define RTC_IO_X32P_SLP_SEL    (BIT(16))
#define RTC_IO_X32P_SLP_SEL_M  (BIT(16))
#define RTC_IO_X32P_SLP_SEL_V  0x1
#define RTC_IO_X32P_SLP_SEL_S  16
/* RTC_IO_X32P_SLP_IE : R/W ;bitpos:[15] ;default: 1'd0 ; */
/*description: input enable in sleep mode.*/
#define RTC_IO_X32P_SLP_IE    (BIT(15))
#define RTC_IO_X32P_SLP_IE_M  (BIT(15))
#define RTC_IO_X32P_SLP_IE_V  0x1
#define RTC_IO_X32P_SLP_IE_S  15
/* RTC_IO_X32P_SLP_OE : R/W ;bitpos:[14] ;default: 1'd0 ; */
/*description: output enable in sleep mode.*/
#define RTC_IO_X32P_SLP_OE    (BIT(14))
#define RTC_IO_X32P_SLP_OE_M  (BIT(14))
#define RTC_IO_X32P_SLP_OE_V  0x1
#define RTC_IO_X32P_SLP_OE_S  14
/* RTC_IO_X32P_FUN_IE : R/W ;bitpos:[13] ;default: 1'd0 ; */
/*description: input enable in work mode.*/
#define RTC_IO_X32P_FUN_IE    (BIT(13))
#define RTC_IO_X32P_FUN_IE_M  (BIT(13))
#define RTC_IO_X32P_FUN_IE_V  0x1
#define RTC_IO_X32P_FUN_IE_S  13

#define RTC_IO_XTAL_32N_PAD_REG          (DR_REG_RTCIO_BASE + 0xC4)
/* RTC_IO_X32N_DRV : R/W ;bitpos:[30:29] ;default: 2'd2 ; */
/*description: DRV.*/
#define RTC_IO_X32N_DRV    0x00000003
#define RTC_IO_X32N_DRV_M  ((RTC_IO_X32N_DRV_V)<<(RTC_IO_X32N_DRV_S))
#define RTC_IO_X32N_DRV_V  0x3
#define RTC_IO_X32N_DRV_S  29
/* RTC_IO_X32N_RDE : R/W ;bitpos:[28] ;default: 1'd0 ; */
/*description: RDE.*/
#define RTC_IO_X32N_RDE    (BIT(28))
#define RTC_IO_X32N_RDE_M  (BIT(28))
#define RTC_IO_X32N_RDE_V  0x1
#define RTC_IO_X32N_RDE_S  28
/* RTC_IO_X32N_RUE : R/W ;bitpos:[27] ;default: 1'd0 ; */
/*description: RUE.*/
#define RTC_IO_X32N_RUE    (BIT(27))
#define RTC_IO_X32N_RUE_M  (BIT(27))
#define RTC_IO_X32N_RUE_V  0x1
#define RTC_IO_X32N_RUE_S  27
/* RTC_IO_X32N_MUX_SEL : R/W ;bitpos:[19] ;default: 1'd0 ; */
/*description: 1: use RTC GPIO,0: use digital GPIO.*/
#define RTC_IO_X32N_MUX_SEL    (BIT(19))
#define RTC_IO_X32N_MUX_SEL_M  (BIT(19))
#define RTC_IO_X32N_MUX_SEL_V  0x1
#define RTC_IO_X32N_MUX_SEL_S  19
/* RTC_IO_X32N_FUN_SEL : R/W ;bitpos:[18:17] ;default: 2'd0 ; */
/*description: function sel.*/
#define RTC_IO_X32N_FUN_SEL    0x00000003
#define RTC_IO_X32N_FUN_SEL_M  ((RTC_IO_X32N_FUN_SEL_V)<<(RTC_IO_X32N_FUN_SEL_S))
#define RTC_IO_X32N_FUN_SEL_V  0x3
#define RTC_IO_X32N_FUN_SEL_S  17
/* RTC_IO_X32N_SLP_SEL : R/W ;bitpos:[16] ;default: 1'd0 ; */
/*description: 1: enable sleep mode during sleep,0: no sleep mode.*/
#define RTC_IO_X32N_SLP_SEL    (BIT(16))
#define RTC_IO_X32N_SLP_SEL_M  (BIT(16))
#define RTC_IO_X32N_SLP_SEL_V  0x1
#define RTC_IO_X32N_SLP_SEL_S  16
/* RTC_IO_X32N_SLP_IE : R/W ;bitpos:[15] ;default: 1'd0 ; */
/*description: input enable in sleep mode.*/
#define RTC_IO_X32N_SLP_IE    (BIT(15))
#define RTC_IO_X32N_SLP_IE_M  (BIT(15))
#define RTC_IO_X32N_SLP_IE_V  0x1
#define RTC_IO_X32N_SLP_IE_S  15
/* RTC_IO_X32N_SLP_OE : R/W ;bitpos:[14] ;default: 1'd0 ; */
/*description: output enable in sleep mode.*/
#define RTC_IO_X32N_SLP_OE    (BIT(14))
#define RTC_IO_X32N_SLP_OE_M  (BIT(14))
#define RTC_IO_X32N_SLP_OE_V  0x1
#define RTC_IO_X32N_SLP_OE_S  14
/* RTC_IO_X32N_FUN_IE : R/W ;bitpos:[13] ;default: 1'd0 ; */
/*description: input enable in work mode.*/
#define RTC_IO_X32N_FUN_IE    (BIT(13))
#define RTC_IO_X32N_FUN_IE_M  (BIT(13))
#define RTC_IO_X32N_FUN_IE_V  0x1
#define RTC_IO_X32N_FUN_IE_S  13

#define RTC_IO_PAD_DAC1_REG          (DR_REG_RTCIO_BASE + 0xC8)
/* RTC_IO_PDAC1_DRV : R/W ;bitpos:[30:29] ;default: 2'd2 ; */
/*description: PDAC1_DRV.*/
#define RTC_IO_PDAC1_DRV    0x00000003
#define RTC_IO_PDAC1_DRV_M  ((RTC_IO_PDAC1_DRV_V)<<(RTC_IO_PDAC1_DRV_S))
#define RTC_IO_PDAC1_DRV_V  0x3
#define RTC_IO_PDAC1_DRV_S  29
/* RTC_IO_PDAC1_RDE : R/W ;bitpos:[28] ;default: 1'd0 ; */
/*description: PDAC1_RDE.*/
#define RTC_IO_PDAC1_RDE    (BIT(28))
#define RTC_IO_PDAC1_RDE_M  (BIT(28))
#define RTC_IO_PDAC1_RDE_V  0x1
#define RTC_IO_PDAC1_RDE_S  28
/* RTC_IO_PDAC1_RUE : R/W ;bitpos:[27] ;default: 1'd0 ; */
/*description: PDAC1_RUE.*/
#define RTC_IO_PDAC1_RUE    (BIT(27))
#define RTC_IO_PDAC1_RUE_M  (BIT(27))
#define RTC_IO_PDAC1_RUE_V  0x1
#define RTC_IO_PDAC1_RUE_S  27
/* RTC_IO_PDAC1_MUX_SEL : R/W ;bitpos:[19] ;default: 1'd0 ; */
/*description: 1: use RTC GPIO,0: use digital GPIO.*/
#define RTC_IO_PDAC1_MUX_SEL    (BIT(19))
#define RTC_IO_PDAC1_MUX_SEL_M  (BIT(19))
#define RTC_IO_PDAC1_MUX_SEL_V  0x1
#define RTC_IO_PDAC1_MUX_SEL_S  19
/* RTC_IO_PDAC1_FUN_SEL : R/W ;bitpos:[18:17] ;default: 2'd0 ; */
/*description: PDAC1 function sel.*/
#define RTC_IO_PDAC1_FUN_SEL    0x00000003
#define RTC_IO_PDAC1_FUN_SEL_M  ((RTC_IO_PDAC1_FUN_SEL_V)<<(RTC_IO_PDAC1_FUN_SEL_S))
#define RTC_IO_PDAC1_FUN_SEL_V  0x3
#define RTC_IO_PDAC1_FUN_SEL_S  17
/* RTC_IO_PDAC1_SLP_SEL : R/W ;bitpos:[16] ;default: 1'd0 ; */
/*description: 1: enable sleep mode during sleep,0: no sleep mode.*/
#define RTC_IO_PDAC1_SLP_SEL    (BIT(16))
#define RTC_IO_PDAC1_SLP_SEL_M  (BIT(16))
#define RTC_IO_PDAC1_SLP_SEL_V  0x1
#define RTC_IO_PDAC1_SLP_SEL_S  16
/* RTC_IO_PDAC1_SLP_IE : R/W ;bitpos:[15] ;default: 1'd0 ; */
/*description: input enable in sleep mode.*/
#define RTC_IO_PDAC1_SLP_IE    (BIT(15))
#define RTC_IO_PDAC1_SLP_IE_M  (BIT(15))
#define RTC_IO_PDAC1_SLP_IE_V  0x1
#define RTC_IO_PDAC1_SLP_IE_S  15
/* RTC_IO_PDAC1_SLP_OE : R/W ;bitpos:[14] ;default: 1'd0 ; */
/*description: output enable in sleep mode.*/
#define RTC_IO_PDAC1_SLP_OE    (BIT(14))
#define RTC_IO_PDAC1_SLP_OE_M  (BIT(14))
#define RTC_IO_PDAC1_SLP_OE_V  0x1
#define RTC_IO_PDAC1_SLP_OE_S  14
/* RTC_IO_PDAC1_FUN_IE : R/W ;bitpos:[13] ;default: 1'd0 ; */
/*description: input enable in work mode.*/
#define RTC_IO_PDAC1_FUN_IE    (BIT(13))
#define RTC_IO_PDAC1_FUN_IE_M  (BIT(13))
#define RTC_IO_PDAC1_FUN_IE_V  0x1
#define RTC_IO_PDAC1_FUN_IE_S  13
/* RTC_IO_PDAC1_DAC_XPD_FORCE : R/W ;bitpos:[12] ;default: 1'd0 ; */
/*description: 1: use reg_pdac1_xpd_dac to control PDAC1_XPD_DAC,0: use SAR ADC FSM to control
PDAC1_XPD_DAC.*/
#define RTC_IO_PDAC1_DAC_XPD_FORCE    (BIT(12))
#define RTC_IO_PDAC1_DAC_XPD_FORCE_M  (BIT(12))
#define RTC_IO_PDAC1_DAC_XPD_FORCE_V  0x1
#define RTC_IO_PDAC1_DAC_XPD_FORCE_S  12
/* RTC_IO_PDAC1_XPD_DAC : R/W ;bitpos:[11] ;default: 1'd0 ; */
/*description: PDAC1_XPD_DAC.*/
#define RTC_IO_PDAC1_XPD_DAC    (BIT(11))
#define RTC_IO_PDAC1_XPD_DAC_M  (BIT(11))
#define RTC_IO_PDAC1_XPD_DAC_V  0x1
#define RTC_IO_PDAC1_XPD_DAC_S  11
/* RTC_IO_PDAC1_DAC : R/W ;bitpos:[10:3] ;default: 8'd0 ; */
/*description: PDAC1_DAC.*/
#define RTC_IO_PDAC1_DAC    0x000000FF
#define RTC_IO_PDAC1_DAC_M  ((RTC_IO_PDAC1_DAC_V)<<(RTC_IO_PDAC1_DAC_S))
#define RTC_IO_PDAC1_DAC_V  0xFF
#define RTC_IO_PDAC1_DAC_S  3

#define RTC_IO_PAD_DAC2_REG          (DR_REG_RTCIO_BASE + 0xCC)
/* RTC_IO_PDAC2_DRV : R/W ;bitpos:[30:29] ;default: 2'd2 ; */
/*description: PDAC2_DRV.*/
#define RTC_IO_PDAC2_DRV    0x00000003
#define RTC_IO_PDAC2_DRV_M  ((RTC_IO_PDAC2_DRV_V)<<(RTC_IO_PDAC2_DRV_S))
#define RTC_IO_PDAC2_DRV_V  0x3
#define RTC_IO_PDAC2_DRV_S  29
/* RTC_IO_PDAC2_RDE : R/W ;bitpos:[28] ;default: 1'd0 ; */
/*description: PDAC2_RDE.*/
#define RTC_IO_PDAC2_RDE    (BIT(28))
#define RTC_IO_PDAC2_RDE_M  (BIT(28))
#define RTC_IO_PDAC2_RDE_V  0x1
#define RTC_IO_PDAC2_RDE_S  28
/* RTC_IO_PDAC2_RUE : R/W ;bitpos:[27] ;default: 1'd0 ; */
/*description: PDAC2_RUE.*/
#define RTC_IO_PDAC2_RUE    (BIT(27))
#define RTC_IO_PDAC2_RUE_M  (BIT(27))
#define RTC_IO_PDAC2_RUE_V  0x1
#define RTC_IO_PDAC2_RUE_S  27
/* RTC_IO_PDAC2_MUX_SEL : R/W ;bitpos:[19] ;default: 1'd0 ; */
/*description: 1: use RTC GPIO,0: use digital GPIO.*/
#define RTC_IO_PDAC2_MUX_SEL    (BIT(19))
#define RTC_IO_PDAC2_MUX_SEL_M  (BIT(19))
#define RTC_IO_PDAC2_MUX_SEL_V  0x1
#define RTC_IO_PDAC2_MUX_SEL_S  19
/* RTC_IO_PDAC2_FUN_SEL : R/W ;bitpos:[18:17] ;default: 2'd0 ; */
/*description: PDAC1 function sel.*/
#define RTC_IO_PDAC2_FUN_SEL    0x00000003
#define RTC_IO_PDAC2_FUN_SEL_M  ((RTC_IO_PDAC2_FUN_SEL_V)<<(RTC_IO_PDAC2_FUN_SEL_S))
#define RTC_IO_PDAC2_FUN_SEL_V  0x3
#define RTC_IO_PDAC2_FUN_SEL_S  17
/* RTC_IO_PDAC2_SLP_SEL : R/W ;bitpos:[16] ;default: 1'd0 ; */
/*description: 1: enable sleep mode during sleep,0: no sleep mode.*/
#define RTC_IO_PDAC2_SLP_SEL    (BIT(16))
#define RTC_IO_PDAC2_SLP_SEL_M  (BIT(16))
#define RTC_IO_PDAC2_SLP_SEL_V  0x1
#define RTC_IO_PDAC2_SLP_SEL_S  16
/* RTC_IO_PDAC2_SLP_IE : R/W ;bitpos:[15] ;default: 1'd0 ; */
/*description: input enable in sleep mode.*/
#define RTC_IO_PDAC2_SLP_IE    (BIT(15))
#define RTC_IO_PDAC2_SLP_IE_M  (BIT(15))
#define RTC_IO_PDAC2_SLP_IE_V  0x1
#define RTC_IO_PDAC2_SLP_IE_S  15
/* RTC_IO_PDAC2_SLP_OE : R/W ;bitpos:[14] ;default: 1'd0 ; */
/*description: output enable in sleep mode.*/
#define RTC_IO_PDAC2_SLP_OE    (BIT(14))
#define RTC_IO_PDAC2_SLP_OE_M  (BIT(14))
#define RTC_IO_PDAC2_SLP_OE_V  0x1
#define RTC_IO_PDAC2_SLP_OE_S  14
/* RTC_IO_PDAC2_FUN_IE : R/W ;bitpos:[13] ;default: 1'd0 ; */
/*description: input enable in work mode.*/
#define RTC_IO_PDAC2_FUN_IE    (BIT(13))
#define RTC_IO_PDAC2_FUN_IE_M  (BIT(13))
#define RTC_IO_PDAC2_FUN_IE_V  0x1
#define RTC_IO_PDAC2_FUN_IE_S  13
/* RTC_IO_PDAC2_DAC_XPD_FORCE : R/W ;bitpos:[12] ;default: 1'd0 ; */
/*description: 1: use reg_pdac2_xpd_dac to control PDAC2_XPD_DAC,0: use SAR ADC FSM to control
PDAC2_XPD_DAC.*/
#define RTC_IO_PDAC2_DAC_XPD_FORCE    (BIT(12))
#define RTC_IO_PDAC2_DAC_XPD_FORCE_M  (BIT(12))
#define RTC_IO_PDAC2_DAC_XPD_FORCE_V  0x1
#define RTC_IO_PDAC2_DAC_XPD_FORCE_S  12
/* RTC_IO_PDAC2_XPD_DAC : R/W ;bitpos:[11] ;default: 1'd0 ; */
/*description: PDAC2_XPD_DAC.*/
#define RTC_IO_PDAC2_XPD_DAC    (BIT(11))
#define RTC_IO_PDAC2_XPD_DAC_M  (BIT(11))
#define RTC_IO_PDAC2_XPD_DAC_V  0x1
#define RTC_IO_PDAC2_XPD_DAC_S  11
/* RTC_IO_PDAC2_DAC : R/W ;bitpos:[10:3] ;default: 8'd0 ; */
/*description: PDAC2_DAC.*/
#define RTC_IO_PDAC2_DAC    0x000000FF
#define RTC_IO_PDAC2_DAC_M  ((RTC_IO_PDAC2_DAC_V)<<(RTC_IO_PDAC2_DAC_S))
#define RTC_IO_PDAC2_DAC_V  0xFF
#define RTC_IO_PDAC2_DAC_S  3

#define RTC_IO_RTC_PAD19_REG          (DR_REG_RTCIO_BASE + 0xD0)
/* RTC_IO_PAD19_DRV : R/W ;bitpos:[30:29] ;default: 2'd2 ; */
/*description: DRV.*/
#define RTC_IO_PAD19_DRV    0x00000003
#define RTC_IO_PAD19_DRV_M  ((RTC_IO_PAD19_DRV_V)<<(RTC_IO_PAD19_DRV_S))
#define RTC_IO_PAD19_DRV_V  0x3
#define RTC_IO_PAD19_DRV_S  29
/* RTC_IO_PAD19_RDE : R/W ;bitpos:[28] ;default: 1'd1 ; */
/*description: RDE.*/
#define RTC_IO_PAD19_RDE    (BIT(28))
#define RTC_IO_PAD19_RDE_M  (BIT(28))
#define RTC_IO_PAD19_RDE_V  0x1
#define RTC_IO_PAD19_RDE_S  28
/* RTC_IO_PAD19_RUE : R/W ;bitpos:[27] ;default: 1'd0 ; */
/*description: RUE.*/
#define RTC_IO_PAD19_RUE    (BIT(27))
#define RTC_IO_PAD19_RUE_M  (BIT(27))
#define RTC_IO_PAD19_RUE_V  0x1
#define RTC_IO_PAD19_RUE_S  27
/* RTC_IO_PAD19_MUX_SEL : R/W ;bitpos:[19] ;default: 1'd0 ; */
/*description: 1: use RTC GPIO,0: use digital GPIO.*/
#define RTC_IO_PAD19_MUX_SEL    (BIT(19))
#define RTC_IO_PAD19_MUX_SEL_M  (BIT(19))
#define RTC_IO_PAD19_MUX_SEL_V  0x1
#define RTC_IO_PAD19_MUX_SEL_S  19
/* RTC_IO_PAD19_FUN_SEL : R/W ;bitpos:[18:17] ;default: 2'd0 ; */
/*description: function sel.*/
#define RTC_IO_PAD19_FUN_SEL    0x00000003
#define RTC_IO_PAD19_FUN_SEL_M  ((RTC_IO_PAD19_FUN_SEL_V)<<(RTC_IO_PAD19_FUN_SEL_S))
#define RTC_IO_PAD19_FUN_SEL_V  0x3
#define RTC_IO_PAD19_FUN_SEL_S  17
/* RTC_IO_PAD19_SLP_SEL : R/W ;bitpos:[16] ;default: 1'd0 ; */
/*description: 1: enable sleep mode during sleep,0: no sleep mode.*/
#define RTC_IO_PAD19_SLP_SEL    (BIT(16))
#define RTC_IO_PAD19_SLP_SEL_M  (BIT(16))
#define RTC_IO_PAD19_SLP_SEL_V  0x1
#define RTC_IO_PAD19_SLP_SEL_S  16
/* RTC_IO_PAD19_SLP_IE : R/W ;bitpos:[15] ;default: 1'd0 ; */
/*description: input enable in sleep mode.*/
#define RTC_IO_PAD19_SLP_IE    (BIT(15))
#define RTC_IO_PAD19_SLP_IE_M  (BIT(15))
#define RTC_IO_PAD19_SLP_IE_V  0x1
#define RTC_IO_PAD19_SLP_IE_S  15
/* RTC_IO_PAD19_SLP_OE : R/W ;bitpos:[14] ;default: 1'd0 ; */
/*description: output enable in sleep mode.*/
#define RTC_IO_PAD19_SLP_OE    (BIT(14))
#define RTC_IO_PAD19_SLP_OE_M  (BIT(14))
#define RTC_IO_PAD19_SLP_OE_V  0x1
#define RTC_IO_PAD19_SLP_OE_S  14
/* RTC_IO_PAD19_FUN_IE : R/W ;bitpos:[13] ;default: 1'd0 ; */
/*description: input enable in work mode.*/
#define RTC_IO_PAD19_FUN_IE    (BIT(13))
#define RTC_IO_PAD19_FUN_IE_M  (BIT(13))
#define RTC_IO_PAD19_FUN_IE_V  0x1
#define RTC_IO_PAD19_FUN_IE_S  13

#define RTC_IO_RTC_PAD20_REG          (DR_REG_RTCIO_BASE + 0xD4)
/* RTC_IO_PAD20_DRV : R/W ;bitpos:[30:29] ;default: 2'd2 ; */
/*description: DRV.*/
#define RTC_IO_PAD20_DRV    0x00000003
#define RTC_IO_PAD20_DRV_M  ((RTC_IO_PAD20_DRV_V)<<(RTC_IO_PAD20_DRV_S))
#define RTC_IO_PAD20_DRV_V  0x3
#define RTC_IO_PAD20_DRV_S  29
/* RTC_IO_PAD20_RDE : R/W ;bitpos:[28] ;default: 1'd1 ; */
/*description: RDE.*/
#define RTC_IO_PAD20_RDE    (BIT(28))
#define RTC_IO_PAD20_RDE_M  (BIT(28))
#define RTC_IO_PAD20_RDE_V  0x1
#define RTC_IO_PAD20_RDE_S  28
/* RTC_IO_PAD20_RUE : R/W ;bitpos:[27] ;default: 1'd0 ; */
/*description: RUE.*/
#define RTC_IO_PAD20_RUE    (BIT(27))
#define RTC_IO_PAD20_RUE_M  (BIT(27))
#define RTC_IO_PAD20_RUE_V  0x1
#define RTC_IO_PAD20_RUE_S  27
/* RTC_IO_PAD20_MUX_SEL : R/W ;bitpos:[19] ;default: 1'd0 ; */
/*description: 1: use RTC GPIO,0: use digital GPIO.*/
#define RTC_IO_PAD20_MUX_SEL    (BIT(19))
#define RTC_IO_PAD20_MUX_SEL_M  (BIT(19))
#define RTC_IO_PAD20_MUX_SEL_V  0x1
#define RTC_IO_PAD20_MUX_SEL_S  19
/* RTC_IO_PAD20_FUN_SEL : R/W ;bitpos:[18:17] ;default: 2'd0 ; */
/*description: function sel.*/
#define RTC_IO_PAD20_FUN_SEL    0x00000003
#define RTC_IO_PAD20_FUN_SEL_M  ((RTC_IO_PAD20_FUN_SEL_V)<<(RTC_IO_PAD20_FUN_SEL_S))
#define RTC_IO_PAD20_FUN_SEL_V  0x3
#define RTC_IO_PAD20_FUN_SEL_S  17
/* RTC_IO_PAD20_SLP_SEL : R/W ;bitpos:[16] ;default: 1'd0 ; */
/*description: 1: enable sleep mode during sleep,0: no sleep mode.*/
#define RTC_IO_PAD20_SLP_SEL    (BIT(16))
#define RTC_IO_PAD20_SLP_SEL_M  (BIT(16))
#define RTC_IO_PAD20_SLP_SEL_V  0x1
#define RTC_IO_PAD20_SLP_SEL_S  16
/* RTC_IO_PAD20_SLP_IE : R/W ;bitpos:[15] ;default: 1'd0 ; */
/*description: input enable in sleep mode.*/
#define RTC_IO_PAD20_SLP_IE    (BIT(15))
#define RTC_IO_PAD20_SLP_IE_M  (BIT(15))
#define RTC_IO_PAD20_SLP_IE_V  0x1
#define RTC_IO_PAD20_SLP_IE_S  15
/* RTC_IO_PAD20_SLP_OE : R/W ;bitpos:[14] ;default: 1'd0 ; */
/*description: output enable in sleep mode.*/
#define RTC_IO_PAD20_SLP_OE    (BIT(14))
#define RTC_IO_PAD20_SLP_OE_M  (BIT(14))
#define RTC_IO_PAD20_SLP_OE_V  0x1
#define RTC_IO_PAD20_SLP_OE_S  14
/* RTC_IO_PAD20_FUN_IE : R/W ;bitpos:[13] ;default: 1'd0 ; */
/*description: input enable in work mode.*/
#define RTC_IO_PAD20_FUN_IE    (BIT(13))
#define RTC_IO_PAD20_FUN_IE_M  (BIT(13))
#define RTC_IO_PAD20_FUN_IE_V  0x1
#define RTC_IO_PAD20_FUN_IE_S  13

#define RTC_IO_RTC_PAD21_REG          (DR_REG_RTCIO_BASE + 0xD8)
/* RTC_IO_PAD21_DRV : R/W ;bitpos:[30:29] ;default: 2'd2 ; */
/*description: DRV.*/
#define RTC_IO_PAD21_DRV    0x00000003
#define RTC_IO_PAD21_DRV_M  ((RTC_IO_PAD21_DRV_V)<<(RTC_IO_PAD21_DRV_S))
#define RTC_IO_PAD21_DRV_V  0x3
#define RTC_IO_PAD21_DRV_S  29
/* RTC_IO_PAD21_RDE : R/W ;bitpos:[28] ;default: 1'd1 ; */
/*description: RDE.*/
#define RTC_IO_PAD21_RDE    (BIT(28))
#define RTC_IO_PAD21_RDE_M  (BIT(28))
#define RTC_IO_PAD21_RDE_V  0x1
#define RTC_IO_PAD21_RDE_S  28
/* RTC_IO_PAD21_RUE : R/W ;bitpos:[27] ;default: 1'd0 ; */
/*description: RUE.*/
#define RTC_IO_PAD21_RUE    (BIT(27))
#define RTC_IO_PAD21_RUE_M  (BIT(27))
#define RTC_IO_PAD21_RUE_V  0x1
#define RTC_IO_PAD21_RUE_S  27
/* RTC_IO_PAD21_MUX_SEL : R/W ;bitpos:[19] ;default: 1'd0 ; */
/*description: 1: use RTC GPIO,0: use digital GPIO.*/
#define RTC_IO_PAD21_MUX_SEL    (BIT(19))
#define RTC_IO_PAD21_MUX_SEL_M  (BIT(19))
#define RTC_IO_PAD21_MUX_SEL_V  0x1
#define RTC_IO_PAD21_MUX_SEL_S  19
/* RTC_IO_PAD21_FUN_SEL : R/W ;bitpos:[18:17] ;default: 2'd0 ; */
/*description: function sel.*/
#define RTC_IO_PAD21_FUN_SEL    0x00000003
#define RTC_IO_PAD21_FUN_SEL_M  ((RTC_IO_PAD21_FUN_SEL_V)<<(RTC_IO_PAD21_FUN_SEL_S))
#define RTC_IO_PAD21_FUN_SEL_V  0x3
#define RTC_IO_PAD21_FUN_SEL_S  17
/* RTC_IO_PAD21_SLP_SEL : R/W ;bitpos:[16] ;default: 1'd0 ; */
/*description: 1: enable sleep mode during sleep,0: no sleep mode.*/
#define RTC_IO_PAD21_SLP_SEL    (BIT(16))
#define RTC_IO_PAD21_SLP_SEL_M  (BIT(16))
#define RTC_IO_PAD21_SLP_SEL_V  0x1
#define RTC_IO_PAD21_SLP_SEL_S  16
/* RTC_IO_PAD21_SLP_IE : R/W ;bitpos:[15] ;default: 1'd0 ; */
/*description: input enable in sleep mode.*/
#define RTC_IO_PAD21_SLP_IE    (BIT(15))
#define RTC_IO_PAD21_SLP_IE_M  (BIT(15))
#define RTC_IO_PAD21_SLP_IE_V  0x1
#define RTC_IO_PAD21_SLP_IE_S  15
/* RTC_IO_PAD21_SLP_OE : R/W ;bitpos:[14] ;default: 1'd0 ; */
/*description: output enable in sleep mode.*/
#define RTC_IO_PAD21_SLP_OE    (BIT(14))
#define RTC_IO_PAD21_SLP_OE_M  (BIT(14))
#define RTC_IO_PAD21_SLP_OE_V  0x1
#define RTC_IO_PAD21_SLP_OE_S  14
/* RTC_IO_PAD21_FUN_IE : R/W ;bitpos:[13] ;default: 1'd0 ; */
/*description: input enable in work mode.*/
#define RTC_IO_PAD21_FUN_IE    (BIT(13))
#define RTC_IO_PAD21_FUN_IE_M  (BIT(13))
#define RTC_IO_PAD21_FUN_IE_V  0x1
#define RTC_IO_PAD21_FUN_IE_S  13

#define RTC_IO_EXT_WAKEUP0_REG          (DR_REG_RTCIO_BASE + 0xDC)
/* RTC_IO_EXT_WAKEUP0_SEL : R/W ;bitpos:[31:27] ;default: 5'd0 ; */
/*description: .*/
#define RTC_IO_EXT_WAKEUP0_SEL    0x0000001F
#define RTC_IO_EXT_WAKEUP0_SEL_M  ((RTC_IO_EXT_WAKEUP0_SEL_V)<<(RTC_IO_EXT_WAKEUP0_SEL_S))
#define RTC_IO_EXT_WAKEUP0_SEL_V  0x1F
#define RTC_IO_EXT_WAKEUP0_SEL_S  27

#define RTC_IO_XTL_EXT_CTR_REG          (DR_REG_RTCIO_BASE + 0xE0)
/* RTC_IO_XTL_EXT_CTR_SEL : R/W ;bitpos:[31:27] ;default: 5'd0 ; */
/*description: select RTC GPIO 0 ~ 17 to control XTAL.*/
#define RTC_IO_XTL_EXT_CTR_SEL    0x0000001F
#define RTC_IO_XTL_EXT_CTR_SEL_M  ((RTC_IO_XTL_EXT_CTR_SEL_V)<<(RTC_IO_XTL_EXT_CTR_SEL_S))
#define RTC_IO_XTL_EXT_CTR_SEL_V  0x1F
#define RTC_IO_XTL_EXT_CTR_SEL_S  27

#define RTC_IO_SAR_I2C_IO_REG          (DR_REG_RTCIO_BASE + 0xE4)
/* RTC_IO_SAR_I2C_SDA_SEL : R/W ;bitpos:[31:30] ;default: 2'd0 ; */
/*description: .*/
#define RTC_IO_SAR_I2C_SDA_SEL    0x00000003
#define RTC_IO_SAR_I2C_SDA_SEL_M  ((RTC_IO_SAR_I2C_SDA_SEL_V)<<(RTC_IO_SAR_I2C_SDA_SEL_S))
#define RTC_IO_SAR_I2C_SDA_SEL_V  0x3
#define RTC_IO_SAR_I2C_SDA_SEL_S  30
/* RTC_IO_SAR_I2C_SCL_SEL : R/W ;bitpos:[29:28] ;default: 2'd0 ; */
/*description: .*/
#define RTC_IO_SAR_I2C_SCL_SEL    0x00000003
#define RTC_IO_SAR_I2C_SCL_SEL_M  ((RTC_IO_SAR_I2C_SCL_SEL_V)<<(RTC_IO_SAR_I2C_SCL_SEL_S))
#define RTC_IO_SAR_I2C_SCL_SEL_V  0x3
#define RTC_IO_SAR_I2C_SCL_SEL_S  28
/* RTC_IO_SAR_DEBUG_BIT_SEL : R/W ;bitpos:[27:23] ;default: 5'h0 ; */
/*description: .*/
#define RTC_IO_SAR_DEBUG_BIT_SEL    0x0000001F
#define RTC_IO_SAR_DEBUG_BIT_SEL_M  ((RTC_IO_SAR_DEBUG_BIT_SEL_V)<<(RTC_IO_SAR_DEBUG_BIT_SEL_S))
#define RTC_IO_SAR_DEBUG_BIT_SEL_V  0x1F
#define RTC_IO_SAR_DEBUG_BIT_SEL_S  23

#define RTC_IO_TOUCH_CTRL_REG          (DR_REG_RTCIO_BASE + 0xE8)
/* RTC_IO_IO_TOUCH_BUFMODE : R/W ;bitpos:[4] ;default: 1'd0 ; */
/*description: BUF_MODE when touch work without fsm.*/
#define RTC_IO_IO_TOUCH_BUFMODE    (BIT(4))
#define RTC_IO_IO_TOUCH_BUFMODE_M  (BIT(4))
#define RTC_IO_IO_TOUCH_BUFMODE_V  0x1
#define RTC_IO_IO_TOUCH_BUFMODE_S  4
/* RTC_IO_IO_TOUCH_BUFSEL : R/W ;bitpos:[3:0] ;default: 4'd0 ; */
/*description: BUF_SEL when touch work without fsm.*/
#define RTC_IO_IO_TOUCH_BUFSEL    0x0000000F
#define RTC_IO_IO_TOUCH_BUFSEL_M  ((RTC_IO_IO_TOUCH_BUFSEL_V)<<(RTC_IO_IO_TOUCH_BUFSEL_S))
#define RTC_IO_IO_TOUCH_BUFSEL_V  0xF
#define RTC_IO_IO_TOUCH_BUFSEL_S  0

#define RTC_IO_DATE_REG          (DR_REG_RTCIO_BASE + 0x1FC)
/* RTC_IO_DATE : R/W ;bitpos:[27:0] ;default: 28'h2101180 ; */
/*description: .*/
#define RTC_IO_DATE    0x0FFFFFFF
#define RTC_IO_DATE_M  ((RTC_IO_DATE_V)<<(RTC_IO_DATE_S))
#define RTC_IO_DATE_V  0xFFFFFFF
#define RTC_IO_DATE_S  0


#ifdef __cplusplus
}
#endif



#endif /*_SOC_RTC_IO_REG_H_ */
