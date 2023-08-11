/*
 * SPDX-FileCopyrightText: 2015-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "soc/soc_caps.h"
#include "soc/gpio_num.h"
#include "esp_bit_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    GPIO_PORT_0 = 0,
    GPIO_PORT_MAX,
} gpio_port_t;

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
#define GPIO_PIN_REG_10          IO_MUX_GPIO10_REG
#define GPIO_PIN_REG_11          IO_MUX_GPIO11_REG
#define GPIO_PIN_REG_12          IO_MUX_GPIO12_REG
#define GPIO_PIN_REG_13          IO_MUX_GPIO13_REG
#define GPIO_PIN_REG_14          IO_MUX_GPIO14_REG
#define GPIO_PIN_REG_15          IO_MUX_GPIO15_REG
#define GPIO_PIN_REG_16          IO_MUX_GPIO16_REG
#define GPIO_PIN_REG_17          IO_MUX_GPIO17_REG
#define GPIO_PIN_REG_18          IO_MUX_GPIO18_REG
#define GPIO_PIN_REG_19          IO_MUX_GPIO19_REG
#define GPIO_PIN_REG_20          IO_MUX_GPIO20_REG
#define GPIO_PIN_REG_21          IO_MUX_GPIO21_REG
#define GPIO_PIN_REG_22          IO_MUX_GPIO22_REG
#define GPIO_PIN_REG_23          IO_MUX_GPIO23_REG
#define GPIO_PIN_REG_24          IO_MUX_GPIO24_REG
#define GPIO_PIN_REG_25          IO_MUX_GPIO25_REG
#define GPIO_PIN_REG_26          IO_MUX_GPIO26_REG
#define GPIO_PIN_REG_27          IO_MUX_GPIO27_REG
#define GPIO_PIN_REG_28          IO_MUX_GPIO28_REG
#define GPIO_PIN_REG_29          IO_MUX_GPIO29_REG
#define GPIO_PIN_REG_30          IO_MUX_GPIO30_REG
#define GPIO_PIN_REG_31          IO_MUX_GPIO31_REG
#define GPIO_PIN_REG_32          IO_MUX_GPIO32_REG
#define GPIO_PIN_REG_33          IO_MUX_GPIO33_REG
#define GPIO_PIN_REG_34          IO_MUX_GPIO34_REG
#define GPIO_PIN_REG_35          IO_MUX_GPIO35_REG
#define GPIO_PIN_REG_36          IO_MUX_GPIO36_REG
#define GPIO_PIN_REG_37          IO_MUX_GPIO37_REG
#define GPIO_PIN_REG_38          IO_MUX_GPIO38_REG
#define GPIO_PIN_REG_39          IO_MUX_GPIO39_REG
#define GPIO_PIN_REG_40          IO_MUX_GPIO40_REG
#define GPIO_PIN_REG_41          IO_MUX_GPIO41_REG
#define GPIO_PIN_REG_42          IO_MUX_GPIO42_REG
#define GPIO_PIN_REG_43          IO_MUX_GPIO43_REG
#define GPIO_PIN_REG_44          IO_MUX_GPIO44_REG
#define GPIO_PIN_REG_45          IO_MUX_GPIO45_REG
#define GPIO_PIN_REG_46          IO_MUX_GPIO46_REG
#define GPIO_PIN_REG_47          IO_MUX_GPIO47_REG
#define GPIO_PIN_REG_48          IO_MUX_GPIO48_REG
#define GPIO_PIN_REG_49          IO_MUX_GPIO49_REG
#define GPIO_PIN_REG_50          IO_MUX_GPIO50_REG
#define GPIO_PIN_REG_51          IO_MUX_GPIO51_REG
#define GPIO_PIN_REG_52          IO_MUX_GPIO52_REG
#define GPIO_PIN_REG_53          IO_MUX_GPIO53_REG
#define GPIO_PIN_REG_54          IO_MUX_GPIO54_REG
#define GPIO_PIN_REG_55          IO_MUX_GPIO55_REG
#define GPIO_PIN_REG_56          IO_MUX_GPIO56_REG

typedef enum {
    GPIO_INTR_DISABLE = 0,     /*!< Disable GPIO interrupt                             */
    GPIO_INTR_POSEDGE = 1,     /*!< GPIO interrupt type : rising edge                  */
    GPIO_INTR_NEGEDGE = 2,     /*!< GPIO interrupt type : falling edge                 */
    GPIO_INTR_ANYEDGE = 3,     /*!< GPIO interrupt type : both rising and falling edge */
    GPIO_INTR_LOW_LEVEL = 4,   /*!< GPIO interrupt type : input low level trigger      */
    GPIO_INTR_HIGH_LEVEL = 5,  /*!< GPIO interrupt type : input high level trigger     */
    GPIO_INTR_MAX,
} gpio_int_type_t;

