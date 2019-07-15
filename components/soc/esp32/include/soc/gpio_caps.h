// Copyright 2015-2019 Espressif Systems (Shanghai) PTE LTD
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

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

// ESP32 has 1 GPIO peripheral
#define GPIO_PORT_0             (0) /*!< GPIO port 0 */
#define GPIO_PORT_MAX           (1) /*!< GPIO port max */
#define SOC_GPIO_PORT           (GPIO_PORT_MAX)
#define GPIO_PIN_COUNT          (40)

#define GPIO_SEL_0              (BIT(0))                         /*!< Pin 0 selected */
#define GPIO_SEL_1              (BIT(1))                         /*!< Pin 1 selected */
#define GPIO_SEL_2              (BIT(2))                         /*!< Pin 2 selected
                                                                      @note There are more macros
                                                                      like that up to pin 39,
                                                                      excluding pins 20, 24 and 28..31.
                                                                      They are not shown here
                                                                      to reduce redundant information. */
/** @cond */
#define GPIO_SEL_3              (BIT(3))                         /*!< Pin 3 selected */
#define GPIO_SEL_4              (BIT(4))                         /*!< Pin 4 selected */
#define GPIO_SEL_5              (BIT(5))                         /*!< Pin 5 selected */
#define GPIO_SEL_6              (BIT(6))                         /*!< Pin 6 selected */
#define GPIO_SEL_7              (BIT(7))                         /*!< Pin 7 selected */
#define GPIO_SEL_8              (BIT(8))                         /*!< Pin 8 selected */
#define GPIO_SEL_9              (BIT(9))                         /*!< Pin 9 selected */
#define GPIO_SEL_10             (BIT(10))                        /*!< Pin 10 selected */
#define GPIO_SEL_11             (BIT(11))                        /*!< Pin 11 selected */
#define GPIO_SEL_12             (BIT(12))                        /*!< Pin 12 selected */
#define GPIO_SEL_13             (BIT(13))                        /*!< Pin 13 selected */
#define GPIO_SEL_14             (BIT(14))                        /*!< Pin 14 selected */
#define GPIO_SEL_15             (BIT(15))                        /*!< Pin 15 selected */
#define GPIO_SEL_16             (BIT(16))                        /*!< Pin 16 selected */
#define GPIO_SEL_17             (BIT(17))                        /*!< Pin 17 selected */
#define GPIO_SEL_18             (BIT(18))                        /*!< Pin 18 selected */
#define GPIO_SEL_19             (BIT(19))                        /*!< Pin 19 selected */

#define GPIO_SEL_21             (BIT(21))                        /*!< Pin 21 selected */
#define GPIO_SEL_22             (BIT(22))                        /*!< Pin 22 selected */
#define GPIO_SEL_23             (BIT(23))                        /*!< Pin 23 selected */

#define GPIO_SEL_25             (BIT(25))                        /*!< Pin 25 selected */
#define GPIO_SEL_26             (BIT(26))                        /*!< Pin 26 selected */
#define GPIO_SEL_27             (BIT(27))                        /*!< Pin 27 selected */

#define GPIO_SEL_32             ((uint64_t)(((uint64_t)1)<<32))  /*!< Pin 32 selected */
#define GPIO_SEL_33             ((uint64_t)(((uint64_t)1)<<33))  /*!< Pin 33 selected */
#define GPIO_SEL_34             ((uint64_t)(((uint64_t)1)<<34))  /*!< Pin 34 selected */
#define GPIO_SEL_35             ((uint64_t)(((uint64_t)1)<<35))  /*!< Pin 35 selected */
#define GPIO_SEL_36             ((uint64_t)(((uint64_t)1)<<36))  /*!< Pin 36 selected */
#define GPIO_SEL_37             ((uint64_t)(((uint64_t)1)<<37))  /*!< Pin 37 selected */
#define GPIO_SEL_38             ((uint64_t)(((uint64_t)1)<<38))  /*!< Pin 38 selected */
#define GPIO_SEL_39             ((uint64_t)(((uint64_t)1)<<39))  /*!< Pin 39 selected */

