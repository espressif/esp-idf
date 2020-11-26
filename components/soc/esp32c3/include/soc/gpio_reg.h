// Copyright 2020 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#ifndef _SOC_GPIO_REG_H_
#define _SOC_GPIO_REG_H_


#ifdef __cplusplus
extern "C" {
#endif
#include "soc.h"
#define GPIO_PIN_CONFIG_MSB                    12
#define GPIO_PIN_CONFIG_LSB                     11
#define GPIO_PIN_CONFIG_MASK                   0x00001800
#define GPIO_PIN_CONFIG_GET(x)                 (((x) & GPIO_PIN_CONFIG_MASK) >> GPIO_PIN_CONFIG_LSB)
#define GPIO_PIN_CONFIG_SET(x)                  (((x) << GPIO_PIN_CONFIG_LSB) & GPIO_PIN_CONFIG_MASK)

#define GPIO_WAKEUP_ENABLE                     1
#define GPIO_WAKEUP_DISABLE                    (~GPIO_WAKEUP_ENABLE)
#define GPIO_PIN_WAKEUP_ENABLE_MSB             10
#define GPIO_PIN_WAKEUP_ENABLE_LSB              10
#define GPIO_PIN_WAKEUP_ENABLE_MASK            0x00000400
#define GPIO_PIN_WAKEUP_ENABLE_GET(x)          (((x) & GPIO_PIN_WAKEUP_ENABLE_MASK) >> GPIO_PIN_WAKEUP_ENABLE_LSB)
#define GPIO_PIN_WAKEUP_ENABLE_SET(x)           (((x) << GPIO_PIN_WAKEUP_ENABLE_LSB) & GPIO_PIN_WAKEUP_ENABLE_MASK)

#define GPIO_PIN_INT_TYPE_MASK                 0x380
#define GPIO_PIN_INT_TYPE_MSB                  9
#define GPIO_PIN_INT_TYPE_LSB                  7
#define GPIO_PIN_INT_TYPE_GET(x)               (((x) & GPIO_PIN_INT_TYPE_MASK) >> GPIO_PIN_INT_TYPE_LSB)
#define GPIO_PIN_INT_TYPE_SET(x)               (((x) << GPIO_PIN_INT_TYPE_LSB) & GPIO_PIN_INT_TYPE_MASK)

#define GPIO_PAD_DRIVER_ENABLE                 1
#define GPIO_PAD_DRIVER_DISABLE                        (~GPIO_PAD_DRIVER_ENABLE)
#define GPIO_PIN_PAD_DRIVER_MSB                        2
#define GPIO_PIN_PAD_DRIVER_LSB                        2
#define GPIO_PIN_PAD_DRIVER_MASK               0x00000004
#define GPIO_PIN_PAD_DRIVER_GET(x)             (((x) & GPIO_PIN_PAD_DRIVER_MASK) >> GPIO_PIN_PAD_DRIVER_LSB)
#define GPIO_PIN_PAD_DRIVER_SET(x)             (((x) << GPIO_PIN_PAD_DRIVER_LSB) & GPIO_PIN_PAD_DRIVER_MASK)

#define GPIO_BT_SELECT_REG          (DR_REG_GPIO_BASE + 0x0)
/* GPIO_BT_SEL : R/W ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: */
#define GPIO_BT_SEL  0xFFFFFFFF
#define GPIO_BT_SEL_M  ((GPIO_BT_SEL_V)<<(GPIO_BT_SEL_S))
#define GPIO_BT_SEL_V  0xFFFFFFFF
#define GPIO_BT_SEL_S  0

#define GPIO_OUT_REG          (DR_REG_GPIO_BASE + 0x4)
/* GPIO_OUT_DATA : R/W ;bitpos:[25:0] ;default: 26'h0 ; */
/*description: */
#define GPIO_OUT_DATA  0x03FFFFFF
#define GPIO_OUT_DATA_M  ((GPIO_OUT_DATA_V)<<(GPIO_OUT_DATA_S))
#define GPIO_OUT_DATA_V  0x3FFFFFF
#define GPIO_OUT_DATA_S  0

#define GPIO_OUT_W1TS_REG          (DR_REG_GPIO_BASE + 0x8)
/* GPIO_OUT_W1TS : WO ;bitpos:[25:0] ;default: 26'h0 ; */
/*description: */
#define GPIO_OUT_W1TS  0x03FFFFFF
#define GPIO_OUT_W1TS_M  ((GPIO_OUT_W1TS_V)<<(GPIO_OUT_W1TS_S))
#define GPIO_OUT_W1TS_V  0x3FFFFFF
#define GPIO_OUT_W1TS_S  0

#define GPIO_OUT_W1TC_REG          (DR_REG_GPIO_BASE + 0xC)
/* GPIO_OUT_W1TC : WO ;bitpos:[25:0] ;default: 26'h0 ; */
/*description: */
#define GPIO_OUT_W1TC  0x03FFFFFF
#define GPIO_OUT_W1TC_M  ((GPIO_OUT_W1TC_V)<<(GPIO_OUT_W1TC_S))
#define GPIO_OUT_W1TC_V  0x3FFFFFF
#define GPIO_OUT_W1TC_S  0

#define GPIO_SDIO_SELECT_REG          (DR_REG_GPIO_BASE + 0x1C)
/* GPIO_SDIO_SEL : R/W ;bitpos:[7:0] ;default: 8'h0 ; */
/*description: */
#define GPIO_SDIO_SEL  0x000000FF
#define GPIO_SDIO_SEL_M  ((GPIO_SDIO_SEL_V)<<(GPIO_SDIO_SEL_S))
#define GPIO_SDIO_SEL_V  0xFF
#define GPIO_SDIO_SEL_S  0

#define GPIO_ENABLE_REG          (DR_REG_GPIO_BASE + 0x20)
/* GPIO_ENABLE_DATA : R/W ;bitpos:[25:0] ;default: 26'h0 ; */
/*description: */
#define GPIO_ENABLE_DATA  0x03FFFFFF
#define GPIO_ENABLE_DATA_M  ((GPIO_ENABLE_DATA_V)<<(GPIO_ENABLE_DATA_S))
#define GPIO_ENABLE_DATA_V  0x3FFFFFF
#define GPIO_ENABLE_DATA_S  0

#define GPIO_ENABLE_W1TS_REG          (DR_REG_GPIO_BASE + 0x24)
/* GPIO_ENABLE_W1TS : WO ;bitpos:[25:0] ;default: 26'h0 ; */
/*description: */
#define GPIO_ENABLE_W1TS  0x03FFFFFF
#define GPIO_ENABLE_W1TS_M  ((GPIO_ENABLE_W1TS_V)<<(GPIO_ENABLE_W1TS_S))
#define GPIO_ENABLE_W1TS_V  0x3FFFFFF
#define GPIO_ENABLE_W1TS_S  0

#define GPIO_ENABLE_W1TC_REG          (DR_REG_GPIO_BASE + 0x28)
/* GPIO_ENABLE_W1TC : WO ;bitpos:[25:0] ;default: 26'h0 ; */
/*description: */
#define GPIO_ENABLE_W1TC  0x03FFFFFF
#define GPIO_ENABLE_W1TC_M  ((GPIO_ENABLE_W1TC_V)<<(GPIO_ENABLE_W1TC_S))
#define GPIO_ENABLE_W1TC_V  0x3FFFFFF
#define GPIO_ENABLE_W1TC_S  0

#define GPIO_STRAP_REG          (DR_REG_GPIO_BASE + 0x38)
/* GPIO_STRAPPING : RO ;bitpos:[15:0] ;default:  ; */
/*description: */
#define GPIO_STRAPPING  0x0000FFFF
#define GPIO_STRAPPING_M  ((GPIO_STRAPPING_V)<<(GPIO_STRAPPING_S))
#define GPIO_STRAPPING_V  0xFFFF
#define GPIO_STRAPPING_S  0

#define GPIO_IN_REG          (DR_REG_GPIO_BASE + 0x3C)
/* GPIO_IN_DATA : RO ;bitpos:[25:0] ;default:  ; */
/*description: */
#define GPIO_IN_DATA  0x03FFFFFF
#define GPIO_IN_DATA_M  ((GPIO_IN_DATA_V)<<(GPIO_IN_DATA_S))
#define GPIO_IN_DATA_V  0x3FFFFFF
#define GPIO_IN_DATA_S  0

#define GPIO_STATUS_REG          (DR_REG_GPIO_BASE + 0x44)
/* GPIO_STATUS_INT : R/W ;bitpos:[25:0] ;default: 26'h0 ; */
/*description: */
#define GPIO_STATUS_INT  0x03FFFFFF
#define GPIO_STATUS_INT_M  ((GPIO_STATUS_INT_V)<<(GPIO_STATUS_INT_S))
#define GPIO_STATUS_INT_V  0x3FFFFFF
#define GPIO_STATUS_INT_S  0

#define GPIO_STATUS_W1TS_REG          (DR_REG_GPIO_BASE + 0x48)
/* GPIO_STATUS_W1TS : WO ;bitpos:[25:0] ;default: 26'h0 ; */
/*description: */
#define GPIO_STATUS_W1TS  0x03FFFFFF
#define GPIO_STATUS_W1TS_M  ((GPIO_STATUS_W1TS_V)<<(GPIO_STATUS_W1TS_S))
#define GPIO_STATUS_W1TS_V  0x3FFFFFF
#define GPIO_STATUS_W1TS_S  0

#define GPIO_STATUS_W1TC_REG          (DR_REG_GPIO_BASE + 0x4C)
/* GPIO_STATUS_W1TC : WO ;bitpos:[25:0] ;default: 26'h0 ; */
/*description: */
#define GPIO_STATUS_W1TC  0x03FFFFFF
#define GPIO_STATUS_W1TC_M  ((GPIO_STATUS_W1TC_V)<<(GPIO_STATUS_W1TC_S))
#define GPIO_STATUS_W1TC_V  0x3FFFFFF
#define GPIO_STATUS_W1TC_S  0

#define GPIO_PCPU_INT_REG          (DR_REG_GPIO_BASE + 0x5C)
/* GPIO_PROCPU_INT : RO ;bitpos:[25:0] ;default: 26'h0 ; */
/*description: */
#define GPIO_PROCPU_INT  0x03FFFFFF
#define GPIO_PROCPU_INT_M  ((GPIO_PROCPU_INT_V)<<(GPIO_PROCPU_INT_S))
#define GPIO_PROCPU_INT_V  0x3FFFFFF
#define GPIO_PROCPU_INT_S  0

#define GPIO_PCPU_NMI_INT_REG          (DR_REG_GPIO_BASE + 0x60)
/* GPIO_PROCPU_NMI_INT : RO ;bitpos:[25:0] ;default: 26'h0 ; */
/*description: */
#define GPIO_PROCPU_NMI_INT  0x03FFFFFF
#define GPIO_PROCPU_NMI_INT_M  ((GPIO_PROCPU_NMI_INT_V)<<(GPIO_PROCPU_NMI_INT_S))
#define GPIO_PROCPU_NMI_INT_V  0x3FFFFFF
#define GPIO_PROCPU_NMI_INT_S  0

#define GPIO_CPUSDIO_INT_REG          (DR_REG_GPIO_BASE + 0x64)
/* GPIO_SDIO_INT : RO ;bitpos:[25:0] ;default: 26'h0 ; */
/*description: */
#define GPIO_SDIO_INT  0x03FFFFFF
#define GPIO_SDIO_INT_M  ((GPIO_SDIO_INT_V)<<(GPIO_SDIO_INT_S))
#define GPIO_SDIO_INT_V  0x3FFFFFF
#define GPIO_SDIO_INT_S  0

#define GPIO_PIN0_REG          (DR_REG_GPIO_BASE + 0x74)
/* GPIO_PIN0_INT_ENA : R/W ;bitpos:[17:13] ;default: 5'h0 ; */
/*description: */
#define GPIO_PIN0_INT_ENA  0x0000001F
#define GPIO_PIN0_INT_ENA_M  ((GPIO_PIN0_INT_ENA_V)<<(GPIO_PIN0_INT_ENA_S))
#define GPIO_PIN0_INT_ENA_V  0x1F
#define GPIO_PIN0_INT_ENA_S  13
/* GPIO_PIN0_CONFIG : R/W ;bitpos:[12:11] ;default: 2'h0 ; */
/*description: */
#define GPIO_PIN0_CONFIG  0x00000003
#define GPIO_PIN0_CONFIG_M  ((GPIO_PIN0_CONFIG_V)<<(GPIO_PIN0_CONFIG_S))
#define GPIO_PIN0_CONFIG_V  0x3
#define GPIO_PIN0_CONFIG_S  11
/* GPIO_PIN0_WAKEUP_ENABLE : R/W ;bitpos:[10] ;default: 1'h0 ; */
/*description: */
#define GPIO_PIN0_WAKEUP_ENABLE  (BIT(10))
#define GPIO_PIN0_WAKEUP_ENABLE_M  (BIT(10))
#define GPIO_PIN0_WAKEUP_ENABLE_V  0x1
#define GPIO_PIN0_WAKEUP_ENABLE_S  10
/* GPIO_PIN0_INT_TYPE : R/W ;bitpos:[9:7] ;default: 3'h0 ; */
/*description: */
#define GPIO_PIN0_INT_TYPE  0x00000007
#define GPIO_PIN0_INT_TYPE_M  ((GPIO_PIN0_INT_TYPE_V)<<(GPIO_PIN0_INT_TYPE_S))
#define GPIO_PIN0_INT_TYPE_V  0x7
#define GPIO_PIN0_INT_TYPE_S  7
/* GPIO_PIN0_SYNC1_BYPASS : R/W ;bitpos:[4:3] ;default: 2'h0 ; */
/*description: */
#define GPIO_PIN0_SYNC1_BYPASS  0x00000003
#define GPIO_PIN0_SYNC1_BYPASS_M  ((GPIO_PIN0_SYNC1_BYPASS_V)<<(GPIO_PIN0_SYNC1_BYPASS_S))
#define GPIO_PIN0_SYNC1_BYPASS_V  0x3
#define GPIO_PIN0_SYNC1_BYPASS_S  3
/* GPIO_PIN0_PAD_DRIVER : R/W ;bitpos:[2] ;default: 1'h0 ; */
/*description: */
#define GPIO_PIN0_PAD_DRIVER  (BIT(2))
#define GPIO_PIN0_PAD_DRIVER_M  (BIT(2))
#define GPIO_PIN0_PAD_DRIVER_V  0x1
#define GPIO_PIN0_PAD_DRIVER_S  2
/* GPIO_PIN0_SYNC2_BYPASS : R/W ;bitpos:[1:0] ;default: 2'h0 ; */
/*description: */
#define GPIO_PIN0_SYNC2_BYPASS  0x00000003
#define GPIO_PIN0_SYNC2_BYPASS_M  ((GPIO_PIN0_SYNC2_BYPASS_V)<<(GPIO_PIN0_SYNC2_BYPASS_S))
#define GPIO_PIN0_SYNC2_BYPASS_V  0x3
#define GPIO_PIN0_SYNC2_BYPASS_S  0

#define GPIO_PIN1_REG          (DR_REG_GPIO_BASE + 0x78)
/* GPIO_PIN1_INT_ENA : R/W ;bitpos:[17:13] ;default: 5'h0 ; */
/*description: */
#define GPIO_PIN1_INT_ENA  0x0000001F
#define GPIO_PIN1_INT_ENA_M  ((GPIO_PIN1_INT_ENA_V)<<(GPIO_PIN1_INT_ENA_S))
#define GPIO_PIN1_INT_ENA_V  0x1F
#define GPIO_PIN1_INT_ENA_S  13
/* GPIO_PIN1_CONFIG : R/W ;bitpos:[12:11] ;default: 2'h0 ; */
/*description: */
#define GPIO_PIN1_CONFIG  0x00000003
#define GPIO_PIN1_CONFIG_M  ((GPIO_PIN1_CONFIG_V)<<(GPIO_PIN1_CONFIG_S))
#define GPIO_PIN1_CONFIG_V  0x3
#define GPIO_PIN1_CONFIG_S  11
/* GPIO_PIN1_WAKEUP_ENABLE : R/W ;bitpos:[10] ;default: 1'h0 ; */
/*description: */
#define GPIO_PIN1_WAKEUP_ENABLE  (BIT(10))
#define GPIO_PIN1_WAKEUP_ENABLE_M  (BIT(10))
#define GPIO_PIN1_WAKEUP_ENABLE_V  0x1
#define GPIO_PIN1_WAKEUP_ENABLE_S  10
/* GPIO_PIN1_INT_TYPE : R/W ;bitpos:[9:7] ;default: 3'h0 ; */
/*description: */
#define GPIO_PIN1_INT_TYPE  0x00000007
#define GPIO_PIN1_INT_TYPE_M  ((GPIO_PIN1_INT_TYPE_V)<<(GPIO_PIN1_INT_TYPE_S))
#define GPIO_PIN1_INT_TYPE_V  0x7
#define GPIO_PIN1_INT_TYPE_S  7
/* GPIO_PIN1_SYNC1_BYPASS : R/W ;bitpos:[4:3] ;default: 2'h0 ; */
/*description: */
#define GPIO_PIN1_SYNC1_BYPASS  0x00000003
#define GPIO_PIN1_SYNC1_BYPASS_M  ((GPIO_PIN1_SYNC1_BYPASS_V)<<(GPIO_PIN1_SYNC1_BYPASS_S))
#define GPIO_PIN1_SYNC1_BYPASS_V  0x3
#define GPIO_PIN1_SYNC1_BYPASS_S  3
/* GPIO_PIN1_PAD_DRIVER : R/W ;bitpos:[2] ;default: 1'h0 ; */
/*description: */
#define GPIO_PIN1_PAD_DRIVER  (BIT(2))
#define GPIO_PIN1_PAD_DRIVER_M  (BIT(2))
#define GPIO_PIN1_PAD_DRIVER_V  0x1
#define GPIO_PIN1_PAD_DRIVER_S  2
/* GPIO_PIN1_SYNC2_BYPASS : R/W ;bitpos:[1:0] ;default: 2'h0 ; */
/*description: */
#define GPIO_PIN1_SYNC2_BYPASS  0x00000003
#define GPIO_PIN1_SYNC2_BYPASS_M  ((GPIO_PIN1_SYNC2_BYPASS_V)<<(GPIO_PIN1_SYNC2_BYPASS_S))
#define GPIO_PIN1_SYNC2_BYPASS_V  0x3
#define GPIO_PIN1_SYNC2_BYPASS_S  0

#define GPIO_PIN2_REG          (DR_REG_GPIO_BASE + 0x7C)
/* GPIO_PIN2_INT_ENA : R/W ;bitpos:[17:13] ;default: 5'h0 ; */
/*description: */
#define GPIO_PIN2_INT_ENA  0x0000001F
#define GPIO_PIN2_INT_ENA_M  ((GPIO_PIN2_INT_ENA_V)<<(GPIO_PIN2_INT_ENA_S))
#define GPIO_PIN2_INT_ENA_V  0x1F
#define GPIO_PIN2_INT_ENA_S  13
/* GPIO_PIN2_CONFIG : R/W ;bitpos:[12:11] ;default: 2'h0 ; */
/*description: */
#define GPIO_PIN2_CONFIG  0x00000003
#define GPIO_PIN2_CONFIG_M  ((GPIO_PIN2_CONFIG_V)<<(GPIO_PIN2_CONFIG_S))
#define GPIO_PIN2_CONFIG_V  0x3
#define GPIO_PIN2_CONFIG_S  11
/* GPIO_PIN2_WAKEUP_ENABLE : R/W ;bitpos:[10] ;default: 1'h0 ; */
/*description: */
#define GPIO_PIN2_WAKEUP_ENABLE  (BIT(10))
#define GPIO_PIN2_WAKEUP_ENABLE_M  (BIT(10))
#define GPIO_PIN2_WAKEUP_ENABLE_V  0x1
#define GPIO_PIN2_WAKEUP_ENABLE_S  10
/* GPIO_PIN2_INT_TYPE : R/W ;bitpos:[9:7] ;default: 3'h0 ; */
/*description: */
#define GPIO_PIN2_INT_TYPE  0x00000007
#define GPIO_PIN2_INT_TYPE_M  ((GPIO_PIN2_INT_TYPE_V)<<(GPIO_PIN2_INT_TYPE_S))
#define GPIO_PIN2_INT_TYPE_V  0x7
#define GPIO_PIN2_INT_TYPE_S  7
/* GPIO_PIN2_SYNC1_BYPASS : R/W ;bitpos:[4:3] ;default: 2'h0 ; */
/*description: */
#define GPIO_PIN2_SYNC1_BYPASS  0x00000003
#define GPIO_PIN2_SYNC1_BYPASS_M  ((GPIO_PIN2_SYNC1_BYPASS_V)<<(GPIO_PIN2_SYNC1_BYPASS_S))
#define GPIO_PIN2_SYNC1_BYPASS_V  0x3
#define GPIO_PIN2_SYNC1_BYPASS_S  3
/* GPIO_PIN2_PAD_DRIVER : R/W ;bitpos:[2] ;default: 1'h0 ; */
/*description: */
#define GPIO_PIN2_PAD_DRIVER  (BIT(2))
#define GPIO_PIN2_PAD_DRIVER_M  (BIT(2))
#define GPIO_PIN2_PAD_DRIVER_V  0x1
#define GPIO_PIN2_PAD_DRIVER_S  2
/* GPIO_PIN2_SYNC2_BYPASS : R/W ;bitpos:[1:0] ;default: 2'h0 ; */
/*description: */
#define GPIO_PIN2_SYNC2_BYPASS  0x00000003
#define GPIO_PIN2_SYNC2_BYPASS_M  ((GPIO_PIN2_SYNC2_BYPASS_V)<<(GPIO_PIN2_SYNC2_BYPASS_S))
#define GPIO_PIN2_SYNC2_BYPASS_V  0x3
#define GPIO_PIN2_SYNC2_BYPASS_S  0

#define GPIO_PIN3_REG          (DR_REG_GPIO_BASE + 0x80)
/* GPIO_PIN3_INT_ENA : R/W ;bitpos:[17:13] ;default: 5'h0 ; */
/*description: */
#define GPIO_PIN3_INT_ENA  0x0000001F
#define GPIO_PIN3_INT_ENA_M  ((GPIO_PIN3_INT_ENA_V)<<(GPIO_PIN3_INT_ENA_S))
#define GPIO_PIN3_INT_ENA_V  0x1F
#define GPIO_PIN3_INT_ENA_S  13
/* GPIO_PIN3_CONFIG : R/W ;bitpos:[12:11] ;default: 2'h0 ; */
/*description: */
#define GPIO_PIN3_CONFIG  0x00000003
#define GPIO_PIN3_CONFIG_M  ((GPIO_PIN3_CONFIG_V)<<(GPIO_PIN3_CONFIG_S))
#define GPIO_PIN3_CONFIG_V  0x3
#define GPIO_PIN3_CONFIG_S  11
/* GPIO_PIN3_WAKEUP_ENABLE : R/W ;bitpos:[10] ;default: 1'h0 ; */
/*description: */
#define GPIO_PIN3_WAKEUP_ENABLE  (BIT(10))
#define GPIO_PIN3_WAKEUP_ENABLE_M  (BIT(10))
#define GPIO_PIN3_WAKEUP_ENABLE_V  0x1
#define GPIO_PIN3_WAKEUP_ENABLE_S  10
/* GPIO_PIN3_INT_TYPE : R/W ;bitpos:[9:7] ;default: 3'h0 ; */
/*description: */
#define GPIO_PIN3_INT_TYPE  0x00000007
#define GPIO_PIN3_INT_TYPE_M  ((GPIO_PIN3_INT_TYPE_V)<<(GPIO_PIN3_INT_TYPE_S))
#define GPIO_PIN3_INT_TYPE_V  0x7
#define GPIO_PIN3_INT_TYPE_S  7
/* GPIO_PIN3_SYNC1_BYPASS : R/W ;bitpos:[4:3] ;default: 2'h0 ; */
/*description: */
#define GPIO_PIN3_SYNC1_BYPASS  0x00000003
#define GPIO_PIN3_SYNC1_BYPASS_M  ((GPIO_PIN3_SYNC1_BYPASS_V)<<(GPIO_PIN3_SYNC1_BYPASS_S))
#define GPIO_PIN3_SYNC1_BYPASS_V  0x3
#define GPIO_PIN3_SYNC1_BYPASS_S  3
/* GPIO_PIN3_PAD_DRIVER : R/W ;bitpos:[2] ;default: 1'h0 ; */
/*description: */
#define GPIO_PIN3_PAD_DRIVER  (BIT(2))
#define GPIO_PIN3_PAD_DRIVER_M  (BIT(2))
#define GPIO_PIN3_PAD_DRIVER_V  0x1
#define GPIO_PIN3_PAD_DRIVER_S  2
/* GPIO_PIN3_SYNC2_BYPASS : R/W ;bitpos:[1:0] ;default: 2'h0 ; */
/*description: */
#define GPIO_PIN3_SYNC2_BYPASS  0x00000003
#define GPIO_PIN3_SYNC2_BYPASS_M  ((GPIO_PIN3_SYNC2_BYPASS_V)<<(GPIO_PIN3_SYNC2_BYPASS_S))
#define GPIO_PIN3_SYNC2_BYPASS_V  0x3
#define GPIO_PIN3_SYNC2_BYPASS_S  0

#define GPIO_PIN4_REG          (DR_REG_GPIO_BASE + 0x84)
/* GPIO_PIN4_INT_ENA : R/W ;bitpos:[17:13] ;default: 5'h0 ; */
/*description: */
#define GPIO_PIN4_INT_ENA  0x0000001F
#define GPIO_PIN4_INT_ENA_M  ((GPIO_PIN4_INT_ENA_V)<<(GPIO_PIN4_INT_ENA_S))
#define GPIO_PIN4_INT_ENA_V  0x1F
#define GPIO_PIN4_INT_ENA_S  13
/* GPIO_PIN4_CONFIG : R/W ;bitpos:[12:11] ;default: 2'h0 ; */
/*description: */
#define GPIO_PIN4_CONFIG  0x00000003
#define GPIO_PIN4_CONFIG_M  ((GPIO_PIN4_CONFIG_V)<<(GPIO_PIN4_CONFIG_S))
#define GPIO_PIN4_CONFIG_V  0x3
#define GPIO_PIN4_CONFIG_S  11
/* GPIO_PIN4_WAKEUP_ENABLE : R/W ;bitpos:[10] ;default: 1'h0 ; */
/*description: */
#define GPIO_PIN4_WAKEUP_ENABLE  (BIT(10))
#define GPIO_PIN4_WAKEUP_ENABLE_M  (BIT(10))
#define GPIO_PIN4_WAKEUP_ENABLE_V  0x1
#define GPIO_PIN4_WAKEUP_ENABLE_S  10
/* GPIO_PIN4_INT_TYPE : R/W ;bitpos:[9:7] ;default: 3'h0 ; */
/*description: */
#define GPIO_PIN4_INT_TYPE  0x00000007
#define GPIO_PIN4_INT_TYPE_M  ((GPIO_PIN4_INT_TYPE_V)<<(GPIO_PIN4_INT_TYPE_S))
#define GPIO_PIN4_INT_TYPE_V  0x7
#define GPIO_PIN4_INT_TYPE_S  7
/* GPIO_PIN4_SYNC1_BYPASS : R/W ;bitpos:[4:3] ;default: 2'h0 ; */
/*description: */
#define GPIO_PIN4_SYNC1_BYPASS  0x00000003
#define GPIO_PIN4_SYNC1_BYPASS_M  ((GPIO_PIN4_SYNC1_BYPASS_V)<<(GPIO_PIN4_SYNC1_BYPASS_S))
#define GPIO_PIN4_SYNC1_BYPASS_V  0x3
#define GPIO_PIN4_SYNC1_BYPASS_S  3
/* GPIO_PIN4_PAD_DRIVER : R/W ;bitpos:[2] ;default: 1'h0 ; */
/*description: */
#define GPIO_PIN4_PAD_DRIVER  (BIT(2))
#define GPIO_PIN4_PAD_DRIVER_M  (BIT(2))
#define GPIO_PIN4_PAD_DRIVER_V  0x1
#define GPIO_PIN4_PAD_DRIVER_S  2
/* GPIO_PIN4_SYNC2_BYPASS : R/W ;bitpos:[1:0] ;default: 2'h0 ; */
/*description: */
#define GPIO_PIN4_SYNC2_BYPASS  0x00000003
#define GPIO_PIN4_SYNC2_BYPASS_M  ((GPIO_PIN4_SYNC2_BYPASS_V)<<(GPIO_PIN4_SYNC2_BYPASS_S))
#define GPIO_PIN4_SYNC2_BYPASS_V  0x3
#define GPIO_PIN4_SYNC2_BYPASS_S  0

#define GPIO_PIN5_REG          (DR_REG_GPIO_BASE + 0x88)
/* GPIO_PIN5_INT_ENA : R/W ;bitpos:[17:13] ;default: 5'h0 ; */
/*description: */
#define GPIO_PIN5_INT_ENA  0x0000001F
#define GPIO_PIN5_INT_ENA_M  ((GPIO_PIN5_INT_ENA_V)<<(GPIO_PIN5_INT_ENA_S))
#define GPIO_PIN5_INT_ENA_V  0x1F
#define GPIO_PIN5_INT_ENA_S  13
/* GPIO_PIN5_CONFIG : R/W ;bitpos:[12:11] ;default: 2'h0 ; */
/*description: */
#define GPIO_PIN5_CONFIG  0x00000003
#define GPIO_PIN5_CONFIG_M  ((GPIO_PIN5_CONFIG_V)<<(GPIO_PIN5_CONFIG_S))
#define GPIO_PIN5_CONFIG_V  0x3
#define GPIO_PIN5_CONFIG_S  11
/* GPIO_PIN5_WAKEUP_ENABLE : R/W ;bitpos:[10] ;default: 1'h0 ; */
/*description: */
#define GPIO_PIN5_WAKEUP_ENABLE  (BIT(10))
#define GPIO_PIN5_WAKEUP_ENABLE_M  (BIT(10))
#define GPIO_PIN5_WAKEUP_ENABLE_V  0x1
#define GPIO_PIN5_WAKEUP_ENABLE_S  10
/* GPIO_PIN5_INT_TYPE : R/W ;bitpos:[9:7] ;default: 3'h0 ; */
/*description: */
#define GPIO_PIN5_INT_TYPE  0x00000007
#define GPIO_PIN5_INT_TYPE_M  ((GPIO_PIN5_INT_TYPE_V)<<(GPIO_PIN5_INT_TYPE_S))
#define GPIO_PIN5_INT_TYPE_V  0x7
#define GPIO_PIN5_INT_TYPE_S  7
/* GPIO_PIN5_SYNC1_BYPASS : R/W ;bitpos:[4:3] ;default: 2'h0 ; */
/*description: */
#define GPIO_PIN5_SYNC1_BYPASS  0x00000003
#define GPIO_PIN5_SYNC1_BYPASS_M  ((GPIO_PIN5_SYNC1_BYPASS_V)<<(GPIO_PIN5_SYNC1_BYPASS_S))
#define GPIO_PIN5_SYNC1_BYPASS_V  0x3
#define GPIO_PIN5_SYNC1_BYPASS_S  3
/* GPIO_PIN5_PAD_DRIVER : R/W ;bitpos:[2] ;default: 1'h0 ; */
/*description: */
#define GPIO_PIN5_PAD_DRIVER  (BIT(2))
#define GPIO_PIN5_PAD_DRIVER_M  (BIT(2))
#define GPIO_PIN5_PAD_DRIVER_V  0x1
#define GPIO_PIN5_PAD_DRIVER_S  2
/* GPIO_PIN5_SYNC2_BYPASS : R/W ;bitpos:[1:0] ;default: 2'h0 ; */
/*description: */
#define GPIO_PIN5_SYNC2_BYPASS  0x00000003
#define GPIO_PIN5_SYNC2_BYPASS_M  ((GPIO_PIN5_SYNC2_BYPASS_V)<<(GPIO_PIN5_SYNC2_BYPASS_S))
#define GPIO_PIN5_SYNC2_BYPASS_V  0x3
#define GPIO_PIN5_SYNC2_BYPASS_S  0

#define GPIO_PIN6_REG          (DR_REG_GPIO_BASE + 0x8C)
/* GPIO_PIN6_INT_ENA : R/W ;bitpos:[17:13] ;default: 5'h0 ; */
/*description: */
#define GPIO_PIN6_INT_ENA  0x0000001F
#define GPIO_PIN6_INT_ENA_M  ((GPIO_PIN6_INT_ENA_V)<<(GPIO_PIN6_INT_ENA_S))
#define GPIO_PIN6_INT_ENA_V  0x1F
#define GPIO_PIN6_INT_ENA_S  13
/* GPIO_PIN6_CONFIG : R/W ;bitpos:[12:11] ;default: 2'h0 ; */
/*description: */
#define GPIO_PIN6_CONFIG  0x00000003
#define GPIO_PIN6_CONFIG_M  ((GPIO_PIN6_CONFIG_V)<<(GPIO_PIN6_CONFIG_S))
#define GPIO_PIN6_CONFIG_V  0x3
#define GPIO_PIN6_CONFIG_S  11
/* GPIO_PIN6_WAKEUP_ENABLE : R/W ;bitpos:[10] ;default: 1'h0 ; */
/*description: */
#define GPIO_PIN6_WAKEUP_ENABLE  (BIT(10))
#define GPIO_PIN6_WAKEUP_ENABLE_M  (BIT(10))
#define GPIO_PIN6_WAKEUP_ENABLE_V  0x1
#define GPIO_PIN6_WAKEUP_ENABLE_S  10
/* GPIO_PIN6_INT_TYPE : R/W ;bitpos:[9:7] ;default: 3'h0 ; */
/*description: */
#define GPIO_PIN6_INT_TYPE  0x00000007
#define GPIO_PIN6_INT_TYPE_M  ((GPIO_PIN6_INT_TYPE_V)<<(GPIO_PIN6_INT_TYPE_S))
#define GPIO_PIN6_INT_TYPE_V  0x7
#define GPIO_PIN6_INT_TYPE_S  7
/* GPIO_PIN6_SYNC1_BYPASS : R/W ;bitpos:[4:3] ;default: 2'h0 ; */
/*description: */
#define GPIO_PIN6_SYNC1_BYPASS  0x00000003
#define GPIO_PIN6_SYNC1_BYPASS_M  ((GPIO_PIN6_SYNC1_BYPASS_V)<<(GPIO_PIN6_SYNC1_BYPASS_S))
#define GPIO_PIN6_SYNC1_BYPASS_V  0x3
#define GPIO_PIN6_SYNC1_BYPASS_S  3
/* GPIO_PIN6_PAD_DRIVER : R/W ;bitpos:[2] ;default: 1'h0 ; */
/*description: */
#define GPIO_PIN6_PAD_DRIVER  (BIT(2))
#define GPIO_PIN6_PAD_DRIVER_M  (BIT(2))
#define GPIO_PIN6_PAD_DRIVER_V  0x1
#define GPIO_PIN6_PAD_DRIVER_S  2
/* GPIO_PIN6_SYNC2_BYPASS : R/W ;bitpos:[1:0] ;default: 2'h0 ; */
/*description: */
#define GPIO_PIN6_SYNC2_BYPASS  0x00000003
#define GPIO_PIN6_SYNC2_BYPASS_M  ((GPIO_PIN6_SYNC2_BYPASS_V)<<(GPIO_PIN6_SYNC2_BYPASS_S))
#define GPIO_PIN6_SYNC2_BYPASS_V  0x3
#define GPIO_PIN6_SYNC2_BYPASS_S  0

#define GPIO_PIN7_REG          (DR_REG_GPIO_BASE + 0x90)
/* GPIO_PIN7_INT_ENA : R/W ;bitpos:[17:13] ;default: 5'h0 ; */
/*description: */
#define GPIO_PIN7_INT_ENA  0x0000001F
#define GPIO_PIN7_INT_ENA_M  ((GPIO_PIN7_INT_ENA_V)<<(GPIO_PIN7_INT_ENA_S))
#define GPIO_PIN7_INT_ENA_V  0x1F
#define GPIO_PIN7_INT_ENA_S  13
/* GPIO_PIN7_CONFIG : R/W ;bitpos:[12:11] ;default: 2'h0 ; */
/*description: */
#define GPIO_PIN7_CONFIG  0x00000003
#define GPIO_PIN7_CONFIG_M  ((GPIO_PIN7_CONFIG_V)<<(GPIO_PIN7_CONFIG_S))
#define GPIO_PIN7_CONFIG_V  0x3
#define GPIO_PIN7_CONFIG_S  11
/* GPIO_PIN7_WAKEUP_ENABLE : R/W ;bitpos:[10] ;default: 1'h0 ; */
/*description: */
#define GPIO_PIN7_WAKEUP_ENABLE  (BIT(10))
#define GPIO_PIN7_WAKEUP_ENABLE_M  (BIT(10))
#define GPIO_PIN7_WAKEUP_ENABLE_V  0x1
#define GPIO_PIN7_WAKEUP_ENABLE_S  10
/* GPIO_PIN7_INT_TYPE : R/W ;bitpos:[9:7] ;default: 3'h0 ; */
/*description: */
#define GPIO_PIN7_INT_TYPE  0x00000007
#define GPIO_PIN7_INT_TYPE_M  ((GPIO_PIN7_INT_TYPE_V)<<(GPIO_PIN7_INT_TYPE_S))
#define GPIO_PIN7_INT_TYPE_V  0x7
#define GPIO_PIN7_INT_TYPE_S  7
/* GPIO_PIN7_SYNC1_BYPASS : R/W ;bitpos:[4:3] ;default: 2'h0 ; */
/*description: */
#define GPIO_PIN7_SYNC1_BYPASS  0x00000003
#define GPIO_PIN7_SYNC1_BYPASS_M  ((GPIO_PIN7_SYNC1_BYPASS_V)<<(GPIO_PIN7_SYNC1_BYPASS_S))
#define GPIO_PIN7_SYNC1_BYPASS_V  0x3
#define GPIO_PIN7_SYNC1_BYPASS_S  3
/* GPIO_PIN7_PAD_DRIVER : R/W ;bitpos:[2] ;default: 1'h0 ; */
/*description: */
#define GPIO_PIN7_PAD_DRIVER  (BIT(2))
#define GPIO_PIN7_PAD_DRIVER_M  (BIT(2))
#define GPIO_PIN7_PAD_DRIVER_V  0x1
#define GPIO_PIN7_PAD_DRIVER_S  2
/* GPIO_PIN7_SYNC2_BYPASS : R/W ;bitpos:[1:0] ;default: 2'h0 ; */
/*description: */
#define GPIO_PIN7_SYNC2_BYPASS  0x00000003
#define GPIO_PIN7_SYNC2_BYPASS_M  ((GPIO_PIN7_SYNC2_BYPASS_V)<<(GPIO_PIN7_SYNC2_BYPASS_S))
#define GPIO_PIN7_SYNC2_BYPASS_V  0x3
#define GPIO_PIN7_SYNC2_BYPASS_S  0

#define GPIO_PIN8_REG          (DR_REG_GPIO_BASE + 0x94)
/* GPIO_PIN8_INT_ENA : R/W ;bitpos:[17:13] ;default: 5'h0 ; */
/*description: */
#define GPIO_PIN8_INT_ENA  0x0000001F
#define GPIO_PIN8_INT_ENA_M  ((GPIO_PIN8_INT_ENA_V)<<(GPIO_PIN8_INT_ENA_S))
#define GPIO_PIN8_INT_ENA_V  0x1F
#define GPIO_PIN8_INT_ENA_S  13
/* GPIO_PIN8_CONFIG : R/W ;bitpos:[12:11] ;default: 2'h0 ; */
/*description: */
#define GPIO_PIN8_CONFIG  0x00000003
#define GPIO_PIN8_CONFIG_M  ((GPIO_PIN8_CONFIG_V)<<(GPIO_PIN8_CONFIG_S))
#define GPIO_PIN8_CONFIG_V  0x3
#define GPIO_PIN8_CONFIG_S  11
/* GPIO_PIN8_WAKEUP_ENABLE : R/W ;bitpos:[10] ;default: 1'h0 ; */
/*description: */
#define GPIO_PIN8_WAKEUP_ENABLE  (BIT(10))
#define GPIO_PIN8_WAKEUP_ENABLE_M  (BIT(10))
#define GPIO_PIN8_WAKEUP_ENABLE_V  0x1
#define GPIO_PIN8_WAKEUP_ENABLE_S  10
/* GPIO_PIN8_INT_TYPE : R/W ;bitpos:[9:7] ;default: 3'h0 ; */
/*description: */
#define GPIO_PIN8_INT_TYPE  0x00000007
#define GPIO_PIN8_INT_TYPE_M  ((GPIO_PIN8_INT_TYPE_V)<<(GPIO_PIN8_INT_TYPE_S))
#define GPIO_PIN8_INT_TYPE_V  0x7
#define GPIO_PIN8_INT_TYPE_S  7
/* GPIO_PIN8_SYNC1_BYPASS : R/W ;bitpos:[4:3] ;default: 2'h0 ; */
/*description: */
#define GPIO_PIN8_SYNC1_BYPASS  0x00000003
#define GPIO_PIN8_SYNC1_BYPASS_M  ((GPIO_PIN8_SYNC1_BYPASS_V)<<(GPIO_PIN8_SYNC1_BYPASS_S))
#define GPIO_PIN8_SYNC1_BYPASS_V  0x3
#define GPIO_PIN8_SYNC1_BYPASS_S  3
/* GPIO_PIN8_PAD_DRIVER : R/W ;bitpos:[2] ;default: 1'h0 ; */
/*description: */
#define GPIO_PIN8_PAD_DRIVER  (BIT(2))
#define GPIO_PIN8_PAD_DRIVER_M  (BIT(2))
#define GPIO_PIN8_PAD_DRIVER_V  0x1
#define GPIO_PIN8_PAD_DRIVER_S  2
/* GPIO_PIN8_SYNC2_BYPASS : R/W ;bitpos:[1:0] ;default: 2'h0 ; */
/*description: */
#define GPIO_PIN8_SYNC2_BYPASS  0x00000003
#define GPIO_PIN8_SYNC2_BYPASS_M  ((GPIO_PIN8_SYNC2_BYPASS_V)<<(GPIO_PIN8_SYNC2_BYPASS_S))
#define GPIO_PIN8_SYNC2_BYPASS_V  0x3
#define GPIO_PIN8_SYNC2_BYPASS_S  0

#define GPIO_PIN9_REG          (DR_REG_GPIO_BASE + 0x98)
/* GPIO_PIN9_INT_ENA : R/W ;bitpos:[17:13] ;default: 5'h0 ; */
/*description: */
#define GPIO_PIN9_INT_ENA  0x0000001F
#define GPIO_PIN9_INT_ENA_M  ((GPIO_PIN9_INT_ENA_V)<<(GPIO_PIN9_INT_ENA_S))
#define GPIO_PIN9_INT_ENA_V  0x1F
#define GPIO_PIN9_INT_ENA_S  13
/* GPIO_PIN9_CONFIG : R/W ;bitpos:[12:11] ;default: 2'h0 ; */
/*description: */
#define GPIO_PIN9_CONFIG  0x00000003
#define GPIO_PIN9_CONFIG_M  ((GPIO_PIN9_CONFIG_V)<<(GPIO_PIN9_CONFIG_S))
#define GPIO_PIN9_CONFIG_V  0x3
#define GPIO_PIN9_CONFIG_S  11
/* GPIO_PIN9_WAKEUP_ENABLE : R/W ;bitpos:[10] ;default: 1'h0 ; */
/*description: */
#define GPIO_PIN9_WAKEUP_ENABLE  (BIT(10))
#define GPIO_PIN9_WAKEUP_ENABLE_M  (BIT(10))
#define GPIO_PIN9_WAKEUP_ENABLE_V  0x1
#define GPIO_PIN9_WAKEUP_ENABLE_S  10
/* GPIO_PIN9_INT_TYPE : R/W ;bitpos:[9:7] ;default: 3'h0 ; */
/*description: */
#define GPIO_PIN9_INT_TYPE  0x00000007
#define GPIO_PIN9_INT_TYPE_M  ((GPIO_PIN9_INT_TYPE_V)<<(GPIO_PIN9_INT_TYPE_S))
#define GPIO_PIN9_INT_TYPE_V  0x7
#define GPIO_PIN9_INT_TYPE_S  7
/* GPIO_PIN9_SYNC1_BYPASS : R/W ;bitpos:[4:3] ;default: 2'h0 ; */
/*description: */
#define GPIO_PIN9_SYNC1_BYPASS  0x00000003
#define GPIO_PIN9_SYNC1_BYPASS_M  ((GPIO_PIN9_SYNC1_BYPASS_V)<<(GPIO_PIN9_SYNC1_BYPASS_S))
#define GPIO_PIN9_SYNC1_BYPASS_V  0x3
#define GPIO_PIN9_SYNC1_BYPASS_S  3
/* GPIO_PIN9_PAD_DRIVER : R/W ;bitpos:[2] ;default: 1'h0 ; */
/*description: */
#define GPIO_PIN9_PAD_DRIVER  (BIT(2))
#define GPIO_PIN9_PAD_DRIVER_M  (BIT(2))
#define GPIO_PIN9_PAD_DRIVER_V  0x1
#define GPIO_PIN9_PAD_DRIVER_S  2
/* GPIO_PIN9_SYNC2_BYPASS : R/W ;bitpos:[1:0] ;default: 2'h0 ; */
/*description: */
#define GPIO_PIN9_SYNC2_BYPASS  0x00000003
#define GPIO_PIN9_SYNC2_BYPASS_M  ((GPIO_PIN9_SYNC2_BYPASS_V)<<(GPIO_PIN9_SYNC2_BYPASS_S))
#define GPIO_PIN9_SYNC2_BYPASS_V  0x3
#define GPIO_PIN9_SYNC2_BYPASS_S  0

#define GPIO_PIN10_REG          (DR_REG_GPIO_BASE + 0x9C)
/* GPIO_PIN10_INT_ENA : R/W ;bitpos:[17:13] ;default: 5'h0 ; */
/*description: */
#define GPIO_PIN10_INT_ENA  0x0000001F
#define GPIO_PIN10_INT_ENA_M  ((GPIO_PIN10_INT_ENA_V)<<(GPIO_PIN10_INT_ENA_S))
#define GPIO_PIN10_INT_ENA_V  0x1F
#define GPIO_PIN10_INT_ENA_S  13
/* GPIO_PIN10_CONFIG : R/W ;bitpos:[12:11] ;default: 2'h0 ; */
/*description: */
#define GPIO_PIN10_CONFIG  0x00000003
#define GPIO_PIN10_CONFIG_M  ((GPIO_PIN10_CONFIG_V)<<(GPIO_PIN10_CONFIG_S))
#define GPIO_PIN10_CONFIG_V  0x3
#define GPIO_PIN10_CONFIG_S  11
/* GPIO_PIN10_WAKEUP_ENABLE : R/W ;bitpos:[10] ;default: 1'h0 ; */
/*description: */
#define GPIO_PIN10_WAKEUP_ENABLE  (BIT(10))
#define GPIO_PIN10_WAKEUP_ENABLE_M  (BIT(10))
#define GPIO_PIN10_WAKEUP_ENABLE_V  0x1
#define GPIO_PIN10_WAKEUP_ENABLE_S  10
/* GPIO_PIN10_INT_TYPE : R/W ;bitpos:[9:7] ;default: 3'h0 ; */
/*description: */
#define GPIO_PIN10_INT_TYPE  0x00000007
#define GPIO_PIN10_INT_TYPE_M  ((GPIO_PIN10_INT_TYPE_V)<<(GPIO_PIN10_INT_TYPE_S))
#define GPIO_PIN10_INT_TYPE_V  0x7
#define GPIO_PIN10_INT_TYPE_S  7
/* GPIO_PIN10_SYNC1_BYPASS : R/W ;bitpos:[4:3] ;default: 2'h0 ; */
/*description: */
#define GPIO_PIN10_SYNC1_BYPASS  0x00000003
#define GPIO_PIN10_SYNC1_BYPASS_M  ((GPIO_PIN10_SYNC1_BYPASS_V)<<(GPIO_PIN10_SYNC1_BYPASS_S))
#define GPIO_PIN10_SYNC1_BYPASS_V  0x3
#define GPIO_PIN10_SYNC1_BYPASS_S  3
/* GPIO_PIN10_PAD_DRIVER : R/W ;bitpos:[2] ;default: 1'h0 ; */
/*description: */
#define GPIO_PIN10_PAD_DRIVER  (BIT(2))
#define GPIO_PIN10_PAD_DRIVER_M  (BIT(2))
#define GPIO_PIN10_PAD_DRIVER_V  0x1
#define GPIO_PIN10_PAD_DRIVER_S  2
/* GPIO_PIN10_SYNC2_BYPASS : R/W ;bitpos:[1:0] ;default: 2'h0 ; */
/*description: */
#define GPIO_PIN10_SYNC2_BYPASS  0x00000003
#define GPIO_PIN10_SYNC2_BYPASS_M  ((GPIO_PIN10_SYNC2_BYPASS_V)<<(GPIO_PIN10_SYNC2_BYPASS_S))
#define GPIO_PIN10_SYNC2_BYPASS_V  0x3
#define GPIO_PIN10_SYNC2_BYPASS_S  0

#define GPIO_PIN11_REG          (DR_REG_GPIO_BASE + 0xA0)
/* GPIO_PIN11_INT_ENA : R/W ;bitpos:[17:13] ;default: 5'h0 ; */
/*description: */
#define GPIO_PIN11_INT_ENA  0x0000001F
#define GPIO_PIN11_INT_ENA_M  ((GPIO_PIN11_INT_ENA_V)<<(GPIO_PIN11_INT_ENA_S))
#define GPIO_PIN11_INT_ENA_V  0x1F
#define GPIO_PIN11_INT_ENA_S  13
/* GPIO_PIN11_CONFIG : R/W ;bitpos:[12:11] ;default: 2'h0 ; */
/*description: */
#define GPIO_PIN11_CONFIG  0x00000003
#define GPIO_PIN11_CONFIG_M  ((GPIO_PIN11_CONFIG_V)<<(GPIO_PIN11_CONFIG_S))
#define GPIO_PIN11_CONFIG_V  0x3
#define GPIO_PIN11_CONFIG_S  11
/* GPIO_PIN11_WAKEUP_ENABLE : R/W ;bitpos:[10] ;default: 1'h0 ; */
/*description: */
#define GPIO_PIN11_WAKEUP_ENABLE  (BIT(10))
#define GPIO_PIN11_WAKEUP_ENABLE_M  (BIT(10))
#define GPIO_PIN11_WAKEUP_ENABLE_V  0x1
#define GPIO_PIN11_WAKEUP_ENABLE_S  10
/* GPIO_PIN11_INT_TYPE : R/W ;bitpos:[9:7] ;default: 3'h0 ; */
/*description: */
#define GPIO_PIN11_INT_TYPE  0x00000007
#define GPIO_PIN11_INT_TYPE_M  ((GPIO_PIN11_INT_TYPE_V)<<(GPIO_PIN11_INT_TYPE_S))
#define GPIO_PIN11_INT_TYPE_V  0x7
#define GPIO_PIN11_INT_TYPE_S  7
/* GPIO_PIN11_SYNC1_BYPASS : R/W ;bitpos:[4:3] ;default: 2'h0 ; */
/*description: */
#define GPIO_PIN11_SYNC1_BYPASS  0x00000003
#define GPIO_PIN11_SYNC1_BYPASS_M  ((GPIO_PIN11_SYNC1_BYPASS_V)<<(GPIO_PIN11_SYNC1_BYPASS_S))
#define GPIO_PIN11_SYNC1_BYPASS_V  0x3
#define GPIO_PIN11_SYNC1_BYPASS_S  3
/* GPIO_PIN11_PAD_DRIVER : R/W ;bitpos:[2] ;default: 1'h0 ; */
/*description: */
#define GPIO_PIN11_PAD_DRIVER  (BIT(2))
#define GPIO_PIN11_PAD_DRIVER_M  (BIT(2))
#define GPIO_PIN11_PAD_DRIVER_V  0x1
#define GPIO_PIN11_PAD_DRIVER_S  2
/* GPIO_PIN11_SYNC2_BYPASS : R/W ;bitpos:[1:0] ;default: 2'h0 ; */
/*description: */
#define GPIO_PIN11_SYNC2_BYPASS  0x00000003
#define GPIO_PIN11_SYNC2_BYPASS_M  ((GPIO_PIN11_SYNC2_BYPASS_V)<<(GPIO_PIN11_SYNC2_BYPASS_S))
#define GPIO_PIN11_SYNC2_BYPASS_V  0x3
#define GPIO_PIN11_SYNC2_BYPASS_S  0

#define GPIO_PIN12_REG          (DR_REG_GPIO_BASE + 0xA4)
/* GPIO_PIN12_INT_ENA : R/W ;bitpos:[17:13] ;default: 5'h0 ; */
/*description: */
#define GPIO_PIN12_INT_ENA  0x0000001F
#define GPIO_PIN12_INT_ENA_M  ((GPIO_PIN12_INT_ENA_V)<<(GPIO_PIN12_INT_ENA_S))
#define GPIO_PIN12_INT_ENA_V  0x1F
#define GPIO_PIN12_INT_ENA_S  13
/* GPIO_PIN12_CONFIG : R/W ;bitpos:[12:11] ;default: 2'h0 ; */
/*description: */
#define GPIO_PIN12_CONFIG  0x00000003
#define GPIO_PIN12_CONFIG_M  ((GPIO_PIN12_CONFIG_V)<<(GPIO_PIN12_CONFIG_S))
#define GPIO_PIN12_CONFIG_V  0x3
#define GPIO_PIN12_CONFIG_S  11
/* GPIO_PIN12_WAKEUP_ENABLE : R/W ;bitpos:[10] ;default: 1'h0 ; */
/*description: */
#define GPIO_PIN12_WAKEUP_ENABLE  (BIT(10))
#define GPIO_PIN12_WAKEUP_ENABLE_M  (BIT(10))
#define GPIO_PIN12_WAKEUP_ENABLE_V  0x1
#define GPIO_PIN12_WAKEUP_ENABLE_S  10
/* GPIO_PIN12_INT_TYPE : R/W ;bitpos:[9:7] ;default: 3'h0 ; */
/*description: */
#define GPIO_PIN12_INT_TYPE  0x00000007
#define GPIO_PIN12_INT_TYPE_M  ((GPIO_PIN12_INT_TYPE_V)<<(GPIO_PIN12_INT_TYPE_S))
#define GPIO_PIN12_INT_TYPE_V  0x7
#define GPIO_PIN12_INT_TYPE_S  7
/* GPIO_PIN12_SYNC1_BYPASS : R/W ;bitpos:[4:3] ;default: 2'h0 ; */
/*description: */
#define GPIO_PIN12_SYNC1_BYPASS  0x00000003
#define GPIO_PIN12_SYNC1_BYPASS_M  ((GPIO_PIN12_SYNC1_BYPASS_V)<<(GPIO_PIN12_SYNC1_BYPASS_S))
#define GPIO_PIN12_SYNC1_BYPASS_V  0x3
#define GPIO_PIN12_SYNC1_BYPASS_S  3
/* GPIO_PIN12_PAD_DRIVER : R/W ;bitpos:[2] ;default: 1'h0 ; */
/*description: */
#define GPIO_PIN12_PAD_DRIVER  (BIT(2))
#define GPIO_PIN12_PAD_DRIVER_M  (BIT(2))
#define GPIO_PIN12_PAD_DRIVER_V  0x1
#define GPIO_PIN12_PAD_DRIVER_S  2
/* GPIO_PIN12_SYNC2_BYPASS : R/W ;bitpos:[1:0] ;default: 2'h0 ; */
/*description: */
#define GPIO_PIN12_SYNC2_BYPASS  0x00000003
#define GPIO_PIN12_SYNC2_BYPASS_M  ((GPIO_PIN12_SYNC2_BYPASS_V)<<(GPIO_PIN12_SYNC2_BYPASS_S))
#define GPIO_PIN12_SYNC2_BYPASS_V  0x3
#define GPIO_PIN12_SYNC2_BYPASS_S  0

#define GPIO_PIN13_REG          (DR_REG_GPIO_BASE + 0xA8)
/* GPIO_PIN13_INT_ENA : R/W ;bitpos:[17:13] ;default: 5'h0 ; */
/*description: */
#define GPIO_PIN13_INT_ENA  0x0000001F
#define GPIO_PIN13_INT_ENA_M  ((GPIO_PIN13_INT_ENA_V)<<(GPIO_PIN13_INT_ENA_S))
#define GPIO_PIN13_INT_ENA_V  0x1F
#define GPIO_PIN13_INT_ENA_S  13
/* GPIO_PIN13_CONFIG : R/W ;bitpos:[12:11] ;default: 2'h0 ; */
/*description: */
#define GPIO_PIN13_CONFIG  0x00000003
#define GPIO_PIN13_CONFIG_M  ((GPIO_PIN13_CONFIG_V)<<(GPIO_PIN13_CONFIG_S))
#define GPIO_PIN13_CONFIG_V  0x3
#define GPIO_PIN13_CONFIG_S  11
/* GPIO_PIN13_WAKEUP_ENABLE : R/W ;bitpos:[10] ;default: 1'h0 ; */
/*description: */
#define GPIO_PIN13_WAKEUP_ENABLE  (BIT(10))
#define GPIO_PIN13_WAKEUP_ENABLE_M  (BIT(10))
#define GPIO_PIN13_WAKEUP_ENABLE_V  0x1
#define GPIO_PIN13_WAKEUP_ENABLE_S  10
/* GPIO_PIN13_INT_TYPE : R/W ;bitpos:[9:7] ;default: 3'h0 ; */
/*description: */
#define GPIO_PIN13_INT_TYPE  0x00000007
#define GPIO_PIN13_INT_TYPE_M  ((GPIO_PIN13_INT_TYPE_V)<<(GPIO_PIN13_INT_TYPE_S))
#define GPIO_PIN13_INT_TYPE_V  0x7
#define GPIO_PIN13_INT_TYPE_S  7
/* GPIO_PIN13_SYNC1_BYPASS : R/W ;bitpos:[4:3] ;default: 2'h0 ; */
/*description: */
#define GPIO_PIN13_SYNC1_BYPASS  0x00000003
#define GPIO_PIN13_SYNC1_BYPASS_M  ((GPIO_PIN13_SYNC1_BYPASS_V)<<(GPIO_PIN13_SYNC1_BYPASS_S))
#define GPIO_PIN13_SYNC1_BYPASS_V  0x3
#define GPIO_PIN13_SYNC1_BYPASS_S  3
/* GPIO_PIN13_PAD_DRIVER : R/W ;bitpos:[2] ;default: 1'h0 ; */
/*description: */
#define GPIO_PIN13_PAD_DRIVER  (BIT(2))
#define GPIO_PIN13_PAD_DRIVER_M  (BIT(2))
#define GPIO_PIN13_PAD_DRIVER_V  0x1
#define GPIO_PIN13_PAD_DRIVER_S  2
/* GPIO_PIN13_SYNC2_BYPASS : R/W ;bitpos:[1:0] ;default: 2'h0 ; */
/*description: */
#define GPIO_PIN13_SYNC2_BYPASS  0x00000003
#define GPIO_PIN13_SYNC2_BYPASS_M  ((GPIO_PIN13_SYNC2_BYPASS_V)<<(GPIO_PIN13_SYNC2_BYPASS_S))
#define GPIO_PIN13_SYNC2_BYPASS_V  0x3
#define GPIO_PIN13_SYNC2_BYPASS_S  0

#define GPIO_PIN14_REG          (DR_REG_GPIO_BASE + 0xAC)
/* GPIO_PIN14_INT_ENA : R/W ;bitpos:[17:13] ;default: 5'h0 ; */
/*description: */
#define GPIO_PIN14_INT_ENA  0x0000001F
#define GPIO_PIN14_INT_ENA_M  ((GPIO_PIN14_INT_ENA_V)<<(GPIO_PIN14_INT_ENA_S))
#define GPIO_PIN14_INT_ENA_V  0x1F
#define GPIO_PIN14_INT_ENA_S  13
/* GPIO_PIN14_CONFIG : R/W ;bitpos:[12:11] ;default: 2'h0 ; */
/*description: */
#define GPIO_PIN14_CONFIG  0x00000003
#define GPIO_PIN14_CONFIG_M  ((GPIO_PIN14_CONFIG_V)<<(GPIO_PIN14_CONFIG_S))
#define GPIO_PIN14_CONFIG_V  0x3
#define GPIO_PIN14_CONFIG_S  11
/* GPIO_PIN14_WAKEUP_ENABLE : R/W ;bitpos:[10] ;default: 1'h0 ; */
/*description: */
#define GPIO_PIN14_WAKEUP_ENABLE  (BIT(10))
#define GPIO_PIN14_WAKEUP_ENABLE_M  (BIT(10))
#define GPIO_PIN14_WAKEUP_ENABLE_V  0x1
#define GPIO_PIN14_WAKEUP_ENABLE_S  10
/* GPIO_PIN14_INT_TYPE : R/W ;bitpos:[9:7] ;default: 3'h0 ; */
/*description: */
#define GPIO_PIN14_INT_TYPE  0x00000007
#define GPIO_PIN14_INT_TYPE_M  ((GPIO_PIN14_INT_TYPE_V)<<(GPIO_PIN14_INT_TYPE_S))
#define GPIO_PIN14_INT_TYPE_V  0x7
#define GPIO_PIN14_INT_TYPE_S  7
/* GPIO_PIN14_SYNC1_BYPASS : R/W ;bitpos:[4:3] ;default: 2'h0 ; */
/*description: */
#define GPIO_PIN14_SYNC1_BYPASS  0x00000003
#define GPIO_PIN14_SYNC1_BYPASS_M  ((GPIO_PIN14_SYNC1_BYPASS_V)<<(GPIO_PIN14_SYNC1_BYPASS_S))
#define GPIO_PIN14_SYNC1_BYPASS_V  0x3
#define GPIO_PIN14_SYNC1_BYPASS_S  3
/* GPIO_PIN14_PAD_DRIVER : R/W ;bitpos:[2] ;default: 1'h0 ; */
/*description: */
#define GPIO_PIN14_PAD_DRIVER  (BIT(2))
#define GPIO_PIN14_PAD_DRIVER_M  (BIT(2))
#define GPIO_PIN14_PAD_DRIVER_V  0x1
#define GPIO_PIN14_PAD_DRIVER_S  2
/* GPIO_PIN14_SYNC2_BYPASS : R/W ;bitpos:[1:0] ;default: 2'h0 ; */
/*description: */
#define GPIO_PIN14_SYNC2_BYPASS  0x00000003
#define GPIO_PIN14_SYNC2_BYPASS_M  ((GPIO_PIN14_SYNC2_BYPASS_V)<<(GPIO_PIN14_SYNC2_BYPASS_S))
#define GPIO_PIN14_SYNC2_BYPASS_V  0x3
#define GPIO_PIN14_SYNC2_BYPASS_S  0

#define GPIO_PIN15_REG          (DR_REG_GPIO_BASE + 0xB0)
/* GPIO_PIN15_INT_ENA : R/W ;bitpos:[17:13] ;default: 5'h0 ; */
/*description: */
#define GPIO_PIN15_INT_ENA  0x0000001F
#define GPIO_PIN15_INT_ENA_M  ((GPIO_PIN15_INT_ENA_V)<<(GPIO_PIN15_INT_ENA_S))
#define GPIO_PIN15_INT_ENA_V  0x1F
#define GPIO_PIN15_INT_ENA_S  13
/* GPIO_PIN15_CONFIG : R/W ;bitpos:[12:11] ;default: 2'h0 ; */
/*description: */
#define GPIO_PIN15_CONFIG  0x00000003
#define GPIO_PIN15_CONFIG_M  ((GPIO_PIN15_CONFIG_V)<<(GPIO_PIN15_CONFIG_S))
#define GPIO_PIN15_CONFIG_V  0x3
#define GPIO_PIN15_CONFIG_S  11
/* GPIO_PIN15_WAKEUP_ENABLE : R/W ;bitpos:[10] ;default: 1'h0 ; */
/*description: */
#define GPIO_PIN15_WAKEUP_ENABLE  (BIT(10))
#define GPIO_PIN15_WAKEUP_ENABLE_M  (BIT(10))
#define GPIO_PIN15_WAKEUP_ENABLE_V  0x1
#define GPIO_PIN15_WAKEUP_ENABLE_S  10
/* GPIO_PIN15_INT_TYPE : R/W ;bitpos:[9:7] ;default: 3'h0 ; */
/*description: */
#define GPIO_PIN15_INT_TYPE  0x00000007
#define GPIO_PIN15_INT_TYPE_M  ((GPIO_PIN15_INT_TYPE_V)<<(GPIO_PIN15_INT_TYPE_S))
#define GPIO_PIN15_INT_TYPE_V  0x7
#define GPIO_PIN15_INT_TYPE_S  7
/* GPIO_PIN15_SYNC1_BYPASS : R/W ;bitpos:[4:3] ;default: 2'h0 ; */
/*description: */
#define GPIO_PIN15_SYNC1_BYPASS  0x00000003
#define GPIO_PIN15_SYNC1_BYPASS_M  ((GPIO_PIN15_SYNC1_BYPASS_V)<<(GPIO_PIN15_SYNC1_BYPASS_S))
#define GPIO_PIN15_SYNC1_BYPASS_V  0x3
#define GPIO_PIN15_SYNC1_BYPASS_S  3
/* GPIO_PIN15_PAD_DRIVER : R/W ;bitpos:[2] ;default: 1'h0 ; */
/*description: */
#define GPIO_PIN15_PAD_DRIVER  (BIT(2))
#define GPIO_PIN15_PAD_DRIVER_M  (BIT(2))
#define GPIO_PIN15_PAD_DRIVER_V  0x1
#define GPIO_PIN15_PAD_DRIVER_S  2
/* GPIO_PIN15_SYNC2_BYPASS : R/W ;bitpos:[1:0] ;default: 2'h0 ; */
/*description: */
#define GPIO_PIN15_SYNC2_BYPASS  0x00000003
#define GPIO_PIN15_SYNC2_BYPASS_M  ((GPIO_PIN15_SYNC2_BYPASS_V)<<(GPIO_PIN15_SYNC2_BYPASS_S))
#define GPIO_PIN15_SYNC2_BYPASS_V  0x3
#define GPIO_PIN15_SYNC2_BYPASS_S  0

#define GPIO_PIN16_REG          (DR_REG_GPIO_BASE + 0xB4)
/* GPIO_PIN16_INT_ENA : R/W ;bitpos:[17:13] ;default: 5'h0 ; */
/*description: */
#define GPIO_PIN16_INT_ENA  0x0000001F
#define GPIO_PIN16_INT_ENA_M  ((GPIO_PIN16_INT_ENA_V)<<(GPIO_PIN16_INT_ENA_S))
#define GPIO_PIN16_INT_ENA_V  0x1F
#define GPIO_PIN16_INT_ENA_S  13
/* GPIO_PIN16_CONFIG : R/W ;bitpos:[12:11] ;default: 2'h0 ; */
/*description: */
#define GPIO_PIN16_CONFIG  0x00000003
#define GPIO_PIN16_CONFIG_M  ((GPIO_PIN16_CONFIG_V)<<(GPIO_PIN16_CONFIG_S))
#define GPIO_PIN16_CONFIG_V  0x3
#define GPIO_PIN16_CONFIG_S  11
/* GPIO_PIN16_WAKEUP_ENABLE : R/W ;bitpos:[10] ;default: 1'h0 ; */
/*description: */
#define GPIO_PIN16_WAKEUP_ENABLE  (BIT(10))
#define GPIO_PIN16_WAKEUP_ENABLE_M  (BIT(10))
#define GPIO_PIN16_WAKEUP_ENABLE_V  0x1
#define GPIO_PIN16_WAKEUP_ENABLE_S  10
/* GPIO_PIN16_INT_TYPE : R/W ;bitpos:[9:7] ;default: 3'h0 ; */
/*description: */
#define GPIO_PIN16_INT_TYPE  0x00000007
#define GPIO_PIN16_INT_TYPE_M  ((GPIO_PIN16_INT_TYPE_V)<<(GPIO_PIN16_INT_TYPE_S))
#define GPIO_PIN16_INT_TYPE_V  0x7
#define GPIO_PIN16_INT_TYPE_S  7
/* GPIO_PIN16_SYNC1_BYPASS : R/W ;bitpos:[4:3] ;default: 2'h0 ; */
/*description: */
#define GPIO_PIN16_SYNC1_BYPASS  0x00000003
#define GPIO_PIN16_SYNC1_BYPASS_M  ((GPIO_PIN16_SYNC1_BYPASS_V)<<(GPIO_PIN16_SYNC1_BYPASS_S))
#define GPIO_PIN16_SYNC1_BYPASS_V  0x3
#define GPIO_PIN16_SYNC1_BYPASS_S  3
/* GPIO_PIN16_PAD_DRIVER : R/W ;bitpos:[2] ;default: 1'h0 ; */
/*description: */
#define GPIO_PIN16_PAD_DRIVER  (BIT(2))
#define GPIO_PIN16_PAD_DRIVER_M  (BIT(2))
#define GPIO_PIN16_PAD_DRIVER_V  0x1
#define GPIO_PIN16_PAD_DRIVER_S  2
/* GPIO_PIN16_SYNC2_BYPASS : R/W ;bitpos:[1:0] ;default: 2'h0 ; */
/*description: */
#define GPIO_PIN16_SYNC2_BYPASS  0x00000003
#define GPIO_PIN16_SYNC2_BYPASS_M  ((GPIO_PIN16_SYNC2_BYPASS_V)<<(GPIO_PIN16_SYNC2_BYPASS_S))
#define GPIO_PIN16_SYNC2_BYPASS_V  0x3
#define GPIO_PIN16_SYNC2_BYPASS_S  0

#define GPIO_PIN17_REG          (DR_REG_GPIO_BASE + 0xB8)
/* GPIO_PIN17_INT_ENA : R/W ;bitpos:[17:13] ;default: 5'h0 ; */
/*description: */
#define GPIO_PIN17_INT_ENA  0x0000001F
#define GPIO_PIN17_INT_ENA_M  ((GPIO_PIN17_INT_ENA_V)<<(GPIO_PIN17_INT_ENA_S))
#define GPIO_PIN17_INT_ENA_V  0x1F
#define GPIO_PIN17_INT_ENA_S  13
/* GPIO_PIN17_CONFIG : R/W ;bitpos:[12:11] ;default: 2'h0 ; */
/*description: */
#define GPIO_PIN17_CONFIG  0x00000003
#define GPIO_PIN17_CONFIG_M  ((GPIO_PIN17_CONFIG_V)<<(GPIO_PIN17_CONFIG_S))
#define GPIO_PIN17_CONFIG_V  0x3
#define GPIO_PIN17_CONFIG_S  11
/* GPIO_PIN17_WAKEUP_ENABLE : R/W ;bitpos:[10] ;default: 1'h0 ; */
/*description: */
#define GPIO_PIN17_WAKEUP_ENABLE  (BIT(10))
#define GPIO_PIN17_WAKEUP_ENABLE_M  (BIT(10))
#define GPIO_PIN17_WAKEUP_ENABLE_V  0x1
#define GPIO_PIN17_WAKEUP_ENABLE_S  10
/* GPIO_PIN17_INT_TYPE : R/W ;bitpos:[9:7] ;default: 3'h0 ; */
/*description: */
#define GPIO_PIN17_INT_TYPE  0x00000007
#define GPIO_PIN17_INT_TYPE_M  ((GPIO_PIN17_INT_TYPE_V)<<(GPIO_PIN17_INT_TYPE_S))
#define GPIO_PIN17_INT_TYPE_V  0x7
#define GPIO_PIN17_INT_TYPE_S  7
/* GPIO_PIN17_SYNC1_BYPASS : R/W ;bitpos:[4:3] ;default: 2'h0 ; */
/*description: */
#define GPIO_PIN17_SYNC1_BYPASS  0x00000003
#define GPIO_PIN17_SYNC1_BYPASS_M  ((GPIO_PIN17_SYNC1_BYPASS_V)<<(GPIO_PIN17_SYNC1_BYPASS_S))
#define GPIO_PIN17_SYNC1_BYPASS_V  0x3
#define GPIO_PIN17_SYNC1_BYPASS_S  3
/* GPIO_PIN17_PAD_DRIVER : R/W ;bitpos:[2] ;default: 1'h0 ; */
/*description: */
#define GPIO_PIN17_PAD_DRIVER  (BIT(2))
#define GPIO_PIN17_PAD_DRIVER_M  (BIT(2))
#define GPIO_PIN17_PAD_DRIVER_V  0x1
#define GPIO_PIN17_PAD_DRIVER_S  2
/* GPIO_PIN17_SYNC2_BYPASS : R/W ;bitpos:[1:0] ;default: 2'h0 ; */
/*description: */
#define GPIO_PIN17_SYNC2_BYPASS  0x00000003
#define GPIO_PIN17_SYNC2_BYPASS_M  ((GPIO_PIN17_SYNC2_BYPASS_V)<<(GPIO_PIN17_SYNC2_BYPASS_S))
#define GPIO_PIN17_SYNC2_BYPASS_V  0x3
#define GPIO_PIN17_SYNC2_BYPASS_S  0

#define GPIO_PIN18_REG          (DR_REG_GPIO_BASE + 0xBC)
/* GPIO_PIN18_INT_ENA : R/W ;bitpos:[17:13] ;default: 5'h0 ; */
/*description: */
#define GPIO_PIN18_INT_ENA  0x0000001F
#define GPIO_PIN18_INT_ENA_M  ((GPIO_PIN18_INT_ENA_V)<<(GPIO_PIN18_INT_ENA_S))
#define GPIO_PIN18_INT_ENA_V  0x1F
#define GPIO_PIN18_INT_ENA_S  13
/* GPIO_PIN18_CONFIG : R/W ;bitpos:[12:11] ;default: 2'h0 ; */
/*description: */
#define GPIO_PIN18_CONFIG  0x00000003
#define GPIO_PIN18_CONFIG_M  ((GPIO_PIN18_CONFIG_V)<<(GPIO_PIN18_CONFIG_S))
#define GPIO_PIN18_CONFIG_V  0x3
#define GPIO_PIN18_CONFIG_S  11
/* GPIO_PIN18_WAKEUP_ENABLE : R/W ;bitpos:[10] ;default: 1'h0 ; */
/*description: */
#define GPIO_PIN18_WAKEUP_ENABLE  (BIT(10))
#define GPIO_PIN18_WAKEUP_ENABLE_M  (BIT(10))
#define GPIO_PIN18_WAKEUP_ENABLE_V  0x1
#define GPIO_PIN18_WAKEUP_ENABLE_S  10
/* GPIO_PIN18_INT_TYPE : R/W ;bitpos:[9:7] ;default: 3'h0 ; */
/*description: */
#define GPIO_PIN18_INT_TYPE  0x00000007
#define GPIO_PIN18_INT_TYPE_M  ((GPIO_PIN18_INT_TYPE_V)<<(GPIO_PIN18_INT_TYPE_S))
#define GPIO_PIN18_INT_TYPE_V  0x7
#define GPIO_PIN18_INT_TYPE_S  7
/* GPIO_PIN18_SYNC1_BYPASS : R/W ;bitpos:[4:3] ;default: 2'h0 ; */
/*description: */
#define GPIO_PIN18_SYNC1_BYPASS  0x00000003
#define GPIO_PIN18_SYNC1_BYPASS_M  ((GPIO_PIN18_SYNC1_BYPASS_V)<<(GPIO_PIN18_SYNC1_BYPASS_S))
#define GPIO_PIN18_SYNC1_BYPASS_V  0x3
#define GPIO_PIN18_SYNC1_BYPASS_S  3
/* GPIO_PIN18_PAD_DRIVER : R/W ;bitpos:[2] ;default: 1'h0 ; */
/*description: */
#define GPIO_PIN18_PAD_DRIVER  (BIT(2))
#define GPIO_PIN18_PAD_DRIVER_M  (BIT(2))
#define GPIO_PIN18_PAD_DRIVER_V  0x1
#define GPIO_PIN18_PAD_DRIVER_S  2
/* GPIO_PIN18_SYNC2_BYPASS : R/W ;bitpos:[1:0] ;default: 2'h0 ; */
/*description: */
#define GPIO_PIN18_SYNC2_BYPASS  0x00000003
#define GPIO_PIN18_SYNC2_BYPASS_M  ((GPIO_PIN18_SYNC2_BYPASS_V)<<(GPIO_PIN18_SYNC2_BYPASS_S))
#define GPIO_PIN18_SYNC2_BYPASS_V  0x3
#define GPIO_PIN18_SYNC2_BYPASS_S  0

#define GPIO_PIN19_REG          (DR_REG_GPIO_BASE + 0xC0)
/* GPIO_PIN19_INT_ENA : R/W ;bitpos:[17:13] ;default: 5'h0 ; */
/*description: */
#define GPIO_PIN19_INT_ENA  0x0000001F
#define GPIO_PIN19_INT_ENA_M  ((GPIO_PIN19_INT_ENA_V)<<(GPIO_PIN19_INT_ENA_S))
#define GPIO_PIN19_INT_ENA_V  0x1F
#define GPIO_PIN19_INT_ENA_S  13
/* GPIO_PIN19_CONFIG : R/W ;bitpos:[12:11] ;default: 2'h0 ; */
/*description: */
#define GPIO_PIN19_CONFIG  0x00000003
#define GPIO_PIN19_CONFIG_M  ((GPIO_PIN19_CONFIG_V)<<(GPIO_PIN19_CONFIG_S))
#define GPIO_PIN19_CONFIG_V  0x3
#define GPIO_PIN19_CONFIG_S  11
/* GPIO_PIN19_WAKEUP_ENABLE : R/W ;bitpos:[10] ;default: 1'h0 ; */
/*description: */
#define GPIO_PIN19_WAKEUP_ENABLE  (BIT(10))
#define GPIO_PIN19_WAKEUP_ENABLE_M  (BIT(10))
#define GPIO_PIN19_WAKEUP_ENABLE_V  0x1
#define GPIO_PIN19_WAKEUP_ENABLE_S  10
/* GPIO_PIN19_INT_TYPE : R/W ;bitpos:[9:7] ;default: 3'h0 ; */
/*description: */
#define GPIO_PIN19_INT_TYPE  0x00000007
#define GPIO_PIN19_INT_TYPE_M  ((GPIO_PIN19_INT_TYPE_V)<<(GPIO_PIN19_INT_TYPE_S))
#define GPIO_PIN19_INT_TYPE_V  0x7
#define GPIO_PIN19_INT_TYPE_S  7
/* GPIO_PIN19_SYNC1_BYPASS : R/W ;bitpos:[4:3] ;default: 2'h0 ; */
/*description: */
#define GPIO_PIN19_SYNC1_BYPASS  0x00000003
#define GPIO_PIN19_SYNC1_BYPASS_M  ((GPIO_PIN19_SYNC1_BYPASS_V)<<(GPIO_PIN19_SYNC1_BYPASS_S))
#define GPIO_PIN19_SYNC1_BYPASS_V  0x3
#define GPIO_PIN19_SYNC1_BYPASS_S  3
/* GPIO_PIN19_PAD_DRIVER : R/W ;bitpos:[2] ;default: 1'h0 ; */
/*description: */
#define GPIO_PIN19_PAD_DRIVER  (BIT(2))
#define GPIO_PIN19_PAD_DRIVER_M  (BIT(2))
#define GPIO_PIN19_PAD_DRIVER_V  0x1
#define GPIO_PIN19_PAD_DRIVER_S  2
/* GPIO_PIN19_SYNC2_BYPASS : R/W ;bitpos:[1:0] ;default: 2'h0 ; */
/*description: */
#define GPIO_PIN19_SYNC2_BYPASS  0x00000003
#define GPIO_PIN19_SYNC2_BYPASS_M  ((GPIO_PIN19_SYNC2_BYPASS_V)<<(GPIO_PIN19_SYNC2_BYPASS_S))
#define GPIO_PIN19_SYNC2_BYPASS_V  0x3
#define GPIO_PIN19_SYNC2_BYPASS_S  0

#define GPIO_PIN20_REG          (DR_REG_GPIO_BASE + 0xC4)
/* GPIO_PIN20_INT_ENA : R/W ;bitpos:[17:13] ;default: 5'h0 ; */
/*description: */
#define GPIO_PIN20_INT_ENA  0x0000001F
#define GPIO_PIN20_INT_ENA_M  ((GPIO_PIN20_INT_ENA_V)<<(GPIO_PIN20_INT_ENA_S))
#define GPIO_PIN20_INT_ENA_V  0x1F
#define GPIO_PIN20_INT_ENA_S  13
/* GPIO_PIN20_CONFIG : R/W ;bitpos:[12:11] ;default: 2'h0 ; */
/*description: */
#define GPIO_PIN20_CONFIG  0x00000003
#define GPIO_PIN20_CONFIG_M  ((GPIO_PIN20_CONFIG_V)<<(GPIO_PIN20_CONFIG_S))
#define GPIO_PIN20_CONFIG_V  0x3
#define GPIO_PIN20_CONFIG_S  11
/* GPIO_PIN20_WAKEUP_ENABLE : R/W ;bitpos:[10] ;default: 1'h0 ; */
/*description: */
#define GPIO_PIN20_WAKEUP_ENABLE  (BIT(10))
#define GPIO_PIN20_WAKEUP_ENABLE_M  (BIT(10))
#define GPIO_PIN20_WAKEUP_ENABLE_V  0x1
#define GPIO_PIN20_WAKEUP_ENABLE_S  10
/* GPIO_PIN20_INT_TYPE : R/W ;bitpos:[9:7] ;default: 3'h0 ; */
/*description: */
#define GPIO_PIN20_INT_TYPE  0x00000007
#define GPIO_PIN20_INT_TYPE_M  ((GPIO_PIN20_INT_TYPE_V)<<(GPIO_PIN20_INT_TYPE_S))
#define GPIO_PIN20_INT_TYPE_V  0x7
#define GPIO_PIN20_INT_TYPE_S  7
/* GPIO_PIN20_SYNC1_BYPASS : R/W ;bitpos:[4:3] ;default: 2'h0 ; */
/*description: */
#define GPIO_PIN20_SYNC1_BYPASS  0x00000003
#define GPIO_PIN20_SYNC1_BYPASS_M  ((GPIO_PIN20_SYNC1_BYPASS_V)<<(GPIO_PIN20_SYNC1_BYPASS_S))
#define GPIO_PIN20_SYNC1_BYPASS_V  0x3
#define GPIO_PIN20_SYNC1_BYPASS_S  3
/* GPIO_PIN20_PAD_DRIVER : R/W ;bitpos:[2] ;default: 1'h0 ; */
/*description: */
#define GPIO_PIN20_PAD_DRIVER  (BIT(2))
#define GPIO_PIN20_PAD_DRIVER_M  (BIT(2))
#define GPIO_PIN20_PAD_DRIVER_V  0x1
#define GPIO_PIN20_PAD_DRIVER_S  2
/* GPIO_PIN20_SYNC2_BYPASS : R/W ;bitpos:[1:0] ;default: 2'h0 ; */
/*description: */
#define GPIO_PIN20_SYNC2_BYPASS  0x00000003
#define GPIO_PIN20_SYNC2_BYPASS_M  ((GPIO_PIN20_SYNC2_BYPASS_V)<<(GPIO_PIN20_SYNC2_BYPASS_S))
#define GPIO_PIN20_SYNC2_BYPASS_V  0x3
#define GPIO_PIN20_SYNC2_BYPASS_S  0

#define GPIO_PIN21_REG          (DR_REG_GPIO_BASE + 0xC8)
/* GPIO_PIN21_INT_ENA : R/W ;bitpos:[17:13] ;default: 5'h0 ; */
/*description: */
#define GPIO_PIN21_INT_ENA  0x0000001F
#define GPIO_PIN21_INT_ENA_M  ((GPIO_PIN21_INT_ENA_V)<<(GPIO_PIN21_INT_ENA_S))
#define GPIO_PIN21_INT_ENA_V  0x1F
#define GPIO_PIN21_INT_ENA_S  13
/* GPIO_PIN21_CONFIG : R/W ;bitpos:[12:11] ;default: 2'h0 ; */
/*description: */
#define GPIO_PIN21_CONFIG  0x00000003
#define GPIO_PIN21_CONFIG_M  ((GPIO_PIN21_CONFIG_V)<<(GPIO_PIN21_CONFIG_S))
#define GPIO_PIN21_CONFIG_V  0x3
#define GPIO_PIN21_CONFIG_S  11
/* GPIO_PIN21_WAKEUP_ENABLE : R/W ;bitpos:[10] ;default: 1'h0 ; */
/*description: */
#define GPIO_PIN21_WAKEUP_ENABLE  (BIT(10))
#define GPIO_PIN21_WAKEUP_ENABLE_M  (BIT(10))
#define GPIO_PIN21_WAKEUP_ENABLE_V  0x1
#define GPIO_PIN21_WAKEUP_ENABLE_S  10
/* GPIO_PIN21_INT_TYPE : R/W ;bitpos:[9:7] ;default: 3'h0 ; */
/*description: */
#define GPIO_PIN21_INT_TYPE  0x00000007
#define GPIO_PIN21_INT_TYPE_M  ((GPIO_PIN21_INT_TYPE_V)<<(GPIO_PIN21_INT_TYPE_S))
#define GPIO_PIN21_INT_TYPE_V  0x7
#define GPIO_PIN21_INT_TYPE_S  7
/* GPIO_PIN21_SYNC1_BYPASS : R/W ;bitpos:[4:3] ;default: 2'h0 ; */
/*description: */
#define GPIO_PIN21_SYNC1_BYPASS  0x00000003
#define GPIO_PIN21_SYNC1_BYPASS_M  ((GPIO_PIN21_SYNC1_BYPASS_V)<<(GPIO_PIN21_SYNC1_BYPASS_S))
#define GPIO_PIN21_SYNC1_BYPASS_V  0x3
#define GPIO_PIN21_SYNC1_BYPASS_S  3
/* GPIO_PIN21_PAD_DRIVER : R/W ;bitpos:[2] ;default: 1'h0 ; */
/*description: */
#define GPIO_PIN21_PAD_DRIVER  (BIT(2))
#define GPIO_PIN21_PAD_DRIVER_M  (BIT(2))
#define GPIO_PIN21_PAD_DRIVER_V  0x1
#define GPIO_PIN21_PAD_DRIVER_S  2
/* GPIO_PIN21_SYNC2_BYPASS : R/W ;bitpos:[1:0] ;default: 2'h0 ; */
/*description: */
#define GPIO_PIN21_SYNC2_BYPASS  0x00000003
#define GPIO_PIN21_SYNC2_BYPASS_M  ((GPIO_PIN21_SYNC2_BYPASS_V)<<(GPIO_PIN21_SYNC2_BYPASS_S))
#define GPIO_PIN21_SYNC2_BYPASS_V  0x3
#define GPIO_PIN21_SYNC2_BYPASS_S  0

#define GPIO_PIN22_REG          (DR_REG_GPIO_BASE + 0xCC)
/* GPIO_PIN22_INT_ENA : R/W ;bitpos:[17:13] ;default: 5'h0 ; */
/*description: */
#define GPIO_PIN22_INT_ENA  0x0000001F
#define GPIO_PIN22_INT_ENA_M  ((GPIO_PIN22_INT_ENA_V)<<(GPIO_PIN22_INT_ENA_S))
#define GPIO_PIN22_INT_ENA_V  0x1F
#define GPIO_PIN22_INT_ENA_S  13
/* GPIO_PIN22_CONFIG : R/W ;bitpos:[12:11] ;default: 2'h0 ; */
/*description: */
#define GPIO_PIN22_CONFIG  0x00000003
#define GPIO_PIN22_CONFIG_M  ((GPIO_PIN22_CONFIG_V)<<(GPIO_PIN22_CONFIG_S))
#define GPIO_PIN22_CONFIG_V  0x3
#define GPIO_PIN22_CONFIG_S  11
/* GPIO_PIN22_WAKEUP_ENABLE : R/W ;bitpos:[10] ;default: 1'h0 ; */
/*description: */
#define GPIO_PIN22_WAKEUP_ENABLE  (BIT(10))
#define GPIO_PIN22_WAKEUP_ENABLE_M  (BIT(10))
#define GPIO_PIN22_WAKEUP_ENABLE_V  0x1
#define GPIO_PIN22_WAKEUP_ENABLE_S  10
/* GPIO_PIN22_INT_TYPE : R/W ;bitpos:[9:7] ;default: 3'h0 ; */
/*description: */
#define GPIO_PIN22_INT_TYPE  0x00000007
#define GPIO_PIN22_INT_TYPE_M  ((GPIO_PIN22_INT_TYPE_V)<<(GPIO_PIN22_INT_TYPE_S))
#define GPIO_PIN22_INT_TYPE_V  0x7
#define GPIO_PIN22_INT_TYPE_S  7
/* GPIO_PIN22_SYNC1_BYPASS : R/W ;bitpos:[4:3] ;default: 2'h0 ; */
/*description: */
#define GPIO_PIN22_SYNC1_BYPASS  0x00000003
#define GPIO_PIN22_SYNC1_BYPASS_M  ((GPIO_PIN22_SYNC1_BYPASS_V)<<(GPIO_PIN22_SYNC1_BYPASS_S))
#define GPIO_PIN22_SYNC1_BYPASS_V  0x3
#define GPIO_PIN22_SYNC1_BYPASS_S  3
/* GPIO_PIN22_PAD_DRIVER : R/W ;bitpos:[2] ;default: 1'h0 ; */
/*description: */
#define GPIO_PIN22_PAD_DRIVER  (BIT(2))
#define GPIO_PIN22_PAD_DRIVER_M  (BIT(2))
#define GPIO_PIN22_PAD_DRIVER_V  0x1
#define GPIO_PIN22_PAD_DRIVER_S  2
/* GPIO_PIN22_SYNC2_BYPASS : R/W ;bitpos:[1:0] ;default: 2'h0 ; */
/*description: */
#define GPIO_PIN22_SYNC2_BYPASS  0x00000003
#define GPIO_PIN22_SYNC2_BYPASS_M  ((GPIO_PIN22_SYNC2_BYPASS_V)<<(GPIO_PIN22_SYNC2_BYPASS_S))
#define GPIO_PIN22_SYNC2_BYPASS_V  0x3
#define GPIO_PIN22_SYNC2_BYPASS_S  0

#define GPIO_PIN23_REG          (DR_REG_GPIO_BASE + 0xD0)
/* GPIO_PIN23_INT_ENA : R/W ;bitpos:[17:13] ;default: 5'h0 ; */
/*description: */
#define GPIO_PIN23_INT_ENA  0x0000001F
#define GPIO_PIN23_INT_ENA_M  ((GPIO_PIN23_INT_ENA_V)<<(GPIO_PIN23_INT_ENA_S))
#define GPIO_PIN23_INT_ENA_V  0x1F
#define GPIO_PIN23_INT_ENA_S  13
/* GPIO_PIN23_CONFIG : R/W ;bitpos:[12:11] ;default: 2'h0 ; */
/*description: */
#define GPIO_PIN23_CONFIG  0x00000003
#define GPIO_PIN23_CONFIG_M  ((GPIO_PIN23_CONFIG_V)<<(GPIO_PIN23_CONFIG_S))
#define GPIO_PIN23_CONFIG_V  0x3
#define GPIO_PIN23_CONFIG_S  11
/* GPIO_PIN23_WAKEUP_ENABLE : R/W ;bitpos:[10] ;default: 1'h0 ; */
/*description: */
#define GPIO_PIN23_WAKEUP_ENABLE  (BIT(10))
#define GPIO_PIN23_WAKEUP_ENABLE_M  (BIT(10))
#define GPIO_PIN23_WAKEUP_ENABLE_V  0x1
#define GPIO_PIN23_WAKEUP_ENABLE_S  10
/* GPIO_PIN23_INT_TYPE : R/W ;bitpos:[9:7] ;default: 3'h0 ; */
/*description: */
#define GPIO_PIN23_INT_TYPE  0x00000007
#define GPIO_PIN23_INT_TYPE_M  ((GPIO_PIN23_INT_TYPE_V)<<(GPIO_PIN23_INT_TYPE_S))
#define GPIO_PIN23_INT_TYPE_V  0x7
#define GPIO_PIN23_INT_TYPE_S  7
/* GPIO_PIN23_SYNC1_BYPASS : R/W ;bitpos:[4:3] ;default: 2'h0 ; */
/*description: */
#define GPIO_PIN23_SYNC1_BYPASS  0x00000003
#define GPIO_PIN23_SYNC1_BYPASS_M  ((GPIO_PIN23_SYNC1_BYPASS_V)<<(GPIO_PIN23_SYNC1_BYPASS_S))
#define GPIO_PIN23_SYNC1_BYPASS_V  0x3
#define GPIO_PIN23_SYNC1_BYPASS_S  3
/* GPIO_PIN23_PAD_DRIVER : R/W ;bitpos:[2] ;default: 1'h0 ; */
/*description: */
#define GPIO_PIN23_PAD_DRIVER  (BIT(2))
#define GPIO_PIN23_PAD_DRIVER_M  (BIT(2))
#define GPIO_PIN23_PAD_DRIVER_V  0x1
#define GPIO_PIN23_PAD_DRIVER_S  2
/* GPIO_PIN23_SYNC2_BYPASS : R/W ;bitpos:[1:0] ;default: 2'h0 ; */
/*description: */
#define GPIO_PIN23_SYNC2_BYPASS  0x00000003
#define GPIO_PIN23_SYNC2_BYPASS_M  ((GPIO_PIN23_SYNC2_BYPASS_V)<<(GPIO_PIN23_SYNC2_BYPASS_S))
#define GPIO_PIN23_SYNC2_BYPASS_V  0x3
#define GPIO_PIN23_SYNC2_BYPASS_S  0

#define GPIO_PIN24_REG          (DR_REG_GPIO_BASE + 0xD4)
/* GPIO_PIN24_INT_ENA : R/W ;bitpos:[17:13] ;default: 5'h0 ; */
/*description: */
#define GPIO_PIN24_INT_ENA  0x0000001F
#define GPIO_PIN24_INT_ENA_M  ((GPIO_PIN24_INT_ENA_V)<<(GPIO_PIN24_INT_ENA_S))
#define GPIO_PIN24_INT_ENA_V  0x1F
#define GPIO_PIN24_INT_ENA_S  13
/* GPIO_PIN24_CONFIG : R/W ;bitpos:[12:11] ;default: 2'h0 ; */
/*description: */
#define GPIO_PIN24_CONFIG  0x00000003
#define GPIO_PIN24_CONFIG_M  ((GPIO_PIN24_CONFIG_V)<<(GPIO_PIN24_CONFIG_S))
#define GPIO_PIN24_CONFIG_V  0x3
#define GPIO_PIN24_CONFIG_S  11
/* GPIO_PIN24_WAKEUP_ENABLE : R/W ;bitpos:[10] ;default: 1'h0 ; */
/*description: */
#define GPIO_PIN24_WAKEUP_ENABLE  (BIT(10))
#define GPIO_PIN24_WAKEUP_ENABLE_M  (BIT(10))
#define GPIO_PIN24_WAKEUP_ENABLE_V  0x1
#define GPIO_PIN24_WAKEUP_ENABLE_S  10
/* GPIO_PIN24_INT_TYPE : R/W ;bitpos:[9:7] ;default: 3'h0 ; */
/*description: */
#define GPIO_PIN24_INT_TYPE  0x00000007
#define GPIO_PIN24_INT_TYPE_M  ((GPIO_PIN24_INT_TYPE_V)<<(GPIO_PIN24_INT_TYPE_S))
#define GPIO_PIN24_INT_TYPE_V  0x7
#define GPIO_PIN24_INT_TYPE_S  7
/* GPIO_PIN24_SYNC1_BYPASS : R/W ;bitpos:[4:3] ;default: 2'h0 ; */
/*description: */
#define GPIO_PIN24_SYNC1_BYPASS  0x00000003
#define GPIO_PIN24_SYNC1_BYPASS_M  ((GPIO_PIN24_SYNC1_BYPASS_V)<<(GPIO_PIN24_SYNC1_BYPASS_S))
#define GPIO_PIN24_SYNC1_BYPASS_V  0x3
#define GPIO_PIN24_SYNC1_BYPASS_S  3
/* GPIO_PIN24_PAD_DRIVER : R/W ;bitpos:[2] ;default: 1'h0 ; */
/*description: */
#define GPIO_PIN24_PAD_DRIVER  (BIT(2))
#define GPIO_PIN24_PAD_DRIVER_M  (BIT(2))
#define GPIO_PIN24_PAD_DRIVER_V  0x1
#define GPIO_PIN24_PAD_DRIVER_S  2
/* GPIO_PIN24_SYNC2_BYPASS : R/W ;bitpos:[1:0] ;default: 2'h0 ; */
/*description: */
#define GPIO_PIN24_SYNC2_BYPASS  0x00000003
#define GPIO_PIN24_SYNC2_BYPASS_M  ((GPIO_PIN24_SYNC2_BYPASS_V)<<(GPIO_PIN24_SYNC2_BYPASS_S))
#define GPIO_PIN24_SYNC2_BYPASS_V  0x3
#define GPIO_PIN24_SYNC2_BYPASS_S  0

#define GPIO_PIN25_REG          (DR_REG_GPIO_BASE + 0xD8)
/* GPIO_PIN25_INT_ENA : R/W ;bitpos:[17:13] ;default: 5'h0 ; */
/*description: */
#define GPIO_PIN25_INT_ENA  0x0000001F
#define GPIO_PIN25_INT_ENA_M  ((GPIO_PIN25_INT_ENA_V)<<(GPIO_PIN25_INT_ENA_S))
#define GPIO_PIN25_INT_ENA_V  0x1F
#define GPIO_PIN25_INT_ENA_S  13
/* GPIO_PIN25_CONFIG : R/W ;bitpos:[12:11] ;default: 2'h0 ; */
/*description: */
#define GPIO_PIN25_CONFIG  0x00000003
#define GPIO_PIN25_CONFIG_M  ((GPIO_PIN25_CONFIG_V)<<(GPIO_PIN25_CONFIG_S))
#define GPIO_PIN25_CONFIG_V  0x3
#define GPIO_PIN25_CONFIG_S  11
/* GPIO_PIN25_WAKEUP_ENABLE : R/W ;bitpos:[10] ;default: 1'h0 ; */
/*description: */
#define GPIO_PIN25_WAKEUP_ENABLE  (BIT(10))
#define GPIO_PIN25_WAKEUP_ENABLE_M  (BIT(10))
#define GPIO_PIN25_WAKEUP_ENABLE_V  0x1
#define GPIO_PIN25_WAKEUP_ENABLE_S  10
/* GPIO_PIN25_INT_TYPE : R/W ;bitpos:[9:7] ;default: 3'h0 ; */
/*description: */
#define GPIO_PIN25_INT_TYPE  0x00000007
#define GPIO_PIN25_INT_TYPE_M  ((GPIO_PIN25_INT_TYPE_V)<<(GPIO_PIN25_INT_TYPE_S))
#define GPIO_PIN25_INT_TYPE_V  0x7
#define GPIO_PIN25_INT_TYPE_S  7
/* GPIO_PIN25_SYNC1_BYPASS : R/W ;bitpos:[4:3] ;default: 2'h0 ; */
/*description: */
#define GPIO_PIN25_SYNC1_BYPASS  0x00000003
#define GPIO_PIN25_SYNC1_BYPASS_M  ((GPIO_PIN25_SYNC1_BYPASS_V)<<(GPIO_PIN25_SYNC1_BYPASS_S))
#define GPIO_PIN25_SYNC1_BYPASS_V  0x3
#define GPIO_PIN25_SYNC1_BYPASS_S  3
/* GPIO_PIN25_PAD_DRIVER : R/W ;bitpos:[2] ;default: 1'h0 ; */
/*description: */
#define GPIO_PIN25_PAD_DRIVER  (BIT(2))
#define GPIO_PIN25_PAD_DRIVER_M  (BIT(2))
#define GPIO_PIN25_PAD_DRIVER_V  0x1
#define GPIO_PIN25_PAD_DRIVER_S  2
/* GPIO_PIN25_SYNC2_BYPASS : R/W ;bitpos:[1:0] ;default: 2'h0 ; */
/*description: */
#define GPIO_PIN25_SYNC2_BYPASS  0x00000003
#define GPIO_PIN25_SYNC2_BYPASS_M  ((GPIO_PIN25_SYNC2_BYPASS_V)<<(GPIO_PIN25_SYNC2_BYPASS_S))
#define GPIO_PIN25_SYNC2_BYPASS_V  0x3
#define GPIO_PIN25_SYNC2_BYPASS_S  0

#define GPIO_STATUS_NEXT_REG          (DR_REG_GPIO_BASE + 0x14C)
/* GPIO_STATUS_INTERRUPT_NEXT : RO ;bitpos:[25:0] ;default: 26'h0 ; */
/*description: */
#define GPIO_STATUS_INTERRUPT_NEXT  0x03FFFFFF
#define GPIO_STATUS_INTERRUPT_NEXT_M  ((GPIO_STATUS_INTERRUPT_NEXT_V)<<(GPIO_STATUS_INTERRUPT_NEXT_S))
#define GPIO_STATUS_INTERRUPT_NEXT_V  0x3FFFFFF
#define GPIO_STATUS_INTERRUPT_NEXT_S  0

#define GPIO_FUNC0_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x154)
/* GPIO_SIG0_IN_SEL : R/W ;bitpos:[6] ;default: 1'h0 ; */
/*description: */
#define GPIO_SIG0_IN_SEL  (BIT(6))
#define GPIO_SIG0_IN_SEL_M  (BIT(6))
#define GPIO_SIG0_IN_SEL_V  0x1
#define GPIO_SIG0_IN_SEL_S  6
/* GPIO_FUNC0_IN_INV_SEL : R/W ;bitpos:[5] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC0_IN_INV_SEL  (BIT(5))
#define GPIO_FUNC0_IN_INV_SEL_M  (BIT(5))
#define GPIO_FUNC0_IN_INV_SEL_V  0x1
#define GPIO_FUNC0_IN_INV_SEL_S  5
/* GPIO_FUNC0_IN_SEL : R/W ;bitpos:[4:0] ;default: 5'h0 ; */
/*description: */
#define GPIO_FUNC0_IN_SEL  0x0000001F
#define GPIO_FUNC0_IN_SEL_M  ((GPIO_FUNC0_IN_SEL_V)<<(GPIO_FUNC0_IN_SEL_S))
#define GPIO_FUNC0_IN_SEL_V  0x1F
#define GPIO_FUNC0_IN_SEL_S  0

#define GPIO_FUNC1_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x158)
/* GPIO_SIG1_IN_SEL : R/W ;bitpos:[6] ;default: 1'h0 ; */
/*description: */
#define GPIO_SIG1_IN_SEL  (BIT(6))
#define GPIO_SIG1_IN_SEL_M  (BIT(6))
#define GPIO_SIG1_IN_SEL_V  0x1
#define GPIO_SIG1_IN_SEL_S  6
/* GPIO_FUNC1_IN_INV_SEL : R/W ;bitpos:[5] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC1_IN_INV_SEL  (BIT(5))
#define GPIO_FUNC1_IN_INV_SEL_M  (BIT(5))
#define GPIO_FUNC1_IN_INV_SEL_V  0x1
#define GPIO_FUNC1_IN_INV_SEL_S  5
/* GPIO_FUNC1_IN_SEL : R/W ;bitpos:[4:0] ;default: 5'h0 ; */
/*description: */
#define GPIO_FUNC1_IN_SEL  0x0000001F
#define GPIO_FUNC1_IN_SEL_M  ((GPIO_FUNC1_IN_SEL_V)<<(GPIO_FUNC1_IN_SEL_S))
#define GPIO_FUNC1_IN_SEL_V  0x1F
#define GPIO_FUNC1_IN_SEL_S  0

#define GPIO_FUNC2_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x15C)
/* GPIO_SIG2_IN_SEL : R/W ;bitpos:[6] ;default: 1'h0 ; */
/*description: */
#define GPIO_SIG2_IN_SEL  (BIT(6))
#define GPIO_SIG2_IN_SEL_M  (BIT(6))
#define GPIO_SIG2_IN_SEL_V  0x1
#define GPIO_SIG2_IN_SEL_S  6
/* GPIO_FUNC2_IN_INV_SEL : R/W ;bitpos:[5] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC2_IN_INV_SEL  (BIT(5))
#define GPIO_FUNC2_IN_INV_SEL_M  (BIT(5))
#define GPIO_FUNC2_IN_INV_SEL_V  0x1
#define GPIO_FUNC2_IN_INV_SEL_S  5
/* GPIO_FUNC2_IN_SEL : R/W ;bitpos:[4:0] ;default: 5'h0 ; */
/*description: */
#define GPIO_FUNC2_IN_SEL  0x0000001F
#define GPIO_FUNC2_IN_SEL_M  ((GPIO_FUNC2_IN_SEL_V)<<(GPIO_FUNC2_IN_SEL_S))
#define GPIO_FUNC2_IN_SEL_V  0x1F
#define GPIO_FUNC2_IN_SEL_S  0

#define GPIO_FUNC3_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x160)
/* GPIO_SIG3_IN_SEL : R/W ;bitpos:[6] ;default: 1'h0 ; */
/*description: */
#define GPIO_SIG3_IN_SEL  (BIT(6))
#define GPIO_SIG3_IN_SEL_M  (BIT(6))
#define GPIO_SIG3_IN_SEL_V  0x1
#define GPIO_SIG3_IN_SEL_S  6
/* GPIO_FUNC3_IN_INV_SEL : R/W ;bitpos:[5] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC3_IN_INV_SEL  (BIT(5))
#define GPIO_FUNC3_IN_INV_SEL_M  (BIT(5))
#define GPIO_FUNC3_IN_INV_SEL_V  0x1
#define GPIO_FUNC3_IN_INV_SEL_S  5
/* GPIO_FUNC3_IN_SEL : R/W ;bitpos:[4:0] ;default: 5'h0 ; */
/*description: */
#define GPIO_FUNC3_IN_SEL  0x0000001F
#define GPIO_FUNC3_IN_SEL_M  ((GPIO_FUNC3_IN_SEL_V)<<(GPIO_FUNC3_IN_SEL_S))
#define GPIO_FUNC3_IN_SEL_V  0x1F
#define GPIO_FUNC3_IN_SEL_S  0

#define GPIO_FUNC4_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x164)
/* GPIO_SIG4_IN_SEL : R/W ;bitpos:[6] ;default: 1'h0 ; */
/*description: */
#define GPIO_SIG4_IN_SEL  (BIT(6))
#define GPIO_SIG4_IN_SEL_M  (BIT(6))
#define GPIO_SIG4_IN_SEL_V  0x1
#define GPIO_SIG4_IN_SEL_S  6
/* GPIO_FUNC4_IN_INV_SEL : R/W ;bitpos:[5] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC4_IN_INV_SEL  (BIT(5))
#define GPIO_FUNC4_IN_INV_SEL_M  (BIT(5))
#define GPIO_FUNC4_IN_INV_SEL_V  0x1
#define GPIO_FUNC4_IN_INV_SEL_S  5
/* GPIO_FUNC4_IN_SEL : R/W ;bitpos:[4:0] ;default: 5'h0 ; */
/*description: */
#define GPIO_FUNC4_IN_SEL  0x0000001F
#define GPIO_FUNC4_IN_SEL_M  ((GPIO_FUNC4_IN_SEL_V)<<(GPIO_FUNC4_IN_SEL_S))
#define GPIO_FUNC4_IN_SEL_V  0x1F
#define GPIO_FUNC4_IN_SEL_S  0

#define GPIO_FUNC5_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x168)
/* GPIO_SIG5_IN_SEL : R/W ;bitpos:[6] ;default: 1'h0 ; */
/*description: */
#define GPIO_SIG5_IN_SEL  (BIT(6))
#define GPIO_SIG5_IN_SEL_M  (BIT(6))
#define GPIO_SIG5_IN_SEL_V  0x1
#define GPIO_SIG5_IN_SEL_S  6
/* GPIO_FUNC5_IN_INV_SEL : R/W ;bitpos:[5] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC5_IN_INV_SEL  (BIT(5))
#define GPIO_FUNC5_IN_INV_SEL_M  (BIT(5))
#define GPIO_FUNC5_IN_INV_SEL_V  0x1
#define GPIO_FUNC5_IN_INV_SEL_S  5
/* GPIO_FUNC5_IN_SEL : R/W ;bitpos:[4:0] ;default: 5'h0 ; */
/*description: */
#define GPIO_FUNC5_IN_SEL  0x0000001F
#define GPIO_FUNC5_IN_SEL_M  ((GPIO_FUNC5_IN_SEL_V)<<(GPIO_FUNC5_IN_SEL_S))
#define GPIO_FUNC5_IN_SEL_V  0x1F
#define GPIO_FUNC5_IN_SEL_S  0

#define GPIO_FUNC6_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x16C)
/* GPIO_SIG6_IN_SEL : R/W ;bitpos:[6] ;default: 1'h0 ; */
/*description: */
#define GPIO_SIG6_IN_SEL  (BIT(6))
#define GPIO_SIG6_IN_SEL_M  (BIT(6))
#define GPIO_SIG6_IN_SEL_V  0x1
#define GPIO_SIG6_IN_SEL_S  6
/* GPIO_FUNC6_IN_INV_SEL : R/W ;bitpos:[5] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC6_IN_INV_SEL  (BIT(5))
#define GPIO_FUNC6_IN_INV_SEL_M  (BIT(5))
#define GPIO_FUNC6_IN_INV_SEL_V  0x1
#define GPIO_FUNC6_IN_INV_SEL_S  5
/* GPIO_FUNC6_IN_SEL : R/W ;bitpos:[4:0] ;default: 5'h0 ; */
/*description: */
#define GPIO_FUNC6_IN_SEL  0x0000001F
#define GPIO_FUNC6_IN_SEL_M  ((GPIO_FUNC6_IN_SEL_V)<<(GPIO_FUNC6_IN_SEL_S))
#define GPIO_FUNC6_IN_SEL_V  0x1F
#define GPIO_FUNC6_IN_SEL_S  0

#define GPIO_FUNC7_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x170)
/* GPIO_SIG7_IN_SEL : R/W ;bitpos:[6] ;default: 1'h0 ; */
/*description: */
#define GPIO_SIG7_IN_SEL  (BIT(6))
#define GPIO_SIG7_IN_SEL_M  (BIT(6))
#define GPIO_SIG7_IN_SEL_V  0x1
#define GPIO_SIG7_IN_SEL_S  6
/* GPIO_FUNC7_IN_INV_SEL : R/W ;bitpos:[5] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC7_IN_INV_SEL  (BIT(5))
#define GPIO_FUNC7_IN_INV_SEL_M  (BIT(5))
#define GPIO_FUNC7_IN_INV_SEL_V  0x1
#define GPIO_FUNC7_IN_INV_SEL_S  5
/* GPIO_FUNC7_IN_SEL : R/W ;bitpos:[4:0] ;default: 5'h0 ; */
/*description: */
#define GPIO_FUNC7_IN_SEL  0x0000001F
#define GPIO_FUNC7_IN_SEL_M  ((GPIO_FUNC7_IN_SEL_V)<<(GPIO_FUNC7_IN_SEL_S))
#define GPIO_FUNC7_IN_SEL_V  0x1F
#define GPIO_FUNC7_IN_SEL_S  0

#define GPIO_FUNC8_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x174)
/* GPIO_SIG8_IN_SEL : R/W ;bitpos:[6] ;default: 1'h0 ; */
/*description: */
#define GPIO_SIG8_IN_SEL  (BIT(6))
#define GPIO_SIG8_IN_SEL_M  (BIT(6))
#define GPIO_SIG8_IN_SEL_V  0x1
#define GPIO_SIG8_IN_SEL_S  6
/* GPIO_FUNC8_IN_INV_SEL : R/W ;bitpos:[5] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC8_IN_INV_SEL  (BIT(5))
#define GPIO_FUNC8_IN_INV_SEL_M  (BIT(5))
#define GPIO_FUNC8_IN_INV_SEL_V  0x1
#define GPIO_FUNC8_IN_INV_SEL_S  5
/* GPIO_FUNC8_IN_SEL : R/W ;bitpos:[4:0] ;default: 5'h0 ; */
/*description: */
#define GPIO_FUNC8_IN_SEL  0x0000001F
#define GPIO_FUNC8_IN_SEL_M  ((GPIO_FUNC8_IN_SEL_V)<<(GPIO_FUNC8_IN_SEL_S))
#define GPIO_FUNC8_IN_SEL_V  0x1F
#define GPIO_FUNC8_IN_SEL_S  0

#define GPIO_FUNC9_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x178)
/* GPIO_SIG9_IN_SEL : R/W ;bitpos:[6] ;default: 1'h0 ; */
/*description: */
#define GPIO_SIG9_IN_SEL  (BIT(6))
#define GPIO_SIG9_IN_SEL_M  (BIT(6))
#define GPIO_SIG9_IN_SEL_V  0x1
#define GPIO_SIG9_IN_SEL_S  6
/* GPIO_FUNC9_IN_INV_SEL : R/W ;bitpos:[5] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC9_IN_INV_SEL  (BIT(5))
#define GPIO_FUNC9_IN_INV_SEL_M  (BIT(5))
#define GPIO_FUNC9_IN_INV_SEL_V  0x1
#define GPIO_FUNC9_IN_INV_SEL_S  5
/* GPIO_FUNC9_IN_SEL : R/W ;bitpos:[4:0] ;default: 5'h0 ; */
/*description: */
#define GPIO_FUNC9_IN_SEL  0x0000001F
#define GPIO_FUNC9_IN_SEL_M  ((GPIO_FUNC9_IN_SEL_V)<<(GPIO_FUNC9_IN_SEL_S))
#define GPIO_FUNC9_IN_SEL_V  0x1F
#define GPIO_FUNC9_IN_SEL_S  0

#define GPIO_FUNC10_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x17C)
/* GPIO_SIG10_IN_SEL : R/W ;bitpos:[6] ;default: 1'h0 ; */
/*description: */
#define GPIO_SIG10_IN_SEL  (BIT(6))
#define GPIO_SIG10_IN_SEL_M  (BIT(6))
#define GPIO_SIG10_IN_SEL_V  0x1
#define GPIO_SIG10_IN_SEL_S  6
/* GPIO_FUNC10_IN_INV_SEL : R/W ;bitpos:[5] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC10_IN_INV_SEL  (BIT(5))
#define GPIO_FUNC10_IN_INV_SEL_M  (BIT(5))
#define GPIO_FUNC10_IN_INV_SEL_V  0x1
#define GPIO_FUNC10_IN_INV_SEL_S  5
/* GPIO_FUNC10_IN_SEL : R/W ;bitpos:[4:0] ;default: 5'h0 ; */
/*description: */
#define GPIO_FUNC10_IN_SEL  0x0000001F
#define GPIO_FUNC10_IN_SEL_M  ((GPIO_FUNC10_IN_SEL_V)<<(GPIO_FUNC10_IN_SEL_S))
#define GPIO_FUNC10_IN_SEL_V  0x1F
#define GPIO_FUNC10_IN_SEL_S  0

#define GPIO_FUNC11_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x180)
/* GPIO_SIG11_IN_SEL : R/W ;bitpos:[6] ;default: 1'h0 ; */
/*description: */
#define GPIO_SIG11_IN_SEL  (BIT(6))
#define GPIO_SIG11_IN_SEL_M  (BIT(6))
#define GPIO_SIG11_IN_SEL_V  0x1
#define GPIO_SIG11_IN_SEL_S  6
/* GPIO_FUNC11_IN_INV_SEL : R/W ;bitpos:[5] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC11_IN_INV_SEL  (BIT(5))
#define GPIO_FUNC11_IN_INV_SEL_M  (BIT(5))
#define GPIO_FUNC11_IN_INV_SEL_V  0x1
#define GPIO_FUNC11_IN_INV_SEL_S  5
/* GPIO_FUNC11_IN_SEL : R/W ;bitpos:[4:0] ;default: 5'h0 ; */
/*description: */
#define GPIO_FUNC11_IN_SEL  0x0000001F
#define GPIO_FUNC11_IN_SEL_M  ((GPIO_FUNC11_IN_SEL_V)<<(GPIO_FUNC11_IN_SEL_S))
#define GPIO_FUNC11_IN_SEL_V  0x1F
#define GPIO_FUNC11_IN_SEL_S  0

#define GPIO_FUNC12_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x184)
/* GPIO_SIG12_IN_SEL : R/W ;bitpos:[6] ;default: 1'h0 ; */
/*description: */
#define GPIO_SIG12_IN_SEL  (BIT(6))
#define GPIO_SIG12_IN_SEL_M  (BIT(6))
#define GPIO_SIG12_IN_SEL_V  0x1
#define GPIO_SIG12_IN_SEL_S  6
/* GPIO_FUNC12_IN_INV_SEL : R/W ;bitpos:[5] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC12_IN_INV_SEL  (BIT(5))
#define GPIO_FUNC12_IN_INV_SEL_M  (BIT(5))
#define GPIO_FUNC12_IN_INV_SEL_V  0x1
#define GPIO_FUNC12_IN_INV_SEL_S  5
/* GPIO_FUNC12_IN_SEL : R/W ;bitpos:[4:0] ;default: 5'h0 ; */
/*description: */
#define GPIO_FUNC12_IN_SEL  0x0000001F
#define GPIO_FUNC12_IN_SEL_M  ((GPIO_FUNC12_IN_SEL_V)<<(GPIO_FUNC12_IN_SEL_S))
#define GPIO_FUNC12_IN_SEL_V  0x1F
#define GPIO_FUNC12_IN_SEL_S  0

#define GPIO_FUNC13_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x188)
/* GPIO_SIG13_IN_SEL : R/W ;bitpos:[6] ;default: 1'h0 ; */
/*description: */
#define GPIO_SIG13_IN_SEL  (BIT(6))
#define GPIO_SIG13_IN_SEL_M  (BIT(6))
#define GPIO_SIG13_IN_SEL_V  0x1
#define GPIO_SIG13_IN_SEL_S  6
/* GPIO_FUNC13_IN_INV_SEL : R/W ;bitpos:[5] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC13_IN_INV_SEL  (BIT(5))
#define GPIO_FUNC13_IN_INV_SEL_M  (BIT(5))
#define GPIO_FUNC13_IN_INV_SEL_V  0x1
#define GPIO_FUNC13_IN_INV_SEL_S  5
/* GPIO_FUNC13_IN_SEL : R/W ;bitpos:[4:0] ;default: 5'h0 ; */
/*description: */
#define GPIO_FUNC13_IN_SEL  0x0000001F
#define GPIO_FUNC13_IN_SEL_M  ((GPIO_FUNC13_IN_SEL_V)<<(GPIO_FUNC13_IN_SEL_S))
#define GPIO_FUNC13_IN_SEL_V  0x1F
#define GPIO_FUNC13_IN_SEL_S  0

#define GPIO_FUNC14_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x18C)
/* GPIO_SIG14_IN_SEL : R/W ;bitpos:[6] ;default: 1'h0 ; */
/*description: */
#define GPIO_SIG14_IN_SEL  (BIT(6))
#define GPIO_SIG14_IN_SEL_M  (BIT(6))
#define GPIO_SIG14_IN_SEL_V  0x1
#define GPIO_SIG14_IN_SEL_S  6
/* GPIO_FUNC14_IN_INV_SEL : R/W ;bitpos:[5] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC14_IN_INV_SEL  (BIT(5))
#define GPIO_FUNC14_IN_INV_SEL_M  (BIT(5))
#define GPIO_FUNC14_IN_INV_SEL_V  0x1
#define GPIO_FUNC14_IN_INV_SEL_S  5
/* GPIO_FUNC14_IN_SEL : R/W ;bitpos:[4:0] ;default: 5'h0 ; */
/*description: */
#define GPIO_FUNC14_IN_SEL  0x0000001F
#define GPIO_FUNC14_IN_SEL_M  ((GPIO_FUNC14_IN_SEL_V)<<(GPIO_FUNC14_IN_SEL_S))
#define GPIO_FUNC14_IN_SEL_V  0x1F
#define GPIO_FUNC14_IN_SEL_S  0

#define GPIO_FUNC15_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x190)
/* GPIO_SIG15_IN_SEL : R/W ;bitpos:[6] ;default: 1'h0 ; */
/*description: */
#define GPIO_SIG15_IN_SEL  (BIT(6))
#define GPIO_SIG15_IN_SEL_M  (BIT(6))
#define GPIO_SIG15_IN_SEL_V  0x1
#define GPIO_SIG15_IN_SEL_S  6
/* GPIO_FUNC15_IN_INV_SEL : R/W ;bitpos:[5] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC15_IN_INV_SEL  (BIT(5))
#define GPIO_FUNC15_IN_INV_SEL_M  (BIT(5))
#define GPIO_FUNC15_IN_INV_SEL_V  0x1
#define GPIO_FUNC15_IN_INV_SEL_S  5
/* GPIO_FUNC15_IN_SEL : R/W ;bitpos:[4:0] ;default: 5'h0 ; */
/*description: */
#define GPIO_FUNC15_IN_SEL  0x0000001F
#define GPIO_FUNC15_IN_SEL_M  ((GPIO_FUNC15_IN_SEL_V)<<(GPIO_FUNC15_IN_SEL_S))
#define GPIO_FUNC15_IN_SEL_V  0x1F
#define GPIO_FUNC15_IN_SEL_S  0

#define GPIO_FUNC16_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x194)
/* GPIO_SIG16_IN_SEL : R/W ;bitpos:[6] ;default: 1'h0 ; */
/*description: */
#define GPIO_SIG16_IN_SEL  (BIT(6))
#define GPIO_SIG16_IN_SEL_M  (BIT(6))
#define GPIO_SIG16_IN_SEL_V  0x1
#define GPIO_SIG16_IN_SEL_S  6
/* GPIO_FUNC16_IN_INV_SEL : R/W ;bitpos:[5] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC16_IN_INV_SEL  (BIT(5))
#define GPIO_FUNC16_IN_INV_SEL_M  (BIT(5))
#define GPIO_FUNC16_IN_INV_SEL_V  0x1
#define GPIO_FUNC16_IN_INV_SEL_S  5
/* GPIO_FUNC16_IN_SEL : R/W ;bitpos:[4:0] ;default: 5'h0 ; */
/*description: */
#define GPIO_FUNC16_IN_SEL  0x0000001F
#define GPIO_FUNC16_IN_SEL_M  ((GPIO_FUNC16_IN_SEL_V)<<(GPIO_FUNC16_IN_SEL_S))
#define GPIO_FUNC16_IN_SEL_V  0x1F
#define GPIO_FUNC16_IN_SEL_S  0

#define GPIO_FUNC17_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x198)
/* GPIO_SIG17_IN_SEL : R/W ;bitpos:[6] ;default: 1'h0 ; */
/*description: */
#define GPIO_SIG17_IN_SEL  (BIT(6))
#define GPIO_SIG17_IN_SEL_M  (BIT(6))
#define GPIO_SIG17_IN_SEL_V  0x1
#define GPIO_SIG17_IN_SEL_S  6
/* GPIO_FUNC17_IN_INV_SEL : R/W ;bitpos:[5] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC17_IN_INV_SEL  (BIT(5))
#define GPIO_FUNC17_IN_INV_SEL_M  (BIT(5))
#define GPIO_FUNC17_IN_INV_SEL_V  0x1
#define GPIO_FUNC17_IN_INV_SEL_S  5
/* GPIO_FUNC17_IN_SEL : R/W ;bitpos:[4:0] ;default: 5'h0 ; */
/*description: */
#define GPIO_FUNC17_IN_SEL  0x0000001F
#define GPIO_FUNC17_IN_SEL_M  ((GPIO_FUNC17_IN_SEL_V)<<(GPIO_FUNC17_IN_SEL_S))
#define GPIO_FUNC17_IN_SEL_V  0x1F
#define GPIO_FUNC17_IN_SEL_S  0

#define GPIO_FUNC18_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x19C)
/* GPIO_SIG18_IN_SEL : R/W ;bitpos:[6] ;default: 1'h0 ; */
/*description: */
#define GPIO_SIG18_IN_SEL  (BIT(6))
#define GPIO_SIG18_IN_SEL_M  (BIT(6))
#define GPIO_SIG18_IN_SEL_V  0x1
#define GPIO_SIG18_IN_SEL_S  6
/* GPIO_FUNC18_IN_INV_SEL : R/W ;bitpos:[5] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC18_IN_INV_SEL  (BIT(5))
#define GPIO_FUNC18_IN_INV_SEL_M  (BIT(5))
#define GPIO_FUNC18_IN_INV_SEL_V  0x1
#define GPIO_FUNC18_IN_INV_SEL_S  5
/* GPIO_FUNC18_IN_SEL : R/W ;bitpos:[4:0] ;default: 5'h0 ; */
/*description: */
#define GPIO_FUNC18_IN_SEL  0x0000001F
#define GPIO_FUNC18_IN_SEL_M  ((GPIO_FUNC18_IN_SEL_V)<<(GPIO_FUNC18_IN_SEL_S))
#define GPIO_FUNC18_IN_SEL_V  0x1F
#define GPIO_FUNC18_IN_SEL_S  0

#define GPIO_FUNC19_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x1A0)
/* GPIO_SIG19_IN_SEL : R/W ;bitpos:[6] ;default: 1'h0 ; */
/*description: */
#define GPIO_SIG19_IN_SEL  (BIT(6))
#define GPIO_SIG19_IN_SEL_M  (BIT(6))
#define GPIO_SIG19_IN_SEL_V  0x1
#define GPIO_SIG19_IN_SEL_S  6
/* GPIO_FUNC19_IN_INV_SEL : R/W ;bitpos:[5] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC19_IN_INV_SEL  (BIT(5))
#define GPIO_FUNC19_IN_INV_SEL_M  (BIT(5))
#define GPIO_FUNC19_IN_INV_SEL_V  0x1
#define GPIO_FUNC19_IN_INV_SEL_S  5
/* GPIO_FUNC19_IN_SEL : R/W ;bitpos:[4:0] ;default: 5'h0 ; */
/*description: */
#define GPIO_FUNC19_IN_SEL  0x0000001F
#define GPIO_FUNC19_IN_SEL_M  ((GPIO_FUNC19_IN_SEL_V)<<(GPIO_FUNC19_IN_SEL_S))
#define GPIO_FUNC19_IN_SEL_V  0x1F
#define GPIO_FUNC19_IN_SEL_S  0

#define GPIO_FUNC20_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x1A4)
/* GPIO_SIG20_IN_SEL : R/W ;bitpos:[6] ;default: 1'h0 ; */
/*description: */
#define GPIO_SIG20_IN_SEL  (BIT(6))
#define GPIO_SIG20_IN_SEL_M  (BIT(6))
#define GPIO_SIG20_IN_SEL_V  0x1
#define GPIO_SIG20_IN_SEL_S  6
/* GPIO_FUNC20_IN_INV_SEL : R/W ;bitpos:[5] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC20_IN_INV_SEL  (BIT(5))
#define GPIO_FUNC20_IN_INV_SEL_M  (BIT(5))
#define GPIO_FUNC20_IN_INV_SEL_V  0x1
#define GPIO_FUNC20_IN_INV_SEL_S  5
/* GPIO_FUNC20_IN_SEL : R/W ;bitpos:[4:0] ;default: 5'h0 ; */
/*description: */
#define GPIO_FUNC20_IN_SEL  0x0000001F
#define GPIO_FUNC20_IN_SEL_M  ((GPIO_FUNC20_IN_SEL_V)<<(GPIO_FUNC20_IN_SEL_S))
#define GPIO_FUNC20_IN_SEL_V  0x1F
#define GPIO_FUNC20_IN_SEL_S  0

#define GPIO_FUNC21_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x1A8)
/* GPIO_SIG21_IN_SEL : R/W ;bitpos:[6] ;default: 1'h0 ; */
/*description: */
#define GPIO_SIG21_IN_SEL  (BIT(6))
#define GPIO_SIG21_IN_SEL_M  (BIT(6))
#define GPIO_SIG21_IN_SEL_V  0x1
#define GPIO_SIG21_IN_SEL_S  6
/* GPIO_FUNC21_IN_INV_SEL : R/W ;bitpos:[5] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC21_IN_INV_SEL  (BIT(5))
#define GPIO_FUNC21_IN_INV_SEL_M  (BIT(5))
#define GPIO_FUNC21_IN_INV_SEL_V  0x1
#define GPIO_FUNC21_IN_INV_SEL_S  5
/* GPIO_FUNC21_IN_SEL : R/W ;bitpos:[4:0] ;default: 5'h0 ; */
/*description: */
#define GPIO_FUNC21_IN_SEL  0x0000001F
#define GPIO_FUNC21_IN_SEL_M  ((GPIO_FUNC21_IN_SEL_V)<<(GPIO_FUNC21_IN_SEL_S))
#define GPIO_FUNC21_IN_SEL_V  0x1F
#define GPIO_FUNC21_IN_SEL_S  0

#define GPIO_FUNC22_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x1AC)
/* GPIO_SIG22_IN_SEL : R/W ;bitpos:[6] ;default: 1'h0 ; */
/*description: */
#define GPIO_SIG22_IN_SEL  (BIT(6))
#define GPIO_SIG22_IN_SEL_M  (BIT(6))
#define GPIO_SIG22_IN_SEL_V  0x1
#define GPIO_SIG22_IN_SEL_S  6
/* GPIO_FUNC22_IN_INV_SEL : R/W ;bitpos:[5] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC22_IN_INV_SEL  (BIT(5))
#define GPIO_FUNC22_IN_INV_SEL_M  (BIT(5))
#define GPIO_FUNC22_IN_INV_SEL_V  0x1
#define GPIO_FUNC22_IN_INV_SEL_S  5
/* GPIO_FUNC22_IN_SEL : R/W ;bitpos:[4:0] ;default: 5'h0 ; */
/*description: */
#define GPIO_FUNC22_IN_SEL  0x0000001F
#define GPIO_FUNC22_IN_SEL_M  ((GPIO_FUNC22_IN_SEL_V)<<(GPIO_FUNC22_IN_SEL_S))
#define GPIO_FUNC22_IN_SEL_V  0x1F
#define GPIO_FUNC22_IN_SEL_S  0

#define GPIO_FUNC23_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x1B0)
/* GPIO_SIG23_IN_SEL : R/W ;bitpos:[6] ;default: 1'h0 ; */
/*description: */
#define GPIO_SIG23_IN_SEL  (BIT(6))
#define GPIO_SIG23_IN_SEL_M  (BIT(6))
#define GPIO_SIG23_IN_SEL_V  0x1
#define GPIO_SIG23_IN_SEL_S  6
/* GPIO_FUNC23_IN_INV_SEL : R/W ;bitpos:[5] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC23_IN_INV_SEL  (BIT(5))
#define GPIO_FUNC23_IN_INV_SEL_M  (BIT(5))
#define GPIO_FUNC23_IN_INV_SEL_V  0x1
#define GPIO_FUNC23_IN_INV_SEL_S  5
/* GPIO_FUNC23_IN_SEL : R/W ;bitpos:[4:0] ;default: 5'h0 ; */
/*description: */
#define GPIO_FUNC23_IN_SEL  0x0000001F
#define GPIO_FUNC23_IN_SEL_M  ((GPIO_FUNC23_IN_SEL_V)<<(GPIO_FUNC23_IN_SEL_S))
#define GPIO_FUNC23_IN_SEL_V  0x1F
#define GPIO_FUNC23_IN_SEL_S  0

#define GPIO_FUNC24_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x1B4)
/* GPIO_SIG24_IN_SEL : R/W ;bitpos:[6] ;default: 1'h0 ; */
/*description: */
#define GPIO_SIG24_IN_SEL  (BIT(6))
#define GPIO_SIG24_IN_SEL_M  (BIT(6))
#define GPIO_SIG24_IN_SEL_V  0x1
#define GPIO_SIG24_IN_SEL_S  6
/* GPIO_FUNC24_IN_INV_SEL : R/W ;bitpos:[5] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC24_IN_INV_SEL  (BIT(5))
#define GPIO_FUNC24_IN_INV_SEL_M  (BIT(5))
#define GPIO_FUNC24_IN_INV_SEL_V  0x1
#define GPIO_FUNC24_IN_INV_SEL_S  5
/* GPIO_FUNC24_IN_SEL : R/W ;bitpos:[4:0] ;default: 5'h0 ; */
/*description: */
#define GPIO_FUNC24_IN_SEL  0x0000001F
#define GPIO_FUNC24_IN_SEL_M  ((GPIO_FUNC24_IN_SEL_V)<<(GPIO_FUNC24_IN_SEL_S))
#define GPIO_FUNC24_IN_SEL_V  0x1F
#define GPIO_FUNC24_IN_SEL_S  0

#define GPIO_FUNC25_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x1B8)
/* GPIO_SIG25_IN_SEL : R/W ;bitpos:[6] ;default: 1'h0 ; */
/*description: */
#define GPIO_SIG25_IN_SEL  (BIT(6))
#define GPIO_SIG25_IN_SEL_M  (BIT(6))
#define GPIO_SIG25_IN_SEL_V  0x1
#define GPIO_SIG25_IN_SEL_S  6
/* GPIO_FUNC25_IN_INV_SEL : R/W ;bitpos:[5] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC25_IN_INV_SEL  (BIT(5))
#define GPIO_FUNC25_IN_INV_SEL_M  (BIT(5))
#define GPIO_FUNC25_IN_INV_SEL_V  0x1
#define GPIO_FUNC25_IN_INV_SEL_S  5
/* GPIO_FUNC25_IN_SEL : R/W ;bitpos:[4:0] ;default: 5'h0 ; */
/*description: */
#define GPIO_FUNC25_IN_SEL  0x0000001F
#define GPIO_FUNC25_IN_SEL_M  ((GPIO_FUNC25_IN_SEL_V)<<(GPIO_FUNC25_IN_SEL_S))
#define GPIO_FUNC25_IN_SEL_V  0x1F
#define GPIO_FUNC25_IN_SEL_S  0

#define GPIO_FUNC26_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x1BC)
/* GPIO_SIG26_IN_SEL : R/W ;bitpos:[6] ;default: 1'h0 ; */
/*description: */
#define GPIO_SIG26_IN_SEL  (BIT(6))
#define GPIO_SIG26_IN_SEL_M  (BIT(6))
#define GPIO_SIG26_IN_SEL_V  0x1
#define GPIO_SIG26_IN_SEL_S  6
/* GPIO_FUNC26_IN_INV_SEL : R/W ;bitpos:[5] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC26_IN_INV_SEL  (BIT(5))
#define GPIO_FUNC26_IN_INV_SEL_M  (BIT(5))
#define GPIO_FUNC26_IN_INV_SEL_V  0x1
#define GPIO_FUNC26_IN_INV_SEL_S  5
/* GPIO_FUNC26_IN_SEL : R/W ;bitpos:[4:0] ;default: 5'h0 ; */
/*description: */
#define GPIO_FUNC26_IN_SEL  0x0000001F
#define GPIO_FUNC26_IN_SEL_M  ((GPIO_FUNC26_IN_SEL_V)<<(GPIO_FUNC26_IN_SEL_S))
#define GPIO_FUNC26_IN_SEL_V  0x1F
#define GPIO_FUNC26_IN_SEL_S  0

#define GPIO_FUNC27_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x1C0)
/* GPIO_SIG27_IN_SEL : R/W ;bitpos:[6] ;default: 1'h0 ; */
/*description: */
#define GPIO_SIG27_IN_SEL  (BIT(6))
#define GPIO_SIG27_IN_SEL_M  (BIT(6))
#define GPIO_SIG27_IN_SEL_V  0x1
#define GPIO_SIG27_IN_SEL_S  6
/* GPIO_FUNC27_IN_INV_SEL : R/W ;bitpos:[5] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC27_IN_INV_SEL  (BIT(5))
#define GPIO_FUNC27_IN_INV_SEL_M  (BIT(5))
#define GPIO_FUNC27_IN_INV_SEL_V  0x1
#define GPIO_FUNC27_IN_INV_SEL_S  5
/* GPIO_FUNC27_IN_SEL : R/W ;bitpos:[4:0] ;default: 5'h0 ; */
/*description: */
#define GPIO_FUNC27_IN_SEL  0x0000001F
#define GPIO_FUNC27_IN_SEL_M  ((GPIO_FUNC27_IN_SEL_V)<<(GPIO_FUNC27_IN_SEL_S))
#define GPIO_FUNC27_IN_SEL_V  0x1F
#define GPIO_FUNC27_IN_SEL_S  0

#define GPIO_FUNC28_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x1C4)
/* GPIO_SIG28_IN_SEL : R/W ;bitpos:[6] ;default: 1'h0 ; */
/*description: */
#define GPIO_SIG28_IN_SEL  (BIT(6))
#define GPIO_SIG28_IN_SEL_M  (BIT(6))
#define GPIO_SIG28_IN_SEL_V  0x1
#define GPIO_SIG28_IN_SEL_S  6
/* GPIO_FUNC28_IN_INV_SEL : R/W ;bitpos:[5] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC28_IN_INV_SEL  (BIT(5))
#define GPIO_FUNC28_IN_INV_SEL_M  (BIT(5))
#define GPIO_FUNC28_IN_INV_SEL_V  0x1
#define GPIO_FUNC28_IN_INV_SEL_S  5
/* GPIO_FUNC28_IN_SEL : R/W ;bitpos:[4:0] ;default: 5'h0 ; */
/*description: */
#define GPIO_FUNC28_IN_SEL  0x0000001F
#define GPIO_FUNC28_IN_SEL_M  ((GPIO_FUNC28_IN_SEL_V)<<(GPIO_FUNC28_IN_SEL_S))
#define GPIO_FUNC28_IN_SEL_V  0x1F
#define GPIO_FUNC28_IN_SEL_S  0

#define GPIO_FUNC29_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x1C8)
/* GPIO_SIG29_IN_SEL : R/W ;bitpos:[6] ;default: 1'h0 ; */
/*description: */
#define GPIO_SIG29_IN_SEL  (BIT(6))
#define GPIO_SIG29_IN_SEL_M  (BIT(6))
#define GPIO_SIG29_IN_SEL_V  0x1
#define GPIO_SIG29_IN_SEL_S  6
/* GPIO_FUNC29_IN_INV_SEL : R/W ;bitpos:[5] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC29_IN_INV_SEL  (BIT(5))
#define GPIO_FUNC29_IN_INV_SEL_M  (BIT(5))
#define GPIO_FUNC29_IN_INV_SEL_V  0x1
#define GPIO_FUNC29_IN_INV_SEL_S  5
/* GPIO_FUNC29_IN_SEL : R/W ;bitpos:[4:0] ;default: 5'h0 ; */
/*description: */
#define GPIO_FUNC29_IN_SEL  0x0000001F
#define GPIO_FUNC29_IN_SEL_M  ((GPIO_FUNC29_IN_SEL_V)<<(GPIO_FUNC29_IN_SEL_S))
#define GPIO_FUNC29_IN_SEL_V  0x1F
#define GPIO_FUNC29_IN_SEL_S  0

#define GPIO_FUNC30_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x1CC)
/* GPIO_SIG30_IN_SEL : R/W ;bitpos:[6] ;default: 1'h0 ; */
/*description: */
#define GPIO_SIG30_IN_SEL  (BIT(6))
#define GPIO_SIG30_IN_SEL_M  (BIT(6))
#define GPIO_SIG30_IN_SEL_V  0x1
#define GPIO_SIG30_IN_SEL_S  6
/* GPIO_FUNC30_IN_INV_SEL : R/W ;bitpos:[5] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC30_IN_INV_SEL  (BIT(5))
#define GPIO_FUNC30_IN_INV_SEL_M  (BIT(5))
#define GPIO_FUNC30_IN_INV_SEL_V  0x1
#define GPIO_FUNC30_IN_INV_SEL_S  5
/* GPIO_FUNC30_IN_SEL : R/W ;bitpos:[4:0] ;default: 5'h0 ; */
/*description: */
#define GPIO_FUNC30_IN_SEL  0x0000001F
#define GPIO_FUNC30_IN_SEL_M  ((GPIO_FUNC30_IN_SEL_V)<<(GPIO_FUNC30_IN_SEL_S))
#define GPIO_FUNC30_IN_SEL_V  0x1F
#define GPIO_FUNC30_IN_SEL_S  0

#define GPIO_FUNC31_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x1D0)
/* GPIO_SIG31_IN_SEL : R/W ;bitpos:[6] ;default: 1'h0 ; */
/*description: */
#define GPIO_SIG31_IN_SEL  (BIT(6))
#define GPIO_SIG31_IN_SEL_M  (BIT(6))
#define GPIO_SIG31_IN_SEL_V  0x1
#define GPIO_SIG31_IN_SEL_S  6
/* GPIO_FUNC31_IN_INV_SEL : R/W ;bitpos:[5] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC31_IN_INV_SEL  (BIT(5))
#define GPIO_FUNC31_IN_INV_SEL_M  (BIT(5))
#define GPIO_FUNC31_IN_INV_SEL_V  0x1
#define GPIO_FUNC31_IN_INV_SEL_S  5
/* GPIO_FUNC31_IN_SEL : R/W ;bitpos:[4:0] ;default: 5'h0 ; */
/*description: */
#define GPIO_FUNC31_IN_SEL  0x0000001F
#define GPIO_FUNC31_IN_SEL_M  ((GPIO_FUNC31_IN_SEL_V)<<(GPIO_FUNC31_IN_SEL_S))
#define GPIO_FUNC31_IN_SEL_V  0x1F
#define GPIO_FUNC31_IN_SEL_S  0

#define GPIO_FUNC32_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x1D4)
/* GPIO_SIG32_IN_SEL : R/W ;bitpos:[6] ;default: 1'h0 ; */
/*description: */
#define GPIO_SIG32_IN_SEL  (BIT(6))
#define GPIO_SIG32_IN_SEL_M  (BIT(6))
#define GPIO_SIG32_IN_SEL_V  0x1
#define GPIO_SIG32_IN_SEL_S  6
/* GPIO_FUNC32_IN_INV_SEL : R/W ;bitpos:[5] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC32_IN_INV_SEL  (BIT(5))
#define GPIO_FUNC32_IN_INV_SEL_M  (BIT(5))
#define GPIO_FUNC32_IN_INV_SEL_V  0x1
#define GPIO_FUNC32_IN_INV_SEL_S  5
/* GPIO_FUNC32_IN_SEL : R/W ;bitpos:[4:0] ;default: 5'h0 ; */
/*description: */
#define GPIO_FUNC32_IN_SEL  0x0000001F
#define GPIO_FUNC32_IN_SEL_M  ((GPIO_FUNC32_IN_SEL_V)<<(GPIO_FUNC32_IN_SEL_S))
#define GPIO_FUNC32_IN_SEL_V  0x1F
#define GPIO_FUNC32_IN_SEL_S  0

#define GPIO_FUNC33_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x1D8)
/* GPIO_SIG33_IN_SEL : R/W ;bitpos:[6] ;default: 1'h0 ; */
/*description: */
#define GPIO_SIG33_IN_SEL  (BIT(6))
#define GPIO_SIG33_IN_SEL_M  (BIT(6))
#define GPIO_SIG33_IN_SEL_V  0x1
#define GPIO_SIG33_IN_SEL_S  6
/* GPIO_FUNC33_IN_INV_SEL : R/W ;bitpos:[5] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC33_IN_INV_SEL  (BIT(5))
#define GPIO_FUNC33_IN_INV_SEL_M  (BIT(5))
#define GPIO_FUNC33_IN_INV_SEL_V  0x1
#define GPIO_FUNC33_IN_INV_SEL_S  5
/* GPIO_FUNC33_IN_SEL : R/W ;bitpos:[4:0] ;default: 5'h0 ; */
/*description: */
#define GPIO_FUNC33_IN_SEL  0x0000001F
#define GPIO_FUNC33_IN_SEL_M  ((GPIO_FUNC33_IN_SEL_V)<<(GPIO_FUNC33_IN_SEL_S))
#define GPIO_FUNC33_IN_SEL_V  0x1F
#define GPIO_FUNC33_IN_SEL_S  0

#define GPIO_FUNC34_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x1DC)
/* GPIO_SIG34_IN_SEL : R/W ;bitpos:[6] ;default: 1'h0 ; */
/*description: */
#define GPIO_SIG34_IN_SEL  (BIT(6))
#define GPIO_SIG34_IN_SEL_M  (BIT(6))
#define GPIO_SIG34_IN_SEL_V  0x1
#define GPIO_SIG34_IN_SEL_S  6
/* GPIO_FUNC34_IN_INV_SEL : R/W ;bitpos:[5] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC34_IN_INV_SEL  (BIT(5))
#define GPIO_FUNC34_IN_INV_SEL_M  (BIT(5))
#define GPIO_FUNC34_IN_INV_SEL_V  0x1
#define GPIO_FUNC34_IN_INV_SEL_S  5
/* GPIO_FUNC34_IN_SEL : R/W ;bitpos:[4:0] ;default: 5'h0 ; */
/*description: */
#define GPIO_FUNC34_IN_SEL  0x0000001F
#define GPIO_FUNC34_IN_SEL_M  ((GPIO_FUNC34_IN_SEL_V)<<(GPIO_FUNC34_IN_SEL_S))
#define GPIO_FUNC34_IN_SEL_V  0x1F
#define GPIO_FUNC34_IN_SEL_S  0

#define GPIO_FUNC35_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x1E0)
/* GPIO_SIG35_IN_SEL : R/W ;bitpos:[6] ;default: 1'h0 ; */
/*description: */
#define GPIO_SIG35_IN_SEL  (BIT(6))
#define GPIO_SIG35_IN_SEL_M  (BIT(6))
#define GPIO_SIG35_IN_SEL_V  0x1
#define GPIO_SIG35_IN_SEL_S  6
/* GPIO_FUNC35_IN_INV_SEL : R/W ;bitpos:[5] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC35_IN_INV_SEL  (BIT(5))
#define GPIO_FUNC35_IN_INV_SEL_M  (BIT(5))
#define GPIO_FUNC35_IN_INV_SEL_V  0x1
#define GPIO_FUNC35_IN_INV_SEL_S  5
/* GPIO_FUNC35_IN_SEL : R/W ;bitpos:[4:0] ;default: 5'h0 ; */
/*description: */
#define GPIO_FUNC35_IN_SEL  0x0000001F
#define GPIO_FUNC35_IN_SEL_M  ((GPIO_FUNC35_IN_SEL_V)<<(GPIO_FUNC35_IN_SEL_S))
#define GPIO_FUNC35_IN_SEL_V  0x1F
#define GPIO_FUNC35_IN_SEL_S  0

#define GPIO_FUNC36_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x1E4)
/* GPIO_SIG36_IN_SEL : R/W ;bitpos:[6] ;default: 1'h0 ; */
/*description: */
#define GPIO_SIG36_IN_SEL  (BIT(6))
#define GPIO_SIG36_IN_SEL_M  (BIT(6))
#define GPIO_SIG36_IN_SEL_V  0x1
#define GPIO_SIG36_IN_SEL_S  6
/* GPIO_FUNC36_IN_INV_SEL : R/W ;bitpos:[5] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC36_IN_INV_SEL  (BIT(5))
#define GPIO_FUNC36_IN_INV_SEL_M  (BIT(5))
#define GPIO_FUNC36_IN_INV_SEL_V  0x1
#define GPIO_FUNC36_IN_INV_SEL_S  5
/* GPIO_FUNC36_IN_SEL : R/W ;bitpos:[4:0] ;default: 5'h0 ; */
/*description: */
#define GPIO_FUNC36_IN_SEL  0x0000001F
#define GPIO_FUNC36_IN_SEL_M  ((GPIO_FUNC36_IN_SEL_V)<<(GPIO_FUNC36_IN_SEL_S))
#define GPIO_FUNC36_IN_SEL_V  0x1F
#define GPIO_FUNC36_IN_SEL_S  0

#define GPIO_FUNC37_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x1E8)
/* GPIO_SIG37_IN_SEL : R/W ;bitpos:[6] ;default: 1'h0 ; */
/*description: */
#define GPIO_SIG37_IN_SEL  (BIT(6))
#define GPIO_SIG37_IN_SEL_M  (BIT(6))
#define GPIO_SIG37_IN_SEL_V  0x1
#define GPIO_SIG37_IN_SEL_S  6
/* GPIO_FUNC37_IN_INV_SEL : R/W ;bitpos:[5] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC37_IN_INV_SEL  (BIT(5))
#define GPIO_FUNC37_IN_INV_SEL_M  (BIT(5))
#define GPIO_FUNC37_IN_INV_SEL_V  0x1
#define GPIO_FUNC37_IN_INV_SEL_S  5
/* GPIO_FUNC37_IN_SEL : R/W ;bitpos:[4:0] ;default: 5'h0 ; */
/*description: */
#define GPIO_FUNC37_IN_SEL  0x0000001F
#define GPIO_FUNC37_IN_SEL_M  ((GPIO_FUNC37_IN_SEL_V)<<(GPIO_FUNC37_IN_SEL_S))
#define GPIO_FUNC37_IN_SEL_V  0x1F
#define GPIO_FUNC37_IN_SEL_S  0

#define GPIO_FUNC38_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x1EC)
/* GPIO_SIG38_IN_SEL : R/W ;bitpos:[6] ;default: 1'h0 ; */
/*description: */
#define GPIO_SIG38_IN_SEL  (BIT(6))
#define GPIO_SIG38_IN_SEL_M  (BIT(6))
#define GPIO_SIG38_IN_SEL_V  0x1
#define GPIO_SIG38_IN_SEL_S  6
/* GPIO_FUNC38_IN_INV_SEL : R/W ;bitpos:[5] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC38_IN_INV_SEL  (BIT(5))
#define GPIO_FUNC38_IN_INV_SEL_M  (BIT(5))
#define GPIO_FUNC38_IN_INV_SEL_V  0x1
#define GPIO_FUNC38_IN_INV_SEL_S  5
/* GPIO_FUNC38_IN_SEL : R/W ;bitpos:[4:0] ;default: 5'h0 ; */
/*description: */
#define GPIO_FUNC38_IN_SEL  0x0000001F
#define GPIO_FUNC38_IN_SEL_M  ((GPIO_FUNC38_IN_SEL_V)<<(GPIO_FUNC38_IN_SEL_S))
#define GPIO_FUNC38_IN_SEL_V  0x1F
#define GPIO_FUNC38_IN_SEL_S  0

#define GPIO_FUNC39_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x1F0)
/* GPIO_SIG39_IN_SEL : R/W ;bitpos:[6] ;default: 1'h0 ; */
/*description: */
#define GPIO_SIG39_IN_SEL  (BIT(6))
#define GPIO_SIG39_IN_SEL_M  (BIT(6))
#define GPIO_SIG39_IN_SEL_V  0x1
#define GPIO_SIG39_IN_SEL_S  6
/* GPIO_FUNC39_IN_INV_SEL : R/W ;bitpos:[5] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC39_IN_INV_SEL  (BIT(5))
#define GPIO_FUNC39_IN_INV_SEL_M  (BIT(5))
#define GPIO_FUNC39_IN_INV_SEL_V  0x1
#define GPIO_FUNC39_IN_INV_SEL_S  5
/* GPIO_FUNC39_IN_SEL : R/W ;bitpos:[4:0] ;default: 5'h0 ; */
/*description: */
#define GPIO_FUNC39_IN_SEL  0x0000001F
#define GPIO_FUNC39_IN_SEL_M  ((GPIO_FUNC39_IN_SEL_V)<<(GPIO_FUNC39_IN_SEL_S))
#define GPIO_FUNC39_IN_SEL_V  0x1F
#define GPIO_FUNC39_IN_SEL_S  0

#define GPIO_FUNC40_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x1F4)
/* GPIO_SIG40_IN_SEL : R/W ;bitpos:[6] ;default: 1'h0 ; */
/*description: */
#define GPIO_SIG40_IN_SEL  (BIT(6))
#define GPIO_SIG40_IN_SEL_M  (BIT(6))
#define GPIO_SIG40_IN_SEL_V  0x1
#define GPIO_SIG40_IN_SEL_S  6
/* GPIO_FUNC40_IN_INV_SEL : R/W ;bitpos:[5] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC40_IN_INV_SEL  (BIT(5))
#define GPIO_FUNC40_IN_INV_SEL_M  (BIT(5))
#define GPIO_FUNC40_IN_INV_SEL_V  0x1
#define GPIO_FUNC40_IN_INV_SEL_S  5
/* GPIO_FUNC40_IN_SEL : R/W ;bitpos:[4:0] ;default: 5'h0 ; */
/*description: */
#define GPIO_FUNC40_IN_SEL  0x0000001F
#define GPIO_FUNC40_IN_SEL_M  ((GPIO_FUNC40_IN_SEL_V)<<(GPIO_FUNC40_IN_SEL_S))
#define GPIO_FUNC40_IN_SEL_V  0x1F
#define GPIO_FUNC40_IN_SEL_S  0

#define GPIO_FUNC41_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x1F8)
/* GPIO_SIG41_IN_SEL : R/W ;bitpos:[6] ;default: 1'h0 ; */
/*description: */
#define GPIO_SIG41_IN_SEL  (BIT(6))
#define GPIO_SIG41_IN_SEL_M  (BIT(6))
#define GPIO_SIG41_IN_SEL_V  0x1
#define GPIO_SIG41_IN_SEL_S  6
/* GPIO_FUNC41_IN_INV_SEL : R/W ;bitpos:[5] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC41_IN_INV_SEL  (BIT(5))
#define GPIO_FUNC41_IN_INV_SEL_M  (BIT(5))
#define GPIO_FUNC41_IN_INV_SEL_V  0x1
#define GPIO_FUNC41_IN_INV_SEL_S  5
/* GPIO_FUNC41_IN_SEL : R/W ;bitpos:[4:0] ;default: 5'h0 ; */
/*description: */
#define GPIO_FUNC41_IN_SEL  0x0000001F
#define GPIO_FUNC41_IN_SEL_M  ((GPIO_FUNC41_IN_SEL_V)<<(GPIO_FUNC41_IN_SEL_S))
#define GPIO_FUNC41_IN_SEL_V  0x1F
#define GPIO_FUNC41_IN_SEL_S  0

#define GPIO_FUNC42_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x1FC)
/* GPIO_SIG42_IN_SEL : R/W ;bitpos:[6] ;default: 1'h0 ; */
/*description: */
#define GPIO_SIG42_IN_SEL  (BIT(6))
#define GPIO_SIG42_IN_SEL_M  (BIT(6))
#define GPIO_SIG42_IN_SEL_V  0x1
#define GPIO_SIG42_IN_SEL_S  6
/* GPIO_FUNC42_IN_INV_SEL : R/W ;bitpos:[5] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC42_IN_INV_SEL  (BIT(5))
#define GPIO_FUNC42_IN_INV_SEL_M  (BIT(5))
#define GPIO_FUNC42_IN_INV_SEL_V  0x1
#define GPIO_FUNC42_IN_INV_SEL_S  5
/* GPIO_FUNC42_IN_SEL : R/W ;bitpos:[4:0] ;default: 5'h0 ; */
/*description: */
#define GPIO_FUNC42_IN_SEL  0x0000001F
#define GPIO_FUNC42_IN_SEL_M  ((GPIO_FUNC42_IN_SEL_V)<<(GPIO_FUNC42_IN_SEL_S))
#define GPIO_FUNC42_IN_SEL_V  0x1F
#define GPIO_FUNC42_IN_SEL_S  0

#define GPIO_FUNC43_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x200)
/* GPIO_SIG43_IN_SEL : R/W ;bitpos:[6] ;default: 1'h0 ; */
/*description: */
#define GPIO_SIG43_IN_SEL  (BIT(6))
#define GPIO_SIG43_IN_SEL_M  (BIT(6))
#define GPIO_SIG43_IN_SEL_V  0x1
#define GPIO_SIG43_IN_SEL_S  6
/* GPIO_FUNC43_IN_INV_SEL : R/W ;bitpos:[5] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC43_IN_INV_SEL  (BIT(5))
#define GPIO_FUNC43_IN_INV_SEL_M  (BIT(5))
#define GPIO_FUNC43_IN_INV_SEL_V  0x1
#define GPIO_FUNC43_IN_INV_SEL_S  5
/* GPIO_FUNC43_IN_SEL : R/W ;bitpos:[4:0] ;default: 5'h0 ; */
/*description: */
#define GPIO_FUNC43_IN_SEL  0x0000001F
#define GPIO_FUNC43_IN_SEL_M  ((GPIO_FUNC43_IN_SEL_V)<<(GPIO_FUNC43_IN_SEL_S))
#define GPIO_FUNC43_IN_SEL_V  0x1F
#define GPIO_FUNC43_IN_SEL_S  0

#define GPIO_FUNC44_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x204)
/* GPIO_SIG44_IN_SEL : R/W ;bitpos:[6] ;default: 1'h0 ; */
/*description: */
#define GPIO_SIG44_IN_SEL  (BIT(6))
#define GPIO_SIG44_IN_SEL_M  (BIT(6))
#define GPIO_SIG44_IN_SEL_V  0x1
#define GPIO_SIG44_IN_SEL_S  6
/* GPIO_FUNC44_IN_INV_SEL : R/W ;bitpos:[5] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC44_IN_INV_SEL  (BIT(5))
#define GPIO_FUNC44_IN_INV_SEL_M  (BIT(5))
#define GPIO_FUNC44_IN_INV_SEL_V  0x1
#define GPIO_FUNC44_IN_INV_SEL_S  5
/* GPIO_FUNC44_IN_SEL : R/W ;bitpos:[4:0] ;default: 5'h0 ; */
/*description: */
#define GPIO_FUNC44_IN_SEL  0x0000001F
#define GPIO_FUNC44_IN_SEL_M  ((GPIO_FUNC44_IN_SEL_V)<<(GPIO_FUNC44_IN_SEL_S))
#define GPIO_FUNC44_IN_SEL_V  0x1F
#define GPIO_FUNC44_IN_SEL_S  0

#define GPIO_FUNC45_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x208)
/* GPIO_SIG45_IN_SEL : R/W ;bitpos:[6] ;default: 1'h0 ; */
/*description: */
#define GPIO_SIG45_IN_SEL  (BIT(6))
#define GPIO_SIG45_IN_SEL_M  (BIT(6))
#define GPIO_SIG45_IN_SEL_V  0x1
#define GPIO_SIG45_IN_SEL_S  6
/* GPIO_FUNC45_IN_INV_SEL : R/W ;bitpos:[5] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC45_IN_INV_SEL  (BIT(5))
#define GPIO_FUNC45_IN_INV_SEL_M  (BIT(5))
#define GPIO_FUNC45_IN_INV_SEL_V  0x1
#define GPIO_FUNC45_IN_INV_SEL_S  5
/* GPIO_FUNC45_IN_SEL : R/W ;bitpos:[4:0] ;default: 5'h0 ; */
/*description: */
#define GPIO_FUNC45_IN_SEL  0x0000001F
#define GPIO_FUNC45_IN_SEL_M  ((GPIO_FUNC45_IN_SEL_V)<<(GPIO_FUNC45_IN_SEL_S))
#define GPIO_FUNC45_IN_SEL_V  0x1F
#define GPIO_FUNC45_IN_SEL_S  0

#define GPIO_FUNC46_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x20C)
/* GPIO_SIG46_IN_SEL : R/W ;bitpos:[6] ;default: 1'h0 ; */
/*description: */
#define GPIO_SIG46_IN_SEL  (BIT(6))
#define GPIO_SIG46_IN_SEL_M  (BIT(6))
#define GPIO_SIG46_IN_SEL_V  0x1
#define GPIO_SIG46_IN_SEL_S  6
/* GPIO_FUNC46_IN_INV_SEL : R/W ;bitpos:[5] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC46_IN_INV_SEL  (BIT(5))
#define GPIO_FUNC46_IN_INV_SEL_M  (BIT(5))
#define GPIO_FUNC46_IN_INV_SEL_V  0x1
#define GPIO_FUNC46_IN_INV_SEL_S  5
/* GPIO_FUNC46_IN_SEL : R/W ;bitpos:[4:0] ;default: 5'h0 ; */
/*description: */
#define GPIO_FUNC46_IN_SEL  0x0000001F
#define GPIO_FUNC46_IN_SEL_M  ((GPIO_FUNC46_IN_SEL_V)<<(GPIO_FUNC46_IN_SEL_S))
#define GPIO_FUNC46_IN_SEL_V  0x1F
#define GPIO_FUNC46_IN_SEL_S  0

#define GPIO_FUNC47_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x210)
/* GPIO_SIG47_IN_SEL : R/W ;bitpos:[6] ;default: 1'h0 ; */
/*description: */
#define GPIO_SIG47_IN_SEL  (BIT(6))
#define GPIO_SIG47_IN_SEL_M  (BIT(6))
#define GPIO_SIG47_IN_SEL_V  0x1
#define GPIO_SIG47_IN_SEL_S  6
/* GPIO_FUNC47_IN_INV_SEL : R/W ;bitpos:[5] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC47_IN_INV_SEL  (BIT(5))
#define GPIO_FUNC47_IN_INV_SEL_M  (BIT(5))
#define GPIO_FUNC47_IN_INV_SEL_V  0x1
#define GPIO_FUNC47_IN_INV_SEL_S  5
/* GPIO_FUNC47_IN_SEL : R/W ;bitpos:[4:0] ;default: 5'h0 ; */
/*description: */
#define GPIO_FUNC47_IN_SEL  0x0000001F
#define GPIO_FUNC47_IN_SEL_M  ((GPIO_FUNC47_IN_SEL_V)<<(GPIO_FUNC47_IN_SEL_S))
#define GPIO_FUNC47_IN_SEL_V  0x1F
#define GPIO_FUNC47_IN_SEL_S  0

#define GPIO_FUNC48_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x214)
/* GPIO_SIG48_IN_SEL : R/W ;bitpos:[6] ;default: 1'h0 ; */
/*description: */
#define GPIO_SIG48_IN_SEL  (BIT(6))
#define GPIO_SIG48_IN_SEL_M  (BIT(6))
#define GPIO_SIG48_IN_SEL_V  0x1
#define GPIO_SIG48_IN_SEL_S  6
/* GPIO_FUNC48_IN_INV_SEL : R/W ;bitpos:[5] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC48_IN_INV_SEL  (BIT(5))
#define GPIO_FUNC48_IN_INV_SEL_M  (BIT(5))
#define GPIO_FUNC48_IN_INV_SEL_V  0x1
#define GPIO_FUNC48_IN_INV_SEL_S  5
/* GPIO_FUNC48_IN_SEL : R/W ;bitpos:[4:0] ;default: 5'h0 ; */
/*description: */
#define GPIO_FUNC48_IN_SEL  0x0000001F
#define GPIO_FUNC48_IN_SEL_M  ((GPIO_FUNC48_IN_SEL_V)<<(GPIO_FUNC48_IN_SEL_S))
#define GPIO_FUNC48_IN_SEL_V  0x1F
#define GPIO_FUNC48_IN_SEL_S  0

#define GPIO_FUNC49_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x218)
/* GPIO_SIG49_IN_SEL : R/W ;bitpos:[6] ;default: 1'h0 ; */
/*description: */
#define GPIO_SIG49_IN_SEL  (BIT(6))
#define GPIO_SIG49_IN_SEL_M  (BIT(6))
#define GPIO_SIG49_IN_SEL_V  0x1
#define GPIO_SIG49_IN_SEL_S  6
/* GPIO_FUNC49_IN_INV_SEL : R/W ;bitpos:[5] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC49_IN_INV_SEL  (BIT(5))
#define GPIO_FUNC49_IN_INV_SEL_M  (BIT(5))
#define GPIO_FUNC49_IN_INV_SEL_V  0x1
#define GPIO_FUNC49_IN_INV_SEL_S  5
/* GPIO_FUNC49_IN_SEL : R/W ;bitpos:[4:0] ;default: 5'h0 ; */
/*description: */
#define GPIO_FUNC49_IN_SEL  0x0000001F
#define GPIO_FUNC49_IN_SEL_M  ((GPIO_FUNC49_IN_SEL_V)<<(GPIO_FUNC49_IN_SEL_S))
#define GPIO_FUNC49_IN_SEL_V  0x1F
#define GPIO_FUNC49_IN_SEL_S  0

#define GPIO_FUNC50_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x21C)
/* GPIO_SIG50_IN_SEL : R/W ;bitpos:[6] ;default: 1'h0 ; */
/*description: */
#define GPIO_SIG50_IN_SEL  (BIT(6))
#define GPIO_SIG50_IN_SEL_M  (BIT(6))
#define GPIO_SIG50_IN_SEL_V  0x1
#define GPIO_SIG50_IN_SEL_S  6
/* GPIO_FUNC50_IN_INV_SEL : R/W ;bitpos:[5] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC50_IN_INV_SEL  (BIT(5))
#define GPIO_FUNC50_IN_INV_SEL_M  (BIT(5))
#define GPIO_FUNC50_IN_INV_SEL_V  0x1
#define GPIO_FUNC50_IN_INV_SEL_S  5
/* GPIO_FUNC50_IN_SEL : R/W ;bitpos:[4:0] ;default: 5'h0 ; */
/*description: */
#define GPIO_FUNC50_IN_SEL  0x0000001F
#define GPIO_FUNC50_IN_SEL_M  ((GPIO_FUNC50_IN_SEL_V)<<(GPIO_FUNC50_IN_SEL_S))
#define GPIO_FUNC50_IN_SEL_V  0x1F
#define GPIO_FUNC50_IN_SEL_S  0

#define GPIO_FUNC51_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x220)
/* GPIO_SIG51_IN_SEL : R/W ;bitpos:[6] ;default: 1'h0 ; */
/*description: */
#define GPIO_SIG51_IN_SEL  (BIT(6))
#define GPIO_SIG51_IN_SEL_M  (BIT(6))
#define GPIO_SIG51_IN_SEL_V  0x1
#define GPIO_SIG51_IN_SEL_S  6
/* GPIO_FUNC51_IN_INV_SEL : R/W ;bitpos:[5] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC51_IN_INV_SEL  (BIT(5))
#define GPIO_FUNC51_IN_INV_SEL_M  (BIT(5))
#define GPIO_FUNC51_IN_INV_SEL_V  0x1
#define GPIO_FUNC51_IN_INV_SEL_S  5
/* GPIO_FUNC51_IN_SEL : R/W ;bitpos:[4:0] ;default: 5'h0 ; */
/*description: */
#define GPIO_FUNC51_IN_SEL  0x0000001F
#define GPIO_FUNC51_IN_SEL_M  ((GPIO_FUNC51_IN_SEL_V)<<(GPIO_FUNC51_IN_SEL_S))
#define GPIO_FUNC51_IN_SEL_V  0x1F
#define GPIO_FUNC51_IN_SEL_S  0

#define GPIO_FUNC52_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x224)
/* GPIO_SIG52_IN_SEL : R/W ;bitpos:[6] ;default: 1'h0 ; */
/*description: */
#define GPIO_SIG52_IN_SEL  (BIT(6))
#define GPIO_SIG52_IN_SEL_M  (BIT(6))
#define GPIO_SIG52_IN_SEL_V  0x1
#define GPIO_SIG52_IN_SEL_S  6
/* GPIO_FUNC52_IN_INV_SEL : R/W ;bitpos:[5] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC52_IN_INV_SEL  (BIT(5))
#define GPIO_FUNC52_IN_INV_SEL_M  (BIT(5))
#define GPIO_FUNC52_IN_INV_SEL_V  0x1
#define GPIO_FUNC52_IN_INV_SEL_S  5
/* GPIO_FUNC52_IN_SEL : R/W ;bitpos:[4:0] ;default: 5'h0 ; */
/*description: */
#define GPIO_FUNC52_IN_SEL  0x0000001F
#define GPIO_FUNC52_IN_SEL_M  ((GPIO_FUNC52_IN_SEL_V)<<(GPIO_FUNC52_IN_SEL_S))
#define GPIO_FUNC52_IN_SEL_V  0x1F
#define GPIO_FUNC52_IN_SEL_S  0

#define GPIO_FUNC53_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x228)
/* GPIO_SIG53_IN_SEL : R/W ;bitpos:[6] ;default: 1'h0 ; */
/*description: */
#define GPIO_SIG53_IN_SEL  (BIT(6))
#define GPIO_SIG53_IN_SEL_M  (BIT(6))
#define GPIO_SIG53_IN_SEL_V  0x1
#define GPIO_SIG53_IN_SEL_S  6
/* GPIO_FUNC53_IN_INV_SEL : R/W ;bitpos:[5] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC53_IN_INV_SEL  (BIT(5))
#define GPIO_FUNC53_IN_INV_SEL_M  (BIT(5))
#define GPIO_FUNC53_IN_INV_SEL_V  0x1
#define GPIO_FUNC53_IN_INV_SEL_S  5
/* GPIO_FUNC53_IN_SEL : R/W ;bitpos:[4:0] ;default: 5'h0 ; */
/*description: */
#define GPIO_FUNC53_IN_SEL  0x0000001F
#define GPIO_FUNC53_IN_SEL_M  ((GPIO_FUNC53_IN_SEL_V)<<(GPIO_FUNC53_IN_SEL_S))
#define GPIO_FUNC53_IN_SEL_V  0x1F
#define GPIO_FUNC53_IN_SEL_S  0

#define GPIO_FUNC54_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x22C)
/* GPIO_SIG54_IN_SEL : R/W ;bitpos:[6] ;default: 1'h0 ; */
/*description: */
#define GPIO_SIG54_IN_SEL  (BIT(6))
#define GPIO_SIG54_IN_SEL_M  (BIT(6))
#define GPIO_SIG54_IN_SEL_V  0x1
#define GPIO_SIG54_IN_SEL_S  6
/* GPIO_FUNC54_IN_INV_SEL : R/W ;bitpos:[5] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC54_IN_INV_SEL  (BIT(5))
#define GPIO_FUNC54_IN_INV_SEL_M  (BIT(5))
#define GPIO_FUNC54_IN_INV_SEL_V  0x1
#define GPIO_FUNC54_IN_INV_SEL_S  5
/* GPIO_FUNC54_IN_SEL : R/W ;bitpos:[4:0] ;default: 5'h0 ; */
/*description: */
#define GPIO_FUNC54_IN_SEL  0x0000001F
#define GPIO_FUNC54_IN_SEL_M  ((GPIO_FUNC54_IN_SEL_V)<<(GPIO_FUNC54_IN_SEL_S))
#define GPIO_FUNC54_IN_SEL_V  0x1F
#define GPIO_FUNC54_IN_SEL_S  0

#define GPIO_FUNC55_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x230)
/* GPIO_SIG55_IN_SEL : R/W ;bitpos:[6] ;default: 1'h0 ; */
/*description: */
#define GPIO_SIG55_IN_SEL  (BIT(6))
#define GPIO_SIG55_IN_SEL_M  (BIT(6))
#define GPIO_SIG55_IN_SEL_V  0x1
#define GPIO_SIG55_IN_SEL_S  6
/* GPIO_FUNC55_IN_INV_SEL : R/W ;bitpos:[5] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC55_IN_INV_SEL  (BIT(5))
#define GPIO_FUNC55_IN_INV_SEL_M  (BIT(5))
#define GPIO_FUNC55_IN_INV_SEL_V  0x1
#define GPIO_FUNC55_IN_INV_SEL_S  5
/* GPIO_FUNC55_IN_SEL : R/W ;bitpos:[4:0] ;default: 5'h0 ; */
/*description: */
#define GPIO_FUNC55_IN_SEL  0x0000001F
#define GPIO_FUNC55_IN_SEL_M  ((GPIO_FUNC55_IN_SEL_V)<<(GPIO_FUNC55_IN_SEL_S))
#define GPIO_FUNC55_IN_SEL_V  0x1F
#define GPIO_FUNC55_IN_SEL_S  0

#define GPIO_FUNC56_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x234)
/* GPIO_SIG56_IN_SEL : R/W ;bitpos:[6] ;default: 1'h0 ; */
/*description: */
#define GPIO_SIG56_IN_SEL  (BIT(6))
#define GPIO_SIG56_IN_SEL_M  (BIT(6))
#define GPIO_SIG56_IN_SEL_V  0x1
#define GPIO_SIG56_IN_SEL_S  6
/* GPIO_FUNC56_IN_INV_SEL : R/W ;bitpos:[5] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC56_IN_INV_SEL  (BIT(5))
#define GPIO_FUNC56_IN_INV_SEL_M  (BIT(5))
#define GPIO_FUNC56_IN_INV_SEL_V  0x1
#define GPIO_FUNC56_IN_INV_SEL_S  5
/* GPIO_FUNC56_IN_SEL : R/W ;bitpos:[4:0] ;default: 5'h0 ; */
/*description: */
#define GPIO_FUNC56_IN_SEL  0x0000001F
#define GPIO_FUNC56_IN_SEL_M  ((GPIO_FUNC56_IN_SEL_V)<<(GPIO_FUNC56_IN_SEL_S))
#define GPIO_FUNC56_IN_SEL_V  0x1F
#define GPIO_FUNC56_IN_SEL_S  0

#define GPIO_FUNC57_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x238)
/* GPIO_SIG57_IN_SEL : R/W ;bitpos:[6] ;default: 1'h0 ; */
/*description: */
#define GPIO_SIG57_IN_SEL  (BIT(6))
#define GPIO_SIG57_IN_SEL_M  (BIT(6))
#define GPIO_SIG57_IN_SEL_V  0x1
#define GPIO_SIG57_IN_SEL_S  6
/* GPIO_FUNC57_IN_INV_SEL : R/W ;bitpos:[5] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC57_IN_INV_SEL  (BIT(5))
#define GPIO_FUNC57_IN_INV_SEL_M  (BIT(5))
#define GPIO_FUNC57_IN_INV_SEL_V  0x1
#define GPIO_FUNC57_IN_INV_SEL_S  5
/* GPIO_FUNC57_IN_SEL : R/W ;bitpos:[4:0] ;default: 5'h0 ; */
/*description: */
#define GPIO_FUNC57_IN_SEL  0x0000001F
#define GPIO_FUNC57_IN_SEL_M  ((GPIO_FUNC57_IN_SEL_V)<<(GPIO_FUNC57_IN_SEL_S))
#define GPIO_FUNC57_IN_SEL_V  0x1F
#define GPIO_FUNC57_IN_SEL_S  0

#define GPIO_FUNC58_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x23C)
/* GPIO_SIG58_IN_SEL : R/W ;bitpos:[6] ;default: 1'h0 ; */
/*description: */
#define GPIO_SIG58_IN_SEL  (BIT(6))
#define GPIO_SIG58_IN_SEL_M  (BIT(6))
#define GPIO_SIG58_IN_SEL_V  0x1
#define GPIO_SIG58_IN_SEL_S  6
/* GPIO_FUNC58_IN_INV_SEL : R/W ;bitpos:[5] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC58_IN_INV_SEL  (BIT(5))
#define GPIO_FUNC58_IN_INV_SEL_M  (BIT(5))
#define GPIO_FUNC58_IN_INV_SEL_V  0x1
#define GPIO_FUNC58_IN_INV_SEL_S  5
/* GPIO_FUNC58_IN_SEL : R/W ;bitpos:[4:0] ;default: 5'h0 ; */
/*description: */
#define GPIO_FUNC58_IN_SEL  0x0000001F
#define GPIO_FUNC58_IN_SEL_M  ((GPIO_FUNC58_IN_SEL_V)<<(GPIO_FUNC58_IN_SEL_S))
#define GPIO_FUNC58_IN_SEL_V  0x1F
#define GPIO_FUNC58_IN_SEL_S  0

#define GPIO_FUNC59_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x240)
/* GPIO_SIG59_IN_SEL : R/W ;bitpos:[6] ;default: 1'h0 ; */
/*description: */
#define GPIO_SIG59_IN_SEL  (BIT(6))
#define GPIO_SIG59_IN_SEL_M  (BIT(6))
#define GPIO_SIG59_IN_SEL_V  0x1
#define GPIO_SIG59_IN_SEL_S  6
/* GPIO_FUNC59_IN_INV_SEL : R/W ;bitpos:[5] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC59_IN_INV_SEL  (BIT(5))
#define GPIO_FUNC59_IN_INV_SEL_M  (BIT(5))
#define GPIO_FUNC59_IN_INV_SEL_V  0x1
#define GPIO_FUNC59_IN_INV_SEL_S  5
/* GPIO_FUNC59_IN_SEL : R/W ;bitpos:[4:0] ;default: 5'h0 ; */
/*description: */
#define GPIO_FUNC59_IN_SEL  0x0000001F
#define GPIO_FUNC59_IN_SEL_M  ((GPIO_FUNC59_IN_SEL_V)<<(GPIO_FUNC59_IN_SEL_S))
#define GPIO_FUNC59_IN_SEL_V  0x1F
#define GPIO_FUNC59_IN_SEL_S  0

#define GPIO_FUNC60_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x244)
/* GPIO_SIG60_IN_SEL : R/W ;bitpos:[6] ;default: 1'h0 ; */
/*description: */
#define GPIO_SIG60_IN_SEL  (BIT(6))
#define GPIO_SIG60_IN_SEL_M  (BIT(6))
#define GPIO_SIG60_IN_SEL_V  0x1
#define GPIO_SIG60_IN_SEL_S  6
/* GPIO_FUNC60_IN_INV_SEL : R/W ;bitpos:[5] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC60_IN_INV_SEL  (BIT(5))
#define GPIO_FUNC60_IN_INV_SEL_M  (BIT(5))
#define GPIO_FUNC60_IN_INV_SEL_V  0x1
#define GPIO_FUNC60_IN_INV_SEL_S  5
/* GPIO_FUNC60_IN_SEL : R/W ;bitpos:[4:0] ;default: 5'h0 ; */
/*description: */
#define GPIO_FUNC60_IN_SEL  0x0000001F
#define GPIO_FUNC60_IN_SEL_M  ((GPIO_FUNC60_IN_SEL_V)<<(GPIO_FUNC60_IN_SEL_S))
#define GPIO_FUNC60_IN_SEL_V  0x1F
#define GPIO_FUNC60_IN_SEL_S  0

#define GPIO_FUNC61_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x248)
/* GPIO_SIG61_IN_SEL : R/W ;bitpos:[6] ;default: 1'h0 ; */
/*description: */
#define GPIO_SIG61_IN_SEL  (BIT(6))
#define GPIO_SIG61_IN_SEL_M  (BIT(6))
#define GPIO_SIG61_IN_SEL_V  0x1
#define GPIO_SIG61_IN_SEL_S  6
/* GPIO_FUNC61_IN_INV_SEL : R/W ;bitpos:[5] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC61_IN_INV_SEL  (BIT(5))
#define GPIO_FUNC61_IN_INV_SEL_M  (BIT(5))
#define GPIO_FUNC61_IN_INV_SEL_V  0x1
#define GPIO_FUNC61_IN_INV_SEL_S  5
/* GPIO_FUNC61_IN_SEL : R/W ;bitpos:[4:0] ;default: 5'h0 ; */
/*description: */
#define GPIO_FUNC61_IN_SEL  0x0000001F
#define GPIO_FUNC61_IN_SEL_M  ((GPIO_FUNC61_IN_SEL_V)<<(GPIO_FUNC61_IN_SEL_S))
#define GPIO_FUNC61_IN_SEL_V  0x1F
#define GPIO_FUNC61_IN_SEL_S  0

#define GPIO_FUNC62_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x24C)
/* GPIO_SIG62_IN_SEL : R/W ;bitpos:[6] ;default: 1'h0 ; */
/*description: */
#define GPIO_SIG62_IN_SEL  (BIT(6))
#define GPIO_SIG62_IN_SEL_M  (BIT(6))
#define GPIO_SIG62_IN_SEL_V  0x1
#define GPIO_SIG62_IN_SEL_S  6
/* GPIO_FUNC62_IN_INV_SEL : R/W ;bitpos:[5] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC62_IN_INV_SEL  (BIT(5))
#define GPIO_FUNC62_IN_INV_SEL_M  (BIT(5))
#define GPIO_FUNC62_IN_INV_SEL_V  0x1
#define GPIO_FUNC62_IN_INV_SEL_S  5
/* GPIO_FUNC62_IN_SEL : R/W ;bitpos:[4:0] ;default: 5'h0 ; */
/*description: */
#define GPIO_FUNC62_IN_SEL  0x0000001F
#define GPIO_FUNC62_IN_SEL_M  ((GPIO_FUNC62_IN_SEL_V)<<(GPIO_FUNC62_IN_SEL_S))
#define GPIO_FUNC62_IN_SEL_V  0x1F
#define GPIO_FUNC62_IN_SEL_S  0

#define GPIO_FUNC63_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x250)
/* GPIO_SIG63_IN_SEL : R/W ;bitpos:[6] ;default: 1'h0 ; */
/*description: */
#define GPIO_SIG63_IN_SEL  (BIT(6))
#define GPIO_SIG63_IN_SEL_M  (BIT(6))
#define GPIO_SIG63_IN_SEL_V  0x1
#define GPIO_SIG63_IN_SEL_S  6
/* GPIO_FUNC63_IN_INV_SEL : R/W ;bitpos:[5] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC63_IN_INV_SEL  (BIT(5))
#define GPIO_FUNC63_IN_INV_SEL_M  (BIT(5))
#define GPIO_FUNC63_IN_INV_SEL_V  0x1
#define GPIO_FUNC63_IN_INV_SEL_S  5
/* GPIO_FUNC63_IN_SEL : R/W ;bitpos:[4:0] ;default: 5'h0 ; */
/*description: */
#define GPIO_FUNC63_IN_SEL  0x0000001F
#define GPIO_FUNC63_IN_SEL_M  ((GPIO_FUNC63_IN_SEL_V)<<(GPIO_FUNC63_IN_SEL_S))
#define GPIO_FUNC63_IN_SEL_V  0x1F
#define GPIO_FUNC63_IN_SEL_S  0

#define GPIO_FUNC64_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x254)
/* GPIO_SIG64_IN_SEL : R/W ;bitpos:[6] ;default: 1'h0 ; */
/*description: */
#define GPIO_SIG64_IN_SEL  (BIT(6))
#define GPIO_SIG64_IN_SEL_M  (BIT(6))
#define GPIO_SIG64_IN_SEL_V  0x1
#define GPIO_SIG64_IN_SEL_S  6
/* GPIO_FUNC64_IN_INV_SEL : R/W ;bitpos:[5] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC64_IN_INV_SEL  (BIT(5))
#define GPIO_FUNC64_IN_INV_SEL_M  (BIT(5))
#define GPIO_FUNC64_IN_INV_SEL_V  0x1
#define GPIO_FUNC64_IN_INV_SEL_S  5
/* GPIO_FUNC64_IN_SEL : R/W ;bitpos:[4:0] ;default: 5'h0 ; */
/*description: */
#define GPIO_FUNC64_IN_SEL  0x0000001F
#define GPIO_FUNC64_IN_SEL_M  ((GPIO_FUNC64_IN_SEL_V)<<(GPIO_FUNC64_IN_SEL_S))
#define GPIO_FUNC64_IN_SEL_V  0x1F
#define GPIO_FUNC64_IN_SEL_S  0

#define GPIO_FUNC65_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x258)
/* GPIO_SIG65_IN_SEL : R/W ;bitpos:[6] ;default: 1'h0 ; */
/*description: */
#define GPIO_SIG65_IN_SEL  (BIT(6))
#define GPIO_SIG65_IN_SEL_M  (BIT(6))
#define GPIO_SIG65_IN_SEL_V  0x1
#define GPIO_SIG65_IN_SEL_S  6
/* GPIO_FUNC65_IN_INV_SEL : R/W ;bitpos:[5] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC65_IN_INV_SEL  (BIT(5))
#define GPIO_FUNC65_IN_INV_SEL_M  (BIT(5))
#define GPIO_FUNC65_IN_INV_SEL_V  0x1
#define GPIO_FUNC65_IN_INV_SEL_S  5
/* GPIO_FUNC65_IN_SEL : R/W ;bitpos:[4:0] ;default: 5'h0 ; */
/*description: */
#define GPIO_FUNC65_IN_SEL  0x0000001F
#define GPIO_FUNC65_IN_SEL_M  ((GPIO_FUNC65_IN_SEL_V)<<(GPIO_FUNC65_IN_SEL_S))
#define GPIO_FUNC65_IN_SEL_V  0x1F
#define GPIO_FUNC65_IN_SEL_S  0

#define GPIO_FUNC66_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x25C)
/* GPIO_SIG66_IN_SEL : R/W ;bitpos:[6] ;default: 1'h0 ; */
/*description: */
#define GPIO_SIG66_IN_SEL  (BIT(6))
#define GPIO_SIG66_IN_SEL_M  (BIT(6))
#define GPIO_SIG66_IN_SEL_V  0x1
#define GPIO_SIG66_IN_SEL_S  6
/* GPIO_FUNC66_IN_INV_SEL : R/W ;bitpos:[5] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC66_IN_INV_SEL  (BIT(5))
#define GPIO_FUNC66_IN_INV_SEL_M  (BIT(5))
#define GPIO_FUNC66_IN_INV_SEL_V  0x1
#define GPIO_FUNC66_IN_INV_SEL_S  5
/* GPIO_FUNC66_IN_SEL : R/W ;bitpos:[4:0] ;default: 5'h0 ; */
/*description: */
#define GPIO_FUNC66_IN_SEL  0x0000001F
#define GPIO_FUNC66_IN_SEL_M  ((GPIO_FUNC66_IN_SEL_V)<<(GPIO_FUNC66_IN_SEL_S))
#define GPIO_FUNC66_IN_SEL_V  0x1F
#define GPIO_FUNC66_IN_SEL_S  0

#define GPIO_FUNC67_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x260)
/* GPIO_SIG67_IN_SEL : R/W ;bitpos:[6] ;default: 1'h0 ; */
/*description: */
#define GPIO_SIG67_IN_SEL  (BIT(6))
#define GPIO_SIG67_IN_SEL_M  (BIT(6))
#define GPIO_SIG67_IN_SEL_V  0x1
#define GPIO_SIG67_IN_SEL_S  6
/* GPIO_FUNC67_IN_INV_SEL : R/W ;bitpos:[5] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC67_IN_INV_SEL  (BIT(5))
#define GPIO_FUNC67_IN_INV_SEL_M  (BIT(5))
#define GPIO_FUNC67_IN_INV_SEL_V  0x1
#define GPIO_FUNC67_IN_INV_SEL_S  5
/* GPIO_FUNC67_IN_SEL : R/W ;bitpos:[4:0] ;default: 5'h0 ; */
/*description: */
#define GPIO_FUNC67_IN_SEL  0x0000001F
#define GPIO_FUNC67_IN_SEL_M  ((GPIO_FUNC67_IN_SEL_V)<<(GPIO_FUNC67_IN_SEL_S))
#define GPIO_FUNC67_IN_SEL_V  0x1F
#define GPIO_FUNC67_IN_SEL_S  0

#define GPIO_FUNC68_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x264)
/* GPIO_SIG68_IN_SEL : R/W ;bitpos:[6] ;default: 1'h0 ; */
/*description: */
#define GPIO_SIG68_IN_SEL  (BIT(6))
#define GPIO_SIG68_IN_SEL_M  (BIT(6))
#define GPIO_SIG68_IN_SEL_V  0x1
#define GPIO_SIG68_IN_SEL_S  6
/* GPIO_FUNC68_IN_INV_SEL : R/W ;bitpos:[5] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC68_IN_INV_SEL  (BIT(5))
#define GPIO_FUNC68_IN_INV_SEL_M  (BIT(5))
#define GPIO_FUNC68_IN_INV_SEL_V  0x1
#define GPIO_FUNC68_IN_INV_SEL_S  5
/* GPIO_FUNC68_IN_SEL : R/W ;bitpos:[4:0] ;default: 5'h0 ; */
/*description: */
#define GPIO_FUNC68_IN_SEL  0x0000001F
#define GPIO_FUNC68_IN_SEL_M  ((GPIO_FUNC68_IN_SEL_V)<<(GPIO_FUNC68_IN_SEL_S))
#define GPIO_FUNC68_IN_SEL_V  0x1F
#define GPIO_FUNC68_IN_SEL_S  0

#define GPIO_FUNC69_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x268)
/* GPIO_SIG69_IN_SEL : R/W ;bitpos:[6] ;default: 1'h0 ; */
/*description: */
#define GPIO_SIG69_IN_SEL  (BIT(6))
#define GPIO_SIG69_IN_SEL_M  (BIT(6))
#define GPIO_SIG69_IN_SEL_V  0x1
#define GPIO_SIG69_IN_SEL_S  6
/* GPIO_FUNC69_IN_INV_SEL : R/W ;bitpos:[5] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC69_IN_INV_SEL  (BIT(5))
#define GPIO_FUNC69_IN_INV_SEL_M  (BIT(5))
#define GPIO_FUNC69_IN_INV_SEL_V  0x1
#define GPIO_FUNC69_IN_INV_SEL_S  5
/* GPIO_FUNC69_IN_SEL : R/W ;bitpos:[4:0] ;default: 5'h0 ; */
/*description: */
#define GPIO_FUNC69_IN_SEL  0x0000001F
#define GPIO_FUNC69_IN_SEL_M  ((GPIO_FUNC69_IN_SEL_V)<<(GPIO_FUNC69_IN_SEL_S))
#define GPIO_FUNC69_IN_SEL_V  0x1F
#define GPIO_FUNC69_IN_SEL_S  0

#define GPIO_FUNC70_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x26C)
/* GPIO_SIG70_IN_SEL : R/W ;bitpos:[6] ;default: 1'h0 ; */
/*description: */
#define GPIO_SIG70_IN_SEL  (BIT(6))
#define GPIO_SIG70_IN_SEL_M  (BIT(6))
#define GPIO_SIG70_IN_SEL_V  0x1
#define GPIO_SIG70_IN_SEL_S  6
/* GPIO_FUNC70_IN_INV_SEL : R/W ;bitpos:[5] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC70_IN_INV_SEL  (BIT(5))
#define GPIO_FUNC70_IN_INV_SEL_M  (BIT(5))
#define GPIO_FUNC70_IN_INV_SEL_V  0x1
#define GPIO_FUNC70_IN_INV_SEL_S  5
/* GPIO_FUNC70_IN_SEL : R/W ;bitpos:[4:0] ;default: 5'h0 ; */
/*description: */
#define GPIO_FUNC70_IN_SEL  0x0000001F
#define GPIO_FUNC70_IN_SEL_M  ((GPIO_FUNC70_IN_SEL_V)<<(GPIO_FUNC70_IN_SEL_S))
#define GPIO_FUNC70_IN_SEL_V  0x1F
#define GPIO_FUNC70_IN_SEL_S  0

#define GPIO_FUNC71_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x270)
/* GPIO_SIG71_IN_SEL : R/W ;bitpos:[6] ;default: 1'h0 ; */
/*description: */
#define GPIO_SIG71_IN_SEL  (BIT(6))
#define GPIO_SIG71_IN_SEL_M  (BIT(6))
#define GPIO_SIG71_IN_SEL_V  0x1
#define GPIO_SIG71_IN_SEL_S  6
/* GPIO_FUNC71_IN_INV_SEL : R/W ;bitpos:[5] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC71_IN_INV_SEL  (BIT(5))
#define GPIO_FUNC71_IN_INV_SEL_M  (BIT(5))
#define GPIO_FUNC71_IN_INV_SEL_V  0x1
#define GPIO_FUNC71_IN_INV_SEL_S  5
/* GPIO_FUNC71_IN_SEL : R/W ;bitpos:[4:0] ;default: 5'h0 ; */
/*description: */
#define GPIO_FUNC71_IN_SEL  0x0000001F
#define GPIO_FUNC71_IN_SEL_M  ((GPIO_FUNC71_IN_SEL_V)<<(GPIO_FUNC71_IN_SEL_S))
#define GPIO_FUNC71_IN_SEL_V  0x1F
#define GPIO_FUNC71_IN_SEL_S  0

#define GPIO_FUNC72_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x274)
/* GPIO_SIG72_IN_SEL : R/W ;bitpos:[6] ;default: 1'h0 ; */
/*description: */
#define GPIO_SIG72_IN_SEL  (BIT(6))
#define GPIO_SIG72_IN_SEL_M  (BIT(6))
#define GPIO_SIG72_IN_SEL_V  0x1
#define GPIO_SIG72_IN_SEL_S  6
/* GPIO_FUNC72_IN_INV_SEL : R/W ;bitpos:[5] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC72_IN_INV_SEL  (BIT(5))
#define GPIO_FUNC72_IN_INV_SEL_M  (BIT(5))
#define GPIO_FUNC72_IN_INV_SEL_V  0x1
#define GPIO_FUNC72_IN_INV_SEL_S  5
/* GPIO_FUNC72_IN_SEL : R/W ;bitpos:[4:0] ;default: 5'h0 ; */
/*description: */
#define GPIO_FUNC72_IN_SEL  0x0000001F
#define GPIO_FUNC72_IN_SEL_M  ((GPIO_FUNC72_IN_SEL_V)<<(GPIO_FUNC72_IN_SEL_S))
#define GPIO_FUNC72_IN_SEL_V  0x1F
#define GPIO_FUNC72_IN_SEL_S  0

#define GPIO_FUNC73_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x278)
/* GPIO_SIG73_IN_SEL : R/W ;bitpos:[6] ;default: 1'h0 ; */
/*description: */
#define GPIO_SIG73_IN_SEL  (BIT(6))
#define GPIO_SIG73_IN_SEL_M  (BIT(6))
#define GPIO_SIG73_IN_SEL_V  0x1
#define GPIO_SIG73_IN_SEL_S  6
/* GPIO_FUNC73_IN_INV_SEL : R/W ;bitpos:[5] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC73_IN_INV_SEL  (BIT(5))
#define GPIO_FUNC73_IN_INV_SEL_M  (BIT(5))
#define GPIO_FUNC73_IN_INV_SEL_V  0x1
#define GPIO_FUNC73_IN_INV_SEL_S  5
/* GPIO_FUNC73_IN_SEL : R/W ;bitpos:[4:0] ;default: 5'h0 ; */
/*description: */
#define GPIO_FUNC73_IN_SEL  0x0000001F
#define GPIO_FUNC73_IN_SEL_M  ((GPIO_FUNC73_IN_SEL_V)<<(GPIO_FUNC73_IN_SEL_S))
#define GPIO_FUNC73_IN_SEL_V  0x1F
#define GPIO_FUNC73_IN_SEL_S  0

#define GPIO_FUNC74_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x27C)
/* GPIO_SIG74_IN_SEL : R/W ;bitpos:[6] ;default: 1'h0 ; */
/*description: */
#define GPIO_SIG74_IN_SEL  (BIT(6))
#define GPIO_SIG74_IN_SEL_M  (BIT(6))
#define GPIO_SIG74_IN_SEL_V  0x1
#define GPIO_SIG74_IN_SEL_S  6
/* GPIO_FUNC74_IN_INV_SEL : R/W ;bitpos:[5] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC74_IN_INV_SEL  (BIT(5))
#define GPIO_FUNC74_IN_INV_SEL_M  (BIT(5))
#define GPIO_FUNC74_IN_INV_SEL_V  0x1
#define GPIO_FUNC74_IN_INV_SEL_S  5
/* GPIO_FUNC74_IN_SEL : R/W ;bitpos:[4:0] ;default: 5'h0 ; */
/*description: */
#define GPIO_FUNC74_IN_SEL  0x0000001F
#define GPIO_FUNC74_IN_SEL_M  ((GPIO_FUNC74_IN_SEL_V)<<(GPIO_FUNC74_IN_SEL_S))
#define GPIO_FUNC74_IN_SEL_V  0x1F
#define GPIO_FUNC74_IN_SEL_S  0

#define GPIO_FUNC75_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x280)
/* GPIO_SIG75_IN_SEL : R/W ;bitpos:[6] ;default: 1'h0 ; */
/*description: */
#define GPIO_SIG75_IN_SEL  (BIT(6))
#define GPIO_SIG75_IN_SEL_M  (BIT(6))
#define GPIO_SIG75_IN_SEL_V  0x1
#define GPIO_SIG75_IN_SEL_S  6
/* GPIO_FUNC75_IN_INV_SEL : R/W ;bitpos:[5] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC75_IN_INV_SEL  (BIT(5))
#define GPIO_FUNC75_IN_INV_SEL_M  (BIT(5))
#define GPIO_FUNC75_IN_INV_SEL_V  0x1
#define GPIO_FUNC75_IN_INV_SEL_S  5
/* GPIO_FUNC75_IN_SEL : R/W ;bitpos:[4:0] ;default: 5'h0 ; */
/*description: */
#define GPIO_FUNC75_IN_SEL  0x0000001F
#define GPIO_FUNC75_IN_SEL_M  ((GPIO_FUNC75_IN_SEL_V)<<(GPIO_FUNC75_IN_SEL_S))
#define GPIO_FUNC75_IN_SEL_V  0x1F
#define GPIO_FUNC75_IN_SEL_S  0

#define GPIO_FUNC76_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x284)
/* GPIO_SIG76_IN_SEL : R/W ;bitpos:[6] ;default: 1'h0 ; */
/*description: */
#define GPIO_SIG76_IN_SEL  (BIT(6))
#define GPIO_SIG76_IN_SEL_M  (BIT(6))
#define GPIO_SIG76_IN_SEL_V  0x1
#define GPIO_SIG76_IN_SEL_S  6
/* GPIO_FUNC76_IN_INV_SEL : R/W ;bitpos:[5] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC76_IN_INV_SEL  (BIT(5))
#define GPIO_FUNC76_IN_INV_SEL_M  (BIT(5))
#define GPIO_FUNC76_IN_INV_SEL_V  0x1
#define GPIO_FUNC76_IN_INV_SEL_S  5
/* GPIO_FUNC76_IN_SEL : R/W ;bitpos:[4:0] ;default: 5'h0 ; */
/*description: */
#define GPIO_FUNC76_IN_SEL  0x0000001F
#define GPIO_FUNC76_IN_SEL_M  ((GPIO_FUNC76_IN_SEL_V)<<(GPIO_FUNC76_IN_SEL_S))
#define GPIO_FUNC76_IN_SEL_V  0x1F
#define GPIO_FUNC76_IN_SEL_S  0

#define GPIO_FUNC77_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x288)
/* GPIO_SIG77_IN_SEL : R/W ;bitpos:[6] ;default: 1'h0 ; */
/*description: */
#define GPIO_SIG77_IN_SEL  (BIT(6))
#define GPIO_SIG77_IN_SEL_M  (BIT(6))
#define GPIO_SIG77_IN_SEL_V  0x1
#define GPIO_SIG77_IN_SEL_S  6
/* GPIO_FUNC77_IN_INV_SEL : R/W ;bitpos:[5] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC77_IN_INV_SEL  (BIT(5))
#define GPIO_FUNC77_IN_INV_SEL_M  (BIT(5))
#define GPIO_FUNC77_IN_INV_SEL_V  0x1
#define GPIO_FUNC77_IN_INV_SEL_S  5
/* GPIO_FUNC77_IN_SEL : R/W ;bitpos:[4:0] ;default: 5'h0 ; */
/*description: */
#define GPIO_FUNC77_IN_SEL  0x0000001F
#define GPIO_FUNC77_IN_SEL_M  ((GPIO_FUNC77_IN_SEL_V)<<(GPIO_FUNC77_IN_SEL_S))
#define GPIO_FUNC77_IN_SEL_V  0x1F
#define GPIO_FUNC77_IN_SEL_S  0

#define GPIO_FUNC78_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x28C)
/* GPIO_SIG78_IN_SEL : R/W ;bitpos:[6] ;default: 1'h0 ; */
/*description: */
#define GPIO_SIG78_IN_SEL  (BIT(6))
#define GPIO_SIG78_IN_SEL_M  (BIT(6))
#define GPIO_SIG78_IN_SEL_V  0x1
#define GPIO_SIG78_IN_SEL_S  6
/* GPIO_FUNC78_IN_INV_SEL : R/W ;bitpos:[5] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC78_IN_INV_SEL  (BIT(5))
#define GPIO_FUNC78_IN_INV_SEL_M  (BIT(5))
#define GPIO_FUNC78_IN_INV_SEL_V  0x1
#define GPIO_FUNC78_IN_INV_SEL_S  5
/* GPIO_FUNC78_IN_SEL : R/W ;bitpos:[4:0] ;default: 5'h0 ; */
/*description: */
#define GPIO_FUNC78_IN_SEL  0x0000001F
#define GPIO_FUNC78_IN_SEL_M  ((GPIO_FUNC78_IN_SEL_V)<<(GPIO_FUNC78_IN_SEL_S))
#define GPIO_FUNC78_IN_SEL_V  0x1F
#define GPIO_FUNC78_IN_SEL_S  0

#define GPIO_FUNC79_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x290)
/* GPIO_SIG79_IN_SEL : R/W ;bitpos:[6] ;default: 1'h0 ; */
/*description: */
#define GPIO_SIG79_IN_SEL  (BIT(6))
#define GPIO_SIG79_IN_SEL_M  (BIT(6))
#define GPIO_SIG79_IN_SEL_V  0x1
#define GPIO_SIG79_IN_SEL_S  6
/* GPIO_FUNC79_IN_INV_SEL : R/W ;bitpos:[5] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC79_IN_INV_SEL  (BIT(5))
#define GPIO_FUNC79_IN_INV_SEL_M  (BIT(5))
#define GPIO_FUNC79_IN_INV_SEL_V  0x1
#define GPIO_FUNC79_IN_INV_SEL_S  5
/* GPIO_FUNC79_IN_SEL : R/W ;bitpos:[4:0] ;default: 5'h0 ; */
/*description: */
#define GPIO_FUNC79_IN_SEL  0x0000001F
#define GPIO_FUNC79_IN_SEL_M  ((GPIO_FUNC79_IN_SEL_V)<<(GPIO_FUNC79_IN_SEL_S))
#define GPIO_FUNC79_IN_SEL_V  0x1F
#define GPIO_FUNC79_IN_SEL_S  0

#define GPIO_FUNC80_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x294)
/* GPIO_SIG80_IN_SEL : R/W ;bitpos:[6] ;default: 1'h0 ; */
/*description: */
#define GPIO_SIG80_IN_SEL  (BIT(6))
#define GPIO_SIG80_IN_SEL_M  (BIT(6))
#define GPIO_SIG80_IN_SEL_V  0x1
#define GPIO_SIG80_IN_SEL_S  6
/* GPIO_FUNC80_IN_INV_SEL : R/W ;bitpos:[5] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC80_IN_INV_SEL  (BIT(5))
#define GPIO_FUNC80_IN_INV_SEL_M  (BIT(5))
#define GPIO_FUNC80_IN_INV_SEL_V  0x1
#define GPIO_FUNC80_IN_INV_SEL_S  5
/* GPIO_FUNC80_IN_SEL : R/W ;bitpos:[4:0] ;default: 5'h0 ; */
/*description: */
#define GPIO_FUNC80_IN_SEL  0x0000001F
#define GPIO_FUNC80_IN_SEL_M  ((GPIO_FUNC80_IN_SEL_V)<<(GPIO_FUNC80_IN_SEL_S))
#define GPIO_FUNC80_IN_SEL_V  0x1F
#define GPIO_FUNC80_IN_SEL_S  0

#define GPIO_FUNC81_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x298)
/* GPIO_SIG81_IN_SEL : R/W ;bitpos:[6] ;default: 1'h0 ; */
/*description: */
#define GPIO_SIG81_IN_SEL  (BIT(6))
#define GPIO_SIG81_IN_SEL_M  (BIT(6))
#define GPIO_SIG81_IN_SEL_V  0x1
#define GPIO_SIG81_IN_SEL_S  6
/* GPIO_FUNC81_IN_INV_SEL : R/W ;bitpos:[5] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC81_IN_INV_SEL  (BIT(5))
#define GPIO_FUNC81_IN_INV_SEL_M  (BIT(5))
#define GPIO_FUNC81_IN_INV_SEL_V  0x1
#define GPIO_FUNC81_IN_INV_SEL_S  5
/* GPIO_FUNC81_IN_SEL : R/W ;bitpos:[4:0] ;default: 5'h0 ; */
/*description: */
#define GPIO_FUNC81_IN_SEL  0x0000001F
#define GPIO_FUNC81_IN_SEL_M  ((GPIO_FUNC81_IN_SEL_V)<<(GPIO_FUNC81_IN_SEL_S))
#define GPIO_FUNC81_IN_SEL_V  0x1F
#define GPIO_FUNC81_IN_SEL_S  0

#define GPIO_FUNC82_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x29C)
/* GPIO_SIG82_IN_SEL : R/W ;bitpos:[6] ;default: 1'h0 ; */
/*description: */
#define GPIO_SIG82_IN_SEL  (BIT(6))
#define GPIO_SIG82_IN_SEL_M  (BIT(6))
#define GPIO_SIG82_IN_SEL_V  0x1
#define GPIO_SIG82_IN_SEL_S  6
/* GPIO_FUNC82_IN_INV_SEL : R/W ;bitpos:[5] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC82_IN_INV_SEL  (BIT(5))
#define GPIO_FUNC82_IN_INV_SEL_M  (BIT(5))
#define GPIO_FUNC82_IN_INV_SEL_V  0x1
#define GPIO_FUNC82_IN_INV_SEL_S  5
/* GPIO_FUNC82_IN_SEL : R/W ;bitpos:[4:0] ;default: 5'h0 ; */
/*description: */
#define GPIO_FUNC82_IN_SEL  0x0000001F
#define GPIO_FUNC82_IN_SEL_M  ((GPIO_FUNC82_IN_SEL_V)<<(GPIO_FUNC82_IN_SEL_S))
#define GPIO_FUNC82_IN_SEL_V  0x1F
#define GPIO_FUNC82_IN_SEL_S  0

#define GPIO_FUNC83_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x2A0)
/* GPIO_SIG83_IN_SEL : R/W ;bitpos:[6] ;default: 1'h0 ; */
/*description: */
#define GPIO_SIG83_IN_SEL  (BIT(6))
#define GPIO_SIG83_IN_SEL_M  (BIT(6))
#define GPIO_SIG83_IN_SEL_V  0x1
#define GPIO_SIG83_IN_SEL_S  6
/* GPIO_FUNC83_IN_INV_SEL : R/W ;bitpos:[5] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC83_IN_INV_SEL  (BIT(5))
#define GPIO_FUNC83_IN_INV_SEL_M  (BIT(5))
#define GPIO_FUNC83_IN_INV_SEL_V  0x1
#define GPIO_FUNC83_IN_INV_SEL_S  5
/* GPIO_FUNC83_IN_SEL : R/W ;bitpos:[4:0] ;default: 5'h0 ; */
/*description: */
#define GPIO_FUNC83_IN_SEL  0x0000001F
#define GPIO_FUNC83_IN_SEL_M  ((GPIO_FUNC83_IN_SEL_V)<<(GPIO_FUNC83_IN_SEL_S))
#define GPIO_FUNC83_IN_SEL_V  0x1F
#define GPIO_FUNC83_IN_SEL_S  0

#define GPIO_FUNC84_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x2A4)
/* GPIO_SIG84_IN_SEL : R/W ;bitpos:[6] ;default: 1'h0 ; */
/*description: */
#define GPIO_SIG84_IN_SEL  (BIT(6))
#define GPIO_SIG84_IN_SEL_M  (BIT(6))
#define GPIO_SIG84_IN_SEL_V  0x1
#define GPIO_SIG84_IN_SEL_S  6
/* GPIO_FUNC84_IN_INV_SEL : R/W ;bitpos:[5] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC84_IN_INV_SEL  (BIT(5))
#define GPIO_FUNC84_IN_INV_SEL_M  (BIT(5))
#define GPIO_FUNC84_IN_INV_SEL_V  0x1
#define GPIO_FUNC84_IN_INV_SEL_S  5
/* GPIO_FUNC84_IN_SEL : R/W ;bitpos:[4:0] ;default: 5'h0 ; */
/*description: */
#define GPIO_FUNC84_IN_SEL  0x0000001F
#define GPIO_FUNC84_IN_SEL_M  ((GPIO_FUNC84_IN_SEL_V)<<(GPIO_FUNC84_IN_SEL_S))
#define GPIO_FUNC84_IN_SEL_V  0x1F
#define GPIO_FUNC84_IN_SEL_S  0

#define GPIO_FUNC85_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x2A8)
/* GPIO_SIG85_IN_SEL : R/W ;bitpos:[6] ;default: 1'h0 ; */
/*description: */
#define GPIO_SIG85_IN_SEL  (BIT(6))
#define GPIO_SIG85_IN_SEL_M  (BIT(6))
#define GPIO_SIG85_IN_SEL_V  0x1
#define GPIO_SIG85_IN_SEL_S  6
/* GPIO_FUNC85_IN_INV_SEL : R/W ;bitpos:[5] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC85_IN_INV_SEL  (BIT(5))
#define GPIO_FUNC85_IN_INV_SEL_M  (BIT(5))
#define GPIO_FUNC85_IN_INV_SEL_V  0x1
#define GPIO_FUNC85_IN_INV_SEL_S  5
/* GPIO_FUNC85_IN_SEL : R/W ;bitpos:[4:0] ;default: 5'h0 ; */
/*description: */
#define GPIO_FUNC85_IN_SEL  0x0000001F
#define GPIO_FUNC85_IN_SEL_M  ((GPIO_FUNC85_IN_SEL_V)<<(GPIO_FUNC85_IN_SEL_S))
#define GPIO_FUNC85_IN_SEL_V  0x1F
#define GPIO_FUNC85_IN_SEL_S  0

#define GPIO_FUNC86_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x2AC)
/* GPIO_SIG86_IN_SEL : R/W ;bitpos:[6] ;default: 1'h0 ; */
/*description: */
#define GPIO_SIG86_IN_SEL  (BIT(6))
#define GPIO_SIG86_IN_SEL_M  (BIT(6))
#define GPIO_SIG86_IN_SEL_V  0x1
#define GPIO_SIG86_IN_SEL_S  6
/* GPIO_FUNC86_IN_INV_SEL : R/W ;bitpos:[5] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC86_IN_INV_SEL  (BIT(5))
#define GPIO_FUNC86_IN_INV_SEL_M  (BIT(5))
#define GPIO_FUNC86_IN_INV_SEL_V  0x1
#define GPIO_FUNC86_IN_INV_SEL_S  5
/* GPIO_FUNC86_IN_SEL : R/W ;bitpos:[4:0] ;default: 5'h0 ; */
/*description: */
#define GPIO_FUNC86_IN_SEL  0x0000001F
#define GPIO_FUNC86_IN_SEL_M  ((GPIO_FUNC86_IN_SEL_V)<<(GPIO_FUNC86_IN_SEL_S))
#define GPIO_FUNC86_IN_SEL_V  0x1F
#define GPIO_FUNC86_IN_SEL_S  0

#define GPIO_FUNC87_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x2B0)
/* GPIO_SIG87_IN_SEL : R/W ;bitpos:[6] ;default: 1'h0 ; */
/*description: */
#define GPIO_SIG87_IN_SEL  (BIT(6))
#define GPIO_SIG87_IN_SEL_M  (BIT(6))
#define GPIO_SIG87_IN_SEL_V  0x1
#define GPIO_SIG87_IN_SEL_S  6
/* GPIO_FUNC87_IN_INV_SEL : R/W ;bitpos:[5] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC87_IN_INV_SEL  (BIT(5))
#define GPIO_FUNC87_IN_INV_SEL_M  (BIT(5))
#define GPIO_FUNC87_IN_INV_SEL_V  0x1
#define GPIO_FUNC87_IN_INV_SEL_S  5
/* GPIO_FUNC87_IN_SEL : R/W ;bitpos:[4:0] ;default: 5'h0 ; */
/*description: */
#define GPIO_FUNC87_IN_SEL  0x0000001F
#define GPIO_FUNC87_IN_SEL_M  ((GPIO_FUNC87_IN_SEL_V)<<(GPIO_FUNC87_IN_SEL_S))
#define GPIO_FUNC87_IN_SEL_V  0x1F
#define GPIO_FUNC87_IN_SEL_S  0

#define GPIO_FUNC88_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x2B4)
/* GPIO_SIG88_IN_SEL : R/W ;bitpos:[6] ;default: 1'h0 ; */
/*description: */
#define GPIO_SIG88_IN_SEL  (BIT(6))
#define GPIO_SIG88_IN_SEL_M  (BIT(6))
#define GPIO_SIG88_IN_SEL_V  0x1
#define GPIO_SIG88_IN_SEL_S  6
/* GPIO_FUNC88_IN_INV_SEL : R/W ;bitpos:[5] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC88_IN_INV_SEL  (BIT(5))
#define GPIO_FUNC88_IN_INV_SEL_M  (BIT(5))
#define GPIO_FUNC88_IN_INV_SEL_V  0x1
#define GPIO_FUNC88_IN_INV_SEL_S  5
/* GPIO_FUNC88_IN_SEL : R/W ;bitpos:[4:0] ;default: 5'h0 ; */
/*description: */
#define GPIO_FUNC88_IN_SEL  0x0000001F
#define GPIO_FUNC88_IN_SEL_M  ((GPIO_FUNC88_IN_SEL_V)<<(GPIO_FUNC88_IN_SEL_S))
#define GPIO_FUNC88_IN_SEL_V  0x1F
#define GPIO_FUNC88_IN_SEL_S  0

#define GPIO_FUNC89_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x2B8)
/* GPIO_SIG89_IN_SEL : R/W ;bitpos:[6] ;default: 1'h0 ; */
/*description: */
#define GPIO_SIG89_IN_SEL  (BIT(6))
#define GPIO_SIG89_IN_SEL_M  (BIT(6))
#define GPIO_SIG89_IN_SEL_V  0x1
#define GPIO_SIG89_IN_SEL_S  6
/* GPIO_FUNC89_IN_INV_SEL : R/W ;bitpos:[5] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC89_IN_INV_SEL  (BIT(5))
#define GPIO_FUNC89_IN_INV_SEL_M  (BIT(5))
#define GPIO_FUNC89_IN_INV_SEL_V  0x1
#define GPIO_FUNC89_IN_INV_SEL_S  5
/* GPIO_FUNC89_IN_SEL : R/W ;bitpos:[4:0] ;default: 5'h0 ; */
/*description: */
#define GPIO_FUNC89_IN_SEL  0x0000001F
#define GPIO_FUNC89_IN_SEL_M  ((GPIO_FUNC89_IN_SEL_V)<<(GPIO_FUNC89_IN_SEL_S))
#define GPIO_FUNC89_IN_SEL_V  0x1F
#define GPIO_FUNC89_IN_SEL_S  0

#define GPIO_FUNC90_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x2BC)
/* GPIO_SIG90_IN_SEL : R/W ;bitpos:[6] ;default: 1'h0 ; */
/*description: */
#define GPIO_SIG90_IN_SEL  (BIT(6))
#define GPIO_SIG90_IN_SEL_M  (BIT(6))
#define GPIO_SIG90_IN_SEL_V  0x1
#define GPIO_SIG90_IN_SEL_S  6
/* GPIO_FUNC90_IN_INV_SEL : R/W ;bitpos:[5] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC90_IN_INV_SEL  (BIT(5))
#define GPIO_FUNC90_IN_INV_SEL_M  (BIT(5))
#define GPIO_FUNC90_IN_INV_SEL_V  0x1
#define GPIO_FUNC90_IN_INV_SEL_S  5
/* GPIO_FUNC90_IN_SEL : R/W ;bitpos:[4:0] ;default: 5'h0 ; */
/*description: */
#define GPIO_FUNC90_IN_SEL  0x0000001F
#define GPIO_FUNC90_IN_SEL_M  ((GPIO_FUNC90_IN_SEL_V)<<(GPIO_FUNC90_IN_SEL_S))
#define GPIO_FUNC90_IN_SEL_V  0x1F
#define GPIO_FUNC90_IN_SEL_S  0

#define GPIO_FUNC91_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x2C0)
/* GPIO_SIG91_IN_SEL : R/W ;bitpos:[6] ;default: 1'h0 ; */
/*description: */
#define GPIO_SIG91_IN_SEL  (BIT(6))
#define GPIO_SIG91_IN_SEL_M  (BIT(6))
#define GPIO_SIG91_IN_SEL_V  0x1
#define GPIO_SIG91_IN_SEL_S  6
/* GPIO_FUNC91_IN_INV_SEL : R/W ;bitpos:[5] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC91_IN_INV_SEL  (BIT(5))
#define GPIO_FUNC91_IN_INV_SEL_M  (BIT(5))
#define GPIO_FUNC91_IN_INV_SEL_V  0x1
#define GPIO_FUNC91_IN_INV_SEL_S  5
/* GPIO_FUNC91_IN_SEL : R/W ;bitpos:[4:0] ;default: 5'h0 ; */
/*description: */
#define GPIO_FUNC91_IN_SEL  0x0000001F
#define GPIO_FUNC91_IN_SEL_M  ((GPIO_FUNC91_IN_SEL_V)<<(GPIO_FUNC91_IN_SEL_S))
#define GPIO_FUNC91_IN_SEL_V  0x1F
#define GPIO_FUNC91_IN_SEL_S  0

#define GPIO_FUNC92_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x2C4)
/* GPIO_SIG92_IN_SEL : R/W ;bitpos:[6] ;default: 1'h0 ; */
/*description: */
#define GPIO_SIG92_IN_SEL  (BIT(6))
#define GPIO_SIG92_IN_SEL_M  (BIT(6))
#define GPIO_SIG92_IN_SEL_V  0x1
#define GPIO_SIG92_IN_SEL_S  6
/* GPIO_FUNC92_IN_INV_SEL : R/W ;bitpos:[5] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC92_IN_INV_SEL  (BIT(5))
#define GPIO_FUNC92_IN_INV_SEL_M  (BIT(5))
#define GPIO_FUNC92_IN_INV_SEL_V  0x1
#define GPIO_FUNC92_IN_INV_SEL_S  5
/* GPIO_FUNC92_IN_SEL : R/W ;bitpos:[4:0] ;default: 5'h0 ; */
/*description: */
#define GPIO_FUNC92_IN_SEL  0x0000001F
#define GPIO_FUNC92_IN_SEL_M  ((GPIO_FUNC92_IN_SEL_V)<<(GPIO_FUNC92_IN_SEL_S))
#define GPIO_FUNC92_IN_SEL_V  0x1F
#define GPIO_FUNC92_IN_SEL_S  0

#define GPIO_FUNC93_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x2C8)
/* GPIO_SIG93_IN_SEL : R/W ;bitpos:[6] ;default: 1'h0 ; */
/*description: */
#define GPIO_SIG93_IN_SEL  (BIT(6))
#define GPIO_SIG93_IN_SEL_M  (BIT(6))
#define GPIO_SIG93_IN_SEL_V  0x1
#define GPIO_SIG93_IN_SEL_S  6
/* GPIO_FUNC93_IN_INV_SEL : R/W ;bitpos:[5] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC93_IN_INV_SEL  (BIT(5))
#define GPIO_FUNC93_IN_INV_SEL_M  (BIT(5))
#define GPIO_FUNC93_IN_INV_SEL_V  0x1
#define GPIO_FUNC93_IN_INV_SEL_S  5
/* GPIO_FUNC93_IN_SEL : R/W ;bitpos:[4:0] ;default: 5'h0 ; */
/*description: */
#define GPIO_FUNC93_IN_SEL  0x0000001F
#define GPIO_FUNC93_IN_SEL_M  ((GPIO_FUNC93_IN_SEL_V)<<(GPIO_FUNC93_IN_SEL_S))
#define GPIO_FUNC93_IN_SEL_V  0x1F
#define GPIO_FUNC93_IN_SEL_S  0

#define GPIO_FUNC94_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x2CC)
/* GPIO_SIG94_IN_SEL : R/W ;bitpos:[6] ;default: 1'h0 ; */
/*description: */
#define GPIO_SIG94_IN_SEL  (BIT(6))
#define GPIO_SIG94_IN_SEL_M  (BIT(6))
#define GPIO_SIG94_IN_SEL_V  0x1
#define GPIO_SIG94_IN_SEL_S  6
/* GPIO_FUNC94_IN_INV_SEL : R/W ;bitpos:[5] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC94_IN_INV_SEL  (BIT(5))
#define GPIO_FUNC94_IN_INV_SEL_M  (BIT(5))
#define GPIO_FUNC94_IN_INV_SEL_V  0x1
#define GPIO_FUNC94_IN_INV_SEL_S  5
/* GPIO_FUNC94_IN_SEL : R/W ;bitpos:[4:0] ;default: 5'h0 ; */
/*description: */
#define GPIO_FUNC94_IN_SEL  0x0000001F
#define GPIO_FUNC94_IN_SEL_M  ((GPIO_FUNC94_IN_SEL_V)<<(GPIO_FUNC94_IN_SEL_S))
#define GPIO_FUNC94_IN_SEL_V  0x1F
#define GPIO_FUNC94_IN_SEL_S  0

#define GPIO_FUNC95_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x2D0)
/* GPIO_SIG95_IN_SEL : R/W ;bitpos:[6] ;default: 1'h0 ; */
/*description: */
#define GPIO_SIG95_IN_SEL  (BIT(6))
#define GPIO_SIG95_IN_SEL_M  (BIT(6))
#define GPIO_SIG95_IN_SEL_V  0x1
#define GPIO_SIG95_IN_SEL_S  6
/* GPIO_FUNC95_IN_INV_SEL : R/W ;bitpos:[5] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC95_IN_INV_SEL  (BIT(5))
#define GPIO_FUNC95_IN_INV_SEL_M  (BIT(5))
#define GPIO_FUNC95_IN_INV_SEL_V  0x1
#define GPIO_FUNC95_IN_INV_SEL_S  5
/* GPIO_FUNC95_IN_SEL : R/W ;bitpos:[4:0] ;default: 5'h0 ; */
/*description: */
#define GPIO_FUNC95_IN_SEL  0x0000001F
#define GPIO_FUNC95_IN_SEL_M  ((GPIO_FUNC95_IN_SEL_V)<<(GPIO_FUNC95_IN_SEL_S))
#define GPIO_FUNC95_IN_SEL_V  0x1F
#define GPIO_FUNC95_IN_SEL_S  0

#define GPIO_FUNC96_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x2D4)
/* GPIO_SIG96_IN_SEL : R/W ;bitpos:[6] ;default: 1'h0 ; */
/*description: */
#define GPIO_SIG96_IN_SEL  (BIT(6))
#define GPIO_SIG96_IN_SEL_M  (BIT(6))
#define GPIO_SIG96_IN_SEL_V  0x1
#define GPIO_SIG96_IN_SEL_S  6
/* GPIO_FUNC96_IN_INV_SEL : R/W ;bitpos:[5] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC96_IN_INV_SEL  (BIT(5))
#define GPIO_FUNC96_IN_INV_SEL_M  (BIT(5))
#define GPIO_FUNC96_IN_INV_SEL_V  0x1
#define GPIO_FUNC96_IN_INV_SEL_S  5
/* GPIO_FUNC96_IN_SEL : R/W ;bitpos:[4:0] ;default: 5'h0 ; */
/*description: */
#define GPIO_FUNC96_IN_SEL  0x0000001F
#define GPIO_FUNC96_IN_SEL_M  ((GPIO_FUNC96_IN_SEL_V)<<(GPIO_FUNC96_IN_SEL_S))
#define GPIO_FUNC96_IN_SEL_V  0x1F
#define GPIO_FUNC96_IN_SEL_S  0

#define GPIO_FUNC97_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x2D8)
/* GPIO_SIG97_IN_SEL : R/W ;bitpos:[6] ;default: 1'h0 ; */
/*description: */
#define GPIO_SIG97_IN_SEL  (BIT(6))
#define GPIO_SIG97_IN_SEL_M  (BIT(6))
#define GPIO_SIG97_IN_SEL_V  0x1
#define GPIO_SIG97_IN_SEL_S  6
/* GPIO_FUNC97_IN_INV_SEL : R/W ;bitpos:[5] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC97_IN_INV_SEL  (BIT(5))
#define GPIO_FUNC97_IN_INV_SEL_M  (BIT(5))
#define GPIO_FUNC97_IN_INV_SEL_V  0x1
#define GPIO_FUNC97_IN_INV_SEL_S  5
/* GPIO_FUNC97_IN_SEL : R/W ;bitpos:[4:0] ;default: 5'h0 ; */
/*description: */
#define GPIO_FUNC97_IN_SEL  0x0000001F
#define GPIO_FUNC97_IN_SEL_M  ((GPIO_FUNC97_IN_SEL_V)<<(GPIO_FUNC97_IN_SEL_S))
#define GPIO_FUNC97_IN_SEL_V  0x1F
#define GPIO_FUNC97_IN_SEL_S  0

#define GPIO_FUNC98_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x2DC)
/* GPIO_SIG98_IN_SEL : R/W ;bitpos:[6] ;default: 1'h0 ; */
/*description: */
#define GPIO_SIG98_IN_SEL  (BIT(6))
#define GPIO_SIG98_IN_SEL_M  (BIT(6))
#define GPIO_SIG98_IN_SEL_V  0x1
#define GPIO_SIG98_IN_SEL_S  6
/* GPIO_FUNC98_IN_INV_SEL : R/W ;bitpos:[5] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC98_IN_INV_SEL  (BIT(5))
#define GPIO_FUNC98_IN_INV_SEL_M  (BIT(5))
#define GPIO_FUNC98_IN_INV_SEL_V  0x1
#define GPIO_FUNC98_IN_INV_SEL_S  5
/* GPIO_FUNC98_IN_SEL : R/W ;bitpos:[4:0] ;default: 5'h0 ; */
/*description: */
#define GPIO_FUNC98_IN_SEL  0x0000001F
#define GPIO_FUNC98_IN_SEL_M  ((GPIO_FUNC98_IN_SEL_V)<<(GPIO_FUNC98_IN_SEL_S))
#define GPIO_FUNC98_IN_SEL_V  0x1F
#define GPIO_FUNC98_IN_SEL_S  0

#define GPIO_FUNC99_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x2E0)
/* GPIO_SIG99_IN_SEL : R/W ;bitpos:[6] ;default: 1'h0 ; */
/*description: */
#define GPIO_SIG99_IN_SEL  (BIT(6))
#define GPIO_SIG99_IN_SEL_M  (BIT(6))
#define GPIO_SIG99_IN_SEL_V  0x1
#define GPIO_SIG99_IN_SEL_S  6
/* GPIO_FUNC99_IN_INV_SEL : R/W ;bitpos:[5] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC99_IN_INV_SEL  (BIT(5))
#define GPIO_FUNC99_IN_INV_SEL_M  (BIT(5))
#define GPIO_FUNC99_IN_INV_SEL_V  0x1
#define GPIO_FUNC99_IN_INV_SEL_S  5
/* GPIO_FUNC99_IN_SEL : R/W ;bitpos:[4:0] ;default: 5'h0 ; */
/*description: */
#define GPIO_FUNC99_IN_SEL  0x0000001F
#define GPIO_FUNC99_IN_SEL_M  ((GPIO_FUNC99_IN_SEL_V)<<(GPIO_FUNC99_IN_SEL_S))
#define GPIO_FUNC99_IN_SEL_V  0x1F
#define GPIO_FUNC99_IN_SEL_S  0

#define GPIO_FUNC100_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x2E4)
/* GPIO_SIG100_IN_SEL : R/W ;bitpos:[6] ;default: 1'h0 ; */
/*description: */
#define GPIO_SIG100_IN_SEL  (BIT(6))
#define GPIO_SIG100_IN_SEL_M  (BIT(6))
#define GPIO_SIG100_IN_SEL_V  0x1
#define GPIO_SIG100_IN_SEL_S  6
/* GPIO_FUNC100_IN_INV_SEL : R/W ;bitpos:[5] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC100_IN_INV_SEL  (BIT(5))
#define GPIO_FUNC100_IN_INV_SEL_M  (BIT(5))
#define GPIO_FUNC100_IN_INV_SEL_V  0x1
#define GPIO_FUNC100_IN_INV_SEL_S  5
/* GPIO_FUNC100_IN_SEL : R/W ;bitpos:[4:0] ;default: 5'h0 ; */
/*description: */
#define GPIO_FUNC100_IN_SEL  0x0000001F
#define GPIO_FUNC100_IN_SEL_M  ((GPIO_FUNC100_IN_SEL_V)<<(GPIO_FUNC100_IN_SEL_S))
#define GPIO_FUNC100_IN_SEL_V  0x1F
#define GPIO_FUNC100_IN_SEL_S  0

#define GPIO_FUNC101_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x2E8)
/* GPIO_SIG101_IN_SEL : R/W ;bitpos:[6] ;default: 1'h0 ; */
/*description: */
#define GPIO_SIG101_IN_SEL  (BIT(6))
#define GPIO_SIG101_IN_SEL_M  (BIT(6))
#define GPIO_SIG101_IN_SEL_V  0x1
#define GPIO_SIG101_IN_SEL_S  6
/* GPIO_FUNC101_IN_INV_SEL : R/W ;bitpos:[5] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC101_IN_INV_SEL  (BIT(5))
#define GPIO_FUNC101_IN_INV_SEL_M  (BIT(5))
#define GPIO_FUNC101_IN_INV_SEL_V  0x1
#define GPIO_FUNC101_IN_INV_SEL_S  5
/* GPIO_FUNC101_IN_SEL : R/W ;bitpos:[4:0] ;default: 5'h0 ; */
/*description: */
#define GPIO_FUNC101_IN_SEL  0x0000001F
#define GPIO_FUNC101_IN_SEL_M  ((GPIO_FUNC101_IN_SEL_V)<<(GPIO_FUNC101_IN_SEL_S))
#define GPIO_FUNC101_IN_SEL_V  0x1F
#define GPIO_FUNC101_IN_SEL_S  0

#define GPIO_FUNC102_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x2EC)
/* GPIO_SIG102_IN_SEL : R/W ;bitpos:[6] ;default: 1'h0 ; */
/*description: */
#define GPIO_SIG102_IN_SEL  (BIT(6))
#define GPIO_SIG102_IN_SEL_M  (BIT(6))
#define GPIO_SIG102_IN_SEL_V  0x1
#define GPIO_SIG102_IN_SEL_S  6
/* GPIO_FUNC102_IN_INV_SEL : R/W ;bitpos:[5] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC102_IN_INV_SEL  (BIT(5))
#define GPIO_FUNC102_IN_INV_SEL_M  (BIT(5))
#define GPIO_FUNC102_IN_INV_SEL_V  0x1
#define GPIO_FUNC102_IN_INV_SEL_S  5
/* GPIO_FUNC102_IN_SEL : R/W ;bitpos:[4:0] ;default: 5'h0 ; */
/*description: */
#define GPIO_FUNC102_IN_SEL  0x0000001F
#define GPIO_FUNC102_IN_SEL_M  ((GPIO_FUNC102_IN_SEL_V)<<(GPIO_FUNC102_IN_SEL_S))
#define GPIO_FUNC102_IN_SEL_V  0x1F
#define GPIO_FUNC102_IN_SEL_S  0

#define GPIO_FUNC103_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x2F0)
/* GPIO_SIG103_IN_SEL : R/W ;bitpos:[6] ;default: 1'h0 ; */
/*description: */
#define GPIO_SIG103_IN_SEL  (BIT(6))
#define GPIO_SIG103_IN_SEL_M  (BIT(6))
#define GPIO_SIG103_IN_SEL_V  0x1
#define GPIO_SIG103_IN_SEL_S  6
/* GPIO_FUNC103_IN_INV_SEL : R/W ;bitpos:[5] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC103_IN_INV_SEL  (BIT(5))
#define GPIO_FUNC103_IN_INV_SEL_M  (BIT(5))
#define GPIO_FUNC103_IN_INV_SEL_V  0x1
#define GPIO_FUNC103_IN_INV_SEL_S  5
/* GPIO_FUNC103_IN_SEL : R/W ;bitpos:[4:0] ;default: 5'h0 ; */
/*description: */
#define GPIO_FUNC103_IN_SEL  0x0000001F
#define GPIO_FUNC103_IN_SEL_M  ((GPIO_FUNC103_IN_SEL_V)<<(GPIO_FUNC103_IN_SEL_S))
#define GPIO_FUNC103_IN_SEL_V  0x1F
#define GPIO_FUNC103_IN_SEL_S  0

#define GPIO_FUNC104_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x2F4)
/* GPIO_SIG104_IN_SEL : R/W ;bitpos:[6] ;default: 1'h0 ; */
/*description: */
#define GPIO_SIG104_IN_SEL  (BIT(6))
#define GPIO_SIG104_IN_SEL_M  (BIT(6))
#define GPIO_SIG104_IN_SEL_V  0x1
#define GPIO_SIG104_IN_SEL_S  6
/* GPIO_FUNC104_IN_INV_SEL : R/W ;bitpos:[5] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC104_IN_INV_SEL  (BIT(5))
#define GPIO_FUNC104_IN_INV_SEL_M  (BIT(5))
#define GPIO_FUNC104_IN_INV_SEL_V  0x1
#define GPIO_FUNC104_IN_INV_SEL_S  5
/* GPIO_FUNC104_IN_SEL : R/W ;bitpos:[4:0] ;default: 5'h0 ; */
/*description: */
#define GPIO_FUNC104_IN_SEL  0x0000001F
#define GPIO_FUNC104_IN_SEL_M  ((GPIO_FUNC104_IN_SEL_V)<<(GPIO_FUNC104_IN_SEL_S))
#define GPIO_FUNC104_IN_SEL_V  0x1F
#define GPIO_FUNC104_IN_SEL_S  0

#define GPIO_FUNC105_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x2F8)
/* GPIO_SIG105_IN_SEL : R/W ;bitpos:[6] ;default: 1'h0 ; */
/*description: */
#define GPIO_SIG105_IN_SEL  (BIT(6))
#define GPIO_SIG105_IN_SEL_M  (BIT(6))
#define GPIO_SIG105_IN_SEL_V  0x1
#define GPIO_SIG105_IN_SEL_S  6
/* GPIO_FUNC105_IN_INV_SEL : R/W ;bitpos:[5] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC105_IN_INV_SEL  (BIT(5))
#define GPIO_FUNC105_IN_INV_SEL_M  (BIT(5))
#define GPIO_FUNC105_IN_INV_SEL_V  0x1
#define GPIO_FUNC105_IN_INV_SEL_S  5
/* GPIO_FUNC105_IN_SEL : R/W ;bitpos:[4:0] ;default: 5'h0 ; */
/*description: */
#define GPIO_FUNC105_IN_SEL  0x0000001F
#define GPIO_FUNC105_IN_SEL_M  ((GPIO_FUNC105_IN_SEL_V)<<(GPIO_FUNC105_IN_SEL_S))
#define GPIO_FUNC105_IN_SEL_V  0x1F
#define GPIO_FUNC105_IN_SEL_S  0

#define GPIO_FUNC106_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x2FC)
/* GPIO_SIG106_IN_SEL : R/W ;bitpos:[6] ;default: 1'h0 ; */
/*description: */
#define GPIO_SIG106_IN_SEL  (BIT(6))
#define GPIO_SIG106_IN_SEL_M  (BIT(6))
#define GPIO_SIG106_IN_SEL_V  0x1
#define GPIO_SIG106_IN_SEL_S  6
/* GPIO_FUNC106_IN_INV_SEL : R/W ;bitpos:[5] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC106_IN_INV_SEL  (BIT(5))
#define GPIO_FUNC106_IN_INV_SEL_M  (BIT(5))
#define GPIO_FUNC106_IN_INV_SEL_V  0x1
#define GPIO_FUNC106_IN_INV_SEL_S  5
/* GPIO_FUNC106_IN_SEL : R/W ;bitpos:[4:0] ;default: 5'h0 ; */
/*description: */
#define GPIO_FUNC106_IN_SEL  0x0000001F
#define GPIO_FUNC106_IN_SEL_M  ((GPIO_FUNC106_IN_SEL_V)<<(GPIO_FUNC106_IN_SEL_S))
#define GPIO_FUNC106_IN_SEL_V  0x1F
#define GPIO_FUNC106_IN_SEL_S  0

#define GPIO_FUNC107_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x300)
/* GPIO_SIG107_IN_SEL : R/W ;bitpos:[6] ;default: 1'h0 ; */
/*description: */
#define GPIO_SIG107_IN_SEL  (BIT(6))
#define GPIO_SIG107_IN_SEL_M  (BIT(6))
#define GPIO_SIG107_IN_SEL_V  0x1
#define GPIO_SIG107_IN_SEL_S  6
/* GPIO_FUNC107_IN_INV_SEL : R/W ;bitpos:[5] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC107_IN_INV_SEL  (BIT(5))
#define GPIO_FUNC107_IN_INV_SEL_M  (BIT(5))
#define GPIO_FUNC107_IN_INV_SEL_V  0x1
#define GPIO_FUNC107_IN_INV_SEL_S  5
/* GPIO_FUNC107_IN_SEL : R/W ;bitpos:[4:0] ;default: 5'h0 ; */
/*description: */
#define GPIO_FUNC107_IN_SEL  0x0000001F
#define GPIO_FUNC107_IN_SEL_M  ((GPIO_FUNC107_IN_SEL_V)<<(GPIO_FUNC107_IN_SEL_S))
#define GPIO_FUNC107_IN_SEL_V  0x1F
#define GPIO_FUNC107_IN_SEL_S  0

#define GPIO_FUNC108_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x304)
/* GPIO_SIG108_IN_SEL : R/W ;bitpos:[6] ;default: 1'h0 ; */
/*description: */
#define GPIO_SIG108_IN_SEL  (BIT(6))
#define GPIO_SIG108_IN_SEL_M  (BIT(6))
#define GPIO_SIG108_IN_SEL_V  0x1
#define GPIO_SIG108_IN_SEL_S  6
/* GPIO_FUNC108_IN_INV_SEL : R/W ;bitpos:[5] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC108_IN_INV_SEL  (BIT(5))
#define GPIO_FUNC108_IN_INV_SEL_M  (BIT(5))
#define GPIO_FUNC108_IN_INV_SEL_V  0x1
#define GPIO_FUNC108_IN_INV_SEL_S  5
/* GPIO_FUNC108_IN_SEL : R/W ;bitpos:[4:0] ;default: 5'h0 ; */
/*description: */
#define GPIO_FUNC108_IN_SEL  0x0000001F
#define GPIO_FUNC108_IN_SEL_M  ((GPIO_FUNC108_IN_SEL_V)<<(GPIO_FUNC108_IN_SEL_S))
#define GPIO_FUNC108_IN_SEL_V  0x1F
#define GPIO_FUNC108_IN_SEL_S  0

#define GPIO_FUNC109_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x308)
/* GPIO_SIG109_IN_SEL : R/W ;bitpos:[6] ;default: 1'h0 ; */
/*description: */
#define GPIO_SIG109_IN_SEL  (BIT(6))
#define GPIO_SIG109_IN_SEL_M  (BIT(6))
#define GPIO_SIG109_IN_SEL_V  0x1
#define GPIO_SIG109_IN_SEL_S  6
/* GPIO_FUNC109_IN_INV_SEL : R/W ;bitpos:[5] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC109_IN_INV_SEL  (BIT(5))
#define GPIO_FUNC109_IN_INV_SEL_M  (BIT(5))
#define GPIO_FUNC109_IN_INV_SEL_V  0x1
#define GPIO_FUNC109_IN_INV_SEL_S  5
/* GPIO_FUNC109_IN_SEL : R/W ;bitpos:[4:0] ;default: 5'h0 ; */
/*description: */
#define GPIO_FUNC109_IN_SEL  0x0000001F
#define GPIO_FUNC109_IN_SEL_M  ((GPIO_FUNC109_IN_SEL_V)<<(GPIO_FUNC109_IN_SEL_S))
#define GPIO_FUNC109_IN_SEL_V  0x1F
#define GPIO_FUNC109_IN_SEL_S  0

#define GPIO_FUNC110_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x30C)
/* GPIO_SIG110_IN_SEL : R/W ;bitpos:[6] ;default: 1'h0 ; */
/*description: */
#define GPIO_SIG110_IN_SEL  (BIT(6))
#define GPIO_SIG110_IN_SEL_M  (BIT(6))
#define GPIO_SIG110_IN_SEL_V  0x1
#define GPIO_SIG110_IN_SEL_S  6
/* GPIO_FUNC110_IN_INV_SEL : R/W ;bitpos:[5] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC110_IN_INV_SEL  (BIT(5))
#define GPIO_FUNC110_IN_INV_SEL_M  (BIT(5))
#define GPIO_FUNC110_IN_INV_SEL_V  0x1
#define GPIO_FUNC110_IN_INV_SEL_S  5
/* GPIO_FUNC110_IN_SEL : R/W ;bitpos:[4:0] ;default: 5'h0 ; */
/*description: */
#define GPIO_FUNC110_IN_SEL  0x0000001F
#define GPIO_FUNC110_IN_SEL_M  ((GPIO_FUNC110_IN_SEL_V)<<(GPIO_FUNC110_IN_SEL_S))
#define GPIO_FUNC110_IN_SEL_V  0x1F
#define GPIO_FUNC110_IN_SEL_S  0

#define GPIO_FUNC111_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x310)
/* GPIO_SIG111_IN_SEL : R/W ;bitpos:[6] ;default: 1'h0 ; */
/*description: */
#define GPIO_SIG111_IN_SEL  (BIT(6))
#define GPIO_SIG111_IN_SEL_M  (BIT(6))
#define GPIO_SIG111_IN_SEL_V  0x1
#define GPIO_SIG111_IN_SEL_S  6
/* GPIO_FUNC111_IN_INV_SEL : R/W ;bitpos:[5] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC111_IN_INV_SEL  (BIT(5))
#define GPIO_FUNC111_IN_INV_SEL_M  (BIT(5))
#define GPIO_FUNC111_IN_INV_SEL_V  0x1
#define GPIO_FUNC111_IN_INV_SEL_S  5
/* GPIO_FUNC111_IN_SEL : R/W ;bitpos:[4:0] ;default: 5'h0 ; */
/*description: */
#define GPIO_FUNC111_IN_SEL  0x0000001F
#define GPIO_FUNC111_IN_SEL_M  ((GPIO_FUNC111_IN_SEL_V)<<(GPIO_FUNC111_IN_SEL_S))
#define GPIO_FUNC111_IN_SEL_V  0x1F
#define GPIO_FUNC111_IN_SEL_S  0

#define GPIO_FUNC112_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x314)
/* GPIO_SIG112_IN_SEL : R/W ;bitpos:[6] ;default: 1'h0 ; */
/*description: */
#define GPIO_SIG112_IN_SEL  (BIT(6))
#define GPIO_SIG112_IN_SEL_M  (BIT(6))
#define GPIO_SIG112_IN_SEL_V  0x1
#define GPIO_SIG112_IN_SEL_S  6
/* GPIO_FUNC112_IN_INV_SEL : R/W ;bitpos:[5] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC112_IN_INV_SEL  (BIT(5))
#define GPIO_FUNC112_IN_INV_SEL_M  (BIT(5))
#define GPIO_FUNC112_IN_INV_SEL_V  0x1
#define GPIO_FUNC112_IN_INV_SEL_S  5
/* GPIO_FUNC112_IN_SEL : R/W ;bitpos:[4:0] ;default: 5'h0 ; */
/*description: */
#define GPIO_FUNC112_IN_SEL  0x0000001F
#define GPIO_FUNC112_IN_SEL_M  ((GPIO_FUNC112_IN_SEL_V)<<(GPIO_FUNC112_IN_SEL_S))
#define GPIO_FUNC112_IN_SEL_V  0x1F
#define GPIO_FUNC112_IN_SEL_S  0

#define GPIO_FUNC113_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x318)
/* GPIO_SIG113_IN_SEL : R/W ;bitpos:[6] ;default: 1'h0 ; */
/*description: */
#define GPIO_SIG113_IN_SEL  (BIT(6))
#define GPIO_SIG113_IN_SEL_M  (BIT(6))
#define GPIO_SIG113_IN_SEL_V  0x1
#define GPIO_SIG113_IN_SEL_S  6
/* GPIO_FUNC113_IN_INV_SEL : R/W ;bitpos:[5] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC113_IN_INV_SEL  (BIT(5))
#define GPIO_FUNC113_IN_INV_SEL_M  (BIT(5))
#define GPIO_FUNC113_IN_INV_SEL_V  0x1
#define GPIO_FUNC113_IN_INV_SEL_S  5
/* GPIO_FUNC113_IN_SEL : R/W ;bitpos:[4:0] ;default: 5'h0 ; */
/*description: */
#define GPIO_FUNC113_IN_SEL  0x0000001F
#define GPIO_FUNC113_IN_SEL_M  ((GPIO_FUNC113_IN_SEL_V)<<(GPIO_FUNC113_IN_SEL_S))
#define GPIO_FUNC113_IN_SEL_V  0x1F
#define GPIO_FUNC113_IN_SEL_S  0

#define GPIO_FUNC114_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x31C)
/* GPIO_SIG114_IN_SEL : R/W ;bitpos:[6] ;default: 1'h0 ; */
/*description: */
#define GPIO_SIG114_IN_SEL  (BIT(6))
#define GPIO_SIG114_IN_SEL_M  (BIT(6))
#define GPIO_SIG114_IN_SEL_V  0x1
#define GPIO_SIG114_IN_SEL_S  6
/* GPIO_FUNC114_IN_INV_SEL : R/W ;bitpos:[5] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC114_IN_INV_SEL  (BIT(5))
#define GPIO_FUNC114_IN_INV_SEL_M  (BIT(5))
#define GPIO_FUNC114_IN_INV_SEL_V  0x1
#define GPIO_FUNC114_IN_INV_SEL_S  5
/* GPIO_FUNC114_IN_SEL : R/W ;bitpos:[4:0] ;default: 5'h0 ; */
/*description: */
#define GPIO_FUNC114_IN_SEL  0x0000001F
#define GPIO_FUNC114_IN_SEL_M  ((GPIO_FUNC114_IN_SEL_V)<<(GPIO_FUNC114_IN_SEL_S))
#define GPIO_FUNC114_IN_SEL_V  0x1F
#define GPIO_FUNC114_IN_SEL_S  0

#define GPIO_FUNC115_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x320)
/* GPIO_SIG115_IN_SEL : R/W ;bitpos:[6] ;default: 1'h0 ; */
/*description: */
#define GPIO_SIG115_IN_SEL  (BIT(6))
#define GPIO_SIG115_IN_SEL_M  (BIT(6))
#define GPIO_SIG115_IN_SEL_V  0x1
#define GPIO_SIG115_IN_SEL_S  6
/* GPIO_FUNC115_IN_INV_SEL : R/W ;bitpos:[5] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC115_IN_INV_SEL  (BIT(5))
#define GPIO_FUNC115_IN_INV_SEL_M  (BIT(5))
#define GPIO_FUNC115_IN_INV_SEL_V  0x1
#define GPIO_FUNC115_IN_INV_SEL_S  5
/* GPIO_FUNC115_IN_SEL : R/W ;bitpos:[4:0] ;default: 5'h0 ; */
/*description: */
#define GPIO_FUNC115_IN_SEL  0x0000001F
#define GPIO_FUNC115_IN_SEL_M  ((GPIO_FUNC115_IN_SEL_V)<<(GPIO_FUNC115_IN_SEL_S))
#define GPIO_FUNC115_IN_SEL_V  0x1F
#define GPIO_FUNC115_IN_SEL_S  0

#define GPIO_FUNC116_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x324)
/* GPIO_SIG116_IN_SEL : R/W ;bitpos:[6] ;default: 1'h0 ; */
/*description: */
#define GPIO_SIG116_IN_SEL  (BIT(6))
#define GPIO_SIG116_IN_SEL_M  (BIT(6))
#define GPIO_SIG116_IN_SEL_V  0x1
#define GPIO_SIG116_IN_SEL_S  6
/* GPIO_FUNC116_IN_INV_SEL : R/W ;bitpos:[5] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC116_IN_INV_SEL  (BIT(5))
#define GPIO_FUNC116_IN_INV_SEL_M  (BIT(5))
#define GPIO_FUNC116_IN_INV_SEL_V  0x1
#define GPIO_FUNC116_IN_INV_SEL_S  5
/* GPIO_FUNC116_IN_SEL : R/W ;bitpos:[4:0] ;default: 5'h0 ; */
/*description: */
#define GPIO_FUNC116_IN_SEL  0x0000001F
#define GPIO_FUNC116_IN_SEL_M  ((GPIO_FUNC116_IN_SEL_V)<<(GPIO_FUNC116_IN_SEL_S))
#define GPIO_FUNC116_IN_SEL_V  0x1F
#define GPIO_FUNC116_IN_SEL_S  0

#define GPIO_FUNC117_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x328)
/* GPIO_SIG117_IN_SEL : R/W ;bitpos:[6] ;default: 1'h0 ; */
/*description: */
#define GPIO_SIG117_IN_SEL  (BIT(6))
#define GPIO_SIG117_IN_SEL_M  (BIT(6))
#define GPIO_SIG117_IN_SEL_V  0x1
#define GPIO_SIG117_IN_SEL_S  6
/* GPIO_FUNC117_IN_INV_SEL : R/W ;bitpos:[5] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC117_IN_INV_SEL  (BIT(5))
#define GPIO_FUNC117_IN_INV_SEL_M  (BIT(5))
#define GPIO_FUNC117_IN_INV_SEL_V  0x1
#define GPIO_FUNC117_IN_INV_SEL_S  5
/* GPIO_FUNC117_IN_SEL : R/W ;bitpos:[4:0] ;default: 5'h0 ; */
/*description: */
#define GPIO_FUNC117_IN_SEL  0x0000001F
#define GPIO_FUNC117_IN_SEL_M  ((GPIO_FUNC117_IN_SEL_V)<<(GPIO_FUNC117_IN_SEL_S))
#define GPIO_FUNC117_IN_SEL_V  0x1F
#define GPIO_FUNC117_IN_SEL_S  0

#define GPIO_FUNC118_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x32C)
/* GPIO_SIG118_IN_SEL : R/W ;bitpos:[6] ;default: 1'h0 ; */
/*description: */
#define GPIO_SIG118_IN_SEL  (BIT(6))
#define GPIO_SIG118_IN_SEL_M  (BIT(6))
#define GPIO_SIG118_IN_SEL_V  0x1
#define GPIO_SIG118_IN_SEL_S  6
/* GPIO_FUNC118_IN_INV_SEL : R/W ;bitpos:[5] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC118_IN_INV_SEL  (BIT(5))
#define GPIO_FUNC118_IN_INV_SEL_M  (BIT(5))
#define GPIO_FUNC118_IN_INV_SEL_V  0x1
#define GPIO_FUNC118_IN_INV_SEL_S  5
/* GPIO_FUNC118_IN_SEL : R/W ;bitpos:[4:0] ;default: 5'h0 ; */
/*description: */
#define GPIO_FUNC118_IN_SEL  0x0000001F
#define GPIO_FUNC118_IN_SEL_M  ((GPIO_FUNC118_IN_SEL_V)<<(GPIO_FUNC118_IN_SEL_S))
#define GPIO_FUNC118_IN_SEL_V  0x1F
#define GPIO_FUNC118_IN_SEL_S  0

#define GPIO_FUNC119_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x330)
/* GPIO_SIG119_IN_SEL : R/W ;bitpos:[6] ;default: 1'h0 ; */
/*description: */
#define GPIO_SIG119_IN_SEL  (BIT(6))
#define GPIO_SIG119_IN_SEL_M  (BIT(6))
#define GPIO_SIG119_IN_SEL_V  0x1
#define GPIO_SIG119_IN_SEL_S  6
/* GPIO_FUNC119_IN_INV_SEL : R/W ;bitpos:[5] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC119_IN_INV_SEL  (BIT(5))
#define GPIO_FUNC119_IN_INV_SEL_M  (BIT(5))
#define GPIO_FUNC119_IN_INV_SEL_V  0x1
#define GPIO_FUNC119_IN_INV_SEL_S  5
/* GPIO_FUNC119_IN_SEL : R/W ;bitpos:[4:0] ;default: 5'h0 ; */
/*description: */
#define GPIO_FUNC119_IN_SEL  0x0000001F
#define GPIO_FUNC119_IN_SEL_M  ((GPIO_FUNC119_IN_SEL_V)<<(GPIO_FUNC119_IN_SEL_S))
#define GPIO_FUNC119_IN_SEL_V  0x1F
#define GPIO_FUNC119_IN_SEL_S  0

#define GPIO_FUNC120_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x334)
/* GPIO_SIG120_IN_SEL : R/W ;bitpos:[6] ;default: 1'h0 ; */
/*description: */
#define GPIO_SIG120_IN_SEL  (BIT(6))
#define GPIO_SIG120_IN_SEL_M  (BIT(6))
#define GPIO_SIG120_IN_SEL_V  0x1
#define GPIO_SIG120_IN_SEL_S  6
/* GPIO_FUNC120_IN_INV_SEL : R/W ;bitpos:[5] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC120_IN_INV_SEL  (BIT(5))
#define GPIO_FUNC120_IN_INV_SEL_M  (BIT(5))
#define GPIO_FUNC120_IN_INV_SEL_V  0x1
#define GPIO_FUNC120_IN_INV_SEL_S  5
/* GPIO_FUNC120_IN_SEL : R/W ;bitpos:[4:0] ;default: 5'h0 ; */
/*description: */
#define GPIO_FUNC120_IN_SEL  0x0000001F
#define GPIO_FUNC120_IN_SEL_M  ((GPIO_FUNC120_IN_SEL_V)<<(GPIO_FUNC120_IN_SEL_S))
#define GPIO_FUNC120_IN_SEL_V  0x1F
#define GPIO_FUNC120_IN_SEL_S  0

#define GPIO_FUNC121_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x338)
/* GPIO_SIG121_IN_SEL : R/W ;bitpos:[6] ;default: 1'h0 ; */
/*description: */
#define GPIO_SIG121_IN_SEL  (BIT(6))
#define GPIO_SIG121_IN_SEL_M  (BIT(6))
#define GPIO_SIG121_IN_SEL_V  0x1
#define GPIO_SIG121_IN_SEL_S  6
/* GPIO_FUNC121_IN_INV_SEL : R/W ;bitpos:[5] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC121_IN_INV_SEL  (BIT(5))
#define GPIO_FUNC121_IN_INV_SEL_M  (BIT(5))
#define GPIO_FUNC121_IN_INV_SEL_V  0x1
#define GPIO_FUNC121_IN_INV_SEL_S  5
/* GPIO_FUNC121_IN_SEL : R/W ;bitpos:[4:0] ;default: 5'h0 ; */
/*description: */
#define GPIO_FUNC121_IN_SEL  0x0000001F
#define GPIO_FUNC121_IN_SEL_M  ((GPIO_FUNC121_IN_SEL_V)<<(GPIO_FUNC121_IN_SEL_S))
#define GPIO_FUNC121_IN_SEL_V  0x1F
#define GPIO_FUNC121_IN_SEL_S  0

#define GPIO_FUNC122_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x33C)
/* GPIO_SIG122_IN_SEL : R/W ;bitpos:[6] ;default: 1'h0 ; */
/*description: */
#define GPIO_SIG122_IN_SEL  (BIT(6))
#define GPIO_SIG122_IN_SEL_M  (BIT(6))
#define GPIO_SIG122_IN_SEL_V  0x1
#define GPIO_SIG122_IN_SEL_S  6
/* GPIO_FUNC122_IN_INV_SEL : R/W ;bitpos:[5] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC122_IN_INV_SEL  (BIT(5))
#define GPIO_FUNC122_IN_INV_SEL_M  (BIT(5))
#define GPIO_FUNC122_IN_INV_SEL_V  0x1
#define GPIO_FUNC122_IN_INV_SEL_S  5
/* GPIO_FUNC122_IN_SEL : R/W ;bitpos:[4:0] ;default: 5'h0 ; */
/*description: */
#define GPIO_FUNC122_IN_SEL  0x0000001F
#define GPIO_FUNC122_IN_SEL_M  ((GPIO_FUNC122_IN_SEL_V)<<(GPIO_FUNC122_IN_SEL_S))
#define GPIO_FUNC122_IN_SEL_V  0x1F
#define GPIO_FUNC122_IN_SEL_S  0

#define GPIO_FUNC123_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x340)
/* GPIO_SIG123_IN_SEL : R/W ;bitpos:[6] ;default: 1'h0 ; */
/*description: */
#define GPIO_SIG123_IN_SEL  (BIT(6))
#define GPIO_SIG123_IN_SEL_M  (BIT(6))
#define GPIO_SIG123_IN_SEL_V  0x1
#define GPIO_SIG123_IN_SEL_S  6
/* GPIO_FUNC123_IN_INV_SEL : R/W ;bitpos:[5] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC123_IN_INV_SEL  (BIT(5))
#define GPIO_FUNC123_IN_INV_SEL_M  (BIT(5))
#define GPIO_FUNC123_IN_INV_SEL_V  0x1
#define GPIO_FUNC123_IN_INV_SEL_S  5
/* GPIO_FUNC123_IN_SEL : R/W ;bitpos:[4:0] ;default: 5'h0 ; */
/*description: */
#define GPIO_FUNC123_IN_SEL  0x0000001F
#define GPIO_FUNC123_IN_SEL_M  ((GPIO_FUNC123_IN_SEL_V)<<(GPIO_FUNC123_IN_SEL_S))
#define GPIO_FUNC123_IN_SEL_V  0x1F
#define GPIO_FUNC123_IN_SEL_S  0

#define GPIO_FUNC124_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x344)
/* GPIO_SIG124_IN_SEL : R/W ;bitpos:[6] ;default: 1'h0 ; */
/*description: */
#define GPIO_SIG124_IN_SEL  (BIT(6))
#define GPIO_SIG124_IN_SEL_M  (BIT(6))
#define GPIO_SIG124_IN_SEL_V  0x1
#define GPIO_SIG124_IN_SEL_S  6
/* GPIO_FUNC124_IN_INV_SEL : R/W ;bitpos:[5] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC124_IN_INV_SEL  (BIT(5))
#define GPIO_FUNC124_IN_INV_SEL_M  (BIT(5))
#define GPIO_FUNC124_IN_INV_SEL_V  0x1
#define GPIO_FUNC124_IN_INV_SEL_S  5
/* GPIO_FUNC124_IN_SEL : R/W ;bitpos:[4:0] ;default: 5'h0 ; */
/*description: */
#define GPIO_FUNC124_IN_SEL  0x0000001F
#define GPIO_FUNC124_IN_SEL_M  ((GPIO_FUNC124_IN_SEL_V)<<(GPIO_FUNC124_IN_SEL_S))
#define GPIO_FUNC124_IN_SEL_V  0x1F
#define GPIO_FUNC124_IN_SEL_S  0

#define GPIO_FUNC125_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x348)
/* GPIO_SIG125_IN_SEL : R/W ;bitpos:[6] ;default: 1'h0 ; */
/*description: */
#define GPIO_SIG125_IN_SEL  (BIT(6))
#define GPIO_SIG125_IN_SEL_M  (BIT(6))
#define GPIO_SIG125_IN_SEL_V  0x1
#define GPIO_SIG125_IN_SEL_S  6
/* GPIO_FUNC125_IN_INV_SEL : R/W ;bitpos:[5] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC125_IN_INV_SEL  (BIT(5))
#define GPIO_FUNC125_IN_INV_SEL_M  (BIT(5))
#define GPIO_FUNC125_IN_INV_SEL_V  0x1
#define GPIO_FUNC125_IN_INV_SEL_S  5
/* GPIO_FUNC125_IN_SEL : R/W ;bitpos:[4:0] ;default: 5'h0 ; */
/*description: */
#define GPIO_FUNC125_IN_SEL  0x0000001F
#define GPIO_FUNC125_IN_SEL_M  ((GPIO_FUNC125_IN_SEL_V)<<(GPIO_FUNC125_IN_SEL_S))
#define GPIO_FUNC125_IN_SEL_V  0x1F
#define GPIO_FUNC125_IN_SEL_S  0

#define GPIO_FUNC126_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x34C)
/* GPIO_SIG126_IN_SEL : R/W ;bitpos:[6] ;default: 1'h0 ; */
/*description: */
#define GPIO_SIG126_IN_SEL  (BIT(6))
#define GPIO_SIG126_IN_SEL_M  (BIT(6))
#define GPIO_SIG126_IN_SEL_V  0x1
#define GPIO_SIG126_IN_SEL_S  6
/* GPIO_FUNC126_IN_INV_SEL : R/W ;bitpos:[5] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC126_IN_INV_SEL  (BIT(5))
#define GPIO_FUNC126_IN_INV_SEL_M  (BIT(5))
#define GPIO_FUNC126_IN_INV_SEL_V  0x1
#define GPIO_FUNC126_IN_INV_SEL_S  5
/* GPIO_FUNC126_IN_SEL : R/W ;bitpos:[4:0] ;default: 5'h0 ; */
/*description: */
#define GPIO_FUNC126_IN_SEL  0x0000001F
#define GPIO_FUNC126_IN_SEL_M  ((GPIO_FUNC126_IN_SEL_V)<<(GPIO_FUNC126_IN_SEL_S))
#define GPIO_FUNC126_IN_SEL_V  0x1F
#define GPIO_FUNC126_IN_SEL_S  0

#define GPIO_FUNC127_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x350)
/* GPIO_SIG127_IN_SEL : R/W ;bitpos:[6] ;default: 1'h0 ; */
/*description: */
#define GPIO_SIG127_IN_SEL  (BIT(6))
#define GPIO_SIG127_IN_SEL_M  (BIT(6))
#define GPIO_SIG127_IN_SEL_V  0x1
#define GPIO_SIG127_IN_SEL_S  6
/* GPIO_FUNC127_IN_INV_SEL : R/W ;bitpos:[5] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC127_IN_INV_SEL  (BIT(5))
#define GPIO_FUNC127_IN_INV_SEL_M  (BIT(5))
#define GPIO_FUNC127_IN_INV_SEL_V  0x1
#define GPIO_FUNC127_IN_INV_SEL_S  5
/* GPIO_FUNC127_IN_SEL : R/W ;bitpos:[4:0] ;default: 5'h0 ; */
/*description: */
#define GPIO_FUNC127_IN_SEL  0x0000001F
#define GPIO_FUNC127_IN_SEL_M  ((GPIO_FUNC127_IN_SEL_V)<<(GPIO_FUNC127_IN_SEL_S))
#define GPIO_FUNC127_IN_SEL_V  0x1F
#define GPIO_FUNC127_IN_SEL_S  0

#define GPIO_FUNC0_OUT_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x554)
/* GPIO_FUNC0_OEN_INV_SEL : R/W ;bitpos:[10] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC0_OEN_INV_SEL  (BIT(10))
#define GPIO_FUNC0_OEN_INV_SEL_M  (BIT(10))
#define GPIO_FUNC0_OEN_INV_SEL_V  0x1
#define GPIO_FUNC0_OEN_INV_SEL_S  10
/* GPIO_FUNC0_OEN_SEL : R/W ;bitpos:[9] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC0_OEN_SEL  (BIT(9))
#define GPIO_FUNC0_OEN_SEL_M  (BIT(9))
#define GPIO_FUNC0_OEN_SEL_V  0x1
#define GPIO_FUNC0_OEN_SEL_S  9
/* GPIO_FUNC0_OUT_INV_SEL : R/W ;bitpos:[8] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC0_OUT_INV_SEL  (BIT(8))
#define GPIO_FUNC0_OUT_INV_SEL_M  (BIT(8))
#define GPIO_FUNC0_OUT_INV_SEL_V  0x1
#define GPIO_FUNC0_OUT_INV_SEL_S  8
/* GPIO_FUNC0_OUT_SEL : R/W ;bitpos:[7:0] ;default: 8'h80 ; */
/*description: */
#define GPIO_FUNC0_OUT_SEL  0x000000FF
#define GPIO_FUNC0_OUT_SEL_M  ((GPIO_FUNC0_OUT_SEL_V)<<(GPIO_FUNC0_OUT_SEL_S))
#define GPIO_FUNC0_OUT_SEL_V  0xFF
#define GPIO_FUNC0_OUT_SEL_S  0

#define GPIO_FUNC1_OUT_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x558)
/* GPIO_FUNC1_OEN_INV_SEL : R/W ;bitpos:[10] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC1_OEN_INV_SEL  (BIT(10))
#define GPIO_FUNC1_OEN_INV_SEL_M  (BIT(10))
#define GPIO_FUNC1_OEN_INV_SEL_V  0x1
#define GPIO_FUNC1_OEN_INV_SEL_S  10
/* GPIO_FUNC1_OEN_SEL : R/W ;bitpos:[9] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC1_OEN_SEL  (BIT(9))
#define GPIO_FUNC1_OEN_SEL_M  (BIT(9))
#define GPIO_FUNC1_OEN_SEL_V  0x1
#define GPIO_FUNC1_OEN_SEL_S  9
/* GPIO_FUNC1_OUT_INV_SEL : R/W ;bitpos:[8] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC1_OUT_INV_SEL  (BIT(8))
#define GPIO_FUNC1_OUT_INV_SEL_M  (BIT(8))
#define GPIO_FUNC1_OUT_INV_SEL_V  0x1
#define GPIO_FUNC1_OUT_INV_SEL_S  8
/* GPIO_FUNC1_OUT_SEL : R/W ;bitpos:[7:0] ;default: 8'h80 ; */
/*description: */
#define GPIO_FUNC1_OUT_SEL  0x000000FF
#define GPIO_FUNC1_OUT_SEL_M  ((GPIO_FUNC1_OUT_SEL_V)<<(GPIO_FUNC1_OUT_SEL_S))
#define GPIO_FUNC1_OUT_SEL_V  0xFF
#define GPIO_FUNC1_OUT_SEL_S  0

#define GPIO_FUNC2_OUT_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x55C)
/* GPIO_FUNC2_OEN_INV_SEL : R/W ;bitpos:[10] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC2_OEN_INV_SEL  (BIT(10))
#define GPIO_FUNC2_OEN_INV_SEL_M  (BIT(10))
#define GPIO_FUNC2_OEN_INV_SEL_V  0x1
#define GPIO_FUNC2_OEN_INV_SEL_S  10
/* GPIO_FUNC2_OEN_SEL : R/W ;bitpos:[9] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC2_OEN_SEL  (BIT(9))
#define GPIO_FUNC2_OEN_SEL_M  (BIT(9))
#define GPIO_FUNC2_OEN_SEL_V  0x1
#define GPIO_FUNC2_OEN_SEL_S  9
/* GPIO_FUNC2_OUT_INV_SEL : R/W ;bitpos:[8] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC2_OUT_INV_SEL  (BIT(8))
#define GPIO_FUNC2_OUT_INV_SEL_M  (BIT(8))
#define GPIO_FUNC2_OUT_INV_SEL_V  0x1
#define GPIO_FUNC2_OUT_INV_SEL_S  8
/* GPIO_FUNC2_OUT_SEL : R/W ;bitpos:[7:0] ;default: 8'h80 ; */
/*description: */
#define GPIO_FUNC2_OUT_SEL  0x000000FF
#define GPIO_FUNC2_OUT_SEL_M  ((GPIO_FUNC2_OUT_SEL_V)<<(GPIO_FUNC2_OUT_SEL_S))
#define GPIO_FUNC2_OUT_SEL_V  0xFF
#define GPIO_FUNC2_OUT_SEL_S  0

#define GPIO_FUNC3_OUT_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x560)
/* GPIO_FUNC3_OEN_INV_SEL : R/W ;bitpos:[10] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC3_OEN_INV_SEL  (BIT(10))
#define GPIO_FUNC3_OEN_INV_SEL_M  (BIT(10))
#define GPIO_FUNC3_OEN_INV_SEL_V  0x1
#define GPIO_FUNC3_OEN_INV_SEL_S  10
/* GPIO_FUNC3_OEN_SEL : R/W ;bitpos:[9] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC3_OEN_SEL  (BIT(9))
#define GPIO_FUNC3_OEN_SEL_M  (BIT(9))
#define GPIO_FUNC3_OEN_SEL_V  0x1
#define GPIO_FUNC3_OEN_SEL_S  9
/* GPIO_FUNC3_OUT_INV_SEL : R/W ;bitpos:[8] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC3_OUT_INV_SEL  (BIT(8))
#define GPIO_FUNC3_OUT_INV_SEL_M  (BIT(8))
#define GPIO_FUNC3_OUT_INV_SEL_V  0x1
#define GPIO_FUNC3_OUT_INV_SEL_S  8
/* GPIO_FUNC3_OUT_SEL : R/W ;bitpos:[7:0] ;default: 8'h80 ; */
/*description: */
#define GPIO_FUNC3_OUT_SEL  0x000000FF
#define GPIO_FUNC3_OUT_SEL_M  ((GPIO_FUNC3_OUT_SEL_V)<<(GPIO_FUNC3_OUT_SEL_S))
#define GPIO_FUNC3_OUT_SEL_V  0xFF
#define GPIO_FUNC3_OUT_SEL_S  0

#define GPIO_FUNC4_OUT_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x564)
/* GPIO_FUNC4_OEN_INV_SEL : R/W ;bitpos:[10] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC4_OEN_INV_SEL  (BIT(10))
#define GPIO_FUNC4_OEN_INV_SEL_M  (BIT(10))
#define GPIO_FUNC4_OEN_INV_SEL_V  0x1
#define GPIO_FUNC4_OEN_INV_SEL_S  10
/* GPIO_FUNC4_OEN_SEL : R/W ;bitpos:[9] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC4_OEN_SEL  (BIT(9))
#define GPIO_FUNC4_OEN_SEL_M  (BIT(9))
#define GPIO_FUNC4_OEN_SEL_V  0x1
#define GPIO_FUNC4_OEN_SEL_S  9
/* GPIO_FUNC4_OUT_INV_SEL : R/W ;bitpos:[8] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC4_OUT_INV_SEL  (BIT(8))
#define GPIO_FUNC4_OUT_INV_SEL_M  (BIT(8))
#define GPIO_FUNC4_OUT_INV_SEL_V  0x1
#define GPIO_FUNC4_OUT_INV_SEL_S  8
/* GPIO_FUNC4_OUT_SEL : R/W ;bitpos:[7:0] ;default: 8'h80 ; */
/*description: */
#define GPIO_FUNC4_OUT_SEL  0x000000FF
#define GPIO_FUNC4_OUT_SEL_M  ((GPIO_FUNC4_OUT_SEL_V)<<(GPIO_FUNC4_OUT_SEL_S))
#define GPIO_FUNC4_OUT_SEL_V  0xFF
#define GPIO_FUNC4_OUT_SEL_S  0

#define GPIO_FUNC5_OUT_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x568)
/* GPIO_FUNC5_OEN_INV_SEL : R/W ;bitpos:[10] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC5_OEN_INV_SEL  (BIT(10))
#define GPIO_FUNC5_OEN_INV_SEL_M  (BIT(10))
#define GPIO_FUNC5_OEN_INV_SEL_V  0x1
#define GPIO_FUNC5_OEN_INV_SEL_S  10
/* GPIO_FUNC5_OEN_SEL : R/W ;bitpos:[9] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC5_OEN_SEL  (BIT(9))
#define GPIO_FUNC5_OEN_SEL_M  (BIT(9))
#define GPIO_FUNC5_OEN_SEL_V  0x1
#define GPIO_FUNC5_OEN_SEL_S  9
/* GPIO_FUNC5_OUT_INV_SEL : R/W ;bitpos:[8] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC5_OUT_INV_SEL  (BIT(8))
#define GPIO_FUNC5_OUT_INV_SEL_M  (BIT(8))
#define GPIO_FUNC5_OUT_INV_SEL_V  0x1
#define GPIO_FUNC5_OUT_INV_SEL_S  8
/* GPIO_FUNC5_OUT_SEL : R/W ;bitpos:[7:0] ;default: 8'h80 ; */
/*description: */
#define GPIO_FUNC5_OUT_SEL  0x000000FF
#define GPIO_FUNC5_OUT_SEL_M  ((GPIO_FUNC5_OUT_SEL_V)<<(GPIO_FUNC5_OUT_SEL_S))
#define GPIO_FUNC5_OUT_SEL_V  0xFF
#define GPIO_FUNC5_OUT_SEL_S  0

#define GPIO_FUNC6_OUT_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x56C)
/* GPIO_FUNC6_OEN_INV_SEL : R/W ;bitpos:[10] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC6_OEN_INV_SEL  (BIT(10))
#define GPIO_FUNC6_OEN_INV_SEL_M  (BIT(10))
#define GPIO_FUNC6_OEN_INV_SEL_V  0x1
#define GPIO_FUNC6_OEN_INV_SEL_S  10
/* GPIO_FUNC6_OEN_SEL : R/W ;bitpos:[9] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC6_OEN_SEL  (BIT(9))
#define GPIO_FUNC6_OEN_SEL_M  (BIT(9))
#define GPIO_FUNC6_OEN_SEL_V  0x1
#define GPIO_FUNC6_OEN_SEL_S  9
/* GPIO_FUNC6_OUT_INV_SEL : R/W ;bitpos:[8] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC6_OUT_INV_SEL  (BIT(8))
#define GPIO_FUNC6_OUT_INV_SEL_M  (BIT(8))
#define GPIO_FUNC6_OUT_INV_SEL_V  0x1
#define GPIO_FUNC6_OUT_INV_SEL_S  8
/* GPIO_FUNC6_OUT_SEL : R/W ;bitpos:[7:0] ;default: 8'h80 ; */
/*description: */
#define GPIO_FUNC6_OUT_SEL  0x000000FF
#define GPIO_FUNC6_OUT_SEL_M  ((GPIO_FUNC6_OUT_SEL_V)<<(GPIO_FUNC6_OUT_SEL_S))
#define GPIO_FUNC6_OUT_SEL_V  0xFF
#define GPIO_FUNC6_OUT_SEL_S  0

#define GPIO_FUNC7_OUT_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x570)
/* GPIO_FUNC7_OEN_INV_SEL : R/W ;bitpos:[10] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC7_OEN_INV_SEL  (BIT(10))
#define GPIO_FUNC7_OEN_INV_SEL_M  (BIT(10))
#define GPIO_FUNC7_OEN_INV_SEL_V  0x1
#define GPIO_FUNC7_OEN_INV_SEL_S  10
/* GPIO_FUNC7_OEN_SEL : R/W ;bitpos:[9] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC7_OEN_SEL  (BIT(9))
#define GPIO_FUNC7_OEN_SEL_M  (BIT(9))
#define GPIO_FUNC7_OEN_SEL_V  0x1
#define GPIO_FUNC7_OEN_SEL_S  9
/* GPIO_FUNC7_OUT_INV_SEL : R/W ;bitpos:[8] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC7_OUT_INV_SEL  (BIT(8))
#define GPIO_FUNC7_OUT_INV_SEL_M  (BIT(8))
#define GPIO_FUNC7_OUT_INV_SEL_V  0x1
#define GPIO_FUNC7_OUT_INV_SEL_S  8
/* GPIO_FUNC7_OUT_SEL : R/W ;bitpos:[7:0] ;default: 8'h80 ; */
/*description: */
#define GPIO_FUNC7_OUT_SEL  0x000000FF
#define GPIO_FUNC7_OUT_SEL_M  ((GPIO_FUNC7_OUT_SEL_V)<<(GPIO_FUNC7_OUT_SEL_S))
#define GPIO_FUNC7_OUT_SEL_V  0xFF
#define GPIO_FUNC7_OUT_SEL_S  0

#define GPIO_FUNC8_OUT_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x574)
/* GPIO_FUNC8_OEN_INV_SEL : R/W ;bitpos:[10] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC8_OEN_INV_SEL  (BIT(10))
#define GPIO_FUNC8_OEN_INV_SEL_M  (BIT(10))
#define GPIO_FUNC8_OEN_INV_SEL_V  0x1
#define GPIO_FUNC8_OEN_INV_SEL_S  10
/* GPIO_FUNC8_OEN_SEL : R/W ;bitpos:[9] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC8_OEN_SEL  (BIT(9))
#define GPIO_FUNC8_OEN_SEL_M  (BIT(9))
#define GPIO_FUNC8_OEN_SEL_V  0x1
#define GPIO_FUNC8_OEN_SEL_S  9
/* GPIO_FUNC8_OUT_INV_SEL : R/W ;bitpos:[8] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC8_OUT_INV_SEL  (BIT(8))
#define GPIO_FUNC8_OUT_INV_SEL_M  (BIT(8))
#define GPIO_FUNC8_OUT_INV_SEL_V  0x1
#define GPIO_FUNC8_OUT_INV_SEL_S  8
/* GPIO_FUNC8_OUT_SEL : R/W ;bitpos:[7:0] ;default: 8'h80 ; */
/*description: */
#define GPIO_FUNC8_OUT_SEL  0x000000FF
#define GPIO_FUNC8_OUT_SEL_M  ((GPIO_FUNC8_OUT_SEL_V)<<(GPIO_FUNC8_OUT_SEL_S))
#define GPIO_FUNC8_OUT_SEL_V  0xFF
#define GPIO_FUNC8_OUT_SEL_S  0

#define GPIO_FUNC9_OUT_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x578)
/* GPIO_FUNC9_OEN_INV_SEL : R/W ;bitpos:[10] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC9_OEN_INV_SEL  (BIT(10))
#define GPIO_FUNC9_OEN_INV_SEL_M  (BIT(10))
#define GPIO_FUNC9_OEN_INV_SEL_V  0x1
#define GPIO_FUNC9_OEN_INV_SEL_S  10
/* GPIO_FUNC9_OEN_SEL : R/W ;bitpos:[9] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC9_OEN_SEL  (BIT(9))
#define GPIO_FUNC9_OEN_SEL_M  (BIT(9))
#define GPIO_FUNC9_OEN_SEL_V  0x1
#define GPIO_FUNC9_OEN_SEL_S  9
/* GPIO_FUNC9_OUT_INV_SEL : R/W ;bitpos:[8] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC9_OUT_INV_SEL  (BIT(8))
#define GPIO_FUNC9_OUT_INV_SEL_M  (BIT(8))
#define GPIO_FUNC9_OUT_INV_SEL_V  0x1
#define GPIO_FUNC9_OUT_INV_SEL_S  8
/* GPIO_FUNC9_OUT_SEL : R/W ;bitpos:[7:0] ;default: 8'h80 ; */
/*description: */
#define GPIO_FUNC9_OUT_SEL  0x000000FF
#define GPIO_FUNC9_OUT_SEL_M  ((GPIO_FUNC9_OUT_SEL_V)<<(GPIO_FUNC9_OUT_SEL_S))
#define GPIO_FUNC9_OUT_SEL_V  0xFF
#define GPIO_FUNC9_OUT_SEL_S  0

#define GPIO_FUNC10_OUT_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x57C)
/* GPIO_FUNC10_OEN_INV_SEL : R/W ;bitpos:[10] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC10_OEN_INV_SEL  (BIT(10))
#define GPIO_FUNC10_OEN_INV_SEL_M  (BIT(10))
#define GPIO_FUNC10_OEN_INV_SEL_V  0x1
#define GPIO_FUNC10_OEN_INV_SEL_S  10
/* GPIO_FUNC10_OEN_SEL : R/W ;bitpos:[9] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC10_OEN_SEL  (BIT(9))
#define GPIO_FUNC10_OEN_SEL_M  (BIT(9))
#define GPIO_FUNC10_OEN_SEL_V  0x1
#define GPIO_FUNC10_OEN_SEL_S  9
/* GPIO_FUNC10_OUT_INV_SEL : R/W ;bitpos:[8] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC10_OUT_INV_SEL  (BIT(8))
#define GPIO_FUNC10_OUT_INV_SEL_M  (BIT(8))
#define GPIO_FUNC10_OUT_INV_SEL_V  0x1
#define GPIO_FUNC10_OUT_INV_SEL_S  8
/* GPIO_FUNC10_OUT_SEL : R/W ;bitpos:[7:0] ;default: 8'h80 ; */
/*description: */
#define GPIO_FUNC10_OUT_SEL  0x000000FF
#define GPIO_FUNC10_OUT_SEL_M  ((GPIO_FUNC10_OUT_SEL_V)<<(GPIO_FUNC10_OUT_SEL_S))
#define GPIO_FUNC10_OUT_SEL_V  0xFF
#define GPIO_FUNC10_OUT_SEL_S  0

#define GPIO_FUNC11_OUT_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x580)
/* GPIO_FUNC11_OEN_INV_SEL : R/W ;bitpos:[10] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC11_OEN_INV_SEL  (BIT(10))
#define GPIO_FUNC11_OEN_INV_SEL_M  (BIT(10))
#define GPIO_FUNC11_OEN_INV_SEL_V  0x1
#define GPIO_FUNC11_OEN_INV_SEL_S  10
/* GPIO_FUNC11_OEN_SEL : R/W ;bitpos:[9] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC11_OEN_SEL  (BIT(9))
#define GPIO_FUNC11_OEN_SEL_M  (BIT(9))
#define GPIO_FUNC11_OEN_SEL_V  0x1
#define GPIO_FUNC11_OEN_SEL_S  9
/* GPIO_FUNC11_OUT_INV_SEL : R/W ;bitpos:[8] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC11_OUT_INV_SEL  (BIT(8))
#define GPIO_FUNC11_OUT_INV_SEL_M  (BIT(8))
#define GPIO_FUNC11_OUT_INV_SEL_V  0x1
#define GPIO_FUNC11_OUT_INV_SEL_S  8
/* GPIO_FUNC11_OUT_SEL : R/W ;bitpos:[7:0] ;default: 8'h80 ; */
/*description: */
#define GPIO_FUNC11_OUT_SEL  0x000000FF
#define GPIO_FUNC11_OUT_SEL_M  ((GPIO_FUNC11_OUT_SEL_V)<<(GPIO_FUNC11_OUT_SEL_S))
#define GPIO_FUNC11_OUT_SEL_V  0xFF
#define GPIO_FUNC11_OUT_SEL_S  0

#define GPIO_FUNC12_OUT_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x584)
/* GPIO_FUNC12_OEN_INV_SEL : R/W ;bitpos:[10] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC12_OEN_INV_SEL  (BIT(10))
#define GPIO_FUNC12_OEN_INV_SEL_M  (BIT(10))
#define GPIO_FUNC12_OEN_INV_SEL_V  0x1
#define GPIO_FUNC12_OEN_INV_SEL_S  10
/* GPIO_FUNC12_OEN_SEL : R/W ;bitpos:[9] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC12_OEN_SEL  (BIT(9))
#define GPIO_FUNC12_OEN_SEL_M  (BIT(9))
#define GPIO_FUNC12_OEN_SEL_V  0x1
#define GPIO_FUNC12_OEN_SEL_S  9
/* GPIO_FUNC12_OUT_INV_SEL : R/W ;bitpos:[8] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC12_OUT_INV_SEL  (BIT(8))
#define GPIO_FUNC12_OUT_INV_SEL_M  (BIT(8))
#define GPIO_FUNC12_OUT_INV_SEL_V  0x1
#define GPIO_FUNC12_OUT_INV_SEL_S  8
/* GPIO_FUNC12_OUT_SEL : R/W ;bitpos:[7:0] ;default: 8'h80 ; */
/*description: */
#define GPIO_FUNC12_OUT_SEL  0x000000FF
#define GPIO_FUNC12_OUT_SEL_M  ((GPIO_FUNC12_OUT_SEL_V)<<(GPIO_FUNC12_OUT_SEL_S))
#define GPIO_FUNC12_OUT_SEL_V  0xFF
#define GPIO_FUNC12_OUT_SEL_S  0

#define GPIO_FUNC13_OUT_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x588)
/* GPIO_FUNC13_OEN_INV_SEL : R/W ;bitpos:[10] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC13_OEN_INV_SEL  (BIT(10))
#define GPIO_FUNC13_OEN_INV_SEL_M  (BIT(10))
#define GPIO_FUNC13_OEN_INV_SEL_V  0x1
#define GPIO_FUNC13_OEN_INV_SEL_S  10
/* GPIO_FUNC13_OEN_SEL : R/W ;bitpos:[9] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC13_OEN_SEL  (BIT(9))
#define GPIO_FUNC13_OEN_SEL_M  (BIT(9))
#define GPIO_FUNC13_OEN_SEL_V  0x1
#define GPIO_FUNC13_OEN_SEL_S  9
/* GPIO_FUNC13_OUT_INV_SEL : R/W ;bitpos:[8] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC13_OUT_INV_SEL  (BIT(8))
#define GPIO_FUNC13_OUT_INV_SEL_M  (BIT(8))
#define GPIO_FUNC13_OUT_INV_SEL_V  0x1
#define GPIO_FUNC13_OUT_INV_SEL_S  8
/* GPIO_FUNC13_OUT_SEL : R/W ;bitpos:[7:0] ;default: 8'h80 ; */
/*description: */
#define GPIO_FUNC13_OUT_SEL  0x000000FF
#define GPIO_FUNC13_OUT_SEL_M  ((GPIO_FUNC13_OUT_SEL_V)<<(GPIO_FUNC13_OUT_SEL_S))
#define GPIO_FUNC13_OUT_SEL_V  0xFF
#define GPIO_FUNC13_OUT_SEL_S  0

#define GPIO_FUNC14_OUT_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x58C)
/* GPIO_FUNC14_OEN_INV_SEL : R/W ;bitpos:[10] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC14_OEN_INV_SEL  (BIT(10))
#define GPIO_FUNC14_OEN_INV_SEL_M  (BIT(10))
#define GPIO_FUNC14_OEN_INV_SEL_V  0x1
#define GPIO_FUNC14_OEN_INV_SEL_S  10
/* GPIO_FUNC14_OEN_SEL : R/W ;bitpos:[9] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC14_OEN_SEL  (BIT(9))
#define GPIO_FUNC14_OEN_SEL_M  (BIT(9))
#define GPIO_FUNC14_OEN_SEL_V  0x1
#define GPIO_FUNC14_OEN_SEL_S  9
/* GPIO_FUNC14_OUT_INV_SEL : R/W ;bitpos:[8] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC14_OUT_INV_SEL  (BIT(8))
#define GPIO_FUNC14_OUT_INV_SEL_M  (BIT(8))
#define GPIO_FUNC14_OUT_INV_SEL_V  0x1
#define GPIO_FUNC14_OUT_INV_SEL_S  8
/* GPIO_FUNC14_OUT_SEL : R/W ;bitpos:[7:0] ;default: 8'h80 ; */
/*description: */
#define GPIO_FUNC14_OUT_SEL  0x000000FF
#define GPIO_FUNC14_OUT_SEL_M  ((GPIO_FUNC14_OUT_SEL_V)<<(GPIO_FUNC14_OUT_SEL_S))
#define GPIO_FUNC14_OUT_SEL_V  0xFF
#define GPIO_FUNC14_OUT_SEL_S  0

#define GPIO_FUNC15_OUT_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x590)
/* GPIO_FUNC15_OEN_INV_SEL : R/W ;bitpos:[10] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC15_OEN_INV_SEL  (BIT(10))
#define GPIO_FUNC15_OEN_INV_SEL_M  (BIT(10))
#define GPIO_FUNC15_OEN_INV_SEL_V  0x1
#define GPIO_FUNC15_OEN_INV_SEL_S  10
/* GPIO_FUNC15_OEN_SEL : R/W ;bitpos:[9] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC15_OEN_SEL  (BIT(9))
#define GPIO_FUNC15_OEN_SEL_M  (BIT(9))
#define GPIO_FUNC15_OEN_SEL_V  0x1
#define GPIO_FUNC15_OEN_SEL_S  9
/* GPIO_FUNC15_OUT_INV_SEL : R/W ;bitpos:[8] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC15_OUT_INV_SEL  (BIT(8))
#define GPIO_FUNC15_OUT_INV_SEL_M  (BIT(8))
#define GPIO_FUNC15_OUT_INV_SEL_V  0x1
#define GPIO_FUNC15_OUT_INV_SEL_S  8
/* GPIO_FUNC15_OUT_SEL : R/W ;bitpos:[7:0] ;default: 8'h80 ; */
/*description: */
#define GPIO_FUNC15_OUT_SEL  0x000000FF
#define GPIO_FUNC15_OUT_SEL_M  ((GPIO_FUNC15_OUT_SEL_V)<<(GPIO_FUNC15_OUT_SEL_S))
#define GPIO_FUNC15_OUT_SEL_V  0xFF
#define GPIO_FUNC15_OUT_SEL_S  0

#define GPIO_FUNC16_OUT_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x594)
/* GPIO_FUNC16_OEN_INV_SEL : R/W ;bitpos:[10] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC16_OEN_INV_SEL  (BIT(10))
#define GPIO_FUNC16_OEN_INV_SEL_M  (BIT(10))
#define GPIO_FUNC16_OEN_INV_SEL_V  0x1
#define GPIO_FUNC16_OEN_INV_SEL_S  10
/* GPIO_FUNC16_OEN_SEL : R/W ;bitpos:[9] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC16_OEN_SEL  (BIT(9))
#define GPIO_FUNC16_OEN_SEL_M  (BIT(9))
#define GPIO_FUNC16_OEN_SEL_V  0x1
#define GPIO_FUNC16_OEN_SEL_S  9
/* GPIO_FUNC16_OUT_INV_SEL : R/W ;bitpos:[8] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC16_OUT_INV_SEL  (BIT(8))
#define GPIO_FUNC16_OUT_INV_SEL_M  (BIT(8))
#define GPIO_FUNC16_OUT_INV_SEL_V  0x1
#define GPIO_FUNC16_OUT_INV_SEL_S  8
/* GPIO_FUNC16_OUT_SEL : R/W ;bitpos:[7:0] ;default: 8'h80 ; */
/*description: */
#define GPIO_FUNC16_OUT_SEL  0x000000FF
#define GPIO_FUNC16_OUT_SEL_M  ((GPIO_FUNC16_OUT_SEL_V)<<(GPIO_FUNC16_OUT_SEL_S))
#define GPIO_FUNC16_OUT_SEL_V  0xFF
#define GPIO_FUNC16_OUT_SEL_S  0

#define GPIO_FUNC17_OUT_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x598)
/* GPIO_FUNC17_OEN_INV_SEL : R/W ;bitpos:[10] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC17_OEN_INV_SEL  (BIT(10))
#define GPIO_FUNC17_OEN_INV_SEL_M  (BIT(10))
#define GPIO_FUNC17_OEN_INV_SEL_V  0x1
#define GPIO_FUNC17_OEN_INV_SEL_S  10
/* GPIO_FUNC17_OEN_SEL : R/W ;bitpos:[9] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC17_OEN_SEL  (BIT(9))
#define GPIO_FUNC17_OEN_SEL_M  (BIT(9))
#define GPIO_FUNC17_OEN_SEL_V  0x1
#define GPIO_FUNC17_OEN_SEL_S  9
/* GPIO_FUNC17_OUT_INV_SEL : R/W ;bitpos:[8] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC17_OUT_INV_SEL  (BIT(8))
#define GPIO_FUNC17_OUT_INV_SEL_M  (BIT(8))
#define GPIO_FUNC17_OUT_INV_SEL_V  0x1
#define GPIO_FUNC17_OUT_INV_SEL_S  8
/* GPIO_FUNC17_OUT_SEL : R/W ;bitpos:[7:0] ;default: 8'h80 ; */
/*description: */
#define GPIO_FUNC17_OUT_SEL  0x000000FF
#define GPIO_FUNC17_OUT_SEL_M  ((GPIO_FUNC17_OUT_SEL_V)<<(GPIO_FUNC17_OUT_SEL_S))
#define GPIO_FUNC17_OUT_SEL_V  0xFF
#define GPIO_FUNC17_OUT_SEL_S  0

#define GPIO_FUNC18_OUT_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x59C)
/* GPIO_FUNC18_OEN_INV_SEL : R/W ;bitpos:[10] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC18_OEN_INV_SEL  (BIT(10))
#define GPIO_FUNC18_OEN_INV_SEL_M  (BIT(10))
#define GPIO_FUNC18_OEN_INV_SEL_V  0x1
#define GPIO_FUNC18_OEN_INV_SEL_S  10
/* GPIO_FUNC18_OEN_SEL : R/W ;bitpos:[9] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC18_OEN_SEL  (BIT(9))
#define GPIO_FUNC18_OEN_SEL_M  (BIT(9))
#define GPIO_FUNC18_OEN_SEL_V  0x1
#define GPIO_FUNC18_OEN_SEL_S  9
/* GPIO_FUNC18_OUT_INV_SEL : R/W ;bitpos:[8] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC18_OUT_INV_SEL  (BIT(8))
#define GPIO_FUNC18_OUT_INV_SEL_M  (BIT(8))
#define GPIO_FUNC18_OUT_INV_SEL_V  0x1
#define GPIO_FUNC18_OUT_INV_SEL_S  8
/* GPIO_FUNC18_OUT_SEL : R/W ;bitpos:[7:0] ;default: 8'h80 ; */
/*description: */
#define GPIO_FUNC18_OUT_SEL  0x000000FF
#define GPIO_FUNC18_OUT_SEL_M  ((GPIO_FUNC18_OUT_SEL_V)<<(GPIO_FUNC18_OUT_SEL_S))
#define GPIO_FUNC18_OUT_SEL_V  0xFF
#define GPIO_FUNC18_OUT_SEL_S  0

#define GPIO_FUNC19_OUT_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x5A0)
/* GPIO_FUNC19_OEN_INV_SEL : R/W ;bitpos:[10] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC19_OEN_INV_SEL  (BIT(10))
#define GPIO_FUNC19_OEN_INV_SEL_M  (BIT(10))
#define GPIO_FUNC19_OEN_INV_SEL_V  0x1
#define GPIO_FUNC19_OEN_INV_SEL_S  10
/* GPIO_FUNC19_OEN_SEL : R/W ;bitpos:[9] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC19_OEN_SEL  (BIT(9))
#define GPIO_FUNC19_OEN_SEL_M  (BIT(9))
#define GPIO_FUNC19_OEN_SEL_V  0x1
#define GPIO_FUNC19_OEN_SEL_S  9
/* GPIO_FUNC19_OUT_INV_SEL : R/W ;bitpos:[8] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC19_OUT_INV_SEL  (BIT(8))
#define GPIO_FUNC19_OUT_INV_SEL_M  (BIT(8))
#define GPIO_FUNC19_OUT_INV_SEL_V  0x1
#define GPIO_FUNC19_OUT_INV_SEL_S  8
/* GPIO_FUNC19_OUT_SEL : R/W ;bitpos:[7:0] ;default: 8'h80 ; */
/*description: */
#define GPIO_FUNC19_OUT_SEL  0x000000FF
#define GPIO_FUNC19_OUT_SEL_M  ((GPIO_FUNC19_OUT_SEL_V)<<(GPIO_FUNC19_OUT_SEL_S))
#define GPIO_FUNC19_OUT_SEL_V  0xFF
#define GPIO_FUNC19_OUT_SEL_S  0

#define GPIO_FUNC20_OUT_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x5A4)
/* GPIO_FUNC20_OEN_INV_SEL : R/W ;bitpos:[10] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC20_OEN_INV_SEL  (BIT(10))
#define GPIO_FUNC20_OEN_INV_SEL_M  (BIT(10))
#define GPIO_FUNC20_OEN_INV_SEL_V  0x1
#define GPIO_FUNC20_OEN_INV_SEL_S  10
/* GPIO_FUNC20_OEN_SEL : R/W ;bitpos:[9] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC20_OEN_SEL  (BIT(9))
#define GPIO_FUNC20_OEN_SEL_M  (BIT(9))
#define GPIO_FUNC20_OEN_SEL_V  0x1
#define GPIO_FUNC20_OEN_SEL_S  9
/* GPIO_FUNC20_OUT_INV_SEL : R/W ;bitpos:[8] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC20_OUT_INV_SEL  (BIT(8))
#define GPIO_FUNC20_OUT_INV_SEL_M  (BIT(8))
#define GPIO_FUNC20_OUT_INV_SEL_V  0x1
#define GPIO_FUNC20_OUT_INV_SEL_S  8
/* GPIO_FUNC20_OUT_SEL : R/W ;bitpos:[7:0] ;default: 8'h80 ; */
/*description: */
#define GPIO_FUNC20_OUT_SEL  0x000000FF
#define GPIO_FUNC20_OUT_SEL_M  ((GPIO_FUNC20_OUT_SEL_V)<<(GPIO_FUNC20_OUT_SEL_S))
#define GPIO_FUNC20_OUT_SEL_V  0xFF
#define GPIO_FUNC20_OUT_SEL_S  0

#define GPIO_FUNC21_OUT_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x5A8)
/* GPIO_FUNC21_OEN_INV_SEL : R/W ;bitpos:[10] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC21_OEN_INV_SEL  (BIT(10))
#define GPIO_FUNC21_OEN_INV_SEL_M  (BIT(10))
#define GPIO_FUNC21_OEN_INV_SEL_V  0x1
#define GPIO_FUNC21_OEN_INV_SEL_S  10
/* GPIO_FUNC21_OEN_SEL : R/W ;bitpos:[9] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC21_OEN_SEL  (BIT(9))
#define GPIO_FUNC21_OEN_SEL_M  (BIT(9))
#define GPIO_FUNC21_OEN_SEL_V  0x1
#define GPIO_FUNC21_OEN_SEL_S  9
/* GPIO_FUNC21_OUT_INV_SEL : R/W ;bitpos:[8] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC21_OUT_INV_SEL  (BIT(8))
#define GPIO_FUNC21_OUT_INV_SEL_M  (BIT(8))
#define GPIO_FUNC21_OUT_INV_SEL_V  0x1
#define GPIO_FUNC21_OUT_INV_SEL_S  8
/* GPIO_FUNC21_OUT_SEL : R/W ;bitpos:[7:0] ;default: 8'h80 ; */
/*description: */
#define GPIO_FUNC21_OUT_SEL  0x000000FF
#define GPIO_FUNC21_OUT_SEL_M  ((GPIO_FUNC21_OUT_SEL_V)<<(GPIO_FUNC21_OUT_SEL_S))
#define GPIO_FUNC21_OUT_SEL_V  0xFF
#define GPIO_FUNC21_OUT_SEL_S  0

#define GPIO_FUNC22_OUT_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x5AC)
/* GPIO_FUNC22_OEN_INV_SEL : R/W ;bitpos:[10] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC22_OEN_INV_SEL  (BIT(10))
#define GPIO_FUNC22_OEN_INV_SEL_M  (BIT(10))
#define GPIO_FUNC22_OEN_INV_SEL_V  0x1
#define GPIO_FUNC22_OEN_INV_SEL_S  10
/* GPIO_FUNC22_OEN_SEL : R/W ;bitpos:[9] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC22_OEN_SEL  (BIT(9))
#define GPIO_FUNC22_OEN_SEL_M  (BIT(9))
#define GPIO_FUNC22_OEN_SEL_V  0x1
#define GPIO_FUNC22_OEN_SEL_S  9
/* GPIO_FUNC22_OUT_INV_SEL : R/W ;bitpos:[8] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC22_OUT_INV_SEL  (BIT(8))
#define GPIO_FUNC22_OUT_INV_SEL_M  (BIT(8))
#define GPIO_FUNC22_OUT_INV_SEL_V  0x1
#define GPIO_FUNC22_OUT_INV_SEL_S  8
/* GPIO_FUNC22_OUT_SEL : R/W ;bitpos:[7:0] ;default: 8'h80 ; */
/*description: */
#define GPIO_FUNC22_OUT_SEL  0x000000FF
#define GPIO_FUNC22_OUT_SEL_M  ((GPIO_FUNC22_OUT_SEL_V)<<(GPIO_FUNC22_OUT_SEL_S))
#define GPIO_FUNC22_OUT_SEL_V  0xFF
#define GPIO_FUNC22_OUT_SEL_S  0

#define GPIO_FUNC23_OUT_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x5B0)
/* GPIO_FUNC23_OEN_INV_SEL : R/W ;bitpos:[10] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC23_OEN_INV_SEL  (BIT(10))
#define GPIO_FUNC23_OEN_INV_SEL_M  (BIT(10))
#define GPIO_FUNC23_OEN_INV_SEL_V  0x1
#define GPIO_FUNC23_OEN_INV_SEL_S  10
/* GPIO_FUNC23_OEN_SEL : R/W ;bitpos:[9] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC23_OEN_SEL  (BIT(9))
#define GPIO_FUNC23_OEN_SEL_M  (BIT(9))
#define GPIO_FUNC23_OEN_SEL_V  0x1
#define GPIO_FUNC23_OEN_SEL_S  9
/* GPIO_FUNC23_OUT_INV_SEL : R/W ;bitpos:[8] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC23_OUT_INV_SEL  (BIT(8))
#define GPIO_FUNC23_OUT_INV_SEL_M  (BIT(8))
#define GPIO_FUNC23_OUT_INV_SEL_V  0x1
#define GPIO_FUNC23_OUT_INV_SEL_S  8
/* GPIO_FUNC23_OUT_SEL : R/W ;bitpos:[7:0] ;default: 8'h80 ; */
/*description: */
#define GPIO_FUNC23_OUT_SEL  0x000000FF
#define GPIO_FUNC23_OUT_SEL_M  ((GPIO_FUNC23_OUT_SEL_V)<<(GPIO_FUNC23_OUT_SEL_S))
#define GPIO_FUNC23_OUT_SEL_V  0xFF
#define GPIO_FUNC23_OUT_SEL_S  0

#define GPIO_FUNC24_OUT_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x5B4)
/* GPIO_FUNC24_OEN_INV_SEL : R/W ;bitpos:[10] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC24_OEN_INV_SEL  (BIT(10))
#define GPIO_FUNC24_OEN_INV_SEL_M  (BIT(10))
#define GPIO_FUNC24_OEN_INV_SEL_V  0x1
#define GPIO_FUNC24_OEN_INV_SEL_S  10
/* GPIO_FUNC24_OEN_SEL : R/W ;bitpos:[9] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC24_OEN_SEL  (BIT(9))
#define GPIO_FUNC24_OEN_SEL_M  (BIT(9))
#define GPIO_FUNC24_OEN_SEL_V  0x1
#define GPIO_FUNC24_OEN_SEL_S  9
/* GPIO_FUNC24_OUT_INV_SEL : R/W ;bitpos:[8] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC24_OUT_INV_SEL  (BIT(8))
#define GPIO_FUNC24_OUT_INV_SEL_M  (BIT(8))
#define GPIO_FUNC24_OUT_INV_SEL_V  0x1
#define GPIO_FUNC24_OUT_INV_SEL_S  8
/* GPIO_FUNC24_OUT_SEL : R/W ;bitpos:[7:0] ;default: 8'h80 ; */
/*description: */
#define GPIO_FUNC24_OUT_SEL  0x000000FF
#define GPIO_FUNC24_OUT_SEL_M  ((GPIO_FUNC24_OUT_SEL_V)<<(GPIO_FUNC24_OUT_SEL_S))
#define GPIO_FUNC24_OUT_SEL_V  0xFF
#define GPIO_FUNC24_OUT_SEL_S  0

#define GPIO_FUNC25_OUT_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x5B8)
/* GPIO_FUNC25_OEN_INV_SEL : R/W ;bitpos:[10] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC25_OEN_INV_SEL  (BIT(10))
#define GPIO_FUNC25_OEN_INV_SEL_M  (BIT(10))
#define GPIO_FUNC25_OEN_INV_SEL_V  0x1
#define GPIO_FUNC25_OEN_INV_SEL_S  10
/* GPIO_FUNC25_OEN_SEL : R/W ;bitpos:[9] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC25_OEN_SEL  (BIT(9))
#define GPIO_FUNC25_OEN_SEL_M  (BIT(9))
#define GPIO_FUNC25_OEN_SEL_V  0x1
#define GPIO_FUNC25_OEN_SEL_S  9
/* GPIO_FUNC25_OUT_INV_SEL : R/W ;bitpos:[8] ;default: 1'h0 ; */
/*description: */
#define GPIO_FUNC25_OUT_INV_SEL  (BIT(8))
#define GPIO_FUNC25_OUT_INV_SEL_M  (BIT(8))
#define GPIO_FUNC25_OUT_INV_SEL_V  0x1
#define GPIO_FUNC25_OUT_INV_SEL_S  8
/* GPIO_FUNC25_OUT_SEL : R/W ;bitpos:[7:0] ;default: 8'h80 ; */
/*description: */
#define GPIO_FUNC25_OUT_SEL  0x000000FF
#define GPIO_FUNC25_OUT_SEL_M  ((GPIO_FUNC25_OUT_SEL_V)<<(GPIO_FUNC25_OUT_SEL_S))
#define GPIO_FUNC25_OUT_SEL_V  0xFF
#define GPIO_FUNC25_OUT_SEL_S  0

#define GPIO_CLOCK_GATE_REG          (DR_REG_GPIO_BASE + 0x62C)
/* GPIO_CLK_EN : R/W ;bitpos:[0] ;default: 1'b1 ; */
/*description: */
#define GPIO_CLK_EN  (BIT(0))
#define GPIO_CLK_EN_M  (BIT(0))
#define GPIO_CLK_EN_V  0x1
#define GPIO_CLK_EN_S  0

#define GPIO_DATE_REG          (DR_REG_GPIO_BASE + 0x6FC)
/* GPIO_DATE : R/W ;bitpos:[27:0] ;default: 28'h2006130 ; */
/*description: */
#define GPIO_DATE  0x0FFFFFFF
#define GPIO_DATE_M  ((GPIO_DATE_V)<<(GPIO_DATE_S))
#define GPIO_DATE_V  0xFFFFFFF
#define GPIO_DATE_S  0

#ifdef __cplusplus
}
#endif



#endif /*_SOC_GPIO_REG_H_ */