/** @cond */
#define GPIO_MODE_DEF_DISABLE         (0)
#define GPIO_MODE_DEF_INPUT           (BIT0)    ///< bit mask for input
#define GPIO_MODE_DEF_OUTPUT          (BIT1)    ///< bit mask for output
#define GPIO_MODE_DEF_OD              (BIT2)    ///< bit mask for OD mode
/** @endcond */

typedef enum {
    GPIO_MODE_DISABLE = GPIO_MODE_DEF_DISABLE,                                                         /*!< GPIO mode : disable input and output             */
    GPIO_MODE_INPUT = GPIO_MODE_DEF_INPUT,                                                             /*!< GPIO mode : input only                           */
    GPIO_MODE_OUTPUT = GPIO_MODE_DEF_OUTPUT,                                                           /*!< GPIO mode : output only mode                     */
    GPIO_MODE_OUTPUT_OD = ((GPIO_MODE_DEF_OUTPUT) | (GPIO_MODE_DEF_OD)),                               /*!< GPIO mode : output only with open-drain mode     */
    GPIO_MODE_INPUT_OUTPUT_OD = ((GPIO_MODE_DEF_INPUT) | (GPIO_MODE_DEF_OUTPUT) | (GPIO_MODE_DEF_OD)), /*!< GPIO mode : output and input with open-drain mode*/
    GPIO_MODE_INPUT_OUTPUT = ((GPIO_MODE_DEF_INPUT) | (GPIO_MODE_DEF_OUTPUT)),                         /*!< GPIO mode : output and input mode                */
} gpio_mode_t;

typedef enum {
    GPIO_PULLUP_DISABLE = 0x0,     /*!< Disable GPIO pull-up resistor */
    GPIO_PULLUP_ENABLE = 0x1,      /*!< Enable GPIO pull-up resistor */
} gpio_pullup_t;

typedef enum {
    GPIO_PULLDOWN_DISABLE = 0x0,   /*!< Disable GPIO pull-down resistor */
    GPIO_PULLDOWN_ENABLE = 0x1,    /*!< Enable GPIO pull-down resistor  */
} gpio_pulldown_t;

typedef enum {
    GPIO_PULLUP_ONLY,               /*!< Pad pull up            */
    GPIO_PULLDOWN_ONLY,             /*!< Pad pull down          */
    GPIO_PULLUP_PULLDOWN,           /*!< Pad pull up + pull down*/
    GPIO_FLOATING,                  /*!< Pad floating           */
} gpio_pull_mode_t;

typedef enum {
    GPIO_DRIVE_CAP_0       = 0,    /*!< Pad drive capability: weak          */
    GPIO_DRIVE_CAP_1       = 1,    /*!< Pad drive capability: stronger      */
    GPIO_DRIVE_CAP_2       = 2,    /*!< Pad drive capability: medium */
    GPIO_DRIVE_CAP_DEFAULT = 2,    /*!< Pad drive capability: medium */
    GPIO_DRIVE_CAP_3       = 3,    /*!< Pad drive capability: strongest     */
    GPIO_DRIVE_CAP_MAX,
} gpio_drive_cap_t;

#if SOC_GPIO_SUPPORT_PIN_HYS_FILTER
/**
 * @brief Available option for configuring hysteresis feature of GPIOs
 */
typedef enum {
#if SOC_GPIO_SUPPORT_PIN_HYS_CTRL_BY_EFUSE
    GPIO_HYS_CTRL_EFUSE     = 0,    /*!< Pad input hysteresis ctrl by efuse */
#endif
    GPIO_HYS_SOFT_DISABLE,          /*!< Pad input hysteresis disable by software */
    GPIO_HYS_SOFT_ENABLE,           /*!< Pad input hysteresis enable by software */
} gpio_hys_ctrl_mode_t;
#endif

#ifdef __cplusplus
}
#endif