#define GPIO_PIN_REG_0          IO_MUX_GPIO0_REG
#define GPIO_PIN_REG_1          IO_MUX_GPIO1_REG
#define GPIO_PIN_REG_2          IO_MUX_GPIO2_REG
#define GPIO_PIN_REG_3          IO_MUX_GPIO3_REG
#define GPIO_PIN_REG_4          IO_MUX_GPIO4_REG
#define GPIO_PIN_REG_5          IO_MUX_GPIO5_REG
#define GPIO_PIN_REG_6          IO_MUX_GPIO6_REG
#define GPIO_PIN_REG_7          IO_MUX_GPIO7_REG
#define GPIO_PIN_REG_8          IO_MUX_GPIO8_REG
#define GPIO_PIN_REG_9          IO_MUX_GPIO9_REG
#define GPIO_PIN_REG_10         IO_MUX_GPIO10_REG
#define GPIO_PIN_REG_11         IO_MUX_GPIO11_REG
#define GPIO_PIN_REG_12         IO_MUX_GPIO12_REG
#define GPIO_PIN_REG_13         IO_MUX_GPIO13_REG
#define GPIO_PIN_REG_14         IO_MUX_GPIO14_REG
#define GPIO_PIN_REG_15         IO_MUX_GPIO15_REG
#define GPIO_PIN_REG_16         IO_MUX_GPIO16_REG
#define GPIO_PIN_REG_17         IO_MUX_GPIO17_REG
#define GPIO_PIN_REG_18         IO_MUX_GPIO18_REG
#define GPIO_PIN_REG_19         IO_MUX_GPIO19_REG
#define GPIO_PIN_REG_20         IO_MUX_GPIO20_REG
#define GPIO_PIN_REG_21         IO_MUX_GPIO21_REG
#define GPIO_PIN_REG_22         IO_MUX_GPIO22_REG
#define GPIO_PIN_REG_23         IO_MUX_GPIO23_REG
#define GPIO_PIN_REG_25         IO_MUX_GPIO25_REG
#define GPIO_PIN_REG_26         IO_MUX_GPIO26_REG
#define GPIO_PIN_REG_27         IO_MUX_GPIO27_REG
#define GPIO_PIN_REG_32         IO_MUX_GPIO32_REG
#define GPIO_PIN_REG_33         IO_MUX_GPIO33_REG
#define GPIO_PIN_REG_34         IO_MUX_GPIO34_REG
#define GPIO_PIN_REG_35         IO_MUX_GPIO35_REG
#define GPIO_PIN_REG_36         IO_MUX_GPIO36_REG
#define GPIO_PIN_REG_37         IO_MUX_GPIO37_REG
#define GPIO_PIN_REG_38         IO_MUX_GPIO38_REG
#define GPIO_PIN_REG_39         IO_MUX_GPIO39_REG

#define GPIO_APP_CPU_INTR_ENA      (BIT(0))
#define GPIO_APP_CPU_NMI_INTR_ENA  (BIT(1))
#define GPIO_PRO_CPU_INTR_ENA      (BIT(2))
#define GPIO_PRO_CPU_NMI_INTR_ENA  (BIT(3))
#define GPIO_SDIO_EXT_INTR_ENA     (BIT(4))

#define GPIO_MODE_DEF_DISABLE         (0)
#define GPIO_MODE_DEF_INPUT           (BIT0)
#define GPIO_MODE_DEF_OUTPUT          (BIT1)
#define GPIO_MODE_DEF_OD              (BIT2)

/** @endcond */

#define GPIO_IS_VALID_GPIO(gpio_num)              ((gpio_num < GPIO_PIN_COUNT && GPIO_PIN_MUX_REG[gpio_num] != 0))                                     /*!< Check whether it is a valid GPIO number */
#define GPIO_IS_VALID_OUTPUT_GPIO(gpio_num)       ((GPIO_IS_VALID_GPIO(gpio_num)) && (gpio_num < 34))                                                  /*!< Check whether it can be a valid GPIO number of output mode */
#define GPIO_MASK_CONTAIN_INPUT_GPIO(gpio_mask)   ((gpio_mask & (GPIO_SEL_34 | GPIO_SEL_35 | GPIO_SEL_36 | GPIO_SEL_37 | GPIO_SEL_38 | GPIO_SEL_39)))  /*!< Check whether it contains input io */

#define GPIO_NUM_NC (-1)    /*!< Use to signal not connected to S/W */
#define GPIO_NUM_0 (0)     /*!< GPIO0, input and output */
#define GPIO_NUM_1 (1)     /*!< GPIO1, input and output */
#define GPIO_NUM_2 (2)     /*!< GPIO2, input and output
                             @note There are more enumerations like that
                             up to GPIO39, excluding GPIO20, GPIO24 and GPIO28..31.
                             They are not shown here to reduce redundant information.
                             @note GPIO34..39 are input mode only. */
    /** @cond */
#define GPIO_NUM_3 (3)     /*!< GPIO3, input and output */
#define GPIO_NUM_4 (4)     /*!< GPIO4, input and output */
#define GPIO_NUM_5 (5)     /*!< GPIO5, input and output */
#define GPIO_NUM_6 (6)     /*!< GPIO6, input and output */
#define GPIO_NUM_7 (7)     /*!< GPIO7, input and output */
#define GPIO_NUM_8 (8)     /*!< GPIO8, input and output */
#define GPIO_NUM_9 (9)     /*!< GPIO9, input and output */
#define GPIO_NUM_10 (10)   /*!< GPIO10, input and output */
#define GPIO_NUM_11 (11)   /*!< GPIO11, input and output */
#define GPIO_NUM_12 (12)   /*!< GPIO12, input and output */
#define GPIO_NUM_13 (13)   /*!< GPIO13, input and output */
#define GPIO_NUM_14 (14)   /*!< GPIO14, input and output */
#define GPIO_NUM_15 (15)   /*!< GPIO15, input and output */
#define GPIO_NUM_16 (16)   /*!< GPIO16, input and output */
#define GPIO_NUM_17 (17)   /*!< GPIO17, input and output */
#define GPIO_NUM_18 (18)   /*!< GPIO18, input and output */
#define GPIO_NUM_19 (19)   /*!< GPIO19, input and output */

#define GPIO_NUM_21 (21)   /*!< GPIO21, input and output */
#define GPIO_NUM_22 (22)   /*!< GPIO22, input and output */
#define GPIO_NUM_23 (23)   /*!< GPIO23, input and output */

#define GPIO_NUM_25 (25)   /*!< GPIO25, input and output */
#define GPIO_NUM_26 (26)   /*!< GPIO26, input and output */
#define GPIO_NUM_27 (27)   /*!< GPIO27, input and output */

#define GPIO_NUM_32 (32)   /*!< GPIO32, input and output */
#define GPIO_NUM_33 (33)   /*!< GPIO33, input and output */
#define GPIO_NUM_34 (34)   /*!< GPIO34, input mode only */
#define GPIO_NUM_35 (35)   /*!< GPIO35, input mode only */
#define GPIO_NUM_36 (36)   /*!< GPIO36, input mode only */
#define GPIO_NUM_37 (37)   /*!< GPIO37, input mode only */
#define GPIO_NUM_38 (38)   /*!< GPIO38, input mode only */
#define GPIO_NUM_39 (39)   /*!< GPIO39, input mode only */
#define GPIO_NUM_MAX (40)
    /** @endcond */

#ifdef __cplusplus
}
#endif
